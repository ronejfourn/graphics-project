#include "png.hpp"
#include <stdlib.h>
#include <memory.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_HLIT 286
#define MAX_HDIST 32
#define MAX_CODE_LENGTH 15

static const char *errorStr = "";
const char *getPNGError() {return errorStr;}

struct ByteBuffer
{
    u8 *bytes;
    i32 bitCount;
    i32 byteCount;
};

struct Huffman
{
    u32 maxLen, symCount;
    u32 *symbols;
    u32 *lengths;
    u32 *counts;
};

struct Chunk
{
    u32 type, crc;
    ByteBuffer data;
};

typedef const void * Bytes;

static u8*   readEntireFile(const char *fileName, size_t *size);
static u32   correctEndian(u32 a);
static Bytes getBytes(ByteBuffer *lb, i32 n, i32 *r);
static bool  getChunk(ByteBuffer *b, Chunk* c, u32 type);
static i32   checkSignature(ByteBuffer *lb);
#define      getType(lb, T, r) *(const T*)getBytes(lb, sizeof(T), r)
static u32   idatGetBits(ByteBuffer *idat, u8  n, ByteBuffer *b, i32 *r);
#define      idatGetType(lb, T, b, r) *(const T*)idatGetBytes(lb, sizeof(T), b, r)
static i32   uncompressed(ByteBuffer *idat, ByteBuffer *b, i32 idx, u8 *decompData);
static void  huffmanConstruct(Huffman *h);
static u32   huffmanDecode(ByteBuffer *idat, ByteBuffer *b, Huffman *h);
static bool  fixedHuffman (u32 *litlendistLengths, Huffman *litlenHuffman, Huffman *distHuffman);
static bool  dynamicHuffman(ByteBuffer *idat, ByteBuffer *b, u32 *litlendistLengths, Huffman *litlenHuffman, Huffman *distHuffman);
static u32   decompressUsingHuffman(u8 *decompData, ByteBuffer *idat, ByteBuffer *b, u32 idx, Huffman *litlenHuffman, Huffman *distHuffman);
static bool  reverseFilter(u8 *pixels, u8 *decomData, u32 bpp, u32 width, u32 height);
static u8    paethPredictor(i16 a, i16 b, i16 c);

//
//
//

u8 *loadPNGFromFile(const char *fileName, u32 *w, u32 *h)
{
    size_t size = 0;
    u8 *buffer = readEntireFile(fileName, &size);
    if (!buffer) {
        errorStr = "failed to open file";
        return nullptr;
    }
    u8 *ret = loadPNGFromMemory(buffer, size, w, h);
    free(buffer);
    return ret;
}

