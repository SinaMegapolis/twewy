/* ============================================================================
 * BATCH B4 -- the "badge" task's own support routines,
 * 0x0208d3bc - 0x0208e504 (18 functions, 4880 bytes).
 *
 * WHAT THIS BATCH IS
 * -----------------
 * These are not a slice of somebody else's call graph -- they are one whole
 * task's body. The TaskHandle at 0x0209926c names it, and the name string at
 * 0x0209a49c is "Tsk_OtosuGame_badge": {name, taskFunc = 0x0208dc68,
 * dataSize = 0x25C}. The stage table at 0x02099278 is
 *
 *     0x0208d6dc  init
 *     0x0208d7e4  update
 *     0x0208da74  render
 *     0x0208db44  cleanup
 *
 * and 0x0208dc68 is the usual four-slot trampoline over it. So the first
 * four stage-shaped functions here are that task's lifecycle, 0x0208d3bc /
 * 0x0208d4cc / 0x0208d554 are its three sprite loaders (one per embedded
 * Sprite), 0x0208d5dc is its state reset, 0x0208d9ec its per-frame position
 * push, 0x0208dcb0 its spawner, and the tail of the band
 * (0x0208df2c .. 0x0208e504) is the pair-versus-pair collision code that the
 * pin task runs against itself.
 *
 * The same task is modelled twice more in this translation unit: the header's
 * `OtuPinTask` is the narrow 0x174 prefix of it that the pool queries use, and
 * band B3's `OtuBadgeState` is the badge AI's own full view of the same 0x25C
 * bytes under different field names. All three describe one object. There is
 * no "badger" task -- that was a misreading of the handle's
 * "Tsk_OtosuGame_badge", confirmed against the ROM's task strings, none of
 * which contains "badger" except `Tsk_OtosuGame_badgeradar`.
 *
 * STRUCTURAL FINDINGS
 * -------------------
 * 1. Three Sprites are embedded at +0x0C, +0x4C and +0x8C. That is not an
 *    inference from the address arithmetic: 0x0208d9ec stores to +0x18/+0x1A
 *    and +0x98/+0x9A, which are Sprite.posX/posY (+0x0C/+0x0E) inside a
 *    Sprite at +0x0C and +0x8C respectively, and 0x0208da74 does the same at
 *    +0x58/+0x5A for the one at +0x4C. 0x0C + 0x40 + 0x40 = 0x8C exactly.
 * 2. The +0x100 sub-object is real and is what OtuFieldAccess.c's four
 *    0x100+0x78..0x7E accessors read. 0x0208d5dc writes them from four
 *    *bytes* of a 0x1C-stride record table (so they are four s16 fields fed
 *    byte by byte, not one u32), and also writes +0x100+0x9E = 5.
 * 3. The record table at +0x170 is indexed by the u16 at +0x16C with a
 *    0x1C *byte* stride. +0x16C is the same tray-slot pointer the header
 *    documents for OtuPinTask.pinID, and 0x130 is the same "no pin" sentinel.
 * 4. `kind` at +0xF8 is the state's discriminator. Three different switches
 *    over it appear in this band and they select *different* sets:
 *      0x0208d7e4 update : 1,3,6,7 share func_ov039_0208af6c, then a full
 *                          1..9 dispatch to nine per-kind handlers
 *      0x0208da74 render : 1,3,4,5,7,9 render, 8 renders only for subKind
 *                          0/1/6
 *      0x0208e28c        : only kinds 1,6,7 may interact
 *      0x0208e37c        : only kinds 1,3,4,6,7
 *      0x0208e504        : only kinds 1,6,7,9
 *    So 1/6/7 is one class (they interact and are what 0x0208e28c drives) and
 *    9 joins them only for the wall/board case in 0x0208e504.
 * 5. The three sprite loaders differ only in which 0x2C-byte SpriteAnimation
 *    template they copy (0x02099288 / 0x020992b4 / 0x020992e0 -- exactly one
 *    struct apart each) and, for the first only, in a pin-dependent
 *    binIden/packIndex patch. The `<< 0x1C` / `>> 0x1A` pair is the
 *    `dataType` bitfield insert; the `bic #0x380` / `orr #0x300` in the first
 *    one alone is `bits_7_9 = 6`.
 * 6. The child-task handles at +0x1D4..+0x258 are created by band 1/2/3/4/5/7's
 *    spawners in a fixed order and deleted in reverse by 0x0208db44, which is
 *    the same set of eleven child sorts the header's OtuPinTask comment calls
 *    "the three pin types the results screen scores separately" -- except this
 *    task creates *eleven*, so the three-kind story in the header is about
 *    querying, not about what a pin owns.
 *
 * NOT DETERMINED / KNOWN GAPS
 * ---------------------------
 *  - The `adds r4, #0x12C` / `bne` / `adds r4, #0x130` / `popeq` tail of
 *    0x0208dff0 compares *addresses*, with no loads at all. No reading of
 *    "vel.x != 0 || vel.y != 0" produces that (it needs two loads), and both
 *    fields are read as data everywhere else in the overlay. Written below as
 *    the pointer test the flags encode, with a comment.
 *  - +0xE4 is a pointer and +0xE0 an index multiplied by *two*, then bytes +8
 *    and +9 of the result are read. A 2-byte stride with +8/+9 offsets is not
 *    self-consistent; reproduced literally.
 *  - `self->unk_0CC = (u32)unk_140 * 0x10 / 0x10000` is guessed from the
 *    `lsl #4` / `lsr #16` pair. A plain `>> 12` is one instruction, so the
 *    source really did spell it as a multiply and a divide; which of the two
 *    is which cannot be told from the encoding.
 *  - Six of the eleven spawners 0x0208dcb0 calls are typed `void` in bands
 *    2/4/5 while the target stores their return value into a child-handle
 *    field. Band 4 already retyped two siblings of exactly this kind (02096b18,
 *    02096e4c) to `s32` for the same reason; the same fix is needed here and
 *    is called out at the call sites.
 *
 * WHERE THIS FILE HAS TO BE INCLUDED
 * ----------------------------------
 * Last, after OtuPinSprites.inc. It calls 0x0208f40c / 0x0208f770 / 0x0208fa5c /
 * 0x0208fe60 (band 1), 0x0209383c / 0x02093cd8 / 0x02093d18 (band 2),
 * 0x02095750 / 0x02095ca0 (band 3), 0x02096124 / 0x02096548 / 0x02096154
 * (band 4), 0x0209771c / 0x02097a70 (band 5) and 0x02090e1c (band 7) with no
 * declaration of its own for any of them, so going in earlier means an implicit
 * `int (...)` that then collides with the real definition -- the failure mode
 * OtuBands.h already documents for bands 4/5/1.
 * ==========================================================================*/

