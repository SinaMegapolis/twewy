#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/CombatBgRes.h"

static const u16 data_ov003_020d8c96[];
static const u32 data_ov003_020d91d4[];
static const u32 data_ov003_020d9210[];
static const u32 data_ov003_020d924c[];
static const u32 data_ov003_020d9288[];
static const u32 data_ov003_020d92c4[];
static const u32 data_ov003_020d9300[];
static const u32 data_ov003_020d933c[];
static const u32 data_ov003_020d9378[];
static const u32 data_ov003_020d93b4[];
static const u32 data_ov003_020d93f0[];
static const u32 data_ov003_020d942c[];
static const u32 data_ov003_020d9468[];
static const u32 data_ov003_020d94a4[];
static const u32 data_ov003_020d94e0[];
static const u32 data_ov003_020d951c[];
static const u32 data_ov003_020d9558[];
static const u32 data_ov003_020d9594[];
static const u32 data_ov003_020d95d0[];
static const u32 data_ov003_020d960c[];
static const u32 data_ov003_020d9648[];
static const u32 data_ov003_020d9684[];
static const u32 data_ov003_020d96c0[];
static const u32 data_ov003_020d96fc[];
static const u32 data_ov003_020d9738[];
static const u32 data_ov003_020d9774[];
static const u32 data_ov003_020d97b0[];
static const u32 data_ov003_020d97ec[];
static const u32 data_ov003_020d9828[];
static const u32 data_ov003_020d9864[];
static const u32 data_ov003_020d98a0[];
static const u32 data_ov003_020d98dc[];
static const u32 data_ov003_020d9918[];
static const u32 data_ov003_020d9954[];
static const u32 data_ov003_020d9990[];
static const u32 data_ov003_020d99cc[];
static const u32 data_ov003_020d9a08[];
static const u32 data_ov003_020d9a44[];

// Background resource scripts and the file names they are read from (`BtlField_RG`/`BtlField_UG` resource tables).
const u32 data_ov003_020d889c[4] = {(u32)CombatBgRes_CmdLoadPalette, 0x00030001, 0x000B0004, (u32)CombatBgRes_CmdEnd};

static const u32 data_ov003_020d88ac[5] = {0x00010041, 0x00420040, 0x00400001, 0xFFFF0000, 0};

const u32 data_ov003_020d88c0[6] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9558, 1, 0x01900000, 0x000C0003,
                                    (u32)CombatBgRes_CmdEnd};

static const u32 data_ov003_020d88d8[6] = {0x0001002A, 0x002B0020, 0x00200001, 0x0001002C, 0x00000020, 0x0000FFFF};

