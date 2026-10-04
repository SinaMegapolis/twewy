#ifndef OTU_FIELDACCESS_SHARED_H
#define OTU_FIELDACCESS_SHARED_H

/*
 * Private declarations for the ov039 region files (OtuFieldAccess.c,
 * OtuBoard.c, ...). dsd claims one contiguous `.text` range per source file,
 * so the overlay is split by address; everything the regions share --
 * typedefs, externs, file-scope macros and function prototypes -- lives
 * here, in the order the original single translation unit declared it.
 * See docs/overlays.md for the layout.
 */

#include "CriSndMgr.h"
#include "Debug/Sugata/TinPinSlammer.h"
#include "EasyFade.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Save.h"
#include "SndMgr.h"
#include "SpriteMgr.h"
#include "common_data.h"
#include <nitro/fx/fx_atan.h>
#include <nitro/fx/fx_division.h>
#include <nitro/mi/cpumem.h>

/*
 * 0x02093000 - 0x02094000: the pinball simulation's own task family.
 *
 * Where OtuFieldAccess.c is the overlay's field accessors, this band is the
 * machinery those accessors are called from: four EasyTask tasks, each built
 * out of the same four-stage lifecycle (init / update / render / cleanup, wired
 * through a `TaskStages` table), and each owning a handful of `Sprite`s.
 *
 * All four tasks are built the same way, and the constants are what make them
 * tellable apart. The task names are readable in the `.rodata` handles:
 *
 *   Tsk_OtosuGame_ovbg          data_ov039_020999b4, 0x850 bytes
 *       init 02092f88  update 020933c0  render 020933f0  cleanup 020933f8
 *       Four BG layers (char + screen + palette each) and one animated
 *       palette source at +0x840. No sprites at all.
 *
 *   Tsk_OtosuGame_badgeradar    data_ov039_02099a60, 0x60 bytes
 *       init 02093668  update 020936b8  render 020937a0  cleanup 020937dc
 *       One sprite that tracks a child task's +0x120/+0x124 position and
 *       renders it as a Q12.12 -> pixel conversion.
 *
 *   Tsk_OtosuGame_badgescounter data_ov039_02099ab0, 0x198 bytes
 *       init 02093b08  update 02093b98  render 02093bc0  cleanup 02093c44
 *       Six sprites: one conditional pair at +0x04/+0x44 and four digit
 *       sprites at +0x84..+0x184, gated by a bitmask at +0x194.
 *
 *   Tsk_OtosuGame_countdown     data_ov039_02099b50, 0xD0 bytes
 *       init 02093f70  update 02093fcc  render 0209411c  cleanup 02094158
 *       Three digit sprites and a countdown at +0xC4 measured in 1/60th
 *       ticks (`arg1 * 0x3C`), with 0x4B0 (20 seconds) as its alarm point.
 *
 * Four findings worth stating up front, because they are what makes the rest
 * readable:
 *
 *   1. **The load wrappers are the same body five times over.** 020935d4,
 *      0209392c, 020939b0, 02093a30 and 02093ee4 each copy a 0x2C-byte
 *      `SpriteAnimation` template out of `.rodata` onto the stack, patch three
 *      fields, and call `_Sprite_Load`. What differs between them is only
 *      which template and which patch -- so they are written out five times
 *      rather than shared. `-inline noauto` means a shared `static` would
 *      compile to a real `bl` and cost every one of them its body.
 *
 *   2. **The sprite-cell builders are byte-identical here, not merely
 *      similar.** 0209352c, 02093884 and 02093e3c have the same body as each
 *      other and differ from the twenty-two in OtuFieldAccess.c in exactly one
 *      instruction: they end with the constant depth key 3 where the twenty-two
 *      call the packer. Three copies, zero constants varied. They are also
 *      `SpriteFrameInfoCallback`s, so their second argument is the callback arg
 *      and their *third* is the mode -- see the comment above 0209352c.
 *
 *   3. **Two different clocks, one unit.** The background task's palette
 *      animation steps one entry per call with a count of 0x12; the countdown
 *      task's clock is in 1/60th ticks and 0x4B0 is twenty of them. Nothing
 *      in this band ties the two together, but the 0x12 and the 0x3C are both
 *      per-frame quantities and 02092f88's palette source and 02093fcc's
 *      countdown both run once per update.
 *
 *   4. **0x130 is a pin-tray value, not a character.** 02093d18 scans for it as
 *      an "empty slot" sentinel and 02093d68 writes digit glyphs as 0x21 + n --
 *      ASCII digits minus 0x0F. Both are working in the tray's value space, so
 *      the counter task's "score" and the pin tray are the same numbering, and
 *      the animation indices in `data_ov039_02099aa8` (also 0x21) are in it
 *      too.
 */

/* Overlays' own helpers, not yet decompiled; declared here rather than in the
 * shared header because five band files are compiled as one translation unit. */
extern s32 func_ov039_02098dbc(void* anim);

extern s32 func_ov039_02098d7c(void* anim, s32 base, const void* table, s16 count);

extern s32 func_ov039_0208ef4c(void* task, u32 arg1);

/* The 0x10-byte animated-palette object at the background task's +0x840. */
typedef struct {
    /* 0x00 */ const u16* table; // compared against the table pointer to see if it is set
    /* 0x04 */ s32        count;
    /* 0x08 */ s16        timer;
    /* 0x0A */ u8         pad_0A[2];
    /* 0x0C */ u16*       base;
} OtuPaletteAnim;

// Size: 0x10

/** The four-layer background task's state block, 0x850 bytes. */
typedef struct {
    /* 0x000 */ u8               pad_000[0x008];
    /* 0x008 */ Data*            fileData;
    /* 0x00C */ PaletteResource* palettes[4];
    /* 0x01C */ BgResource*      chars[4];
    /* 0x02C */ BgResource*      screens[4];
    /* 0x03C */ u8               tilemap[0x804];
    /* 0x840 */ OtuPaletteAnim   paletteAnim;
} OtuBgTaskData;

/** The badge-radar task's state block, 0x60 bytes: one sprite and its state. */
typedef struct {
    /* 0x00 */ u32    unk_00;
    /* 0x04 */ Sprite sprite;
    /* 0x44 */ s32    x;        // Q12.12, converted to pixels by the render stage
    /* 0x48 */ s32    y;        // Q12.12
    /* 0x4C */ u32    targetId; // task id of the child being tracked
    /* 0x50 */ s32    animBias; // added to the sprite's animation index
    /* 0x54 */ u8*    table;
    /* 0x58 */ s32    linked;
    /* 0x5C */ s32    animBase; // 1 selects animation 5, anything else animation 1
} OtuRadarData;

// Size: 0x60

/** The badge-counter task's state block, 0x198 bytes: six sprites. */
typedef struct {
    /* 0x000 */ u32    unk_000;   // low nibble becomes each sprite's dataType
    /* 0x004 */ Sprite spriteA;   // only present when hasSpriteA is set
    /* 0x044 */ Sprite spriteB;
    /* 0x084 */ Sprite digits[4]; // gated by the bitmask at +0x194
    /* 0x184 */ u32    sourceId;  // task id read by the update stage
    /* 0x188 */ s32    unk_188;   // which menu: 7 animations and a y offset per value
    /* 0x18C */ s32    resolved;  // sourceId resolved in the pool
    /* 0x190 */ s32    hasSpriteA;
    /* 0x194 */ u16    visible;   // bit N is digits[N]
    /* 0x196 */ u8     pad_196[2];
} OtuCounterData;

// Size: 0x198

/** The countdown task's state block, 0xD0 bytes: three digit sprites. */
typedef struct {
    /* 0x000 */ u32    unk_000;   // low nibble becomes each sprite's dataType
    /* 0x004 */ Sprite digits[3];
    /* 0x0C4 */ s32    countdown; // 1/60th ticks remaining
    /* 0x0C8 */ s32    unk_0C8;   // "draw the digits"; set once by the init stage
    /* 0x0CC */ s32    alarmed;   // one-frame pulse at the 0x4B0 crossing
} OtuCountdownData;

// Size: 0xD0

/*
 * The four-word argument block the init stages read, except that the radar and
 * counter tasks carry a fifth. 02093668 reads +0x00..+0x10 (five words);
 * 02093b08 and 02093f70 read only +0x00..+0x0C.  Left as offsets because the
 * fields' meanings differ per task.
 */
typedef struct {
    s32 unk_00;
    s32 unk_04;
    s32 unk_08;
    s32 unk_0C;
    s32 unk_10;
} OtuInitArgs;

// Size: 0x14

/* The animation templates the load wrappers copy out of `.rodata`. */
extern const SpriteAnimation data_ov039_02099a7c;

extern const SpriteAnimation data_ov039_02099acc;

extern const SpriteAnimation data_ov039_02099af8;

extern const SpriteAnimation data_ov039_02099b24;

extern const SpriteAnimation data_ov039_02099b6c;

/** Four `s16` offsets: the x positions of the counter task's digit sprites. */
extern const u16 data_ov039_02099aa8[4];

/* The four-stage tables, and the `TaskHandle`s the create wrappers pass. */
extern const TaskStages data_ov039_020999c0;

extern const TaskStages data_ov039_02099a6c;

extern const TaskStages data_ov039_02099abc;

extern const TaskStages data_ov039_02099b5c;

extern const TaskHandle data_ov039_020999b4;

extern const TaskHandle data_ov039_02099a60;

extern const TaskHandle data_ov039_02099ab0;

extern const TaskHandle data_ov039_02099b50;

/** The table 020934e0 refuses to re-point the animated palette at twice. */
extern const u16 data_ov039_02099a18[1];

/**
 * @brief Where a badge was spawned: one flag byte and its starting tile.
 *
 * Read through +0xE8. Bit 0 of `flags` is copied straight into bit 0 of the
 * badge's packed state word by 0x0208acc0, which is the only thing that ties
 * the two objects together; `tileX`/`tileY` are the tile the badge belongs to
 * and are shifted up by twelve to become Q12.12.
 */
typedef struct {
    /* 0x0 */ u8 flags; // bit 0 is mirrored into the badge's stateFlags
    /* 0x1 */ u8 pad_01;
    /* 0x2 */ u8 tileX;
    /* 0x3 */ u8 tileY;
} OtuHomePos;

// Size: 0x4

/**
 * @brief One pin's behaviour record, 0x1C-byte stride, reached through +0x170.
 *
 * Indexed by the tray slot at +0x16C, so the stride is a `mla` against 0x1C
 * rather than an array subscript the compiler could fold away:
 * `ldr r0, [r4, #0x16c] / ldr r3, [r4, #0x170] / ldrh r2, [r0, #0x0] /
 * mov r0, #0x1c / mla r0, r2, r0, r3`.
 *
 * This was two structs, this one and a second called `OtuPinRow`, and they were
 * never two records. The target shows one array: the four leading bytes at +0x00
 * are read with `ldrb` by 0208d5dc as tile[0..3], and +0x04 is read with `ldrb`
 * by both 0208af6c and 0208e130 and used as `ldr rX, [rX, r2, lsl #0x4]` -- an
 * index into a 0x10-stride table. data_ov039_0209a3e0, _3e4 and _3e8 are three
 * *fields* of one such array, not three tables, which is what let the two bands
 * read +0x04 as an unrelated `speedIndex` and `weight`. One type covers both
 * now.
 *
 * `tile[]` are the frame budgets the badge is spawned with, in the order
 * 6, 8, 7, 9 -- see OtuTimers, which is what they are copied into.
 */
typedef struct {
    /* 0x00 */ u8  tile[4];   // copied into OtuTimers as phases 6, 8, 7, 9
    /* 0x04 */ u8  tuneIndex; // index into data_ov039_0209a3e0, 0x10 stride
    /* 0x05 */ u8  pad_05;
    /* 0x06 */ s16 friction;  // Q12.12, scaled by unk_144 / 3 into a heading offset
    /* 0x08 */ u16 animX;
    /* 0x0A */ u16 animY;
    /* 0x0C */ u8  pad_0C[0x04];
    /* 0x10 */ s16 curveA;
    /* 0x12 */ s16 curveB;
    /* 0x14 */ u16 curveC;
    /* 0x16 */ u16 curveD;
    /* 0x18 */ u8  pad_18[0x04];
} OtuBadgeSlot;

// Size: 0x1C

/**
 * @brief The badge's six 16-bit countdown slots, 0x28 bytes at +0x178.
 *
 * Every one is read-modify-written with a `ldrsh`/`sub 1`/`strh` triple, which
 * is what identifies them as counters that are allowed to go negative rather
 * than as flags, and each is the one and only guard on its phase's entry
 * setter.
 *
 * The first four are the frame budgets for phases 6 to 9, one each, and
 * func_ov039_0208b94c/0208b9b4/0208ba34/0208ba70 -- the four phase entry
 * setters -- each decrement the one belonging to the phase it enters. They are
 * *seeded* from the pin-slot record rather than computed: 0208d5dc copies
 * slot->tile[0..3] into them at spawn, and note the order is not the phase
 * order, it is tile[0]->phase 6, tile[1]->phase 8, tile[2]->phase 7,
 * tile[3]->phase 9. That is why an earlier reading of these four as
 * `tile0..tile3` was half right -- correct about where the values come from,
 * wrong about what they are for. The last two are the trail cursor and its
 * timer, unrelated to the phases.
 *
 * The task's own code reaches them through a `+0x100` base register and then a
 * small displacement -- `add rX, self, #0x100` followed by `ldrsh [rX, #0x78]`
 * -- rather than folding 0x178 into the load. Keeping them in a struct of their
 * own at 0x178 preserves that split, because the displacement is too large for
 * the short form to be what this build emits.
 */
typedef struct {
    /* 0x00 */ s16 trackFrames; // self +0x178: phase 6
    /* 0x02 */ s16 bounceTimer; // self +0x17A: phase 8
    /* 0x04 */ s16 arcFrames;   // self +0x17C: phase 7
    /* 0x06 */ s16 spinFrames;  // self +0x17E: phase 9
    /* 0x08 */ u8  pad_08[0x1C];
    /* 0x24 */ s16 trailIndex;  // self +0x19C: cursor into the trail id table
    /* 0x26 */ s16 trailTimer;  // self +0x19E: frames between trail marks
} OtuTimers;

// Size: 0x28

/**
 * @brief "Tsk_OtosuGame_badge": the badge task, 0x25C bytes.
 *
 * This is the same object the header models as `OtuPinTask`, which stops at
 * 0x174; that type exists for the five nearest-child queries and only needs
 * +0xF8/+0x120/+0x124/+0x148/+0x16C, so it is left alone. Every offset below
 * 0x174 agrees with it -- `kind`, `x`, `y`, `alive`, `pinID` -- and this type
 * continues past it.
 *
 * It is also the same object band B3 models as `OtuBadgeState`, seen from the
 * AI's side and under its own field names. The two do not share every name, so
 * they are kept apart rather than merged; the handle above ties them together.
 *
 * Positions and velocities are Q12.12, the overlay's usual scale, and three
 * Sprites are embedded rather than pointed at (see the header note).
 */
typedef struct OtuBadge {
    /* 0x000 */ TinPinSlammer_Scene* scene;
    /* 0x004 */ s32                  dataType; // copied into SpriteAnimation.dataType
    /* 0x008 */ TaskPool*            pool;     // saved by the init stage
    /* 0x00C */ Sprite               spriteA;
    /* 0x04C */ Sprite               spriteB;
    /* 0x08C */ Sprite               spriteC;
    /* 0x0CC */ s32                  unk_0CC; // travel rescaled by 1/4096
    /* 0x0D0 */ s32                  unk_0D0; // starts at 0x1000
    /* 0x0D4 */ s32                  unk_0D4; // starts at 0x1000
    /* 0x0D8 */ u16                  unk_0D8;
    /* 0x0DA */ u16                  unk_0DA;
    /* 0x0DC */ s32                  unk_0DC; // raised to 1 by init and by update
    /* 0x0E0 */ s32                  index;   // which badge this is; passed to the pool resolvers
    /* 0x0E4 */ u8*                  board;   // see the note at the top of the file
    /* 0x0E8 */ OtuHomePos*          home;    // non-NULL switches on the variant path
    /* 0x0EC */ u16                  unk_0EC; // home->tileX
    /* 0x0EE */ u16                  contactFlags;
    /* 0x0F0 */ u16                  stateFlags;
    /* 0x0F4 */ s32                  step;    // sub-phase within `kind`
    /* 0x0F8 */ s32                  phase;   // 1..9; see the note below before renaming
    /* 0x0FC */ s32                  subKind; // gates kind 8 in the render stage
    /* 0x100 */ s32                  frameBudget;
    /* 0x104 */ s32                  lastTileType;
    /* 0x108 */ s32                  lastCellX;
    /* 0x10C */ s32                  lastCellY;
    /* 0x110 */ OtuPoint             origin;  // the render subtracts these
    /* 0x118 */ OtuPoint             unk_118;
    /* 0x120 */ OtuPoint             pos;     // Q12.12
    /* 0x128 */ s32                  unk_128; // added into pos.y by the render
    /* 0x12C */ OtuPoint             vel;
    /* 0x134 */ s32                  unk_134;
    /* 0x138 */ OtuPoint             dir;      // vel re-normalised in here when vel is non-zero
    /* 0x140 */ s32                  travel;   // integrated from vel each update
    /* 0x144 */ s32                  velMag;   // decays by a fixed step each update
    /* 0x148 */ s32                  alive;
    /* 0x14C */ OtuPoint             aimStart; // where the current aim began
    /* 0x154 */ OtuPoint             aimCur;   // where it is aiming now
    /* 0x15C */ s32                  unk_15C;
    /* 0x160 */ s32                  unk_160;
    /* 0x164 */ s32                  unk_164;
    /* 0x168 */ s32                  flags;     // init 0xA2; bit 1 blocks the render
    /* 0x16C */ u16*                 pinID;     // a tray slot; 0x130 means "no pin"
    /* 0x170 */ OtuBadgeSlot*        slots;     // per-pin record, 0x1C stride, indexed by *pinID
    /* 0x174 */ u16*                 chanceTbl; // one 0x10000 chance per AI
    /* 0x178 */ OtuTimers            timers;    // 0x28 bytes, so it also covers 0x19C/0x19E
    /* 0x1A0 */ u16                  unk_1A0;
    /* 0x1A4 */ s32                  unk_1A4;   // raised to 1 by the init stage
    /* 0x1A8 */ s32                  hasLabel;  // gates two of the children
    /* 0x1AC */ s32                  unk_1AC;
    /* 0x1B0 */ struct OtuBadge*     partner;   // the pin this one last touched
    /* 0x1B4 */ s32                  curAI;     // 0x11 = parked
    /* 0x1B8 */ s32                  unk_1B8;   // counts down, then re-raises curAI
    /* 0x1BC */ struct OtuBadge*     chaseTarget;
    /* 0x1C0 */ OtuPoint             anchorPt;  // where the current AI sent us
    /* 0x1C8 */ s32                  unk_1C8;
    /* 0x1CC */ s32                  unk_1CC;   // frames spent aiming; 0x1E = commit
    /* 0x1D0 */ s32                  mode;      // 0x10 once the badge is committed
    /* 0x1D4 */ s32                  child0;
    /* 0x1D8 */ s32                  child1;
    /* 0x1DC */ s32                  child2;
    /* 0x1E0 */ s32                  child3;
    /* 0x1E4 */ u32                  taskId1; // the arc task
    /* 0x1E8 */ u32                  taskId2; // the track task
    /* 0x1EC */ u32                  taskId3; // the spin task
    /* 0x1F0 */ s32                  child7;
    /* 0x1F4 */ s32                  child8;
    /* 0x1F8 */ u32                  trailId[12]; // see the note below
    /* 0x228 */ s32                  pairIds[2];
    /* 0x230 */ s32                  labelTask;
    /* 0x234 */ s32                  child9;
    /* 0x238 */ s32                  groupIds[8];
    /* 0x258 */ u32                  taskId; // the badge's own sprite task
} OtuBadge;

