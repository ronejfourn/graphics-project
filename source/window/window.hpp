#pragma once

#include "utility/common.hpp"

struct Events;

namespace Window {
    struct Config {
        const char *title = "Window";
        u32 width         = 1280;
        u32 height        = 720;
        i32 openglMajor   = 3;
        i32 openglMinor   = 3;

        bool doublebuffer = true;
        i32 redBits      = 8;
        i32 greenBits    = 8;
        i32 blueBits     = 8;
        i32 alphaBits    = 8;
        i32 depthBits    = 24;
        i32 stencilBits  = 0;
    };

    void initialize(const Config &cfg);
    void terminate();
    bool shouldClose();
    const Events &pollEvents();
    void swapBuffers();
    void swapInterval(i32 i);
    void sleep(u32 ms);
};
