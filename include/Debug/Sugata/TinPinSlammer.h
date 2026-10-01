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
    /* 0x69C */ s32 unk_69C;          // TaskHandle of the fade task; deleted on teardown
    /* 0x6A0 */ u8  pad_6A0[4];
    /* 0x6A4 */ s32 unk_6A4;          // "background resource created" flag
    /* 0x6A8 */ u8  resHandler[0x18]; // 0x18-byte resource-handler object
    /* 0x6C0 */ u8  pad_6C0[0x41C];
    /* 0xADC */ s32 unk_ADC;          // "setup incomplete" flag, raised while a stage runs
    /* 0xAE0 */ s32 unk_AE0;          // forces a state transition when set
    /* 0xAE4 */ u8  pad_AE4[0xC];
    /* 0xAF0 */ s32 unk_AF0;          // scene mode selector (0 = plain, 1 = alternate)
    /* 0xAF4 */ s32 unk_AF4;
    /* 0xAF8 */ u8  pad_AF8[0x3E8];
    /* 0xEE0 */ s32 unk_EE0;
    /* 0xEE4 */ s32 unk_EE4;
    /* 0xEE8 */ s32 unk_EE8;
    /* 0xEEC */ s32 unk_EEC;
} TinPinSlammer_State; // Size: 0xEF0

/**
 * @brief The scene's stage-dispatch container, based at scene + 0x41AC4.
 *
 * A small state machine the scene uses to sequence its setup stages.  The
 * overlay's four helpers drive it:
 *   - `02098a60` runs the stage's action callback, allocating a stage block on
 *     first use,
 *   - `02098acc` runs the stage's sub-callback by index,
 *   - `02098af4` runs the stage's cleanup callback,
 *   - `02098b44` reports whether the stage has anything left to do.
 *
 * Field meanings are inferred from the helper bodies; names stay as offsets
 * until a caller pins them down.
 */
typedef struct {
    /* 0x00 */ Heap* heap;   // allocator for the stage block
    /* 0x04 */ void* stage;  // current stage block
    /* 0x08 */ s32   stageIndex;
    /* 0x0C */ void* action; // stage's action callback
    /* 0x10 */ BOOL  active; // stage is live
    /* 0x14 */ void* stageBlock;
} OtuStageDispatch;          // Size: 0x18

/** Byte offset of the stage-dispatch container within the scene object. */
#define TIN_PIN_SLAMMER_STAGE_OFFSET 0x41AC4

/**
 * @brief A two-byte slot of global state the scene entry points clear.
 *
 * Lives in the overlay's `.bss`; the entry points zero both the 16-bit value
 * at +0 and the one at +4 before bringing the display up.
 */
typedef struct {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u8  pad_02[2];
    /* 0x04 */ u16 unk4;
} OtuSceneSlotState; // Size: 0x8

/**
 * @brief The scene's stage-dispatch container, reached inline.
 *
 * The target re-loads the container's address from the literal pool at every
 * use rather than keeping it in a register, so the access is spelled out
 * inline instead of via a local pointer.
 */
#define OTU_STAGE(scene) ((OtuStageDispatch*)((u8*)(scene) + TIN_PIN_SLAMMER_STAGE_OFFSET))

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
    /* 0x00000 */ u8                  pad_000[0x41000];
    /* 0x41000 */ TinPinSlammer_State state;
    /* 0x41EF0 */ u8                  pad_41EF0[0xBD4];
    /* 0x44AC4 */ OtuStageDispatch    stage;
} TinPinSlammer_Scene;
// NOTE: the stage container sits at scene + 0x41AC4, which is *inside* the
// region after `state` (state ends at 0x41EF0).  Keep reaching it with
// TIN_PIN_SLAMMER_STAGE_OFFSET rather than as the member declared here, which
// only exists to document the shape.

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
void func_ov039_02082724(TinPinSlammer_Scene* scene);
void func_ov039_0208273c(TinPinSlammer_Scene* scene);
void func_ov039_02082754(TinPinSlammer_Scene* scene);
void func_ov039_02082774(TinPinSlammer_Scene* scene);
void func_ov039_020827d0(TinPinSlammer_Scene* scene);