static const u32 data_ov003_020d88f0[6] = {0x00010036, 0x00370040, 0x00080001, 0x00010038, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d8908[6] = {0x0001000C, 0x000D0020, 0x00200001, 0x0001000E, 0x00000080, 0x0000FFFF};

static const u32 data_ov003_020d8920[6] = {0x0001003C, 0x003D0040, 0x00400001, 0x0001003E, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d8938[6] = {0x00010046, 0x0047000C, 0x000C0001, 0x00010048, 0x0000000C, 0x0000FFFF};

static const u32 data_ov003_020d8950[6] = {0x0001004C, 0x004D000A, 0x000A0001, 0x0001004E, 0x0000000A, 0x0000FFFF};

static const u32 data_ov003_020d8968[6] = {0x00010012, 0x00130008, 0x00080001, 0x00010014, 8, 0x0000FFFF};

static const u32 data_ov003_020d8980[6] = {0x00010018, 0x00190060, 0x00600001, 0x0001001A, 0x00000060, 0x0000FFFF};

const u32 data_ov003_020d8998[6] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d8c96, 2, 0x01800000, 0x000C0003,
                                    (u32)CombatBgRes_CmdEnd};

static const u32 data_ov003_020d89b0[6] = {0x00010024, 0x00250010, 0x00100001, 0x00010026, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d89c8[6] = {0x0001001E, 0x001F0010, 0x00100001, 0x00010020, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d89e0[6] = {0x00010004, 0x00060008, 0x00080001, 0x00010008, 8, 0x0000FFFF};

static const u32 data_ov003_020d89f8[6] = {0x00010030, 0x00310040, 0x00400001, 0x00010032, 0x00000140, 0x0000FFFF};

const u32 data_ov003_020d8a10[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00060003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00060001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00060002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8a38[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00080003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00080001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00080002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8a60[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000E0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000E0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000E0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8a88[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000D0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000D0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000D0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8ab0[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000C0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000C0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000C0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8ad8[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000F0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000F0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000F0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8b00[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000A0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000A0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000A0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8b28[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00090003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00090001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00090002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8b50[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00070003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00070001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00070002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8b78[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00100003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00100001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00100002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8ba0[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00040003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00040001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00040002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8bc8[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x00050003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x00050001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x00050002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8bf0[10] = {
    (u32)CombatBgRes_CmdLoadPalette, 0x000B0003, 0x000B0004, (u32)CombatBgRes_CmdLoadChar, 0x000B0001, 0x01A00001,
    (u32)CombatBgRes_CmdLoadChar,    0x000B0002, 0x01A00002, (u32)CombatBgRes_CmdEnd};

static const u16 data_ov003_020d8c18[21] = {0, 0xFFFE, 0, 0x41, 2,    0, 0x41, 1, 0x40,   0, 0xFFFE,
                                            0, 0x42,   2, 0,    0x42, 1, 0x40, 0, 0xFFFF, 0};

static const u16 data_ov003_020d8c42[21] = {0, 0xFFFE, 0, 0x3F, 2,    0, 0x3F, 1, 0x40,   0, 0xFFFE,
                                            0, 0x40,   2, 0,    0x40, 1, 0x40, 0, 0xFFFF, 0};

static const u16 data_ov003_020d8c6c[21] = {0, 0xFFFE, 0, 0x3F, 2,    0, 0x3F, 1, 0x40,   0, 0xFFFE,
                                            0, 0x40,   2, 0,    0x40, 1, 0x40, 0, 0xFFFF, 0};

static const u16 data_ov003_020d8c96[21] = {0, 0xFFFE, 0, 3, 2, 0, 3,    1, 0x40,   0, 0xFFFE,
                                            0, 4,      2, 0, 4, 1, 0x40, 0, 0xFFFF, 0};

const u32 data_ov003_020d8cc0[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9918, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9954, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8cec[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d97b0, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d97ec, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8d18[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9990, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d99cc, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8d44[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d93b4, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d93f0, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8d70[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9a08, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9a44, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8d9c[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d942c, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d94a4, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8dc8[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d91d4, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9210, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8df4[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d92c4, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9300, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8e20[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d924c, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9288, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8e4c[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d933c, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9378, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8e78[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9828, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d9864, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8ea4[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d98a0, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d98dc, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8ed0[11] = {(u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d8c42, 1, 0x01800000, 0x000A0002,
                                     (u32)CombatBgRes_CmdLoadSlot, (u32)data_ov003_020d8c18, 2, 0x01A00001, 0x000C0003,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8efc[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d94e0,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8908,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8f34[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d951c,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8968,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8f6c[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9594,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8980,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8fa4[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d95d0,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d89c8,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d8fdc[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d960c,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d89b0,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d9014[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9648,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d88d8,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d904c[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9684,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d89f8,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d9084[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d96c0,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d88f0,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d90bc[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d96fc,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8920,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d90f4[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8c6c,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d88ac,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d912c[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9468,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d89e0,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d9164[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9738,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8938,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

const u32 data_ov003_020d919c[14] = {(u32)CombatBgRes_CmdLoadPalette,
                                     0x004F0001,
                                     0x000E0001,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d9774,
                                     1,
                                     0x01800000,
                                     0x000B0003,
                                     (u32)CombatBgRes_CmdLoadSlot,
                                     (u32)data_ov003_020d8950,
                                     2,
                                     0x01800001,
                                     0,
                                     (u32)CombatBgRes_CmdEnd};

static const u32 data_ov003_020d91d4[15] = {0xFFFE0000, 0x00270000, 2,          0x00010027, 0x00000020,
                                            0x0000FFFE, 0x00020028, 0x00280000, 0x00200001, 0xFFFE0000,
                                            0x00290000, 2,          0x00010029, 0x00000020, 0x0000FFFF};

static const u32 data_ov003_020d9210[15] = {0xFFFE0000, 0x002A0000, 2,          0x0001002A, 0x00000020,
                                            0x0000FFFE, 0x0002002B, 0x002B0000, 0x00200001, 0xFFFE0000,
                                            0x002C0000, 2,          0x0001002C, 0x00000020, 0x0000FFFF};

static const u32 data_ov003_020d924c[15] = {0xFFFE0000, 0x002D0000, 2,          0x0001002D, 0x00000040,
                                            0x0000FFFE, 0x0002002E, 0x002E0000, 0x00400001, 0xFFFE0000,
                                            0x002F0000, 2,          0x0001002F, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d9288[15] = {0xFFFE0000, 0x00300000, 2,          0x00010030, 0x00000040,
                                            0x0000FFFE, 0x00020031, 0x00310000, 0x00400001, 0xFFFE0000,
                                            0x00320000, 2,          0x00010032, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d92c4[15] = {0xFFFE0000, 0x00330000, 2,          0x00010033, 0x00000040,
                                            0x0000FFFE, 0x00020034, 0x00340000, 0x00080001, 0xFFFE0000,
                                            0x00350000, 2,          0x00010035, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d9300[15] = {0xFFFE0000, 0x00360000, 2,          0x00010036, 0x00000040,
                                            0x0000FFFE, 0x00020037, 0x00370000, 0x00080001, 0xFFFE0000,
                                            0x00380000, 2,          0x00010038, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d933c[15] = {0xFFFE0000, 0x00390000, 2,          0x00010039, 0x00000040,
                                            0x0000FFFE, 0x0002003A, 0x003A0000, 0x00400001, 0xFFFE0000,
                                            0x003B0000, 2,          0x0001003B, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d9378[15] = {0xFFFE0000, 0x003C0000, 2,          0x0001003C, 0x00000040,
                                            0x0000FFFE, 0x0002003D, 0x003D0000, 0x00400001, 0xFFFE0000,
                                            0x003E0000, 2,          0x0001003E, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d93b4[15] = {0xFFFE0000, 0x00430000, 2,          0x00010043, 0x0000000C,
                                            0x0000FFFE, 0x00020044, 0x00440000, 0x000C0001, 0xFFFE0000,
                                            0x00450000, 2,          0x00010045, 0x0000000C, 0x0000FFFF};

static const u32 data_ov003_020d93f0[15] = {0xFFFE0000, 0x00460000, 2,          0x00010046, 0x0000000C,
                                            0x0000FFFE, 0x00020047, 0x00470000, 0x000C0001, 0xFFFE0000,
                                            0x00480000, 2,          0x00010048, 0x0000000C, 0x0000FFFF};

static const u32 data_ov003_020d942c[15] = {0xFFFE0000, 0x00490000, 2,          0x00010049, 0x0000000A,
                                            0x0000FFFE, 0x0002004A, 0x004A0000, 0x000A0001, 0xFFFE0000,
                                            0x004B0000, 2,          0x0001004B, 0x0000000A, 0x0000FFFF};

static const u32 data_ov003_020d9468[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 8,
                                            0x0000FFFE, 0x00020005, 0x00050000, 0x00080001, 0xFFFE0000,
                                            0x00070000, 2,          0x00010007, 8,          0x0000FFFF};

static const u32 data_ov003_020d94a4[15] = {0xFFFE0000, 0x004C0000, 2,          0x0001004C, 0x0000000A,
                                            0x0000FFFE, 0x0002004D, 0x004D0000, 0x000A0001, 0xFFFE0000,
                                            0x004E0000, 2,          0x0001004E, 0x0000000A, 0x0000FFFF};

static const u32 data_ov003_020d94e0[15] = {0xFFFE0000, 0x00090000, 2,          0x00010009, 0x00000020,
                                            0x0000FFFE, 0x0002000A, 0x000A0000, 0x00200001, 0xFFFE0000,
                                            0x000B0000, 2,          0x0001000B, 0x00000080, 0x0000FFFF};

static const u32 data_ov003_020d951c[15] = {0xFFFE0000, 0x000F0000, 2,          0x0001000F, 8,
                                            0x0000FFFE, 0x00020010, 0x00100000, 0x00080001, 0xFFFE0000,
                                            0x00110000, 2,          0x00010011, 8,          0x0000FFFF};

static const u32 data_ov003_020d9558[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 0x0000000A,
                                            0x0000FFFE, 0x00020004, 0x00040000, 0x000A0001, 0xFFFE0000,
                                            0x00050000, 2,          0x00010005, 0x0000000A, 0x0000FFFF};

static const u32 data_ov003_020d9594[15] = {0xFFFE0000, 0x00150000, 2,          0x00010015, 0x00000060,
                                            0x0000FFFE, 0x00020016, 0x00160000, 0x00600001, 0xFFFE0000,
                                            0x00170000, 2,          0x00010017, 0x00000060, 0x0000FFFF};

static const u32 data_ov003_020d95d0[15] = {0xFFFE0000, 0x001B0000, 2,          0x0001001B, 0x00000010,
                                            0x0000FFFE, 0x0002001C, 0x001C0000, 0x00100001, 0xFFFE0000,
                                            0x001D0000, 2,          0x0001001D, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d960c[15] = {0xFFFE0000, 0x00210000, 2,          0x00010021, 0x00000010,
                                            0x0000FFFE, 0x00020022, 0x00220000, 0x00100001, 0xFFFE0000,
                                            0x00230000, 2,          0x00010023, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d9648[15] = {0xFFFE0000, 0x00270000, 2,          0x00010027, 0x00000020,
                                            0x0000FFFE, 0x00020028, 0x00280000, 0x00200001, 0xFFFE0000,
                                            0x00290000, 2,          0x00010029, 0x00000020, 0x0000FFFF};

static const u32 data_ov003_020d9684[15] = {0xFFFE0000, 0x002D0000, 2,          0x0001002D, 0x00000040,
                                            0x0000FFFE, 0x0002002E, 0x002E0000, 0x00400001, 0xFFFE0000,
                                            0x002F0000, 2,          0x0001002F, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d96c0[15] = {0xFFFE0000, 0x00330000, 2,          0x00010033, 0x00000040,
                                            0x0000FFFE, 0x00020034, 0x00340000, 0x00080001, 0xFFFE0000,
                                            0x00350000, 2,          0x00010035, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d96fc[15] = {0xFFFE0000, 0x00390000, 2,          0x00010039, 0x00000040,
                                            0x0000FFFE, 0x0002003A, 0x003A0000, 0x00400001, 0xFFFE0000,
                                            0x003B0000, 2,          0x0001003B, 0x00000140, 0x0000FFFF};

static const u32 data_ov003_020d9738[15] = {0xFFFE0000, 0x00430000, 2,          0x00010043, 0x0000000C,
                                            0x0000FFFE, 0x00020044, 0x00440000, 0x000C0001, 0xFFFE0000,
                                            0x00450000, 2,          0x00010045, 0x0000000C, 0x0000FFFF};

static const u32 data_ov003_020d9774[15] = {0xFFFE0000, 0x00490000, 2,          0x00010049, 0x0000000A,
                                            0x0000FFFE, 0x0002004A, 0x004A0000, 0x000A0001, 0xFFFE0000,
                                            0x004B0000, 2,          0x0001004B, 0x0000000A, 0x0000FFFF};

static const u32 data_ov003_020d97b0[15] = {0xFFFE0000, 0x00030000, 2,          0x00010003, 8,
                                            0x0000FFFE, 0x00020004, 0x00040000, 0x00080001, 0xFFFE0000,
                                            0x00050000, 2,          0x00010005, 8,          0x0000FFFF};

static const u32 data_ov003_020d97ec[15] = {0xFFFE0000, 0x00060000, 2,          0x00010006, 8,
                                            0x0000FFFE, 0x00020007, 0x00070000, 0x00080001, 0xFFFE0000,
                                            0x00080000, 2,          0x00010008, 8,          0x0000FFFF};

static const u32 data_ov003_020d9828[15] = {0xFFFE0000, 0x00090000, 2,          0x00010009, 0x00000020,
                                            0x0000FFFE, 0x0002000A, 0x000A0000, 0x00200001, 0xFFFE0000,
                                            0x000B0000, 2,          0x0001000B, 0x00000080, 0x0000FFFF};

static const u32 data_ov003_020d9864[15] = {0xFFFE0000, 0x000C0000, 2,          0x0001000C, 0x00000020,
                                            0x0000FFFE, 0x0002000D, 0x000D0000, 0x00200001, 0xFFFE0000,
                                            0x000E0000, 2,          0x0001000E, 0x00000080, 0x0000FFFF};

static const u32 data_ov003_020d98a0[15] = {0xFFFE0000, 0x000F0000, 2,          0x0001000F, 8,
                                            0x0000FFFE, 0x00020010, 0x00100000, 0x00080001, 0xFFFE0000,
                                            0x00110000, 2,          0x00010011, 8,          0x0000FFFF};

static const u32 data_ov003_020d98dc[15] = {0xFFFE0000, 0x00120000, 2,          0x00010012, 8,
                                            0x0000FFFE, 0x00020013, 0x00130000, 0x00080001, 0xFFFE0000,
                                            0x00140000, 2,          0x00010014, 8,          0x0000FFFF};

static const u32 data_ov003_020d9918[15] = {0xFFFE0000, 0x00150000, 2,          0x00010015, 0x00000060,
                                            0x0000FFFE, 0x00020016, 0x00160000, 0x00600001, 0xFFFE0000,
                                            0x00170000, 2,          0x00010017, 0x00000060, 0x0000FFFF};

static const u32 data_ov003_020d9954[15] = {0xFFFE0000, 0x00180000, 2,          0x00010018, 0x00000060,
                                            0x0000FFFE, 0x00020019, 0x00190000, 0x00600001, 0xFFFE0000,
                                            0x001A0000, 2,          0x0001001A, 0x00000060, 0x0000FFFF};

static const u32 data_ov003_020d9990[15] = {0xFFFE0000, 0x001B0000, 2,          0x0001001B, 0x00000010,
                                            0x0000FFFE, 0x0002001C, 0x001C0000, 0x00100001, 0xFFFE0000,
                                            0x001D0000, 2,          0x0001001D, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d99cc[15] = {0xFFFE0000, 0x001E0000, 2,          0x0001001E, 0x00000010,
                                            0x0000FFFE, 0x0002001F, 0x001F0000, 0x00100001, 0xFFFE0000,
                                            0x00200000, 2,          0x00010020, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d9a08[15] = {0xFFFE0000, 0x00210000, 2,          0x00010021, 0x00000010,
                                            0x0000FFFE, 0x00020022, 0x00220000, 0x00100001, 0xFFFE0000,
                                            0x00230000, 2,          0x00010023, 0x00000010, 0x0000FFFF};

static const u32 data_ov003_020d9a44[15] = {0xFFFE0000, 0x00240000, 2,          0x00010024, 0x00000010,
                                            0x0000FFFE, 0x00020025, 0x00250000, 0x00100001, 0xFFFE0000,
                                            0x00260000, 2,          0x00010026, 0x00000010, 0x0000FFFF};

const u32* data_ov003_020e3e64 = data_ov003_020d88c0;

const u32* data_ov003_020e3e68 = data_ov003_020d8998;

const u32* data_ov003_020e3e6c[13] = {
    data_ov003_020d8ed0, data_ov003_020d8d18, data_ov003_020d8d44, data_ov003_020d8ea4, data_ov003_020d8d70,
    data_ov003_020d8d9c, data_ov003_020d8df4, data_ov003_020d8dc8, data_ov003_020d8e78, data_ov003_020d8e4c,
    data_ov003_020d8e20, data_ov003_020d8cc0, data_ov003_020d8cec,
};

const u32* data_ov003_020e3ea0[13] = {
    data_ov003_020d90f4, data_ov003_020d8fa4, data_ov003_020d9164, data_ov003_020d8f34, data_ov003_020d8fdc,
    data_ov003_020d919c, data_ov003_020d9084, data_ov003_020d9014, data_ov003_020d8efc, data_ov003_020d90bc,
    data_ov003_020d904c, data_ov003_020d8f6c, data_ov003_020d912c,
};

const u32* data_ov003_020e3ed4[14] = {
    data_ov003_020d8a60, data_ov003_020d8a38, data_ov003_020d8ad8, data_ov003_020d8a10, data_ov003_020d8b28,
    data_ov003_020d8b78, data_ov003_020d8ab0, data_ov003_020d8b00, data_ov003_020d8bc8, data_ov003_020d8a88,
    data_ov003_020d8bf0, data_ov003_020d8b50, data_ov003_020d8ba0, data_ov003_020d889c,
};
