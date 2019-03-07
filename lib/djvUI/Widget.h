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

#pragma once

#include <djvUI/Margin.h>
#include <djvUI/Style.h>

#include <djvAV/Enum.h>

#include <djvCore/BBox.h>
#include <djvCore/Context.h>
#include <djvCore/Event.h>
#include <djvCore/IObject.h>

#include <functional>
#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Font
        {
            class System;

        } // namespace Font

        namespace Render
        {
            class Render2D;

        } // namespace Render

    } // namespace AV

    namespace UI
    {
        class Action;
        class IWindowSystem;
        class IconSystem;
        class Tooltip;
        class UISystem;
        class Window;

        //! This class provides the base widget functionality.
        class Widget : public Core::IObject
        {
            DJV_NON_COPYABLE(Widget);

        protected:
            void _init(Core::Context *);
            inline Widget();

        public:
            virtual ~Widget();

            //! Create a new widget.
            static std::shared_ptr<Widget> create(Core::Context *);

            //! Get the top-level window.
            std::weak_ptr<Window> getWindow();

            //! \name Visibility
            ///@{

            inline bool isVisible(bool parents = false) const;
            virtual void setVisible(bool);
            void show();
            void hide();

            inline bool isClipped() const;
            inline const Core::BBox2f & getClipRect() const;

            inline float getOpacity(bool parents = false) const;
            void setOpacity(float);

            ///@}

            //! \name Layout
            ///@{

            inline const glm::vec2& getMinimumSize() const;
            inline const Core::BBox2f& getGeometry() const;
            inline glm::vec2 getSize() const;
            inline float getWidth() const;
            inline float getHeight() const;
            void setGeometry(const Core::BBox2f&);
            void move(const glm::vec2&);
            void resize(const glm::vec2&);

            //! This function is called during the layout event to determine the height of a
            //! widget based upon the given width. This is used for example by FlowLayout
            //! and TextBlock.
            inline virtual float getHeightForWidth(float) const;

            inline const Layout::Margin& getMargin() const;
            void setMargin(const Layout::Margin&);

            inline HAlign getHAlign() const;
            inline VAlign getVAlign() const;
            void setHAlign(HAlign);
            void setVAlign(VAlign);

            //! Utility function for computing the widget geometry.
            static Core::BBox2f getAlign(const Core::BBox2f&, const glm::vec2& minimumSize, HAlign, VAlign);

            ///@}

            //! \name Style
            ///@{

            inline ColorRole getBackgroundRole() const;
            void setBackgroundRole(ColorRole);

            ///@}

            //! \name Input
            ///@{

            inline bool isPointerEnabled() const;
            void setPointerEnabled(bool);

            bool hasTextFocus() const;
            void takeTextFocus();
            void releaseTextFocus();

            ///@}

            //! \name Actions
            ///@{

            inline const std::vector<std::shared_ptr<Action> > & getActions() const;
            virtual void addAction(const std::shared_ptr<Action>&);
            virtual void removeAction(const std::shared_ptr<Action>&);
            virtual void clearActions();

            ///@}

            //! \name Tooltip
            ///@{

            inline const std::string & getTooltip() const;
            void setTooltip(const std::string &);

            ///@}

            //! Get the number of widgets that currently exist.
            static size_t getGlobalWidgetCount();

            bool event(Core::Event::IEvent&) override;

        protected:
            //! \name Events
            ///@{

            virtual void _styleEvent(Core::Event::Style&) {}
            virtual void _preLayoutEvent(Core::Event::PreLayout&) {}
            virtual void _layoutEvent(Core::Event::Layout&) {}
            virtual void _clipEvent(Core::Event::Clip&) {}
            virtual void _paintEvent(Core::Event::Paint&);
            virtual void _paintOverlayEvent(Core::Event::PaintOverlay&) {}
            virtual void _pointerEnterEvent(Core::Event::PointerEnter&);
            virtual void _pointerLeaveEvent(Core::Event::PointerLeave&);
            virtual void _pointerMoveEvent(Core::Event::PointerMove&);
            virtual void _buttonPressEvent(Core::Event::ButtonPress&) {}
            virtual void _buttonReleaseEvent(Core::Event::ButtonRelease&) {}
            virtual void _scrollEvent(Core::Event::Scroll&) {}
            virtual void _dropEvent(Core::Event::Drop&) {}
            virtual void _keyPressEvent(Core::Event::KeyPress&);
            virtual void _keyReleaseEvent(Core::Event::KeyRelease&) {}
            virtual void _textFocusEvent(Core::Event::TextFocus&) {}
            virtual void _textFocusLostEvent(Core::Event::TextFocusLost&) {}
            virtual void _textEvent(Core::Event::Text&) {}

            ///@}

            //! \name Convenience Functions
            ///@{

            inline const std::weak_ptr<AV::Font::System>& _getFontSystem() const;
            inline const std::weak_ptr<AV::Render::Render2D>& _getRender() const;
            inline const std::weak_ptr<IconSystem>& _getIconSystem() const;
            inline const std::weak_ptr<Style::Style>& _getStyle() const;

            AV::Image::Color _getColorWithOpacity(const AV::Image::Color &) const;

            ///@}

            //! Call this function when the widget needs resizing.
            void _resize();

            //! Call this function to redraw the widget.
            void _redraw();

            //! Set the minimum size. This is computed and set in the pre-layout event.
            void _setMinimumSize(const glm::vec2&);

            inline float _getUpdateTime() const;
            inline float _getElapsedTime() const;
            inline const std::map<Core::Event::PointerID, glm::vec2> _getPointerHover() const;

            std::string _getTooltipText() const;
            std::shared_ptr<Widget> _createTooltipDefault(const std::string &);
            virtual std::shared_ptr<Widget> _createTooltip(const glm::vec2 & pos);

        private:
            float _updateTime  = 0.f;
            float _elapsedTime = 0.f;
            bool  _styleInit   = false;

            bool         _visible        = true;
            bool         _visibleInit    = true;
            bool         _parentsVisible = true;
            bool         _clipped        = false;
            Core::BBox2f _clipRect       = Core::BBox2f(0.f, 0.f, 0.f, 0.f);
            float        _opacity        = 1.f;
            float        _parentsOpacity = 1.f;

            Core::BBox2f   _geometry    = Core::BBox2f(0.f, 0.f, 0.f, 0.f);
            glm::vec2      _minimumSize = glm::vec2(0.f, 0.f);
            Layout::Margin _margin;
            HAlign         _hAlign      = HAlign::Fill;
            VAlign         _vAlign      = VAlign::Fill;

            ColorRole _backgroundRole = ColorRole::None;

            bool                                        _pointerEnabled = false;
            std::map<Core::Event::PointerID, glm::vec2> _pointerHover;

            std::vector<std::shared_ptr<Action> > _actions;

            std::string _tooltipText;
            struct TooltipData
            {
                float                    timer   = 0.f;
                std::shared_ptr<Tooltip> tooltip;
            };
            std::map<Core::Event::PointerID, TooltipData> _pointerToTooltips;

            static bool _resizeRequest;
            static bool _redrawRequest;

            static std::weak_ptr<AV::Font::System>     _fontSystem;
            static std::weak_ptr<AV::Render::Render2D> _render;
            static std::weak_ptr<UISystem>             _uiSystem;
            static std::weak_ptr<IconSystem>           _iconSystem;
            static std::weak_ptr<Style::Style>         _style;

            friend class IWindowSystem;
        };

    } // namespace UI
} // namespace djv

#include <djvUI/WidgetInline.h>
