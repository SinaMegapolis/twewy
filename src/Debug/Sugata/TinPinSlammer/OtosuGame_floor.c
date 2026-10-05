/**
 * @file OtosuGame_floor.c
 * @brief The `Tsk_OtosuGame_floor` task.
 */

#include "OtuFieldAccessShared.h"

/**
 * Stamps a 4x4 block of tile numbers into a 32-wide map. `value` is a u16
 * narrowed after every increment.
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
 * Builds the floor: loads the layout kind's tile art and the five animated
 * palettes, allocates one 0x800-byte tile cell per 8x8 block of the layout,
 * stamps every layout cell's 4x4 tile block into its cell and hands the cell
 * table to the BG map.
 *
 * The layout's kind byte is re-read at every use; held in a local it costs a
 * stack slot the target does not have.
 */
// Nonmatching: 88.3%, register naming and two extra stack words.
s32 func_ov039_02091b98(TaskPool* pool, Task* task, OtuBoardArgs* args) {
    OtuFloor* self = task->data;
    Data*     data;
    Data*     data2;
    u8*       pal;
    u8*       chr;
    u8*       scr;
    Heap*     heap;
    s32       row;
    s32       row2;
    s32       cellsWide;
    s32       cellsHigh;
    s32       i;
    s32       j;
    s32       run;
    u8        wide;
    s32       col;

    self->loaded   = 1;
    self->dataType = args->dataType;
    self->heap     = args->heap;
    self->layout   = args->layout;

    s32 sheetTable[3]    = {2, 3, 4};
    s32 styleTable[3][3] = {
        {1, 2, 3},
        {1, 2, 3},
        {1, 2, 3}
    };
    s32 miscTable[3] = {4, 4, 4};

    heap = self->heap;

    data       = DatMgr_LoadRawData(args->dataType, NULL, 0, &data_ov039_0209a0b4[sheetTable[self->layout->kind]]);
    self->data = data;

    row2 = miscTable[self->layout->kind];

    row  = styleTable[self->layout->kind][self->layout->variant];
    wide = self->layout->width;

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

        self->palette = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 5);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->chars = BgResMgr_AllocChar32(g_BgResourceManagers[0], chr,
                                               g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        PaletteMgr_Flush(g_PaletteManagers[0], self->palette);
    }

    data2          = DatMgr_LoadRawData(args->dataType, NULL, 0, data_ov039_0209a0bc);
    self->animData = data2;

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x10);
        }
        self->animPalettes[0].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[0].anim, (s32)pal, data_ov039_0209964c, 8), 0, 6, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[0].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x18);
        }
        self->animPalettes[1].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[1].anim, (s32)pal, data_ov039_02099620, 4), 0, 7, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[1].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x20);
        }
        self->animPalettes[2].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[2].anim, (s32)pal, data_ov039_0209966c, 8), 0, 8, 1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[2].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x28);
        }
        self->animPalettes[3].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[3].anim, (s32)pal, data_ov039_020996b0, 0x19), 0, 9,
            1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[3].palette);
    }

    {
        if (data2 == NULL) {
            pal = NULL;
        } else {
            u8* base = (u8*)data2->buffer + 0x20;
            pal      = base + *(u32*)(base + 0x30);
        }
        self->animPalettes[4].palette = PaletteMgr_AllocPalette(
            g_PaletteManagers[0], func_ov039_02098d7c(&self->animPalettes[4].anim, (s32)pal, data_ov039_02099630, 7), 0, 0xA,
            1);
        PaletteMgr_Flush(g_PaletteManagers[0], self->animPalettes[4].palette);
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
            self->animChars = BgResMgr_AllocChar32(g_BgResourceManagers[0], chr,
                                                   g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0x300, size);
        }
    }

    cellsWide = (wide + 7 + ((u32)((wide + 7) >> 2) >> 0x1D)) >> 3;
    cellsHigh = (self->layout->height + 7 + ((u32)((self->layout->height + 7) >> 2) >> 0x1D)) >> 3;

    self->tilePool = Mem_AllocHeapTail(heap, cellsHigh * (cellsWide << 0xB));
    self->tiles    = Mem_AllocHeapTail(heap, cellsHigh * (cellsWide * 4));
    MI_CpuSet(self->tilePool, 0, Mem_GetBlockSize(heap, self->tilePool));

    if (cellsHigh > 0) {
        i   = 0;
        run = 0;
        do {
            j = 0;
            if (cellsWide > 0) {
                do {
                    self->tiles[run + j] = self->tilePool + (j + run) * 0x800;
                    j                    = j + 1;
                } while (j < cellsWide);
            }
            i   = i + 1;
            run = run + cellsWide;
        } while (i < cellsHigh);
    }

    if (self->layout->height > 0) {
        col = 0;
        do {
            if (self->layout->width > 0) {
                j = 0;
                do {
                    u8* cellBase = self->layout->cells + (col * self->layout->width + j) * 2;
                    u8  cellByte = cellBase[1];
                    u16 tileVal  = *(u16*)(data_ov039_0209a5d8[self->layout->kind] + cellByte * 2);

                    func_ov039_02091b40(
                        tileVal, (u16*)(self->tiles[(j / 8) * cellsWide + (col / 8)] + ((j % 8) * 4 + (col % 8) * 128) * 2));
                    j = j + 1;
                } while (j < (s32)self->layout->width);
            }
            col = col + 1;
        } while (col < (s32)self->layout->height);
    }

    func_0200d1d8(&self->bg, 0, 1, 0, self->tiles, cellsWide, cellsHigh);
    return 1;
}

