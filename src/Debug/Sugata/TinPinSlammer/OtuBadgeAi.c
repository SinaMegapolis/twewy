#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0208c4ec - 0x0208d3bc. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/* --- family A: should this pin run its update this frame? ---------------- */

/**
 * @brief Frame-budget test for one pin phase: roll against chanceTbl[5], require
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

    if (func_ov039_02098ca8(&self->pos, &other->pos) > 0x46000) {
        return 0;
    }

    func_ov039_0208ba34(self);

    return 1;
}

/** @brief As 0x0208c4ec, but chanceTbl[6], the timer at +0x178, a speed cap of
 *         0x3000, and its own phase handler. */
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

    if (func_ov039_02098ca8(&self->pos, &other->pos) > 0x46000) {
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

    if (func_ov039_02098ca8(&self->pos, &other->pos) > 0x32000) {
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

    if (func_ov039_02098ca8(&self->pos, &other->pos) > 0x64000) {
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

    if (func_ov039_02098ca8(&self->pos, &other->pos) > 0x64000) {
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

        if (func_ov039_02098ca8(&self->pos, &other->pos) > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, &other->pos);

        self->curAI       = 0xA;
        self->unk_1B8     = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, &self->chaseTarget->pos);
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

        func_ov039_0208be30(self, &other->pos);

        self->curAI       = 0x10;
        self->unk_1B8     = 1;
        self->chaseTarget = other;

        return 1;
    }

    if (func_ov039_0208efb0(self->chaseTarget, 0x444) != 0) {
        func_ov039_0208be30(self, &self->chaseTarget->pos);
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

    if (self->alive > 0) {
        return;
    }

    if (self->unk_128 != 0) {
        return;
    }

    fn = (s32(**)(OtuBadgeState*))data_ov039_0209a4b0;

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
    OtuPinTask*    data;
    s32            gx;
    s32            gy;
    u8             cx;
    u8             cy;
    s32            i;
    s32            count;

    if (self->subKind != 0) {
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
    self->pos.x     = (((u32)self->board[0x18 + i * 4 + 2] << 5) + 0x10) << 0xC;
    self->pos.y     = (((u32)self->board[0x18 + i * 4 + 3] << 5) + 0x10) << 0xC;
    self->unk_1A4   = 1;
    self->lastCellX = self->board[0x18 + i * 4 + 2];
    self->lastCellY = self->board[0x18 + i * 4 + 3];

    func_ov039_02097aa4((OtuTaskSprite*)data, (OtuTaskParams*)&self->pos, 2);

    self->subKind = 1;
}

/**
 * @brief One frame of the pin: release, tick, fade out and tear down.
 */
void func_ov039_0208cb64(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board) == 0) {
        self->phase               = 1;
        self->step                = 0;
        *(s32*)((u8*)self + 0xD0) = 0x1000;
        *(s32*)((u8*)self + 0xD4) = 0x1000;
        return;
    }

    if (self->alive > 0) {
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
    s32            i;

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae8c(self);

    if (self->pinID[0] >= 0x130) {
        goto refresh;
    }

    self->pinID = self->pinID + 1;

    func_ov039_02093d18((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)), self->pinID);

    if (self->hasLabel != 0) {
        if (self->pinID[0] >= 0x130) {
            goto refresh;
        }

        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)), 1, self->pinID[1] == 0x130);
    }

    if (self->pinID[0] >= 0x130) {
        return;
    }

    func_ov039_02087d04(0x33F, &self->pos, &self->origin);
    return;

refresh:
    self->unk_1AC = 0;

    func_ov039_02093d68((OtuCounterData*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x1F4)), self->unk_1AC);

    func_ov039_0208d5dc(self);

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x228 + i * 4)));
    }
}

/**
 * @brief The fade-out state.
 */
