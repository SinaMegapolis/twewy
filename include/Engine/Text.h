#ifndef TEXT_H
#define TEXT_H

#include "Engine/File/DatMgr.h"
#include <nitro/types.h>

typedef struct {
    u16 x;
    u16 y;
} TextU16Pair;

// Background text prefix. Enclosing users provide storage through offset 0x350:
// 256 queued u16 codes at 0x110, followed by two 4bpp tiles at 0x310.
// Keep this prefix at 0x114 until those enclosing layouts are fully recovered.
typedef struct {
    /* 0x000 */ u32         vertical;
    /* 0x004 */ u16         mapWidth;
    /* 0x006 */ u16         mapHeight;
    /* 0x008 */ s32         xOffset;
    /* 0x00C */ s32         yOffset;
    /* 0x010 */ TextU16Pair spacing;
    /* 0x014 */ TextU16Pair tileOrigin;
    /* 0x018 */ TextU16Pair tileSize;
    /* 0x01C */ u16         pixelOriginX;
    /* 0x01E */ u16         pixelOriginY;
    /* 0x020 */ u16         pixelWidth;
    /* 0x022 */ u16         pixelHeight;
    /* 0x024 */ u32         pixels;
    /* 0x028 */ u32         tilemap;
    /* 0x02C */ u32         blankGlyph;
    /* 0x030 */ u32         backgroundFill;
    /* 0x034 */ u16         blankTile;
    /* 0x036 */ u16         firstTile;
    /* 0x038 */ u32         fonts;
    /* 0x03C */ u32         datSlot;
    /* 0x040 */ Data*       data;
    /* 0x044 */ u16         bitsPerPixel;
    /* 0x046 */ u16         glyphStrideBytes;
    /* 0x048 */ u16         glyphTileWidth;
    /* 0x04A */ u16         glyphTileHeight;
    /* 0x04C */ union {
        struct {
            u16 glyphWidth;
            u16 glyphHeight;
        };
        TextU16Pair fontSize;
    };
    /* 0x050 */ u8*   codeMap;
    /* 0x054 */ u32   bitmap;
    /* 0x058 */ u8*   advanceWidths;
    /* 0x05C */ u32   advanceWidthCount;
    /* 0x060 */ u16   defaultAdvance;
    /* 0x062 */ u16   padding62;
    /* 0x064 */ u16   queueWrite;
    /* 0x066 */ u16   queueRead;
    /* 0x068 */ u16*  queue;
    /* 0x06C */ u16   blitReserved;
    /* 0x06E */ u16   blitBitsPerPixel;
    /* 0x070 */ u16*  blitMode;
    /* 0x074 */ s32   sourceRowBytes;
    /* 0x078 */ s32   destinationX;
    /* 0x07C */ s32   blitWidth;
    /* 0x080 */ s32   blitHeight;
    /* 0x084 */ s32   sourcePixels;
    /* 0x088 */ void* primaryRowPixels;
    /* 0x08C */ void* blitPalette;
    /* 0x090 */ u32   palette[16];
    /* 0x0D0 */ u32   savedPalette[16];
    /* 0x110 */ u32   queueStorage;
} TextObject;

void Text_ClearAndResetPosition(TextObject* obj);
void Text_ClearRect(TextObject* obj, s32 x, s32 y, s32 width, s32 height);
void Text_RenderToScreen(TextObject* textObj, s32 x, s32 y, const char* string);

#endif