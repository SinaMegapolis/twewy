#include "Engine/Text.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/File/BinMgr.h"

typedef struct {
    u16 x;
    u16 y;
    u16 width;
    u16 height;
} TextTileArea;

// FontData.bin and Font_Funakosi.bin use these 0x24-byte face descriptors.
// Entry indices address the nested PACK; glyph bitmaps store linear 1/2bpp rows.
typedef struct {
    /* 0x00 */ BinIdentifier* bin;
    /* 0x04 */ s32            packIndex;
    /* 0x08 */ s32            loadMode;
    /* 0x0C */ u16            bitsPerPixel;
    /* 0x0E */ u16            glyphStrideBytes;
    /* 0x10 */ TextU16Pair    dimensions;
    /* 0x14 */ s32            codeMapEntry;
    /* 0x18 */ s32            bitmapEntry;
    /* 0x1C */ s32            advanceWidthsEntry;
    /* 0x20 */ u16            defaultAdvance;
    /* 0x22 */ u16            padding;
} TextFontEntry;

typedef struct {
    /* 0x00 */ s32         screen;
    /* 0x04 */ s32         datSlot;
    /* 0x08 */ s32         vertical;
    /* 0x0C */ TextU16Pair tileOrigin;
    /* 0x10 */ TextU16Pair tileSize;
    /* 0x14 */ TextU16Pair spacing;
    /* 0x18 */ s32         pixels;
    /* 0x1C */ s32         paletteIndex;
    /* 0x20 */ s32         buffer;
    /* 0x24 */ s32         fonts;
    /* 0x28 */ s32         fontIndex;
    /* 0x2C */ u16         controls[16];
} TextSpriteInitArgs;

typedef struct {
    /* 0x00 */ u8          reserved[8];
    /* 0x08 */ u32         datSlot;
    /* 0x0C */ u32         vertical;
    /* 0x10 */ TextU16Pair tileOrigin;
    /* 0x14 */ TextU16Pair tileSize;
    /* 0x18 */ TextU16Pair spacing;
    /* 0x1C */ u32         pixels;
    /* 0x20 */ u32         tilemap;
    /* 0x24 */ u32         fonts;
    /* 0x28 */ u32         fontIndex;
    /* 0x2C */ u16         blankTile;
    /* 0x2E */ u16         firstTile;
    /* 0x30 */ u16         controls[16];
} TextScreenInitArgs;

// The blitter accepts this view at BG offset 0x6C or OBJ offset 0x74.
typedef struct {
    u16   reserved;
    u16   bitsPerPixel;
    u32   mode;
    u32   sourceRowBytes;
    u32   destinationX;
    u32   width;
    u32   height;
    void* source;
    void* destination;
    u32   palette;
} TextGlyphBlit;

typedef struct {
    /* 0x000 */ u32         screen;
    /* 0x004 */ s32         vertical;
    /* 0x008 */ s32         xOffset;
    /* 0x00C */ s32         yOffset;
    /* 0x010 */ TextU16Pair spacing;
    /* 0x014 */ TextU16Pair tileOrigin;
    /* 0x018 */ TextU16Pair tileSize;
    /* 0x01C */ u16         pixelOriginX;
    /* 0x01E */ u16         pixelOriginY;
    /* 0x020 */ u16         pixelWidth;
    /* 0x022 */ u16         pixelHeight;
    /* 0x024 */ u32         pixelBytes;
    /* 0x028 */ u32         pixels;
    /* 0x02C */ u32         paletteIndex;
    /* 0x030 */ u32         blankGlyph;
    /* 0x034 */ u32         backgroundFill;
    /* 0x038 */ u32         tileOffsets;
    /* 0x03C */ u32         pieces;
    /* 0x040 */ u32         fonts;
    /* 0x044 */ s32         datSlot;
    /* 0x048 */ Data*       data;
    /* 0x04C */ u16         bitsPerPixel;
    /* 0x04E */ u16         glyphStrideBytes;
    /* 0x050 */ s16         glyphTileWidth;
    /* 0x052 */ s16         glyphTileHeight;
    /* 0x054 */ TextU16Pair fontSize;
    /* 0x058 */ u8*         codeMap;
    /* 0x05C */ u32         bitmap;
    /* 0x060 */ u32         advanceWidths;
    /* 0x064 */ s32         advanceWidthCount;
    /* 0x068 */ u16         defaultAdvance;
    /* 0x06A */ u16         padding6A;
    /* 0x06C */ u16         queueWrite;
    /* 0x06E */ u16         queueRead;
    /* 0x070 */ u16*        queue;
    /* 0x074 */ u16         blitReserved;
    /* 0x076 */ u16         blitBitsPerPixel;
    /* 0x078 */ s32         blitMode;
    /* 0x07C */ s32         sourceRowBytes;
    /* 0x080 */ s32         destinationX;
    /* 0x084 */ s32         blitWidth;
    /* 0x088 */ s32         blitHeight;
    /* 0x08C */ void*       sourcePixels;
    /* 0x090 */ void*       rowPixels;
    /* 0x094 */ void*       blitPalette;
    /* 0x098 */ u32         palette[16];
    /* 0x0D8 */ u32         savedPalette[16];
    /* 0x118 */ u16         queueStorage[256];
    /* 0x318 */ u32         tileCache[16];
} SpriteTextObject;

s32 Text_GetTilemapAddress(TextObject* obj, s32 x, s32 y);

void Text_ResetQueue(TextObject* obj);

