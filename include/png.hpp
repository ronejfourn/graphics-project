#pragma once

#include "common.hpp"
u8 *loadPNGFromFile(const char *file_name, u32 *w, u32 *h);
u8 *loadPNGFromMemory(const u8 *buffer, size_t size, u32 *w, u32 *h);
const char *getPNGError();
