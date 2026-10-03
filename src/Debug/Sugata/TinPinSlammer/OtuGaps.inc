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
extern const TaskHandle data_ov039_020995a4;
extern const TaskHandle data_ov039_020995ec;
extern const TaskHandle data_ov039_02099998;

/* The stage tables the four-word dispatchers copy and index. */
extern const TaskStages data_ov039_02099568;
extern const TaskStages data_ov039_020995b0;
extern const TaskStages data_ov039_02099610;
extern const TaskStages data_ov039_020999a4;
extern const TaskStages data_ov039_0209a030;
extern const TaskStages data_ov039_0209a078;

/*
 * The four sprite-load helpers the parameter initialisers hand the block to.
 * All of them are the OtuTaskStages.inc loader shape and all of them take the
 * caller's `args` straight through in r2 -- which is why the callers never
 * reload it, and why the constant-zero register in the callers moves off r2.
 */
void func_ov039_02091788(void* self, void* sprite, s32* args);
void func_ov039_02094bac(void* self, void* sprite, s32* args);
void func_ov039_020911dc(void* self, void* sprite, s32* args);
void func_ov039_020980b8(void* self, void* sprite, s32* args);

extern const TaskHandle     data_ov039_0209955c;
extern const SpriteAnimation data_ov039_0209a088;
extern s32*                  data_ov039_0209a620[];

/* The child-tile setup helper func_ov039_020929ec drives. */
void func_ov039_0209276c(void* self, s32 paramsAddr);

/* The 7- and 4-argument cell set-up pair func_ov039_0209276c drives. */
void func_0200d898(void* buf, void* src, s32 w, s32 h);
void func_0200d1d8(void* obj, s32 a, s32 b, s32 c, void* buf, s32 w, s32 h);
void func_0200d858(void* obj, s32 a, s32 b, s32 c);

/* The per-child step func_ov039_02091070 runs four times. */
void func_ov039_020983c8(void* data, OtuPoint* pt, void* table, s32 index);

/* The two more func_ov039_0208e9f8 dispatches to, alongside +0x1628. */
s32  func_ov039_02090e9c(OtuHammer* self, OtuBar* out);
void func_ov039_0208fee0(OtuShadowTask* self, OtuPinRecord* out);

/* The three Q12.12 constants func_ov039_02094ca8 damps with. */
extern s32 data_ov039_0209a300;
extern s32 data_ov039_0209a304;
extern s32 data_ov039_0209a310;

/* The four more func_ov039_02094e9c randomises its launch between. */
extern s32 data_ov039_0209a2fc;
extern s32 data_ov039_0209a30c;
extern s32 data_ov039_0209a314;
extern s32 data_ov039_0209a328;

/** The 4-word template func_ov039_020983c8 copies onto the sprite block. */
typedef struct {
    /* 0x00 */ s32 w0;
    /* 0x04 */ s32 w1;
    /* 0x08 */ s32 w2;
    /* 0x0C */ s32 w3;
} OtuWord4; // Size: 0x10
extern const SpriteAnimation data_ov039_020995c0;
extern const SpriteAnimation data_ov039_02099578;
extern const SpriteAnimation data_ov039_02099c80;
extern const SpriteAnimation data_ov039_0209a040;
extern const SpriteAnimation data_ov039_02099504;
extern const SpriteAnimation data_ov039_02099530;

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

/* ------------------------------------------------------------------ */
/* The six-word record reset, the sprite step pair and the child       */
/* teardown.                                                           */
/* ------------------------------------------------------------------ */

/** The flat six-word record `func_ov039_02098a20` zeroes out. */
typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 unk_14;
} OtuRec6;

/**
 * @brief Stores `owner` in the first word and clears the other five.
 *
 * The zero stores are not in address order and neither a designator list nor a
 * memset reproduces the target's 0/4/C/10/8/14 sequence, so the six writes are
 * spelled out in the order the target emits them.
 */
void func_ov039_02098a20(void* a, void* owner) {
    OtuRec6* rec = (OtuRec6*)a;

    rec->unk_00 = (s32)owner;
    rec->unk_04 = 0;
    rec->unk_0C = 0;
    rec->unk_10 = 0;
    rec->unk_08 = 0;
    rec->unk_14 = 0;
}

/** Steps the sprite block at task+0x18 when +0x44 is set. */
s32 func_ov039_020988d8(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x44) != 0) {
        Sprite_Update((Sprite*)(sprite + 4));
    }
    return 1;
}

/** Renders the sprite block at task+0x18 when +0x44 is set. */
s32 func_ov039_020988fc(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x44) != 0) {
        Sprite_RenderFrame((Sprite*)(sprite + 4));
    }
    return 1;
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x6C is set.
 *
 * The same pair of Q12.12 position writes as `func_ov039_02094dac`, minus that
 * function's +0x60 term and against a different guard word.
 */
s32 func_ov039_02091a5c(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x6C) != 0) {
        *(s16*)((u8*)sprite + 0x0C) =
            (s16)((*(s32*)((u8*)sprite + 0x58) - *(s32*)((u8*)sprite + 0x50)) >> 12);
        *(s16*)((u8*)sprite + 0x0E) =
            (s16)((*(s32*)((u8*)sprite + 0x5C) - *(s32*)((u8*)sprite + 0x54)) >> 12);
        Sprite_RenderFrame(sprite);
    }
    return 1;
}

/** Releases the block's sprite and deletes its eight child tasks. */
s32 func_ov039_02091524(TaskPool* pool, void* task) {
    OtuChildGroup* self = *(OtuChildGroup**)((u8*)task + 0x18);
    s32            i;

    Sprite_Release((Sprite*)((u8*)self + 4));

    for (i = 0; i < 8; i++) {
        EasyTask_DeleteTask(pool, self->ids[i]);
    }
    return 1;
}

/** The two-argument result-screen spawners: one word of dataType, one of id. */
s32 func_ov039_02091b00(TaskPool* pool, s32 dataType, s32 childId) {
    s32 args[2];

    args[0] = dataType;
    args[1] = childId;
    return EasyTask_CreateTask(pool, &data_ov039_020995a4, NULL, 0, NULL, args);
}

/** The board's three-word spawner. */
s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap,
                        OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_020995ec, NULL, 0, NULL, args);
}

/** The board's second three-word spawner, against its own task table. */
s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap,
                        OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_02099998, NULL, 0, NULL, args);
}

/**
 * @brief Runs the stage handler at `index` from the table at data_ov039_02099568.
 *
 * Same stack copy and index as `func_ov039_02094188`, against a different table.
 */
void func_ov039_02091560(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099568;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_020995b0. */
void func_ov039_02091ab8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020995b0;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_02099610. */
void func_ov039_020922c8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_02099610;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_020999a4. */
void func_ov039_02092c8c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020999a4;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_0209a030. */
void func_ov039_0209834c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a030;

    table.iter[index](a, b, c);
}

/** The same dispatcher against data_ov039_0209a078. */
void func_ov039_020989a8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a078;

    table.iter[index](a, b, c);
}

/**
 * @brief Steps the sprite block at task+0x18 when +0x80 is set.
 *
 * `func_ov039_02094dac` again, one guard word further along.
 */
s32 func_ov039_020982e8(void* pool, void* task) {
    Sprite* sprite = *(Sprite**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)((u8*)sprite + 0x80) != 0) {
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

/**
 * @brief The same two Q12.12 position writes against a shifted field set.
 *
 * The x half reads the +0x5C/+0x54 pair and the y half adds a third term at
 * +0x64 before subtracting +0x58; the block is rendered at +4 rather than at
 * its base, which is the one place it differs from `func_ov039_020982e8`.
 */
s32 func_ov039_020914d0(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    if (*(s32*)(sprite + 0x6C) != 0) {
        *(s16*)(sprite + 0x10) =
            (s16)((*(s32*)(sprite + 0x5C) - *(s32*)(sprite + 0x54)) >> 12);
        *(s16*)(sprite + 0x12) =
            (s16)((*(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x64) -
                   *(s32*)(sprite + 0x58)) >>
                  12);
        Sprite_RenderFrame((Sprite*)(sprite + 4));
    }
    return 1;
}

/**
 * @brief Clears the parameter block at task+0x18 and re-derives it.
 *
 * The store order is not address order: the four zero words at +0x50 come after
 * the one at +0x68, and +0x40 comes last. Written in the target's order because
 * nothing about the sequence is recoverable from the field names.
 */
s32 func_ov039_02091814(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x68) = args[1];
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;
    *(s32*)(sprite + 0x6C) = 0;
    *(s32*)(sprite + 0x70) = 0;

    func_ov039_02091788(sprite, sprite, args);
    return 1;
}

/** The same reset against the sibling helper, over a longer zero run. */
s32 func_ov039_02094c50(void* pool, void* task, s32* args) {
    u8* sprite = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(sprite + 0x74) = 0;
    *(s32*)(sprite + 0x78) = 0;
    *(s32*)(sprite + 0x50) = 0;
    *(s32*)(sprite + 0x54) = 0;
    *(s32*)(sprite + 0x58) = 0;
    *(s32*)(sprite + 0x5C) = 0;
    *(s32*)(sprite + 0x60) = 0;
    *(s32*)(sprite + 0x64) = 0;
    *(s32*)(sprite + 0x68) = 0;
    *(s32*)(sprite + 0x40) = 0;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s16*)(sprite + 0x4C) = 0;
    *(s16*)(sprite + 0x4E) = 0;

    func_ov039_02094bac(sprite, sprite, args);
    return 1;
}

/**
 * @brief Stamps a 4x4 block of tile numbers into a 32-wide map.
 *
 * Each row is 32 entries apart, so the value advances by 0x20 per row while
 * only the first four entries of each are written. `value` is a u16 and is
 * narrowed after every increment, which is why the loop reads as
 * `add`/`lsl`/`lsr` rather than a plain `add`.
 */
void func_ov039_02091b40(u16 start, u16* map) {
    u16 value = start;
    s32 col;
    s32 row;

    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            map[row * 0x20 + col] = value;
            value++;
        }
        value += 0x1C;
    }
}

/**
 * @brief Runs the +0x04 object's +0x04 handler on `arg`, then frees the block's
 * allocation.
 *
 * The two conditions are one `&&` chain, not two `if`s: the target guards the
 * handler load with `ldrne` so the pointer is loaded only when the object is
 * present, and it is then called from the loaded register rather than reloaded.
 * The assignment is written inside the condition on purpose, because splitting
 * it into a separate statement turns the `cmpne` pair into two branches.
 *
 * The free is `Mem_Free(heap, ptr)`: the +0x14 word is both the guard and the
 * pointer handed in, which is why it is loaded into a local before the test.
 */
void func_ov039_02098af4(void* task, void* arg) {
    u8*   self = (u8*)task;
    u8*   obj;
    void* ptr;
    void (*fn)(void*);

    if (*(s32*)(self + 0x10) == 0) {
        return;
    }

    obj = *(u8**)(self + 0x04);
    if (obj != NULL && (fn = *(void (**)(void*))(obj + 0x04)) != NULL) {
        fn(arg);
    }

    ptr = *(void**)(self + 0x14);
    if (ptr == NULL) {
        return;
    }

    Mem_Free(*(Heap**)(self + 0x00), ptr);
    *(s32*)(self + 0x14) = 0;
}

/**
 * @brief Resets the block at task+0x18 and re-derives it through +0x11dc.
 *
 * The first store is the *pool*, not a field of the block's own state: the
 * block's word 0 is the heap it will later allocate out of. The zero and
 * 0x1000 runs are written in the target's order rather than by address.
 */
