#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm010.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

typedef struct BtlEnm010Shot {
    /* 0x00 */ s32 unk_00; // distance along the heading
    /* 0x04 */ s32 unk_04; // speed
    /* 0x08 */ s32 unk_08; // acceleration
    /* 0x0C */ s16 unk_0C; // screen x
    /* 0x0E */ s16 unk_0E; // screen y
} BtlEnm010Shot;           // Size: 0x10

typedef struct BtlEnm010SingleShot {
    /* 0x00 */ Ov003EnemyActor* unk_000;
    /* 0x04 */ BattleSprite     sprite;
    /* 0x64 */ s16              unk_064; // launch SE played
    /* 0x66 */ u16              unk_066;
    /* 0x68 */ BtlEnm010Shot    shot[3];
    /* 0x98 */ s32              unk_098; // owner x at launch
    /* 0x9C */ s32              unk_09C; // origin
    /* 0xA0 */ s32              unk_0A0;
    /* 0xA4 */ s32              unk_0A4;
    /* 0xA8 */ s16              unk_0A8; // launch delay
    /* 0xAA */ u16              unk_0AA; // heading
    /* 0xAC */ BattleAttackArgs args;    // copy of the owner's baseArgs
} BtlEnm010SingleShot;                   // Size: 0xB4

typedef struct BtlEnm010SingleShotArgs {
    /* 0x00 */ Ov003EnemyActor* field_00;
    /* 0x04 */ s32              field_04; // offset from the owner
    /* 0x08 */ s32              field_08;
    /* 0x0C */ s32              field_0C;
    /* 0x10 */ s32              field_10; // speed
    /* 0x14 */ u16              field_14; // heading
    /* 0x16 */ s16              field_16; // launch delay
} BtlEnm010SingleShotArgs;

static char data_ov011_0212cc38[28];

static s32 func_ov011_02128348(TaskPool* pool, Task* task, void* args, s32 stage);
static s32 func_ov011_021283a8(BtlEnm010SingleShot* data, BtlEnm010SingleShotArgs* arg1);
static s32 func_ov011_02128698(BtlEnm010SingleShot* p);
static s32 func_ov011_02128704(BtlEnm010SingleShot* p);

static s32 func_ov011_021284bc(BtlEnm010SingleShot* p);

static const s32 data_ov011_0212c1ec[3] = {4096, 3584, 3072};

static const TaskHandle data_ov011_0212c1f8 = {(const char*)data_ov011_0212cc38, func_ov011_02128348, 180};

static const s32 data_ov011_0212c204[3] = {4096, 3277, 2458};

static char data_ov011_0212cc38[28] = {0x54, 0x73, 0x6B, 0x5F, 0x42, 0x74, 0x6C, 0x45, 0x6E, 0x6D, 0x30, 0x31, 0x30, 0x5F,
                                       0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x53, 0x68, 0x6F, 0x74, 0x00, 0x00, 0x00, 0x00};

s32 func_ov011_021282b8(Ov003EnemyActor* owner, s32 arg1, s32 arg2, s32 arg3, u16 arg4, s32 arg5, s16 arg6) {
    BtlEnm010SingleShotArgs t;
    TaskPool*               pool;

    if (func_ov003_020c37f8(&owner->sprite) == 0) {
        pool = &g_CombatGlobal->unk_00000;
    } else {
        pool = &g_CombatGlobal->taskPool;
    }
    t.field_00 = owner;
    t.field_04 = arg1;
    t.field_08 = arg2;
    t.field_0C = arg3;
    t.field_10 = arg5;
    t.field_16 = arg6;
    t.field_14 = arg4;
    return EasyTask_CreateTask(pool, &data_ov011_0212c1f8, 0, 0, 0, (void*)&t);
}

static s32 func_ov011_02128348(TaskPool* pool, Task* task, void* args, s32 stage) {
    void* p;
    s32   r;

    p = task->data;
    r = 1;
    switch (stage) {
        case 0:
            r = func_ov011_021283a8(p, args);
            break;
        case 1:
            r = func_ov011_021284bc(p);
            break;
        case 2:
            r = func_ov011_02128698(p);
            break;
        case 3:
            r = func_ov011_02128704(p);
            break;
    }
    return r;
}

