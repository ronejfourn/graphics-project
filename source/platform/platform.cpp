#include "glad/glad.h"
#include "platform/platform.hpp"

Platform::Platform() {
    events.window.w = DEFAULT_WINDOW_WIDTH;
    events.window.h = DEFAULT_WINDOW_HEIGHT;
    _init();
    if (!gladLoadGL())
        die("failed to init glad");
    glViewport(0, 0, events.window.w, events.window.h);
}

Platform::~Platform() {
    _destroy();
}

Platform &Platform::instance() {
    static Platform s_instance;
    return s_instance;
}

void Platform::pollEvents() {
    events.advance();
    _pollEvents();
}

void Platform::swapBuffers() {
    _swapBuffers();
}

void Platform::swapInterval(i32 i) {
    _swapInterval(i);
}

void Platform::sleep(u32 ms) {
    _sleep(ms);
}

#if PLATFORM_X11
#include "specific/x11platform.cpp"
#elif PLATFORM_WIN32
#include "specific/win32platform.cpp"
#endif
