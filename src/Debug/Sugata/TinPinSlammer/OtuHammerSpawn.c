#include "OtuFieldAccessShared.h"

/*
 * ov039 region 0x02098394 - 0x02098bb0: the tail of the hammerhahen task, the
 * wrwait task, and the scene stage sequencer's helpers. The shared types,
 * externs and prototypes are in OtuFieldAccessShared.h.
 */

/* ==================================================================== */
/* Tsk_OtosuGame_hammerhahen (continued from OtuSpriteTasks.c)          */
/* ==================================================================== */

s32 func_ov039_02098394(TaskPool* pool, s32 dataType, s32 pinId) {
    OtuPinSpriteArgs args;

    args.dataType = dataType;
    args.childId  = pinId;

    return EasyTask_CreateTask(pool, &data_ov039_0209a024, NULL, 0, NULL, &args);
}

/**
 * Launches a hammer fragment from `at` with the hammer's current affine, a
 * random spin, and animation 2 for the first fragment and 1 for the rest.
 */
// Nonmatching: 90.6%. The target loads `at`'s halves with two `ldr`s; this
// build merges them into an `ldmia`, which also swaps their registers.
void func_ov039_020983c8(OtuHammerHahen* self, OtuPoint* at, OamAffineParam* affine, s32 index) {
    s32 ax;
    s32 ay;
    s32 cell;
    s32 airtime;

    self->active = 1;
    self->affine = *affine;

    ax = at->x;
    ay = at->y;

    self->pos.x  = ax;
    self->pos.y  = ay;
    self->height = 0;

    self->speed = data_ov039_0209a314;
    self->speed = self->speed + RNG_Next(data_ov039_0209a328 - data_ov039_0209a314);

    self->vz = -data_ov039_0209a2fc;
    self->vz = self->vz - RNG_Next(data_ov039_0209a30c - data_ov039_0209a2fc);

    airtime = OTU_ABS_AIRTIME(self->vz);

    self->lifeMax = airtime;
    self->life    = airtime;

    cell        = RNG_Next(0x10000) >> 4;
    self->dir.x = ((s16*)data_0205e4e0)[cell * 2 + 1];
    self->dir.y = ((s16*)data_0205e4e0)[cell * 2];

    self->spin = RNG_Next(0x800) + 0x800;

    Sprite_ChangeAnimation(&self->sprite, self->sprite.animData, (s16)(index == 0 ? 2 : 1), self->sprite.cellTable);
}

/* ==================================================================== */
/* Tsk_OtosuGame_wrwait                                                 */
/* ==================================================================== */

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

/* ==================================================================== */
/* The scene's stage sequencer (OtuStageDispatch).                      */
/* ==================================================================== */

/** Resets the sequencer. The stores are in the target's 0/4/C/10/8/14 order. */
void func_ov039_02098a20(OtuStageDispatch* dispatch, Heap* heap) {
    dispatch->heap       = heap;
    dispatch->stage      = NULL;
    dispatch->next       = NULL;
    dispatch->pending    = FALSE;
    dispatch->stageIndex = 0;
    dispatch->stageBlock = NULL;
}

/** Queues `next` to be entered by the next func_ov039_02098a60. */
void func_ov039_02098a40(OtuStageDispatch* dispatch, const OtuSceneStage* next) {
    dispatch->next    = next;
    dispatch->pending = TRUE;
}

/** Moves on to the stage's next step. */
void func_ov039_02098a50(OtuStageDispatch* dispatch) {
    dispatch->stageIndex = dispatch->stageIndex + 1;
}

/** Enters the queued stage: allocates its block and runs its `enter`. */
void func_ov039_02098a60(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage;
    OtuStageHandler      enter;

    if (dispatch->pending == FALSE) {
        return;
    }

    dispatch->stage      = dispatch->next;
    dispatch->stageIndex = 0;
    dispatch->next       = NULL;
    dispatch->pending    = FALSE;

    stage = dispatch->stage;
    if (stage->blockSize != 0) {
        dispatch->stageBlock = Mem_AllocHeapTail(dispatch->heap, stage->blockSize);
    }

    stage = dispatch->stage;
    if (stage != NULL && (enter = stage->enter) != NULL) {
        enter(scene);
    }
}

/** Runs the current stage's current step. */
void func_ov039_02098acc(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage = dispatch->stage;

    if (stage == NULL) {
        return;
    }

    stage->steps[dispatch->stageIndex](scene);
}

/**
 * Exits the current stage before a queued switch: runs its `exit` and frees its
 * block. The handler test is one `&&` chain, which is the target's `ldrne`.
 */
void func_ov039_02098af4(OtuStageDispatch* dispatch, void* scene) {
    const OtuSceneStage* stage;
    void*                block;
    OtuStageHandler      exit;

    if (dispatch->pending == FALSE) {
        return;
    }

    stage = dispatch->stage;
    if (stage != NULL && (exit = stage->exit) != NULL) {
        exit(scene);
    }

    block = dispatch->stageBlock;
    if (block == NULL) {
        return;
    }

    Mem_Free(dispatch->heap, block);
    dispatch->stageBlock = NULL;
}

/** True when the sequencer is pending a switch to no stage at all. */
s32 func_ov039_02098b44(OtuStageDispatch* dispatch) {
    if (dispatch->next == NULL && dispatch->pending != FALSE) {
        return 1;
    }
    return 0;
}

const OtuSceneStage* func_ov039_02098b68(OtuStageDispatch* dispatch) {
    return dispatch->stage;
}

void* func_ov039_02098b70(OtuStageDispatch* dispatch) {
    return dispatch->stageBlock;
}

/** dst = src. */
void func_ov039_02098b78(OtuPoint* src, OtuPoint* dst) {
    dst->x = src->x;
    dst->y = src->y;
}

/** out = a + b. */
void func_ov039_02098b8c(OtuPoint* a, OtuPoint* b, OtuPoint* out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}
