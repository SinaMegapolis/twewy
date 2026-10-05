/**
 * @file OtosuGame_badge.c
 * @brief The `Tsk_OtosuGame_badge` task: a badge on the board, with its
 *        movement, collisions, AI and accessors.
 *
 * The behaviour and the task are one TU in the original: the task's name
 * (0209a49c) comes before the AI's state table (0209a4b0) in .data.
 */

#include "OtuFieldAccessShared.h"

#define OTU_CELL_BITS 0x1FFFF

#define OTU_CELL_SIZE 0x20000

/*
 * Every Q12.12 multiply below is written out rather than hidden behind a macro
 * so each one can be read against the `smull`/`adds #0x800`/`adc`/`lsr #0xC`
 * quartet it produces. The rounding constant is in all of them: the target
 * rounds to nearest, and dropping it is a one-bit-per-call difference that a
 * match will not forgive.
 */

/** Rebuilds the packed state word from the home tile's flag. */
void func_ov039_0208acc0(OtuBadge* self) {
    u16 home  = (self->pad->touched & 1) != 0 ? 1 : 0;
    u16 flags = self->touchFlags;

    // Bit 1 records "the badge is not where its home tile says it is". The
    // condition is the XOR masked back down to one bit rather than a plain
    // compare, which is what produces the target's eor/and/tst trio.
    if ((flags ^ home) & home & 1) {
        flags |= 2;
    } else {
        flags &= ~2;
    }
    self->touchFlags = flags;

    // Bit 2 records the transition itself, so it runs the other way round: the
    // badge has to have been home and no longer is.
    flags = self->touchFlags;
    if ((flags ^ home) & flags & 1) {
        flags |= 4;
    } else {
        flags &= ~4;
    }
    self->touchFlags = flags;

    // Bit 0 is not a flag at all; it is the home tile's bit 0 latched in.
    self->touchFlags &= ~1;
    self->touchFlags |= home;
}

/**
 * Scales a velocity down to at most `shortfall`, never up.
 *
 * The magnitude is taken first, so the vector is left normalised on return,
 * and a zero-length vector is left alone rather than normalised into the
 * direction of nothing.
 */
void func_ov039_0208ad2c(OtuPoint* vel, s32 shortfall) {
    OtuPoint norm;
    OtuPoint zero;
    s32      len = func_ov039_02098d10(vel);
    s32      scale;

    if (len <= 0) {
        return;
    }

    scale  = len - shortfall;
    zero.x = 0;
    zero.y = 0;

    // `shortfall` is an allowance, not a demand: a vector already shorter than
    // it is left alone rather than stretched.
    if (scale < 0) {
        scale = 0;
    }

    func_ov039_02098d3c(vel, &norm);
    func_ov039_02098c00(scale, &norm, &zero, vel);
}

/** Enters phase 2: pick the badge's sprite up and send it back to the home tile. */
void func_ov039_0208ad88(OtuBadge* self) {
    if (self->phase == 2) {
        return;
    }

    func_ov039_02097aa4(EasyTask_GetTaskData(self->pool, self->warpId), (void*)&self->pos, 1);
    func_ov039_02087d04(0x343, &self->pos, &self->origin);

    self->vel.x   = 0;
    self->vel.y   = 0;
    self->step    = 0;
    self->phase   = 2;
    self->subKind = 0;
}

/** Enters phase 3: sit still for 0x1E frames with the badge visible. */
void func_ov039_0208ade8(OtuBadge* self) {
    if (self->phase == 3) {
        return;
    }

    self->step        = 0;
    self->phase       = 3;
    self->frameBudget = 0x1E;
    self->stun        = 0;
}

/** Enters phase 4: retire whatever the badge was attached to. */
void func_ov039_0208ae14(OtuBadge* self) {
    if (self->phase == 4) {
        return;
    }

    // A badge still carrying a sound handle hands it back with a different cue
    // depending on whether it had a live rival to chase.
    if (self->partner != NULL && *self->pinID < 0x130) {
        func_ov039_0208a490(self, self->stun > 0 ? 5 : 2);
    }
    self->partner = NULL;

    func_ov039_02087dc0(self->scene, self);

    self->stun        = 0;
    self->step        = 0;
    self->phase       = 4;
    self->frameBudget = 0x3C;
}

/** Enters phase 5: place the badge on its starting tile and start it rolling. */
void func_ov039_0208ae8c(OtuBadge* self) {
    if (self->hasLabel != 0) {
        EasyFade_FadeMainDisplay(2, 0x10, 0x1000);
    }

    self->travel  = 0;
    self->velMag  = 0;
    self->unk_164 = 0x50;
    self->step    = 0;
    self->phase   = 5;
    self->subKind = 0;

    // Tiles are 0x20 pixels and every badge is inset by half a tile, which is
    // what lands it in the middle of its cell rather than on a corner. The
    // starting tile comes from the board's two-bytes-per-badge table at +8.
    self->startPos.x = ((self->board->start[self->index][0] << 5) + 0x10) << 0xC;
    self->startPos.y = ((self->board->start[self->index][1] << 5) + 0x10) << 0xC;
    self->pos.x      = self->startPos.x;
    self->pos.y      = self->startPos.y;
    self->vel.x      = 0;
    self->vel.y      = 0;

    self->dir.x   = 0x1000;
    self->dir.y   = 0;
    self->stun    = 0;
    self->unk_1A4 = 1;
}

/** Enters phase 7's approach: kill the velocity and play the corner sound. */
void func_ov039_0208af38(OtuBadge* self) {
    self->vz   = data_ov039_0209a31c;
    self->step = 0;

    func_ov039_02087d04(0x344, &self->pos, &self->origin);
}

/**
 * The rolling phase: accelerate, turn, clamp to the board, bounce off walls
 * and wall corners, drop trail marks, and finally react to the tile the badge
 * has just arrived on.
 */