/**
 * @brief The same 0x25C task, as band B3 saw it.
 *
 * This used to be a second, independently written struct over the same bytes,
 * which is what forced the `(OtuBadgeState*)` casts scattered through
 * OtuPinTray.c. The two never disagreed on an offset -- only on names, and
 * where they differed the better name won -- so it is now an alias.
 *
 * `OtuBadgeState` is kept as a name because it still documents *which band*
 * reads the AI half of the struct, which the pool queries and the render do not.
 */
typedef OtuBadge OtuBadgeState;

/* The one field this merge can get wrong silently: OtuTimers is 0x28 bytes, so it
 * swallows 0x19C/0x19E, and declaring those two again would push everything after
 * 0x178 up by four without a single diagnostic. The task really is 0x25C bytes,
 * and three separate views previously agreed on that, so pin it. */
typedef char OtuBadge_SizeMustBe_0x25C[(sizeof(OtuBadge) == 0x25C) ? 1 : -1];

/**
 * @brief 0x1F8 was `children[12]` in OtuBadge and `trailId[12]` in
 *        OtuBadgeState. It is not two arrays, and it is not a conflation --
 *        checked, because the two names did suggest an overlap.
 *
 * The suspicion was a 4-versus-12 mismatch: OtuMeters.c walks a
 * `children[i]` with `for (i = 0; i < 4; i++)`, and 0208db44 walks 0x1F8 with
 * `cmp r6, #0xc`. That comparison is a red herring. The 4-iteration loops are
 * in functions whose parameter is an `OtuHammer*`, not this struct -- 02090d90
 * does `ldr r4, [r1, #0x18]` for task->data and then `ldr r1, [r0, #0x128]`,
 * and +0x128 is `OtuHammer.children[4]`, a different field on a different task.
 * Two structs in this overlay both have a `children` array; they are unrelated.
 *
 * Every read of *this* field is twelve wide. The badge's teardown walks 0x1F8
 * with `cmp r6, #0xc` and nothing else in the field is ever indexed, and
 * OtuBadgeState places a trail mark through `trailId[timers.trailIndex]` and
 * wraps the cursor at `>= 0xC`. So twelve is the width, `trailId` is the role,
 * and `children` was simply a name with no use behind it -- the badge view
 * invented it and no code ever read it that way.
 */

/**
 * @brief 0xF8 was called `kind` by two of the three badge views, and it is
 *        wrong. It is a phase counter.
 *
 * Checked against the target rather than argued from the names. The predicate
 * 0208e984 is `ldr r1, [r0, #0xf8] / cmp r1, #0x8 / moveq r0, #0x1` -- so the
 * pool queries really do test +0xF8 against 6, 7, 8 and 9. But the phase entry
 * setters *write* that same word: 0208b94c is `mov r0, #0x6 / str r0,
 * [r4, #0xf8]` and 0208ba70 does the same with #0x9, and the two in between use
 * 7 and 8. Each of the four also decrements one field of OtuTimers and has its
 * own doc comment saying "Enters phase N".
 *
 * So one word is both written as a monotone 6,7,8,9 and tested against those
 * same values. It cannot be a per-badge type tag -- a type is assigned once, and
 * this is assigned four times by four different functions in sequence. What the
 * predicates actually select is "the nearest child currently in phase N", which
 * is a sensible thing for this overlay to ask: phases 6 to 9 are the track, arc,
 * bounce and spin behaviours, i.e. the ones where the pin is reachable.
 *
 * `OtuBadgeState` had it right as `phase`; the other two bands only ever saw a
 * comparison constant and reached for `kind`.
 */

/**
 * @brief The same 0x25C task again, as the pool queries see it.
 *
 * A third view of the identical object, previously trimmed to 0x174 bytes with
 * padding up to every offset. OtuPinAccessors.c already documented the
 * relationship ("Every offset OtuPinTask does model agrees with OtuBadge, so
 * the cast is always safe"), so the trimmed view bought nothing but the padding
 * and the `(OtuPinTask*)` casts.
 *
 * The name is kept because it is still the useful annotation: it marks the
 * handful of fields the five nearest-child queries read (index, kind, pos,
 * alive, pinID, and the sub-word pair) as distinct from the AI's own fields.
 */
typedef OtuBadge OtuPinTask;

/* Copies a task's +0x120/+0x124 pair into `out` (OtuTaskPick.c). Moved here
 * from include/Debug/Sugata/TinPinSlammer.h, which can no longer see the badge
 * struct; the two names below are aliases of it, so the signatures are
 * unchanged. */
void func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out);

/* The five child filters: true when the child is a candidate worth scoring. */
s32 func_ov039_0208e984(OtuPinTask* task); // kind == 8
s32 func_ov039_0208e998(OtuPinTask* task); // kind == 7
s32 func_ov039_0208e9d0(OtuPinTask* task); // kind == 6
s32 func_ov039_0208ee84(OtuPinTask* task); // alive
s32 func_ov039_0208efb0(OtuPinTask* task, s32 which);
/**
 * @brief The palette block inside a loaded `Data`'s buffer.
 *
 * The overlay addresses this the same way in 020934e0 and 02093fcc, and the
 * arithmetic is `buffer + 0x20 + the word at buffer + 0x48` -- i.e. a table of
 * sub-resource offsets starting 0x20 into the buffer, indexed by one of them.
 * Which sub-resource it is (palette, char or screen) depends on the caller.
 */
static inline void* OtuPaletteSource(Data* file) {
    u8* buffer = (u8*)file->buffer + 0x20;

    return buffer + *(u32*)(buffer + 0x28);
}

/* The five sprite-load wrappers. They call each other, so they are declared
 * here rather than in the order they appear. */
void func_ov039_020935d4(OtuRadarData* data, Sprite* sprite);

void func_ov039_0209392c(OtuCounterData* data, Sprite* sprite);

void func_ov039_020939b0(OtuCounterData* data, Sprite* sprite);

void func_ov039_02093a30(OtuCounterData* data, Sprite* sprite, s32 index);

void func_ov039_02093ee4(OtuCountdownData* data, Sprite* sprite, s32 index);

/**
 * @file OtuEntryTasks.c
 *
 * 0x020950b8 - 0x02095fe4: the four sprite tasks the board stage spawns.
 *
 * The `.rodata` tables a few hundred bytes above this band give the grouping
 * away. Each of the four is a `TaskHandle` (name, entry, data size) followed
 * by a `TaskStages` array of four pointers, and each `TaskStages` array lists
 * exactly four functions in this band or immediately around it:
 *
 *   0x02099cac  "Tsk_OtosuGame_slash"  0x70 bytes, entry 02095420
 *       init 02095144  update 02095194  render 020953c4  destroy 0209540c
 *   0x02099cf4  "Tsk_OtosuGame_track"  0x70 bytes, entry 02095708
 *       init 020955f4  update 02095638  render 020956a4  destroy 020956f0
 *   0x02099d44  "Tsk_OtosuGame_point"  0x174 bytes, entry 02095c58
 *       init 02095a18  update 02095a78  render 02095b6c  destroy 02095c2c
 *   0x02099d8c  "Tsk_OtosuGame_entry"  0xC8 bytes, entry 020960dc
 *       init 02095f18  update 02095f58  render 02095fe4  destroy 020960bc
 *
 * So the four `*_RunTask`-shaped routines are the task entries, and each is
 * byte-identical apart from which `TaskStages` object it copies to the stack.
 * Everything else here is one of the four stages, or a helper only one of them
 * calls -- and each task loads its sprite(s) through its own copy of the
 * shared loader shape, with its own `.rodata` `SpriteAnimation` template.
 *
 * The `data_*` names below are the build's own, and the two that are named
 * rather than numeric (`Tsk_OtosuGame_*` and `Seq_Otosu`) are read off the strings
 * at 0x0209a7e0 onward.
 */

/* ------------------------------------------------------------------ */
/* Declarations this TU does not otherwise have.                      */
/*                                                                     */
/* Everything here lives outside this band and outside OtuFieldAccess's */
/* own bodies, so nothing declares it for us.  They go here rather    */
/* than in the shared header, which five agents share.                 */
/* ------------------------------------------------------------------ */

/* The +0x110/+0x114, +0x118/+0x11C and +0x120/+0x124 pair copy-outs. */
void func_ov039_0208e85c(void* task, OtuPoint* out);

void func_ov039_0208e87c(void* task, OtuPoint* out);

/* The overlay's vector unit's four helpers, 0x02098b8c - 0x02098d10. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out);

s32 func_ov039_02098d10(OtuPoint* v);

/* The pin task's own three queries, 0x0208eed0 - 0x0208ef38. */
s32 func_ov039_0208eed0(OtuPinTask* pin);

void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b);

s32 func_ov039_0208ef38(OtuPinTask* pin);

/* The board stage's per-sprite animation stepper, 0x02087bf8. */
void func_ov039_02087bf8(void* anim, void* limit);

/* The base module's fixed-point divide.  No prototype exists in this repo,
 * but the two call sites here both pass a quotient-remainder pair. */
s32 _s32_div_f(s32 a, s32 b);

/* Every stage this band names in a `TaskStages` table.  The tables are written
 * before the stages themselves, so the names have to be in scope first.
 * func_ov039_02095194 and func_ov039_02095a78 are declared but not yet
 * defined here; see the notes on each. */
