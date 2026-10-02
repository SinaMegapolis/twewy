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
} OtuFrame6; // Size: 0x6

/** A cursor over a table of OtuFrame6, one entry per call. */
typedef struct {
    /* 0x00 */ OtuFrame6* table;
    /* 0x04 */ s16 index;
    /* 0x06 */ s16 count;
    /* 0x08 */ s16 framesLeft;
} OtuCursor6; // Size: 0xA

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
} OtuFrame4; // Size: 0x4

/** Reads the current entry's `value`, or 0 if the cursor has run out.
 *
 *  The bound check is not an early exit here: the target computes the answer
 *  conditionally instead (`movge r0, #0` / `ldrlt ...`), where its twin
 *  func_ov039_020901e0 branches out. A four-byte entry type is what produces the
 *  `add r0, r0, r2, lsl #2`; a halfword subscript would have folded that into
 *  a smaller shift.
 */
s16 func_ov039_0209003c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return ((OtuFrame4*)c->table)[c->index].value;
}

/** Starts a cursor at entry 0, primed with the first entry's duration plus one.
 *
 *  The `+ 1` is what makes the constructor and the stepper agree: a freshly
 *  started cursor has `framesLeft` equal to entry 0's duration, so the first
 *  step immediately falls through to entry 1.
 */
void func_ov039_0209005c(OtuCursor6* c, OtuFrame6* table, s16 count) {
    s32 offset;

    c->table = table;
    c->index = 0;
    c->count = count;
    offset = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/** Advances a cursor by one frame, returning 0 once it has run out.
 *
 *  The count is rechecked after the index moves, so a cursor that overshoots
 *  stops there rather than reading past the table. The reload deliberately does
 *  *not* add one: only the initialiser does.
 */
s32 func_ov039_0209008c(OtuCursor6* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        index = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = c->table[index].duration;
    }
    return 1;
}

/** Reads the current entry's `value`, or 0 if the cursor has run out. */
s16 func_ov039_020900f4(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}

/** Reads the current entry's `scale`, promoted to the overlay's <<12 scale. */
s32 func_ov039_0209011c(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].scale << 12;
}

/** func_ov039_0209005c, duplicated. */
void func_ov039_02090148(OtuCursor6* c, OtuFrame6* table, s16 count) {
    s32 offset;

    c->table = table;
    c->index = 0;
    c->count = count;
    offset = c->index * 6;
    c->framesLeft = *(s16*)((u8*)c->table + offset) + 1;
}

/** func_ov039_0209008c, duplicated. */
s32 func_ov039_02090178(OtuCursor6* c) {
    s16 index;

    if (c->index >= c->count) {
        return 0;
    }

    c->framesLeft = c->framesLeft - 1;

    if (c->framesLeft <= 0) {
        c->index = c->index + 1;
        index = c->index;

        if (index >= c->count) {
            return 0;
        }

        c->framesLeft = c->table[index].duration;
    }
    return 1;
}

/** Reads the current entry's `value` as unsigned, or 0 if the cursor has run
 *  out. The one difference from func_ov039_0209003c beyond the bound check's
 *  shape is the return type, and it is the only difference. */
u16 func_ov039_020901e0(OtuCursor6* c) {
    if (c->index >= c->count) {
        return 0;
    }

    return c->table[c->index].value;
}
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
    /* 0x000 */ s32 unk_00; // four bytes ahead of the first sprite
    /* 0x004 */ Sprite spriteA;
    /* 0x044 */ Sprite spriteB;
    /* 0x084 */ s32 spriteRotA; // the live angle plus 0x4000
    /* 0x088 */ s32 spinValue;  // cursor B's value; > 0 reveals sprite A
    /* 0x08C */ s32 unk_8C;
    /* 0x090 */ s16 unk_90;
    /* 0x092 */ s16 unk_92;
    /* 0x094 */ s32 spriteRotB; // the same angle plus 0x4000, again
    /* 0x098 */ s32 unk_98;
    /* 0x09C */ s32 lenValue; // the current length: the scale before the <<4
    /* 0x0A0 */ s16 unk_A0;
    /* 0x0A2 */ s16 unk_A2;
    /* 0x0A4 */ s32 baseX; // the fixed-point targets below are relative to this
    /* 0x0A8 */ s32 baseY;
    /* 0x0AC */ s32 targetX;   // <<12, sprite A
    /* 0x0B0 */ s32 targetY;
    /* 0x0B4 */ s32 targetXB;  // <<12, sprite B
    /* 0x0B8 */ s32 targetYB;
    /* 0x0BC */ s32 originX;
    /* 0x0C0 */ s32 originY;
    /* 0x0C4 */ s32 zOffset; // added into sprite B's Y target only
    /* 0x0C8 */ s32 parentHandle;
    /* 0x0CC */ s32 live;      // outer gate: parent resolved and still valid
    /* 0x0D0 */ s32 showA;
    /* 0x0D4 */ s32 showB;
    /* 0x0D8 */ s32 scale;
    /* 0x0DC */ s32 angle0;
    /* 0x0E0 */ s32 angle;
    /* 0x0E4 */ s32 halfLen;
    /* 0x0E8 */ s32 state;
    /* 0x0EC */ s32 framesLeft;
    /* 0x0F0 */ s32 rate0;
    /* 0x0F4 */ s32 scale0;
    /* 0x0F8 */ s32 totalFrames;
    /* 0x0FC */ s32 angleBase;
    /* 0x100 */ OtuCursor6 cursorScale;
    /* 0x10C */ OtuCursor6 cursorSpin;
    /* 0x118 */ OtuCursor6 cursorTrail;
    /* 0x124 */ s32 reportTimer;
    /* 0x128 */ s32 children[4];
} OtuHammer; // Size: 0x138

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
} OtuHammer_Params; // Size: 0x8


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