/* Wireless-stage routines, still undecompiled (see OtuScene_WirelessStage). */
void func_ov039_02087acc(void);
void func_ov039_02087b04(void);
void func_ov039_02087b74(void);
void func_ov039_02087ac4(void);
void func_ov039_02087ac8(void);

/* The board/pin-logic stage routines named by OtuScene_FirstStage. */
void func_ov039_02086808(void);
void func_ov039_02086728(void);
void func_ov039_020867d4(void);

/* Shared engine entry points the overlay's scene setup drives. */
void func_02025b68(void* resource, TinPinSlammer_ResArgs* args);
void func_02025e30(void* resource);
void func_ov039_02098a20(void* a, void* b);

/* Stage-dispatch helpers (see OtuStageDispatch).  These are not decompiled
 * yet, so they are declared with the untyped signature the target uses. */
void  func_ov039_02098a60(void* dispatch, void* scene);
void  func_ov039_02098acc(void* dispatch, void* scene);
void  func_ov039_02098af4(void* dispatch, void* scene);
s32   func_ov039_02098b44(void* dispatch);
void* func_ov039_02098b68(void* dispatch);
void  func_ov039_02098a40(void* dispatch, void* stage);

/**
 * @brief One entry in the scene's stage table.
 *
 * A stage descriptor: the routine to run, a secondary routine, a pointer to a
 * companion descriptor, and a size.  `OtuScene_WirelessStage` is the entry the
 * scene jumps to when the wireless link needs attention.
 */
typedef struct {
    /* 0x00 */ void* run;
    /* 0x04 */ void* sub;
    /* 0x08 */ void* companion;
    /* 0x0C */ u32   size;
} OtuSceneStage; // Size: 0x10

extern OtuSceneStage OtuScene_WirelessStage;

/**
 * @brief The stage's companion record, a single routine pointer.
 *
 * Unlike `OtuSceneStage` this companion is one word, so it is its own type.
 */
typedef struct {
    /* 0x00 */ void* run;
} OtuSceneStageCompanion; // Size: 0x4

/** The scene's first stage: board/pin logic, 0x3C bytes of state. */
extern OtuSceneStage OtuScene_FirstStage;

/** Shared scene objects. */
extern const char        OtuScene_SequenceName[];
extern OtuSceneSlotState OtuScene_SlotState;

/** Total size of the scene block MainOvlDisp allocates. */
#define TIN_PIN_SLAMMER_SCENE_SIZE 0x44A6C

/** Offset of the byte written just before the first stage is entered. */
#define TIN_PIN_SLAMMER_ENTRY_OFFSET 0x44A64

/** The scene's two task pools and its heap, by offset from the scene base. */
#define OTU_HEAP(scene)        ((Heap*)((u8*)(scene) + TIN_PIN_SLAMMER_HEAP_OFFSET))
#define OTU_HEAP_BUFFER(scene) ((void*)((u8*)(scene) + TIN_PIN_SLAMMER_HEAP_OFFSET + 0x18C))
#define OTU_POOL1(scene)       ((TaskPool*)((u8*)(scene) + TIN_PIN_SLAMMER_POOL1_OFFSET))
#define OTU_POOL2(scene)       ((TaskPool*)((u8*)(scene) + TIN_PIN_SLAMMER_POOL2_OFFSET))

/** Scene entry points. */
void func_ov039_02082978(void);

/* Display/VBlank init entry points in this overlay (not yet decompiled). */
void func_ov039_02083928(void);
void func_ov039_02083944(void);
void func_0200d8f0(void);

/* Two zeroed u16s the scene entry points clear. */
extern u16 data_02066aec;
extern u16 data_02066eec;

/* Wireless-communication entry points in ov040. */
s32  func_ov040_0209cb78(void);
void func_ov040_0209d990(void);
void func_ov040_0209d970(s32 arg0);
void func_ov040_0209ece4(void);
void func_ov040_0209ed20(void);
void func_ov040_0209ed30(void);

/* Wireless entry point in ov003. */
void func_ov003_0209d434(s32 a, s32 b);

extern const void* data_0205c9b0;

#endif // TIN_PIN_SLAMMER_H
