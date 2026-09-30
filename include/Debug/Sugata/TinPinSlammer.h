#ifndef TIN_PIN_SLAMMER_H
#define TIN_PIN_SLAMMER_H

#include "Interface/Menu/MenuCommon.h"

/**
 * @file TinPinSlammer.h
 * @brief Shared declarations for overlay 39, the Tin Pin Slammer (Otosu) minigame.
 *
 * The overlay is driven by a single large scene object that MainOvlDisp
 * allocates out of gDebugHeap and passes to every entry point in r0.  The
 * original source reached the scene's sub-objects through very large fixed
 * offsets (0x11000, 0x41000, 0x44000, ...), so they are modelled here as
 * separate structs reached by explicit pointer arithmetic.
 *
 * Offsets observed in the USA target:
 *   scene + 0x11000 + 0x580  prevResMgr  (MenuStateBase.prevResMgr)
 *   scene + 0x11000 + 0x584  spareDataType
 *   scene + 0x11000 + 0x588  dataType
 *   scene + 0x11400 + 0x18C  heap         (MenuStateBase.heap)
 *   scene + 0x11400 + 0x198  heapBuffer   (MenuStateBase.heapBuffer)
 *   scene + 0x41598          main task pool (MenuStateBase.taskPool)
 *   scene + 0x41618          second task pool
 *   scene + 0x41000 + ...    per-game state block
 *   scene + 0x44000 + ...    pin slot / board tables
 */

#define TIN_PIN_SLAMMER_HEAP_OFFSET  0x11400
#define TIN_PIN_SLAMMER_POOL1_OFFSET 0x41598
#define TIN_PIN_SLAMMER_POOL2_OFFSET 0x41618
#define TIN_PIN_SLAMMER_STATE_OFFSET 0x41000
#define TIN_PIN_SLAMMER_TABLE_OFFSET 0x44000
#define TIN_PIN_SLAMMER_BASE_OFFSET  0x11000

/** Byte offset of the pin-tray `u16` array within the 0x44000 sub-object. */
#define TIN_PIN_SLAMMER_TRAY_OFFSET 0x4C

/** Byte offset of the pin tray from the base of the scene object. */
#define TIN_PIN_SLAMMER_TRAY_BYTE (TIN_PIN_SLAMMER_TABLE_OFFSET + TIN_PIN_SLAMMER_TRAY_OFFSET)

/**
 * @brief The pin tray's first slot, as a `u16` index from the base of the scene.
 *
 * The overlay treats the scene block as one flat `u16` array and indexes the
 * tray straight out of it, so this is (0x44000 + 0x4C) / 2.  Written as a
 * literal because mwcc rejects arithmetic in array subscripts.
 */
#define TIN_PIN_SLAMMER_TRAY_SLOT 0x22026

/** Byte offset of the 0x18-byte resource-handler object the scene owns. */
#define TIN_PIN_SLAMMER_RES_OFFSET 0x416A8

/**
 * @brief Layout of the equipped-pin array the scene setup reads.
 *
 * The overlay does not walk this through `EquippedPin`; it uses a bare byte
 * cursor over the save data and reads each pin's id at a fixed offset from the
 * array base, stepping by the record stride.
 */
#define TIN_PIN_SLAMMER_PIN_ID_OFFSET 0x74
#define TIN_PIN_SLAMMER_PIN_STRIDE    0xA

/**
 * @brief The 0x44000 sub-object: per-scene tables, with the pin tray at +0x4C.
 */
typedef struct {
    /* 0x00 */ u8  pad[0x4C];
    /* 0x4C */ u16 tray[7];
} OtuSceneTables;

/**
 * @brief The 0x10000-byte MenuStateBase block the scene object starts with.
 *
 * Only the fields the overlay's own code touches are modelled; the remainder
 * is left as padding so the pool offsets below stay correct.
 */
typedef struct {
    /* 0x000 */ u8               pad_000[0x580];
    /* 0x580 */ ResourceManager* prevResMgr;
    /* 0x584 */ s32              spareDataType;
    /* 0x588 */ s32              dataType;
    /* 0x58C */ u8               pad_58C[4];
    /* 0x590 */ Heap             heap; // 0x0C bytes
    /* 0x59C */ u8               heapBuffer[0x10000];
    /* 0x1009C */ u8             pad_1009C[0x2F764];
} TinPinSlammer_Base; // Size: 0x30000