static s32 func_ov011_021283a8(BtlEnm010SingleShot* data, BtlEnm010SingleShotArgs* arg1) {
    s32              i;
    s32              v;
    Ov003EnemyActor* o;

    MI_CpuSet(data, 0, 0xB4);
    func_ov011_021258b4(arg1->field_00->sprite.sprite.bits_0_1, &data->sprite, 2);
    BtlSprite_SetAnimFromTable(&data->sprite, 0, 0);
    for (i = 0; i < 3; i++) {
        data->shot[i].unk_08 = (s32)((((long long)data_ov011_0212c204[i] * (long long)(arg1->field_10)) + 0x800) >> 12);
    }
    data->unk_000            = arg1->field_00;
    data->unk_098            = arg1->field_00->actor.position.x;
    data->unk_09C            = arg1->field_00->actor.position.x + arg1->field_04;
    data->unk_0A0            = arg1->field_00->actor.position.y + arg1->field_08;
    data->unk_0A4            = arg1->field_00->actor.position.z + arg1->field_0C;
    data->unk_0A8            = arg1->field_16;
    data->unk_0AA            = arg1->field_14;
    o                        = arg1->field_00;
    data->args.power         = o->actor.baseArgs.power;
    data->args.comboFrames   = o->actor.baseArgs.comboFrames;
    data->args.powerPercent  = o->actor.baseArgs.powerPercent;
    data->args.effectPercent = o->actor.baseArgs.effectPercent;
}

static s32 func_ov011_021284bc(BtlEnm010SingleShot* p) {
    s32            r;
    s32            mode;
    u16            id;
    s32            pan;
    s32            oa;
    s32            ob;
    s16            va;
    s16            vb;
    s32            i;
    BtlEnm010Shot* rec;

    r = 0;
    if (func_ov003_020c3c28() != 0) {
        return r;
    }
    if (p->unk_000 != NULL && (p->unk_000->actor.flags & ACTOR_FLAG_DEFEATED) != 0) {
        return r;
    }
    BtlSprite_Update(&p->sprite);
    if (func_ov003_020c37f8(&p->sprite) != 0) {
        mode = 1;
        id   = 0x5C;
        pan  = BtlStage_ToScreenX(mode, p->unk_098);
    } else {
        mode = 0;
        id   = 0x54;
        pan  = BtlStage_ToScreenX(mode, p->unk_098);
    }
    if (p->unk_064 == 0) {
        p->unk_064 = p->unk_064 + 1;
        BtlSe_PlayPanned(SEIDX_SE_010_BUBBLEATTACK, pan);
    }
    BtlStage_WorldToScreen(mode, &va, &vb, p->unk_09C, p->unk_0A0, p->unk_0A4);
    rec = p->shot;
    for (i = 0; i < 3; i++) {
        if (p->unk_0A8 > 0) {
            p->unk_0A8 = p->unk_0A8 - 1;
        } else {
            rec->unk_00 = rec->unk_00 + rec->unk_04;
            rec->unk_04 = rec->unk_04 + rec->unk_08;
        }
        func_ov003_020cbc50(&oa, &ob, p->unk_0AA, rec->unk_00);
        rec->unk_0C = (oa + (va << 12)) >> 12;
        rec->unk_0E = (ob + (vb << 12)) >> 12;
        if (BtlHit_AttackAtScreen(BtlHit_GetAttackInfo(id), &p->args, rec->unk_0C, rec->unk_0E) == 1) {
            BtlSe_PlayPanned(SEIDX_SE_010_BUBBLEATTACK_HIT, rec->unk_0C);
        }
        {
            s32 v = BtlStage_ToWorldX(mode, rec->unk_0C);
            if (v < 0 - 0x8000 || v > func_ov003_020cb744(mode) + 0x8000) {
                r++;
            }
        }
        rec++;
    }
    return (r == 3) ? 0 : 1;
}

static s32 func_ov011_02128698(BtlEnm010SingleShot* p) {
    BtlEnm010SingleShot* data;
    s32                  base;
    s32                  i;
    BtlEnm010Shot*       rec;

    data = p;
    base = 0;
    rec  = data->shot;
    for (i = 0; i < 3; i++) {
        BtlSprite_SetAffineTransform(&data->sprite, base, data_ov011_0212c1ec[i], data_ov011_0212c1ec[i], base);
        BtlSprite_SetPosition(&data->sprite, rec->unk_0C, rec->unk_0E);
        BtlSprite_Render(&data->sprite);
        rec++;
    }
    return 1;
}

static s32 func_ov011_02128704(BtlEnm010SingleShot* p) {
    BtlSprite_Release(&p->sprite);
    return 1;
}