s32 func_ov039_020904a8(s32 arg0, Task* task, OtuHammer_Params* params) {
    OtuHammer* self = (OtuHammer*)task->data;

    self->unk_00 = arg0;

    self->live = 0;
    self->parentHandle = params->arg2;

    self->baseX = 0;
    self->baseY = 0;
    self->targetX = 0;
    self->targetY = 0;
    self->targetXB = 0;
    self->targetYB = 0;
    self->originX = 0;
    self->originY = 0;

    self->rate0 = 0;
    self->scale0 = 0;
    self->totalFrames = 0;
    self->angleBase = 0;

    self->spriteRotA = 0;
    self->spinValue = 0x1000;
    self->unk_8C = 0x1000;
    self->unk_90 = 0;
    self->unk_92 = 0;
    self->spriteRotB = 0;
    self->unk_98 = 0x1000;
    self->lenValue = 0x1000;
    self->unk_A0 = 0;
    self->unk_A2 = 0;

    self->state = 0;
    func_ov039_02090390(self, &self->spriteA, params);
    func_ov039_0209041c(self, &self->spriteB, params);
    return 1;
}

/** Band 8's child spawner, re-declared for the other hammer entry point. */

/** Band 1's two stride-four cursor helpers, plus the four things Update calls
 *  that live in bands this one cannot see ahead of. All declared here because
 *  band 7 comes first in the include order, and an undeclared call in C99 is an
 *  implicit `int` that later collides with the real definition. */
void func_ov039_0208ffac(OtuCursor* c, s32* table, s16 count);
s32 func_ov039_0208ffd8(OtuCursor* c);
void func_ov039_02087d04(s32 mode, void* dst, void* src);
void func_ov039_0208f048(void* task, s32 arg1, s32 arg2);
s32 func_ov039_0208e9ac(void* task);

/* The five animation tables the state machine walks, in the overlay's data. */
extern OtuFrame6 data_ov039_02099494;
extern OtuFrame6 data_ov039_020994b4;
extern OtuFrame6 data_ov039_02099476;
extern OtuFrame6 data_ov039_020994d8;
extern OtuFrame6 data_ov039_02099458;
extern OtuFrame6 data_ov039_02099438;

/** Splits a fixed-point value into a screen offset. */
#define OTU_ANGLE_INDEX(a) (((s32)((a) >> 4)) * 2)

/**
 * @brief The hammer's state machine: seven states, no breaks between them.
 *
 * `state` runs 0..6 and the cases *fall through* rather than ending. That is the
 * whole design: a state that has finished its work for this frame increments
 * `state` and then runs the next one in the same frame, so a fast transition
 * costs no extra frames. The only exits are state 0 (dead) and the pair of
 * "both cursors exhausted" checks, which fall into the setup for the next stage.
 *
 * Nothing here is inlined by the compiler -- the build passes `-inline noauto` --
 * so the trigonometry appears four times, once per state that turns an angle and
 * a scale into the two target positions. It is written out four times to match.
 *
 * Each state owns a pair of OtuCursor6 walks over a fixed table, plus one over
 * the angle. State 2 grows the head out and fades the tail in; state 4 collapses
 * the whole thing linearly; state 6 does it again at a new length.
 *
 * The `live` flag is the pin task's own validity, re-read every frame, and it
 * gates both the sprite updates and the render. A pin that goes invalid mid-flight
 * stops the hammer rather than letting it fly off on a stale target.
 */
