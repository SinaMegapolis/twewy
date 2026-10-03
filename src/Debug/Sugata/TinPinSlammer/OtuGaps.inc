/* ============================================================================
 * Band 15 -- the pin task's own queries and the small helpers around them.
 *
 * These are the gaps left in OtuFieldAccess.c's own address range plus a few
 * neighbours it can see: the pin task's phase filter (0x0208ee84 - 0x0208efb0)
 * and the wireless input snapshot (0x02088418 - 0x020885f4). They live in a
 * separate .inc and are included LAST so every function they call is already
 * declared or defined above.
 *
 * See OtuBands.h for why this is an include rather than a translation unit,
 * and for the `-ipa file` cache trap that means an edit here does not by
 * itself invalidate OtuFieldAccess.o:
 *
 *     rm build/usa/src/Debug/Sugata/TinPinSlammer/OtuFieldAccess.o
 *     ninja build/usa/src/Debug/Sugata/TinPinSlammer/OtuFieldAccess.o
 * =========================================================================*/

#include "Debug/Sugata/TinPinSlammer.h"
#include "Engine/Core/System.h"
#include "Engine/Math/Random.h"
#include "Engine/IO/TouchInput.h"
#include "SpriteMgr.h"

extern u8  data_ov039_0209ad00;
extern s32 data_ov039_0209ad04;
extern s32 data_ov039_0209ad08[4];

/* The task handles the result-screen spawners hand to EasyTask_CreateTask. */
extern const TaskHandle data_ov039_0209a06c;
extern const TaskHandle data_ov039_02099c64;

/* ------------------------------------------------------------------ */
/* The pin task's own queries, 0x0208ee84 - 0x0208efb0.                */
/* ------------------------------------------------------------------ */

/**
 * @brief True while the pin's "alive" counter at +0x148 is positive.
 *
 * `movgt`/`movle`, not `movne`/`moveq`: the comparison is signed, so this is
 * `> 0` and not `!= 0`. Written as a select into zero, the shape that gives
 * this overlay's predicates their two conditional moves.
 */
s32 func_ov039_0208ee84(OtuPinTask* task) {
    return *(s32*)((u8*)task + 0x148) > 0;
}

/**
 * @brief The pin's three-way phase test, read off +0x128/+0xF8/+0xF4.
 *
 * Answers 1 only when +0x128 is zero and both +0xF8 and +0xF4 are 1 *and* the
 * distance between the points at +0x14C and +0x154 is at least 0x10000. The
 * target's `ldreq`/`cmpeq` run of conditional loads is mwcc folding that
 * five-term short-circuit chain into one compare chain.
 */
s32 func_ov039_0208eed0(void* pin) {
    u8* self = (u8*)pin;
    s32 r    = 0;

    if (*(s32*)(self + 0x128) == 0 && *(s32*)(self + 0xF8) == 1 &&
        *(s32*)(self + 0xF4) == 1)
    {
        if (func_ov039_02098ca8((OtuPoint*)(self + 0x14C), (OtuPoint*)(self + 0x154)) >=
            0x10000)
        {
            r = 1;
        }
    }

    return r;
}

/** Copies the +0x14C and +0x154 pairs out as two points. */
void func_ov039_0208ef14(void* pin, OtuPoint* a, OtuPoint* b) {
    u8* self = (u8*)pin;

    *a = *(OtuPoint*)(self + 0x14C);
    *b = *(OtuPoint*)(self + 0x154);
}

/**
 * @brief True once the pin's age at +0x1CC has reached 0x1E (30).
 *
 * `movge`/`movlt` again: `>= 0x1E`, not `> 0x1E`.
 */
s32 func_ov039_0208ef38(void* task) {
    return *(s32*)((u8*)task + 0x1CC) >= 0x1E;
}

