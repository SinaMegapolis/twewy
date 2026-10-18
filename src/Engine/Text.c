#include "Engine/Text.h"
#include "Engine/Core/OamMgr.h"

typedef struct {
    u16 unk_00;
    u16 unk_02;
} UnkTextPair;
typedef struct {
    u16 half0;
    u16 half1;
    u16 half2;
    u16 half3;
} UnkQuad;
#include "Engine/File/BinMgr.h"

typedef struct {
    /* 0x00 */ BinIdentifier* bin;
    /* 0x04 */ s32            packIndex;
    /* 0x08 */ s32            loadMode;
    /* 0x0C */ TextU16Pair    glyphSize;
    /* 0x10 */ TextU16Pair    dimensions;
    /* 0x14 */ s32            dataOffset;
    /* 0x18 */ s32            paletteOffset;
    /* 0x1C */ s32            mappingOffset;
    /* 0x20 */ u16            flags;
    /* 0x22 */ u16            padding;
} TextFontEntry;

typedef struct {
    /* 0x00 */ s32         screen;
    /* 0x04 */ s32         pack;
    /* 0x08 */ s32         mode;
    /* 0x0C */ TextU16Pair tileOrigin;
    /* 0x10 */ TextU16Pair tileSize;
    /* 0x14 */ TextU16Pair spacing;
    /* 0x18 */ s32         tilemap;
    /* 0x1C */ s32         pixels;
    /* 0x20 */ s32         buffer;
    /* 0x24 */ s32         fonts;
    /* 0x28 */ s32         fontIndex;
    /* 0x2C */ u16         controls[16];
} TextInitArgs;

s32 func_020107a4(TextObject* obj, s32 param_2, s32 param_3);

void func_02010870(TextObject* obj);

void  func_0200fa28(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fa2c(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fb88(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fad4(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fc70(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
s32   func_02010808(TextObject* arg0, s32 arg1, s32 arg2);
void  func_020105b8(TextObject* obj);
void  func_02010adc(TextObject* obj, u16* arg1);
void  func_020105d4(TextObject* obj, u32 arg1, u32 arg2);
void  func_02011bec(TextObject* obj);
BOOL  func_02011c04(TextObject* obj);
void  func_02011c1c(TextObject* obj, u16 arg1);
void  func_02011494(TextObject* obj);
void* func_02011d8c(TextObject* obj, u8* arg1);
void  func_02011e94(TextObject* obj, s32 arg1, s32 arg2, u8* arg3);
void  func_02011e58(TextObject* obj, u16* arg1);
void  func_02011ecc(TextObject* obj);
void  func_02011df8(TextObject* obj);
u16   func_02011d4c(TextObject* obj);
s32   func_02011c48(TextObject* obj);
void  func_020119b0(TextObject* obj, s32 arg1, s32 arg2);
u16   func_020111f0(TextObject* obj);
s32   func_0201122c(TextObject*, s32);
s32   func_02011358(TextObject*, s32, s32);
void  func_02011474(TextObject*, u32, s32);
void  func_020119b0(TextObject* obj, s32 arg1, s32 arg2);
void* func_02011b7c(TextObject* obj, s32 arg1, s32 arg2);

typedef void (*FuncPtr)(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
const FuncPtr data_0205aea4[20] = {
    func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa2c, func_0200fb88, func_0200fa28,
    func_0200fa28, func_0200fad4, func_0200fc70, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28,
    func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28,
};

const s32 data_0205ae64[0x10] = {0xF};
typedef struct {
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    s32 shape;
    s32 size;
} TextOamRegion;
typedef struct {
    s32 bytes;
    s32 tiles;
} TextOamAllocation;
const TextOamRegion data_0205b454[4][4][2] = {
    {
     {
     {0, 0, 1, 1, 0, 0},
     {-1, -1, 0, 0, 0, 0},
     }, {
 {0, 0, 2, 1, 1, 0},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 4, 1, 1, 1},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 4, 1, 1, 1},
 {4, 0, 4, 1, 1, 1},
 }, },
    {
     {
     {0, 0, 1, 2, 2, 0},
     {-1, -1, 0, 0, 0, 0},
     }, {
 {0, 0, 2, 2, 0, 1},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 4, 2, 1, 2},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 4, 2, 1, 2},
 {4, 0, 4, 2, 1, 2},
 }, },
    {
     {
     {0, 0, 1, 4, 2, 1},
     {-1, -1, 0, 0, 0, 0},
     }, {
 {0, 0, 2, 4, 2, 2},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 4, 4, 0, 2},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 8, 4, 1, 3},
 {-1, -1, 0, 0, 0, 0},
 }, },
    {
     {
     {0, 0, 1, 4, 2, 1},
     {0, 4, 1, 4, 2, 1},
     }, {
 {0, 0, 2, 4, 2, 2},
 {0, 4, 2, 4, 2, 2},
 }, {
 {0, 0, 4, 8, 2, 3},
 {-1, -1, 0, 0, 0, 0},
 }, {
 {0, 0, 8, 8, 0, 3},
 {-1, -1, 0, 0, 0, 0},
 }, },
};
extern const u32 data_0205ae24[8][2];
const u32        data_0205b0f4[9][8][3] = {
    {
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000},
     },
    {
     {0xFFFFFFF0, 0xFFFFFFFF, 0xFFFFFFF0},
     {0xFFFFFF0F, 0xFFFFFFFF, 0xFFFFFF00},
     {0xFFFFF0FF, 0xFFFFFFFF, 0xFFFFF000},
     {0xFFFF0FFF, 0xFFFFFFFF, 0xFFFF0000},
     {0xFFF0FFFF, 0xFFFFFFFF, 0xFFF00000},
     {0xFF0FFFFF, 0xFFFFFFFF, 0xFF000000},
     {0xF0FFFFFF, 0xFFFFFFFF, 0xF0000000},
     {0x0FFFFFFF, 0xFFFFFFFF, 0x00000000},
     },
    {
     {0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFF00},
     {0xFFFFF00F, 0xFFFFFFFF, 0xFFFFF000},
     {0xFFFF00FF, 0xFFFFFFFF, 0xFFFF0000},
     {0xFFF00FFF, 0xFFFFFFFF, 0xFFF00000},
     {0xFF00FFFF, 0xFFFFFFFF, 0xFF000000},
     {0xF00FFFFF, 0xFFFFFFFF, 0xF0000000},
     {0x00FFFFFF, 0xFFFFFFFF, 0x00000000},
     {0x0FFFFFFF, 0xFFFFFFF0, 0x00000000},
     },
    {
     {0xFFFFF000, 0xFFFFFFFF, 0xFFFFF000},
     {0xFFFF000F, 0xFFFFFFFF, 0xFFFF0000},
     {0xFFF000FF, 0xFFFFFFFF, 0xFFF00000},
     {0xFF000FFF, 0xFFFFFFFF, 0xFF000000},
     {0xF000FFFF, 0xFFFFFFFF, 0xF0000000},
     {0x000FFFFF, 0xFFFFFFFF, 0x00000000},
     {0x00FFFFFF, 0xFFFFFFF0, 0x00000000},
     {0x0FFFFFFF, 0xFFFFFF00, 0x00000000},
     },
    {
     {0xFFFF0000, 0xFFFFFFFF, 0xFFFF0000},
     {0xFFF0000F, 0xFFFFFFFF, 0xFFF00000},
     {0xFF0000FF, 0xFFFFFFFF, 0xFF000000},
     {0xF0000FFF, 0xFFFFFFFF, 0xF0000000},
     {0x0000FFFF, 0xFFFFFFFF, 0x00000000},
     {0x000FFFFF, 0xFFFFFFF0, 0x00000000},
     {0x00FFFFFF, 0xFFFFFF00, 0x00000000},
     {0x0FFFFFFF, 0xFFFFF000, 0x00000000},
     },
    {
     {0xFFF00000, 0xFFFFFFFF, 0xFFF00000},
     {0xFF00000F, 0xFFFFFFFF, 0xFF000000},
     {0xF00000FF, 0xFFFFFFFF, 0xF0000000},
     {0x00000FFF, 0xFFFFFFFF, 0x00000000},
     {0x0000FFFF, 0xFFFFFFF0, 0x00000000},
     {0x000FFFFF, 0xFFFFFF00, 0x00000000},
     {0x00FFFFFF, 0xFFFFF000, 0x00000000},
     {0x0FFFFFFF, 0xFFFF0000, 0x00000000},
     },
    {
     {0xFF000000, 0xFFFFFFFF, 0xFF000000},
     {0xF000000F, 0xFFFFFFFF, 0xF0000000},
     {0x000000FF, 0xFFFFFFFF, 0x00000000},
     {0x00000FFF, 0xFFFFFFF0, 0x00000000},
     {0x0000FFFF, 0xFFFFFF00, 0x00000000},
     {0x000FFFFF, 0xFFFFF000, 0x00000000},
     {0x00FFFFFF, 0xFFFF0000, 0x00000000},
     {0x0FFFFFFF, 0xFFF00000, 0x00000000},
     },
    {
     {0xF0000000, 0xFFFFFFFF, 0xF0000000},
     {0x0000000F, 0xFFFFFFFF, 0x00000000},
     {0x000000FF, 0xFFFFFFF0, 0x00000000},
     {0x00000FFF, 0xFFFFFF00, 0x00000000},
     {0x0000FFFF, 0xFFFFF000, 0x00000000},
     {0x000FFFFF, 0xFFFF0000, 0x00000000},
     {0x00FFFFFF, 0xFFF00000, 0x00000000},
     {0x0FFFFFFF, 0xFF000000, 0x00000000},
     },
    {
     {0x00000000, 0xFFFFFFFF, 0x00000000},
     {0x0000000F, 0xFFFFFFF0, 0x00000000},
     {0x000000FF, 0xFFFFFF00, 0x00000000},
     {0x00000FFF, 0xFFFFF000, 0x00000000},
     {0x0000FFFF, 0xFFFF0000, 0x00000000},
     {0x000FFFFF, 0xFFF00000, 0x00000000},
     {0x00FFFFFF, 0xFF000000, 0x00000000},
     {0x0FFFFFFF, 0xF0000000, 0x00000000},
     },
};

