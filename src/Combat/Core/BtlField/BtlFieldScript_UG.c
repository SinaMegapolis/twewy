#include "Combat/Core/BtlFieldData.h"

static const Ov003BgAnimFrame   data_ov003_020d9ad0[7];
static const BattleBgScriptWord data_ov003_020d9a80[4];
static const BattleBgScriptWord data_ov003_020d9a90[6];
static const BattleBgScriptWord data_ov003_020d9aa8[10];

const BattleBgScriptWord* data_ov003_020e3f0c[1] = {data_ov003_020d9a90};

const BattleBgScriptWord* data_ov003_020e3f10[2] = {data_ov003_020d9a80, data_ov003_020d9aa8};

static const BattleBgScriptWord data_ov003_020d9a80[4] = {
    {.run = BtlBgScript_CmdLoadPalette},
    {.value = 0x00030001},
    {.value = 0x000D0002},
    {.run = BtlBgScript_CmdEnd},
};

static const BattleBgScriptWord data_ov003_020d9a90[6] = {
    {.run = BtlBgScript_CmdLoadSlot},
    {.anims = data_ov003_020d9ad0},
    {.value = 0x00000002},
    {.value = 0x03000000},
    {.value = 0x000D0001},
    {.run = BtlBgScript_CmdEnd},
};

static const BattleBgScriptWord data_ov003_020d9aa8[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00040003}, {.value = 0x000D0002},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00040001}, {.value = 0x03000001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00040002}, {.value = 0x03000002},
    {.run = BtlBgScript_CmdEnd},
};

static const Ov003BgAnimFrame data_ov003_020d9ad0[7] = {
    {0, -2,  0},
    {3,  2,  0},
    {3,  1, 64},
    {0, -2,  0},
    {4,  2,  0},
    {4,  1, 64},
    {0, -1,  0},
};
