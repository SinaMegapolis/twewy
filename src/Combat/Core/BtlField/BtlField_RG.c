#include "Battle/Core/BtlBgScript.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "Display.h"
#include "Engine/Core/DMA.h"
#include "Engine/EasyTask.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/BgResMgr.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ScreenMapper.h"
#include "Save.h"
#include "SpriteMgr.h"
#include "main_02026528.h"

#include <nitro/g2.h>
#include <nitro/mi/cpumem.h>

// The sub-screen field background of a battle (`Tsk_BtlField_RG`): the twin of `BtlField_UG`. One BG tilemap driven by
// the camera, two floor strips that scroll over it and, for some backgrounds, a third strip with its own characters
// on BG0.

struct BtlFieldRG;

typedef struct BtlFieldRG {
    /* 0x00 */ BattleCamera*    camera;
    /* 0x04 */ Data*            data;
    /* 0x08 */ PaletteResource* palette;
    /* 0x0C */ BgResource*      charResA;
    /* 0x10 */ BgResource*      charResB;
    /* 0x14 */ ScreenMapEntry   rowMaps[2];
    /* 0x64 */ const void*      rowData[2];
    /* 0x6C */ s32              hasExtra; // the third strip is in use
    /* 0x70 */ BgResource*      extraChars;
    /* 0x74 */ ScreenMapEntry   extraMap;
    /* 0x9C */ const void*      extraData[2];
    /* 0xA4 */ s16              timer; // fade-in timer, 0 = idle
    /* 0xA6 */ s16              unk_A6;
    /* 0xA8 */ void (*update)(struct BtlFieldRG* self);
    /* 0xAC */ BattleBgScriptState bgScript;
} BtlFieldRG; // Size: 0x138

static s32 BtlField_RG_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

const TaskHandle Tsk_BtlField_RG = {"Tsk_BtlField_RG", BtlField_RG_RunTask, sizeof(BtlFieldRG)};

/// Resets the fade timer and swaps in the background's scroll-update callback.
static void func_ov003_020929a4(BtlFieldRG* state, void (*update)(BtlFieldRG*)) {
    state->update = update;
    state->unk_A6 = 0;
    state->timer  = 0;
}

/// Sets BG1 and BG2 of the sub engine up as the two text layers the floor strips are drawn on.
static void func_ov003_020929b8(void) {
    DisplayBGSettings* bg;

    bg                 = &g_DisplaySettings.engineState[1].bgSettings[1];
    bg->bgMode         = DISPLAY_BGMODE_TEXT;
    bg->screenSizeText = GX_BG_SIZE_TEXT_512x256;
    bg->colorMode      = GX_BG_COLORS_256;
    bg->screenBase     = 0;
    bg->charBase       = 1;
    bg->extPlttSlot    = 0;
    G2S_SetBG1Control(GX_BG_SIZE_TEXT_512x256, 1, GX_BG_SCRBASE_0x0000, 1, 0);

    bg                 = &g_DisplaySettings.engineState[1].bgSettings[2];
    bg->bgMode         = DISPLAY_BGMODE_TEXT;
    bg->screenSizeText = GX_BG_SIZE_TEXT_512x256;
    bg->colorMode      = GX_BG_COLORS_256;
    bg->screenBase     = 2;
    bg->charBase       = 3;
    bg->extPlttSlot    = 1;
    G2S_SetBG2Control(GX_BG_SIZE_TEXT_512x256, 1, 2, 3, 0);

    g_DisplaySettings.engineState[1].bgSettings[1].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.controls[1].layers |= 6;
}

/// Runs the background script that the save's battle index picks (the first one if it is out of range).
static void BtlField_RG_RunBgScript(BtlFieldRG* state, const BattleBgScriptWord** scripts, u16 count) {
    u16 index = gSaveData.unk_3232;

    if (scripts != NULL) {
        if (index >= count) {
            index = 0;
        }
        BtlBgScript_Run(&state->bgScript, scripts[index], state->data);
    }
}

