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

#include <djvCore/DrivesModel.h>

#include <djvCore/Path.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <mutex>
#include <thread>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t timeout = 100;

        } // namespace

        struct DrivesModel::Private
        {
            std::vector<Path> drives;
            std::shared_ptr<ListSubject<Path> > drivesSubject;
            std::mutex mutex;
            std::thread thread;
            std::atomic<bool> running;
            std::shared_ptr<Timer> timer;
        };
            
        void DrivesModel::_init(const std::shared_ptr<Context>& context)
        {
            _p->drivesSubject = ListSubject<Path>::create();
                
            _p->running = true;
            _p->thread = std::thread(
                [this]
            {
                while (_p->running)
                {
                    const std::vector<Path> drives = _getDrives();
                    {
                        std::lock_guard<std::mutex> lock(_p->mutex);
                        _p->drives = drives;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                }
            });

            _p->timer = Timer::create(context);
            _p->timer->setRepeating(true);
            _p->timer->start(
                std::chrono::milliseconds(timeout),
                [this](float)
            {
                std::vector<Path> drives;
                {
                    std::lock_guard<std::mutex> lock(_p->mutex);
                    drives = _p->drives;
                }
                _p->drivesSubject->setIfChanged(drives);
            });
        }

        DrivesModel::DrivesModel() :
            _p(new Private)
        {}

        DrivesModel::~DrivesModel()
        {
            _p->running = false;
            _p->thread.join();
        }

        std::shared_ptr<DrivesModel> DrivesModel::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<DrivesModel>(new DrivesModel);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IListSubject<Path> > DrivesModel::getDrives() const
        {
            return _p->drivesSubject;
        }

    } // namespace Core
} // namespace djv
