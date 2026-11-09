#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm010.h"
#include "Engine/Math/Random.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlEnm010LserRec {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 pad_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} BtlEnm010LserRec;

typedef struct BtlEnm010LserEmit {
    /* 0x00 */ BtlEnm010LserRec rec[3];
    /* 0x24 */ s16              unk_24;
    /* 0x26 */ s16              unk_26;
    /* 0x28 */ s16              unk_28;
    /* 0x2A */ u16              pad_2A;
} BtlEnm010LserEmit;

typedef struct BtlEnm010Lser {
    /* 0x000 */ Ov003EnemyActor*  unk_00;
    /* 0x004 */ BattleActor       copy; // the owner's actor, refreshed every frame
    /* 0x080 */ BattleSprite      sprite[4];
    /* 0x200 */ BtlEnm010LserEmit emit;
    /* 0x22C */ s32               unk_22C;
    /* 0x230 */ s32               unk_230;
    /* 0x234 */ s32               unk_234;
    /* 0x238 */ s32               pad_238;
    /* 0x23C */ s32               unk_23C;
    /* 0x240 */ s32               unk_240;
    /* 0x244 */ s32               unk_244;
    /* 0x248 */ s32               unk_248;
    /* 0x24C */ s32               unk_24C;
    /* 0x250 */ u8                unk_250;
    /* 0x251 */ u8                pad_251[3];
} BtlEnm010Lser;

typedef struct BtlEnm010LserArgs {
    /* 0x00 */ Ov003EnemyActor* unk_00;
    /* 0x04 */ s32              pad_04[3];
    /* 0x10 */ s32              unk_10;
    /* 0x14 */ s32              unk_14;
} BtlEnm010LserArgs;

static char data_ov011_0212cbe8[20];

static s32 func_ov011_02125cf8(TaskPool* pool, Task* task, void* args, s32 stage);
static s32 func_ov011_02125d48(BtlEnm010Lser* data, BtlEnm010LserArgs* args);
static s32 func_ov011_02125e14(BtlEnm010Lser* data);
static s32 func_ov011_02125f24(BtlEnm010Lser* data);
static s32 func_ov011_02126064(BtlEnm010Lser* data);
static s32 func_ov011_02126354(BtlEnm010Lser* data);
static s32 func_ov011_021265a8(BtlEnm010Lser* data);

static s32 func_ov011_021260e8(BtlEnm010Lser* data);

static const TaskHandle data_ov011_0212c118 = {(const char*)data_ov011_0212cbe8, func_ov011_02125cf8, 596};

static const s32 data_ov011_0212c124[3] = {-32768, -65536, 0};

static char data_ov011_0212cbe8[20] = {0x54, 0x73, 0x6B, 0x5F, 0x42, 0x74, 0x6C, 0x45, 0x6E, 0x6D,
                                       0x30, 0x31, 0x30, 0x5F, 0x4C, 0x73, 0x65, 0x72, 0x00, 0x00};

s32 func_ov011_02125b98(Ov003EnemyActor* owner, s32 arg1) {
    BtlEnm010LserArgs args;
    TaskPool*         pool;

    if (func_ov003_020c37f8(&owner->sprite) == 0) {
        pool = &g_CombatGlobal->unk_00000;
    } else {
        pool = &g_CombatGlobal->taskPool;
    }
    args.unk_00 = owner;
    args.unk_14 = arg1;
    return EasyTask_CreateTask(pool, &data_ov011_0212c118, 0, 0, 0, &args);
}

static void func_ov011_02125c00(s32* outX, s32* outY, s32* outZ, BattleActor* owner, s32 index) {
    *outX = (owner->isFlipped == 0) ? owner->position.x - 0x40000 : owner->position.x + 0x40000;
    *outY = owner->position.y;
    *outZ = owner->position.z + data_ov011_0212c124[index];
}

