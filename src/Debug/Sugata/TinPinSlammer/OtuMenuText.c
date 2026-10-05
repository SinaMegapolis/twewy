#include "Debug/Sugata/TinPinSlammer.h"

/**
 * @brief The scene's process-stage dispatch tables.
 *
 * These live in `.rodata`, immediately before this TU's `.data` range.
 * `MainOvlDisp_GetProcessStage` returns the index; each table maps it to the
 * routine for the scene at that stage.  Index 1 is the per-frame update and
 * index 2 the teardown, so each table pairs one scene variant's entry point
 * with its own update and teardown.
 */
const OtuStageHandler OtuScene_PlainHandlers[3] = {
    func_ov039_02082ae0,
    func_ov039_02082e98,
    func_ov039_020831cc,
};

const OtuStageHandler OtuScene_WirelessHandlers[3] = {
    func_ov039_02082c50,
    func_ov039_02082ff8,
    func_ov039_020831d8,
};

/**
 * @brief The three debug-sequence names, all the same string.
 *
 * The target holds three copies of the pointer rather than one string and three
 * pointers, and the scene entry points index it.  Reproduced literally: mwcc
 * emits one pool word per object, so collapsing it to a single
 * `static const char*` would change the emitted references.
 */
static const char kOtuSequenceName[] = "Seq_Otosu()";

const char* const OtuScene_SequenceNames[3] = {
    kOtuSequenceName,
    kOtuSequenceName,
    kOtuSequenceName,
};

/*
 * Otosu .data objects: the scene's stage descriptors, its debug-sequence name,
 * and the three result-screen menus' labels and scrollbar separators.
 *
 * This TU owns 0x0209a140-0x0209a2fc in one range because dsd allows a single
 * .data entry per TU, and the natural groupings (stage descriptors, then the
 * strings) are not contiguous.
 *
 * The Shift-JIS label bytes come from tools/ov039_strgen.py, which reads them
 * out of build/usa/asm/ov039_4.s; the build compiles this file with -enc SJIS.
 * mwcc sorts .data objects by size ascending and breaks ties with a heapsort
 * over creation order, so the declaration order here is load-bearing -- do not
 * reorder.
 */

/**
 * @brief The wireless stage's companion descriptor.
 *
 * Three routines the wireless stage sequences through -- one more than the
 * single-routine companion the first stage uses, so it is a bare pointer array.
 */
const OtuStageHandler OtuScene_WirelessStage_Companion[3] = {
    func_ov039_02087acc,
    func_ov039_02087b04,
    func_ov039_02087b74,
};

/**
 * @brief The stage the scene enters when the wireless link needs attention.
 */
OtuSceneStage OtuScene_WirelessStage = {
    func_ov039_02087ac4,
    func_ov039_02087ac8,
    OtuScene_WirelessStage_Companion,
    0,
};

/**
 * @brief The scene's first stage: the board/pin-logic stage.
 *
 * Runs after the shared scene setup and owns 0x3C bytes of state.
 */
OtuStageHandler OtuScene_FirstStage_Companion[1] = {
    func_ov039_02086808,
};

OtuSceneStage OtuScene_FirstStage = {
    func_ov039_02086728,
    func_ov039_020867d4,
    OtuScene_FirstStage_Companion,
    0x3C,
};

/**
 * @brief The stage the wireless board entry point enters instead.
 *
 * Same shape as the first stage but with a much larger state block (0x2D0) and
 * its own companion routine list at 0x0209a32c.
 */
OtuStageHandler OtuScene_WirelessBoard_Companion[1] = {
    func_ov039_020897dc,
};

OtuSceneStage OtuScene_WirelessBoard = {
    func_ov039_02089780,
    func_ov039_020897d0,
    OtuScene_WirelessBoard_Companion,
    0x2D0,
};

/**
 * @brief Companion routine lists for the two remaining stage descriptors.
 *
 * Both are six-entry pointer arrays; the stage descriptors at 0x0209a3bc and
 * 0x0209a3cc point at them.
 */
