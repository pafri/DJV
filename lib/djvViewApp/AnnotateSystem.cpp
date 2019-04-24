//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/AnnotateSystem.h>

#include <djvViewApp/AnnotateTool.h>

#include <djvUI/Action.h>
#include <djvUI/Menu.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSystem::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<AnnotateTool> annotateTool;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void AnnotateSystem::_init(Context * context)
        {
            IViewSystem::_init("djv::ViewApp::AnnotateSystem", context);

            DJV_PRIVATE_PTR();
            p.actions["Tool"] = UI::Action::create();
            p.actions["Tool"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Tool"]->setShortcut(GLFW_KEY_A, GLFW_MOD_CONTROL);

            p.menu = UI::Menu::create(context);
            p.menu->addAction(p.actions["Tool"]);

            auto weak = std::weak_ptr<AnnotateSystem>(std::dynamic_pointer_cast<AnnotateSystem>(shared_from_this()));
            _setCloseToolCallback(
                [weak](const std::string & name)
            {
                if (auto system = weak.lock())
                {
                    const auto i = system->_p->actions.find(name);
                    if (i != system->_p->actions.end())
                    {
                        i->second->setChecked(false);
                    }
                }
            });

            p.clickedObservers["Tool"] = ValueObserver<bool>::create(
                p.actions["Tool"]->observeChecked(),
                [weak, context](bool value)
            {
                if (auto system = weak.lock())
                {
                    if (value)
                    {
                        system->_openTool("Tool", AnnotateTool::create(context));
                    }
                    else
                    {
                        system->_closeTool("Tool");
                    }
                }
            });

            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        AnnotateSystem::AnnotateSystem() :
            _p(new Private)
        {}

        AnnotateSystem::~AnnotateSystem()
        {}

        std::shared_ptr<AnnotateSystem> AnnotateSystem::create(Context * context)
        {
            auto out = std::shared_ptr<AnnotateSystem>(new AnnotateSystem);
            out->_init(context);
            return out;
        }

        std::map<std::string, std::shared_ptr<UI::Action> > AnnotateSystem::getActions()
        {
            return _p->actions;
        }

        MenuData AnnotateSystem::getMenu()
        {
            return
            {
                _p->menu,
                "H"
            };
        }

        void AnnotateSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["Tool"]->setText(_getText(DJV_TEXT("Show Annotations")));

            p.menu->setText(_getText(DJV_TEXT("Annotate")));
        }

    } // namespace ViewApp
} // namespace djv