/**
 * @brief The pin-scoring filter: true when a child is a real pin.
 *
 * Two guards open it -- the nearest-child query at +0x120 must answer non-zero,
 * and the pin id at +0x16C must not be the 0x130 "no pin" sentinel -- and the
 * real answer comes from func_ov039_0208ef4c.
 */
s32 func_ov039_0208efb0(OtuPinTask* task, s32 which) {
    u8* self = (u8*)task;

    if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) == 0) {
        return 0;
    }
    if (*(u16*)*(s32*)(self + 0x16C) == 0x130) {
        return 0;
    }

    return func_ov039_0208ef4c(task, which);
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
    u8* self = (u8*)task;
    s32 kind = *(s32*)(self + 0xF8);
    s32 r    = 0;

    switch (kind) {
        case 1:
        default:
            if (func_ov039_0208a794((OtuPoint*)(self + 0x120), (OtuCellGrid*)*(s32*)(self + 0xE4)) !=
                0xC)
            {
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
} OtuPadState; // Size: 0x6

extern OtuPadState data_ov039_0209ad20;

/**
 * @brief Maps the scene's mode selector onto a local player index.
 *
 * Zero for mode 0, ov040's index for mode 1, zero for anything else. The target
 * tests `!= 0` and then `== 1` rather than just `== 1`, so both tests are kept.
 */
s32 func_ov039_02088418(s32 mode) {
    s32 r = 0;

    if (mode != 0 && mode == 1) {
        r = func_ov040_0209d728();
    }

    return r;
}

/** The six-byte touch-pad descriptor for player slot `slot`. */
void* func_ov039_02088440(s32 slot) {
    return &data_ov039_0209af20[slot];
}

/**
 * @brief Polls the touch screen into one of those descriptors.
 *
 * `TouchInput_GetCoord` writes two words, so the coordinate bytes are read from
 * a stack pair rather than a single value. The "was touched" latch at +0 is set
 * from `TouchInput_IsTouchActive` through a select, and the `SysControl` word is
 * copied to +4 before the shared latch `data_ov039_0209ad00` is cleared.
 */
void func_ov039_02088564(void* padPtr) {
    OtuPadState* pad = (OtuPadState*)padPtr;
    TouchCoord   coord;

    TouchInput_GetCoord(&coord);

    pad->x = (u8)coord.x;
    pad->y = (u8)coord.y;

    if (TouchInput_IsTouchActive() != 0) {
        pad->touched = 1;
    } else {
        pad->touched = 0;
    }

    pad->sysControl       = *(u16*)&SysControl;
    pad->phase            = data_ov039_0209ad00;
    data_ov039_0209ad00   = 0;
}

/** Polls player 0 unless the board has already latched the pads. */
void func_ov039_020885cc(void) {
    if (data_ov039_0209ad04 != 0) {
        return;
    }

    func_ov039_02088564((OtuPadState*)&data_ov039_0209ad20);
}

/**
 * @brief Refreshes the four child-records from the wireless link.
 *
 * Only runs while ov040's link state is 5. When it is, a second ov040 query
 * decides between "records are live" and "records are gone": live records are
 * copied out of the returned pointers and the per-slot live flag raised, gone
 * ones are zeroed and the flag cleared, and the board's latch at
 * `data_ov039_0209ad04` is set to match. Any other link state clears all four
 * flags and the latch.
 */
void func_ov039_02088454(void) {
    s32 i;
    s32 src;

    if (func_ov040_0209cb78() != 5) {
        goto clear;
    }

    if (func_ov040_0209d4c4(&data_ov039_0209ad20) != 0) {
        for (i = 0; i < 4; i++) {
            src = func_ov040_0209d4a4((u16)i);
            if (src != 0) {
                MI_CpuCopyU8(src, &data_ov039_0209af20[i], 6);
                data_ov039_0209ad08[i] = 1;
            } else {
                MI_CpuSet(&data_ov039_0209af20[i], 0, 6);
                data_ov039_0209ad08[i] = 0;
            }
        }

        data_ov039_0209ad04 = 0;
        return;
    }

    for (i = 0; i < 4; i++) {
        data_ov039_0209ad08[i] = 0;
    }
    data_ov039_0209ad04 = 1;
    return;

clear:
    for (i = 0; i < 4; i++) {
        data_ov039_0209ad08[i] = 0;
    }
    data_ov039_0209ad04 = 0;
}

/* ------------------------------------------------------------------ */
/* The badge-task state predicates, 0x02091014 - 0x0209167c.           */
/* ------------------------------------------------------------------ */

/** True while the badge task has an active phase at +0xE8. */
s32 func_ov039_02091014(void* task) {
    return *(s32*)((u8*)task + 0xE8) != 0;
}

/**
 * @brief Seeds the badge task's cursors and enters the active phase.
 *
 * Four cursors: +0xF0, +0xF4 and +0xF8 take the three words, +0xFC takes the
 * fourth as a u16 (the target reads it with `ldrh` from the stack argument),
 * and both the +0x124 sub-phase and the +0xE8 phase are raised to 1.
 */
void func_ov039_02091028(void* task, s32 a, s32 b, s32 c, u16 d) {
    u8* self = (u8*)task;

    *(s32*)(self + 0xF0) = a;
    *(s32*)(self + 0xF4) = b;
    *(s32*)(self + 0xF8) = c;
    *(s32*)(self + 0xFC) = d;
    *(s32*)(self + 0x124) = 1;
    *(s32*)(self + 0xE8)  = 1;
}

/** Clears the +0xEC cursor once the +0xE8 phase has reached 4. */
void func_ov039_0209104c(void* task) {
    if (*(s32*)((u8*)task + 0xE8) == 4) {
        *(s32*)((u8*)task + 0xEC) = 0;
    }
}

/**
 * @brief The +0xE0 field, zero-extended to sixteen bits.
 *
 * The target loads a whole word and masks it with `lsl #0x10 / lsr #0x10`, so
 * the field is not a halfword in the source -- the read is a word cast down.
 */
u16 func_ov039_02091060(void* task) {
    return (u16)*(s32*)((u8*)task + 0xE0);
}

/**
 * @brief Latch the +0x7C value into +0x78 while the +0x80 phase is 3.
 *
 * A conditional load-and-store pair rather than a branch, so it stays a plain
 * `if` with the two statements in that order.
 */
void func_ov039_02091668(void* task) {
    u8* self = (u8*)task;

    if (*(s32*)(self + 0x80) == 3) {
        *(s32*)(self + 0x78) = *(s32*)(self + 0x7C);
    }
}

/** True while the +0x80 phase is non-zero. */
s32 func_ov039_0209167c(void* task) {
    return *(s32*)((u8*)task + 0x80) != 0;
}

/** Raises bit 1 of the +0x18 flags when `event` is non-zero. */
void func_ov039_020923b4(void* task, s32 event) {
    if (event != 0) {
        *(s32*)((u8*)task + 0x18) |= 2;
    }
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02094dfc(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02098338(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
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
                OtuPoint scratch = { *(s32*)(self + 0x120) - 0x80000,
                                     *(s32*)(self + 0x124) - 0x60000 };

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

/**
 * @brief The pin child's aim scale, 0x1000 when nothing applies.
 *
 * Kind 8, sub-kind 1: while the +0x100 cursor is under 10 this is the cursor
 * over 10, on a 0x1000 scale.
 *
 * Kind 8, sub-kind 4: the cursor at +0x100 against half the cell's size word,
 * counted down from 0x1000.
 */
s32 func_ov039_0208e8c4(void* task) {
    u8* self    = (u8*)task;
    s32 scale   = 0x1000;

    if (*(s32*)(self + 0xF8) == 8) {
        switch (*(s32*)(self + 0xFC)) {
            case 1: {
                s32 cursor = *(s32*)(self + 0x100);

                if (cursor < 0xA) {
                    scale = FX_Divide(cursor << 12, 0xA000);
                }
                break;
            }

            case 4: {
                u16 index = *(u16*)*(s32*)(self + 0x16C);
                s32 base  = *(s32*)(self + 0x170);
                u32 size  = *(u16*)((u8*)(base + index * 0x1C) + 0xE);

                if (*(s32*)(self + 0x100) < (s32)(size >> 1)) {
                    scale = 0x1000 - FX_Divide(*(s32*)(self + 0x100) << 12,
                                               (s32)(size >> 1) << 12);
                }
                break;
            }

            default:
                break;
        }
    }

    return scale;
}

/* ------------------------------------------------------------------ */
/* The result-screen task lifecycle callbacks and small shims.         */
/* ------------------------------------------------------------------ */

/** The three one-word predicates reduced to `return 1`. */
s32 func_ov039_020921f4(void) {
    return 1;
}

s32 func_ov039_02092bf0(void) {
    return 1;
}

s32 func_ov039_02096d98(void) {
    return 1;
}

/** Releases the sprite at task+0x18 and reports success. */
s32 func_ov039_02091aa4(void* pool, void* task) {
    (void)pool;
    Sprite_Release(*(Sprite**)((u8*)task + 0x18));
    return 1;
}

/** out = a + b, component-wise. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

/** True when +0x0C is clear and +0x10 is set. */
s32 func_ov039_02098b44(void* task) {
    if (*(s32*)((u8*)task + 0x0C) == 0 && *(s32*)((u8*)task + 0x10) != 0) {
        return 1;
    }
    return 0;
}

/** A four-entry handler table copied to the stack before one is called. */
extern const TaskStages data_ov039_02099c70;

/**
 * @brief Runs the stage handler at `index` from the table.
 *
 * The table is copied onto the stack first (the `ldm`/`stm` pair) and only then
 * indexed, which is why it is a struct copy rather than four assigns: mwcc
 * emits the copy for the whole struct and a plain `ldr` of the selected entry.
 */
void func_ov039_02094188(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099b5c;

    table.iter[index](a, b, c);
}

void func_ov039_02094e10(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099c70;

    table.iter[index](a, b, c);
}

/**
 * @brief Calls the +0x04 handler's slot at +0x08 through the object at +0x08.
 *
 * `scene` is passed straight through as the handler's only argument.
 */
void func_ov039_02098acc(void* task, void* scene) {
    u8* self = (u8*)task;
    u8* obj  = *(u8**)(self + 0x04);
    void (**table)(void*);

    if (obj == NULL) {
        return;
    }

    table = *(void (***)(void*))(obj + 0x08);
    table[*(s32*)(self + 0x08)](scene);
}

/** Spawns the one-word argument task table data_ov039_0209a06c. */
s32 func_ov039_020989f0(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_0209a06c, NULL, 0, NULL, &args);
}

/** Spawns the task table data_ov039_02099c64 with one word of args. */
s32 func_ov039_02094e58(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_02099c64, NULL, 0, NULL, &args);
}

/** Spawns the task table data_ov039_02099b50 with two words of args. */
s32 func_ov039_020941d0(TaskPool* pool, s32 a, s32 b) {
    s32 args[2];

    args[0] = a;
    args[1] = b;
    return EasyTask_CreateTask(pool, &data_ov039_02099b50, NULL, 0, NULL, args);
}

/** A 0x28-strided flag slot: only its first word is touched. */
typedef struct {
    /* 0x00 */ s32 flags;
    /* 0x04 */ u8  pad_04[0x24];
} OtuFlagSlot;

/** Raises bit 1 in two +0x28-strided flag words from +0x30 on. */
void func_ov039_02092e04(void* task, s32 enable) {
    s32          i;
    OtuFlagSlot* p;

    if (enable == 0) {
        return;
    }

    p = (OtuFlagSlot*)((u8*)task + 0x30);
    for (i = 0; i < 2; i++) {
        p->flags |= 2;
        p++;
    }
}

/** Releases the three sprites at sprite+4, 0x40 apart. */
s32 func_ov039_02094158(void* pool, void* task) {
    s32    i;
    Sprite* p = (Sprite*)((u8*)*(void**)((u8*)task + 0x18) + 4);

    (void)pool;

    for (i = 0; i < 3; i++) {
        Sprite_Release(p);
        p = (Sprite*)((u8*)p + 0x40);
    }
    return 1;
}

/** Renders the three sprites at sprite+4, 0x40 apart, when +0xC8 is set. */
s32 func_ov039_0209411c(void* pool, void* task) {
    u8*     sprite = *(u8**)((u8*)task + 0x18);
    s32     i;
    Sprite* p;

    (void)pool;

    if (*(s32*)(sprite + 0xC8) != 0) {
        p = (Sprite*)(sprite + 4);
        for (i = 0; i < 3; i++) {
            Sprite_RenderFrame(p);
            p = (Sprite*)((u8*)p + 0x40);
        }
    }
    return 1;
}

/** Steps and renders the sprite block at task+0x18 when +0x78 is set. */
s32 func_ov039_02094dac(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x78) != 0) {
        *(s16*)((u8*)sprite + 0x0C) =
            (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x60) +
                   (*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54))) >>
                  12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** The three-word point-and-scale record `func_ov039_02091628` fills in. */