const OtuStageHandler OtuScene_MenuStage_Companion[6] = {
    func_ov039_02089f68, func_ov039_0208a098, func_ov039_0208a324,
    func_ov039_0208a354, func_ov039_0208a3f4, func_ov039_0208a454,
};

const OtuStageHandler OtuScene_ResultStage_Companion[6] = {
    func_ov039_02089950, func_ov039_02089a80, func_ov039_02089d3c,
    func_ov039_02089d6c, func_ov039_02089e0c, func_ov039_02089e78,
};

OtuSceneStage OtuScene_MenuStage = {
    func_ov039_020898a8,
    func_ov039_02089918,
    OtuScene_MenuStage_Companion,
    0x2D0,
};

OtuSceneStage OtuScene_ResultStage = {
    func_ov039_02089ec0,
    func_ov039_02089f30,
    OtuScene_ResultStage_Companion,
    0x2D0,
};

/* The tuning constants SINGLE MENU 1 and 2 edit; see TinPinSlammer.h. */
s32 data_ov039_0209a2fc = 0x2000;
s32 data_ov039_0209a300 = 0x1000;
s32 data_ov039_0209a304 = 0x133;
s32 data_ov039_0209a308 = 0x8000;
s32 data_ov039_0209a30c = 0x4000;
s32 data_ov039_0209a310 = 0x400;
s32 data_ov039_0209a314 = 0x3000;
s32 data_ov039_0209a318 = 0x133;
s32 data_ov039_0209a31c = -0x2000;
s32 data_ov039_0209a320 = -0x2000;
s32 data_ov039_0209a324 = 0x800;
s32 data_ov039_0209a328 = 0x9800;

OtuPinTune data_ov039_0209a3dc[10] = {
    {0x8000, 0x148, 0x99A, 0x14CD},
    {0x7AB8, 0x171, 0x948, 0x1614},
    {0x7548, 0x19A, 0x91F, 0x175C},
    {0x7000, 0x19A, 0x8CD, 0x187B},
    {0x6AB8, 0x1C3, 0x87B, 0x19C3},
    {0x6548, 0x1EC, 0x829, 0x1B0A},
    {0x6000, 0x214, 0x800, 0x1C52},
    {0x5AB8, 0x214, 0x7AE, 0x1D9A},
    {0x5548, 0x23D, 0x75C, 0x1EB8},
    {0x5000, 0x266, 0x733, 0x2000},
};

OtuPinTune data_ov039_0209a47c = {0x5000, 0x4CD, 0x4CD, 0x3000};
OtuPinTune data_ov039_0209a48c = {0x2800, 0x266, 0x4CD, 0x7000};

/**
 * @brief Overlay-global state cleared by every scene entry point.
 */
OtuSceneSlotState OtuScene_SlotState;

/*
 * Otosu menu labels and separators.
 *
 * Generated by tools/ov039_strgen.py from build/usa/asm/ov039_4.s -- the
 * Shift-JIS bodies come straight from the ROM, so do not hand-edit them;
 * the build compiles this file with -enc SJIS.
 *
 * These are the three Otosu result-screen menus (SINGLE MENU 1/2/3).
 * func_ov039_02086174 picks a menu by index and draws its rows;
 * func_ov039_02083bb0 draws the two-value scrollbar using the separator
 * glyphs below.
 *
 * Order matters: mwcc sorts .data objects by size ascending and breaks ties
 * with a heapsort over creation order, so this sequence is load-bearing.
 */

/* Otu_str_0209a14c: 12 bytes at data_ov039_0209a14c */
const char Otu_str_0209a14c[12] = {83, 101, 113, 95, 79, 116, 111, 115, 117, 40, 41, 0};

/* data_ov039_0209a16c: 4 bytes at data_ov039_0209a16c */
const char data_ov039_0209a16c[4] = {32, 0, 0, 0};

