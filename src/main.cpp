#include "platform.hpp"
#include "game.hpp"

#include <chrono>
#include <cstdio>

int main() {
    Platform &platform = Platform::instance();
    platform.swapInterval(1);

    std::chrono::high_resolution_clock a;

    Game &game = Game::instance();
    while (!platform.events.quit) {
        auto t1 = a.now();

        game.update(platform.events);

        auto t2 = a.now();

        game.render();

        auto t3 = a.now();

        platform.swapBuffers();
        platform.pollEvents();

        auto t4 = a.now();

        std::chrono::duration<double, std::milli> d1 = t2 - t1;
        std::chrono::duration<double, std::milli> d2 = t3 - t1;
        std::chrono::duration<double, std::milli> d3 = t4 - t1;

        // printf("update: %f\nrender: %f\ntotal: %f\n", d1.count(), d2.count(), d3.count());
    }

    return 0;
}
