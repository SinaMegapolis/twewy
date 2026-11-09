#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "main_02026528.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The short message popup: a single sprite that slides down towards its target Z, fades in,
// then fades out.

typedef struct BtlPop_Message {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ s32          x;     // world position the popup tracks
    /* 0x64 */ s32          y;
    /* 0x68 */ s32          z;     // display Z, eased towards `targetZ`
    /* 0x6C */ s32          targetZ;
    /* 0x70 */ s16          phase; // 0 = slide in, 1 = fade out
    /* 0x72 */ s16          timer;
    /* 0x74 */ BOOL         isActive;
} BtlPop_Message; // Size: 0x78

typedef struct BtlPop_MessageArgs {
    /* 0x00 */ s32 engine;   // screen selector, also the sprite loader flag
    /* 0x04 */ u16 animBase; // added to the base animation index of the message sprite
    /* 0x08 */ s32 x;
    /* 0x0C */ s32 y;
    /* 0x10 */ s32 z;
} BtlPop_MessageArgs; // Size: 0x14

static s32 BtlPop_Message_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_Message = {"Tsk_BtlPop_Message", BtlPop_Message_RunTask, sizeof(BtlPop_Message)};

extern BinIdentifier data_ov003_020d7800;

static s32 BtlPop_Message_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Message*     message = task->data;
    BtlPop_MessageArgs* params  = args;
    s32                 animIndex;
    s32                 z;

    MI_CpuSet(message, 0, sizeof(BtlPop_Message));
    message->isActive = TRUE;
    message->x        = params->x;
    message->y        = params->y;
    z                 = params->z;
    message->z        = z;
    message->targetZ  = z - 0x20000;
    message->phase    = 0;
    message->timer    = 0;

    if (params->engine == 0) {
        animIndex = 0x3D;
        g_CombatGlobal->popupCountMain++;
    } else {
        animIndex = 0x3F;
        g_CombatGlobal->popupCountSub++;
    }
    BtlSprite_LoadDirectNoProto(params->engine, &message->sprite, &data_ov003_020d7800, 0x25, 0x27, 0x26, animIndex,
                                (u16)(params->animBase + 50));
    BtlSprite_SetPriority(&message->sprite, 0);
    BtlSprite_SetSortKey(&message->sprite, 1);
    return 1;
}

static s32 BtlPop_Message_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Message* message = task->data;

    switch (message->phase) {
        case 0: {
            if (message->timer == 0) {
                message->timer = 30;
            }
            func_020265d4(&message->z, message->targetZ, (u16)message->timer);
            message->timer--;
            if (message->timer <= 0) {
                message->phase++;
                message->timer = 0;
            }
        } break;

        case 1: {
            if (message->timer == 0) {
                message->timer = 8;
            }
            message->timer--;
            if (message->timer <= 0) {
                message->isActive = FALSE;
            }
        } break;
    }
    BtlSprite_Update(&message->sprite);
    return message->isActive;
}

static s32 BtlPop_Message_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Message* message = task->data;
    s32             posX;
    s32             posY;

    if (message->sprite.sprite.bits_0_1 == 0) {
        posX = CombatStage_ToScreenX(0, message->x);
        posY = CombatStage_ToScreenY(0, message->y, message->z);
    } else {
        posX = CombatStage_ToScreenX(1, message->x);
        posY = CombatStage_ToScreenY(1, message->y, message->z);
    }
    BtlSprite_SetPosition(&message->sprite, posX, posY);
    BtlSprite_Render(&message->sprite);
    return 1;
}

static s32 BtlPop_Message_Release(TaskPool* pool, Task* task, void* args) {
    BtlPop_Message* message = task->data;

    if (message->sprite.sprite.bits_0_1 == 0) {
        g_CombatGlobal->popupCountMain--;
    } else {
        g_CombatGlobal->popupCountSub--;
    }
    BtlSprite_Release(&message->sprite);
    return 1;
}

static s32 BtlPop_Message_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Message_Init,
        .update     = BtlPop_Message_Update,
        .render     = BtlPop_Message_Render,
        .cleanup    = BtlPop_Message_Release,
    };
    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209c984(s32 engine, s32 animBase, s32 x, s32 y, s32 z) {
    BtlPop_MessageArgs params;
    s32                popupCount;

    if (engine == 0) {
        popupCount = g_CombatGlobal->popupCountMain;
    } else {
        popupCount = g_CombatGlobal->popupCountSub;
    }

    popupCount = popupCount < 10;
    if (popupCount != 0) {
        params.engine   = engine;
        params.animBase = animBase;
        params.x        = x;
        params.y        = y;
        params.z        = z;
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Message, NULL, 0, NULL, &params);
    }
}