/// Loads the characters of the third strip. Returns NULL when the background has none.
static BgResource* func_ov003_02092a90(BtlFieldRG* state, u8 index) {
    BgResource* result = NULL;

    if (index != 0xFF) {
        const BtlFieldExtra* extra = &BtlField_RG_Extras[index];
        void*                chars = Data_GetPackEntryData(state->data, extra->charPack);

        result = BgResMgr_AllocCharExtended(g_BgResourceManagers[1], chars,
                                            g_DisplaySettings.engineState[1].bgSettings[0].charBase, 0, extra->charCount);
    }
    return result;
}

/// Sets the third strip up on BG0. Returns 0 when the background has none.
static s32 func_ov003_02092b14(BtlFieldRG* state, u8 index) {
    const BtlFieldExtra* extra;

    if (index == 0xFF) {
        return 0;
    }
    extra               = &BtlField_RG_Extras[index];
    state->extraData[0] = Ov003Cutin_Tiles(state->data, extra->rowIndex0);
    state->extraData[1] = Ov003Cutin_Tiles(state->data, extra->rowIndex1);
    func_0200d1d8(&state->extraMap, 1, 0, NULL, state->extraData, 2, 1);
    func_0200d858(&state->extraMap, 0, 1, NULL);
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.controls[1].layers |= 1;
    return 1;
}

/// Fades the background in with the battle's transition, then hands the task to the live update.
static void func_ov003_02092c10(BtlFieldRG* state) {
    if (state->timer == 0) {
        state->camera->shakeX = -0x48000;
        state->timer          = 100;
    }
    if (state->timer > 0) {
        func_02026590(&state->camera->shakeX, 0, (u16)state->timer);
        state->timer--;
    }
    if (state->timer <= 0) {
        func_ov003_020929a4(state, NULL);
    }
}

/// Eases the camera's view point toward its target, keeping it inside the floor's bounds.
static void func_ov003_02092c88(BattleCamera* camera) {
    s32 dx;
    s32 dy;
    s32 dz;
    s32 xMin, xMax;

    dx = (camera->unk_34 - camera->unk_28) >> 2;
    dy = (camera->unk_38 - camera->unk_2C) >> 2;
    dz = (camera->unk_3C - camera->unk_30) >> 2;

    if (dx > 0x2000) {
        dx = 0x2000;
    } else if (dx < -0x2000) {
        dx = -0x2000;
    }
    if (dy > 0x2000) {
        dy = 0x2000;
    } else if (dy < -0x2000) {
        dy = -0x2000;
    }
    if (dz > 0x2000) {
        dz = 0x2000;
    } else if (dz < -0x2000) {
        dz = -0x2000;
    }

    camera->unk_28 += dx;
    camera->unk_2C += dy;
    camera->unk_30 += dz;
    camera->unk_40 = camera->shakeX + camera->unk_2C + camera->unk_30;

    xMin = camera->unk_04 + 0x80000;
    xMax = camera->unk_04 - 0x80000 + camera->unk_0C;
    if (camera->unk_28 < xMin) {
        camera->unk_28 = xMin;
    } else if (camera->unk_28 > xMax) {
        camera->unk_28 = xMax;
    }
    if (camera->unk_40 < 0x178000) {
        camera->unk_40 = 0x178000;
    } else if (camera->unk_40 > 0x198000) {
        camera->unk_40 = 0x198000;
    }
    camera->unk_28 = camera->unk_28 + camera->unk_44;
    camera->unk_40 += camera->unk_48 + camera->unk_4C;
}

/// Parks the strips behind the camera.
static void func_ov003_02092dbc(BtlFieldRG* state) {
    BtlField_ScrollRow(&state->rowMaps[0], state->camera, 0x80000, 0x60000);
    BtlField_ScrollRow(&state->rowMaps[1], state->camera, 0x180000, 0x60000);
    if (!(state->hasExtra == 0)) {
        BtlField_ScrollRow(&state->extraMap, state->camera, 0x80000, 0x60000);
    }
}