s32 func_ov039_02095144(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095194(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020953c4(TaskPool* pool, Task* task, void* args);

s32 func_ov039_0209540c(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020955f4(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095638(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020956a4(TaskPool* pool, Task* task, void* args);

s32 func_ov039_020956f0(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095a18(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095a78(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095b6c(TaskPool* pool, Task* task, void* args);

s32 func_ov039_02095c2c(TaskPool* pool, Task* task, void* args);

/* The four `SpriteAnimation` templates and the four task descriptors, read by
 * address.  These are in the overlay's gap-filled `.rodata`, so they are
 * declared rather than defined. */
extern TaskHandle data_ov039_02099cac;

extern TaskHandle data_ov039_02099cf4;

extern TaskHandle data_ov039_02099d44;

/* data_ov039_02099d8c is declared const by band 4. */

extern const TaskStages data_ov039_02099cb8;

extern const TaskStages data_ov039_02099d00;

extern const TaskStages data_ov039_02099d50;

extern SpriteAnimation data_ov039_02099cc8;

extern SpriteAnimation data_ov039_02099d10;

extern SpriteAnimation data_ov039_02099d60;

extern SpriteAnimation data_ov039_02099da8;

/* Four `s16` values, 12, 11, 1, 1 -- one per sprite of the point task. */
extern const s16 data_ov039_02099d3c[4];

/* The base module's shared `s16` slope table, indexed by `angle >> 4`. */
extern s32 data_0205e4e0[];

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief One of the four slots in "Tsk_OtosuGame_point", 0x14 bytes.
 *
 * A 0x40-stride array of these at +0x120, one per sprite. `live` gates the
 * whole slot; `delay` counts the sprite's own animation down; `offsetX` and
 * `accum` are the two components of the position the render adds to the
 * anchor difference; `phase` is the step the update integrates into `accum`.
 */
typedef struct {
    /* 0x00 */ s32 live;
    /* 0x04 */ s32 delay;
    /* 0x08 */ s32 offsetX;
    /* 0x0C */ s32 accum;
    /* 0x10 */ s32 phase;
} OtuPointSlot;

// Size: 0x14

/**
 * @brief "Tsk_OtosuGame_slash", 0x70 bytes.
 *
 * One `Sprite` at the base. +0x48/+0x4C is the pair the loader subtracts, and
 * +0x58/+0x5C the pair it subtracts *it* from -- so the sprite is drawn at the
 * difference between them, which the render recomputes as
 * `(+0x58 - +0x50) >> 12` against the pair the update refreshes.
 */
typedef struct {
    /* 0x00 */ Sprite sprite;
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s16    unk_4C;
    /* 0x4E */ s16    unk_4E;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuSlashTask;

// Size: 0x70

/**
 * @brief "Tsk_OtosuGame_track", 0x70 bytes.
 *
 * Same size as the slash task but the `Sprite` sits at +0x08 rather than at
 * +0, and the subtracted pair is +0x50 against +0x48. The render therefore
 * computes the mirror of the slash task's expression -- `(+0x50 - +0x48) >> 12`
 * where the slash task computes `(+0x58 - +0x50) >> 12` -- which is why the
 * two tasks' loaders differ by which word they load.
 */
typedef struct {
    /* 0x00 */ s32    unk_00;
    /* 0x04 */ s32    unk_04;
    /* 0x08 */ Sprite sprite;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s32    unk_4C;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
} OtuTrackTask;

// Size: 0x70

/**
 * @brief "Tsk_OtosuGame_point", 0x174 bytes.
 *
 * Four `Sprite`s at 0x40 stride from +0x00 and four `OtuPointSlot`s at +0x120.
 * The anchor pair at +0x100/+0x104 is where the tracked pin is and the one at
 * +0x108/+0x10C is where this task is, both Q12.12; +0x110 holds the pin's
 * task id; +0x114 and +0x118 are the frame counter and the "still live" flag;
 * +0x170 is the counter the update decrements and clamps.
 */
typedef struct {
    /* 0x000 */ Sprite       sprite[4];
    /* 0x100 */ s32          unk_100;
    /* 0x104 */ s32          unk_104;
    /* 0x108 */ s32          unk_108;
    /* 0x10C */ s32          unk_10C;
    /* 0x110 */ s32          unk_110;
    /* 0x114 */ s32          unk_114;
    /* 0x118 */ s32          unk_118;
    /* 0x11C */ s32          unk_11C;
    /* 0x120 */ OtuPointSlot slot[4];
    /* 0x170 */ s32          unk_170;
} OtuPointTask;

// Size: 0x174

/**
 * @brief One 0x10-byte animation-state block of the entry task.
 *
 * The two sprites each have one of these as their limit, and each also has a
 * 0x0C-byte running block that 0x02087bf8 writes. Only the scale words at the
 * block's +0x04 and +0x08 are read here -- they are what
 * `OamMgr_AllocAffineGroup` is handed -- but the whole 0x10 is there because
 * the stride is what keeps the two sprites apart.
 */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 scaleX;
    /* 0x08 */ s32 scaleY;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
} OtuEntryAnim;

// Size: 0x10

/**
 * @brief "Tsk_OtosuGame_entry", 0xC8 bytes.
 *
 * Two `Sprite`s at +0x00 and +0x40, each with a 0x10-byte limit block and a
 * 0x0C-byte running block behind it, so the second sprite's animation state
 * begins at +0xA0 and the two running blocks at +0xB0 and +0xBC.
 */
typedef struct {
    /* 0x00 */ Sprite       sprite;
    /* 0x40 */ Sprite       sprite2;
    /* 0x80 */ s32          unk_80;
    /* 0x84 */ s32          unk_84;
    /* 0x88 */ s32          unk_88;
    /* 0x8C */ s32          unk_8C;
    /* 0x90 */ OtuEntryAnim anim0;
    /* 0xA0 */ OtuEntryAnim anim1;
    /* 0xB0 */ u8           run0[0x0C];
    /* 0xBC */ u8           run1[0x0C];
} OtuEntryTask;

// Size: 0xC8

/**
 * @brief The block the three task-creating helpers pack and pass by pointer.
 *
 * Which words each one packs is its own business, so there are three types
 * rather than one over-declared union. The track task's middle word is the
 * caller's own pool pointer, which is why its block is three words and the
 * other two are two.
 */
typedef struct {
    s32 unk_00;
    s32 unk_04;
} OtuTaskArgs2;

typedef struct {
    s32       unk_00;
    TaskPool* unk_04;
    s32       unk_08;
} OtuTaskArgs3;

typedef struct {
    s32 unk_00;
} OtuTaskArgs1;

/* ==================================================================== */
/* The one function in this band that is deliberately absent.          */
/* ==================================================================== */

/*
 * func_ov039_02096d98 is `mov r0, #0x1` / `bx lr` and is not decompiled here.
 * OtuFieldAccess.c leaves it out, along with 0x020921f4, 0x02092bf0 and
 * 0x020933f0, on the grounds that four identical bodies at four addresses are
 * at least as likely to be four different predicates as four copies of one.
 * That reasoning does not survive contact with the callers, and the correction
 * is recorded here because the comment it contradicts lives in the shared TU
 * and not in this file.
 *
 * None of the four is ever `bl`'d. Each appears exactly once, as a `.word` in a
 * four-entry TaskStages table, and in every case it is the *third* entry:
 *
 *   0x02099610  { 02091b98, 02092154, 020921f4, 020921fc }
 *   0x020999a4  { 020929ec, 02092a78, 02092bf0, 02092bf8 }
 *   0x020999c0  { 02092f88, 020933c0, 020933f0, 020933f8 }
 *   0x02099ec8  { 02096d70, 02096d98, 02096da0, 02096dec }
 *
 * The slot order is initialize / update / render / cleanup -- fixed by
 * 02099de0, whose four entries are this file's own 020963c8 (clears the object
 * and loads the sprite), 02096404 (per-frame child fetch), 020964a4
 * (Sprite_RenderFrame) and 020964ec (Sprite_Release). So all four bodies are
 * the *render* stage of a different task, and the other three stages of each of
 * those four tasks all end `mov r0, #0x1` / `pop {..., pc}`. A render stage
 * that draws nothing and returns 1 is a coherent reading; four unrelated
 * predicates is not, because a predicate needs a load and there are none.
 *
 * What is left open is only whether the original source said `return 1;` or
 * something the compiler folded to the same two instructions -- which is not
 * decidable from the ROM and does not change the code. The function is left out
 * because the shared file's decision to leave it out is not mine to reverse,
 * and because doing so without being able to amend the comment it contradicts
 * would leave the tree asserting two things at once.
 */

/* ==================================================================== */
/* Referenced overlay data and cross-module routines.                  */
/* ==================================================================== */

/* ------------------------------------------------------------------ */
/* The three-sprite task of 02096874 / 020968c0 / 020969fc / 02096b48. */
/* ------------------------------------------------------------------ */

/*
 * Three `Sprite`s on a 0x40 stride from +0, then a state block from +0xC0.
 * All four stage functions in this band agree on that layout and on the four
 * state words, so they are named together. The text-cell block at +0xD0 is the
 * same four-word shape as `OtuTextBlock` (cleared x, 1.0 y, 1.0 scale, zero
 * width) followed by the cursor func_ov039_02087ba0 builds at +0xE0.
 *
 * Named because the state words are read from *all four* stages and the field
 * names are what stop the offsets being re-derived per function -- verified
 * codegen-neutral.
 *
 * The three sprites are `pad_000` and stay raw casts. Spelling them as
 * `Sprite sprite[3]` is *worse*, and measurably so: `Sprite_Update(self + 0x40)`
 * became `add r0, r4, #0x3800` rather than `add r0, r4, #0x40` (020968c0,
 * 99.97%), because mwcc treats a pointer to `sprite[n]` as needing the index
 * scaled and folds the 0x40 stride into an `mla`-style displacement. The
 * array member is correct as a *description* and wrong as *code* here.
 */
typedef struct {
    /* 0x000 */ u8  pad_000[0x0C0];
    /* 0x0C0 */ s32 visible; // "draw the text block"; gates the tail of Update
    /* 0x0C4 */ s32 which;   // which of the three sprites the stage acts on
    /* 0x0C8 */ s32 state;   // the four-state machine, 0..3
    /* 0x0CC */ s32 counter; // 0x3C-frame timer, reloaded on each transition
    /* 0x0D0 */ s32 textX;
    /* 0x0D4 */ s32 textY;
    /* 0x0D8 */ s32 textScale;
    /* 0x0DC */ s16 textWidth;
    /* 0x0DE */ s16 pad_DE;
} OtuTripleSprite;

// Size: 0xE0

/*
 * The four TaskHandles and the four TaskStages tables this band dispatches
 * through.  All eight are `.rodata` in the original overlay and are not
 * claimed by any delinks entry, so they are referenced by the build's own
 * symbol names rather than redeclared as new objects -- see the note on the
 * row tables in TinPinSlammer.h for why that matters.
 */
extern const TaskHandle data_ov039_02099d8c;

extern const TaskHandle data_ov039_02099dd4;

extern const TaskHandle data_ov039_02099e1c;

extern const TaskHandle data_ov039_02099ebc;

extern const TaskStages data_ov039_02099d98;

extern const TaskStages data_ov039_02099de0;

extern const TaskStages data_ov039_02099e28;

extern const TaskStages data_ov039_02099ec8;

/*
 * The six SpriteAnimation templates the sprite loaders copy to the stack.
 * Each is 0x2C bytes, which is exactly sizeof(SpriteAnimation), and each names
 * a different one of the overlay's sprite-cell builders as its frame-info
 * callback -- that pairing is the only thing that distinguishes them.
 */
extern const SpriteAnimation data_ov039_02099df0;

extern const SpriteAnimation data_ov039_02099e38;

extern const SpriteAnimation data_ov039_02099e64;

extern const SpriteAnimation data_ov039_02099e90;

extern const SpriteAnimation data_ov039_02099ed8;

extern const SpriteAnimation data_ov039_02099f20;

/*
 * The glyph tables the two inits at the end of this file build cursors over.
 * Declared as arrays, not as `const void*`: these symbols *are* the tables, so
 * a pointer declaration makes every use cost an extra `ldr` through the
 * symbol, which the target does not have.
 */
extern const u8 data_ov039_0209a830[];

extern const u8 data_ov039_0209a8a8[];

extern const u8 data_ov039_0209a938[];

extern const u8 data_ov039_0209aa0c[];

/* Within this band: the sprite loaders the init stages call. */
void func_ov039_0209633c(void* self, void* sprite, s32* args);

void func_ov039_02096718(void* self, void* sprite, s32* args);

void func_ov039_0209678c(void* self, void* sprite, s32* args);

void func_ov039_02096800(void* self, void* sprite, s32* args);

void func_ov039_02096d00(void* self, void* sprite);

void func_ov039_02096f40(void* self, void* sprite, s32* args);

/* Elsewhere in the overlay: the animation-phase counter 02096da0 reads. */
extern s32 func_ov040_0209cb5c(void);

/* Elsewhere in the overlay: the point-copy helper 02096404 calls that the
 * shared header does not declare (func_ov039_0208e6e0 is already there), and
 * the text-cell pair the two switch bodies below use. */
void func_ov039_02087ba0(void* out, void* table, s32 count, void* cell);

/*
 * ============================================================================
 * Band 5: the four pin-sprite tasks.
 * ============================================================================
 *
 * This band is four copies of one task type. Each owns a base-game `Sprite` at
 * the head of its data block and a run of pinball state after it, and each is
 * driven through the engine's `TaskStages` quartet -- init, update, render,
 * cleanup -- held in one of four `.rodata` tables:
 *
 *   data_ov039_02099f10   02096fcc  0209702c  02097160  020971b0
 *   data_ov039_02099f58   020974e0  02097528  02097670  020976c0
 *   data_ov039_02099fa0   02097918  02097954  020979cc  02097a14
 *   data_ov039_02099fe8   02097c30  02097c90  02097dbc  02097e0c
 *
 * Every stage takes `(TaskPool*, Task*, void*)`, returns 1, and reaches its
 * object through `task->data` -- the `ldr rX, [r1, #0x18]` that opens all
 * sixteen. Which of the sixteen are here:
 *
 *   first  table   init (02096fcc) is below this band; the other three are here
 *   second table   init/update/render/cleanup all here (020974e0..020976c0)
 *   third  table   all four here (02097918..02097a14)
 *   fourth table   all four here (02097c30..02097e0c)
 *
 * All sixteen stages are here except 0x02096fcc, which is the first table's init
 * and sits below this band; it is declared (unprototyped) where it is needed.
 * The band also holds the four stage dispatchers, the four task-creation
 * wrappers, the four `_Sprite_Load` wrappers and the four public setup routines.
 *
 * What the quartet does is legible from the code: an update stage fetches
 * another task's data, asks for its +0x110/+0x114 pair through
 * func_ov039_0208e85c, and blends the sprite toward it with
 * func_ov039_02098c00, so this is a sprite that chases a pin. What the pin is,
 * and what any of the counters mean, the code does not say and neither does
 * this file.
 */

/**
 * @brief A pin-sprite task's data block: a base-game `Sprite` plus overlay state.
 *
 * The first 0x40 bytes are an ordinary `Sprite`, and that is not a guess: it is
 * forced three ways over. The block is handed straight to `Sprite_Update`,
 * `Sprite_RenderFrame`, `Sprite_Release` and `_Sprite_Load`, its +0x18 and +0x1C
 * are passed back out as `Sprite_ChangeAnimation`'s `animData` and `cellTable`
 * arguments, and its first word is tested as a bitfield. Everything from +0x40
 * on is the overlay's own.
 *
 * The words are kept as a flat run rather than grouped into points, because the
 * two helpers that take them (`func_ov039_0208e85c` writes one,
 * `func_ov039_02098c00` reads one and writes another) are handed bare addresses
 * and the layouts differ between the four objects: the second task's block is
 * the first's shifted down by 0x10 for the first six fields and not for the
 * rest. Grouping them would assert a correspondence the code does not have.
 *
 * `unk_4C` is the one word in the run that is *not* only a word, and the
 * distinction is worth keeping. 02097c30 clears 0x4C and 0x4E as two separate
 * halfwords -- hence the `*(s16*)` casts, and hence no member for 0x4E -- while
 * the two init zero-runs at 020974e0 and 02097918 clear 0x4C with a single
 * 32-bit store. Splitting the field into `s16 unk_4C; s16 unk_4E;` was tried
 * and is *not* codegen-neutral: mwcc does not merge two adjacent same-value
 * halfword stores into one word store, so 020974e0 drops 100% -> 91.11% and
 * 02097918 drops 100% -> 89.33%, each losing exactly one `str r12, [r0, #0x4c]`
 * against two `strh`. So `s32` is what the original declared, and 02097c30's
 * halfword pair was written deliberately through a cast.
 */
typedef struct {
    /* 0x00 */ Sprite sprite; // 0x40 bytes; +0x0C/+0x0E are posX/posY
    /* 0x40 */ s32    unk_40;
    /* 0x44 */ s32    unk_44;
    /* 0x48 */ s32    unk_48;
    /* 0x4C */ s32    unk_4C;
    /* 0x50 */ s32    unk_50;
    /* 0x54 */ s32    unk_54;
    /* 0x58 */ s32    unk_58;
    /* 0x5C */ s32    unk_5C;
    /* 0x60 */ s32    unk_60;
    /* 0x64 */ s32    unk_64;
    /* 0x68 */ s32    unk_68;
    /* 0x6C */ s32    unk_6C;
    /* 0x70 */ s32    unk_70;
    /* 0x74 */ s32    unk_74;
    /* 0x78 */ s32    unk_78;
    /* 0x7C */ s32    unk_7C;
    /* 0x80 */ s32    unk_80;
    /* 0x84 */ s32    unk_84;
} OtuTaskSprite;

// Size: 0x88

/** @brief One of the four stages of a pin-sprite task. */
typedef s32 (*OtuTaskSpriteStage)(TaskPool* pool, Task* task, void* param);

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The four task descriptors the `EasyTask_CreateTask` wrappers hand over, and
 * the four `.rodata` stage tables the dispatchers name. All of these live in the
 * overlay's gap-filled `.rodata`, so they are referenced by the build's own
 * names: declaring an object of one's own would land it at an address of the
 * linker choosing, not the one the target uses, which is the same reason the
 * result-screen row tables are declared that way.
 *
 * The stage tables are declared but not referenced -- the dispatchers build their
 * own copies as local initialisers, because that is the only form mwcc emits as
 * the `ldm/stm` the target has. They are declared anyway so that the addresses
 * are on the record, and so that the one-row relocation-name gap on each
 * dispatcher is legible rather than mysterious. See the dispatcher section.
 */
extern const TaskHandle data_ov039_02099f04;

extern const TaskHandle data_ov039_02099f4c;

extern const TaskHandle data_ov039_02099f94;

extern const TaskHandle data_ov039_02099fdc;

extern const TaskStages data_ov039_02099f10;

extern const TaskStages data_ov039_02099f58;

extern const TaskStages data_ov039_02099fa0;

extern const TaskStages data_ov039_02099fe8;

/**
 * @brief The bin every pin-sprite in the overlay draws its cells from.
 *
 * All four `SpriteAnimation` templates in the overlay name this one record (bin
 * id 39); it is the only thing tying the four sprites' art together. Not
 * identified further -- it is a plain identifier record in the overlay's `.data`.
 */
extern const BinIdentifier data_ov039_0209a0dc;

/*
 * Eight Q12.12 constants in the overlay's `.data`, read by the two setup
 * routines. Values recovered with `tools/ov039_table.py`:
 *
 *   0209a2fc = 0x2000    0209a300 = 0x1000    0209a304 = 0x0133
 *   0209a30c = 0x4000    0209a310 = 0x0400    0209a314 = 0x3000
 *   0209a328 = 0x9800
 *
 * None of them is used the same way twice: 0x0133 is a per-frame bleed out of a
 * count, 0x0400 is both a `FX_Divide` denominator and a per-frame spin-up, and
 * 0x2000/0x4000 are the bounds of the two random ranges the starting spin is
 * drawn from.
 *
 * They are deliberately *not* declared `const`. They live in `.data`, not
 * `.rodata`, and mwcc acts on that: told a symbol is const it will keep its
 * value in a callee-saved register across the `FX_Divide` call in the two setup
 * routines, where the target reloads it every time. The `const` costs three
 * instructions there and buys nothing.
 */
extern s32 data_ov039_0209a2fc;

extern s32 data_ov039_0209a300;

extern s32 data_ov039_0209a304;

extern s32 data_ov039_0209a30c;

extern s32 data_ov039_0209a310;

extern s32 data_ov039_0209a314;

extern s32 data_ov039_0209a328;

/**
 * @brief The base game's random `s16` pair table, indexed off `RNG_Next`.
 *
 * Unidentified. It is a plain `s16` array in the main module, and both this band
 * and the boss code read a consecutive pair out of it at a random index. This
 * band uses it as a random direction vector; that is an inference from the two
 * values never being equal and neither being zero, not something the data says.
 */
/* data_0205e4e0 is an array of s32, declared by band 3 -- casting its *value*
 * rather than its address was worth 43 points on func_ov039_02095788. */

/**
 * @brief Reads a task's +0x110/+0x114 pair out to a point, 0x0208e85c.
 *
 * Defined in OtuFieldAccess.c, which this file is #included into *above* the
 * definition, so it has to be declared here rather than picked up from the
 * header -- the header does not carry it, and without a declaration mwcc infers
 * `int (...)` from the call below and then rejects the definition.
 */

/**
 * @brief The overlay's Q12.12 point blend, 0x02098c00.
 *
 * `out = base + (offset * scale) >> 12`, componentwise. Declared without a
 * prototype so that this file cannot disagree with whoever defines it.
 */

/** @brief The base game's 32-bit signed divide. Also unprototyped. */
s32 _s32_div_f();

/*
 * The four `_Sprite_Load` wrappers. Each fills a `SpriteAnimation` template with
 * the object's own anchor and owner and loads one sprite; only the field the
 * anchor is read from and the template's constant differ between them.
 *
 * The third argument is the task's creation block again, read as a word and
 * narrowed to a halfword: the target loads a full word and masks it with
 * `lsl #0x10; lsr #0x10` before folding it into the template's four-bit
 * `dataType` field, which a `u16*` parameter would have turned into a single
 * `ldrh` instead. The target passes nothing for this argument and the callee
 * reads whatever is in r2, which is still the block because no caller has
 * touched r2 since loading the id out of it. Passing it explicitly costs no
 * instruction -- r2 already holds it -- and keeps the declaration honest.
 */
void func_ov039_02097454(OtuTaskSprite* self, Sprite* sprite, s32* dataType);

void func_ov039_0209788c(OtuTaskSprite* self, Sprite* sprite, s32* dataType);

void func_ov039_02097ba4(OtuTaskSprite* self, Sprite* sprite, s32* dataType);

/* ------------------------------------------------------------------ */
/* The create wrappers.                                                */
/* ------------------------------------------------------------------ */

/*
 * Each pushes the two words it was given as the new task's creation parameter
 * block and calls `EasyTask_CreateTask` with the NULL data pointer, priority 0
 * and NULL parent the engine's signature asks for.
 *
 * The block is a named two-word struct rather than two scalar locals, and that
 * is load-bearing: only the block's *address* is ever taken, so with scalars mwcc
 * drops the second store as dead and the frame shrinks by four bytes. A struct
 * whose address is taken keeps both.
 *
 * Every `init` stage receives this block as its third argument and reads it as
 * two words: the first becomes the sprite's initial position, the second a task
 * id the object then chases.
 *
 * The stages take it as `void*` and the four public setup routines take
 * `OtuTaskParams*`, and that is not an inconsistency. A stage's signature is
 * fixed by `TaskStages` -- every stage shares `(TaskPool*, Task*, void*)` because
 * the engine's table is homogeneous -- and mwcc rejects the implicit conversion
 * if one stage narrows it. The public routines have no such constraint and say
 * what they mean.
 */

/** @brief The two-word parameter block `EasyTask_CreateTask` hands to a task. */
typedef struct {
    /* 0x0 */ s32 word0;
    /* 0x4 */ s32 word1;
} OtuTaskParams;

// Size: 0x8

/* Forward declarations for the two functions below that this file's own
 * call sites reach first. Without them C infers `int (...)` and the real
 * definition then reads as a redeclaration. This is band 5 declaring its own
 * functions, not a dependency on another band.
 *
 * They sit here rather than at the top of the file because they name
 * OtuTaskSprite and OtuTaskParams, which are defined just above: the two
 * typedefs live in this file rather than the feature header because this is
 * their only user. */
s32 func_ov039_0209720c(TaskPool* pool, s32 word0, s32 word1);

void func_ov039_02097240(OtuTaskSprite* s, OtuTaskParams* param);

/* ------------------------------------------------------------------ */
/* The four stage dispatchers.                                         */
/* ------------------------------------------------------------------ */

/*
 * One body, four times: index the task's four-entry stage table and call it.
 *
 * This is the task's `TaskHandle.taskFunc`, and it is why the sixteen stages all
 * share one signature -- the table is what the engine walks. The frame is what
 * tells the story: `sub sp, #0x10` plus `add ip, sp, #0` and the
 * `ldm lr, {r0..r3} / stm ip, {r0..r3}` pair is mwcc rendering a four-pointer
 * *local array initialiser*, not a read from `extern const TaskStages`. So each
 * dispatcher writes its table out as a brace initialiser even though the same
 * bytes also exist in the overlay's gap-filled `.rodata` at
 * 0x02099f10/0x02099f58/0x02099fa0/0x02099fe8 -- the `extern` declarations for
 * those are above and are what the *relocation names* would otherwise have
 * pointed at. An anonymous template costs one relocation-name row; a referenced
 * `extern` would move the bytes.
 *
 * The first table's init entry, 0x02096fcc, lives below this band and is declared
 * here without a prototype so this file cannot disagree with whoever defines it.
 */
s32 func_ov039_02096fcc();

/* ============================================================================
 * Band 6: Tsk_OtosuGame_obstacle
 *
 * One of the overlay's obstacle sprites. Every stage callback returns 1, and
 * the state block begins with a Sprite: Sprite_Update, Sprite_RenderFrame and
 * Sprite_Release are all handed `task->data` unchanged, so the task state *is*
 * the sprite rather than holding a pointer to one.
 *
 * The sprite is stationary. Init takes a target position in 12-bit fixed point
 * and Render recomputes the screen position from it every frame; Update spends
 * exactly one frame swapping the animation to its looping variant. The only
 * motion is the whole group being scaled, which is what `scale` is for.
 *
 * Placeholder names, so objdiff can pair these against the target. The roles
 * they stand for, in the order the stage table at 0x02099948 holds them:
 *
 *   func_ov039_020926a8  RunTask    (also the handle's taskFunc)
 *   func_ov039_0209258c  Init
 *   func_ov039_02092608  Update
 *   func_ov039_02092658  Render
 *   func_ov039_02092694  Destroy
 *   func_ov039_02092484  Load
 *   func_ov039_020926f0  CreateTask
 *
 * The roles are not guesses -- the target's own stage table holds these four
 * callback addresses in order, and the handle's taskFunc field is the
 * dispatcher. Renaming them is a separate pass: it means editing symbols.txt
 * for USA *and* JP, and a rename that misses either region breaks pairing.
 * ==========================================================================*/

/** The caller's per-obstacle numbers, reached through OtuObstacle_Args.params. */
typedef struct {
    /* 0x00 */ u16 targetX; // <<12 into OtuObstacle.targetX
    /* 0x02 */ u16 targetY; // <<12 into OtuObstacle.targetY
    /* 0x04 */ u16 kind;    // 0..2; picks a row of each sprite table below
} OtuObstacle_Params;

// Size: 0x6

typedef struct {
    /* 0x00 */ Sprite sprite;      // Size: 0x40. Passed straight to the sprite API.
    /* 0x40 */ s32    originX;     // Screen position at spawn; the fixed-point target
    /* 0x44 */ s32    originY;     // is measured relative to it.
    /* 0x48 */ s32    targetX;     // Fixed point, <<12.
    /* 0x4C */ s32    targetY;     // Fixed point, <<12.
    /* 0x50 */ s32    scale;       // 16.16 scale factor, picked by kind.
    /* 0x54 */ s32    animPending; // One-shot: set at spawn, cleared by the first Update.
} OtuObstacle;

// Size: 0x58

typedef struct {
    /* 0x00 */ s32                 oamAttrs; // ORed into the template's OAM word, shifted to bit 6.
    /* 0x04 */ s16                 unk_04;
    /* 0x06 */ s16                 slot;     // Which of the three palette slots to use.
    /* 0x08 */ OtuObstacle_Params* params;
} OtuObstacle_Args;

// Size: 0xC

/** The handle, the stage table and the sprite template are all already in the
 *  overlay's .rodata, so they are referenced rather than redefined. The name
 *  string inside the handle is the ground truth for what this task is. */
extern const TaskHandle data_ov039_02099930;

// "Tsk_OtosuGame_obstacle", size 0x58

/** The sprite template. Every field Load does not patch is already correct here,
 *  and the five it does patch all hold their kind-0 values, so this is literally
 *  the index-0 case that the table lookups below then re-derive. */
extern const SpriteAnimation data_ov039_0209996c;

/* Five tables of three, indexed by OtuObstacle_Params.kind. They are separate
 * arrays rather than one array of a struct because the target loads each base
 * address into its own register and indexes them independently. */
extern const s16 data_ov039_02099918[3];

// -> params.unk_26
extern const s16 data_ov039_0209991e[3];

// -> params.unk_1C
extern const s16 data_ov039_02099924[3];

// -> params.unk_28
extern const s16 data_ov039_0209992a[3];

// -> params.packIndex

/** Palette slots. A row per kind, a column per OtuObstacle_Args.slot; all three
 *  rows currently hold the same {4, 5, 6}, so kind does not yet change colour. */
extern const s16 data_ov039_02099958[3][3];

// -> params.unk_20

/** The per-kind 16.16 scale factors, {0x20000, 0x18000, 0x10000}. Init copies
 *  these to the stack and indexes the copy. */
typedef struct {
    s32 v[3];
} OtuObstacle_Scales;

extern const OtuObstacle_Scales data_ov039_0209993c;

/* ============================================================================
 * The cursor family at 0x0209003c - 0x020901e0.
 *
 * Eight functions in two identical pairs, and both halves of the band-7 hammer
 * task call all eight, so they come first: nothing else in this file links
 * until these exist.
 *
 * The shape is OtuCursor from band 1 -- table, index, count, framesLeft, 0xA
 * bytes -- but the table is walked with a stride of six *bytes*, so these entries are
 * 6 where band 1's are 4. Two functions are declared over a
 * four-byte entry instead; see func_ov039_0209003c for why that is not a typo.
 *
 *   func_ov039_0209005c / func_ov039_02090148   start a cursor at entry 0
 *   func_ov039_0209008c / func_ov039_02090178   step it by one frame
 *   func_ov039_0209003c / func_ov039_020901e0   read the entry's `value`
 *   func_ov039_020900f4                         read the entry's `value`
 *   func_ov039_0209011c                         read the entry's `scale`, <<12
 *
 * Each pair is byte-for-byte the same code, which is why the two families can
 * be read as one: the hammer task keeps one cursor per animated quantity and
 * the compiler was pointed at the same routine for each.
 * ==========================================================================*/

/** One entry of the table: three halfwords, a six-byte stride.
 *
 * The stride is six *bytes*, not six halfwords -- the target's only multiply is
 * `smulbb r0, r2, r0` against an immediate of 6 feeding a byte offset, so the
 * immediate is a byte count. The three readers take the halfword at +0, +2 and
 * +4 respectively.
 */
typedef struct {
    /* 0x00 */ s16 duration; // frames to hold this entry
    /* 0x02 */ s16 value;    // what func_ov039_020900f4 / _1e0 return
    /* 0x04 */ s16 scale;    // <<12 on use, by func_ov039_0209011c
} OtuFrame6;

// Size: 0x6

/** A cursor over a table of OtuFrame6, one entry per call. */
typedef struct {
    /* 0x00 */ OtuFrame6* table;
    /* 0x04 */ s16        index;
    /* 0x06 */ s16        count;
    /* 0x08 */ s16        framesLeft;
} OtuCursor6;

// Size: 0xA

/** One entry as func_ov039_0209003c reads it: four bytes, not twelve.
 *
 *  Only this one function indexes the cursor's table with a stride of four.
 *  Everything else in the family -- including the cursor's own initialiser and
 *  stepper -- uses six, so this is a genuine disagreement in the original code
 *  rather than a transcription slip, and it is reproduced rather than tidied.
 */
typedef struct {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 value;
} OtuFrame4;

/* ============================================================================
 * Tsk_OtosuGame_hammer -- the task itself, 0x020904a8 - 0x02090e1c.
 *
 * Identity read from the ROM rather than inferred: the handle at 0x0209942c
 * carries the name string at 0x0209a594, which is "Tsk_OtosuGame_hammer", a
 * taskFunc of 0x02090dd4 (func_ov039_02090dd4, below) and a dataSize of 0x138.
 * The stage table at 0x02099448 holds, in order, initialize 0x020904a8, update
 * 0x0209054c, render 0x02090cec, cleanup 0x02090d90. That is the same four-word
 * layout as every other task in the overlay, and it is why func_ov039_02090dd4
 * can be a plain dispatcher.
 *
 * Two Sprites are embedded rather than pointed at -- Sprite_Update,
 * Sprite_RenderFrame and Sprite_Release are all handed `self + 4` and
 * `self + 0x44` directly -- so the state block begins four bytes into the task's
 * data. Positions are 12-bit fixed point measured against `baseX`/`baseY`, the
 * same convention as band 6's obstacle, which makes this the third task in the
 * overlay to use it.
 * ==========================================================================*/

extern const TaskStages data_ov039_02099448;

extern const TaskHandle data_ov039_0209942c;

/** Band 8's child spawner. Declared here because band 7 comes first in the
 *  include order, and left undeclared it would be picked up as implicit `int`. */
s32 func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2);

/** The hammer's per-task state. Two sprites, then the numbers Update drives. */
typedef struct {
    /* 0x000 */ s32        unk_00; // four bytes ahead of the first sprite
    /* 0x004 */ Sprite     spriteA;
    /* 0x044 */ Sprite     spriteB;
    /* 0x084 */ s32        spriteRotA; // the live angle plus 0x4000
    /* 0x088 */ s32        spinValue;  // cursor B's value; > 0 reveals sprite A
    /* 0x08C */ s32        unk_8C;
    /* 0x090 */ s16        unk_90;
    /* 0x092 */ s16        unk_92;
    /* 0x094 */ s32        spriteRotB; // the same angle plus 0x4000, again
    /* 0x098 */ s32        unk_98;
    /* 0x09C */ s32        lenValue;   // the current length: the scale before the <<4
    /* 0x0A0 */ s16        unk_A0;
    /* 0x0A2 */ s16        unk_A2;
    /* 0x0A4 */ s32        baseX;    // the fixed-point targets below are relative to this
    /* 0x0A8 */ s32        baseY;
    /* 0x0AC */ s32        targetX;  // <<12, sprite A
    /* 0x0B0 */ s32        targetY;
    /* 0x0B4 */ s32        targetXB; // <<12, sprite B
    /* 0x0B8 */ s32        targetYB;
    /* 0x0BC */ s32        originX;
    /* 0x0C0 */ s32        originY;
    /* 0x0C4 */ s32        zOffset; // added into sprite B's Y target only
    /* 0x0C8 */ s32        parentHandle;
    /* 0x0CC */ s32        live;    // outer gate: parent resolved and still valid
    /* 0x0D0 */ s32        showA;
    /* 0x0D4 */ s32        showB;
    /* 0x0D8 */ s32        scale;
    /* 0x0DC */ s32        angle0;
    /* 0x0E0 */ s32        angle;
    /* 0x0E4 */ s32        halfLen;
    /* 0x0E8 */ s32        state;
    /* 0x0EC */ s32        framesLeft;
    /* 0x0F0 */ s32        rate0;
    /* 0x0F4 */ s32        scale0;
    /* 0x0F8 */ s32        totalFrames;
    /* 0x0FC */ s32        angleBase;
    /* 0x100 */ OtuCursor6 cursorScale;
    /* 0x10C */ OtuCursor6 cursorSpin;
    /* 0x118 */ OtuCursor6 cursorTrail;
    /* 0x124 */ s32        reportTimer;
    /* 0x128 */ s32        children[4];
} OtuHammer;

// Size: 0x138

/** The two words the task is created with, and which Init reads back.
 *
 *  This is EasyTask_CreateTask's sixth argument and nothing more: the target's
 *  `str r5, [sp, #8] / str r4, [sp, #0xc]` is this pair being built, and the
 *  `stm sp, {r2, ip}` beside it is the fifth and sixth *stack* arguments being
 *  written -- NULL and `&params`. There is no outer struct; what looked like one
 *  in the m2c draft is just those two stack slots.
 */
typedef struct {
    /* 0x00 */ s32 arg1; // the kind, read by the two sprite loaders
    /* 0x04 */ s32 arg2; // the parent handle, stored at +0xC8
} OtuHammer_Params;

// Size: 0x8

/**
 * The task's initializer: zeroes the state, remembers the parent, and loads the
 * two sprites.
 *
 * `arg0` is stored at +0x00, four bytes ahead of sprite A, and nothing in this
 * band reads it -- it is the spawn-time argument the renderer never needs.
 *
 * The two sprite loaders are not decompiled and are not referenced anywhere else
 * in the overlay, so they are left as externs: dsd resolves them against the
 * original overlay rather than failing the link, which is how the rest of this
 * band's still-absent callees work too.
 */
extern void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params);

extern void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params);

/** Band 8's child spawner, re-declared for the other hammer entry point. */

/** Band 1's two stride-four cursor helpers, plus the four things Update calls
 *  that live in bands this one cannot see ahead of. All declared here because
 *  band 7 comes first in the include order, and an undeclared call in C99 is an
 *  implicit `int` that later collides with the real definition. */
void func_ov039_0208ffac(OtuCursor* c, s32* table, s16 count);

s32 func_ov039_0208ffd8(OtuCursor* c);

void func_ov039_02087d04(s32 mode, void* dst, void* src);

void func_ov039_0208f048(void* task, s32 arg1, s32 arg2);

s32 func_ov039_0208e9ac(OtuPinTask* task);

/* The five animation tables the state machine walks, in the overlay's data. */
extern OtuFrame6 data_ov039_02099494;

extern OtuFrame6 data_ov039_020994b4;

extern OtuFrame6 data_ov039_02099476;

extern OtuFrame6 data_ov039_020994d8;

extern OtuFrame6 data_ov039_02099458;

extern OtuFrame6 data_ov039_02099438;

/* ============================================================================
 * The transform helper at 0x02090e9c.
 *
 * Nothing in this band calls it -- it is reached from somewhere outside the
 * hammer's own stage table -- but it takes the hammer's state struct, so it
 * belongs here. Read from the target:
 *
 *   - it refuses to do anything unless `state` is 4, the one state in which the
 *     hammer is drawn as a stretched bar rather than a sprite pair;
 *   - it writes *two* points, each {x, y, 0x10000}, and returns 2. The 0x10000 is
 *     Q16.16 one, so this is a pair of 2D vertices with a pinned w -- an oriented
 *     segment, not three corners as the earlier note guessed;
 *   - each point is the pin's origin, pushed out along `angle` by `scale` in
 *     Q12.12, then displaced by 16 in the perpendicular direction -- one point
 *     each way, so the pair is the bar's two ends.
 * ==========================================================================*/

/** One vertex of the bar: two fixed-point coordinates and a pinned w. */
typedef struct {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 w; // 0x10000, Q16.16 one
} OtuBarVertex;

// Size: 0xC

/** The bar: its two ends. */
typedef struct {
    /* 0x00 */ OtuBarVertex v[2];
} OtuBar;

/** The handle this spawns. Already in the overlay's .rodata. */
extern const TaskHandle data_ov039_0209a024;

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * The overlay's `.rodata` is gap-filled from 0x02098f3c to 0x0209a12c and its
 * `.data` from 0x0209a49c to 0x0209ad00, and every symbol this band names in
 * those windows is in the gap. Declaring an object of our own would put it at
 * an address of the linker choosing rather than the one the target uses, so
 * they keep the build's own names. See the same note in band 5.
 *
 * `data_ov039_0209a0fc` is the `BinIdentifier` all four templates name -- the
 * only thing tying this task's eighteen sprites to one piece of art. It is
 * the same *kind* of record as band 5's `data_ov039_0209a0dc` (one overlay,
 * one bin id) but a different object, so it is declared rather than shared.
 */
extern const TaskHandle data_ov039_02099b98;

extern const TaskStages data_ov039_02099ba4;

extern SpriteAnimation data_ov039_02099bb4;

extern SpriteAnimation data_ov039_02099be0;

extern SpriteAnimation data_ov039_02099c0c;

extern SpriteAnimation data_ov039_02099c38;

extern const BinIdentifier data_ov039_0209a0fc;

/** @brief One cell's single-digit position: two halfwords. */
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
} OtuGaugePos;

// Size: 0x4

/**
 * @brief One cell's two-digit position: four signed halfwords.
 *
 * The last pair is the first minus five on x, so `lowX`/`highX` read left to
 * right as tens-then-units. The halfword stride is what the code uses: 0x02094418
 * addresses entry `index` and then offsets by `which * 4` to pick a half of it.
 */
typedef struct {
    /* 0x00 */ u16 lowX;  // units digit
    /* 0x02 */ u16 lowY;
    /* 0x04 */ u16 highX; // tens digit, five pixels left of lowX
    /* 0x06 */ u16 highY;
} OtuGaugeDigits;

// Size: 0x8

/*
 * The two digit-position tables. Both are declared non-`const` because they
 * live in the overlay's `.data`, not its `.rodata` -- band 5 records that
 * telling mwcc a `.data` symbol is const costs it a reload across a call, and
 * neither table is folded here (both are indexed by the loop counter) so the
 * distinction is free either way, but the section is the honest answer.
 *
 * Values, recovered with `tools/ov039_bytes.py`: 0x0209a780 holds
 * (0x37,0x9C) (0x27,0x83) (0x14,0x90) (0x1E,0xA7), and 0x0209a790's first
 * pair repeats those exactly while its second is five to the left. So the
 * two-digit table is the one-digit table plus the tens digit, which is exactly
 * the choice the update's `count < 10` / `count >= 10` split makes.
 */
extern OtuGaugePos data_ov039_0209a780[4];

extern OtuGaugeDigits data_ov039_0209a790[4];

/**
 * @brief The overlay's fourth "is this pin the kind I care about" filter.
 *
 * The shared header declares the other three (0x0208e984 kind 8, 0x0208e998
 * kind 7, 0x0208e9d0 kind 6) but not this one. The target's body is
 * `ldr r1,[r0,#0xf8]; cmp r1,#9; moveq r0,#1` -- kind 9 -- so it belongs to
 * that family and sits between them at 0x0208e9e4, inside OtuFieldAccess.c's
 * own claim and not yet defined there.
 */
s32 func_ov039_0208e9e4(OtuPinTask* task);

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                   */
/* ------------------------------------------------------------------ */

/**
 * @brief The two words `func_ov039_02094ab4` packs and the init stage reads.
 *
 * The first becomes the task's `dataType` and is folded into the `dataType`
 * bitfield of all eighteen `SpriteAnimation` templates; the second is the pin
 * task's handle, which the update resolves through `EasyTask_GetTaskData` and
 * then queries four ways. This is a struct rather than two scalar locals
 * because only its *address* is ever taken, and with scalars mwcc drops the
 * second store as dead and the frame shrinks by four bytes (band 5 records the
 * same for its own block).
 */
typedef struct {
    /* 0x00 */ s32 dataType; // SpriteAnimation.dataType, four bits
    /* 0x04 */ s32 pinId;    // handle of the pin task this gauge watches
} OtuGaugeArgs;

// Size: 0x8

/**
 * @brief One cell of the gauge: its two digit sprites, 0x80 apart.
 *
 * The 0x80 stride is load-bearing and is the reason this is a struct rather
 * than a flat `Sprite[8]`: the update and the render each walk the "low" and
 * the "high" sprite with a *separate* pointer advancing 0x80 per cell, side by
 * side with the two 0x40-stride walks, and mwcc's address arithmetic follows
 * the source's. Indexing a flat array by `i * 2` would fold the multiply and
 * change the instruction the loop is built from.
 *
 * "low" is the units digit: it is repositioned and re-animated on every change
 * of the count, and is the one shown alone when the count is below ten.
 * "high" is the tens digit: it is only repositioned, re-animated, updated and
 * drawn once the count reaches ten.
 */
typedef struct {
    /* 0x00 */ Sprite spriteLow;  // units digit
    /* 0x40 */ Sprite spriteHigh; // tens digit
} OtuGaugePair;

// Size: 0x80

/**
 * @brief "Tsk_OtosuGame_specialgauge", 0x4B4 bytes.
 *
 * The size is the target's, not an estimate: the `TaskHandle` at 0x02099b98
 * carries it as its third word.
 *
 * The layout is derived from the strides the code uses, and every offset in it
 * is fixed by at least one load or store in this band. From 0x0C to 0x40B the
 * block is eighteen `Sprite`s with nothing between them, which the arithmetic
 * above pins from three independent directions. The 0x400 tail is
 * `OtuEntryAnim`-shaped: a rotation and an x/y scale pair handed to
 * `OamMgr_AllocAffineGroup`, two halfwords nothing in this band reads, and two
 * four-entry arrays the update compares against.
 *
 * The two wide sprites are named for what the code does to them rather than for
 * what they depict: `plate` (0x40C) is only ever updated and rendered, and
 * `dial` (0x44C) is the one whose OAM attribute word has an affine slot index
 * inserted into it every frame.
 */
typedef struct {
    /* 0x000 */ s32          dataType;     // folded into every template's dataType field
    /* 0x004 */ s32          pinId;        // the tracked pin's task handle
    /* 0x008 */ s32          running;      // raised by Update, tested by Render
    /* 0x00C */ Sprite       spriteA[4];   // four cells' first 0x40-walk sprite
    /* 0x10C */ Sprite       spriteB[4];   // four cells' second 0x40-walk sprite
    /* 0x20C */ OtuGaugePair digit[4];     // the four two-digit displays
    /* 0x40C */ Sprite       plate;        // the wide backing sprite
    /* 0x44C */ Sprite       dial;         // the wide sprite drawn through the affine path
    /* 0x48C */ s32          rotation;     // +0x100 per frame, kept to 16 bits
    /* 0x490 */ s32          scaleX;       // 0x1000, handed to the affine allocator
    /* 0x494 */ s32          scaleY;       // 0x1000 likewise
    /* 0x498 */ s16          unk_498;      // cleared by Init, read by nothing in this band
    /* 0x49A */ s16          unk_49A;      // likewise
    /* 0x49C */ s16          lastCount[4]; // the count each cell was last drawn for
    /* 0x4A4 */ s32          alive[4];     // the filter's answer for this cell's pin
} OtuGauge;

// Size: 0x4B4

/**
 * @brief The palette block inside a loaded sprite resource's buffer.
 *
 * `buffer + 0x20` is the overlay's `PackHeader`; the word at the caller's
 * offset into the table that follows is a `PackEntry`'s `offset` field, and
 * adding it to the table's base is the sub-resource. This is the same walk as
 * band 2's `OtuPaletteSource` and as `Data_GetPackEntryData`, written out with
 * the entry index as a parameter because this band needs two of them: the
 * update takes the fifth entry when a cell has gone dead and the fourth when it
 * has come back, and the arithmetic differs only in the constant.
 *
 * Declared here rather than reusing band 2's helper so this band does not
 * depend on the include order, and named differently so the two can coexist.
 */
static inline void* OtuGaugePaletteSource(Data* file, s32 packEntry) {
    PackEntry* entries = (PackEntry*)((u8*)file->buffer + sizeof(PackHeader));

    return (u8*)entries + entries[packEntry].offset;
}

/* ============================================================================
 * Data
 * ==========================================================================*/

/* An overlay-global byte block in the main module, at 0x02071cf0. Nothing in
 * this batch owns it; two unrelated tables inside it are written here. Declared
 * as a bare array so no dsd symbol has to be invented for the block itself. */
extern u8 data_02071cf0[];

/* The overlay's own bin identifier for this stage's data, and the eight 10-entry
 * tables that go with it. All read-only here, and all reached through the
 * overlay's own symbol names rather than invented ones -- declaring the arrays
 * and letting dsd place them would put the pool words at addresses of the
 * linker's choosing instead of the target's. */
extern const BinIdentifier data_ov039_0209a114;

/* Offsets and sizes of the inline parameter block at stage+0x150. */
extern const s32 data_ov039_020990fc[10];

extern const s32 data_ov039_02099124[10];

/* Size and bin offset of each of the three heap buffers. */
extern const s32 data_ov039_0209914c[10];

extern const s32 data_ov039_02099174[10];

extern const s32 data_ov039_0209919c[10];

extern const s32 data_ov039_020991c4[10];

extern const s32 data_ov039_020991ec[10];

extern const s32 data_ov039_02099214[10];

/* Score-row seeds, indexed by the same stage index as the tables above. */
extern const s32 data_ov039_0209a360[10];

/* ============================================================================
 * Types
 * ==========================================================================*/

/** One 0x10-byte record of an inbound wireless packet.
 *
 *  Four words, and the copy at 02088698 moves twelve of them in one go with
 *  `ldm`/`stm` -- which is what mwcc emits for a 16-byte struct assignment, and
 *  is why this is a struct rather than a `memcpy`.
 *
 *  Those four words are opaque on purpose: this is the received packet's
 *  payload, copied verbatim from the wire buffer, and nothing in the overlay
 *  ever reads an individual word of it. The fields it *can* see live either side
 *  of the array -- `packetKind` at +0x06 is compared against the wire packet's
 *  own +0x50, and `gotPacket` at +0x08 is set once the copy completes. So this
 *  is not an unfinished decode; it is a payload with no structure to recover. */
typedef struct {
    /* 0x00 */ s32 w[4];
} OtuWireRecord;

// Size: 0x10

/** The 0x10-byte parameter block at stage + 0x150.
 *
 *  Loaded verbatim from the overlay's bin by 02088c50 and handed *by address* to
 *  three task factories, so the overlay never interprets it here. Only three
 *  bytes are ever read back, and all three with `ldrb`, so all three are u8.
 *  `effectMax` becoming the effect count is the load-bearing one: 02088c50
 *  zero-extends it and stores it into a word field. */
typedef struct {
    /* 0x00 */ u8 kind;      // == 1 makes both rounds clear the OBJ layers
    /* 0x01 */ u8 variant;   // handed to the obstacle factory as `slot`
    /* 0x02 */ u8 pad_02[2];
    /* 0x04 */ u8 effectMax; // copied to stage->effectCount by 02088c50
    u8            pad_05[0x0B];
} OtuBoardParams;

// Size: 0x10

/** One of the countdown block's three 0x10-byte groups.
 *
 *  `groupCount` is the number of menu cells this group spans -- it is what
 *  02088a8c multiplies by 0x22 to find where the next group's sprites start,
 *  and what 02088df0 counts non-zero to decide how many pin children to spawn.
 *  `row` is the 0xE-byte copy of one pin-tray slot. */
typedef struct {
    /* 0x04 */ u16 groupCount;
    /* 0x06 */ u8  row[0x0A];
} OtuBoardCountdownGroup;

// Size: 0x10

/** The board's countdown block: two digit columns and three score groups.
 *
 *  The 0x34 stride is what makes this a menu-indexed array. It agrees with the
 *  header's OtuMenuCountdown on the two leading digit columns -- which are the
 *  header's `thousands` and `hundreds` at +0x84/+0x86 of the 0x44000 sub-object,
 *  i.e. +0 and +2 of this block -- and differs in that it also describes the
 *  group count and the tray copy that follow them. */
typedef struct {
    /* 0x00 */ u16                    digits[2];
    /* 0x04 */ OtuBoardCountdownGroup group[3];
} OtuBoardCountdown;

// Size: 0x34

/** The board stage's block: the object every function in this batch reaches
 *  through `func_ov039_02098b70(OTU_STAGE(scene))`.
 *
 *  Nothing in decompiled code allocates this -- func_ov039_02098a60 does, from
 *  the size in the stage descriptor -- so the extent below is read off the
 *  highest offset any function here touches (0x2CC) and rounded to 0x2D0, which
 *  is the size the overlay's own stage descriptors advertise for the two board
 *  stages. Fields keep their offsets; the roles are named only where the code
 *  says what they are. */
typedef struct {
    /* 0x000 */ u8                 pad_000[0x06];
    /* 0x006 */ u16                packetKind;   // matched against the wire packet's +0x50
    /* 0x008 */ s32                gotPacket;
    /* 0x00C */ OtuWireRecord      rxRecord[12]; // 0xC0 bytes, filled wholesale by 02088698
    /* 0x0CC */ u16                active;       // raised by both entry points at 02089780/898a8
    /* 0x0CE */ u8                 pad_0CE[0x72];
    /* 0x140 */ s32                childCount;   // number of pin children
    /* 0x144 */ s32                effectCount;
    /* 0x148 */ s32                helperCursor; // 0..0x40, round-robin base for 02089064
    /* 0x14C */ s32                stageIndex;   // indexes all eight data tables above
    /* 0x150 */ OtuBoardParams     params;
    /* 0x160 */ u8*                bufA;         // 0x1388 bytes from the heap
    /* 0x164 */ u8*                bufB;         // handed to the obstacle factory as 8-byte records
    /* 0x168 */ u8*                bufC;
    /* 0x16C */ OtuBoardCountdown* countdown;
    /* 0x170 */ s32                countdownAux; // the block's second digit column, or a table entry
    /* 0x174 */ s32                timer;        // set to 0x258 by 020898a8, read nowhere here
    /* 0x178 */ s32                outcome;      // 020893fc's verdict: 1, 2 or 3
    /* 0x17C */ s32                childIds[4];  // child task handles
    /* 0x18C */ s32                helperIds[0x40];
    /* 0x28C */ s32                effectIds[8];
    /* 0x2AC */ s32                pinTask;
    /* 0x2B0 */ s32                bgTaskA;
    /* 0x2B4 */ s32                bgTaskB;
    /* 0x2B8 */ s32                bgTaskC;    // the animated palette's owner
    /* 0x2BC */ s32                fxTask;     // the round-state task 020894cc polls
    /* 0x2C0 */ s32                fxTask2;
    /* 0x2C4 */ s32                soundTask;  // lives in pool 1, not pool 2
    /* 0x2C8 */ s32                soundTask2; // ditto; deleted by 02089918
    /* 0x2CC */ s32                soundTask3; // ditto; created but never read here
} OtuPinStage;

// Size: 0x2D0

/* ============================================================================
 * Callee declarations. Nothing here is defined in this file.
 * ==========================================================================*/

/* --- In this overlay, not yet decompiled ---------------------------------*/

/** Maps the scene's mode selector onto a local player index.
 *
 *  Read from the target: 0 for mode 0, ov040's index for mode 1, 0 for anything
 *  else. Re-read inside several loops rather than hoisted, which is why the call
 *  sites below spell it out each time. */
s32 func_ov039_02088418(s32 mode);

/** The six-byte touch-pad descriptor for player slot `i`. */
void* func_ov039_02088440(s32 slot);

/** Polls the touch screen into one of those descriptors and resets the
 *  "was touched" latch. */
void func_ov039_02088564(void* pad);

/** The pin child's "is this one worth resolving" test, and its follower. */
s32 func_ov039_0208e9f8(TaskPool* pool, void* task);

void func_ov039_0208eaa0(void* self, void* other);

/** The pairwise interaction predicates: contact, push-apart, and effect-pull. */
s32 func_ov039_0208e28c(OtuPinTask* a, OtuPinTask* b);

s32 func_ov039_0208e37c(OtuPinTask* a, OtuPinTask* b);

s32 func_ov039_0208e504(OtuPinTask* a, OtuObstacle* b);

/** Reads a pin child's *own* aim point -- a different pair from the +0x120 one
 *  that func_ov039_0208e6e0 copies out. */
void func_ov039_0208e6fc(void* task, OtuPoint* out);

/** Creates one pin child. Ten parameters, four in registers and six on the
 *  stack; the last six are the block's parameter block, the scene, the scene's
 *  +0x41EF0 block, one pin-tray slot, an "is this the first child" flag and a
 *  per-group sprite base. */
s32 func_ov039_0208dcb0(TaskPool* pool, s32 dataType, s32 slot, void* pad, void* params, void* scene, void* board,
                        void* traySlot, s32 isFirst, void* groupBase);

/** Task factories. All six are the same three-line wrapper around
 *  EasyTask_CreateTask and all six return the new handle in r0. */
s32 func_ov039_02094e58(TaskPool* pool, s32 dataType);

s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params);

s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params);

s32 func_ov039_020941d0(TaskPool* pool, s32 dataType, s32 aux);

s32 func_ov039_02094ab4(TaskPool* pool, s32 dataType, s32 pin);

s32 func_ov039_020989f0(TaskPool* pool, s32 dataType);

/** Feeds one helper child a point and resets it. */
void func_ov039_02094e88(void* task, OtuPoint* in);

/** Feeds one helper child a point and a fresh random offset. */
void func_ov039_02094e9c(void* task, OtuPoint* in);

/** Places one of two sprites at a point, given a bearing and a length. */
void func_ov039_02092348(void* task, s32 x, s32 y);

void func_ov039_020923b4(void* task, s32 event);

void func_ov039_02092d0c(void* task, s32 x, s32 y);

void func_ov039_02092e04(void* task, s32 event);

/** ov040's packet hand-off. */
void func_ov040_0209cb98(void* packet, s32 which);

/** Population count of a bit mask. Declared the same way Boss03.c declares it,
 *  since the overlay uses it for exactly one thing here: how many players share
 *  a top score. */
s32 func_02047e84(u16 bits);

/* --- Defined in OtuVecOps.c, which a band include cannot see -------------- */

/* --- Defined in OtuPinSprites, which is included *after* this file --------- */

/** out = a scaled vector offset, and the overlay's shared "apply a sprite
 *  action" helper (declared unprototyped there). */
s32 func_ov039_0208f00c(void* task);

// the tray slot holds a real pin
void func_ov039_0208f024(void* task);

// retract one child's sprites
s32 func_ov039_0208f0b0(void* task);

// the tray slot is empty (0x130)
void func_ov039_0208f0c8(OtuPinTask* task);

// clear the child's +0x84/+0x80 pair
void func_ov039_0208f104(void* task);

/**
 * @file OtuPinSprites.c
 *
 * Band 1 of the overlay: 0x0208f000 - 0x02090000.
 *
 * This band is four sibling sprite tasks and the handful of small helpers that
 * surround them. The four tasks are near-copies of each other and the target
 * says so explicitly: each one is a TaskHandle plus a four-entry TaskStages
 * table plus a 0x2C-byte SpriteAnimation template, and the templates' only
 * differences are a resource id and the animation frame count. The task names
 * are still in the ROM, at 0x0209a4f4 onwards:
 *
 *   0x0209930c  Tsk_OtosuGame_shadow   Init 0208f2a4  Update 0208f2f0  Render 0208f360  Release 0208f3b0
 *   0x02099354  Tsk_OtosuGame_piyo     Init 0208f5a8  Update 0208f5e0  Render 0208f6cc  Release 0208f714
 *   0x0209939c  Tsk_OtosuGame_marker   Init 0208f900  Update 0208f938  Render 0208f9b8  Release 0208fa00
 *   0x020993e4  Tsk_OtosuGame_meteo    Init 0208fbe0  Update 0208fc38  Render 0208fd8c  Release 0208fddc
 *
 * So the recurring constants across the four are resource ids (the SpriteAnimation
 * templates at 0x02099328/0x2099370/0x020993b8/0x02099400), not frame counts.
 * All four templates carry the same 0x0050 initial x/y and the same callback
 * slot, and differ only in `packIndex` (1, 2, 8, 8) and the anim/cell indices --
 * which is exactly the set a pinball table needs: one shadow, one "piyo"
 * (Japanese for the springy part), one marker and one weather effect.
 */

/* The overlay's four sprite-task resources. Unidentified: keep the names. */
extern const SpriteAnimation data_ov039_02099328;

extern const SpriteAnimation data_ov039_02099370;

extern const SpriteAnimation data_ov039_020993b8;

extern const SpriteAnimation data_ov039_02099400;

/* The four task handles and their TaskStages tables. */
extern const TaskHandle data_ov039_0209930c;

extern const TaskHandle data_ov039_02099354;

extern const TaskHandle data_ov039_0209939c;

extern const TaskHandle data_ov039_020993e4;

extern const TaskStages data_ov039_02099318;

extern const TaskStages data_ov039_02099360;

extern const TaskStages data_ov039_020993a8;

extern const TaskStages data_ov039_020993f0;

/* A 0x24-byte table passed to func_ov039_02087ba0 by the state-3 setup. */
extern const s32 data_ov039_0209a54c[9];

/* ------------------------------------------------------------------ */
/* Sibling accessors this band calls.                                  */
/*                                                                      */
/* The five defined in OtuFieldAccess.c are declared with the same     */
/* `void*` first parameter they are defined with, because OtuPinSprites.c  */
/* is #included into that file and mwcc rejects a mismatched redeclaration. */
/* ------------------------------------------------------------------ */

/* Defined in OtuFieldAccess.c below the band include; declared here so mwcc
 * does not fall back to an implicit int() prototype and then reject the real
 * definition. */
void func_ov039_02096270(void* task);

void func_ov039_02095ddc(void* task);

s32 func_ov039_0208e6f4(OtuPinTask* task);

s32 func_ov039_0208ee98(OtuPinTask* task);

s32 func_ov039_0208e890(OtuPinTask* task);

s32 func_ov039_0208e8c4(OtuPinTask* task);

s32 func_ov039_0208e950(OtuPinTask* task);

s32 func_ov039_0208a490(void* task, s32 arg2);

/* ==================================================================== */
/* The four _Sprite_Load wrappers: 0208f1f8, 0208f4fc, 0208f874,        */
/* 0208fb54.  One body, four times, three constants varied.             */
/* ==================================================================== */

/**
 * @brief The pin-shadow task's data block, 0x68 bytes.
 *
 * `Sprite` first, because _Sprite_Load is handed `self` itself (the sprite lives
 * at offset 0) and the two point pairs follow the two coordinate words the
 * wrapper writes.
 *
 * This is the only one of the four with a point pair at +0x50 and another at
 * +0x58 *and* the `+3` offset in its render step, which is what distinguishes
 * the shadow's placement from the other three.
 */
/*
 * One struct for all four tasks, deliberately minimal.
 *
 * The four tasks use the same word range for *different* things, and the
 * layouts genuinely overlap rather than sitting side by side:
 *
 *   +0x40/+0x44   piyo, marker   -- the pin's +0x110/+0x114 pair
 *   +0x48/+0x4C   piyo, marker   -- the pin's +0x120/+0x124 pair
 *   +0x44/+0x48   shadow, meteo  -- a Q12 scale pair, seeded to 0x1000
 *   +0x50/+0x54   shadow, meteo  -- the pin's +0x110/+0x114 pair
 *   +0x58/+0x5C   shadow, meteo  -- the pin's +0x120/+0x124 pair
 *
 * So +0x44 is a point's y on the piyo and a scale on the shadow, and naming both
 * readings would mean a union whose members mwcc then lays out a word apart from
 * where the target has them. Only the words the code reaches *unambiguously*
 * across all four tasks are named; the overlapping ones are read by raw offset at
 * the use site, which is also how OtuFieldAccess.c models this overlay's
 * structures.
 */
typedef struct {
    /* 0x00 */ Sprite   sprite;
    /* 0x40 */ s32      unk_40;
    /* 0x44 */ s32      unk_44;
    /* 0x48 */ s32      unk_48;
    /* 0x4C */ u16      unk_4C;
    /* 0x4E */ u16      unk_4E;
    /* 0x50 */ OtuPoint pinPosA_hi;   // shadow/meteo: the pin's +0x110/+0x114 pair
    /* 0x58 */ OtuPoint pinPosB_hi;   // shadow/meteo: the pin's +0x120/+0x124 pair
    /* 0x60 */ s32      childId;      // shadow: the pin id it tracks
    /* 0x64 */ s32      meteoChildId; // meteo: its pin id sits one word higher
    /* 0x68 */ s32      alive;        // meteo only: the kind == 8 filter's result
    /* 0x6C */ s32      state;        // meteo only: 0-3, the sub-state machine
    /* 0x70 */ s32      timer;        // meteo only: the state machine's work counter
} OtuShadowTask;

/**
 * @brief The two-word argument block the pin tasks are created with.
 *
 * `dataType` is read back by the four `_Sprite_Load` wrappers (it lands in the
 * animation template's dataType nibble); `childId` is read by the four init steps,
 * which store it as the id of the pin the new task will track.
 */
typedef struct {
    s32 dataType;
    s32 childId;
} OtuPinSpriteArgs;

// Size: 0x8

/* The four load wrappers, defined further down but called by the init steps. */
void func_ov039_0208f1f8(OtuShadowTask* self, Sprite* sprite, s32* args);

void func_ov039_0208f4fc(OtuShadowTask* self, Sprite* sprite, s32* args);

void func_ov039_0208f874(OtuShadowTask* self, Sprite* sprite, s32* args);

void func_ov039_0208fb54(OtuShadowTask* self, Sprite* sprite, s32* args);

/**
 * @brief A position plus a scale, the three-word record func_ov039_0208fee0 fills.
 *
 * Named rather than spelled as an `s32 out[3]` because the array form lets mwcc
 * merge the first two stores into one `stmia`, which the target does not do.
 */
typedef struct {
    s32 x;
    s32 y;
    s32 scale;
} OtuPinRecord;

/* ------------------------------------------------------------------ */
/* Integration notes.                                                  */
/* ------------------------------------------------------------------ */

/*
 * Two declarations outside this band have to change before it can be included,
 * and both are stale placeholders rather than real prototypes:
 *
 *   * include/Debug/Sugata/TinPinSlammer.h lines 246-260 declare fourteen of
 *     these as `void func_ov039_XXXXXXXX(void);`. Every one of them takes the
 *     scene in r0 -- it is the first thing the prologue saves -- so the
 *     definitions below conflict with them and mwcc will reject the pair. Those
 *     fourteen lines have to go (or be given the real prototype) at the same
 *     time this band lands. They are not repeated here: a redeclaration would
 *     not help, since the conflict is with the header, not with a local copy.
 *
 *   * OtuPinSprites line 73 declares `s32 func_ov039_0208a490(void*, s32);` and
 *     calls it discarding the result. The definition below matches that
 *     prototype exactly -- including the untyped first parameter, which the
 *     target's `ldr r0, [r5, #8]` does not pin down any better -- so nothing has
 *     to change there.
 *
 * Include order: this band uses `func_ov039_02098bb0`, `func_ov039_02098c00`
 * and `func_ov039_02098d10` as band 3 declares them (and `func_ov039_02098d10`
 * as the *s32* band 3 declares, not as the `void` OtuVecOps.c defines it), so
 * it has to come after OtuEntryTasks. Anything before OtuPinSprites works; the
 * natural slot is next to OtuGauge.
 */

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * All of these sit in the overlay's gap-filled `.data`/`.bss`, so declaring an
 * object of our own would put it wherever the linker chose rather than at the
 * address the target's pool word names. They keep the build's own names, for
 * the same reason and with the same caveat as the offset tables in band 13.
 */

/** The overlay-global phase byte: 1, 2, 3 or 4, raised by the stage's pollers. */
extern u8 data_ov039_0209ad00;

/** The 0x10-byte scratch buffer handed to the wireless stack with a 0x10 length. */
extern u8 data_ov039_0209b120[0x10];

/*
 * Four cross-overlay objects in ov038. `data_ov038_0209edc0` is passed to the
 * two 02047xxx entry points as an argument block, `...eef4` to
 * func_ov040_0209d970 as a name, and `...ef48` / `...ef6c` to ov003's
 * registration routine as the first of its two arguments. Note the *two* copies
 * of the last one: 02089a80 passes ...ef48 and 0208a098 passes ...ef6c. That is
 * a real difference between the two board variants, not a transcription slip.
 */
extern u32 data_ov038_0209edc0;

extern u32 data_ov038_0209eef4;

extern u32 data_ov038_0209ef48;

extern u32 data_ov038_0209ef6c;

/**
 * @brief The pin's Q12.12 speed table, sixteen bytes per entry.
 *
 * 0208a6f8 indexes it with `ldr r0, [base, index, lsl #4]` and reads the word at
 * offset 0, so the stride is sixteen *bytes* and the value is Q12.12 (the
 * entries run 0x148, 0x171, 0x19A ... 0x7AB8, 0x8000 -- i.e. 0.08 to 2.0 in
 * Q12.12, which is what the caller then multiplies a Q12.12 angle by).
 *
 * Declared with an incomplete extent: the index is a byte read out of a record
 * whose layout this band cannot see, so the count is not knowable from here.
 */
typedef struct {
    /* 0x00 */ s32 speed; // Q12.12
    /* 0x04 */ u8  pad_04[0x0C];
} OtuSpeedEntry;

// Size: 0x10

extern OtuSpeedEntry data_ov039_0209a3dc[];

/**
 * @brief One child record of the wireless board, six bytes.
 *
 * Reached only through `func_ov039_02088440`, which is
 * `data_ov039_0209af20 + index * 6`. Only the phase byte at +1 is read by
 * anything in this band, and it is read three separate times -- once per polling
 * loop, against the constants 1, 2, 3 and 4 -- which is what sequences the two
 * board variants' rounds.
 *
 * Six is a byte count, not three halfwords: the accessor is
 * `mov r1, #6; mla r0, r1, r0, base`.
 */
typedef struct {
    /* 0x00 */ u8 pad_00;
    /* 0x01 */ u8 phase; // 1, 2, 3 or 4 -- which round this child has reached
    /* 0x02 */ u8 pad_02[4];
} OtuChildRecord;

// Size: 0x6

extern OtuChildRecord data_ov039_0209af20[];

/**
 * @brief The twelve four-byte cell-type tables 0208a794 selects between.
 *
 * Each is a 2x2 grid of bit flags packed into four bytes: entry `gy32 * 2 + gx32`
 * with both halves of the division by 32. The values are drawn from the set
 * {0, 1, 0x100, 0x101}, i.e. two independent bit flags, and the mapping is
 * driven by the cell's type byte minus 0x10:
 *
 *     1 -> 48   2 -> 44   3 -> 50   4 -> 3C   5 -> 4C   6 -> 40
 *     7 -> 68   8 -> 64   9..13 -> 60   14..18 -> 5C
 *    19..23 -> 58   24..28 -> 54   anything else -> the cell's own first byte
 *
 * They are `.data`, not `.rodata`, so they are declared non-`const`.
 */
extern u8 data_ov039_0209923c[4];

extern u8 data_ov039_02099240[4];

extern u8 data_ov039_02099244[4];

extern u8 data_ov039_02099248[4];

extern u8 data_ov039_0209924c[4];

extern u8 data_ov039_02099250[4];

extern u8 data_ov039_02099254[4];

extern u8 data_ov039_02099258[4];

extern u8 data_ov039_0209925c[4];

extern u8 data_ov039_02099260[4];

extern u8 data_ov039_02099264[4];

extern u8 data_ov039_02099268[4];

/* ------------------------------------------------------------------ */
/* Callees in this overlay that are not decompiled yet.                 */
/* ------------------------------------------------------------------ */

/*
 * Signatures are inferred from the call sites, and only the ones the call sites
 * pin down are given real parameter lists -- mwcc reads an unknown `T*` as
 * `int`, which then collides with the real definition wherever one exists.
 */

/** `data_ov039_0209af20 + index * 6`; nothing else. */

/** Number of children in a child list (`dispatch->stageBlock` + 0x144). */
s32 func_ov039_020883ac(TinPinSlammer_Scene* scene);

/** The `index`th child of a child list, resolved out of pool 2. */
void* func_ov039_020883c8(TinPinSlammer_Scene* scene, s32 index);

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
s32 func_ov039_020885f4(void* record, TinPinSlammer_Scene* scene);

/** Wireless receive callback: (record, scene). Reached only as a function pointer. */
void func_ov039_02088698(void* record, TinPinSlammer_Scene* scene);

/** The stage's "board has changed" entry; clears state.unk_698 and re-seeds. */
void func_ov039_02088ec4(TinPinSlammer_Scene* scene);

/** The stage's board teardown. */
void func_ov039_02088f18(TinPinSlammer_Scene* scene);

/** The board's per-frame physics, run at the end of both tick routines. */
void func_ov039_020894cc(TinPinSlammer_Scene* scene);

/** Creates the second child task; returns its handle. */

/* OtuVecOps.c's three helpers, declared here because nothing inside this
 * translation unit already declares them and an implicit `int (...)` would lose
 * the `OtuPoint*` parameters. The prototypes are copied verbatim from their
 * definitions. Note that func_ov039_02098d10 is deliberately *not* repeated: band
 * 3 declares it `s32` and OtuVecOps.c defines it `void`, and this band needs the
 * value, so band 3's declaration is the one that has to win in the include
 * order. That inconsistency is pre-existing. */
s32 func_ov039_02098c40(OtuPoint* a, OtuPoint* b);

void func_ov039_02098d3c(OtuPoint* src, OtuPoint* dest);

/* 02087cac is defined in OtuCountdown.c, so this TU needs its own declaration
 * before it can be named as the wireless stack's setup callback. */
void func_ov039_02087cac(TinPinSlammer_Scene* scene, s32 slot, const void* src, s32 len);

/*
 * Overlay 40's wireless stack, plus the two 02047xxx/02044xxx engine entry
 * points the board's setup calls. All void except where noted.
 */
s32 func_ov040_0209cb68(void);

s32 func_ov040_0209cde4(void);

void func_ov040_0209caac(s32 arg0);

void func_ov040_0209cb9c(void);

void func_ov040_0209cb08(void* state, s32 arg1);

void func_ov040_0209cabc(void* state, s32 arg1);

void func_ov040_0209c158(void);

void func_ov040_0209ba04(void (*cb)(void*, TinPinSlammer_Scene*), void* scene, void* buf, s32 arg3);

void func_ov040_0209d0a8(s32 arg0, s32 arg1, s32 arg2);

void func_ov040_0209d290(s32 arg0, void* arg1);

void func_ov040_0209d40c(void (*cb)(), void* arg);

void func_ov040_0209d420(void (*cb)(), void* arg);

void func_ov040_0209d588(void);

s32 func_ov040_0209d728(void);

void func_ov040_0209d818(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);

void func_ov040_0209d848(s32 arg0);

void func_ov040_0209ece8(u8* buf, s32 len);

void func_ov040_0209ec5c(void (*cb)(TinPinSlammer_Scene*, s32, const void*, s32), void* scene, void* buf, s32 len);

void func_ov040_0209ed58(s32 arg0);

void func_ov040_0209ef88(void);

void func_ov040_0209efc0(void);

s32 func_020442f8(void);

void func_020471ec(s32 a0, s32 a1, void* arg);

void func_020472a8(void* arg);

void func_02047338(void);

void func_0204737c(void);

s32 func_02047e84(u16 arg0);

/* ------------------------------------------------------------------ */
/* Task-data shapes.                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief The wireless board's stage block, 0x2D0 bytes.
 *
 * Reached as `func_ov039_02098b70(OTU_STAGE(scene))`, which is a one-instruction
 * accessor for `dispatch->stageBlock`. The size is not an estimate: it is the
 * `size` word the header's `OtuScene_WirelessBoard` descriptor carries, and
 * 0x2CC is the last field anything here touches.
 *
 * Every field below is placed on the offset at least one load or store in this
 * band uses. Note that +0x140 and +0x144 are two *different* counts: +0x140 is
 * latched from `state.unk_EE0` by the init stage and is what the six
 * child-polling loops bound themselves by (i.e. the shared `OTU_CHILD_COUNT`
 * field), while +0x144 is what `func_ov039_020883ac` reports and what
 * 0x0208a988's placement loop walks. Conflating them is the easy mistake here.
 *
 * The three words at 0x2C4, 0x2C8 and 0x2CC are task handles: 0x2C8
 * and 0x2CC are deleted by the cleanup and by stage 8 respectively, and 0x2C4 is
 * resolved through pool 1 by both tick routines. The child-handle arrays are at
 * +0x17C and +0x28C in the stage block layout; they are accessed by offset rather
 * than as named fields in this struct definition.
 */
typedef struct {
    /* 0x000 */ s32 unk_00;     // the board stage's dispatch selector, 0..8
    /* 0x004 */ u16 unk_04;     // handshake flag; the two board variants set it either way
    /* 0x006 */ u16 unk_06;     // two bytes of wireless state, pushed as a pair
    /* 0x008 */ s32 state;      // 0..8, the selector both update routines switch on
    /* 0x00C */ u8  pad_00C[0x134];
    /* 0x140 */ s32 childCount; // latched from state.unk_EE0; number of pin children
    /* 0x144 */ s32 subCount;   // func_ov039_020883ac's answer: a *different* count
    /* 0x148 */ u8  pad_148[4];
    /* 0x14C */ s32 slotIndex;  // written from the 0x44000 block's +0xA68
    /* 0x150 */ u8  pad_150[0x20];
    /* 0x170 */ s32 unk_170;    // written from data_ov039_0209a360[slotIndex]
    /* 0x174 */ s32 timer;      // 0x258 on entry, counted down every frame
    /* 0x178 */ u8  pad_178[0x14C];
    /* 0x2C4 */ s32 tickTask;   // handle resolved through pool 1 by the tick stages
    /* 0x2C8 */ s32 initTask;   // handle deleted by the cleanup
    /* 0x2CC */ s32 fadeTask;   // handle deleted by stage 8
} OtuWirelessStage;

// Size: 0x2D0

/**
 * @brief The pin task's own state block.
 *
 * This is the same object the shared header calls `OtuPinTask` -- +0x120/+0x124
 * are that type's `x`/`y` and +0x16C its `pinID` -- but the header's version
 * stops at 0x174 and these routines reach 0x228, so the wider shape is declared
 * here rather than by widening a type that matched code already depends on.
 *
 * The five consecutive two-word pairs at +0x110, +0x118, +0x120, +0x128 and
 * +0x138 are the overlay's five OtuPoint-shaped slots; `OtuFieldAccess.c` gives
 * each of them a getter/setter/copy-out triple, which is what identifies them.
 * Only +0x110 and +0x120 are used here, and they are the `src` and `dst` pair
 * every `func_ov039_02087d04` call in the overlay takes.
 */
typedef struct {
    /* 0x000 */ u8        pad_000[8];
    /* 0x008 */ TaskPool* pool;       // the pool every handle below is resolved through
    /* 0x00C */ u8        pad_00C[0x104];
    /* 0x110 */ OtuPoint  anchor;     // the `src` of func_ov039_02087d04
    /* 0x118 */ u8        pad_118[8];
    /* 0x120 */ OtuPoint  pos;        // the `dst` of func_ov039_02087d04
    /* 0x128 */ u8        pad_128[4];
    /* 0x12C */ OtuPoint  vel;        // clamped to length 0x8000 by 0208a6c4
    /* 0x134 */ u8        pad_134[4];
    /* 0x138 */ OtuPoint  dir;        // vel re-normalised into here when vel is non-zero
    /* 0x140 */ u8        pad_140[0x2C];
    /* 0x16C */ u16*      pinID;      // a tray slot; 0x130 there means "no pin"
    /* 0x170 */ u8*       slots;      // OtuBoardSlot[], indexed by *pinID
    /* 0x174 */ u8        pad_174[0x2C];
    /* 0x1A0 */ s16       slot;       // round-robins over the two children at +0x228
    /* 0x1A2 */ u8        pad_1A2[0xA];
    /* 0x1AC */ s32       total;      // accumulated score, clamped to 0x3E7
    /* 0x1B0 */ u8        pad_1B0[0x20];
    /* 0x1D0 */ s32       mode;       // 0x10 selects one of two blend modes
    /* 0x1D4 */ u8        pad_1D4[0x20];
    /* 0x1F4 */ s32       counterId;  // handle of the task 02093d68 drives
    /* 0x1F8 */ u8        pad_1F8[0x30];
    /* 0x228 */ s32       childId[2]; // the two children the score is split between
} OtuPinLogic;

// Size: 0x230

/**
 * @brief One board slot's record, 0x1C bytes.
 *
 * The pin task's +0x170 points at an array of these, indexed by the pin id in
 * its tray slot with a *byte* stride of 0x1C. Only the byte at +4 is read here,
 * and it is an index into `data_ov039_0209a3dc` rather than a value.
 */
typedef struct {
    /* 0x00 */ u8 pad_00[4];
    /* 0x04 */ u8 speedIndex;
    /* 0x05 */ u8 pad_05[0x17];
} OtuBoardSlot;

// Size: 0x1C

/**
 * @brief The board's cell grid, as 0208a794 sees it.
 *
 * Two fields are read: a byte scale at +0x02 that multiplies the coarse row
 * term, and a byte pointer at +0x10 to an array of four-byte cell records whose
 * first two bytes are two independent states. Nothing else in this band (or in
 * anything else decompiled so far) reaches into it, so the rest is padding kept
 * only to hold the two offsets.
 */
typedef struct {
    /* 0x00 */ u8  pad_00[2];
    /* 0x02 */ u8  rowScale; // how many cell rows one screen row spans
    /* 0x03 */ u8  pad_03[0x0D];
    /* 0x10 */ u8* cells;    // four bytes per entry: two state bytes, two unused
} OtuCellGrid;

// Size: 0x14

/**
 * @brief The change-mask latch 0208ac98 keeps.
 *
 * A pointer to a live 16-bit state word, the value that word held last time, and
 * the bits that have changed since. Only the three fields at +0xE8..+0xEF are
 * known, so the stated size is the extent of that, not the object's real one.
 */
typedef struct {
    u8               pad_000[0xE8];
    /* 0x0E8 */ u16* state; // the live word; the one read is at its own +0x04
    /* 0x0EC */ u16  last;
    /* 0x0EE */ u16  changed;
} OtuInputLatch;

/** One Q12.12 multiply, rounded to nearest. */
static inline s32 OtuQ12Mul(s32 a, s32 b) {
    return (s32)(((s64)a * b + 0x800) >> 12);
}

/* ------------------------------------------------------------------ */
/* Data the band reads but does not define.                            */
/* ------------------------------------------------------------------ */

/*
 * Every symbol here lives in the overlay's own gap-filled `.data` and keeps
 * the build's name for it. Declaring an object of our own would let the
 * linker place it at an address of its choosing rather than the one the target
 * uses, which costs the pool word; see the same note in band 5 and band 13.
 */

/** 0x133 -- the badge's per-frame acceleration, in Q12.12. */
extern s32 data_ov039_0209a318;

/** -0x2000 -- the velocity the phase-7 entry sets going through a corner. */
extern s32 data_ov039_0209a31c;

/** -0x2000 again, as the phase-9 entry's terminal velocity. */
extern s32 data_ov039_0209a320;

/** 0x800 -- the bounce re-normalisation factor. */
extern s32 data_ov039_0209a324;

/** 0x8000 -- the axis speed the four tile-driven launch cases set. */
extern s32 data_ov039_0209a308;

/*
 * The movement-rate table, four words, is declared as
 * `data_ov039_0209a39c[4]` in TinPinSlammer.h -- func_ov039_02084020 and
 * func_ov039_020840c0 reach the same four words from OtuScoreRow.c.
 */

/**
 * @brief One badge kind's speed record: a 16-byte-stride Q12.12 multiplier.
 *
 * Indexed by the behaviour record's +0x04 byte, which is why the target gets
 * the stride as a shift rather than as a scale. Only the leading word is read
 * by this band; the other three are whatever the board data carries.
 */
typedef struct {
    /* 0x00 */ s32 speed; // Q12.12 multiplier applied to the badge's velocity
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} OtuBadgeSpeed;

// Size: 0x10
extern OtuBadgeSpeed data_ov039_0209a3e0[];

/**
 * @brief The two scale tables 0x0208e130 indexes, four words apart, 0x10 stride.
 *
 *  Both are read as `ldr rX, [base, index, lsl #4]`, and the bases are four
 *  bytes apart, so they are one array of 0x10-byte records read at two
 *  different field offsets rather than two independent tables -- the same
 *  array as OtuBadgeSpeed just above, reached at its +0x04 and +0x08 instead
 *  of its +0x00. Declared as two so each pool word names the address the
 *  target names.
 *
 *  This is the array OtuBadgeSlot.tuneIndex points into, which is the third
 *  piece of evidence that the byte at record+0x04 is one index rather than the
 *  unrelated `speedIndex` and `weight` the two badge views each called it.
 */
typedef struct {
    /* 0x00 */ s32 scaleA;
    /* 0x04 */ s32 scaleB;
    u8             pad_08[0x08];
} OtuPinScale;

// Size: 0x10

/* OtuPinRow is gone: it was these same 0x1C bytes seen from the other end,
 * so there is no second record and no second cast. See OtuBadgeSlot. */

/**
 * @brief The nine-word block the init stage reads and the spawner builds.
 *
 * Built in the outgoing-argument area of 0x0208dcb0 as nine consecutive
 * words at sp+8..sp+0x28 and handed to EasyTask_CreateTask as its `param`,
 * which is why the frame is 0x48 with the block's own pointer at sp+4.
 */
typedef struct {
    /* 0x00 */ s32   unk_00; // -> self + 0x004, the SpriteAnimation dataType
    /* 0x04 */ s32   unk_04; // -> self + 0x0E0
    /* 0x08 */ void* unk_08; // -> self + 0x0E8
    /* 0x0C */ u8*   unk_0C; // -> self + 0x0E4
    /* 0x10 */ s32   unk_10; // -> self + 0x000
    /* 0x14 */ u8*   rowTable;
    /* 0x18 */ u16*  pinID;
    /* 0x1C */ s32   unk_1C; // -> self + 0x1A8
    /* 0x20 */ s32   unk_20; // -> self + 0x174
} OtuBadge_InitArgs;

// Size: 0x24

/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/* Band 12's own functions, declared here because they are called from
   earlier code above. Without this each such call becomes an implicit
   `int (...)` and the definition below is then a redeclaration of it. */
/* ------------------------------------------------------------------ */
/* Callees outside this batch. These must sit above every use: with no
   prototype a call becomes an implicit `int (...)`, and a prototype
   below the call is then a redeclaration of it. Declaring them at the
   foot of the file, where this block started, cost 39 errors.        */
/* ------------------------------------------------------------------ */
extern const SpriteAnimation data_ov039_02099288;

extern const SpriteAnimation data_ov039_020992b4;

extern const SpriteAnimation data_ov039_020992e0;

extern const TaskStages data_ov039_02099278;

extern const TaskHandle data_ov039_0209926c;

extern const BinIdentifier data_ov039_0209a0b4[5];

extern const OtuPinScale data_ov039_0209a3e4[];

extern const OtuPinScale data_ov039_0209a3e8[];

extern void func_ov039_0208c9cc(OtuBadge* self);

extern void func_ov039_0208ca1c(OtuBadge* self);

extern void func_ov039_0208cb64(OtuBadge* self);

extern void func_ov039_0208cc4c(OtuBadge* self);

extern void func_ov039_0208cd50(OtuBadge* self);

extern void func_ov039_0208ce20(OtuBadge* self);

extern void func_ov039_0208ce54(OtuBadge* self);

extern void func_ov039_0208ce88(OtuBadge* self);

extern void func_ov039_0208d210(OtuBadge* self);

extern s32 func_ov039_020915a8(TaskPool* pool, s32 dataType, s32 childId);

extern s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 childId);

