#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/Core/OamMgr.h"
#include <nitro/mi/cpumem.h>

// The slash streak: a stretched, rotated sprite along a stylus slash that fades out, with a sound
// picked by the slash length.

typedef struct BtlTchSlash {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ s16          x;
    /* 0x62 */ s16          y;
    /* 0x64 */ s32          scaleX;
    /* 0x68 */ s32          scaleY;
    /* 0x6C */ u16          angle;
    /* 0x6E */ s8           alpha;
    /* 0x6F */ u8           pad_6F;
    /* 0x70 */ s32          fade;
    /* 0x74 */ BOOL         isActive;
} BtlTchSlash; // Size: 0x78

static s32 BtlTch_Slash_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlTch_Slash = {"Tsk_BtlTch_Slash", BtlTch_Slash_RunTask, sizeof(BtlTchSlash)};

static s32 BtlTch_Slash_Init(TaskPool* pool, Task* task, void* args) {
    BtlTchSlash* effect = task->data;
    BattleTouch* input  = &gBattle->touchWork;

    MI_CpuSet(effect, 0, sizeof(*effect));
    effect->isActive = TRUE;
    effect->x        = input->startX;
    effect->y        = input->startY;
    effect->scaleX   = 0x1000;
    effect->scaleY   = input->distance / 64;
    effect->angle    = input->angle + 0xC000;
    effect->alpha    = 31;
    effect->fade     = 0x1000;
    BtlSprite_LoadDirect(2, &effect->sprite, &Iden_BtlTch, 1, 3, 2, 4, 9);
    BtlSprite_SetAnim(&effect->sprite, 9, 1);
    BtlSprite_SetPosition(&effect->sprite, effect->x, effect->y);
    BtlSprite_SetPriority(&effect->sprite, 0);
    BtlSprite_SetSortKey(&effect->sprite, 0x3F2000);
    if (effect->scaleY < 0x800) {
        BtlSe_PlayPanned(SEIDX_SE_PENEFFECTS, effect->x);
    } else if (effect->scaleY < 0x1000) {
        BtlSe_PlayPanned(SEIDX_SE_PENEFFECTM, effect->x);
    } else {
        BtlSe_PlayPanned(SEIDX_SE_PENEFFECTL, effect->x);
    }
    return 1;
}

static s32 BtlTch_Slash_Update(TaskPool* pool, Task* task, void* args) {
    BtlTchSlash* effect = task->data;

    effect->alpha = (effect->fade * 31) >> 12;
    effect->fade -= 123;
    if (effect->alpha < 0) {
        effect->alpha = 0;
    }
    if (SpriteMgr_IsAnimationFinished(&effect->sprite.sprite)) {
        effect->isActive = FALSE;
    }
    BtlSprite_Update(&effect->sprite);
    return effect->isActive;
}

// Nonmatching: affine-index and bitfield temporaries use different registers.
static s32 BtlTch_Slash_Render(TaskPool* pool, Task* task, void* args) {
    BtlTchSlash* effect = task->data;
    u16 affine = OamMgr_AllocAffineGroup(&g_OamMgr[DISPLAY_EXTENDED], effect->angle, effect->scaleX, effect->scaleY, 0);

    effect->sprite.sprite.unk_0A.unk_00 = 1;
    effect->sprite.sprite.unk_0A.unk_05 = affine;
    BtlSprite_RenderTranslucent(&effect->sprite, effect->alpha);
    return 1;
}

static s32 BtlTch_Slash_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlTch_Slash_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlTch_Slash_Init,
        .update     = BtlTch_Slash_Update,
        .render     = BtlTch_Slash_Render,
        .cleanup    = BtlTch_Slash_Release,
    };

    return stages.iter[stage](pool, task, args);
}