const u16 data_0205aef4[256] = {
    0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xA,    0xB,    0xC,    0xD,    0xE,
    0xF,    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,
    0x1E,   0x1F,   0x8140, 0x8149, 0x8160, 0x818C, 0x8190, 0x818B, 0x8195, 0x818C, 0x8169, 0x816A, 0x8196, 0x817B, 0x8143,
    0x817C, 0x8144, 0x815E, 0x824F, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256, 0x8257, 0x8258, 0x8146, 0x8147,
    0x8183, 0x8181, 0x8184, 0x8148, 0x8197, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266, 0x8267, 0x8268, 0x8269,
    0x826A, 0x826B, 0x826C, 0x826D, 0x826E, 0x826F, 0x8270, 0x8271, 0x8272, 0x8273, 0x8274, 0x8275, 0x8276, 0x8277, 0x8278,
    0x8279, 0x816D, 0x818F, 0x816E, 0x814F, 0x8151, 0x824D, 0x8281, 0x8282, 0x8283, 0x8284, 0x8285, 0x8286, 0x8287, 0x8288,
    0x8289, 0x828A, 0x828B, 0x828C, 0x828D, 0x828E, 0x828F, 0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296, 0x8297,
    0x8298, 0x8299, 0x829A, 0x8167, 0x8162, 0x8168, 0x8164, 0x8140, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159,
};

u16 func_0200f94c(s32 idx) {
    return data_0205aef4[idx];
}

u16 func_0200f960(u32 arg0, u8* arg1) {
    u16 offset      = ((arg0 & 0x7F00) >> 6);
    u16 base_offset = arg1[offset] | (arg1[offset + 1] << 8);
    u16 width       = (arg1[offset + 3] << 8) | arg1[offset + 2];
    u16 result      = 0;

    if (base_offset != 0xFFFF) {
        u8 char_width = arg1[(u16)(base_offset + ((arg0 & 0xFF) - 0x40))];
        if (char_width != 0xFF) {
            result = char_width + width;
        }
    }

    return result;
}

u16 func_0200f9e0(s32 arg0, u8* arg1, u8* arg2, u32 arg3, u16 arg4) {
    u16 var_r4 = 0;

    if (arg2 != 0) {
        u32 idx = func_0200f960(arg0, arg1);
        if (idx < arg3) {
            var_r4 = arg2[idx];
        }
    }
    if (var_r4 == 0) {
        var_r4 = arg4;
    }
    return var_r4;
}

u32 func_0200fa10(u32 param_1, u8* param_2, u32 param_3, u32 param_4) {
    return param_4 * func_0200f960(param_1, param_2) + param_3;
}

void func_0200fa28(u32*, u8*, u32*, s32, s32, u32, u32*, u32*) {
    return;
}

void func_0200fa2c(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
                   u32* shifts) {
    u32 pixels;
    s32 row;
    s32 packed;
    s32 bit;
    u32 sourceOffset;

    width *= 4;
    row = 0;
    if (height <= 0) {
        return;
    }
    sourceOffset = 0;
    do {
        pixels = 0;
        bit    = 0;
        packed = source[sourceOffset];
        if (width > 0) {
            do {
                u32 decodedPixel = palette[packed & 1];
                packed           = packed >> 1;
                pixels |= decodedPixel << bit;
                bit += 4;
            } while (bit < width);
        }
        destination[row] &= masks[0];
        sourceOffset += sourceStride;
        destination[row] |= pixels << shifts[0];
        u32 rightPixels      = destination[row + 8];
        u32 rightMasked      = rightPixels & masks[1];
        destination[row + 8] = rightMasked;
        destination[row + 8] = rightMasked | (pixels >> shifts[1]);
        row += 1;
    } while (row < height);
}

void func_0200fad4(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
                   u32* shifts) {
    u32 pixels;
    s32 row;
    s32 packed;
    s32 bit;
    u32 sourceOffset;

    width *= 4;
    row = 0;
    if (height <= 0) {
        return;
    }
    sourceOffset = 0;
    do {
        pixels = 0;
        bit    = 0;
        packed = (source[sourceOffset + 1] << 8) | source[sourceOffset];
        if (width > 0) {
            do {
                u32 decodedPixel = palette[packed & 3];
                packed           = packed >> 2;
                pixels |= decodedPixel << bit;
                bit += 4;
            } while (bit < width);
        }
        destination[row] &= masks[0];
        sourceOffset += sourceStride;
        destination[row] |= pixels << shifts[0];
        u32 rightPixels      = destination[row + 8];
        u32 rightMasked      = rightPixels & masks[1];
        destination[row + 8] = rightMasked;
        destination[row + 8] = rightMasked | (pixels >> shifts[1]);
        row += 1;
    } while (row < height);
}

void func_0200fb88(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
                   u32* shifts) {
    u32 pixels;
    u32 preserveMask;
    s32 packed;
    u32 sourceOffset;
    s32 row;
    s32 bit;
    width *= 4;
    row = 0;
    if (height <= 0)
        return;
    sourceOffset = 0;
    do {
        pixels       = 0;
        preserveMask = 0;
        bit          = 0;
        packed       = source[sourceOffset];
        if (width > 0) {
            do {
                s32 paletteIndex = packed & 1;
                pixels |= palette[paletteIndex] << bit;
                preserveMask |= data_0205ae64[paletteIndex] << bit;
                packed >>= 1;
                bit += 4;
            } while (bit < width);
        }
        destination[row] &= masks[0] | (preserveMask << shifts[0]);
        sourceOffset += sourceStride;
        destination[row] |= pixels << shifts[0];
        u32 rightPixels      = destination[row + 8];
        u32 rightMasked      = rightPixels & (masks[1] | (preserveMask >> shifts[1]));
        destination[row + 8] = rightMasked;
        destination[row + 8] = rightMasked | (pixels >> shifts[1]);
        row++;
    } while (row < height);
}