/* ============================================================================
 * Band 15 -- the badge AI: per-kind "should this pin act now?" tests, the chase
 * and homing states, and the per-frame update.
 *
 * Nineteen functions, 0x0208c4ec to 0x0208ce54. Every body here was transcribed
 * from objdiff's own target disassembly, not from build/usa/asm/ov039_4.s --
 * that file is a stale March snapshot of a source state that no longer exists,
 * and reading it cost this band a whole false start.
 *
 * The first five are one family and differ only in four numbers: which entry of
 * self->chanceTbl is the roll, which timer is compared against zero, whether
 * the speed cap applies, the distance limit, and which per-kind update runs.
 * They are written out separately rather than shared through a helper, because
 * that is how the target has them -- five separate bodies, five separate
 * addresses, no shared tail.
 * =========================================================================*/

/* The fade-manager block the target reaches as `.word gFaders`. Only the
 * word at +8 is touched by this band. */
extern u32 gFaders[];

/* The per-kind dispatch table 0x0208c9cc walks, one function pointer per
 * entry, indexed from 1 to 0x10 inclusive. */
extern u32 data_ov039_0209a4b0[];

/* Callees with no owner yet. Declared here rather than relied on implicitly:
 * under -lang=c99 an undeclared call becomes an implicit int(...), and the
 * pointer arguments below are then rejected. */
