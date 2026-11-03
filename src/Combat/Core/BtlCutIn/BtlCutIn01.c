#include "Combat/Core/BtlCutIn.h"
#include "Combat/Core/CombatCutin.h"
#include "Display.h"
#include "EasyFade.h"
#include "Engine/Resources/ScreenMapper.h"
#include "Player/Stats.h"
#include <nitro/mi/cpumem.h>
#include <nitro/reg.h>

// The first fusion cut-in (`Tsk_BtlCutIn01`): Neku and the partner slide in from opposite sides, strike a pose
// and fade out. The task runs a fixed program of steps; each step returns 0 to wait for the next frame, 1 to end
// the task, or 2 to run the following step immediately.

typedef struct BtlCutIn01 BtlCutIn01;
typedef s32 (*BtlCutIn01Step)(BtlCutIn01*);

typedef struct BtlCutIn01Scene {
    /* 0x00 */ Data*            pack;
    /* 0x04 */ PaletteResource* palettes[2]; // sub, main
    /* 0x0C */ BgResource*      characters[2];
    /* 0x14 */ ScreenMapEntry   subMap;
    /* 0x3C */ void*            subTiles;
    /* 0x40 */ u32              reserved40;
    /* 0x44 */ ScreenMapEntry   mainMap;
    /* 0x6C */ void*            mainTiles[2];
    /* 0x74 */ s32              x;
    /* 0x78 */ CombatSprite     sprite;
    /* 0xD8 */ s16              y;
    /* 0xDA */ u16              reservedDA;
} BtlCutIn01Scene; // Size: 0xDC

struct BtlCutIn01 {
    /* 0x000 */ const BtlCutIn01Step* program;
    /* 0x004 */ u32                   reserved004;
    /* 0x008 */ s32                   active;
    /* 0x00C */ BtlCutIn01Scene       scenes[2]; // Neku, partner
    /* 0x1C4 */ u16                   kind;
    /* 0x1C6 */ u16                   reserved1C6;
    /* 0x1C8 */ Data*                 overlayPack;
    /* 0x1CC */ u32                   reserved1CC[2];
    /* 0x1D4 */ PaletteResource*      overlayPalettes[2]; // main, sub
    /* 0x1DC */ void*                 overlayMainTiles[3];
    /* 0x1E8 */ void*                 overlaySubTiles[3];
    /* 0x1F4 */ ScreenMapEntry        overlaySubMap;
    /* 0x21C */ ScreenMapEntry        overlayMainMap;
    /* 0x244 */ s32                   overlaySubY;
    /* 0x248 */ s32                   overlayMainY;
    /* 0x24C */ u32                   reserved24C;
    /* 0x250 */ s16                   timer;
    /* 0x252 */ s16                   programIndex;
    /* 0x254 */ u16                   elapsed;
    /* 0x256 */ u16                   delay;
    /* 0x258 */ u32                   reserved258;
    /* 0x25C */ s32                   speed;
    /* 0x260 */ void (*update)(BtlCutIn01*);
    /* 0x264 */ u32 reserved264;
    /* 0x268 */ s16 subTimer;
    /* 0x26A */ s16 subPhase;
    /* 0x26C */ s16 subActive;
    /* 0x26E */ u16 reserved26E;
}; // Size: 0x270

typedef struct BtlCutIn01Resources {
    u16 pack, subPalette, subCharacters, mainCharacters, subMap, mainMap, extraMainMap;
    u16 reserved[9];
} BtlCutIn01Resources; // Size: 0x20

/// Per partner: the sprite's palette slot and its height on screen.
typedef struct BtlCutIn01SpriteLayout {
    u16 paletteSlots[3], y[3];
} BtlCutIn01SpriteLayout;

extern const BinIdentifier data_ov003_020d7828;
extern u8                  data_ov003_020e71d9;

static const SpriteAnimEntry BtlCutIn01_NekuAnims[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1}
};

static const BtlCutIn01SpriteLayout BtlCutIn01_PartnerSpriteLayout = {
    { 8, 12, 16},
    {96, 80, 64}
};

static const Ov003CutinPaletteEntries BtlCutIn01_OverlayPalettes = {
    {8, 9, 10}
};

/// Two animations per partner: the entrance and the pose.
static const SpriteAnimEntry BtlCutIn01_PartnerAnims[6] = {
    { 5,  7,  6, 0},
    { 5,  7,  6, 1},
    { 9, 11, 10, 0},
    { 9, 11, 10, 1},
    {13, 15, 14, 0},
    {13, 15, 14, 1}
};