u8 *loadPNGFromMemory(const u8 *buffer, size_t size, u32 *w, u32 *h)
{
    *w = 0, *h = 0;
    ByteBuffer bbuf;
    bbuf.bytes = (u8 *)buffer;
    bbuf.byteCount = size;
    bbuf.bitCount  = 8;

    if (!checkSignature(&bbuf)) {
        errorStr = "not a PNG file, PNG signature not found";
        return nullptr;
    }

    i32 r;
    Chunk chunk;
    if (!getChunk(&bbuf, &chunk, *(u32*)"IHDR")) {
        errorStr = "corrupt PNG, IHDR chunk is not first";
        return nullptr;
    }

    struct {
        u32 w, h;
        u8 bitDepth, colorType, compMethod;
        u8 filtMethod, laceMethod;
    } ihdr = {};

    ihdr.w = getType(&chunk.data, u32, &r);
    ihdr.h = getType(&chunk.data, u32, &r);
    ihdr.w = correctEndian(ihdr.w);
    ihdr.h = correctEndian(ihdr.h);
    ihdr.bitDepth   = getType(&chunk.data, u8, &r);
    ihdr.colorType  = getType(&chunk.data, u8, &r);
    ihdr.compMethod = getType(&chunk.data, u8, &r);
    ihdr.filtMethod = getType(&chunk.data, u8, &r);
    ihdr.laceMethod = getType(&chunk.data, u8, &r);

#define EASSERT(cond, err) if (!(cond)) {errorStr = err; return nullptr;}
    EASSERT(ihdr.bitDepth   == 8, "unsupported bit depth");
    EASSERT(ihdr.colorType  == 2 || ihdr.colorType == 6, "unsupported color type");
    EASSERT(ihdr.compMethod == 0, "unsupported compression method");
    EASSERT(ihdr.filtMethod == 0, "unsupported filter method");
    EASSERT(ihdr.laceMethod == 0, "unsupported interlacing method");

    while (chunk.type != *(u32*)"IDAT") {
        if (!getChunk(&bbuf, &chunk, 0)) {
            errorStr = "corrupt PNG, IDAT chunks missing";
            return nullptr;
        }
    }

    ByteBuffer idat = chunk.data;
    u8 cmf = getType(&idat, u8, &r);
    u8 flg = getType(&idat, u8, &r); (void)flg;
    u8 cm  = (cmf & ((1 << 4) - 1));
    EASSERT(cm == 8, "compression method must be 'deflate'");
#undef EASSERT

    u32 idx = 0;
    u32 bpp = (ihdr.colorType == 2 ? 3 : 4);
    u32 decompSize = ihdr.w * ihdr.h * bpp + ihdr.h;
    u8 *decompData = (u8*)calloc(decompSize, 1);
    if (!decompData) {
        errorStr = "out of memory";
        return nullptr;
    }

    u32 litlendistLengths[MAX_HLIT + MAX_HDIST];
    u32 litlenSymbols[MAX_HLIT], litlenCounts[MAX_CODE_LENGTH + 1];
    u32 distSyms[MAX_HDIST], distCounts[MAX_CODE_LENGTH + 1];

    Huffman litlenHuffman;
    litlenHuffman.symbols = litlenSymbols;
    litlenHuffman.counts  = litlenCounts;

    Huffman distHuffman;
    distHuffman.symbols = distSyms;
    distHuffman.counts  = distCounts;

    u32 bfinal, btype;
    do {
        bfinal = idatGetBits(&idat, 1, &bbuf, &r);
        btype  = idatGetBits(&idat, 2, &bbuf, &r);

        if (btype == 0) {
            idx = uncompressed(&idat, &bbuf, idx, decompData);
            if (!idx) {
                free(decompData);
                return nullptr;
            }
        }

        if (btype == 3) {
            errorStr = "corrupt PNG, bad BTYPE";
            free(decompData);
            return nullptr;
        }

        bool ok = (btype == 1) ?
            fixedHuffman  (litlendistLengths, &litlenHuffman, &distHuffman) :
            dynamicHuffman(&idat, &bbuf, litlendistLengths, &litlenHuffman, &distHuffman);

        if (!ok) {
            free(decompData);
            return nullptr;
        }

        huffmanConstruct(&litlenHuffman);
        huffmanConstruct(&distHuffman);
        idx = decompressUsingHuffman(decompData, &idat, &bbuf, idx, &litlenHuffman, &distHuffman);
        if (!idx) {
            free(decompData);
            return nullptr;
        }
    } while (!bfinal);

    if (idat.bitCount != 8) {
        idat.bytes     += 1;
        idat.bitCount   = 8;
        idat.byteCount -= 1;
    }

    u32 alder32 = idatGetBits(&idat, 32, &bbuf, &r); (void)alder32;
    ASSERT(idat.bitCount == 0 && idat.byteCount == 1, "");
    u8 *pixels = (u8 *)calloc(1, 4 * ihdr.w * ihdr.h);
    if (!pixels) {
        errorStr = "out of memory";
        free(decompData);
        return nullptr;
    }
    memset(pixels, 0xff, 4 * ihdr.w * ihdr.h);

    if (!reverseFilter(pixels, decompData, bpp, ihdr.w, ihdr.h)) {
        free(pixels);
        free(decompData);
        return nullptr;
    }

    free(decompData);

    *w = ihdr.w, *h = ihdr.h;
    return pixels;
}

//
//
//

Bytes getBytes(ByteBuffer *lb, i32 n, i32 *r)
{
    *r = n;
    ASSERT(lb->bytes && lb->bitCount == 8, "bad ByteBuffer");
    const void *ret = (const void *)lb->bytes;
    if (n > lb->byteCount) {
        n -= lb->byteCount;
        lb->byteCount = 0;
        lb->bitCount  = 0;
        lb->bytes     = nullptr;
    } else {
        lb->bytes     += n;
        lb->byteCount -= n;
        n -= n;
    }
    *r = n;
    return ret;
}

