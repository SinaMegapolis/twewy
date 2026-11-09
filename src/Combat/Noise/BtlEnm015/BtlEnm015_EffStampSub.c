#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm015.h"

#include "Combat/Core/CombatApi.h"
#include "SndMgr.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlEnm015StampSub {
    /* 0x00 */ BtlEnm015*   unk_00;
    /* 0x04 */ BattleSprite sprite;
    /* 0x64 */ s16          unk_64;
    /* 0x66 */ s16          unk_66;
    /* 0x68 */ s32          unk_68;
    /* 0x6C */ s32          unk_6C;
    /* 0x70 */ s32          unk_70;
} BtlEnm015StampSub; // Size: 0x74
static s32 func_ov013_02126a30(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlEnm015_EffStampSub = {"Tsk_BtlEnm015_EffStampSub", func_ov013_02126a30, 0x74};

static s32 func_ov013_02126788(BtlEnm015StampSub* data) {
    s32          engine = func_ov003_020c37f8(&data->unk_00->base.sprite) != 0;
    BattleActor* base;
    s32          se;

    if (engine != 0) {
        base = g_CombatGlobal->partner;
    } else {
        base = g_CombatGlobal->player;
    }
    se = (engine != 0) ? 0x8A : 0x88;

    switch (data->unk_64) {
        case 0: {
            s32 h;

            data->unk_70 += 0xF000;
            if (data->unk_70 >= 0) {
                s32 r;

                func_ov013_02125838(data->unk_00);
                if (SndMgr_IsSEPlaying(SEIDX_SE_015_FOOTSTAMP02) == 0) {
                    CombatSe_Play(SEIDX_SE_015_FOOTSTAMP02);
                }
                h = CombatHit_GetAttackInfo(se);
                r = CombatHit_AttackAt(h, &data->unk_00->base.actor.baseArgs, data->unk_68, data->unk_6C, data->unk_70);
                if (r == 1) {
                    func_ov003_020c4cc4(&data->unk_00->base, 0x228);
                } else if (r != 2) {
                    if (CombatHit_Stagger(engine, base, 0) != 0) {
                        base->hit.stunTimer = 1;
                    }
                }
                data->unk_64 = data->unk_64 + 1;
                data->unk_66 = 0x3C;
            }
            break;
        }

        case 1:
            data->unk_66 = data->unk_66 - 1;
            if (data->unk_66 < 0) {
                return 0;
            }
            break;
    }

    BtlSprite_Update(&data->sprite);
    return 1;
}

static void func_ov013_021268d8(BtlEnm015StampSub* data) {
    s16 x;
    s16 y;

    BtlStage_WorldToScreen(func_ov003_020c37f8(&data->unk_00->base.sprite) != 0, &x, &y, data->unk_68, data->unk_6C, 0);
    BtlSprite_SetPosition(&data->sprite, x, y);
    BtlSprite_SetSortKey(&data->sprite, 0x7FFFFFFF - data->unk_6C);
    BtlSprite_Render(&data->sprite);
}

static void func_ov013_02126950(BtlEnm015StampSub* data) {
    BtlSprite_Release(&data->sprite);
}

static void func_ov013_02126960(BtlEnm015StampSub* data, Enm015Spawn* args) {
    BtlEnm015* owner = args->unk_00;

    MI_CpuSet(data, 0, sizeof(BtlEnm015StampSub));
    data->unk_00 = args->unk_00;
    data->unk_68 = args->unk_08;
    data->unk_6C = args->unk_0C;
    data->unk_70 = 0;

    u16* table  = (u16*)func_ov013_021256e4();
    s32  engine = owner->base.sprite.sprite.bits_0_1;

    SpriteAnimationEx anim;
    BtlSprite_InitAnim(&anim.anim, engine, func_ov013_021256c0(owner->base.spriteIndex));
    u16 animData         = table[3];
    anim.anim.unk_20     = func_ov013_021256d0(owner->base.spriteIndex);
    anim.animTable       = NULL;
    anim.anim.unk_26     = table[2];
    anim.anim.unk_28     = table[1];
    anim.anim.unk_1C     = table[0];
    anim.anim.bits_10_11 = 0;
    anim.anim.unk_22     = 2;
    anim.anim.animIndex  = animData + 1;
    BtlSprite_Load(&data->sprite, &anim);
}

static s32 func_ov013_02126a30(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm015StampSub* data  = task->data;
    BtlEnm015*         owner = data->unk_00;

    switch (stage) {
        case 0:
            func_ov013_02126960(data, (Enm015Spawn*)args);
            break;
        case 1:
            if ((owner->base.actor.flags & ACTOR_FLAG_DEFEATED) || func_ov003_020c3c28() != 0) {
                return 0;
            }
            return func_ov013_02126788(data);
        case 2:
            func_ov013_021268d8(data);
            break;
        case 3:
            func_ov013_02126950(data);
            break;
    }
    return 1;
}
