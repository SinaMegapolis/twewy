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
    /* 0x00000 */ ResourceManager  resMgr; // 0x11580 bytes
    /* 0x11580 */ ResourceManager* prevResMgr;
    /* 0x11584 */ s32              spareDataType;
    /* 0x11588 */ s32              dataType;
    /* 0x1158C */ Heap             heap; // 0x0C bytes
    /* 0x11598 */ u8               heapBuffer[0x10000];
    /* 0x21598 */ u8               pad_21598[0x1FA68];
} TinPinSlammer_Base; // Size: 0x41000

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
    /* 0xAE4 */ u16 linkStatus;       // SystemStatusFlags.unk_06, latched at entry
    /* 0xAE6 */ u8  pad_AE6[2];
    /* 0xAE8 */ s32 wirelessStatus;   // SystemStatusFlags.unk_07, latched at entry
                                      // NB: the two SystemStatusFlags bits are named unk_06/unk_07 on purpose.
                                      // unk_07 is a save/boot flag and unk_06 a tri-state power/hinge mode --
                                      // neither is a wireless flag, they are merely consumed by the wireless
                                      // screens. See docs/overlays.md for the evidence.
    /* 0xAEC */ u8  pad_AEC[4];
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
 * @brief Two words of overlay-global state the scene entry points clear.
 *
 * The entry points address this with a single base register and clear the
 * second word with a 32-bit store and the first with an 8-bit store, so the
 * first is a `u8` with three bytes of padding after it.
 */
typedef struct {
    /* 0x0 */ u8  flag;
    /* 0x1 */ u8  pad_01[3];
    /* 0x4 */ s32 count;
} OtuSceneSlotState; // Size: 0x8
extern OtuSceneSlotState OtuScene_SlotState;

/**
 * @brief The scene's stage-dispatch container, reached inline.
 *
 * The target re-loads the container's address from the literal pool at every
 * use rather than keeping it in a register, so the access is spelled out
 * inline instead of via a local pointer.
 */
#define OTU_STAGE(scene) ((OtuStageDispatch*)((u8*)(scene) + TIN_PIN_SLAMMER_STAGE_OFFSET))

/** The scene's text object, where the result screens' rows are drawn. */
#define OTU_TEXT(scene) ((TextObject*)((u8*)(scene) + 0x416B4))

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
    /* 0x00000 */ TinPinSlammer_Base  base; // 0x40000 bytes
    /* 0x41000 */ TinPinSlammer_State state;
    /* 0x41EF0 */ u8                  pad_41EF0[0x2B74];
    /* 0x44A64 */ s32                 menuIndex; // which result menu is showing
    /* 0x44A68 */ u8                  pad_44A68[0x5C];
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
/* Loads the scene's resources, fills the pin tray and starts the fade task.
 * Called by both entry points that build a results screen. */
void func_ov039_020825e8(TinPinSlammer_Scene* scene);
void func_ov039_02082754(TinPinSlammer_Scene* scene);
void func_ov039_02082774(TinPinSlammer_Scene* scene);
void func_ov039_020827d0(TinPinSlammer_Scene* scene);

/* Wireless-stage routines, still undecompiled (see OtuScene_WirelessStage). */
void func_ov039_02087b04(TinPinSlammer_Scene* scene);
void func_ov039_02087ac4(void);
void func_ov039_02087ac8(void);

/* The board/pin-logic stage routines named by OtuScene_FirstStage. */
void func_ov039_02086728(TinPinSlammer_Scene* scene);
void func_ov039_020867d4(TinPinSlammer_Scene* scene);

/* The stage routines named by OtuScene_WirelessBoard, OtuScene_MenuStage and
 * OtuScene_ResultStage.  These live in the board/pin-logic and result-screen
 * blocks. Those that take a scene pointer are drafted now and declared with
 * it; the rest are still absent and keep the (void) placeholder. */
void func_ov039_02089780(TinPinSlammer_Scene* scene);
void func_ov039_020897d0(TinPinSlammer_Scene* scene);
void func_ov039_020897dc(TinPinSlammer_Scene* scene);
void func_ov039_020898a8(TinPinSlammer_Scene* scene);
void func_ov039_02089918(TinPinSlammer_Scene* scene);
void func_ov039_02089ec0(TinPinSlammer_Scene* scene);
void func_ov039_02089f30(TinPinSlammer_Scene* scene);
void func_ov039_02089950(TinPinSlammer_Scene* scene);
void func_ov039_02089a80(TinPinSlammer_Scene* scene);
void func_ov039_02089d3c(TinPinSlammer_Scene* scene);
void func_ov039_02089d6c(TinPinSlammer_Scene* scene);
void func_ov039_02089e0c(TinPinSlammer_Scene* scene);
void func_ov039_02089e78(TinPinSlammer_Scene* scene);
void func_ov039_02089f68(TinPinSlammer_Scene* scene);
void func_ov039_0208a098(TinPinSlammer_Scene* scene);
void func_ov039_0208a324(TinPinSlammer_Scene* scene);
void func_ov039_0208a354(TinPinSlammer_Scene* scene);
void func_ov039_0208a3f4(TinPinSlammer_Scene* scene);
void func_ov039_0208a454(TinPinSlammer_Scene* scene);

