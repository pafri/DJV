// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FontSettings.h>

#include <djvRender2D/FontSystem.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct Font::Private
            {
                std::shared_ptr<Observer::MapSubject<std::string, std::string> > localeFonts;
            };

            void Font::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Font", context);

                DJV_PRIVATE_PTR();
                p.localeFonts = Observer::MapSubject<std::string, std::string>::create();
                p.localeFonts->setItem("Default", Render2D::Font::familyDefault);
                p.localeFonts->setItem("ja", "Noto Sans CJK JP");
                p.localeFonts->setItem("ko", "Noto Sans CJK KR");
                p.localeFonts->setItem("zh", "Noto Sans CJK SC");

                _load();
            }

            Font::Font() :
                _p(new Private)
            {}

            Font::~Font()
            {}

            std::shared_ptr<Font> Font::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Font>(new Font);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<Observer::MapSubject<std::string, std::string> >& Font::observeLocaleFonts() const
            {
                return _p->localeFonts;
            }

            void Font::load(const rapidjson::Value& value)
            {
                if (value.IsObject())
                {
                    read("LocaleFonts", value, _p->localeFonts);
                }
            }

            rapidjson::Value Font::save(rapidjson::Document::AllocatorType& allocator)
            {
                rapidjson::Value out(rapidjson::kObjectType);
                write("LocaleFonts", _p->localeFonts->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

