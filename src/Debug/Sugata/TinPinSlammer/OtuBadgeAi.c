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
s32 func_ov039_0208c4ec(OtuBadge* self) {
    OtuBadge* other;

    if (self->chanceTbl[5] < RNG_Next(0x10000)) {
        return 0;
    }

    if (self->bounceTimer <= 0) {
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
s32 func_ov039_0208c568(OtuBadge* self) {
    OtuBadge* other;

    if (self->chanceTbl[6] < RNG_Next(0x10000)) {
        return 0;
    }

    if (self->trackFrames <= 0) {
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
s32 func_ov039_0208c5f8(OtuBadge* self) {
    OtuBadge* other;

    if (self->chanceTbl[7] < RNG_Next(0x10000)) {
        return 0;
    }

    if (self->trackFrames <= 0) {
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
s32 func_ov039_0208c688(OtuBadge* self) {
    OtuBadge* other;

    if (self->chanceTbl[8] < RNG_Next(0x10000)) {
        return 0;
    }

    if (self->arcFrames <= 0) {
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
s32 func_ov039_0208c718(OtuBadge* self) {
    OtuBadge* other;

    if (self->chanceTbl[9] < RNG_Next(0x10000)) {
        return 0;
    }

    if (self->bounceTimer <= 0) {
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
s32 func_ov039_0208c794(OtuBadge* self) {
    OtuBadge* other;

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
s32 func_ov039_0208c84c(OtuBadge* self) {
    if (self->curAI != 0xB) {
        if (self->chanceTbl[11] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208a794(&self->pos, self->board) != 0xC) {
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

void func_ov039_0208c8ac(OtuBadge* self) {
    func_ov039_0208c128(self, 0xC, 0xC, 0xC, 1);
}

void func_ov039_0208c8cc(OtuBadge* self) {
    func_ov039_0208c128(self, 0xD, 5, 5, 1);
}

void func_ov039_0208c8ec(OtuBadge* self) {
    func_ov039_0208c128(self, 0xE, 7, 7, 1);
}

void func_ov039_0208c90c(OtuBadge* self) {
    func_ov039_0208c128(self, 0xF, 8, 0xB, 1);
}

/** @brief The third latching state, curAI 0x10 -- same shape as 0x0208c794. */
s32 func_ov039_0208c92c(OtuBadge* self) {
    OtuBadge* other;

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
    OtuBadge* self = (OtuBadge*)arg;
    s32 (**fn)(OtuBadge*);
    s32 i;

    if (self->stun > 0) {
        return;
    }

    if (self->height != 0) {
        return;
    }

    fn = (s32(**)(OtuBadge*))data_ov039_0209a4b0;

    for (i = 1; i < 0x11; i++) {
        if (fn[i](self) != 0) {
            return;
        }
    }
}

/**
 * Warps the badge: when it comes to rest on a board warp cell it jumps to the
 * warp's target cell and plays the warp sprite (subKind 0 -> 1); once that
 * finishes it drops back to phase 1 (subKind 1).
 */
void func_ov039_0208ca1c(OtuBadge* self) {
    OtuWarp* warp;
    u8       cell[2];
    s32      count;
    s32      i;
    u8       x;
    u8       y;

    switch (self->subKind) {
        case 0:
            warp = EasyTask_GetTaskData(self->pool, self->warpId);

            if (func_ov039_02097ad8(warp) != 0) {
                return;
            }

            cell[0] = (self->pos.x >> 0xC) / 32;
            cell[1] = (self->pos.y >> 0xC) / 32;

            count = self->board->warpCount;

            for (i = 0; i < self->board->warpCount; i++) {
                if (*(u16*)&self->board->warps[i] == *(u16*)cell) {
                    break;
                }
            }

            if (i >= count) {
                return;
            }

            x = self->board->warps[i].toX;
            y = self->board->warps[i].toY;

            self->pos.x     = ((x << 5) + 0x10) << 0xC;
            self->pos.y     = ((self->board->warps[i].toY << 5) + 0x10) << 0xC;
            self->unk_1A4   = 1;
            self->lastCellX = x;
            self->lastCellY = y;

            func_ov039_02097aa4(warp, &self->pos, 2);

            self->subKind = 1;
            break;

        case 1:
            if (func_ov039_02097ad8(EasyTask_GetTaskData(self->pool, self->warpId)) != 0) {
                return;
            }

            self->phase = 1;
            self->step  = 0;
            break;
    }
}

/**
 * @brief One frame of the pin: release, tick, fade out and tear down.
 */
void func_ov039_0208cb64(OtuBadge* arg) {
    OtuBadge* self = (OtuBadge*)arg;
    if (func_ov039_0208a794(&self->pos, self->board) == 0) {
        self->phase         = 1;
        self->step          = 0;
        self->affine.scaleX = 0x1000;
        self->affine.scaleY = 0x1000;
        return;
    }

    if (self->stun > 0) {
        return;
    }

    if (self->pad != NULL) {
        if ((self->pressedKeys & 0x82) == 0) {
            return;
        }

        func_ov039_0208ba70(self);
        return;
    }

    func_ov039_0208c218(self);

    self->affine.scaleX = (self->frameBudget << 11) / 30 + 0x800;
    self->affine.scaleY = self->affine.scaleX;

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae14(self);

    func_ov039_02087d04(0x33C, &self->pos, &self->origin);

    Sprite_Release(&self->spriteA);

    func_ov039_0209657c(EasyTask_GetTaskData(self->pool, self->deadId));
}

/**
 * @brief The main update: tick the frame budget, advance the tray slot, and
 *        refresh the counter and the two linked-list tasks.
 */
void func_ov039_0208cc4c(OtuBadge* arg) {
    OtuBadge* self = (OtuBadge*)arg;
    s32       i;

    self->frameBudget = self->frameBudget - 1;

    if (self->frameBudget > 0) {
        return;
    }

    func_ov039_0208ae8c(self);

    if (self->pinID[0] >= 0x130) {
        goto refresh;
    }

    self->pinID = self->pinID + 1;

    func_ov039_02093d18(EasyTask_GetTaskData(self->pool, self->counterId), self->pinID);

    if (self->hasLabel != 0) {
        if (self->pinID[0] >= 0x130) {
            goto refresh;
        }

        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->entryId), 1, self->pinID[1] == 0x130);
    }

    if (self->pinID[0] >= 0x130) {
        return;
    }

    func_ov039_02087d04(0x33F, &self->pos, &self->origin);
    return;

refresh:
    self->score = 0;

    func_ov039_02093d68(EasyTask_GetTaskData(self->pool, self->counterId), self->score);

    func_ov039_0208d5dc(self);

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc(EasyTask_GetTaskData(self->pool, self->pointIds[i]));
    }
}

/**
 * @brief The fade-out state.
 */
void func_ov039_0208cd50(OtuBadge* arg) {
    OtuBadge* self = (OtuBadge*)arg;
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
        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->entryId), 2, 0);
    }

    *(s32*)((u8*)gFaders + 8) = 0x10000;

    EasyFade_FadeMainDisplay(2, 0x10, 0x1000);

    self->unk_164 = 0x34;
    self->subKind = 1;
}

/* --- the two one-line probes -------------------------------------------- */

/** @brief If that probe task is finished, retire the pin. */
void func_ov039_0208ce20(OtuBadge* arg) {
    OtuBadge* self = (OtuBadge*)arg;
    if (func_ov039_0209167c(EasyTask_GetTaskData(self->pool, self->needleId)) != 0) {
        return;
    }

    self->phase = 1;
    self->step  = 0;
}

/** @brief The same shape, against a different task id. */
void func_ov039_0208ce54(OtuBadge* arg) {
    OtuBadge* self = (OtuBadge*)arg;
    if (func_ov039_02091014(EasyTask_GetTaskData(self->pool, self->hammerId)) != 0) {
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
void func_ov039_0208ce88(OtuBadge* self) {

    switch (self->subKind) {
        default:
            break;

        case 0:
            self->frameBudget = 0x14;
            self->subKind     = self->subKind + 1;
            self->height      = 0;
            func_ov039_0208fefc(EasyTask_GetTaskData(self->pool, self->meteoId), 0x14);
            func_ov039_02087d04(0x331, &self->pos, &self->origin);
            return;

        case 1:
            self->height      = self->height - 0xA000;
            self->frameBudget = self->frameBudget - 1;
            if (self->frameBudget > 0) {
                return;
            }
            self->frameBudget = self->slots[*self->pinID].meteoHold;
            self->subKind     = self->subKind + 1;
            return;

        case 2: {
            OtuPoint aim;
            OtuPoint dir;

            if (self->pad != NULL) {
                if (self->touchFlags & 1) {
                    OtuPadState* source = self->pad;

                    aim.x = source->x << 0xC;
                    aim.y = source->y << 0xC;
                    func_ov039_02098b8c(&self->homeOffset, &aim, &aim);
                    if (func_ov039_0208a988(&aim, self->board, self->scene) != 0) {
                        self->pos.x = aim.x;
                        self->pos.y = aim.y;
                    }
                }
            } else {
                OtuBadge* cand = func_ov039_02087e2c(self->pool, self->scene, self->index);

                if (cand != NULL) {
                    func_ov039_02098bb0(&cand->pos, &self->pos, &dir);
                    if (func_ov039_02098d10(&dir) < 0x1E000) {
                        self->frameBudget = 1;
                    } else {
                        func_ov039_02098d3c(&dir, &dir);
                        func_ov039_02098c00(0x1800, &dir, &self->pos, &dir);
                        if (func_ov039_0208a988(&dir, self->board, self->scene) != 0) {
                            self->pos.x = dir.x;
                            self->pos.y = dir.y;
                        }
                    }
                }
            }

            self->frameBudget = self->frameBudget - 1;
            if (self->frameBudget > 0 && !(self->touchFlags & 4)) {
                return;
            }
            self->frameBudget = 0xA;
            self->subKind     = self->subKind + 1;
            return;
        }

        case 3:
            self->frameBudget = self->frameBudget - 1;
            if (self->frameBudget > 0) {
                return;
            }
            func_ov039_0208ff30(EasyTask_GetTaskData(self->pool, self->meteoId));
            self->frameBudget = self->slots[*self->pinID].meteoSquash;
            self->subKind     = self->subKind + 1;
            func_ov039_02087d04(0x332, &self->pos, &self->origin);
            return;

        case 4: {
            s32 i = 0;

            self->height      = self->height + -self->height / self->frameBudget;
            self->frameBudget = self->frameBudget - 1;
            if (self->frameBudget > 0) {
                return;
            }
            func_ov039_0208ff68(EasyTask_GetTaskData(self->pool, self->meteoId));

            do {
                s32 step = i * 0x10000;

                func_ov039_02097750(EasyTask_GetTaskData(self->pool, self->smokeIds[i]), &self->pos,
                                    (u32)((step + ((u32)(step >> 2) >> 0x1D)) << 0xD) >> 0x10, 0x2000, 0xCD, 0x14);
                i = i + 1;
            } while (i < 8);

            func_ov039_02087d04(0x341, &self->pos, &self->origin);
            self->frameBudget = 0x28;
            self->subKind     = self->subKind + 1;
            return;
        }

        case 5:
            self->frameBudget = self->frameBudget - 1;
            if (self->frameBudget > 0) {
                return;
            }
            self->subKind = self->subKind + 1;
            return;

        case 6:
            self->phase = 1;
            self->step  = 0;
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
void func_ov039_0208d210(OtuBadge* self) {
    s32 count;
    s32 rate;
    s32 v;

    count = self->frameBudget;
    if (count > 0) {
        --count;
        self->frameBudget = count;
        if (count != 0) {
            return;
        }

        func_ov039_02089064(self->scene, &self->pos);
        func_ov039_02087d04(0x34E, &self->pos, &self->origin);
        return;
    }

    self->vz = self->vz + data_ov039_0209a318;

    /* The cell record is reached through a u16 index whose home is itself a
     * pointer at +0x16C, and the record is 0x1C bytes at +0x170. */
    {
        u16            index     = *self->pinID;
        OtuBadgeParam* slots     = self->slots;
        u8             tuneIndex = slots[index].tuneIndex;

        rate = data_ov039_0209a39c[3];
        rate = (s32)(((s64)data_ov039_0209a3e0[tuneIndex][0] * rate + 0x800) >> 12);
    }

    func_ov039_0208ad2c(&self->vel, rate);
    func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);

    v            = self->height + self->vz;
    self->height = v;
    if (v > 0) {
        self->height = 0;
        self->phase  = 1;
        self->step   = 0;
    }
}

/* ==================================================================== */
/* Tsk_OtosuGame_badge                                                  */
/* ==================================================================== */

SpriteFrameInfo* func_ov039_0208d2f8(Sprite* sprite, s32 arg, s32 mode) {
    OtuBadge* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}