extern OtuPinTask* func_ov039_0208817c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

extern OtuPinTask* func_ov039_02088294(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

extern OtuPinTask* func_ov039_02087e2c(TaskPool* pool, TinPinSlammer_Scene* scene, s32 index);

extern s32 func_ov039_02097ad8(void* data);

extern s32 func_ov039_0209167c(void* data);

extern s32 func_ov039_02091014(void* data);

extern u8 data_ov039_0209ad00;

extern s32 data_ov039_0209ad04;

extern s32 data_ov039_0209ad08[4];

/* The task handles the result-screen spawners hand to EasyTask_CreateTask. */
extern const TaskHandle data_ov039_0209a06c;

extern const TaskHandle data_ov039_02099c64;

extern const TaskHandle data_ov039_020995a4;

extern const TaskHandle data_ov039_020995ec;

extern const TaskHandle data_ov039_02099998;

/* The stage tables the four-word dispatchers copy and index. */
extern const TaskStages data_ov039_02099568;

extern const TaskStages data_ov039_020995b0;

extern const TaskStages data_ov039_02099610;

extern const TaskStages data_ov039_020999a4;

extern const TaskStages data_ov039_0209a030;

extern const TaskStages data_ov039_0209a078;

/*
 * The four sprite-load helpers the parameter initialisers hand the block to.
 * All of them are the OtuTaskStages loader shape and all of them take the
 * caller's `args` straight through in r2 -- which is why the callers never
 * reload it, and why the constant-zero register in the callers moves off r2.
 */
void func_ov039_02091788(void* self, void* sprite, s32* args);

void func_ov039_02094bac(void* self, void* sprite, s32* args);

void func_ov039_020911dc(void* self, void* sprite, s32* args);

void func_ov039_020980b8(void* self, void* sprite, s32* args);

extern const TaskHandle data_ov039_0209955c;

extern const SpriteAnimation data_ov039_0209a088;

extern s32* data_ov039_0209a620[];

/* The child-tile setup helper func_ov039_020929ec drives. */
void func_ov039_0209276c(void* self, s32 paramsAddr);

/* The 7- and 4-argument cell set-up pair func_ov039_0209276c drives. */
void func_0200d898(void* buf, void* src, s32 w, s32 h);

void func_0200d1d8(void* obj, s32 a, s32 b, s32 c, void* buf, s32 w, s32 h);

void func_0200d858(void* obj, s32 a, s32 b, s32 c);

/* The per-child step func_ov039_02091070 runs four times. */
void func_ov039_020983c8(void* data, OtuPoint* pt, void* table, s32 index);

/* The two more func_ov039_0208e9f8 dispatches to, alongside +0x1628. */
s32 func_ov039_02090e9c(OtuHammer* self, OtuBar* out);

void func_ov039_0208fee0(OtuShadowTask* self, OtuPinRecord* out);

/* The three Q12.12 constants func_ov039_02094ca8 damps with. */
extern s32 data_ov039_0209a300;

extern s32 data_ov039_0209a304;

extern s32 data_ov039_0209a310;

/* The four more func_ov039_02094e9c randomises its launch between. */
extern s32 data_ov039_0209a2fc;

extern s32 data_ov039_0209a30c;

extern s32 data_ov039_0209a314;

extern s32 data_ov039_0209a328;

/** The 4-word template func_ov039_020983c8 copies onto the sprite block. */
typedef struct {
    /* 0x00 */ s32 w0;
    /* 0x04 */ s32 w1;
    /* 0x08 */ s32 w2;
    /* 0x0C */ s32 w3;
} OtuWord4;

// Size: 0x10
extern const SpriteAnimation data_ov039_020995c0;

extern const SpriteAnimation data_ov039_02099578;

extern const SpriteAnimation data_ov039_02099c80;

extern const SpriteAnimation data_ov039_0209a040;

extern const SpriteAnimation data_ov039_02099504;

extern const SpriteAnimation data_ov039_02099530;

/* ------------------------------------------------------------------ */
/* The wireless input snapshot, 0x02088418 - 0x020885f4.               */
/* ------------------------------------------------------------------ */

/**
 * @brief One player's touch-pad descriptor, six bytes.
 *
 * `data_ov039_0209ad20` is one of these and `data_ov039_0209af20` is an array of
 * them; `func_ov039_02088440` is `data_ov039_0209af20 + index * 6`. The first
 * byte is the "was touched" latch the poller resets, the next two are the touch
 * coordinate bytes, the fourth is the phase byte the board's loops read, and
 * +4/+5 are the raw `SysControl` word copied in each poll.
 */
typedef struct {
    /* 0x00 */ u8  touched;
    /* 0x01 */ u8  phase;
    /* 0x02 */ u8  x;
    /* 0x03 */ u8  y;
    /* 0x04 */ u16 sysControl;
} OtuPadState;

// Size: 0x6

extern OtuPadState data_ov039_0209ad20;

/** A four-entry handler table copied to the stack before one is called. */
extern const TaskStages data_ov039_02099c70;

/** A 0x28-strided flag slot: only its first word is touched. */
typedef struct {
    /* 0x00 */ s32 flags;
    /* 0x04 */ u8  pad_04[0x24];
} OtuFlagSlot;

/**
 * @brief The three-word point-and-scale record `func_ov039_02091628` fills in.
 *
 * This is OtuPinRecord: same three words, same field names, same role as an
 * out-parameter. The two functions differ only in how the target stores them --
 * 0208fee0 writes three separate `str` at +0/+4/+8, while 02091628 writes
 * `stmia r1, {r2, r3}` then one `str`. The difference comes from the source
 * around the stores, not from this type, so one type serves both.
 */
typedef OtuPinRecord OtuPoint3;

/** The child-task group stepped by `func_ov039_02091690`. */
typedef struct {
    /* 0x00 */ TaskPool*     pool;
    /* 0x04 */ u8            pad_04[0x58];
    /* 0x5C */ OtuTaskParams params;
    /* 0x64 */ u8            pad_64[0x20];
    /* 0x84 */ s32           ids[8];
} OtuChildGroup;

/* ------------------------------------------------------------------ */
/* The six-word record reset, the sprite step pair and the child       */
/* teardown.                                                           */
/* ------------------------------------------------------------------ */

/** The flat six-word record `func_ov039_02098a20` zeroes out. */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} OtuRec6;

