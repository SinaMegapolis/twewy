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
    /* 0x084 */ s32 spriteAngleA; // both angles are the live angle plus 0x4000
    /* 0x088 */ s32 spriteScaleA; // unity at spawn
    /* 0x08C */ s32 spriteScaleB; // unity at spawn
    /* 0x090 */ s16 unk_90;
    /* 0x092 */ s16 unk_92;
    /* 0x094 */ s32 spriteAngleB;
    /* 0x098 */ s32 spinScaleA;
    /* 0x09C */ s32 spinScaleB;
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

    self->spriteAngleA = 0;
    self->spriteScaleA = 0x1000;
    self->spriteScaleB = 0x1000;
    self->unk_90 = 0;
    self->unk_92 = 0;
    self->spriteAngleB = 0;
    self->spinScaleA = 0x1000;
    self->spinScaleB = 0x1000;
    self->unk_A0 = 0;
    self->unk_A2 = 0;

    self->state = 0;
    func_ov039_02090390(self, &self->spriteA, params);
    func_ov039_0209041c(self, &self->spriteB, params);
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
