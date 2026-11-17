#include "Combat/Core/BtlCutIn.h"
#include "Combat/Core/BtlSpecial.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "EasyFade.h"
#include "Engine/Resources/ScreenMapper.h"
#include "Player/Stats.h"
#include "SndMgr.h"
#include "main_02026528.h"
#include <nitro/mi/cpumem.h>

// The cut-in core and the partner's cut-in (`Tsk_BtlCutIn00`): the actor screen-position helpers, the
// special-attack voice lines that `BtlCutIn01`/`BtlCutIn02` reuse, and the cut-in background loaders.

typedef struct BtlCutIn00Background {
    /* 0x00 */ Data*            pack;
    /* 0x04 */ BgResource*      characters;
    /* 0x08 */ PaletteResource* palette;
    /* 0x0C */ ScreenMapEntry   maps[2];
    /* 0x5C */ void*            tileData[2];
    /* 0x64 */ s32              offsets[2];
} BtlCutIn00Background; // Size: 0x6C

typedef struct BtlCutIn00 {
    /* 0x000 */ BtlCutIn00Background backgrounds[2]; // sub screen, main screen
    /* 0x0D8 */ u16                  kind;
    /* 0x0DA */ u16                  voiceKind;
    /* 0x0DC */ s32                  unk_DC;
    /* 0x0E0 */ s32                  unk_E0;
    /* 0x0E4 */ u16                  unk_E4;
    /* 0x0E6 */ u16                  reserved_E6;
    /* 0x0E8 */ Data*                overlayPack;
    /* 0x0EC */ u32                  reserved_EC;
    /* 0x0F0 */ BgResource*          overlayCharacters[2];
    /* 0x0F8 */ PaletteResource*     overlayPalettes[2];
    /* 0x100 */ s16                  overlayTimer;
    /* 0x102 */ u16                  reserved_102;
    /* 0x104 */ s32                  overlayOffsets[2];
    /* 0x10C */ ScreenMapEntry       overlayMaps[2];
    /* 0x15C */ void*                overlayTileData[2][3];
    /* 0x174 */ s16                  elapsed;
    /* 0x176 */ u16                  reserved_176;
    /* 0x178 */ void (*update)(struct BtlCutIn00*);
    /* 0x17C */ s16 timer;
    /* 0x17E */ s16 phase;
    /* 0x180 */ s32 active;
    /* 0x184 */ u32 reserved_184;
} BtlCutIn00; // Size: 0x188

typedef struct BtlCutIn00Args {
    u16 kind;
    u16 partner;
} BtlCutIn00Args; // Size: 0x4

/// One voice line per fusion level (cut-ins 00, 01 and 02).
typedef struct BtlCutInVoices {
    SndMgrSeIdx levels[3];
} BtlCutInVoices;

typedef struct BtlCutInBgResources {
    u16 pack, palette, characters, maps[2];
} BtlCutInBgResources;

static s32 BtlCutIn00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage);

/// 1 while a fusion attack plays, 2 during a partner's feat.
static s32 BtlCutIn_SpecialKind;

static const BtlCutInVoices BtlCutIn_FirstVoicesShiki = {
    {SEIDX_SHIKI_BTL_SPC_01, SEIDX_SHIKI_BTL_SPC_02, SEIDX_SHIKI_BTL_SPC_03}
};

const TaskHandle Tsk_BtlCutIn00 = {"Tsk_BtlCutIn00", BtlCutIn00_RunTask, sizeof(BtlCutIn00)};

static const Ov003CutinPaletteEntries BtlCutIn00_OverlayPalettes = {
    {10, 11, 12}
};

static const BtlCutInBgResources BtlCutIn00_BgResources[6] = {
    {4, 4, 1, {2, 3}},
    {5, 4, 1, {2, 3}},
    {6, 4, 1, {2, 3}},
    {7, 4, 1, {2, 3}},
    {8, 4, 1, {2, 3}},
    {9, 4, 1, {2, 3}},
};

