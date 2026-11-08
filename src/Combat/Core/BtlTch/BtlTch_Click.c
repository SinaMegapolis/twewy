#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include <nitro/mi/cpumem.h>

// The tap burst: a one-shot sprite where the stylus was lifted, bigger for longer tap chains.

typedef struct BtlTchClick {
    /* 0x00 */ CombatSprite sprite;
    /* 0x60 */ BOOL         isActive;
} BtlTchClick; // Size: 0x64

static s32 BtlTch_Click_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlTch_Click = {"Tsk_BtlTch_Click", BtlTch_Click_RunTask, sizeof(BtlTchClick)};

static s32 BtlTch_Click_Init(TaskPool* pool, Task* task, void* args) {
    BtlTchClick*  effect = task->data;
    BtlTouchWork* input  = &g_CombatGlobal->touchWork;

    MI_CpuSet(effect, 0, sizeof(*effect));
    effect->isActive = TRUE;
    CombatSprite_LoadDirect(2, &effect->sprite, &Iden_BtlTch, 1, 3, 2, 4, 1);
    if (input->tapCount < 1) {
        CombatSprite_SetAnim(&effect->sprite, 1, 1);
    } else if (input->tapCount < 5) {
        CombatSprite_SetAnim(&effect->sprite, 2, 1);
    } else {
        CombatSprite_SetAnim(&effect->sprite, 3, 1);
    }
    CombatSprite_SetPosition(&effect->sprite, input->endX, input->endY);
    func_ov003_02082738(&effect->sprite, 0);
    func_ov003_02082730(&effect->sprite, 0x3F2000);
    return 1;
}

static s32 BtlTch_Click_Update(TaskPool* pool, Task* task, void* args) {
    BtlTchClick* effect = task->data;

    if (SpriteMgr_IsAnimationFinished(&effect->sprite.sprite)) {
        effect->isActive = FALSE;
    }
    CombatSprite_Update(&effect->sprite);
    return effect->isActive;
}

static s32 BtlTch_Click_Render(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Render(task->data);
    return 1;
}

static s32 BtlTch_Click_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
    return 1;
}

static s32 BtlTch_Click_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlTch_Click_Init,
        .update     = BtlTch_Click_Update,
        .render     = BtlTch_Click_Render,
        .cleanup    = BtlTch_Click_Release,
    };

    return stages.iter[stage](pool, task, args);
}
