#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02092484 - 0x020933c0: the tail of the obstacle task, the bg
 * task and the head of the ovbg task. The shared types, externs and prototypes
 * are in OtuFieldAccessShared.h.
 */

/*
 * A pack entry of `rec`: the table at `rec->buffer + 0x20` is 8 bytes a row,
 * and each row's first word is an offset from the table. NULL for a missing
 * record or a non-positive index. A macro because it appears three times and a
 * helper would be a `bl`; the NULL case is the `then` because that is the
 * target's block layout.
 */
#define Otu_RES_REF(out, rec, idx)                        \
    do {                                                  \
        if ((rec) == NULL || (idx) <= 0) {                \
            (out) = NULL;                                 \
        } else {                                          \
            u8* _base = (u8*)(rec)->buffer;               \
            u8* _tbl  = _base + 0x20;                     \
            (out)     = _tbl + *(s32*)(_tbl + (idx) * 8); \
        }                                                 \
    } while (0)

/* ==================================================================== */
/* Tsk_OtosuGame_obstacle (continued from OtuMeters.c)                  */
/* ==================================================================== */

/**
 * Loads the obstacle sprite, patching the template's art by `kind` and its
 * palette by `kind` and `slot`. The dataType edit is one expression because
 * mwcc hoists the argument loads above it regardless.
 */
void func_ov039_02092484(OtuObstacle* self, Sprite* sprite, OtuObstacle_Args* args) {
    SpriteAnimation params = data_ov039_0209996c;
    u16             kind   = args->params->kind;

    *(u16*)&params   = (u16)(*(u16*)&params & ~0x3C) | (u16)((u16)args->oamAttrs << 2);
    params.owner     = self;
    params.packIndex = data_ov039_0209992a[kind];
    params.unk_20    = data_ov039_02099958[kind][args->slot];
    params.unk_26    = data_ov039_02099918[kind];
    params.unk_1C    = data_ov039_0209991e[kind];
    params.unk_28    = data_ov039_02099924[kind];
    params.posX      = (s16)((self->pos.x - self->origin.x) >> 12);
    params.posY      = (s16)((self->pos.y - self->origin.y) >> 12);
    _Sprite_Load(sprite, &params);
}

s32 func_ov039_0209258c(TaskPool* pool, Task* task, void* args) {
    OtuObstacle*      self      = task->data;
    OtuObstacle_Args* a         = args;
    s32               scales[3] = {0x20000, 0x18000, 0x10000}; // 16.16, per kind

    self->origin.x    = 0;
    self->origin.y    = 0;
    self->pos.x       = (s32)a->params->targetX << 12;
    self->pos.y       = (s32)a->params->targetY << 12;
    self->scale       = scales[a->params->kind];
    self->animPending = 0;

    func_ov039_02092484(self, &self->sprite, a);
    return 1;
}

/** Switches to the hit animation once when asked, then steps the sprite. */
s32 func_ov039_02092608(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

    if (self->animPending != 0) {
        self->sprite.animationMode = ANIM_MODE_ONCE;
        Sprite_SetAnimation(&self->sprite, self->sprite.animData, 2, self->sprite.cellTable);
        self->animPending = 0;
    }

    Sprite_Update(&self->sprite);
    return 1;
}

s32 func_ov039_02092658(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

    self->sprite.posX = (s16)((self->pos.x - self->origin.x) >> 12);
    self->sprite.posY = (s16)((self->pos.y - self->origin.y) >> 12);

    Sprite_RenderFrame(&self->sprite);
    return 1;
}

s32 func_ov039_02092694(TaskPool* pool, Task* task, void* args) {
    OtuObstacle* self = task->data;

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

/** Sets the point the obstacle is drawn relative to. */
void func_ov039_02092730(OtuObstacle* self, OtuPoint* origin) {
    self->origin = *origin;
}

void func_ov039_02092744(OtuObstacle* self, OtuPoint* out) {
    *out = self->pos;
}

s32 func_ov039_02092758(OtuObstacle* self) {
    return self->scale;
}

/** Plays the hit animation on the next update. */
void func_ov039_02092760(OtuObstacle* self) {
    self->animPending = 1;
}

/* ==================================================================== */
/* Tsk_OtosuGame_bg                                                     */
/* ==================================================================== */

/**
 * Loads one of the bg task's two layers: its palette, chars and cell data,
 * allocated against BG layer `params->group` on the main display.
 *
 * `PaletteMgr_AllocPaletteNoProto` because `palStart` is a word going into an
 * s16 parameter, and the record is stored and re-read rather than kept in a
 * local: both are what the target does.
 */
// Nonmatching: 86.7%, register naming in the first block.
void func_ov039_0209276c(OtuBg* self, OtuResParams* params) {
    Data* rec;
    u8*   ref0;
    u8*   ref1;
    u8*   ref2;
    s32   size;

    self->data[params->slot] =
        DatMgr_LoadRawData(self->dataType, NULL, 0, (BinIdentifier*)&data_ov039_0209a0b4[params->fileId]);
    rec = self->data[params->slot];

    /* The first index is read above the NULL test, as the target has it. */
    {
        s32 idxA = params->idx[self->layout->variant];

        Otu_RES_REF(ref0, rec, idxA);
    }
    Otu_RES_REF(ref1, rec, params->idx1);
    Otu_RES_REF(ref2, rec, params->idx2);

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

    func_0200d1d8(&self->maps[params->slot], 0, params->group, 0, self->buffers[params->slot], params->width, params->height);

    func_0200d858(&self->maps[params->slot], 1, 1, 0);

    self->widths[params->slot]  = params->width << 20;
    self->heights[params->slot] = params->height << 20;

    if (params->layers >= 0) {
        g_DisplaySettings.controls[0].layers |= params->layers;
    }

    g_DisplaySettings.engineState[0].bgSettings[params->group].priority = params->priority;
}

/**
 * Loads both layers for the layout's kind, if it has any. The `active` store
 * is predicated in the target, so the kind-0 case is the `else`.
 */
s32 func_ov039_020929ec(TaskPool* pool, Task* task, OtuBoardArgs* args) {
    OtuBg* self = task->data;
    s32    i;

    self->dataType = args->dataType;
    self->heap     = args->heap;
    self->layout   = args->layout;

    if (self->layout->kind != 0) {
        self->active = 1;
    } else {
        self->active = 0;
        return 1;
    }

    for (i = 0; i < 2; i++) {
        func_ov039_0209276c(self, data_ov039_0209a620[self->layout->kind][i]);
        self->scrollX[i] = 0;
        self->scrollY[i] = 0;
    }
    return 1;
}

/**
 * Scrolls the layers, wrapping at their size and flagging the map for upload.
 * Kind 1 scrolls layer 1 slowly; kind 2 also scrolls layer 0 twice as fast.
 */
s32 func_ov039_02092a78(TaskPool* pool, Task* task, void* args) {
    OtuBg* self = task->data;
    s32    limit;
    s32    v;

    if (self->active == 0) {
        return 1;
    }

    switch (self->layout->kind) {
        case 1:
            v                = self->scrollX[1] + 0x19A;
            self->scrollX[1] = v;
            limit            = self->widths[1];
            if (v >= limit) {
                self->scrollX[1] %= limit;
                self->maps[1].flags |= 2;
            }

            v                = self->scrollY[1] + 0x19A;
            self->scrollY[1] = v;
            limit            = self->heights[1];
            if (v >= limit) {
                self->scrollY[1] %= limit;
                self->maps[1].flags |= 2;
            }
            break;

        case 2:
            v                = self->scrollX[0] + 0x333;
            self->scrollX[0] = v;
            limit            = self->widths[0];
            if (v >= limit) {
                self->scrollX[0] %= limit;
                self->maps[0].flags |= 2;
            }

            v                = self->scrollY[0] + 0x333;
            self->scrollY[0] = v;
            limit            = self->heights[0];
            if (v >= limit) {
                self->scrollY[0] %= limit;
                self->maps[0].flags |= 2;
            }

            v                = self->scrollX[1] + 0x19A;
            self->scrollX[1] = v;
            limit            = self->widths[1];
            if (v >= limit) {
                self->scrollX[1] %= limit;
                self->maps[1].flags |= 2;
            }

            v                = self->scrollY[1] + 0x19A;
            self->scrollY[1] = v;
            limit            = self->heights[1];
            if (v >= limit) {
                self->scrollY[1] %= limit;
                self->maps[1].flags |= 2;
            }
            break;

        default:
            break;
    }
    return 1;
}

/** The bg task's render stage: the BG layers draw themselves. */
s32 func_ov039_02092bf0(void) {
    return 1;
}

s32 func_ov039_02092bf8(TaskPool* pool, Task* task, void* args) {
    OtuBg* self = task->data;
    s32    i;

    if (self->active == 0) {
        return 1;
    }

    func_0200d954(0, 2);
    func_0200d954(0, 3);

    for (i = 0; i < 2; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->chars[i]);
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->palettes[i]);
        Mem_Free(self->heap, self->buffers[i]);
        DatMgr_ReleaseData(self->data[i]);
    }
    return 1;
}

s32 func_ov039_02092c8c(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_020999a4;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02092cd4(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;
    return EasyTask_CreateTask(pool, &data_ov039_02099998, NULL, 0, NULL, &args);
}

/**
 * Scrolls both layers to (x, y) plus their own scroll, flagging affine layers
 * for update. Written out twice: a helper would be a `bl`.
 */
// Nonmatching: 98.5%, the same register swap as func_ov039_02092348.
void func_ov039_02092d0c(OtuBg* self, s32 x, s32 y) {
    DisplayEngineState* group;
    s32                 b;
    s32                 px;
    s32                 py;

    if (self->active == 0) {
        return;
    }

    b     = self->maps[0].layer;
    px    = x + self->scrollX[0];
    py    = y + self->scrollY[0];
    group = &g_DisplaySettings.engineState[self->maps[0].display];
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

    b     = self->maps[1].layer;
    px    = x + self->scrollX[1];
    py    = y + self->scrollY[1];
    group = &g_DisplaySettings.engineState[self->maps[1].display];
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

/** Marks both layers' maps dirty when `event` is set. */
void func_ov039_02092e04(OtuBg* self, s32 event) {
    s32  i;
    s32* flags;

    if (event == 0) {
        return;
    }

    flags = &self->maps[0].flags;
    for (i = 0; i < 2; i++) {
        *flags |= 2;
        flags += sizeof(OtuBgMap) / sizeof(s32);
    }
}

/* ==================================================================== */
/* Tsk_OtosuGame_ovbg (continued in OtuCounters.c)                      */
/* ==================================================================== */

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
