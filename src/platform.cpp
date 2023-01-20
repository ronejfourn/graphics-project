#include "platform.hpp"
#include "events.hpp"

static Events ev;

#if PLATFORM_X11
#include "platform/x11platform.cpp"
#elif PLATFORM_WIN32
#include "platform/win32platform.cpp"
#endif

extern int blockGame(const Events &);

int main() {
    ev.window.w = DEFAULT_WINDOW_WIDTH;
    ev.window.h = DEFAULT_WINDOW_HEIGHT;
    return blockGame(ev);
}