void  Text_BlitTileNoop(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  Text_BlitTile1bpp(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  Text_BlitTile1bppTransparent(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  Text_BlitTile2bpp(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  Text_BlitTile2bppTransparent(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
s32   Text_GetTilePixels(TextObject* obj, s32 x, s32 y);
void  Text_ReleaseFont(TextObject* obj);
void  Text_SetPalette(TextObject* obj, u16* palette);
void  Text_SelectFont(TextObject* obj, u32 fonts, u32 fontIndex);
void  TextSprite_ResetQueue(SpriteTextObject* obj);
BOOL  TextSprite_IsQueueEmpty(SpriteTextObject* obj);
void  TextSprite_EnqueueCode(SpriteTextObject* obj, u16 code);
void  TextSprite_Clear(SpriteTextObject* obj);
void* TextSprite_EnqueueString(SpriteTextObject* obj, u8* string);
void  TextSprite_Render(SpriteTextObject* obj, s32 x, s32 y, u8* string);
void  TextSprite_SetPalette(SpriteTextObject* obj, u16* palette);
void  TextSprite_ClearAndResetPosition(SpriteTextObject* obj);
void  TextSprite_Flush(SpriteTextObject* obj);
u16   TextSprite_PeekCode(SpriteTextObject* obj);
s32   TextSprite_ReadFormattedCode(SpriteTextObject* obj);
u16   TextSprite_DequeueCode(SpriteTextObject* obj);
s32   TextSprite_DrawGlyph(SpriteTextObject*, s32);
s32   TextSprite_DrawControl(SpriteTextObject*, s32, s32);
void  TextSprite_DrawCode(SpriteTextObject*, u32, s32);
void  TextSprite_SelectFont(SpriteTextObject* obj, s32 fonts, s32 fontIndex);
void* TextSprite_GetTilePixels(SpriteTextObject* obj, s32 x, s32 y);

typedef void (*TextTileBlitter)(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
// Indexed by (source bpp * 4) + mode; mode 1 preserves source color zero.
const TextTileBlitter Text_TileBlitters[20] = {
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTile1bpp, Text_BlitTile1bppTransparent,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTile2bpp, Text_BlitTile2bppTransparent,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTileNoop, Text_BlitTileNoop,
    Text_BlitTileNoop, Text_BlitTileNoop,
};

const s32 Text_TransparentPixelMasks[0x10] = {0xF};
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
const TextOamRegion Text_OamRegions[4][4][2] = {
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
extern const u32 Text_TileShifts[8][2];
const u32        Text_TileMasks[9][8][3] = {
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

// Preserve the game's punctuation substitutions as well as fullwidth ASCII.
const u16 Text_AsciiToSjisTable[256] = {
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

u16 Text_AsciiToSjis(s32 ascii) {
    return Text_AsciiToSjisTable[ascii];
}

// Two u16s per Shift-JIS page: payload offset and glyph base.
// Missing pages (0xFFFF) and missing bytes (0xFF) select glyph zero.
u16 Text_FindGlyph(u32 code, u8* codeMap) {
    u16 pageOffset    = ((code & 0x7F00) >> 6);
    u16 payloadOffset = codeMap[pageOffset] | (codeMap[pageOffset + 1] << 8);
    u16 glyphBase     = (codeMap[pageOffset + 3] << 8) | codeMap[pageOffset + 2];
    u16 result        = 0;

    if (payloadOffset != 0xFFFF) {
        u8 glyphOffset = codeMap[(u16)(payloadOffset + ((code & 0xFF) - 0x40))];
        if (glyphOffset != 0xFF) {
            result = glyphOffset + glyphBase;
        }
    }

    return result;
}

u16 Text_GetAdvance(s32 code, u8* codeMap, u8* advanceWidths, u32 advanceWidthCount, u16 defaultAdvance) {
    u16 advance = 0;

    if (advanceWidths != 0) {
        u32 glyph = Text_FindGlyph(code, codeMap);
        if (glyph < advanceWidthCount) {
            advance = advanceWidths[glyph];
        }
    }
    if (advance == 0) {
        advance = defaultAdvance;
    }
    return advance;
}

u32 Text_GetGlyphBitmap(u32 code, u8* codeMap, u32 bitmap, u32 glyphStrideBytes) {
    return glyphStrideBytes * Text_FindGlyph(code, codeMap) + bitmap;
}

void Text_BlitTileNoop(u32*, u8*, u32*, s32, s32, u32, u32*, u32*) {
    return;
}

void Text_BlitTile1bpp(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
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

void Text_BlitTile2bpp(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride, u32* masks,
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

void Text_BlitTile1bppTransparent(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride,
                                  u32* masks, u32* shifts) {
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
                preserveMask |= Text_TransparentPixelMasks[paletteIndex] << bit;
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

void Text_BlitTile2bppTransparent(u32* destination, u8* source, u32* palette, s32 width, s32 height, u32 sourceStride,
                                  u32* masks, u32* shifts) {
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
                preserveMask |= Text_TransparentPixelMasks[paletteIndex] << bit;
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

s32 Text_BlitTile(TextGlyphBlit* glyph) {
    s32 width;
    s32 height;
    s32 pixelOffset;

    width = glyph->width;
    if (width > 8) {
        width = 8;
    }
    height      = (s32)glyph->height > 8 ? 8 : (s32)glyph->height;
    pixelOffset = glyph->destinationX;

    Text_TileBlitters[(glyph->bitsPerPixel << 2) + glyph->mode](
        glyph->destination, glyph->source, glyph->palette, width, height, glyph->sourceRowBytes,
        Text_TileMasks[width][pixelOffset], Text_TileShifts[pixelOffset]);

    return ((pixelOffset + width + 7) & ~7) >> 3;
}

s32 Text_BlitGlyph(TextObject* obj, s32 xOffset, s32 yOffset, s32 glyphSource, s32 width, s32 height) {
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
    u16   clipWidth = obj->pixelWidth;
    if ((s32)(xOffset + width) > (s32)clipWidth) {
        width = clipWidth - xOffset;
    }
    u16 clipHeight = obj->pixelHeight;
    if ((s32)(clippedY + height) > (s32)clipHeight) {
        height = (s32)clipHeight - clippedY;
    }
    if (width <= 0 || height <= 0) {
        return 0;
    }
    {
        firstTile             = (s32)(u16)(obj->firstTile + (((clippedY >> 3) * obj->tileSize.x) + (clippedX >> 3)));
        s32 rowOffset         = clippedY & 7;
        tileCache             = (u8*)obj + 0x310;
        obj->primaryRowPixels = (void*)((u8*)tileCache + (rowOffset * 4));
        obj->destinationX     = clippedX & 7;
        obj->blitHeight       = 8 - rowOffset;
        rowsDrawn             = 0;
        if (height > 0) {
            do {
                currentTile   = (u16*)Text_GetTilemapAddress(obj, clippedX, clippedY + rowsDrawn);
                currentPixels = (void*)Text_GetTilePixels(obj, clippedX, clippedY + rowsDrawn);
                if (currentTile == NULL && currentPixels == NULL) {
                    goto block_14;
                }
                if (currentTile != NULL && *currentTile == obj->blankTile) {
                block_14:
                    MI_CpuFill(obj->backgroundFill, tileCache, 0x20);
                } else {
                    MI_CpuCopy(currentPixels, tileCache, 0x20);
                }
                columnsDrawn      = 0;
                obj->sourcePixels = (rowsDrawn * obj->sourceRowBytes) + glyphData;
                obj->blitWidth    = width;
                if (obj->blitWidth > 0) {
                    s32 tileColumn = 0;
                    do {
                        if (currentTile != NULL) {
                            nextTile = currentTile + 1;
                        } else {
                            nextTile = (u16*)Text_GetTilemapAddress(obj, clippedX + columnsDrawn + 8, clippedY + rowsDrawn);
                        }
                        if (currentPixels != NULL) {
                            nextPixels = (void*)((u8*)currentPixels + 0x20);
                        } else {
                            nextPixels = (void*)Text_GetTilePixels(obj, clippedX + columnsDrawn + 8, clippedY + rowsDrawn);
                        }
                        if (nextTile == NULL && nextPixels == NULL) {
                            goto block_29;
                        }
                        if (nextTile != NULL && *nextTile == obj->blankTile) {
                        block_29:
                            MI_CpuFill(obj->backgroundFill, (u8*)tileCache + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(nextPixels, (u8*)tileCache + 0x20, 0x20);
                        }
                        tileSpan = Text_BlitTile((TextGlyphBlit*)&obj->blitReserved);
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
                        obj->sourcePixels = obj->sourcePixels + obj->blitBitsPerPixel;
                        columnsDrawn += 8;
                        obj->blitWidth = obj->blitWidth - 8;
                    } while (obj->blitWidth > 0);
                }
                firstTile             = (s32)(u16)(firstTile + obj->tileSize.x);
                obj->primaryRowPixels = tileCache;
                rowsDrawn += obj->blitHeight;
                s32 remainingRows = height - rowsDrawn;
                obj->blitHeight   = remainingRows;
                if (remainingRows > 8) {
                    obj->blitHeight = 8;
                }
            } while (rowsDrawn < height);
        }
        return (s32)(((clippedX + (s32)width + 6) - (clippedX & ~7)) & ~7) >> 3;
    }
    return 0;
}

u16 Text_DequeueCode(TextObject* obj) {
    if (obj->queueWrite != obj->queueRead) {
        u16 val = obj->queue[obj->queueRead];
        obj->queueRead++;
        obj->queueRead = obj->queueRead & 0xff;
        return val;
    }
    return 0;
}

BOOL Text_DrawGlyph(TextObject* obj, u32 code) {
    u32 glyphBitmap = Text_GetGlyphBitmap(code, obj->codeMap, obj->bitmap, obj->glyphStrideBytes);
    u32 advance     = Text_GetAdvance(code, obj->codeMap, obj->advanceWidths, obj->advanceWidthCount, obj->defaultAdvance);

    Text_BlitGlyph(obj, obj->xOffset, obj->yOffset, glyphBitmap, advance, obj->glyphHeight);
    if (obj->vertical == 0) {
        obj->xOffset += advance;
    } else {
        obj->yOffset += obj->glyphHeight;
    }
    if ((obj->spacing.x != 0) && (obj->blitMode == 0)) {
        if (obj->vertical == 0) {
            Text_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, obj->spacing.x, obj->glyphHeight);
            obj->xOffset += obj->spacing.x;
        } else {
            Text_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, obj->glyphWidth, obj->spacing.y);
            obj->yOffset += obj->spacing.y;
        }
    }
    return TRUE;
}

static inline void SetOffsets(TextObject* obj, u32 x, u32 y) {
    obj->xOffset = x;
    obj->yOffset = y;
}

BOOL Text_DrawControl(TextObject* obj, u32 code, u32 drawBlank) {
    BOOL advanced = FALSE;

    switch (code) {
        case 0:
            break;
        case 10:
            if (obj->vertical == 0) {
                SetOffsets(obj, 0, obj->yOffset + (obj->glyphHeight + obj->spacing.y));
            } else {
                SetOffsets(obj, obj->xOffset - (obj->glyphWidth + obj->spacing.x), 0);
            }
            break;
        case 9:
            if (obj->vertical == 0) {
                if (drawBlank == 1) {
                    Text_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, 8 - (obj->xOffset & 7U),
                                   obj->glyphHeight);
                }
                SetOffsets(obj, (obj->xOffset + 8) & ~7, obj->yOffset);
            } else {
                if (drawBlank == 1) {
                    Text_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, obj->glyphWidth, 8 - (obj->yOffset & 7U));
                }
                SetOffsets(obj, obj->xOffset, (obj->yOffset + 8) & ~7);
            }
            advanced = TRUE;
            break;
    }

    return advanced;
}

void Text_DrawCode(TextObject* obj, s32 code, s32 unused) {
    if (code & 0xFF00) {
        Text_DrawGlyph(obj, code);
        return;
    }
    Text_DrawControl(obj, code, 1);
}

void Text_Clear(TextObject* obj) {
    if (obj->tileSize.x == 0 && obj->tileSize.y == 0) {
        return;
    }

    u16 i;
    s32 iVar3 = Text_GetTilemapAddress(obj, 0, 0);

    if (iVar3 != 0) {
        for (i = 0; i < obj->tileSize.y; i++) {
            MI_CpuFillU16(obj->blankTile, iVar3, obj->tileSize.x << 1);
            iVar3 += obj->mapWidth * 2;
        }
    } else {
        MI_CpuFill(obj->backgroundFill, Text_GetTilePixels(obj, 0, 0), obj->tileSize.x * obj->tileSize.y * 0x20);
    }
}

void Text_Init(TextObject* obj, TextScreenInitArgs* args) {
    obj->vertical  = args->vertical;
    obj->datSlot   = args->datSlot;
    obj->data      = NULL;
    obj->mapWidth  = 0x20;
    obj->mapHeight = 0x20;

    obj->spacing    = args->spacing;
    obj->tileOrigin = args->tileOrigin;
    obj->tileSize   = args->tileSize;

    obj->pixelOriginX = args->tileOrigin.x << 3;
    obj->pixelOriginY = args->tileOrigin.y << 3;
    obj->pixelWidth   = args->tileSize.x << 3;
    obj->pixelHeight  = args->tileSize.y << 3;

    obj->pixels    = args->pixels;
    obj->tilemap   = args->tilemap;
    obj->blankTile = args->blankTile;
    obj->firstTile = args->firstTile;

    Text_SelectFont(obj, args->fonts, args->fontIndex);
    obj->fonts      = args->fonts;
    obj->blankGlyph = Text_GetGlyphBitmap(0x8140, obj->codeMap, obj->bitmap, obj->glyphStrideBytes);
    Text_SetPalette(obj, &args->controls[0]);
    Text_ResetQueue(obj);
}

void Text_ReleaseFont(TextObject* obj) {
    DatMgr_ReleaseData(obj->data);
    obj->data = NULL;
}

void Text_SelectFont(TextObject* obj, u32 fonts, u32 fontIndex) {
    obj->fonts                = (s32)fonts;
    void*                data = obj->data;
    const TextFontEntry* face = (const TextFontEntry*)fonts + fontIndex;
    if (data == NULL) {
        if (data != NULL) {
            DatMgr_ReleaseData((Data*)data);
        }
        s32 compressed = 0;
        s32 datSlot;
        if (face->loadMode == 1) {
            compressed = 1;
        }
        s32 bin       = (s32)face->bin;
        s32 packIndex = face->packIndex;
        datSlot       = obj->datSlot;
        if (BinMgr_FindById(bin) == NULL) {
            data = DatMgr_LoadPackEntry(datSlot, NULL, 0, (BinIdentifier*)bin, packIndex, (BOOL)compressed);
        } else {
            data = DatMgr_LoadPackEntryDirect(datSlot, (BinIdentifier*)bin, packIndex, compressed);
        }
    }
    obj->data             = (Data*)data;
    obj->bitsPerPixel     = face->bitsPerPixel;
    obj->glyphStrideBytes = face->glyphStrideBytes;
    obj->fontSize         = face->dimensions;
    s32   codeMapEntry    = face->codeMapEntry;
    void* codeMap         = NULL;
    if (data == NULL || codeMapEntry <= 0) {
        codeMap = NULL;
    } else {
        void* packEntries = (u8*)((Data*)data)->buffer + 0x20;
        codeMap           = (u8*)packEntries + *(s32*)((u8*)packEntries + codeMapEntry * 8);
    }
    obj->codeMap      = (u8*)codeMap;
    s32   bitmapEntry = face->bitmapEntry;
    void* bitmap      = NULL;
    if (data == NULL || bitmapEntry <= 0) {
        bitmap = NULL;
    } else {
        void* packEntries = (u8*)((Data*)data)->buffer + 0x20;
        bitmap            = (u8*)packEntries + *(s32*)((u8*)packEntries + bitmapEntry * 8);
    }
    obj->bitmap              = (u32)bitmap;
    s32   advanceWidthsEntry = face->advanceWidthsEntry;
    void* advanceWidths      = NULL;
    if (data == NULL || advanceWidthsEntry <= 0) {
        advanceWidths = NULL;
    } else {
        void* packEntries = (u8*)((Data*)data)->buffer + 0x20;
        advanceWidths     = (u8*)packEntries + *(s32*)((u8*)packEntries + advanceWidthsEntry * 8);
    }
    obj->advanceWidths     = (u8*)advanceWidths;
    obj->advanceWidthCount = *(s32*)((u8*)((Data*)data)->buffer + (face->advanceWidthsEntry * 8) + 0x24);
    obj->defaultAdvance    = face->defaultAdvance;
    obj->glyphTileWidth    = (((face->dimensions.x) + 7) & ~7) >> 3;
    obj->glyphTileHeight   = (((face->dimensions.y) + 7) & ~7) >> 3;
    obj->blitReserved      = 0;
    obj->blitBitsPerPixel  = obj->bitsPerPixel;
    obj->sourceRowBytes    = (((obj->glyphWidth * obj->bitsPerPixel) + 7) & ~7) >> 3;
    obj->blitPalette       = obj->palette;
    obj->blitMode          = 0;
    // Keep these aliasing reloads: named self-assignments change MWCC output.
    s32 temp_y   = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset = *(s32*)((u8*)obj + 0x08);
    obj->yOffset = temp_y;
}

s32 Text_GetTilemapAddress(TextObject* obj, s32 x, s32 y) {
    if (x < 0 || x >= obj->pixelWidth || y < 0 || y >= obj->pixelHeight)
        return 0;
    s32 row     = (y + obj->pixelOriginY) >> 3;
    s32 address = obj->tilemap + (row * obj->mapWidth) * 2;
    x += obj->pixelOriginX;
    return address + (x >> 3) * 2;
}

s32 Text_GetTilePixels(TextObject* obj, s32 x, s32 y) {
    register u32 pixels = 0;
    s32          offset = pixels - 1;

    if ((x >= 0) && (x < obj->pixelWidth) && (y >= 0) && (y < obj->pixelHeight)) {
        s32 tileOffset = (y >> 3) * obj->tileSize.x;
        u32 rowOffset  = (y << 0x1D);
        tileOffset += x >> 3;
        tileOffset <<= 5;
        rowOffset >>= 0x1B;
        offset = tileOffset + rowOffset;
    }

    if (offset >= 0) {
        pixels = obj->pixels + (offset & ~0x1F);
    }

    return pixels;
}

void Text_ResetQueue(TextObject* obj) {
    obj->queue      = &obj->queueStorage;
    obj->queueWrite = 0;
    obj->queueRead  = 0;
}

BOOL Text_IsQueueEmpty(TextObject* obj) {
    return obj->queueWrite == obj->queueRead;
}

void Text_EnqueueCode(TextObject* obj, u16 code) {
    obj->queue[obj->queueWrite] = code;
    obj->queueWrite++;
    obj->queueWrite &= 0xff;
}

// Formatting consumes @, a command (F/b/f/a/e), and one hexadecimal digit.
s32 Text_ReadFormattedCode(TextObject* obj) {
    u16 value;
    s32 command;
    s32 character;
    for (;;) {
        character = Text_DequeueCode(obj);
        if (character != 0x40) {
            return character;
        }
        command = Text_DequeueCode(obj);
        value   = Text_DequeueCode(obj);
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
                Text_SelectFont(obj, obj->fonts, value);
                break;
            case 0x62:
                obj->palette[0] = value;
                break;
            case 0x66:
                obj->palette[1] = value;
                break;
            case 0x61:
                obj->palette[2] = value;
                break;
            case 0x65:
                obj->palette[3] = value;
                break;
        }
    }
}

s32 Text_PeekCode(TextObject* arg0) {
    u16 temp_r4;
    u16 temp_r5;

    temp_r4 = arg0->queueWrite;
    temp_r5 = arg0->queueRead;
loop_1:
    s32 val = Text_DequeueCode(arg0);
    if (val == 0x40) {
        Text_DequeueCode(arg0);
        Text_DequeueCode(arg0);
        goto loop_1;
    }
    arg0->queueWrite = temp_r4;
    arg0->queueRead  = temp_r5;
    return val;
}

// High-bit bytes consume a big-endian two-byte code; other bytes use the ASCII table.
void* Text_EnqueueString(TextObject* obj, u8* string) {
    u8  byte;
    u8* cursor;

    cursor = string;
    byte   = *cursor;
    if (byte != 0) {
        do {
            if (byte & 0x80) {
                Text_EnqueueCode(obj, (u16)(cursor[1] | (byte << 8)));
                cursor += 2;
            } else {
                Text_EnqueueCode(obj, Text_AsciiToSjis(byte));
                cursor += 1;
            }
            byte = *cursor;
        } while (byte != 0);
    }
    return cursor + 1;
}

void Text_Flush(TextObject* obj) {
    while (Text_IsQueueEmpty(obj) == FALSE) {
        s32 charCode = Text_ReadFormattedCode(obj);
        s32 result   = Text_PeekCode(obj);
        s32 val      = 0;

        if (result == 0 || result == 0xA) {
            val = 1;
        }

        Text_DrawCode(obj, charCode, val);
    }
}

void Text_SetPalette(TextObject* obj, u16* palette) {
    s32 index = 0;
    do {
        u16 color                = palette[index];
        obj->palette[index]      = color;
        obj->savedPalette[index] = color;
        index += 1;
    } while (index < 0x10);
    u32 fill = obj->palette[0];
    fill |= fill * 0x10;
    fill |= fill << 8;
    obj->backgroundFill = fill | (fill << 0x10);
}

void Text_RenderToScreen(TextObject* obj, s32 xOffset, s32 yOffset, const char* string) {
    Text_Flush(obj);
    obj->xOffset = xOffset;
    obj->yOffset = yOffset;
    Text_EnqueueString(obj, string);
    Text_Flush(obj);
}

void Text_ClearAndResetPosition(TextObject* obj) {
    s32 sVar1;

    Text_Clear(obj);
    if (obj->vertical == 0) {
        sVar1 = 0;
    } else {
        sVar1 = obj->pixelWidth - obj->glyphWidth;
    }
    obj->xOffset = sVar1;
    obj->yOffset = 0;
}

// Nonmatching: pixelX and the strength-reduced (pixelX >> 3) tile counter swap registers (r5/r9)
void Text_ClearRect(TextObject* obj, s32 xOffset, s32 yOffset, s32 width, s32 height) {
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
    clipWidth = obj->pixelWidth;
    if ((s32)(clippedX + clippedWidth) > (s32)clipWidth) {
        clippedWidth = clipWidth - clippedX;
    }
    clipHeight = obj->pixelHeight;
    if ((s32)(clippedY + height) > (s32)clipHeight) {
        height = clipHeight - clippedY;
    }
    if ((clippedWidth > 0) && (height > 0)) {
        savedControls = obj->blitMode;
        obj->blitMode = NULL;
        right         = clippedX + clippedWidth;
        obj->xOffset  = right;
        bottom        = clippedY + height;
        obj->yOffset  = bottom;
        topRemainder  = clippedY & 7;
        if (topRemainder != 0) {
            topLeftRemainder = clippedX & 7;
            if (topLeftRemainder != 0) {
                Text_BlitGlyph(obj, clippedX, clippedY, obj->blankGlyph, 8 - topLeftRemainder, 8 - topRemainder);
            }
            topX          = (clippedX + 7) & ~7;
            topRightTileX = right & ~7;
            if (topX < topRightTileX) {
                do {
                    Text_BlitGlyph(obj, topX, clippedY, obj->blankGlyph, 8, 8 - topRemainder);
                    topX += 8;
                } while (topX < topRightTileX);
            }
            topRightRemainder = right & 7;
            if ((topRightRemainder != 0) && ((s32)(clippedX & ~7) < topRightTileX)) {
                Text_BlitGlyph(obj, topRightTileX, clippedY, obj->blankGlyph, topRightRemainder, 8 - topRemainder);
            }
        }
        leftAligned   = (clippedX + 7) & ~7;
        rowY          = (clippedY + 7) & ~7;
        rightTileX    = right & ~7;
        alignedBottom = bottom & ~7;
        if (Text_GetTilemapAddress(obj, 0, 0) != 0) {
            if (rowY < alignedBottom) {
                mapLeftRemainder  = clippedX & 7;
                mapLeftWidth      = 8 - mapLeftRemainder;
                mapRightRemainder = right & 7;
                mapLeftTileX      = clippedX & ~7;
                do {
                    if (mapLeftRemainder != 0) {
                        Text_BlitGlyph(obj, clippedX, rowY, obj->blankGlyph, mapLeftWidth, 8);
                    }
                    tileMap = (u16*)Text_GetTilemapAddress(obj, 0, rowY);
                    mapX    = leftAligned;
                    if (leftAligned < rightTileX) {
                        do {
                            tileMap[mapX >> 3] = obj->blankTile;
                            mapX += 8;
                        } while (mapX < rightTileX);
                    }
                    if ((mapRightRemainder != 0) && (mapLeftTileX < rightTileX)) {
                        Text_BlitGlyph(obj, rightTileX, rowY, obj->blankGlyph, mapRightRemainder, 8);
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
                    Text_BlitGlyph(obj, leftAligned, rowY, obj->blankGlyph, pixelLeftWidth, 8);
                }
                pixels = (void*)Text_GetTilePixels(obj, leftAligned, rowY);
                pixelX = leftAligned;
                if (leftAligned < rightTileX) {
                    do {
                        MI_CpuFill(obj->backgroundFill, (u8*)pixels + ((pixelX >> 3) << 5), 0x20);
                        pixelX += 8;
                    } while (pixelX < rightTileX);
                }
                if ((pixelRightRemainder != 0) && (pixelLeftTileX < rightTileX)) {
                    Text_BlitGlyph(obj, rightTileX, rowY, obj->blankGlyph, pixelRightRemainder, 8);
                }
                rowY += 8;
            } while (rowY < alignedBottom);
        }
        bottomRemainder = bottom & 7;
        if ((bottomRemainder != 0) && ((s32)(clippedY & ~7) < alignedBottom)) {
            bottomLeftRemainder = clippedX & 7;
            if (bottomLeftRemainder != 0) {
                Text_BlitGlyph(obj, clippedX, alignedBottom, obj->blankGlyph, 8 - bottomLeftRemainder, bottomRemainder);
            }
            if (leftAligned < rightTileX) {
                do {
                    Text_BlitGlyph(obj, leftAligned, alignedBottom, obj->blankGlyph, 8, bottomRemainder);
                    leftAligned += 8;
                } while (leftAligned < rightTileX);
            }
            bottomRightRemainder = right & 7;
            if ((bottomRightRemainder != 0) && ((s32)(clippedX & ~7) < rightTileX)) {
                Text_BlitGlyph(obj, rightTileX, alignedBottom, obj->blankGlyph, bottomRightRemainder, bottomRemainder);
            }
        }
        obj->blitMode = savedControls;
    }
}

s32 TextSprite_BlitGlyph(SpriteTextObject* obj, s32 xOffset, s32 yOffset, s32 glyphSource, s32 width, s32 height) {
    void* tileCache;
    s32   rowsDrawn;
    s32   columnsDrawn;
    s32   tileSpan;
    void* currentPixels;

    void* nextPixels;
    s32   clippedY   = yOffset;
    s32   glyphData  = glyphSource;
    u16   clipWidth  = obj->pixelWidth;
    u16   clipHeight = obj->pixelHeight;
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
        tileCache         = obj->tileCache;
        s32 rowOffset     = clippedY & 7;
        obj->rowPixels    = (void*)((u8*)tileCache + (rowOffset * 4));
        obj->destinationX = xOffset & 7;
        obj->blitHeight   = 8 - rowOffset;
        rowsDrawn         = 0;
        if (height > 0) {
            do {
                void* firstPixels = TextSprite_GetTilePixels(obj, xOffset, clippedY + rowsDrawn);
                currentPixels     = firstPixels;
                if (currentPixels == NULL) {
                    MI_CpuFill(obj->backgroundFill, tileCache, 0x20);
                } else {
                    MI_CpuCopy(firstPixels, tileCache, 0x20);
                }
                columnsDrawn      = 0;
                obj->sourcePixels = (rowsDrawn * obj->sourceRowBytes) + glyphData;
                obj->blitWidth    = width;
                if (obj->blitWidth > 0) {
                    do {
                        nextPixels = TextSprite_GetTilePixels(obj, xOffset + columnsDrawn + 8, clippedY + rowsDrawn);
                        if (nextPixels == NULL) {
                            MI_CpuFill(obj->backgroundFill, (u8*)tileCache + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(nextPixels, (u8*)tileCache + 0x20, 0x20);
                        }
                        tileSpan = Text_BlitTile((TextGlyphBlit*)&obj->blitReserved);
                        if (currentPixels != NULL) {
                            MI_CpuCopy(tileCache, currentPixels, 0x20);
                        }
                        if ((tileSpan > 1) && (nextPixels != NULL)) {
                            MI_CpuCopy((u8*)tileCache + 0x20, nextPixels, 0x20);
                        }
                        currentPixels = nextPixels;
                        MI_CpuCopy((u8*)tileCache + 0x20, tileCache, 0x20);
                        columnsDrawn += 8;
                        obj->sourcePixels = obj->sourcePixels + obj->blitBitsPerPixel;
                        obj->blitWidth    = obj->blitWidth - 8;
                    } while (obj->blitWidth > 0);
                }
                obj->rowPixels = tileCache;
                rowsDrawn += obj->blitHeight;
                s32 remainingRows = height - rowsDrawn;
                obj->blitHeight   = remainingRows;
                if (remainingRows > 8) {
                    obj->blitHeight = 8;
                }
            } while (rowsDrawn < height);
        }
        return (s32)(((xOffset + width + 6) - (xOffset & ~7)) & ~7) >> 3;
    }
    return 0;
}
u16 TextSprite_DequeueCode(SpriteTextObject* obj) {
    if (obj->queueWrite != obj->queueRead) {
        u16 val = obj->queue[obj->queueRead];
        obj->queueRead++;
        obj->queueRead &= 0xff;
        return val;
    }
    return 0;
}

s32 TextSprite_DrawGlyph(SpriteTextObject* obj, s32 code) {
    s32 glyphBitmap = Text_GetGlyphBitmap(code, obj->codeMap, obj->bitmap, obj->glyphStrideBytes);
    s32 advance = (s32)Text_GetAdvance(code, obj->codeMap, (s32) * (s32*)((u8*)obj + 0x60), (s32) * (s32*)((u8*)obj + 0x64),
                                       obj->defaultAdvance);
    TextSprite_BlitGlyph(obj, obj->xOffset, obj->yOffset, glyphBitmap, advance, (s32) * (u16*)((u8*)obj + 0x56));
    if (obj->vertical == 0) {
        obj->xOffset = obj->xOffset + advance;
    } else {
        obj->yOffset = obj->yOffset + obj->fontSize.y;
    }
    u16 spacingX = obj->spacing.x;
    if (spacingX != 0 && obj->blitMode == 0) {
        if (obj->vertical == 0) {
            TextSprite_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, (s32)spacingX,
                                 (s32) * (u16*)((u8*)obj + 0x56));
            obj->xOffset = obj->xOffset + obj->spacing.x;
        } else {
            TextSprite_BlitGlyph(obj, obj->xOffset, obj->yOffset, obj->blankGlyph, (s32) * (u16*)((u8*)obj + 0x54),
                                 (s32)obj->spacing.y);
            obj->yOffset = obj->yOffset + obj->spacing.y;
        }
    }
    return 1;
}

void TextSprite_DrawCode(SpriteTextObject* obj, u32 code, s32 unused) {
    if ((code & 0xff00) != 0) {
        TextSprite_DrawGlyph(obj, (s32)code);
    } else {
        TextSprite_DrawControl(obj, (s32)code, 1);
    }
}

s32 TextSprite_DrawControl(SpriteTextObject* obj, s32 code, s32 drawBlank) {
    s32 advanced = 0;
    switch (code) {
        case 0:
            break;
        case 10:
            if (obj->vertical == 0) {
                s32 nextY    = obj->yOffset + (obj->fontSize.y + obj->spacing.y);
                obj->xOffset = 0;
                obj->yOffset = nextY;
            } else {
                s32 temp_r1  = obj->fontSize.x + obj->spacing.x;
                obj->xOffset = obj->xOffset - temp_r1;
                obj->yOffset = 0;
            }
            break;
        case 9:
            if (obj->vertical == 0) {
                if (drawBlank == 1) {
                    s32 temp_r1 = obj->xOffset;
                    TextSprite_BlitGlyph(obj, temp_r1, obj->yOffset, obj->blankGlyph, 8 - (temp_r1 & 7),
                                         (s32) * (u16*)((u8*)obj + 0x56));
                }
                // Keep these aliasing reloads: named self-assignments change MWCC output.
                s32 temp_y   = *(s32*)((u8*)obj + 0x0C);
                obj->xOffset = (obj->xOffset + 8) & ~7;
                obj->yOffset = temp_y;
            } else {
                if (drawBlank == 1) {
                    s32 temp_r2 = obj->yOffset;
                    TextSprite_BlitGlyph(obj, obj->xOffset, temp_r2, obj->blankGlyph, (s32) * (u16*)((u8*)obj + 0x54),
                                         8 - (temp_r2 & 7));
                }
                s32 temp_x   = *(s32*)((u8*)obj + 0x08);
                s32 temp_r1  = obj->yOffset + 8;
                obj->xOffset = temp_x;
                obj->yOffset = temp_r1 & ~7;
            }
            advanced = 1;
            break;
    }
    return advanced;
}

void TextSprite_Clear(SpriteTextObject* obj) {
    if (obj->tileSize.x == 0 && obj->tileSize.y == 0) {
        return;
    }
    MI_CpuFill(obj->backgroundFill, TextSprite_GetTilePixels(obj, 0, 0), obj->pixelBytes);
}

// Nonmatching: target keeps a dead per-tile counter in r11, shifting registers and two stack slots
TextOamAllocation TextSprite_BuildOam(s32 screen, TextTileArea area, u16* tileMap, OamCellPiece* pieces, s32 palette) {
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

    width        = area.width;
    height       = area.height;
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
                        region      = Text_OamRegions[heightPower][widthPower];
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

void TextSprite_UpdateOam(SpriteTextObject* obj) {
    obj->pixelBytes = TextSprite_BuildOam(obj->screen, *(TextTileArea*)(void*)&obj->tileOrigin.x, (u16*)obj->tileOffsets,
                                          (OamCellPiece*)obj->pieces, obj->paletteIndex)
                          .tiles
                      << 5;
}

void TextSprite_Init(SpriteTextObject* obj, void* arg1) {
    const TextSpriteInitArgs* initArgs = arg1;
    obj->vertical                      = initArgs->vertical;
    obj->screen                        = initArgs->screen;
    obj->datSlot                       = initArgs->datSlot;
    obj->data                          = NULL;
    obj->spacing                       = initArgs->spacing;
    obj->tileOrigin                    = initArgs->tileOrigin;
    obj->tileSize                      = initArgs->tileSize;
    obj->pixelOriginX                  = (u16)(initArgs->tileOrigin.x * 8);
    obj->pixelOriginY                  = (u16)(initArgs->tileOrigin.y * 8);
    obj->pixelWidth                    = (u16)(initArgs->tileSize.x * 8);
    obj->pixelHeight                   = (u16)(initArgs->tileSize.y * 8);
    obj->pixels                        = initArgs->pixels;
    obj->paletteIndex                  = initArgs->paletteIndex;
    obj->tileOffsets                   = initArgs->buffer;
    obj->pieces                        = initArgs->buffer + (((obj->tileSize.x * obj->tileSize.y * 2) + 0x1F) & ~0x1F);
    TextSprite_SelectFont(obj, initArgs->fonts, initArgs->fontIndex);
    obj->fonts      = initArgs->fonts;
    obj->blankGlyph = Text_GetGlyphBitmap(0x8140, obj->codeMap, obj->bitmap, obj->glyphStrideBytes);
    TextSprite_SetPalette(obj, initArgs->controls);
    TextSprite_ResetQueue(obj);
    TextSprite_UpdateOam(obj);
}

void TextSprite_ReleaseFont(SpriteTextObject* obj) {
    DatMgr_ReleaseData(obj->data);
    obj->data = NULL;
}

void TextSprite_SelectFont(SpriteTextObject* obj, s32 fonts, s32 fontIndex) {
    obj->fonts                = fonts;
    Data*                data = obj->data;
    const TextFontEntry* face = (const TextFontEntry*)fonts + fontIndex;
    if (data != NULL) {
        DatMgr_ReleaseData(data);
    }
    s32 compressed = 0;
    s32 datSlot;
    if (face->loadMode == 1) {
        compressed = 1;
    }
    s32 bin       = (s32)face->bin;
    s32 packIndex = face->packIndex;
    datSlot       = obj->datSlot;
    if (BinMgr_FindById(bin) == NULL) {
        data = DatMgr_LoadPackEntry(datSlot, NULL, 0, (BinIdentifier*)bin, packIndex, (BOOL)compressed);
    } else {
        data = DatMgr_LoadPackEntryDirect(datSlot, (BinIdentifier*)bin, packIndex, compressed);
    }
    obj->data             = data;
    obj->bitsPerPixel     = face->bitsPerPixel;
    obj->glyphStrideBytes = face->glyphStrideBytes;
    obj->fontSize         = face->dimensions;
    s32   codeMapEntry    = face->codeMapEntry;
    void* codeMap         = NULL;
    if (data == NULL || codeMapEntry <= 0) {
        codeMap = NULL;
    } else {
        void* packEntries = (u8*)data->buffer + 0x20;
        codeMap           = (u8*)packEntries + *(s32*)((u8*)packEntries + codeMapEntry * 8);
    }
    obj->codeMap      = (u8*)codeMap;
    s32   bitmapEntry = face->bitmapEntry;
    void* bitmap      = NULL;
    if (data == NULL || bitmapEntry <= 0) {
        bitmap = NULL;
    } else {
        void* bitmapEntries = (u8*)data->buffer + 0x20;
        bitmap              = (u8*)bitmapEntries + *(s32*)((u8*)bitmapEntries + bitmapEntry * 8);
    }
    obj->bitmap              = (u32)bitmap;
    s32   advanceWidthsEntry = face->advanceWidthsEntry;
    void* advanceWidths      = NULL;
    if (data == NULL || advanceWidthsEntry <= 0) {
        advanceWidths = NULL;
    } else {
        void* widthEntries = (u8*)data->buffer + 0x20;
        advanceWidths      = (u8*)widthEntries + *(s32*)((u8*)widthEntries + advanceWidthsEntry * 8);
    }
    obj->advanceWidths     = (u32)advanceWidths;
    obj->advanceWidthCount = *(s32*)((u8*)data->buffer + (face->advanceWidthsEntry * 8) + 0x24);
    obj->defaultAdvance    = face->defaultAdvance;
    obj->glyphTileWidth    = (((face->dimensions.x) + 7) & ~7) >> 3;
    obj->glyphTileHeight   = (((face->dimensions.y) + 7) & ~7) >> 3;
    obj->blitReserved      = 0;
    obj->blitBitsPerPixel  = obj->bitsPerPixel;
    obj->sourceRowBytes    = ((((obj->fontSize.x) * obj->bitsPerPixel) + 7) & ~7) >> 3;
    obj->blitPalette       = obj->palette;
    obj->blitMode          = 0;
    // Keep these aliasing reloads: named self-assignments change MWCC output.
    s32 temp_y   = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset = *(s32*)((u8*)obj + 0x08);
    obj->yOffset = temp_y;
}

void* TextSprite_GetTilePixels(SpriteTextObject* obj, s32 x, s32 y) {
    void* pixels = NULL;
    s32   offset = pixels - 1;
    if (x >= 0 && x < obj->pixelWidth && y >= 0 && y < obj->pixelHeight) {
        offset = *(u16*)((u8*)(u32)obj->tileOffsets + ((((y >> 3) * obj->tileSize.x) + (x >> 3)) * 2));
    }
    if (offset != 0xFFFF) {
        pixels = (void*)(obj->pixels + (offset & ~0x1F));
    }
    return pixels;
}

void TextSprite_ResetQueue(SpriteTextObject* obj) {
    obj->queue      = obj->queueStorage;
    obj->queueWrite = 0;
    obj->queueRead  = 0;
}

BOOL TextSprite_IsQueueEmpty(SpriteTextObject* obj) {
    if (obj->queueWrite == obj->queueRead) {
        return TRUE;
    }
    return FALSE;
}

void TextSprite_EnqueueCode(SpriteTextObject* obj, u16 code) {
    obj->queue[obj->queueWrite] = code;
    obj->queueWrite++;
    obj->queueWrite = (u16)(u8)obj->queueWrite;
}

s32 TextSprite_ReadFormattedCode(SpriteTextObject* obj) {
    u16 value;
    s32 command;
    s32 character;
    for (;;) {
        character = TextSprite_DequeueCode(obj);
        if (character != 0x40) {
            return character;
        }
        command = TextSprite_DequeueCode(obj);
        value   = TextSprite_DequeueCode(obj);
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
                TextSprite_SelectFont(obj, obj->fonts, value);
                break;
            case 0x62:
                obj->palette[0] = value;
                break;
            case 0x66:
                obj->palette[1] = value;
                break;
            case 0x61:
                obj->palette[2] = value;
                break;
            case 0x65:
                obj->palette[3] = value;
                break;
        }
    }
}

u16 TextSprite_PeekCode(SpriteTextObject* obj) {
    u16 temp_r4 = obj->queueWrite;
    u16 temp_r5 = obj->queueRead;
    u16 val;
loop_1:
    val = TextSprite_DequeueCode(obj);
    if (val == 0x40) {
        TextSprite_DequeueCode(obj);
        TextSprite_DequeueCode(obj);
        goto loop_1;
    }
    obj->queueWrite = temp_r4;
    obj->queueRead  = temp_r5;
    return val;
}

void* TextSprite_EnqueueString(SpriteTextObject* obj, u8* string) {
    u8* cursor = string;
    u8  byte   = *cursor;
    if (byte != 0) {
        do {
            if (byte & 0x80) {
                TextSprite_EnqueueCode(obj, (u16)(cursor[1] | (byte << 8)));
                cursor += 2;
            } else {
                TextSprite_EnqueueCode(obj, Text_AsciiToSjis(byte));
                cursor += 1;
            }
            byte = *cursor;
        } while (byte != 0);
    }
    return cursor + 1;
}

void TextSprite_Flush(SpriteTextObject* obj) {
    if (TextSprite_IsQueueEmpty(obj)) {
        return;
    }
    do {
        s32 temp_r6 = TextSprite_ReadFormattedCode(obj);
        u16 temp_r0 = TextSprite_PeekCode(obj);
        s32 var_r2  = 0;
        if (!(temp_r0 != 0 && temp_r0 != 0xA)) {
            var_r2 = 1;
        }
        TextSprite_DrawCode(obj, (u32)temp_r6, var_r2);
    } while (!TextSprite_IsQueueEmpty(obj));
}

void TextSprite_SetPalette(SpriteTextObject* obj, u16* palette) {
    s32 index = 0;
    do {
        u16 color                = palette[index];
        obj->palette[index]      = color;
        obj->savedPalette[index] = color;
        index += 1;
    } while (index < 0x10);
    u32 fill = obj->palette[0];
    fill |= fill * 0x10;
    fill |= fill << 8;
    obj->backgroundFill = fill | (fill << 0x10);
}

void TextSprite_Render(SpriteTextObject* obj, s32 x, s32 y, u8* string) {
    TextSprite_Flush(obj);
    obj->xOffset = x;
    obj->yOffset = y;
    TextSprite_EnqueueString(obj, string);
    TextSprite_Flush(obj);
}

void TextSprite_ClearAndResetPosition(SpriteTextObject* obj) {
    s32 var_r0;
    TextSprite_Clear(obj);
    if (obj->vertical == 0) {
        var_r0 = 0;
    } else {
        var_r0 = obj->pixelWidth - (u16) * (u16*)((u8*)obj + 0x54);
    }
    obj->xOffset = var_r0;
    obj->yOffset = 0;
}