static const BtlCutInVoices BtlCutIn_SecondVoicesJoshua = {
    {SEIDX_NEKU_BTL_SPC_YOSHUA01, SEIDX_NEKU_BTL_SPC_YOSHUA02, SEIDX_YOSHUA_BTL_SPC_03}
};

static const BtlCutInVoices BtlCutIn_FirstVoicesJoshua = {
    {SEIDX_YOSHUA_BTL_SPC_01, SEIDX_YOSHUA_BTL_SPC_02, SEIDX_NEKU_BTL_SPC_YOSHUA03}
};

static const BtlCutInVoices BtlCutIn_FirstVoicesBeat = {
    {SEIDX_BIITO_BTL_SPC_01, SEIDX_BIITO_BTL_SPC_02, SEIDX_NEKU_BTL_SPC_BIITO03}
};

static const BtlCutInVoices BtlCutIn_SecondVoicesShiki = {
    {SEIDX_NEKU_BTL_SPC_SHIKI01, SEIDX_NEKU_BTL_SPC_SHIKI02, SEIDX_NEKU_BTL_SPC_SHIKI03}
};

static const BtlCutInVoices BtlCutIn_SecondVoicesBeat = {
    {SEIDX_NEKU_BTL_SPC_BIITO01, SEIDX_NEKU_BTL_SPC_BIITO02, SEIDX_BIITO_BTL_SPC_03}
};

static void BtlCutIn00_SlideOut(BtlCutIn00* self);
static void BtlCutIn00_WaitFadeOut(BtlCutIn00* self);

extern u8 data_ov003_020e71d9;

s32 BtlCutIn_GetSnapshotScreenX(s32 screen, Ov003ActorSnapshot* entry) {
    if (screen == 1) {
        return BtlStage_ToScreenX(1, entry->unit->actor.position.x);
    } else {
        return BtlStage_ToScreenX(0, entry->unit->actor.position.x);
    }
}

s32 BtlCutIn_GetSnapshotScreenY(s32 screen, Ov003ActorSnapshot* entry) {
    if (screen == 1) {
        return BtlStage_ToScreenY(1, entry->unit->actor.position.y, entry->unit->actor.position.z);
    } else {
        return BtlStage_ToScreenY(0, entry->unit->actor.position.y, entry->unit->actor.position.z);
    }
}

void BtlCutIn_SetSnapshotScreenX(s32 screen, Ov003ActorSnapshot* entry, s32 x) {
    if (screen == 1) {
        entry->unit->actor.position.x = BtlStage_ToWorldX(1, x);
    } else {
        entry->unit->actor.position.x = BtlStage_ToWorldX(0, x);
    }
}

void BtlCutIn_SetSnapshotScreenY(s32 screen, Ov003ActorSnapshot* entry, s32 y) {
    if (screen == 1) {
        entry->unit->actor.position.y = BtlStage_ToWorldY(1, y, entry->unit->actor.position.z);
    } else {
        entry->unit->actor.position.y = BtlStage_ToWorldY(0, y, entry->unit->actor.position.z);
    }
}

void BtlCutIn_SetSpecialKind(s32 kind) {
    BtlCutIn_SpecialKind = kind;
}

s32 BtlCutIn_GetSpecialKind(void) {
    return BtlCutIn_SpecialKind;
}

