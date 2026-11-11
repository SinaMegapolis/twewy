#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm014.h"
#include "Engine/Math/Random.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/fx.h>
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

static s32  func_ov012_02126a4c(BtlEnm014*);
static s32  func_ov012_02126ad8(BtlEnm014*);
static void func_ov012_02126dac(BtlEnm014*);
static void func_ov012_02126e2c(BtlEnm014*);
static void func_ov012_02126ea0(BtlEnm014*);
static void func_ov012_02127134(BtlEnm014*);
static void func_ov012_02127378(BtlEnm014*);
static void func_ov012_02127634(BtlEnm014*);
static s32  func_ov012_021277b0(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlEnm014_UG = {"Tsk_BtlEnm014_UG", func_ov012_021277b0, 0x1F0};

static s32 func_ov012_02126a1c(BtlEnm014* data) {
    if (func_ov012_02126ad8(data) != 0) {
        return 1;
    }
    if (func_ov012_02126a4c(data) != 0) {
        return 1;
    }
    return 0;
}

static s32 func_ov012_02126a4c(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;
    s32          lo     = data->base.actor.position.x - 0x3C000;

    if (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        lo = data->base.actor.position.x;
    }
    if (!(player->position.x - player->radius >= lo + 0x3C000 || player->position.x + player->radius <= lo ||
          player->position.y >= data->base.actor.position.y + 0xF000 ||
          player->position.y <= data->base.actor.position.y - 0xF000))
    {
        func_ov012_02125768(data, func_ov012_02127134);
        return 1;
    }
    return 0;
}

static s32 func_ov012_02126ad8(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;
    s32          lo;

    if (data->base.spriteIndex == 0) {
        return 0;
    }
    lo = data->base.actor.position.x - 0x50000;
    if (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        lo = data->base.actor.position.x;
    }
    if (!(player->position.x - player->radius >= lo + 0x50000 || player->position.x + player->radius <= lo ||
          player->position.y >= data->base.actor.position.y + 0xA000 ||
          player->position.y <= data->base.actor.position.y - 0xA000))
    {
        func_ov012_02125768(data, func_ov012_02127378);
        return 1;
    }
    return 0;
}

static void func_ov012_02126b74(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;
    s32          d      = player->position.x - data->unk_1E4;

    if (d < 0) {
        d = -d;
    }
    if (d > data->unk_1E0.word) {
        data->unk_1E0.word = d;
    }
    data->unk_1E4 = player->position.x;
}

static void func_ov012_02126bb0(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;

    data->unk_1E0.word = 0;
    data->unk_1E4      = player->position.x;
}

static void func_ov012_02126bd8(BtlEnm014* data) {
    if (data->base.spriteIndex == 2) {
        data->unk_1E0.word = 0;
    }
    func_ov012_02125768(data, (data->unk_1E0.word > 0x6000) ? func_ov012_02126e2c : func_ov012_02126dac);
    func_ov012_02126bb0(data);
}

static s32 func_ov012_02126c1c(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;

    if (!(data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED)) {
        if (data->base.actor.position.x < player->position.x) {
            goto hit;
        }
    }
    if (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        if (data->base.actor.position.x > player->position.x) {
            goto hit;
        }
    }
    goto miss;

hit:
    return 1;

miss:
    return 0;
}

static void func_ov012_02126c74(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;
    s32          dx     = player->position.x - data->base.actor.position.x;
    s32          aim;
    s32          idx;
    s32          sin;
    s32          mag;
    s32          cos;

    if (player->position.x > data->base.actor.position.x) {
        if (dx > 0x28000) {
            aim = dx - 0x28000;
        } else {
            aim = -1;
        }
    } else {
        if (dx < -0x28000) {
            aim = dx + 0x28000;
        } else {
            aim = 1;
        }
    }

    idx           = FX_Atan2Idx(player->position.y - data->base.actor.position.y, aim) >> 4;
    sin           = FX_SinCosTable_[idx * 2 + 1];
    cos           = FX_SinCosTable_[idx * 2];
    mag           = FX_Mul(ABS(sin) + 0x1000, 0x800);
    data->unk_1D8 = FX_Mul(sin, mag);
    data->unk_1DC = FX_Mul(cos, mag);

    if (data->unk_1D8 != 0) {
        return;
    }
    data->unk_1D8 = FX32_CONVERT(aim) >> 4;
}

static void func_ov012_02126dac(BtlEnm014* data) {
    switch (data->unk_1C2) {
        case 0:
            BtlSprite_SetAnimFromTable(&data->base.sprite, 0, 0);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = func_ov003_020c42ec(&data->base);
            break;
        case 1:
            data->unk_1C0 = data->unk_1C0 - 1;
            if (data->unk_1C0 < 0) {
                func_ov012_02125768(data, func_ov012_02126ea0);
            }
            break;
    }
}

static void func_ov012_02126e2c(BtlEnm014* data) {
    switch (data->unk_1C2) {
        case 0:
            BtlSprite_SetAnimFromTable(&data->base.sprite, 1, 1);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            break;
        case 1:
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            func_ov012_02125768(data, func_ov012_02126dac);
            break;
    }
}

static void func_ov012_02126ea0(BtlEnm014* data) {
    if (func_ov012_02126a1c(data) != 0) {
        return;
    }
    switch (data->unk_1C2) {
        case 0:
            BtlSprite_SetAnimFromTable(&data->base.sprite, 2, 1);
            func_ov012_02126c74(data);
            func_ov003_020c4b5c(&data->base);
            func_ov012_02126bb0(data);
            data->unk_1E8 = 0;
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0x13;
            break;

        case 1:
            data->base.actor.position.x = data->base.actor.position.x + data->unk_1D8;
            data->base.actor.position.y = data->base.actor.position.y + data->unk_1DC;
            func_ov003_020ccf1c(&data->unk_1D8, -(ABS(data->unk_1D8) >> 4));
            func_ov003_020ccf1c(&data->unk_1DC, -(ABS(data->unk_1DC) >> 4));
            func_ov012_02126b74(data);
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            data->unk_1D8 = 0;
            data->unk_1DC = 0;
            data->unk_1E8 = func_ov012_02126c1c(data);
            if (data->unk_1C0 == 0x13) {
                func_ov003_020c4cc4(&data->base, 0x218);
            }
            data->unk_1C0 = data->unk_1C0 - 1;
            if (data->unk_1C0 > 0) {
                return;
            }
            func_ov012_02126c74(data);
            BtlSprite_SetAnimFromTable(&data->base.sprite, 3, 1);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0x13;
            if (data->unk_1E8 != 0) {
                data->unk_1D8 = 0;
            }
            break;

        case 2:
            data->base.actor.position.x = data->base.actor.position.x + data->unk_1D8;
            data->base.actor.position.y = data->base.actor.position.y + data->unk_1DC;
            func_ov003_020ccf1c(&data->unk_1D8, -(ABS(data->unk_1D8) >> 4));
            func_ov003_020ccf1c(&data->unk_1DC, -(ABS(data->unk_1DC) >> 4));
            func_ov012_02126b74(data);
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            data->unk_1D8 = 0;
            data->unk_1DC = 0;
            if (data->unk_1C0 == 0x13) {
                func_ov003_020c4cc4(&data->base, 0x218);
            }
            data->unk_1C0 = data->unk_1C0 - 1;
            if (data->unk_1C0 > 0) {
                return;
            }
            if (func_ov012_02126c1c(data) != 0 || data->unk_1E8 != 0) {
                func_ov012_02126bd8(data);
                return;
            }
            BtlSprite_SetAnimFromTable(&data->base.sprite, 2, 1);
            func_ov012_02126c74(data);
            data->unk_1C2 = data->unk_1C2 - 1;
            data->unk_1C0 = 0x13;
            break;
    }
}

static void func_ov012_02127134(BtlEnm014* data) {
    BattleActor* player = g_CombatGlobal->player;

    switch (data->unk_1C2) {
        case 0:
            BtlSprite_SetAnimFromTable(&data->base.sprite, 0xC, 1);
            data->unk_1D0 = 0;
            func_ov012_02126bb0(data);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            return;

        case 1: {
            s32 d;
            s32 x;
            data->unk_1C0 = data->unk_1C0 + 1;
            if (data->unk_1C0 == 0x28) {
                func_ov003_020c4cc4(&data->base, 0x219);
            }
            if (data->unk_1C0 > 0x28 && data->unk_1C0 < 0x32) {
                d = FX32_CONVERT(func_ov003_020cd11c(0x82));
                d = (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) ? d : -d;
                x = data->base.actor.position.x;
                if (data->unk_1D0 == 0) {
                    data->unk_1D0 = BtlHit_AttackAt(BtlHit_GetAttackInfo(0x82), &data->base.actor.baseArgs, x + d,
                                                    data->base.actor.position.y, data->base.actor.position.z);
                    if (data->unk_1D0 == 1) {
                        func_ov003_020c4cc4(&data->base, 0x21A);
                        player->zVelocity = FX_Mul(player->zVelocity, 0x2000);
                        player->yVelocity = FX_Mul(player->yVelocity, 0x1800);
                    }
                }
            }
            func_ov012_02126b74(data);
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            BtlSprite_SetAnimFromTable(&data->base.sprite, 0xD, 1);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            return;
        }

        case 2:
            func_ov012_02126b74(data);
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            if (func_ov012_02126c1c(data) != 0 && data->unk_1D0 == 1) {
                goto dash;
            }
            func_ov012_02126bd8(data);
            return;

        dash:
            func_ov012_02125768(data, func_ov012_02126dac);
            return;
    }
}

static void func_ov012_02127378(BtlEnm014* data) {
    switch (data->unk_1C2) {
        case 0:
            data->unk_1EC = 0;
            if ((u16)(data->base.spriteIndex + 0xFFFE) <= 1) {
                data->unk_1EC = RNG_Next(2);
            }
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            /* fallthrough */

        case 1:
            BtlSprite_SetAnimFromTable(&data->base.sprite, 0xE, 1);
            data->unk_1D0 = 0;
            func_ov012_02126bb0(data);
            func_ov012_021256f0(0, data, 1);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            return;

        case 2: {
            s32 se;
            data->unk_1C0 = data->unk_1C0 + 1;
            func_ov012_02126b74(data);
            if (data->unk_1C0 == 0x31) {
                func_ov003_020c4cc4(&data->base, 0x21B);
                func_ov003_020c4cc4(&data->base, (data->base.spriteIndex == 3) ? 0x21D : 0x21C);
            }
            if (data->unk_1C0 > 0x31 && data->unk_1C0 < 0x3C && data->unk_1D0 == 0) {
                s32 d;
                s32 h;
                se            = (data->base.spriteIndex == 3) ? 0x84 : 0x83;
                d             = FX32_CONVERT(func_ov003_020cd11c(se));
                d             = (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) ? d : -d;
                h             = BtlHit_GetAttackInfo(se);
                data->unk_1D0 = BtlHit_AttackAt(h, &data->base.actor.baseArgs, data->base.actor.position.x + d,
                                                data->base.actor.position.y, data->base.actor.position.z);
            }
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            BtlSprite_SetAnimFromTable(&data->base.sprite, 0xF, 1);
            data->unk_1C2 = data->unk_1C2 + 1;
            data->unk_1C0 = 0;
            return;
        }

        case 3:
            func_ov012_02126b74(data);
            if (SpriteMgr_IsAnimationFinished(&data->base.sprite.sprite) == 0) {
                return;
            }
            if (data->unk_1EC > 0) {
                func_ov003_020c4b5c(&data->base);
                data->unk_1EC = data->unk_1EC - 1;
                data->unk_1C2 = 1;
                return;
            }
            if (func_ov012_02126c1c(data) != 0) {
                func_ov012_02126bd8(data);
                return;
            }
            func_ov012_02125768(data, func_ov012_02126dac);
            return;
    }
}

static void func_ov012_02127608(BtlEnm014* data) {
    if (func_ov003_020c6bc8(&data->base, 7) != 0) {
        return;
    }
    func_ov012_02125768(data, func_ov012_02127634);
}

static void func_ov012_02127634(BtlEnm014* data) {
    if (func_ov003_020c6c2c(&data->base, 7) != 0) {
        return;
    }
    func_ov012_02125768(data, func_ov012_02126dac);
}

static void func_ov012_02127660(BtlEnm014* data) {
    if (func_ov003_020c6bac(&data->base, 9) != 0) {
        return;
    }
    func_ov012_02125768(data, func_ov012_02126dac);
}

static s32 func_ov012_0212768c(BtlEnm014* data) {
    switch (BtlActor_PopPendingCommand(&data->base.actor)) {
        case ACTOR_CMD_NONE:
            break;
        case ACTOR_CMD_HIT:
            if (data->base.actor.periodicEffectMode == 4) {
                func_ov012_02125768(data, func_ov012_021257e8);
            }
            if (data->base.actor.zVelocity < 0) {
                data->base.actor.zVelocity = 0;
            }
            break;
        case ACTOR_CMD_DEFEAT:
            BtlCollision_SetDisabled(&data->base.collider, 1);
            func_ov012_02125768(data, func_ov012_02125810);
            break;
        case ACTOR_CMD_HOLD:
            func_ov012_02125768(data, func_ov012_02127608);
            break;
        case ACTOR_CMD_RELEASE:
            func_ov012_02125768(data, func_ov012_02127634);
            break;
        case ACTOR_CMD_KNOCKDOWN:
            func_ov012_02125768(data, func_ov012_02127660);
            break;
        case ACTOR_CMD_FEAT:
            func_ov012_02125768(data, func_ov012_0212582c);
            break;
    }

    if (data->unk_1C4 != func_ov012_02127660) {
        func_ov012_02125958(data);
    }
    func_ov012_02125858(data);
    if (data->unk_1C4 != NULL) {
        data->unk_1C4(data);
    }
    func_ov003_020c4668(&data->base);
    func_ov003_020ccea8(&data->base.actor, &data->base.sprite);
    func_ov012_021258e8();
    return data->unk_1CC;
}

static s32 func_ov012_021277b0(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm014* data = task->data;

    switch (stage) {
        case 0:
            MI_CpuSet(data, 0, 0x1F0);
            func_ov012_02125790(0, data, args, func_ov012_02126dac);
            func_ov012_02125768(data, func_ov012_021257c4);
            break;
        case 1:
            return func_ov012_0212768c(data);
        case 2:
            func_ov003_020c48b0(&data->base);
            break;
        case 3:
            func_ov003_020c492c(&data->base);
            break;
    }
    return 1;
}
