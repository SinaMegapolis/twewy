/**
 * @file OtosuGame_wrwait.c
 * @brief The `Tsk_OtosuGame_wrwait` task.
 */

#include "OtuFieldAccessShared.h"

/** "Tsk_OtosuGame_wrwait": the wireless "please wait" screen, a BG on both displays. */
typedef struct {
    /* 0x00 */ s32    dataType;
    /* 0x04 */ Sprite sprite;
    /* 0x44 */ s32    visible;
    /* 0x48 */ Data*  data;
    /* 0x4C */ struct {
        PaletteResource* palette;
        BgResource*      chars;
        BgResource*      screen;
    } bg[2]; // one per display
} OtuWrwait; // Size: 0x64

/* The task's handle, stage table and sprite template. */
extern const TaskHandle      data_ov039_0209a06c;
extern const TaskStages      data_ov039_0209a078;
extern const SpriteAnimation data_ov039_0209a088;

/** The 0x27-sized bin the loader pulls, plus its three sub-objects. */
extern const BinIdentifier data_ov039_0209a0e4;

SpriteFrameInfo* func_ov039_02098538(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

void func_ov039_020985e0(OtuWrwait* self, Sprite* sprite) {
    SpriteAnimation anim = data_ov039_0209a088;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    _Sprite_Load(sprite, &anim);
}

/**
 * Shows BG1 on both displays, loads the wait screen's palette, char and screen
 * data into it, loads the sprite and fades both displays in.
 */
s32 func_ov039_02098650(TaskPool* pool, Task* task, OtuTaskArgs1* args) {
    DisplayEngineState* state;
    OtuWrwait*          self = task->data;
    Data*               data;

    self->visible  = 1;
    self->dataType = args->dataType;

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

    data       = DatMgr_LoadRawData(args->dataType, NULL, 0, &data_ov039_0209a0e4);
    self->data = data;

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

        self->bg[0].palette = PaletteMgr_AllocPalette(g_PaletteManagers[0], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->bg[0].chars = BgResMgr_AllocChar32(g_BgResourceManagers[0], chr,
                                                     g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, size);
        }
        self->bg[0].screen =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], scr, g_DisplaySettings.engineState[0].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[0].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[0], self->bg[0].palette);

        self->bg[1].palette = PaletteMgr_AllocPalette(g_PaletteManagers[1], pal, 0, 0, 1);
        {
            u32 size = (u32)((*(u32*)chr & ~0xFF) >> 8);

            if ((*(u8*)chr & 0xF0) == 0) {
                size -= 4;
            }
            self->bg[1].chars = BgResMgr_AllocChar32(g_BgResourceManagers[1], chr,
                                                     g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, size);
        }
        self->bg[1].screen =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], scr, g_DisplaySettings.engineState[1].bgSettings[1].screenBase,
                                 (u32)g_DisplaySettings.engineState[1].bgSettings[1].screenSizeText);
        PaletteMgr_Flush(g_PaletteManagers[1], self->bg[1].palette);
    }

    func_ov039_020985e0(self, &self->sprite);
    EasyFade_FadeMainDisplay(2, 0, 0x1000);
    EasyFade_FadeSubDisplay(2, 0, 0x1000);
    return 1;
}

s32 func_ov039_020988d8(TaskPool* pool, Task* task, void* args) {
    OtuWrwait* self = task->data;

    if (self->visible != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

s32 func_ov039_020988fc(TaskPool* pool, Task* task, void* args) {
    OtuWrwait* self = task->data;

    if (self->visible != 0) {
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 func_ov039_02098920(TaskPool* pool, Task* task, void* args) {
    OtuWrwait* self = task->data;

    BgResMgr_ReleaseScreen(g_BgResourceManagers[0], self->bg[0].screen);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->bg[0].chars);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->bg[0].palette);

    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], self->bg[1].screen);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], self->bg[1].chars);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->bg[1].palette);

    DatMgr_ReleaseData(self->data);
    Sprite_Release(&self->sprite);
    return 1;
}

s32 func_ov039_020989a8(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_0209a078;

    return stages.iter[stage](pool, task, args);
}

s32 func_ov039_020989f0(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &data_ov039_0209a06c, NULL, 0, NULL, &args);
}