void func_0200fc70(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
                   u32* shifts) {
    u32 pixels;
    u32 preserveMask;
    s32 packed;
    s32 row;
    s32 bit;
    u32 sourceOffset;
    width *= 4;
    row = 0;
    if (height <= 0)
        return;
    sourceOffset = 0;
    do {
        pixels       = 0;
        preserveMask = 0;
        bit          = 0;
        packed       = (source[sourceOffset + 1] << 8) | source[sourceOffset];
        if (width > 0) {
            do {
                s32 paletteIndex = packed & 3;
                pixels |= palette[paletteIndex] << bit;
                preserveMask |= data_0205ae64[paletteIndex] << bit;
                packed >>= 2;
                bit += 4;
            } while (bit < width);
        }
        destination[row] &= masks[0] | (preserveMask << shifts[0]);
        sourceOffset += sourceStride;
        destination[row] |= pixels << shifts[0];
        u32 rightPixels      = destination[row + 8];
        u32 rightMasked      = rightPixels & (masks[1] | (preserveMask >> shifts[1]));
        destination[row + 8] = rightMasked;
        destination[row + 8] = rightMasked | (pixels >> shifts[1]);
        row++;
    } while (row < height);
}

s32 func_0200fd64(UnkMedText* glyph) {
    s32 width;
    s32 height;
    s32 pixelOffset;

    width = glyph->unk_10;
    if (width > 8) {
        width = 8;
    }
    height      = (s32)glyph->unk_14 > 8 ? 8 : (s32)glyph->unk_14;
    pixelOffset = glyph->unk_0C;

    data_0205aea4[(glyph->unk_02 << 2) + glyph->unk_04](glyph->unk_1C, glyph->unk_18, glyph->unk_20, width, height,
                                                        glyph->unk_08, data_0205b0f4[width][pixelOffset],
                                                        data_0205ae24[pixelOffset]);

    return ((pixelOffset + width + 7) & ~7) >> 3;
}

s32 func_0200fe08(TextObject* obj, s32 xOffset, s32 yOffset, s32 glyphSource, s32 width, s32 height) {
    void* tileCache;
    s32   columnsDrawn;
    u16*  currentTile;
    u16*  nextTile;
    void* nextPixels;

    void* currentPixels;
    s32   clippedX  = xOffset;
    s32   clippedY  = yOffset;
    s32   glyphData = glyphSource;
    s32   rowsDrawn;
    s32   tileSpan;
    s32   firstTile;
    u16   clipWidth = obj->unk_20;
    if ((s32)(xOffset + width) > (s32)clipWidth) {
        width = clipWidth - xOffset;
    }
    u16 clipHeight = obj->unk_22;
    if ((s32)(clippedY + height) > (s32)clipHeight) {
        height = (s32)clipHeight - clippedY;
    }
    if (width <= 0 || height <= 0) {
        return 0;
    }
    {
        firstTile             = (s32)(u16)(obj->unk_36 + (((clippedY >> 3) * obj->unk_18[0]) + (clippedX >> 3)));
        s32 rowOffset         = clippedY & 7;
        tileCache             = (u8*)obj + 0x310;
        obj->primaryRowPixels = (void*)((u8*)tileCache + (rowOffset * 4));
        obj->unk_78           = clippedX & 7;
        obj->unk_80           = 8 - rowOffset;
        rowsDrawn             = 0;
        if (height > 0) {
            do {
                currentTile   = (u16*)func_020107a4(obj, clippedX, clippedY + rowsDrawn);
                currentPixels = (void*)func_02010808(obj, clippedX, clippedY + rowsDrawn);
                if (currentTile == NULL && currentPixels == NULL) {
                    goto block_14;
                }
                if (currentTile != NULL && *currentTile == obj->unk_34) {
                block_14:
                    MI_CpuFill(obj->unk_30, tileCache, 0x20);
                } else {
                    MI_CpuCopy(currentPixels, tileCache, 0x20);
                }
                columnsDrawn = 0;
                obj->unk_84  = (rowsDrawn * obj->unk_74) + glyphData;
                obj->unk_7C  = width;
                if (obj->unk_7C > 0) {
                    s32 tileColumn = 0;
                    do {
                        if (currentTile != NULL) {
                            nextTile = currentTile + 1;
                        } else {
                            nextTile = (u16*)func_020107a4(obj, clippedX + columnsDrawn + 8, clippedY + rowsDrawn);
                        }
                        if (currentPixels != NULL) {
                            nextPixels = (void*)((u8*)currentPixels + 0x20);
                        } else {
                            nextPixels = (void*)func_02010808(obj, clippedX + columnsDrawn + 8, clippedY + rowsDrawn);
                        }
                        if (nextTile == NULL && nextPixels == NULL) {
                            goto block_29;
                        }
                        if (nextTile != NULL && *nextTile == obj->unk_34) {
                        block_29:
                            MI_CpuFill(obj->unk_30, (u8*)tileCache + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(nextPixels, (u8*)tileCache + 0x20, 0x20);
                        }
                        tileSpan = func_0200fd64((UnkMedText*)((u8*)obj + 0x6C));
                        if (currentTile != NULL) {
                            *currentTile = (u16)(firstTile + tileColumn);
                        }
                        if (currentPixels != NULL) {
                            MI_CpuCopy(tileCache, currentPixels, 0x20);
                        }
                        if (tileSpan > 1) {
                            if (nextTile != NULL) {
                                *nextTile = (u16)(firstTile + tileColumn + 1);
                            }
                            if (nextPixels != NULL) {
                                MI_CpuCopy((u8*)tileCache + 0x20, nextPixels, 0x20);
                            }
                        }
                        currentTile   = nextTile;
                        currentPixels = nextPixels;
                        MI_CpuCopy((u8*)tileCache + 0x20, tileCache, 0x20);
                        tileColumn += 1;
                        obj->unk_84 = obj->unk_84 + obj->unk_6E;
                        columnsDrawn += 8;
                        obj->unk_7C = obj->unk_7C - 8;
                    } while (obj->unk_7C > 0);
                }
                firstTile             = (s32)(u16)(firstTile + obj->unk_18[0]);
                obj->primaryRowPixels = tileCache;
                rowsDrawn += obj->unk_80;
                s32 remainingRows = height - rowsDrawn;
                obj->unk_80       = remainingRows;
                if (remainingRows > 8) {
                    obj->unk_80 = 8;
                }
            } while (rowsDrawn < height);
        }
        return (s32)(((clippedX + (s32)width + 6) - (clippedX & ~7)) & ~7) >> 3;
    }
    return 0;
}

u16 func_02010170(TextObject* obj) {
    if (obj->unk_64 != obj->unk_66) {
        u16 val = obj->unk_68[obj->unk_66];
        obj->unk_66++;
        obj->unk_66 = obj->unk_66 & 0xff;
        return val;
    }
    return 0;
}

BOOL func_020101ac(TextObject* obj, u32 arg1) {
    u32 iVar1 = func_0200fa10(arg1, obj->unk_50, obj->unk_54, obj->unk_46);
    u32 uVar1 = func_0200f9e0(arg1, obj->unk_50, obj->unk_58, obj->unk_5C, obj->unk_60);

    func_0200fe08(obj, obj->xOffset, obj->yOffset, iVar1, uVar1, obj->unk_4E);
    if (obj->unk_00 == 0) {
        obj->xOffset += uVar1;
    } else {
        obj->yOffset += obj->unk_4E;
    }
    if ((obj->unk_10[0] != 0) && (obj->unk_70 == 0)) {
        if (obj->unk_00 == 0) {
            func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_10[0], obj->unk_4E);
            obj->xOffset += obj->unk_10[0];
        } else {
            func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_4C, obj->unk_10[1]);
            obj->yOffset += obj->unk_10[1];
        }
    }
    return TRUE;
}

static inline void SetOffsets(TextObject* obj, u32 x, u32 y) {
    obj->xOffset = x;
    obj->yOffset = y;
}

