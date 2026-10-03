#include "OtuFieldAccessShared.h"

// Size: 0x90

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
#define Otu_RES_REF(out, rec, idx)                        \
    do {                                                  \
        if ((rec) == NULL || (idx) <= 0) {                \
            (out) = NULL;                                 \
        } else {                                          \
            u8* _base = *(u8**)((u8*)(rec) + 0x08);       \
            u8* _tbl  = _base + 0x20;                     \
            (out)     = _tbl + *(s32*)(_tbl + (idx) * 8); \
        }                                                 \
    } while (0)

/**
 * Fill a SpriteAnimation for this obstacle from the shared template.
 *
 * The template is copied whole and then six fields are overwritten, so nothing
 * here depends on the template's current contents except the OAM word, which is
 * masked rather than replaced.
 */
void func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args) {
    SpriteAnimation params = data_ov039_0209996c;
    u16             kind   = args->params->kind;

    /* Bits 0-1 and 6-15 of the OAM word survive; 2-5 are cleared and rebuilt
     * from the caller's value, which needs a shift of 2 to line up. The target
     * spells it `lsl #0x1c` then `orr ..., lsr #0x1a`, a net shift of 2 -- and
     * because it truncates the caller to 16 bits first, only the caller's low
     * four bits can reach bits 2-5.
     *
     * Written as one expression on purpose. The target reads the word out of the
     * stack frame as the first instruction after the template copy and stores it
     * back twenty-odd instructions later; splitting the read into a named local
     * to try to schedule it there produced byte-identical code, and 720
     * permutations of the statement order could not reproduce it either. mwcc
     * hoists the `args->params` and `args->oamAttrs` loads above the stack read
     * regardless of how the source is written. */
    *(u16*)&params   = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner     = self;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20    = data_ov039_02099958[kind][args->slot];
    params.unk_26    = data_ov039_02099918[kind];
    params.unk_1C    = data_ov039_0209991e[kind];
    params.unk_28    = data_ov039_02099924[kind];
    params.posX      = (s16)((self->targetX - self->originX) >> 12);
    params.posY      = (s16)((self->targetY - self->originY) >> 12);
    _Sprite_Load(sprite, &params);
}

/** Spawn-time setup. Nothing moves on this frame; the first Render does. */
s32 func_ov039_0209258c(TaskPool* pool, Task* task, void* args) {
    OtuObstacle*      self = (OtuObstacle*)task->data;
    OtuObstacle_Args* a    = (OtuObstacle_Args*)args;
    /* The 16.16 scale per kind, copied from the overlay's data rather than spelled
     * out here: the target's literal pool names data_ov039_0209993c for this load,
     * where a brace-initialised local would name a compiler-generated template
     * instead. Same ldm/stm copy either way, different literal.
     *
     * A struct wrapper, not an array: `s32 x[3] = someArray;` is an illegal
     * initialisation, but copying one struct from another is exactly the
     * 12-byte ldm/stm the target performs. */
    OtuObstacle_Scales scales = data_ov039_0209993c;

    self->originX     = 0;
    self->originY     = 0;
    self->targetX     = (s32)a->params->targetX << 12;
    self->targetY     = (s32)a->params->targetY << 12;
    self->scale       = scales.v[a->params->kind];
    self->animPending = 0;

    func_ov039_02092484(self, &self->sprite, a);
    return 1;
}