bool getChunk(ByteBuffer *b, Chunk* c, u32 type)
{
    i32 r;
    c->data.bitCount  = 8;
    c->data.byteCount = getType(b, u32, &r);
    if (r != 0) return false;
    c->data.byteCount = correctEndian(c->data.byteCount);
    c->type = getType(b, u32, &r);
    if (r != 0) return false;
    if (b->byteCount < c->data.byteCount) return false;
    c->data.bytes = b->bytes;
    b->bytes     += c->data.byteCount;
    b->byteCount -= c->data.byteCount;
    c->crc = getType(b, u32, &r);
    if (r != 0) return false;
    return type == 0 || c->type == type;
}

i32 uncompressed(ByteBuffer *idat, ByteBuffer *b, i32 idx, u8 *decompData)
{
    if (idat->byteCount > 0) {
        idat->bytes ++;
        idat->byteCount --;
        idat->bitCount = 8;
    } else {
        idat->bytes     = nullptr;
        idat->byteCount = 0;
        idat->bitCount  = 0;
    }

    i32 r;
    u16 len = 0, nlen = 0;
    len  |= idatGetBits(idat, 8, b, &r) << 8;
    len  |= idatGetBits(idat, 8, b, &r) << 0;
    nlen |= idatGetBits(idat, 8, b, &r) << 8;
    nlen |= idatGetBits(idat, 8, b, &r) << 0;
    if (len != ~nlen) {
        errorStr = "corrupt PNG, invalid LEN";
        return 0;
    }

    while (len) {
        Bytes dat = getBytes(idat, len, &r);
        memcpy(decompData + idx, dat, len - r);
        idx += len - r;
        len = r;
        if (len) {
            Chunk chunk;
            if (!getChunk(b, &chunk, *(u32*)"IDAT")) {
                errorStr = "corrupt PNG, IDAT chunks missing or not consecutive";
                return 0;
            }

            *idat = chunk.data;
        }
    }
    return idx;
}

bool fixedHuffman(u32 *litlendistLengths, Huffman *litlenHuffman, Huffman *distHuffman)
{
#define FIXED_MAX_LIT_VALUE 288
#define FIXED_MAX_DIST_VALUE 31
    i32 i = 0;
    for (; i < FIXED_MAX_LIT_VALUE; i ++) {
        u32 v = 0;
        if (i <= 143) v = 8;
        else if (i <= 255) v = 9;
        else if (i <= 279) v = 7;
        else v = 8;
        litlendistLengths[i] = v;
    }

    for (; i < MAX_HLIT + MAX_HDIST; i++)
        litlendistLengths[i] = 5;

    litlenHuffman->lengths  = litlendistLengths;
    litlenHuffman->symCount = FIXED_MAX_LIT_VALUE;
    litlenHuffman->maxLen   = MAX_CODE_LENGTH;

    distHuffman->lengths  = litlendistLengths + FIXED_MAX_LIT_VALUE;
    distHuffman->symCount = FIXED_MAX_DIST_VALUE;
    distHuffman->maxLen   = MAX_CODE_LENGTH;

    return 1;
#undef FIXED_MAX_LIT_VALUE
#undef FIXED_MAX_DIST_VALUE
}

bool dynamicHuffman(ByteBuffer *idat, ByteBuffer *b, u32 *litlendistLengths, Huffman *litlenHuffman, Huffman *distHuffman)
{
#define MAX_HCLEN 19
#define MAX_CL_CODE_LENGTH 7
    i32 r;
    u32 hlit  = idatGetBits(idat, 5, b, &r) + 257;
    u32 hdist = idatGetBits(idat, 5, b, &r) + 1;
    u32 hclen = idatGetBits(idat, 4, b, &r) + 4;

    u8  order[MAX_HCLEN] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    u32 lengths[MAX_HCLEN], symbols[19], counts[MAX_CL_CODE_LENGTH + 1];
    memset(lengths, 0, sizeof(lengths));

    for (u32 i = 0; i < hclen; i ++)
        lengths[order[i]] = idatGetBits(idat, 3, b, &r);

    Huffman codelenHuffman;
    codelenHuffman.lengths  = lengths;
    codelenHuffman.symbols  = symbols;
    codelenHuffman.counts   = counts;
    codelenHuffman.maxLen   = MAX_CL_CODE_LENGTH;
    codelenHuffman.symCount = MAX_HCLEN;
    huffmanConstruct(&codelenHuffman);

    u32 index = 0;
    while (index < hlit + hdist) {
        i32 symbol = huffmanDecode(idat, b, &codelenHuffman);
        u32 rep = 0;
        if (symbol <= 15) {
            rep = 1;
        } else if (symbol == 16) {
            symbol = litlendistLengths[index - 1];
            rep = idatGetBits(idat, 2, b, &r) + 3;
        } else if (symbol == 17) {
            symbol = 0;
            rep = idatGetBits(idat, 3, b, &r) + 3;
        } else if (symbol == 18) {
            symbol = 0;
            rep = idatGetBits(idat, 7, b, &r) + 11;
        } else {
            errorStr = "corrupt PNG, bad symbol while constructing dynamic huffman tree";
            return false;
        }
        while (rep --)
            litlendistLengths[index ++] = symbol;
    }

    litlenHuffman->lengths  = litlendistLengths;
    litlenHuffman->maxLen   = MAX_CODE_LENGTH;
    litlenHuffman->symCount = hlit;

    distHuffman->lengths  = litlendistLengths + hlit;
    distHuffman->maxLen   = MAX_CODE_LENGTH;
    distHuffman->symCount = hdist;
    return true;
#undef MAX_HCLEN
#undef MAX_CL_CODE_LENGTH
}