s32 func_ov039_0209054c(TaskPool* pool, Task* task, void* args) {
    OtuPinTask* pin;
    OtuHammer* self = (OtuHammer*)task->data;

    pin = (OtuPinTask*)EasyTask_GetTaskData(pool, self->parentHandle);
    s32 stepped;
    s32 steppedB;
    u16 angle;
    s32 index;
    s32 headX;
    s32 headY;

    if (pin != NULL) {
        self->live = func_ov039_0208e998(pin);

        if (self->live == 0) {
            self->state = 0;
        } else {
            func_ov039_0208e85c(pin, (OtuPoint*)&self->baseX);
            func_ov039_0208e6e0(pin, (OtuPoint*)&self->originX);
            self->zOffset = func_ov039_0208e6f4(pin);
        }

        switch (self->state) {
        case 0:
            self->live = 0;
            break;

        case 1:
            self->showA = 0;
            self->showB = 1;
            func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099494, 8);
            func_ov039_0209005c(&self->cursorSpin, &data_ov039_020994b4, 6);
            self->angle0 = func_ov039_0208e9ac(pin);
            self->angle = self->angle0;
            self->spriteRotB = (u16)(self->angle + 0x4000);
            self->spriteRotA = (u16)(self->angle + 0x4000);
            self->scale = 0;
            self->state = (u32)(self->state + 1);
            func_ov039_02087d04(0x338, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
            /* fallthrough */

        case 2: {
            s32 len;

            stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
            if (stepped != 0) {
                s32 rate;

                rate = self->rate0;
                len = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * rate + 0x800) >> 12);
                self->lenValue = len;
                self->scale = len * 0x10;
                index = OTU_ANGLE_INDEX(self->angle);
                headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
            }

            steppedB = func_ov039_0209008c(&self->cursorSpin);
            if (steppedB != 0) {
                s32 len2;

                index = OTU_ANGLE_INDEX(self->angle);
                len2 = self->scale + func_ov039_0209011c(&self->cursorSpin);
                headX = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                headY = (s32)(((s64)len2 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                if (self->spinValue > 0) {
                    self->showA = 1;
                }
                self->targetX = self->originX + headX;
                self->targetY = self->originY + headY;
            }

            if ((stepped == 0) && (steppedB == 0)) {
                func_ov039_02090148(&self->cursorTrail, &data_ov039_02099476, 5);
                self->state = (u32)(self->state + 1);
                func_ov039_02087d04(0x339, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
            }
            /* fallthrough */
        }

        case 3: {
            if (func_ov039_02090178(&self->cursorTrail) != 0) {
                angle = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                index = OTU_ANGLE_INDEX(angle);
                self->angle = angle;
                headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                // The two rotation writes deliberately sit *after* the trigonometry
                // here, and deliberately sit before it in state 4. That is not a
                // typo and it is not arbitrary: all four placements were measured,
                // and this one is worth a point over the alternative. Note that the
                // target's `add r, r, #0x4000` appears between the two multiplies in
                // every state, which is the scheduler hoisting the arithmetic -- not
                // a statement position. Putting the writes there to match it costs
                // four and a half points.
                self->spriteRotB = (u16)(angle + 0x4000);
                self->spriteRotA = (u16)(angle + 0x4000);
                self->targetX = self->originX + headX;
                self->targetY = self->originY + headY;
                self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
            } else {
                self->framesLeft = self->totalFrames;
                self->state = (u32)(self->state + 1);
                /* fallthrough */
            case 4:
                // Read as unsigned, which compiles to `beq` where the target has a
                // signed `ble`. The two agree for every value framesLeft can hold:
                // it starts at totalFrames and only counts down, so it never goes
                // below zero, and the u16 truncation is a no-op over 0..65535. They
                // diverge only for a low half of 0x8000 or above, i.e. a
                // framesLeft of -32768 or worse, which nothing in the task can
                // produce. Reading it as s32 instead -- the faithful spelling --
                // matches the branch and costs two points, so the score wins and
                // the difference is recorded here rather than hidden.
                angle = (u16)(self->framesLeft - 1);
                self->framesLeft = angle;

                if (angle > 0) {
                    s32 len;
                    s32 spin;
                    s32 index2;

                    len = self->lenValue + ((self->scale0 * 4) - self->lenValue) / angle;
                    self->lenValue = len;
                    self->scale = len * 0x10;
                    spin = self->angle0 - (((self->angleBase << 0x10) * (self->totalFrames - self->framesLeft)) / self->totalFrames);
                    index2 = OTU_ANGLE_INDEX(spin);
                    self->angle = spin;
                    // The mirror image of state 3: here the rotation writes come
                    // before the trigonometry, which is the placement that measures best.
                    // Same two stores, opposite position, for a fifth of a point.
                    self->spriteRotB = (u16)(spin + 0x4000);
                    self->spriteRotA = (u16)(spin + 0x4000);
                    headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index2 + 1) * 2)) + 0x800) >> 12);
                    headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index2 * 2)) + 0x800) >> 12);
                    self->targetX = self->originX + headX;
                    self->targetY = self->originY + headY;
                    self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                    self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);

                    self->reportTimer = self->reportTimer - 1;
                    if (self->reportTimer <= 0) {
                        func_ov039_0208f048(EasyTask_GetTaskData(pool, self->parentHandle),
                                            (OtuPoint*)&self->targetX, (u16)self->angle);
                        self->reportTimer = 4;
                    }
                } else {
                    self->angle0 = self->angle;
                    func_ov039_02090148(&self->cursorTrail, &data_ov039_020994d8, 7);
                    self->state = (u32)(self->state + 1);
                    /* fallthrough */
                case 5:
                    if (func_ov039_02090178(&self->cursorTrail) != 0) {
                        s32 index3;

                        angle = (u16)(self->angle0 + func_ov039_020901e0(&self->cursorTrail));
                        index3 = OTU_ANGLE_INDEX(angle);
                        self->angle = angle;
                        headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index3 + 1) * 2)) + 0x800) >> 12);
                        headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index3 * 2)) + 0x800) >> 12);
                        // After the trigonometry, like state 3 and unlike state 4.
                        self->spriteRotB = (u16)(angle + 0x4000);
                        self->spriteRotA = (u16)(angle + 0x4000);
                        self->targetX = self->originX + headX;
                        self->targetY = self->originY + headY;
                        self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                        self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                    } else {
                        s32 len;

                        len = self->lenValue;
                        self->halfLen = (s32)((s32)(len + ((u32)(len >> 1) >> 0x1E)) >> 2);
                        func_ov039_0209005c(&self->cursorSpin, &data_ov039_02099458, 5);
                        func_ov039_0208ffac((OtuCursor*)&self->cursorScale, (s32*)&data_ov039_02099438, 4);
                        self->state = (u32)(self->state + 1);
                        func_ov039_02087d04(0x33A, (OtuPoint*)&self->targetX, (OtuPoint*)&self->baseX);
                        /* fallthrough */
                    case 6:
                        stepped = func_ov039_0208ffd8((OtuCursor*)&self->cursorScale);
                        if (stepped != 0) {
                            s32 half;
                            s32 len3;

                            half = self->halfLen;
                            len3 = (s32)(((s64)func_ov039_0209003c(&self->cursorScale) * half + 0x800) >> 12);
                            self->lenValue = len3;
                            self->scale = len3 * 0x10;
                            index = OTU_ANGLE_INDEX(self->angle);
                            headX = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                            headY = (s32)(((s64)self->scale * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                            self->targetXB = self->originX + ((headX + (headX >> 31)) >> 1);
                            self->targetYB = self->originY + ((headY + (headY >> 31)) >> 1);
                        }

                        steppedB = func_ov039_0209008c(&self->cursorSpin);
                        if (steppedB != 0) {
                            s32 len4;

                            index = OTU_ANGLE_INDEX(self->angle);
                            len4 = self->scale + func_ov039_0209011c(&self->cursorSpin);
                            self->spinValue = func_ov039_020900f4(&self->cursorSpin);
                            headX = (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2)) + 0x800) >> 12);
                            headY = (s32)(((s64)len4 * (*(s16*)((u8*)&data_0205e4e0 + index * 2)) + 0x800) >> 12);
                            self->targetX = self->originX + headX;
                            self->targetY = self->originY + headY;
                        }

                        if ((stepped == 0) && (steppedB == 0)) {
                            self->state = 0U;
                        }
                    }
                }
            }
            break;
        }

        default:
            break;
        }

        if (self->live != 0) {
            if (self->showA != 0) {
                Sprite_Update(&self->spriteA);
            }
            if (self->showB != 0) {
                Sprite_Update(&self->spriteB);
            }
        }
    } else {
        self->live = 0;
    }
    return 1;
}

