//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#pragma once

#include <djvScene/Primitive.h>

#include <djvCore/Range.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace Scene
    {
        //! This class provides the base functionality for cameras.
        class ICamera : public IPrimitive
        {
            DJV_NON_COPYABLE(ICamera);

        protected:
            ICamera();

        public:
            virtual ~ICamera() = 0;

            virtual const glm::mat4x4& getV() const = 0;
            virtual const glm::mat4x4& getP() const = 0;
        };

        //! This class provides a basic camera.
        class BasicCamera : public ICamera
        {
        protected:
            BasicCamera();

        public:
            static std::shared_ptr<BasicCamera> create();

            void setEye(const glm::vec3&);
            void setTarget(const glm::vec3&);
            void setUp(const glm::vec3&);
            void setFOV(float);
            void setAspect(float);
            void setClip(const Core::FloatRange&);

            const glm::mat4x4& getV() const override;
            const glm::mat4x4& getP() const override;

        private:
            void _update();

            glm::vec3 _eye = glm::vec3(10.F, 20.F, -20.F);
            glm::vec3 _target = glm::vec3(0.F, 0.F, 0.F);
            glm::vec3 _up = glm::vec3(0.F, 1.F, 0.F);
            float _fov = 45.F;
            float _aspect = 1.F;
            Core::FloatRange _clip = Core::FloatRange(0.1F, 10000.F);
            glm::mat4x4 _v = glm::mat4x4(1.F);
            glm::mat4x4 _p = glm::mat4x4(1.F);
        };

        //! This class provides a polar camera.
        class PolarCamera : public ICamera
        {
        protected:
            PolarCamera();

        public:
            static std::shared_ptr<PolarCamera> create();

            float getDistance() const;
            float getLatitude() const;
            float getLongitude() const;

            void setTarget(const glm::vec3&);
            void setDistance(float);
            void setLatitude(float);
            void setLongitude(float);
            void setFOV(float);
            void setAspect(float);
            void setClip(const Core::FloatRange&);

            const glm::mat4x4& getV() const override;
            const glm::mat4x4& getP() const override;

        private:
            void _update();

            glm::vec3 _target = glm::vec3(0.F, 0.F, 0.F);
            float _distance = 20.F;
            float _latitude = 30.F;
            float _longitude = 30.F;
            float _fov = 45.F;
            float _aspect = 1.F;
            Core::FloatRange _clip = Core::FloatRange(.1F, 10000.F);
            glm::mat4x4 _v = glm::mat4x4(1.F);
            glm::mat4x4 _p = glm::mat4x4(1.F);
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/CameraInline.h>
