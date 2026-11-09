#include "Battle/Core/BtlBgScript.h"
#include "Combat/Core/BtlFieldData.h"

static const Ov003BgAnimFrame data_ov003_020d8c96[];
static const Ov003BgAnimFrame data_ov003_020d91d4[];
static const Ov003BgAnimFrame data_ov003_020d9210[];
static const Ov003BgAnimFrame data_ov003_020d924c[];
static const Ov003BgAnimFrame data_ov003_020d9288[];
static const Ov003BgAnimFrame data_ov003_020d92c4[];
static const Ov003BgAnimFrame data_ov003_020d9300[];
static const Ov003BgAnimFrame data_ov003_020d933c[];
static const Ov003BgAnimFrame data_ov003_020d9378[];
static const Ov003BgAnimFrame data_ov003_020d93b4[];
static const Ov003BgAnimFrame data_ov003_020d93f0[];
static const Ov003BgAnimFrame data_ov003_020d942c[];
static const Ov003BgAnimFrame data_ov003_020d9468[];
static const Ov003BgAnimFrame data_ov003_020d94a4[];
static const Ov003BgAnimFrame data_ov003_020d94e0[];
static const Ov003BgAnimFrame data_ov003_020d951c[];
static const Ov003BgAnimFrame data_ov003_020d9558[];
static const Ov003BgAnimFrame data_ov003_020d9594[];
static const Ov003BgAnimFrame data_ov003_020d95d0[];
static const Ov003BgAnimFrame data_ov003_020d960c[];
static const Ov003BgAnimFrame data_ov003_020d9648[];
static const Ov003BgAnimFrame data_ov003_020d9684[];
static const Ov003BgAnimFrame data_ov003_020d96c0[];
static const Ov003BgAnimFrame data_ov003_020d96fc[];
static const Ov003BgAnimFrame data_ov003_020d9738[];
static const Ov003BgAnimFrame data_ov003_020d9774[];
static const Ov003BgAnimFrame data_ov003_020d97b0[];
static const Ov003BgAnimFrame data_ov003_020d97ec[];
static const Ov003BgAnimFrame data_ov003_020d9828[];
static const Ov003BgAnimFrame data_ov003_020d9864[];
static const Ov003BgAnimFrame data_ov003_020d98a0[];
static const Ov003BgAnimFrame data_ov003_020d98dc[];
static const Ov003BgAnimFrame data_ov003_020d9918[];
static const Ov003BgAnimFrame data_ov003_020d9954[];
static const Ov003BgAnimFrame data_ov003_020d9990[];
static const Ov003BgAnimFrame data_ov003_020d99cc[];
static const Ov003BgAnimFrame data_ov003_020d9a08[];
static const Ov003BgAnimFrame data_ov003_020d9a44[];

// Background scripts and the file names they are read from (the `scripts` tables of `BtlField_RG`/`BtlField_UG`).
const BattleBgScriptWord data_ov003_020d889c[4] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00030001}, {.value = 0x000B0004}, {.run = BtlBgScript_CmdEnd}};

static const Ov003BgAnimFrame data_ov003_020d88ac[3] = {
    {65,  1, 64},
    {66,  1, 64},
    { 0, -1,  0}
};

const BattleBgScriptWord data_ov003_020d88c0[6] = {{.run = BtlBgScript_CmdLoadSlot},
                                                   {.anims = data_ov003_020d9558},
                                                   {.value = 1},
                                                   {.value = 0x01900000},
                                                   {.value = 0x000C0003},
                                                   {.run = BtlBgScript_CmdEnd}};