void func_ov039_0208cd50(OtuBadge* arg) {
    OtuBadgeState* self = (OtuBadgeState*)arg;
    if (self->hasLabel == 0) {
        return;
    }

    EasyFade_FadeMainDisplay(3, 0, 0x1E);

    if (self->subKind == 0) {
        return;
    }

    if (self->subKind != 1) {
        return;
    }

    self->unk_164 = self->unk_164 - 1;

    if (self->unk_164 > 0) {
        return;
    }

    if (self->hasLabel != 0) {
        func_ov039_02096154((void*)EasyTask_GetTaskData(self->pool, *(u32*)((u8*)self + 0x230)), 2, 0);
    }

    *(s32*)((u8*)gFaders + 8) = 0x10000;

    EasyFade_FadeMainDisplay(2, 0x10, 0x1000);

    self->unk_164 = 0x34;
    self->subKind = 1;
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

/* ------------------------------------------------------------------ */
/* The 7-phase badge intro sequence, 0x0208ce88.                       */
/* ------------------------------------------------------------------ */

/**
 * @brief The badge's seven-phase intro, one state machine step per call.
 *
 * The badge block's +0xF4 flag pair and +0xFC phase counter drive it:
 *
 *   0 seeds a 0x14-frame hold and the +0x128 offset, fires
 *     `func_ov039_0208fefc` and sfx mode 0x331;
 *   1 winds +0x128 down by 0xA000 per frame and, when the hold lands, reloads
 *     the frame count from the badge's cell table at +0xC;
 *   2 is the free-move probe -- either wireless (+0xE8 set: adds the +2/+3
 *     byte pair as a Q12.12 point scaled) or local (nearest pin via
 *     `func_ov039_02087e2c`, then a 0x1E000-range test against 0x1800), and
 *     below both, steps the +0x100 counter and the +0xF0 bit-2 check;
 *   3 fires `func_ov039_0208ff30` and re-seeds from cell +0xE, mode 0x332;
 *   4 damps +0x128 to zero, fires `func_ov039_0208ff68`, then spawns 8 task
 *     sprites via `func_ov039_02097750` on a Q12.12 rotation step, mode 0x341
 *     and a 0x28-frame hold;
 *   5 drains the hold, and case 6 raises the +0xF8 "done" flag.
 *
 * Cell counts are u16 at +0x16C-indexed * 0x1C rows of the +0x170 table.
 */
// Nonmatching: 92.4%, and every row it does flag reduces to one fact: the
// target keeps `self` in r9 for the whole function and starts the case-4
// loop's constants at r6 (0xCD) / r5 (0x14) / r4 (0x2000), while this build
// puts `self` in r8 and hands the three constants r5/r4/r9. Because of that
// rename over 100 near-identical rows differ. Four things were real and are
// fixed: the +0x238 group slots must be read as a typed `groupIds[8]` (flat
// `self + 0x238 + i*4` strength-reduced into a walking pointer); the case-4
// selector's step must be spelled `i * 0x10000` inside the loop, not an
// accumulator outside -- an accumulator walking +0x10000 made mwcc split a
// parallel `step >> 2` IV and walk it by 0x4000, which the target does not
// have; case 2's point pair must be assigned x before y (mwcc stores x to the
// lower stack word first); and case 2's wireless +0xE8 pointer must be a named
// local, or mwcc loads it twice instead of once. The `aim` pair's own stack
// slots prologue/epilogue, every branch target and every store now agree.
void func_ov039_0208ce88(OtuBadge* task) {
    u8* self = (u8*)task;

    switch (*(s32*)(self + 0xFC)) {
        default:
            break;

        case 0:
            *(s32*)(self + 0x100) = 0x14;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            *(s32*)(self + 0x128) = 0;
            func_ov039_0208fefc(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)), 0x14);
            func_ov039_02087d04(0x331, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            return;

        case 1:
            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) - 0xA000;
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            *(s32*)(self + 0x100) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0xC);
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;

        case 2: {
            OtuPoint aim;
            OtuPoint dir;

            if (*(s32*)(self + 0xE8) != NULL) {
                if (*(u16*)(self + 0xF0) & 1) {
                    u8* source = *(u8**)(self + 0xE8);

                    aim.x = *(u8*)(source + 2) << 0xC;
                    aim.y = *(u8*)(source + 3) << 0xC;
                    func_ov039_02098b8c((OtuPoint*)(self + 0x118), &aim, &aim);
                    if (func_ov039_0208a988(&aim, *(OtuCellGrid**)(self + 0xE4), *(TinPinSlammer_Scene**)(self + 0)) != 0) {
                        *(s32*)(self + 0x120) = aim.x;
                        *(s32*)(self + 0x124) = aim.y;
                    }
                }
            } else {
                OtuPinTask* cand =
                    func_ov039_02087e2c(*(TaskPool**)(self + 8), *(TinPinSlammer_Scene**)(self + 0), *(s32*)(self + 0xE0));

                if (cand != NULL) {
                    func_ov039_02098bb0((OtuPoint*)((u8*)cand + 0x120), (OtuPoint*)(self + 0x120), &dir);
                    if (func_ov039_02098d10(&dir) < 0x1E000) {
                        *(s32*)(self + 0x100) = 1;
                    } else {
                        func_ov039_02098d3c(&dir, &dir);
                        func_ov039_02098c00(0x1800, &dir, (OtuPoint*)(self + 0x120), &dir);
                        if (func_ov039_0208a988(&dir, *(OtuCellGrid**)(self + 0xE4), *(TinPinSlammer_Scene**)(self + 0)) != 0)
                        {
                            *(s32*)(self + 0x120) = dir.x;
                            *(s32*)(self + 0x124) = dir.y;
                        }
                    }
                }
            }

            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0 && !(*(u16*)(self + 0xF0) & 4)) {
                return;
            }
            *(s32*)(self + 0x100) = 0xA;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;
        }

        case 3:
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            func_ov039_0208ff30(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)));
            *(s32*)(self + 0x100) = *(u16*)(*(u8**)(self + 0x170) + (u32) * (u16*)*(s32*)(self + 0x16C) * 0x1C + 0xE);
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            func_ov039_02087d04(0x332, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            return;

        case 4: {
            s32 i = 0;

            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) + -*(s32*)(self + 0x128) / *(s32*)(self + 0x100);
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            func_ov039_0208ff68(EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)));

            do {
                s32 step = i * 0x10000;

                func_ov039_02097750(EasyTask_GetTaskData(*(void**)(self + 8), task->groupIds[i]),
                                    (OtuTaskParams*)(self + 0x120), (u32)((step + ((u32)(step >> 2) >> 0x1D)) << 0xD) >> 0x10,
                                    0x2000, 0xCD, 0x14);
                i = i + 1;
            } while (i < 8);

            func_ov039_02087d04(0x341, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
            *(s32*)(self + 0x100) = 0x28;
            *(s32*)(self + 0xFC)  = *(s32*)(self + 0xFC) + 1;
            return;
        }

        case 5:
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            *(s32*)(self + 0xFC) = *(s32*)(self + 0xFC) + 1;
            return;

        case 6:
            *(s32*)(self + 0xF8) = 1;
            *(s32*)(self + 0xF4) = 0;
            return;
    }
}