s32 func_ov039_02091268(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    *(TaskPool**)(block + 0x00) = pool;
    *(s32*)(block + 0x6C) = 0;
    *(s32*)(block + 0x68) = args[1];
    *(s32*)(block + 0x54) = 0;
    *(s32*)(block + 0x58) = 0;
    *(s32*)(block + 0x5C) = 0;
    *(s32*)(block + 0x60) = 0;
    *(s32*)(block + 0x64) = 0;
    *(s32*)(block + 0x44) = 0;
    *(s32*)(block + 0x48) = 0x1000;
    *(s32*)(block + 0x4C) = 0x1000;
    *(s16*)(block + 0x50) = 0;
    *(s16*)(block + 0x52) = 0;
    *(s32*)(block + 0x80) = 0;

    func_ov039_020911dc(block, block + 4, args);
    return 1;
}

/** The same reset against the +0x80b8 helper, over a longer zero run. */
s32 func_ov039_02098144(TaskPool* pool, void* task, s32* args) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    *(s32*)(block + 0x7C) = 0;
    *(s32*)(block + 0x80) = 0;
    *(s32*)(block + 0x78) = args[1];
    *(s32*)(block + 0x50) = 0;
    *(s32*)(block + 0x54) = 0;
    *(s32*)(block + 0x58) = 0;
    *(s32*)(block + 0x5C) = 0;
    *(s32*)(block + 0x60) = 0;
    *(s32*)(block + 0x64) = 0;
    *(s32*)(block + 0x68) = 0;
    *(s32*)(block + 0x40) = 0;
    *(s32*)(block + 0x44) = 0x2000;
    *(s32*)(block + 0x48) = 0x2000;
    *(s16*)(block + 0x4C) = 0;
    *(s16*)(block + 0x4E) = 0;

    func_ov039_020980b8(block, block, args);
    return 1;
}

/** Loads the sprite for the task whose anim template is data_ov039_0209a088. */
void func_ov039_020985e0(void* self, void* sprite) {
    SpriteAnimation anim = data_ov039_0209a088;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)self;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * @brief Runs the group's stage at +0x10/+0x14 and records a position pair.
 *
 * The group is an element of `g_DisplaySettings.engineState`, sized 0x220,
 * which is what folds the +0x64 addend into the pool word: spelling the base as
 * a raw `+ 0x64` leaves mwcc materialising `g_DisplaySettings` and then adding
 * 0x64 in a register. The stage kind is the first word of `bgSettings[b]` and is
 * a real `switch` -- the target builds a six-entry jump table rather than
 * testing the values, so an `if` chain is not interchangeable here. Cases 1-5
 * share one body and case 0 falls through to `default`, which is why the
 * table's first entry lands on the same label as the out-of-range branch.
 *
 * The three writes land on `bgAffines[b].unk_14` and the `bgOffsets[b]` pair.
 */
// Nonmatching: 98.5%, and the complete gap is one register rename. The target
// keeps the engine-state index in r4 and the group pointer in lr; this source
// has them the other way round, so every `mla`/`add` reads `r4` where the
// target reads `lr`. mwcc hoists the pool-constant load to the top of the
// block, which is what makes the group pointer win r4; the target loads the
// index first. Ten shapes were tried -- all six declaration orders, no named
// group local at all (leaving the address to CSE), array decay instead of
// &array[i], a named base local, and both load orders -- and four of them score
// identically here while the rest are worse. Every other instruction agrees.
void func_ov039_02092348(void* self, s32 x, s32 y) {
    s32 a = *(s32*)((u8*)self + 0x10);
    s32 b = *(s32*)((u8*)self + 0x14);
    DisplayEngineState* group = g_DisplaySettings.engineState + a;
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }

    group->bgOffsets[b].hOffset = x;
    group->bgOffsets[b].vOffset = y;
}

/**
 * @brief Promotes the +0x0C word into +0x04, optionally allocates, then calls
 *        the +0x04 object's first handler.
 *
 * The allocation is sized by the +0x0C word of the object that was just moved
 * into +0x04, which is why that word is tested before the call. The handler is
 * one `&&` chain for the same reason as `func_ov039_02098af4`.
 */
void func_ov039_02098a60(void* task, void* arg) {
    u8*   self = (u8*)task;
    u8*   obj;
    s32   size;
    void (*fn)(void*);

    if (*(s32*)(self + 0x10) == 0) {
        return;
    }

    *(s32*)(self + 0x04) = *(s32*)(self + 0x0C);
    *(s32*)(self + 0x08) = 0;
    *(s32*)(self + 0x0C) = 0;
    *(s32*)(self + 0x10) = 0;

    obj  = *(u8**)(self + 0x04);
    size = *(s32*)(obj + 0x0C);
    if (size != 0) {
        *(void**)(self + 0x14) = Mem_AllocHeapTail(*(Heap**)(self + 0x00), size);
    }

    obj = *(u8**)(self + 0x04);
    if (obj != NULL && (fn = *(void (**)(void*))(obj + 0x00)) != NULL) {
        fn(arg);
    }
}

/**
 * @brief Spawns the group task and its eight children, returning the handle.
 *
 * The children are named into the group's +0x84 array, which is the same
 * `OtuChildGroup` layout `func_ov039_02091524` and `func_ov039_02091690` walk.
 */
s32 func_ov039_020915a8(TaskPool* pool, s32 word0, s32 word1) {
    OtuTaskParams  args;
    s32            handle;
    OtuChildGroup* group;
    s32            i;

    args.word0 = word0;
    args.word1 = word1;

    handle = EasyTask_CreateTask(pool, &data_ov039_0209955c, NULL, 0, NULL, &args);
    group  = EasyTask_GetTaskData(pool, handle);

    for (i = 0; i < 8; i++) {
        group->ids[i] = func_ov039_02097e68(pool, word0, word1);
    }
    return handle;
}

/**
 * The two loaders that share `func_ov039_020985e0`'s shape and are re-run by the
 * parameter resets above. Both take the caller's `args` in the third slot and
 * read the dataType out of its first word.
 */

/** Loads the sprite for the task whose anim template is data_ov039_020995c0. */
void func_ov039_02091788(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_020995c0;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099578. */
void func_ov039_020911dc(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099578;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x5C) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x60) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_0209a040. */
void func_ov039_020980b8(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_0209a040;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX     = *(s32*)((u8*)self + 0x58) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0x5C) >> 12;
    _Sprite_Load((Sprite*)sprite, &anim);
}

/**
 * Loads the sprite for the task whose anim template is data_ov039_02099c80.
 *
 * The only member of the family whose position is a difference rather than a
 * raw value: x is `+0x58 - +0x50` and y adds a third term at +0x60, the same
 * pair `func_ov039_02094dac` writes into the sprite's posX/posY.
 */
void func_ov039_02094bac(void* self, void* sprite, s32* args) {
    SpriteAnimation anim = data_ov039_02099c80;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)args;
    anim.posX = (s32)((*(s32*)((u8*)self + 0x58) - *(s32*)((u8*)self + 0x50)) >> 12);
    anim.posY = (s32)((*(s32*)((u8*)self + 0x60) +
                       (*(s32*)((u8*)self + 0x5C) - *(s32*)((u8*)self + 0x54))) >>
                      12);
    _Sprite_Load((Sprite*)sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099504. */
void func_ov039_02090390(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099504;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xAC) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB0) >> 12;
    _Sprite_Load(sprite, &anim);
}

/** Loads the sprite for the task whose anim template is data_ov039_02099530. */
void func_ov039_0209041c(OtuHammer* self, Sprite* sprite, OtuHammer_Params* params) {
    SpriteAnimation anim = data_ov039_02099530;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)params;
    anim.posX     = *(s32*)((u8*)self + 0xB4) >> 12;
    anim.posY     = *(s32*)((u8*)self + 0xB8) >> 12;
    _Sprite_Load(sprite, &anim);
}

/* ------------------------------------------------------------------ */
/* The resource teardown pair and the palette-source installers.       */
/* ------------------------------------------------------------------ */

/**
 * @brief Releases every screen, char, palette and data handle the block holds.
 *
 * The six manager calls run in pairs per display: screen, char, then palette,
 * with the palette manager indexed separately from the two Bg resource
 * managers. The final pair releases the data handle at +0x48 and the sprite
 * block at +4.
 */
s32 func_ov039_02098920(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    BgResMgr_ReleaseScreen(g_BgResourceManagers[0], *(BgResource**)(block + 0x54));
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x50));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0],
                               *(PaletteResource**)(block + 0x4C));

    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], *(BgResource**)(block + 0x60));
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], *(BgResource**)(block + 0x5C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[1],
                               *(PaletteResource**)(block + 0x58));

    DatMgr_ReleaseData(*(Data**)(block + 0x48));
    Sprite_Release((Sprite*)(block + 4));
    return 1;
}

/** The two-wide background slot group torn down by `func_ov039_02092bf8`. */
typedef struct {
    /* 0x00 */ u8               pad_00[0x04];
    /* 0x04 */ Heap*            heap;
    /* 0x08 */ s32              flag;
    /* 0x0C */ u8               pad_0C[0x04];
    /* 0x10 */ Data*            dataIds[2];
    /* 0x18 */ PaletteResource* paletteIds[2];
    /* 0x20 */ BgResource*      charIds[2];
    /* 0x28 */ u8               pad_28[0x50];
    /* 0x78 */ void*            buffers[2];
} OtuBgGroup; // Size: 0x80

/**
 * @brief The two-wide version of the same teardown, plus a slot free.
 *
 * Returns early when +0x08 is clear, and that early `return 1` is what lets
 * mwcc if-convert the guard into `moveq`/`popeq` -- the sibling
 * `func_ov039_0209411c` had to spell its guard the other way round to stop it.
 */
s32 func_ov039_02092bf8(void* pool, void* task) {
    OtuBgGroup* group = *(OtuBgGroup**)((u8*)task + 0x18);
    s32         i;

    (void)pool;

    if (group->flag == 0) {
        return 1;
    }

    func_0200d954(0, 2);
    func_0200d954(0, 3);

    for (i = 0; i < 2; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], group->charIds[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], group->paletteIds[i]);
        Mem_Free(group->heap, group->buffers[i]);
        DatMgr_ReleaseData(group->dataIds[i]);
    }
    return 1;
}

/**
 * @brief Points palette manager 0 at five sources, one per 0x14-strided slot.
 *
 * Each slot is a frame cursor at +0 and a palette resource at +0x10; the cursor
 * is stepped by `func_ov039_02098dbc` and its result is the source palette, so
 * the call has to be the third argument expression and not hoisted into a
 * statement of its own.
 */
s32 func_ov039_02092154(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    /* Spelled out five times rather than looped: the target unrolls it. */
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64),
                         (void*)func_ov039_02098dbc(block + 0x54));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78),
                         (void*)func_ov039_02098dbc(block + 0x68));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C),
                         (void*)func_ov039_02098dbc(block + 0x7C));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0),
                         (void*)func_ov039_02098dbc(block + 0x90));
    PaletteMgr_SetSource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4),
                         (void*)func_ov039_02098dbc(block + 0xA4));
    return 1;
}

/**
 * @brief The full resource teardown: two buffers, two chars, six palettes, two
 *        data handles.
 *
 * The six palette releases are the same 0x14-strided slot run
 * `func_ov039_02092154` installs, read back off the block in the order the
 * install wrote them.
 */
s32 func_ov039_020921fc(void* pool, void* task) {
    u8* block = *(u8**)((u8*)task + 0x18);

    (void)pool;

    func_0200d954(0, 1);

    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x38));
    Mem_Free(*(Heap**)(block + 0x04), *(void**)(block + 0x3C));

    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x50));
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], *(BgResource**)(block + 0x48));

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x4C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x64));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x78));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x8C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xA0));
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0xB4));

    DatMgr_ReleaseData(*(Data**)(block + 0x44));
    DatMgr_ReleaseData(*(Data**)(block + 0x40));
    return 1;
}