/* Shared engine entry points the overlay's scene setup drives. */
void func_02025b68(void* resource, TinPinSlammer_ResArgs* args);
void func_02025e30(void* resource);
void func_ov039_02098a20(void* a, void* b);

/* The two scene-setup variants; 0x02083944 is the wireless one. */
void func_ov039_02083928(void);
void func_ov039_02083944(void);

/* The stage's exit routine, run first by the scene teardown. */
void func_ov039_02083960(void);

/* The two VBlank handlers, and the GX bring-up they share. */
void func_ov039_020832c0(void);
void func_ov039_02083750(void);
void func_ov039_02083838(void);

/* The wireless stack routines the wireless VBlank handler runs first. */
void func_ov039_02088454(void);
void func_ov039_020885cc(void);

/* The two board-stage tick routines the per-frame updates call. */

/* Redraws one of the result screen's three menus. */
void func_ov039_020842bc(TinPinSlammer_Scene* scene, s32 menu);

/* The overlays the two per-frame updates can push once the stage goes mid-setup. */
void func_ov039_0208690c(void);
void func_ov039_0208694c(void);
void func_ov039_0208698c(void);
void func_ov039_020869cc(void);

/**
 * @brief The scene's process-stage dispatch tables, in `.rodata`.
 *
 * `MainOvlDisp_GetProcessStage` yields the index; each table maps it to the
 * routine that handles the scene at that stage.  There is one table per scene
 * variant, and both end in the matching teardown.
 *
 * The slots hold bare addresses, so the element type is untyped: the dispatcher
 * passes the scene in r0, but the two entry-point slots ignore it entirely and
 * mwcc rejects a function-pointer initialiser for a differently-typed function.
 */
typedef void (*OtuStageHandler)();

/* Referenced by the dispatch tables; see the table comments for their roles. */
void func_ov039_02082ae0(void);
void func_ov039_02082c50(void);
void func_ov039_02082e98(TinPinSlammer_Scene* scene);
void func_ov039_02082ff8(TinPinSlammer_Scene* scene);
void func_ov039_020831cc(TinPinSlammer_Scene* scene);
void func_ov039_020831d8(TinPinSlammer_Scene* scene);

extern const OtuStageHandler OtuScene_PlainHandlers[3];
extern const OtuStageHandler OtuScene_WirelessHandlers[3];

/**
 * @brief Character offset of each hideable column in a result-screen row.
 *
 * Indexed by the row's current value to find which character of the assembled
 * buffer a column owns, so a column can be blanked without disturbing the rest
 * of the row.  Word entries -- every user loads with
 * `ldr [base, index, lsl #2]`.
 *
 * There is one table per row layout, seven in all, differing only in width. The
 * `d.ddd` family is four, eight, twelve and sixteen entries wide for one
 * through four groups; the pin-count row needs nine and the three-digit score
 * rows eighteen.
 *
 * These are referenced by the build's own symbol names rather than by names
 * invented here. Declaring an array and letting dsd place it does not work: the
 * `.rodata` claim is largely gap-filled from the original overlay, so a
 * declared array lands at an address of the linker choosing, not the one the
 * target uses. The bytes were right anyway -- it was the pool word that was
 * wrong. Naming them as the build does puts the pool word back, which is what
 * takes the three-digit row from 99.95% to 100%.
 *
 * Values are recoverable with `tools/ov039_table.py <address>`.
 */
extern const s32 data_ov039_02098f18[9];  /* pin-count row, six digit groups  */
extern const s32 data_ov039_02098e68[4];  /* one d.ddd group                  */
extern const s32 data_ov039_02098e88[4];  /* one d.ddd group, later instance  */
extern const s32 data_ov039_02098e98[4];  /* one d.ddd group, later still     */
extern const s32 data_ov039_02098ef8[8];  /* two d.ddd groups                 */
extern const s32 data_ov039_02098f64[12]; /* three d.ddd groups               */
extern const s32 data_ov039_02098fc4[12]; /* three d.ddd groups, later        */
extern const s32 data_ov039_02098ff4[16]; /* four d.ddd groups                */
extern const s32 data_ov039_02099074[16]; /* four d.ddd groups, later instance */
/* Same twelve entries again, 0x40 bytes lower, used by 020840c0's row. */
extern const s32 data_ov039_02099034[12];
extern const s32 data_ov039_020990b4[18]; /* six three-digit slots            */

