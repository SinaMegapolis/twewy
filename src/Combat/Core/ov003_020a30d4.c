#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatBgRes.h"
#include "Combat/Core/CombatSprite.h"
#include "Combat/Core/CombatTouch.h"
#include "EasyFade.h"
#include "Engine/Color.h"
#include "Engine/Core/DMA.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/Input.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ResourceMgr.h"
#include "Save.h"
#include "common_data.h"

#include <cri/adxt.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi/cpumem.h>

// The touch-action side of the battle UI: the 0x30-byte touch event queue, the touch action
// actors linked into `Ov003Global + 0x3D8BC` and the pin/psych bookkeeping that feeds them.
// The actors themselves are created by `ov003_020a3b9c.c`.

// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------

/// The 0x24-byte record `func_020274ac` loads out of `Data/AbilityData.bin` for one ability id.
/// Byte 0 of `flags` says whether the ability is usable by any touch at all, bytes 1..3 are the
/// per-touch usability flags; the four slots are the touch actions the ability grants.

/// One touch action an ability grants: the action id and the amount it contributes.

/// One queued touch event; the 14-entry pool sits at `Ov003Global + 0x3D4BC`.

/// The 6-byte rendering of the six touch slots that the 3D layer reads back.

/// The per-touch start positions written by `func_ov003_020a39dc` (owned by arm9).
extern const u8 data_0205c19d[];
/// Task handle of the touch icon task (owned by the battle setup unit).
extern const TaskHandle data_ov003_020d7d24;

// ---------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------

/// A word of a touch-action resource script: an entry point or one of its in-line operands. The
/// retail symbol table splits these scripts at non-record boundaries, so they stay raw words.
typedef union Ov003ScriptWord {
    u32   value;
    void* ptr;
    s32 (*run)(void);
} Ov003ScriptWord; // Size: 4

/// An eight-byte sprite animation record of the touch resource scripts.
typedef struct Ov003AnimRecord {
    /* 0x0 */ u8  unk_00;
    /* 0x2 */ u16 unk_02;
    /* 0x4 */ s16 unk_04;
} Ov003AnimRecord; // Size: 0x6

/// One record of the per-touch icon table at `data_ov003_020da80c`, indexed by the pack id that
/// `func_ov003_020a3498` receives. Fields 0x04..0x16 feed a `SpriteAnimationEx`.
typedef struct Ov003TouchIconParams {
    /* 0x00 */ const BinIdentifier* binIden;
    /* 0x04 */ u16                  unk_04;
    /* 0x06 */ s16                  unk_06;
    /* 0x08 */ SpriteAnimEntry      entry;
    /* 0x10 */ u16                  unk_10;
    /* 0x12 */ u16                  unk_12;
    /* 0x14 */ u16                  unk_14;
    /* 0x16 */ u16                  unk_16;
} Ov003TouchIconParams; // Size: 0x18

/// One touch target rectangle, in 0x200-based screen units. Same layout as `Ov003CameraPathPos`
/// in `ov003_02083300.c`; both should move into a shared header.
typedef struct Ov003PathRect {
    /* 0x0 */ s16 unk_00;
    /* 0x2 */ s16 unk_02;
    /* 0x4 */ s16 unk_04;
    /* 0x6 */ s16 unk_06;
} Ov003PathRect; // Size: 0x8

/// One entry of a camera path table: the target rectangle, an optional extra table, the sprite
/// parameters and the pack the sprite comes from.
typedef struct __attribute__((packed)) Ov003TouchPathEntry {
    /* 0x00 */ const void* pos;
    /* 0x04 */ const void* anims;
    /* 0x08 */ u8          unk_08[0x10];
    /* 0x18 */ const void* iden;
} Ov003TouchPathEntry; // Size: 0x1C

/// The sub-engine camera path table. Its last record ends with an in-line copy of a bin
/// identifier, so the id word lands in the table and only the path pointer trails it.
typedef struct Ov003SubCameraPathTable {
    /* 0x000 */ Ov003TouchPathEntry entries[40];
    /* 0x460 */ const void*         lastPos;
    /* 0x464 */ const void*         lastAnims;
    /* 0x468 */ u8                  lastData[0x10];
    /* 0x478 */ u32                 lastIdenType;
    /* 0x47C */ const char*         lastIdenPath;
} Ov003SubCameraPathTable; // Size: 0x480

/// The main camera path table is split by the retail symbol table 0x17 bytes into its first
/// record; these two objects are the two halves of the same `Ov003TouchPathEntry[41]` table.
typedef struct __attribute__((packed)) Ov003CameraPathHead {
    /* 0x00 */ const void* pos;
    /* 0x04 */ u8          data[0x13];
} Ov003CameraPathHead; // Size: 0x17

typedef struct __attribute__((packed)) Ov003CameraPathTail {
    /* 0x000 */ u8                   lastData;
    /* 0x001 */ const BinIdentifier* firstIden;
    /* 0x005 */ Ov003TouchPathEntry  entries[40];
} Ov003CameraPathTail; // Size: 0x465

/// A bin identifier with two extra pairs of halfwords (the load parameters of a sprite slot).
typedef struct Ov003BinIdenEx {
    /* 0x0 */ BinIdentifier iden;
    /* 0x8 */ u16           unk_08[4];
} Ov003BinIdenEx; // Size: 0x10

extern const Ov003ScriptWord         data_ov003_020d9a80[4];
extern const Ov003ScriptWord         data_ov003_020d9a90[6];
extern const Ov003ScriptWord         data_ov003_020d9aa8[10];
static const u8                      data_ov003_020d9ad0[0x2C];
static void                          func_ov003_020a3924(void);
extern const u8                      data_ov003_020d9afc[0x18];
extern const void* const             data_ov003_020d9c90[1];
extern const Ov003SubCameraPathTable data_ov003_020d9c94;
extern const BinIdentifier           data_ov003_020da114;
extern const BinIdentifier           data_ov003_020da124;
extern const BinIdentifier           data_ov003_020da154;
extern const BinIdentifier           data_ov003_020da16c;
extern const BinIdentifier           data_ov003_020da174;
extern const BinIdentifier           data_ov003_020da1ac;
extern const BinIdentifier           data_ov003_020da1b4;
extern const void* const             data_ov003_020da2bc[1];
extern const Ov003TouchPathEntry     data_ov003_020da2c0[41];
extern;
extern const BinIdentifier data_ov003_020da744;
static const u16           data_ov003_020d8c96[];
static const u32           data_ov003_020d91d4[];
static const u32           data_ov003_020d9210[];
static const u32           data_ov003_020d924c[];
static const u32           data_ov003_020d9288[];
static const u32           data_ov003_020d92c4[];
static const u32           data_ov003_020d9300[];
static const u32           data_ov003_020d933c[];
static const u32           data_ov003_020d9378[];
static const u32           data_ov003_020d93b4[];
static const u32           data_ov003_020d93f0[];
static const u32           data_ov003_020d942c[];
static const u32           data_ov003_020d9468[];
static const u32           data_ov003_020d94a4[];
static const u32           data_ov003_020d94e0[];
static const u32           data_ov003_020d951c[];
static const u32           data_ov003_020d9558[];
static const u32           data_ov003_020d9594[];
static const u32           data_ov003_020d95d0[];
static const u32           data_ov003_020d960c[];
static const u32           data_ov003_020d9648[];
static const u32           data_ov003_020d9684[];
static const u32           data_ov003_020d96c0[];
static const u32           data_ov003_020d96fc[];
static const u32           data_ov003_020d9738[];
static const u32           data_ov003_020d9774[];
static const u32           data_ov003_020d97b0[];
static const u32           data_ov003_020d97ec[];
static const u32           data_ov003_020d9828[];
static const u32           data_ov003_020d9864[];
static const u32           data_ov003_020d98a0[];
static const u32           data_ov003_020d98dc[];
static const u32           data_ov003_020d9918[];
static const u32           data_ov003_020d9954[];
static const u32           data_ov003_020d9990[];
static const u32           data_ov003_020d99cc[];
static const u32           data_ov003_020d9a08[];
static const u32           data_ov003_020d9a44[];

// Background resource scripts and the file names they are read from (`BtlField_RG`/`BtlField_UG` resource tables).
const u32        data_ov003_020d889c[4]  = {(u32)func_ov003_02083478, 0x00030001, 0x000B0004, (u32)func_ov003_02083470};
static const u32 data_ov003_020d88ac[5]  = {0x00010041, 0x00420040, 0x00400001, 0xFFFF0000, 0};
const u32        data_ov003_020d88c0[6]  = {(u32)func_ov003_02083700, (u32)data_ov003_020d9558, 1, 0x01900000, 0x000C0003,
                                            (u32)func_ov003_02083470};