/* data_ov039_0209a170: 4 bytes at data_ov039_0209a170 */
const char data_ov039_0209a170[4] = {42, 0, 0, 0};

/* data_ov039_0209a174: 4 bytes at data_ov039_0209a174 */
const char data_ov039_0209a174[4] = {62, 0, 0, 0};

/* Otu_str_0209a178: 16 bytes at data_ov039_0209a178 */
const char Otu_str_0209a178[16] = {83, 73, 78, 71, 76, 69, 32, 77, 69, 78, 85, 32, 49, 0, 0, 0};

/* Otu_str_0209a188: 12 bytes at data_ov039_0209a188 */
const char Otu_str_0209a188[12] = {131, 88, 131, 94, 129, 91, 131, 103, 0, 0, 0, 0};

/* Otu_str_0209a194: 12 bytes at data_ov039_0209a194 */
const char Otu_str_0209a194[12] = {131, 137, 131, 69, 131, 147, 131, 104, 0, 0, 0, 0};

/* Otu_str_0209a1a0: 8 bytes at data_ov039_0209a1a0 */
const char Otu_str_0209a1a0[8] = {150, 128, 142, 67, 0, 0, 0, 0};

/* Otu_str_0209a1a8: 8 bytes at data_ov039_0209a1a8 */
const char Otu_str_0209a1a8[8] = {131, 111, 131, 98, 131, 87, 49, 0};

/* Otu_str_0209a1b0: 8 bytes at data_ov039_0209a1b0 */
const char Otu_str_0209a1b0[8] = {131, 111, 131, 98, 131, 87, 50, 0};

/* Otu_str_0209a1b8: 8 bytes at data_ov039_0209a1b8 */
const char Otu_str_0209a1b8[8] = {131, 111, 131, 98, 131, 87, 51, 0};

/* Otu_str_0209a1c0: 8 bytes at data_ov039_0209a1c0 */
const char Otu_str_0209a1c0[8] = {131, 111, 131, 98, 131, 87, 52, 0};

/* Otu_str_0209a1c8: 8 bytes at data_ov039_0209a1c8 */
const char Otu_str_0209a1c8[8] = {143, 100, 130, 179, 49, 0, 0, 0};

/* Otu_str_0209a1d0: 8 bytes at data_ov039_0209a1d0 */
const char Otu_str_0209a1d0[8] = {143, 100, 130, 179, 50, 0, 0, 0};

/* Otu_str_0209a1d8: 8 bytes at data_ov039_0209a1d8 */
const char Otu_str_0209a1d8[8] = {143, 100, 130, 179, 51, 0, 0, 0};

/* Otu_str_0209a1e0: 8 bytes at data_ov039_0209a1e0 */
const char Otu_str_0209a1e0[8] = {143, 100, 130, 179, 52, 0, 0, 0};

/* Otu_str_0209a1e8: 8 bytes at data_ov039_0209a1e8 */
const char Otu_str_0209a1e8[8] = {143, 100, 130, 179, 53, 0, 0, 0};

/* Otu_str_0209a1f0: 8 bytes at data_ov039_0209a1f0 */
const char Otu_str_0209a1f0[8] = {143, 100, 130, 179, 54, 0, 0, 0};

/* Otu_str_0209a1f8: 8 bytes at data_ov039_0209a1f8 */
const char Otu_str_0209a1f8[8] = {143, 100, 130, 179, 55, 0, 0, 0};

/* Otu_str_0209a200: 4 bytes at data_ov039_0209a200 */
const char Otu_str_0209a200[4] = {143, 100, 130, 179};

/* Otu_str_0209a204: 4 bytes at data_ov039_0209a204 */
const char Otu_str_0209a204[4] = {56, 0, 0, 0};

/* Otu_str_0209a208: 8 bytes at data_ov039_0209a208 */
const char Otu_str_0209a208[8] = {143, 100, 130, 179, 57, 0, 0, 0};

/* Otu_str_0209a210: 8 bytes at data_ov039_0209a210 */
const char Otu_str_0209a210[8] = {143, 100, 130, 179, 49, 48, 0, 0};

