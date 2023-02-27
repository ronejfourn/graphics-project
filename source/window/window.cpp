#include "window/window.hpp"
#include "window/events.hpp"

namespace Window {
    static bool initialized = false;

    void _initialize(const Config &);
    void _terminate();
    void _pollEvents();
    void _swapBuffers();
    void _swapInterval(i32 i);
    void _sleep(u32 ms);

    static Events events;
}

void Window::initialize(const Window::Config &cfg) {
    if (initialized) return;

    events.quit = false;
    events.window.w = cfg.width;
    events.window.h = cfg.height;

    _initialize(cfg);
    initialized = true;
}

void Window::terminate() {
    _terminate();
}

bool Window::shouldClose() {
    return events.quit;
}

const Events &Window::pollEvents() {
    ASSERT(initialized, "Platform not initialized");
    events.advance();
    _pollEvents();
    return events;
}

void Window::swapBuffers() {
    ASSERT(initialized, "Platform not initialized");
    _swapBuffers();
}

void Window::swapInterval(i32 i) {
    ASSERT(initialized, "Platform not initialized");
    _swapInterval(i);
}

void Window::sleep(u32 ms) {
    ASSERT(initialized, "Platform not initialized");
    _sleep(ms);
}

#if PLATFORM_X11
#include "platform/x11platform.cpp"
#elif PLATFORM_WIN32
#include "platform/win32platform.cpp"
#endif
