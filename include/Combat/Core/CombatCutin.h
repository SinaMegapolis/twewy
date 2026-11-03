#ifndef COMBAT_COMBATCUTIN_H
#define COMBAT_COMBATCUTIN_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatActorParams.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/BgResMgr.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ScreenMapper.h"

// Cut-in tile entries store a size word before their tile payload.
typedef struct Ov003CutinTileBlock {
    u32 size;
    u8  tiles[];
} Ov003CutinTileBlock;

typedef struct Ov003CutinPaletteEntries {
    s32 entries[3];
} Ov003CutinPaletteEntries;

static inline void* Ov003Cutin_Tiles(Data* pack, s32 index) {
    return ((Ov003CutinTileBlock*)Data_GetPackEntryData(pack, index))->tiles;
}

typedef struct Ov003CutinActor {
    /* 0x000 */ Ov003EnemyActor base;
    /* 0x1C0 */ u8              unk_1C0[0x1D0 - 0x1C0];
    /* 0x1D0 */ s32             velocityX;
    /* 0x1D4 */ s32             velocityY;
    /* 0x1D8 */ s32             velocityZ;
    /* 0x1DC */ s32             targetX;
    /* 0x1E0 */ s32             targetY;
    /* 0x1E4 */ s32             targetZ;
    /* 0x1E8 */ s32             unk_1E8;
    /* 0x1EC */ s32             timer;
    /* 0x1F0 */ s32             timerLimit;
    /* 0x1F4 */ s32             state;
} Ov003CutinActor; // Size: 0x1F8

typedef union Ov003CutinFlags8 {
    /* 0x0 */ u8 raw;
    struct {
        /* bit 0 */ u8     b0   : 1;
        /* bit 1 */ u8     b1   : 1;
        /* bit 2 */ u8     b2   : 1;
        /* bit 3 */ u8     b3   : 1;
        /* bits 4..7 */ u8 rest : 4;
    } bits;
} Ov003CutinFlags8; // Size: 0x1

typedef struct Ov003CutinGlobal {
    /* 0x00 */ u16              unk_00;
    /* 0x02 */ u8               unk_02;
    /* 0x03 */ u8               unk_03;
    /* 0x04 */ Ov003CutinFlags8 flags;
    /* 0x05 */ u8               unk_05[0x8 - 0x5];
    /* 0x08 */ void*            taskMain; // main-engine cut-in task data
    /* 0x0C */ void*            taskSub;
    /* 0x10 */ void*            unk_10;
    /* 0x14 */ void*            cameraMain;
    /* 0x18 */ void*            cameraSub;
    /* 0x1C */ void*            unk_1C;
} Ov003CutinGlobal; // Size: 0x20

extern u8 data_ov003_020e71e4[];
#define g_Cutin ((Ov003CutinGlobal*)data_ov003_020e71e4)

typedef struct Ov003ActorSnapshot {
    /* 0x00 */ Ov003EnemyActor* unit;
    /* 0x04 */ CombatActor      actor;
    /* 0x80 */ u8               reserved[0x1C];
} Ov003ActorSnapshot; // Size: 0x9C

typedef struct Ov003ActorSnapshots {
    /* 0x00 */ u16                 mainCount;
    /* 0x02 */ u16                 subCount;
    /* 0x04 */ Ov003ActorSnapshot* sub;
    /* 0x08 */ Ov003ActorSnapshot* main;
} Ov003ActorSnapshots; // Size: 0xC

typedef void (*Ov003SnapshotCallback)(s32, Ov003ActorSnapshot*, void*);

void func_ov003_020bd020(void);
void func_ov003_020bd078(void);
void func_ov003_020bd114(void);
void func_ov003_020bd150(s32 mode, Ov003SnapshotCallback callback, void* args);

/// One task-spawn record produced while parsing a cut-in script; also the data block the
/// EasyTask callbacks receive.
typedef struct Ov003CutinCmdData {
    /* 0x00 */ s32 group;
    /* 0x04 */ u16 sprite;
    /* 0x06 */ union {
        u16             flags;
        Ov003CutinFlags spawnFlags;
    };
    /* 0x08 */ s32 x;
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
} Ov003CutinCmdData; // Size: 0x14

typedef struct Ov003CutinScript {
    /* 0x00 */ void* data;
    /* 0x04 */ u16   count;
    /* 0x06 */ u16   flags;
    /* 0x08 */ s32   cursor;
    /* 0x0C */ s32   unk_0C;
    /* 0x10 */ s32   unk_10;
} Ov003CutinScript; // Size: 0x14

/// A circular buffer of motion tracks plus the playback cursor.
typedef struct Ov003CutinTracks {
    /* 0x00 */ Vec* tracks;
    /* 0x04 */ u16  count;
    /* 0x06 */ u16  cursor;
} Ov003CutinTracks; // Size: 0x8

s32  BtlCutIn_GetSnapshotScreenX(s32 screen, Ov003ActorSnapshot* entry);
s32  BtlCutIn_GetSnapshotScreenY(s32 screen, Ov003ActorSnapshot* entry);
void BtlCutIn_SetSnapshotScreenX(s32 screen, Ov003ActorSnapshot* entry, s32 x);
void BtlCutIn_SetSnapshotScreenY(s32 screen, Ov003ActorSnapshot* entry, s32 y);
void BtlCutIn_SetSpecialKind(s32 kind);
s32  BtlCutIn_GetSpecialKind(void);
void BtlCutIn_PlayFirstVoice(s32 kind, s32 level);
void BtlCutIn_PlaySecondVoice(s32 kind, s32 level);

#endif