u32 decompressUsingHuffman(u8 *decompData, ByteBuffer *idat, ByteBuffer *b, u32 idx, Huffman *litlenHuffman, Huffman *distHuffman)
{
    const u16  lenBase[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, };
    const u8  lenExtra[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, };
    const u16 distBase[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, };
    const u8 distExtra[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, };
    i32 r;

    for (;;) {
        /* decode literal/length value from input stream */
        u32 value = huffmanDecode(idat, b, litlenHuffman);

        if (value < 256) {
            /* copy value (literal byte) to output stream */
            decompData[idx++] = value;
            continue;
        }

        if (value == 256) /* end of block */
            break;

        if(!(value >= 257 && value <= 285)) {
            errorStr = "corrupt PNG, invalid length value while decompressing";
            return 0;
        }

        u16 lenIndex = value - 257;
        u32 len = lenBase[lenIndex];
        u8 extraBits = lenExtra[lenIndex];
        len += idatGetBits(idat, extraBits, b, &r);

        /* decode distance from input stream */
        u32 distIndex = huffmanDecode(idat, b, distHuffman);
        u32 dist = distBase[distIndex];
        extraBits = distExtra[distIndex];
        dist += idatGetBits(idat, extraBits, b, &r);

        /* move backwards distance bytes in the output */
        u8 *src = decompData + idx - dist;
        while (len --) {
            /* stream, and copy length bytes from this */
            /* position to the output stream. */
            decompData[idx++] = *src;
            src++;
        }
    }

    return idx;
}

bool reverseFilter(u8 *pixels, u8 *decompData, u32 bpp, u32 width, u32 height)
{
    u8 *filt, *recon, *prev, type = 0;
    enum { ftNone = 0, ftSub, ftUp, ftAverage, ftPaeth, firstAverage};
    type  = *decompData;
    filt  = decompData + 1;
    recon = pixels;
    prev  = recon - 4 * width;
    type  = (type == ftUp) ? ftNone : (type == ftPaeth) ? ftSub :
            (type == ftAverage) ? firstAverage : type;

    u32 bpl = bpp * width + 1;
    for (u32 i = 0; i < height; i ++) {
        type = *decompData;
        filt = decompData + 1;
        if (type == ftNone) {
            for (u32 i = 0; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j];
                recon += 4, prev += 4, filt += bpp;
            }
        } else if (type == ftSub) {
            for (u32 j = 0; j < bpp; j ++)
                recon[j] = filt[j];
            recon += 4, prev += 4, filt += bpp;
            for (u32 i = 1; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j] + (recon - 4)[j];
                recon += 4, prev += 4, filt += bpp;
            }
        } else if (type == ftUp) {
            for (u32 i = 0; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j] + prev[j];
                recon += 4, prev += 4, filt += bpp;
            }
        } else if (type == ftAverage) {
            for (u32 j = 0; j < bpp; j ++)
                recon[j] = filt[j] + prev[j] / 2;
            recon += 4, prev += 4, filt += bpp;
            for (u32 i = 1; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j] + (prev[j] + (recon - 4)[j]) / 2;
                recon += 4, prev += 4, filt += bpp;
            }
        } else if (type == ftPaeth) {
            for (u32 j = 0; j < bpp; j ++)
                recon[j] = filt[j] + prev[j];
            recon += 4, prev += 4, filt += bpp;
            for (u32 i = 1; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j] + paethPredictor((recon - 4)[j], prev[j], (prev - 4)[j]);
                recon += 4, prev += 4, filt += bpp;
            }
        }else if (type == firstAverage) {
            for (u32 j = 0; j < bpp; j ++)
                recon[j] = filt[j];
            recon += 4, prev += 4, filt += bpp;
            for (u32 i = 1; i < width; i ++) {
                for (u32 j = 0; j < bpp; j ++)
                    recon[j] = filt[j] + (recon - 4)[j] / 2;
                recon += 4, prev += 4, filt += bpp;
            }
        } else {
            errorStr = "corrupt PNG, invalid filter type";
            return false;
        }
        decompData += bpl;
    }
    return true;
}

