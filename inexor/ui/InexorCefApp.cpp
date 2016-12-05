#include <iostream>

#include "inexor/ui/InexorCefApp.hpp"

namespace inexor {
namespace ui {

InexorCefApp::InexorCefApp(int width, int height)
{
    spdlog::get("global")->info() << "init: cef: construct InexorCefApp (width: " << width << " height " << height << ")\n";

    context_manager = new context::InexorContextManager();

    layer_manager = new layer::InexorLayerManager(width, height);
    context_manager->AddSubContext(layer_manager);

    mouse_manager = new input::InexorMouseManager(layer_manager, width, height);
    context_manager->AddSubContext(mouse_manager);

    keyboard_manager = new input::InexorKeyboardManager(layer_manager);
    context_manager->AddSubContext(keyboard_manager);

    SetScreenSize(width, height);

    InitHudLayer();
    InitAppLayer();
}

void InexorCefApp::InitHudLayer()
{
    std::string layer_name("hud");
    std::string layer_url("http://localhost:48702/hud/");
    hud_layer = new layer::InexorHudLayer(layer_name, layer_url);
    hud_layer->Show();
    context_manager->AddSubContext(hud_layer);
    layer_manager->AddLayerProvider(hud_layer);
    spdlog::get("global")->debug() << "init: cef: hud layer\n";
}

void InexorCefApp::InitAppLayer()
{
    std::string layer_name("app");
    std::string layer_url("http://localhost:48702/treeui/");
    app_layer = new layer::InexorAppLayer(layer_name, layer_url);
    app_layer->Show();
    context_manager->AddSubContext(app_layer);
    layer_manager->AddLayerProvider(app_layer);
    spdlog::get("global")->debug() << "init: cef: app layer\n";
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
    spdlog::get("global")->debug() << " ====================== InexorCefApp::OnContextCreated() Injecting inexor object into javascript context\n";
    CefRefPtr<CefV8Value> window_object = context->GetGlobal();
    context->GetGlobal()->SetValue(context_manager->GetContextName(), context_manager->GetContext(), V8_PROPERTY_ATTRIBUTE_NONE);
}

bool InexorCefApp::HandleSdlEvent(SDL_Event event) {
    // std::cerr << "InexorCefApp::HandleSdlEvent()\n";
    switch(event.type) {
        case SDL_TEXTINPUT:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            GetKeyboardManager()->SendKeyEvent(event);
            return true;

        case SDL_MOUSEMOTION:
            GetMouseManager()->SendMouseMoveEvent(event);
            return true;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            GetMouseManager()->SendMouseClickEvent(event);
            return true;

        case SDL_MOUSEWHEEL:
            GetMouseManager()->SendMouseWheelEvent(event);
            return true;

        default:
            return false;
    }
}

}
}