/* The countdown rows' column-blanking tables, one per row width.  Same shape as
 * the nine above: entry N is the character position to blank for row N, and the
 * gap at each space position is what keeps a blanked column from eating its
 * neighbour's separator. */
extern const s32 data_ov039_02098e48[2];  /* one two-digit group (020855e0)      */
extern const s32 data_ov039_02098e50[3];  /* the pin-count row's three columns  */
extern const s32 data_ov039_02098e5c[3];  /* three digits          (020851bc)     */
extern const s32 data_ov039_02098e78[4];  /* one d.ddd group       (02085770)     */
extern const s32 data_ov039_02098ea8[6];  /* two three-digit groups (020858e4)   */
extern const s32 data_ov039_02098ec0[6];  /* two three-digit groups (02085b30)   */
extern const s32 data_ov039_02098ed8[8];  /* four two-digit groups  (02085388)   */
extern const s32 data_ov039_02098f3c[10]; /* two five-digit groups  (02084f08)   */
extern const s32 data_ov039_02098f94[12]; /* six mixed groups       (02085e54)   */

/** @} */

/**
 * @brief A six-byte scratch buffer the result rows are formatted into.
 *
 * Four digits with a decimal point wedged at index 1, then a space terminator,
 * so the renderer can treat it as a NUL-free fixed-width string.
 */
typedef struct {
    /* 0x0 */ char thousands;
    /* 0x1 */ char point;
    /* 0x2 */ char hundreds;
    /* 0x3 */ char tens;
    /* 0x4 */ char ones;
    /* 0x5 */ char space;
} OtuTextDigits; // Size: 0x6

/**
 * @brief The countdown's anchor coordinate block, indexed by menu slot.
 *
 * `func_ov039_02084458` and `func_ov039_020844f8` both address this as
 * `OtuText_Shadow + slot * 0x10`, which is four consecutive `s32` per slot.
 * The block is contiguous with `OtuText_RowLayout`, which starts three words
 * later, so the slots overlap the head of that table -- see OtuMenuText.c for
 * the declaration order, which is what makes the addresses line up.
 */
#define OTU_COUNTDOWN_COLUMN(slot) ((s32*)((u8*)&OtuText_Shadow + (slot) * 0x10))

extern s32       OtuText_Shadow;
extern s32       OtuText_LineHeight;
extern s32       OtuText_TabStop;
extern s32       OtuText_XFar;
extern s32       OtuText_XNear;
extern s32       OtuText_XWide;
extern s32       OtuText_XMid;
extern s32       OtuText_YSmall;
extern s32       OtuText_YFar;
extern s32       OtuText_YTiny;
extern s32       OtuText_YMid;
extern s32       OtuText_XTight;
extern s32       OtuText_YTight;
extern s32       OtuText_YUnderflow;
extern s32       OtuText_YOverflow;
extern const s32 OtuText_RowLayout[37];

/*
 * Anchor constants and counters the steppers and row builders reach that sit in
 * a `.data` range this TU does not source, so they keep the build's own names
 * for the same reason as the offset tables above.
 *
 * Values: 0x3000, -0x4000, 0x6000 for the three anchors; the two counters at
 * 0x0209a390 and 0x0209a394 hold 10 and 1000 and are stepped by
 * func_ov039_02084e1c (one of the two, chosen by a parity test) and formatted
 * as a five-digit pair by func_ov039_02084f08.
 */
extern s32 data_ov039_0209a388;
extern s32 data_ov039_0209a38c;
extern s32 data_ov039_0209a390;
extern s32 data_ov039_0209a394;
extern s32 data_ov039_0209a398;

/*
 * The badge movement-rate table and the countdown row built from it, at
 * 0x0209a39c-0x0209a3ab.  Values 0x1800, 0xb33, 0x3000 and 0x4cd: the per-frame
 * rate a badge accumulates for tile type 0, for type 4, for type 3, and the
 * override used when something other than the board is steering.  Indexed by the
 * tile-type switch in 0x0208af6c, never by a loop counter.
 *
 * func_ov039_020840c0 walks the run as one four-word group, while
 * func_ov039_02084020 picks between the four words as separate anchor objects,
 * so the three trailing words get their own names -- the ones the delinker gave
 * each word of the array -- alongside the array itself.
 */
extern s32 data_ov039_0209a39c[4];
extern s32 data_ov039_0209a3a0;
extern s32 data_ov039_0209a3a4;
extern s32 data_ov039_0209a3a8;