/* ------------------------------------------------------------------ */
/* Data the band reads.                                               */
/* ------------------------------------------------------------------ */

/** The badge task's three SpriteAnimation templates, 0x2C bytes each. */

/** The four stage slots, indexed by the stage argument of 0x0208dc68. */

/** The task's handle: name "Tsk_OtosuGame_badge", taskFunc 0x0208dc68, 0x25C. */

/*
 * The bin identifiers the first sprite loader patches in. Both pool words sit
 * inside one run of {0x27, char*} pairs at 0x0209a0b4 and the two are exactly
 * five entries (0x28 bytes) apart, so the run is one array and the two names
 * are element 0 and element 5 of it. Declared as two so the pool words keep
 * the target's names, per the note in TinPinSlammer.h about gap-filled data.
 */

/** The two scale tables 0x0208e130 indexes, four words apart, 0x10 stride.
 *
 *  Both are read as `ldr rX, [base, index, lsl #4]`, and the bases are four
 *  bytes apart, so they are one array of 0x10-byte records read at two
 *  different field offsets rather than two independent tables. Declared as
 *  two so each pool word names the address the target names.
 */
typedef struct {
    /* 0x00 */ s32 scaleA;
    /* 0x04 */ s32 scaleB;
    u8        pad_08[0x08];
} OtuPinScale; // Size: 0x10


/** The constant 0x800 0x0208e504 scales the pin's speed by. */

/* ------------------------------------------------------------------ */
/* Shapes.                                                            */
/* ------------------------------------------------------------------ */

/** One 0x1C-byte row of the tray table at +0x170. */
typedef struct {
    /* 0x00 */ u8 tile[4];  // copied byte by byte to +0x178..+0x17E as four s16s
    /* 0x04 */ u8 weight;   // indexes the two OtuPinScale tables
    u8          pad_05[0x17];
} OtuPinRow; // Size: 0x1C

/** Q12.12 multiply, rounded -- the form the target spells with smull + 0x800. */
#define OTU_MUL_Q12(a, b) ((s32)((((s64)(a) * (s64)(b)) + 0x800) >> 12))

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
    /* 0x000 */ s32          unk_000;   // the spawn argument, unused here
    /* 0x004 */ s32          dataType;  // copied into SpriteAnimation.dataType
    /* 0x008 */ TaskPool*    pool;      // saved by the init stage
    /* 0x00C */ Sprite       spriteA;
    /* 0x04C */ Sprite       spriteB;
    /* 0x08C */ Sprite       spriteC;
    /* 0x0CC */ s32          unk_0CC;   // unk_140 rescaled by 1/4096
    /* 0x0D0 */ s32          unk_0D0;   // starts at 0x1000
    /* 0x0D4 */ s32          unk_0D4;   // starts at 0x1000
    /* 0x0D8 */ u16          unk_0D8;
    /* 0x0DA */ u16          unk_0DA;
    /* 0x0DC */ s32          unk_0DC;   // raised to 1 by init and by update
    /* 0x0E0 */ s32          tileIndex; // scaled by two, see the init stage
    /* 0x0E4 */ u8*          tileTable;
    /* 0x0E8 */ void*        unk_0E8;   // non-NULL switches on the variant path
    /* 0x0EC */ u16          unk_0EC;   // *(u16*)(unk_0E8 + 4)
    /* 0x0EE */ u16          unk_0EE;
    /* 0x0F0 */ u16          unk_0F0;
    /* 0x0F4 */ s32          unk_0F4;
    /* 0x0F8 */ s32          kind;      // 1..9; see the header note above
    /* 0x0FC */ s32          subKind;   // gates kind 8 in the render stage
    /* 0x100 */ s32          unk_100;
    /* 0x104 */ s32          unk_104;   // starts at 1
    /* 0x108 */ s32          unk_108;
    /* 0x10C */ s32          unk_10C;
    /* 0x110 */ s32          anchorX;   // the render subtracts these
    /* 0x114 */ s32          anchorY;
    /* 0x118 */ s32          unk_118;
    /* 0x11C */ s32          unk_11C;
    /* 0x120 */ s32          x;         // Q12.12, OtuPoint
    /* 0x124 */ s32          y;
    /* 0x128 */ s32          unk_128;   // added into y by the render
    /* 0x12C */ OtuPoint     vel;
    /* 0x134 */ s32          unk_134;
    /* 0x138 */ OtuPoint     dir;
    /* 0x140 */ s32          travel;    // integrated from vel each update
    /* 0x144 */ s32          velMag;    // decays by a fixed step each update
    /* 0x148 */ s32          alive;
    /* 0x14C */ u8           pad_14C[0x1C];
    /* 0x168 */ s32          flags;     // init 0xA2; bit 1 blocks the render
    /* 0x16C */ u16*         pinID;     // a tray slot; 0x130 means "no pin"
    /* 0x170 */ u8*          rowTable;  // OtuPinRow[], 0x1C stride
    /* 0x174 */ s32          unk_174;
    /* 0x178 */ s16          tile0;     // rowTable[*pinID].tile[0]
    /* 0x17A */ s16          tile1;
    /* 0x17C */ s16          tile2;
    /* 0x17E */ s16          tile3;
    /* 0x180 */ u8           pad_180[0x1C];
    /* 0x19C */ u16          unk_19C;
    /* 0x19E */ u16          unk_19E;   // starts at 5
    /* 0x1A0 */ u16          unk_1A0;
    /* 0x1A2 */ u8           pad_1A2[2];
    /* 0x1A4 */ s32          unk_1A4;   // raised to 1 by the init stage
    /* 0x1A8 */ s32          hasLabel;  // gates two of the children
    /* 0x1AC */ s32          unk_1AC;
    /* 0x1B0 */ struct OtuBadge* partner; // the pin this one last touched
    /* 0x1B4 */ s32          unk_1B4;
    /* 0x1B8 */ s32          unk_1B8;   // counts down, then re-raises unk_1B4
    /* 0x1BC */ s32          unk_1BC;
    /* 0x1C0 */ s32          unk_1C0;
    /* 0x1C4 */ s32          unk_1C4;
    /* 0x1C8 */ s32          unk_1C8;
    /* 0x1CC */ s32          unk_1CC;
    /* 0x1D0 */ s32          unk_1D0;   // frame counter, saturating
    /* 0x1D4 */ s32          child0;
    /* 0x1D8 */ s32          child1;
    /* 0x1DC */ s32          child2;
    /* 0x1E0 */ s32          child3;
    /* 0x1E4 */ s32          child4;
    /* 0x1E8 */ s32          child5;
    /* 0x1EC */ s32          child6;
    /* 0x1F0 */ s32          child7;
    /* 0x1F4 */ s32          child8;
    /* 0x1F8 */ s32          children[12];
    /* 0x228 */ s32          pairIds[2];
    /* 0x230 */ s32          labelTask;
    /* 0x234 */ s32          child9;
    /* 0x238 */ s32          groupIds[8];
    /* 0x258 */ s32          child10;
} OtuBadge; // Size: 0x25C