static const u32 data_ov003_020d88d8[6]  = {0x0001002A, 0x002B0020, 0x00200001, 0x0001002C, 0x00000020, 0x0000FFFF};
static const u32 data_ov003_020d88f0[6]  = {0x00010036, 0x00370040, 0x00080001, 0x00010038, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d8908[6]  = {0x0001000C, 0x000D0020, 0x00200001, 0x0001000E, 0x00000080, 0x0000FFFF};
static const u32 data_ov003_020d8920[6]  = {0x0001003C, 0x003D0040, 0x00400001, 0x0001003E, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d8938[6]  = {0x00010046, 0x0047000C, 0x000C0001, 0x00010048, 0x0000000C, 0x0000FFFF};
static const u32 data_ov003_020d8950[6]  = {0x0001004C, 0x004D000A, 0x000A0001, 0x0001004E, 0x0000000A, 0x0000FFFF};
static const u32 data_ov003_020d8968[6]  = {0x00010012, 0x00130008, 0x00080001, 0x00010014, 8, 0x0000FFFF};
static const u32 data_ov003_020d8980[6]  = {0x00010018, 0x00190060, 0x00600001, 0x0001001A, 0x00000060, 0x0000FFFF};
const u32        data_ov003_020d8998[6]  = {(u32)func_ov003_02083700, (u32)data_ov003_020d8c96, 2, 0x01800000, 0x000C0003,
                                            (u32)func_ov003_02083470};
static const u32 data_ov003_020d89b0[6]  = {0x00010024, 0x00250010, 0x00100001, 0x00010026, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d89c8[6]  = {0x0001001E, 0x001F0010, 0x00100001, 0x00010020, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d89e0[6]  = {0x00010004, 0x00060008, 0x00080001, 0x00010008, 8, 0x0000FFFF};
static const u32 data_ov003_020d89f8[6]  = {0x00010030, 0x00310040, 0x00400001, 0x00010032, 0x00000140, 0x0000FFFF};
const u32        data_ov003_020d8a10[10] = {
    (u32)func_ov003_02083478, 0x00060003, 0x000B0004, (u32)func_ov003_020835d4, 0x00060001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00060002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8a38[10] = {
    (u32)func_ov003_02083478, 0x00080003, 0x000B0004, (u32)func_ov003_020835d4, 0x00080001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00080002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8a60[10] = {
    (u32)func_ov003_02083478, 0x000E0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000E0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000E0002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8a88[10] = {
    (u32)func_ov003_02083478, 0x000D0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000D0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000D0002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8ab0[10] = {
    (u32)func_ov003_02083478, 0x000C0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000C0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000C0002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8ad8[10] = {
    (u32)func_ov003_02083478, 0x000F0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000F0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000F0002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8b00[10] = {
    (u32)func_ov003_02083478, 0x000A0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000A0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000A0002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8b28[10] = {
    (u32)func_ov003_02083478, 0x00090003, 0x000B0004, (u32)func_ov003_020835d4, 0x00090001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00090002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8b50[10] = {
    (u32)func_ov003_02083478, 0x00070003, 0x000B0004, (u32)func_ov003_020835d4, 0x00070001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00070002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8b78[10] = {
    (u32)func_ov003_02083478, 0x00100003, 0x000B0004, (u32)func_ov003_020835d4, 0x00100001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00100002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8ba0[10] = {
    (u32)func_ov003_02083478, 0x00040003, 0x000B0004, (u32)func_ov003_020835d4, 0x00040001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00040002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8bc8[10] = {
    (u32)func_ov003_02083478, 0x00050003, 0x000B0004, (u32)func_ov003_020835d4, 0x00050001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x00050002, 0x01A00002, (u32)func_ov003_02083470};
const u32 data_ov003_020d8bf0[10] = {
    (u32)func_ov003_02083478, 0x000B0003, 0x000B0004, (u32)func_ov003_020835d4, 0x000B0001, 0x01A00001,
    (u32)func_ov003_020835d4, 0x000B0002, 0x01A00002, (u32)func_ov003_02083470};
static const u16 data_ov003_020d8c18[21] = {0, 0xFFFE, 0, 0x41, 2,    0, 0x41, 1, 0x40,   0, 0xFFFE,
                                            0, 0x42,   2, 0,    0x42, 1, 0x40, 0, 0xFFFF, 0};
static const u16 data_ov003_020d8c42[21] = {0, 0xFFFE, 0, 0x3F, 2,    0, 0x3F, 1, 0x40,   0, 0xFFFE,
                                            0, 0x40,   2, 0,    0x40, 1, 0x40, 0, 0xFFFF, 0};
static const u16 data_ov003_020d8c6c[21] = {0, 0xFFFE, 0, 0x3F, 2,    0, 0x3F, 1, 0x40,   0, 0xFFFE,
                                            0, 0x40,   2, 0,    0x40, 1, 0x40, 0, 0xFFFF, 0};
static const u16 data_ov003_020d8c96[21] = {0, 0xFFFE, 0, 3, 2, 0, 3,    1, 0x40,   0, 0xFFFE,
                                            0, 4,      2, 0, 4, 1, 0x40, 0, 0xFFFF, 0};
const u32        data_ov003_020d8cc0[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d9918, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9954, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8cec[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d97b0, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d97ec, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8d18[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d9990, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d99cc, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8d44[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d93b4, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d93f0, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8d70[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d9a08, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9a44, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8d9c[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d942c, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d94a4, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8dc8[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d91d4, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9210, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8df4[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d92c4, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9300, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8e20[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d924c, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9288, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8e4c[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d933c, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9378, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8e78[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d9828, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d9864, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8ea4[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d98a0, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d98dc, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8ed0[11] = {(u32)func_ov003_02083700, (u32)data_ov003_020d8c42, 1, 0x01800000, 0x000A0002,
                                            (u32)func_ov003_02083700, (u32)data_ov003_020d8c18, 2, 0x01A00001, 0x000C0003,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8efc[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d94e0,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8908,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8f34[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d951c,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8968,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8f6c[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9594,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8980,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8fa4[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d95d0,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d89c8,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d8fdc[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d960c,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d89b0,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d9014[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9648,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d88d8,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d904c[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9684,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d89f8,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d9084[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d96c0,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d88f0,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d90bc[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d96fc,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8920,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d90f4[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8c6c,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d88ac,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d912c[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9468,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d89e0,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d9164[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9738,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8938,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
const u32        data_ov003_020d919c[14] = {(u32)func_ov003_02083478,
                                            0x004F0001,
                                            0x000E0001,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d9774,
                                            1,
                                            0x01800000,
                                            0x000B0003,
                                            (u32)func_ov003_02083700,
                                            (u32)data_ov003_020d8950,
                                            2,
                                            0x01800001,
                                            0,
                                            (u32)func_ov003_02083470};
static const u32 data_ov003_020d91d4[15] = {0xFFFE0000, 0x00270000, 2,          0x00010027, 0x00000020,
                                            0x0000FFFE, 0x00020028, 0x00280000, 0x00200001, 0xFFFE0000,
                                            0x00290000, 2,          0x00010029, 0x00000020, 0x0000FFFF};
static const u32 data_ov003_020d9210[15] = {0xFFFE0000, 0x002A0000, 2,          0x0001002A, 0x00000020,
                                            0x0000FFFE, 0x0002002B, 0x002B0000, 0x00200001, 0xFFFE0000,
                                            0x002C0000, 2,          0x0001002C, 0x00000020, 0x0000FFFF};
static const u32 data_ov003_020d924c[15] = {0xFFFE0000, 0x002D0000, 2,          0x0001002D, 0x00000040,
                                            0x0000FFFE, 0x0002002E, 0x002E0000, 0x00400001, 0xFFFE0000,
                                            0x002F0000, 2,          0x0001002F, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d9288[15] = {0xFFFE0000, 0x00300000, 2,          0x00010030, 0x00000040,
                                            0x0000FFFE, 0x00020031, 0x00310000, 0x00400001, 0xFFFE0000,
                                            0x00320000, 2,          0x00010032, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d92c4[15] = {0xFFFE0000, 0x00330000, 2,          0x00010033, 0x00000040,
                                            0x0000FFFE, 0x00020034, 0x00340000, 0x00080001, 0xFFFE0000,
                                            0x00350000, 2,          0x00010035, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d9300[15] = {0xFFFE0000, 0x00360000, 2,          0x00010036, 0x00000040,
                                            0x0000FFFE, 0x00020037, 0x00370000, 0x00080001, 0xFFFE0000,
                                            0x00380000, 2,          0x00010038, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d933c[15] = {0xFFFE0000, 0x00390000, 2,          0x00010039, 0x00000040,
                                            0x0000FFFE, 0x0002003A, 0x003A0000, 0x00400001, 0xFFFE0000,
                                            0x003B0000, 2,          0x0001003B, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d9378[15] = {0xFFFE0000, 0x003C0000, 2,          0x0001003C, 0x00000040,
                                            0x0000FFFE, 0x0002003D, 0x003D0000, 0x00400001, 0xFFFE0000,
                                            0x003E0000, 2,          0x0001003E, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d93b4[15] = {0xFFFE0000, 0x00430000, 2,          0x00010043, 0x0000000C,
                                            0x0000FFFE, 0x00020044, 0x00440000, 0x000C0001, 0xFFFE0000,
                                            0x00450000, 2,          0x00010045, 0x0000000C, 0x0000FFFF};
static const u32 data_ov003_020d93f0[15] = {0xFFFE0000, 0x00460000, 2,          0x00010046, 0x0000000C,
                                            0x0000FFFE, 0x00020047, 0x00470000, 0x000C0001, 0xFFFE0000,
                                            0x00480000, 2,          0x00010048, 0x0000000C, 0x0000FFFF};
static const u32 data_ov003_020d942c[15] = {0xFFFE0000, 0x00490000, 2,          0x00010049, 0x0000000A,
                                            0x0000FFFE, 0x0002004A, 0x004A0000, 0x000A0001, 0xFFFE0000,
                                            0x004B0000, 2,          0x0001004B, 0x0000000A, 0x0000FFFF};
static const u32 data_ov003_020d9468[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 8,
                                            0x0000FFFE, 0x00020005, 0x00050000, 0x00080001, 0xFFFE0000,
                                            0x00070000, 2,          0x00010007, 8,          0x0000FFFF};
static const u32 data_ov003_020d94a4[15] = {0xFFFE0000, 0x004C0000, 2,          0x0001004C, 0x0000000A,
                                            0x0000FFFE, 0x0002004D, 0x004D0000, 0x000A0001, 0xFFFE0000,
                                            0x004E0000, 2,          0x0001004E, 0x0000000A, 0x0000FFFF};
static const u32 data_ov003_020d94e0[15] = {0xFFFE0000, 0x00090000, 2,          0x00010009, 0x00000020,
                                            0x0000FFFE, 0x0002000A, 0x000A0000, 0x00200001, 0xFFFE0000,
                                            0x000B0000, 2,          0x0001000B, 0x00000080, 0x0000FFFF};
static const u32 data_ov003_020d951c[15] = {0xFFFE0000, 0x000F0000, 2,          0x0001000F, 8,
                                            0x0000FFFE, 0x00020010, 0x00100000, 0x00080001, 0xFFFE0000,
                                            0x00110000, 2,          0x00010011, 8,          0x0000FFFF};
static const u32 data_ov003_020d9558[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 0x0000000A,
                                            0x0000FFFE, 0x00020004, 0x00040000, 0x000A0001, 0xFFFE0000,
                                            0x00050000, 2,          0x00010005, 0x0000000A, 0x0000FFFF};
static const u32 data_ov003_020d9594[15] = {0xFFFE0000, 0x00150000, 2,          0x00010015, 0x00000060,
                                            0x0000FFFE, 0x00020016, 0x00160000, 0x00600001, 0xFFFE0000,
                                            0x00170000, 2,          0x00010017, 0x00000060, 0x0000FFFF};
static const u32 data_ov003_020d95d0[15] = {0xFFFE0000, 0x001B0000, 2,          0x0001001B, 0x00000010,
                                            0x0000FFFE, 0x0002001C, 0x001C0000, 0x00100001, 0xFFFE0000,
                                            0x001D0000, 2,          0x0001001D, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d960c[15] = {0xFFFE0000, 0x00210000, 2,          0x00010021, 0x00000010,
                                            0x0000FFFE, 0x00020022, 0x00220000, 0x00100001, 0xFFFE0000,
                                            0x00230000, 2,          0x00010023, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d9648[15] = {0xFFFE0000, 0x00270000, 2,          0x00010027, 0x00000020,
                                            0x0000FFFE, 0x00020028, 0x00280000, 0x00200001, 0xFFFE0000,
                                            0x00290000, 2,          0x00010029, 0x00000020, 0x0000FFFF};
static const u32 data_ov003_020d9684[15] = {0xFFFE0000, 0x002D0000, 2,          0x0001002D, 0x00000040,
                                            0x0000FFFE, 0x0002002E, 0x002E0000, 0x00400001, 0xFFFE0000,
                                            0x002F0000, 2,          0x0001002F, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d96c0[15] = {0xFFFE0000, 0x00330000, 2,          0x00010033, 0x00000040,
                                            0x0000FFFE, 0x00020034, 0x00340000, 0x00080001, 0xFFFE0000,
                                            0x00350000, 2,          0x00010035, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d96fc[15] = {0xFFFE0000, 0x00390000, 2,          0x00010039, 0x00000040,
                                            0x0000FFFE, 0x0002003A, 0x003A0000, 0x00400001, 0xFFFE0000,
                                            0x003B0000, 2,          0x0001003B, 0x00000140, 0x0000FFFF};
static const u32 data_ov003_020d9738[15] = {0xFFFE0000, 0x00430000, 2,          0x00010043, 0x0000000C,
                                            0x0000FFFE, 0x00020044, 0x00440000, 0x000C0001, 0xFFFE0000,
                                            0x00450000, 2,          0x00010045, 0x0000000C, 0x0000FFFF};
static const u32 data_ov003_020d9774[15] = {0xFFFE0000, 0x00490000, 2,          0x00010049, 0x0000000A,
                                            0x0000FFFE, 0x0002004A, 0x004A0000, 0x000A0001, 0xFFFE0000,
                                            0x004B0000, 2,          0x0001004B, 0x0000000A, 0x0000FFFF};
static const u32 data_ov003_020d97b0[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 8,
                                            0x0000FFFE, 0x00020004, 0x00040000, 0x00080001, 0xFFFE0000,
                                            0x00050000, 2,          0x00010005, 8,          0x0000FFFF};
static const u32 data_ov003_020d97ec[15] = {0xFFFE0000, 0x00060000, 2,          0x00010006, 8,
                                            0x0000FFFE, 0x00020007, 0x00070000, 0x00080001, 0xFFFE0000,
                                            0x00080000, 2,          0x00010008, 8,          0x0000FFFF};
static const u32 data_ov003_020d9828[15] = {0xFFFE0000, 0x00090000, 2,          0x00010009, 0x00000020,
                                            0x0000FFFE, 0x0002000A, 0x000A0000, 0x00200001, 0xFFFE0000,
                                            0x000B0000, 2,          0x0001000B, 0x00000080, 0x0000FFFF};
static const u32 data_ov003_020d9864[15] = {0xFFFE0000, 0x000C0000, 2,          0x0001000C, 0x00000020,
                                            0x0000FFFE, 0x0002000D, 0x000D0000, 0x00200001, 0xFFFE0000,
                                            0x000E0000, 2,          0x0001000E, 0x00000080, 0x0000FFFF};
static const u32 data_ov003_020d98a0[15] = {0xFFFE0000, 0x000F0000, 2,          0x0001000F, 8,
                                            0x0000FFFE, 0x00020010, 0x00100000, 0x00080001, 0xFFFE0000,
                                            0x00110000, 2,          0x00010011, 8,          0x0000FFFF};
static const u32 data_ov003_020d98dc[15] = {0xFFFE0000, 0x00120000, 2,          0x00010012, 8,
                                            0x0000FFFE, 0x00020013, 0x00130000, 0x00080001, 0xFFFE0000,
                                            0x00140000, 2,          0x00010014, 8,          0x0000FFFF};
static const u32 data_ov003_020d9918[15] = {0xFFFE0000, 0x00150000, 2,          0x00010015, 0x00000060,
                                            0x0000FFFE, 0x00020016, 0x00160000, 0x00600001, 0xFFFE0000,
                                            0x00170000, 2,          0x00010017, 0x00000060, 0x0000FFFF};
static const u32 data_ov003_020d9954[15] = {0xFFFE0000, 0x00180000, 2,          0x00010018, 0x00000060,
                                            0x0000FFFE, 0x00020019, 0x00190000, 0x00600001, 0xFFFE0000,
                                            0x001A0000, 2,          0x0001001A, 0x00000060, 0x0000FFFF};
static const u32 data_ov003_020d9990[15] = {0xFFFE0000, 0x001B0000, 2,          0x0001001B, 0x00000010,
                                            0x0000FFFE, 0x0002001C, 0x001C0000, 0x00100001, 0xFFFE0000,
                                            0x001D0000, 2,          0x0001001D, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d99cc[15] = {0xFFFE0000, 0x001E0000, 2,          0x0001001E, 0x00000010,
                                            0x0000FFFE, 0x0002001F, 0x001F0000, 0x00100001, 0xFFFE0000,
                                            0x00200000, 2,          0x00010020, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d9a08[15] = {0xFFFE0000, 0x00210000, 2,          0x00010021, 0x00000010,
                                            0x0000FFFE, 0x00020022, 0x00220000, 0x00100001, 0xFFFE0000,
                                            0x00230000, 2,          0x00010023, 0x00000010, 0x0000FFFF};
static const u32 data_ov003_020d9a44[15] = {0xFFFE0000, 0x00240000, 2,          0x00010024, 0x00000010,
                                            0x0000FFFE, 0x00020025, 0x00250000, 0x00100001, 0xFFFE0000,
                                            0x00260000, 2,          0x00010026, 0x00000010, 0x0000FFFF};

const u32* data_ov003_020e3e64     = data_ov003_020d88c0;
const u32* data_ov003_020e3e68     = data_ov003_020d8998;
const u32* data_ov003_020e3e6c[13] = {
    data_ov003_020d8ed0, data_ov003_020d8d18, data_ov003_020d8d44, data_ov003_020d8ea4, data_ov003_020d8d70,
    data_ov003_020d8d9c, data_ov003_020d8df4, data_ov003_020d8dc8, data_ov003_020d8e78, data_ov003_020d8e4c,
    data_ov003_020d8e20, data_ov003_020d8cc0, data_ov003_020d8cec,
};
const u32* data_ov003_020e3ea0[13] = {
    data_ov003_020d90f4, data_ov003_020d8fa4, data_ov003_020d9164, data_ov003_020d8f34, data_ov003_020d8fdc,
    data_ov003_020d919c, data_ov003_020d9084, data_ov003_020d9014, data_ov003_020d8efc, data_ov003_020d90bc,
    data_ov003_020d904c, data_ov003_020d8f6c, data_ov003_020d912c,
};
const u32* data_ov003_020e3ed4[14] = {
    data_ov003_020d8a60, data_ov003_020d8a38, data_ov003_020d8ad8, data_ov003_020d8a10, data_ov003_020d8b28,
    data_ov003_020d8b78, data_ov003_020d8ab0, data_ov003_020d8b00, data_ov003_020d8bc8, data_ov003_020d8a88,
    data_ov003_020d8bf0, data_ov003_020d8b50, data_ov003_020d8ba0, data_ov003_020d889c,
};

const void* data_ov003_020e3f0c[1]  = {(void*)data_ov003_020d9a90};
const void* data_ov003_020e3f10[2]  = {(void*)data_ov003_020d9a80, (void*)data_ov003_020d9aa8};
char        data_ov003_020e3f18[32] = "Apl_Fur/Grp_BtlFld_Ce002RG.bin";
char        data_ov003_020e3f38[32] = "Apl_Fur/Grp_BtlFld_No009RG.bin";
char        data_ov003_020e3f58[32] = "Apl_Fur/Grp_BtlFld_Ex001RG.bin";
char        data_ov003_020e3f78[32] = "Apl_Fur/Grp_BtlFld_We004RG.bin";
char        data_ov003_020e3f98[32] = "Apl_Fur/Grp_BtlFld_We003RG.bin";
char        data_ov003_020e3fb8[32] = "Apl_Fur/Grp_BtlFld_Mk014RG.bin";
char        data_ov003_020e3fd8[32] = "Apl_Fur/Grp_BtlFld_We001RG.bin";
char        data_ov003_020e3ff8[32] = "Apl_Fur/Grp_BtlFld_So002RG.bin";
char        data_ov003_020e4018[32] = "Apl_Fur/Grp_BtlFld_So001RG.bin";
char        data_ov003_020e4038[32] = "Apl_Fur/Grp_BtlFld_Ea003RG.bin";
char        data_ov003_020e4058[32] = "Apl_Fur/Grp_BtlFld_Ea002RG.bin";
char        data_ov003_020e4078[32] = "Apl_Fur/Grp_BtlFld_Ea001RG.bin";
char        data_ov003_020e4098[32] = "Apl_Fur/Grp_BtlFld_No010RG.bin";
char        data_ov003_020e40b8[32] = "Apl_Fur/Grp_BtlFld_Ce001RG.bin";
char        data_ov003_020e40d8[32] = "Apl_Fur/Grp_BtlFld_No008RG.bin";
char        data_ov003_020e40f8[32] = "Apl_Fur/Grp_BtlFld_No006RG.bin";
char        data_ov003_020e4118[32] = "Apl_Fur/Grp_BtlFld_No005RG.bin";
char        data_ov003_020e4138[32] = "Apl_Fur/Grp_BtlFld_No004RG.bin";
char        data_ov003_020e4158[32] = "Apl_Fur/Grp_BtlFld_No003RG.bin";
char        data_ov003_020e4178[32] = "Apl_Fur/Grp_BtlFld_No002RG.bin";
char        data_ov003_020e4198[32] = "Apl_Fur/Grp_BtlFld_No001RG.bin";
char        data_ov003_020e41b8[32] = "Apl_Fur/Grp_BtlFld_Ce004RG.bin";
char        data_ov003_020e41d8[32] = "Apl_Fur/Grp_BtlFld_Ce003RG.bin";
char        data_ov003_020e41f8[32] = "Apl_Fur/Grp_BtlFld_No001UG.bin";
char        data_ov003_020e4218[32] = "Apl_Fur/Grp_BtlFld_Ce001UG.bin";
char        data_ov003_020e4238[32] = "Apl_Fur/Grp_BtlFld_Ex003UG.bin";
char        data_ov003_020e4258[32] = "Apl_Fur/Grp_BtlFld_Ex001UG.bin";
char        data_ov003_020e4278[32] = "Apl_Fur/Grp_BtlFld_We004UG.bin";
char        data_ov003_020e4298[32] = "Apl_Fur/Grp_BtlFld_We003UG.bin";
char        data_ov003_020e42b8[32] = "Apl_Fur/Grp_BtlFld_Mk014UG.bin";
char        data_ov003_020e42d8[32] = "Apl_Fur/Grp_BtlFld_We001UG.bin";
char        data_ov003_020e42f8[32] = "Apl_Fur/Grp_BtlFld_So002UG.bin";
char        data_ov003_020e4318[32] = "Apl_Fur/Grp_BtlFld_So001UG.bin";
char        data_ov003_020e4338[32] = "Apl_Fur/Grp_BtlFld_Ea003UG.bin";
char        data_ov003_020e4358[32] = "Apl_Fur/Grp_BtlFld_Ea002UG.bin";
char        data_ov003_020e4378[32] = "Apl_Fur/Grp_BtlFld_Ea001UG.bin";
char        data_ov003_020e4398[32] = "Apl_Fur/Grp_BtlFld_No010UG.bin";
char        data_ov003_020e43b8[32] = "Apl_Fur/Grp_BtlFld_No009UG.bin";
char        data_ov003_020e43d8[32] = "Apl_Fur/Grp_BtlFld_No008UG.bin";
char        data_ov003_020e43f8[32] = "Apl_Fur/Grp_BtlFld_No006UG.bin";
char        data_ov003_020e4418[32] = "Apl_Fur/Grp_BtlFld_No005UG.bin";
char        data_ov003_020e4438[32] = "Apl_Fur/Grp_BtlFld_No004UG.bin";
char        data_ov003_020e4458[32] = "Apl_Fur/Grp_BtlFld_No003UG.bin";
char        data_ov003_020e4478[32] = "Apl_Fur/Grp_BtlFld_No002UG.bin";
char        data_ov003_020e4498[32] = "Apl_Fur/Grp_BtlFld_Ce004UG.bin";
char        data_ov003_020e44b8[32] = "Apl_Fur/Grp_BtlFld_Ce003UG.bin";
char        data_ov003_020e44d8[32] = "Apl_Fur/Grp_BtlFld_Ce002UG.bin";

static char data_ov003_020e44f8[0x18] = "Data/AbilityData.bin";

static char data_ov003_020e4510[0x18] = "Apl_Tak/ItemData.bin";

static char data_ov003_020e4528[0x1C] = "Apl_Hir/Grp_BtlCircle.bin";

static char data_ov003_020e4544[0x1C] = "Apl_Hir/Grp_BtlPsy_Hold.bin";

static char data_ov003_020e4560[0x1C] = "Apl_Hir/Grp_BtlPsy_Lime.bin";

static char data_ov003_020e457c[0x1C] = "Apl_Hir/Grp_BtlPsy_Bomb.bin";

static char data_ov003_020e4598[0x20] = "Apl_Hir/Grp_BtlPsy_Element.bin";

static char data_ov003_020e45b8[0x20] = "Apl_Hir/Grp_BtlPsy_Recover.bin";

static char data_ov003_020e45d8[0x20] = "Apl_Hir/Grp_BtlPsy_Barrier.bin";

static char data_ov003_020e45f8[0x24] = "Apl_Hir/Grp_BtlPsy_BlackHoll.bin";

static char data_ov003_020e461c[0x24] = "Apl_Hir/Grp_BtlPsy_ShockWave.bin";

static char data_ov003_020e4640[0x24] = "Apl_Hir/Grp_BtlPsy_Territory.bin";

static char data_ov003_020e4664[0x24] = "Apl_Hir/Grp_BtlPsy_IcePillar.bin";

static char data_ov003_020e4688[0x24] = "Apl_Hir/Grp_BtlPsy_FieldShake.bin";

static char data_ov003_020e46ac[0x24] = "Apl_Hir/Grp_BtlPsy_DragAttack.bin";

static char data_ov003_020e46d0[0x24] = "Apl_Hir/Grp_BtlPsy_StreetLive.bin";

static char data_ov003_020e46f4[0x24] = "Apl_Hir/Grp_BtlPsy_MeteorFall.bin";

static char data_ov003_020e4718[0x24] = "Apl_Hir/Grp_BtlPsy_ChargeLaser.bin";

static char data_ov003_020e473c[0x24] = "Apl_Hir/Grp_BtlPsy_AttackPaint.bin";

static char data_ov003_020e4760[0x24] = "Apl_Hir/Grp_BtlPsy_ElectricGen.bin";

static char data_ov003_020e4784[0x24] = "Apl_Hir/Grp_BtlPsy_ElementBind.bin";

static char data_ov003_020e47a8[0x24] = "Apl_Hir/Grp_BtlPsy_SlashThunder.bin";

static char data_ov003_020e47cc[0x24] = "Apl_Hir/Grp_BtlPsy_AbsorbEffect.bin";

static char data_ov003_020e47f0[0x28] = "Apl_Hir/Grp_BtlPsy_Psychokinesis.bin";

static char data_ov003_020e4818[0x28] = "Apl_Hir/Grp_BtlPsy_LockOnThunder.bin";

static char           data_ov003_020e4840[0x28] = "Apl_Hir/Grp_BtlPsy_BadgeFormation.bin";
const Ov003ScriptWord data_ov003_020d9a80[4]    = {
    {.value = (u32)func_ov003_02083478},
    {.value = 0x00030001},
    {.value = 0x000D0002},
    {.value = (u32)func_ov003_02083470},
};

const Ov003ScriptWord data_ov003_020d9a90[6] = {
    {.value = (u32)func_ov003_02083700},
    {.value = (u32)data_ov003_020d9ad0},
    {.value = 0x00000002},
    {.value = 0x03000000},
    {.value = 0x000D0001},
    {.value = (u32)func_ov003_02083470},
};

const Ov003ScriptWord data_ov003_020d9aa8[10] = {
    {.value = (u32)func_ov003_02083478}, {.value = 0x00040003}, {.value = 0x000D0002},
    {.value = (u32)func_ov003_020835d4}, {.value = 0x00040001}, {.value = 0x03000001},
    {.value = (u32)func_ov003_020835d4}, {.value = 0x00040002}, {.value = 0x03000002},
    {.value = (u32)func_ov003_02083470},
};

static const u8 data_ov003_020d9ad0[0x2C] = {
    0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01,
    0x00, 0x40, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
};

const u8 data_ov003_020d9afc[0x18] = {
    0x05, 0x00, 0xC0, 0x3A, 0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0xC0, 0x15,
    0x03, 0x00, 0x04, 0x00, 0x04, 0x00, 0xC0, 0x0C, 0x03, 0x00, 0x04, 0x00,
};

static const Ov003AnimRecord data_ov003_020d9b14[1] = {
    {8, 0x0036, -40},
};

static const Ov003AnimRecord data_ov003_020d9b1a[1] = {
    {8, 0x0036, -40},
};

static const Ov003AnimRecord data_ov003_020d9b20[1] = {
    {8, 0x003A, -40},
};

static const Ov003AnimRecord data_ov003_020d9b26[1] = {
    {8, 0x0036, -40},
};

static const Ov003AnimRecord data_ov003_020d9b2c[1] = {
    {8, 0x0036, -40},
};

static const Ov003AnimRecord data_ov003_020d9b32[1] = {
    {8, 0x0048, -40},
};

static const Ov003AnimRecord data_ov003_020d9b38[1] = {
    {8, 0x003A, -40},
};

static const Ov003AnimRecord data_ov003_020d9b3e[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9b44[1] = {
    {8, 0x003C, -40},
};

static const Ov003AnimRecord data_ov003_020d9b4a[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9b50[1] = {
    {8, 0x0030, -40},
};

static const Ov003AnimRecord data_ov003_020d9b56[1] = {
    {8, 0x0030, -40},
};

static const Ov003AnimRecord data_ov003_020d9b5c[1] = {
    {8, 0x0030, -40},
};

static const Ov003AnimRecord data_ov003_020d9b62[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9b68[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9b6e[1] = {
    {61, 0x0033, 0},
};

static const Ov003AnimRecord data_ov003_020d9b74[1] = {
    {8, 0x0034, -40},
};

static const Ov003AnimRecord data_ov003_020d9b7a[1] = {
    {8, 0x0040, -40},
};

static const Ov003AnimRecord data_ov003_020d9b80[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9b86[1] = {
    {8, 0x0036, -40},
};

static const Ov003AnimRecord data_ov003_020d9b8c[1] = {
    {8, 0x0034, -40},
};

static const Ov003AnimRecord data_ov003_020d9b92[1] = {
    {8, 0x0028, -40},
};

static const Ov003AnimRecord data_ov003_020d9b98[1] = {
    {8, 0x0030, -40},
};

static const Ov003AnimRecord data_ov003_020d9b9e[1] = {
    {8, 0x0038, -40},
};

static const Ov003AnimRecord data_ov003_020d9ba4[1] = {
    {8, 0x0028, -40},
};

static const u8 data_ov003_020d9baa[0xE] = {
    0x08, 0x00, 0x28, 0x00, 0xD8, 0xFF, 0x08, 0x00, 0x38, 0x00, 0xD8, 0xFF, 0x00, 0x00,
};

static const BinIdentifier data_ov003_020d9bb8 = {3, (char*)&data_ov003_020e40b8};

static const BinIdentifier data_ov003_020d9bc0 = {3, (char*)&data_ov003_020e40b8};

static const BinIdentifier data_ov003_020d9bc8 = {3, (char*)&data_ov003_020e3f18};

static const BinIdentifier data_ov003_020d9bd0 = {3, (char*)&data_ov003_020e41d8};

static const BinIdentifier data_ov003_020d9bd8 = {3, (char*)&data_ov003_020e41b8};

static const BinIdentifier data_ov003_020d9be0 = {3, (char*)&data_ov003_020e4198};

static const BinIdentifier data_ov003_020d9be8 = {3, (char*)&data_ov003_020e4178};

static const BinIdentifier data_ov003_020d9bf0 = {3, (char*)&data_ov003_020e4158};

static const BinIdentifier data_ov003_020d9bf8 = {3, (char*)&data_ov003_020e4138};

static const BinIdentifier data_ov003_020d9c00 = {3, (char*)&data_ov003_020e4118};

static const BinIdentifier data_ov003_020d9c08 = {3, (char*)&data_ov003_020e40f8};

static const BinIdentifier data_ov003_020d9c10 = {3, (char*)&data_ov003_020e40f8};

static const BinIdentifier data_ov003_020d9c18 = {3, (char*)&data_ov003_020e40d8};

static const BinIdentifier data_ov003_020d9c20 = {3, (char*)&data_ov003_020e3f38};

static const BinIdentifier data_ov003_020d9c28 = {3, (char*)&data_ov003_020e4098};

static const BinIdentifier data_ov003_020d9c30 = {3, (char*)&data_ov003_020e4078};

static const BinIdentifier data_ov003_020d9c38 = {3, (char*)&data_ov003_020e4058};

static const BinIdentifier data_ov003_020d9c40 = {3, (char*)&data_ov003_020e4038};

static const BinIdentifier data_ov003_020d9c48 = {3, (char*)&data_ov003_020e4018};

static const BinIdentifier data_ov003_020d9c50 = {3, (char*)&data_ov003_020e3ff8};

static const BinIdentifier data_ov003_020d9c58 = {3, (char*)&data_ov003_020e3fd8};

static const BinIdentifier data_ov003_020d9c60 = {3, (char*)&data_ov003_020e3fb8};

static const BinIdentifier data_ov003_020d9c68 = {3, (char*)&data_ov003_020e3f98};

static const BinIdentifier data_ov003_020d9c70 = {3, (char*)&data_ov003_020e3f78};

static const BinIdentifier data_ov003_020d9c78 = {3, (char*)&data_ov003_020e3f58};

static const BinIdentifier data_ov003_020d9c80[2] = {
    {3, (char*)&data_ov003_020e3f58},
    {3, (char*)&data_ov003_020e4018},
};

const void* const data_ov003_020d9c90[1] = {(const void*)&data_ov003_020d9bb8};

const Ov003SubCameraPathTable data_ov003_020d9c94 = {
    {
     {&data_ov003_020d9b14,
         &data_ov003_020e3ea0,
         {0x01, 0x02, 0x03, 0x0B, 0x01, 0x02, 0x80, 0x6D, 0x80, 0x68, 0x01, 0x02, 0x0D, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bc0},
     {&data_ov003_020d9b1a,
         &data_ov003_020e3ea0,
         {0x01, 0x02, 0x03, 0x0B, 0x01, 0x02, 0x80, 0x6D, 0x80, 0x68, 0x01, 0x02, 0x0D, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bc8},
     {&data_ov003_020d9b20,
         &data_ov003_020e3ed4,
         {0x01, 0x02, 0x03, 0x0B, 0x01, 0x02, 0xC0, 0x75, 0x00, 0x6C, 0x01, 0x02, 0x0E, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bd0},
     {&data_ov003_020d9b26,
         &data_ov003_020e3e6c,
         {0x01, 0x02, 0x03, 0x0A, 0x01, 0x02, 0x00, 0x60, 0x00, 0x68, 0x01, 0x02, 0x0D, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bd8},
     {&data_ov003_020d9b2c,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x7C, 0xC0, 0x7A, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00},
         &data_ov003_020d9be0},
     {&data_ov003_020d9b32,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x75, 0xC0, 0x75, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9be8},
     {&data_ov003_020d9b38,
         &data_ov003_020e3e68,
         {0x01, 0x02, 0x03, 0x0C, 0x01, 0x02, 0x40, 0x62, 0x40, 0x62, 0x01, 0x02, 0x01, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bf0},
     {&data_ov003_020d9b3e,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x77, 0x80, 0x74, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00},
         &data_ov003_020d9bf8},
     {&data_ov003_020d9b44,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x68, 0x40, 0x6D, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c00},
     {&data_ov003_020d9b4a,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x68, 0x40, 0x76, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c08},
     {&data_ov003_020d9b50,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x78, 0x00, 0x76, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c10},
     {&data_ov003_020d9b56,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x78, 0x00, 0x76, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c18},
     {&data_ov003_020d9b5c,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x5F, 0xC0, 0x7F, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c20},
     {&data_ov003_020d9b62,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x74, 0x40, 0x55, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c28},
     {&data_ov003_020d9b68,
         &data_ov003_020e3e64,
         {0x01, 0x02, 0x03, 0x0C, 0x01, 0x02, 0x80, 0x6B, 0x40, 0x60, 0x01, 0x02, 0x01, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c30},
     {&data_ov003_020d9b6e,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x73, 0x40, 0x6F, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c38},
     {&data_ov003_020d9b74,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x6C, 0x80, 0x5B, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c40},
     {&data_ov003_020d9b7a,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c48},
     {&data_ov003_020d9b80,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x7E, 0x40, 0x7C, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c50},
     {&data_ov003_020d9b86,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x6D, 0xC0, 0x65, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c58},
     {&data_ov003_020d9b8c,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xC0, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00},
         &data_ov003_020d9c60},
     {&data_ov003_020d9b92,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xC0, 0x5D, 0x40, 0x67, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c68},
     {&data_ov003_020d9b98,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x7C, 0xC0, 0x62, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c70},
     {&data_ov003_020d9b9e,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x7A, 0x00, 0x80, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c78},
     {&data_ov003_020d9ba4,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x45, 0x00, 0x2F, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c80},
     {&data_ov003_020d9baa,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x45, 0x00, 0x2F, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c48},
     {&data_ov003_020d9b80,
         NULL,
         {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x7E, 0x40, 0x7C, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9be8},
     {&data_ov003_020d9b38,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x40, 0x62, 0x40, 0x62, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c20},
     {&data_ov003_020d9b62,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x40, 0x74, 0x40, 0x55, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c40},
     {&data_ov003_020d9b7a,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bc8},
     {&data_ov003_020d9b20,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0xC0, 0x75, 0x00, 0x6C, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c28},
     {&data_ov003_020d9b68,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x80, 0x6B, 0x40, 0x60, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c08},
     {&data_ov003_020d9b50,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x78, 0x00, 0x76, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c18},
     {&data_ov003_020d9b5c,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x5F, 0xC0, 0x7F, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c00},
     {&data_ov003_020d9b4a,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x68, 0x40, 0x76, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c70},
     {&data_ov003_020d9b9e,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x40, 0x7A, 0x00, 0x80, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bd8},
     {&data_ov003_020d9b2c,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x7C, 0xC0, 0x7A, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00},
         &data_ov003_020d9c68},
     {&data_ov003_020d9b98,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x40, 0x7C, 0xC0, 0x62, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9be0},
     {&data_ov003_020d9b32,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x80, 0x75, 0xC0, 0x75, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9bc0},
     {&data_ov003_020d9b1a,
         NULL,
         {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x80, 0x6D, 0x80, 0x68, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
         &data_ov003_020d9c60},
     },
    &data_ov003_020d9b92,
    NULL,
    {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xC0, 0x5D, 0x40, 0x67, 0x01, 0x02, 0x00, 0xFF, 0x00, 0x00},
    0x3,
    (const char*)&data_ov003_020e41f8,
};

const BinIdentifier data_ov003_020da114 = {3, (char*)&data_ov003_020e4218};

static const BinIdentifier data_ov003_020da11c = {3, (char*)&data_ov003_020e44d8};

const BinIdentifier data_ov003_020da124 = {3, (char*)&data_ov003_020e44b8};

static const BinIdentifier data_ov003_020da12c = {3, (char*)&data_ov003_020e4498};

static const BinIdentifier data_ov003_020da134 = {3, (char*)&data_ov003_020e41f8};

static const BinIdentifier data_ov003_020da13c = {3, (char*)&data_ov003_020e4478};

static const BinIdentifier data_ov003_020da144 = {3, (char*)&data_ov003_020e4458};

static const BinIdentifier data_ov003_020da14c = {3, (char*)&data_ov003_020e4438};

const BinIdentifier data_ov003_020da154 = {3, (char*)&data_ov003_020e4418};

static const BinIdentifier data_ov003_020da15c = {3, (char*)&data_ov003_020e43f8};

static const BinIdentifier data_ov003_020da164 = {3, (char*)&data_ov003_020e43f8};

const BinIdentifier data_ov003_020da16c = {3, (char*)&data_ov003_020e43d8};

const BinIdentifier data_ov003_020da174 = {3, (char*)&data_ov003_020e43b8};

static const BinIdentifier data_ov003_020da17c = {3, (char*)&data_ov003_020e4398};

static const BinIdentifier data_ov003_020da184 = {3, (char*)&data_ov003_020e4378};

static const BinIdentifier data_ov003_020da18c = {3, (char*)&data_ov003_020e4358};

static const BinIdentifier data_ov003_020da194 = {3, (char*)&data_ov003_020e4338};

static const BinIdentifier data_ov003_020da19c = {3, (char*)&data_ov003_020e4318};

static const BinIdentifier data_ov003_020da1a4 = {3, (char*)&data_ov003_020e42f8};

const BinIdentifier data_ov003_020da1ac = {3, (char*)&data_ov003_020e42d8};

const BinIdentifier data_ov003_020da1b4 = {3, (char*)&data_ov003_020e42b8};

static const BinIdentifier data_ov003_020da1bc = {3, (char*)&data_ov003_020e4298};

static const BinIdentifier data_ov003_020da1c4 = {3, (char*)&data_ov003_020e4278};

static const BinIdentifier data_ov003_020da1cc = {3, (char*)&data_ov003_020e4258};

static const BinIdentifier data_ov003_020da1d4 = {3, (char*)&data_ov003_020e4258};

static const Ov003BinIdenEx data_ov003_020da1dc = {
    {3, (char*)&data_ov003_020e4238},
    {0x0180, 0x0070, 0x0070, 0x0180}
};

static const Ov003PathRect data_ov003_020da1ec = {512, 192, 192, 384};

static const Ov003PathRect data_ov003_020da1f4 = {384, 80, 80, 272};

static const Ov003PathRect data_ov003_020da1fc = {384, 128, 128, 320};

static const Ov003PathRect data_ov003_020da204 = {304, 80, 80, 272};

static const Ov003PathRect data_ov003_020da20c = {384, 128, 128, 384};

static const Ov003PathRect data_ov003_020da214 = {384, 128, 128, 320};

static const Ov003PathRect data_ov003_020da21c = {464, 80, 80, 272};

static const Ov003PathRect data_ov003_020da224 = {384, 128, 128, 320};

static const Ov003PathRect data_ov003_020da22c = {464, 144, 144, 320};

static const Ov003PathRect data_ov003_020da234 = {304, 88, 120, 320};

static const Ov003PathRect data_ov003_020da23c = {304, 88, 120, 320};

static const Ov003PathRect data_ov003_020da244 = {384, 128, 128, 320};

static const Ov003PathRect data_ov003_020da24c = {384, 160, 160, 352};

static const Ov003PathRect data_ov003_020da254 = {384, 128, 128, 320};

static const Ov003PathRect data_ov003_020da25c = {384, 96, 144, 320};

static const Ov003PathRect data_ov003_020da264 = {384, 80, 80, 272};

static const Ov003PathRect data_ov003_020da26c = {464, 80, 80, 288};

static const Ov003PathRect data_ov003_020da274 = {384, 128, 128, 336};

static const Ov003PathRect data_ov003_020da27c = {464, 80, 80, 272};

static const Ov003PathRect data_ov003_020da284 = {464, 128, 80, 320};

static const Ov003PathRect data_ov003_020da28c = {384, 156, 156, 352};

static const Ov003PathRect data_ov003_020da294 = {304, 128, 128, 296};

static const Ov003PathRect data_ov003_020da29c = {464, 128, 128, 336};

static const Ov003PathRect data_ov003_020da2a4 = {384, 128, 128, 352};

static const Ov003PathRect data_ov003_020da2ac = {384, 128, 128, 352};

static const Ov003PathRect data_ov003_020da2b4 = {384, 128, 128, 336};

const void* const data_ov003_020da2bc[1] = {(const void*)&data_ov003_020da134};

const Ov003TouchPathEntry data_ov003_020da2c0[41] = {
    {&data_ov003_020da20c,
     NULL,{0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x59, 0xC0, 0x58, 0x01, 0x02, 0x03, 0x04, 0x00, 0x04},
     &data_ov003_020da114},
    {&data_ov003_020da1ec,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x7C, 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00},
     &data_ov003_020da11c},
    {&data_ov003_020da1f4,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x54, 0x60, 0x49, 0x01, 0x02, 0x03, 0x04, 0x00, 0x01},
     &data_ov003_020da124},
    {&data_ov003_020da1fc,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x61, 0x40, 0x5B, 0x01, 0x02, 0x03, 0x04, 0x00, 0x02},
     &data_ov003_020da12c},
    {&data_ov003_020da204,
     &data_ov003_020e3f10,
     {0x01, 0x02, 0x03, 0x0D, 0x01, 0x02, 0xC0, 0x62, 0xC0, 0x62, 0x01, 0x02, 0x03, 0x04, 0x02, 0x03},
     &data_ov003_020da134},
    {&data_ov003_020da20c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x59, 0xC0, 0x58, 0x01, 0x02, 0x03, 0x04, 0x00, 0x04},
     &data_ov003_020da13c},
    {&data_ov003_020da214,
     &data_ov003_020e3f0c,
     {0x01, 0x02, 0x03, 0x0D, 0x01, 0x02, 0x00, 0x54, 0x00, 0x60, 0x01, 0x02, 0x03, 0x04, 0x01, 0x05},
     &data_ov003_020da144},
    {&data_ov003_020da21c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x60, 0x6F, 0x20, 0x70, 0x01, 0x02, 0x03, 0x04, 0x00, 0x06},
     &data_ov003_020da14c},
    {&data_ov003_020da224,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xA0, 0x5E, 0x00, 0x58, 0x01, 0x02, 0x03, 0x04, 0x00, 0x07},
     &data_ov003_020da154},
    {&data_ov003_020da22c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x76, 0xA0, 0x7F, 0x01, 0x02, 0x03, 0x04, 0x00, 0x08},
     &data_ov003_020da15c},
    {&data_ov003_020da234,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x5E, 0x60, 0x5C, 0x01, 0x02, 0x03, 0x04, 0x00, 0x09},
     &data_ov003_020da164},
    {&data_ov003_020da23c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x5E, 0x60, 0x5C, 0x01, 0x02, 0x03, 0x04, 0x00, 0x09},
     &data_ov003_020da16c},
    {&data_ov003_020da244,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x20, 0x4A, 0xC0, 0x4B, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0A},
     &data_ov003_020da174},
    {&data_ov003_020da24c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x20, 0x5C, 0xE0, 0x64, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0B},
     &data_ov003_020da17c},
    {&data_ov003_020da254,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x68, 0xE0, 0x66, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0C},
     &data_ov003_020da184},
    {&data_ov003_020da25c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x73, 0x40, 0x77, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0D},
     &data_ov003_020da18c},
    {&data_ov003_020da264,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x20, 0x5A, 0xA0, 0x57, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0E},
     &data_ov003_020da194},
    {&data_ov003_020da26c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x6E, 0x20, 0x6D, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0F},
     &data_ov003_020da19c},
    {&data_ov003_020da274,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x62, 0xA0, 0x62, 0x01, 0x02, 0x03, 0x04, 0x00, 0x10},
     &data_ov003_020da1a4},
    {&data_ov003_020da27c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x00, 0x71, 0xE0, 0x6E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x11},
     &data_ov003_020da1ac},
    {&data_ov003_020da284,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x80, 0x73, 0xA0, 0x7F, 0x01, 0x02, 0x03, 0x04, 0x00, 0x12},
     &data_ov003_020da1b4},
    {&data_ov003_020da28c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x78, 0x60, 0x77, 0x01, 0x02, 0x03, 0x04, 0x00, 0x13},
     &data_ov003_020da1bc},
    {&data_ov003_020da294,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x60, 0x39, 0x40, 0x48, 0x01, 0x02, 0x03, 0x04, 0x00, 0x14},
     &data_ov003_020da1c4},
    {&data_ov003_020da29c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x60, 0x74, 0xA0, 0x7E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x15},
     &data_ov003_020da1cc},
    {&data_ov003_020da2a4,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xE0, 0x5C, 0xC0, 0x57, 0x01, 0x02, 0x03, 0x04, 0x00, 0x16},
     &data_ov003_020da1d4},
    {&data_ov003_020da2ac,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xE0, 0x5C, 0xC0, 0x57, 0x01, 0x02, 0x03, 0x04, 0x00, 0x16},
     &data_ov003_020da1dc},
    {&data_ov003_020da2b4,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0xE0, 0x61, 0x60, 0x5F, 0x01, 0x02, 0x03, 0x04, 0x00, 0x10},
     &data_ov003_020da13c},
    {&data_ov003_020da214,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x54, 0x00, 0x60, 0x01, 0x02, 0x03, 0x04, 0x00, 0x05},
     &data_ov003_020da174},
    {&data_ov003_020da24c,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x20, 0x5C, 0xE0, 0x64, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0B},
     &data_ov003_020da194},
    {&data_ov003_020da26c,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x6E, 0x20, 0x6D, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0F},
     &data_ov003_020da11c},
    {&data_ov003_020da1f4,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x54, 0x60, 0x49, 0x01, 0x02, 0x03, 0x04, 0x00, 0x01},
     &data_ov003_020da17c},
    {&data_ov003_020da254,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x40, 0x68, 0xE0, 0x66, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0C},
     &data_ov003_020da15c},
    {&data_ov003_020da234,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x80, 0x5E, 0x60, 0x5C, 0x01, 0x02, 0x03, 0x04, 0x00, 0x09},
     &data_ov003_020da16c},
    {&data_ov003_020da244,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x20, 0x4A, 0xC0, 0x4B, 0x01, 0x02, 0x03, 0x04, 0x00, 0x0A},
     &data_ov003_020da154},
    {&data_ov003_020da22c,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x76, 0xA0, 0x7F, 0x01, 0x02, 0x03, 0x04, 0x00, 0x08},
     &data_ov003_020da1c4},
    {&data_ov003_020da29c,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x60, 0x74, 0xA0, 0x7E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x15},
     &data_ov003_020da12c},
    {&data_ov003_020da204,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0xC0, 0x62, 0xC0, 0x62, 0x01, 0x02, 0x03, 0x04, 0x00, 0x03},
     &data_ov003_020da1bc},
    {&data_ov003_020da294,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x60, 0x39, 0x40, 0x48, 0x01, 0x02, 0x03, 0x04, 0x00, 0x14},
     &data_ov003_020da134},
    {&data_ov003_020da20c,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x59, 0xC0, 0x58, 0x01, 0x02, 0x03, 0x04, 0x00, 0x04},
     &data_ov003_020da114},
    {&data_ov003_020da1ec,
     NULL, {0x01, 0x02, 0x04, 0x0F, 0x01, 0x02, 0x00, 0x7C, 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00},
     &data_ov003_020da1b4},
    {&data_ov003_020da28c,
     NULL, {0x01, 0x02, 0x03, 0x0F, 0x01, 0x02, 0x40, 0x78, 0x60, 0x77, 0x01, 0x02, 0x03, 0x04, 0x00, 0x13},
     (const void*)0x4    },
};

static const BinIdentifier data_ov003_020da73c = {3, (char*)&data_ov003_020e44f8};

const BinIdentifier data_ov003_020da744 = {3, (char*)&data_ov003_020e4510};

static const BinIdentifier data_ov003_020da74c = {3, (char*)&data_ov003_020e46ac};

static const BinIdentifier data_ov003_020da754 = {3, (char*)&data_ov003_020e4818};

static const BinIdentifier data_ov003_020da75c = {3, (char*)&data_ov003_020e45f8};

static const BinIdentifier data_ov003_020da764 = {3, (char*)&data_ov003_020e4688};

static const BinIdentifier data_ov003_020da76c = {3, (char*)&data_ov003_020e4598};

static const BinIdentifier data_ov003_020da774 = {3, (char*)&data_ov003_020e4784};

static const BinIdentifier data_ov003_020da77c = {3, (char*)&data_ov003_020e4664};

static const BinIdentifier data_ov003_020da784 = {3, (char*)&data_ov003_020e4840};

static const BinIdentifier data_ov003_020da78c = {3, (char*)&data_ov003_020e4640};

static const BinIdentifier data_ov003_020da794 = {3, (char*)&data_ov003_020e457c};

static const BinIdentifier data_ov003_020da79c = {3, (char*)&data_ov003_020e47f0};

static const BinIdentifier data_ov003_020da7a4 = {3, (char*)&data_ov003_020e47cc};

static const BinIdentifier data_ov003_020da7ac = {3, (char*)&data_ov003_020e4760};

static const BinIdentifier data_ov003_020da7b4 = {3, (char*)&data_ov003_020e4528};

static const BinIdentifier data_ov003_020da7bc = {3, (char*)&data_ov003_020e461c};

static const BinIdentifier data_ov003_020da7c4 = {3, (char*)&data_ov003_020e45b8};

static const BinIdentifier data_ov003_020da7cc = {3, (char*)&data_ov003_020e4560};

static const BinIdentifier data_ov003_020da7d4 = {3, (char*)&data_ov003_020e45d8};

static const BinIdentifier data_ov003_020da7dc = {3, (char*)&data_ov003_020e46d0};

static const BinIdentifier data_ov003_020da7e4 = {3, (char*)&data_ov003_020e47a8};

static const BinIdentifier data_ov003_020da7ec = {3, (char*)&data_ov003_020e46f4};

static const BinIdentifier data_ov003_020da7f4 = {3, (char*)&data_ov003_020e473c};

static const BinIdentifier data_ov003_020da7fc = {3, (char*)&data_ov003_020e4544};

static const BinIdentifier data_ov003_020da804 = {3, (char*)&data_ov003_020e4718};

static const Ov003TouchIconParams data_ov003_020da80c[258] = {
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  1,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  2,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  3,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da754, 3,  4,    {1, 3, 2, 0}, 0,  7, 128, 0},
    {&data_ov003_020da754, 3,  5,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da754, 3,  5,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da754, 3,  6,    {1, 3, 2, 0}, 1,  6, 128, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  2,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 0,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 1,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  1,    {1, 3, 2, 0}, 2,  5, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 0,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 1,  4, 256, 0},
    {&data_ov003_020da7e4, 3,  3,    {1, 3, 2, 0}, 2,  4, 256, 0},
    {&data_ov003_020da76c, 3,  3,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  1,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  2,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  5,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  6,  16, 0},
    {&data_ov003_020da76c, 3,  4,    {1, 3, 2, 0}, 0,  7,  16, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  1,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  2,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  3,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  4,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  6,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  5,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  7,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  7,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  8,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  8,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 10,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 10,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 2,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 11,    {1, 3, 2, 0}, 3,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 12,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 12,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 13,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 13,    {1, 3, 2, 0}, 3,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 14,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3,  9,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 17,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 17,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da7bc, 3, 18,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 18,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 3,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 0,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 1,  5,  32, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 4,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 16,    {1, 3, 2, 0}, 2,  5, 128, 0},
    {&data_ov003_020da7bc, 3, 15,    {1, 3, 2, 0}, 1,  4,  64, 0},
    {&data_ov003_020da7bc, 3, 15,    {1, 3, 2, 0}, 1,  5,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {16, 18, 17, 0}, 0, 19,  72, 0},
    {&data_ov003_020da7ec, 3,  1, {16, 18, 17, 0}, 1, 19,  72, 0},
    {&data_ov003_020da7ec, 3,  1,    {1, 3, 2, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {1, 3, 2, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {4, 6, 5, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {4, 6, 5, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {7, 9, 8, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1,    {7, 9, 8, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {10, 12, 11, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {10, 12, 11, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {13, 15, 14, 0}, 0, 19,  64, 0},
    {&data_ov003_020da7ec, 3,  1, {13, 15, 14, 0}, 1, 19,  64, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7f4, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 0,  6, 128, 0},
    {&data_ov003_020da7f4, 3,  1,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 1,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 2,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 3,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {5, 7, 6, 0}, 4,  8,  90, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 0,  4,   4, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 1,  4,   8, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 2,  4,  16, 0},
    {&data_ov003_020da7ac, 3,  1,    {1, 3, 2, 0}, 3,  4,  16, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 4,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 5,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 6,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 7,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 5,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 8,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  2,    {1, 3, 2, 0}, 9,  4,  48, 0},
    {&data_ov003_020da7ac, 3,  3,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da7d4, 3,  1,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  1,    {4, 6, 5, 0}, 0,  7,  96, 0},
    {&data_ov003_020da7d4, 3,  2,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  2,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da7d4, 3,  3,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  3,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da7d4, 3,  4,    {1, 3, 2, 0}, 0,  7,   1, 0},
    {&data_ov003_020da7d4, 3,  4,    {4, 6, 5, 0}, 0,  7,  48, 0},
    {&data_ov003_020da77c, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  2,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  3,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da77c, 3,  3,    {1, 3, 2, 0}, 0,  5,  64, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 0,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  5, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 1,  6, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 2,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  4, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  5, 128, 0},
    {&data_ov003_020da794, 3,  2,    {1, 3, 2, 0}, 3,  6, 128, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da794, 3,  4,    {1, 3, 2, 0}, 1,  4, 208, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 4,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 5,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 6,  4,  40, 0},
    {&data_ov003_020da794, 3,  1,    {1, 3, 2, 0}, 3,  4,  40, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da794, 3,  6,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da794, 3,  3,    {1, 3, 2, 0}, 0,  4,  40, 0},
    {&data_ov003_020da794, 3,  3,    {1, 3, 2, 0}, 1,  4,  40, 0},
    {&data_ov003_020da794, 3,  4,    {1, 3, 2, 0}, 0,  4, 208, 0},
    {&data_ov003_020da794, 3,  5,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 1,  4,  64, 0},
    {&data_ov003_020da75c, 3,  1,    {1, 3, 2, 0}, 2,  4,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0,  8,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0,  9,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0, 10,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {4, 6, 5, 0}, 0, 11,  64, 0},
    {&data_ov003_020da74c, 3,  1,    {1, 3, 2, 0}, 0,  7,  64, 0},
    {&data_ov003_020da79c, 3,  2,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da79c, 3,  3,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da79c, 3,  3,    {1, 3, 2, 0}, 0,  4,  32, 0},
    {&data_ov003_020da79c, 3,  1,    {1, 3, 2, 0}, 0,  4,  34, 0},
    {&data_ov003_020da7cc, 3,  1,    {1, 3, 2, 0}, 0,  4,  64, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 2,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 3,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 4,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 5,  4,   1, 0},
    {&data_ov003_020da7a4, 3,  1,    {1, 3, 2, 0}, 6,  4,   1, 0},
    {&data_ov003_020da7b4, 3,  1,    {1, 3, 2, 0}, 2,  4,   4, 0},
    {&data_ov003_020da7b4, 3,  1,    {1, 3, 2, 0}, 3,  4,   6, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da774, 3,  1,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da774, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da764, 3,  1,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da764, 3,  1,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da764, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  2,    {5, 7, 6, 0}, 0,  8,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 0,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 8,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  1,    {1, 3, 2, 0}, 5,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 1,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 2,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 3,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  4,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 1,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 2,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 3,  4,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 4,  4,  16, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 5,  4,   4, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 6,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 7,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {1, 3, 2, 0}, 8,  4,  20, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 1,  9,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 2,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 3,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 5,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 2,  9,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 4,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 6,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 7,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 8,  8,  80, 0},
    {&data_ov003_020da7dc, 3,  1,    {5, 7, 6, 0}, 6,  9,  80, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 0,  4,  16, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 1,  4,  16, 0},
    {&data_ov003_020da78c, 3,  1,    {1, 3, 2, 0}, 5,  4,  16, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 0,  4, 220, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 0,  5, 220, 0},
    {&data_ov003_020da804, 3,  1,    {6, 8, 7, 0}, 0,  9,  64, 0},
    {&data_ov003_020da804, 3,  1,    {6, 8, 7, 0}, 0, 10,  64, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 2,  4, 220, 0},
    {&data_ov003_020da804, 3,  1,    {1, 3, 2, 0}, 2,  5, 220, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  4,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  5,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 0,  6,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 1,  6,  32, 0},
    {&data_ov003_020da7c4, 3,  1,    {1, 3, 2, 0}, 2,  6,  32, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 4,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 5,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 6,  4,  96, 0},
    {&data_ov003_020da7fc, 3,  3,    {1, 3, 2, 0}, 7,  4,  96, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 1,  4,  48, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 0,  4,  48, 0},
    {&data_ov003_020da784, 3,  1,    {1, 3, 2, 0}, 2,  4,  48, 0},
};

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Queues a touch event for ability `id` unless the ability cannot be used by `touchIndex`
/// (a value of -1 or 0..2 selects "any touch" or one specific touch).
void func_ov003_020a30d4(s32 id, s32 touchIndex, u32 slot) {
    u32              anyTouch = 1;
    Ov003TouchEvent* events;
    Ov003AbilityData ability;
    s32              i;

    events = data_ov003_020e71b8->touchEvents;
    func_020274ac(&ability, id, &data_ov003_020da73c);
    if (touchIndex >= 0 && touchIndex <= 2) {
        anyTouch = 0;
        if (ability.flags.usable[touchIndex] == 0) {
            return;
        }
    } else if (ability.flags.valid == 0) {
        return;
    }
    for (i = 0; i < 0xE; i++) {
        if (events[i].unk_00 == 0xFF) {
            events[i].unk_00   = id;
            events[i].anyTouch = anyTouch;
            events[i].ability  = ability;
            events[i].slot     = slot;
            return;
        }
    }
}

/// TRUE when a touch event queued for `touchMode` (0 = any touch, 1 = any-touch events,
/// 2 = single-touch events) applies to them.
static s32 func_ov003_020a31bc(s32 touchMode, s32 anyTouch) {
    switch (touchMode) {
        case 0:
            break;
        case 1:
            if (anyTouch == 0) {
                return 0;
            }
            break;
        case 2:
            if (anyTouch != 0) {
                return 0;
            }
            break;
    }
    return 1;
}

/// TRUE unless the actor of touch slot `slot` is in a state that cannot accept a touch.
// Nonmatching: mwcc range-folds the `state == 4 || state == 5` test instead of the cmp/cmpne chain.
static s32 func_ov003_020a31fc(s32 slot) {
    Ov003TouchActor* actor;

    if (slot == -1) {
        return 1;
    }
    for (actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC); actor != NULL; actor = func_ov003_02084984(&actor->node))
    {
        if (actor->slot != slot) {
            continue;
        }
        if (actor->state == 4 || actor->state == 5) {
            return 0;
        }
        if (actor->state == 0 && (actor->request == 4 || actor->request == 6)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

/// TRUE when `actionId` is one of the actions a queued touch event grants.
static s32 func_ov003_020a3298(s32 actionId, s32 touchMode) {
    Ov003TouchEvent*        events = data_ov003_020e71b8->touchEvents;
    const Ov003AbilityData* ability;
    s32                     i;
    s32                     j;

    for (i = 0; i < 0xE; i++) {
        Ov003TouchEvent* event = &events[i];

        if (event->unk_00 == 0xFF || func_ov003_020a31bc(touchMode, event->anyTouch) == 0 ||
            func_ov003_020a31fc(event->slot) == 0)
        {
            continue;
        }
        ability = &event->ability;
        for (j = 0; j < 4; j++) {
            if (actionId == ability->slots[j].actionId) {
                return 1;
            }
        }
    }
    return 0;
}

s32 func_ov003_020a3330(s32 actionId) {
    return func_ov003_020a3298(actionId, 0);
}

s32 func_ov003_020a3340(s32 actionId) {
    return func_ov003_020a3298(actionId, 1);
}

s32 func_ov003_020a3350(s32 actionId) {
    return func_ov003_020a3298(actionId, 2);
}

/// The amount of `actionId` a queued touch event grants: most actions add the recorded amount,
/// but a few (the multi-hit ones) report their per-action value directly.
static s32 func_ov003_020a3360(s32 actionId, s32 touchMode) {
    Ov003TouchEvent*        events = data_ov003_020e71b8->touchEvents;
    const Ov003AbilityData* ability;
    s32                     i;
    s32                     j;
    s32                     total = 0;

    for (i = 0; i < 0xE; i++) {
        Ov003TouchEvent* event = &events[i];

        if (event->unk_00 == 0xFF || func_ov003_020a31bc(touchMode, event->anyTouch) == 0 ||
            func_ov003_020a31fc(event->slot) == 0)
        {
            continue;
        }
        ability = &event->ability;
        for (j = 0; j < 4; j++) {
            if (actionId != ability->slots[j].actionId) {
                continue;
            }
            switch (actionId) {
                case 6:
                case 10:
                case 11:
                case 15:
                case 33:
                    return ability->slots[j].amount;
                default:
                    total += ability->slots[j].amount;
                    break;
            }
        }
    }
    return total;
}

s32 func_ov003_020a3468(s32 actionId) {
    return func_ov003_020a3360(actionId, 0);
}

s32 func_ov003_020a3478(s32 actionId) {
    return func_ov003_020a3360(actionId, 1);
}

s32 func_ov003_020a3488(s32 actionId) {
    return func_ov003_020a3360(actionId, 2);
}

/// Loads the icon of touch entry `index` into `cSprite`; `arg2` selects the looped/one-shot form
/// and `arg4` is passed through to the animation setup.
// Nonmatching: the SpriteAnimationEx field stores are scheduled differently.
s32 func_ov003_020a3498(CombatSprite* cSprite, s32 arg1, s32 arg2, s32 index, u16 arg4) {
    const Ov003TouchIconParams* params = &data_ov003_020da80c[index];
    SpriteAnimationEx           anim;

    CombatSprite_InitAnim(&anim.anim, arg1, params->binIden);
    anim.anim.bits_7_9  = 6;
    anim.anim.unk_18    = params->unk_04;
    anim.anim.packIndex = params->unk_06;
    if (arg2 == 0) {
        anim.anim.bits_10_11 = 1;
        anim.anim.unk_1C     = params->entry.charDataIndex;
        anim.anim.unk_1E     = params->unk_14 << 5;
        anim.anim.unk_26     = params->entry.frameDataIndex;
        anim.anim.unk_28     = params->entry.paletteDataIndex;
        anim.anim.unk_20     = params->unk_12;
        anim.anim.animIndex  = params->entry.animDataIndex + 1;
        anim.animTable       = &params->entry;
    } else {
        anim.anim.bits_10_11 = 0;
        anim.anim.unk_1C     = params->entry.charDataIndex;
        anim.anim.unk_26     = params->entry.frameDataIndex;
        anim.anim.unk_28     = params->entry.paletteDataIndex;
        anim.anim.unk_20     = params->unk_12;
        anim.anim.animIndex  = 1;
        anim.animTable       = NULL;
    }
    if (CombatSprite_Load(cSprite, &anim) == 0) {
        return 0;
    }
    CombatSprite_SetAnim(cSprite, params->unk_10, arg4);
    return 1;
}

void func_ov003_020a35b8(CombatSprite* cSprite, s32 index, s32 arg2) {
    const Ov003TouchIconParams* params = &data_ov003_020da80c[index];

    CombatSprite_SetAnim(cSprite, params->unk_10, arg2);
}

/// Drops every touch action actor of the battle.
void func_ov003_020a35d8(void) {
    data_ov003_020e71b8->touchActorCount = 0;
    func_ov003_020849ac(&data_ov003_020e71b8->unk3D8BC);
}

/// Publishes the actor positions to the 3D layer's slot table.
// Nonmatching: the `MainData` slot table base is two adds instead of a folded pool word.
void func_ov003_020a3608(void) {
    Ov003TouchSlotTable* table = &gSaveData.battleTouchSlots;
    Ov003TouchActor*     actor;

    actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC);
    MI_CpuSet(table->kind, 0, 6);
    if (actor != NULL && actor->kind == 0xFFFE) {
        table->playerX = actor->x >> 12;
        table->playerY = actor->y >> 12;
        actor          = func_ov003_02084984(&actor->node);
    }
    if (actor == NULL) {
        return;
    }
    do {
        if (!(actor->effect.state == 0x3C && actor->state == 3)) {
            table->x[actor->slot] = actor->x >> 12;
            table->y[actor->slot] = actor->y >> 12;
        }
        if (data_ov003_020e71b8->unk3D890 != 0xD) {
            table->kind[actor->slot] = 0;
        } else {
            table->kind[actor->slot]   = actor->state;
            table->unk_14[actor->slot] = actor->unk_044;
            table->unk_20[actor->slot] = actor->unk_048;
        }
        actor = func_ov003_02084984(&actor->node);
    } while (actor != NULL);
}

/// Queues one touch event for every equipped pin of the current deck (`arg0` selects the deck
/// of the active player).
void func_ov003_020a3728(s32 arg0) {
    RawPinData pin;
    u16        i;

    for (i = 0; i < 6; i++) {
        u16 pinId = func_ov003_020a4718(arg0, i)->pinID;

        if (pinId != 0xFFFF) {
            func_ov003_020a474c(pinId, &pin);
            if (func_ov003_020a6cc4(pin.brand) == 0 && pin.abilityId != 0xFF) {
                func_ov003_020a30d4(pin.abilityId, -1, i);
            }
        }
    }
}

/// Spawns the touch icon task and queues one action per equipped pin of the current deck.
// Nonmatching: the psych-id flag switch lowers to a two-level table in the target.
void func_ov003_020a37a8(s32 arg0) {
    Ov003PinAction action;
    RawPinData     pin;
    u16            i;

    EasyTask_CreateTask(&data_ov003_020e71b8->unk_10118, &data_ov003_020d7d24, NULL, 0, NULL, NULL);
    for (i = 0; i < 6; i++) {
        u16                pinId = func_ov003_020a4718(arg0, i)->pinID;
        const EquippedPin* equipped;

        if (pinId == 0xFFFF) {
            continue;
        }
        equipped = func_ov003_020a4718(arg0, i);
        func_ov003_020a474c(pinId, &pin);
        action.pinId  = pinId;
        action.level  = equipped->flags.bits.level;
        action.unk_04 = equipped->unk_08;
        action.pin    = &pin;
        action.slot   = i;
        switch (pin.durationType) {
            case 2:
                func_ov003_020a7e68(&action);
                break;
            case 1:
            case 3:
                func_ov003_020a7640(&action);
                break;
            default:
                func_ov003_020a6d24(&action);
                break;
        }
        switch (pin.psychId) {
            case 9:
            case 14:
            case 18:
            case 40:
                data_ov003_020e71b8->touchWork.flags |= 2;
                break;
            case 15:
            case 17:
            case 26:
                data_ov003_020e71b8->touchWork.flags |= 0x20;
                break;
        }
    }
    func_ov003_020a3924();
}

/// Runs one update of every touch action actor.
static void func_ov003_020a3924(void) {
    Ov003TouchActor* actor;

    actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC);
    func_ov003_020a4158();
    while (actor != NULL) {
        if (actor->kind != 0xFFFE) {
            func_ov003_020a45d4(actor);
        }
        actor = func_ov003_02084984(&actor->node);
    }
}

/// TRUE while at least one non-player actor is alive.
s32 func_ov003_020a3980(void) {
    Ov003TouchActor* actor;

    for (actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC); actor != NULL; actor = func_ov003_02084984(&actor->node))
    {
        if (actor->kind != 0xFFFE && actor->unk_00E != 2) {
            return 1;
        }
    }
    return 0;
}

/// Snaps every actor back onto its starting slot position.
static void func_ov003_020a39dc(void) {
    Ov003TouchActor* actor;

    for (actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC); actor != NULL; actor = func_ov003_02084984(&actor->node))
    {
        actor->x = data_0205c19d[actor->slot * 2] << 12;
        actor->y = data_0205c19d[actor->slot * 2 + 1] << 12;
    }
}

/// Clears the "actor is being dragged" flag of every actor.
void func_ov003_020a3a40(void) {
    Ov003TouchActor* actor;

    for (actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC); actor != NULL; actor = func_ov003_02084984(&actor->node))
    {
        actor->unk_04C = 0;
    }
}

/// Picks the actor under the touch/stylus and moves it there; SELECT resets the positions.
void func_ov003_020a3a80(void) {
    Ov003TouchActor* actor;
    Ov003TouchActor* selected = NULL;

    if (InputStatus.buttonState.pressedButtons & INPUT_BUTTON_SELECT) {
        func_ov003_020a39dc();
        return;
    }
    for (actor = func_ov003_0208495c(&data_ov003_020e71b8->unk3D8BC); actor != NULL; actor = func_ov003_02084984(&actor->node))
    {
        s32 active = func_ov003_020a3e50(actor);

        if (actor->unk_04C != 0) {
            if (active != 0 && func_ov003_02085cfc(6) != 0) {
                selected = actor;
            } else {
                actor->unk_04C = 0;
            }
        }
        if (selected == NULL && func_ov003_02085cfc(6) != 0 && active != 0) {
            if (func_ov003_020a410c(actor, data_ov003_020e71b8->touchWork.x, data_ov003_020e71b8->touchWork.y) != 0) {
                selected       = actor;
                actor->unk_04C = 1;
            }
        }
    }
    if (selected != NULL) {
        selected->x = data_ov003_020e71b8->touchWork.x << 12;
        selected->y = data_ov003_020e71b8->touchWork.y << 12;
    }
    func_ov003_020a4158();
}