static const Ov003BgAnimFrame data_ov003_020d88d8[4] = {
    {42,  1, 32},
    {43,  1, 32},
    {44,  1, 32},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d88f0[4] = {
    {54,  1,  64},
    {55,  1,   8},
    {56,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d8908[4] = {
    {12,  1,  32},
    {13,  1,  32},
    {14,  1, 128},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d8920[4] = {
    {60,  1,  64},
    {61,  1,  64},
    {62,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d8938[4] = {
    {70,  1, 12},
    {71,  1, 12},
    {72,  1, 12},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d8950[4] = {
    {76,  1, 10},
    {77,  1, 10},
    {78,  1, 10},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d8968[4] = {
    {18,  1, 8},
    {19,  1, 8},
    {20,  1, 8},
    { 0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d8980[4] = {
    {24,  1, 96},
    {25,  1, 96},
    {26,  1, 96},
    { 0, -1,  0}
};

const BattleBgScriptWord data_ov003_020d8998[6] = {{.run = BtlBgScript_CmdLoadSlot},
                                                   {.anims = data_ov003_020d8c96},
                                                   {.value = 2},
                                                   {.value = 0x01800000},
                                                   {.value = 0x000C0003},
                                                   {.run = BtlBgScript_CmdEnd}};

static const Ov003BgAnimFrame data_ov003_020d89b0[4] = {
    {36,  1, 16},
    {37,  1, 16},
    {38,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d89c8[4] = {
    {30,  1, 16},
    {31,  1, 16},
    {32,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d89e0[4] = {
    {4,  1, 8},
    {6,  1, 8},
    {8,  1, 8},
    {0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d89f8[4] = {
    {48,  1,  64},
    {49,  1,  64},
    {50,  1, 320},
    { 0, -1,   0}
};

const BattleBgScriptWord data_ov003_020d8a10[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00060003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00060001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00060002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8a38[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00080003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00080001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00080002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8a60[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000E0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000E0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000E0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8a88[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000D0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000D0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000D0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8ab0[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000C0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000C0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000C0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8ad8[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000F0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000F0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000F0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8b00[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000A0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000A0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000A0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8b28[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00090003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00090001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00090002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8b50[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00070003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00070001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00070002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8b78[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00100003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00100001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00100002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8ba0[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00040003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00040001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00040002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8bc8[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x00050003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00050001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x00050002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8bf0[10] = {
    {.run = BtlBgScript_CmdLoadPalette}, {.value = 0x000B0003}, {.value = 0x000B0004},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000B0001}, {.value = 0x01A00001},
    {.run = BtlBgScript_CmdLoadChar},    {.value = 0x000B0002}, {.value = 0x01A00002},
    {.run = BtlBgScript_CmdEnd}};

static const Ov003BgAnimFrame data_ov003_020d8c18[7] = {
    { 0, -2,  0},
    {65,  2,  0},
    {65,  1, 64},
    { 0, -2,  0},
    {66,  2,  0},
    {66,  1, 64},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d8c42[7] = {
    { 0, -2,  0},
    {63,  2,  0},
    {63,  1, 64},
    { 0, -2,  0},
    {64,  2,  0},
    {64,  1, 64},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d8c6c[7] = {
    { 0, -2,  0},
    {63,  2,  0},
    {63,  1, 64},
    { 0, -2,  0},
    {64,  2,  0},
    {64,  1, 64},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d8c96[7] = {
    {0, -2,  0},
    {3,  2,  0},
    {3,  1, 64},
    {0, -2,  0},
    {4,  2,  0},
    {4,  1, 64},
    {0, -1,  0}
};

const BattleBgScriptWord data_ov003_020d8cc0[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9918},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9954},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8cec[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d97b0},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d97ec},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8d18[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9990},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d99cc},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8d44[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d93b4},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d93f0},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8d70[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9a08},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9a44},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8d9c[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d942c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d94a4},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8dc8[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d91d4},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9210},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8df4[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d92c4},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9300},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8e20[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d924c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9288},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8e4c[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d933c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9378},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8e78[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9828},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9864},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8ea4[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d98a0},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d98dc},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8ed0[11] = {{.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8c42},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000A0002},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8c18},
                                                    {.value = 2},
                                                    {.value = 0x01A00001},
                                                    {.value = 0x000C0003},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8efc[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d94e0},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8908},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8f34[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d951c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8968},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8f6c[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9594},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8980},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8fa4[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d95d0},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d89c8},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d8fdc[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d960c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d89b0},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d9014[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9648},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d88d8},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d904c[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9684},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d89f8},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d9084[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d96c0},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d88f0},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d90bc[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d96fc},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8920},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d90f4[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8c6c},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d88ac},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d912c[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9468},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d89e0},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d9164[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9738},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8938},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

const BattleBgScriptWord data_ov003_020d919c[14] = {{.run = BtlBgScript_CmdLoadPalette},
                                                    {.value = 0x004F0001},
                                                    {.value = 0x000E0001},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d9774},
                                                    {.value = 1},
                                                    {.value = 0x01800000},
                                                    {.value = 0x000B0003},
                                                    {.run = BtlBgScript_CmdLoadSlot},
                                                    {.anims = data_ov003_020d8950},
                                                    {.value = 2},
                                                    {.value = 0x01800001},
                                                    {.value = 0},
                                                    {.run = BtlBgScript_CmdEnd}};

static const Ov003BgAnimFrame data_ov003_020d91d4[10] = {
    { 0, -2,  0},
    {39,  2,  0},
    {39,  1, 32},
    { 0, -2,  0},
    {40,  2,  0},
    {40,  1, 32},
    { 0, -2,  0},
    {41,  2,  0},
    {41,  1, 32},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9210[10] = {
    { 0, -2,  0},
    {42,  2,  0},
    {42,  1, 32},
    { 0, -2,  0},
    {43,  2,  0},
    {43,  1, 32},
    { 0, -2,  0},
    {44,  2,  0},
    {44,  1, 32},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d924c[10] = {
    { 0, -2,   0},
    {45,  2,   0},
    {45,  1,  64},
    { 0, -2,   0},
    {46,  2,   0},
    {46,  1,  64},
    { 0, -2,   0},
    {47,  2,   0},
    {47,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d9288[10] = {
    { 0, -2,   0},
    {48,  2,   0},
    {48,  1,  64},
    { 0, -2,   0},
    {49,  2,   0},
    {49,  1,  64},
    { 0, -2,   0},
    {50,  2,   0},
    {50,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d92c4[10] = {
    { 0, -2,   0},
    {51,  2,   0},
    {51,  1,  64},
    { 0, -2,   0},
    {52,  2,   0},
    {52,  1,   8},
    { 0, -2,   0},
    {53,  2,   0},
    {53,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d9300[10] = {
    { 0, -2,   0},
    {54,  2,   0},
    {54,  1,  64},
    { 0, -2,   0},
    {55,  2,   0},
    {55,  1,   8},
    { 0, -2,   0},
    {56,  2,   0},
    {56,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d933c[10] = {
    { 0, -2,   0},
    {57,  2,   0},
    {57,  1,  64},
    { 0, -2,   0},
    {58,  2,   0},
    {58,  1,  64},
    { 0, -2,   0},
    {59,  2,   0},
    {59,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d9378[10] = {
    { 0, -2,   0},
    {60,  2,   0},
    {60,  1,  64},
    { 0, -2,   0},
    {61,  2,   0},
    {61,  1,  64},
    { 0, -2,   0},
    {62,  2,   0},
    {62,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d93b4[10] = {
    { 0, -2,  0},
    {67,  2,  0},
    {67,  1, 12},
    { 0, -2,  0},
    {68,  2,  0},
    {68,  1, 12},
    { 0, -2,  0},
    {69,  2,  0},
    {69,  1, 12},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d93f0[10] = {
    { 0, -2,  0},
    {70,  2,  0},
    {70,  1, 12},
    { 0, -2,  0},
    {71,  2,  0},
    {71,  1, 12},
    { 0, -2,  0},
    {72,  2,  0},
    {72,  1, 12},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d942c[10] = {
    { 0, -2,  0},
    {73,  2,  0},
    {73,  1, 10},
    { 0, -2,  0},
    {74,  2,  0},
    {74,  1, 10},
    { 0, -2,  0},
    {75,  2,  0},
    {75,  1, 10},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9468[10] = {
    {0, -2, 0},
    {3,  2, 0},
    {3,  1, 8},
    {0, -2, 0},
    {5,  2, 0},
    {5,  1, 8},
    {0, -2, 0},
    {7,  2, 0},
    {7,  1, 8},
    {0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d94a4[10] = {
    { 0, -2,  0},
    {76,  2,  0},
    {76,  1, 10},
    { 0, -2,  0},
    {77,  2,  0},
    {77,  1, 10},
    { 0, -2,  0},
    {78,  2,  0},
    {78,  1, 10},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d94e0[10] = {
    { 0, -2,   0},
    { 9,  2,   0},
    { 9,  1,  32},
    { 0, -2,   0},
    {10,  2,   0},
    {10,  1,  32},
    { 0, -2,   0},
    {11,  2,   0},
    {11,  1, 128},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d951c[10] = {
    { 0, -2, 0},
    {15,  2, 0},
    {15,  1, 8},
    { 0, -2, 0},
    {16,  2, 0},
    {16,  1, 8},
    { 0, -2, 0},
    {17,  2, 0},
    {17,  1, 8},
    { 0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d9558[10] = {
    {0, -2,  0},
    {3,  2,  0},
    {3,  1, 10},
    {0, -2,  0},
    {4,  2,  0},
    {4,  1, 10},
    {0, -2,  0},
    {5,  2,  0},
    {5,  1, 10},
    {0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9594[10] = {
    { 0, -2,  0},
    {21,  2,  0},
    {21,  1, 96},
    { 0, -2,  0},
    {22,  2,  0},
    {22,  1, 96},
    { 0, -2,  0},
    {23,  2,  0},
    {23,  1, 96},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d95d0[10] = {
    { 0, -2,  0},
    {27,  2,  0},
    {27,  1, 16},
    { 0, -2,  0},
    {28,  2,  0},
    {28,  1, 16},
    { 0, -2,  0},
    {29,  2,  0},
    {29,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d960c[10] = {
    { 0, -2,  0},
    {33,  2,  0},
    {33,  1, 16},
    { 0, -2,  0},
    {34,  2,  0},
    {34,  1, 16},
    { 0, -2,  0},
    {35,  2,  0},
    {35,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9648[10] = {
    { 0, -2,  0},
    {39,  2,  0},
    {39,  1, 32},
    { 0, -2,  0},
    {40,  2,  0},
    {40,  1, 32},
    { 0, -2,  0},
    {41,  2,  0},
    {41,  1, 32},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9684[10] = {
    { 0, -2,   0},
    {45,  2,   0},
    {45,  1,  64},
    { 0, -2,   0},
    {46,  2,   0},
    {46,  1,  64},
    { 0, -2,   0},
    {47,  2,   0},
    {47,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d96c0[10] = {
    { 0, -2,   0},
    {51,  2,   0},
    {51,  1,  64},
    { 0, -2,   0},
    {52,  2,   0},
    {52,  1,   8},
    { 0, -2,   0},
    {53,  2,   0},
    {53,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d96fc[10] = {
    { 0, -2,   0},
    {57,  2,   0},
    {57,  1,  64},
    { 0, -2,   0},
    {58,  2,   0},
    {58,  1,  64},
    { 0, -2,   0},
    {59,  2,   0},
    {59,  1, 320},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d9738[10] = {
    { 0, -2,  0},
    {67,  2,  0},
    {67,  1, 12},
    { 0, -2,  0},
    {68,  2,  0},
    {68,  1, 12},
    { 0, -2,  0},
    {69,  2,  0},
    {69,  1, 12},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9774[10] = {
    { 0, -2,  0},
    {73,  2,  0},
    {73,  1, 10},
    { 0, -2,  0},
    {74,  2,  0},
    {74,  1, 10},
    { 0, -2,  0},
    {75,  2,  0},
    {75,  1, 10},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d97b0[10] = {
    {0, -2, 0},
    {3,  2, 0},
    {3,  1, 8},
    {0, -2, 0},
    {4,  2, 0},
    {4,  1, 8},
    {0, -2, 0},
    {5,  2, 0},
    {5,  1, 8},
    {0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d97ec[10] = {
    {0, -2, 0},
    {6,  2, 0},
    {6,  1, 8},
    {0, -2, 0},
    {7,  2, 0},
    {7,  1, 8},
    {0, -2, 0},
    {8,  2, 0},
    {8,  1, 8},
    {0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d9828[10] = {
    { 0, -2,   0},
    { 9,  2,   0},
    { 9,  1,  32},
    { 0, -2,   0},
    {10,  2,   0},
    {10,  1,  32},
    { 0, -2,   0},
    {11,  2,   0},
    {11,  1, 128},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d9864[10] = {
    { 0, -2,   0},
    {12,  2,   0},
    {12,  1,  32},
    { 0, -2,   0},
    {13,  2,   0},
    {13,  1,  32},
    { 0, -2,   0},
    {14,  2,   0},
    {14,  1, 128},
    { 0, -1,   0}
};

static const Ov003BgAnimFrame data_ov003_020d98a0[10] = {
    { 0, -2, 0},
    {15,  2, 0},
    {15,  1, 8},
    { 0, -2, 0},
    {16,  2, 0},
    {16,  1, 8},
    { 0, -2, 0},
    {17,  2, 0},
    {17,  1, 8},
    { 0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d98dc[10] = {
    { 0, -2, 0},
    {18,  2, 0},
    {18,  1, 8},
    { 0, -2, 0},
    {19,  2, 0},
    {19,  1, 8},
    { 0, -2, 0},
    {20,  2, 0},
    {20,  1, 8},
    { 0, -1, 0}
};

static const Ov003BgAnimFrame data_ov003_020d9918[10] = {
    { 0, -2,  0},
    {21,  2,  0},
    {21,  1, 96},
    { 0, -2,  0},
    {22,  2,  0},
    {22,  1, 96},
    { 0, -2,  0},
    {23,  2,  0},
    {23,  1, 96},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9954[10] = {
    { 0, -2,  0},
    {24,  2,  0},
    {24,  1, 96},
    { 0, -2,  0},
    {25,  2,  0},
    {25,  1, 96},
    { 0, -2,  0},
    {26,  2,  0},
    {26,  1, 96},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9990[10] = {
    { 0, -2,  0},
    {27,  2,  0},
    {27,  1, 16},
    { 0, -2,  0},
    {28,  2,  0},
    {28,  1, 16},
    { 0, -2,  0},
    {29,  2,  0},
    {29,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d99cc[10] = {
    { 0, -2,  0},
    {30,  2,  0},
    {30,  1, 16},
    { 0, -2,  0},
    {31,  2,  0},
    {31,  1, 16},
    { 0, -2,  0},
    {32,  2,  0},
    {32,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9a08[10] = {
    { 0, -2,  0},
    {33,  2,  0},
    {33,  1, 16},
    { 0, -2,  0},
    {34,  2,  0},
    {34,  1, 16},
    { 0, -2,  0},
    {35,  2,  0},
    {35,  1, 16},
    { 0, -1,  0}
};

static const Ov003BgAnimFrame data_ov003_020d9a44[10] = {
    { 0, -2,  0},
    {36,  2,  0},
    {36,  1, 16},
    { 0, -2,  0},
    {37,  2,  0},
    {37,  1, 16},
    { 0, -2,  0},
    {38,  2,  0},
    {38,  1, 16},
    { 0, -1,  0}
};

const BattleBgScriptWord* data_ov003_020e3e64 = data_ov003_020d88c0;

const BattleBgScriptWord* data_ov003_020e3e68 = data_ov003_020d8998;

const BattleBgScriptWord* data_ov003_020e3e6c[13] = {
    data_ov003_020d8ed0, data_ov003_020d8d18, data_ov003_020d8d44, data_ov003_020d8ea4, data_ov003_020d8d70,
    data_ov003_020d8d9c, data_ov003_020d8df4, data_ov003_020d8dc8, data_ov003_020d8e78, data_ov003_020d8e4c,
    data_ov003_020d8e20, data_ov003_020d8cc0, data_ov003_020d8cec,
};

const BattleBgScriptWord* data_ov003_020e3ea0[13] = {
    data_ov003_020d90f4, data_ov003_020d8fa4, data_ov003_020d9164, data_ov003_020d8f34, data_ov003_020d8fdc,
    data_ov003_020d919c, data_ov003_020d9084, data_ov003_020d9014, data_ov003_020d8efc, data_ov003_020d90bc,
    data_ov003_020d904c, data_ov003_020d8f6c, data_ov003_020d912c,
};

const BattleBgScriptWord* data_ov003_020e3ed4[14] = {
    data_ov003_020d8a60, data_ov003_020d8a38, data_ov003_020d8ad8, data_ov003_020d8a10, data_ov003_020d8b28,
    data_ov003_020d8b78, data_ov003_020d8ab0, data_ov003_020d8b00, data_ov003_020d8bc8, data_ov003_020d8a88,
    data_ov003_020d8bf0, data_ov003_020d8b50, data_ov003_020d8ba0, data_ov003_020d889c,
};
