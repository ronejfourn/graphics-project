#include "platform.hpp"
#include "game.hpp"

int main() {
    Platform &platform = Platform::instance();
    platform.swapInterval(1);

    Game &game = Game::instance();
    while (!platform.events.quit) {
        game.update(platform.events);
        game.render();

        platform.swapBuffers();
        platform.pollEvents();
    }

    return 0;
}