/** The row of the tray table this pin's tray slot names.
 *
 *  Spelled `base + *pinID * 0x1C` rather than as `&rowTable[*pinID]` on
 *  purpose: the target keeps the scaled add and the fixed displacement
 *  separate (`mla r1, r3, r1, r6` then `ldrb r3, [r1, #4]`), and folding the
 *  two together costs an instruction. Same reasoning as OTU_CHILD_ID.
 */
#define OTU_PIN_ROW(self) ((OtuPinRow*)((u8*)(self)->rowTable + *(self)->pinID * 0x1C))

/** The pin's position, as the vector helpers want it. */
#define OTU_PIN_POS(self) ((OtuPoint*)&(self)->x)

/** The pin's velocity. */
#define OTU_PIN_VEL(self) ((OtuPoint*)&(self)->vel)

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
} OtuBadge_InitArgs; // Size: 0x24

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

/* ------------------------------------------------------------------ */
/* The three sprite loaders.                                          */
/* ------------------------------------------------------------------ */

/** Loads sprite A, picking the bin and pack index from the tray slot.
 *
 *  The whole 0x2C-byte template is copied in first (three `ldm`/`stm` pairs,
 *  0x20 + 0x0C bytes, which is exactly sizeof(SpriteAnimation)), so the
 *  template's own binIden and packIndex are overwritten rather than merely
 *  defaulted. 0x130 at the tray slot means "no pin" and selects the second
 *  bin with a fixed pack index of 0xB.
 */
void func_ov039_0208d3bc(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_02099288;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;
    // The only one of the three that sets this. `bic #0x380` / `orr #0x300`
    // clears bits 7..9 and writes 6 into them.
    anim.bits_7_9 = 6;

    if (*self->pinID < 0x130) {
        anim.binIden   = &data_ov039_0209a0b4[0];
        anim.packIndex = *self->pinID + 1;
    } else {
        anim.binIden   = (BinIdentifier*)&data_ov039_0209a0dc;
        anim.packIndex = 0xB;
    }

    anim.unk_1C = 1;
    anim.unk_20 = 4;
    anim.unk_26 = 2;
    anim.unk_28 = 3;

    _Sprite_Load(sprite, &anim);
}

/** Loads sprite B. The same loader with the second template and no patch-up. */
void func_ov039_0208d4cc(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_020992b4;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;

    _Sprite_Load(sprite, &anim);
}

/** Loads sprite C. func_ov039_0208d4cc with the third template.
 *
 *  The third template differs from the second only in its packIndex (1 against
 *  6) and its animIndex (5 against 1), neither of which the loader touches --
 *  the three really are one function instantiated three times. */