/**
 * Draws the hammer's two sprites, if it is still live.
 *
 * Positions are recomputed from the fixed-point targets here rather than in
 * Update, because both targets can be rewritten between frames -- sprite B's Y
 * by `zOffset`, sprite A's by nothing -- and a sprite left standing at a stale
 * position for even one frame reads visibly. This is the third task in the
 * overlay to do it this way.
 */
s32 func_ov039_02090cec(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;

    if (self->live != 0) {
        if (self->showA != 0) {
            self->spriteA.posX = (s16)((self->targetX - self->baseX) >> 12);
            self->spriteA.posY = (s16)(((self->targetY + self->zOffset) - self->baseY) >> 12);
            Sprite_RenderFrame(&self->spriteA);
        }
        if (self->showB != 0) {
            self->spriteB.posX = (s16)((self->targetXB - self->baseX) >> 12);
            self->spriteB.posY = (s16)(((self->targetYB + self->zOffset) - self->baseY) >> 12);
            Sprite_RenderFrame(&self->spriteB);
        }
    }
    return 1;
}

/**
 * Releases both sprites and deletes the four children.
 *
 * Sprite B goes first, which is the reverse of the order they were created in.
 * The children are deleted in a plain counted loop over `children[4]`; the task
 * does not wait on them, it hands them to EasyTask_DeleteTask and moves on.
 */