/** The block `func_ov039_02092d0c` drives two display groups out of. */
typedef struct {
    /* 0x00 */ u8  pad_00[0x08];
    /* 0x08 */ s32 guard;
    /* 0x0C */ u8  pad_0C[0x1C];
    /* 0x28 */ s32 a1;
    /* 0x2C */ s32 b1;
    /* 0x30 */ u8  pad_30[0x20];
    /* 0x50 */ s32 a2;
    /* 0x54 */ s32 b2;
    /* 0x58 */ u8  pad_58[0x38];
    /* 0x90 */ s32 u1;
    /* 0x94 */ s32 u2;
    /* 0x98 */ s32 v1;
    /* 0x9C */ s32 v2;
} OtuBoardPair; // Size: 0xA0

/**
 * @brief Runs `func_ov039_02092348`'s stage step twice, over two field sets.
 *
 * Both halves are the same operation on `g_DisplaySettings.engineState`: pick
 * the group by a 0x220-strided index, switch on the `bgSettings[b].bgMode`
 * first word, raise `bgAffines[b].unk_14` for cases 1-5, and write the position
 * pair into `bgOffsets[b]` -- except that here the pair is an offset added to
 * the incoming x/y rather than a replacement. Spelled out twice rather than
 * factored, because `-inline noauto` would turn a shared helper into a `bl`.
 */
// Nonmatching: 98.5%, and the gap is the same one func_ov039_02092348 has: the
// engine-state index and the `g_DisplaySettings.engineState` base swap r4/r5
// between the two sides. See that marker for the shapes already tried.
void func_ov039_02092d0c(void* task, s32 x, s32 y) {
    OtuBoardPair*       self = (OtuBoardPair*)task;
    DisplayEngineState* group;
    s32                 b;
    s32                 px;
    s32                 py;

    if (self->guard == 0) {
        return;
    }

    b     = self->b1;
    px    = x + self->u1;
    py    = y + self->v1;
    group = &g_DisplaySettings.engineState[self->a1];
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }
    group->bgOffsets[b].hOffset = px;
    group->bgOffsets[b].vOffset = py;

    b     = self->b2;
    px    = x + self->u2;
    py    = y + self->v2;
    group = &g_DisplaySettings.engineState[self->a2];
    switch (group->bgSettings[b].bgMode) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            group->bgAffines[b].unk_14 = 1;
            break;

        default:
            break;
    }
    group->bgOffsets[b].hOffset = px;
    group->bgOffsets[b].vOffset = py;
}
/** The block `func_ov039_020929ec` stores its params into and then sets up. */
typedef struct {
    /* 0x00 */ s32 word0;
    /* 0x04 */ s32 word1;
    /* 0x08 */ s32 active;
    /* 0x0C */ u8* selector;
    /* 0x10 */ u8  pad_10[0x80];
    /* 0x90 */ s32 flagsA[2];
    /* 0x98 */ s32 flagsB[2];
} OtuTileSetup; // Size: 0xA0

/**
 * @brief Stores the three-word params block and runs the tile setup twice.
 *
 * +0x0C is a pointer to a byte that both selects the table in
 * `data_ov039_0209a620` and gates the whole body: when it is zero the block is
 * still written and the function returns, which is the `else` half of the
 * `movne`/`strne`/`bne` sequence rather than an early guard.
 */
s32 func_ov039_020929ec(void* pool, void* task, s32* params) {
    OtuTileSetup* self = *(OtuTileSetup**)((u8*)task + 0x18);
    s32           i;

    (void)pool;

    self->word0    = params[0];
    self->word1    = params[1];
    self->selector = (u8*)params[2];

    if (*self->selector != 0) {
        self->active = 1;
    } else {
        self->active = 0;
        return 1;
    }

    for (i = 0; i < 2; i++) {
        func_ov039_0209276c(self, data_ov039_0209a620[*self->selector][i]);
        self->flagsA[i] = 0;
        self->flagsB[i] = 0;
    }
    return 1;
}

/**
 * @brief The +0x8538 member of the sprite-slot selector family.
 *
 * Byte-for-byte the shape of `func_ov039_0209352c`: the same
 * `&data_0206b408` pointer, the same three flat short-circuit tests over +0x18,
 * +0x1C and +0x16, the same two-step u16 lookup, and `depthKey` set to 3 rather
 * than computed. The tail sets `depthKey` only -- there is no +0x0C write here,
 * which is what separates this one from `func_ov039_0208d2f8`.
 */
// Nonmatching: 88.7%, byte-for-byte the same gap as func_ov039_0209352c, which
// this is a copy of. The target reloads the +0x16 index and the +0x1C table
// pointer after the `slot->unk_04` store (`ldrsh r2, [r0, #0x16]` / `ldr r3,
// [r0, #0x1c]`); this source keeps both in registers across that store, so the
// second half of the two-step lookup reads `r12` where the target reads `r3`.
// Writing the lookup through `t`'s fields rather than the locals does not
// restore the reload either -- the store to `slot` is a global and mwcc has
// already decided it cannot alias `t`.
OtuSpriteSlot* func_ov039_02098538(OtuSpriteTask* t, s32 arg, s32 sel) {
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
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

            if (*(s32*)((u8*)t + 0x18) != 0 && (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0)
            {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->depthKey = 3;
            return slot;
        }

        default:
            return NULL;
    }
}

/** The four-child arc group stepped by `func_ov039_02091070`. */
typedef struct {
    /* 0x00 */ TaskPool* pool;
    /* 0x04 */ u8        pad_04[0xA8];
    /* 0xAC */ OtuPoint  home;
    /* 0xB4 */ u8        pad_B4[0x08];
    /* 0xBC */ OtuPoint  center;
    /* 0xC4 */ u8        pad_C4[0x64];
    /* 0x128 */ s32      ids[4];
} OtuArcGroup; // Size: 0x138

/**
 * @brief Steps the four child tasks named by +0x128 and places them along an arc.
 *
 * Child 0 is placed at the raw `home` pair against the +0x84 table; the other
 * three are placed by difference -- `func_ov039_02098bb0` takes the arc chord
 * between `home` and `center`, then `func_ov039_02098c00` scales it by a quarter
 * of the phase counter and adds it back to `center`, against the +0x94 table.
 * The phase advances 0x1000 per child and is divided by 4 when used, so the
 * scale runs 0, 0x400, 0x800, 0xC00.
 */
// Nonmatching: 89.4%, and the gap is one register's worth of allocation. The
// target gives `table` its own callee-saved register (r7) and so needs six plus
// a `push {r3, ...}` alignment dummy; this source computes `table` straight
// into the call's r2 and needs only five. Every other difference follows from
// that: the whole register naming shifts by one because the values pack
// tighter. Three things were real and are fixed -- the ids load needs a typed
// `ids[4]` at +0x128 or mwcc strength-reduces `i * 4` into a running offset
// instead of the `add rN, rN, rN, lsl #2` the target has; the `/ 4` must be
// taken of `i * 0x1000` rather than of an accumulator, or mwcc folds the divide
// into the induction variable and advances it by 0x400 instead of 0x1000; and
// the home pair has to be loaded before either is stored or the two do not come
// out in the target's order. Declaration order of `table` and of the home pair
// makes no difference.
void func_ov039_02091070(OtuArcGroup* self) {
    OtuPoint pt;
    s32      i;

    for (i = 0; i < 4; i++) {
        void* table;
        void* data = EasyTask_GetTaskData(self->pool, self->ids[i]);

        if (i == 0) {
            s32 hy = self->home.y;
            s32 hx = self->home.x;

            pt.x  = hx;
            pt.y  = hy;
            table = (u8*)self + 0x84;
        } else {
            /* `i * 0x1000` is spelled out rather than kept as an accumulator:
             * mwcc strength-reduces the product into a `+ 0x1000` counter but
             * still emits a real signed divide for the `/ 4`, which is what the
             * target has. An accumulator declared outside the loop gets the
             * divide folded into it and advances by 0x400 instead. */
            s32 phase = i * 0x1000;

            func_ov039_02098bb0(&self->home, &self->center, &pt);
            func_ov039_02098c00(phase / 4, &pt, &self->center, &pt);
            table = (u8*)self + 0x94;
        }

        func_ov039_020983c8(data, &pt, table, i);
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

        func_ov039_02089064(*(TinPinSlammer_Scene**)(self + 0x00),
                            (OtuPoint*)(self + 0x120));
        func_ov039_02087d04(0x34E, (OtuPoint*)(self + 0x120),
                            (OtuPoint*)(self + 0x110));
        return;
    }

    *(s32*)(self + 0x134) = *(s32*)(self + 0x134) + data_ov039_0209a318;

    /* The cell record is reached through a u16 index whose home is itself a
     * pointer at +0x16C, and the record is 0x1C bytes at +0x170. */
    {
        u16 index      = *(u16*)*(s32*)(self + 0x16C);
        u8* slots      = *(u8**)(self + 0x170);
        u8  speedIndex = *(u8*)(slots + index * 0x1C + 4);

        rate = data_ov039_0209a39c[3];
        rate = (s32)(((s64)data_ov039_0209a3e0[speedIndex].speed * rate + 0x800) >>
                     12);
    }

    func_ov039_0208ad2c((OtuPoint*)(self + 0x12C), rate);
    func_ov039_02098b8c((OtuPoint*)(self + 0x120), (OtuPoint*)(self + 0x12C),
                        (OtuPoint*)(self + 0x120));

    v = *(s32*)(self + 0x128) + *(s32*)(self + 0x134);
    *(s32*)(self + 0x128) = v;
    if (v > 0) {
        *(s32*)(self + 0x128) = 0;
        *(s32*)(self + 0xF8)  = 1;
        *(s32*)(self + 0xF4)  = 0;
    }
}

/**
 * @brief The spring step: a scaled value, then a damped velocity and a bounce.
 *
 * +0x80 is a countdown. While it runs, +0x44 and +0x48 are both set to
 * `(count << 13) / +0x7C`, the +0x6C axis is stepped down by
 * `data_ov039_0209a304` and clamped at zero, +0x70 is stepped up by
 * `data_ov039_0209a310`, and `func_ov039_02098c00` integrates the pair at
 * +0x64 into +0x58. The +0x60 accumulator is then moved by +0x70 and, when it
 * overshoots, is clamped and its velocity damped by `-data_ov039_0209a300`.
 */
