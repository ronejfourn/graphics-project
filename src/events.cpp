#include "events.hpp"
#include "common.hpp"

Events::Events()
{
    cursor = {0, 0};
    window = {0, 0, false};
    wheel  = 0;
    for (int i = 0; i < _KEY_TOTAL_; i ++)
        keyStates[i] = 0;
    for (int i = 0; i < _BTN_TOTAL_; i ++)
        btnStates[i] = 0;
    shouldClose = false;
}

void Events::advance()
{
    window.resized = false;
    wheel = 0;
    for (int i = 0; i < _KEY_TOTAL_; i ++)
        keyStates[i] = (keyStates[i] << 1) | (keyStates[i] & 1);
    for (int i = 0; i < _BTN_TOTAL_; i ++)
        btnStates[i] = (btnStates[i] << 1) | (btnStates[i] & 1);
}

bool Events::keyPressed(u32 k) const
{
    ASSERT(k < _KEY_TOTAL_, "key out of range");
    return (keyStates[k] & ~(keyStates[k] >> 1)) & 1;
}
bool Events::keyReleased(u32 k) const
{
    ASSERT(k < _KEY_TOTAL_, "key out of range");
    return (~keyStates[k] & (keyStates[k] >> 1)) & 1;
}
bool Events::keyHeld(u32 k) const
{
    ASSERT(k < _KEY_TOTAL_, "key out of range");
    return (keyStates[k] & (keyStates[k] >> 1)) & 1;
}

bool Events::buttonPressed(u32 b) const
{
    ASSERT(b < _BTN_TOTAL_, "btn out of range");
    return (btnStates[b] & ~(btnStates[b] >> 1)) & 1;
}
bool Events::buttonReleased(u32 b) const
{
    ASSERT(b < _BTN_TOTAL_, "btn out of range");
    return (~btnStates[b] & (btnStates[b] >> 1)) & 1;
}
bool Events::buttonHeld(u32 b) const
{
    ASSERT(b < _BTN_TOTAL_, "btn out of range");
    return (btnStates[b] & (btnStates[b] >> 1)) & 1;
}
