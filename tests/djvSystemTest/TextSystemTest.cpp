// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/TextSystemTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        TextSystemTest::TextSystemTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::TextSystemTest", tempPath, context)
        {}
                
        void TextSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<TextSystem>();
                
                for (const auto& i : system->getLocales())
                {
                    std::stringstream ss;
                    ss << "Locale: " << i;
                    _print(ss.str());
                }

                auto localeObserver = Observer::Value<std::string>::create(
                    system->observeCurrentLocale(),
                    [this](const std::string& value)
                    {
                        std::stringstream ss;
                        ss << "Current locale: " << value;
                        _print(ss.str());
                    });
                
                {
                    _print(system->getText("boolean_true"));
                    _print(system->getID("True"));
                }
                
                {
                    system->setCurrentLocale("zh");
                    system->setCurrentLocale("zh");
                    DJV_ASSERT("zh" == system->observeCurrentLocale()->get());
                    _print(system->getText("boolean_true"));
                    system->setCurrentLocale("en");
                }
            }
        }
                
    } // namespace SystemTest
} // namespace djv

