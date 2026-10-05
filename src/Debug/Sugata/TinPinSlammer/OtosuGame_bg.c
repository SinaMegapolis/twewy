/**
 * @file OtosuGame_bg.c
 * @brief The `Tsk_OtosuGame_bg` task.
 */

#include "OtuFieldAccessShared.h"

extern const TaskHandle Tsk_OtosuGame_bg;
extern const TaskStages data_ov039_020999a4;

/* The 7- and 4-argument cell set-up pair func_ov039_0209276c drives. */
void func_0200d898(void* buf, void* src, s32 w, s32 h);
void func_0200d858(void* obj, s32 a, s32 b, s32 c);

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
} OtuResParams;

/* Per layout kind, the bg task's two layers. */
extern OtuResParams** data_ov039_0209a620[];

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

/**
 * Loads one of the bg task's two layers: its palette, chars and cell data,
 * allocated against BG layer `params->group` on the main display.
 *
 * `PaletteMgr_AllocPaletteNoProto` because `palStart` is a word going into an
 * s16 parameter, and the record is stored and re-read rather than kept in a
 * local: both are what the target does.
 */
// Nonmatching: 86.7%, register naming in the first block.
void func_ov039_0209276c(OtosuGame_bg* self, OtuResParams* params) {
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
s32 OtosuGame_bg_Init(TaskPool* pool, Task* task, OtuBoardArgs* args) {
    OtosuGame_bg* self = task->data;
    s32           i;

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
s32 OtosuGame_bg_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_bg* self = task->data;
    s32           limit;
    s32           v;

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
s32 OtosuGame_bg_Render(void) {
    return 1;
}

s32 OtosuGame_bg_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_bg* self = task->data;
    s32           i;

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

s32 OtosuGame_bg_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_020999a4;

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_bg_CreateTask(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_bg, NULL, 0, NULL, &args);
}

/**
 * Scrolls both layers to (x, y) plus their own scroll, flagging affine layers
 * for update. Written out twice: a helper would be a `bl`.
 */
// Nonmatching: 98.5%, the same register swap as func_ov039_02092348.
void func_ov039_02092d0c(OtosuGame_bg* self, s32 x, s32 y) {
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
void func_ov039_02092e04(OtosuGame_bg* self, s32 event) {
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