/* Steps one countdown column by a single decimal place (see OtuText.c). */
void func_ov039_02083af4(s32* value, s32 direction, u32 place);

/* Formats a Q12.12 value as `d.ddd` into a six-byte group. */
void func_ov039_02083a20(OtuTextDigits* out, s32 value);

void func_ov039_02084458(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_020844f8(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_020845dc(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_0208462c(TinPinSlammer_Scene* scene);
void func_ov039_020846c4(TinPinSlammer_Scene* scene, s32 step);

/* The d.ddd rows and their steppers (OtuRows.c). */
void func_ov039_02084738(TinPinSlammer_Scene* scene);
void func_ov039_020847ec(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02084874(TinPinSlammer_Scene* scene);
void func_ov039_02084944(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_020849a4(TinPinSlammer_Scene* scene);
void func_ov039_02084a3c(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02084ac4(TinPinSlammer_Scene* scene);
void func_ov039_02084b94(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02084c34(TinPinSlammer_Scene* scene);
void func_ov039_02084d20(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02084d80(TinPinSlammer_Scene* scene);

/* The countdown steppers and row builders (OtuScoreRow.c). */
void func_ov039_02084e1c(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02084f08(TinPinSlammer_Scene* scene);
void func_ov039_02085124(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_020851bc(TinPinSlammer_Scene* scene);
void func_ov039_020852c0(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_02085388(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_0208554c(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_020855e0(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_020856c8(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_02085770(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02085818(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_020858e4(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02085a64(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_02085b30(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02085cb0(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_02085e54(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02085fb4(TinPinSlammer_Scene* scene, s32 slot, s32 step);

/**
 * @brief A text block's four coordinate words.
 *
 * The wireless menu and results entry points exchange these two objects at
 * scene setup, so both are plain 16-byte structs assigned through a stack
 * temporary rather than through pointers.
 */
typedef struct {
    /* 0x0 */ s32 x0;
    /* 0x4 */ s32 y0;
    /* 0x8 */ s32 shared;
    /* 0xC */ s32 x1;
} OtuTextBlock; // Size: 0x10

extern OtuTextBlock OtuScene_WirelessMenu;
extern OtuTextBlock OtuScene_ResultMenu;

/**
 * @brief The result screens' scrollbar markers, single characters.
 *
 * Drawn by func_ov039_02083bb0: a dot repeated along the track, one for the
 * head, and a fill for whatever range the value covers.
 */
extern const char data_ov039_0209a16c[];
extern const char data_ov039_0209a170[];
extern const char data_ov039_0209a174[];

/**
 * @brief One row's state in the stage block's scroll table.
 *
 * The stage block (reached as `dispatch->stageBlock`) is an array of these,
 * 16 bytes each, indexed by a count in the block's first word.  A row tracks
 * the value being displayed, the last value drawn, and the row's index -- the
 * two together are what the result screen animates a number into place, and the
 * comparison of current against last is what tells the scrollbar whether it
 * needs redrawing.
 */
typedef struct {
    /* 0x00 */ s32 index;      // which row of the table this is
    /* 0x04 */ s32 value;      // value to display
    /* 0x08 */ s32 drawnValue; // value already drawn, for change detection
    /* 0x0C */ s32 row;        // row index, for vertical placement
    /* 0x10 */ s32 drawnRow;   // row already drawn
    /* 0x14 */ u8  pad_14[0x20];
    /* 0x34 */ s32 flags;      // 0x20 hides this column in the pin-count row
    /* 0x38 */ s32 trayCount;  // saturating tray-slot counter, see below
} OtuTextRow;                  // Size: 0x3C

/*
 * A note on what this struct conflates, since it is load-bearing.
 *
 * Two different objects share one type here. The *base* pointer owns a pair of
 * fields at +0x34 and +0x38, and is read directly as `table->flags` and
 * `table->trayCount`. The *elements* are a separate 16-byte-stride array reached
 * through OTU_TEXT_ROW, and only their first five words are known.
 *
 * So `OtuTextRow` is not really "one row". It is the union of the two views, and
 * the 16-byte stride in OTU_TEXT_ROW is what keeps them from colliding -- which
 * is why the stride is spelled out as a literal instead of `sizeof`.
 *
 * Splitting this into two structs would be tidier but would change every
 * `table->` access in the matched row builders, so it stays as one type until
 * there is a reason to touch the 26 functions that already match.
 *
 * `trayCount` is what func_ov039_02085124 steps: the row's `value / 3` selects a
 * multiplier of 100, 10 or 1 -- hundreds, tens, ones -- and `mult * step` is
 * added in. The result is clamped into [0, 0x130] by two separate tests: a
 * negative result becomes the 0x130 sentinel, and anything at or above 0x131
 * is reset to 0. 0x130 is the same "empty tray slot" value func_ov039_020824a0
 * fills the tray with and func_ov039_020842bc renders as 0, so this is the
 * result screen counting pins into a slot, not a display value.
 */

/**
 * The row at `index` in a table reached through the scene's dispatch.
 *
 * The target walks the table with a 16-byte stride (`add r0, r4, r0, lsl #4`)
 * while reading a fifth field at +0x10, so the stride is one word shorter than
 * the struct.  The indexing is spelled out rather than left to array
 * subscripting for exactly that reason.
 */
#define OTU_TEXT_ROW(block, index) ((OtuTextRow*)((u8*)(block) + (index) * 0x10))

/**
 * @brief One menu's digit columns, 0x1C bytes each.
 *
 * The steppers in OtuScoreRow.c index these with a 0x1C stride off scene+0xEF0
 * (reached as two adds, so keep the split -- see OTU_DIGIT_COLUMN). Each holds a
 * handful of independent saturating counters at fixed offsets, stepped one
 * decimal place at a time by the row's `value / N`:
 *
 *   +0x00  u8   single digit, modulus 10   (0208554c)
 *   +0x04  u8   two digits, modulus 100    (020852c0)
 *   +0x06  s16  stepped through the place stepper (020856c8)
 *   +0x08  s16  up to 999, two places      (02085818)
 *   +0x0C  s16  up to 999, two places      (02085a64)
 *   +0x18  s16  up to 999, three places    (02085fb4)
 *
 * Only the offsets actually read by a decompiled function are named; the rest is
 * padding because the stride, not the layout, is what matters to codegen.
 */
typedef struct {
    u8             pad_00[0x06];
    s16            unk_06;
    u16            unk_08;
    u16            unk_0A;
    u16            unk_0C;
    u16            unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ u16 unk_14;
    /* 0x16 */ u16 unk_16;
    /* 0x18 */ u16 unk_18;
    u8             pad_1A[0x02];
} OtuDigitColumn; // Size: 0x1C

/* The countdown sequencer, OtuCountdown.c.
 *
 * Declared here rather than with the other scene entry points because
 * `OtuTextRow` is not typedef'd until further down, and mwcc reads an unknown
 * `T*` as `int` -- which then collides with the definition.
 *
 * They take the scene but they are not Task stages: there is no TaskHandle here
 * and nothing in this band is created through EasyTask_CreateTask. See the note
 * at the bottom of OtuCountdown.c.
 */
void func_ov039_02086060(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02086174(TinPinSlammer_Scene* scene);
void func_ov039_02086808(TinPinSlammer_Scene* scene);
void func_ov039_02087acc(TinPinSlammer_Scene* scene);
void func_ov039_02087b74(TinPinSlammer_Scene* scene);
void func_ov039_02087c8c(TinPinSlammer_Scene* scene, s32 menu);
void func_ov039_02087dc0(TinPinSlammer_Scene* scene, void* data);

/**
 * @brief A two-word point, copied out of a task and compared against another.
 *
 * func_ov039_0208e6e0 copies a task's +0x120/+0x124 pair into one of these, and
 * func_ov039_02098ca8 takes two of them and returns a score.
 */
typedef struct {
    s32 x;
    s32 y;
} OtuPoint; // Size: 0x8

/**
 * @brief A child task in the scene's pin pool.
 *
 * The overlay keeps a pool of these under the stage task, and the five queries
 * in OtuTaskPick.c each walk the pool asking every child for its position and
 * returning the nearest one that passes their own filter.
 *
 * The three filters that test `kind` are the same function body with a different
 * constant (6, 7, 8), which is why three of the five queries are byte-identical
 * apart from one `cmp`. That is the overlay's clearest evidence yet that 6, 7
 * and 8 are three sorts of the same thing -- most likely the three pin types
 * the results screen scores separately.
 *
 * `pinID` points at a tray slot's value and 0x130 there means "no pin", which
 * is the same sentinel func_ov039_020824a0 fills the tray with; func_ov039_0208efb0
 * rejects those. Only fields the queries or their filters read are named.
 */
/**
 * @brief One pin, 0x174 bytes -- the trimmed view of the same object band 12
 *        models in full as OtuBadge, and band B3 as OtuBadgeState.
 *
 *  All three are views of one 0x25C task, "Tsk_OtosuGame_badge". This one is
 *  the narrow prefix the pool queries need; see OtuBadge's own comment for the
 *  handle that ties them together.
 *
 *  The offsets in the comments are load-bearing. This type used to carry no
 *  padding at all, so C laid its fields out at 0x0, 0x4, 0x8 ... and every
 *  access compiled to a displacement four times too small: `task->kind` became
 *  `ldr r0, [r0, #4]` instead of `ldr r1, [r0, #0xf8]`. Nothing noticed for as
 *  long as the type was only ever used as an opaque pointer -- the seven
 *  accessors at 0x0208e6cc..0x0208e87c that read through it were all sitting at
 *  60%, and four of this band's seven predicates at 57.8%.
 */
typedef struct {
    u8               pad_000[0xE4];
    /* 0x0E4 */ s32  unk_E4;
    u8               pad_0E8[0x10];
    /* 0x0F8 */ s32  kind; // 6, 7 or 8 -- which sort of pin this is
    u8               pad_0FC[0x24];
    /* 0x120 */ s32  x;
    /* 0x124 */ s32  y;
    u8               pad_128[0x20];
    /* 0x148 */ s32  alive; // non-zero while the pin is still in play
    u8               pad_14C[0x20];
    /* 0x16C */ u16* pinID; // a tray slot's value; 0x130 means "no pin"
    u8               pad_170[0x04];
} OtuPinTask;               // Size: 0x174

/**
 * The child task ids the stage keeps at +0x17C, one word per pool slot.
 *
 * Indexing is by a plain word stride and the count lives separately at +0x140,
 * so both are reached through these rather than as struct members -- OtuTextRow
 * is a conflated type already and widening it would disturb the matched row
 * builders.
 */
#define OTU_CHILD_COUNT(table) (*(s32*)((u8*)(table) + 0x140))

/**
 * The child task id at pool slot `i`.
 *
 * Spelled as `(table + i * 4) + 0x17C` rather than `table + (0x17C + i * 4)` on
 * purpose. The second form folds into a single computed displacement and costs
 * an add; the target keeps the scaled add and the fixed +0x17C load separate, so
 * the association here is load-bearing.
 */
#define OTU_CHILD_ID(table, i) (*(s32*)((u8*)(table) + (i) * 4 + 0x17C))

/* Copies a task's +0x120/+0x124 pair into `out` (OtuTaskPick.c). */
void func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out);

/* Returns a score for the pair of points, via the overlay's vector unit. */
s32 func_ov039_02098ca8(OtuPoint* a, OtuPoint* b);

/* The five child filters: true when the child is a candidate worth scoring. */
s32 func_ov039_0208e984(OtuPinTask* task); // kind == 8
s32 func_ov039_0208e998(OtuPinTask* task); // kind == 7
s32 func_ov039_0208e9d0(OtuPinTask* task); // kind == 6
s32 func_ov039_0208ee84(OtuPinTask* task); // alive
s32 func_ov039_0208efb0(OtuPinTask* task, s32 which);

/**
 * @brief The sprite slot this overlay fills in, 0x14 bytes.
 *
 * Not overlay data: `data_0206b408` is a `.bss` object owned by `SpriteMgr` in
 * the main module, and the overlay writes it directly. That is the overlay
 * reaching out of itself rather than using a call, and it is the only time in
 * this overlay that a global's owner is known rather than merely its address.
 *
 * Fields stay as offsets. Only +0x00, +0x04, +0x08, +0x0C and +0x10 are reached
 * by any decompiled function here, and nothing yet pins down what the first four
 * mean; +0x10 holds a packed depth key rather than a pointer even though it is
 * read and written as one, which is why it is typed as the key it is.
 */
typedef struct {
    /* 0x00 */ s32   unk_00;
    /* 0x04 */ s32   unk_04;
    /* 0x08 */ s32   unk_08; // a byte pointer, but stored as a word
    /* 0x0C */ void* unk_0C;
    /* 0x10 */ s32   depthKey;
} OtuSpriteSlot; // Size: 0x14

extern OtuSpriteSlot* data_0206b408;

/**
 * @brief A task that can produce a sprite cell.
 *
 * Only the four fields the twenty-two cell builders read are named: a signed
 * index at +0x16 and a table pointer at +0x1C, which together drive the
 * two-step lookup, plus the coordinate fields at +0x4C and onwards that the
 * packer is fed from.
 */
typedef struct {
    u8 pad_16[0x100];
} OtuSpriteTask;

/* Packs a Q12.12 pair into a single sortable depth key (OtuSpriteCell.c). */
s32 func_ov039_02088400(s32 x, s32 y, s32 axis);

/* The twenty-two sprite-cell builders.  Each is the same body with three
 * constants varied; see OtuSpriteCell.c for what they are. */
OtuSpriteSlot* func_ov039_0208d2f8(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_0208f134(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_0208f440(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_0208fa90(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02090208(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020902cc(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02091118(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020916c4(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020923c8(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02094ae8(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02094ff4(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_0209549c(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020958a8(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02095dec(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02096280(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_0209659c(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02096660(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02096e7c(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02097398(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020977d0(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02097ae0(OtuSpriteTask* task, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02097ff4(OtuSpriteTask* task, s32 arg, s32 mode);

/**
 * @brief The digit-column array, reached the way the target reaches it.
 *
 * The digit-column array. (Folded to `scene + 0x41EF0`; the old note said the
 * two-add spelling was load-bearing, but folding it is verified codegen-neutral.)
 */
#define OTU_DIGIT_COLUMN(scene, slot) ((OtuDigitColumn*)((u8*)(scene) + 0x41EF0 + (slot) * 0x1C))

/**
 * @brief The same array, reached with the stride multiply done first.
 *
 * Three of the functions (020855e0, 02085770, 02085cb0, 02085e54) form the
 * address as `scene + slot * 0x1C` and *then* add 0x41000, where the rest add
 * 0x41000 before the multiply. mwcc preserves that ordering, so the two forms
 * need separate macros even though they name the same bytes.
 */
#define OTU_DIGIT_COLUMN_STRIDED(scene, slot) ((OtuDigitColumn*)((u8*)(scene) + (slot) * 0x1C + 0x41000))

/**
 * @brief A table of `OtuTextRow`, reachable from the scene's dispatch.
 *
 * `func_ov039_02098b70` is a one-instruction accessor for `dispatch->stageBlock`.
 */
void* func_ov039_02098b70(OtuStageDispatch* dispatch);

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

/**
 * @brief One menu's countdown block, at scene + 0x44000 + index * 0x34.
 *
 * The result screens show three menus side by side; each has its own copy of the
 * score columns, and the countdown animation walks them one decimal place per
 * frame.  The index at +0xA64 of the scene's 0x44000 block selects which menu is
 * being animated.
 */
typedef struct {
    /* 0x84 */ s16 thousands; // the digit column being counted
    /* 0x86 */ s16 hundreds;
    /* 0x88 */ s16 tens;
    /* 0x98 */ s16 unk_98;
    /* 0xA8 */ s16 unk_A8;
} OtuMenuCountdown; // Size: 0x34

/**
 * The countdown block for the menu the scene is currently animating.
 *
 * The index is read from the scene rather than passed in, which is why the base
 * is rebuilt on every access in the target.
 */
#define OTU_COUNTDOWN(scene) ((OtuMenuCountdown*)((u8*)(scene) + 0x44000 + (scene)->menuIndex * 0x34))

/**
 * @brief The pin-tray counts, six `u16` slots at scene + 0x4404C.
 *
 * `func_ov039_020842bc` reads six consecutive halfwords from here and renders
 * each as a zero-padded three-digit field.  The block is 0xE bytes per slot and
 * the index is multiplied in rather than folded, so the target's case 0 arm
 * computes `base + which * 7` even though `which` is 0 on that path.
 *
 * Slot value 0x130 is the sentinel for "empty", and is displayed as 000 rather
 * than 305.
 */
#define OTU_PIN_TRAY(scene, slot) ((u16*)((u8*)(scene) + 0x4404C + (slot) * 0xE))

/**
 * @brief The score row for menu slot `slot`, six `u16` at scene + 0x4408A.
 *
 * Three menus are shown side by side and each gets its own row, so the block is
 * indexed by `slot` (0..2).  Note the `menuIndex` multiply happens *after* the
 * 0x8A displacement in the target -- the countdown base is `scene + 0x4408A`
 * and the menu stride is added to that -- so the two are not one flat offset.
 * Same rendering and same 0x130 sentinel as `OTU_PIN_TRAY`.
 */
#define OTU_SCORE_ROW(scene, slot) ((u16*)((u8*)(scene) + 0x4408A + (scene)->menuIndex * 0x34 + (slot) * 0x10))

/** The stage the wireless-board entry point enters; 0x2D0 bytes of state. */
extern OtuSceneStage OtuScene_WirelessBoard;

/** The result-screen menu and results stages, both 0x2D0 bytes of state. */
extern OtuSceneStage OtuScene_MenuStage;
extern OtuSceneStage OtuScene_ResultStage;

/** Shared scene objects. */
/**
 * @brief The overlay's three debug-sequence names, all "Seq_Otosu()".
 *
 * The scene entry points index this table rather than using the string
 * directly, and the three variants pick different slots (the wireless entry
 * uses index 2).
 */
extern const char* const OtuScene_SequenceNames[3];

/** Total size of the scene block MainOvlDisp allocates. */
#define TIN_PIN_SLAMMER_SCENE_SIZE 0x44A6C

/**
 * Id of the overlay the per-frame updates push when the stage goes mid-setup.
 * 2 is the debug-menu overlay these callbacks live in.
 */
#define OTU_OVERLAY_ID 2

/** Offset of the byte written just before the first stage is entered. */
#define TIN_PIN_SLAMMER_ENTRY_OFFSET 0x44A64

/**
 * The wireless save record the overlay's entry points read.
 *
 * It is not `gSaveState`: the three entry points index one common record but
 * disagree about which global it hangs off, so it is addressed by absolute
 * offset from `gSaveData`.  The two bases differ per call site — the menus and
 * results entry point uses +0x3000, the per-frame update uses +0x1000 — so the
 * offset is passed in rather than baked into a single macro.
 */
#define OTU_WIRELESS_RECORD(base, off) ((u8*)&gSaveData + (base) + (off))

/**
 * A save-record field addressed as `base + off`, where the target splits the
 * displacement across two adds rather than folding it into one constant.
 *
 * func_ov039_02082c50 reaches its 6-byte copy source this way, as
 * `gSaveData + 0x3EC` then `+ 0x3000`. Written as a single folded offset it
 * compiles to one `add` and the function comes out 16 bytes short.
 */
#define OTU_WIRELESS_RECORD_SPLIT(off) ((u8*)&gSaveData + (off) + 0x3000)

/** The scene's two task pools and its heap, by offset from the scene base. */
#define OTU_HEAP(scene)        (&(scene)->base.heap)
#define OTU_HEAP_BUFFER(scene) ((void*)&(scene)->base.heapBuffer)
#define OTU_POOL1(scene)       ((TaskPool*)((u8*)(scene) + TIN_PIN_SLAMMER_POOL1_OFFSET))
#define OTU_POOL2(scene)       ((TaskPool*)((u8*)(scene) + TIN_PIN_SLAMMER_POOL2_OFFSET))

/** Scene entry points. */
void func_ov039_02082978(void);

/* Display/VBlank init entry points in this overlay (not yet decompiled). */
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

/* A cursor over a table of s32s, walked four bytes at a time.
 *
 * Declared here rather than in a band: bands 1 and 7 both drive these cursors, band
 * 7 comes first in the include order, and neither should have to own it.
 */
/**
 * @brief A cursor over a table of s32s, 12 bytes.
 *
 * Not part of the pin tasks: this walks an indexed list one entry per call,
 * spending `remaining` frames on each. func_ov039_0208ffac starts it at entry 0
 * and func_ov039_0208ffd8 advances it.
 */
typedef struct {
    /* 0x00 */ s32* table;
    /* 0x04 */ s16  index;
    /* 0x06 */ s16  count;
    /* 0x08 */ s16  framesLeft;
} OtuCursor; // Size: 0xA
/* ============================================================================
 * OtuVecOps.c's vector helpers.
 *
 * Those functions live in their own translation unit, so a band that calls one
 * cannot see it unless it is declared here. Only func_ov039_02098ca8 had a
 * declaration, and the bands were each carrying private copies of the others --
 * which is how four functions in one batch ended up calling an implicit
 * `int (...)` and being rejected for the pointer arguments.
 *
 * All the arithmetic is Q12.12 and all of them take OtuPoint*, which is defined
 * above, so this block only needs the prototypes.
 *
 * func_ov039_02098d7c and func_ov039_02098dbc are NOT here: their parameter
 * types live inside OtuVecOps.c. Nothing outside that file calls them yet.
 * ==========================================================================*/

void func_ov039_02098bb0(OtuPoint* a, OtuPoint* b, OtuPoint* out);
void func_ov039_02098bd4(s32 angle, OtuPoint* a, OtuPoint* out);
void func_ov039_02098c00(s32 angle, OtuPoint* a, OtuPoint* b, OtuPoint* out);
s32  func_ov039_02098c40(OtuPoint* a, OtuPoint* b);      // dot product
s32  func_ov039_02098c70(OtuPoint* a, OtuPoint* b);      // cross product's z
s32  func_ov039_02098d10(OtuPoint* v);                   // Vec_Magnitude over 2D
void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest); // Vec_Normalize over 2D

/* ============================================================================
 * The obstacle accessors, from band 3's range and never yet written here.
 * Batch B4's OtuBadge calls all three, and with no prototype anywhere above
 * them each call became an implicit `int (...)`.
 *
 * Only func_ov039_02092758's result is used, and it is read straight out of r0,
 * so the return type here does not have to be right to generate the right code
 * at the call site.
 * =========================================================================*/

void func_ov039_02092744(void* obstacle, OtuPoint* out); // read the position
s32  func_ov039_02092758(void* obstacle);                // read the radius
void func_ov039_02092760(void* obstacle);                // apply/refresh

#endif                                                   // TIN_PIN_SLAMMER_H
