#include "Combat/Core/BtlTch.h"
#include "Battle/Core/BtlSprite.h"
#include "Battle/Core/BtlTouch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

// The stylus cursor icon: a sprite on the touch screen that follows the stylus while it is down.

static s32 BtlTch_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Task_BtlTch = {"Task_BtlTch", BtlTch_RunTask, sizeof(BtlIcon)};

static void BtlTch_UpdateHold(BtlIcon* icon);
static void BtlTch_UpdateDrag(BtlIcon* icon);

static void BtlTch_SetUpdate(BtlIcon* icon, void (*update)(BtlIcon*)) {
    icon->update = update;
}

/// Switches the cursor animation whenever the recognised touch gesture changes.
static void BtlTch_FollowGesture(BtlIcon* icon) {
    if (icon->animState == g_CombatGlobal->touchWork.gesture) {
        return;
    }

    icon->animState = g_CombatGlobal->touchWork.gesture;

    switch (icon->animState) {
        case 5:
            BtlSprite_SetAnim(&icon->sprite, 4, 1);
            BtlTch_SetUpdate(icon, BtlTch_UpdateHold);
            icon->unk_6C = 1;
            return;

        case 6:
        case 7:
            BtlTch_SetUpdate(icon, BtlTch_UpdateDrag);
            icon->unk_6C = 1;
            return;

        case 1:
            BtlSprite_SetAnim(&icon->sprite, 0, 0);
            BtlTch_SetUpdate(icon, NULL);
            icon->unk_6C = 1;
            return;

        default:
            BtlTch_SetUpdate(icon, NULL);
            icon->unk_6C = 0;
            return;
    }
}

/// Plays the hold animation to its end, then loops the held pose.
static void BtlTch_UpdateHold(BtlIcon* icon) {
    if (!SpriteMgr_IsAnimationFinished(&icon->sprite.sprite)) {
        return;
    }
    BtlSprite_SetAnim(&icon->sprite, 5, 0);
    BtlTch_SetUpdate(icon, NULL);
}

/// Picks the drag animation from how long the stylus has been scribbling.
static void BtlTch_UpdateDrag(BtlIcon* icon) {
    if (g_CombatGlobal->touchWork.sweepFrames < 2) {
        BtlSprite_SetAnim(&icon->sprite, 6, 0);
    } else if (g_CombatGlobal->touchWork.sweepFrames < 8) {
        BtlSprite_SetAnim(&icon->sprite, 7, 0);
    } else {
        BtlSprite_SetAnim(&icon->sprite, 8, 0);
    }
}

static s32 BtlTch_Init(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    MI_CpuSet(icon, 0, sizeof(*icon));
    icon->unk_6C    = 0;
    icon->animState = 0;
    BtlTch_SetUpdate(icon, NULL);
    BtlSprite_LoadDirect(2, &icon->sprite, &Iden_BtlTch, 1, 3, 2, 4, 0);
    BtlSprite_SetAnim(&icon->sprite, 0, 0);
    BtlSprite_SetPriority(&icon->sprite, 0);
    BtlSprite_SetSortKey(&icon->sprite, 0x3F2000);
    return 1;
}

static s32 BtlTch_Update(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    BtlTch_FollowGesture(icon);
    if (icon->update != NULL) {
        icon->update(icon);
    }
    if (icon->unk_6C != 0) {
        icon->x = g_CombatGlobal->touchWork.x;
        icon->y = g_CombatGlobal->touchWork.y;
        BtlSprite_Update(&icon->sprite);
    }
    return 1;
}

static s32 BtlTch_Render(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    if (icon->unk_6C != 0) {
        BtlSprite_SetPosition(&icon->sprite, icon->x, icon->y);
        BtlSprite_Render(&icon->sprite);
    }
    return 1;
}

static s32 BtlTch_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlTch_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlTch_Init,
        .update     = BtlTch_Update,
        .render     = BtlTch_Render,
        .cleanup    = BtlTch_Release,
    };

    return stages.iter[stage](pool, task, args);
}