typedef struct {
    /* 0x00 */ s32 x;
    /* 0x04 */ s32 y;
    /* 0x08 */ s32 scale;
} OtuPoint3;

/**
 * @brief Reads the +0x5C/+0x60 pair into a point when the +0x80 phase is 3.
 *
 * Returns 0 otherwise, leaving the point untouched.
 */
s32 func_ov039_02091628(void* task, OtuPoint3* out) {
    u8* self = (u8*)task;
    s32 y;
    s32 x;

    if (*(s32*)(self + 0x80) != 3) {
        return 0;
    }

    y = *(s32*)(self + 0x60);
    x = *(s32*)(self + 0x5C);

    out->x     = x;
    out->y     = y;
    out->scale = 0x20000;
    return 1;
}

/** The child-task group stepped by `func_ov039_02091690`. */
typedef struct {
    /* 0x00 */ TaskPool*     pool;
    /* 0x04 */ u8            pad_04[0x58];
    /* 0x5C */ OtuTaskParams params;
    /* 0x64 */ u8            pad_64[0x20];
    /* 0x84 */ s32           ids[8];
} OtuChildGroup;

/** Advances the eight child tasks named by +0x84. */
void func_ov039_02091690(OtuChildGroup* self) {
    s32 i;

    for (i = 0; i < 8; i++) {
        func_ov039_02097e9c(EasyTask_GetTaskData(self->pool, self->ids[i]),
                            &self->params);
    }
}

/**
 * @brief The wireless board's per-pad step: advance this child's phase when the
 *        pad has been tapped.
 *
 * The record's +0x10 word is a cursor into the round's phases and +0x14 is the
 * phase the pad must already be on; the store increments the record's counter
 * at +6 and calls the ov040 stepper with the "still in range" flag.
 */
s32 func_ov039_020885f4(void* record, TinPinSlammer_Scene* scene) {
    u8* self   = (u8*)record;
    u8* target = (u8*)func_ov039_02098b70(OTU_STAGE(scene));
    s32 flag;

    if (*(u16*)(self + 0x10) >= *(s32*)(target + 0x140)) {
        return 0;
    }

    if (*(u16*)(self + 0x14) == *(u16*)(target + 6)) {
        (*(u16*)(target + 6))++;

        if (*(s32*)(target + 6) < *(s32*)(target + 0x140)) {
            flag = 1;
        } else {
            flag = 0;
        }

        func_ov040_0209d788(0, 0, target + 6, 2, flag);
        return 1;
    }

    return 0;
}