void BtlCutIn_PlayFirstVoice(s32 kind, s32 level) {
    switch (kind) {
        case PARTNER_SHIKI: {
            BtlCutInVoices voices = BtlCutIn_FirstVoicesShiki;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case PARTNER_JOSHUA: {
            BtlCutInVoices voices = BtlCutIn_FirstVoicesJoshua;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case PARTNER_BEAT: {
            BtlCutInVoices voices = BtlCutIn_FirstVoicesBeat;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case 3:
            SndMgr_StartPlayingSE(SEIDX_YASHIRO_B_BTL_ATCM04);
            break;
    }
}

void BtlCutIn_PlaySecondVoice(s32 kind, s32 level) {
    switch (kind) {
        case PARTNER_SHIKI: {
            BtlCutInVoices voices = BtlCutIn_SecondVoicesShiki;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case PARTNER_JOSHUA: {
            BtlCutInVoices voices = BtlCutIn_SecondVoicesJoshua;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case PARTNER_BEAT: {
            BtlCutInVoices voices = BtlCutIn_SecondVoicesBeat;
            SndMgr_StartPlayingSE(voices.levels[level]);
            break;
        }
        case 3:
            SndMgr_StartPlayingSE(SEIDX_KARIYA_B_BTL_ATCS04);
            break;
    }
}

static void BtlCutIn00_SetUpdate(BtlCutIn00* self, void (*update)(BtlCutIn00*)) {
    self->update = update;
    self->timer  = 0;
    self->phase  = 0;
}

static void BtlCutIn00_ApplyBgOffsets(BtlCutIn00* self) {
    Display_SetBGOffset(self->overlayMaps[0].engineId, self->overlayMaps[0].bgLayer, self->overlayOffsets[0], 0);
    Display_SetBGOffset(self->overlayMaps[1].engineId, self->overlayMaps[1].bgLayer, self->overlayOffsets[1], 0);
    Display_SetBGOffset(self->backgrounds[0].maps[0].engineId, self->backgrounds[0].maps[0].bgLayer,
                        self->backgrounds[0].offsets[0] - 0x80000, 0);
    Display_SetBGOffset(self->backgrounds[0].maps[1].engineId, self->backgrounds[0].maps[1].bgLayer,
                        self->backgrounds[0].offsets[1] - 0x80000, 0);
    Display_SetBGOffset(self->backgrounds[1].maps[0].engineId, self->backgrounds[1].maps[0].bgLayer,
                        self->backgrounds[1].offsets[0] - 0x80000, 0);
    Display_SetBGOffset(self->backgrounds[1].maps[1].engineId, self->backgrounds[1].maps[1].bgLayer,
                        self->backgrounds[1].offsets[1] - 0x80000, 0);
}

static void BtlCutIn00_LoadSubBackground(BtlCutIn00Background* self, s32 index) {
    const BtlCutInBgResources* resources = &BtlCutIn00_BgResources[index];

    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 1, 0, 0x4004);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 1, 1, 0x4204);
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 0;
    Display_SetBGOffset(1, 1, 0, 0);
    Display_SetBGOffset(1, 2, 0, 0);
    self->offsets[0] = -0x80000;
    self->offsets[1] = -0x80000;
    self->pack       = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, resources->pack, 0);
    void* palette    = Data_GetPackEntryData(self->pack, resources->palette);
    void* characters = Data_GetPackEntryData(self->pack, resources->characters);
    self->palette    = PaletteMgr_AllocPalette(g_PaletteManagers[1], palette, 0, 0, 15);
    self->characters = BgResMgr_AllocChar32(g_BgResourceManagers[1], characters,
                                            g_DisplaySettings.engineState[1].bgSettings[1].charBase, 0, 0x8000);
    PaletteMgr_Flush(g_PaletteManagers[1], self->palette);
    self->tileData[0] = Ov003Cutin_Tiles(self->pack, resources->maps[0]);
    self->tileData[1] = Ov003Cutin_Tiles(self->pack, resources->maps[1]);
    func_0200d1d8(&self->maps[0], 1, 1, NULL, &self->tileData[0], 1, 1);
    func_0200d1d8(&self->maps[1], 1, 2, NULL, &self->tileData[1], 1, 1);
    func_0200d858(&self->maps[0], 0, 0, 0);
    func_0200d858(&self->maps[1], 0, 0, 0);
}

static void BtlCutIn00_LoadMainBackground(BtlCutIn00Background* self, s32 index) {
    const BtlCutInBgResources* resources = &BtlCutIn00_BgResources[index];

    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 3, 0, 0x400C);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 3, 1, 0x420C);
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 0;
    Display_SetBGOffset(0, 1, 0, 0);
    Display_SetBGOffset(0, 2, 0, 0);
    self->offsets[0] = 0x180000;
    self->offsets[1] = 0x180000;
    self->pack       = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, resources->pack, 0);
    void* palette    = Data_GetPackEntryData(self->pack, resources->palette);
    void* characters = Data_GetPackEntryData(self->pack, resources->characters);
    self->palette    = PaletteMgr_AllocPalette(g_PaletteManagers[0], palette, 0, 0, 15);
    self->characters = BgResMgr_AllocChar32(g_BgResourceManagers[0], characters,
                                            g_DisplaySettings.engineState[0].bgSettings[1].charBase, 0, 0x8000);
    PaletteMgr_Flush(g_PaletteManagers[0], self->palette);
    self->tileData[0] = Ov003Cutin_Tiles(self->pack, resources->maps[0]);
    self->tileData[1] = Ov003Cutin_Tiles(self->pack, resources->maps[1]);
    func_0200d1d8(&self->maps[0], 0, 1, NULL, &self->tileData[0], 1, 1);
    func_0200d1d8(&self->maps[1], 0, 2, NULL, &self->tileData[1], 1, 1);
    func_0200d858(&self->maps[0], 0, 0, 0);
    func_0200d858(&self->maps[1], 0, 0, 0);
}