/** One frame of work: swap to the looping animation, then advance the sprite. */
s32 func_ov039_02092608(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    if (self->animPending != 0) {
        /* Bits 5-6 of the sprite's first word are animationMode; this selects
         * mode 2. */
        *(u32*)&self->sprite = (*(u32*)&self->sprite & ~0x60) | 0x40;
        /* animFrame is the literal 2 and cellTable comes last. Passing them the
         * other way round compiles -- the cell table pointer truncates to an
         * s16 frame without complaint -- and is wrong. */
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

/** The sprite is stationary, so this only refreshes its position from the
 *  fixed-point target. It runs every frame rather than on movement, because the
 *  target can be rewritten while the sprite sits still. */
s32 func_ov039_02092658(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    self->sprite.posX = (s16)((self->targetX - self->originX) >> 12);
    self->sprite.posY = (s16)((self->targetY - self->originY) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 func_ov039_02092694(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = (OtuObstacle*)task->data;

    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_020926a8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = func_ov039_0209258c,
        .update     = func_ov039_02092608,
        .render     = func_ov039_02092658,
        .cleanup    = func_ov039_02092694,
    };
    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_020926f0(TaskPool* pool, s32 oamAttrs, s16 unk_04, s16 slot, OtuObstacle_Params* params) {
    OtuObstacle_Args args;

    args.oamAttrs = oamAttrs;
    args.unk_04   = unk_04;
    args.slot     = slot;
    args.params   = params;

    return EasyTask_CreateTask(pool, &data_ov039_02099930, NULL, 0, NULL, &args);
}

/* ------------------------------------------------------------------ */
/* A second, smaller object: 0x40 - 0x54.                              */
/* ------------------------------------------------------------------ */

/** Writes the +0x40/+0x44 pair from a point -- the target loads from the point
 *  and stores into the task, so this is a setter despite its twin below. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_02092730(void* task, OtuPoint* in) {
    *(OtuPoint*)((u8*)task + 0x40) = *in;
}

/** Reads the +0x48/+0x4C pair out to a point. */
// Written as a whole-`OtuPoint` assignment. Two scalar stores make mwcc
// interleave the first store between the two loads, and a named local makes
// it merge them into `stmia`; the struct assignment is the target's four
// instructions (ldr, ldr, str, str) exactly.
void func_ov039_02092744(void* task, OtuPoint* out) {
    *out = *(OtuPoint*)((u8*)task + 0x48);
}

/** A single word at +0x50. */
s32 func_ov039_02092758(void* task) {
    return *(s32*)((u8*)task + 0x50);
}

/** Raises the +0x54 word to 1. */
void func_ov039_02092760(void* task) {
    *(s32*)((u8*)task + 0x54) = 1;
}

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
        DatMgr_LoadRawData(self->dataType, NULL, 0, (BinIdentifier*)&data_ov039_0209a0b4[params->fileId]);
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
        PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], ref0, 0, params->palStart, params->palCount);

    size = (s32)((*(u32*)ref1) >> 8);
    if ((*(u8*)ref1 & 0xF0) == 0) {
        size -= 4;
    }

    self->chars[params->slot] = BgResMgr_AllocChar32(
        g_BgResourceManagers[0], ref1, g_DisplaySettings.engineState[0].bgSettings[params->group].charBase, 0, size);

    PaletteMgr_Flush(g_PaletteManagers[0], self->palettes[params->slot]);

    self->buffers[params->slot] = Mem_AllocHeapTail(self->heap, params->height * (params->width * 4));

    func_0200d898(self->buffers[params->slot], ref2 + 4, params->width, params->height);

    func_0200d1d8(&self->slots[params->slot], 0, params->group, 0, self->buffers[params->slot], params->width, params->height);

    func_0200d858(&self->slots[params->slot], 1, 1, 0);

    self->widths[params->slot]  = params->width << 20;
    self->heights[params->slot] = params->height << 20;

    if (params->layers >= 0) {
        g_DisplaySettings.controls[0].layers |= params->layers;
    }

    g_DisplaySettings.engineState[0].bgSettings[params->group].priority = params->priority;
}

// Size: 0xA0

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
            v                    = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit                = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v                    = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit                = *(s32*)(self + 0x8C);
            if (v >= limit) {
                *(s32*)(self + 0x9C) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }
            break;

        case 2:
            v                    = *(s32*)(self + 0x90) + 0x333;
            *(s32*)(self + 0x90) = v;
            limit                = *(s32*)(self + 0x80);
            if (v >= limit) {
                *(s32*)(self + 0x90) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v                    = *(s32*)(self + 0x98) + 0x333;
            *(s32*)(self + 0x98) = v;
            limit                = *(s32*)(self + 0x88);
            if (v >= limit) {
                *(s32*)(self + 0x98) %= limit;
                *(s32*)(self + 0x30) |= 2;
            }

            v                    = *(s32*)(self + 0x94) + 0x19A;
            *(s32*)(self + 0x94) = v;
            limit                = *(s32*)(self + 0x84);
            if (v >= limit) {
                *(s32*)(self + 0x94) %= limit;
                *(s32*)(self + 0x58) |= 2;
            }

            v                    = *(s32*)(self + 0x9C) + 0x19A;
            *(s32*)(self + 0x9C) = v;
            limit                = *(s32*)(self + 0x8C);
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

s32 func_ov039_02092bf0(void) {
    return 1;
}

// Size: 0x80

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

/** The same dispatcher against data_ov039_020999a4. */
void func_ov039_02092c8c(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_020999a4;

    table.iter[index](a, b, c);
}

/** The board's second three-word spawner, against its own task table. */
s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardParams* params) {
    s32 args[3];

    args[0] = dataType;
    args[1] = (s32)heap;
    args[2] = (s32)params;
    return EasyTask_CreateTask(pool, &data_ov039_02099998, NULL, 0, NULL, args);
}

// Size: 0xA0

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

    data                  = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0f4);
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

        *(PaletteResource**)(sprite + 0xC) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x1C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x2C) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[0].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0xC));

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
            g_PaletteManagers[1], func_ov039_02098d7c(sprite + 0x840, (s32)pal, &data_ov039_020999d0, 0x12), 0, 1, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x20) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x30) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x10));

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

        *(PaletteResource**)(sprite + 0x14) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 2, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x24) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0x3F0, size);
        }
        *(BgResource**)(sprite + 0x34) = BgResMgr_AllocScreen(
            g_BgResourceManagers[1], (void*)(sprite + 0x3C), g_DisplaySettings.engineState[1].bgSettings[2].screenBase,
            (u32)g_DisplaySettings.engineState[1].bgSettings[2].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x14));

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

        *(PaletteResource**)(sprite + 0x18) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 3, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x28) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x38) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[3].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[3].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x18));
    }
    return 1;
}
