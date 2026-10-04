#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x0208acc0 - 0x0208c4ec. One translation unit of the
 * overlay; dsd gives each file a single contiguous `.text` claim. The
 * shared types, externs and prototypes are in OtuFieldAccessShared.h.
 */
/* ------------------------------------------------------------------ */
/* The band.                                                           */
/* ------------------------------------------------------------------ */

/**
 * The mask of the sub-tile bits of one board cell.
 *
 * A cell is 0x20000 across in Q12.12, so this is `0x20000 - 1`: masking with it
 * and then adding or subtracting a whole cell walks from one cell boundary to
 * the next without having to know where in the cell the badge actually is.
 */
#define OTU_CELL_BITS 0x1FFFF

/** One board cell, in Q12.12. */
#define OTU_CELL_SIZE 0x20000

/*
 * Every Q12.12 multiply below is written out rather than hidden behind a macro
 * so each one can be read against the `smull`/`adds #0x800`/`adc`/`lsr #0xC`
 * quartet it produces. The rounding constant is in all of them: the target
 * rounds to nearest, and dropping it is a one-bit-per-call difference that a
 * match will not forgive.
 */

/** Rebuilds the packed state word from the home tile's flag. */
void func_ov039_0208acc0(OtuBadgeState* self) {
    u16 home  = (self->home->flags & 1) != 0 ? 1 : 0;
    u16 flags = self->stateFlags;

    // Bit 1 records "the badge is not where its home tile says it is". The
    // condition is the XOR masked back down to one bit rather than a plain
    // compare, which is what produces the target's eor/and/tst trio.
    if ((flags ^ home) & home & 1) {
        flags |= 2;
    } else {
        flags &= ~2;
    }
    self->stateFlags = flags;

    // Bit 2 records the transition itself, so it runs the other way round: the
    // badge has to have been home and no longer is.
    flags = self->stateFlags;
    if ((flags ^ home) & flags & 1) {
        flags |= 4;
    } else {
        flags &= ~4;
    }
    self->stateFlags = flags;

    // Bit 0 is not a flag at all; it is the home tile's bit 0 latched in.
    self->stateFlags &= ~1;
    self->stateFlags |= home;
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
void func_ov039_0208ad88(OtuBadgeState* self) {
    if (self->phase == 2) {
        return;
    }

    func_ov039_02097aa4(EasyTask_GetTaskData(self->pool, self->taskId), (void*)&self->pos, 1);
    func_ov039_02087d04(0x343, &self->pos, &self->origin);

    self->vel.x   = 0;
    self->vel.y   = 0;
    self->step    = 0;
    self->phase   = 2;
    self->subKind = 0;
}

/** Enters phase 3: sit still for 0x1E frames with the badge visible. */
void func_ov039_0208ade8(OtuBadgeState* self) {
    if (self->phase == 3) {
        return;
    }

    self->step        = 0;
    self->phase       = 3;
    self->frameBudget = 0x1E;
    self->alive       = 0;
}

/** Enters phase 4: retire whatever the badge was attached to. */
void func_ov039_0208ae14(OtuBadgeState* self) {
    if (self->phase == 4) {
        return;
    }

    // A badge still carrying a sound handle hands it back with a different cue
    // depending on whether it had a live rival to chase.
    if (self->partner != NULL && *self->pinID < 0x130) {
        func_ov039_0208a490(self, self->alive > 0 ? 5 : 2);
    }
    self->partner = NULL;

    func_ov039_02087dc0(self->scene, self);

    self->alive       = 0;
    self->step        = 0;
    self->phase       = 4;
    self->frameBudget = 0x3C;
}

/** Enters phase 5: place the badge on its starting tile and start it rolling. */
void func_ov039_0208ae8c(OtuBadgeState* self) {
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
    self->unk_15C = ((self->board[self->index * 2 + 8] << 5) + 0x10) << 0xC;
    self->unk_160 = ((self->board[self->index * 2 + 9] << 5) + 0x10) << 0xC;
    self->pos.x   = self->unk_15C;
    self->pos.y   = self->unk_160;
    self->vel.x   = 0;
    self->vel.y   = 0;

    self->dir.x   = 0x1000;
    self->dir.y   = 0;
    self->alive   = 0;
    self->unk_1A4 = 1;
}

/** Enters phase 7's approach: kill the velocity and play the corner sound. */
void func_ov039_0208af38(OtuBadgeState* self) {
    self->unk_134 = data_ov039_0209a31c;
    self->step    = 0;

    func_ov039_02087d04(0x344, &self->pos, &self->origin);
}

/**
 * The rolling phase: accelerate, turn, clamp to the board, bounce off walls
 * and wall corners, drop trail marks, and finally react to the tile the badge
 * has just arrived on.
 */
void func_ov039_0208af6c(OtuBadgeState* self) {
    OtuPoint       probe;
    OtuPoint       norm;
    OtuPoint       zero;
    OtuBadgeSlot*  slot;
    OtuBadgeSpeed* speed;
    s32            tile;
    s32            cellX;
    s32            cellY;
    s32            len;
    s32            hit;

    self->unk_134 = self->unk_134 + data_ov039_0209a318;

    // Acceleration, unless something has already committed the badge to a
    // mode of its own.
    if (self->mode <= 0) {
        s32 rate;

        if (self->unk_128 == 0) {
            tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

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
        speed = &data_ov039_0209a3e0[slot->tuneIndex];

        rate = (s32)(((s64)speed->speed * rate + 0x800) >> 12);

        // While the badge is still being set up its rate is doubled. The test
        // is `phase <= 3`, i.e. every phase before the rolling one.
        if (self->phase <= 3) {
            rate = rate * 2;
        }

        func_ov039_0208ad2c(&self->vel, rate);
    }

    // Turning, but only while nothing external is steering the badge.
    if (self->unk_128 == 0) {
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
        self->vel.x = (s32)(((s64)len * *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2) + 0x800) >> 12);
        self->vel.y = (s32)(((s64)len * *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2) + 0x800) >> 12);
    }

    func_ov039_02098b8c(&self->pos, &self->vel, &self->pos);

    // The board is a fixed playfield, so both axes are clamped to a hard inset
    // rather than to whatever extent the tile lookup would accept.
    if (self->pos.x < 0x80000) {
        self->pos.x = 0x80000;
    } else {
        s32 limit = ((s32)self->board[2] << 5) - 0x80;

        if (self->pos.x >= (limit << 0xC)) {
            self->pos.x = (limit << 0xC) - 1;
        }
    }
    if (self->pos.y < 0x60000) {
        self->pos.y = 0x60000;
    } else {
        s32 limit = ((s32)self->board[3] << 5) - 0x60;

        if (self->pos.y >= (limit << 0xC)) {
            self->pos.y = (limit << 0xC) - 1;
        }
    }

    // A negative accumulator is an impulse off something solid. Once it has
    // been absorbed both accumulators are cleared together.
    self->unk_128 = self->unk_128 + self->unk_134;
    if (self->unk_128 > 0) {
        self->unk_134 = 0;
        self->unk_128 = 0;
    }

    // Trail marks, dropped every few frames while the badge is moving fast.
    if (self->unk_128 == 0 && self->phase != 3) {
        s32 trailLen = func_ov039_02098d10(&self->vel);

        if (trailLen > 0x1000) {
            s32 angle;

            self->timers.trailTimer = self->timers.trailTimer - 1;
            if (self->timers.trailTimer <= 0) {
                angle = FX_Atan2Idx(self->vel.y, self->vel.x);

                // Two halves, so the mark is centred on the badge rather than
                // trailing off one side of it.
                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                                    (void*)&self->pos, angle, 1, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                func_ov039_02095788((void*)EasyTask_GetTaskData(self->pool, self->trailId[self->timers.trailIndex]),
                                    (void*)&self->pos, angle, 0, trailLen);
                self->timers.trailIndex = self->timers.trailIndex + 1;

                if (self->timers.trailIndex >= 0xC) {
                    self->timers.trailIndex = 0;
                }
                self->timers.trailTimer = 5;
            }
        }
    }

    // Walls. A wall is tile type 2; the badge is folded back onto the boundary
    // it crossed rather than stopped at the probe point. An axis that has
    // already reflected suppresses all four corner tests below.
    if (self->unk_128 == 0) {
        hit = 0;

        if (self->vel.x > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x + 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x & ~OTU_CELL_BITS) - 0xC000;
                self->vel.x = self->vel.x * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.x < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.x = probe.x - 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.x = (probe.x | OTU_CELL_BITS) + 0xC000;
                self->vel.x = -self->vel.x;
                hit         = 1;
            }
        }

        if (self->vel.y > 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y + 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
            if (tile == 2) {
                self->pos.y = (probe.y & ~OTU_CELL_BITS) - 0xC000;
                self->vel.y = self->vel.y * -0x2000;
                hit         = 1;
            }
        } else if (self->vel.y < 0) {
            func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
            probe.y = probe.y - 0xC000;
            tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
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
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
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
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x + OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
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
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y - OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
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
                func_ov039_02098b78((OtuStageDispatch*)&self->pos, (s32*)&probe);
                probe.x = probe.x - OTU_CELL_SIZE;
                probe.y = probe.y + OTU_CELL_SIZE;
                tile    = func_ov039_0208a794(&probe, (OtuCellGrid*)self->board);
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
    if (self->unk_128 != 0) {
        return;
    }

    tile = func_ov039_0208a794(&self->pos, (OtuCellGrid*)self->board);

    if (tile == 0) {
        func_ov039_0208ade8(self);
    } else if (tile == 12) {
        self->unk_0DC = 0;
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
            if (self->board[5] != 0) {
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
void func_ov039_0208b94c(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.trackFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 6;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091654(EasyTask_GetTaskData(self->pool, self->taskId2), slot->animX, slot->animY);

    self->timers.trackFrames = self->timers.trackFrames - 1;
}

/** Enters phase 7: hand the badge to the arc task for `arcFrames`. */
void func_ov039_0208b9b4(OtuBadgeState* self) {
    OtuBadgeSlot* slot;

    if (self->timers.arcFrames <= 0) {
        return;
    }

    self->step  = 0;
    self->phase = 7;

    slot = &self->slots[self->pinID[0]];
    func_ov039_02091028(EasyTask_GetTaskData(self->pool, self->taskId1), slot->curveA, slot->curveB, slot->curveC,
                        slot->curveD);

    self->timers.arcFrames = self->timers.arcFrames - 1;
}

/** Enters phase 8: stop dead for `bounceFrames`. */
void func_ov039_0208ba34(OtuBadgeState* self) {
    if (self->timers.bounceTimer <= 0) {
        return;
    }

    self->vel.x   = 0;
    self->vel.y   = 0;
    self->phase   = 8;
    self->step    = 0;
    self->subKind = 0;

    self->timers.bounceTimer = self->timers.bounceTimer - 1;
}

/** Enters phase 9: spin down to a stop over `spinFrames`. */
void func_ov039_0208ba70(OtuBadgeState* self) {
    OtuPoint zero;

    if (self->timers.spinFrames <= 0) {
        return;
    }

    zero.x = 0;
    zero.y = 0;

    func_ov039_02098c00(data_ov039_0209a38c, &self->dir, &zero, &self->vel);
    func_ov039_02098d3c(&self->vel, &self->dir);

    self->unk_134     = data_ov039_0209a320;
    self->step        = 0;
    self->phase       = 9;
    self->step        = 0;
    self->frameBudget = 9;

    func_ov039_02091b34(EasyTask_GetTaskData(self->pool, self->taskId3));

    if (self->alive > 0) {
        func_ov039_0208a490(self, 1);
    }

    self->timers.spinFrames = self->timers.spinFrames - 1;
}

/** The per-frame decision: which set-piece phase, if any, runs this frame. */
void func_ov039_0208bb2c(OtuBadgeState* self) {
    OtuPoint fromHome;
    OtuPoint fromPos;
    OtuPoint mid;
    OtuPoint legA;
    OtuPoint legB;

    // A badge that still has a rival attached is finishing an interaction. The
    // two-frame decrement only happens once the interaction is flagged as over,
    // so the frames are counted from the end rather than from the start.
    if (self->alive > 0) {
        if (self->stateFlags & 2) {
            self->alive = self->alive - 2;
            if (self->alive < 0) {
                self->alive = 0;
            }
        }
        self->step = 0;
        return;
    }

    if (self->unk_128 != 0) {
        self->step = 0;
        return;
    }

    // The set-piece phases, in the order they are allowed to claim the frame.
    if (self->contactFlags & 0x11) {
        self->step = 0;
        func_ov039_0208b94c(self);
        return;
    }
    if (self->contactFlags & 0x820) {
        self->step = 0;
        func_ov039_0208b9b4(self);
        return;
    }
    if (self->contactFlags & 0x440) {
        self->step = 0;
        func_ov039_0208ba34(self);
        return;
    }

    if (self->step == 0) {
        // Wait for the home tile to report itself before aiming anywhere.
        if (!(self->stateFlags & 2)) {
            return;
        }

        self->unk_1CC = 0;
        self->step    = 1;

        self->aimStart.x = self->home->tileX << 0xC;
        self->aimStart.y = self->home->tileY << 0xC;
        self->aimCur.x   = self->aimStart.x;
        self->aimCur.y   = self->aimStart.y;

    } else if (self->step == 1) {
        s32 turn;
        s32 scale;
        s32 reach;
        s32 sign;

        if (self->unk_1CC < 0x1E) {
            self->unk_1CC = self->unk_1CC + 1;
        }

        // The aim point tracks the home tile, so until the badge has somewhere
        // else to be it is still following it.
        self->aimCur.x = self->home->tileX << 0xC;
        self->aimCur.y = self->home->tileY << 0xC;

        if (!(self->stateFlags & 4)) {
            return;
        }

        self->step = 0;

        turn = func_ov039_02098ca8(&self->aimStart, &self->aimCur);
        if (turn <= 0) {
            return;
        }

        // Join the two aim points by an offset from where the badge is now.
        func_ov039_02098b8c(&self->aimStart, &self->unk_118, &fromHome);
        func_ov039_02098b8c(&self->aimCur, &self->unk_118, &fromPos);

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
        if (self->unk_1CC >= 0x1E) {
            scale      = (s32)(((s64)scale * 0x1800 + 0x800) >> 12);
            self->mode = 0x10;
        }

        func_ov039_02098d3c(&mid, &mid);
        func_ov039_0208a6f8((OtuPinLogic*)self, &mid, scale);

        if (self->unk_1CC >= 0x1E) {
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
void func_ov039_0208be30(OtuBadgeState* self, OtuPoint* dir) {
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

    func_ov039_0208a6f8((OtuPinLogic*)self, &step, 0x1000);

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
s32 func_ov039_0208bed8(OtuBadgeState* self, s32 rings, s32 loType, s32 hiType, OtuPoint* out) {
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
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.y = at.y + OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
            if (tile >= loType && tile <= hiType) {
                out->x = at.x;
                out->y = at.y;
                return 1;
            }
            at.x = at.x - OTU_CELL_SIZE;
        }

        for (i = 0; i < span; i++) {
            tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
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
s32 func_ov039_0208c128(OtuBadgeState* self, s32 which, s32 loType, s32 hiType, s32 kind) {
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
s32 func_ov039_0208c218(OtuBadgeState* self) {
    // Once the badge is committed to something it stops re-rolling, so the
    // chance test is only worth making while it is still free.
    if (self->curAI != 0 && self->chanceTbl[0] < RNG_Next(0x10000)) {
        return 0;
    }

    func_ov039_0208ba70(self);
    return 1;
}

/** The "keep rolling forward until something opens up" AI. */
s32 func_ov039_0208c258(OtuBadgeState* self) {
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
        tile = func_ov039_0208a794(&at, (OtuCellGrid*)self->board);
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
s32 func_ov039_0208c304(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         alive;
    s32         gap;

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
s32 func_ov039_0208c3bc(OtuBadgeState* self) {
    OtuPinTask* target;
    OtuPoint    away;
    s32         gap;

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
s32 func_ov039_0208c45c(OtuBadgeState* self) {
    OtuPinTask* target;
    s32         speed;
    s32         gap;

    if (self->chanceTbl[4] < RNG_Next(0x10000)) {
        return 0;
    }

    // An arc needs an arc task with frames left on it, and is not worth
    // starting for a badge already moving too fast to join it.
    if (self->timers.arcFrames <= 0) {
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
