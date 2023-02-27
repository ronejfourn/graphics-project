#include "window/window.hpp"
#include "scene/scene.hpp"
#include "glad/glad.h"

#include <chrono>
#include <cstdio>

int main() {
    Window::Config cfg = { "Block Game" };
    Window::initialize(cfg);
    if (!gladLoadGL())
        die("failed to init glad");

    Window::swapInterval(0);

    std::chrono::high_resolution_clock clock;
    f32 deltaTime = 0;

    auto diff = [&clock](auto then) -> f32 {
        auto now = clock.now();
        std::chrono::duration<f32, std::milli> d = now - then;
        return d.count();
    };

    Scene scene;
    while (!Window::shouldClose()) {
        auto t1 = clock.now();

        auto events = Window::pollEvents();
        scene.update(events, deltaTime);
        scene.render();
        Window::swapBuffers();

        deltaTime = diff(t1);
        if (deltaTime < 16)
            Window::sleep(16 - deltaTime);
        deltaTime = diff(t1);
    }

    Window::terminate();

    return 0;
}
