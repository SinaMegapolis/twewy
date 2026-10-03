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
extern OtuPinTask* func_ov039_0208817c(TaskPool* pool, void* scene, s32 index);
extern OtuPinTask* func_ov039_02088294(TaskPool* pool, void* scene, s32 index);
extern OtuPinTask* func_ov039_02087e2c(TaskPool* pool, void* scene, s32 index);
extern s32        func_ov039_02097ad8(void* data);
extern s32        func_ov039_0209167c(void* data);
extern s32        func_ov039_02091014(void* data);

/* --- family A: should this pin run its update this frame? ---------------- */

/**
 * @brief Frame-budget test for one pin kind: roll against chanceTbl[5], require
 *        the timer at +0x17A to be positive, require the scene to still have
 *        that pin, then require it to be within 0x46000 of us.
 *
 *  0x46000 is 18.0 in Q12.12. Returns 1 when the update should run.
 */
s32 func_ov039_0208c4ec(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[5] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7A) <= 0) {
        return 0;
    }

    other = func_ov039_0208817c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x46000) {
        return 0;
    }

    func_ov039_0208ba34(self);

    return 1;
}

/** @brief As 0x0208c4ec, but chanceTbl[6], the timer at +0x178, a speed cap of
 *         0x3000, and its own kind handler. */
s32 func_ov039_0208c568(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[6] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x78) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02088294(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x46000) {
        return 0;
    }

    func_ov039_0208b94c(self);

    return 1;
}

/** @brief As 0x0208c568, but chanceTbl[7] and a 0x32000 distance limit. */
s32 func_ov039_0208c5f8(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[7] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x78) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x32000) {
        return 0;
    }

    func_ov039_0208b94c(self);

    return 1;
}

/** @brief chanceTbl[8], the timer at +0x17C, and a 0x64000 limit. */
s32 func_ov039_0208c688(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[8] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7C) <= 0) {
        return 0;
    }

    if (func_ov039_02098d10(&self->vel) >= 0x3000) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x64000) {
        return 0;
    }

    func_ov039_0208b9b4(self);

    return 1;
}

/** @brief chanceTbl[9], the timer at +0x17A, no speed cap, 0x64000. */
s32 func_ov039_0208c718(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->chanceTbl[9] < RNG_Next(0x10000)) {
        return 0;
    }

    if (*(s16*)((u8*)self + 0x100 + 0x7A) <= 0) {
        return 0;
    }

    other = func_ov039_02087e2c(self->pool, self->scene, self->index);

    if (other == NULL) {
        return 0;
    }

    if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0x64000) {
        return 0;
    }

    func_ov039_0208ba34(self);

    return 1;
}

/* --- the two states that latch on curAI ---------------------------------- */

/**
 * @brief The chase state, curAI 0xA: acquire a target once, then steer toward
 *        it for as long as it is still alive.
 *
 *  The acquire path and the chase path both end by returning 1, so the `goto`
 *  is the target's own shape and a flag would not compile the same.
 */
s32 func_ov039_0208c794(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->curAI != 0xA) {
        if (self->chanceTbl[10] < RNG_Next(0x10000)) {
            return 0;
        }

        other = func_ov039_02087e2c(self->pool, self->scene, self->index);

        if (other == NULL) {
            return 0;
        }

        if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->x) > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&other->x);

        self->curAI      = 0xA;
        self->unk_1B8    = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&self->chaseTarget->x);
    }

    return 1;
}

/** @brief The homing state, curAI 0xB: acquire once when standing on tile 12,
 *         then do nothing but answer 1. */
s32 func_ov039_0208c84c(OtuBadgeState* self) {
    if (self->curAI != 0xB) {
        if (self->chanceTbl[11] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board) != 0xC) {
            return 0;
        }

        self->curAI   = 0xB;
        self->unk_1B8 = 0x3C;
    }

    return 1;
}

/* --- the four task-spawner wrappers -------------------------------------- */

/* Each just seeds 0x0208c128 with a fixed triple and a flag. The argument that
 * lands on the stack is written out longhand because mwcc builds the outgoing
 * area from the declaration, not from the call. */

void func_ov039_0208c8ac(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xC, 0xC, 0xC, 1);
}

void func_ov039_0208c8cc(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xD, 5, 5, 1);
}

void func_ov039_0208c8ec(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xE, 7, 7, 1);
}

void func_ov039_0208c90c(OtuBadgeState* self) {
    func_ov039_0208c128(self, 0xF, 8, 0xB, 1);
}

/** @brief The third latching state, curAI 0x10 -- same shape as 0x0208c794. */
s32 func_ov039_0208c92c(OtuBadgeState* self) {
    OtuPinTask* other;

    if (self->curAI != 0x10) {
        if (self->chanceTbl[16] < RNG_Next(0x10000)) {
            return 0;
        }

        other = func_ov039_02087e2c(self->pool, self->scene, self->index);

        if (other == NULL) {
            return 0;
        }

        func_ov039_0208be30(self, (OtuPoint*)&other->x);

        self->curAI       = 0x10;
        self->unk_1B8     = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, (OtuPoint*)&self->chaseTarget->x);
    }

    return 1;
}

/**
 * @brief Ask every live pin, in turn, whether it still wants to act.
 *
 *  The table at data_ov039_0209a4b0 holds one function pointer per entry, and
 *  the loop runs from index 1 to 0x10 inclusive. Any non-zero answer stops the
 *  walk, so this reads as an early-out over the table.
 */
void func_ov039_0208c9cc(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    s32 (**fn)(OtuBadgeState*);
    s32 i;

    if (self->unk_148 > 0) {
        return;
    }

    if (self->unk_128 != 0) {
        return;
    }

    fn = (s32 (**)(OtuBadgeState*))data_ov039_0209a4b0;

    for (i = 1; i < 0x11; i++) {
        if (fn[i](self) != 0) {
            return;
        }
    }
}

