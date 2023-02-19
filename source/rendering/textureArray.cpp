#include "rendering/textureArray.hpp"
#include "utility/png.hpp"
#include "glad/glad.h"
#include <memory.h>

TextureArray::TextureArray(const char *path, int x, int y)
{
    u32 imgW, imgH;

    u8 *data = loadPNGFromFile(path, &imgW, &imgH);
    if (!data)
        die("while creating texture array: %s\n", getPNGError());
    tileX = x;
    tileY = y;

    u32 tileW = imgW / tileX;
    u32 tileH = imgH / tileY;

    u32 imageCount = tileX * tileY;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, tileW, tileH, imageCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    u32 tileSizeX = tileW * 4;
    u32 rowlen = tileSizeX * tileX;
    u32 tileRowSize = rowlen * tileH;
    u8 *tile = new u8[tileSizeX * tileH];

    for (u32 iy = 0; iy < tileY; ++iy) {
        for (u32 ix = 0; ix < tileX; ++ix) {
            u8 *ptr = data + iy * tileRowSize + ix * tileSizeX;
            for (u32 row = 0; row < tileH; ++row)
                memcpy(tile + row * tileSizeX, ptr + row * rowlen, tileSizeX);

            int i = iy * tileX + ix;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileW, tileH, 1, GL_RGBA, GL_UNSIGNED_BYTE, tile);
        }
    }
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    delete[] tile;
}

void TextureArray::bind()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}
