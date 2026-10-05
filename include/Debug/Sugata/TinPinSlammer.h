#ifndef TIN_PIN_SLAMMER_H
#define TIN_PIN_SLAMMER_H

#include "Engine/Core/OamMgr.h"
#include "Engine/Text.h"
#include "Interface/Menu/MenuCommon.h"
#include "SpriteMgr.h"

/**
 * @file TinPinSlammer.h
 * @brief Shared declarations for overlay 39, the Tin Pin Slammer (Otosu) minigame.
 *
 * The overlay is driven by a single large scene object (TinPinSlammer_Scene)
 * that its entry points allocate out of gDebugHeap and MainOvlDisp passes to
 * every process stage in r0.
 */

/**
 * @brief A scene stage routine. Unprototyped: the routines disagree on whether
 * they take the scene, and mwcc rejects a mismatched function-pointer
 * initialiser.
 */
typedef void (*OtuStageHandler)();

/**
 * @brief A stage of the scene: entered once, stepped through `steps` by
 * OtuStageDispatch.stageIndex, and exited once. `blockSize` bytes of state are
 * allocated for it as OtuStageDispatch.stageBlock.
 */
typedef struct {
    /* 0x00 */ OtuStageHandler        enter;
    /* 0x04 */ OtuStageHandler        exit;
    /* 0x08 */ const OtuStageHandler* steps;
    /* 0x0C */ u32                    blockSize;
} OtuSceneStage; // Size: 0x10

/**
 * @brief The scene's stage sequencer (TinPinSlammer_Scene.stage).
 *
 * func_ov039_02098a40 queues `next`; func_ov039_02098a60 switches to it (running
 * its `enter` and allocating its block), func_ov039_02098acc runs the current
 * step, and func_ov039_02098af4 exits the stage and frees its block.
 */
typedef struct {
    /* 0x00 */ Heap*                heap;       // allocator for stageBlock
    /* 0x04 */ const OtuSceneStage* stage;      // the running stage
    /* 0x08 */ s32                  stageIndex; // which of stage->steps runs
    /* 0x0C */ const OtuSceneStage* next;       // queued by func_ov039_02098a40
    /* 0x10 */ BOOL                 pending;    // a switch to `next` is queued
    /* 0x14 */ void*                stageBlock;
} OtuStageDispatch;                             // Size: 0x18

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
 * @brief One pin's parameters: one record of BeBadge_Parm.bin, indexed by pin id.
 *
 * The scene loads the whole file into TinPinSlammer_Scene.badgeParams, the
 * badges read their pin's record through OtuBadge.slots, and SINGLE MENU 3 of
 * the debug editor edits one record at a time.
 */
typedef struct {
    /* 0x00 */ u8  tile[4];      // frame budgets per phase, in the order 6, 8, 7, 9
    /* 0x04 */ u8  tuneIndex;    // index into data_ov039_0209a3e0, 0x10 stride
    /* 0x05 */ u8  pad_05;
    /* 0x06 */ s16 friction;     // Q12.12, scaled by velMag / 3 into a heading offset
    /* 0x08 */ u16 needleCharge; // the needle attack's frames, func_ov039_02091654
    /* 0x0A */ u16 needleHold;
    /* 0x0C */ u16 meteoHold;    // the meteo attack's frames, func_ov039_0208ce88
    /* 0x0E */ u16 meteoSquash;
    /* 0x10 */ s16 hammerRate;   // the hammer attack, func_ov039_02091028
    /* 0x12 */ s16 hammerLength;
    /* 0x14 */ u16 hammerFrames;
    /* 0x16 */ u16 hammerArc;
    /* 0x18 */ u16 stunFrames; // how long a hit stuns this pin
    /* 0x1A */ u8  pad_1A[0x02];
} OtuBadgeParam;               // Size: 0x1C

/** The pin id that marks an empty deck slot; also the number of pin records. */
#define OTU_NO_PIN 0x130

/** One opponent of a single-player match. */
typedef struct {
    /* 0x0 */ u16 ai;      // record of BeBadge_AI.bin (TinPinSlammer_Scene.ai) it plays with
    /* 0x2 */ u16 deck[7]; // pin ids, OTU_NO_PIN for an empty slot
} OtuMatchOpponent;        // Size: 0x10

/** One single-player match: one record of BeBadge_Single.bin. */
typedef struct {
    /* 0x00 */ u16              board;     // which of the ten boards
    /* 0x02 */ u16              timeLimit; // seconds
    /* 0x04 */ OtuMatchOpponent opponents[3];
} OtuMatch;                                // Size: 0x34