void func_ov039_0208d554(OtuBadge* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_020992e0;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    anim.posX     = self->x >> 12;
    anim.posY     = self->y >> 12;

    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The task's state reset and its four stages.                        */
/* ------------------------------------------------------------------ */

/**
 * @brief Zeroes the per-frame state and re-reads the pin's tray row.
 *
 *  Two details are load-bearing. The four bytes at +0x178..+0x17E are read
 *  from the row one `ldrb` at a time and stored with `strh`, so they are four
 *  separate s16 fields rather than one word -- that is also what makes the
 *  +0x100 sub-object that OtuFieldAccess.c's four accessors point at.
 *
 *  The target derives `self + 0x100` once and reaches all five of those
 *  fields through it. Written flat here; the base is a register-allocation
 *  choice the source does not get to make.
 */
void func_ov039_0208d5dc(OtuBadge* self) {
    self->unk_0F4 = 0;
    self->unk_104 = 1;
    self->unk_108 = 0;
    self->unk_10C = 0;
    self->unk_128 = 0;
    self->vel.x   = 0;
    self->vel.y   = 0;
    self->dir.x   = 0x1000;
    self->dir.y   = 0;
    self->unk_134 = 0;
    self->travel  = 0;
    self->velMag  = 0;
    self->unk_0CC = 0;
    self->unk_0D0 = 0x1000;
    self->unk_0D4 = 0x1000;
    self->unk_0D8 = 0;
    self->unk_0DA = 0;
    self->unk_19E = 5;
    self->alive   = 0;
    self->unk_1CC = 0;
    self->unk_1D0 = 0;
    self->flags   = 0xA2;
    self->partner = NULL;
    self->unk_1B4 = 0x11;
    self->unk_1B8 = 0;
    self->unk_1BC = 0;
    self->unk_1C0 = 0;
    self->unk_1C4 = 0;

    // As in 0x0208e130: the row is re-derived for each byte rather than
    // hoisted, because the target reloads pinID and rowTable every time.
    self->tile0 = OTU_PIN_ROW(self)->tile[0];
    self->tile1 = OTU_PIN_ROW(self)->tile[1];
    self->tile2 = OTU_PIN_ROW(self)->tile[2];
    self->tile3 = OTU_PIN_ROW(self)->tile[3];

    func_ov039_0208d3bc(self, &self->spriteA);
}

/**
 * @brief The init stage: takes the spawn block, seeds the grid position,
 *        resets the state and loads all three sprites.
 *
 *  The grid position comes out of two *bytes* of the tile table at +0xE4,
 *  scaled by 32 and biased by 0x10 before being promoted to Q12.12 -- so the
 *  board is a 32-pixel grid and +0x120/+0x124 are that cell in fixed point.
 *
 *  The `add r1, r1, r2, lsl #1` is a two-byte stride over an index that is
 *  itself a word, and the two reads are at +8 and +9 of the result. That does
 *  not describe any record this overlay uses elsewhere, so it is reproduced
 *  exactly rather than tidied into a plausible layout.
 *
 *  `unk_0EC` is the one field read out of the variant pointer: `*(u16*)(unk_0E8 + 4)`
 *  when there is one, zero otherwise -- the `ldrhne`/`strhne`/`strheq` shape.
 */
s32 func_ov039_0208d6dc(TaskPool* pool, Task* task, OtuBadge_InitArgs* args) {
    OtuBadge* self = (OtuBadge*)task->data;
    u8*        cell;

    self->dataType = args->unk_00;
    self->pool     = pool;
    self->tileIndex = args->unk_04;
    self->unk_0E8   = args->unk_08;
    self->tileTable = args->unk_0C;
    self->unk_000   = args->unk_10;
    self->unk_0DC   = 1;
    self->unk_19C   = 0;
    self->unk_1A0   = 0;
    self->hasLabel  = args->unk_1C;
    self->unk_1AC   = 0;
    self->unk_1C8   = 0;
    self->rowTable  = args->rowTable;
    self->pinID     = args->pinID;
    self->unk_174   = args->unk_20;

    self->unk_0EC = (self->unk_0E8 != NULL) ? *(u16*)((u8*)self->unk_0E8 + 4) : 0;

    self->unk_0EE = 0;
    self->unk_0F0 = 0;
    self->anchorX = 0;
    self->anchorY = 0;
    self->unk_118 = 0;
    self->unk_11C = 0;

    cell = self->tileTable + self->tileIndex * 2;
    self->x = ((cell[8] << 5) + 0x10) << 12;
    self->y = ((cell[9] << 5) + 0x10) << 12;

    self->unk_1A4 = 1;

    func_ov039_0208d5dc(self);
    func_ov039_0208d4cc(self, &self->spriteB);
    func_ov039_0208d554(self, &self->spriteC);

    return 1;
}

/**
 * @brief The update stage: the per-kind handler, the travel integration, then
 *        the per-kind behaviour dispatch.
 *
 *  Two things worth stating. The three saturating counters (+0x1D0, +0x148,
 *  +0x168) are three separate `cmp`/`subgt`/`strgt` triples in the target, so
 *  they are three statements and not a loop. And +0x144 is decayed by a step
 *  read from `data_ov039_0209a390` (which holds 10) -- `add r0, r0, r0, lsl #1`
 *  then `lsl #0xc`, so the source is `(v * 3) << 12`, not `v * 0x3000` folded.
 *
 *  The `switch` over `kind` here is a *table* dispatch (`cmp r0, #9; addls pc,
 *  pc, r0, lsl #2`) with nine out-of-line bodies, one per kind, all named.
 */
s32 func_ov039_0208d7e4(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;

    if (self->unk_0E8 != NULL) {
        func_ov039_0208ac98((OtuInputLatch*)self);
        func_ov039_0208acc0((OtuBadgeState*)self);
    }

    self->unk_0DC = 1;
    self->unk_0D0 = 0x1000;
    self->unk_0D4 = 0x1000;

    if (self->unk_1D0 > 0) {
        self->unk_1D0--;
    }
    if (self->alive > 0) {
        self->alive--;
    }
    if (self->flags > 0) {
        self->flags--;
    }

    switch (self->kind) {
        case 1:
        case 3:
        case 6:
        case 7:
            func_ov039_0208af6c((OtuBadgeState*)self);
            break;

        default:
            break;
    }

    self->travel = self->travel + self->velMag;

    if (self->velMag > 0) {
        self->velMag = self->velMag - ((data_ov039_0209a390 * 3) << 12);
        if (self->velMag < 0) {
            self->velMag = 0;
        }
    } else if (self->velMag < 0) {
        self->velMag = self->velMag + ((data_ov039_0209a390 * 3) << 12);
        if (self->velMag > 0) {
            self->velMag = 0;
        }
    }

    // `lsl #4` then `lsr #0x10`: a multiply by 0x10 followed by an unsigned
    // divide by 0x10000, not a shift right by twelve. A plain `>> 12` is one
    // instruction and would not be what the target wrote.
    self->unk_0CC = (u32)self->travel * 0x10 / 0x10000;

    if (self->unk_1B8 > 0) {
        self->unk_1B8 = self->unk_1B8 - 1;
        if (self->unk_1B8 <= 0) {
            self->unk_1B4 = 0x11;
        }
    }

    switch (self->kind) {
        case 1:
            if (self->unk_0E8 != NULL) {
                func_ov039_0208bb2c((OtuBadgeState*)self);
            } else {
                func_ov039_0208c9cc(self);
            }
            break;

        case 2:
            func_ov039_0208ca1c(self);
            break;

        case 3:
            func_ov039_0208cb64(self);
            break;

        case 4:
            func_ov039_0208cc4c(self);
            break;

        case 5:
            func_ov039_0208cd50(self);
            break;

        case 6:
            func_ov039_0208ce20(self);
            break;

        case 7:
            func_ov039_0208ce54(self);
            break;

        case 8:
            func_ov039_0208ce88(self);
            break;

        case 9:
            func_ov039_0208d210(self);
            break;

        default:
            break;
    }

    Sprite_Update(&self->spriteA);
    Sprite_Update(&self->spriteB);
    Sprite_Update(&self->spriteC);

    return 1;
}

/**
 * @brief Pushes the pin's grid position into sprite C's and sprite A's cells.
 *
 *  The same two expressions twice; only sprite C's are gated on +0x1D0. The
 *  stored fields are `Sprite.posX`/`Sprite.posY` (a Sprite's +0x0C/+0x0E),
 *  which is what fixes the sprite bases at +0x0C/+0x4C/+0x8C.
 */
void func_ov039_0208d9ec(OtuBadge* self) {
    if (self->unk_1D0 > 0) {
        self->spriteC.posX = (self->x - self->anchorX) >> 12;
        self->spriteC.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

        Sprite_RenderFrame(&self->spriteC);
    }

    self->spriteA.posX = (self->x - self->anchorX) >> 12;
    self->spriteA.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

    Sprite_RenderFrame(&self->spriteA);
}

/**
 * @brief The render stage.
 *
 *  While +0xDC is clear the pin has not started moving, so only the middle
 *  sprite is drawn and only if the pin carries a label; after that the pin
 *  renders itself through 0x0208d9ec, but only for the kinds listed, and kind
 *  8 only for three of its sub-kinds. +0x168 bit 1 suppresses all of it.
 */
s32 func_ov039_0208da74(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;

    if (self->unk_0DC == 0) {
        if (self->hasLabel != 0) {
            self->spriteB.posX = (self->x - self->anchorX) >> 12;
            self->spriteB.posY = ((self->y + self->unk_128) - self->anchorY) >> 12;

            Sprite_RenderFrame(&self->spriteB);
        }

        return 1;
    }

    if (self->flags & 2) {
        return 1;
    }

    switch (self->kind) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 7:
        case 9:
            func_ov039_0208d9ec(self);
            break;

        case 8:
            if (self->subKind == 0 || self->subKind == 1 || self->subKind == 6) {
                func_ov039_0208d9ec(self);
            }
            break;

        default:
            break;
    }

    return 1;
}