static void func_ov011_02125c44(BattleActor* owner, BtlEnm010LserRec* rec, BattleSprite* sprite, s32 enabled) {
    s32 v = rec->unk_04 + rec->unk_08;

    rec->unk_04 = v;
    if (v >= 0x8000) {
        return;
    }
    if (enabled == 0) {
        return;
    }
    if (RNG_Next(0x64) < 0x32) {
        BtlSprite_SetAnimFromTable(sprite, 1, 1);
    } else {
        BtlSprite_SetAnimFromTable(sprite, 2, 1);
    }
    BtlSprite_Restart(sprite);
    rec->unk_00 = RNG_Next(0x4000);
    if (owner->isFlipped == 0) {
        rec->unk_00 = rec->unk_00 + 0x6000;
    } else {
        rec->unk_00 = rec->unk_00 - 0x2000;
    }
    rec->unk_04 = 0x20000;
    rec->unk_08 = 0 - (RNG_Next(0x1001) + 0x1000);
}

static s32 func_ov011_02125cf8(TaskPool* pool, Task* task, void* args, s32 stage) {
    BtlEnm010Lser* data = task->data;

    switch (stage) {
        case 0:
            return func_ov011_02125d48(data, args);
        case 1:
            return func_ov011_02125e14(data);
        case 2:
            return func_ov011_02126354(data);
        case 3:
            return func_ov011_021265a8(data);
        default:
            return 1;
    }
}

static s32 func_ov011_02125d48(BtlEnm010Lser* data, BtlEnm010LserArgs* args) {
    s32           i;
    BattleSprite* sp;

    MI_CpuSet(data, 0, 0x254);
    sp = data->sprite;
    for (i = 0; i < 4; i++) {
        func_ov011_021258b4(args->unk_00->sprite.sprite.bits_0_1, sp, 2);
        sp++;
    }
    data->unk_00  = args->unk_00;
    data->unk_23C = 0;
    data->unk_240 = 0;
    data->unk_244 = args->unk_10;
    if (args->unk_00->actor.isFlipped == 0) {
        data->unk_244 = 0 - data->unk_244;
    }
    data->unk_24C = args->unk_14;
    if (args->unk_14 == 0) {
        data->emit.unk_28 = 0;
    } else {
        data->emit.unk_28 = 1;
    }
    if (func_ov003_020c37f8(&args->unk_00->sprite) != 0) {
        data->unk_250 |= 8;
    } else {
        data->unk_250 &= ~8;
    }
    return 1;
}

static s32 func_ov011_02125e14(BtlEnm010Lser* data) {
    s32           result = 0;
    s32           i;
    BattleSprite* sp;

    if (func_ov003_020c3c28() != 0) {
        return 0;
    }
    if (data->unk_00 != NULL) {
        if (data->unk_00->actor.flags & ACTOR_FLAG_DEFEATED) {
            return 0;
        }
    }
    if (data->unk_00 != NULL) {
        if (func_ov003_020cc354(&data->copy) != 0 || (data->copy.flags & ACTOR_FLAG_LAUNCHED) != 0 ||
            (data->copy.flags & ACTOR_FLAG_HELD) != 0)
        {
            data->unk_00 = NULL;
        }
    }
    if (data->unk_00 != NULL) {
        data->copy = data->unk_00->actor;
    }
    switch (data->emit.unk_28) {
        case 0:
            result = func_ov011_02125f24(data);
            break;
        case 1:
            result = func_ov011_02126064(data);
            break;
        case 2:
            result = func_ov011_021260e8(data);
            break;
    }
    sp = data->sprite;
    for (i = 0; i < 4; i++) {
        BtlSprite_Update(sp);
        sp++;
    }
    return result;
}