/** The two-wide background slot group torn down by `func_ov039_02092bf8`. */
typedef struct {
    /* 0x00 */ u8               pad_00[0x04];
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ s32              flag;
    /* 0x0C */ u8               pad_0C[0x04];
    /* 0x10 */ Data*            dataIds[2];
    /* 0x18 */ PaletteResource* paletteIds[2];
    /* 0x20 */ BgResource*      charIds[2];
    /* 0x28 */ u8               pad_28[0x50];
    /* 0x78 */ void*            buffers[2];
} OtuBgGroup;

/** The block `func_ov039_02092d0c` drives two display groups out of. */
typedef struct {
    /* 0x00 */ u8  pad_00[0x08];
    /* 0x08 */ s32 guard;
    /* 0x0C */ u8  pad_0C[0x1C];
    /* 0x28 */ s32 a1;
    /* 0x2C */ s32 b1;
    /* 0x30 */ u8  pad_30[0x20];
    /* 0x50 */ s32 a2;
    /* 0x54 */ s32 b2;
    /* 0x58 */ u8  pad_58[0x38];
    /* 0x90 */ s32 u1;
    /* 0x94 */ s32 u2;
    /* 0x98 */ s32 v1;
    /* 0x9C */ s32 v2;
} OtuBoardPair;