/** Steps the five animated palettes. Unrolled, as the target has it. */
s32 func_ov039_02092154(TaskPool* pool, Task* task, void* args) {
    OtuFloor* self = task->data;

    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[0].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[0].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[1].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[1].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[2].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[2].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[3].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[3].anim));
    PaletteMgr_SetSource(g_PaletteManagers[0], self->animPalettes[4].palette,
                         (void*)func_ov039_02098dbc(&self->animPalettes[4].anim));
    return 1;
}

/** The floor's render stage: the BG draws itself. */
s32 func_ov039_020921f4(void) {
    return 1;
}

s32 func_ov039_020921fc(TaskPool* pool, Task* task, void* args) {
    OtuFloor* self = task->data;

    func_0200d954(0, 1);

    Mem_Free(self->heap, self->tiles);
    Mem_Free(self->heap, self->tilePool);

    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->chars);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->animChars);

    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[0].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[1].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[2].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[3].palette);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->animPalettes[4].palette);

    DatMgr_ReleaseData(self->data);
    DatMgr_ReleaseData(self->animData);
    return 1;
}

s32 func_ov039_020922c8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_02099610;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_02092310(TaskPool* pool, s32 dataType, Heap* heap, OtuBoardLayout* layout) {
    OtuBoardArgs args;

    args.dataType = dataType;
    args.heap     = heap;
    args.layout   = layout;
    return EasyTask_CreateTask(pool, &data_ov039_020995ec, NULL, 0, NULL, &args);
}

/**
 * Scrolls the floor's BG layer to (x, y), flagging an affine layer for update.
 * The bgMode test is a switch: the target has a six-entry jump table.
 */
// Nonmatching: 98.5%, one register swap between the engine index and the
// engine-state pointer; ten source shapes were tried.
void func_ov039_02092348(OtuFloor* self, s32 x, s32 y) {
    s32                 a     = self->bg.display;
    s32                 b     = self->bg.layer;
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

/** Marks the floor's BG map dirty when `event` is set. */
void func_ov039_020923b4(OtuFloor* self, s32 event) {
    if (event != 0) {
        self->bg.flags |= 2;
    }
}