BOOL func_020102d8(TextObject* obj, u32 arg1, u32 arg2) {
    BOOL BVar1 = FALSE;

    switch (arg1) {
        case 0:
            break;
        case 10:
            if (obj->unk_00 == 0) {
                SetOffsets(obj, 0, obj->yOffset + (obj->unk_4E + obj->unk_10[1]));
            } else {
                SetOffsets(obj, obj->xOffset - (obj->unk_4C + obj->unk_10[0]), 0);
            }
            break;
        case 9:
            if (obj->unk_00 == 0) {
                if (arg2 == 1) {
                    func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, 8 - (obj->xOffset & 7U), obj->unk_4E);
                }
                SetOffsets(obj, (obj->xOffset + 8) & ~7, obj->yOffset);
            } else {
                if (arg2 == 1) {
                    func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_4C, 8 - (obj->yOffset & 7U));
                }
                SetOffsets(obj, obj->xOffset, (obj->yOffset + 8) & ~7);
            }
            BVar1 = TRUE;
            break;
    }

    return BVar1;
}

void func_020103f4(TextObject* arg0, s32 arg1, s32 unused) {
    if (arg1 & 0xFF00) {
        func_020101ac(arg0, arg1);
        return;
    }
    func_020102d8(arg0, arg1, 1);
}

void func_02010414(TextObject* obj) {
    if (obj->unk_18[0] == 0 && obj->unk_18[1] == 0) {
        return;
    }

    u16 i;
    s32 iVar3 = func_020107a4(obj, 0, 0);

    if (iVar3 != 0) {
        for (i = 0; i < obj->unk_18[1]; i++) {
            MI_CpuFillU16(obj->unk_34, iVar3, obj->unk_18[0] << 1);
            iVar3 += obj->unk_04 * 2;
        }
    } else {
        MI_CpuFill(obj->unk_30, func_02010808(obj, 0, 0), obj->unk_18[0] * obj->unk_18[1] * 0x20);
    }
}

void func_020104c0(TextObject* obj, UnkSmallText* param_2) {
    obj->unk_00 = param_2->unk_0C;
    obj->unk_3C = param_2->unk_08;
    obj->data   = NULL;
    obj->unk_04 = 0x20;
    obj->unk_06 = 0x20;

    obj->unk_10 = param_2->unk_18;
    obj->unk_14 = param_2->unk_10;
    obj->unk_18 = param_2->unk_14;

    obj->unk_1C = param_2->unk_10[0] << 3;
    obj->unk_1E = param_2->unk_10[1] << 3;
    obj->unk_20 = param_2->unk_14[0] << 3;
    obj->unk_22 = param_2->unk_14[1] << 3;

    obj->unk_24 = param_2->unk_1C;
    obj->unk_28 = param_2->unk_20;
    obj->unk_34 = param_2->unk_2C;
    obj->unk_36 = param_2->unk_2E;

    func_020105d4(obj, param_2->unk_24, param_2->unk_28);
    obj->unk_38 = param_2->unk_24;
    obj->unk_2C = func_0200fa10(0x8140, obj->unk_50, obj->unk_54, obj->unk_46);
    func_02010adc(obj, &param_2->unk_30);
    func_02010870(obj);
}

void func_020105b8(TextObject* obj) {
    DatMgr_ReleaseData(obj->data);
    obj->data = NULL;
}

void func_020105d4(TextObject* obj, u32 arg1, u32 arg2) {
    obj->unk_38                  = (s32)arg1;
    void*                var_r0  = obj->data;
    const TextFontEntry* temp_r4 = (const TextFontEntry*)arg1 + arg2;
    if (var_r0 == NULL) {
        if (var_r0 != NULL) {
            DatMgr_ReleaseData((Data*)var_r0);
        }
        s32 var_r6 = 0;
        s32 temp_r7;
        if (*(s32*)((u8*)temp_r4 + 8) == 1) {
            var_r6 = 1;
        }
        s32 temp_r8 = *(s32*)temp_r4;
        s32 temp_r9 = *(s32*)((u8*)temp_r4 + 4);
        temp_r7     = obj->unk_3C;
        if (BinMgr_FindById(temp_r8) == NULL) {
            var_r0 = DatMgr_LoadPackEntry(temp_r7, NULL, 0, (BinIdentifier*)temp_r8, temp_r9, (BOOL)var_r6);
        } else {
            var_r0 = DatMgr_LoadPackEntryDirect(temp_r7, (BinIdentifier*)temp_r8, temp_r9, var_r6);
        }
    }
    obj->data     = (Data*)var_r0;
    obj->unk_44   = (u16) * (u16*)((u8*)temp_r4 + 12);
    obj->unk_46   = (u16) * (u16*)((u8*)temp_r4 + 14);
    obj->fontSize = temp_r4->dimensions;
    s32   temp_r3 = *(s32*)((u8*)temp_r4 + 20);
    void* var_r1  = NULL;
    if (var_r0 == NULL || temp_r3 <= 0) {
        var_r1 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1        = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3 * 8);
    }
    obj->unk_50     = (u8*)var_r1;
    s32   temp_r3_2 = *(s32*)((u8*)temp_r4 + 24);
    void* var_r1_2  = NULL;
    if (var_r0 == NULL || temp_r3_2 <= 0) {
        var_r1_2 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1_2      = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3_2 * 8);
    }
    obj->unk_54     = (u32)var_r1_2;
    s32   temp_r3_3 = *(s32*)((u8*)temp_r4 + 28);
    void* var_r1_3  = NULL;
    if (var_r0 == NULL || temp_r3_3 <= 0) {
        var_r1_3 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1_3      = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3_3 * 8);
    }
    obj->unk_58 = (u8*)var_r1_3;
    obj->unk_5C = *(s32*)((u8*)((Data*)var_r0)->buffer + (*(s32*)((u8*)temp_r4 + 28) * 8) + 0x24);
    // Actually m2c: arg0->unk5C = (s32)(var_r0->buffer + (temp_r4->unk1C * 8))->unk24;
    obj->unk_60  = *(u16*)((u8*)temp_r4 + 32);
    obj->unk_48  = (((*(u16*)((u8*)temp_r4 + 16)) + 7) & ~7) >> 3;
    obj->unk_4A  = (((*(u16*)((u8*)temp_r4 + 18)) + 7) & ~7) >> 3;
    obj->unk_6C  = 0;
    obj->unk_6E  = obj->unk_44;
    obj->unk_74  = (((obj->unk_4C * obj->unk_44) + 7) & ~7) >> 3;
    obj->unk_8C  = (void*)((u8*)obj + 0x90);
    obj->unk_70  = 0;
    s32 temp_y   = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset = *(s32*)((u8*)obj + 0x08);
    obj->yOffset = temp_y;
}

s32 func_020107a4(TextObject* obj, s32 x, s32 y) {
    if (x < 0 || x >= obj->unk_20 || y < 0 || y >= obj->unk_22)
        return 0;
    s32 row     = (y + obj->unk_1E) >> 3;
    s32 address = obj->unk_28 + (row * obj->unk_04) * 2;
    x += obj->unk_1C;
    return address + (x >> 3) * 2;
}

s32 func_02010808(TextObject* arg0, s32 arg1, s32 arg2) {
    register u32 var_lr = 0;
    s32          var_ip = var_lr - 1;

    if ((arg1 >= 0) && (arg1 < arg0->unk_20) && (arg2 >= 0) && (arg2 < arg0->unk_22)) {
        s32 var_r3 = (arg2 >> 3) * arg0->unk_18[0];
        u32 var_r2 = (arg2 << 0x1D);
        var_r3 += arg1 >> 3;
        var_r3 <<= 5;
        var_r2 >>= 0x1B;
        var_ip = var_r3 + var_r2;
    }

    if (var_ip >= 0) {
        var_lr = arg0->unk_24 + (var_ip & ~0x1F);
    }

    return var_lr;
}

void func_02010870(TextObject* obj) {
    obj->unk_68 = &obj->unk_110;
    obj->unk_64 = 0;
    obj->unk_66 = 0;
}

BOOL func_02010888(TextObject* obj) {
    return obj->unk_64 == obj->unk_66;
}

void func_020108a0(TextObject* obj, u16 param_2) {
    obj->unk_68[obj->unk_64] = param_2;
    obj->unk_64++;
    obj->unk_64 &= 0xff;
}