/**
 * @brief The cleanup stage: deletes all eleven children, then the sprites.
 *
 *  The three looped groups are walked as `self + i * 4 + offset` rather than
 *  indexed off the array member, because that is the shape the target emits
 *  (`add r0, r5, r6, lsl #2` then `ldr r1, [r0, #0x238]`) and folding the
 *  displacement into the scaled add costs an instruction.
 *
 *  Deletion order is the reverse of creation, except that the labelled child
 *  is created between the pair group and the twelve, and is deleted between
 *  the pair group and child9 -- so the order is exact, not simply reversed.
 */
s32 func_ov039_0208db44(TaskPool* pool, Task* task, void* args) {
    OtuBadge* self = (OtuBadge*)task->data;
    s32       i;

    EasyTask_DeleteTask(pool, self->child10);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x238));
    }

    EasyTask_DeleteTask(pool, self->child9);

    if (self->hasLabel != 0) {
        EasyTask_DeleteTask(pool, self->labelTask);
    }

    for (i = 0; i < 2; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x228));
    }

    for (i = 0; i < 12; i++) {
        EasyTask_DeleteTask(pool, *(s32*)((u8*)self + i * 4 + 0x1F8));
    }

    EasyTask_DeleteTask(pool, self->child8);
    EasyTask_DeleteTask(pool, self->child7);
    EasyTask_DeleteTask(pool, self->child6);
    EasyTask_DeleteTask(pool, self->child5);
    EasyTask_DeleteTask(pool, self->child4);
    EasyTask_DeleteTask(pool, self->child3);
    EasyTask_DeleteTask(pool, self->child2);
    EasyTask_DeleteTask(pool, self->child1);
    EasyTask_DeleteTask(pool, self->child0);

    Sprite_Release(&self->spriteA);
    Sprite_Release(&self->spriteB);
    Sprite_Release(&self->spriteC);

    return 1;
}

