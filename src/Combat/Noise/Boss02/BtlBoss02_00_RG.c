#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEnemyActor.h"
#include "Combat/Noise/Boss02.h"
#include "Combat/Noise/Boss03.h"
#include "Engine/Math/Random.h"
#include "SpriteMgr.h"
#include <nitro/mi/cpumem.h>
#include <nitro/types.h>

typedef struct BtlBoss02_00_RG {
    /* 0x000 */ Ov003EnemyActor base;
    /* 0x1C0 */ s32             unk_1C0;
    /* 0x1C4 */ s32             unk_1C4;
    /* 0x1C8 */ s32             unk_1C8;
    /* 0x1CC */ void (*unk_1CC)(struct BtlBoss02_00_RG*);
    /* 0x1D0 */ BattleAttackArgs attackArgs;
    /* 0x1D8 */ BattleSprite     unk_1D8;
    /* 0x238 */ BattleSprite*    unk_238;
    /* 0x23C */ s32              unk_23C;
    /* 0x240 */ s32              unk_240;
    /* 0x244 */ s32              unk_244;
    /* 0x248 */ s32              unk_248;
    /* 0x24C */ u16              unk_24C;
    /* 0x24E */ u16              unk_24E;
    /* 0x250 */ u32              unk_250;
    /* 0x254 */ u32              unk_254;
    /* 0x258 */ u16              unk_258;
    /* 0x25A */ u16              unk_25A;
    /* 0x25C */ u32              unk_25C;
    /* 0x260 */ u16              unk_260;
    /* 0x262 */ s16              unk_262;
} BtlBoss02_00_RG; // Size: 0x264

static void func_ov017_02125c6c(BtlBoss02_00_RG* rg);
static void func_ov017_02125e5c(BtlBoss02_00_RG* rg);
static void func_ov017_02125fb4(BtlBoss02_00_RG* rg);
static s32  BtlBoss02_00_RG_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlBoss02_00_RG = {"Tsk_BtlBoss02_00_RG", BtlBoss02_00_RG_RunTask, 0x264};

const Vec data_ov017_02141538[4] = {
    {-0x50000, 0,        0},
    { 0x50000, 0,        0},
    {-0x50000, 0, -0x41000},
    { 0x50000, 0, -0x41000},
};

extern const BinIdentifier data_ov017_02141520;
extern SpriteAnimEntry     data_ov017_021415a8;
extern s16                 data_ov017_0214398c[];
extern s16                 data_ov017_0214399c[];
extern const TaskHandle    data_ov017_02141f30;
extern u16                 data_ov017_02143980;

static void BtlBoss02_00_RG_SetState(BtlBoss02_00_RG* rg, void (*callback)(BtlBoss02_00_RG*)) {
    ((void (*)())func_ov003_020c427c)();
    rg->unk_1CC = callback;
    rg->unk_1C8 = 0;
    rg->unk_1C0 = 0;
}

static s16 func_ov017_021256e4(BtlBoss02_00_RG* rg) {
    while (rg->unk_260 == RNG_Next(4)) {
    }
}

static void func_ov017_02125708(BtlBoss02_00_RG* rg) {
    rg->base.unk_1AC = rg->base.actor.position.x = g_CombatGlobal->unk3D7C0[1].unk_20 + data_ov017_02141538[rg->unk_260].x;
    rg->base.unk_1B0 = rg->base.actor.position.y = g_CombatGlobal->unk3D7C0[1].unk_24 + data_ov017_02141538[rg->unk_260].y;
    rg->base.unk_1B4 = rg->base.actor.position.z = data_ov017_02141538[rg->unk_260].z;
}

static void func_ov017_02125794(BtlBoss02_00_RG* rg) {
    BtlSprite_LoadFromTable(1, &rg->unk_1D8, &data_ov017_02141520, &data_ov017_021415a8, 0x10, 7, 0x80);
    rg->unk_238 = &rg->unk_1D8;
    rg->unk_23C = g_CombatGlobal->unk3D7C0[1].unk_0C / 2;
    rg->unk_240 = g_CombatGlobal->unk3D7C0[1].unk_10 / 2;
    rg->unk_244 = 0x40000;
    func_ov003_020c3acc(1, 762, rg->unk_23C);
    rg->unk_248 = 1;
}