s32 func_020108cc(TextObject* obj) {
    u16 value;
    s32 command;
    s32 character;
    for (;;) {
        character = func_02010170(obj);
        if (character != 0x40) {
            return character;
        }
        command = func_02010170(obj);
        value   = func_02010170(obj);
        if (value >= 0x30U && value <= 0x39U) {
            value -= 0x30;
        } else if (value >= 0x61U && value <= 0x66U) {
            value -= 0x57;
        } else if (value >= 0x41U && value <= 0x46U) {
            value -= 0x37;
        } else {
            value = 0x10;
        }
        switch (command) {
            case 0x46:
                func_020105d4(obj, obj->unk_38, value);
                break;
            case 0x62:
                obj->unk_90 = value;
                break;
            case 0x66:
                obj->unk_94 = value;
                break;
            case 0x61:
                obj->unk_98 = value;
                break;
            case 0x65:
                obj->unk_9C = value;
                break;
        }
    }
}

s32 func_020109d0(TextObject* arg0) {
    u16 temp_r4;
    u16 temp_r5;

    temp_r4 = arg0->unk_64;
    temp_r5 = arg0->unk_66;
loop_1:
    s32 val = func_02010170(arg0);
    if (val == 0x40) {
        func_02010170(arg0);
        func_02010170(arg0);
        goto loop_1;
    }
    arg0->unk_64 = temp_r4;
    arg0->unk_66 = temp_r5;
    return val;
}

void* func_02010a10(TextObject* arg0, u8* arg1) {
    u8  var_r2;
    u8* var_r4;

    var_r4 = arg1;
    var_r2 = *var_r4;
    if (var_r2 != 0) {
        do {
            if (var_r2 & 0x80) {
                func_020108a0(arg0, (u16)(var_r4[1] | (var_r2 << 8)));
                var_r4 += 2;
            } else {
                func_020108a0(arg0, func_0200f94c(var_r2));
                var_r4 += 1;
            }
            var_r2 = *var_r4;
        } while (var_r2 != 0);
    }
    return var_r4 + 1;
}

void func_02010a7c(TextObject* arg0) {
    while (func_02010888(arg0) == FALSE) {
        s32 charCode = func_020108cc(arg0);
        s32 result   = func_020109d0(arg0);
        s32 val      = 0;

        if (result == 0 || result == 0xA) {
            val = 1;
        }

        func_020103f4(arg0, charCode, val);
    }
}

void func_02010adc(TextObject* obj, u16* arg1) {
    s32 var_ip = 0;
    do {
        u16 temp_r3                = arg1[var_ip];
        ((u32*)obj)[0x24 + var_ip] = temp_r3;
        ((u32*)obj)[0x34 + var_ip] = temp_r3;
        var_ip += 1;
    } while (var_ip < 0x10);
    u32 temp_r1 = *(u32*)((u8*)obj + 0x90);
    temp_r1 |= temp_r1 * 0x10;
    temp_r1 |= temp_r1 << 8;
    *(u32*)((u8*)obj + 0x30) = temp_r1 | (temp_r1 << 0x10);
}

void Text_RenderToScreen(TextObject* obj, s32 xOffset, s32 yOffset, const char* string) {
    func_02010a7c(obj);
    obj->xOffset = xOffset;
    obj->yOffset = yOffset;
    func_02010a10(obj, string);
    func_02010a7c(obj);
}

void func_02010b50(TextObject* obj) {
    s32 sVar1;

    func_02010414(obj);
    if (obj->unk_00 == 0) {
        sVar1 = 0;
    } else {
        sVar1 = obj->unk_20 - obj->unk_4C;
    }
    obj->xOffset = sVar1;
    obj->yOffset = 0;
}

// Nonmatching: pixelX and the strength-reduced (pixelX >> 3) tile counter swap registers (r5/r9)
void func_02010b84(TextObject* obj, s32 xOffset, s32 yOffset, s32 width, s32 height) {
    s32   clippedX = xOffset;
    s32   clippedY = yOffset;
    u16*  savedControls;
    s32   pixelLeftRemainder;
    s32   pixelLeftWidth;
    s32   pixelRightRemainder;
    s32   pixelLeftTileX;
    s32   mapLeftRemainder;
    s32   right;
    s32   bottom;
    s32   alignedBottom;
    s32   clippedWidth;
    u16   clipWidth;
    u16   clipHeight;
    s32   topRemainder;
    s32   topLeftRemainder;
    s32   topX;
    s32   topRightTileX;
    s32   topRightRemainder;
    void* pixels;
    s32   pixelX;
    s32   rightTileX;
    s32   mapLeftWidth;
    s32   mapRightRemainder;
    s32   mapLeftTileX;
    u16*  tileMap;
    s32   mapX;
    s32   leftAligned;
    s32   rowY;
    s32   bottomRemainder;
    s32   bottomLeftRemainder;
    s32   bottomRightRemainder;
    clippedWidth = width;
    if (xOffset < 0) {
        clippedWidth += xOffset;
        clippedX = 0;
    }
    if (clippedY < 0) {
        height += clippedY;
        clippedY = 0;
    }
    clipWidth = obj->unk_20;
    if ((s32)(clippedX + clippedWidth) > (s32)clipWidth) {
        clippedWidth = clipWidth - clippedX;
    }
    clipHeight = obj->unk_22;
    if ((s32)(clippedY + height) > (s32)clipHeight) {
        height = clipHeight - clippedY;
    }
    if ((clippedWidth > 0) && (height > 0)) {
        savedControls = obj->unk_70;
        obj->unk_70   = NULL;
        right         = clippedX + clippedWidth;
        obj->xOffset  = right;
        bottom        = clippedY + height;
        obj->yOffset  = bottom;
        topRemainder  = clippedY & 7;
        if (topRemainder != 0) {
            topLeftRemainder = clippedX & 7;
            if (topLeftRemainder != 0) {
                func_0200fe08(obj, clippedX, clippedY, obj->unk_2C, 8 - topLeftRemainder, 8 - topRemainder);
            }
            topX          = (clippedX + 7) & ~7;
            topRightTileX = right & ~7;
            if (topX < topRightTileX) {
                do {
                    func_0200fe08(obj, topX, clippedY, obj->unk_2C, 8, 8 - topRemainder);
                    topX += 8;
                } while (topX < topRightTileX);
            }
            topRightRemainder = right & 7;
            if ((topRightRemainder != 0) && ((s32)(clippedX & ~7) < topRightTileX)) {
                func_0200fe08(obj, topRightTileX, clippedY, obj->unk_2C, topRightRemainder, 8 - topRemainder);
            }
        }
        leftAligned   = (clippedX + 7) & ~7;
        rowY          = (clippedY + 7) & ~7;
        rightTileX    = right & ~7;
        alignedBottom = bottom & ~7;
        if (func_020107a4(obj, 0, 0) != 0) {
            if (rowY < alignedBottom) {
                mapLeftRemainder  = clippedX & 7;
                mapLeftWidth      = 8 - mapLeftRemainder;
                mapRightRemainder = right & 7;
                mapLeftTileX      = clippedX & ~7;
                do {
                    if (mapLeftRemainder != 0) {
                        func_0200fe08(obj, clippedX, rowY, obj->unk_2C, mapLeftWidth, 8);
                    }
                    tileMap = (u16*)func_020107a4(obj, 0, rowY);
                    mapX    = leftAligned;
                    if (leftAligned < rightTileX) {
                        do {
                            tileMap[mapX >> 3] = obj->unk_34;
                            mapX += 8;
                        } while (mapX < rightTileX);
                    }
                    if ((mapRightRemainder != 0) && (mapLeftTileX < rightTileX)) {
                        func_0200fe08(obj, rightTileX, rowY, obj->unk_2C, mapRightRemainder, 8);
                    }
                    rowY += 8;
                } while (rowY < alignedBottom);
            }
        } else if (rowY < alignedBottom) {
            pixelLeftRemainder  = clippedX & 7;
            pixelLeftWidth      = 8 - pixelLeftRemainder;
            pixelRightRemainder = right & 7;
            pixelLeftTileX      = clippedX & ~7;
            do {
                if (pixelLeftRemainder != 0) {
                    func_0200fe08(obj, leftAligned, rowY, obj->unk_2C, pixelLeftWidth, 8);
                }
                pixels = (void*)func_02010808(obj, leftAligned, rowY);
                pixelX = leftAligned;
                if (leftAligned < rightTileX) {
                    do {
                        MI_CpuFill(obj->unk_30, (u8*)pixels + ((pixelX >> 3) << 5), 0x20);
                        pixelX += 8;
                    } while (pixelX < rightTileX);
                }
                if ((pixelRightRemainder != 0) && (pixelLeftTileX < rightTileX)) {
                    func_0200fe08(obj, rightTileX, rowY, obj->unk_2C, pixelRightRemainder, 8);
                }
                rowY += 8;
            } while (rowY < alignedBottom);
        }
        bottomRemainder = bottom & 7;
        if ((bottomRemainder != 0) && ((s32)(clippedY & ~7) < alignedBottom)) {
            bottomLeftRemainder = clippedX & 7;
            if (bottomLeftRemainder != 0) {
                func_0200fe08(obj, clippedX, alignedBottom, obj->unk_2C, 8 - bottomLeftRemainder, bottomRemainder);
            }
            if (leftAligned < rightTileX) {
                do {
                    func_0200fe08(obj, leftAligned, alignedBottom, obj->unk_2C, 8, bottomRemainder);
                    leftAligned += 8;
                } while (leftAligned < rightTileX);
            }
            bottomRightRemainder = right & 7;
            if ((bottomRightRemainder != 0) && ((s32)(clippedX & ~7) < rightTileX)) {
                func_0200fe08(obj, rightTileX, alignedBottom, obj->unk_2C, bottomRightRemainder, bottomRemainder);
            }
        }
        obj->unk_70 = savedControls;
    }
}