s32 func_ov039_02094ca8(void* pool, void* task) {
    u8* sprite = *(u8**)((u8*)task + 0x18);
    s32 count;
    s32 scale;
    s32 v;

    (void)pool;

    /* One `return 1` at the end, and the guard arms ordered so the *clear* is
     * the `then`: the target lays that inline as the fall-through and branches
     * to the spring body, which `if (count > 0) { spring } else { clear }`
     * inverts. `func_ov039_020981a4` needed the opposite polarity on its first
     * guard -- in both cases it is the layout, not the meaning, that decides. */
    if (*(s32*)(sprite + 0x74) != 0) {
        count = *(s32*)(sprite + 0x80) - 1;
        *(s32*)(sprite + 0x80) = count;

        if (count <= 0) {
            *(s32*)(sprite + 0x74) = 0;
            *(s32*)(sprite + 0x78) = 0;
        } else {
            scale = (count << 13) / *(s32*)(sprite + 0x7C);
            *(s32*)(sprite + 0x48) = scale;
            *(s32*)(sprite + 0x44) = scale;

            v = *(s32*)(sprite + 0x6C) - data_ov039_0209a304;
            *(s32*)(sprite + 0x6C) = v;
            if (v < 0) {
                *(s32*)(sprite + 0x6C) = 0;
            }

            *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) + data_ov039_0209a310;

            func_ov039_02098c00(*(s32*)(sprite + 0x6C),
                                (OtuPoint*)(sprite + 0x64),
                                (OtuPoint*)(sprite + 0x58),
                                (OtuPoint*)(sprite + 0x58));

            v = *(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x70);
            *(s32*)(sprite + 0x60) = v;
            if (v > 0) {
                *(s32*)(sprite + 0x60) = 0;
                if (*(s32*)(sprite + 0x70) > 0) {
                    *(s32*)(sprite + 0x70) =
                        (s32)(((s64)*(s32*)(sprite + 0x70) *
                               -data_ov039_0209a300 + 0x800) >>
                              12);
                }
            }

            Sprite_Update((Sprite*)sprite);
            *(s32*)(sprite + 0x78) = 1;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The +0x276c resource-allocation setup.                              */
/* ------------------------------------------------------------------ */

/** The 0x28-byte per-slot object func_ov039_0209276c drives. */
typedef struct {
    /* 0x00 */ u8 pad[0x28];
} OtuResSlot; // Size: 0x28

/** The per-slot parameter block func_ov039_0209276c reads. */
typedef struct {
    /* 0x00 */ s32 slot;
    /* 0x04 */ s32 width;
    /* 0x08 */ s32 height;
    /* 0x0C */ s32 fileId;
    /* 0x10 */ s32 group;
    /* 0x14 */ s32 layers;
    /* 0x18 */ s32 priority;
    /* 0x1C */ s32 idx[2];
    /* 0x24 */ s32 pad_24;
    /* 0x28 */ s32 idx1;
    /* 0x2C */ s32 idx2;
    /* 0x30 */ s32 palStart;
    /* 0x34 */ s32 palCount;
} OtuResParams; // Size: 0x38

/** The block func_ov039_0209276c loads and allocates into. */
typedef struct {
    /* 0x00 */ s32               dataType;
    /* 0x04 */ Heap*             heap;
    /* 0x08 */ u8                pad_08[0x04];
    /* 0x0C */ u8*               selector;
    /* 0x10 */ Data*             data[2];
    /* 0x18 */ PaletteResource*  palettes[2];
    /* 0x20 */ BgResource*       chars[2];
    /* 0x28 */ OtuResSlot        slots[2];
    /* 0x78 */ void*             buffers[2];
    /* 0x80 */ s32               widths[2];
    /* 0x88 */ s32               heights[2];
} OtuResGroup; // Size: 0x90

/*
 * A relative-offset lookup: the record's +0x08 word is a base, the table at
 * `base + 0x20` is 8 bytes a row, and each row's first word is a signed offset
 * from `base + 0x20` itself. Spelled through a macro because it is used three
 * times and `-inline noauto` would turn a shared helper into a `bl` -- the
 * target has the four instructions three times over.
 *
 * The `NULL`/non-positive case is the `then`, not the `else`: the target lays
 * that one out inline as the fall-through and branches to the body, so the
 * natural `if (rec && idx > 0) { body } else { NULL }` arm order inverts it.
 */
#define Otu_RES_REF(out, rec, idx)                                         \
    do {                                                                   \
        if ((rec) == NULL || (idx) <= 0) {                                 \
            (out) = NULL;                                                  \
        } else {                                                           \
            u8* _base = *(u8**)((u8*)(rec) + 0x08);                        \
            u8* _tbl  = _base + 0x20;                                      \
            (out)     = _tbl + *(s32*)(_tbl + (idx) * 8);                   \
        }                                                                  \
    } while (0)

/**
 * @brief Loads a slot's raw data, then allocates its palette, chars and buffer.
 *
 * The +0x00 word is the `DatMgr_LoadRawData` dataType, not a pool pointer. The
 * three relative references are the palette source, the char source and the
 * cell source in that order; the char source's first word is split into a
 * 24-bit size and a nibble flag, and the size drops by 4 when that nibble is
 * clear.
 */
// Nonmatching: 86.7%, and the rest is register naming and scheduling rather
// than anything structural. Three things were real and are fixed:
//
//   * `PaletteMgr_AllocPalette`'s `start` is `s16`, so the prototype makes mwcc
//     narrow `palStart` with a `lsl #0x10` the target does not have. The call
//     goes through `PaletteMgr_AllocPaletteNoProto`, which is what that macro
//     in PaletteMgr.h is for.
//   * The record has to be stored into the block and read back out, not kept in
//     a local -- the target reloads `rec` after `self->data[slot] = rec` and the
//     store is what forces it.
//   * The first index is hoisted out of the macro. Its load is independent of
//     the `rec == NULL` test and the target has it above the branch; leaving it
//     inside puts the branch first.
//
// What is left is `selector` in r1 vs r0 and `slot` in r0 vs r2 in the first
// block, plus `mov r3, #0` for the `offset` argument landing elsewhere.
void func_ov039_0209276c(void* selfArg, s32 paramsAddr) {
    OtuResGroup*  self   = (OtuResGroup*)selfArg;
    OtuResParams* params = (OtuResParams*)paramsAddr;
    Data*         rec;
    u8*           ref0;
    u8*           ref1;
    u8*           ref2;
    s32           size;

    /* Written as a store-then-reload rather than kept in a local: the target
     * re-reads the record out of the block after storing it, and the store is
     * what forces that. */
    self->data[params->slot] =
        DatMgr_LoadRawData(self->dataType, NULL, 0,
                           (BinIdentifier*)&data_ov039_0209a0b4[params->fileId]);
    rec = self->data[params->slot];

    /* The first index is hoisted: its load is independent of the `rec == NULL`
     * test and the target has it above the branch, where leaving it inside the
     * macro puts the branch first. */
    {
        s32 idxA = params->idx[*(u8*)(self->selector + 1)];

        Otu_RES_REF(ref0, rec, idxA);
    }
    Otu_RES_REF(ref1, rec, params->idx1);
    Otu_RES_REF(ref2, rec, params->idx2);

    /* `PaletteMgr_AllocPaletteNoProto` because `palStart` is a 32-bit value
     * going into a `s16` parameter: the prototype makes mwcc narrow it with a
     * `lsl #0x10` the target does not have. */
    self->palettes[params->slot] =
        PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], ref0, 0,
                                       params->palStart, params->palCount);

    size = (s32)((*(u32*)ref1) >> 8);
    if ((*(u8*)ref1 & 0xF0) == 0) {
        size -= 4;
    }

    self->chars[params->slot] = BgResMgr_AllocChar32(
        g_BgResourceManagers[0], ref1,
        g_DisplaySettings.engineState[0].bgSettings[params->group].charBase, 0,
        size);

    PaletteMgr_Flush(g_PaletteManagers[0], self->palettes[params->slot]);

    self->buffers[params->slot] =
        Mem_AllocHeapTail(self->heap, params->height * (params->width * 4));

    func_0200d898(self->buffers[params->slot], ref2 + 4, params->width,
                  params->height);

    func_0200d1d8(&self->slots[params->slot], 0, params->group, 0,
                  self->buffers[params->slot], params->width, params->height);

    func_0200d858(&self->slots[params->slot], 1, 1, 0);

    self->widths[params->slot]  = params->width << 20;
    self->heights[params->slot] = params->height << 20;

    if (params->layers >= 0) {
        g_DisplaySettings.controls[0].layers |= params->layers;
    }

    g_DisplaySettings.engineState[0].bgSettings[params->group].priority =
        params->priority;
}

/**
 * @brief Randomises the sprite's launch vector and picks a starting animation.
 *
 * The two velocity components are drawn from opposite ends of their ranges:
 * +0x6C starts at `data_ov039_0209a314` and is pushed *up* by a random span,
 * +0x70 starts at the negation of `data_ov039_0209a2fc` and is pushed *down*.
 * The magnitude at +0x7C/+0x80 is then that second component scaled by 3 and
 * taken as an absolute value -- spelled as the three-way ternary `abs` is a
 * macro for, which is why `FX_Divide` is called three times in the target
 * rather than once and reused.
 */
// Nonmatching: 85.5%, all of it scheduling and register choice at the top of
// the body. The target loads both `at` components before either is stored (x in
// r2, y in r1); this source stores x before it loads y. Hoisting the two reads
// into named locals is the obvious fix and scores *worse* (84%), because it
// also moves the pool-constant loads. Everything from the `data_ov039_0209a314`
// store down agrees instruction for instruction, including the three
// `FX_Divide` calls.
void func_ov039_02094e9c(void* task, OtuPoint* at) {
    u8* sprite = (u8*)task;
    s32 cell;
    s32 mag;

    *(s32*)(sprite + 0x74) = 1;
    *(s32*)(sprite + 0x44) = 0x2000;
    *(s32*)(sprite + 0x48) = 0x2000;
    *(s32*)(sprite + 0x58) = at->x;
    *(s32*)(sprite + 0x5C) = at->y;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) =
        *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) =
        *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG                                                     \
    ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    /* `abs` as the macro it is: the argument is evaluated once for the test and
     * once per arm, so the three-way form is load-bearing. The `< 0` polarity
     * is too -- the target lays the negating arm out inline and branches to the
     * plain one (`bpl`), and the `>= 0` spelling inverts that. */
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x7C) = mag;
    *(s32*)(sprite + 0x80) = mag;

    /* The sin/cos table is s32[] but holds pairs of s16, so each half has to be
     * fetched through a s16* at a doubled byte offset. */
    cell = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18),
                           *(s32*)(sprite + 0x1C), (s16)(RNG_Next(3) + 1));
}

/**
 * @brief `func_ov039_02094e9c`'s child variant: same launch, plus a template
 *        copy and a different animation pick.
 *
 * Three differences from the parent. A 4-word template is copied onto the
 * sprite block at +0x40 where the parent writes two fixed 0x2000 halves; the
 * magnitude lands at +0x84/+0x88 rather than +0x7C/+0x80; and the animation
 * index is `index == 0 ? 2 : 1` rather than a fresh `RNG_Next(3) + 1`. The
 * +0x74 word is a s16 and is `RNG_Next(0x800) + 0x800`.
 */
// Nonmatching: 85.4%. Two differences left. The target keeps `at`'s halves as
// two separate `ldr`s (x in r2, y in r1); this source merges them into
// `ldmia lr, {r1, r2}`, which also swaps which half lands in which register.
// Reading them straight into the stores instead interleaves the writes and
// scores 84.3%, and hoisting them into locals is what got this to 85.4% -- the
// merge is a load-pair decision made against this frame's register pressure and
// there is no source shape here that stops it. Everything from the +0x6C store
// down agrees instruction for instruction.
void func_ov039_020983c8(void* task, OtuPoint* at, void* table, s32 index) {
    u8* sprite = (u8*)task;
    s32 ax;
    s32 ay;
    s32 cell;
    s32 mag;

    *(s32*)(sprite + 0x7C) = 1;
    *(OtuWord4*)(sprite + 0x40) = *(OtuWord4*)table;

    /* Both halves are read before either is written: the target keeps x in r2
     * and y in r1 across the pair of stores. */
    ax = at->x;
    ay = at->y;

    *(s32*)(sprite + 0x58) = ax;
    *(s32*)(sprite + 0x5C) = ay;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) =
        *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) =
        *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG                                                     \
    ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x84) = mag;
    *(s32*)(sprite + 0x88) = mag;

    cell = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    *(s16*)(sprite + 0x74) = RNG_Next(0x800) + 0x800;

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18),
                           *(s32*)(sprite + 0x1C),
                           (s16)(index == 0 ? 2 : 1));
}

