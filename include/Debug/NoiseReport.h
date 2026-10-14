#ifndef DEBUG_NOISEREPORT_H
#define DEBUG_NOISEREPORT_H

#include "Display.h"
#include "Engine/EasyTask.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Resources/ResourceMgr.h"
#include "SpriteMgr.h"
#include "Util/SysFont.h"
#include "common_data.h"

typedef struct {
    /* 0x00 */ s32 exitFlag;
    /* 0x04 */ s32 helpOpen;
    /* 0x08 */ s16 helpPage;
    /* 0x0A */ s16 dropRateMode;
    /* 0x0C */ s16 scrollRow;
    /* 0x0E */ s16 selectedNoise;
} NoiseReportPending; // Size: 0x10

typedef struct {
    /* 0x00 */ s32                unk_00;
    /* 0x04 */ TaskPool*          pool;
    /* 0x08 */ s32                unk_08;
    /* 0x0C */ s32                unk_0C;
    /* 0x10 */ s16                unk_10;
    /* 0x12 */ s16                unk_12;
    /* 0x14 */ s16                unk_14;
    /* 0x16 */ s16                unk_16;
    /* 0x18 */ NoiseReportPending pending;
    /* 0x28 */ u16                unk_28;
    /* 0x2A */ u16                unk_2A;
    /* 0x2C */ s32                unk_2C;
    /* 0x30 */ TouchCoord         touchCoord;
    /* 0x38 */ s16                unk_38;
    /* 0x3A */ s16                unk_3A;
} NoiseReport; // Size: 0x3C

typedef struct {
    /* 0x00000 */ ResourceManager  unk_00000;
    /* 0x11580 */ ResourceManager* unk_11580;
    /* 0x11584 */ Heap             heap;
    /* 0x11590 */ MemBlock         memBlock;
    /* 0x115B0 */ char             unk_115B0[0xFFE0];
    /* 0x21590 */ TaskPool         taskPool;
    /* 0x21610 */ NoiseReport      unk_21610;
    /* 0x2164C */ u16              unk_2164C;
    /* 0x2164E */ char             unk_2164E[2];
} NoiseReportState; // Size: 0x21650

typedef struct {
    /* 0x00 */ Data*            data;
    /* 0x04 */ BgResource*      charRes;
    /* 0x08 */ BgResource*      screenRes;
    /* 0x0C */ PaletteResource* palRes;
    /* 0x10 */ DisplayEngine    engine;
    /* 0x14 */ DisplayBGLayer   layer;
    /* 0x18 */ s32              hOffset;
    /* 0x1C */ s32              vOffset;
    /* 0x20 */ s32              visible;
    /* 0x24 */ s32              priority;
    /* 0x28 */ s32              blendCoeff0;
    /* 0x2C */ s32              blendCoeff1;
    /* 0x30 */ s32              blendLayer1;
} NrepBgLayer; // Size: 0x34

typedef struct {
    /* 0x00 */ const BinIdentifier* binIden;
    /* 0x04 */ s32                  packIndex;
    /* 0x08 */ s32                  charEntry;
    /* 0x0C */ s32                  screenEntry;
    /* 0x10 */ s32                  palEntry;
    /* 0x14 */ u32                  charSize;
    /* 0x18 */ s32                  palStart;
    /* 0x1C */ u32                  palCount;
    /* 0x20 */ DisplayEngine        engine;
    /* 0x24 */ DisplayBGLayer       layer;
    /* 0x28 */ s32                  priority;
} NrepBgConfig; // Size: 0x2C

typedef struct {
    /* 0x00 */ NrepBgLayer layers[4];
} NrepBG; // Size: 0xD0

typedef struct {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u8  color;
    /* 0x05 */ u8  pad[3];
    /* 0x08 */ s32 msgId;
} NrepFontItemConfig; // Size: 0x0C

typedef struct {
    /* 0x000 */ u16*         screenBuf[2];
    /* 0x008 */ u16*         charBuf[2];
    /* 0x010 */ NrepBgLayer bgLayers[2];
    /* 0x078 */ SysFont     lowerFonts[20];
    /* 0xA28 */ SysFont     upperFonts[6];
    /* 0xD10 */ s32         prevHelpOpen;
    /* 0xD14 */ s16         prevDropRateMode;
    /* 0xD16 */ s16         prevSelectedNoise;
    /* 0xD18 */ s32         prevHelpPage;
} NrepFont; // Size: 0xD1C