/// Neku's scene, then one scene per partner.
static const BtlCutIn01Resources BtlCutIn01_SceneResources[4] = {
    {10, 5, 1, 2, 3, 4, 0xFFFF, {0}},
    {11, 5, 1, 2, 3, 4, 0xFFFF, {0}},
    {12, 5, 1, 2, 3, 4, 0xFFFF, {0}},
    {13, 6, 1, 2, 3, 4,      5, {0}}
};

static void BtlCutIn01_NextStep(BtlCutIn01* self) {
    self->programIndex++;
    self->subTimer  = 0;
    self->subPhase  = 0;
    self->subActive = 0;
}

static void BtlCutIn01_SetProgram(BtlCutIn01* self, const BtlCutIn01Step* program) {
    self->program = program;
}

static s32 BtlCutIn01_StepFinish(BtlCutIn01* self) {
    EasyFade_FadeBothDisplays(0, 16, 0x1000);
    return 1;
}

// Nonmatching: display-state store scheduling.
static s32 BtlCutIn01_StepInitDisplay(BtlCutIn01* self) {
    g_DisplaySettings.controls[0].windows |= 4;
    g_DisplaySettings.engineState[0].windowObj            = 8;
    g_DisplaySettings.engineState[0].windowObjEffects     = 0;
    g_DisplaySettings.engineState[0].windowOutside        = 0x1F;
    g_DisplaySettings.engineState[0].windowOutsideEffects = 0;
    g_DisplaySettings.controls[1].windows |= 4;
    g_DisplaySettings.engineState[1].windowObj            = 8;
    g_DisplaySettings.engineState[1].windowObjEffects     = 0;
    g_DisplaySettings.engineState[1].windowOutside        = 0x1F;
    g_DisplaySettings.engineState[1].windowOutsideEffects = 0;
    g_DisplaySettings.engineState[0].blendMode            = 1;
    Display_SetBlendLayers(0, 0, 0x21);
    g_DisplaySettings.engineState[1].blendMode = 1;
    Display_SetBlendLayers(1, 0, 0x21);
    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 1, 0, 0x4004);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 3, 1, 0x420C);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 1, 1, 0x4404);
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 1, 0, 0x4004);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 3, 1, 0x420C);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 1, 1, 0x4404);
    g_DisplaySettings.engineState[0].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;
    Display_SetMainLayers(0x1E);
    Display_SetSubLayers(0x1E);
    Display_Commit();
    BtlCutIn01_NextStep(self);
    return 0;
}

static s32 BtlCutIn01_StepNop(BtlCutIn01* self) {
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepRelease(BtlCutIn01* self) {
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->scenes[0].palettes[1]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->scenes[0].palettes[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->scenes[1].palettes[1]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->scenes[1].palettes[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->overlayPalettes[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->overlayPalettes[1]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->scenes[0].characters[1]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], self->scenes[0].characters[0]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->scenes[1].characters[1]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], self->scenes[1].characters[0]);
    CombatSprite_Release(&self->scenes[0].sprite);
    CombatSprite_Release(&self->scenes[1].sprite);
    func_0200d954(0, 1);
    func_0200d954(0, 2);
    func_0200d954(0, 3);
    func_0200d954(1, 1);
    func_0200d954(1, 2);
    func_0200d954(1, 3);
    DatMgr_ReleaseData(self->overlayPack);
    DatMgr_ReleaseData(self->scenes[1].pack);
    DatMgr_ReleaseData(self->scenes[0].pack);
    BtlCutIn01_NextStep(self);
    return 0;
}

static s32 BtlCutIn01_StepFadeIn(BtlCutIn01* self) {
    EasyFade_FadeBothDisplays(0, 0, 0x800);
    BtlCutIn01_NextStep(self);
    return 0;
}

static s32 BtlCutIn01_StepFadeOut(BtlCutIn01* self) {
    EasyFade_FadeBothDisplays(0, 16, 0x1000);
    BtlCutIn01_NextStep(self);
    return 0;
}

static void BtlCutIn01_LoadOverlays(BtlCutIn01* self, u16 kind) {
    self->overlayPack                = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, 2, 0);
    Ov003CutinPaletteEntries entries = BtlCutIn01_OverlayPalettes;
    void*                    palette = Data_GetPackEntryData(self->overlayPack, entries.entries[kind]);
    self->overlaySubTiles[0]         = Ov003Cutin_Tiles(self->overlayPack, 5);
    self->overlaySubTiles[1]         = Ov003Cutin_Tiles(self->overlayPack, 6);
    self->overlaySubTiles[2]         = Ov003Cutin_Tiles(self->overlayPack, 7);
    self->overlayPalettes[1]         = PaletteMgr_AllocPalette(g_PaletteManagers[1], palette, 0, 15, 1);
    func_0200d1d8(&self->overlaySubMap, 1, 3, NULL, self->overlaySubTiles, 1, 3);
    PaletteMgr_Flush(g_PaletteManagers[1], self->overlayPalettes[1]);
    func_0200d858(&self->overlaySubMap, 1, 1, 0);
    self->overlaySubY         = 0;
    palette                   = Data_GetPackEntryData(self->overlayPack, 4);
    self->overlayMainTiles[0] = Ov003Cutin_Tiles(self->overlayPack, 1);
    self->overlayMainTiles[1] = Ov003Cutin_Tiles(self->overlayPack, 2);
    self->overlayMainTiles[2] = Ov003Cutin_Tiles(self->overlayPack, 3);
    self->overlayPalettes[0]  = PaletteMgr_AllocPalette(g_PaletteManagers[0], palette, 0, 15, 1);
    func_0200d1d8(&self->overlayMainMap, 0, 3, NULL, self->overlayMainTiles, 1, 3);
    PaletteMgr_Flush(g_PaletteManagers[0], self->overlayPalettes[0]);
    func_0200d858(&self->overlayMainMap, 1, 1, 0);
    self->overlayMainY = 0x200000;
    Display_SetBGOffset(self->overlaySubMap.engineId, self->overlaySubMap.bgLayer, 0, self->overlaySubY);
    Display_SetBGOffset(self->overlayMainMap.engineId, self->overlayMainMap.bgLayer, 0, self->overlayMainY);
}