void func_ov039_0208af6c(OtuBadge* self) {
    OtuPoint       probe;
    OtuPoint       norm;
    OtuPoint       zero;
    OtuBadgeParam* slot;
    const s32*     accel;
    s32            tile;
    s32            cellX;
    s32            cellY;
    s32            len;
    s32            hit;

    self->vz = self->vz + data_ov039_0209a318;

    // Acceleration, unless something has already committed the badge to a
    // mode of its own.
    if (self->mode <= 0) {
        s32 rate;

        if (self->height == 0) {
            tile = func_ov039_0208a794(&self->pos, self->board);

            switch (tile) {
                case 3:
                    rate = data_ov039_0209a39c[2];
                    break;

                case 4:
                    rate = data_ov039_0209a39c[1];
                    break;

                default:
                    rate = data_ov039_0209a39c[0];
                    break;
            }
        } else {
            rate = data_ov039_0209a39c[3];
        }

        slot  = &self->slots[self->pinID[0]];
        accel = data_ov039_0209a3e0[slot->tuneIndex];

        rate = (s32)(((s64)*accel * rate + 0x800) >> 12);

        // While the badge is still being set up its rate is doubled. The test
        // is `phase <= 3`, i.e. every phase before the rolling one.
        if (self->phase <= 3) {
            rate = rate * 2;
        }

        func_ov039_0208ad2c(&self->vel, rate);
    }

    // Turning, but only while nothing external is steering the badge.
    if (self->height == 0) {
        s32 turn;
        s16 dir;
        s32 cell;

        len  = func_ov039_02098d10(&self->vel);
        slot = &self->slots[self->pinID[0]];

        // How hard the badge curves is the friction coefficient scaled by the
        // wobble 0x0208be30 left behind.
        turn = (s32)(((s64)slot->friction * (self->velMag / 3) + 0x800) >> 12);

        dir  = FX_Atan2Idx(self->vel.y, self->vel.x) + turn;
        cell = dir >> 4;

        /* The sin/cos table is s32[] by band 3's declaration but holds pairs of
         * s16, so each element has to be fetched through a s16* at a doubled
         * byte offset -- the same shape bands 5 and 7 use. */
        self->vel.x = (s32)(((s64)len * ((s16*)data_0205e4e0)[cell * 2 + 1] + 0x800) >> 12);
        self->vel.y = (s32)(((s64)len * ((s16*)data_0205e4e0)[cell * 2] + 0x800) >> 12);
    }

    func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);

    // The board is a fixed playfield, so both axes are clamped to a hard inset
    // rather than to whatever extent the tile lookup would accept.
    if (self->pos.x < 0x80000) {
        self->pos.x = 0x80000;
    } else {
        s32 limit = ((s32)self->board->width << 5) - 0x80;

        if (self->pos.x >= (limit << 0xC)) {
            self->pos.x = (limit << 0xC) - 1;
        }
    }
    if (self->pos.y < 0x60000) {
        self->pos.y = 0x60000;
    } else {
        s32 limit = ((s32)self->board->height << 5) - 0x60;

        if (self->pos.y >= (limit << 0xC)) {
            self->pos.y = (limit << 0xC) - 1;
        }
    }

    // A negative accumulator is an impulse off something solid. Once it has
    // been absorbed both accumulators are cleared together.
    self->height = self->height + self->vz;
    if (self->height > 0) {
        self->vz     = 0;
        self->height = 0;
    }

    // Trail marks, dropped every few frames while the badge is moving fast.
    if (self->height == 0 && self->phase != 3) {
        s32 trailLen = func_ov039_02098d10(&self->vel);

        if (trailLen > 0x1000) {
            s32 angle;

            self->trailTimer = self->trailTimer - 1;
            if (self->trailTimer <= 0) {
                angle = FX_Atan2Idx(self->vel.y, self->vel.x);

                // Two halves, so the mark is centred on the badge rather than
                // trailing off one side of it.
                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trackIds[self->trailIndex]), &self->pos,
                                    angle, 1, trailLen);
                self->trailIndex = self->trailIndex + 1;

                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trackIds[self->trailIndex]), &self->pos,
                                    angle, 0, trailLen);
                self->trailIndex = self->trailIndex + 1;

                if (self->trailIndex >= 0xC) {
                    self->trailIndex = 0;
                }
                self->trailTimer = 5;
            }
        }
    }

    // Walls. A wall is tile type 2; the badge is folded back onto the boundary
    // it crossed rather than stopped at the probe point. An axis that has
    // already reflected suppresses all four corner tests below.
    if (self->height == 0) {
        hit = 0;

        if (self->vel.x > 0) {
            func_ov039_02098b78(&self->pos, &probe);
            probe.x = probe.x + 0xC000;
            tile    = func_ov039_0208a794(&probe, self->board);
            if (tile == 2) {
                self->pos.x = (probe.x & ~OTU_CELL_BITS) - 0xC000;
                self->vel.x = self->vel.x * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.x < 0) {
            func_ov039_02098b78(&self->pos, &probe);
            probe.x = probe.x - 0xC000;
            tile    = func_ov039_0208a794(&probe, self->board);
            if (tile == 2) {
                self->pos.x = (probe.x | OTU_CELL_BITS) + 0xC000;
                self->vel.x = -self->vel.x;
                hit         = 1;
            }
        }

        if (self->vel.y > 0) {
            func_ov039_02098b78(&self->pos, &probe);
            probe.y = probe.y + 0xC000;
            tile    = func_ov039_0208a794(&probe, self->board);
            if (tile == 2) {
                self->pos.y = (probe.y & ~OTU_CELL_BITS) - 0xC000;
                self->vel.y = self->vel.y * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.y < 0) {
            func_ov039_02098b78(&self->pos, &probe);
            probe.y = probe.y - 0xC000;
            tile    = func_ov039_0208a794(&probe, self->board);
            if (tile == 2) {
                self->pos.y = (probe.y | OTU_CELL_BITS) + 0xC000;
                self->vel.y = -self->vel.y;
                hit         = 1;
            }
        }

        // Corners. The probe is pushed a whole cell along each velocity
        // component, snapped to the cell boundary, and only then measured: a
        // corner the badge could follow round is not worth reversing for, so
        // it has to be closer than the reversal's own distance.
        if (hit == 0) {
            if (self->vel.x > 0 || self->vel.y < 0) {
                func_ov039_02098b78(&self->pos, &probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x > 0 || self->vel.y > 0) {
                func_ov039_02098b78(&self->pos, &probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, self->board);
                if (tile == 2) {
                    probe.x = probe.x & ~OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x - 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y < 0) {
                func_ov039_02098b78(&self->pos, &probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y | OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y + 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }

            if (self->vel.x < 0 || self->vel.y > 0) {
                func_ov039_02098b78(&self->pos, &probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, self->board);
                if (tile == 2) {
                    probe.x = probe.x | OTU_CELL_BITS;
                    probe.y = probe.y & ~OTU_CELL_BITS;
                    len     = func_ov039_02098ca8(&self->pos, &probe);
                    if (len < 0xC000) {
                        self->pos.x = probe.x + 0xC000;
                        hit         = 1;
                        self->pos.y = probe.y - 0xC000;
                        self->vel.x = -self->vel.x;
                        self->vel.y = -self->vel.y;
                    }
                }
            }
        }

        // Having hit something, re-normalise so the next turn is taken from the
        // reflected heading rather than the incoming one.
        if (hit != 0) {
            zero.x = 0;
            zero.y = 0;

            len = func_ov039_02098d10(&self->vel);
            if (len > 0) {
                func_ov039_02098d3c(&self->vel, &norm);
                func_ov039_02098c00((s32)(((s64)len * data_ov039_0209a324 + 0x800) >> 12), &norm, &zero, &self->vel);
                self->dir = norm;
            }
        }
    }

    // `unk_128` is the "something else owns this badge" latch; while it is set
    // the tile under the badge is not read at all.
    if (self->height != 0) {
        return;
    }

    tile = func_ov039_0208a794(&self->pos, self->board);

    if (tile == 0) {
        func_ov039_0208ade8(self);
    } else if (tile == 12) {
        self->visible = 0;
    }

    // Tile changes are detected by the cell the badge is standing in rather
    // than by its position, so a badge drifting inside one tile does not
    // re-trigger anything.
    cellX = (self->pos.x >> 0xC) / 32;
    cellY = (self->pos.y >> 0xC) / 32;
    if (self->lastCellX == (cellX >> 5) && self->lastCellY == (cellY >> 5)) {
        return;
    }

    switch (tile) {
        case 5:
            // Only the board that admits it has a home to go back to.
            if (self->board->warpCount != 0) {
                func_ov039_0208ad88(self);
            }
            break;

        case 7:
            func_ov039_0208af38(self);
            break;

        case 8:
            self->vel.x = 0;
            self->vel.y = -data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->dir);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 9:
            self->vel.x = data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->dir);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 10:
            self->vel.x = 0;
            self->vel.y = data_ov039_0209a308;
            func_ov039_02098d3c(&self->vel, &self->dir);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;

        case 11:
            self->vel.x = -data_ov039_0209a308;
            self->vel.y = 0;
            func_ov039_02098d3c(&self->vel, &self->dir);
            func_ov039_02087d04(0x345, &self->pos, &self->origin);
            break;
    }

    self->lastTileType = tile;
    self->lastCellX    = cellX >> 5;
    self->lastCellY    = cellY >> 5;
}

/** Enters phase 6: hand the badge to the track task for `trackFrames`. */
void func_ov039_0208b94c(OtuBadge* self) {
    OtuBadgeParam* slot;

    if (self->trackFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 6;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091654(EasyTask_GetTaskData(self->pool, self->needleId), slot->needleCharge, slot->needleHold);

    self->trackFrames = self->trackFrames - 1;
}

/** Enters phase 7: hand the badge to the arc task for `arcFrames`. */
void func_ov039_0208b9b4(OtuBadge* self) {
    OtuBadgeParam* slot;

    if (self->arcFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 7;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091028(EasyTask_GetTaskData(self->pool, self->hammerId), slot->hammerRate, slot->hammerLength,
                        slot->hammerFrames, slot->hammerArc);

    self->arcFrames = self->arcFrames - 1;
}

/** Enters phase 8: stop dead for `bounceFrames`. */
void func_ov039_0208ba34(OtuBadge* self) {
    if (self->bounceTimer <= 0) {
        return;
    }

    self->vel.x   = 0;
    self->vel.y   = 0;
    self->phase   = 8;
    self->step    = 0;
    self->subKind = 0;

    self->bounceTimer = self->bounceTimer - 1;
}

/** Enters phase 9: spin down to a stop over `spinFrames`. */
void func_ov039_0208ba70(OtuBadge* self) {
    OtuPoint zero;

    if (self->spinFrames <= 0) {
        return;
    }

    zero.x = 0;
    zero.y = 0;

    func_ov039_02098c00(data_ov039_0209a38c, &self->dir, &zero, &self->vel);
    func_ov039_02098d3c(&self->vel, &self->dir);

    self->vz          = data_ov039_0209a320;
    self->step        = 0;
    self->phase       = 9;
    self->step        = 0;
    self->frameBudget = 9;

    func_ov039_02091b34(EasyTask_GetTaskData(self->pool, self->handId));

    if (self->stun > 0) {
        func_ov039_0208a490(self, 1);
    }

    self->spinFrames = self->spinFrames - 1;
}

/** The per-frame decision: which set-piece phase, if any, runs this frame. */
void func_ov039_0208bb2c(OtuBadge* self) {
    OtuPoint fromHome;
    OtuPoint fromPos;
    OtuPoint mid;
    OtuPoint legA;
    OtuPoint legB;

    // A badge that still has a rival attached is finishing an interaction. The
    // two-frame decrement only happens once the interaction is flagged as over,
    // so the frames are counted from the end rather than from the start.
    if (self->stun > 0) {
        if (self->touchFlags & 2) {
            self->stun = self->stun - 2;
            if (self->stun < 0) {
                self->stun = 0;
            }
        }
        self->step = 0;
        return;
    }

    if (self->height != 0) {
        self->step = 0;
        return;
    }

    // The set-piece phases, in the order they are allowed to claim the frame.
    if (self->pressedKeys & 0x11) {
        self->step = 0;
        func_ov039_0208b94c(self);
        return;
    }
    if (self->pressedKeys & 0x820) {
        self->step = 0;
        func_ov039_0208b9b4(self);
        return;
    }
    if (self->pressedKeys & 0x440) {
        self->step = 0;
        func_ov039_0208ba34(self);
        return;
    }

    if (self->step == 0) {
        // Wait for the home tile to report itself before aiming anywhere.
        if (!(self->touchFlags & 2)) {
            return;
        }

        self->aimFrames = 0;
        self->step      = 1;

        self->aimStart.x = self->pad->x << 0xC;
        self->aimStart.y = self->pad->y << 0xC;
        self->aimCur.x   = self->aimStart.x;
        self->aimCur.y   = self->aimStart.y;

    } else if (self->step == 1) {
        s32 turn;
        s32 scale;
        s32 reach;
        s32 sign;

        if (self->aimFrames < 0x1E) {
            self->aimFrames = self->aimFrames + 1;
        }

        // The aim point tracks the home tile, so until the badge has somewhere
        // else to be it is still following it.
        self->aimCur.x = self->pad->x << 0xC;
        self->aimCur.y = self->pad->y << 0xC;

        if (!(self->touchFlags & 4)) {
            return;
        }

        self->step = 0;

        turn = func_ov039_02098ca8(&self->aimStart, &self->aimCur);
        if (turn <= 0) {
            return;
        }

        // Join the two aim points by an offset from where the badge is now.
        func_ov039_02098b8c(&self->aimStart, &self->homeOffset, &fromHome);
        func_ov039_02098b8c(&self->aimCur, &self->homeOffset, &fromPos);

        reach = func_ov039_0208a624(&fromHome, &fromPos, &self->pos);
        if (reach == 0) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &mid);

        turn = func_ov039_02098d10(&mid);
        if (turn > 0x50000) {
            turn = 0x50000;
        }
        scale = FX_Divide(turn, 0x50000);

        // Having aimed for a full half second the badge commits: the mode flag
        // goes up and the turn is sharpened from here on.
        if (self->aimFrames >= 0x1E) {
            scale      = (s32)(((s64)scale * 0x1800 + 0x800) >> 12);
            self->mode = 0x10;
        }

        func_ov039_02098d3c(&mid, &mid);
        func_ov039_0208a6f8(self, &mid, scale);

        if (self->aimFrames >= 0x1E) {
            return;
        }

        func_ov039_02098bb0(&fromPos, &fromHome, &legB);
        func_ov039_02098bb0(&self->pos, &fromHome, &legA);

        reach = func_ov039_0208a530(&fromHome, &fromPos, &self->pos);
        if (reach > 0x24000) {
            reach = 0x24000;
        }
        reach = FX_Divide(reach, 0x24000);

        if (*self->pinID >= 0x130) {
            return;
        }

        // Which way round the turn is decides the sign, and with it whether
        // the badge ends up winding tight or winding loose.
        sign = (func_ov039_02098c70(&legB, &legA) >= 0) ? data_ov039_0209a394 : -data_ov039_0209a394;

        self->velMag = (s32)(((s64)(sign * scale) * reach + 0x800) >> 12) * 3;
    }
}

/** Steers the badge along `dir` at full speed, jittering its wobble. */
void func_ov039_0208be30(OtuBadge* self, OtuPoint* dir) {
    OtuPoint step;

    func_ov039_02098bb0(dir, &self->pos, &step);

    // A zero vector has no direction to normalise, and normalising it would
    // hand back whatever the divide unit happens to leave in the register.
    if (step.x != 0 || step.y != 0) {
        func_ov039_02098d3c(&step, &step);
    } else {
        step.x = 0x1000;
        step.y = 0;
    }

    func_ov039_0208a6f8(self, &step, 0x1000);

    if (*self->pinID >= 0x130) {
        return;
    }

    // The wobble is a full-width random offset, tripled to match the three
    // the caller divides it back down by.
    self->velMag = (s32)((RNG_Next(data_ov039_0209a394 * 2) - data_ov039_0209a394) << 0xC) * 3;
}

/**
 * Walks the board outwards from the badge in a square spiral, looking for the
 * first cell whose tile type falls in [loType, hiType]. True when one is
 * found, in which case `out` receives that cell's point.
 */
s32 func_ov039_0208bed8(OtuBadge* self, s32 rings, s32 loType, s32 hiType, OtuPoint* out) {
    OtuPoint at;
    s32      ring;
    s32      span = 2;
    s32      i;
    s32      tile;

    at.x = self->pos.x - OTU_CELL_SIZE;
    at.y = self->pos.y - OTU_CELL_SIZE;

    for (ring = 1; ring <= rings; ring++) {
        // Four straight walks round the ring: right, down, left, up. The span
        // grows by two per ring, which is what makes this a spiral rather than
        // a sweep. The ring is entered one cell down-left of where the badge
        // is, so the search never revisits the cell it started from.
        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x - OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y - OTU_CELL_SIZE;
        }

        at.x = at.x - OTU_CELL_SIZE;
        at.y = at.y - OTU_CELL_SIZE;
        span = span + 2;
    }

    return 0;
}

/** True when two points are in the same board cell. */
s32 func_ov039_0208c0e4(OtuPoint* a, OtuPoint* b) {
    if ((a->x & ~OTU_CELL_BITS) != (b->x & ~OTU_CELL_BITS)) {
        return 0;
    }
    if ((a->y & ~OTU_CELL_BITS) == (b->y & ~OTU_CELL_BITS)) {
        return 1;
    }
    return 0;
}

/** The "wander to an interesting tile" AI, entry `which` of the chance table. */
s32 func_ov039_0208c128(OtuBadge* self, s32 which, s32 loType, s32 hiType, s32 kind) {
    OtuPoint found;

    // Re-rolling only happens once the badge is running a different AI than
    // last frame. While it is still on the old one the previous decision is
    // simply carried forward.
    //
    // The two `return 1;` paths are an if/else that falls through to one return,
    // not an early return inside the first branch: the target branches to a
    // shared epilogue, and an inline `return 1;` there makes mwcc emit the
    // epilogue twice instead. (objdiff 100%.)
    if (self->curAI != which) {
        if (self->chanceTbl[which] < RNG_Next(0x10000)) {
            return 0;
        }

        if (func_ov039_0208bed8(self, 3, loType, hiType, &found) == 0) {
            return 0;
        }

        // Landing back on the anchor is not a move, so it is rejected.
        if (func_ov039_0208c0e4(&found, &self->anchorPt) != 0) {
            return 0;
        }

        func_ov039_0208be30(self, &found);

        self->curAI    = which;
        self->unk_1B8  = kind;
        self->anchorPt = found;
    } else {
        if (func_ov039_0208c0e4(&self->pos, &self->anchorPt) != 0) {
            self->curAI = 0x11;
        } else {
            func_ov039_0208be30(self, &self->anchorPt);
        }
    }

    return 1;
}

/** The "spin down in place" AI. */
s32 func_ov039_0208c218(OtuBadge* self) {
    // Once the badge is committed to something it stops re-rolling, so the
    // chance test is only worth making while it is still free.
    if (self->curAI != 0 && self->chanceTbl[0] < RNG_Next(0x10000)) {
        return 0;
    }

    func_ov039_0208ba70(self);
    return 1;
}

/** The "keep rolling forward until something opens up" AI. */
s32 func_ov039_0208c258(OtuBadge* self) {
    OtuPoint at;
    s32      tries = 0;
    s32      tile;

    if (self->chanceTbl[1] < RNG_Next(0x10000)) {
        return 0;
    }

    at.x = self->pos.x;
    at.y = self->pos.y;

    // Look four cells ahead for open road. Giving up after four is the point
    // of the loop: a boxed-in badge should stop rather than commit.
    do {
        func_ov039_02098b8c(&at, &self->vel, &at);
        tile = func_ov039_0208a794(&at, self->board);
        if (tile == 0) {
            break;
        }
        tries++;
    } while (tries < 4);

    if (tries >= 4) {
        return 0;
    }

    func_ov039_02098bb0(&self->pos, &self->vel, &at);
    func_ov039_0208be30(self, &at);
    return 1;
}

/** The "chase the nearest rival badge" AI. */
s32 func_ov039_0208c304(OtuBadge* self) {
    OtuBadge* target;
    s32       alive;
    s32       gap;

    if (self->curAI != 2) {
        if (self->chanceTbl[2] < RNG_Next(0x10000)) {
            return 0;
        }

        target = func_ov039_02087f4c(self->pool, self->scene, self->index);
        if (target == NULL) {
            return 0;
        }

        gap = func_ov039_02098ca8(&self->pos, &target->pos);
        if (gap > 0xC8000) {
            return 0;
        }

        func_ov039_0208be30(self, &target->pos);

        self->curAI       = 2;
        self->unk_1B8     = 1;
        self->chaseTarget = target;
        return 1;
    }

    // Already chasing: keep going while the target is still worth chasing.
    target = self->chaseTarget;
    alive  = func_ov039_0208efb0(target, 0x444);
    if (alive != 0) {
        func_ov039_0208be30(self, &target->pos);
    }
    return 1;
}

/** The "run away from the nearest rival badge" AI. */
s32 func_ov039_0208c3bc(OtuBadge* self) {
    OtuBadge* target;
    OtuPoint  away;
    s32       gap;

    if (self->chanceTbl[3] < RNG_Next(0x10000)) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, &target->pos);
    if (gap > 0x46000) {
        return 0;
    }

    // Two subtractions, which net out to `target - (self - target)`: running
    // away is the chase reflected through the target.
    func_ov039_02098bb0(&target->pos, &self->pos, &away);
    func_ov039_02098bb0(&self->pos, &away, &away);

    func_ov039_0208be30(self, &away);
    return 1;
}

/** The "take the scripted arc if there is one left" AI. */
s32 func_ov039_0208c45c(OtuBadge* self) {
    OtuBadge* target;
    s32       speed;
    s32       gap;

    if (self->chanceTbl[4] < RNG_Next(0x10000)) {
        return 0;
    }

    // An arc needs an arc task with frames left on it, and is not worth
    // starting for a badge already moving too fast to join it.
    if (self->arcFrames <= 0) {
        return 0;
    }
    speed = func_ov039_02098d10(&self->vel);
    if (speed >= 0x3000) {
        return 0;
    }

    target = func_ov039_02088064(self->pool, self->scene, self->index);
    if (target == NULL) {
        return 0;
    }

    gap = func_ov039_02098ca8(&self->pos, &target->pos);
    if (gap > 0x46000) {
        return 0;
    }

    func_ov039_0208b9b4(self);
    return 1;
}

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

/* The 7-phase badge intro sequence, 0x0208ce88. */

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

SpriteFrameInfo* func_ov039_0208d2f8(Sprite* sprite, s32 arg, s32 mode) {
    OtuBadge* owner = sprite->owner;

    Sprite_FrameInfoCallbackAffineSorted(sprite, mode, &owner->affine, func_ov039_02088400(3, owner->pos.y, owner->height));
}

/* The three sprite loaders. */

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
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;
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
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;

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
    anim.posX     = self->pos.x >> 12;
    anim.posY     = self->pos.y >> 12;

    _Sprite_Load(sprite, &anim);
}