u8 paethPredictor(i16 a, i16 b, i16 c)
{
#define ABS(c) ((c) < 0 ? -(c) : (c))
    i16 p = a + b - c;
    i16 pa = ABS(p - a);
    i16 pb = ABS(p - b);
    i16 pc = ABS(p - c);
    u8 Pr;
    if (pa <= pb && pa <= pc) Pr = a;
    else if (pb <= pc) Pr = b;
    else Pr = c;
    return Pr;
#undef ABS
}

i32 checkSignature(ByteBuffer *lb)
{
    i32 r;
    u8 sig[8] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
    return *(u64*)sig == *(u64*)getBytes(lb, 8, &r);
}

u32 correctEndian(u32 a)
{
#define BSWAP(a) (a << 24) | ((a & 0xff00) << 8) | ((a >> 8) & 0xff00) | (a >> 24)
    i32 x = 1;
    char k = *(char *)&x;
    if (k) return BSWAP(a);
    else return a;
#undef BSWAP
}

u32 idatGetBits(ByteBuffer *idat, u8 n, ByteBuffer *b, i32 *r)
{
    ASSERT(n <= 32, "getting too many bits");
    u32 ret = 0, i = 0;
    while (n > 0) {
        if (idat->byteCount <= 0) {
            Chunk chunk;
            if (!getChunk(b, &chunk, *(u32*)"IDAT")) {
                errorStr = "corrupt PNG, IDAT chunks missing or not consecutive";
                break;
            }
            *idat = chunk.data;
        }
        u8 bits = *idat->bytes >> (8 - idat->bitCount);
        if (n > idat->bitCount) {
            ret |= bits << i;
            i += idat->bitCount;
            n -= idat->bitCount;
            idat->bytes ++;
            idat->byteCount --;
            idat->bitCount = 8;
        } else if (idat->bitCount > 0) {
            bits &= (1 << n) - 1;
            ret |= bits << i;
            idat->bitCount -= n;
            i += n;
            n -= n;
        }
        else break;
    }
    return ret;
}

void huffmanConstruct(Huffman *h)
{
    u32 base[MAX_CODE_LENGTH + 1];
    memset(h->counts , 0, sizeof(u32) * (h->maxLen + 1));
    for (u32 i = 0; i < h->symCount; i ++)
        h->counts[h->lengths[i]]++;
    h->counts[0] = 0;
    i32 acc = 0;
    for (u32 i = 0; i <= h->maxLen; i++) {
        base[i] = acc;
        acc += h->counts[i];
    }
    for (u32 i = 0; i < h->symCount; i++) {
        i32 len = h->lengths[i];
        if (len) h->symbols[base[len]++] = i;
    }
}

u32 huffmanDecode(ByteBuffer *idat, ByteBuffer *b, Huffman *h)
{
    i32 r;
    u32 code = 0, base = 0, first = 0;
    for (u32 len = 1; len <= h->maxLen; len++) {
        first = (first + h->counts[len - 1]) << 1;
        code |= idatGetBits(idat, 1, b, &r);
        i32 count = h->counts[len];
        if (count) {
            i32 off = code - first;
            if (off >= 0 && off < count)
                return h->symbols[base + off];
            base += h->counts[len];
        }
        code <<= 1;
    }
    return -1;
}

static u8 *readEntireFile(const char *fileName, size_t *size)
{
    *size = 0;
    u8 * bf = nullptr;
    FILE *fp = fopen(fileName, "rb");
    if (!fp) { return nullptr; }
    fseek(fp, 0, SEEK_END);
    i32 sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz == -1) {
        fclose(fp);
        return nullptr;
    }
    bf = (u8*)calloc(sz + 1, 1);
    if (!bf) {
        fclose(fp);
        return nullptr;
    }
    fread(bf, 1, sz, fp);
    *size = sz;
    return bf;
}