/* Otu_str_0209a218: 12 bytes at data_ov039_0209a218 */
const char Otu_str_0209a218[12] = {143, 100, 151, 205, 137, 193, 145, 172, 147, 120, 0, 0};

/* Otu_str_0209a224: 8 bytes at data_ov039_0209a224 */
const char Otu_str_0209a224[8] = {131, 94, 129, 91, 131, 147, 0, 0};

/* Otu_str_0209a22c: 8 bytes at data_ov039_0209a22c */
const char Otu_str_0209a22c[8] = {131, 112, 131, 108, 131, 139, 0, 0};

/* Otu_str_0209a234: 8 bytes at data_ov039_0209a234 */
const char Otu_str_0209a234[8] = {139, 67, 144, 226, 0, 0, 0, 0};

/* Otu_str_0209a23c: 16 bytes at data_ov039_0209a23c */
const char Otu_str_0209a23c[16] = {83, 73, 78, 71, 76, 69, 32, 77, 69, 78, 85, 32, 50, 0, 0, 0};

/* Otu_str_0209a24c: 12 bytes at data_ov039_0209a24c */
const char Otu_str_0209a24c[12] = {143, 213, 147, 203, 137, 137, 143, 111, 49, 0, 0, 0};

/* Otu_str_0209a258: 12 bytes at data_ov039_0209a258 */
const char Otu_str_0209a258[12] = {143, 213, 147, 203, 137, 137, 143, 111, 50, 0, 0, 0};

/* Otu_str_0209a264: 8 bytes at data_ov039_0209a264 */
const char Otu_str_0209a264[8] = {149, 75, 142, 69, 139, 90, 0, 0};

/* Otu_str_0209a26c: 12 bytes at data_ov039_0209a26c */
const char Otu_str_0209a26c[12] = {131, 111, 131, 98, 131, 87, 137, 241, 147, 93, 0, 0};

/* Otu_str_0209a278: 16 bytes at data_ov039_0209a278 */
const char Otu_str_0209a278[16] = {83, 73, 78, 71, 76, 69, 32, 77, 69, 78, 85, 32, 51, 0, 0, 0};

/* Otu_str_0209a288: 12 bytes at data_ov039_0209a288 */
const char Otu_str_0209a288[12] = {131, 111, 131, 98, 131, 87, 73, 68, 0, 0, 0, 0};

/* Otu_str_0209a294: 12 bytes at data_ov039_0209a294 */
const char Otu_str_0209a294[12] = {149, 75, 142, 69, 139, 90, 143, 138, 142, 157, 0, 0};

/* Otu_str_0209a2a0: 8 bytes at data_ov039_0209a2a0 */
const char Otu_str_0209a2a0[8] = {143, 100, 130, 179, 0, 0, 0, 0};

/* Otu_str_0209a2a8: 12 bytes at data_ov039_0209a2a8 */
const char Otu_str_0209a2a8[12] = {130, 220, 130, 170, 130, 233, 151, 205, 0, 0, 0, 0};

/* Otu_str_0209a2b4: 12 bytes at data_ov039_0209a2b4 */
const char Otu_str_0209a2b4[12] = {131, 106, 129, 91, 131, 104, 131, 139, 0, 0, 0, 0};

/* Otu_str_0209a2c0: 8 bytes at data_ov039_0209a2c0 */
const char Otu_str_0209a2c0[8] = {131, 129, 131, 101, 131, 73, 0, 0};

/* Otu_str_0209a2c8: 12 bytes at data_ov039_0209a2c8 */
const char Otu_str_0209a2c8[12] = {131, 110, 131, 147, 131, 125, 129, 91, 0, 0, 0, 0};

/* Otu_str_0209a2d4: 12 bytes at data_ov039_0209a2d4 */
const char Otu_str_0209a2d4[12] = {139, 67, 144, 226, 142, 158, 138, 212, 0, 0, 0, 0};