s32 func_02010fd8(TextObject* obj, s32 xOffset, s32 yOffset, s32 glyphSource, s32 width, s32 height) {
    void* tileCache;
    s32   rowsDrawn;
    s32   columnsDrawn;
    s32   tileSpan;
    void* currentPixels;

    void* nextPixels;
    s32   clippedY   = yOffset;
    s32   glyphData  = glyphSource;
    u16   clipWidth  = obj->unk_20;
    u16   clipHeight = obj->unk_22;
    if ((s32)(xOffset + width) > (s32)clipWidth) {
        width = clipWidth - xOffset;
    }
    if ((s32)(clippedY + height) > (s32)clipHeight) {
        height = clipHeight - clippedY;
    }
    if (width <= 0 || height <= 0) {
        return 0;
    }
    {
        tileCache               = (u8*)obj + 0x318;
        s32 rowOffset           = clippedY & 7;
        obj->secondaryRowPixels = (void*)((u8*)tileCache + (rowOffset * 4));
        obj->unk_80             = xOffset & 7;
        obj->unk_88             = 8 - rowOffset;
        rowsDrawn               = 0;
        if (height > 0) {
            do {
                void* firstPixels = func_02011b7c(obj, xOffset, clippedY + rowsDrawn);
                currentPixels     = firstPixels;
                if (currentPixels == NULL) {
                    MI_CpuFill(*(void**)((u8*)obj + 0x34), tileCache, 0x20);
                } else {
                    MI_CpuCopy(firstPixels, tileCache, 0x20);
                }
                columnsDrawn = 0;
                obj->unk_8C  = (rowsDrawn * obj->unk_7C) + glyphData;
                obj->unk_84  = width;
                if (obj->unk_84 > 0) {
                    do {
                        nextPixels = func_02011b7c(obj, xOffset + columnsDrawn + 8, clippedY + rowsDrawn);
                        if (nextPixels == NULL) {
                            MI_CpuFill(*(void**)((u8*)obj + 0x34), (u8*)tileCache + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(nextPixels, (u8*)tileCache + 0x20, 0x20);
                        }
                        tileSpan = func_0200fd64((UnkMedText*)((u8*)obj + 0x74));
                        if (currentPixels != NULL) {
                            MI_CpuCopy(tileCache, currentPixels, 0x20);
                        }
                        if ((tileSpan > 1) && (nextPixels != NULL)) {
                            MI_CpuCopy((u8*)tileCache + 0x20, nextPixels, 0x20);
                        }
                        currentPixels = nextPixels;
                        MI_CpuCopy((u8*)tileCache + 0x20, tileCache, 0x20);
                        columnsDrawn += 8;
                        obj->unk_8C = obj->unk_8C + *(u16*)((u8*)obj + 0x76);
                        obj->unk_84 = obj->unk_84 - 8;
                    } while (obj->unk_84 > 0);
                }
                obj->secondaryRowPixels = tileCache;
                rowsDrawn += obj->unk_88;
                s32 remainingRows = height - rowsDrawn;
                obj->unk_88       = remainingRows;
                if (remainingRows > 8) {
                    obj->unk_88 = 8;
                }
            } while (rowsDrawn < height);
        }
        return (s32)(((xOffset + width + 6) - (xOffset & ~7)) & ~7) >> 3;
    }
    return 0;
}
u16 func_020111f0(TextObject* obj) {
    if (obj->unk_6C != obj->unk_6E) {
        u16 val = obj->unk_70[obj->unk_6E];
        obj->unk_6E++;
        obj->unk_6E &= 0xff;
        return val;
    }
    return 0;
}

s32 func_0201122c(TextObject* obj, s32 arg1) {
    s32 temp_r6 = func_0200fa10(arg1, obj->unk_58, obj->unk_5C, obj->unk_4E);
    s32 temp_r0 = (s32)func_0200f9e0(arg1, obj->unk_58, (s32) * (s32*)((u8*)obj + 0x60), (s32) * (s32*)((u8*)obj + 0x64),
                                     *(u16*)((u8*)obj + 0x68));
    func_02010fd8(obj, obj->xOffset, obj->yOffset, temp_r6, temp_r0, (s32) * (u16*)((u8*)obj + 0x56));
    if (*(s32*)((u8*)obj + 0x04) == 0) {
        obj->xOffset = obj->xOffset + temp_r0;
    } else {
        obj->yOffset = obj->yOffset + *(u16*)((u8*)obj + 0x56);
    }
    u16 temp_r1 = obj->unk_10[0];
    if (temp_r1 != 0 && obj->unk_78 == 0) {
        if (*(s32*)((u8*)obj + 0x04) == 0) {
            func_02010fd8(obj, obj->xOffset, obj->yOffset, obj->unk_30, (s32)temp_r1, (s32) * (u16*)((u8*)obj + 0x56));
            obj->xOffset = obj->xOffset + obj->unk_10[0];
        } else {
            func_02010fd8(obj, obj->xOffset, obj->yOffset, obj->unk_30, (s32) * (u16*)((u8*)obj + 0x54), (s32)obj->unk_10[1]);
            obj->yOffset = obj->yOffset + obj->unk_10[1];
        }
    }
    return 1;
}

void func_02011474(TextObject* arg0, u32 arg1, s32 arg2) {
    if ((arg1 & 0xff00) != 0) {
        func_0201122c(arg0, (s32)arg1);
    } else {
        func_02011358(arg0, (s32)arg1, 1);
    }
}

s32 func_02011358(TextObject* obj, s32 arg1, s32 arg2) {
    s32 var_r3 = 0;
    switch (arg1) {
        case 0:
            break;
        case 10:
            if (*(s32*)((u8*)obj + 0x04) == 0) {
                s32 nextY    = obj->yOffset + (*(u16*)((u8*)obj + 0x56) + obj->unk_10[1]);
                obj->xOffset = 0;
                obj->yOffset = nextY;
            } else {
                s32 temp_r1  = *(u16*)((u8*)obj + 0x54) + obj->unk_10[0];
                obj->xOffset = obj->xOffset - temp_r1;
                obj->yOffset = 0;
            }
            break;
        case 9:
            if (*(s32*)((u8*)obj + 0x04) == 0) {
                if (arg2 == 1) {
                    s32 temp_r1 = obj->xOffset;
                    func_02010fd8(obj, temp_r1, obj->yOffset, obj->unk_30, 8 - (temp_r1 & 7), (s32) * (u16*)((u8*)obj + 0x56));
                }
                s32 temp_y   = obj->yOffset;
                obj->xOffset = (obj->xOffset + 8) & ~7;
                obj->yOffset = temp_y;
            } else {
                if (arg2 == 1) {
                    s32 temp_r2 = obj->yOffset;
                    func_02010fd8(obj, obj->xOffset, temp_r2, obj->unk_30, (s32) * (u16*)((u8*)obj + 0x54), 8 - (temp_r2 & 7));
                }
                s32 temp_x   = *(s32*)((u8*)obj + 0x08);
                s32 temp_r1  = obj->yOffset + 8;
                obj->xOffset = temp_x;
                obj->yOffset = temp_r1 & ~7;
            }
            var_r3 = 1;
            break;
    }
    return var_r3;
}

void func_02011494(TextObject* obj) {
    if (obj->unk_18[0] == 0 && obj->unk_18[1] == 0) {
        return;
    }
    MI_CpuFill(*(s32*)((u8*)obj + 0x34), func_02011b7c(obj, 0, 0), obj->unk_24);
}

// Nonmatching: target keeps a dead per-tile counter in r11, shifting registers and two stack slots
TextOamAllocation func_020114d4(s32 screen, UnkQuad area, u16* tileMap, OamCellPiece* pieces, s32 palette) {
    TextOamAllocation    allocation;
    s32                  widthPower;
    s32                  heightPower;
    s32                  regionIndex;
    s32                  pieceIndex;
    OamCharConfig*       mapping;
    s32                  rowBase;
    s32                  remainingHeight;
    s32                  height;
    s32                  column;
    s32                  tileMapBytes;
    s32                  row;
    s32                  regionColumn;
    s32                  tile;
    s32                  tileCount;
    s32                  remainingWidth;
    const TextOamRegion* region;
    s32                  regionRow;
    s32                  width;

    width        = area.half2;
    height       = area.half3;
    tileMapBytes = (width * height * 2 + 31) & ~31;
    mapping      = g_OamMgr[screen].charBases[0];
    MI_CpuSet(tileMap, 0xFF, tileMapBytes);
    tileCount  = 0;
    pieceIndex = 0;
    row        = 0;
    if (height > 0) {
        rowBase = 0;
        do {
            column = 0;
            if (width > 0) {
                remainingHeight = height - row;
                do {
                    if (tileMap[rowBase + column] == 0xFFFF) {
                        remainingWidth = width - column;
                        if (remainingWidth >= 8)
                            widthPower = 3;
                        else if (remainingWidth >= 4)
                            widthPower = 2;
                        else if (remainingWidth >= 2)
                            widthPower = 1;
                        else if (remainingWidth >= 1)
                            widthPower = 0;
                        if (remainingHeight >= 8)
                            heightPower = 3;
                        else if (remainingHeight >= 4)
                            heightPower = 2;
                        else if (remainingHeight >= 2)
                            heightPower = 1;
                        else if (remainingHeight >= 1)
                            heightPower = 0;
                        region      = data_0205b454[heightPower][widthPower];
                        regionIndex = 0;
                        do {
                            tile      = tileCount;
                            regionRow = 0;
                            if (region->height > 0) {
                                do {
                                    regionColumn = 0;
                                    if (region->width > 0) {
                                        do {
                                            tileMap[(region->x + regionColumn) + width * (region->y + (row + regionRow)) +
                                                    column] = tile << 5;
                                            tile++;
                                            regionColumn++;
                                        } while (regionColumn < region->width);
                                    }
                                    regionRow++;
                                } while (regionRow < region->height);
                            }
                            if (regionRow > 0) {
                                ((u16*)pieces)[pieceIndex]     = tileCount | 0x8000;
                                ((u16*)pieces)[pieceIndex + 1] = ((row + region->y) * 8) | (region->shape << 14);
                                ((u16*)pieces)[pieceIndex + 2] = ((column + region->x) * 8) | (region->size << 14);
                                ((u16*)pieces)[pieceIndex + 3] = tileCount | (palette << 12);
                                tileCount +=
                                    (mapping->unk_00 >> 5) *
                                    (((regionRow * regionColumn) + ((mapping->unk_00 >> 5) - 1)) / (mapping->unk_00 >> 5));
                                pieceIndex += 4;
                            }
                            region++;
                            regionIndex++;
                        } while (regionIndex < 2);
                    }
                    column++;
                } while (column < width);
            }
            rowBase += width;
            row++;
        } while (row < height);
    }
    allocation.tiles           = tileCount;
    ((u16*)pieces)[pieceIndex] = 0xFFFF;
    allocation.bytes           = ((((pieceIndex >> 2) + 1) * 8 + 31) & ~31) + tileMapBytes;
    return allocation;
}

void func_02011810(TextObject* obj) {
    obj->unk_24 = func_020114d4(obj->unk_00, *(UnkQuad*)(void*)&obj->unk_14[0], (u16*)obj->unk_38, (OamCellPiece*)obj->unk_3C,
                                obj->unk_2C)
                      .tiles
                  << 5;
}

void func_0201187c(TextObject* obj, void* arg1) {
    const TextInitArgs* initArgs = arg1;
    *(s32*)((u8*)obj + 0x04)     = *(s32*)((u8*)arg1 + 8);
    obj->unk_00                  = *(s32*)arg1;
    *(s32*)((u8*)obj + 0x44)     = *(s32*)((u8*)arg1 + 4);
    *(Data**)((u8*)obj + 0x48)   = NULL;
    obj->spacing                 = initArgs->spacing;
    obj->tileOrigin              = initArgs->tileOrigin;
    obj->tileSize                = initArgs->tileSize;
    obj->unk_1C                  = (u16)(*(u16*)((u8*)arg1 + 12) * 8);
    obj->unk_1E                  = (u16)(*(u16*)((u8*)arg1 + 14) * 8);
    obj->unk_20                  = (u16)(*(u16*)((u8*)arg1 + 16) * 8);
    obj->unk_22                  = (u16)(*(u16*)((u8*)arg1 + 18) * 8);
    obj->unk_28                  = *(s32*)((u8*)arg1 + 24);
    obj->unk_2C                  = *(s32*)((u8*)arg1 + 28);
    obj->unk_38                  = *(s32*)((u8*)arg1 + 32);
    obj->unk_3C                  = *(s32*)((u8*)arg1 + 32) + (((obj->unk_18[0] * obj->unk_18[1] * 2) + 0x1F) & ~0x1F);
    func_020119b0(obj, *(s32*)((u8*)arg1 + 36), *(s32*)((u8*)arg1 + 40));
    *(s32*)((u8*)obj + 0x40) = *(s32*)((u8*)arg1 + 36);
    obj->unk_30              = func_0200fa10(0x8140, obj->unk_58, obj->unk_5C, obj->unk_4E);
    func_02011e58(obj, (u8*)arg1 + 0x2C);
    func_02011bec(obj);
    func_02011810(obj);
}

void func_02011994(TextObject* obj) {
    DatMgr_ReleaseData(*(Data**)((u8*)obj + 0x48));
    *(Data**)((u8*)obj + 0x48) = NULL;
}

void func_020119b0(TextObject* obj, s32 arg1, s32 arg2) {
    *(s32*)((u8*)obj + 0x40)     = arg1;
    Data*                var_r0  = *(Data**)((u8*)obj + 0x48);
    const TextFontEntry* temp_r4 = (const TextFontEntry*)arg1 + arg2;
    if (var_r0 != NULL) {
        DatMgr_ReleaseData(var_r0);
    }
    s32 var_r6 = 0;
    s32 temp_r7;
    if (*(s32*)((u8*)temp_r4 + 8) == 1) {
        var_r6 = 1;
    }
    s32 temp_r8 = *(s32*)temp_r4;
    s32 temp_r9 = *(s32*)((u8*)temp_r4 + 4);
    temp_r7     = *(s32*)((u8*)obj + 0x44);
    if (BinMgr_FindById(temp_r8) == NULL) {
        var_r0 = DatMgr_LoadPackEntry(temp_r7, NULL, 0, (BinIdentifier*)temp_r8, temp_r9, (BOOL)var_r6);
    } else {
        var_r0 = DatMgr_LoadPackEntryDirect(temp_r7, (BinIdentifier*)temp_r8, temp_r9, var_r6);
    }
    *(Data**)((u8*)obj + 0x48) = var_r0;
    obj->unk_4C                = (u16) * (u16*)((u8*)temp_r4 + 12);
    obj->unk_4E                = (u16) * (u16*)((u8*)temp_r4 + 14);
    obj->secondaryFontSize     = temp_r4->dimensions;
    s32   temp_r3              = *(s32*)((u8*)temp_r4 + 20);
    void* var_r1               = NULL;
    if (var_r0 == NULL || temp_r3 <= 0) {
        var_r1 = NULL;
    } else {
        void* temp_r2 = (u8*)var_r0->buffer + 0x20;
        var_r1        = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3 * 8);
    }
    obj->unk_58     = (u8*)var_r1;
    s32   temp_r3_2 = *(s32*)((u8*)temp_r4 + 24);
    void* var_r1_2  = NULL;
    if (var_r0 == NULL || temp_r3_2 <= 0) {
        var_r1_2 = NULL;
    } else {
        void* temp_r2_2 = (u8*)var_r0->buffer + 0x20;
        var_r1_2        = (u8*)temp_r2_2 + *(s32*)((u8*)temp_r2_2 + temp_r3_2 * 8);
    }
    obj->unk_5C     = (u32)var_r1_2;
    s32   temp_r3_3 = *(s32*)((u8*)temp_r4 + 28);
    void* var_r1_3  = NULL;
    if (var_r0 == NULL || temp_r3_3 <= 0) {
        var_r1_3 = NULL;
    } else {
        void* temp_r2_3 = (u8*)var_r0->buffer + 0x20;
        var_r1_3        = (u8*)temp_r2_3 + *(s32*)((u8*)temp_r2_3 + temp_r3_3 * 8);
    }
    *(u32*)((u8*)obj + 0x60) = (u32)var_r1_3;
    *(s32*)((u8*)obj + 0x64) = *(s32*)((u8*)var_r0->buffer + (*(s32*)((u8*)temp_r4 + 28) * 8) + 0x24);
    *(u16*)((u8*)obj + 0x68) = *(u16*)((u8*)temp_r4 + 32);
    *(s16*)((u8*)obj + 0x50) = (((*(u16*)((u8*)temp_r4 + 16)) + 7) & ~7) >> 3;
    *(s16*)((u8*)obj + 0x52) = (((*(u16*)((u8*)temp_r4 + 18)) + 7) & ~7) >> 3;
    *(u16*)((u8*)obj + 0x74) = 0;
    *(u16*)((u8*)obj + 0x76) = obj->unk_4C;
    obj->unk_7C              = ((((*(u16*)((u8*)obj + 0x54)) * obj->unk_4C) + 7) & ~7) >> 3;
    obj->unk_94              = (void*)((u8*)obj + 0x98);
    obj->unk_78              = 0;
    s32 temp_y               = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset             = *(s32*)((u8*)obj + 0x08);
    obj->yOffset             = temp_y;
}

