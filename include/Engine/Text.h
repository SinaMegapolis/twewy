#ifndef TEXT_H
#define TEXT_H

#include "Engine/File/DatMgr.h"
#include <nitro/types.h>

typedef struct {
    u16 x;
    u16 y;
} TextU16Pair;

typedef struct {
    /* 0x00 */ char unk_00[0x8];
    /* 0x08 */ u32  unk_08;
    /* 0x0C */ u32  unk_0C;
    /* 0x10 */ u16  unk_10[2];
    /* 0x14 */ u16  unk_14[2];
    /* 0x18 */ u16  unk_18[2];
    /* 0x1C */ u32  unk_1C;
    /* 0x20 */ u32  unk_20;
    /* 0x24 */ u32  unk_24;
    /* 0x28 */ u32  unk_28;
    /* 0x2C */ u16  unk_2C;
    /* 0x2E */ u16  unk_2E;
    /* 0x30 */ u32  unk_30;
} UnkSmallText;

typedef struct {
    /* 0x00 */ u16   unk_00;
    /* 0x02 */ u16   unk_02;
    /* 0x04 */ u32   unk_04;
    /* 0x08 */ u32   unk_08;
    /* 0x0C */ u32   unk_0C;
    /* 0x10 */ u32   unk_10;
    /* 0x14 */ u32   unk_14;
    /* 0x18 */ void* unk_18;
    /* 0x1C */ void* unk_1C;
    /* 0x20 */ u32   unk_20;
} UnkMedText;

typedef struct {
    /* 0x000 */ u32 unk_00;
    /* 0x004 */ u16 unk_04;
    /* 0x006 */ u16 unk_06;
    /* 0x008 */ s32 xOffset;
    /* 0x00C */ s32 yOffset;
    /* 0x010 */ union {
        u16         unk_10[2];
        TextU16Pair spacing;
    };
    /* 0x014 */ union {
        u16         unk_14[2];
        TextU16Pair tileOrigin;
    };
    /* 0x018 */ union {
        u16         unk_18[2];
        TextU16Pair tileSize;
    };
    /* 0x01C */ u16   unk_1C;
    /* 0x01E */ u16   unk_1E;
    /* 0x020 */ u16   unk_20;
    /* 0x022 */ u16   unk_22;
    /* 0x024 */ u32   unk_24;
    /* 0x028 */ u32   unk_28;
    /* 0x02C */ u32   unk_2C;
    /* 0x030 */ u32   unk_30;
    /* 0x034 */ u16   unk_34;
    /* 0x036 */ u16   unk_36;
    /* 0x038 */ u32   unk_38;
    /* 0x03C */ u32   unk_3C;
    /* 0x040 */ Data* data;
    /* 0x044 */ u16   unk_44;
    /* 0x046 */ u16   unk_46;
    /* 0x048 */ u16   unk_48;
    /* 0x04A */ u16   unk_4A;
    /* 0x04C */ union {
        struct {
            u16 unk_4C;
            u16 unk_4E;
        };
        TextU16Pair fontSize;
    };
    /* 0x050 */ u8* unk_50;
    /* 0x054 */ union {
        u32         unk_54;
        TextU16Pair secondaryFontSize;
    };
    /* 0x058 */ u8*  unk_58;
    /* 0x05C */ u32  unk_5C;
    /* 0x060 */ u16  unk_60;
    /* 0x062 */ u16  unk_62;
    /* 0x064 */ u16  unk_64;
    /* 0x066 */ u16  unk_66;
    /* 0x068 */ u16* unk_68;
    /* 0x06C */ u16  unk_6C;
    /* 0x06E */ u16  unk_6E;
    /* 0x070 */ u16* unk_70;
    /* 0x074 */ s32  unk_74;
    /* 0x078 */ s32  unk_78;
    /* 0x07C */ s32  unk_7C;
    /* 0x080 */ s32  unk_80;
    /* 0x084 */ s32  unk_84;
    /* 0x088 */ union {
        s32   unk_88;
        void* primaryRowPixels;
    };
    /* 0x08C */ void* unk_8C;
    /* 0x090 */ union {
        u32   unk_90;
        void* secondaryRowPixels;
    };
    /* 0x094 */ u32  unk_94;
    /* 0x098 */ u32  unk_98;
    /* 0x09C */ u32  unk_9C;
    /* 0x0A0 */ char unk_A0[0x70];
    /* 0x100 */ u32  unk_110;
} TextObject;

void Text_RenderToScreen(TextObject* textObj, s32 x, s32 y, const char* string);

#endif