/**
 * @brief The Otosu scene object.
 *
 * Starts like MenuStateBase, but with a 0x30000-byte heap and a second task
 * pool. The tail holds the three data files the scene loads: the pin
 * parameters, the single-player matches and the CPU players' AI records.
 */
typedef struct {
    /* 0x00000 */ ResourceManager  resMgr;
    /* 0x11580 */ ResourceManager* prevResMgr;
    /* 0x11584 */ s32              spareDataType;
    /* 0x11588 */ s32              dataType;
    /* 0x1158C */ Heap             heap;
    /* 0x11598 */ u8               heapBuffer[0x30000];
    /* 0x41598 */ TaskPool         pool1;   // the fade and the screen-wide tasks
    /* 0x41618 */ TaskPool         pool2;   // the board's tasks
    /* 0x41698 */ BOOL             playing; // a round is running: pool 2 is processed
    /* 0x4169C */ s32              fadeTask;
    /* 0x416A0 */ u8               pad_416A0[4];
    /* 0x416A4 */ BOOL             textReady; // textRes and text are set up
    /* 0x416A8 */ u8               textRes[0xC];
    /* 0x416B4 */ TextObject       text;      // the debug editor's text layer
    /* 0x417C8 */ u8               pad_417C8[0x2FC];
    /* 0x41AC4 */ OtuStageDispatch stage;
    /* 0x41ADC */ BOOL             done;           // the stage sequence has ended; leave the overlay
    /* 0x41AE0 */ BOOL             linkError;      // leave through the connection-error screen
    /* 0x41AE4 */ u16              linkStatus;     // SystemStatusFlags.unk_06, latched at entry
    /* 0x41AE6 */ u8               pad_41AE6[2];
    /* 0x41AE8 */ s32              wirelessStatus; // SystemStatusFlags.unk_07, latched at entry
                                                   // NB: neither SystemStatusFlags bit is a wireless
                                                   // flag; they are only consumed by the wireless
                                                   // screens. See docs/overlays.md.
    /* 0x41AEC */ u8            pad_41AEC[4];
    /* 0x41AF0 */ BOOL          multiplayer;       // the wireless variant of the scene
    /* 0x41AF4 */ s32           unk_41AF4;
    /* 0x41AF8 */ u8            pad_41AF8[0x3E0];
    /* 0x41ED8 */ u8            parentBssid[6]; // the parent the wireless stage scans for
    /* 0x41EDE */ u8            pad_41EDE[2];
    /* 0x41EE0 */ s32           playerCount;
    /* 0x41EE4 */ BOOL          linkLost;                    // raised by the link's disconnect callback
    /* 0x41EE8 */ BOOL          linkTimeout;                 // raised when the wireless stage's timer runs out
    /* 0x41EEC */ BOOL          linkOpen;                    // the wireless stack is up and needs shutting down
    /* 0x41EF0 */ OtuBadgeParam badgeParams[OTU_NO_PIN + 1]; // BeBadge_Parm.bin
    /* 0x4404C */ u16           decks[4][7];                 // each player's pin ids
    /* 0x44084 */ OtuMatch      matches[44];                 // BeBadge_Single.bin
    /* 0x44974 */ u8            ai[7][0x22];                 // BeBadge_AI.bin
    /* 0x44A62 */ u8            pad_44A62[2];
    /* 0x44A64 */ s32           matchIndex;                  // the match being played or edited
    /* 0x44A68 */ s32           boardIndex;
} TinPinSlammer_Scene;                                       // Size: 0x44A6C

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
void func_ov039_020824a0(TinPinSlammer_Scene* scene);
void func_ov039_02082520(TinPinSlammer_Scene* scene);
void func_ov039_020825b0(TinPinSlammer_Scene* scene);
void func_ov039_02082724(TinPinSlammer_Scene* scene);
void func_ov039_0208273c(TinPinSlammer_Scene* scene);
/* Loads the scene's data files, empties the decks and starts the fade task. */
void func_ov039_020825e8(TinPinSlammer_Scene* scene);
void func_ov039_02082754(TinPinSlammer_Scene* scene);
void func_ov039_02082774(TinPinSlammer_Scene* scene);
void func_ov039_020827d0(TinPinSlammer_Scene* scene);

/* The wireless stage's steps (OtuScene_WirelessStage). */
void func_ov039_02087b04(TinPinSlammer_Scene* scene);
void func_ov039_02087ac4(void);
void func_ov039_02087ac8(void);

