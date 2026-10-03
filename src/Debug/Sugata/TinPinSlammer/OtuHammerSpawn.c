#include "OtuFieldAccessShared.h"

/** Spawns one child of a simulation task, packing two words of args.
 *
 * Typed as returning the new task's handle, because Tsk_OtosuGame_hammer's
 * CreateTask stores the result into its `children[i]` and the target's
 * `str r0, [r1, #0x128]` is unambiguous about that. This costs nothing here: the
 * body has no `mov r0` of its own in either typing, so the handle already comes
 * back in r0 and both spellings compile to the same eight instructions.
 */
s32 func_ov039_02098394(TaskPool* pool, s32 arg1, s32 arg2) {
    s32 args[2];

    args[0] = arg1;
    args[1] = arg2;

    return EasyTask_CreateTask(pool, &data_ov039_0209a024, NULL, 0, NULL, args);
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

    *(s32*)(sprite + 0x7C)      = 1;
    *(OtuWord4*)(sprite + 0x40) = *(OtuWord4*)table;

    /* Both halves are read before either is written: the target keeps x in r2
     * and y in r1 across the pair of stores. */
    ax = at->x;
    ay = at->y;

    *(s32*)(sprite + 0x58) = ax;
    *(s32*)(sprite + 0x5C) = ay;
    *(s32*)(sprite + 0x60) = 0;

    *(s32*)(sprite + 0x6C) = data_ov039_0209a314;
    *(s32*)(sprite + 0x6C) = *(s32*)(sprite + 0x6C) + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    *(s32*)(sprite + 0x70) = -data_ov039_0209a2fc;
    *(s32*)(sprite + 0x70) = *(s32*)(sprite + 0x70) - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

#define OTU_SPRING_MAG ((FX_Divide(*(s32*)(sprite + 0x70), data_ov039_0209a310) * 3) >> 12)
    mag = OTU_SPRING_MAG < 0 ? -OTU_SPRING_MAG : OTU_SPRING_MAG;
#undef OTU_SPRING_MAG

    *(s32*)(sprite + 0x84) = mag;
    *(s32*)(sprite + 0x88) = mag;

    cell                   = RNG_Next(0x10000) >> 4;
    *(s32*)(sprite + 0x64) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2 + 1) * 2);
    *(s32*)(sprite + 0x68) = *(s16*)((u8*)&data_0205e4e0 + (cell * 2) * 2);

    *(s16*)(sprite + 0x74) = RNG_Next(0x800) + 0x800;

    Sprite_ChangeAnimation((Sprite*)sprite, *(s32*)(sprite + 0x18), *(s32*)(sprite + 0x1C), (s16)(index == 0 ? 2 : 1));
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

/** Loads the sprite for the task whose anim template is data_ov039_0209a088. */
void func_ov039_020985e0(void* self, void* sprite) {
    SpriteAnimation anim = data_ov039_0209a088;

    anim.owner    = self;
    anim.dataType = (u16) * (s32*)self;
    _Sprite_Load((Sprite*)sprite, &anim);
}

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

    data                   = DatMgr_LoadRawData(*(s32*)args, NULL, 0, &data_ov039_0209a0e4);
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

        *(PaletteResource**)(sprite + 0x4C) = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x50) = BgResMgr_AllocChar32(
                g_BgResourceManagers[0], chr, g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x54) =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], scr, g_DisplaySettings.engineState[0].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[0].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[0], *(PaletteResource**)(sprite + 0x4C));

        *(PaletteResource**)(sprite + 0x58) = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            *(BgResource**)(sprite + 0x5C) = BgResMgr_AllocChar32(
                g_BgResourceManagers[1], chr, g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        *(BgResource**)(sprite + 0x60) =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], *(PaletteResource**)(sprite + 0x58));
    }

    func_ov039_020985e0(sprite, sprite + 4);
    EasyFade_FadeMainDisplay(2, 0, 0x1000);
    EasyFade_FadeSubDisplay(2, 0, 0x1000);
    return 1;
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
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], *(PaletteResource**)(block + 0x4C));

    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], *(BgResource**)(block + 0x60));
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], *(BgResource**)(block + 0x5C));
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], *(PaletteResource**)(block + 0x58));

    DatMgr_ReleaseData(*(Data**)(block + 0x48));
    Sprite_Release((Sprite*)(block + 4));
    return 1;
}

/** The same dispatcher against data_ov039_0209a078. */
void func_ov039_020989a8(void* a, void* b, void* c, s32 index) {
    TaskStages table = data_ov039_0209a078;

    table.iter[index](a, b, c);
}

/** Spawns the one-word argument task table data_ov039_0209a06c. */
s32 func_ov039_020989f0(TaskPool* pool, s32 arg) {
    s32 args = arg;

    return EasyTask_CreateTask(pool, &data_ov039_0209a06c, NULL, 0, NULL, &args);
}

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

/* ------------------------------------------------------------------ */
/* The dispatch container's own fields.                               */
/* ------------------------------------------------------------------ */

/**
 * Stores a stage descriptor as the container's action and raises its active flag.
 *
 * This is the call the entry points make when choosing which stage to run --
 * func_ov039_02082c50 branches on it to pick the menu stage or the result stage.
 */
void func_ov039_02098a40(void* dispatch, void* stage) {
    *(void**)((u8*)dispatch + 0x0C) = stage;
    *(s32*)((u8*)dispatch + 0x10)   = 1;
}

/** Increments the stage index -- the only accessor here that does not assign. */
void func_ov039_02098a50(OtuStageDispatch* dispatch) {
    dispatch->stageIndex = dispatch->stageIndex + 1;
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
    u8* self = (u8*)task;
    u8* obj;
    s32 size;
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

/** True when +0x0C is clear and +0x10 is set. */
s32 func_ov039_02098b44(void* task) {
    if (*(s32*)((u8*)task + 0x0C) == 0 && *(s32*)((u8*)task + 0x10) != 0) {
        return 1;
    }
    return 0;
}

/** Reads the current stage block. */
void* func_ov039_02098b68(void* dispatch) {
    return *(void**)((u8*)dispatch + 0x04);
}

/** Reads the stage block the row table hangs off. */
void* func_ov039_02098b70(OtuStageDispatch* dispatch) {
    return dispatch->stageBlock;
}

/** Copies the container's first two words out. */
void func_ov039_02098b78(OtuStageDispatch* dispatch, s32* out) {
    out[0] = *(s32*)((u8*)dispatch + 0x00);
    out[1] = *(s32*)((u8*)dispatch + 0x04);
}

/** out = a + b, component-wise. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}
