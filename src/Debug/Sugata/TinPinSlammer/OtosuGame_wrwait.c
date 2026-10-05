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
    } bg[2];        // one per display
} OtosuGame_wrwait; // Size: 0x64

/* The task's handle, stage table and sprite template. */
extern const TaskHandle      Tsk_OtosuGame_wrwait;
extern const TaskStages      data_ov039_0209a078;
extern const SpriteAnimation OtosuGame_wrwait_Anim;

/** The 0x27-sized bin the loader pulls, plus its three sub-objects. */
extern const BinIdentifier data_ov039_0209a0e4;

SpriteFrameInfo* OtosuGame_wrwait_GetFrameInfo(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallbackSorted(sprite, mode, 3);
}

void OtosuGame_wrwait_Load(OtosuGame_wrwait* self, Sprite* sprite) {
    SpriteAnimation anim = OtosuGame_wrwait_Anim;

    anim.owner    = self;
    anim.dataType = (u16)self->dataType;
    _Sprite_Load(sprite, &anim);
}

/**
 * Shows BG1 on both displays, loads the wait screen's palette, char and screen
 * data into it, loads the sprite and fades both displays in.
 */
s32 OtosuGame_wrwait_Init(TaskPool* pool, Task* task, OtuTaskArgs1* args) {
    DisplayEngineState* state;
    OtosuGame_wrwait*   self = task->data;
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

    OtosuGame_wrwait_Load(self, &self->sprite);
    EasyFade_FadeMainDisplay(2, 0, 0x1000);
    EasyFade_FadeSubDisplay(2, 0, 0x1000);
    return 1;
}

s32 OtosuGame_wrwait_Update(TaskPool* pool, Task* task, void* args) {
    OtosuGame_wrwait* self = task->data;

    if (self->visible != 0) {
        Sprite_Update(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_wrwait_Render(TaskPool* pool, Task* task, void* args) {
    OtosuGame_wrwait* self = task->data;

    if (self->visible != 0) {
        Sprite_RenderFrame(&self->sprite);
    }
    return 1;
}

s32 OtosuGame_wrwait_Destroy(TaskPool* pool, Task* task, void* args) {
    OtosuGame_wrwait* self = task->data;

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

s32 OtosuGame_wrwait_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = data_ov039_0209a078;

    return stages.iter[stage](pool, task, args);
}

s32 OtosuGame_wrwait_CreateTask(TaskPool* pool, s32 dataType) {
    OtuTaskArgs1 args;

    args.dataType = dataType;
    return EasyTask_CreateTask(pool, &Tsk_OtosuGame_wrwait, NULL, 0, NULL, &args);
}