typedef struct {
    /* 0x00 */ s16 width;
    /* 0x02 */ s16 height;
    /* 0x04 */ s16 boxX[2];
    /* 0x08 */ s16 boxY[2];
    /* 0x0C */ u16 pad[2];
} NrepIconLayout; // Size: 0x14

typedef struct {
    /* 0x00 */ Sprite sprites[2];
    /* 0x80 */ u8     pressed[2];
    /* 0x82 */ s16    pressTimer;
} NrepIcon; // Size: 0x84

typedef struct {
    /* 0x00 */ s16 animIndex;
    /* 0x02 */ s16 posX;
    /* 0x04 */ s16 posY;
    /* 0x06 */ s16 hitOffsetX;
    /* 0x08 */ s16 hitOffsetY;
    /* 0x0A */ s16 hitWidth;
    /* 0x0C */ s16 hitHeight;
} NrepInterDHitbox; // Size: 0x0E

typedef struct {
    /* 0x000 */ Sprite sprites[13];
    /* 0x340 */ u32    draggingScroll;
    /* 0x344 */ s16    scrollDragOffset;
    /* 0x346 */ s16    helpBtnTimer;
    /* 0x348 */ s16    closingHelp;
    /* 0x34A */ s16    scrollRow;
    /* 0x34C */ s16    cursorCol;
    /* 0x34E */ s16    selectedNoise;
    /* 0x350 */ s16    keyRepeatTimer;
    /* 0x352 */ s16    scrollKnobY;
    /* 0x354 */ u32    touchActive;
} NrepInterD; // Size: 0x358

typedef struct {
    /* 0x00 */ s32 animIndex;
    /* 0x04 */ s16 posX;
    /* 0x06 */ s16 posY;
} NrepInterUEntry; // Size: 0x08

typedef struct {
    /* 0x000 */ Sprite sprites[15];
    /* 0x3C0 */ s32    unk_3C0;
} NrepInterU; // Size: 0x3C4

typedef struct {
    /* 0x000 */ s32    unk_000;
    /* 0x004 */ Sprite unk_004;
    /* 0x044 */ Sprite unk_044;
    /* 0x084 */ Sprite sprites[29]; // stat digits, body icons, and the boss-time banner
    /* 0x7C4 */ s16    unk_7C4;
    /* 0x7C6 */ char   unk_7C6[2];
} NrepPreview; // Size: 0x7C8

typedef struct {
    /* 0x000 */ s32    unk_000;
    /* 0x004 */ Sprite unk_004;
    /* 0x044 */ Sprite unk_044;
    /* 0x084 */ Sprite unk_084;
    /* 0x0C4 */ Sprite unk_0C4;
    /* 0x104 */ Sprite unk_104;
    /* 0x144 */ Sprite unk_144;
    /* 0x184 */ Sprite unk_184;
    /* 0x1C4 */ Sprite unk_1C4;
    /* 0x204 */ Sprite unk_204;
} NrepSeal; // Size: 0x244

typedef struct {
    /* 0x000 */ NrepSeal seals[24];
    /* 0x3660 */ s16      loadedRow;
    /* 0x3662 */ s16      unk_3662;
} NrepSealTaskData; // Size: 0x3664

typedef struct {
    /* 0x00 */ s8  killCount;
    /* 0x01 */ s8  dropFlags;
    /* 0x02 */ s16 seenFlag;
    /* 0x04 */ u32 bestTime;
} NrepNoiseRecord; // Size: 0x8

typedef struct {
    /* 0x00 */ u8  unk_00[7];
    /* 0x07 */ u8  taboo;
    /* 0x08 */ u16 yen;
    /* 0x0A */ s16 hp;
    /* 0x0C */ s16 atk;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 resistPos;
    /* 0x12 */ s16 resistNeg;
    /* 0x14 */ u8  unk_14[0x18];
    /* 0x2C */ s16 drops[4];
    /* 0x34 */ s16 dropRates[4];
} NrepEnemyData; // Size: 0x3C

#endif      // DEBUG_NOISEREPORT_H