/**
 * @brief Per-game state block based at scene + 0x41000.
 *
 * The target writes these fields; names are kept as offsets until the field's
 * role is confirmed by the code that reads it.  Padding keeps every field on
 * the offset the target uses.
 */
typedef struct {
    /* 0x000 */ u8  pad_000[0x698];
    /* 0x698 */ s32 unk_698;
    /* 0x69C */ s32 unk_69C; // TaskHandle of the fade task; deleted on teardown
    /* 0x6A0 */ u8  pad_6A0[4];
    /* 0x6A4 */ s32 unk_6A4; // "background resource created" flag
} TinPinSlammer_State;       // Size: 0x6A8

/**
 * @brief State fields that sit past the resource-handler object.
 *
 * Based at scene + 0x416C0, i.e. immediately after the 0x18-byte resource
 * object.  Offsets are absolute-from-scene minus 0x416C0.
 */
typedef struct {
    /* 0x000 */ u8  pad_000[0x41C];
    /* 0x41C */ s32 unk_ADC;
    /* 0x420 */ s32 unk_AE0; // forces a state transition when set
    /* 0x424 */ u8  pad_424[0xC];
    /* 0x430 */ s32 unk_AF0; // scene mode selector (0 = plain, 1 = alternate)
    /* 0x434 */ s32 unk_AF4;
    /* 0x438 */ u8  pad_438[0x3E8];
    /* 0x820 */ s32 unk_EE0;
    /* 0x824 */ s32 unk_EE4;
    /* 0x828 */ s32 unk_EE8;
    /* 0x82C */ s32 unk_EEC;
} TinPinSlammer_StateTail;

/**
 * @brief The whole Otosu scene block, as one struct.
 *
 * The scene is a single heap block whose sub-objects sit at very large offsets.
 * Modelling them as members of one struct (with padding between) is what the
 * overlay's own codegen implies: every field past 0xFFF is too far for a `ldr`
 * displacement, so mwcc re-derives the containing pointer on each access rather
 * than hoisting it, exactly as the target does.
 */
typedef struct {
    /* 0x00000 */ u8                      pad_000[0x41000];
    /* 0x41000 */ TinPinSlammer_State     state;
    /* 0x416A8 */ u8                      resHandler[0x18];
    /* 0x416C0 */ TinPinSlammer_StateTail tail;
} TinPinSlammer_Scene; // Size: 0x41EF0

/** Byte offset of the scene's 0x10000-byte MenuStateBase block. */
#define TIN_PIN_SLAMMER_BASE_BYTE 0x11000

/**
 * @brief Resource-handler argument block, 0x20 bytes.
 *
 * Built on the stack by func_ov039_02082520 and handed to the shared
 * UnkResourceHandler create function.
 */
typedef struct {
    /* 0x00 */ s32   unk_00;
    /* 0x04 */ s32   unk_04;
    /* 0x08 */ s32   dataType;
    /* 0x0C */ void* unk_0C;
    /* 0x10 */ s32   unk_10;
    /* 0x14 */ s32   unk_14;
    /* 0x18 */ u16   unk_18;
    /* 0x1A */ u16   unk_1A;
    /* 0x1C */ u16   unk_1C;
    /* 0x1E */ u16   unk_1E;
} TinPinSlammer_ResArgs; // Size: 0x20

/* Overlay entry points. */
void func_ov039_020824a0(u16* scene);
void func_ov039_02082520(TinPinSlammer_Scene* scene);
void func_ov039_020825b0(TinPinSlammer_Scene* scene);

/* Shared engine entry points the overlay's scene setup drives. */
void func_02025b68(void* resource, TinPinSlammer_ResArgs* args);
void func_02025e30(void* resource);
void func_ov039_02098a20(void* a, void* b);

extern const void* data_0205c9b0;

#endif // TIN_PIN_SLAMMER_H
