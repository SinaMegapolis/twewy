#ifndef COMBAT_CORE_COMBAT_H
#define COMBAT_CORE_COMBAT_H

#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Combat/Core/CombatPsych.h"
#include "Engine/EasyTask.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Save/MainData.h"
#include "SndMgrSeIdx.h"

/// The camera shake animation driven through the battle camera views.
typedef struct Ov003CameraAnim {
    /* 0x00 */ s32          index; // which of the two cameras
    /* 0x04 */ BattleStage* camera;
    /* 0x08 */ s32          timer; // -1 = idle
    /* 0x0C */ s32          period;
    /* 0x10 */ s32          dir;   // 1 = negative offset
    /* 0x14 */ s32          offset;
} Ov003CameraAnim;                 // Size: 0x18

// Used in TaskStage.render functions
void func_ov003_020c4878(void*);

void func_ov003_020c48fc(void*);

// Creates a psych task and forwards its ID (-1 on allocation/initialization failure).
s32 func_ov003_020a6000(const TaskHandle* handle, void* arg);

/// One 8-byte record of an `Ov003AnimLayer`'s frame table.
typedef struct Ov003AnimFrame {
    /* 0x00 */ u16 dataId;
    /* 0x02 */ s16 entry;
    /* 0x04 */ u16 duration;
    /* 0x06 */ s16 randomRange;
} Ov003AnimFrame; // Size: 0x8

/// One layer of a layered sprite animation: a chain of slots that share a frame index.
typedef struct Ov003AnimLayer {
    /* 0x00 */ BinIdentifier*        iden;
    /* 0x04 */ Data*                 data;
    /* 0x08 */ s32                   unk_08;
    /* 0x0C */ void**                dest; // receives the current cell pointer
    /* 0x10 */ const Ov003AnimFrame* frames;
    /* 0x14 */ s16                   loadedId;
    /* 0x16 */ s16                   index;
    /* 0x18 */ s16                   duration;
    /* 0x1A */ s16                   timer;
    union {
        /* 0x1C */ struct {
            u16 loop    : 1; // restart from frame 0 at the end
            u16 oneShot : 1; // reached the end and stopped
            u16 random  : 1; // `duration` of 0 means "pick a random one in [1, randomRange]"
            u16 chained : 1; // driven by a parent layer
            u16 ended   : 1; // the end marker was already hit once
            u16 _pad1C  : 11;
        };
        u16 flags;
    };
    /* 0x1E */ s16                    unk_1E;
    /* 0x20 */ struct Ov003AnimLayer* next;
} Ov003AnimLayer; // Size: 0x24

typedef struct Ov003CutinSaveState {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u32 slots[9];
    /* 0x28 */ s32 unk_28;
    /* 0x2C */ s32 unk_2C;
    /* 0x30 */ u32 magic;
} Ov003CutinSaveState; // Size: 0x34

extern Ov003CutinSaveState gCombatCutinSave;

#endif // COMBAT_CORE_COMBAT_H
