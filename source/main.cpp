#include "platform/platform.hpp"
#include "scene/scene.hpp"

#include <chrono>
#include <cstdio>

int main() {
    Platform &platform = Platform::instance();
    platform.swapInterval(0);

    std::chrono::high_resolution_clock a;
    std::chrono::duration<f64, std::milli> d;
    f64 dt = 0;

    Scene &scene = Scene::instance();
    while (!platform.events.quit) {
        auto t1 = a.now();

        scene.update(platform.events);
        scene.render();

        platform.swapBuffers();
        platform.pollEvents();

        auto t2 = a.now();

        d = t2 - t1;
        dt = d.count();

        if (dt < 16)
            platform.sleep(16 - dt);

        auto t3 = a.now();
        d  = t3 - t1;
        dt = d.count();
    }

    return 0;
}
