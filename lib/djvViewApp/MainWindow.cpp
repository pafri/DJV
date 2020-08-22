// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MainWindow.h>

#include <djvViewApp/Application.h>
#include <djvViewApp/BackgroundImageWidget.h>
#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/IToolSystem.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaCanvas.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/SettingsWidget.h>
#include <djvViewApp/SettingsSystem.h>
#include <djvViewApp/ToolSystem.h>
#include <djvViewApp/ViewSystem.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ThermometerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Drawer.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/MenuButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#define DJV_DEMO

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MainWindow::Private
        {
            std::vector<std::shared_ptr<Media> > media;
            
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> mediaActionGroup;
            std::shared_ptr<UI::Menu> mediaMenu;
            std::shared_ptr<UI::Button::Menu> mediaButton;
            std::shared_ptr<UI::ToolButton> settingsButton;
            std::shared_ptr<UI::MenuBar> menuBar;
            std::shared_ptr<MediaCanvas> mediaCanvas;
            std::shared_ptr<UI::MDI::Canvas> canvas;
#ifdef DJV_DEMO
            std::shared_ptr<UI::Label> titleLabel;
#endif // DJV_DEMO
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<ValueObserver<bool> > settingsActionObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<Media> > > mediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<bool> > maximizeObserver;
            std::shared_ptr<ValueObserver<float> > fadeObserver;
        };
        
        void MainWindow::_init(const std::shared_ptr<Core::Context>& context)
        {
            UI::Window::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MainWindow");

            auto viewSystems = context->getSystemsT<IViewSystem>();
            std::map<std::string, std::shared_ptr<UI::Menu> > menus;
            for (auto system : viewSystems)
            {
                for (auto action : system->getActions())
                {
                    addAction(action.second);
                }
                auto menu = system->getMenu();
                if (menu.menu)
                {
                    menus[menu.sortKey] = menu.menu;
                }
            }

            p.actions["Escape"] = UI::Action::create();
            p.actions["Escape"]->setShortcut(GLFW_KEY_ESCAPE);
            for (auto i : p.actions)
            {
                addAction(i.second);
            }

            p.mediaActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.mediaMenu = UI::Menu::create(context);
            p.mediaMenu->setMinimumSizeRole(UI::MetricsRole::None);
            addChild(p.mediaMenu);
            p.mediaButton = UI::Button::Menu::create(UI::MenuButtonStyle::Flat, context);
            p.mediaButton->setPopupIcon("djvIconPopupMenu");
            p.mediaButton->setEnabled(false);

            auto maximizeButton = UI::ToolButton::create(context);
            auto autoHideButton = UI::ToolButton::create(context);
            auto windowSystem = context->getSystemT<WindowSystem>();
            if (windowSystem)
            {
                maximizeButton->addAction(windowSystem->getActions()["Maximize"]);
                autoHideButton->addAction(windowSystem->getActions()["AutoHide"]);
            }

            auto viewFillButton = UI::ToolButton::create(context);
            auto viewFrameButton = UI::ToolButton::create(context);
            auto viewCenterButton = UI::ToolButton::create(context);
            auto viewSystem = context->getSystemT<ViewSystem>();
            if (viewSystem)
            {
                viewFillButton->addAction(viewSystem->getActions()["FillLock"]);
                viewFrameButton->addAction(viewSystem->getActions()["FrameLock"]);
                viewCenterButton->addAction(viewSystem->getActions()["CenterLock"]);
            }

            std::map<std::string, std::shared_ptr<UI::ToolButton> > toolButtons;
            for (const auto& i : context->getSystemsT<IToolSystem>())
            {
                auto button = UI::ToolButton::create(context);
                auto data = i->getToolAction();
                button->addAction(data.action);
                toolButtons[data.sortKey] = button;
            }

            p.settingsButton = UI::ToolButton::create(context);
            p.settingsButton->setButtonType(UI::ButtonType::Toggle);
            auto toolSystem = context->getSystemT<ToolSystem>();
            if (toolSystem)
            {
                p.settingsButton->addAction(toolSystem->getActions()["Settings"]);
            }
            auto settingsDrawer = UI::Drawer::create(UI::Side::Right, context);

            p.menuBar = UI::MenuBar::create(context);
            p.menuBar->setBackgroundRole(UI::ColorRole::OverlayLight);
            for (auto i : menus)
            {
                p.menuBar->addChild(i.second);
            }
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.mediaButton);
            p.menuBar->setStretch(p.mediaButton, UI::RowStretch::Expand, UI::Side::Right);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(maximizeButton);
            p.menuBar->addChild(autoHideButton);
            p.menuBar->addSeparator(UI::Side::Right);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(viewFillButton);
            hLayout->addChild(viewFrameButton);
            hLayout->addChild(viewCenterButton);
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            for (const auto& i : toolButtons)
            {
                hLayout->addChild(i.second);
            }
            p.menuBar->addChild(hLayout);
            p.menuBar->addSeparator(UI::Side::Right);
            p.menuBar->addChild(p.settingsButton);

            auto backgroundImageWidget = BackgroundImageWidget::create(context);

            p.mediaCanvas = MediaCanvas::create(context);

            p.canvas = UI::MDI::Canvas::create(context);
            for (auto system : viewSystems)
            {
                system->setCanvas(p.canvas);
            }