static s32 func_ov011_02125f24(BtlEnm010Lser* data) {
    s32               flag;
    s32               i;
    BattleSprite*     sp;
    BtlEnm010LserRec* rec;

    if (data->unk_00 == NULL) {
        return 0;
    }
    if (data->emit.unk_24 == 0) {
        BtlSprite_SetAnimFromTable(&data->sprite[0], 1, 1);
        BtlSprite_SetAnimFromTable(&data->sprite[1], 1, 1);
        BtlSprite_SetAnimFromTable(&data->sprite[2], 1, 1);
        BtlSprite_SetAnimFromTable(&data->sprite[3], 3, 0);
        data->unk_250     = (data->unk_250 & ~1) | 1;
        data->unk_250     = data->unk_250 & ~2;
        data->emit.unk_26 = 0x2F;
        CombatSe_PlayPanned((SndMgrSeIdx)0x1E5, BtlStage_ToScreenX(0, data->copy.position.x));
    }
    flag = 1;
    if (data->emit.unk_24 > 0x24) {
        flag = 0;
    }
    sp  = data->sprite;
    rec = data->emit.rec;
    for (i = 0; i < 3; i++) {
        func_ov011_02125c44(&data->copy, rec, sp, flag);
        sp++;
        rec++;
    }
    if (data->emit.unk_24 == 8) {
        data->unk_250 |= 2;
    }
    if (data->emit.unk_24 < data->emit.unk_26) {
        data->emit.unk_24 = data->emit.unk_24 + 1;
    } else {
        data->emit.unk_24 = 0;
        data->emit.unk_28 = 2;
    }
    return 1;
}

static s32 func_ov011_02126064(BtlEnm010Lser* data) {
    if (data->unk_00 == NULL) {
        return 0;
    }
    if (data->emit.unk_24 == 0) {
        BtlSprite_SetAnimFromTable(&data->sprite[3], 3, 0);
        data->unk_250     = data->unk_250 | 2;
        data->emit.unk_26 = 4;
    }
    if (data->emit.unk_24 < data->emit.unk_26) {
        data->emit.unk_24 = data->emit.unk_24 + 1;
    } else {
        data->emit.unk_24 = 0;
        data->emit.unk_28 = 2;
    }
    return 1;
}

static s32 func_ov011_021260e8(BtlEnm010Lser* data) {
    s32 mode;

    if (((u32)(data->unk_250 << 28) >> 31) != 0) {
        mode = 1;
    } else {
        mode = 0;
    }
    if (data->emit.unk_24 == 0) {
        data->emit.unk_24 = data->emit.unk_24 + 1;
        BtlSprite_SetAnimFromTable(&data->sprite[0], 5, 0);
        BtlSprite_SetAnimFromTable(&data->sprite[3], 4, 1);
        func_ov011_02125c00(&data->unk_22C, &data->unk_230, &data->unk_234, &data->copy, data->unk_24C);
        if (data->copy.isFlipped == 0) {
            data->unk_248 = 0 - 0x8000;
            data->unk_22C = data->unk_22C - 0x28000;
        } else {
            data->unk_248 = 0x8000;
            data->unk_22C = data->unk_22C + 0x28000;
            BtlSprite_SetFlip(&data->sprite[0], 1);
            BtlSprite_SetFlip(&data->sprite[3], 1);
        }
        data->unk_250 &= ~1;
    }
    if (SpriteMgr_IsAnimationFinished(&data->sprite[3].sprite) != 0) {
        data->unk_250 &= ~2;
    }
    if (data->emit.unk_24 == 0x11) {
        data->unk_250 |= 4;
        CombatSe_PlayPanned(SEIDX_SE_010_LAZERATTACK, BtlStage_ToScreenX(mode, data->copy.position.x));
    }
    if ((u32)(data->unk_250 << 29) >> 31) {
        data->unk_22C = data->unk_22C + data->unk_248;
        if (func_ov003_020cc7f4(BtlStage_ToScreenX(mode, data->unk_22C), 0, 0x60, 0) == 0) {
            return 0;
        }
    }
    if (data->emit.unk_24 >= 0xA) {
        Ov003AttackInfo rec;
        s32             x;
        s32             idx;

        if (func_ov003_020c37f8(&data->sprite[0]) != 0) {
            idx = 0x5E;
        } else {
            idx = 0x56;
        }
        rec = *CombatHit_GetAttackInfo((u16)idx);
        if (data->emit.unk_24 < 0xE) {
            rec.unk_05 = 0x18;
            rec.unk_07 = 0x20;
            x          = (data->copy.isFlipped == 0) ? data->unk_22C + 0x18000 : data->unk_22C - 0x18000;
        } else if (data->emit.unk_24 < 0x11) {
            rec.unk_05 = 0x20;
            rec.unk_07 = 0x10;
            x          = (data->copy.isFlipped == 0) ? data->unk_22C + 0x28000 : data->unk_22C - 0x18000;
        } else {
            x = data->unk_22C;
        }
        if (CombatHit_AttackAt(&rec, &data->copy.baseArgs, x, data->unk_230, data->unk_234) == 1) {
            CombatSe_PlayPanned(SEIDX_SE_010_BUBBLEATTACK_HIT, BtlStage_ToScreenX(mode, x));
        }
    }
    data->emit.unk_24 = data->emit.unk_24 + 1;
    return 1;
}