void* func_02011b7c(TextObject* obj, s32 arg1, s32 arg2) {
    void* var_lr = NULL;
    s32   var_ip = var_lr - 1;
    if (arg1 >= 0 && arg1 < obj->unk_20 && arg2 >= 0 && arg2 < obj->unk_22) {
        var_ip = *(u16*)((u8*)(u32)obj->unk_38 + ((((arg2 >> 3) * obj->unk_18[0]) + (arg1 >> 3)) * 2));
    }
    if (var_ip != 0xFFFF) {
        var_lr = (void*)(obj->unk_28 + (var_ip & ~0x1F));
    }
    return var_lr;
}

void func_02011bec(TextObject* obj) {
    obj->unk_70 = (u16*)((u8*)obj + 0x118);
    obj->unk_6C = 0;
    obj->unk_6E = 0;
}

BOOL func_02011c04(TextObject* obj) {
    if (obj->unk_6C == obj->unk_6E) {
        return TRUE;
    }
    return FALSE;
}

void func_02011c1c(TextObject* obj, u16 arg1) {
    obj->unk_70[obj->unk_6C] = arg1;
    obj->unk_6C++;
    obj->unk_6C = (u16)(u8)obj->unk_6C;
}

s32 func_02011c48(TextObject* obj) {
    u16 value;
    s32 command;
    s32 character;
    for (;;) {
        character = func_020111f0(obj);
        if (character != 0x40) {
            return character;
        }
        command = func_020111f0(obj);
        value   = func_020111f0(obj);
        if (value >= 0x30U && value <= 0x39U) {
            value -= 0x30;
        } else if (value >= 0x61U && value <= 0x66U) {
            value -= 0x57;
        } else if (value >= 0x41U && value <= 0x46U) {
            value -= 0x37;
        } else {
            value = 0x10;
        }
        switch (command) {
            case 0x46:
                func_020119b0(obj, (s32)obj->data, value);
                break;
            case 0x62:
                obj->unk_98 = value;
                break;
            case 0x66:
                obj->unk_9C = value;
                break;
            case 0x61:
                *(s32*)((u8*)obj + 0xa0) = value;
                break;
            case 0x65:
                *(s32*)((u8*)obj + 0xa4) = value;
                break;
        }
    }
}