/**
 * @brief Advances up to four counters and raises a flag when each wraps.
 *
 * The selector byte at +0x0C picks which half runs: case 1 advances the +0x94
 * pair against the +0x84/+0x8C limits and flags +0x58, case 2 does that *and*
 * the +0x90 pair against +0x80/+0x88 with a smaller step flagging +0x30. Each
 * counter is compared against its limit and, on reaching it, taken modulo it --
 * `_s32_div_f` leaves the remainder in r1, which is what is stored back.
 */
s32 func_ov039_02092a78(void* pool, void* task) {
    u8* self = *(u8**)((u8*)task + 0x18);
    s32 limit;
    s32 v;

    (void)pool;

    if (*(s32*)(self + 0x08) == 0) {
        return 1;
    }

    switch (*(u8*)*(s32*)(self + 0x0C)) {
        case 1:
            v = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit = *(s32*)(self + 0x8C);
            if (v >= limit) {
                *(s32*)(self + 0x9C) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }
            break;

        case 2:
            v = *(s32*)(self + 0x90) + 0x333;
            *(s32*)(self + 0x90) = v;
            limit = *(s32*)(self + 0x80);
            if (v >= limit) {
                *(s32*)(self + 0x90) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v = *(s32*)(self + 0x98) + 0x333;
            *(s32*)(self + 0x98) = v;
            limit = *(s32*)(self + 0x88);
            if (v >= limit) {
                *(s32*)(self + 0x98) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit = *(s32*)(self + 0x8C);
            if (v >= limit) {
                *(s32*)(self + 0x9C) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }
            break;

        default:
            break;
    }
    return 1;
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
s32 func_ov039_0208e9f8(TaskPool* pool, void* task) {
    u8* self = (u8*)task;
    s32 claimed = 0;
    s32 ret;

    switch (*(s32*)(self + 0xF8)) {
        case 6:
            ret = func_ov039_02091628(
                EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E8)),
                (OtuPoint3*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 7:
            ret = func_ov039_02090e9c(
                EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E4)),
                (OtuBar*)(self + 0x184));
            *(s32*)(self + 0x180) = ret;
            if (ret > 0) {
                claimed = 1;
            }
            break;

        case 8:
            /* Two sequential guards with their own `break`, not one combined
             * condition: `&&`, nested `if`s and a negated `||` all fold the
             * pair into `ldreq`/`cmpeq`, where the target branches past the
             * body twice. */
            if (*(s32*)(self + 0xFC) != 5) {
                break;
            }
            if (*(s32*)(self + 0x100) != 0x27) {
                break;
            }

            claimed = 1;
            func_ov039_0208fee0(
                EasyTask_GetTaskData(pool, *(s32*)(self + 0x1E0)),
                (OtuPinRecord*)(self + 0x184));
            *(s32*)(self + 0x180) = claimed;
            break;

        default:
            break;
    }
    return claimed;
}

/**
 * @brief The +0x8f7b4 member of the sprite-slot selector family.
 *
 * Same `&data_0206b408` pointer, same three flat short-circuit tests and same
 * two-step u16 lookup as `func_ov039_0209352c`. What separates it is the tail:
 * `depthKey` is derived from the +0x24 object's +0x4C word, masked to 0x7FF and
 * shifted back up by 12 with a +1 bias -- a Q12.12 value rounded up to the next
 * whole step. That tail runs whether or not the lookup succeeded, which is why
 * it sits outside the guard.
 */
// Nonmatching: 84.3%, and it carries two of the family's known gaps at once.
// `table` lands in lr here rather than r12, which forces a `stmdb`/`ldmia`
// prologue the target does not have and shifts the index into r0; and the
// lookup keeps +0x16/+0x1C in registers across the `slot->unk_04` store where
// the target reloads both, the same thing that holds func_ov039_02098538 and
// func_ov039_0209352c at 88.7%. Swapping the `obj`/`slot` declarations was
// worth 10% (74.4% -> 84.3%) because it moved `slot` onto r3 and `obj` onto r1;
// swapping `index`/`table` does nothing.
OtuSpriteSlot* func_ov039_0208f7b4(OtuSpriteTask* t, s32 arg, s32 sel) {
    u8*            obj  = *(u8**)((u8*)t + 0x24);
    OtuSpriteSlot* slot = (OtuSpriteSlot*)&data_0206b408;

    switch (sel) {
        case 1:
            slot->unk_00 = 1;
            return slot;

        case 2: {
            u8* table;
            s32 index;

            slot->unk_04   = 0;
            slot->unk_08   = 0;
            slot->unk_0C   = 0;
            slot->depthKey = -1;

            if (*(s32*)((u8*)t + 0x18) != 0 &&
                (table = *(u8**)((u8*)t + 0x1C)) != NULL &&
                (index = *(s16*)((u8*)t + 0x16)) >= 0) {
                slot->unk_04 = ((u16*)table)[index * 4 + 1];
                slot->unk_08 = (s32)(u8*)(table + ((u16*)table)[index * 4] * 2);
            }

            slot->depthKey = (((*(s32*)(obj + 0x4C) >> 12) & 0x7FF) + 1) << 12;
            return slot;
        }

        default:
            return NULL;
    }
}

/**
 * @brief Builds a 2x2 tile bitmask grid over the +0x04 object's cell array.
 *
 * The +0x04 object is a width/height header at +2/+3 with a cell array pointer
 * at +0x10, cells two bytes apart. The grid is walked in 2x2 blocks: each block
 * samples the four cells at (5+2j .. 7+2j) x (5+2i .. 7+2i), ORs a `bit` into a
 * mask that starts at 0x23F0, and shifts `bit` left for the next cell. `bit` is
 * a u16 and is narrowed after every shift, which is why the loop reads as
 * `lsl`/`lsl`/`lsr` rather than one `lsl`.
 *
 * The output is a u16 grid at +0x40 with 32 entries per row, offset by
 * `xBase` entries and `yBase` rows. xBase/yBase are the block counts put back
 * into grid space: `(0x28 - (w - 10)) / 2 + 10` and `(0x28 - (h - 10)) / 2 + 2`.
 */
// Nonmatching: 36.4%, and it is a long way from done -- worth recording what is
// known so the next pass does not re-derive it.
//
//   * The block counts are `0x28 - (w - 10)`, not `50 - w`. mwcc keeps the
//     inner subtract as its own instruction and folding the pair into one
//     `50 - w` costs it. Worth 4% on its own.
//   * `cols` spills: the target keeps `(w - 9) / 2` in r11 and allocates five
//     stack words (`sub sp, #0x14`), this source needs seven and puts an extra
//     `cols <= 0` guard in front of the outer loop that the target does not
//     have -- mwcc rotated the inner loop and added the pre-header test. That
//     rotation is what the frame-size gap and the extra check both come from.
//   * The mask/bit inner pair, the `0x23F0` seed, the u16 narrowing of `bit`
//     and the `ptr + (y * w + x) * 2` cell fetch are all believed correct.
void func_ov039_02092e30(void* task) {
    u8* self = (u8*)task;
    u8* obj  = *(u8**)(self + 0x04);
    s32 w    = obj[2];
    s32 h    = obj[3];
    u8* ptr  = *(u8**)(obj + 0x10);

    /* The block counts are put back into grid space as `0x28 - (w - 10)`,
     * not `50 - w`: mwcc keeps the inner `w - 10` / `h - 10` as its own
     * subtract and the outer `rsb` as a separate one, and folding them into a
     * single `50 - w` costs that instruction. */
    s32 dw    = w - 10;
    s32 dh    = h - 10;
    s32 xBase = (0x28 - dw) / 2 + 10;
    s32 yBase = (0x28 - dh) / 2 + 2;
    s32 rows  = (h - 9) / 2;
    s32 cols  = (w - 9) / 2;
    s32 j;
    s32 i;

    if (rows <= 0) {
        return;
    }

    for (j = 0; j < rows; j++) {
        for (i = 0; i < cols; i++) {
            u16 mask = 0x23F0;
            u16 bit  = 1;
            s32 y;
            s32 x;

            for (y = 5 + j * 2; y < 7 + j * 2; y++) {
                for (x = 5 + i * 2; x < 7 + i * 2; x++) {
                    if (*(u8*)(ptr + (y * w + x) * 2) != 0) {
                        mask |= bit;
                    }
                    bit = (u16)(bit << 1);
                }
            }

            *(u16*)(self + 0x40 + xBase * 2 + (yBase + j) * 64 + i * 2) = mask;
        }
    }
}

/**
 * @brief The +0x81a4 sibling of `func_ov039_02094ca8`'s spring step.
 *
 * Same integral, same damping, same bounce -- but the countdown is at +0x88
 * against a +0x84 divisor, the magnitude is written to +0x44/+0x48, and the
 * step runs only when the +0x78 task's data resolves *and* +0x7C is set. The
 * +0x40 word is a u16 accumulator advanced by the u16 at +0x74 and narrowed
 * after the add, then stored back into the 32-bit slot.
 */
s32 func_ov039_020981a4(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* data   = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x78));
    s32   count;
    s32   scale;
    s32   v;

    /* One `return 1` at the end rather than one per exit: the target shares a
     * single epilogue across the three ways out. The first guard is also
     * negated -- `data != NULL` with the clear as its `else` -- because that is
     * what makes mwcc branch *to* the clear block and lay the main body out as
     * the fall-through, which is the target's shape. */
    if (data != NULL) {
        if (*(s32*)(sprite + 0x7C) != 0) {
            func_ov039_0208e85c(data, (OtuPoint*)(sprite + 0x50));

            count = *(s32*)(sprite + 0x88) - 1;
            *(s32*)(sprite + 0x88) = count;

            if (count <= 0) {
                *(s32*)(sprite + 0x7C) = 0;
                *(s32*)(sprite + 0x80) = 0;
            } else {
                scale = (count << 13) / *(s32*)(sprite + 0x84);
                *(s32*)(sprite + 0x48) = scale;
                *(s32*)(sprite + 0x44) = scale;

                *(s32*)(sprite + 0x40) =
                    (u16)(*(s32*)(sprite + 0x40) + *(u16*)(sprite + 0x74));

                v = *(s32*)(sprite + 0x6C) - data_ov039_0209a304;
                *(s32*)(sprite + 0x6C) = v;
                if (v < 0) {
                    *(s32*)(sprite + 0x6C) = 0;
                }

                *(s32*)(sprite + 0x70) =
                    *(s32*)(sprite + 0x70) + data_ov039_0209a310;

                func_ov039_02098c00(*(s32*)(sprite + 0x6C),
                                    (OtuPoint*)(sprite + 0x64),
                                    (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));

                v = *(s32*)(sprite + 0x60) + *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x60) = v;
                if (v > 0) {
                    *(s32*)(sprite + 0x60) = 0;
                    if (*(s32*)(sprite + 0x70) > 0) {
                        *(s32*)(sprite + 0x70) =
                            (s32)(((s64)*(s32*)(sprite + 0x70) *
                                   -data_ov039_0209a300 + 0x800) >>
                                  12);
                    }
                }

                Sprite_Update((Sprite*)sprite);
                *(s32*)(sprite + 0x80) = 1;
            }
        }
    } else {
        *(s32*)(sprite + 0x7C) = 0;
        *(s32*)(sprite + 0x80) = 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The five-phase swing task, 0x02091868.                              */
/* ------------------------------------------------------------------ */

/**
 * @brief The swing task's phase machine: wind-up, swing, recover.
 *
 * The sprite block (task+0x18) carries its own small state block: +0x6C is the
 * "step this frame" flag, +0x70 the phase, +0x74 a 4-frame hold count and
 * +0x48 the swing angle. Phase 1 seeds the swing from the pin's +0x138
 * position and falls straight through into phase 2, which is the swing
 * itself: it copies the pin's position pair, aims by 0x38000 and slews
 * +0x48 from 0x2000 toward zero over the four frames of +0x74. Phases 3 and 4
 * walk the angle back and the completion checks are gated on the owner's bit
 * 10 (the "swing enabled" bit), read as `(word << 0x15) >> 0x1F`.
 */
s32 func_ov039_02091868(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        switch (*(s32*)(sprite + 0x70)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x6C) = 1;
                func_ov039_0208e6cc(pin, (OtuPoint*)(sprite + 0x60));
                func_ov039_02098bd4(-0x1000, (OtuPoint*)(sprite + 0x60),
                                    (OtuPoint*)(sprite + 0x60));
                *(s32*)(sprite + 0x40) =
                    (u16)(FX_Atan2Idx(*(s32*)(sprite + 0x64),
                                      *(s32*)(sprite + 0x60)) +
                          0xC000);
                *(s32*)(sprite + 0x48) = 0;
                *(s32*)(sprite + 0x70) = 2;
                *(s32*)(sprite + 0x74) = 4;
                Sprite_SetAnimation((Sprite*)sprite, *(s16**)(sprite + 0x18), 1,
                                    *(SpriteCell**)(sprite + 0x1C));
                /* fall through */
            case 2:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));
                func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x58));
                func_ov039_02098c00(0x38000, (OtuPoint*)(sprite + 0x60),
                                    (OtuPoint*)(sprite + 0x58),
                                    (OtuPoint*)(sprite + 0x58));
                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) =
                        *(s32*)(sprite + 0x48) +
                        (0x2000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                }

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 3;
                    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18),
                                           2, *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 3:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (((u32)(*(s32*)sprite << 0x15)) >> 0x1F == 1) {
                    *(s32*)(sprite + 0x70) = 4;
                    *(s32*)(sprite + 0x74) = 4;
                    Sprite_ChangeAnimation(
                        (Sprite*)sprite, *(s32*)(sprite + 0x18), 3,
                        *(SpriteCell**)(sprite + 0x1C));
                }
                break;

            case 4:
                func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x50));

                if (*(s32*)(sprite + 0x74) > 0) {
                    *(s32*)(sprite + 0x48) =
                        *(s32*)(sprite + 0x48) +
                        -*(s32*)(sprite + 0x48) / *(s32*)(sprite + 0x74);
                    *(s32*)(sprite + 0x74) = *(s32*)(sprite + 0x74) - 1;
                } else {
                    *(s32*)(sprite + 0x70) = 0;
                    *(s32*)(sprite + 0x6C) = 0;
                }
                break;
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }

    if (*(s32*)(sprite + 0x6C) != 0) {
        Sprite_Update((Sprite*)sprite);
    }
    return 1;
}

/**
 * @brief The hammer-charge task's phase machine: raise, hold, swing.
 *
 * Sprite state block: +0x6C the step flag, +0x80 the phase, +0x78 a running
 * frame count and +0x7C a sub-count. Phase 0 clears the flag; phase 1 sets the
 * charge mode 0x334 and the wind-up is phase 2, which ramps +0x48 from 0x99A
 * toward 0x1000 over the +0x70 frames -- both +0x48 and its +0x4C copy keep
 * the ramp value. On frame zero it raises the sprite's +4 bit-6 bit and enters
 * the hold, phase 3, which counts +0x7C down and re-arms the charge (mode
 * 0x336) once its own counter lands. Phase 4 runs the same ramp back against
 * 0x99A. Phase 2 re-arms at frame zero and phase 3 counts +0x7C down before
 * running the same ramp back in phase 4, which zeroes the phase on landing.
 * While their frame count is still positive, phases 2 and 3 break out of the
 * switch; the fall-throughs (1 to 2, then on to 3 and 4) only happen on the
 * counts that reach zero.
 */
s32 func_ov039_020912c8(TaskPool* pool, void* task) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    void* pin;

    pin = EasyTask_GetTaskData(pool, *(s32*)(sprite + 0x68));

    if (pin != NULL) {
        s32 alive = func_ov039_0208e9d0(pin);

        *(s32*)(sprite + 0x6C) = alive;
        if (alive == 0) {
            *(s32*)(sprite + 0x80) = 0;
        } else {
            func_ov039_0208e85c(pin, (OtuPoint*)(sprite + 0x54));
            func_ov039_0208e6e0(pin, (OtuPoint*)(sprite + 0x5C));
            *(s32*)(sprite + 0x64) = func_ov039_0208e6f4(pin);
        }

        switch (*(s32*)(sprite + 0x80)) {
            case 0:
                *(s32*)(sprite + 0x6C) = 0;
                break;

            case 1:
                *(s32*)(sprite + 0x4C) = 0x99A;
                *(s32*)(sprite + 0x48) = 0x99A;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4),
                                       *(s32*)(sprite + 0x1C), 0xE,
                                       *(SpriteCell**)(sprite + 0x20));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x70);
                *(s32*)(sprite + 0x80) = 2;
                /* fall through */
            case 2:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) +
                    (0x1000 - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4),
                                       *(s32*)(sprite + 0x1C), 0xF,
                                       *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x334, (OtuPoint*)(sprite + 0x5C),
                                    (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x74);
                *(s32*)(sprite + 0x7C) = 0x1E;
                *(s32*)(sprite + 0x80) = 3;
                /* fall through */
            case 3:
                if (*(s32*)(sprite + 0x7C) > 0) {
                    *(s32*)(sprite + 0x7C) = *(s32*)(sprite + 0x7C) - 1;
                }
                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) > 0) {
                    break;
                }

                *(s32*)(sprite + 4) = (*(s32*)(sprite + 4) & ~0x60) | 0x40;
                Sprite_ChangeAnimation((Sprite*)(sprite + 4),
                                       *(s32*)(sprite + 0x1C), 2,
                                       *(SpriteCell**)(sprite + 0x20));
                func_ov039_02087d04(0x336, (OtuPoint*)(sprite + 0x5C),
                                    (OtuPoint*)(sprite + 0x54));
                *(s32*)(sprite + 0x78) = 0xE;
                *(s32*)(sprite + 0x80) = 4;
                /* fall through */
            case 4:
                *(s32*)(sprite + 0x48) = *(s32*)(sprite + 0x48) +
                    (0x99A - *(s32*)(sprite + 0x48)) / *(s32*)(sprite + 0x78);
                *(s32*)(sprite + 0x4C) = *(s32*)(sprite + 0x48);

                *(s32*)(sprite + 0x78) = *(s32*)(sprite + 0x78) - 1;

                if (*(s32*)(sprite + 0x78) <= 0) {
                    *(s32*)(sprite + 0x80) = 0;
                }
                break;
        }

        if (*(s32*)(sprite + 0x6C) != 0) {
            Sprite_Update((Sprite*)(sprite + 4));
        }
    } else {
        *(s32*)(sprite + 0x6C) = 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The board-shape background loader, 0x02098650.                      */
/* ------------------------------------------------------------------ */

/** The 0x27-sized bin the loader pulls, plus its three sub-objects. */
extern const BinIdentifier data_ov039_0209a0e4;

/**
 * @brief Loads the board's background data and allocates both displays from
 *        it.
 *
 * The result screen's background setup: sets the step flag at +0x44 and the
 * uncopied `dataType` word at +0, raises both displays' BG1 layer mask
 * (`0x1F | 0x12`), marks `bgAffines[1].unk_14` when the layer's bgMode is one
 * of the five scaled modes and clears both displays' +1 offsets, then loads
 * `0209a0e4`'s bin and walks its pointer table: the buffer's +8/+0x10/+0x18
 * words hold relative offsets from the buffer's +0x20 to the palette, char
 * and screen sources. Each source is allocated once per display (palette
 * first, then char and screen from `bgSettings[1]`, palette flush in between),
 * the sprite is reloaded and both displays fade to black.
 */
s32 func_ov039_02098650(TaskPool* pool, void* task, s32* args) {
    DisplayEngineState* state;
    u8*                 sprite = *(u8**)((u8*)task + 0x18);
    Data*               data;

    *(s32*)(sprite + 0x44) = 1;
    *(s32*)sprite          = *(s32*)args;

    g_DisplaySettings.controls[0].layers = 0x1F;
    g_DisplaySettings.controls[0].layers |= 0x12;
    state = &g_DisplaySettings.engineState[0];
    switch (state->bgSettings[1].bgMode) {
        case DISPLAY_BGMODE_AFFINE:
        case DISPLAY_BGMODE_PLTT:
        case DISPLAY_BGMODE_BMP256:
        case DISPLAY_BGMODE_BMPDIRECT:
        case DISPLAY_BGMODE_BMPLARGE:
            state->bgAffines[1].unk_14 = 1;
            break;

        default:
            break;
    }
    state->bgOffsets[1].hOffset = 0;
    state->bgOffsets[1].vOffset = 0;

    g_DisplaySettings.controls[1].layers = 0x1F;
    g_DisplaySettings.controls[1].layers |= 0x12;
    state = &g_DisplaySettings.engineState[1];
    switch (state->bgSettings[1].bgMode) {
        case DISPLAY_BGMODE_AFFINE:
        case DISPLAY_BGMODE_PLTT:
        case DISPLAY_BGMODE_BMP256:
        case DISPLAY_BGMODE_BMPDIRECT:
        case DISPLAY_BGMODE_BMPLARGE:
            state->bgAffines[1].unk_14 = 1;
            break;

        default:
            break;
    }
    state->bgOffsets[1].hOffset = 0;
    state->bgOffsets[1].vOffset = 0;

    data = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0e4);
    *(s32*)(sprite + 0x48) = (s32)data;

    {
        void* pal;
        void* chr;
        void* scr;

        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 8);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x10);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x18);
        }

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr,
                g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0,
                size);
        }
        *(BgResource**)(sprite + 0x54) = BgResMgr_AllocScreen(
            g_BgResourceManagers[0], scr,
            g_DisplaySettings.engineState[0].bgSettings[1].screenBase,
            (u32)g_DisplaySettings.engineState[0].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0x4C));

        *(PaletteResource**)(sprite + 0x58) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x5C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr,
                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0,
                size);
        }
        *(BgResource**)(sprite + 0x60) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], scr,
            g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[1]
                       .screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1],
                         *(PaletteResource**)(sprite + 0x58));
    }

    func_ov039_020985e0(sprite, sprite + 4);
    EasyFade_FadeMainDisplay(2, 0, 0x1000);
    EasyFade_FadeSubDisplay(2, 0, 0x1000);
    return 1;
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
            func_ov039_0208fefc(
                EasyTask_GetTaskData(*(void**)(self + 8), *(s32*)(self + 0x1E0)),
                0x14);
            func_ov039_02087d04(0x331, (OtuPoint*)(self + 0x120),
                                (OtuPoint*)(self + 0x110));
            return;

        case 1:
            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) - 0xA000;
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            *(s32*)(self + 0x100) =
                *(u16*)(*(u8**)(self + 0x170) +
                        (u32)*(u16*)*(s32*)(self + 0x16C) * 0x1C + 0xC);
            *(s32*)(self + 0xFC) = *(s32*)(self + 0xFC) + 1;
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
                    if (func_ov039_0208a988(
                            &aim, *(OtuCellGrid**)(self + 0xE4),
                            *(TinPinSlammer_Scene**)(self + 0)) != 0)
                    {
                        *(s32*)(self + 0x120) = aim.x;
                        *(s32*)(self + 0x124) = aim.y;
                    }
                }
            } else {
                OtuPinTask* cand = func_ov039_02087e2c(
                    *(TaskPool**)(self + 8), *(TinPinSlammer_Scene**)(self + 0),
                    *(s32*)(self + 0xE0));

                if (cand != NULL) {
                    func_ov039_02098bb0((OtuPoint*)((u8*)cand + 0x120),
                                        (OtuPoint*)(self + 0x120), &dir);
                    if (func_ov039_02098d10(&dir) < 0x1E000) {
                        *(s32*)(self + 0x100) = 1;
                    } else {
                        func_ov039_02098d3c(&dir, &dir);
                        func_ov039_02098c00(0x1800, &dir,
                                            (OtuPoint*)(self + 0x120), &dir);
                        if (func_ov039_0208a988(
                                &dir, *(OtuCellGrid**)(self + 0xE4),
                                *(TinPinSlammer_Scene**)(self + 0)) != 0)
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
            func_ov039_0208ff30(
                EasyTask_GetTaskData(*(void**)(self + 8),
                                     *(s32*)(self + 0x1E0)));
            *(s32*)(self + 0x100) =
                *(u16*)(*(u8**)(self + 0x170) +
                        (u32)*(u16*)*(s32*)(self + 0x16C) * 0x1C + 0xE);
            *(s32*)(self + 0xFC) = *(s32*)(self + 0xFC) + 1;
            func_ov039_02087d04(0x332, (OtuPoint*)(self + 0x120),
                                (OtuPoint*)(self + 0x110));
            return;

        case 4: {
            s32 i = 0;

            *(s32*)(self + 0x128) = *(s32*)(self + 0x128) +
                -*(s32*)(self + 0x128) / *(s32*)(self + 0x100);
            *(s32*)(self + 0x100) = *(s32*)(self + 0x100) - 1;
            if (*(s32*)(self + 0x100) > 0) {
                return;
            }
            func_ov039_0208ff68(
                EasyTask_GetTaskData(*(void**)(self + 8),
                                     *(s32*)(self + 0x1E0)));

            do {
                s32 step = i * 0x10000;

                func_ov039_02097750(
                    EasyTask_GetTaskData(*(void**)(self + 8),
                                         ((OtuBadge*)task)->groupIds[i]),
                    (OtuTaskParams*)(self + 0x120),
                    (u32)((step + ((u32)(step >> 2) >> 0x1D)) << 0xD) >> 0x10,
                    0x2000, 0xCD, 0x14);
                i = i + 1;
            } while (i < 8);

            func_ov039_02087d04(0x341, (OtuPoint*)(self + 0x120),
                                (OtuPoint*)(self + 0x110));
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

/* ------------------------------------------------------------------ */
/* The badge clash wind-up, 0x0208eaa0.                                */
/* ------------------------------------------------------------------ */

/** The Q12 scale word and the 0x10-stride per-speed factor table. */
// extern s32 data_ov039_0209a398;   (already declared in TinPinSlammer.h)
// extern OtuSpeedEntry data_ov039_0209a3dc[];  (declared in OtuPinLogic.inc)

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
// Nonmatching: 89.9%. Three scheduling shape differences left:
//   * the contact block. Target's contact block schedule mixes the y-diff
//     loads before the x-diff stores, laid out: load +0x124 pair, +0x120 self
//     copy, y-sub, +0x120 pair, x-sub, then store +0x12C before +0x130. This
//     source compiles straightforwardly in address order. Reversing the two
//     assignments in C scores worse (89.0).
//   * the tail's pool words. Target reads `data_ov039_0209a398` before the
//     +0x16C/+0x170 cell chain, so its literal pool lists a398 ahead of
//     a3dc; this source's factor-read runs first, giving the reversed pool
//     order. The inline-scale spelling (no scale local) supposedly matched
//     m2c's, but scored 85.9 -- the smull operand order flipped too.
//   * the `i = 0` init lands after the walk-inits here, before them in the
//     target (one `mov r7` row).
// The contact's `!= 0 || != 0` negation (swapped d3c arms) was worth +9% on
// its own and matched subsequently. Everything else -- all three mode bodies,
// the wind-down, the 64-bit smull/adc scale and every branch target -- agrees.
void func_ov039_0208eaa0(void* self_, void* task) {
    u8* other = (u8*)self_;
    u8* self  = (u8*)task;

    if (*(s32*)(self + 0x128) != 0) {
        return;
    }
    if (*(s32*)(self + 0x168) > 0) {
        return;
    }

    switch (*(s32*)(self + 0xF8)) {
        /* m2c's label order: default first (it joins the big block), then the
         * modes that share the body, then mode 8's phase gate, then the
         * modes that return. */
        default:
        case 1:
        case 6:
        case 7:
            break;

        case 8:
            switch (*(s32*)(self + 0xFC)) {
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

    if (*(s32*)(other + 0x180) <= 0) {
        return;
    }
    {
        s32 i          = 0;
        u8* mask       = other;
        u8* entry      = other + 0x184;

        do {
            s32 limit = *(s32*)(mask + 0x18C) + 0xC000;

            if (func_ov039_02098ca8((OtuPoint*)(self + 0x120),
                                    (OtuPoint*)entry) >= limit)
            {
                goto next;
            }

            switch (*(s32*)(other + 0xF8)) {
                case 6: {
                    func_ov039_02091668(EasyTask_GetTaskData(
                        *(void**)(other + 8), *(s32*)(other + 0x1E8)));
                    func_ov039_02087d04(0x337, (OtuPoint*)(other + 0x120),
                                        (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) =
                        *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) =
                        *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 ||
                        *(s32*)(self + 0x130) != 0)
                    {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C),
                                            (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                case 7: {
                    void* cand = EasyTask_GetTaskData(*(void**)(other + 8),
                                                      *(s32*)(other + 0x1E4));
                    u16  cursor = (u16)(func_ov039_02091060(cand) - 0x4000);
                    s32  pair   = (cursor >> 4) * 2;

                    func_ov039_0209104c(cand);

                    *(s32*)(self + 0x12C) = ((s16*)data_0205e4e0)[pair + 1];
                    *(s32*)(self + 0x130) = ((s16*)data_0205e4e0)[pair];

                    if (*(s32*)(self + 0xF8) == 6) {
                        func_ov039_02091690(EasyTask_GetTaskData(
                            *(void**)(self + 8), *(s32*)(self + 0x1E8)));
                    }
                    func_ov039_02087d04(0x33B, (OtuPoint*)(other + 0x120),
                                        (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }
                    break;
                }

                case 8: {
                    if (*(s32*)(self + 0xF8) == 7) {
                        func_ov039_02091070(EasyTask_GetTaskData(
                            *(void**)(self + 8), *(s32*)(self + 0x1E4)));
                    }
                    func_ov039_02087d04(0x333, (OtuPoint*)(other + 0x120),
                                        (OtuPoint*)(other + 0x110));

                    if (*(s32*)(self + 0x148) <= 0) {
                        if (*(u16*)*(s32*)(self + 0x16C) < 0x130) {
                            func_ov039_0208a490(self_, 1);
                        }
                    }

                    *(s32*)(self + 0x12C) =
                        *(s32*)(self + 0x120) - *(s32*)(other + 0x120);
                    *(s32*)(self + 0x130) =
                        *(s32*)(self + 0x124) - *(s32*)(other + 0x124);

                    if (*(s32*)(self + 0x12C) != 0 ||
                        *(s32*)(self + 0x130) != 0)
                    {
                        func_ov039_02098d3c((OtuPoint*)(self + 0x12C),
                                            (OtuPoint*)(self + 0x12C));
                    } else {
                        *(s32*)(self + 0x12C) = 0x1000;
                        *(s32*)(self + 0x130) = 0;
                    }
                    break;
                }

                default:
                    break;
            }

            if (*(s32*)(self + 0x148) <= 0) {
                *(s32*)(self + 0x148) =
                    *(u16*)(*(u8**)(self + 0x170) +
                            (u32)*(u16*)*(s32*)(self + 0x16C) * 0x1C + 0x18);
                *(s32*)(self + 0xF8) = 1;
                *(s32*)(self + 0xF4) = 0;
                func_ov039_0208f7a4(
                    EasyTask_GetTaskData(*(void**)(self + 8),
                                         *(s32*)(self + 0x1D8)),
                    *(s32*)(self + 0x148));
            }
            {
                OtuPoint  start;
                OtuPoint* base   = (OtuPoint*)(self + 0x12C);
                s32       factor = *(s32*)(
                    (u8*)&data_ov039_0209a3dc +
                    *(u8*)(*(u8**)(self + 0x170) +
                           (u32)*(u16*)*(s32*)(self + 0x16C) * 0x1C + 4) * 0x10 + 8);
                s32       scale;

                start.x = 0;
                scale   = (s32)(((s64)data_ov039_0209a398 * factor + 0x800) >> 0xC);
                start.y = 0;

                *(s32*)(self + 0x138) = *(s32*)(self + 0x12C);
                *(s32*)(self + 0x13C) = *(s32*)(self + 0x130);

                func_ov039_02098c00(scale, base, &start, base);
                *(s32*)(other + 0x1B0) = (s32)task;
                *(s32*)(self + 0x1B0)  = (s32)self_;
            }

        next:
            mask += 0xC;
            entry += 0xC;
            i = i + 1;
        } while (i < *(s32*)(other + 0x180));
    }
}

/* ------------------------------------------------------------------ */
/* The wrestling-banner board loader, 0x02092f88.                      */
/* ------------------------------------------------------------------ */

/** The board's bin id and the 0x12-wide frame-slot table. */
extern const BinIdentifier data_ov039_0209a0f4;
extern const void*         data_ov039_020999d0;

/**
 * @brief The wrestling board's background loader, one slot per layer.
 *
 * Reaches the sprite block at task+0x18, stamps the two words the caller
 * passes in +0/+4, marks the sub engine's BG0-3 layers on and sets up the
 * blend block (mode 1 over layers 3 and 0x3E, coefficients 0xA/6). Then four
 * identical resource slots, each reading its palette/char/screen sources out
 * of the Data buffer's relative offsets (+8/+0x10/+0x18, +0x20/+0x30/+0x38,
 * +0x40/+0x48, +0x50/+0x70/+0x78), allocating palette/char against
 * `engineState[1].bgSettings[N]` (the screen draw against
 * `bgSettings[2]`'s +2 and 0x3F0), and flushing. Layer 1's palette source is
 * the frame cursor at +0x840 loaded through `func_ov039_02098d7c` against
 * `data_ov039_020999d0` with 0x12 slots. In the middle: the 0x3F0 scratch
 * fill, `func_ov039_02092e30`'s mask build and the +0x3C word.
 */
s32 func_ov039_02092f88(TaskPool* pool, void* task, s32* args) {
    u8*   sprite = *(u8**)((u8*)task + 0x18);
    Data* data;
    u8*   pal;
    u8*   chr;
    u8*   scr;

    *(s32*)(sprite + 0x0) = *(s32*)((u8*)args + 4);
    *(s32*)(sprite + 0x4) = *(s32*)((u8*)args + 8);

    g_DisplaySettings.controls[1].layers |= 0xF;
    g_DisplaySettings.engineState[1].blendMode   = 1;
    g_DisplaySettings.engineState[1].blendLayer0 = 3;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x3E;
    g_DisplaySettings.engineState[1].blendCoeff0 = 0xA;
    g_DisplaySettings.engineState[1].blendCoeff1 = 6;

    data = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0f4);
    *(s32*)(sprite + 0x8) = (s32)data;

    {
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 8);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x10);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x18);
        }

        *(PaletteResource**)(sprite + 0xC) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x1C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr,
                g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0,
                size);
        }
        *(BgResource**)(sprite + 0x2C) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], scr,
            g_DisplaySettings.engineState[1].bgSettings[0].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[0]
                       .screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1],
                         *(PaletteResource**)(sprite + 0xC));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x30);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x38);
        }

        *(PaletteResource**)(sprite + 0x10) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1],
            func_ov039_02098d7c(sprite + 0x840, (s32)pal,
                                &data_ov039_020999d0, 0x12),
            0, 1, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x20) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr,
                g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0,
                size);
        }
        *(BgResource**)(sprite + 0x30) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], scr,
            g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[1]
                       .screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1],
                         *(PaletteResource**)(sprite + 0x10));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x40);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x48);
        }

        MI_CpuFillU16(0x3F0, sprite + 0x3C, 0x804);
        func_ov039_02092e30(sprite);
        *(s32*)(sprite + 0x3C) = 0x80400;

        *(PaletteResource**)(sprite + 0x14) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], pal, 0, 2, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x24) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr,
                g_DisplaySettings.engineState[1].bgSettings[2].charBase,
                0x3F0, size);
        }
        *(BgResource**)(sprite + 0x34) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], (void*)(sprite + 0x3C),
            g_DisplaySettings.engineState[1].bgSettings[2].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[2]
                       .screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1],
                         *(PaletteResource**)(sprite + 0x14));

        data = *(Data**)(sprite + 0x8);
        if (data == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x50);
        }
        if (data == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x70);
        }
        if (data == NULL) {
            scr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            scr      = base + *(u32*)(base + 0x78);
        }

        *(PaletteResource**)(sprite + 0x18) = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], pal, 0, 3, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x28) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr,
                g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0,
                size);
        }
        *(BgResource**)(sprite + 0x38) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], scr,
            g_DisplaySettings.engineState[1].bgSettings[3].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[3]
                       .screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1],
                         *(PaletteResource**)(sprite + 0x18));
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* The wrestling-board switchboard block, 0x02091b98.                  */
/* ------------------------------------------------------------------ */

/** The 0x40-byte table slicer copied to the bottom-half of the grid. */
extern s32                data_ov039_020995f8[];
extern s32                data_ov039_02099604[];
extern s32                data_ov039_0209968c[];
extern s32                data_ov039_02099620[];
extern s32                data_ov039_02099630[];
extern s32                data_ov039_0209964c[];
extern s32                data_ov039_0209966c[];
extern s32                data_ov039_020996b0[];
extern const BinIdentifier data_ov039_0209a0bc[4];
extern u8*                 data_ov039_0209a5d8[3];

/** The whole-word block shapes the loader's three stack tables copy out to. */
typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
} OtuWord3; // Size: 0xC

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
    s32 w8;
} OtuWord9; // Size: 0x24

/**
 * @brief The wrestling board's block-loader: picks a sheet from the args
 *        block, allocates everything it needs and stamps the mixed-grid.
 *
 * The sprite block at task+0x18 gets: +0/+4 the sheet index and the heap, +8
 * the "loading" flag and +C the sheet description pointer. Then: three
 * literal tables are snapped to the stack whose layout picks a row by
 * sheet.cellType and sheet.faceId (a 3x3 u32 matrix plus a flat u32 table);
 * the relative refs +8/+0x10/+0x18 are loaded and allocated via palette and
 * BgResMgr_AllocChar32 against `bgSettings[1].charBase`, palette-flushed.
 * Then five frame cursors (0x54/0x64, 0x68/0x78, 0x7C/0x8C, 0x90/0xA0,
 * 0xA4/0xB4) are allocated against `data_ov039_0209964c` (8 slots),
 * `02099620` (4), `0209966c` (8), `020996b0` (0x19) and `02099630` (7),
 * each from a Data-buffer offset (0x10/0x18/0x20/0x28/0x30), each flushed.
 * The final +0x48 AllocChar32 comes from the +0x38-offset ref against
 * `bgSettings[1].charBase` at offset 0x300.
 *
 * The grid is `sheet.width + 7 / 8` by `sheet.height + 7 / 8`, backed by two
 * heaps -- a tile-pool buffer (0x800 bytes per cell) and a pointer table --
 * MI_CpuSet zeroed, and the pages are linked so rows[i][j] = tilePool +
 * (i + j) * 0x800. Then the layout walk stamps every tile pair
 * through `func_ov039_02091b40`, and the whole thing hands the pointer
 * table and the tile pool to `func_0200d1d8`.
 */