static s32 func_ov011_02126354(BtlEnm010Lser* data) {
    s16               t1;
    s16               t0;
    s32               acc0;
    s32               acc1;
    s32               outX;
    s32               outY;
    s32               outZ;
    s32               vx;
    s32               vy;
    s32               i;
    BtlEnm010LserRec* rec;
    BattleSprite*     sp;
    s32               mode;

    if (func_ov003_020c3c28() != 0) {
        return 0;
    }
    if (func_ov003_020c37f8(&data->sprite[0]) != 0) {
        mode = 1;
    } else {
        mode = 0;
    }
    func_ov011_02125c00(&outX, &outY, &outZ, &data->copy, data->unk_24C);
    vx = FX32_CONVERT(BtlStage_ToScreenX(mode, outX));
    vy = FX32_CONVERT(BtlStage_ToScreenY(mode, outY, outZ));

    if ((u32)(data->unk_250 << 31) >> 31) {
        rec = &data->emit.rec[0];
        sp  = data->sprite;
        for (i = 0; i < 3; i++) {
            func_ov003_020cbc50(&acc0, &acc1, rec->unk_00, rec->unk_04);
            acc0 += vx;
            acc1 += vy;
            BtlSprite_SetPosition(sp, (acc0 * 16) >> 16, (acc1 * 16) >> 16);
            BtlSprite_SetSortKey(sp, 0x7FFFFFFE - outY);
            BtlSprite_Render(sp);
            rec++;
            sp++;
        }
    }

    if ((u32)(data->unk_250 << 30) >> 31) {
        BtlSprite_SetPosition(&data->sprite[3], (vx * 16) >> 16, (vy * 16) >> 16);
        BtlSprite_SetSortKey(&data->sprite[3], 0x7FFFFFFF - outY);
        BtlSprite_Render(&data->sprite[3]);
    }

    if ((u32)(data->unk_250 << 29) >> 31) {
        BtlStage_WorldToScreen(mode, &t1, &t0, data->unk_22C, data->unk_230, data->unk_234);
        BtlSprite_SetPosition(&data->sprite[0], t1, t0);
        BtlSprite_SetSortKey(&data->sprite[0], 0x7FFFFFFD - outY);
        BtlSprite_Render(&data->sprite[0]);
    }
    return 1;
}

static s32 func_ov011_021265a8(BtlEnm010Lser* data) {
    s32           i;
    BattleSprite* sp;

    sp = data->sprite;
    for (i = 0; i < 4; i++) {
        BtlSprite_Release(sp);
        sp++;
    }
    return 1;
}
