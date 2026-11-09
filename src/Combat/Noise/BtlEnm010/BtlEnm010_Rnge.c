#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm010.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlEnm010Rnge {
    /* 0x00 */ BattleSprite     sprite;
    /* 0x60 */ Ov003EnemyActor* unk_060;
    /* 0x64 */ s32              unk_064;
    /* 0x68 */ s32              unk_068;
} BtlEnm010Rnge;

static char data_ov011_0212cc10[20];
static s32  func_ov011_02127ce0(TaskPool* pool, Task* task, void* args, s32 stage);

static const TaskHandle data_ov011_0212c1d4 = {(const char*)data_ov011_0212cc10, func_ov011_02127ce0, 108};

static char data_ov011_0212cc10[20] = {0x54, 0x73, 0x6B, 0x5F, 0x42, 0x74, 0x6C, 0x45, 0x6E, 0x6D,
                                       0x30, 0x31, 0x30, 0x5F, 0x52, 0x6E, 0x67, 0x65, 0x00, 0x00};

s32 func_ov011_02127c84(Ov003EnemyActor* owner) {
    Ov003EnemyActor* data;
    TaskPool*        pool;

    data = owner;
    if (func_ov003_020c37f8(&data->sprite) == 0) {
        pool = &g_CombatGlobal->unk_00000;
    } else {
        pool = &g_CombatGlobal->taskPool;
    }
    return EasyTask_CreateTask(pool, &data_ov011_0212c1d4, 0, 0, 0, &data);
}

static s32 func_ov011_02127ce0(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm010Rnge*    data;
    Ov003EnemyActor** spawn;
    s32               r;
    s32               snd;
    s32               pan;
    s32               bias;
    s32               y;

    data  = task->data;
    spawn = args;
    r     = 1;
    switch (stage) {
        case 0:
            MI_CpuSet(data, 0, 0x6C);
            func_ov011_021258b4((*spawn)->sprite.sprite.bits_0_1, &data->sprite, 0);
            BtlSprite_SetAnimFromTable(&data->sprite, 0, r);
            data->unk_060 = *spawn;
            if ((*spawn)->actor.isFlipped == 1) {
                BtlSprite_SetFlip(&data->sprite, r);
            } else {
                BtlSprite_SetFlip(&data->sprite, 0);
            }
            break;
        case 1:
            if (func_ov003_020c3c28() != 0) {
                return 0;
            }
            if (func_ov003_020cc354(&data->unk_060->actor) != 0 || (data->unk_060->actor.flags & ACTOR_FLAG_HELD) != 0 ||
                (data->unk_060->actor.flags & ACTOR_FLAG_LAUNCHED) != 0)
            {
                return 0;
            }
            BtlSprite_Update(&data->sprite);
            if (func_ov003_020c37f8(&data->sprite) != 0) {
                snd = 0x5B;
                pan = BtlStage_ToScreenX(r, data->unk_060->actor.position.x);
            } else {
                snd = 0x53;
                pan = BtlStage_ToScreenX(0, data->unk_060->actor.position.x);
            }
            bias = 0x80000;
            if (data->unk_060->actor.isFlipped == 0) {
                bias = -bias;
            }
            if (data->sprite.sprite.cellIndex == 2 && data->sprite.sprite.frameTimer == 1) {
                CombatSe_PlayPanned(SEIDX_SE_010_FIREATTACK, pan);
            }
            if (data->sprite.sprite.cellIndex >= 4 && data->sprite.sprite.cellIndex <= 6) {
                if (func_ov003_020c5b2c((u16)snd, data->unk_060->actor.position.x + bias, data->unk_060->actor.position.y,
                                        data->unk_060->actor.position.z, data->unk_060->actor.position.z) == 1)
                {
                    CombatSe_PlayPanned(SEIDX_SE_010_FIREATTACK_HIT, pan);
                }
            }
            if (SpriteMgr_IsAnimationFinished(&data->sprite.sprite) != 0) {
                r = 0;
            }
            break;
        case 2:
            if (data->sprite.sprite.bits_0_1 == 0) {
                pan = BtlStage_ToScreenX(0, data->unk_060->actor.position.x);
                y   = BtlStage_ToScreenY(0, data->unk_060->actor.position.y, data->unk_060->actor.position.z);
            } else {
                pan = BtlStage_ToScreenX(r, data->unk_060->actor.position.x);
                y   = BtlStage_ToScreenY(r, data->unk_060->actor.position.y, data->unk_060->actor.position.z);
            }
            if (data->unk_060->actor.isFlipped == 0) {
                pan = (s16)(pan - 0x20);
            } else {
                pan = (s16)(pan + 0x20);
            }
            BtlSprite_SetPosition(&data->sprite, pan, y);
            BtlSprite_SetSortKey(&data->sprite, 0x7FFFFFFF - (data->unk_060->actor.position.y + 0x20000));
            BtlSprite_Render(&data->sprite);
            break;
        case 3:
            BtlSprite_Release(&data->sprite);
            break;
    }
    return r;
}
