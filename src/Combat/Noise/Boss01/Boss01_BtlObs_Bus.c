#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Noise/Boss01.h"
#include "Combat/Noise/Private/Boss01.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlObs_Bus {
    /* 0x000 */ char         unk_000[0x2];
    /* 0x002 */ s16          unk_002;
    /* 0x004 */ BattleActor  actor;
    /* 0x080 */ char         unk_080[0xC4 - 0x80];
    /* 0x0C4 */ BattleSprite unk_0C4;
    /* 0x124 */ BattleSprite unk_124;
    /* 0x184 */ s32          unk_184;
    /* 0x188 */ void (*unk_188)(struct BtlObs_Bus*);
    /* 0x18C */ s16 unk_18C;
    /* 0x18E */ s16 unk_18E;
    /* 0x190 */ u32 unk_190;
    /* 0x194 */ u32 unk_194;
} BtlObs_Bus; // Size: 0x198

typedef struct {
    /* 0x0 */ u32 unk_0;
} BtlObs_Bus_Args;

static s32 BtlObs_Bus_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

static const u16 data_ov016_02129120[] = {1, 3, 2, 0, 4, 6, 5, 0, 7, 9, 8, 0, 0x5000, 0};

static const BinIdentifier Iden_BtlObs_Bus = {3, "Apl_Mor/Grp_Boss01bus.bin"};

static const TaskHandle Tsk_BtlObs_Bus = {"Tsk_BtlObs_Bus", BtlObs_Bus_RunTask, 0x198};

static void BtlObs_Bus_SetState(BtlObs_Bus* arg0, void (*arg1)(BtlObs_Bus*)) {
    arg0->unk_188 = arg1;
    arg0->unk_18C = 0;
    arg0->unk_18E = 0;
}

static void BtlObs_Bus_OnPhaseStart(BtlObs_Bus* obs) {
    if (obs->actor.flags & ACTOR_FLAG_HELD) {
        if ((*data_ov016_021292c0 == 2) && (obs->unk_124.sprite.resourceData == obs->unk_0C4.sprite.resourceData)) {
            obs->actor.radius = 60;
            obs->actor.height = 100;
            BtlSprite_SetAnimFromTable(&obs->unk_0C4, 2, 0);
            BtlSprite_SetPaletteSource(&obs->unk_0C4, 11);
            BtlSprite_Release(&obs->unk_124);
            BtlHit_LoadSparkForUnit(0, &obs->unk_124, &obs->actor);
        }
    }
}

static void BtlObs_Bus_StateIdle(BtlObs_Bus* obs) {
    return;
}

static void BtlObs_Bus_StateStop(BtlObs_Bus* obs) {
    obs->unk_194 = 0;
}

static void BtlObs_Bus_StateEnd(BtlObs_Bus* obs) {
    return;
}

static void BtlObs_Bus_StateHitFlash(BtlObs_Bus* obs) {
    if (obs->unk_18E == 0) {
        obs->actor.flags |= ACTOR_FLAG_INTANGIBLE;
    }

    if (obs->unk_18E < 20) {
        obs->unk_190 = !obs->unk_190;
    } else if (obs->unk_18E % 3 == 0) {
        obs->unk_190 = 1;
    } else {
        obs->unk_190 = 0;
    }

    if (obs->unk_18E > 20) {
        obs->unk_190 = 0;
        obs->unk_194 = 0;
    } else {
        obs->unk_18E++;
    }
}

static s32 BtlObs_Bus_Init(TaskPool* pool, Task* task, void* args) {
    BtlObs_Bus*      obs     = task->data;
    BtlObs_Bus_Args* busArgs = (BtlObs_Bus_Args*)args;

    MI_CpuSet(obs, 0, sizeof(BtlObs_Bus));
    obs->unk_184 = busArgs->unk_0;
    obs->unk_190 = 1;
    obs->unk_194 = 1;
    BtlActor_Init(&obs->actor, ACTOR_KIND_OBJECT);
    obs->unk_002                 = 25;
    obs->actor.position.x        = 0x180000;
    obs->actor.position.y        = 0x9000;
    obs->actor.position.z        = 0;
    obs->actor.radius            = 100;
    obs->actor.height            = 100;
    obs->actor.zGravity          = 0xCD;
    obs->actor.linearDampingStep = 0xE66;
    BtlSprite_LoadFromTable(0, &obs->unk_0C4, &Iden_BtlObs_Bus, (SpriteAnimEntry*)data_ov016_02129120, 0, 10, 0x180);
    BtlSprite_LoadFromTable(0, &obs->unk_124, &Iden_BtlObs_Bus, (SpriteAnimEntry*)data_ov016_02129120, 1, 10, 0);
    BtlSprite_SetAnimFromTable(&obs->unk_0C4, 0, 0);
    BtlSprite_SetAnimFromTable(&obs->unk_124, 1, 0);
    BtlObs_Bus_SetState(obs, BtlObs_Bus_StateIdle);
    return 1;
}

