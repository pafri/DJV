//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/OpenGLOffscreenBuffer.h>

#include <djvAV/OpenGLTexture.h>

#include <djvCore/Error.h>

//#pragma optimize("", off)

using namespace djv::Core;

using namespace gl;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            void OffscreenBuffer::_init(const Image::Info& info)
            {
                _info = info;

                // Create the texture.
                glGenTextures(1, &_textureID);
                if (!_textureID)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot create OpenGL texture.");
                    throw std::runtime_error(ss.str());
                }
                glBindTexture(GL_TEXTURE_2D, _textureID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    Texture::getInternalFormat(_info.type),
                    _info.size.x,
                    _info.size.y,
                    0,
                    _info.getGLFormat(),
                    _info.getGLType(),
                    0);
                glBindTexture(GL_TEXTURE_2D, 0);

                // Create the FBO.
                glGenFramebuffers(1, &_id);
                if (!_id)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot create OpenGL frame buffer.");
                    throw std::runtime_error(ss.str());
                }
                const OffscreenBufferBinding binding(shared_from_this());
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D,
                    _textureID,
                    0);
                GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (error != GL_FRAMEBUFFER_COMPLETE)
                {
                    std::stringstream ss;
                    ss << DJV_TEXT("Cannot initialize OpenGL frame buffer.");
                    throw std::runtime_error(ss.str());
                }
            }

            OffscreenBuffer::~OffscreenBuffer()
            {
                if (_id)
                {
                    glDeleteFramebuffers(1, &_id);
                    _id = 0;
                }
                if (_textureID)
                {
                    glDeleteTextures(1, &_textureID);
                    _textureID = 0;
                }
            }

            std::shared_ptr<OffscreenBuffer> OffscreenBuffer::create(const Image::Info& info)
            {
                auto out = std::shared_ptr<OffscreenBuffer>(new OffscreenBuffer);
                out->_init(info);
                return out;
            }

            void OffscreenBuffer::bind()
            {
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_restore);
                glBindFramebuffer(GL_FRAMEBUFFER, _id);
            }

            void OffscreenBuffer::unbind()
            {
                glBindFramebuffer(GL_FRAMEBUFFER, _restore);
                _restore = 0;
            }

            OffscreenBufferBinding::OffscreenBufferBinding(const std::shared_ptr<OffscreenBuffer> & buffer) :
                _buffer(buffer)
            {
                _buffer->bind();
            }

            OffscreenBufferBinding::~OffscreenBufferBinding()
            {
                _buffer->unbind();
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv