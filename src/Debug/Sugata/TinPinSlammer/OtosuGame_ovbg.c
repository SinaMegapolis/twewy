/**
 * @file OtosuGame_ovbg.c
 * @brief The `Tsk_OtosuGame_ovbg` task.
 */

#include "OtuFieldAccessShared.h"

/* The task's stage table and handle. */
extern const TaskStages data_ov039_020999c0;
extern const TaskHandle data_ov039_020999b4;

/** The table 020934e0 refuses to re-point the animated palette at twice. */
extern const u16 data_ov039_02099a18[1];

/** The board's bin id and the 0x12-wide frame-slot table. */
extern const BinIdentifier data_ov039_0209a0f4;
extern const void*         data_ov039_020999d0;

/**
 * Builds the overlay layer's screen map from the layout: one entry per 2x2
 * block of cells, centred on the 32x32 screen, whose low four bits say which of
 * the block's cells are occupied (on top of the 0x23F0 base tile).
 */
// Nonmatching: 36.4%. Known: the block counts are `0x28 - (w - 10)` (folding
// to `50 - w` costs an instruction), and the target keeps `cols` in r11 with a
// five-word frame where this build spills and adds a `cols <= 0` pre-test.
void func_ov039_02092e30(OtuOvbg* self) {
    OtuBoardLayout* layout = self->layout;
    s32             w      = layout->width;
    s32             h      = layout->height;
    u8*             cells  = layout->cells;

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
                    if (cells[(y * w + x) * 2] != 0) {
                        mask |= bit;
                    }
                    bit = (u16)(bit << 1);
                }
            }

            self->screen[0][xBase + (yBase + j) * 32 + i] = mask;
        }
    }
}

/**
 * Sets up the four sub-display BG layers: a backdrop, an animated-palette
 * layer, the occupancy overlay built by func_ov039_02092e30 and a top layer,
 * blended over layers 0/1.
 */
s32 func_ov039_02092f88(TaskPool* pool, Task* task, OtuBoardArgs* args) {
    OtuOvbg* self = task->data;
    Data*    data;
    u8*      pal;
    u8*      chr;
    u8*      scr;

    self->heap   = args->heap;
    self->layout = args->layout;

    g_DisplaySettings.controls[1].layers |= 0xF;
    g_DisplaySettings.engineState[1].blendMode   = 1;
    g_DisplaySettings.engineState[1].blendLayer0 = 3;
    g_DisplaySettings.engineState[1].blendLayer1 = 0x3E;
    g_DisplaySettings.engineState[1].blendCoeff0 = 0xA;
    g_DisplaySettings.engineState[1].blendCoeff1 = 6;

    data           = DatMgr_LoadRawData(args->dataType, NULL, 0, &data_ov039_0209a0f4);
    self->fileData = data;

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

        self->palettes[0] = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars[0] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chr,
                                                  g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, size);
        }
        self->screens[0] =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[0].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], self->palettes[0]);

        data = self->fileData;
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

        self->palettes[1] = PaletteMgr_AllocPalette(
            g_PaletteManagers[1], func_ov039_02098d7c(&self->paletteAnim, (s32)pal, &data_ov039_020999d0, 0x12), 0, 1, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chr,
                                                  g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        self->screens[1] =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], self->palettes[1]);

        data = self->fileData;
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

        MI_CpuFillU16(0x3F0, &self->screenHeader, 0x804);
        func_ov039_02092e30(self);
        self->screenHeader = 0x80400;

        self->palettes[2] = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 2, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars[2] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chr,
                                                  g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0x3F0, size);
        }
        self->screens[2] = BgResMgr_AllocScreen(g_BgResourceManagers[1], &self->screenHeader,
                                                g_DisplaySettings.engineState[1].bgSettings[2].screenBase,
                                                (u32)g_DisplaySettings.engineState[1].bgSettings[2].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], self->palettes[2]);

        data = self->fileData;
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

        self->palettes[3] = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 3, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars[3] = BgResMgr_AllocChar32(g_BgResourceManagers[1], chr,
                                                  g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, size);
        }
        self->screens[3] =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[3].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[3].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], self->palettes[3]);
    }
    return 1;
}

/* The background task: Tsk_OtosuGame_ovbg. */

/**
 * @brief Updates the background task's animated palette, 0x020933c0.
 *
 * `func_ov039_02098dbc` advances the ring buffer at +0x840 by one entry and
 * returns the pointer to the palette that entry selects; that pointer is then
 * handed to `PaletteMgr_SetSource` for the second of the task's four palette
 * resources (+0x10, the one init stage 02092f88 allocated with slot index 1).
 *
 * The other three layers are left alone, so this is a per-frame recolour of
 * one BG layer rather than a repaint.
 */
s32 func_ov039_020933c0(TaskPool* pool, Task* self, void* arg) {
    OtuOvbg* data = (OtuOvbg*)self->data;

    PaletteMgr_SetSource(g_PaletteManagers[1], data->palettes[1], func_ov039_02098dbc(&data->paletteAnim));
    return 1;
}

/**
 * @brief The background task's render stage, 0x020933f0.
 *
 * Returns TRUE and draws nothing, which is what a four-BG-layer task needs: its
 * layers are set up once by the init stage and then repainted by the hardware
 * until the cleanup stage releases them.
 *
 * This is decidable from the table slot rather than guessed. `data_ov039_020999c0`
 * is `{02092f88, 020933c0, 020933f0, 020933f8}`, which is init / update /
 * render / cleanup, and 02092f88 allocates four `BgResMgr` layer triples and
 * no sprites. So this is a real empty render stage for a real reason.
 *
 * Note this does *not* generalise to the other three constant-1 bodies in the
 * overlay (020921f4, 02092bf0, 02096d98). 020921f4 and 02092bf0 are also
 * render slots, but 02096d98 sits at index 1 of `data_ov039_02099ec8` -- an
 * update slot -- for a task whose render (02096da0) is the one that does the
 * work. Same instruction, different meaning; the four are left independent.
 */
s32 func_ov039_020933f0(TaskPool* pool, Task* self, void* arg) {
    return 1;
}

/**
 * @brief Releases the background task's four layer triples, 0x020933f8.
 *
 * One pass per layer, screen first, then char data, then the palette -- the
 * reverse of the order init stage 02092f88 acquired them in. The layer index
 * runs 0..3 and is *not* multiplied into the field offsets: each of the three
 * resources is a four-entry array with a four-byte stride, so the three loads
 * inside the loop all share one `data + i * 4` base and differ only in their
 * displacement. Written that way deliberately; indexing the arrays as arrays
 * costs an add per access.
 */
s32 func_ov039_020933f8(TaskPool* pool, Task* self, void* arg) {
    OtuOvbg* data = (OtuOvbg*)self->data;
    s32      i;

    for (i = 0; i < 4; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->screens[i]);
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], data->chars[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], data->palettes[i]);
    }

    DatMgr_ReleaseData(data->fileData);
    return 1;
}

/**
 * @brief The background task's stage dispatcher, 0x02093460.
 *
 * The `TaskStages` table is copied to the stack rather than indexed in place,
 * which is why the target spends four instructions on an `ldm`/`stm` pair
 * before dispatching. `stage` is the fourth argument, which is what
 * `EasyTask` passes when it wants one stage run rather than the whole
 * lifecycle.
 */
s32 func_ov039_02093460(TaskPool* pool, Task* self, void* arg, s32 stage) {
    TaskStages stages = data_ov039_020999c0;

    return stages.iter[stage](pool, self, arg);
}

/**
 * @brief Creates the background task, 0x020934a8.
 *
 * The three arguments are gathered into a stack block and handed to
 * `EasyTask_CreateTask` as its `param`; the init stage 02092f88 reads them back
 * out of there. Priority is 0 and the parent is NULL, so the caller supplies
 * the pool and nothing else.
 *
 * Typed as returning the handle rather than void: a caller elsewhere in this
 * overlay stores the result into a child-handle field. It costs nothing
 * here -- the body has no `mov r0` of its own in either spelling, so the
 * handle already comes back in r0 and both compile to the same
 * instructions. Same finding as func_ov039_02098394.
 */
s32 func_ov039_020934a8(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;

    return EasyTask_CreateTask(pool, &data_ov039_020999b4, NULL, 0, NULL, &args);
}

/**
 * @brief Re-points the animated palette at the overlay's own table, 0x020934e0.
 *
 * Idempotent: if the object at +0x840 is already running `data_ov039_02099a18`
 * this returns without touching it, so a caller can run it every frame. The
 * source pointer handed over is offset 0x48 into the loaded file's buffer --
 * the same expression init stage 02092f88 uses when it first builds the object,
 * so both agree on which half of the file the animation reads.
 *
 * The `count` argument is 0x12, and it is a count of table entries rather than
 * frames: `func_ov039_02098dbc` steps through the table one entry per call and
 * wraps at this value.
 */
void func_ov039_020934e0(OtuOvbg* data) {
    if (data->paletteAnim.table == data_ov039_02099a18) {
        return;
    }

    func_ov039_02098d7c(&data->paletteAnim, data->fileData ? (s32)OtuPaletteSource(data->fileData) : 0, data_ov039_02099a18,
                        0x12);
}
