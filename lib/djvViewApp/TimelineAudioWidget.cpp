// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelineWidgetPrivate.h>

#include <djvViewApp/Media.h>

#include <djvUI/FloatSlider.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AudioWidget::Private
        {
            std::shared_ptr<Media> media;
            float volume = 0.F;
            bool mute = false;

            std::shared_ptr<UI::Numeric::FloatSlider> volumeSlider;
            std::shared_ptr<UI::ToolButton> muteButton;
            std::shared_ptr<UI::HorizontalLayout> layout;

            std::shared_ptr<Observer::Value<float> > volumeObserver;
            std::shared_ptr<Observer::Value<bool> > muteObserver;
        };

        void AudioWidget::_init(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::AudioWidget");

            p.media = media;

            p.volumeSlider = UI::Numeric::FloatSlider::create(context);
            p.volumeSlider->setRange(Math::FloatRange(0.F, 100.F));
            p.volumeSlider->getModel()->setSmallIncrement(1.F);
            p.volumeSlider->getModel()->setLargeIncrement(10.F);

            p.muteButton = UI::ToolButton::create(context);
            p.muteButton->setIcon("djvIconAudioMute");
            p.muteButton->setButtonType(UI::ButtonType::Toggle);
            p.muteButton->setVAlign(UI::VAlign::Fill);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.volumeSlider);
            p.layout->addChild(p.muteButton);
            addChild(p.layout);

            auto weak = std::weak_ptr<AudioWidget>(std::dynamic_pointer_cast<AudioWidget>(shared_from_this()));
            p.volumeSlider->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->media)
                        {
                            widget->_p->media->setVolume(value / 100.F);
                        }
                    }
                });

            p.muteButton->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->media)
                        {
                            widget->_p->media->setMute(value);
                        }
                    }
                });

            p.volumeObserver = Observer::Value<float>::create(
                p.media->observeVolume(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->volume = value;
                        widget->_widgetUpdate();
                    }
                });

            p.muteObserver = Observer::Value<bool>::create(
                p.media->observeMute(),
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->mute = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        AudioWidget::AudioWidget() :
            _p(new Private)
        {}

        std::shared_ptr<AudioWidget> AudioWidget::create(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AudioWidget>(new AudioWidget);
            out->_init(media, context);
            return out;
        }

        void AudioWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void AudioWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void AudioWidget::_initEvent(System::Event::Init& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.volumeSlider->setTooltip(_getText(DJV_TEXT("audio_volume_tooltip")));
                p.muteButton->setTooltip(_getText(DJV_TEXT("audio_mute_tooltip")));
            }
        }

        void AudioWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.volumeSlider->setValue(p.volume * 100.F);
            p.muteButton->setChecked(p.mute);
        }

    } // namespace ViewApp
} // namespace djv