static void BtlCutIn00_LoadOverlays(BtlCutIn00* self, s32 index) {
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 1, 1, 0x4404);
    g_DisplaySettings.controls[0].windows &= ~4;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 1;
    Display_SetBGOffset(0, 3, 0, 0);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 4, 5, 1, 0x4414);
    g_DisplaySettings.controls[1].windows &= ~4;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 0;
    Display_SetBGOffset(1, 3, 0, 0);
    self->overlayPack          = DatMgr_LoadPackEntry(1, NULL, 0, &Iden_BtlSpecial3, 1, 0);
    self->overlayCharacters[0] = BgResMgr_AllocChar32(g_BgResourceManagers[0], Data_GetPackEntryData(self->overlayPack, 1),
                                                      g_DisplaySettings.engineState[0].bgSettings[3].charBase, 0, 0x3AA0);
    self->overlayPalettes[0] =
        PaletteMgr_AllocPalette(g_PaletteManagers[0], Data_GetPackEntryData(self->overlayPack, 5), 0, 15, 1);
    PaletteMgr_Flush(g_PaletteManagers[0], self->overlayPalettes[0]);
    self->overlayTileData[0][0] = Ov003Cutin_Tiles(self->overlayPack, 2);
    self->overlayTileData[0][1] = Ov003Cutin_Tiles(self->overlayPack, 3);
    self->overlayTileData[0][2] = Ov003Cutin_Tiles(self->overlayPack, 4);
    func_0200d1d8(&self->overlayMaps[0], 0, 3, NULL, self->overlayTileData[0], 3, 1);
    func_0200d858(&self->overlayMaps[0], 0, 0, 0);
    self->overlayCharacters[1] = BgResMgr_AllocChar32(g_BgResourceManagers[1], Data_GetPackEntryData(self->overlayPack, 6),
                                                      g_DisplaySettings.engineState[1].bgSettings[3].charBase, 0, 0x3AA0);
    Ov003CutinPaletteEntries palettes = BtlCutIn00_OverlayPalettes;
    self->overlayPalettes[1]          = PaletteMgr_AllocPalette(
        g_PaletteManagers[1], Data_GetPackEntryData(self->overlayPack, palettes.entries[index - 1]), 0, 15, 1);
    PaletteMgr_Flush(g_PaletteManagers[1], self->overlayPalettes[1]);
    self->overlayTileData[1][0] = Ov003Cutin_Tiles(self->overlayPack, 7);
    self->overlayTileData[1][1] = Ov003Cutin_Tiles(self->overlayPack, 8);
    self->overlayTileData[1][2] = Ov003Cutin_Tiles(self->overlayPack, 9);
    func_0200d1d8(&self->overlayMaps[1], 1, 3, NULL, self->overlayTileData[1], 3, 1);
    func_0200d858(&self->overlayMaps[1], 0, 0, 0);
    self->overlayTimer      = 90;
    self->overlayOffsets[0] = 0x200000;
    self->overlayOffsets[1] = 0;
}