// Nonmatching: 81.7%. The body, constants and every branch agree; the residual
// gap is register naming and two extra stack words. What was spent here:
//   * the three literal-table copies must be struct assignments spelled with
//     whole-block casts (`*(OtuWord3*)sheetTable = *(OtuWord3*)...`),
//     not loops or word chains -- only the struct form emits the target's
//     ldm/stm pairs;
//   * the sheet kind must be re-read as a byte expression everywhere it is
//     used (`*(u8*)sheet0`), never held in a named u8 local -- a named local
//     forced an extra stack slot, a `str [sp+0xc]` spill the target does not
//     have, and shifted the whole frame (+2 words) and every pool offset;
//   * `sheet[2]` likewise is re-read per loop iteration (this build forgot
//     that once and dropped 3%);
//   * `sheet[1]` is a fresh leaf byte too -- naming it cost a register;
//   * the two cell-grid [8]-accumulator sizes are `(w+7+(correction)) >> 3`
//     spelled exactly as m2c's -- matches.
// The pointers currently land r5/r9/r6 where the target has r5/r7/r8, so
// every sprite+* memory row reads r6 where the target reads r7. Permuter and
// eight spelling sweeps found nothing further.
s32 func_ov039_02091b98(TaskPool* pool, OtuBadge* task, s32* args) {
    u8*    sprite = *(u8**)((u8*)task + 0x18);
    Data*  data;
    Data*  data2;
    u8*    pal;
    u8*    chr;
    u8*    scr;
    s32    sheetTable[3];
    s32    styleTable[9];
    s32    miscTable[3];
    u8*    sheet0;
    Heap*  heap;
    s32    row;
    s32    row2;
    s32    cellsWide;
    s32    cellsHigh;
    s32    i;
    s32    j;
    s32    run;
    u8     wide;
    s32    col;

    *(s32*)(sprite + 0x8) = 1;
    *(s32*)(sprite + 0x0) = *(s32*)args;
    *(s32*)(sprite + 0x4) = *(s32*)((u8*)args + 4);
    *(s32*)(sprite + 0xC) = *(s32*)((u8*)args + 8);

    *(OtuWord3*)sheetTable = *(OtuWord3*)data_ov039_020995f8;
    *(OtuWord9*)styleTable = *(OtuWord9*)data_ov039_0209968c;
    *(OtuWord3*)miscTable  = *(OtuWord3*)data_ov039_02099604;

    sheet0 = *(u8**)((u8*)args + 8);
    heap   = *(Heap**)(sprite + 4);

    data = DatMgr_LoadRawData(
        *(s32*)args, NULL, 0,
        &data_ov039_0209a0b4[sheetTable[*(u8*)sheet0]]);
    *(s32*)(sprite + 0x44) = (s32)data;

    row2 = miscTable[*(u8*)sheet0];

    row = styleTable[(u32)*(u8*)sheet0 * 3 + sheet0[1]];
    wide = sheet0[2];

    {
        if (data == NULL || row <= 0) {
            chr = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            chr      = base + *(u32*)(base + (u32)row * 8);
        }
        if (data == NULL || row2 <= 0) {
            pal = NULL;
        } else {
            u8* base = (u8*)data->buffer + 0x20;
            pal      = base + *(u32*)(base + (u32)row2 * 8);
        }

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], pal, 0, 0, 5);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr,
                g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0,
                size);
        }
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0x4C));
    }

    data2 = DatMgr_LoadRawData(*(s32*)args, NULL, 0, data_ov039_0209a0bc);
    *(s32*)(sprite + 0x40) = (s32)data2;

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x10);
        }
        *(PaletteResource**)(sprite + 0x64) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0],
            func_ov039_02098d7c(sprite + 0x54, (s32)pal,
                                data_ov039_0209964c, 8),
            0, 6, 1);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0x64));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x18);
        }
        *(PaletteResource**)(sprite + 0x78) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0],
            func_ov039_02098d7c(sprite + 0x68, (s32)pal,
                                data_ov039_02099620, 4),
            0, 7, 1);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0x78));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        *(PaletteResource**)(sprite + 0x8C) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0],
            func_ov039_02098d7c(sprite + 0x7C, (s32)pal,
                                data_ov039_0209966c, 8),
            0, 8, 1);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0x8C));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x28);
        }
        *(PaletteResource**)(sprite + 0xA0) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0],
            func_ov039_02098d7c(sprite + 0x90, (s32)pal,
                                data_ov039_020996b0, 0x19),
            0, 9, 1);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0xA0));
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x30);
        }
        *(PaletteResource**)(sprite + 0xB4) = PaletteMgr_AllocPalette(
            g_PaletteManagers[0],
            func_ov039_02098d7c(sprite + 0xA4, (s32)pal,
                                data_ov039_02099630, 7),
            0, 0xA, 1);
        PaletteMgr_Flush(g_PaletteManagers[0],
                         *(PaletteResource**)(sprite + 0xB4));
    }

    {
        if (data2 == NULL) {
            chr = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            chr      = base + *(u32*)(base + 0x38);
        }
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x48) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr,
                g_DisplaySettings.engineState[0].bgSettings[1].charBase,
                0x300, size);
        }
    }

    cellsWide = (wide + 7 + ((u32)((wide + 7) >> 2) >> 0x1D)) >> 3;
    cellsHigh = (sheet0[3] + 7 + ((u32)((sheet0[3] + 7) >> 2) >> 0x1D)) >> 3;

    *(s32*)(sprite + 0x3C) = (s32)Mem_AllocHeapTail(
        heap, cellsHigh * (cellsWide << 0xB));
    *(s32*)(sprite + 0x38) = (s32)Mem_AllocHeapTail(
        heap, cellsHigh * (cellsWide * 4));
    MI_CpuSet(*(void**)(sprite + 0x3C), 0,
              Mem_GetBlockSize(heap, *(void**)(sprite + 0x3C)));

    if (cellsHigh > 0) {
        i   = 0;
        run = 0;
        do {
            j = 0;
            if (cellsWide > 0) {
                do {
                    *(u32*)((u8*)*(s32*)(sprite + 0x38) + (s32)(run + j) * 4) =
                        (u32)((u8*)*(s32*)(sprite + 0x3C) + (j + run) * 0x800);
                    j = j + 1;
                } while (j < cellsWide);
            }
            i   = i + 1;
            run = run + cellsWide;
        } while (i < cellsHigh);
    }

    if (sheet0[3] > 0) {
        col = 0;
        do {
            if (sheet0[2] > 0) {
                j = 0;
                do {
                    u8* cellBase = *(u8**)(sheet0 + 0x10) +
                                   (col * sheet0[2] + j) * 2;
                    u8   cellByte = cellBase[1];
                    u16  tileVal  = *(u16*)(data_ov039_0209a5d8[*(u8*)sheet0] +
                                            cellByte * 2);

                    func_ov039_02091b40(
                        tileVal,
                        (u16*)(*(u32*)((u8*)*(s32*)(sprite + 0x38) +
                                       ((j / 8) * cellsWide + (col / 8)) * 4) +
                               ((j % 8) * 4 + (col % 8) * 128) * 2));
                    j = j + 1;
                } while (j < (s32)sheet0[2]);
            }
            col = col + 1;
        } while (col < (s32)sheet0[3]);
    }

    func_0200d1d8(sprite + 0x10, 0, 1, 0, *(s32*)(sprite + 0x38), cellsWide,
                  cellsHigh);
    return 1;
}