/** The task's entry point: the four-slot stage trampoline. */
s32 func_ov039_0208dc68(TaskPool* pool, Task* task, void* args, s32 stage) {
    const TaskStages stages = data_ov039_02099278;

    return stages.iter[stage](pool, task, args);
}

/* ------------------------------------------------------------------ */
/* The spawner.                                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief Creates a badge and its eleven children, in a fixed order.
 *
 *  Every spawner is handed the *new* task's id, not the parent's, so the
 *  children know which pin they belong to; that is why `id` is passed
 *  alongside the pool everywhere.
 *
 *  The two `EasyTask_GetTaskData` calls after 0x02093cd8 and 0x02096124 are
 *  not redundant: the first hands the counter task its tray slot pointer
 *  (`args` word 7) and the second pokes the label task with whether the pin's
 *  tray slot holds a label. Both re-derive the pool from `self->pool` rather
 *  than from the argument, which is why +0x008 has to be saved at all.
 *
 *  NB: six of these spawners are typed `void` in bands 2/4/5 while the target
 *  stores their result. Retyped to `s32` there -- band 4 already did this for
 *  0x02096b48/0x02096e4c's siblings -- nothing here changes.
 *
 *  The frame is exactly 0x2C: 8 bytes of outgoing arguments plus the 0x24-byte
 *  block, with no slack. `id`, `self` and `i` therefore have to live in
 *  registers, and there are only five callee-saved ones, so this is at the
 *  edge of what mwcc can hold -- if the frame comes out larger, the fix is to
 *  drop `i` and unroll one of the loops rather than to shrink the block.
 */
/* Typed to band 9's declaration, the one already compiling everywhere else
 * in this translation unit. Taking all nine arguments as s32 and casting at
 * each use collided with band 9 and cascaded into nine "illegal access to
 * local variable" errors inside this body. */
s32 func_ov039_0208dcb0(TaskPool* pool, s32 arg1, s32 arg2, void* arg3, void* arg4,
                       void* arg5, void* arg6, void* arg7, s32 arg8, void* arg9) {
    OtuBadge_InitArgs args;
    OtuBadge*         self;
    s32                id;
    s32                i;

    args.unk_00    = arg1;
    args.unk_04    = arg2;
    args.unk_08    = arg3;
    args.unk_0C    = (u8*)arg4;
    args.unk_10    = arg5;
    args.rowTable  = (u8*)arg6;
    args.pinID     = (u16*)arg7;
    args.unk_1C    = arg8;
    args.unk_20    = arg9;

    id   = EasyTask_CreateTask(pool, &data_ov039_0209926c, NULL, 0, NULL, &args);
    self = (OtuBadge*)EasyTask_GetTaskData(pool, id);

    self->child0 = func_ov039_0208f40c(pool, arg1, id);
    self->child1 = func_ov039_0208f770(pool, arg1, id);
    self->child2 = func_ov039_0208fa5c(pool, arg1, id);
    self->child3 = func_ov039_0208fe60(pool, arg1, id);
    self->child4 = func_ov039_02090e1c(pool, arg1, id);
    self->child5 = func_ov039_020915a8(pool, arg1, id);
    self->child6 = func_ov039_02091b00(pool, arg1, id);

    // The radar and the counter each take two extra words, built in the
    // outgoing-argument area rather than in a frame of their own.
    self->child7 = func_ov039_0209383c(pool, arg1, id, arg2, (s32)arg4, arg8);
    self->child8 = func_ov039_02093cd8(pool, arg1, id, arg2, arg8);
    func_ov039_02093d18(EasyTask_GetTaskData(pool, self->child8), (u16*)arg7);

    for (i = 0; i < 12; i++) {
        *(s32*)((u8*)self + i * 4 + 0x1F8) = func_ov039_02095750(pool, arg1, id);
    }

    for (i = 0; i < 2; i++) {
        *(s32*)((u8*)self + i * 4 + 0x228) = func_ov039_02095ca0(pool, arg1, id);
    }

    if (self->hasLabel != 0) {
        self->labelTask = func_ov039_02096124(pool, arg1);
    }

    self->child9 = func_ov039_02096548(pool, arg1, id);

    for (i = 0; i < 8; i++) {
        *(s32*)((u8*)self + i * 4 + 0x238) = func_ov039_0209771c(pool, arg1, id);
    }

    self->child10 = func_ov039_02097a70(pool, arg1, id);

    if (self->hasLabel != 0 && *self->pinID < 0x130) {
        // The label rides in the *second* halfword of the tray slot, and the
        // `moveq`/`movne` pair shows it is the second halfword that is tested.
        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->labelTask), 1,
                            self->pinID[1] == 0x130 ? 1 : 0);
    }

    func_ov039_0208ae8c((OtuBadgeState*)self);

    return id;
}

/* ------------------------------------------------------------------ */
/* The pair-versus-pair collision code.                                */
/* ------------------------------------------------------------------ */

/**
 * @brief True when two pins are close enough and closing.
 *
 *  Two guards, then a sign test. The first guard is the `ldreq`/`cmpeq`
 *  chain: both velocities are zero means neither pin is heading anywhere, and
 *  that is decided without a branch. The second is the reach test, summed
 *  from the two arguments rather than compared against a constant -- the
 *  callers both pass 0xC000, so this is 3.0 in Q12.12.
 *
 *  The sign test is "at least one of the two dots is positive", i.e. the pair
 *  is not separating on both axes. Returned as a materialised 1/0 rather than
 *  as the comparison, because the target materialises it.
 */