/* The task's state reset and its four stages. */

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
    self->step            = 0;
    self->lastTileType    = 1;
    self->lastCellX       = 0;
    self->lastCellY       = 0;
    self->height          = 0;
    self->vel.x           = 0;
    self->vel.y           = 0;
    self->dir.x           = 0x1000;
    self->dir.y           = 0;
    self->vz              = 0;
    self->travel          = 0;
    self->velMag          = 0;
    self->affine.rotation = 0;
    self->affine.scaleX   = 0x1000;
    self->affine.scaleY   = 0x1000;
    self->affine.unk_0C   = 0;
    self->affine.unk_0E   = 0;
    self->trailTimer      = 5;
    self->stun            = 0;
    self->aimFrames       = 0;
    self->mode            = 0;
    self->flags           = 0xA2;
    self->partner         = NULL;
    self->curAI           = 0x11;
    self->unk_1B8         = 0;
    self->chaseTarget     = NULL;
    self->anchorPt.x      = 0;
    self->anchorPt.y      = 0;

    // As in 0x0208e130: the row is re-derived for each byte rather than
    // hoisted, because the target reloads pinID and rowTable every time.
    self->trackFrames = self->slots[*self->pinID].tile[0];
    self->bounceTimer = self->slots[*self->pinID].tile[1];
    self->arcFrames   = self->slots[*self->pinID].tile[2];
    self->spinFrames  = self->slots[*self->pinID].tile[3];

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
    u8        startX;
    u8        startY;

    self->dataType    = args->dataType;
    self->pool        = pool;
    self->index       = args->unk_04;
    self->pad         = args->pad;
    self->board       = args->board;
    self->scene       = args->unk_10;
    self->visible     = 1;
    self->trailIndex  = 0;
    self->pointCursor = 0;
    self->hasLabel    = args->unk_1C;
    self->score       = 0;
    self->smokeCursor = 0;
    self->slots       = (OtuBadgeParam*)args->rowTable;
    self->pinID       = args->pinID;
    self->chanceTbl   = args->unk_20;

    self->lastKeys = (self->pad != NULL) ? self->pad->sysControl : 0;

    self->pressedKeys  = 0;
    self->touchFlags   = 0;
    self->origin.x     = 0;
    self->origin.y     = 0;
    self->homeOffset.x = 0;
    self->homeOffset.y = 0;

    startX      = self->board->start[self->index][0];
    startY      = self->board->start[self->index][1];
    self->pos.x = ((startX << 5) + 0x10) << 12;
    self->pos.y = ((startY << 5) + 0x10) << 12;

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

    if (self->pad != NULL) {
        func_ov039_0208ac98(self);
        func_ov039_0208acc0(self);
    }

    self->visible       = 1;
    self->affine.scaleX = 0x1000;
    self->affine.scaleY = 0x1000;

    if (self->mode > 0) {
        self->mode--;
    }
    if (self->stun > 0) {
        self->stun--;
    }
    if (self->flags > 0) {
        self->flags--;
    }

    switch (self->phase) {
        case 1:
        case 3:
        case 6:
        case 7:
            func_ov039_0208af6c(self);
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
    self->affine.rotation = (u32)self->travel * 0x10 / 0x10000;

    if (self->unk_1B8 > 0) {
        self->unk_1B8 = self->unk_1B8 - 1;
        if (self->unk_1B8 <= 0) {
            self->curAI = 0x11;
        }
    }

    switch (self->phase) {
        case 1:
            if (self->pad != NULL) {
                func_ov039_0208bb2c(self);
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
    if (self->mode > 0) {
        self->spriteC.posX = (self->pos.x - self->origin.x) >> 12;
        self->spriteC.posY = ((self->pos.y + self->height) - self->origin.y) >> 12;

        Sprite_RenderFrame(&self->spriteC);
    }

    self->spriteA.posX = (self->pos.x - self->origin.x) >> 12;
    self->spriteA.posY = ((self->pos.y + self->height) - self->origin.y) >> 12;

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

    if (self->visible == 0) {
        if (self->hasLabel != 0) {
            self->spriteB.posX = (self->pos.x - self->origin.x) >> 12;
            self->spriteB.posY = ((self->pos.y + self->height) - self->origin.y) >> 12;

            Sprite_RenderFrame(&self->spriteB);
        }

        return 1;
    }

    if (self->flags & 2) {
        return 1;
    }

    switch (self->phase) {
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

    EasyTask_DeleteTask(pool, self->warpId);

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->smokeIds[i]);
    }

    EasyTask_DeleteTask(pool, self->deadId);

    if (self->hasLabel != 0) {
        EasyTask_DeleteTask(pool, self->entryId);
    }

    for (i = 0; i < 2; i++) {
        EasyTask_DeleteTask(pool, self->pointIds[i]);
    }

    for (i = 0; i < 12; i++) {
        EasyTask_DeleteTask(pool, self->trackIds[i]);
    }

    EasyTask_DeleteTask(pool, self->counterId);
    EasyTask_DeleteTask(pool, self->radarId);
    EasyTask_DeleteTask(pool, self->handId);
    EasyTask_DeleteTask(pool, self->needleId);
    EasyTask_DeleteTask(pool, self->hammerId);
    EasyTask_DeleteTask(pool, self->meteoId);
    EasyTask_DeleteTask(pool, self->markerId);
    EasyTask_DeleteTask(pool, self->piyoId);
    EasyTask_DeleteTask(pool, self->shadowId);

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

/* The spawner. */

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

    args.dataType = arg1;
    args.unk_04   = arg2;
    args.pad      = arg3;
    args.board    = arg4;
    args.unk_10   = arg5;
    args.rowTable = (u8*)arg6;
    args.pinID    = (u16*)arg7;
    args.unk_1C   = arg8;
    args.unk_20   = arg9;

    id   = EasyTask_CreateTask(pool, &data_ov039_0209926c, NULL, 0, NULL, &args);
    self = (OtuBadge*)EasyTask_GetTaskData(pool, id);

    self->shadowId = func_ov039_0208f40c(pool, arg1, id);
    self->piyoId   = func_ov039_0208f770(pool, arg1, id);
    self->markerId = func_ov039_0208fa5c(pool, arg1, id);
    self->meteoId  = func_ov039_0208fe60(pool, arg1, id);
    self->hammerId = func_ov039_02090e1c(pool, arg1, id);
    self->needleId = func_ov039_020915a8(pool, arg1, id);
    self->handId   = func_ov039_02091b00(pool, arg1, id);

    // The radar and the counter each take two extra words, built in the
    // outgoing-argument area rather than in a frame of their own.
    self->radarId   = func_ov039_0209383c(pool, arg1, id, arg2, arg4, arg8);
    self->counterId = func_ov039_02093cd8(pool, arg1, id, arg2, arg8);
    func_ov039_02093d18(EasyTask_GetTaskData(pool, self->counterId), (u16*)arg7);

    for (i = 0; i < 12; i++) {
        self->trackIds[i] = func_ov039_02095750(pool, arg1, id);
    }

    for (i = 0; i < 2; i++) {
        self->pointIds[i] = func_ov039_02095ca0(pool, arg1, id);
    }

    if (self->hasLabel != 0) {
        self->entryId = func_ov039_02096124(pool, arg1);
    }

    self->deadId = func_ov039_02096548(pool, arg1, id);

    for (i = 0; i < 8; i++) {
        self->smokeIds[i] = func_ov039_0209771c(pool, arg1, id);
    }

    self->warpId = func_ov039_02097a70(pool, arg1, id);

    if (self->hasLabel != 0 && *self->pinID < 0x130) {
        // The label rides in the *second* halfword of the tray slot, and the
        // `moveq`/`movne` pair shows it is the second halfword that is tested.
        func_ov039_02096154(EasyTask_GetTaskData(self->pool, self->entryId), 1, self->pinID[1] == 0x130 ? 1 : 0);
    }

    func_ov039_0208ae8c(self);

    return id;
}

/* The pair-versus-pair collision code. */

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
    s32 mag = OtuQ12Mul(speed, scaleA);

    func_ov039_02098c00(OtuQ12Mul(mag, scaleB), dir, &other->vel, &other->vel);
    func_ov039_0208a6c4(&other->vel);

    if (&other->vel != NULL && &other->vel.y != NULL) {
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

    *score = OtuQ12Mul(*score, dot);
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

    func_ov039_0208e058(&self->pos, &self->vel, &other->pos, &other->vel, &dir, &score);

    // The row pointer is not hoisted into a local: the target re-reads both
    // `pinID` and `rowTable` from the task on each of the four lookups, so a
    // cached OtuBadgeParam* is a source-level difference, not a scheduling one.
    scale = (self->stun > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, OtuQ12Mul(score, scale), data_ov039_0209a3e4[self->slots[*self->pinID].tuneIndex][0],
                        (other->mode > 0) ? data_ov039_0209a3e8[other->slots[*other->pinID].tuneIndex][0] * 2
                                          : data_ov039_0209a3e8[other->slots[*other->pinID].tuneIndex][0],
                        self);
    self->partner = other;

    scale = (other->stun > 0) ? data_ov039_0209a388 : 0x1000;
    func_ov039_0208dff0(&dir, -OtuQ12Mul(score, scale), data_ov039_0209a3e4[other->slots[*other->pinID].tuneIndex][0],
                        (self->mode > 0) ? data_ov039_0209a3e8[self->slots[*self->pinID].tuneIndex][0] * 2
                                         : data_ov039_0209a3e8[self->slots[*self->pinID].tuneIndex][0],
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
/* These three pairwise predicates were declared `void*` for as long as the badge had
 * three separate views: band 9's declaration governed this translation unit, and
 * giving the definitions real struct types collided with it and cascaded into
 * "expression syntax error" through the whole body. With OtuBadge, OtuBadge
 * and OtuBadge now one type there is nothing left to collide with, so the
 * parameters are spelled out. Verified neutral. */
s32 func_ov039_0208e28c(OtuBadge* self, OtuBadge* other) {

    if (self->flags > 0) {
        return 0;
    }
    if (self->phase != 1 && self->phase != 6 && self->phase != 7) {
        return 0;
    }
    if (self->height != 0) {
        return 0;
    }
    if (other->flags > 0) {
        return 0;
    }
    if (other->phase != 1 && other->phase != 6 && other->phase != 7) {
        return 0;
    }
    if (other->height != 0) {
        return 0;
    }

    if (func_ov039_0208df2c(&self->pos, &self->vel, 0xC000, &other->pos, &other->vel, 0xC000) == 0) {
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
/* Typed as the sibling predicates above; see the note on func_ov039_0208e28c. */
s32 func_ov039_0208e37c(OtuBadge* self, OtuBadge* other) {

    OtuPoint dir;
    s32      dist;
    s32      gap;

    if (self->flags > 0) {
        return 0;
    }

    switch (self->phase) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (self->height != 0) {
        return 0;
    }

    if (other->flags > 0) {
        return 0;
    }

    switch (other->phase) {
        case 0:
        case 2:
        case 5:
        case 8:
            return 0;

        default:
            break;
    }

    if (other->height != 0) {
        return 0;
    }

    dist = func_ov039_02098ca8(&self->pos, &other->pos);

    // The success path is the `if` body, not a fall-through after an early
    // `return 0;`: the target branches to a shared epilogue, and an early
    // return here makes mwcc predicate the exit instead. (objdiff 100%.)
    if (dist <= 0x18000) {
        gap = 0x18000 - dist;

        func_ov039_02098bb0(&self->pos, &other->pos, &dir);

        if (dir.x == 0 && dir.y == 0) {
            dir.x = 0x1000;
            dir.y = 0;
        } else {
            func_ov039_02098d3c(&dir, &dir);
        }

        func_ov039_02098c00(gap / 2, &dir, &self->pos, &self->pos);
        func_ov039_02098c00((-gap) / 2, &dir, &other->pos, &other->pos);

        return 1;
    }
    return 0;
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
/* Typed as the sibling predicates above; see the note on func_ov039_0208e28c. */
s32 func_ov039_0208e504(OtuBadge* self, OtuObstacle* obstacle) {

    OtuPoint other;
    OtuPoint dir;
    s32      radius;
    s32      len;

    switch (self->phase) {
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

    if (func_ov039_02098ca8(&self->pos, &other) >= radius + 0xC000) {
        return 0;
    }

    func_ov039_02098bb0(&self->pos, &other, &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    // Note the argument order: `other` is the base here, not `dir`, so this
    // moves the obstacle's copy of the point rather than the pin's.
    func_ov039_02098c00(radius + 0xC000, &dir, &other, &self->pos);

    func_ov039_02098bb0(&other, &self->pos, &dir);

    if (dir.x != 0 || dir.y != 0) {
        func_ov039_02098d3c(&dir, &dir);
    } else {
        dir.x = 0x1000;
        dir.y = 0;
    }

    len = func_ov039_02098c40(&self->vel, &dir);
    func_ov039_02098c00(-(len * 2), &dir, &self->vel, &self->vel);

    if (self->vel.x != 0 || self->vel.y != 0) {
        func_ov039_02098d3c(&self->vel, &dir);

        len = func_ov039_02098d10(&self->vel);
        func_ov039_02098bd4(OtuQ12Mul(len, data_ov039_0209a324), &dir, &self->vel);

        self->dir.x = dir.x;
        self->dir.y = dir.y;
    }

    func_ov039_02092760(obstacle);

    return 1;
}

/* The badge's point accessors. Each is a whole-OtuPoint assignment: two scalar
 * stores interleave differently, and a local merges into `stmia`. */

void func_ov039_0208e6cc(OtuBadge* self, OtuPoint* out) {
    *out = self->dir;
}

void func_ov039_0208e6e0(OtuBadge* task, OtuPoint* out) {
    *out = task->pos;
}

s32 func_ov039_0208e6f4(OtuBadge* task) {
    return (task)->height;
}

/* The pin-child steering helpers, 0x0208e6fc and 0x0208e8c4. */

/**
 * @brief The velocity a pin child should steer with, written to `out`.
 *
 * Only phase 8 enters the switch; phase 1 and everything past the two cases fall
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
void func_ov039_0208e6fc(OtuBadge* self, OtuPoint* out) {

    if (self->phase == 8) {
        switch (self->subKind) {
            case 2: {
                OtuPoint scratch = {self->pos.x - 0x80000, self->pos.y - 0x60000};

                func_ov039_02098bb0(&scratch, &self->homeOffset, &scratch);

                if (func_ov039_02098d10(&scratch) >= 0x400) {
                    func_ov039_02098d3c(&scratch, &scratch);
                    func_ov039_02098c00(0x400, &scratch, &self->homeOffset, out);
                    return;
                }

                out->x = self->pos.x - 0x80000;
                out->y = self->pos.y - 0x60000;
                return;
            }

            case 3: {
                OtuPoint scratch;
                s32      scale = FX_Divide(0x1000, self->frameBudget << 12);

                scratch.x = self->pos.x - 0x80000;
                scratch.y = self->pos.y - 0x60000;
                func_ov039_02098bb0(&scratch, &self->homeOffset, &scratch);
                func_ov039_02098c00(scale, &scratch, &self->homeOffset, out);
                return;
            }

            default:
                break;
        }
    }

    out->x = self->pos.x - 0x80000;
    out->y = self->pos.y - 0x60000;
}

void func_ov039_0208e848(OtuBadge* self, OtuPoint* origin) {
    self->origin = *origin;
}

/* The stage task's 0x110 - 0x1B4 block. */

void func_ov039_0208e85c(OtuBadge* self, OtuPoint* out) {
    *out = self->origin;
}

void func_ov039_0208e870(OtuBadge* self, s32 x, s32 y) {
    self->homeOffset.x = x;
    self->homeOffset.y = y;
}

void func_ov039_0208e87c(OtuBadge* self, OtuPoint* out) {
    *out = self->homeOffset;
}

/* ============================================================================
 * Band 14 -- the rest of OtuBadge's field accessors.
 *
 * The seven remaining functions in the run that follows band 12, from 0x0208e890
 * to 0x0208e9e4. The seven below them -- 0208e6cc, _6e0, _6f4, _848, _85c, _870
 * and _87c -- are already in OtuFieldAccess.c itself, so this file does not
 * repeat them. Every body here is a handful of instructions copied straight from
 * the target, and where the target used a jump table this keeps the switch so
 * mwcc emits one too.
 *
 * Two signature styles appear here because both already exist for these
 * functions, in bands 1, 7, 13 and the shared header, and the earlier
 * declaration has to be the one the definition agrees with:
 *
 *   OtuBadge*  the header's trimmed +0x174-byte view of the same object,
 *                modelling only the fields the nearest-child queries read.
 *   void*        what band 1 and band 7 happen to pass around.
 *
 * OtuBadge has no field for +0x0DC or +0x0FC, so those two read through
 * OtuBadge, which band 12 declares and which models the whole 0x25C bytes.
 * Every offset OtuBadge does model agrees with OtuBadge, so the cast is
 * always safe.
 *
 * Offsets: +0x0DC unk_0DC, +0x0F8 kind, +0x0FC subKind, +0x138 dir.
 * =========================================================================*/

/**
 * @brief Only kinds 0, 2, 3 and 4 answer with zero; everything else -- kind 1,
 *        and any value past the table -- answers with +0x0DC.
 *
 *  The target reaches its five entries with `cmp r1, #4 / addls pc, pc, r1,
 *  lsl #2`, defaulting to the load, so this stays a switch with one arm per
 *  case value rather than a chain of equality tests.
 */
s32 func_ov039_0208e890(OtuBadge* self) {

    switch (self->phase) {
        case 0:
        case 2:
        case 3:
        case 4:
            return 0;

        case 1:
        default:
            break;
    }

    return self->visible;
}

/**
 * @brief The pin child's aim scale, 0x1000 when nothing applies.
 *
 * Phase 8, sub-kind 1: while the +0x100 cursor is under 10 this is the cursor
 * over 10, on a 0x1000 scale.
 *
 * Phase 8, sub-kind 4: the cursor at +0x100 against half the cell's size word,
 * counted down from 0x1000.
 */
s32 func_ov039_0208e8c4(OtuBadge* self) {
    s32 scale = 0x1000;

    if (self->phase == 8) {
        switch (self->subKind) {
            case 1: {
                s32 cursor = self->frameBudget;

                if (cursor < 0xA) {
                    scale = FX_Divide(cursor << 12, 0xA000);
                }
                break;
            }

            case 4: {
                u32 size = self->slots[*self->pinID].meteoSquash;

                if (self->frameBudget < (s32)(size >> 1)) {
                    scale = 0x1000 - FX_Divide(self->frameBudget << 12, (s32)(size >> 1) << 12);
                }
                break;
            }

            default:
                break;
        }
    }

    return scale;
}

/**
 * @brief A three-valued answer, but only for phase 8: subkind 2 answers 2,
 *        subkind 3 answers 3, and every other combination answers 0.
 *
 *  The `bne` past both tests is why this is a nested if rather than a second
 *  switch -- the subkind is never read unless the kind already matched.
 */
s32 func_ov039_0208e950(OtuBadge* self) {
    s32 r = 0;

    if (self->phase == 8) {
        /* A switch, not two `if`s: with two `if`s mwcc selects both arms with
         * `moveq`, while the target branches to an out-of-line `mov r2, #2` for
         * the first and only then tests for 3 with a conditional move. The arms
         * need their own labels to come out that way. */
        switch (self->subKind) {
            case 2:
                r = 2;
                break;

            case 3:
                r = 3;
                break;
        }
    }

    return r;
}

/**
 * @brief True when the pin's kind is 8.
 *
 *  Written as a local plus a select, not as `return task->phase == 8;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x8 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e984(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 8) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 7.
 *
 *  Written as a local plus a select, not as `return task->phase == 7;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x7 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e998(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 7) {
        r = 1;
    }

    return r;
}

/**
 * @brief The heading of -dir, as an unsigned 16-bit index.
 *
 *  Both components are negated before the call rather than the result being
 *  negated after it, and the answer is cut to sixteen bits with
 *  `lsl #0x10 / lsr #0x10`. That pair is a zero-extend, so the narrowing is to
 *  `u16` and not to `s16` -- an `s16` emits `asr` for the second shift and costs
 *  the match. The return type stays `s32` because band 7 declares it that way
 *  and uses the value.
 */
s32 func_ov039_0208e9ac(OtuBadge* self) {
    u16 a;
    s32 x = -self->dir.x;
    s32 y = -self->dir.y;

    /* y first, x second -- that is the callee's own parameter order
     * (`u16 FX_Atan2Idx(s32 y, s32 x)`), and mwcc evaluates arguments in
     * reverse, which is why the target loads +0x138 into r1 and +0x13C into r2
     * even though r1 is the first argument slot.
     *
     * Nonmatching: the target keeps the call and then narrows with
     * `lsl #0x10 / lsr #0x10`, so its FX_Atan2Idx must have been visible as
     * returning something wider than u16 -- an implicit int. fx_atan.h declares
     * it as u16, so mwcc here proves the narrowing redundant and turns the whole
     * thing into a tail call through a veneer, dropping the mask. Recovering the
     * last 60% means hiding that prototype from this translation unit, which
     * would cost bands 3 and 11 their own narrowing. Not worth it.
     */
    a = FX_Atan2Idx(y, x);

    return a;
}

/**
 * @brief True when the pin's kind is 6.
 *
 *  Written as a local plus a select, not as `return task->phase == 6;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x6 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9d0(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 6) {
        r = 1;
    }

    return r;
}

/**
 * @brief True when the pin's kind is 9.
 *
 *  Written as a local plus a select, not as `return task->phase == 9;`. The
 *  target copies the field into a register, zeroes the result and selects into
 *  it -- `ldr r1, [r0, #0xf8] / mov r0, #0 / cmp r1, #0x9 / moveq r0, #1` --
 *  where a bare comparison compiles to a conditional move against the loaded
 *  field. Same answer, different shape, 58% against 100%.
 */
s32 func_ov039_0208e9e4(OtuBadge* task) {
    s32 found = task->phase;
    s32 r     = 0;

    if (found == 9) {
        r = 1;
    }

    return r;
}

/**
 * @brief Runs the pin's current phase handler and reports whether it claimed.
 *
 * The three cases are the three phase handlers at +0xF8: 6 takes the read from
 * `func_ov039_02091628`, 7 the one from `func_ov039_02090e9c`, and 8 the
 * completion from `func_ov039_0208fee0` -- all three writing the same
 * three-word record at +0x184 and stashing their result in +0x180. Cases 6 and
 * 7 set the return value only when the handler reports a positive count;
 * case 8 claims unconditionally but only when +0xFC is 5 and +0x100 is 0x27.
 */
// Nonmatching: 97.4%, three instructions. The target branches past the body
// twice (`bne`/`bne`) for the +0xFC/+0x100 pair; this source has mwcc
// if-convert the pair into a conditional load (`ldreq r1, [r5, #0x100]`) and a
// single `cmpeq`. Three spellings were tried -- a positive `&&`, nested `if`s,
// a negated `||`, and two sequential guards each with its own `break` -- and
// all four compile to the same folded form. mwcc is choosing conditional
// execution over branching here and there is no source shape that changes that.
s32 func_ov039_0208e9f8(TaskPool* pool, OtuBadge* self) {
    s32 claimed = 0;
    s32 ret;

    switch (self->phase) {
        case 6:
            ret            = func_ov039_02091628(EasyTask_GetTaskData(pool, self->needleId), self->hits);
            self->hitCount = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 7:
            ret            = func_ov039_02090e9c(EasyTask_GetTaskData(pool, self->hammerId), self->hits);
            self->hitCount = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 8:
            /* Two sequential guards with their own `break`, not one combined
             * condition: `&&`, nested `if`s and a negated `||` all fold the
             * pair into `ldreq`/`cmpeq`, where the target branches past the
             * body twice. */
            if (self->subKind != 5) {
                break;
            }
            if (self->frameBudget != 0x27) {
                break;
            }

            claimed = 1;
            func_ov039_0208fee0(EasyTask_GetTaskData(pool, self->meteoId), self->hits);
            self->hitCount = claimed;
            break;

        default:
            break;
    }
    return claimed;
}

/* The badge clash wind-up, 0x0208eaa0. */

/** The Q12 scale word and the 0x10-stride per-speed factor table. */

// extern s32 data_ov039_0209a398;   (already declared in TinPinSlammer.h)

// extern OtuSpeedEntry data_ov039_0209a3dc[];  (declared in OtuPinLogic)

/**
 * @brief Resolves whatever the badge found to clash against and starts the
 *        strike.
 *
 * Gate: +0x128 clear and +0x168 not positive, and the badge's mock mode
 * (+0xF8) one of 1/6/7 or, for mode 8, only while the +0xFC phase is 0 (or
 * past the range). Then for every 0xC-stride entry in the caller's object:
 * the candidate must be within the entry's +0x18C rank plus 0xC000, and the
 * clash sequence runs:
 *
 *   mode 6  fires the latch (mode 0x337), re-raises the 0x130-"no pin" shadow
 *           reset, resolves the contact with d3c (or a unit vector if the
 *           points coincide),
 *   mode 7  drops the +0x4000 phase byte of the active cursor, leaves
 *           `func_ov039_0209104c` to hold it, then starts a directional smoke
 *           pass from the sin/cos table (mode 0x33B) and steps its children
 *           while the badge's kind is still 6,
 *   mode 8  drops the airborne child task, mode 0x333, the contact again, and
 *           the tail that only mode 8 spares.
 * The tail (shared by 6/7/8): the badge's wind-up counter at +0x148 and kind
 * reset (mode 1) with `func_ov039_0208f7a4`, the position delta saved into
 * +0x138/+0x13C, and the two-way "clash partner" handoff at +0x1B0.
 */
// Nonmatching: 89.6%, scheduling and register naming only: the contact
// block's loads, the order of the two pool words, and which callee-saved
// registers the hit walk lands in. (Walking the hits through a raw byte pointer
// to the badge scored 89.9%, through register naming alone.)
void func_ov039_0208eaa0(OtuBadge* other, OtuBadge* self) {

    if (self->height != 0) {
        return;
    }
    if (self->flags > 0) {
        return;
    }

    switch (self->phase) {
        /* m2c's label order: default first (it joins the big block), then the
         * modes that share the body, then mode 8's phase gate, then the
         * modes that return. */
        default:
        case 1:
        case 6:
        case 7:
            break;

        case 8:
            switch (self->subKind) {
                default:
                case 0:
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                    return;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 9:
            return;
    }

    if (other->hitCount <= 0) {
        return;
    }
    {
        s32 i = 0;

        do {
            s32 limit = other->hits[i].scale + 0xC000;

            if (func_ov039_02098ca8(&self->pos, (OtuPoint*)&other->hits[i]) >= limit) {
                goto next;
            }

            switch (other->phase) {
                case 6: {
                    func_ov039_02091668(EasyTask_GetTaskData(other->pool, other->needleId));
                    func_ov039_02087d04(0x337, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }

                    self->vel.x = self->pos.x - other->pos.x;
                    self->vel.y = self->pos.y - other->pos.y;

                    if (self->vel.x != 0 || self->vel.y != 0) {
                        func_ov039_02098d3c(&self->vel, &self->vel);
                    } else {
                        self->vel.x = 0x1000;
                        self->vel.y = 0;
                    }
                    break;
                }

                case 7: {
                    void* cand   = EasyTask_GetTaskData(other->pool, other->hammerId);
                    u16   cursor = (u16)(func_ov039_02091060(cand) - 0x4000);
                    s32   pair   = (cursor >> 4) * 2;

                    func_ov039_0209104c(cand);

                    self->vel.x = ((s16*)data_0205e4e0)[pair + 1];
                    self->vel.y = ((s16*)data_0205e4e0)[pair];

                    if (self->phase == 6) {
                        func_ov039_02091690(EasyTask_GetTaskData(self->pool, self->needleId));
                    }
                    func_ov039_02087d04(0x33B, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }
                    break;
                }

                case 8: {
                    if (self->phase == 7) {
                        func_ov039_02091070(EasyTask_GetTaskData(self->pool, self->hammerId));
                    }
                    func_ov039_02087d04(0x333, &other->pos, &other->origin);

                    if (self->stun <= 0) {
                        if (*self->pinID < 0x130) {
                            func_ov039_0208a490(other, 1);
                        }
                    }

                    self->vel.x = self->pos.x - other->pos.x;
                    self->vel.y = self->pos.y - other->pos.y;

                    if (self->vel.x != 0 || self->vel.y != 0) {
                        func_ov039_02098d3c(&self->vel, &self->vel);
                    } else {
                        self->vel.x = 0x1000;
                        self->vel.y = 0;
                    }
                    break;
                }

                default:
                    break;
            }

            if (self->stun <= 0) {
                self->stun  = self->slots[*self->pinID].stunFrames;
                self->phase = 1;
                self->step  = 0;
                func_ov039_0208f7a4(EasyTask_GetTaskData(self->pool, self->piyoId), self->stun);
            }
            {
                OtuPoint  start;
                OtuPoint* base   = &self->vel;
                s32       factor = data_ov039_0209a3dc[self->slots[*self->pinID].tuneIndex].power;
                s32       scale;

                start.x = 0;
                scale   = (s32)(((s64)data_ov039_0209a398 * factor + 0x800) >> 0xC);
                start.y = 0;

                self->dir.x = self->vel.x;
                self->dir.y = self->vel.y;

                func_ov039_02098c00(scale, base, &start, base);
                other->partner = self;
                self->partner  = other;
            }

        next:
            i = i + 1;
        } while (i < other->hitCount);
    }
}

/* The pin task's own queries, 0x0208ee84 - 0x0208efb0. */

/**
 * @brief True while the pin's "alive" counter at +0x148 is positive.
 *
 * `movgt`/`movle`, not `movne`/`moveq`: the comparison is signed, so this is
 * `> 0` and not `!= 0`. Written as a select into zero, the shape that gives
 * this overlay's predicates their two conditional moves.
 */
s32 func_ov039_0208ee84(OtuBadge* task) {
    return task->stun > 0;
}

/** The badge's remaining stun frames. */
s32 func_ov039_0208ee98(OtuBadge* task) {
    return (task)->stun;
}

/* Four consecutive s16s at base + 0x100 + 0x78. */

/*
 * These four read 0x78, 0x7A, 0x7C and 0x7E off a base already offset by 0x100,
 * so they are four consecutive s16 fields -- `OtuBadge.tile0`..`tile3`, which sit
 * at 0x178. Note the offset was *not* spelled as `0x100 + 0x78`: the split is
 * what makes the target emit a separate `add r0, r0, #0x100` before the `ldrsh`,
 * and naming the field folds it into a single `ldrsh [r0, #0x178]` that this
 * build does not produce. Naming the field is still codegen-neutral -- mwcc keeps
 * the add -- so the accessors read as members and the note records why.
 */

s16 func_ov039_0208eea0(OtuBadge* task) {
    return (task)->trackFrames;
}

s16 func_ov039_0208eeac(OtuBadge* task) {
    return (task)->bounceTimer;
}

s16 func_ov039_0208eeb8(OtuBadge* task) {
    return (task)->arcFrames;
}

s16 func_ov039_0208eec4(OtuBadge* task) {
    return (task)->spinFrames;
}

/**
 * @brief The pin's three-way phase test, read off +0x128/+0xF8/+0xF4.
 *
 * Answers 1 only when +0x128 is zero and both +0xF8 and +0xF4 are 1 *and* the
 * distance between the points at +0x14C and +0x154 is at least 0x10000. The
 * target's `ldreq`/`cmpeq` run of conditional loads is mwcc folding that
 * five-term short-circuit chain into one compare chain.
 */
s32 func_ov039_0208eed0(OtuBadge* self) {
    s32 r = 0;

    if (self->height == 0 && self->phase == 1 && self->step == 1) {
        if (func_ov039_02098ca8(&self->aimStart, &self->aimCur) >= 0x10000) {
            r = 1;
        }
    }

    return r;
}

/** Copies the +0x14C and +0x154 pairs out as two points. */
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b) {
    OtuBadge* self = (OtuBadge*)pin;

    *a = self->aimStart;
    *b = self->aimCur;
}

/**
 * @brief True once the pin's age at +0x1CC has reached 0x1E (30).
 *
 * `movge`/`movlt` again: `>= 0x1E`, not `> 0x1E`.
 */
s32 func_ov039_0208ef38(OtuBadge* task) {
    return (task)->aimFrames >= 0x1E;
}

/**
 * @brief True when the pin should be removed this frame.
 *
 * A switch on the kind at +0xF8 in which only kind 1 answers zero: every other
 * kind, and any kind past the table, goes to the body. That is why the jump
 * table has one distinct arm and the rest share the default -- the case list is
 * `0, 2, 3, 4, 5` plus `default`, with `1` out of line.
 *
 * The body applies the age filter and the RNG gate: a pin whose cell query is
 * not exactly 0xC is removed outright; one that is gets removed with
 * probability `which / 0x10000`.
 */
s32 func_ov039_0208ef4c(void* task, u32 which) {
    OtuBadge* self = (OtuBadge*)task;
    s32       kind = self->phase;
    s32       r    = 0;

    switch (kind) {
        case 1:
        default:
            if (func_ov039_0208a794((OtuPoint*)&self->pos.x, self->board) != 0xC) {
                r = 1;
            } else if (RNG_Next(0x10000) < which) {
                r = 1;
            }
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
    }

    return r;
}

/**
 * @brief The pin-scoring filter: true when a child is a real pin.
 *
 * Two guards open it -- the nearest-child query at +0x120 must answer non-zero,
 * and the pin id at +0x16C must not be the 0x130 "no pin" sentinel -- and the
 * real answer comes from func_ov039_0208ef4c.
 */
s32 func_ov039_0208efb0(OtuBadge* task, s32 which) {
    OtuBadge* self = (OtuBadge*)task;

    if (func_ov039_0208a794((OtuPoint*)&self->pos.x, self->board) == 0) {
        return 0;
    }
    if (*self->pinID == 0x130) {
        return 0;
    }

    return func_ov039_0208ef4c(task, which);
}

/**
 * The +0x1A4 word, read-and-cleared.
 *
 * Returns the old value and stores zero, so a caller polling this sees each
 * value exactly once. It is the only accessor here with that shape.
 */
s32 func_ov039_0208eff8(OtuBadge* self) {
    s32 value = self->unk_1A4;

    self->unk_1A4 = 0;
    return value;
}

/* Badge accessors: 0208f00c - 0208f104. */

/** True when the badge's tray slot holds a real pin (0x130 means empty). */
s32 func_ov039_0208f00c(OtuBadge* self) {
    return *self->pinID < 0x130;
}

/** Tail-calls func_ov039_0208a490 with a second argument of 10. */
void func_ov039_0208f024(OtuBadge* self) {
    func_ov039_0208a490(self, 0xA);
}

s32 func_ov039_0208f034(OtuBadge* task) {
    return (task)->score;
}

/** Parks the badge's AI. */
void func_ov039_0208f03c(OtuBadge* task) {
    (task)->curAI = 0x11;
}

/**
 * Hands the next of the badge's eight group children a point and a selector,
 * round-robin. The cursor is written back before it wraps, and is re-read from
 * the badge on both sides of the call rather than held in a register.
 */
void func_ov039_0208f048(OtuBadge* self, OtuPoint* at, s32 selector) {
    func_ov039_02097750(EasyTask_GetTaskData(self->pool, self->smokeIds[self->smokeCursor]), at, selector, 0x1000, 0x66, 6);

    self->smokeCursor = self->smokeCursor + 1;
    if (self->smokeCursor >= 8) {
        self->smokeCursor = 0;
    }
}

/** True when the badge's tray slot is empty. */
s32 func_ov039_0208f0b0(OtuBadge* self) {
    return *self->pinID == 0x130;
}

/** Resets the badge's label child, if it has one. */
void func_ov039_0208f0c8(OtuBadge* self) {
    if (self->hasLabel != 0) {
        func_ov039_02096270(EasyTask_GetTaskData(self->pool, self->entryId));
    }
}

/** Forgets `other` as this badge's last contact. */
void func_ov039_0208f0f0(OtuBadge* self, OtuBadge* other) {
    if (self->partner == other) {
        self->partner = NULL;
    }
}

/** Resets both of the badge's pair children. */
void func_ov039_0208f104(OtuBadge* self) {
    s32 i;

    for (i = 0; i < 2; i++) {
        func_ov039_02095ddc(EasyTask_GetTaskData(self->pool, self->pointIds[i]));
    }
}