static void func_ov017_02125840(BtlBoss02_00_RG* rg) {
    if (rg->unk_238 != 0) {
        rg->unk_244 -= 0x8000;
        BtlSprite_Update(&rg->unk_1D8);

        s32 var_r1 = g_CombatGlobal->partner->position.z - rg->unk_244;
        if (var_r1 < 0) {
            var_r1 = -var_r1;
        }
        if (rg->unk_248 == 1 && F2I(var_r1) <= 0x10 &&
            CombatHit_AttackCharacter(CombatHit_GetAttackInfo(240), &rg->attackArgs, rg->unk_23C) != 0)
        {
            func_ov017_02130178(1, 1256, 1388, rg->base.actor.position.x);
            rg->unk_248 = 0;
        }
        if (rg->unk_244 <= -0x100000) {
            BtlSprite_Release(&rg->unk_1D8);
            rg->unk_238 = 0;
        }
    }
}

static void func_ov017_02125908(BtlBoss02_00_RG* rg) {
    if (rg->unk_238 != 0) {
        BtlSprite_SetPosition(&rg->unk_1D8, BtlStage_ToScreenX(1, rg->unk_23C),
                              BtlStage_ToScreenY(1, rg->unk_240, rg->unk_244));
        BtlSprite_SetSortKey(&rg->unk_1D8, 0x7FFFFFFE - rg->unk_240);
        BtlSprite_Render(&rg->unk_1D8);
    }
}

static void func_ov017_0212596c(BtlBoss02_00_RG* rg) {
    if (rg->unk_238 != 0) {
        BtlSprite_Release(rg->unk_238);
    }
}

static void func_ov017_02125984(BtlBoss02_00_RG* rg) {
    if (rg->unk_25C != 0) {
        s16 temp_r5 = rg->base.actor.maxHp * 3 / 100;
        func_ov017_02130178(1, 1254, 1388, rg->base.actor.position.x);
        BtlMgr_AddUnitHp(1, &rg->base.actor, temp_r5);
        func_ov003_0209b564(1, temp_r5, &rg->base.actor, 1);
    }
}

static void func_ov017_021259fc(BtlBoss02_00_RG* rg) {
    rg->base.actor.flags &= ~ACTOR_FLAG_IGNORES_HITS;
    func_ov017_02130178(1, 1255, 1388, rg->base.actor.position.x);
    CombatHit_AttackUnit(CombatHit_GetAttackInfo(241), &rg->attackArgs, &rg->base.actor, rg->base.actor.position.x);
    rg->unk_262 = 120;
    rg->unk_24C = 1;
}

static void func_ov017_02125a58(BtlBoss02_00_RG* rg) {
    if (rg->unk_238 == 0) {
        s32 var_r5 = 0;

        while (var_r5 < 1) {
            if (data_ov017_0214398c[var_r5] == 0) {
                func_ov017_02125794(rg);
                data_ov017_0214398c[var_r5] = -1;
                break;
            } else {
                var_r5++;
            }
        }
    } else {
        func_ov017_02125840(rg);
    }

    for (s32 i = 0; i < 3; i++) {
        if (data_ov017_0214399c[i] == 1) {
            func_ov017_02125984(rg);
            data_ov017_0214399c[i] = -1;
        }
    }

    for (s32 i = 0; i < 3; i++) {
        if (data_ov017_0214399c[i] == 2) {
            func_ov017_021259fc(rg);
            data_ov017_0214399c[i] = -1;
        }
    }
}

static void func_ov017_02125b30(BtlBoss02_00_RG* rg) {
    if (func_ov003_020c5bfc(&rg->base) == 0) {
        BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
    }
}

// Nonmatching: Stack
static void func_ov017_02125b58(BtlBoss02_00_RG* rg) {
    Vec vec;
    s16 sp14;

    if (rg->unk_1C0 == 0) {
        BtlSprite_SetAnimFromTable(&rg->base.sprite, 6, 1);
    }
    if (rg->unk_1C0 == 48) {
        vec.x = g_CombatGlobal->unk3D7C0[1].unk_20 + data_ov017_02141538[rg->unk_260].x;
        vec.y = g_CombatGlobal->unk3D7C0[1].unk_24 + data_ov017_02141538[rg->unk_260].y;
        vec.z = data_ov017_02141538[rg->unk_260].z;
        sp14  = (rg->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED);
        EasyTask_CreateTask(&g_CombatGlobal->taskPool, &data_ov017_02141f30, 0, 0, 0, &vec);
    }
    rg->unk_1C0++;
    if (SpriteMgr_IsAnimationFinished(&rg->base.sprite.sprite)) {
        BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
    }
}

