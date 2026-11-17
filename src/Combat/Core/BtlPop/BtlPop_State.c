#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"

#include "Combat/Core/BtlPop.h"

#include <nitro/mi/cpumem.h>

// The battle status popup ("SD"/"ATK" style labels): a sprite that follows one battle unit and
// swaps its animation whenever the unit's periodic effect mode changes.

typedef struct BtlPop_StateArgs {
    /* 0x00 */ s32          engine; // display engine index, also the sprite's `bits_0_1`
    /* 0x04 */ BattleActor* actor;
} BtlPop_StateArgs;                 // Size: 0x8

typedef struct BtlPop_State {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ BattleActor* actor;
    /* 0x64 */ u16          effectMode; // actor->periodicEffectMode the sprite currently shows
    /* 0x66 */ u16          pad_66;
    /* 0x68 */ BOOL         isActive;
} BtlPop_State; // Size: 0x6C

static s32 BtlPop_State_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle Tsk_BtlPop_State = {"Tsk_BtlPop_State", BtlPop_State_RunTask, sizeof(BtlPop_State)};

/// One entry per periodic effect mode: the label sprites of the status icon.
static const SpriteAnimEntry data_ov003_020d80f8[5] = {
    {58, 60, 59, 0},
    {58, 60, 59, 1},
    {58, 60, 59, 2},
    {58, 60, 59, 3},
    {58, 60, 59, 4},
};

/// Picks the label sprite for the unit's current periodic effect mode. Mode 0 (no effect) hides
/// the popup; modes 1..4 map onto the four sprite table entries.
static void func_ov003_0209ca18(BtlPop_State* state) {
    if (state->effectMode == 0) {
        state->isActive = FALSE;
    } else {
        switch (state->effectMode) {
            case 1: {
                BtlSprite_SetAnimFromTable(&state->sprite, 1, 0);
            } break;

            case 2: {
                BtlSprite_SetAnimFromTable(&state->sprite, 2, 0);
            } break;

            case 3: {
                BtlSprite_SetAnimFromTable(&state->sprite, 3, 0);
            } break;

            case 4: {
                BtlSprite_SetAnimFromTable(&state->sprite, 0, 0);
            } break;
        }
    }
}

// Nonmatching: argument scheduling
static s32 BtlPop_State_Init(TaskPool* pool, Task* task, void* args) {
    BtlPop_State*     state  = task->data;
    BtlPop_StateArgs* params = args;

    MI_CpuSet(state, 0, sizeof(BtlPop_State));
    state->actor      = params->actor;
    state->effectMode = state->actor->periodicEffectMode;
    state->isActive   = TRUE;
    BtlSprite_LoadFromTableNoProto(params->engine, &state->sprite, &Iden_BtlMenu, data_ov003_020d80f8, 0,
                                   params->engine == 0 ? 0x3D : 0x3F, 4);
    BtlSprite_SetPriority(&state->sprite, 0);
    func_ov003_0209ca18(state);
    return 1;
}

static s32 BtlPop_State_Update(TaskPool* pool, Task* task, void* args) {
    BtlPop_State* state = task->data;

    if (state->actor->flags & ACTOR_FLAG_DEFEATED) {
        state->isActive = FALSE;
    }
    if (state->effectMode != state->actor->periodicEffectMode) {
        state->effectMode = state->actor->periodicEffectMode;
        func_ov003_0209ca18(state);
    }
    BtlSprite_Update(&state->sprite);
    return state->isActive;
}

static s32 BtlPop_State_Render(TaskPool* pool, Task* task, void* args) {
    BtlPop_State* state = task->data;
    BattleActor*  actor = state->actor;
    s32           posX;
    s32           posY;

    if (state->sprite.sprite.bits_0_1 == 0) {
        posX = BtlStage_ToScreenX(0, actor->position.x + (actor->centerOffsetX << 12));
        posY = BtlStage_ToScreenY(0, actor->position.y, actor->position.z);
        if (state->actor->kind == ACTOR_KIND_PLAYER) {
            posY = (s16)(posY - 48);
        } else {
            posY = (s16)(posY - state->actor->height);
        }
    } else {
        posX = BtlStage_ToScreenX(1, actor->position.x + (actor->centerOffsetX << 12));
        posY = BtlStage_ToScreenY(1, actor->position.y, actor->position.z);
        posY = (s16)(posY - state->actor->height);
    }
    BtlSprite_SetPosition(&state->sprite, posX, posY);
    BtlSprite_SetSortKey(&state->sprite, 10);
    BtlSprite_Render(&state->sprite);
    return 1;
}

static s32 BtlPop_State_Release(TaskPool* pool, Task* task, void* args) {
    BtlSprite_Release(task->data);
    return 1;
}

static s32 BtlPop_State_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlPop_State_Init,
        .update     = BtlPop_State_Update,
        .render     = BtlPop_State_Render,
        .cleanup    = BtlPop_State_Release,
    };
    if (BtlMgr_IsStageFrozen(stage) != 0) {
        return 1;
    } else {
        return stages.iter[stage](pool, task, args);
    }
}

s32 func_ov003_0209ccc0(s32 engine, BattleActor* actor) {
    BtlPop_StateArgs params;

    params.engine = engine;
    params.actor  = actor;
    return EasyTask_CreateTask(&gBattle->commonPool, &Tsk_BtlPop_State, NULL, 0, NULL, &params);
}