/** The block `func_ov039_020929ec` stores its params into and then sets up. */
typedef struct {
    /* 0x00 */ s32 word0;
    /* 0x04 */ s32 word1;
    /* 0x08 */ s32 active;
    /* 0x0C */ u8* selector;
    /* 0x10 */ u8  pad_10[0x80];
    /* 0x90 */ s32 flagsA[2];
    /* 0x98 */ s32 flagsB[2];
} OtuTileSetup;

/** The four-child arc group stepped by `func_ov039_02091070`. */
typedef struct {
    /* 0x00 */ TaskPool* pool;
    /* 0x04 */ u8        pad_04[0xA8];
    /* 0xAC */ OtuPoint  home;
    /* 0xB4 */ u8        pad_B4[0x08];
    /* 0xBC */ OtuPoint  center;
    /* 0xC4 */ u8        pad_C4[0x64];
    /* 0x128 */ s32      ids[4];
} OtuArcGroup;

/* ------------------------------------------------------------------ */
/* The +0x276c resource-allocation setup.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief A 0x28-byte per-slot object the overlay never looks inside.
 *
 * All padding is correct here, not laziness. The two uses in OtuObstacles.c are
 * `&self->slots[params->slot]` handed straight to func_0200d1d8 and
 * func_0200d858, and both take their object as `void*` -- they are the engine's
 * OBJ-resource helpers and this block is theirs. func_ov039_0209276c fills the
 * surrounding `OtuResGroup` (loading the data and palette ids, allocating the
 * buffers) but never touches the slot bodies.
 *
 * So the struct carries no fields on purpose; what it does carry is the 0x28
 * stride and the fact that there are two of them, which is what makes
 * `slots[params->slot]` type-check.
 */
typedef struct {
    /* 0x00 */ u8 pad[0x28];
} OtuResSlot;

// Size: 0x28

/** The per-slot parameter block func_ov039_0209276c reads. */
typedef struct {
    /* 0x00 */ s32 slot;
    /* 0x04 */ s32 width;
    /* 0x08 */ s32 height;
    /* 0x0C */ s32 fileId;
    /* 0x10 */ s32 group;
    /* 0x14 */ s32 layers;
    /* 0x18 */ s32 priority;
    /* 0x1C */ s32 idx[2];
    /* 0x24 */ s32 pad_24;
    /* 0x28 */ s32 idx1;
    /* 0x2C */ s32 idx2;
    /* 0x30 */ s32 palStart;
    /* 0x34 */ s32 palCount;
} OtuResParams;

// Size: 0x38

/** The block func_ov039_0209276c loads and allocates into. */
typedef struct {
    /* 0x00 */ s32              dataType;
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ u8               pad_08[0x04];
    /* 0x0C */ u8*              selector;
    /* 0x10 */ Data*            data[2];
    /* 0x18 */ PaletteResource* palettes[2];
    /* 0x20 */ BgResource*      chars[2];
    /* 0x28 */ OtuResSlot       slots[2];
    /* 0x78 */ void*            buffers[2];
    /* 0x80 */ s32              widths[2];
    /* 0x88 */ s32              heights[2];
} OtuResGroup;

/* ------------------------------------------------------------------ */
/* The board-shape background loader, 0x02098650.                      */
/* ------------------------------------------------------------------ */

/** The 0x27-sized bin the loader pulls, plus its three sub-objects. */
extern const BinIdentifier data_ov039_0209a0e4;

/* ------------------------------------------------------------------ */
/* The wrestling-banner board loader, 0x02092f88.                      */
/* ------------------------------------------------------------------ */

/** The board's bin id and the 0x12-wide frame-slot table. */
extern const BinIdentifier data_ov039_0209a0f4;

extern const void* data_ov039_020999d0;

/* ------------------------------------------------------------------ */
/* The wrestling-board switchboard block, 0x02091b98.                  */
/* ------------------------------------------------------------------ */

/** The 0x40-byte table slicer copied to the bottom-half of the grid. */
extern s32 data_ov039_020995f8[];

extern s32 data_ov039_02099604[];

extern s32 data_ov039_0209968c[];

extern s32 data_ov039_02099620[];

extern s32 data_ov039_02099630[];

extern s32 data_ov039_0209964c[];

extern s32 data_ov039_0209966c[];

extern s32 data_ov039_020996b0[];

extern const BinIdentifier data_ov039_0209a0bc[4];

extern u8* data_ov039_0209a5d8[3];

/** The whole-word block shapes the loader's three stack tables copy out to. */
typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
} OtuWord3;

// Size: 0xC

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
    s32 w8;
} OtuWord9;

