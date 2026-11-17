#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/File/BinMgr.h"

#include "Combat/Core/BtlInf.h"

#include <nitro/mi/cpumem.h>

// The star panel: mirrors the charge-command state (the current charge step, its progress and
// the length of the next step) as two-digit numbers, plus a star that empties once the step
// sequence is done.

typedef struct BtlInf_Star {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ s16          starAnim;   // 0 = filled, 1 = empty
    /* 0x62 */ s16          stepAnim;   // current charge step as an animation index
    /* 0x64 */ s16          progressLo; // progress digits as animation indices, least significant first
    /* 0x66 */ s16          progressHi;
    /* 0x68 */ s16          nextLo;     // next step length digits, least significant first
    /* 0x6A */ s16          nextHi;
    /* 0x6C */ s8           step;       // last seen charge step
    /* 0x6D */ s8           progress;   // last seen progress
    /* 0x6E */ s8           next;       // last seen step length
    /* 0x6F */ char         unk_6F;
    /* 0x70 */ BOOL         isComplete; // the charge step reached the step count
} BtlInf_Star;                          // Size: 0x74

static s32 BtlInf_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlInf_Star = {"Tsk_BtlInf_Star", BtlInf_Star_RunTask, sizeof(BtlInf_Star)};

/// Splits `value` (clamped to 0..99) into two decimal digits, least significant first, and
/// returns how many digits are needed to show it.
static s32 func_ov003_0209d51c(s32* digits, s32 value) {
    s32 v = value;

    if (v < 0) {
        v = 0;
    }
    if (v > 99) {
        v = 99;
    }

    digits[1] = v / 10;
    digits[0] = v % 10;

    if (v >= 10) {
        return 2;
    }
    return 1;
}

/// Draws `sprite` with animation `anim` at (`posX`, `posY`).
static void func_ov003_0209d570(BattleSprite* sprite, u16 anim, s32 posX, s32 posY) {
    BtlSprite_SetAnim(sprite, anim, 0);
    BtlSprite_SetPosition(sprite, posX, posY);
    BtlSprite_Update(sprite);
    BtlSprite_Render(sprite);
}

/// Writes the animation indices of `value`'s two decimal digits to `digits`.
static void func_ov003_0209d5ac(s16* digits, s32 value) {
    s32 split[2];

    func_ov003_0209d51c(split, value);
    digits[0] = (s16)(split[0] + 2);
    digits[1] = (s16)(split[1] + 2);
}

/// Refreshes every element of the panel whose charge-command value changed.
static void func_ov003_0209d5e0(BtlInf_Star* star) {
    BattleCmdDeck* charge = gBattle->cmdDeck;

    if (star->step != charge->stars) {
        star->step     = charge->stars;
        star->stepAnim = (s16)(star->step + 2);
        if (star->step >= charge->maxStars) {
            star->isComplete = TRUE;
            star->starAnim   = 1;
        } else {
            star->isComplete = FALSE;
            star->starAnim   = 0;
        }
    }
    if (charge->progress != star->progress) {
        star->progress = charge->progress;
        func_ov003_0209d5ac(&star->progressLo, star->progress);
    }
    if (charge->threshold != star->next) {
        star->next = charge->threshold;
        func_ov003_0209d5ac(&star->nextLo, star->next);
    }
}

/// Whether the charge command asked for the panel to be hidden.
static BOOL func_ov003_0209d688(void) {
    if (gBattle->cmdDeck->flags & BTL_CMD_DECK_FLAG_DISABLED) {
        return TRUE;
    }
    return FALSE;
}

static s32 BtlInf_Star_Init(TaskPool* pool, Task* task, void* args) {
    BtlInf_Star* star = task->data;

    MI_CpuSet(star, 0, sizeof(BtlInf_Star));
    star->step       = 0;
    star->progress   = 0;
    star->next       = 0;
    star->isComplete = FALSE;

    BtlSprite_LoadDirect(1, &star->sprite, &Iden_BtlMenu, 0x19, 0x1B, 0x1A, 0x3F, 0);
    BtlSprite_SetPriority(&star->sprite, 0);

    star->starAnim = 0;
    star->stepAnim = (s16)(star->step + 2);
    func_ov003_0209d5ac(&star->progressLo, star->progress);
    func_ov003_0209d5ac(&star->nextLo, star->next);
    return 1;
}

static s32 BtlInf_Star_Update(TaskPool* pool, Task* task, void* args) {
    BtlInf_Star* star = task->data;

    if (func_ov003_0209d688()) {
        return 1;
    }
    func_ov003_0209d5e0(star);
    return 1;
}

static s32 BtlInf_Star_Render(TaskPool* pool, Task* task, void* args) {
    BtlInf_Star* star = task->data;

    if (func_ov003_0209d688()) {
        return 1;
    }
    if (star->isComplete == FALSE) {
        func_ov003_0209d570(&star->sprite, star->progressHi, 110, 1);
        func_ov003_0209d570(&star->sprite, star->progressLo, 115, 1);
        func_ov003_0209d570(&star->sprite, star->nextHi, 126, 1);
        func_ov003_0209d570(&star->sprite, star->nextLo, 131, 1);
    }
    func_ov003_0209d570(&star->sprite, star->stepAnim, 150, 1);
    func_ov003_0209d570(&star->sprite, star->starAnim, 128, 0);
    return 1;
}

static s32 BtlInf_Star_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlInf_Star_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlInf_Star_Init,
        .update     = BtlInf_Star_Update,
        .render     = BtlInf_Star_Render,
        .cleanup    = BtlInf_Star_Release,
    };
    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}
