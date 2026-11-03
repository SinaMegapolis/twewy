#include "Combat/Core/BtlTch.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatSprite.h"
#include "Combat/Core/CombatTouch.h"
#include <nitro/mi/cpumem.h>

// The stylus cursor icon: a sprite on the touch screen that follows the stylus while it is down.

extern const BinIdentifier data_ov003_020d6fac;

static s32 BtlTch_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Task_BtlTch = {"Task_BtlTch", BtlTch_RunTask, sizeof(BtlIcon)};

static s32 BtlTch_Init(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    MI_CpuSet(icon, 0, sizeof(*icon));
    icon->unk_6C    = 0;
    icon->animState = 0;
    func_ov003_0208ecb4(icon, NULL);
    CombatSprite_LoadDirect(2, &icon->sprite, &data_ov003_020d6fac, 1, 3, 2, 4, 0);
    CombatSprite_SetAnim(&icon->sprite, 0, 0);
    func_ov003_02082738(&icon->sprite, 0);
    func_ov003_02082730(&icon->sprite, 0x3F2000);
    return 1;
}

static s32 BtlTch_Update(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    func_ov003_0208ecbc(icon);
    if (icon->update != NULL) {
        icon->update(icon);
    }
    if (icon->unk_6C != 0) {
        icon->x = data_ov003_020e71b8->touchWork.x;
        icon->y = data_ov003_020e71b8->touchWork.y;
        CombatSprite_Update(&icon->sprite);
    }
    return 1;
}

static s32 BtlTch_Render(TaskPool* pool, Task* task, void* args) {
    BtlIcon* icon = task->data;

    if (icon->unk_6C != 0) {
        CombatSprite_SetPosition(&icon->sprite, icon->x, icon->y);
        CombatSprite_Render(&icon->sprite);
    }
    return 1;
}

static s32 BtlTch_Release(TaskPool* pool, Task* task, void* args) {
    CombatSprite_Release(task->data);
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