static void func_ov017_02125c6c(BtlBoss02_00_RG* rg) {
    func_ov003_020c4b1c(&rg->base);
    if (rg->unk_1C0 == 0) {
        BtlSprite_SetAnimFromTable(&rg->base.sprite, 0, 0);
        rg->unk_1C4 = func_ov003_020c42ec(&rg->base);
    }
    rg->unk_1C0++;
    if (rg->unk_1C0 >= rg->unk_1C4) {
        if (func_ov003_020c4348(&rg->base) != 0) {
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125b58);
        } else {
            rg->unk_260 = func_ov017_021256e4(rg);
            func_ov017_02125708(rg);
            rg->unk_262 = 36;
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
        }
    }
    if (rg->unk_262 > 0) {
        rg->unk_262--;
    }
    if (rg->unk_262 == 1) {
        if (RNG_Next(100) < 50) {
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125b58);
            rg->unk_262 = 0;
        }
        rg->base.actor.flags |= ACTOR_FLAG_IGNORES_HITS;
    }
}

static void func_ov017_02125d6c(BtlBoss02_00_RG* rg) {
    if (rg->unk_1C0 == 0) {
        rg->base.sprite.sprite.animationMode = 2;
        if (rg->unk_250 == 1) {
            func_ov017_02130178(1, 1248, 1249, rg->base.actor.position.x);
        } else {
            func_ov017_02130178(1, 1250, 1388, rg->base.actor.position.x);
        }
    }
    rg->unk_1C0++;
    if (rg->unk_24C == 0) {
        if (func_ov003_020c62c4(&rg->base, 3) == 0) {
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
            rg->unk_262 = 0;
            rg->base.actor.flags |= ACTOR_FLAG_IGNORES_HITS;
        }
    } else {
        BtlBoss02_00_RG_SetState(rg, func_ov017_02125e5c);
    }
}

static void func_ov017_02125e38(BtlBoss02_00_RG* rg) {
    if (rg->unk_1C0 == 0) {
        rg->unk_25C = 0;
        rg->unk_1C0++;
    }
}

static void func_ov017_02125e5c(BtlBoss02_00_RG* rg) {
    switch (rg->unk_1C8) {
        case 0: {
            BtlSprite_SetAnimFromTable(&rg->base.sprite, 3, 1);
            func_ov003_020c4b1c(&rg->base);
            rg->unk_1C8++;
        } break;

        case 1: {
            if (SpriteMgr_IsAnimationFinished(&rg->base.sprite.sprite)) {
                BtlSprite_SetAnimFromTable(&rg->base.sprite, 4, 0);
                rg->unk_1C8++;
            }
        } break;

        case 2: {
            rg->unk_262--;
            if (rg->unk_262 == 0) {
                BtlSprite_SetAnimFromTable(&rg->base.sprite, 5, 1);
                rg->unk_1C8++;
            }
        } break;

        case 3: {
            if (SpriteMgr_IsAnimationFinished(&rg->base.sprite.sprite)) {
                BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
                rg->unk_1C8++;
            }
        } break;
    }
}

static void func_ov017_02125f48(BtlBoss02_00_RG* rg) {
    rg->unk_260 = func_ov017_021256e4(rg);
    func_ov017_02125708(rg);
    rg->unk_262 = 36;
    rg->base.actor.flags &= ~ACTOR_FLAG_IGNORES_HITS;
    rg->base.actor.flags |= ACTOR_FLAG_INTANGIBLE;
    rg->base.flags18C |= 1;
    BtlBoss02_00_RG_SetState(rg, func_ov017_02125fb4);
    func_ov003_020c3acc(1, 0x2FF, rg->base.actor.position.x);
}

static void func_ov017_02125fb4(BtlBoss02_00_RG* rg) {
    if (rg->unk_1C0 == 0) {
        BtlSprite_SetAnimFromTable(&rg->base.sprite, 0, 0);
    }
    rg->unk_1C0++;
    if (rg->unk_1C0 > 30) {
        func_ov003_020c3acc(1, 0x300, rg->base.actor.position.x);
        rg->base.actor.flags &= ~ACTOR_FLAG_INTANGIBLE;
        rg->base.flags18C &= ~1;
        BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
    }
}

static void func_ov017_0212602c(BtlBoss02_00_RG* rg) {
    if (rg->unk_1C0 == 0) {
        rg->unk_1C0++;
    }

    void* temp_r0_2 = func_ov003_020c72b4(&rg->base, 0, 3);
    rg->base.actor.flags &= ~ACTOR_FLAG_IGNORES_HITS;
    rg->base.actor.zVelocity = 0;
    rg->base.actor.zGravity  = 0;
    if (temp_r0_2 == NULL) {
        rg->unk_262 = 10;
        BtlBoss02_00_RG_SetState(rg, func_ov017_02125c6c);
    }
}

