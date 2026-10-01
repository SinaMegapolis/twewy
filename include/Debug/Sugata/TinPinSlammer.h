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

/* The stage routines named by OtuScene_WirelessBoard, OtuScene_MenuStage and
 * OtuScene_ResultStage.  These live in the board/pin-logic and result-screen
 * blocks, which are not decompiled yet. */
void func_ov039_02089780(void);
void func_ov039_020897d0(void);
void func_ov039_020897dc(void);
void func_ov039_020898a8(void);
void func_ov039_02089918(void);
void func_ov039_02089ec0(void);
void func_ov039_02089f30(void);
void func_ov039_02089950(void);
void func_ov039_02089a80(void);
void func_ov039_02089d3c(void);
void func_ov039_02089d6c(void);
void func_ov039_02089e0c(void);
void func_ov039_02089e78(void);
void func_ov039_02089f68(void);
void func_ov039_0208a098(void);
void func_ov039_0208a324(void);
void func_ov039_0208a354(void);
void func_ov039_0208a3f4(void);
void func_ov039_0208a454(void);

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
void func_ov039_02082774(TinPinSlammer_Scene* scene);
void func_ov039_020827d0(TinPinSlammer_Scene* scene);

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
 * @brief Character offset of each countdown column in a result-screen row.
 *
 * Indexed by the row's current value to find which character of the assembled
 * buffer a column owns, so a column can be blanked without disturbing the rest
 * of the row.  Word entries -- both users load with `ldr [base, index, lsl #2]`.
 *
 * Two tables, one per row layout: `OtuTextFieldOffset` for the six-group
 * pin-count row (func_ov039_02083ed4, nine columns) and `OtuScoreFieldOffset`
 * for the six-slot three-digit rows (func_ov039_020842bc, eighteen columns).
 */
extern const s32 OtuTextFieldOffset[9];

/** The eighteen-column equivalent of `OtuTextFieldOffset`, for the score rows. */
extern const s32 OtuScoreFieldOffset[18];

void func_ov039_020842bc(TinPinSlammer_Scene* scene, s32 which);

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
extern s32       OtuText_XTight;
extern s32       OtuText_YUnderflow;
extern s32       OtuText_YLow;
extern s32       OtuText_YOverflow;
extern const s32 OtuText_RowLayout[37];

/* Steps one countdown column by a single decimal place (see OtuText.c). */
void func_ov039_02083af4(s32* value, s32 direction, u32 place);

/* Formats a Q12.12 value as `d.ddd` into a six-byte group. */
void func_ov039_02083a20(OtuTextDigits* out, s32 value);

/**
 * @brief Character offset of each column in a `d.ddd` row.
 *
 * The third of the row layouts, for func_ov039_020844f8's four fixed-point
 * groups: sixteen columns rather than nine or eighteen.
 */
extern const s32 OtuFixedFieldOffset[16];

void func_ov039_02084458(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_020844f8(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_020845dc(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_0208462c(TinPinSlammer_Scene* scene);
void func_ov039_020846c4(TinPinSlammer_Scene* scene, s32 step);

/** Character offset table for the single-value row drawn by 0208462c. */
extern const s32 OtuTightFieldOffset[4];

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
extern const char OtuGlyph_ScrollFill[];
extern const char OtuGlyph_ScrollDot[];
extern const char OtuGlyph_ScrollHead[];

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
} OtuTextRow;                  // Size: 0x38

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

/** The scene's two task pools and its heap, by offset from the scene base. */
#define OTU_HEAP(scene)        (&(scene)->base.heap)
#define OTU_HEAP_BUFFER(scene) ((void*)&(scene)->base.heapBuffer)
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

#endif // TIN_PIN_SLAMMER_H
