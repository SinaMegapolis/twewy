#include "Combat/Noise/Private/BtlEnm014.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Save.h"

#include <nitro/mi/cpumem.h>

static char data_ov012_02127990[28];

static char                  data_ov012_021279ac[28];
static char                  data_ov012_021279c8[28];
static char                  data_ov012_021279e4[28];
static const BinIdentifier   data_ov012_02127894;
static const BinIdentifier   data_ov012_0212789c;
static const BinIdentifier   data_ov012_021278a4;
static const BinIdentifier   data_ov012_021278ac;
static SpriteAnimEntry const data_ov012_021278b4[8][2];

static const u16 data_ov012_0212783c[4] = {0x10, 0x16, 0x16, 0x16};

static const BattleSpriteDesc data_ov012_02127844 = {&data_ov012_02127894, &data_ov012_021278b4[0][0], 0, 0x10, 0xD8};

static const SpriteAnimEntry data_ov012_02127854[2] = {
    {   4,    6,    5, 0},
    {0x13, 0x15, 0x14, 0},
};

static const BattleSpriteDesc data_ov012_02127864 = {&data_ov012_0212789c, &data_ov012_021278b4[0][0], 0, 0x16, 0xF0};

static const BattleSpriteDesc data_ov012_02127874 = {&data_ov012_021278ac, &data_ov012_021278b4[0][0], 0, 0x16, 0x104};

static const BattleSpriteDesc data_ov012_02127884 = {&data_ov012_021278a4, &data_ov012_021278b4[0][0], 0, 0x16, 0x104};

static const BinIdentifier data_ov012_02127894 = {3, data_ov012_02127990};

static const BinIdentifier data_ov012_0212789c = {3, data_ov012_021279ac};

static const BinIdentifier data_ov012_021278a4 = {3, data_ov012_021279c8};

static const BinIdentifier data_ov012_021278ac = {3, data_ov012_021279e4};

static const SpriteAnimEntry data_ov012_021278b4[8][2] = {
    {         {1, 3, 2, 0},          {1, 3, 2, 1}},
    {         {7, 9, 8, 0},          {7, 9, 8, 1}},
    {   {0xA, 0xC, 0xB, 0},    {0xA, 0xC, 0xB, 1}},
    {   {0xA, 0xC, 0xB, 2},    {0xA, 0xC, 0xB, 3}},
    {   {0xA, 0xC, 0xB, 4},    {0xA, 0xC, 0xB, 0}},
    {   {0xA, 0xC, 0xB, 5},    {0xA, 0xC, 0xB, 2}},
    {   {0xD, 0xF, 0xE, 0},    {0xD, 0xF, 0xE, 1}},
    {{0x10, 0x12, 0x11, 0}, {0x10, 0x12, 0x11, 1}},
};

BattleSpriteDesc* data_ov012_02127980[4] = {
    &data_ov012_02127844,
    &data_ov012_02127864,
    &data_ov012_02127884,
    &data_ov012_02127874,
};

static char data_ov012_02127990[28] = "Apl_Hor/Grp_BtlEnm014.bin";

static char data_ov012_021279ac[28] = "Apl_Hor/Grp_BtlEnm014a.bin";

static char data_ov012_021279c8[28] = "Apl_Hor/Grp_BtlEnm014b.bin";

static char data_ov012_021279e4[28] = "Apl_Hor/Grp_BtlEnm014c.bin";

BinIdentifier* func_ov012_021256c0(s32 index) {
    return (BinIdentifier*)&data_ov012_02127894 + index;
}

const SpriteAnimEntry* func_ov012_021256d0(void) {
    return data_ov012_02127854;
}

u16 func_ov012_021256dc(s32 index) {
    return data_ov012_0212783c[index];
}

void func_ov012_021256f0(s32 engine, BtlEnm014* data, s32 arg2) {
    Enm014Spawn spawn;
    MI_CpuSet(&spawn, 0, sizeof(spawn));
    spawn.unk_00 = data;
    spawn.unk_04 = engine;
    spawn.unk_08 = (u16)arg2;

    TaskPool* pool;
    if (engine == 0) {
        pool = &gBattle->mainPool;
    } else {
        pool = &gBattle->subPool;
    }
    EasyTask_CreateTask(pool, &Tsk_BtlEnm014_Eff, NULL, 0, NULL, &spawn);
}

void func_ov012_02125768(BtlEnm014* data, void (*fn)(BtlEnm014*)) {
    func_ov003_020c427c(&data->base);
    data->unk_1C4 = fn;
    data->unk_1C2 = 0;
    data->unk_1C0 = 0;
}

void func_ov012_02125790(s32 engine, BtlEnm014* data, void* args, void (*next)(BtlEnm014*)) {
    data->unk_1CC = 1;
    data->unk_1C8 = next;
    func_ov003_020c3efc(&data->base, args);
    func_ov003_020ccf58(engine, &data->base);
}

void func_ov012_021257c4(BtlEnm014* data) {
    if (func_ov003_020c5bfc(&data->base) != 0) {
        return;
    }
    func_ov012_02125768(data, data->unk_1C8);
}

void func_ov012_021257e8(BtlEnm014* data) {
    if (func_ov003_020ccfc8(&data->base, 4) != 0) {
        return;
    }
    func_ov012_02125768(data, data->unk_1C8);
}

void func_ov012_02125810(BtlEnm014* data) {
    if (func_ov003_020ccfec(&data->base) == 0) {
        data->unk_1CC = 0;
    }
}

void func_ov012_0212582c(BtlEnm014* data) {
    if (func_ov003_020c72b4(&data->base, 0, 7) != 0) {
        return;
    }
    func_ov012_02125768(data, data->unk_1C8);
}

void func_ov012_02125858(BtlEnm014* data) {
    BattleActor* target;

    if (data->base.sprite.sprite.bits_0_1 == 0) {
        target = gBattle->player;
    } else {
        target = gBattle->partner;
    }
    data->base.actor.damageTakenPercent = 100;
    if (data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        if (data->base.actor.position.x < target->position.x) {
            goto hit;
        }
    }
    if (!(data->base.sprite.flags & BTL_SPRITE_FLAG_FLIPPED)) {
        if (data->base.actor.position.x > target->position.x) {
            goto hit;
        }
    }
    return;

hit:
    data->base.actor.damageTakenPercent = 50;
    if (data->base.spriteIndex == 3) {
        data->base.actor.damageTakenPercent = 0;
    }
}

void func_ov012_021258e8(void) {
    if (gBattle->encounterId != 0x90) {
        return;
    }
    if (gSaveData.battleClearTime <= 0x654) {
        return;
    }
    gBattle->goal = BATTLE_GOAL_SCRIPTED;
    gBattle->flags |= BATTLE_FLAG_SCRIPTED_END;
    gBattle->flags |= BATTLE_FLAG_SCRIPTED_GOAL_MET;
}

void func_ov012_02125958(BtlEnm014* data) {
    if (data->unk_1D4 == 0 && data->base.actor.invincibleTimer != 0 && data->base.actor.damageTakenPercent != 0x64) {
        func_ov012_021256f0(data->base.sprite.sprite.bits_0_1, data, 0);
        func_ov003_020c4cc4(&data->base, 0x21E);
    }
    data->unk_1D4 = data->base.actor.invincibleTimer;
}