// Nonmatching: Instruction reordering
static s32 BtlBoss02_00_RG_Init(TaskPool* pool, Task* task, void* args) {
    BtlBoss02_00_RG* rg = task->data;

    MI_CpuSet(rg, 0, sizeof(BtlBoss02_00_RG));
    func_ov003_020c3efc(&rg->base, args);
    data_ov017_02143980          = rg->base.group;
    rg->unk_254                  = 1;
    rg->unk_25C                  = 1;
    rg->unk_262                  = 0;
    rg->unk_260                  = 0;
    rg->unk_238                  = 0;
    rg->unk_24C                  = 0;
    rg->attackArgs.power         = 100;
    rg->attackArgs.powerPercent  = 100;
    rg->attackArgs.effectPercent = 100;

    rg->base.unk_1AC = rg->base.actor.position.x = g_CombatGlobal->unk3D7C0[1].unk_20 + data_ov017_02141538[0].x;
    rg->base.unk_1B0 = rg->base.actor.position.y = g_CombatGlobal->unk3D7C0[1].unk_24 + data_ov017_02141538[0].y;
    rg->base.unk_1B4 = rg->base.actor.position.z = data_ov017_02141538[0].z;

    rg->base.actor.zVelocity = 0;
    rg->base.actor.zGravity  = 0;
    func_ov003_020c44ac(&rg->base);
    func_ov003_020c4b1c(&rg->base);
    BtlBoss02_00_RG_SetState(rg, func_ov017_02125b30);
    data_ov017_0214398c[0] = -1;

    for (s32 i = 0; i < 3; i++) {
        data_ov017_0214399c[i] = -1;
    }

    rg->base.actor.flags |= ACTOR_FLAG_IGNORES_HITS | ACTOR_FLAG_BOSS_GAUGE_HP;
    g_CombatGlobal->unk3D875 = 2;
    g_CombatGlobal->unk3D878 |= 0x200000;
    func_ov017_02130148();
    return 1;
}

static s32 BtlBoss02_00_RG_Update(TaskPool* pool, Task* task, void* args) {
    BtlBoss02_00_RG* rg = task->data;

    if (g_CombatGlobal->unk3D8D8 < ((g_CombatGlobal->unk3D8DA * 75) / 100) && rg->unk_1CC != func_ov017_02125e38) {
        rg->base.actor.pendingCommand = ACTOR_CMD_DEFEAT;
        g_CombatGlobal->unk3D878 |= 0x20000000;
        func_ov017_021301e0();
    }

    rg->unk_250 = BtlActor_PopPendingCommand(&rg->base.actor);
    switch (rg->unk_250) {
        case 1:
        case 2:
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125d6c);
            break;

        case 7:
            BtlBoss02_00_RG_SetState(rg, func_ov017_02125f48);
            break;

        case 3:
            if (rg->base.flags18C & 0x10) {
                rg->base.flags18C |= 0x20;
            } else {
                BtlBoss02_00_RG_SetState(rg, func_ov017_02125e38);
            }
            break;

        case 6:
            BtlBoss02_00_RG_SetState(rg, func_ov017_0212602c);
            break;

        case 12:
        default:
            break;
    }
    if (rg->unk_1CC != NULL) {
        rg->unk_1CC(rg);
    }
    rg->unk_24C = 0;
    func_ov017_02125a58(rg);
    func_ov003_020c4628(&rg->base);
    func_ov017_0213015c();
    return rg->unk_254;
}

static s32 BtlBoss02_00_RG_Render(TaskPool* pool, Task* task, void* args) {
    BtlBoss02_00_RG* rg = task->data;

    func_ov017_02125908(rg);
    func_ov003_020c4878(rg);
    return 1;
}

static s32 BtlBoss02_00_RG_Destroy(TaskPool* pool, Task* task, void* args) {
    BtlBoss02_00_RG* rg = task->data;

    func_ov017_0212596c(rg);
    func_ov003_020c48fc(rg);
    return 1;
}

static s32 BtlBoss02_00_RG_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = {
        .initialize = BtlBoss02_00_RG_Init,
        .update     = BtlBoss02_00_RG_Update,
        .render     = BtlBoss02_00_RG_Render,
        .cleanup    = BtlBoss02_00_RG_Destroy,
    };
    return stages.iter[stage](pool, task, args);
}