#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Noise/Private/BtlEnm006.h"

#include "Combat/Core/CombatApi.h"
#include <nitro/mi/cpumem.h>

typedef struct Enm006DeadEff {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ Vec          position;
} Enm006DeadEff; // Size: 0x6C (from the Tsk_BtlEnm006_DeadEff TaskHandle)

typedef struct Enm006AnimEntryPad2 {
    /* 0x00 */ SpriteAnimEntry entry;
    /* 0x08 */ u8              pad[2];
} Enm006AnimEntryPad2; // Size: 0xA

static s32  func_ov010_02125730(TaskPool* pool, Task* task, void* args, s32 stage);
static s32  func_ov010_02125780(Enm006DeadEff*, Enm006Spawn*);
static char data_ov010_0212938c[24];
static s32  func_ov010_02125854(Enm006DeadEff* data);
static s32  func_ov010_02125878(Enm006DeadEff* data);
static s32  func_ov010_02125900(Enm006DeadEff* data);

static const s16 data_ov010_02129200[3] = {0x17, 0x17, 0x17};

static const SpriteAnimEntry data_ov010_02129206[1] = {
    {16, 18, 17, 0},
};

static const SpriteAnimEntry data_ov010_0212920e[1] = {
    {16, 18, 17, 0},
};

static const Enm006AnimEntryPad2 data_ov010_02129216 = {
    {16, 18, 17, 0},
    {0, 0},
};

static const TaskHandle Tsk_BtlEnm006_DeadEff = {data_ov010_0212938c, func_ov010_02125730, 0x6C};

static SpriteAnimEntry* data_ov010_02129380[3] = {
    (SpriteAnimEntry*)&data_ov010_02129216.entry,
    &data_ov010_0212920e[0],
    &data_ov010_02129206[0],
};

static char data_ov010_0212938c[24] = "Tsk_BtlEnm006_DeadEff";

void func_ov010_021256d0(BtlEnm006* owner, BtlEnm006* target) {
    BtlEnm006* args[2];
    TaskPool*  pool;
    args[0] = owner;
    args[1] = target;
    if (func_ov003_020c37f8(&target->base.sprite) == 0) {
        pool = &g_CombatGlobal->unk_00000;
    } else {
        pool = &g_CombatGlobal->taskPool;
    }
    EasyTask_CreateTask(pool, &Tsk_BtlEnm006_DeadEff, 0, 0, 0, args);
}

static s32 func_ov010_02125730(TaskPool* pool, Task* task, void* args, s32 stage) {
    void* data = task->data;
    switch (stage) {
        case 0:
            return func_ov010_02125780(data, args);
        case 1:
            return func_ov010_02125854(data);
        case 2:
            return func_ov010_02125878(data);
        case 3:
            return func_ov010_02125900(data);
    }
    return 1;
}

static s32 func_ov010_02125780(Enm006DeadEff* data, Enm006Spawn* args) {
    s32 m;
    u16 v;
    MI_CpuSet(data, 0, 0x6C);
    v = args->unk_00->base.spriteIndex;
    m = args->unk_00->base.sprite.sprite.bits_0_1;
    BtlSprite_LoadFromTable(m, &data->sprite, func_ov010_021256c0(v), data_ov010_02129380[v], 0, data_ov010_02129200[v], 0x30);
    BtlSprite_SetAnimFromTable(&data->sprite, 0, 1);
    BtlSprite_SetFlip(&data->sprite, args->unk_00->base.actor.isFlipped == 1);
    data->position.x = args->unk_00->base.actor.position.x;
    data->position.y = args->unk_00->base.actor.position.y;
    data->position.z = args->unk_00->base.actor.position.z;
    return 1;
}

static s32 func_ov010_02125854(Enm006DeadEff* data) {
    BtlSprite_Update(&data->sprite);
    return SpriteMgr_IsAnimationFinished(&data->sprite.sprite) == 0;
}

static s32 func_ov010_02125878(Enm006DeadEff* data) {
    if (func_ov003_020c3c28() != 0) {
        return 0;
    }
    s16 x;
    s16 y;
    s32 flag = func_ov003_020c37f8(&data->sprite) != 0;
    CombatStage_WorldToScreen(flag, &x, &y, data->position.x, data->position.y, data->position.z);
    BtlSprite_SetPosition(&data->sprite, x, y);
    BtlSprite_SetSortKey(&data->sprite, 0x80000000 - data->position.y);
    BtlSprite_Render(&data->sprite);
    return 1;
}

static s32 func_ov010_02125900(Enm006DeadEff* data) {
    BtlSprite_Release(&data->sprite);
    return 1;
}