s32 func_ov039_0208df2c(OtuPoint* posA, OtuPoint* velA, s32 reachA, OtuPoint* posB, OtuPoint* velB, s32 reachB) {
    OtuPoint dir;
    s32     dotA;
    s32     dotB;

    if (velA->x == 0 && velA->y == 0 && velB->x == 0 && velB->y == 0) {
        return 0;
    }

    if (func_ov039_02098ca8(posA, posB) > reachA + reachB) {
        return 0;
    }

    func_ov039_02098bb0(posB, posA, &dir);
    dotA = func_ov039_02098c40(&dir, velA);

    func_ov039_02098bb0(posA, posB, &dir);
    dotB = func_ov039_02098c40(&dir, velB);

    return (dotA > 0 || dotB > 0) ? 1 : 0;
}

/**
 * @brief Pushes one pin's velocity along a direction and re-normalises it.
 *
 *  The angle is built in two Q12.12 steps, not one, and both round -- the
 *  `smull`/`adds #0x800`/`adc` pairs are two independent rounded multiplies.
 *
 *  The tail guard compares *addresses*: `adds r0, r4, #0x12C` / `bne` and
 *  `adds r0, r4, #0x130` / `popeq`, with no loads between them. Both +0x12C
 *  and +0x130 are read as data everywhere else in this overlay (0x0208e504
 *  writes through them), so a "vel is non-zero" reading cannot be what the
 *  source said -- that needs two `ldr`s. Written here as the pointer test the
 *  flags actually encode, which is the only reading consistent with zero
 *  loads. See the note in the header.
 */
void func_ov039_0208dff0(OtuPoint* dir, s32 speed, s32 scaleA, s32 scaleB, OtuBadge* other) {
    s32 mag = OTU_MUL_Q12(speed, scaleA);

    func_ov039_02098c00(OTU_MUL_Q12(mag, scaleB), dir, &other->vel, &other->vel);
    func_ov039_0208a6c4(&other->vel);

    if (&other->vel != NULL && (OtuPoint*)((u8*)&other->vel + 4) != NULL) {
        func_ov039_02098d3c(&other->vel, &other->dir);
    }
}

/**
 * @brief Measures how hard two pins are pushing apart, and in which direction.
 *
 *  The distance is scaled by a dot with the approach direction, so a pair
 *  that is close but not closing scores near zero rather than the full
 *  distance -- that is what makes the result usable as a force.
 *
 *  `out` doubles as the scratch: it is tested on entry, overwritten with
 *  `posA - velA` in the middle, normalised, and the final dot is taken against
 *  it. It is why the parameter is a pointer to a caller's point rather than a
 *  value.
 */
void func_ov039_0208e058(OtuPoint* posA, OtuPoint* velA, OtuPoint* posB, OtuPoint* velB, OtuPoint* out, s32* score) {
    OtuPoint dir;
    s32     dot;

    // Both of these take the two *velocities*, not the two positions -- the
    // first computes a relative speed and the second a relative velocity, and
    // only the third call below touches the positions.
    *score = func_ov039_02098ca8(velB, velA);
    func_ov039_02098bb0(velB, velA, &dir);

    if (out->x != 0 || out->y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    }

    func_ov039_02098bb0(posA, posB, out);

    if (out->x != 0 || out->y != 0) {
        func_ov039_02098d3c(out, out);
    } else {
        out->x = 0x1000;
        out->y = 0;
    }

    dot = func_ov039_02098c40(&dir, out);
    if (dot < 0) {
        dot = -dot;
    }

    *score = OTU_MUL_Q12(*score, dot);
}

/**
 * @brief The actual impulse: both pins are pushed apart along the line
 *        between them, and linked to each other.
 *
 *  Symmetric in every term -- the first half is (self's scale, other's scale,
 *  target self) and the second is the same with the two swapped and the angle
 *  negated -- which is why it is written as two nearly identical blocks rather
 *  than a loop.
 *
 *  The two scale tables are indexed by the *other* pin's row weight and the
 *  doubling is gated on the *other* pin's frame counter, so a pin still in its
 *  first few frames pushes twice as hard. The Q12.12 scale factor is 0x3000
 *  (`data_ov039_0209a388`) while a settled pin gets 0x1000, i.e. 3.0 against
 *  1.0.
 */
void func_ov039_0208e130(OtuBadge* self, OtuBadge* other) {
    OtuPoint dir;
    s32     score;
    s32     scale;

    func_ov039_0208e058(OTU_PIN_POS(self), OTU_PIN_VEL(self), OTU_PIN_POS(other), OTU_PIN_VEL(other), &dir, &score);

    // The row pointer is not hoisted into a local: the target re-reads both
    // `pinID` and `rowTable` from the task on each of the four lookups, so a
    // cached OtuPinRow* is a source-level difference, not a scheduling one.
    scale = (self->alive > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, OTU_MUL_Q12(score, scale), data_ov039_0209a3e4[OTU_PIN_ROW(self)->weight].scaleA,
                        (other->unk_1D0 > 0) ? data_ov039_0209a3e8[OTU_PIN_ROW(other)->weight].scaleB * 2
                                             : data_ov039_0209a3e8[OTU_PIN_ROW(other)->weight].scaleB,
                        self);
    self->partner = other;

    scale = (other->alive > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, -OTU_MUL_Q12(score, scale), data_ov039_0209a3e4[OTU_PIN_ROW(other)->weight].scaleA,
                        (self->unk_1D0 > 0) ? data_ov039_0209a3e8[OTU_PIN_ROW(self)->weight].scaleB * 2
                                            : data_ov039_0209a3e8[OTU_PIN_ROW(self)->weight].scaleB,
                        other);
    other->partner = self;
}