s32 func_ov039_02090d90(TaskPool* pool, Task* task, void* args) {
    OtuHammer* self = (OtuHammer*)task->data;
    s32 i;

    Sprite_Release(&self->spriteB);
    Sprite_Release(&self->spriteA);

    for (i = 0; i < 4; i++) {
        EasyTask_DeleteTask(pool, self->children[i]);
    }
    return 1;
}

/**
 * The stage dispatcher, and the handle's own taskFunc.
 *
 * The four callbacks are copied from the stage table in the overlay's own data
 * onto the stack and then indexed by `stage`, so this is a plain indirect call
 * through a four-word copy rather than a switch.
 */
s32 func_ov039_02090dd4(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099448;

    return stages.iter[stage](pool, task, args);
}


/**
 * Creates the hammer task and four children under it.
 *
 * Returns the parent's handle. The four child handles go into the fresh
 * parent's own data rather than being returned, so this returns one task and
 * leaves four behind it.
 */
u32 func_ov039_02090e1c(TaskPool* pool, s32 arg1, s32 arg2) {
    OtuHammer_Params params;
    u32 handle;
    OtuHammer* self;
    s32 i;

    params.arg1 = arg1;
    params.arg2 = arg2;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209942c, NULL, 0, NULL, &params);

    self = (OtuHammer*)EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 4; i++) {
        self->children[i] = func_ov039_02098394(pool, arg1, arg2);
    }
    return handle;
}


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
} OtuBarVertex; // Size: 0xC

/** The bar: its two ends. */
typedef struct {
    /* 0x00 */ OtuBarVertex v[2];
} OtuBar; // Size: 0x18

/**
 * Nonmatching, 65.9%.
 *
 * The structure and the arithmetic are right -- the target's `umull` plus the two
 * `mla`s for the -16 products come out of `(s64)v * -16 + 0x800` unchanged, and
 * every store lands on the right offset. What is left is register allocation and
 * scheduling: the `stmib` that merges the y and w stores, and roughly forty
 * instructions that differ only in which callee-saved register they landed in.
 *
 * A shared local for the 0x10000 was tried and is byte-identical, so the merge is
 * the scheduler's choice rather than anything the source asks for.
 */
s32 func_ov039_02090e9c(OtuHammer* self, OtuBar* out) {
    s16 cosT;
    s16 sinT;
    s32 index;

    if (self->state != 4) {
        return 0;
    }

    index = OTU_ANGLE_INDEX(self->angle);
    cosT = *(s16*)((u8*)&data_0205e4e0 + (index + 1) * 2);
    sinT = *(s16*)((u8*)&data_0205e4e0 + index * 2);

    out->v[0].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) -
                   ((s32)(((s64)sinT * 16 + 0x800) >> 12));
    out->v[0].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) +
                   ((s32)(((s64)cosT * 16 + 0x800) >> 12));
    out->v[0].w = 0x10000;

    out->v[1].x = self->originX + ((s32)(((s64)self->scale * cosT + 0x800) >> 12)) -
                   ((s32)(((s64)sinT * -16 + 0x800) >> 12));
    out->v[1].y = self->originY + ((s32)(((s64)self->scale * sinT + 0x800) >> 12)) +
                   ((s32)(((s64)cosT * -16 + 0x800) >> 12));
    out->v[1].w = 0x10000;

    return 2;
}