/* The editor stage's enter and exit (OtuScene_FirstStage). */
void func_ov039_02086728(TinPinSlammer_Scene* scene);
void func_ov039_020867d4(TinPinSlammer_Scene* scene);

/* The board stages' enter, exit and steps (OtuScene_WirelessBoard,
 * OtuScene_MenuStage and OtuScene_ResultStage). */
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
void func_ov039_02098a20(OtuStageDispatch* dispatch, Heap* heap);

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

/* Draws deck row `which` of the editor. */
void func_ov039_020842bc(TinPinSlammer_Scene* scene, s32 which);

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
void func_ov039_02082ae0(void);
void func_ov039_02082c50(void);
void func_ov039_02082e98(TinPinSlammer_Scene* scene);
void func_ov039_02082ff8(TinPinSlammer_Scene* scene);
void func_ov039_020831cc(TinPinSlammer_Scene* scene);
void func_ov039_020831d8(TinPinSlammer_Scene* scene);

extern const OtuStageHandler OtuScene_PlainHandlers[3];
extern const OtuStageHandler OtuScene_WirelessHandlers[3];

/*
 * Column tables, one per editor row layout: entry N is the character the
 * row's digit N is drawn at, which the blink blanks. Referenced by the build's
 * names, which keeps their pool words.
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

extern const s32 data_ov039_02098e48[2];  /* one two-digit group (020855e0)      */
extern const s32 data_ov039_02098e50[3];  /* the pin-count row's three columns  */
extern const s32 data_ov039_02098e5c[3];  /* three digits          (020851bc)     */
extern const s32 data_ov039_02098e78[4];  /* one d.ddd group       (02085770)     */
extern const s32 data_ov039_02098ea8[6];  /* two three-digit groups (020858e4)   */
extern const s32 data_ov039_02098ec0[6];  /* two three-digit groups (02085b30)   */
extern const s32 data_ov039_02098ed8[8];  /* four two-digit groups  (02085388)   */
extern const s32 data_ov039_02098f3c[10]; /* two five-digit groups  (02084f08)   */
extern const s32 data_ov039_02098f94[12]; /* six mixed groups       (02085e54)   */

/** One `d.ddd ` group of an editor row (func_ov039_02083a20). */
typedef struct {
    /* 0x0 */ char thousands;
    /* 0x1 */ char point;
    /* 0x2 */ char hundreds;
    /* 0x3 */ char tens;
    /* 0x4 */ char ones;
    /* 0x5 */ char space;
} OtuTextDigits; // Size: 0x6

/**
 * @brief One weight class: OtuBadgeParam.tuneIndex selects one of ten.
 *
 * SINGLE MENU 1 edits them as its rows 7 to 16 ("weight 1" to "weight 10").
 */
typedef struct {
    /* 0x0 */ s32 launch; // launch speed scale, func_ov039_0208a6f8
    /* 0x4 */ s32 accel;  // speed the badge accelerates to
    /* 0x8 */ s32 power;  // how hard it pushes on contact
    /* 0xC */ s32 weight; // how hard it is to push
} OtuPinTune;             // Size: 0x10

extern OtuPinTune data_ov039_0209a3dc[10];

/*
 * The tuning constants the debug editor's rows edit, by the build's names
 * (Q12.12 unless noted). They live in `.data` and are deliberately not
 * `const`: mwcc would otherwise keep them in registers across calls where the
 * target reloads them.
 *
 *   SINGLE MENU 1  gravity         0209a304
 *                  turn            0209a38c, 0209a320
 *                  panel           0209a31c, 0209a308, 0209a324
 *                  stun            0209a388
 *   SINGLE MENU 2  collision 1     0209a314, 0209a328, 0209a318
 *                  collision 2     0209a2fc, 0209a30c, 0209a310, 0209a300
 *                  special move    0209a398
 *                  badge rotation  0209a394, 0209a390 (integers)
 */
extern s32 data_ov039_0209a2fc;
extern s32 data_ov039_0209a300;
extern s32 data_ov039_0209a304;
extern s32 data_ov039_0209a308;
extern s32 data_ov039_0209a30c;
extern s32 data_ov039_0209a310;
extern s32 data_ov039_0209a314;
extern s32 data_ov039_0209a318;
extern s32 data_ov039_0209a31c;
extern s32 data_ov039_0209a320;
extern s32 data_ov039_0209a324;
extern s32 data_ov039_0209a328;

extern s32 data_ov039_0209a388;
extern s32 data_ov039_0209a38c;
extern s32 data_ov039_0209a390;
extern s32 data_ov039_0209a394;
extern s32 data_ov039_0209a398;