/**
 * @brief Counts down a one-shot, then integrates the badge's velocity.
 *
 * While +0x100 is positive it is a one-shot timer: the countdown reaches zero
 * exactly once and fires the board commit at +0x120 against the +0x110 origin,
 * then every later call returns immediately. Once it is spent the function
 * becomes the per-frame integrator -- the same accel step
 * `func_ov039_0208af6c` takes, but pinned to `data_ov039_0209a39c[3]` rather
 * than to the tile under the badge.
 */
// Nonmatching: 97.5%, one register apart. The target keeps the `slots` pointer
// in r12 across the `data_ov039_0209a39c[3]` load and puts the rate in r1 and
// the speed in r2; this source recycles r12 for the rate and puts the speed in
// r1. Giving the speed a named local makes it worse (97.2%), so the allocation
// is not reachable from the declaration shape.
void func_ov039_0208d210(OtuBadge* task) {
    u8* self = (u8*)task;
    s32 count;
    s32 rate;
    s32 v;

    count = *(s32*)(self + 0x100);
    if (count > 0) {
        --count;
        *(s32*)(self + 0x100) = count;
        if (count != 0) {
            return;
        }

        func_ov039_02089064(*(TinPinSlammer_Scene**)(self + 0x00), (OtuPoint*)(self + 0x120));
        func_ov039_02087d04(0x34E, (OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x110));
        return;
    }

    *(s32*)(self + 0x134) = *(s32*)(self + 0x134) + data_ov039_0209a318;

    /* The cell record is reached through a u16 index whose home is itself a
     * pointer at +0x16C, and the record is 0x1C bytes at +0x170. */
    {
        u16 index     = *(u16*)*(s32*)(self + 0x16C);
        u8* slots     = *(u8**)(self + 0x170);
        u8  tuneIndex = *(u8*)(slots + index * 0x1C + 4);

        rate = data_ov039_0209a39c[3];
        rate = (s32)(((s64)data_ov039_0209a3e0[tuneIndex].speed * rate + 0x800) >> 12);
    }

    func_ov039_0208ad2c((OtuPoint*)(self + 0x12C), rate);
    func_ov039_02098b8c((OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x12C), (OtuPoint*)(self + 0x120));

    v                     = *(s32*)(self + 0x128) + *(s32*)(self + 0x134);
    *(s32*)(self + 0x128) = v;
    if (v > 0) {
        *(s32*)(self + 0x128) = 0;
        *(s32*)(self + 0xF8)  = 1;
        *(s32*)(self + 0xF4)  = 0;
    }
}

