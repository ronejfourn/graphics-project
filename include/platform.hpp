#pragma once

#include "common.hpp"
#include "events.hpp"

class Platform {
public:
    Events events;
    static Platform &instance();

    void pollEvents();
    void swapBuffers();
    void swapInterval(i32 i);
private:
    Platform();
    Platform(Platform&) = delete;
    ~Platform();

    void _init();
    void _destroy();
    void _pollEvents();
    void _swapBuffers();
    void _swapInterval(i32 i);
};