/**
 * @brief The pin-versus-pin test: kinds 1, 6 and 7 only, then hand over to
 *        0x0208e130.
 *
 *  Note this is a *narrower* kind set than 0x0208e37c, which drops 1's
 *  companions 6 and 7 out but admits 3 and 4. The two entry points are
 *  therefore not two spellings of one predicate: one is pin-against-pin
 *  attraction and the other is pin-against-board.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e28c(void* a, void* b) {
    OtuBadge* self  = (OtuBadge*)a;
    OtuBadge* other = (OtuBadge*)b;

    if (self->flags > 0) {
        return 0;
    }
    if (self->kind != 1 && self->kind != 6 && self->kind != 7) {
        return 0;
    }
    if (self->unk_128 != 0) {
        return 0;
    }
    if (other->flags > 0) {
        return 0;
    }
    if (other->kind != 1 && other->kind != 6 && other->kind != 7) {
        return 0;
    }
    if (other->unk_128 != 0) {
        return 0;
    }

    if (func_ov039_0208df2c(OTU_PIN_POS(self), OTU_PIN_VEL(self), 0xC000, OTU_PIN_POS(other),
                           OTU_PIN_VEL(other), 0xC000) == 0) {
        return 0;
    }

    func_ov039_0208e130(self, other);

    return 1;
}

/**
 * @brief The board-collision response: shove both pins apart along the line
 *        between them, each by half the overlap.
 *
 *  A `switch` over each pin's kind, both admitting {1,3,4,6,7} and rejecting
 *  {0,2,5,8} -- the target's two jump tables are identical. 0x18000 is 6.0 in
 *  Q12.12, so this fires inside six grid cells.
 *
 *  The two divides are signed `/ 2` on `gap` and on `-gap` separately, not
 *  `-(gap / 2)`: the target computes `rsb` *before* the `>> 31` sign fix-up, so
 *  an odd overlap rounds the two halves differently and reproduces the
 *  target's asymmetry.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e37c(void* a, void* b) {
    OtuBadge* self  = (OtuBadge*)a;
    OtuBadge* other = (OtuBadge*)b;

    OtuPoint dir;
    s32     dist;
    s32     gap;

    if (self->flags > 0) {
        return 0;
    }

    switch (self->kind) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (self->unk_128 != 0) {
        return 0;
    }

    if (other->flags > 0) {
        return 0;
    }

    switch (other->kind) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (other->unk_128 != 0) {
        return 0;
    }

    dist = func_ov039_02098ca8(OTU_PIN_POS(self), OTU_PIN_POS(other));

    if (dist > 0x18000) {
        return 0;
    }

    gap = 0x18000 - dist;

    func_ov039_02098bb0(OTU_PIN_POS(self), OTU_PIN_POS(other), &dir);

    if (dir.x == 0 && dir.y == 0) {
        dir.x = 0x1000;
        dir.y = 0;
    } else {
        func_ov039_02098d3c(&dir, &dir);
    }

    func_ov039_02098c00(gap / 2, &dir, OTU_PIN_POS(self), OTU_PIN_POS(self));
    func_ov039_02098c00((-gap) / 2, &dir, OTU_PIN_POS(other), OTU_PIN_POS(other));

    return 1;
}

/**
 * @brief The wall/obstacle response, and the one place in this band that
 *        reaches outside the pin task.
 *
 *  The second argument is *not* another pin: it is read through
 *  func_ov039_02092744 / _02092758 / _02092760, which are the accessors for
 *  the overlay's second, smaller object -- a point at +0x48/+0x4C, a radius
 *  at +0x50, and a flag at +0x54. Those three are already defined in
 *  OtuFieldAccess.c above the band includes, so they are called rather than
 *  declared, and the parameter is left `void*` because no type for that object
 *  exists yet.
 *
 *  The third block is the interesting one: the pin's velocity is normalised,
 *  measured, and turned into a *position* offset which lands in +0x138/+0x13C
 *  -- the pair func_ov039_0208e6cc copies out. So +0x138 is where the pin
 *  publishes the shove it received.
 */
/* Typed to band 9's declaration, which comes first in this translation
 * unit and so governs. Taking the real struct types here collides with it
 * and cascades into "expression syntax error" through the whole body. */
s32 func_ov039_0208e504(void* a, void* obstacle) {
    OtuBadge* self = (OtuBadge*)a;

    OtuPoint other;
    OtuPoint dir;
    s32     radius;
    s32     len;

    switch (self->kind) {
        case 1:
        case 6:
        case 7:
        case 9:
            break;

        default:
            return 0;
    }

    func_ov039_02092744(obstacle, &other);
    radius = func_ov039_02092758(obstacle);

    if (func_ov039_02098ca8(OTU_PIN_POS(self), &other) >= radius + 0xC000) {
        return 0;
    }

    func_ov039_02098bb0(OTU_PIN_POS(self), &other, &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    // Note the argument order: `other` is the base here, not `dir`, so this
    // moves the obstacle's copy of the point rather than the pin's.
    func_ov039_02098c00(radius + 0xC000, &dir, &other, OTU_PIN_POS(self));

    func_ov039_02098bb0(&other, OTU_PIN_POS(self), &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    len = func_ov039_02098c40(OTU_PIN_VEL(self), &dir);
    func_ov039_02098c00(-(len * 2), &dir, OTU_PIN_VEL(self), OTU_PIN_VEL(self));

    if (self->vel.x != 0 || self->vel.y != 0) {
        func_ov039_02098d3c(OTU_PIN_VEL(self), &dir);

        len = func_ov039_02098d10(OTU_PIN_VEL(self));
        func_ov039_02098bd4(OTU_MUL_Q12(len, data_ov039_0209a324), &dir, OTU_PIN_VEL(self));

        self->dir.x = dir.x;
        self->dir.y = dir.y;
    }

    func_ov039_02092760(obstacle);

    return 1;
}