u16 func_02011d4c(TextObject* obj) {
    u16 temp_r4 = obj->unk_6C;
    u16 temp_r5 = obj->unk_6E;
    u16 val;
loop_1:
    val = func_020111f0(obj);
    if (val == 0x40) {
        func_020111f0(obj);
        func_020111f0(obj);
        goto loop_1;
    }
    obj->unk_6C = temp_r4;
    obj->unk_6E = temp_r5;
    return val;
}

void* func_02011d8c(TextObject* obj, u8* arg1) {
    u8* var_r4 = arg1;
    u8  var_r2 = *var_r4;
    if (var_r2 != 0) {
        do {
            if (var_r2 & 0x80) {
                func_02011c1c(obj, (u16)(var_r4[1] | (var_r2 << 8)));
                var_r4 += 2;
            } else {
                func_02011c1c(obj, func_0200f94c(var_r2));
                var_r4 += 1;
            }
            var_r2 = *var_r4;
        } while (var_r2 != 0);
    }
    return var_r4 + 1;
}

void func_02011df8(TextObject* obj) {
    if (func_02011c04(obj)) {
        return;
    }
    do {
        s32 temp_r6 = func_02011c48(obj);
        u16 temp_r0 = func_02011d4c(obj);
        s32 var_r2  = 0;
        if (!(temp_r0 != 0 && temp_r0 != 0xA)) {
            var_r2 = 1;
        }
        func_02011474(obj, (u32)temp_r6, var_r2);
    } while (!func_02011c04(obj));
}

void func_02011e58(TextObject* obj, u16* arg1) {
    s32 var_ip = 0;
    do {
        u16 temp_r3                = arg1[var_ip];
        ((u32*)obj)[0x26 + var_ip] = temp_r3;
        ((u32*)obj)[0x36 + var_ip] = temp_r3;
        var_ip += 1;
    } while (var_ip < 0x10);
    u32 temp_r1 = *(u32*)((u8*)obj + 0x98);
    temp_r1 |= temp_r1 * 0x10;
    temp_r1 |= temp_r1 << 8;
    *(u32*)((u8*)obj + 0x34) = temp_r1 | (temp_r1 << 0x10);
}

void func_02011e94(TextObject* obj, s32 arg1, s32 arg2, u8* arg3) {
    func_02011df8(obj);
    obj->xOffset = arg1;
    obj->yOffset = arg2;
    func_02011d8c(obj, arg3);
    func_02011df8(obj);
}

void func_02011ecc(TextObject* obj) {
    s32 var_r0;
    func_02011494(obj);
    if (*(s32*)((u8*)obj + 0x04) == 0) {
        var_r0 = 0;
    } else {
        var_r0 = obj->unk_20 - (u16) * (u16*)((u8*)obj + 0x54);
    }
    obj->xOffset = var_r0;
    obj->yOffset = 0;
}