static s32 BtlObs_Bus_Update(TaskPool* pool, Task* task, void* args) {
    BtlObs_Bus* obs = task->data;

    if (g_CombatGlobal->unk3D878 & 0x1000) {
        return 1;
    }
    switch (BtlActor_PopPendingCommand(&obs->actor)) {
        case ACTOR_CMD_HIT:
            BtlObs_Bus_SetState(obs, BtlObs_Bus_StateHitFlash);
            break;
        case ACTOR_CMD_DEFEAT:
            BtlObs_Bus_SetState(obs, BtlObs_Bus_StateStop);
            break;
        case ACTOR_CMD_RELEASE:
            BtlObs_Bus_SetState(obs, BtlObs_Bus_StateEnd);
            break;
    }
    if (obs->unk_188 != NULL) {
        obs->unk_188(obs);
    }
    BtlActor_UpdatePhysics(&obs->actor);
    BtlObs_Bus_OnPhaseStart(obs);
    BtlSprite_Update(&obs->unk_0C4);
    BtlSprite_Update(&obs->unk_124);
    return obs->unk_194;
}

static s32 BtlObs_Bus_Render(TaskPool* pool, Task* task, void* args) {
    BtlObs_Bus* obs = task->data;

    if (g_CombatGlobal->unk3D878 & 0x1000) {
        return 1;
    }
    if (obs->unk_190 == 0) {
        return 1;
    }

    s16 posX   = BtlStage_ToScreenX(0, obs->actor.position.x);
    s16 posY   = BtlStage_ToScreenY(0, obs->actor.position.y, obs->actor.position.z);
    s32 posY32 = posY;
    if (obs->unk_0C4.animTableIndex == 0) {
        s16 posYOff = posY + 8;

        BtlSprite_SetPosition(&obs->unk_124, posX, posYOff);
        BtlSprite_SetSortKey(&obs->unk_124, 0x7FFFEFFF);
        BtlSprite_Render(&obs->unk_124);
        BtlSprite_SetPosition(&obs->unk_0C4, posX, posY);
        BtlSprite_SetSortKey(&obs->unk_0C4, 0x7FFFEFFF);
        BtlSprite_Render(&obs->unk_0C4);
    } else {
        BtlSprite_SetPosition(&obs->unk_0C4, posX, (s16)posY32);
        BtlSprite_SetSortKey(&obs->unk_0C4, 0x7FFFFFFF - obs->actor.position.y);
        BtlSprite_Render(&obs->unk_0C4);
        BtlHit_DrawSparkOnActor(&obs->unk_124, &obs->actor);
    }
    return 1;
}

static s32 BtlObs_Bus_Destroy(TaskPool* pool, Task* task, void* args) {
    BtlObs_Bus* obs = task->data;

    BtlSprite_Release(&obs->unk_0C4);
    BtlSprite_Release(&obs->unk_124);
    return 1;
}

static s32 BtlObs_Bus_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages data_ov016_02129110 = {
        .initialize = BtlObs_Bus_Init,
        .update     = BtlObs_Bus_Update,
        .render     = BtlObs_Bus_Render,
        .cleanup    = BtlObs_Bus_Destroy,
    };
    return data_ov016_02129110.iter[stage](pool, task, args);
}

void* BtlObs_Bus_CreateTask(struct Boss01_UG* bossUG) {
    struct Boss01_UG* sp8 = bossUG;

    s32 taskId = EasyTask_CreateTask(&g_CombatGlobal->unk_00000, &Tsk_BtlObs_Bus, NULL, 0, NULL, &sp8);
    return EasyTask_GetTaskData(&g_CombatGlobal->unk_00000, taskId);
}
