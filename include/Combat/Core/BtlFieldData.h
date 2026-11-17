#ifndef TWEWY_COMBAT_CORE_BTLFIELDDATA_H
#define TWEWY_COMBAT_CORE_BTLFIELDDATA_H

#include "Battle/Core/BtlBgScript.h"
#include "Engine/File/BinMgr.h"
#include "nitro/types.h"

/// The stage layout of a sub-screen field, in pixels; heights are measured up from the bottom of the 512-pixel
/// background.
typedef struct BtlFieldRectRG {
    /* 0x0 */ s16 floorBottom;   // height of the field's bottom edge
    /* 0x2 */ s16 floorHeight;   // height of the field from its bottom edge to its top edge
    /* 0x4 */ s16 targetOffsetZ; // added to every camera target's height
} BtlFieldRectRG;                // Size: 0x6

/// The stage layout of a main-screen field, in pixels; heights are measured up from the bottom of the 512-pixel
/// background.
typedef struct BtlFieldRect {
    /* 0x0 */ s16 width;         // field width, centred in the background
    /* 0x2 */ s16 floorTopLeft;  // height of the floor's top edge at the field's left edge
    /* 0x4 */ s16 floorTopRight; // height of the floor's top edge at the field's right edge
    /* 0x6 */ s16 viewTop;       // height of the highest point the camera may show
} BtlFieldRect;                  // Size: 0x8

/// A bin identifier with two extra pairs of halfwords (the load parameters of a sprite slot).
typedef struct Ov003BinIdenEx {
    /* 0x0 */ BinIdentifier iden;
    /* 0x8 */ u16           unk_08[4];
} Ov003BinIdenEx; // Size: 0x10

/// One sub-screen field: its background pack, camera rectangle, background scripts and BG layout.
typedef struct BtlFieldDefRG {
    /* 0x00 */ BinIdentifier*             iden;     // pack the background is cut from
    /* 0x04 */ const BtlFieldRectRG*      rect;
    /* 0x08 */ const BattleBgScriptWord** scripts;  // background script of each battle index
    /* 0x0C */ u8                         charPack; // pack indices inside the entry's data
    /* 0x0D */ u8                         rowPack;
    /* 0x0E */ u8                         paletteIndex;
    /* 0x0F */ u8                         palCount;
    /* 0x10 */ u8                         charIndexA;
    /* 0x11 */ u8                         charIndexB;
    /* 0x12 */ u16                        charCountA;
    /* 0x14 */ u16                        charCountB;
    /* 0x16 */ u8                         rowIndex0;
    /* 0x17 */ u8                         rowIndex1;
    /* 0x18 */ u8                         scriptCount; // entries in `scripts`
    /* 0x19 */ u8                         extraIndex;  // 0xFF when the background has no third strip
    /* 0x1A */ u8                         unk_1A[2];
} BtlFieldDefRG;                                       // Size: 0x1C

/// The characters and tilemaps of the third strip.
typedef struct BtlFieldExtra {
    /* 0x0 */ u16 charPack;
    /* 0x2 */ u16 charCount;
    /* 0x4 */ u16 rowIndex0;
    /* 0x6 */ u16 rowIndex1;
} BtlFieldExtra; // Size: 0x8

/// One main-screen field: its background pack, camera rectangle, background scripts and BG layout.
typedef struct BtlFieldDef {
    /* 0x00 */ BinIdentifier*             iden;     // pack the background is cut from
    /* 0x04 */ const BtlFieldRect*        rect;
    /* 0x08 */ const BattleBgScriptWord** scripts;  // background script of each battle index
    /* 0x0C */ u8                         charPack; // pack indices inside the entry's data
    /* 0x0D */ u8                         rowPack;
    /* 0x0E */ u8                         paletteIndex;
    /* 0x0F */ u8                         palCount;
    /* 0x10 */ u8                         charIndexA;
    /* 0x11 */ u8                         charIndexB;
    /* 0x12 */ u16                        charCountA;
    /* 0x14 */ u16                        charCountB;
    /* 0x16 */ u8                         rowIndex0;
    /* 0x17 */ u8                         rowIndex1;
    /* 0x18 */ u8                         rowIndex2;
    /* 0x19 */ u8                         rowIndex3;
    /* 0x1A */ u8                         scriptCount; // entries in `scripts`
    /* 0x1B */ u8                         obsGroup;    // field-object spawn group (`BtlObsSpawn_FieldLists`)
} BtlFieldDef;                                         // Size: 0x1C

// BtlFieldScript_RG.c
extern const BattleBgScriptWord* data_ov003_020e3e64;
extern const BattleBgScriptWord* data_ov003_020e3e68;
extern const BattleBgScriptWord* data_ov003_020e3e6c[13];
extern const BattleBgScriptWord* data_ov003_020e3ea0[13];
extern const BattleBgScriptWord* data_ov003_020e3ed4[14];

// BtlFieldScript_UG.c
extern const BattleBgScriptWord* data_ov003_020e3f0c[1];
extern const BattleBgScriptWord* data_ov003_020e3f10[2];

// BtlFieldExtra.c
extern const BtlFieldExtra BtlField_RG_Extras[3];

// BtlFieldDef_RG.c
extern const BtlFieldDefRG BtlField_RG_Defs[41];

// BtlFieldDef_UG.c
extern const BinIdentifier data_ov003_020da114;
extern const BinIdentifier data_ov003_020da124;
extern const BinIdentifier data_ov003_020da154;
extern const BinIdentifier data_ov003_020da16c;
extern const BinIdentifier data_ov003_020da174;
extern const BinIdentifier data_ov003_020da1ac;
extern const BinIdentifier data_ov003_020da1b4;
extern const BtlFieldDef   BtlField_UG_Defs[41];

#endif // TWEWY_COMBAT_CORE_BTLFIELDDATA_H
