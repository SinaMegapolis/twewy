#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/File/BinMgr.h"
#include "main_02026528.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The technique popup: a task that floats the tech id of the last used psych above the main
// actor. The value is split into its decimal digits (`func_ov003_0209d088`), each digit picks a
// sprite animation, and the popup rises towards the actor before awarding the exp.

typedef struct BtlPop_Tech {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ s16          digits[4]; // least significant first, each the animation `digit + 20`
    /* 0x68 */ s16          digitCount;
    /* 0x6A */ s16          techId;
    /* 0x6C */ s32          x; // world position of the main actor when the popup spawned
    /* 0x70 */ s32          y;
    /* 0x74 */ s32          z; // current height, interpolated towards `targetZ`
    /* 0x78 */ s32          targetZ;
    /* 0x7C */ s16          state;
    /* 0x7E */ s16          timer;
} BtlPop_Tech; // Size: 0x80

typedef struct BtlPop_TechArgs {
    /* 0x0 */ s16 techId;
} BtlPop_TechArgs;

static s32 BtlPop_Tech_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_Tech = {"Tsk_BtlPop_Tech", BtlPop_Tech_RunTask, sizeof(BtlPop_Tech)};

extern const BinIdentifier data_ov003_020d7800;

/// Splits `value` (clamped to 0..9999) into decimal digits, least significant first, and returns
/// how many digits are needed to show it.
static s32 func_ov003_0209d088(s32* digits, s32 value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 9999) {
        value = 9999;
    }

    digits[3] = value / 1000;
    s32 rest  = value % 1000;
    digits[2] = rest / 100;
    rest      = rest % 100;
    digits[1] = rest / 10;
    digits[0] = rest % 10;

    if (value >= 1000) {
        return 4;
    }
    if (value >= 100) {
        return 3;
    }
    if (value >= 10) {
        return 2;
    }
    return 1;
}

/// Loads the popup's sprite and starts it on its first animation.
static void func_ov003_0209d154(BattleSprite* sprite, s32 variant) {
    BtlSprite_LoadDirectNoProto(0, sprite, &data_ov003_020d7800, 0x25, 0x27, 0x26, 0x3D, variant);
    BtlSprite_SetPriority(sprite, 0);
    BtlSprite_SetSortKey(sprite, 1);
}

static s32 BtlPop_Tech_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_Tech*     tech   = task->data;
    BtlPop_TechArgs* params = args;
    s32              digits[4];

    MI_CpuSet(tech, 0, sizeof(BtlPop_Tech));

    tech->techId     = params->techId;
    tech->digitCount = func_ov003_0209d088(digits, params->techId);
    tech->x          = g_CombatGlobal->player->position.x;
    tech->y          = g_CombatGlobal->player->position.y;
    tech->z          = g_CombatGlobal->player->position.z - (g_CombatGlobal->player->centerHeight << 12);
    tech->targetZ    = tech->z - 0x20000;
    tech->state      = 0;
    tech->timer      = 0;

    func_ov003_0209d154(&tech->sprite, 0);
    for (s32 i = 0; i < 4; i++) {
        tech->digits[i] = digits[i] + 20;
    }
    return 1;
}

static s32 BtlPop_Tech_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_Tech* tech = task->data;

    switch (tech->state) {
        case 0: {
            if (tech->timer == 0) {
                tech->timer = 30;
            }
            func_020265d4(&tech->z, tech->targetZ, (u16)tech->timer);
            tech->timer--;
            if (tech->timer <= 0) {
                tech->state++;
                tech->timer = 0;
            }
        } break;

        case 1: {
            if (tech->timer == 0) {
                tech->timer = 8;
            }
            tech->timer--;
            if (tech->timer <= 0) {
                BtlMgr_AddExp(tech->techId);
                return 0;
            }
        } break;
    }
    return 1;
}

// Nonmatching: regalloc
static s32 BtlPop_Tech_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_Tech* tech = task->data;
    s32          posX = BtlStage_ToScreenX(0, tech->x);
    s16          posY = (s16)(BtlStage_ToScreenY(0, tech->y, tech->z) - 8);
    s32          half = (tech->digitCount * 7) / 2 - 7;
    s32          offset;
    s32          i;
    s32          right;

    BtlSprite_SetAnim(&tech->sprite, 0x35, 0);
    BtlSprite_Update(&tech->sprite);

    right = half + posX;
    BtlSprite_SetPosition(&tech->sprite, (s16)(right - (tech->digitCount - 1) * 7), posY);
    BtlSprite_Render(&tech->sprite);

    offset = 0;
    for (i = 0; i < tech->digitCount; i++) {
        BtlSprite_SetAnim(&tech->sprite, tech->digits[i], 0);
        BtlSprite_Update(&tech->sprite);
        BtlSprite_SetPosition(&tech->sprite, (s16)(right - offset), posY);
        BtlSprite_Render(&tech->sprite);
        offset += 7;
    }
    return 1;
}

static s32 BtlPop_Tech_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlPop_Tech_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_Tech_Init,
        .update     = BtlPop_Tech_Update,
        .render     = BtlPop_Tech_Render,
        .cleanup    = BtlPop_Tech_Release,
    };
    if (func_ov003_0208b690(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

void func_ov003_0209d4b0(void) {
    if (g_CombatGlobal->unk3D8FA > 0) {
        BtlPop_TechArgs params = {g_CombatGlobal->unk3D8FA};
        EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlPop_Tech, NULL, 0, NULL, &params);
        g_CombatGlobal->unk3D8FA = 0;
    }
}