#ifdef DJV_DEMO
            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::TextColumn);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
#endif // DJV_DEMO

            p.layout = UI::StackLayout::create(context);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(backgroundImageWidget);
            stackLayout->addChild(p.mediaCanvas);
            stackLayout->addChild(p.canvas);
            p.layout->addChild(stackLayout);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.menuBar);
            vLayout->addChild(settingsDrawer);
            vLayout->setStretch(settingsDrawer, UI::Layout::RowStretch::Expand);
            p.layout->addChild(vLayout);
#ifdef DJV_DEMO
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setMargin(UI::MetricsRole::MarginDialog);
            vLayout->setSpacing(UI::MetricsRole::SpacingLarge);
            vLayout->addChild(p.titleLabel);
            p.layout->addChild(vLayout);
#endif // DJV_DEMO
            addChild(p.layout);

            auto weak = std::weak_ptr<MainWindow>(std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.mediaActionGroup->setRadioCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value >= 0 && value < static_cast<int>(widget->_p->media.size()))
                            {
                                if (auto fileSystem = context->getSystemT<FileSystem>())
                                {
                                    fileSystem->setCurrentMedia(widget->_p->media[value]);
                                }
                            }
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaButton->setOpenCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value)
                        {
                            widget->_p->mediaMenu->popup(widget->_p->mediaButton);
                        }
                        else
                        {
                            widget->_p->mediaMenu->close();
                        }
                    }
                });

            p.mediaMenu->setCloseCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mediaButton->setOpen(false);
                    }
                });

            settingsDrawer->setOpenCallback(
                [contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        out = SettingsWidget::create(context);
                    }
                    return out;
                });

            p.actions["Escape"]->setClickedCallback(
                [contextWeak]
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto windowSystem = context->getSystemT<WindowSystem>())
                    {
                        if (windowSystem->observeFullScreen()->get())
                        {
                            windowSystem->setFullScreen(false);
                        }
                    }
                }
            });

            if (toolSystem)
            {
                p.settingsActionObserver = ValueObserver<bool>::create(
                    toolSystem->getActions()["Settings"]->observeChecked(),
                    [settingsDrawer](bool value)
                    {
                        settingsDrawer->setOpen(value);
                    });
            }

            if (auto context = getContext().lock())
            {
                if (auto fileSystem = context->getSystemT<FileSystem>())
                {
                    p.mediaObserver = ListObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeMedia(),
                        [weak](const std::vector<std::shared_ptr<Media> >& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->media = value;
                                std::vector<std::shared_ptr<UI::Action> > actions;
                                widget->_p->mediaMenu->clearActions();
                                for (const auto& i : widget->_p->media)
                                {
                                    auto action = UI::Action::create();
                                    action->setText(i->getFileInfo().getFileName());
                                    actions.push_back(action);
                                    widget->_p->mediaMenu->addAction(action);
                                }
                                widget->_p->mediaActionGroup->setActions(actions);
                                widget->_p->mediaButton->setEnabled(widget->_p->media.size() > 0);
                            }
                        });

                    p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                        fileSystem->observeCurrentMedia(),
                        [weak](const std::shared_ptr<Media>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                const auto i = std::find(widget->_p->media.begin(), widget->_p->media.end(), value);
                                if (i != widget->_p->media.end())
                                {
                                    widget->_p->mediaActionGroup->setChecked(i - widget->_p->media.begin());
                                }
                                widget->_p->mediaButton->setText(
                                    value ?
                                    value->getFileInfo().getFileName(Frame::invalid, false) :
                                    "-");
                                widget->_p->mediaButton->setTooltip(std::string(
                                    value ?
                                    value->getFileInfo() :
                                    Core::FileSystem::FileInfo()));
                            }
                        });
                }
            }

            if (windowSystem)
            {
                p.maximizeObserver = ValueObserver<bool>::create(
                    windowSystem->observeMaximize(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->mediaCanvas->setMaximize(value);
                        }
                    });

                p.fadeObserver = ValueObserver<float>::create(
                    windowSystem->observeFade(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->menuBar->setOpacity(value);
                    }
                });
            }
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<MediaCanvas>& MainWindow::getMediaCanvas() const
        {
            return _p->mediaCanvas;
        }

        void MainWindow::_dropEvent(Event::Drop & event)
        {
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                OpenOptions openOptions;
                openOptions.pos.reset(new glm::vec2(event.getPointerInfo().projectedPos));
                openOptions.spacing.reset(new float(style->getMetric(UI::MetricsRole::SpacingLarge)));
                auto fileSystem = context->getSystemT<FileSystem>();
                fileSystem->open(event.getDropPaths(), openOptions);
            }
        }

        void MainWindow::_initEvent(Event::Init& event)
        {
            Window::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.mediaButton->setTooltip(_getText(DJV_TEXT("menu_media_popup_tooltip")));
#ifdef DJV_DEMO
                p.titleLabel->setText(_getText(DJV_TEXT("djv_2_0_4")));
#endif // DJV_DEMO
            }
        }

    } // namespace ViewApp
} // namespace djv