// Nonmatching: resource loading and map/sprite register allocation.
static void BtlCutIn01_LoadScenes(BtlCutIn01* self, u16 kind) {
    const BtlCutIn01Resources* partnerRes = &BtlCutIn01_SceneResources[kind + 1];
    const BtlCutIn01Resources* nekuRes    = &BtlCutIn01_SceneResources[0];
    BtlCutIn01Scene*           neku       = &self->scenes[0];
    BtlCutIn01Scene*           partner    = &self->scenes[1];

    neku->pack          = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, 10, 0);
    void* palette       = Data_GetPackEntryData(neku->pack, 5);
    void* characters    = Data_GetPackEntryData(neku->pack, nekuRes->subCharacters);
    neku->subTiles      = Ov003Cutin_Tiles(neku->pack, nekuRes->subMap);
    neku->palettes[0]   = PaletteMgr_AllocPalette(g_PaletteManagers[1], palette, 0, 0, 7);
    neku->characters[0] = BgResMgr_AllocChar32(g_BgResourceManagers[1], characters,
                                               g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x8000);
    func_0200d1d8(&neku->subMap, 1, 1, NULL, &neku->subTiles, 1, 1);
    PaletteMgr_Flush(g_PaletteManagers[1], neku->palettes[0]);
    func_0200d858(&neku->subMap, 0, 0, 0);
    palette             = Data_GetPackEntryData(neku->pack, nekuRes->subPalette);
    characters          = Data_GetPackEntryData(neku->pack, nekuRes->mainCharacters);
    neku->mainTiles[0]  = Ov003Cutin_Tiles(neku->pack, nekuRes->mainMap);
    neku->palettes[1]   = PaletteMgr_AllocPalette(g_PaletteManagers[0], palette, 0, 0, 7);
    neku->characters[1] = BgResMgr_AllocChar32(g_BgResourceManagers[0], characters,
                                               g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);
    func_0200d1d8(&neku->mainMap, 0, 1, NULL, neku->mainTiles, 1, 1);
    PaletteMgr_Flush(g_PaletteManagers[0], neku->palettes[1]);
    func_0200d858(&neku->mainMap, 0, 0, 0);
    CombatSprite_LoadFromTable(1, &neku->sprite, &data_ov003_020d7828, BtlCutIn01_NekuAnims, 0, 4, 80);
    CombatSprite_SetAnimFromTable(&neku->sprite, 0, 1);
    CombatSprite_SetFlip(&neku->sprite, 0);
    neku->x                = -0x100000;
    partner->pack          = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, partnerRes->pack, 0);
    palette                = Data_GetPackEntryData(partner->pack, partnerRes->subPalette);
    characters             = Data_GetPackEntryData(partner->pack, partnerRes->subCharacters);
    partner->subTiles      = Ov003Cutin_Tiles(partner->pack, partnerRes->subMap);
    partner->palettes[0]   = PaletteMgr_AllocPalette(g_PaletteManagers[1], palette, 0, 7, 7);
    partner->characters[0] = BgResMgr_AllocChar32(g_BgResourceManagers[1], characters,
                                                  g_DisplaySettings.engineState[1].bgSettings[2].charBase, 0, 0x8000);
    func_0200d1d8(&partner->subMap, 1, 2, NULL, &partner->subTiles, 1, 1);
    PaletteMgr_Flush(g_PaletteManagers[1], partner->palettes[0]);
    func_0200d858(&partner->subMap, 0, 0, 0);
    palette                = Data_GetPackEntryData(partner->pack, partnerRes->subPalette);
    characters             = Data_GetPackEntryData(partner->pack, partnerRes->mainCharacters);
    partner->mainTiles[0]  = Ov003Cutin_Tiles(partner->pack, partnerRes->mainMap);
    partner->palettes[1]   = PaletteMgr_AllocPalette(g_PaletteManagers[0], palette, 0, 7, 7);
    partner->characters[1] = BgResMgr_AllocChar32(g_BgResourceManagers[0], characters,
                                                  g_DisplaySettings.engineState[0].bgSettings[2].charBase, 0, 0x8000);
    if (kind == PARTNER_BEAT) {
        partner->mainTiles[1] = Ov003Cutin_Tiles(partner->pack, partnerRes->extraMainMap);
        func_0200d1d8(&partner->mainMap, 0, 2, NULL, partner->mainTiles, 2, 1);
    } else {
        func_0200d1d8(&partner->mainMap, 0, 2, NULL, partner->mainTiles, 1, 1);
    }
    PaletteMgr_Flush(g_PaletteManagers[0], partner->palettes[1]);
    func_0200d858(&partner->mainMap, 0, 0, 0);
    BtlCutIn01_LoadOverlays(self, kind);
    BtlCutIn01SpriteLayout layout = BtlCutIn01_PartnerSpriteLayout;
    partner->y                    = layout.y[kind];
    CombatSprite_LoadFromTable(1, &partner->sprite, &data_ov003_020d7828, BtlCutIn01_PartnerAnims, (u16)(kind * 2),
                               layout.paletteSlots[kind], 80);
    CombatSprite_SetAnimFromTable(&partner->sprite, self->kind * 2, 1);
    CombatSprite_SetFlip(&partner->sprite, 0);
    partner->x = 0x100000;
}