static void BtlCutIn00_ReleaseBackgrounds(BtlCutIn00* self) {
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->backgrounds[0].palette);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], self->backgrounds[0].characters);
    func_0200d954(1, 1);
    func_0200d954(1, 2);
    DatMgr_ReleaseData(self->backgrounds[0].pack);
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->backgrounds[1].palette);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->backgrounds[1].characters);
    func_0200d954(0, 1);
    func_0200d954(0, 2);
    DatMgr_ReleaseData(self->backgrounds[1].pack);
}

static void BtlCutIn00_ReleaseOverlays(BtlCutIn00* self) {
    PaletteMgr_ReleaseResource(g_PaletteManagers[0], self->overlayPalettes[0]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[0], self->overlayCharacters[0]);
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], self->overlayPalettes[1]);
    BgResMgr_ReleaseChar(g_BgResourceManagers[1], self->overlayCharacters[1]);
    func_0200d954(0, 3);
    func_0200d954(1, 3);
    DatMgr_ReleaseData(self->overlayPack);
}

static void BtlCutIn00_OnInit(BtlCutIn00* self) {
    return;
}

static void BtlCutIn00_OnRelease(BtlCutIn00* self) {
    return;
}

static void BtlCutIn00_SlideIn(BtlCutIn00* self) {
    if (self->timer == 0) {
        self->timer = 15;
    }
    if (self->timer > 0) {
        func_02026590(&self->backgrounds[0].offsets[0], 0x55000, self->timer);
        func_02026590(&self->backgrounds[1].offsets[0], 0xAA000, self->timer);
        self->backgrounds[0].offsets[1] = self->backgrounds[0].offsets[0];
        self->backgrounds[1].offsets[1] = self->backgrounds[1].offsets[0];
        self->timer--;
    }
    if (self->timer <= 0) {
        BtlCutIn00_SetUpdate(self, BtlCutIn00_SlideOut);
    }
}

static void BtlCutIn00_SlideOut(BtlCutIn00* self) {
    if (self->timer == 0) {
        self->timer = 80;
    }
    if (self->timer == 20) {
        EasyFade_FadeBothDisplays(0, 16, 0x1000);
    }
    if (self->timer > 50) {
        func_02026590(&self->backgrounds[0].offsets[0], 0x66000, self->timer);
        func_02026590(&self->backgrounds[0].offsets[1], 0xA6000, self->timer);
        func_02026590(&self->backgrounds[1].offsets[0], 0x99000, self->timer);
        func_02026590(&self->backgrounds[1].offsets[1], 0x59000, self->timer);
    } else if (self->timer > 0) {
        func_02026590(&self->backgrounds[0].offsets[0], 0x80000, self->timer);
        func_02026590(&self->backgrounds[0].offsets[1], 0xC0000, self->timer);
        func_02026590(&self->backgrounds[1].offsets[0], 0x80000, self->timer);
        func_02026590(&self->backgrounds[1].offsets[1], 0x40000, self->timer);
    }
    if (--self->timer <= 0) {
        BtlCutIn00_SetUpdate(self, BtlCutIn00_WaitFadeOut);
    }
}

static void BtlCutIn00_WaitFadeOut(BtlCutIn00* self) {
    switch (self->phase) {
        case 0:
            self->timer = 15;
            self->phase++;
            break;
        case 1:
            if (!EasyFade_IsFading()) {
                self->active = 0;
            }
            break;
    }
}

