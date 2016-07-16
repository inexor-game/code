#include <iostream>

#include "inexor/ui/CefSubsystem.hpp"
#include "inexor/ui/InexorSettings.hpp"
#include "inexor/util/ExecutablePath.hpp"

using namespace inexor::util;

// TODO: Namespace; encapsulate in the class; damint

SUBSYSTEM_REGISTER(cef, inexor::ui::CefSubsystem);

namespace inexor {
namespace ui {

CefRefPtr<InexorCefApp> cef_app;

CefSubsystem::CefSubsystem() {
    cef_app = new InexorCefApp(1920, 1080);
}

CefSubsystem::~CefSubsystem() {
    cef_app->Destroy();

    // Make sure any outstanding IPC messages are processed
    for(int i = 0; i < 100; i++)
        tick();

    // Stop any forked processes
    CefShutdown();

    // Destroy the cef app
    cef_app = nullptr;
}

void CefSubsystem::tick() {
    // std::cerr << "CefSubsystem::tick() --> CefDoMessageLoopWork()\n";
    CefDoMessageLoopWork();
}

void CefSubsystem::initialize(int argc, char **argv) {
    spdlog::get("global")->debug() << "CefSubsystem::initialize() --> CefInitialize()\n";
    const CefMainArgs args(argc, argv);
    std::string executable_path = inexor::util::ExecutablePathWithoutBinary(argv[0]);
    spdlog::get("global")->info() << "Detected executable path: " << executable_path << "\n";
    InexorSettings settings(executable_path);
    if (!CefInitialize(args, settings, cef_app.get(), NULL)) {
        spdlog::get("global")->error() << "FATAL: Initialization of CEF subprocess failed!\n";
    }
}


}
}