s32            func_ov039_02088400(s32 x, s32 y, s32 axis);
void           func_ov039_02088454(void);
void           func_ov039_020885cc(void);
void           func_ov039_02088688(s32 unused, TinPinSlammer_Scene* scene);
void           func_ov039_0208871c(TinPinSlammer_Scene* scene);
void           func_ov039_020888e0(TinPinSlammer_Scene* scene);
void           func_ov039_02088a8c(TinPinSlammer_Scene* scene);
void           func_ov039_02088b80(TinPinSlammer_Scene* scene);
void           func_ov039_02088c50(TinPinSlammer_Scene* scene);
void           func_ov039_02088df0(TinPinSlammer_Scene* scene);
void           func_ov039_02088fac(TinPinSlammer_Scene* scene);
void           func_ov039_02089064(TinPinSlammer_Scene* scene, OtuPoint* at);
void           func_ov039_020890d8(TinPinSlammer_Scene* scene, OtuPinTask* a, OtuPinTask* b);
void           func_ov039_020891fc(TinPinSlammer_Scene* scene);
void           func_ov039_020892c4(TinPinSlammer_Scene* scene);
void           func_ov039_02089360(TinPinSlammer_Scene* scene);
s32            func_ov039_020893fc(TinPinSlammer_Scene* scene);
void           func_ov039_02089780(TinPinSlammer_Scene* scene);
void           func_ov039_020897d0(TinPinSlammer_Scene* scene);
void           func_ov039_020897dc(TinPinSlammer_Scene* scene);
void           func_ov039_020898a8(TinPinSlammer_Scene* scene);
void           func_ov039_02089918(TinPinSlammer_Scene* scene);
void           func_ov039_02089950(TinPinSlammer_Scene* scene);
void           func_ov039_02089a80(TinPinSlammer_Scene* scene);
void           func_ov039_02089d3c(TinPinSlammer_Scene* scene);
void           func_ov039_02089d6c(TinPinSlammer_Scene* scene);
void           func_ov039_02089e0c(TinPinSlammer_Scene* scene);
void           func_ov039_02089e78(TinPinSlammer_Scene* scene);
void           func_ov039_02089ec0(TinPinSlammer_Scene* scene);
void           func_ov039_02089f30(TinPinSlammer_Scene* scene);
void           func_ov039_02089f68(TinPinSlammer_Scene* scene);
void           func_ov039_0208a098(TinPinSlammer_Scene* scene);
void           func_ov039_0208a324(TinPinSlammer_Scene* scene);
void           func_ov039_0208a354(TinPinSlammer_Scene* scene);
void           func_ov039_0208a3f4(TinPinSlammer_Scene* scene);
void           func_ov039_0208a454(TinPinSlammer_Scene* scene);
void           func_ov039_0208a6c4(OtuPoint* v);
void           func_ov039_0208a6f8(OtuPinLogic* self, OtuPoint* dir, s32 angle);
u8             func_ov039_0208a794(OtuPoint* p, OtuCellGrid* grid);
s32            func_ov039_0208a988(OtuPoint* self, OtuCellGrid* grid, TinPinSlammer_Scene* scene);
void           func_ov039_0208ac98(OtuInputLatch* self);
void           func_ov039_0208acc0(OtuBadgeState* self);
void           func_ov039_0208ad2c(OtuPoint* vel, s32 shortfall);
void           func_ov039_0208ad88(OtuBadgeState* self);
void           func_ov039_0208ade8(OtuBadgeState* self);
void           func_ov039_0208ae14(OtuBadgeState* self);
void           func_ov039_0208ae8c(OtuBadgeState* self);
void           func_ov039_0208af38(OtuBadgeState* self);
void           func_ov039_0208af6c(OtuBadgeState* self);
void           func_ov039_0208b94c(OtuBadgeState* self);
void           func_ov039_0208b9b4(OtuBadgeState* self);
void           func_ov039_0208ba34(OtuBadgeState* self);
void           func_ov039_0208ba70(OtuBadgeState* self);
void           func_ov039_0208bb2c(OtuBadgeState* self);
void           func_ov039_0208be30(OtuBadgeState* self, OtuPoint* dir);
s32            func_ov039_0208bed8(OtuBadgeState* self, s32 rings, s32 loType, s32 hiType, OtuPoint* out);
s32            func_ov039_0208c0e4(OtuPoint* a, OtuPoint* b);
s32            func_ov039_0208c128(OtuBadgeState* self, s32 which, s32 loType, s32 hiType, s32 kind);
s32            func_ov039_0208c218(OtuBadgeState* self);
s32            func_ov039_0208c258(OtuBadgeState* self);
s32            func_ov039_0208c304(OtuBadgeState* self);
s32            func_ov039_0208c3bc(OtuBadgeState* self);
s32            func_ov039_0208c45c(OtuBadgeState* self);
s32            func_ov039_0208c4ec(OtuBadgeState* self);
s32            func_ov039_0208c568(OtuBadgeState* self);
s32            func_ov039_0208c5f8(OtuBadgeState* self);
s32            func_ov039_0208c688(OtuBadgeState* self);
s32            func_ov039_0208c718(OtuBadgeState* self);
s32            func_ov039_0208c794(OtuBadgeState* self);
s32            func_ov039_0208c84c(OtuBadgeState* self);
void           func_ov039_0208c8ac(OtuBadgeState* self);
void           func_ov039_0208c8cc(OtuBadgeState* self);
void           func_ov039_0208c8ec(OtuBadgeState* self);
void           func_ov039_0208c90c(OtuBadgeState* self);
s32            func_ov039_0208c92c(OtuBadgeState* self);
OtuSpriteSlot* func_ov039_0208d2f8(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_0208d3bc(OtuBadge* self, Sprite* sprite);
void           func_ov039_0208d4cc(OtuBadge* self, Sprite* sprite);
void           func_ov039_0208d554(OtuBadge* self, Sprite* sprite);
void           func_ov039_0208d5dc(OtuBadge* self);
s32            func_ov039_0208d6dc(TaskPool* pool, Task* task, OtuBadge_InitArgs* args);
s32            func_ov039_0208d7e4(TaskPool* pool, Task* task, void* args);
void           func_ov039_0208d9ec(OtuBadge* self);
s32            func_ov039_0208da74(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208db44(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208dc68(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_0208df2c(OtuPoint* posA, OtuPoint* velA, s32 reachA, OtuPoint* posB, OtuPoint* velB, s32 reachB);
void           func_ov039_0208dff0(OtuPoint* dir, s32 speed, s32 scaleA, s32 scaleB, OtuBadge* other);
void           func_ov039_0208e058(OtuPoint* posA, OtuPoint* velA, OtuPoint* posB, OtuPoint* velB, OtuPoint* out, s32* score);
void           func_ov039_0208e130(OtuBadge* self, OtuBadge* other);
void           func_ov039_0208e6cc(void* task, OtuPoint* out);
void           func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out);
void           func_ov039_0208e848(void* task, OtuPoint* in);
void           func_ov039_0208e870(void* task, s32 x, s32 y);
s32            func_ov039_0208e984(OtuPinTask* task);
s32            func_ov039_0208e998(OtuPinTask* task);
s32            func_ov039_0208e9d0(OtuPinTask* task);
s32            func_ov039_0208ee84(OtuPinTask* task);
s16            func_ov039_0208eea0(OtuPinTask* task);
s16            func_ov039_0208eeac(OtuPinTask* task);
s16            func_ov039_0208eeb8(OtuPinTask* task);
s16            func_ov039_0208eec4(OtuPinTask* task);
s32            func_ov039_0208efb0(OtuPinTask* task, s32 which);
s32            func_ov039_0208eff8(OtuPinTask* task);
s32            func_ov039_0208f034(OtuPinTask* task);
void           func_ov039_0208f03c(OtuPinTask* task);
void           func_ov039_0208f0f0(void* task, s32 value);
OtuSpriteSlot* func_ov039_0208f134(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_0208f2a4(TaskPool* pool, Task* task, OtuPinSpriteArgs* args);
s32            func_ov039_0208f2f0(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f360(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f3b0(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f3c4(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_0208f40c(TaskPool* pool, s32 dataType, s32 childId);
OtuSpriteSlot* func_ov039_0208f440(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_0208f5a8(TaskPool* pool, Task* task, OtuPinSpriteArgs* args);
s32            func_ov039_0208f5e0(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f6cc(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f714(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f728(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_0208f770(TaskPool* pool, s32 dataType, s32 childId);
void           func_ov039_0208f7a4(void* task, s32 value);
OtuSpriteSlot* func_ov039_0208f7b4(OtuSpriteTask* t, s32 arg, s32 sel);
s32            func_ov039_0208f900(TaskPool* pool, Task* task, OtuPinSpriteArgs* args);
s32            func_ov039_0208f938(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208f9b8(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208fa00(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208fa14(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_0208fa5c(TaskPool* pool, s32 dataType, s32 childId);
OtuSpriteSlot* func_ov039_0208fa90(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_0208fbe0(TaskPool* pool, Task* task, OtuPinSpriteArgs* args);
s32            func_ov039_0208fc38(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208fd8c(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208fddc(TaskPool* pool, Task* task, void* args);
s32            func_ov039_0208fe18(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_0208fe60(TaskPool* pool, s32 dataType, s32 arg2);
void           func_ov039_0208fefc(OtuShadowTask* self, s32 frames);
void           func_ov039_0208ff30(OtuShadowTask* self);
void           func_ov039_0208ff68(OtuShadowTask* self);
s16            func_ov039_0209003c(OtuCursor6* c);
void           func_ov039_0209005c(OtuCursor6* c, OtuFrame6* table, s16 count);
s32            func_ov039_0209008c(OtuCursor6* c);
s16            func_ov039_020900f4(OtuCursor6* c);
s32            func_ov039_0209011c(OtuCursor6* c);
void           func_ov039_02090148(OtuCursor6* c, OtuFrame6* table, s16 count);
s32            func_ov039_02090178(OtuCursor6* c);
u16            func_ov039_020901e0(OtuCursor6* c);
OtuSpriteSlot* func_ov039_02090208(OtuSpriteTask* t, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_020902cc(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_020904a8(s32 arg0, Task* task, OtuHammer_Params* params);
s32            func_ov039_0209054c(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02090cec(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02090d90(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02090dd4(TaskPool* pool, Task* task, void* args, s32 stage);
u32            func_ov039_02090e1c(TaskPool* pool, s32 arg1, s32 arg2);
void           func_ov039_0209104c(void* task);
u16            func_ov039_02091060(void* task);
void           func_ov039_02091070(OtuArcGroup* self);
OtuSpriteSlot* func_ov039_02091118(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_02091268(TaskPool* pool, void* task, s32* args);
s32            func_ov039_020912c8(TaskPool* pool, void* task);
s32            func_ov039_020914d0(void* pool, void* task);
s32            func_ov039_02091524(TaskPool* pool, void* task);
void           func_ov039_02091560(void* a, void* b, void* c, s32 index);
s32            func_ov039_02091628(void* task, OtuPoint3* out);
void           func_ov039_02091654(void* task, s32 x, s32 y);
void           func_ov039_02091668(void* task);
void           func_ov039_02091690(OtuChildGroup* self);
OtuSpriteSlot* func_ov039_020916c4(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_02091814(void* pool, void* task, s32* args);
s32            func_ov039_02091868(TaskPool* pool, void* task);
s32            func_ov039_02091a5c(void* pool, void* task);
s32            func_ov039_02091aa4(void* pool, void* task);
void           func_ov039_02091ab8(void* a, void* b, void* c, s32 index);
void           func_ov039_02091b34(void* task);
void           func_ov039_02091b40(u16 start, u16* map);
s32            func_ov039_02091b98(TaskPool* pool, OtuBadge* task, s32* args);
s32            func_ov039_02092154(void* pool, void* task);
s32            func_ov039_020921f4(void);
s32            func_ov039_020921fc(void* pool, void* task);
void           func_ov039_020922c8(void* a, void* b, void* c, s32 index);
OtuSpriteSlot* func_ov039_020923c8(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args);
s32            func_ov039_0209258c(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02092608(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02092658(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02092694(TaskPool* pool, Task* task, void* args);
s32            func_ov039_020926a8(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params);
void           func_ov039_02092730(void* task, OtuPoint* in);
void           func_ov039_02092744(void* task, OtuPoint* out);
s32            func_ov039_02092758(void* task);
void           func_ov039_02092760(void* task);
s32            func_ov039_020929ec(void* pool, void* task, s32* params);
s32            func_ov039_02092a78(void* pool, void* task);
s32            func_ov039_02092bf0(void);
s32            func_ov039_02092bf8(void* pool, void* task);
void           func_ov039_02092c8c(void* a, void* b, void* c, s32 index);
void           func_ov039_02092e30(void* task);
s32            func_ov039_02092f88(TaskPool* pool, void* task, s32* args);
s32            func_ov039_020933c0(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_020933f0(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_020933f8(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_02093460(TaskPool* pool, Task* self, void* arg, s32 stage);
s32            func_ov039_020934a8(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3);
void           func_ov039_020934e0(OtuBgTaskData* data);
OtuSpriteSlot* func_ov039_0209352c(OtuSpriteTask* t, s32 arg, s32 sel);
s32            func_ov039_02093668(TaskPool* pool, Task* self, OtuInitArgs* args);
s32            func_ov039_020936b8(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_020937a0(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_020937dc(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_020937f4(TaskPool* pool, Task* self, void* arg, s32 stage);
s32            func_ov039_0209383c(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
OtuSpriteSlot* func_ov039_02093884(OtuSpriteTask* t, s32 arg, s32 sel);
s32            func_ov039_02093b08(TaskPool* pool, Task* self, OtuInitArgs* args);
s32            func_ov039_02093b98(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_02093bc0(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_02093c44(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_02093c90(TaskPool* pool, Task* self, void* arg, s32 stage);
s32            func_ov039_02093cd8(TaskPool* pool, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32            func_ov039_02093d18(OtuCounterData* data, u16* slots);
void           func_ov039_02093d68(OtuCounterData* data, s32 value);
OtuSpriteSlot* func_ov039_02093e3c(OtuSpriteTask* t, s32 arg, s32 sel);
s32            func_ov039_02093f70(TaskPool* pool, Task* self, OtuInitArgs* args);
s32            func_ov039_02093fcc(TaskPool* pool, Task* self, void* arg);
s32            func_ov039_0209411c(void* pool, void* task);
s32            func_ov039_02094158(void* pool, void* task);
void           func_ov039_02094188(void* a, void* b, void* c, s32 index);
s32            func_ov039_02094204(void* task);
s32            func_ov039_0209420c(void* task);
OtuSpriteSlot* func_ov039_02094214(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_020942bc(OtuGauge* self, Sprite* sprite);
void           func_ov039_0209432c(OtuGauge* self, Sprite* sprite);
void           func_ov039_0209439c(OtuGauge* self, Sprite* sprite, s32 index);
void           func_ov039_02094418(OtuGauge* self, Sprite* sprite, s32 index, s32 which);
s32            func_ov039_020944bc(TaskPool* pool, Task* task, void* arg);
s32            func_ov039_020945c8(TaskPool* pool, Task* task, void* args);
s32            func_ov039_020948f4(TaskPool* pool, Task* task, void* args);
s32            func_ov039_020949f4(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02094a6c(TaskPool* pool, Task* task, void* args, s32 stage);
OtuSpriteSlot* func_ov039_02094ae8(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_02094c50(void* pool, void* task, s32* args);
s32            func_ov039_02094ca8(void* pool, void* task);
s32            func_ov039_02094dac(void* pool, void* task);
s32            func_ov039_02094dfc(void* pool, void* task);
void           func_ov039_02094e10(void* a, void* b, void* c, s32 index);
OtuSpriteSlot* func_ov039_02094ff4(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_020950b8(OtuSlashTask* self, Sprite* sprite, OtuTaskArgs2* args);
s32            func_ov039_02095420(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_02095468(TaskPool* pool, s32 dataType, s32 pin);
OtuSpriteSlot* func_ov039_0209549c(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_02095558(OtuTrackTask* self, Sprite* sprite, OtuTaskArgs3* args);
s32            func_ov039_02095708(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_02095750(TaskPool* pool, s32 dataType, s32 pin);
void           func_ov039_02095788(OtuTrackTask* self, OtuPointSlot* slot, s32 angle, s32 dir, s32 len);
OtuSpriteSlot* func_ov039_020958a8(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_02095964(OtuPointTask* self, Sprite* sprite, OtuTaskArgs2* args, s32 index);
s32            func_ov039_02095c58(TaskPool* pool, Task* task, void* args, s32 stage);
s32            func_ov039_02095ca0(TaskPool* pool, s32 dataType, s32 pin);
void           func_ov039_02095cd4(OtuPointTask* self, s32 count);
OtuSpriteSlot* func_ov039_02095dec(OtuSpriteTask* t, s32 arg, s32 mode);
void           func_ov039_02095ea4(OtuEntryTask* self, Sprite* sprite, OtuTaskArgs1* args);
s32            func_ov039_02095f18(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02095f58(TaskPool* pool, Task* task, void* args);
s32            func_ov039_02095fe4(TaskPool* pool, Task* task, void* args);
s32            func_ov039_020960bc(void* pool, void* task, void* args);
s32            func_ov039_020960dc(TaskPool* pool, Task* task, void* data, s32 stage);
s32            func_ov039_02096124(TaskPool* pool, s32 arg1);
void           func_ov039_02096154(void* self, s16 which, s32 hasLabel);
OtuSpriteSlot* func_ov039_02096280(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_020963c8(void* pool, void* task, s32* args);
s32            func_ov039_02096404(void* pool, void* task, void* args);
s32            func_ov039_020964a4(void* pool, void* task, void* args);
s32            func_ov039_020964ec(void* pool, void* task, void* args);
s32            func_ov039_02096500(TaskPool* pool, Task* task, void* data, s32 stage);
s32            func_ov039_02096548(TaskPool* pool, s32 arg1, s32 arg2);
void           func_ov039_0209657c(Sprite* self);
OtuSpriteSlot* func_ov039_0209659c(OtuSpriteTask* t, s32 arg, s32 mode);
OtuSpriteSlot* func_ov039_02096660(OtuSpriteTask* t, s32 arg, s32 mode);
s32            func_ov039_02096874(void* pool, void* task, s32* args);
s32            func_ov039_020968c0(void* pool, void* task, void* args);
s32            func_ov039_020969fc(void* pool, void* task, void* args);
s32            func_ov039_02096ab0(void* pool, void* task, void* args);
s32            func_ov039_02096ad0(TaskPool* pool, Task* task, void* data, s32 stage);
s32            func_ov039_02096b18(TaskPool* pool, s32 arg1);
void           func_ov039_02096b48(void* self, s32 which);
s32            func_ov039_02096c44(void* self);
SpriteFrameInfo* func_ov039_02096c58(Sprite* sprite, s32 arg, s32 mode);
s32              func_ov039_02096d70(void* pool, void* task, s32* args);
s32              func_ov039_02096d98(void);
s32              func_ov039_02096da0(void* pool, void* task, void* args);
s32              func_ov039_02096dec(void* pool, void* task, void* args);
s32              func_ov039_02096e04(TaskPool* pool, Task* task, void* data, s32 stage);
s32              func_ov039_02096e4c(TaskPool* pool, s32 arg1);
OtuSpriteSlot*   func_ov039_02096e7c(OtuSpriteTask* t, s32 arg, s32 mode);
s32              func_ov039_0209702c(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097160(TaskPool* pool, Task* task, void* param);
s32              func_ov039_020971b0(TaskPool* pool, Task* task, void* param);
s32              func_ov039_020971c4(TaskPool* pool, Task* task, void* param, s32 stage);
OtuSpriteSlot*   func_ov039_02097398(OtuSpriteTask* t, s32 arg, s32 mode);
s32              func_ov039_020974e0(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097528(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097670(TaskPool* pool, Task* task, void* param);
s32              func_ov039_020976c0(TaskPool* pool, Task* task, void* param);
s32              func_ov039_020976d4(TaskPool* pool, Task* task, void* param, s32 stage);
s32              func_ov039_0209771c(TaskPool* pool, s32 word0, s32 word1);
void             func_ov039_02097750(OtuTaskSprite* s, OtuTaskParams* param, s32 selector, s32 closeBy, s32 step, s32 holdFor);
OtuSpriteSlot*   func_ov039_020977d0(OtuSpriteTask* t, s32 arg, s32 mode);
s32              func_ov039_02097918(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097954(TaskPool* pool, Task* task, void* param);
s32              func_ov039_020979cc(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097a14(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097a28(TaskPool* pool, Task* task, void* param, s32 stage);
s32              func_ov039_02097a70(TaskPool* pool, s32 word0, s32 word1);
void             func_ov039_02097aa4(OtuTaskSprite* s, OtuTaskParams* param, s32 animation);
OtuSpriteSlot*   func_ov039_02097ae0(OtuSpriteTask* t, s32 arg, s32 mode);
s32              func_ov039_02097c30(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097c90(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097dbc(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097e0c(TaskPool* pool, Task* task, void* param);
s32              func_ov039_02097e20(TaskPool* pool, Task* task, void* param, s32 stage);
s32              func_ov039_02097e68(TaskPool* pool, s32 word0, s32 word1);
void             func_ov039_02097e9c(OtuTaskSprite* s, OtuTaskParams* param);
OtuSpriteSlot*   func_ov039_02097ff4(OtuSpriteTask* t, s32 arg, s32 mode);
s32              func_ov039_02098144(TaskPool* pool, void* task, s32* args);
s32              func_ov039_020981a4(TaskPool* pool, void* task);
s32              func_ov039_020982e8(void* pool, void* task);
s32              func_ov039_02098338(void* pool, void* task);
void             func_ov039_0209834c(void* a, void* b, void* c, s32 index);
OtuSpriteSlot*   func_ov039_02098538(OtuSpriteTask* t, s32 arg, s32 sel);
void             func_ov039_020985e0(void* self, void* sprite);
s32              func_ov039_02098650(TaskPool* pool, void* task, s32* args);
s32              func_ov039_020988d8(void* pool, void* task);
s32              func_ov039_020988fc(void* pool, void* task);
s32              func_ov039_02098920(void* pool, void* task);
void             func_ov039_020989a8(void* a, void* b, void* c, s32 index);
void             func_ov039_02098a20(void* a, void* owner);
void             func_ov039_02098a40(void* dispatch, void* stage);
void             func_ov039_02098a50(OtuStageDispatch* dispatch);
void             func_ov039_02098a60(void* task, void* arg);
void             func_ov039_02098acc(void* task, void* scene);
void             func_ov039_02098af4(void* task, void* arg);
s32              func_ov039_02098b44(void* task);
void*            func_ov039_02098b68(void* dispatch);
void             func_ov039_02098b78(OtuStageDispatch* dispatch, s32* out);
#endif