/*
 * The twenty-two instantiations. Kept as one call each rather than as macros so
 * that the per-function argument list stays readable and a diff shows which
 * constant each one actually varies.
 */

// Nonmatching: 68-78%. The body, the three varied constants and the guard chain
// are all correct; what is left is mwcc scheduling the two-step lookup.
//
// Three bugs were real and are fixed, so the remaining gap is not one of those:
//
//   * the axis argument to func_ov039_02088400 is passed and never read -- the
//     target loads it into r2 and then uses r2 only as the mask source, so the
//     last instruction is a plain shift. The packer is now a full MATCH.
//   * slot+0x04 and slot+0x08 are 32-bit words, not a u16 and a byte pointer.
//     Every store in the target is `str`, and the widths are load-bearing.
//   * the guard is three flat short-circuit tests over +0x18, +0x1C and +0x16,
//     not two nested ones. The target's `ldrne` on the table load is the
//     short-circuit, which is why +0x18 is tested at all.
/* `arg` is unused: the target loads r1 nowhere in these bodies and compares
 * only r2. Kept as a named parameter so the selector stays in the third
 * argument slot, which is where the target reads it. */
OtuSpriteSlot* func_ov039_0208d2f8(OtuSpriteTask* t, s32 arg, s32 mode) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    (void)arg;

    switch (mode) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            s32 index;
            u8* table;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            // The two-step lookup: a u16 out of the task's table at one stride,
            // then a byte pointer built from the u16 at the other. Guarded on the
            // table pointer and on the index being non-negative.
            if (t->unk_18 != 0 && (table = t->cellTable) != NULL && (index = t->index) >= 0) {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->unk_0C   = (void*)((u8*)t + 0xCC);
            slot->depthKey = func_ov039_02088400(*(s32*)((u8*)t + 0x124), 0, 3);
            return slot;
        }

        default:
            return NULL;
    }
}