/**
 * @brief Step the pin along the board, in whichever of two directions the
 *        target's own phase field asks for.
 *
 *  Nonmatching: the target derives a cell pair with a rounding `asr #4 / add
 *  lsr #0x1b / asr #5` sequence per axis, then walks a two-byte-per-cell table
 *  at board+0x18 comparing 16-bit values, then reads bytes at +2 and +3 of the
 *  winning cell to rebuild the position as ((byte << 5) + 0x10) << 12. The
 *  arithmetic here reproduces the intent; mwcc's rounding and the ldrh/ldrb
 *  pairing are not reproduced yet.
 */
void func_ov039_0208ca1c(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    OtuPinTask* data;
    s32         gx;
    s32         gy;
    u8          cx;
    u8          cy;
    s32         i;
    s32         count;

    if (self->unk_FC != 0) {
        return;
    }

    data = (OtuPinTask*)EasyTask_GetTaskData(self->pool, self->taskId);

    if (func_ov039_02097ad8(data) != 0) {
        return;
    }

    gx = self->pos.x >> 0xC;
    gy = self->pos.y >> 0xC;
    gx = gx + ((gx >> 4) >> 27);
    gy = gy + ((gy >> 4) >> 27);
    cx = (u8)(gx >> 5);
    cy = (u8)(gy >> 5);

    count = self->board[5];

    for (i = 0; i < count; i++) {
        u16 v = *(u16*)(self->board + 0x18 + i * 4);

        if (v != *(u16*)(cx)) {
            goto found;
        }
    }

    if (i >= count) {
        return;
    }

found:
    self->pos.x = (((u32)self->board[0x18 + i * 4 + 2] << 5) + 0x10) << 0xC;
    self->pos.y = (((u32)self->board[0x18 + i * 4 + 3] << 5) + 0x10) << 0xC;
    self->unk_1A4 = 1;
    self->lastCellX = self->board[0x18 + i * 4 + 2];
    self->lastCellY = self->board[0x18 + i * 4 + 3];

    func_ov039_02097aa4((OtuTaskSprite*)data, (OtuTaskParams*)&self->pos, 2);

    self->unk_FC = 1;
}

/**
 * @brief One frame of the pin: release, tick, fade out and tear down.
 */
void func_ov039_0208cb64(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board) == 0) {
        self->phase    = 1;
        self->step     = 0;
        *(s32*)((u8*)self + 0xD0)  = 0x1000;
        *(s32*)((u8*)self + 0xD4)  = 0x1000;
        return;
    }

    if (self->unk_148 > 0) {
        return;
    }

    if (self->home != NULL) {
        if ((self->contactFlags & 0x82) == 0) {
            return;
        }

        func_ov039_0208ba70(self);
        return;
    }

    func_ov039_0208c218(self);

    *(s32*)((u8*)self + 0xD0) = OTU_MUL_Q12(self->frameBudget, 0x88888889);
    *(s32*)((u8*)self + 0xD4) = *(s32*)((u8*)self + 0xD0);

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae14(self);

    func_ov039_02087d04(0x33C, &self->pos, &self->origin);

    Sprite_Release((Sprite*)((u8*)self + 0xC));

    func_ov039_0209657c((Sprite*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x234)));
}

/**
 * @brief The main update: tick the frame budget, advance the tray slot, and
 *        refresh the counter and the two linked-list tasks.
 */
void func_ov039_0208cc4c(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    s32 i;

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae8c(self);

    if (self->pinID[0] >= 0x130) {
        goto refresh;
    }

    self->pinID = self->pinID + 1;

    func_ov039_02093d18((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)),
                        self->pinID);

    if (self->unk_1A8 != 0) {
        if (self->pinID[0] >= 0x130) {
            goto refresh;
        }

        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)),
                           1, self->pinID[1] == 0x130);
    }

    if (self->pinID[0] >= 0x130) {
        return;
    }

    func_ov039_02087d04(0x33F, &self->pos, &self->origin);
    return;

refresh:
    self->unk_1AC = 0;

    func_ov039_02093d68((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)),
                        self->unk_1AC);

    func_ov039_0208d5dc((OtuBadge*)self);

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x228 + i * 4)));
    }
}

/**
 * @brief The fade-out state.
 */
void func_ov039_0208cd50(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (self->unk_1A8 == 0) {
        return;
    }

    EasyFade_FadeMainDisplay(3, 0, 0x1E);

    if (self->unk_FC == 0) {
        return;
    }

    if (self->unk_FC != 1) {
        return;
    }

    self->unk_164 = self->unk_164 - 1;

    if (self->unk_164 > 0) {
        return;
    }

    if (self->unk_1A8 != 0) {
        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)), 2, 0);
    }

    *(s32*)((u8*)gFaders + 8) = 0x10000;

    EasyFade_FadeMainDisplay(2, 0x10, 0x1000);

    self->unk_164 = 0x34;
    self->unk_FC  = 1;
}


/* --- the two one-line probes -------------------------------------------- */

/** @brief If that probe task is finished, retire the pin. */
void func_ov039_0208ce20(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_0209167c((void*)EasyTask_GetTaskData(self->pool, self->taskId2)) != 0) {
        return;
    }

    self->phase = 1;
    self->step  = 0;
}

/** @brief The same shape, against a different task id. */
void func_ov039_0208ce54(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_02091014((void*)EasyTask_GetTaskData(self->pool, self->taskId1)) != 0) {
        return;
    }

    self->phase = 1;
    self->step  = 0;
}