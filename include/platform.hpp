#pragma once

#include "common.hpp"

bool plInitialize  ();
void plTerminate   ();
void plSwapBuffers ();
void plPollEvents  ();
void plSwapInterval(i32 i);