static s32 BtlCutIn00_Init(TaskPool* pool, Task* task, void* args) {
    BtlCutIn00*     self = task->data;
    BtlCutIn00Args* arg  = args;

    MI_CpuSet(self, 0, sizeof(*self));
    self->kind    = arg != NULL ? arg->kind : 0;
    self->unk_E4  = 0;
    self->unk_DC  = 0x180000;
    self->unk_E0  = -0x80000;
    self->elapsed = 0;
    self->active  = 1;
    if (self->kind == 2) {
        BtlCutIn00_LoadSubBackground(&self->backgrounds[0], 4);
        BtlCutIn00_LoadMainBackground(&self->backgrounds[1], 5);
        BtlCutIn00_LoadOverlays(self, 2);
        self->voiceKind = 3;
    } else {
        switch (data_ov003_020e71d9) {
            case PARTNER_JOSHUA:
                BtlCutIn00_LoadSubBackground(&self->backgrounds[0], 2);
                BtlCutIn00_LoadMainBackground(&self->backgrounds[1], 0);
                BtlCutIn00_LoadOverlays(self, 2);
                self->voiceKind = PARTNER_JOSHUA;
                break;
            case PARTNER_BEAT:
                BtlCutIn00_LoadSubBackground(&self->backgrounds[0], 3);
                BtlCutIn00_LoadMainBackground(&self->backgrounds[1], 0);
                BtlCutIn00_LoadOverlays(self, 3);
                self->voiceKind = PARTNER_BEAT;
                break;
            case PARTNER_SHIKI:
            default:
                BtlCutIn00_LoadSubBackground(&self->backgrounds[0], 1);
                BtlCutIn00_LoadMainBackground(&self->backgrounds[1], 0);
                BtlCutIn00_LoadOverlays(self, 1);
                self->voiceKind = PARTNER_SHIKI;
                break;
        }
    }
    BtlCutIn00_OnInit(self);
    BtlCutIn00_ApplyBgOffsets(self);
    BtlCutIn00_SetUpdate(self, BtlCutIn00_SlideIn);
    EasyFade_FadeBothDisplays(0, 0, 0x800);
    return 1;
}

static s32 BtlCutIn00_Update(TaskPool* pool, Task* task, void* args) {
    BtlCutIn00* self = task->data;

    if (self->update != NULL) {
        self->update(self);
    }
    if (self->overlayTimer > 0) {
        func_02026590(&self->overlayOffsets[0], 0, self->overlayTimer);
        func_02026590(&self->overlayOffsets[1], 0x200000, self->overlayTimer);
        self->overlayTimer--;
    }
    BtlCutIn00_ApplyBgOffsets(self);
    if (self->elapsed == 30) {
        BtlCutIn_PlayFirstVoice(self->voiceKind, 0);
    }
    if (self->elapsed == 72) {
        BtlCutIn_PlaySecondVoice(self->voiceKind, 0);
    }
    self->elapsed++;
    return self->active;
}

static s32 BtlCutIn00_Render(TaskPool* pool, Task* task, void* args) {
    return 1;
}

static s32 BtlCutIn00_Release(TaskPool* pool, Task* task, void* args) {
    BtlCutIn00* self = task->data;

    BtlCutIn00_ReleaseOverlays(self);
    BtlCutIn00_ReleaseBackgrounds(self);
    BtlCutIn00_OnRelease(self);
    return 1;
}

static s32 BtlCutIn00_RunTask(TaskPool* pool, Task* task, void* args, s32 stage) {
    TaskStages stages = {
        .initialize = BtlCutIn00_Init,
        .update     = BtlCutIn00_Update,
        .render     = BtlCutIn00_Render,
        .cleanup    = BtlCutIn00_Release,
    };

    return stages.iter[stage](pool, task, args);
}

s32 BtlCutIn00_CreateKariyaYashiroTask(TaskPool* pool) {
    BtlCutIn00Args args;

    args.kind    = 2;
    args.partner = gBattle->partnerId;
    return EasyTask_CreateTask(pool, &Tsk_BtlCutIn00, NULL, 0, NULL, &args);
}