/*
 * The friction row: the rate a badge accumulates per frame on tile types 0, 4
 * and 3, and the rate used when something other than the board steers. The
 * stepper reaches the last three words by their own names.
 */
extern s32 data_ov039_0209a39c[4];
extern s32 data_ov039_0209a3a0;
extern s32 data_ov039_0209a3a4;
extern s32 data_ov039_0209a3a8;

/* Steps a Q12.12 value by one decimal place of its `d.ddd` form (OtuText.c). */
void func_ov039_02083af4(s32* value, s32 direction, s32 place);

/* Formats a Q12.12 value as `d.ddd` into a six-byte group. */
void func_ov039_02083a20(OtuTextDigits* out, s32 value);

void func_ov039_02084458(TinPinSlammer_Scene* scene, s32 slot, s32 step);
void func_ov039_020844f8(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_020845dc(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_0208462c(TinPinSlammer_Scene* scene);
void func_ov039_020846c4(TinPinSlammer_Scene* scene, s32 step);

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

/* Weight classes 10 and 11, after the ten the editor shows; the wireless
 * entry point swaps them. */
extern OtuPinTune data_ov039_0209a47c;
extern OtuPinTune data_ov039_0209a48c;

/* The cursor's glyphs: " " to erase it, "*" beside the selected row, ">"
 * while a digit is selected. */
extern const char data_ov039_0209a16c[];
extern const char data_ov039_0209a170[];
extern const char data_ov039_0209a174[];

/** One page's cursor in the debug editor. */
typedef struct {
    /* 0x0 */ s32 digit;      // the selected digit of the row; negative while moving between rows
    /* 0x4 */ s32 drawnDigit; // `digit` when the cursor was last drawn
    /* 0x8 */ s32 row;        // the selected row
    /* 0xC */ s32 drawnRow;   // `row` when the cursor was last drawn
} OtuEditCursor;              // Size: 0x10

/**
 * @brief The debug editor's stage block (OtuScene_FirstStage, 0x3C bytes).
 *
 * The editor has three pages: SINGLE MENU 1 edits the overlay's tuning
 * constants and decks, 2 the remaining constants, and 3 one pin's
 * OtuBadgeParam record. The d-pad moves the page's cursor and steps the
 * selected digit, L and R turn the page.
 */
typedef struct {
    /* 0x00 */ s32           page;
    /* 0x04 */ OtuEditCursor cursor[3]; // one per page
    /* 0x34 */ s32           blink;     // frame counter; bit 5 blanks the selected digit
    /* 0x38 */ s32           pinId;     // the pin page 3 edits
} OtuEditor;                            // Size: 0x3C

/* The editor stage (OtuCountdown.c). */
void func_ov039_02086060(TinPinSlammer_Scene* scene, s32 slot);
void func_ov039_02086174(TinPinSlammer_Scene* scene);
void func_ov039_02086808(TinPinSlammer_Scene* scene);
void func_ov039_02087acc(TinPinSlammer_Scene* scene);
void func_ov039_02087b74(TinPinSlammer_Scene* scene);
void func_ov039_02087c8c(TinPinSlammer_Scene* scene, s32 step);
void func_ov039_02087dc0(TinPinSlammer_Scene* scene, void* badge);

/** A point or vector, Q12.12. */
typedef struct {
    s32 x;
    s32 y;
} OtuPoint; // Size: 0x8

/**
 * @brief One keyframe of an affine scale animation: hold `frames`, then move on.
 */
typedef struct {
    /* 0x0 */ u16 frames;
    /* 0x4 */ s32 scaleX;
    /* 0x8 */ s32 scaleY;
} OtuScaleKey; // Size: 0xC

/**
 * @brief Steps an `OamAffineParam`'s scale pair through a table of `OtuScaleKey`.
 *
 * func_ov039_02087ba0 starts it at key 0 and func_ov039_02087bf8 publishes the
 * current key's scales each frame, advancing when `hold` runs out and wrapping
 * to key 0 at `count`.
 */
typedef struct {
    /* 0x0 */ const OtuScaleKey* keys;
    /* 0x4 */ u16                index;
    /* 0x6 */ u16                count;
    /* 0x8 */ u16                hold;
} OtuScaleAnim; // Size: 0xC

void func_ov039_02087ba0(OtuScaleAnim* anim, const OtuScaleKey* keys, u16 count, OamAffineParam* affine);
void func_ov039_02087bf8(OtuScaleAnim* anim, OamAffineParam* affine);

/* The distance between two points, via the hardware square root. */
s32 func_ov039_02098ca8(OtuPoint* a, OtuPoint* b);

/* Packs a layer and a Q12.12 y into an OAM sort key (OtuFieldAccess.c). */
s32 func_ov039_02088400(s32 layer, s32 y, s32 z);

/* The tasks' sprite frame-info callbacks (`SpriteAnimation.frameInfoCallback`). */
SpriteFrameInfo* func_ov039_0208d2f8(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_0208f134(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_0208f440(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_0208fa90(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02090208(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_020902cc(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02091118(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_020916c4(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_020923c8(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02094ae8(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02094ff4(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_0209549c(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_020958a8(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02095dec(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02096280(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_0209659c(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02096660(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02096e7c(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02097398(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_020977d0(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02097ae0(Sprite* sprite, s32 arg, s32 mode);
SpriteFrameInfo* func_ov039_02097ff4(Sprite* sprite, s32 arg, s32 mode);

/* The stage sequencer (OtuHammerSpawn.c). */
void* func_ov039_02098b70(OtuStageDispatch* dispatch);

void                 func_ov039_02098a60(OtuStageDispatch* dispatch, void* scene);
void                 func_ov039_02098acc(OtuStageDispatch* dispatch, void* scene);
void                 func_ov039_02098af4(OtuStageDispatch* dispatch, void* scene);
s32                  func_ov039_02098b44(OtuStageDispatch* dispatch);
const OtuSceneStage* func_ov039_02098b68(OtuStageDispatch* dispatch);
void                 func_ov039_02098a40(OtuStageDispatch* dispatch, const OtuSceneStage* next);

/** The stage the scene enters when the wireless link needs attention. */
extern OtuSceneStage OtuScene_WirelessStage;

/** The debug editor (OtuEditor). */
extern OtuSceneStage OtuScene_FirstStage;

/** The single-player board; its block is an OtuBoardStage. */
extern OtuSceneStage OtuScene_WirelessBoard;

/** The wireless menu and result stages; their block is an OtuBoardStage. */
extern OtuSceneStage OtuScene_MenuStage;
extern OtuSceneStage OtuScene_ResultStage;

/**
 * @brief The overlay's three debug-sequence names, all "Seq_Otosu()".
 *
 * The scene entry points index this table rather than using the string
 * directly, and the three variants pick different slots (the wireless entry
 * uses index 2).
 */
extern const char* const OtuScene_SequenceNames[3];

/** The overlay the scene hands over to when it ends (the Otosu menus). */
#define OTU_OVERLAY_ID 2

#define OTU_HEAP(scene)        (&(scene)->heap)
#define OTU_HEAP_BUFFER(scene) ((void*)&(scene)->heapBuffer)
#define OTU_POOL1(scene)       (&(scene)->pool1)
#define OTU_POOL2(scene)       (&(scene)->pool2)
#define OTU_STAGE(scene)       (&(scene)->stage)
#define OTU_TEXT(scene)        (&(scene)->text)

/** Scene entry points. */
void func_ov039_02082978(void);

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

/** A keyframe of the 4-byte cursor: hold `duration` frames at `value`. */
typedef struct {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 value;
} OtuFrame4; // Size: 0x4

/**
 * @brief A cursor over OtuFrame4 keys, one entry per `duration` frames.
 *
 * func_ov039_0208ffac starts it at entry 0 and func_ov039_0208ffd8 advances it.
 */
typedef struct {
    /* 0x00 */ OtuFrame4* table;
    /* 0x04 */ s16        index;
    /* 0x06 */ s16        count;
    /* 0x08 */ s16        framesLeft;
} OtuCursor; // Size: 0xA

/* The overlay's 2D Q12.12 vector helpers (OtuVecOps.c). */

void func_ov039_02098bb0(OtuPoint* a, OtuPoint* b, OtuPoint* out);
void func_ov039_02098bd4(s32 angle, OtuPoint* a, OtuPoint* out);
void func_ov039_02098c00(s32 angle, OtuPoint* a, OtuPoint* b, OtuPoint* out);
s32  func_ov039_02098c40(OtuPoint* a, OtuPoint* b);      // dot product
s32  func_ov039_02098c70(OtuPoint* a, OtuPoint* b);      // cross product's z
s32  func_ov039_02098d10(OtuPoint* v);                   // Vec_Magnitude over 2D
void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest); // Vec_Normalize over 2D

#endif                                                   // TIN_PIN_SLAMMER_H