static s32 BtlCutIn01_StepLoad(BtlCutIn01* self) {
    BtlCutIn01_LoadScenes(self, self->kind);
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepStartSlide(BtlCutIn01* self) {
    self->speed  = 0x6000;
    self->delay  = 0;
    self->active = 1;
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepSlideIn(BtlCutIn01* self) {
    self->scenes[0].x += self->speed;
    self->scenes[1].x -= self->speed;
    if (self->scenes[0].x > 0x16000) {
        if (self->delay == 0) {
            self->delay = 1;
            self->speed = 0x2000;
        } else if (self->speed > 0x333) {
            self->speed -= 0xCD;
        }
    }
    if (self->scenes[0].x >= 0x40000) {
        self->scenes[0].x = 0x40000;
        self->scenes[1].x = -0x40000;
        BtlCutIn01_NextStep(self);
    }
    return 0;
}

static s32 BtlCutIn01_StepPose(BtlCutIn01* self) {
    CombatSprite_SetAnimFromTable(&self->scenes[0].sprite, 1, 1);
    CombatSprite_SetAnimFromTable(&self->scenes[1].sprite, self->kind * 2 + 1, 1);
    self->timer = 30;
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepWaitTimer(BtlCutIn01* self) {
    self->timer--;
    if (self->timer == 0) {
        BtlCutIn01_NextStep(self);
    }
    return 0;
}

static s32 BtlCutIn01_StepSetLongDelay(BtlCutIn01* self) {
    self->delay = 30;
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepSetShortDelay(BtlCutIn01* self) {
    self->delay = 5;
    BtlCutIn01_NextStep(self);
    return 2;
}

static s32 BtlCutIn01_StepWaitDelay(BtlCutIn01* self) {
    if (self->delay != 0) {
        self->delay--;
    } else {
        BtlCutIn01_NextStep(self);
    }
    return 0;
}

static BtlCutIn01Step BtlCutIn01_Program[15];

static s32 BtlCutIn01_Init(TaskPool* pool, Task* task, void* args) {
    BtlCutIn01* self = task->data;

    MI_CpuSet(self, 0, sizeof(*self));
    BtlCutIn01_SetProgram(self, BtlCutIn01_Program);
    EasyFade_FadeBothDisplays(0, 0, 0x800);
    switch (data_ov003_020e71d9) {
        case PARTNER_BEAT:
            self->kind = PARTNER_BEAT;
            break;
        case PARTNER_JOSHUA:
            self->kind = PARTNER_JOSHUA;
            break;
        default:
        case PARTNER_SHIKI:
            self->kind = PARTNER_SHIKI;
            break;
    }
    return 1;
}

// Nonmatching: step dispatch and screen-map offset register allocation.
static s32 BtlCutIn01_Update(TaskPool* pool, Task* task, void* args) {
    BtlCutIn01* self = task->data;

    if (self->program[self->programIndex] == NULL) {
        return 0;
    }
    s32 result;
    do {
        result = self->program[self->programIndex](self);
    } while (result == 2);
    if (result == 1) {
        return 0;
    }
    if (!self->active) {
        return 1;
    }
    if (self->elapsed == 30) {
        BtlCutIn_PlayFirstVoice(self->kind, 1);
    }
    if (self->elapsed == 72) {
        BtlCutIn_PlaySecondVoice(self->kind, 1);
    }
    self->elapsed++;
    CombatSprite_Update(&self->scenes[0].sprite);
    CombatSprite_Update(&self->scenes[1].sprite);
    if ((self->overlaySubY >> 12) >= 512) {
        self->overlaySubY  = 0x200000;
        self->overlayMainY = 0;
    } else {
        self->overlaySubY += 0x4000;
        self->overlayMainY -= 0x4000;
    }
    Display_SetBGOffset(self->overlaySubMap.engineId, self->overlaySubMap.bgLayer, 0, self->overlaySubY);
    Display_SetBGOffset(self->overlayMainMap.engineId, self->overlayMainMap.bgLayer, 0, self->overlayMainY);
    s32 offset = -(self->scenes[0].x & ~0xFFF);
    Display_SetBGOffset(self->scenes[0].subMap.engineId, self->scenes[0].subMap.bgLayer, offset, 0);
    Display_SetBGOffset(self->scenes[0].mainMap.engineId, self->scenes[0].mainMap.bgLayer, offset, 0);
    offset = -(self->scenes[1].x & ~0xFFF);
    Display_SetBGOffset(self->scenes[1].subMap.engineId, self->scenes[1].subMap.bgLayer, offset, 0);
    Display_SetBGOffset(self->scenes[1].mainMap.engineId, self->scenes[1].mainMap.bgLayer, offset, 0);
    return 1;
}

static s32 BtlCutIn01_Render(TaskPool* pool, Task* task, void* args) {
    BtlCutIn01*      self    = task->data;
    BtlCutIn01Scene* partner = &self->scenes[1];

    if (!self->active) {
        return 1;
    }
    CombatSprite_SetPosition(&self->scenes[0].sprite, (s16)((self->scenes[0].x >> 12) + 128), 80);
    CombatSprite_SetAffineTransform(&self->scenes[0].sprite, 0, 0x1000, 0x1000, 1);
    func_ov003_02082730(&self->scenes[0].sprite, 1);
    CombatSprite_Render(&self->scenes[0].sprite);
    CombatSprite_SetPosition(&partner->sprite, (s16)((partner->x >> 12) + 128), partner->y);
    CombatSprite_SetAffineTransform(&partner->sprite, 0, 0x1000, 0x1000, 1);
    func_ov003_02082730(&partner->sprite, 2);
    CombatSprite_Render(&partner->sprite);
    return 1;
}

static s32 BtlCutIn01_Release(TaskPool* pool, Task* task, void* args) {
    return 0;
}

static s32 BtlCutIn01_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlCutIn01_Init,
        .update     = BtlCutIn01_Update,
        .render     = BtlCutIn01_Render,
        .cleanup    = BtlCutIn01_Release,
    };

    return stages.iter[stage](pool, task, args);
}

const TaskHandle Tsk_BtlCutIn01 = {"Tsk_BtlCutIn01", BtlCutIn01_RunTask, sizeof(BtlCutIn01)};

static BtlCutIn01Step BtlCutIn01_Program[15] = {
    BtlCutIn01_StepInitDisplay, BtlCutIn01_StepNop,       BtlCutIn01_StepLoad,          BtlCutIn01_StepFadeIn,
    BtlCutIn01_StepStartSlide,  BtlCutIn01_StepSlideIn,   BtlCutIn01_StepSetShortDelay, BtlCutIn01_StepWaitDelay,
    BtlCutIn01_StepPose,        BtlCutIn01_StepWaitTimer, BtlCutIn01_StepFadeOut,       BtlCutIn01_StepSetLongDelay,
    BtlCutIn01_StepWaitDelay,   BtlCutIn01_StepRelease,   BtlCutIn01_StepFinish,
};
