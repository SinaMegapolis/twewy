#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0208d3bc - 0x0208e890. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
// Size: 0x1C

/** Q12.12 multiply, rounded -- the form the target spells with smull + 0x800. */
#define OTU_MUL_Q12(a, b) ((s32)((((s64)(a) * (s64)(b)) + 0x800) >> 12))

// Size: 0x25C

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
    u8*       cell;

    self->dataType  = args->unk_00;
    self->pool      = pool;
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

    cell    = self->tileTable + self->tileIndex * 2;
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
s32 func_ov039_0208dcb0(TaskPool* pool, s32 arg1, s32 arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7,
                        s32 arg8, void* arg9) {
    OtuBadge_InitArgs args;
    OtuBadge*         self;
    s32               id;
    s32               i;

    args.unk_00   = arg1;
    args.unk_04   = arg2;
    args.unk_08   = arg3;
    args.unk_0C   = (u8*)arg4;
    args.unk_10   = arg5;
    args.rowTable = (u8*)arg6;
    args.pinID    = (u16*)arg7;
    args.unk_1C   = arg8;
    args.unk_20   = arg9;

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
        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->labelTask), 1, self->pinID[1] == 0x130 ? 1 : 0);
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
    s32      dotA;
    s32      dotB;

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
    s32      dot;

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
    s32      score;
    s32      scale;

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

    if (func_ov039_0208df2c(OTU_PIN_POS(self), OTU_PIN_VEL(self), 0xC000, OTU_PIN_POS(other), OTU_PIN_VEL(other), 0xC000) == 0)
    {
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
    s32      dist;
    s32      gap;

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
    s32      radius;
    s32      len;

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

/** The +0x138/+0x13C pair, copied out -- the fourth such pair on this object. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e6cc(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x138);
}

/** The +0x120/+0x124 pair, copied out. Used by the nearest-child queries. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e6e0(OtuPinTask* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x120);
}

/** A single word at +0x128. */
s32 func_ov039_0208e6f4(void* task) {
    return *(s32*)((u8*)task + 0x128);
}

/* ------------------------------------------------------------------ */
/* The pin-child steering helpers, 0x0208e6fc and 0x0208e8c4.         */
/* ------------------------------------------------------------------ */

/**
 * @brief The velocity a pin child should steer with, written to `out`.
 *
 * Only kind 8 enters the switch; kind 1 and everything past the two cases fall
 * through to the plain "point at the child's own +0x120/+0x124, rebased by the
 * board origin" answer, which is also the whole body for every other kind.
 *
 *   sub-kind 2  steers around a point offset from the pin by (0x80000, 0x60000);
 *               when that offset is at least 0x400 long the vector is
 *               normalised and the angle re-aimed at the pin's +0x118 face.
 *   sub-kind 3  scales the same offset by 0x1000 / (+0x100 << 12) first.
 *
 * The stack point is passed as both the source and the destination of
 * `func_ov039_02098bb0`; that aliasing is what the target does.
 */
// Nonmatching: 73%, and the whole gap is one scheduling choice. The target
// loads the +0x124 word before the +0x120 one and only then subtracts both;
// this source reads them in address order. Writing the two reads in either
// order, as an initialiser, or through named temporaries all move the score but
// none reproduces the target's pair, so this is mwcc's scheduler and not the
// source shape. Every instruction otherwise agrees.
void func_ov039_0208e6fc(void* task, OtuPoint* out) {
    u8* self = (u8*)task;

    if (*(s32*)(self + 0xF8) == 8) {
        switch (*(s32*)(self + 0xFC)) {
            case 2: {
                OtuPoint scratch = {*(s32*)(self + 0x120) - 0x80000, *(s32*)(self + 0x124) - 0x60000};

                func_ov039_02098bb0(&scratch, (OtuPoint*)(self + 0x118), &scratch);

                if (func_ov039_02098d10(&scratch) >= 0x400) {
                    func_ov039_02098d3c(&scratch, &scratch);
                    func_ov039_02098c00(0x400, &scratch, (OtuPoint*)(self + 0x118), out);
                    return;
                }

                out->x = *(s32*)(self + 0x120) - 0x80000;
                out->y = *(s32*)(self + 0x124) - 0x60000;
                return;
            }

            case 3: {
                OtuPoint scratch;
                s32      scale = FX_Divide(0x1000, *(s32*)(self + 0x100) << 12);

                scratch.x = *(s32*)(self + 0x120) - 0x80000;
                scratch.y = *(s32*)(self + 0x124) - 0x60000;
                func_ov039_02098bb0(&scratch, (OtuPoint*)(self + 0x118), &scratch);
                func_ov039_02098c00(scale, &scratch, (OtuPoint*)(self + 0x118), out);
                return;
            }

            default:
                break;
        }
    }

    out->x = *(s32*)(self + 0x120) - 0x80000;
    out->y = *(s32*)(self + 0x124) - 0x60000;
}

/** Writes the +0x110/+0x114 pair from a point. */
void func_ov039_0208e848(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x110) = *in;
}

/* ------------------------------------------------------------------ */
/* The stage task's 0x110 - 0x1B4 block.                              */
/* ------------------------------------------------------------------ */

/** Reads the +0x110/+0x114 pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e85c(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x110);
}

/** Writes the +0x118/+0x11C pair from two values. */
void func_ov039_0208e870(void* task, s32 x, s32 y) {
    *(s32*)((u8*)task + 0x118) = x;
    *(s32*)((u8*)task + 0x11C) = y;
}

/** Reads the +0x118/+0x11C pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_0208e87c(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x118);
}