/// Fetches the background's resources and seeds the tilemaps.
static s32 BtlField_RG_Init(TaskPool* pool, Task* task, void* args) {
    BtlFieldRG*          state = task->data;
    const BtlFieldDefRG* def;
    void*                palData;
    void*                charDataA;
    void*                charDataB;

    MI_CpuSet(state, NULL, sizeof(*state));
    func_ov003_020929b8();
    state->camera = &g_CombatGlobal->unk3D7C0[1];
    state->camera->unk_02 |= 1;

    def = &BtlField_RG_Defs[state->camera->pathId];
    BtlBgScript_Init(&state->bgScript, 1, def->iden);

    state->data = DatMgr_LoadPackEntry(1, NULL, 0, def->iden, def->charPack, 0);
    palData     = Data_GetPackEntryData(state->data, def->paletteIndex);
    charDataA   = Data_GetPackEntryData(state->data, def->charIndexA);
    charDataB   = Data_GetPackEntryData(state->data, def->charIndexB);

    state->palette    = PaletteMgr_AllocPalette(g_PaletteManagers[1], palData, 0, def->palCount, 0);
    state->charResA   = BgResMgr_AllocCharExtended(g_BgResourceManagers[1], charDataA,
                                                   g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, def->charCountA);
    state->charResB   = BgResMgr_AllocCharExtended(g_BgResourceManagers[1], charDataB,
                                                   g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, def->charCountB);
    state->extraChars = func_ov003_02092a90(state, def->extraIndex);
    PaletteMgr_Flush(g_PaletteManagers[1], state->palette);
    DMA_Flush();

    BtlField_RG_RunBgScript(state, def->scripts, def->scriptCount);
    DatMgr_ReleaseData(state->data);

    state->data = DatMgr_LoadPackEntry(1, NULL, 0, def->iden, def->rowPack, 0);
    // The strips start after the 4-byte header of their pack entries.
    state->rowData[0] = Ov003Cutin_Tiles(state->data, def->rowIndex0);
    state->rowData[1] = Ov003Cutin_Tiles(state->data, def->rowIndex1);

    func_0200d1d8(&state->rowMaps[0], 1, 1, NULL, &state->rowData[0], 1, 1);
    func_0200d1d8(&state->rowMaps[1], 1, 2, NULL, &state->rowData[1], 1, 1);
    func_0200d858(&state->rowMaps[0], 0, 1, NULL);
    func_0200d858(&state->rowMaps[1], 0, 1, NULL);
    state->hasExtra = func_ov003_02092b14(state, def->extraIndex);

    if (*(u16*)args == 0) {
        state->camera->unk_28 = state->camera->unk_34;
        state->camera->unk_2C = state->camera->unk_38;
        state->camera->unk_30 = state->camera->unk_3C;
        func_ov003_020929a4(state, func_ov003_02092c10);
    } else {
        state->camera->shakeX = 0;
        func_ov003_020929a4(state, NULL);
    }
    func_ov003_02092dbc(state);
    return 1;
}

/// Advances the background every frame.
static s32 BtlField_RG_Update(TaskPool* pool, Task* task, void* args) {
    BtlFieldRG* state = task->data;

    if (g_CombatGlobal->unk3D878 & 0x1000) {
        return 0;
    }
    if (state->update != NULL) {
        state->update(state);
    }
    BtlBgScript_Update(&state->bgScript);
    func_ov003_02092c88(state->camera);
    return 1;
}

/// Re-parks the background behind the camera.
static s32 BtlField_RG_Render(TaskPool* pool, Task* task, void* args) {
    func_ov003_02092dbc(task->data);
    return 1;
}

/// Releases the background's resources.
static s32 BtlField_RG_Release(TaskPool* pool, Task* task, void* args) {
    BtlFieldRG* state = task->data;

    BgResMgr_ReleaseChar(g_BgResourceManagers[1], state->charResA);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], state->charResB);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], state->palette);
    func_0200d954(1, 1);
    func_0200d954(1, 2);
    BtlBgScript_Release(&state->bgScript);
    if (state->extraChars != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], state->extraChars);
    }
    if (state->hasExtra != 0) {
        func_0200d954(1, 0);
    }
    DatMgr_ReleaseData(state->data);
    state->camera->unk_02 &= ~1;
    return 1;
}

static s32 BtlField_RG_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlField_RG_Init,
        .update     = BtlField_RG_Update,
        .render     = BtlField_RG_Render,
        .cleanup    = BtlField_RG_Release,
    };

    return stages.iter[stage](pool, task, args);
}
