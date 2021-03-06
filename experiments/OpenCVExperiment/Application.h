// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvDesktopApp/Application.h>

#include <djvSystem/FileInfo.h>

class Application : public djv::Desktop::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(std::list<std::string>&);
    Application();

public:
    ~Application() override;

    static std::shared_ptr<Application> create(std::list<std::string>&);

    void run() override;

private:
    void _open(const djv::System::File::Info&);

    DJV_PRIVATE();
};