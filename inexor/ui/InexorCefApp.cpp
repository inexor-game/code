#include <iostream>

#include "inexor/ui/InexorCefApp.hpp"

namespace inexor {
namespace ui {

InexorCefApp::InexorCefApp(std::string instance_id, std::string host, std::string port, int width, int height)
{
    spdlog::get("ui")->info("init: cef: construct InexorCefApp (dimensions: {}x{})", width, height);

    context_manager = new context::InexorContextManager();

    layer_manager = new layer::InexorLayerManager(width, height);
    context_manager->AddSubContext(layer_manager);

    mouse_manager = new input::InexorMouseManager(layer_manager, width, height);
    context_manager->AddSubContext(mouse_manager);

    keyboard_manager = new input::InexorKeyboardManager(layer_manager);
    context_manager->AddSubContext(keyboard_manager);

    SetScreenSize(width, height);

    InitHudLayer(instance_id, host, port);
    InitConsoleLayer(instance_id, host, port);
    InitAppLayer(instance_id, host, port);
}

void InexorCefApp::InitHudLayer(std::string instance_id, std::string host, std::string port)
{
    std::string layer_name("hud");
    std::string layer_url("http://" + host + ":" + port + "/api/v1/interfaces/hud/index.html?instanceId=" + instance_id + "&host=" + host + "&port=" + port);
    hud_layer = new layer::InexorHudLayer(layer_name, layer_url);
    hud_layer->Show();
    context_manager->AddSubContext(hud_layer);
    layer_manager->AddLayerProvider(hud_layer);
    spdlog::get("ui")->debug("init: cef: hud layer");
}

void InexorCefApp::InitConsoleLayer(std::string instance_id, std::string host, std::string port)
{
    std::string layer_name("console");
    std::string layer_url("http://" + host + ":" + port + "/api/v1/interfaces/console/index.html?instanceId=" + instance_id + "&host=" + host + "&port=" + port);
    console_layer = new layer::InexorConsoleLayer(layer_name, layer_url);
    console_layer->Show();
    context_manager->AddSubContext(console_layer);
    layer_manager->AddLayerProvider(console_layer);
    spdlog::get("ui")->debug("init: cef: console layer");
}

void InexorCefApp::InitAppLayer(std::string instance_id, std::string host, std::string port)
{
    std::string layer_name("app");
    std::string layer_url("http://" + host + ":" + port + "/api/v1/interfaces/client-interface/index.html?instanceId=" + instance_id + "&host=" + host + "&port=" + port);
    app_layer = new layer::InexorAppLayer(layer_name, layer_url);
    app_layer->Hide();
    context_manager->AddSubContext(app_layer);
    layer_manager->AddLayerProvider(app_layer);
    spdlog::get("ui")->debug("init: cef: app layer");
}

void InexorCefApp::Destroy()
{
    layer_manager->DestroyLayers();
}

void InexorCefApp::SetScreenSize(int width, int height) {
    mouse_manager->SetScreenSize(width, height);
    layer_manager->SetScreenSize(width, height);
}

void InexorCefApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();
    layer_manager->InitializeLayers();
}

void InexorCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    spdlog::get("ui")->debug(" ====================== InexorCefApp::OnContextCreated() Injecting inexor object into javascript context");
    CefRefPtr<CefV8Value> window_object = context->GetGlobal();
    context->GetGlobal()->SetValue(context_manager->GetContextName(), context_manager->GetContext(), V8_PROPERTY_ATTRIBUTE_NONE);
}

bool InexorCefApp::HandleSdlEvent(SDL_Event &e)
{
    switch(e.type) {
        case SDL_TEXTINPUT:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            GetKeyboardManager()->SendKeyEvent(e);
            return true;

        case SDL_MOUSEMOTION:
            GetMouseManager()->SendMouseMoveEvent(e);
            return true;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            GetMouseManager()->SendMouseClickEvent(e);
            return true;

        case SDL_MOUSEWHEEL:
            GetMouseManager()->SendMouseWheelEvent(e);
            return true;

        default:
            return false;
    }
}

}
}
