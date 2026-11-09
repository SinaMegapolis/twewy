#ifndef COMBAT_COMBATBGANIMSLOT_H
#define COMBAT_COMBATBGANIMSLOT_H

#include "Combat/Core/Combat.h"

/// One frame of an animated BG tile slot. A frame with `entry` -2 is followed by a frame naming the
/// palette to switch to; both are consumed in one step.
typedef struct Ov003BgAnimFrame {
    /* 0x00 */ u16 dataId;   // pack index of this frame's BG tile (or, after a -2 marker, palette) data
    /* 0x02 */ s16 entry;    // entry within that pack; -1: end of animation, -2: palette switch marker
    /* 0x04 */ s16 duration; // frames to show the tiles
} Ov003BgAnimFrame;          // Size: 0x6

/// An animated block of BG tiles: each frame's tiles are copied over the same BG char VRAM range.
typedef struct Ov003BgAnimSlot {
    /* 0x00 */ BinIdentifier*          charIden; // pack the tile data comes from
    /* 0x04 */ BinIdentifier*          palIden;  // pack the palette data comes from
    /* 0x08 */ Data*                   charData; // pack entry of the frame currently shown
    /* 0x0C */ Data*                   paletteData;
    /* 0x10 */ PaletteResource*        palette;
    /* 0x14 */ void*                   charDest; // BG char VRAM the frames are copied to
    /* 0x18 */ const Ov003BgAnimFrame* anims;    // NULL = slot unused
    /* 0x1C */ s16                     dataId;   // pack index of the tiles currently loaded, -1 = none
    /* 0x1E */ s16                     animIndex;
    /* 0x20 */ s16                     timer;    // frames left on the current frame
    union {
        /* 0x22 */ struct {
            u16 palEngine : 2; // display engine the palette is allocated on
            u16 palStart  : 4; // first palette slot
            u16 palCount  : 4; // slots to allocate
            u16 _pad22    : 6;
        };
        u16 palFlags;
    };
    union {
        /* 0x24 */ struct {
            u16 loop     : 1; // restart at the end of the animation
            u16 disabled : 1; // never advances or uploads anything
            u16 hasPal   : 1; // the slot has its own palette, reloaded on palette switch markers
            u16 finished : 1; // animation reached its end marker
            u16 _pad24   : 12;
        };
        u16 flags;
    };
} Ov003BgAnimSlot; // Size: 0x28

#endif
