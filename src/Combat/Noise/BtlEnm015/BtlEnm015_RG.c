#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm015.h"
#include "Engine/Math/Random.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>
static s32  func_ov013_02126dd8(TaskPool* pool, Task* task, void* args, s32 stage);
static void func_ov013_02126b30(BtlEnm015*);
static void func_ov013_02126c64(BtlEnm015*);

const TaskHandle Tsk_BtlEnm015_RG = {"Tsk_BtlEnm015_RG", func_ov013_02126dd8, 0x1E8};

static void func_ov013_02126ab0(BtlEnm015* arg0) {
    switch (arg0->unk_1C2) {
        case 0:
            BtlSprite_SetAnimFromTable(&arg0->base.sprite, 0, 0);
            func_ov003_020c4ee0(&arg0->base);
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = RNG_Next(0x3C) + 0x3C;
            break;

        case 1:
            func_ov013_02125a04(arg0, (s32)func_ov013_02126c64, (s32)func_ov013_02126b30, arg0->base.unk_1A2);
            break;
    }
}

static void func_ov013_02126b30(BtlEnm015* arg0) {
    BattleActor* partner = gBattle->partner;

    switch (arg0->unk_1C2) {
        case 0:
            if (func_ov003_020c6230(&arg0->base) != 0) {
                return;
            }
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = 0;
            break;

        case 1:
            BtlSprite_SetAnimFromTable(&arg0->base.sprite, 7, 1);
            func_ov003_020c4cc4(&arg0->base, 0x220);
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = 0;
            break;

        case 2:
            if (SpriteMgr_IsAnimationFinished(&arg0->base.sprite.sprite) == 0) {
                return;
            }
            BtlSprite_SetAnimFromTable(&arg0->base.sprite, 8, 1);
            func_ov003_020c4cc4(&arg0->base, 0x226);
            func_ov013_021257a4(arg0, 0);
            BtlHit_Stagger(BATTLE_SIDE_PARTNER, partner, 0);
            func_ov013_02125838(arg0);
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = 0;
            break;

        case 3:
            if (SpriteMgr_IsAnimationFinished(&arg0->base.sprite.sprite) == 0) {
                return;
            }
            func_ov013_02125b64(arg0, (s32)func_ov013_02126ab0);
            break;
    }
}

static void func_ov013_02126c64(BtlEnm015* arg0) {
    BattleActor* partner = gBattle->partner;

    switch (arg0->unk_1C2) {
        case 0:
            if (func_ov003_020c6230(&arg0->base) != 0) {
                return;
            }
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = 0;
            break;

        case 1:
            BtlSprite_SetAnimFromTable(&arg0->base.sprite, 9, 1);
            arg0->unk_1DC = partner->position.x;
            arg0->unk_1E0 = partner->position.y;
            arg0->unk_1E4 = partner->position.z;
            arg0->unk_1C2 = arg0->unk_1C2 + 1;
            arg0->unk_1C0 = 0;
            break;

        case 2: {
            s32 h;

            if (arg0->unk_1C0 == 0) {
                func_ov003_020c4cc4(&arg0->base, 0x220);
            }
            if (arg0->unk_1C0 == 0x2B) {
                func_ov013_021256f0(arg0, 1, arg0->unk_1DC, arg0->unk_1E0, arg0->unk_1E4);
            }
            if (arg0->unk_1C0 == 0x2C) {
                func_ov003_020c4cc4(&arg0->base, 0x224);
                h = BtlHit_GetAttackInfo(0x8B);
                if (BtlHit_AttackAt(h, &arg0->base.actor.baseArgs, arg0->unk_1DC, arg0->unk_1E0, arg0->unk_1E4) ==
                    BTL_HIT_RESULT_LANDED)
                {
                    func_ov003_020c4cc4(&arg0->base, 0x225);
                }
            }
            arg0->unk_1C0 = arg0->unk_1C0 + 1;
            if (SpriteMgr_IsAnimationFinished(&arg0->base.sprite.sprite) == 0) {
                return;
            }
            func_ov013_02125b64(arg0, (s32)func_ov013_02126ab0);
            break;
        }
    }
}

static s32 func_ov013_02126dd8(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm015* data = task->data;
    s32        ret  = 1;

    switch (stage) {
        case 0: {
            BattleActor* partner;
            s32          x;

            MI_CpuSet(data, 0, 0x1E8);
            func_ov013_02125b8c(1, data, (s32)args, (s32)func_ov013_02126ab0);
            func_ov013_02125b64(data, (s32)func_ov013_02125e90);
            data->base.unk_1AC          = func_ov003_020ccedc(1) >> 2;
            data->base.actor.position.x = data->base.unk_1AC;
            data->base.unk_1B0          = (func_ov003_020ccefc(1) >> 1) + 1;
            data->base.actor.position.y = data->base.unk_1B0;
            data->base.unk_1B4          = 0;
            data->base.actor.position.z = 0;
            partner                     = gBattle->partner;
            x                           = (func_ov003_020ccedc(1) >> 1) + 0x40000;
            partner->position.x         = x;
            gBattle->stages[1].centerX  = x;
            gBattle->flags |= BATTLE_FLAG_NO_GUARD_TURN;
            break;
        }

        case 1:
            ret = func_ov013_02125d24(data);
            break;

        case 2:
            func_ov013_02125efc(data);
            break;

        case 3:
            func_ov003_020c48fc(data);
            break;
    }
    return ret;
}
