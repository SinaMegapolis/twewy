#include "Combat/Core/BtlFieldData.h"

static const u8              data_ov003_020d9ad0[0x2C];
static const Ov003ScriptWord data_ov003_020d9a80[4];
static const Ov003ScriptWord data_ov003_020d9a90[6];
static const Ov003ScriptWord data_ov003_020d9aa8[10];

const void* data_ov003_020e3f0c[1] = {(void*)data_ov003_020d9a90};

const void* data_ov003_020e3f10[2] = {(void*)data_ov003_020d9a80, (void*)data_ov003_020d9aa8};

static const Ov003ScriptWord data_ov003_020d9a80[4] = {
    {.value = (u32)CombatBgRes_CmdLoadPalette},
    {.value = 0x00030001},
    {.value = 0x000D0002},
    {.value = (u32)CombatBgRes_CmdEnd},
};

static const Ov003ScriptWord data_ov003_020d9a90[6] = {
    {.value = (u32)CombatBgRes_CmdLoadSlot},
    {.value = (u32)data_ov003_020d9ad0},
    {.value = 0x00000002},
    {.value = 0x03000000},
    {.value = 0x000D0001},
    {.value = (u32)CombatBgRes_CmdEnd},
};

static const Ov003ScriptWord data_ov003_020d9aa8[10] = {
    {.value = (u32)CombatBgRes_CmdLoadPalette}, {.value = 0x00040003}, {.value = 0x000D0002},
    {.value = (u32)CombatBgRes_CmdLoadChar},    {.value = 0x00040001}, {.value = 0x03000001},
    {.value = (u32)CombatBgRes_CmdLoadChar},    {.value = 0x00040002}, {.value = 0x03000002},
    {.value = (u32)CombatBgRes_CmdEnd},
};

static const u8 data_ov003_020d9ad0[0x2C] = {
    0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01,
    0x00, 0x40, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
};
