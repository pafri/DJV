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

#include <djvUI/ListButton.h>

#include <djvUI/Action.h>
#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

#include <djvAV/Render2D.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Button
        {
            struct List::Private
            {
                std::shared_ptr<Icon> icon;
                std::shared_ptr<Label> label;
                TextHAlign textHAlign = TextHAlign::Left;
                std::string font;
                std::string fontFace;
                MetricsRole fontSizeRole = MetricsRole::FontMedium;
                std::shared_ptr<HorizontalLayout> layout;
            };

            void List::_init(Context * context)
            {
                IButton::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::Button::List");

                p.layout = HorizontalLayout::create(context);
                p.layout->setMargin(MetricsRole::MarginSmall);
                addChild(p.layout);
            }

            List::List() :
                _p(new Private)
            {}

            List::~List()
            {}

            std::shared_ptr<List> List::create(Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string & text, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setText(text);
                return out;
            }

            std::shared_ptr<List> List::create(const std::string & text, const std::string & icon, Context * context)
            {
                auto out = std::shared_ptr<List>(new List);
                out->_init(context);
                out->setIcon(icon);
                out->setText(text);
                return out;
            }

            std::string List::getIcon() const
            {
                DJV_PRIVATE_PTR();
                return p.icon ? p.icon->getIcon() : std::string();
            }

            void List::setIcon(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.icon)
                    {
                        p.icon = Icon::create(getContext());
                        p.icon->setVAlign(VAlign::Center);
                        p.icon->setIconColorRole(isChecked() ? getCheckedColorRole() : getForegroundColorRole());
                        p.layout->addChild(p.icon);
                        p.icon->moveToFront();
                    }
                    p.icon->setIcon(value);
                }
                else
                {
                    p.layout->removeChild(p.icon);
                    p.icon.reset();
                }
            }

            std::string List::getText() const
            {
                DJV_PRIVATE_PTR();
                return p.label ? p.label->getText() : std::string();
            }

            void List::setText(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                if (!value.empty())
                {
                    if (!p.label)
                    {
                        p.label = Label::create(getContext());
                        p.label->setTextHAlign(p.textHAlign);
                        p.label->setFont(p.font);
                        p.label->setFontFace(p.fontFace);
                        p.label->setFontSizeRole(p.fontSizeRole);
                        p.layout->addChild(p.label);
                        p.layout->setStretch(p.label, RowStretch::Expand);
                        p.label->moveToBack();
                    }
                    p.label->setText(value);
                }
                else
                {
                    p.layout->removeChild(p.label);
                    p.label.reset();
                }
            }

            TextHAlign List::getTextHAlign() const
            {
                return _p->textHAlign;
            }

            void List::setTextHAlign(TextHAlign value)
            {
                DJV_PRIVATE_PTR();
                p.textHAlign = value;
                if (p.label)
                {
                    p.label->setTextHAlign(value);
                }
            }

            const std::string & List::getFont() const
            {
                return _p->font;
            }

            const std::string & List::getFontFace() const
            {
                return _p->fontFace;
            }

            MetricsRole List::getFontSizeRole() const
            {
                return _p->fontSizeRole;
            }

            void List::setFont(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.font = value;
                if (p.label)
                {
                    p.label->setFont(value);
                }
            }

            void List::setFontFace(const std::string & value)
            {
                DJV_PRIVATE_PTR();
                p.fontFace = value;
                if (p.label)
                {
                    p.label->setFontFace(value);
                }
            }

            void List::setFontSizeRole(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                p.fontSizeRole = value;
                if (p.label)
                {
                    p.label->setFontSizeRole(value);
                }
            }

            const Layout::Margin & List::getInsideMargin() const
            {
                return _p->layout->getMargin();
            }

            void List::setInsideMargin(const Layout::Margin & value)
            {
                _p->layout->setMargin(value);
            }

            void List::setChecked(bool value)
            {
                IButton::setChecked(value);
                if (_p->icon)
                {
                    _p->icon->setIconColorRole(value ? getCheckedColorRole() : getForegroundColorRole());
                }
            }

            void List::_preLayoutEvent(Event::PreLayout & event)
            {
                auto style = _getStyle();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                glm::vec2 size = _p->layout->getMinimumSize();
                const ButtonType buttonType = getButtonType();
                if (ButtonType::Toggle    == buttonType ||
                    ButtonType::Radio     == buttonType ||
                    ButtonType::Exclusive == buttonType)
                {
                    size.x += m;
                }
                _setMinimumSize(size);
            }

            void List::_layoutEvent(Event::Layout &)
            {
                auto style = _getStyle();
                BBox2f g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const ButtonType buttonType = getButtonType();
                if (ButtonType::Toggle == buttonType ||
                    ButtonType::Radio == buttonType ||
                    ButtonType::Exclusive == buttonType)
                {
                    g.min.x += m;
                }
                _p->layout->setGeometry(g);
            }

            void List::_paintEvent(Event::Paint& event)
            {
                IButton::_paintEvent(event);
                DJV_PRIVATE_PTR();
                auto render = _getRender();
                auto style = _getStyle();
                const BBox2f& g = getGeometry();
                const float m = style->getMetric(MetricsRole::MarginSmall);
                const ColorRole checkedColorRole = getCheckedColorRole();
                if (_isToggled() && checkedColorRole != ColorRole::None)
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(checkedColorRole)));
                    render->drawRect(BBox2f(g.min.x, g.min.y, m, g.h()));
                }
                if (_isPressed())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getPressedColorRole())));
                    render->drawRect(g);
                }
                else if (_isHovered())
                {
                    render->setFillColor(_getColorWithOpacity(style->getColor(getHoveredColorRole())));
                    render->drawRect(g);
                }
            }

        } // namespace Button
    } // namespace UI
} // namespace Gp
