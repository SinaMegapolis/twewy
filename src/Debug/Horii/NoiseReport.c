#include "Debug/NoiseReport.h"
#include "CriSndMgr.h"
#include "Display.h"
#include "EasyFade.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/Input.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Interface/Menu/MenuCommon.h"
#include "Player/Inventory.h"
#include "Save.h"
#include "SndMgr.h"
#include "common_data.h"

s32 func_ov028_020eb9dc(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020ec4a8(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020ecd08(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020eb760(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020ea6ac(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020ea21c(TaskPool* pool, Task* task, void* arg2, s32 arg3);
s32 func_ov028_020e8d50(TaskPool* pool, Task* task, void* arg2, s32 arg3);

extern NoiseReport* data_ov028_020ed3e0;

s16 data_ov028_020ed3a4[4] = {21, 95, 96, 0};

const TaskHandle data_ov028_020ecf38 = {"Tsk_NrepBG", func_ov028_020e8d50, sizeof(NrepBG)};
const TaskHandle data_ov028_020ed030 = {"Tsk_Font", func_ov028_020ea21c, sizeof(NrepFont)};
const TaskHandle data_ov028_020ed204 = {"Tsk_NrepIcon", func_ov028_020ea6ac, sizeof(NrepIcon)};
const TaskHandle data_ov028_020ed254 = {"Tsk_NrepInterD", func_ov028_020eb760, sizeof(NrepInterD)};
const TaskHandle Tsk_NrepSeal = {"Tsk_NrepSeal", func_ov028_020ecd08, sizeof(NrepSealTaskData)};

extern void ProcessOverlay_MenuTop(void* state);
extern void func_ov030_020ae92c();

extern NrepEnemyData**     data_ov003_020e70ac[];
extern s32                 data_ov028_020ecdc0[][2];
extern s32                 data_ov028_020ecdc4[][2];
extern s32                 data_ov028_020ecd58[][2];
extern s32                 data_ov028_020ecd5c[][2];
extern NoiseReportPending* data_ov028_020ed520;
extern NrepNoiseRecord     data_ov028_020ed524[];

typedef struct {
    s32 (*f[4])(TaskPool*, Task*, void*);
} NrepTaskFuncs;
extern NrepTaskFuncs            data_ov028_020ed210;
extern NrepIconLayout           data_ov028_020ed1f0;
extern const BinIdentifier      data_ov028_020ed1f4;
extern const BinIdentifier      data_ov028_020ed24c;
extern const BinIdentifier      data_ov028_020ed318;
extern const BinIdentifier      data_ov028_020ed3ac;
extern const BinIdentifier      data_ov028_020ed3c0;
extern NrepBgConfig             data_ov028_020ecf44[];
extern NrepBgConfig             data_ov028_020ed0a8[];
extern u16                      data_ov028_020ecff4[];
extern const u16                data_ov028_020ed04c[];
extern const NrepFontItemConfig data_ov028_020ed060[];
extern const NrepFontItemConfig data_ov028_020ed100[];
extern const NrepFontItemConfig data_ov028_020ed1cc;
extern const NrepFontItemConfig data_ov028_020ed1d8;
extern const NrepFontItemConfig data_ov028_020ed1e4;
extern const NrepInterDHitbox   data_ov028_020ed260[];
extern const NrepInterUEntry    data_ov028_020ed32c[];
extern SpriteAnimation          data_ov028_020ed220;
extern SpriteAnimation          data_ov028_020ecef8;

void func_ov028_020e73a4(s32 arg0, s32* arg1, s16* arg2);
void func_ov028_020e776c(NrepBgLayer* param_1, const NrepBgConfig* param_2);
void func_ov028_020e7798(NrepBgLayer* arg0, const NrepBgConfig* arg1);
void func_ov028_020e7888(NrepBgLayer* arg0, const NrepBgConfig* arg1);
void func_ov028_020e7970(NrepBgLayer* arg0, const NrepBgConfig* arg1);
void func_ov028_020e7a50(NrepBgLayer* arg0);
void func_ov028_020e7be8(NrepBgLayer* arg0);
s32  func_ov028_020e7c80(s32 param_1);
s32  func_ov028_020e7cc4(s32 param_1);
void func_ov028_020e7d08(TaskPool* pool, NoiseReport* arg1);
s32  func_ov028_020e7dbc(TaskPool* pool, NoiseReport* arg1);
void func_ov028_020e7f00(TaskPool* pool, NoiseReport* arg1);
void func_ov028_020eb7b0(void);
void func_ov028_020ea6f4(void);
void func_ov028_020ea26c(void);
void func_ov028_020e8bb0(void);
s32  func_ov028_020e8220(void);
void func_ov028_020e8278(void* arg0);
void func_ov028_020e8da0(void);

void func_ov028_020e7fc8(SpriteAnimation* arg0, s32 arg1, s32 arg2, const BinIdentifier* arg3, s16 arg4, s16 arg5, s16 arg6,
                         s16 arg7);
SpriteFrameInfo* func_ov028_020e7f2c(void* arg0, s32 arg1, s32 arg2);
void             func_ov028_020e80a0(Sprite* arg0, s16 arg1, u32 arg2, s16 arg3, s32 arg4);
BOOL             func_ov028_020e8158(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s16 param_5, s16 param_6);
void             func_ov028_020e8194(Sprite* arg0, s16 arg1, s32 arg2, s32 arg3);
void             func_ov028_020e81fc(Sprite* arg0, s16 arg1);
NrepNoiseRecord* func_ov028_020e8230(s32 arg0);
s32              func_ov028_020e8240(s32 arg0);
void             func_ov028_020e8ddc(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void             func_ov028_020e8e18(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void             func_ov028_020e8f0c(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void             func_ov028_020e8fe0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void             func_ov028_020e90b0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void             func_ov028_020e9160(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void func_ov028_020e91cc(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5);
void func_ov028_020e9464(NrepFont* arg0, SysFont* arg1, u16* arg2, s32 arg3);
s32  func_ov028_020e957c(NrepFont* arg0, s32 arg1, s32 arg2);
void func_ov028_020e9650(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5);
void func_ov028_020e9704(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5);
void func_ov028_020e9808(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void func_ov028_020e99a4(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void func_ov028_020e9a1c(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
void func_ov028_020e9ac0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4);
s32  func_ov028_020e9b38(NrepFont* arg0);
s32  func_ov028_020e9b64(NrepFont* arg0);
void func_ov028_020e9bcc(NrepFont* arg0, s32 arg1);
void func_ov028_020e9cd8(NrepFont* arg0);
void func_ov028_020e9f44(NrepFont* arg0);
s32  func_ov028_020ea058(NrepFont* arg0, void* arg1);
s32  func_ov028_020ea15c(NrepFont* arg0);
s32  func_ov028_020ea1a0(NrepFont* arg0);
s32  func_ov028_020ea1a8(NrepFont* arg0);
s16  func_ov028_020ea2a8(s16 arg0, s16 arg1);
void func_ov028_020ea3ec(NrepIcon* arg0);
s32  func_ov028_020ea730(NrepInterD* arg0);
void func_ov028_020ea888(NrepInterD* arg0, s32 arg1);
void func_ov028_020ea960(NrepInterD* arg0);
void func_ov028_020eaa00(NrepInterD* arg0, s32 arg1);
void func_ov028_020eaa90(NrepInterD* arg0, s32 arg1);
void func_ov028_020eab04(NrepInterD* arg0, s32 arg1);
s16  func_ov028_020eaba4(NrepInterD* arg0);
void func_ov028_020eacf8(NrepInterD* arg0);
void func_ov028_020eb054(NrepInterD* arg0);
s16  func_ov028_020eb0d4(NrepInterD* arg0);
s32  func_ov028_020eb174(NrepInterD* param_1, void* param_2);
s32  func_ov028_020eb3e8(NrepInterD* param_1);
s32  func_ov028_020eb654(NrepInterD* param_1);
s32  func_ov028_020eb718(NrepInterD* param_1);
s32  func_ov028_020eb7ec(NrepInterU* param_1, void* param_2);
s32  func_ov028_020eb8ec(NrepInterU* arg0);
s32  func_ov028_020eb918(NrepInterU* param_1);
s32  func_ov028_020eb9b0(NrepInterU* arg0);
void func_ov028_020ec590(NrepSealTaskData* arg0);
void func_ov028_020ec4f8(const BinIdentifier* arg0);
void func_ov028_020eba2c(const BinIdentifier* arg0);

void func_ov028_020e8878(NoiseReportState*);
void func_ov028_020e89d0(NoiseReportState*);
void func_ov028_020e8b68(NoiseReportState*);

// Looks up the combat enemy parameter record for a Noise Report entry index (0..95).
// Suggested name: NoiseReport_GetEnemyData
NrepEnemyData* func_ov028_020e7360(s32 arg0) {
    s16 sp0;
    s32 sp4;

    if (arg0 >= 0x60) {
        return NULL;
    }
    func_ov028_020e73a4(arg0, &sp4, &sp0);
    return data_ov003_020e70ac[sp4][sp0];
}

// Maps a Noise Report entry index to its enemy group and variant indices in the combat enemy table.
// Suggested name: NoiseReport_MapNoiseToEnemyIndices
void func_ov028_020e73a4(s32 arg0, s32* arg1, s16* arg2) {
    s32 i;
    s32 j;

    *arg1 = 0;
    *arg2 = 0;

    for (i = 0x26; i >= 0; i--) {
        if (arg0 < data_ov028_020ecdc0[i][0]) {
            continue;
        }
        *arg1 = data_ov028_020ecdc4[i][0];
        *arg2 = arg0 - data_ov028_020ecdc0[i][0];

        if (*arg1 > 0x23) {
            if (*arg1 > 0x2C) {
                if (*arg1 > 0x30) {
                    switch (*arg1) {
                        case 0x31:
                        case 0x33:
                        case 0x35:
                        case 0x36:
                            *arg2 = 0;
                            break;
                        case 0x37:
                            *arg2 = (arg0 == 0x53) ? 2 : 0;
                            break;
                        case 0x39:
                            *arg2 = (arg0 == 0x51) ? 2 : 0;
                            break;
                        default:
                            break;
                    }
                } else if (*arg1 == 0x30) {
                    *arg2 = 0;
                }
            } else if (*arg1 == 0x2C) {
                *arg2 = 1;
            } else if (*arg1 == 0x2B) {
                *arg2 = 0;
            }
            return;
        }
        if (*arg1 == 0x23) {
            *arg2 = 0;
            return;
        }
        if (*arg1 > 0x1E) {
            if (*arg1 > 0x21) {
                if (*arg1 == 0x22) {
                    *arg2 = 0;
                }
            } else if (*arg1 == 0x21) {
                *arg2 = arg0 - 0x57;
            }
            return;
        }
        if (*arg1 == 0x1E) {
            *arg2 = arg0 - 0x4C;
            return;
        }
        if (*arg1 > 0x11) {
            return;
        }

        switch (*arg1) {
            case 1:
                switch (*arg2) {
                    case 5:
                        *arg2 = 6;
                        break;
                    case 6:
                        *arg2 = 7;
                        break;
                    case 7:
                        *arg2 = 8;
                        break;
                    case 8:
                        *arg2 = 5;
                        break;
                    default:
                        break;
                }
                break;
            case 6:
                if (*arg2 == 2) {
                    *arg2 = 3;
                } else if (*arg2 == 3) {
                    *arg2 = 2;
                }
                break;
            case 8:
                if (*arg2 == 3) {
                    *arg2 = 4;
                }
                break;
            case 9:
                if (*arg2 == 4) {
                    *arg2 = 5;
                } else if (*arg2 == 5) {
                    *arg2 = 4;
                }
                break;
            case 10:
                if (*arg2 == 2) {
                    *arg2 = 3;
                } else if (*arg2 == 3) {
                    *arg2 = 2;
                }
                break;
            case 11:
                if (*arg2 == 1) {
                    *arg2 = 2;
                } else if (*arg2 == 2) {
                    *arg2 = 1;
                }
                break;
            case 12:
                if (*arg2 == 2) {
                    *arg2 = 1;
                } else if (*arg2 == 1) {
                    *arg2 = 2;
                }
                break;
            case 17:
                for (j = 0; j < 0xD; j++) {
                    if (arg0 == data_ov028_020ecd58[j][0]) {
                        *arg1 = data_ov028_020ecd5c[j][0];
                        *arg2 = 0;
                        break;
                    }
                }
                switch (*arg1 - 0x11) {
                    case 0:
                    case 1:
                        *arg2 = 1;
                        break;
                    case 4:
                        *arg2 = 5;
                        break;
                    case 6:
                        *arg2 = 0;
                        break;
                    case 7:
                        *arg2 = 1;
                        break;
                    case 8:
                        *arg2 = 0;
                        break;
                    case 9:
                    case 10:
                        *arg2 = 1;
                        break;
                    case 11:
                    case 12:
                        *arg2 = 0;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
        return;
    }
}

// Loads screen, character, and palette resources for a background layer from its config.
// Suggested name: NrepBg_LoadLayerResources
void func_ov028_020e776c(NrepBgLayer* param_1, const NrepBgConfig* param_2) {
    func_ov028_020e7798(param_1, param_2);
    func_ov028_020e7888(param_1, param_2);
    func_ov028_020e7970(param_1, param_2);
}

// Loads the archive data if needed and allocates the BG screen resource for a layer.
// Suggested name: NrepBg_LoadScreen
void func_ov028_020e7798(NrepBgLayer* arg0, const NrepBgConfig* arg1) {
    void* screenData;

    if (arg0->data == NULL) {
        if (arg1->packIndex <= 0) {
            arg0->data = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)arg1->binIden);
        } else {
            arg0->data = DatMgr_LoadPackEntry(1, NULL, 0, (BinIdentifier*)arg1->binIden, arg1->packIndex, FALSE);
        }
    }
    screenData = Data_GetPackEntryData(arg0->data, arg1->screenEntry);
    if (arg0->screenRes != NULL) {
        return;
    }
    arg0->screenRes = BgResMgr_AllocScreen(g_BgResourceManagers[arg0->engine], screenData,
                                           g_DisplaySettings.engineState[arg0->engine].bgSettings[arg0->layer].screenBase,
                                           g_DisplaySettings.engineState[arg0->engine].bgSettings[arg0->layer].screenSizeText);
}

// Loads the archive data if needed and allocates the BG character tile resource for a layer.
// Suggested name: NrepBg_LoadChar
void func_ov028_020e7888(NrepBgLayer* arg0, const NrepBgConfig* arg1) {
    void* charData;

    if (arg0->data == NULL) {
        if (arg1->packIndex <= 0) {
            arg0->data = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)arg1->binIden);
        } else {
            arg0->data = DatMgr_LoadPackEntry(1, NULL, 0, (BinIdentifier*)arg1->binIden, arg1->packIndex, FALSE);
        }
    }
    charData = Data_GetPackEntryData(arg0->data, arg1->charEntry);
    if (arg0->charRes != NULL) {
        return;
    }
    arg0->charRes =
        BgResMgr_AllocChar32(g_BgResourceManagers[arg0->engine], charData,
                             g_DisplaySettings.engineState[arg0->engine].bgSettings[arg0->layer].charBase, 0, arg1->charSize);
}

// Loads the archive data if needed and allocates and flushes the palette resource for a layer.
// Suggested name: NrepBg_LoadPalette
void func_ov028_020e7970(NrepBgLayer* arg0, const NrepBgConfig* arg1) {
    void* palData;

    if (arg0->data == NULL) {
        if (arg1->packIndex <= 0) {
            arg0->data = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier*)arg1->binIden);
        } else {
            arg0->data = DatMgr_LoadPackEntry(1, NULL, 0, (BinIdentifier*)arg1->binIden, arg1->packIndex, FALSE);
        }
    }
    palData = Data_GetPackEntryData(arg0->data, arg1->palEntry);
    if (arg0->palRes != NULL) {
        return;
    }
    arg0->palRes = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[arg0->engine], palData, 0, arg1->palStart, arg1->palCount);
    PaletteMgr_Flush(g_PaletteManagers[arg0->engine], arg0->palRes);
}

// Applies scroll offsets, visibility, priority, and blending settings for a BG layer to g_DisplaySettings.
// Suggested name: NrepBg_CommitLayerSettings
void func_ov028_020e7a50(NrepBgLayer* arg0) {
    Display_SetBGOffset(arg0->engine, arg0->layer, arg0->hOffset, arg0->vOffset);

    if (arg0->visible != 0) {
        s32                     mask     = func_ov028_020e7cc4(arg0->layer);
        DisplayControlSettings* controls = &g_DisplaySettings.controls[arg0->engine];
        if (mask < 0) {
            controls->layers = LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ;
        } else {
            controls->layers |= mask;
        }
    } else {
        s32                     mask     = func_ov028_020e7cc4(arg0->layer);
        DisplayControlSettings* controls = &g_DisplaySettings.controls[arg0->engine];
        if (mask < 0) {
            controls->layers = LAYER_NONE;
        } else {
            controls->layers &= ~mask;
        }
    }

    g_DisplaySettings.engineState[arg0->engine].bgSettings[arg0->layer].priority = arg0->priority;
    if (arg0->blendLayer1 != 0) {
        g_DisplaySettings.engineState[arg0->engine].blendMode   = 1;
        g_DisplaySettings.engineState[arg0->engine].blendLayer0 = func_ov028_020e7c80(arg0->layer);
        g_DisplaySettings.engineState[arg0->engine].blendLayer1 = arg0->blendLayer1;
        g_DisplaySettings.engineState[arg0->engine].blendCoeff0 = arg0->blendCoeff0;
        g_DisplaySettings.engineState[arg0->engine].blendCoeff1 = arg0->blendCoeff1;
    }
}

// Releases the palette, screen, character, and archive data resources owned by a BG layer.
// Suggested name: NrepBg_ReleaseLayerResources
void func_ov028_020e7be8(NrepBgLayer* arg0) {
    if (arg0->palRes != NULL) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[arg0->engine], arg0->palRes);
        arg0->palRes = NULL;
    }
    if (arg0->screenRes != NULL) {
        BgResMgr_ReleaseScreen(g_BgResourceManagers[arg0->engine], arg0->screenRes);
        arg0->screenRes = NULL;
    }
    if (arg0->charRes != NULL) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[arg0->engine], arg0->charRes);
        arg0->charRes = NULL;
    }
    if (arg0->data != NULL) {
        DatMgr_ReleaseData(arg0->data);
        arg0->data = NULL;
    }
}

// Converts a BG layer index (0..3) into its corresponding blend layer bitmask (1 << layer).
// Suggested name: NrepBg_GetBlendLayerMask
s32 func_ov028_020e7c80(s32 param_1) {
    switch (param_1) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 4;
        case 3:
            return 8;
        default:
            return 0;
    }
}

// Converts a BG layer index (0..3) into its corresponding display visibility bitmask (1 << layer).
// Suggested name: NrepBg_GetLayerVisibilityMask
s32 func_ov028_020e7cc4(s32 param_1) {
    switch (param_1) {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 4;
        case 3:
            return 8;
        default:
            return 0;
    }
}

// Initializes the Noise Report controller state, loads overlays 3 and 31, creates UI tasks, and starts BGM.
// Suggested name: NoiseReport_InitController
void func_ov028_020e7d08(TaskPool* pool, NoiseReport* arg1) {
    MI_CpuSet(arg1, 0, sizeof(NoiseReport));
    data_ov028_020ed3e0 = arg1;
    data_ov028_020ed520 = &arg1->pending;
    OvlMgr_LoadOverlay(3, 3);
    OvlMgr_LoadOverlay(4, 0x1F);
    func_ov028_020e8278(arg1);
    arg1->pool   = pool;
    arg1->unk_00 = DatMgr_AllocateSlot();
    func_ov028_020ea26c();
    func_ov028_020e8bb0();
    func_ov028_020eb7b0();
    func_ov028_020ea6f4();
    if (func_ov028_020e8220() == 0) {
        arg1->unk_38 = 0x1E;
    }
    data_ov028_020ed520->helpPage = 1;
    arg1->unk_10                  = 1;
    CriSndMgr_PlayFile(gSaveData.bgmFile);
}

// Per-frame update for the Noise Report controller: polls input/touch, handles initial help popup delay, and syncs state.
// Suggested name: NoiseReport_UpdateController
s32 func_ov028_020e7dbc(TaskPool* pool, NoiseReport* arg1) {
    if (EasyFade_IsFading()) {
        arg1->unk_28       = 0;
        arg1->unk_2A       = 0;
        arg1->unk_2C       = 0;
        arg1->touchCoord.x = 0;
        arg1->touchCoord.y = 0;
    } else if (arg1->unk_38 > 0) {
        arg1->unk_28       = 0;
        arg1->unk_2A       = 0;
        arg1->unk_2C       = 0;
        arg1->touchCoord.x = 0;
        arg1->touchCoord.y = 0;
        arg1->unk_38--;
        if (arg1->unk_38 <= 0) {
            if (func_ov028_020e8220() == 0) {
                data_ov028_020ed520->helpOpen = 1;
                data_ov028_020ed520->helpPage = 1;
                arg1->unk_10                  = 1;
                SndMgr_StartPlayingSE(0x11C);
            }
        }
    } else {
        arg1->unk_28 = InputStatus.buttonState.pressedButtons;
        arg1->unk_2A = InputStatus.buttonState.currButtons;
        arg1->unk_2C = TouchInput_WasTouchPressed();
        {
            TouchCoord coord;
            TouchInput_GetCoord(&coord);
            arg1->touchCoord = coord;
        }
    }

    arg1->unk_08 = data_ov028_020ed520->exitFlag;
    arg1->unk_0C = data_ov028_020ed520->helpOpen;
    arg1->unk_10 = data_ov028_020ed520->helpPage;
    arg1->unk_12 = data_ov028_020ed520->dropRateMode;
    arg1->unk_14 = data_ov028_020ed520->scrollRow;
    arg1->unk_16 = data_ov028_020ed520->selectedNoise;
    return (arg1->unk_08 != 0) ? 1 : 0;
}

// Stops BGM, clears the DatMgr slot, and unloads overlays 31 and 3 when exiting the Noise Report menu.
// Suggested name: NoiseReport_DestroyController
void func_ov028_020e7f00(TaskPool* pool, NoiseReport* arg1) {
    CriSndMgr_Stop(ADX_TITLE);
    DatMgr_ClearSlot(arg1->unk_00);
    OvlMgr_UnloadOverlay(4);
    OvlMgr_UnloadOverlay(3);
}

// Frame-info callback for Noise Report sprites: fills g_SpriteFrameInfo for update and render modes.
// Suggested name: NoiseReport_GetSpriteFrameInfo
SpriteFrameInfo* func_ov028_020e7f2c(void* arg0, s32 arg1, s32 arg2) {
    Sprite* sp = (Sprite*)arg0;
    Sprite_FrameInfoCallback(sp, arg2);
}

// Initializes a SpriteAnimation descriptor from the default template and sets resource/entry indices.
// Suggested name: NoiseReport_InitSpriteAnim
void func_ov028_020e7fc8(SpriteAnimation* arg0, s32 arg1, s32 arg2, const BinIdentifier* arg3, s16 arg4, s16 arg5, s16 arg6,
                         s16 arg7) {
    SpriteAnimation anim = data_ov028_020ecef8;

    MI_CpuCopyU8(&anim, arg0, sizeof(SpriteAnimation));
    arg0->bits_0_1  = arg1 & 3;
    arg0->dataType  = arg2 & 0xF;
    arg0->binIden   = arg3;
    arg0->unk_18    = 0;
    arg0->packIndex = 0;
    arg0->unk_1C    = arg4;
    arg0->unk_26    = arg5;
    arg0->unk_28    = arg6;
    arg0->unk_20    = arg7;
    arg0->unk_22    = 1;
    arg0->bits_7_9  = 5;
}

// Updates an array of digit sprites to display an unsigned integer in base 10.
// Suggested name: NoiseReport_SetNumberSprites
void func_ov028_020e80a0(Sprite* arg0, s16 arg1, u32 arg2, s16 arg3, s32 arg4) {
    s16 i = arg3;

    if (arg4 == 0 && arg2 == 0) {
        func_ov028_020e81fc(arg0, arg1);
        i = arg3 - 1;
        arg0++;
    }

    for (; i > 0; i--) {
        if (arg4 == 0 && arg2 == 0) {
            func_ov028_020e81fc(arg0, 0);
        } else {
            func_ov028_020e81fc(arg0, (s16)(arg2 % 10 + arg1));
            arg2 = arg2 / 10;
        }
        arg0++;
    }
}

// Tests whether point (param_1, param_2) lies inside the axis-aligned rectangle (param_3, param_4, param_5, param_6).
// Suggested name: NoiseReport_IsPointInRect
BOOL func_ov028_020e8158(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s16 param_5, s16 param_6) {
    if (param_1 >= param_3 && param_1 <= param_3 + param_5 && param_2 >= param_4 && param_2 <= param_4 + param_6) {
        return TRUE;
    }
    return FALSE;
}

// Changes a sprite's animation and cell table by looking up pack entries arg2 and arg3 in its loaded Data.
// Suggested name: NoiseReport_ChangeSpritePackAnim
void func_ov028_020e8194(Sprite* arg0, s16 arg1, s32 arg2, s32 arg3) {
    Data*       data      = arg0->resourceData;
    void*       animData  = Data_GetPackEntryData(data, arg2);
    SpriteCell* cellTable = (SpriteCell*)Data_GetPackEntryData(data, arg3);

    Sprite_ChangeAnimation(arg0, animData, arg1, cellTable);
}

// Sets a sprite's animation index if it differs from the currently active animation index.
// Suggested name: NoiseReport_SetSpriteAnim
void func_ov028_020e81fc(Sprite* arg0, s16 arg1) {
    if (arg0->animIndex == arg1) {
        return;
    }
    Sprite_SetAnimation(arg0, arg0->animData, arg1, arg0->cellTable);
}

// Checks whether the Noise Report help screen has already been viewed in the player's save data.
// Suggested name: NoiseReport_IsHelpSeen
s32 func_ov028_020e8220(void) {
    return Inventory_IsHelpSeen(2);
}

// Returns a pointer to the cached Noise Report record for the given noise index.
// Suggested name: NoiseReport_GetNoiseRecord
NrepNoiseRecord* func_ov028_020e8230(s32 arg0) {
    return &data_ov028_020ed524[arg0];
}

// Checks if a Noise Report entry is locked/unseen or out of range (returns 1 if locked, 0 if unlocked).
// Suggested name: NoiseReport_IsNoiseLocked
s32 func_ov028_020e8240(s32 arg0) {
    extern s16 data_ov028_020ed526[];
    s16        val = data_ov028_020ed526[arg0 * 4];
    if (val == 0 || arg0 >= 0x60 || arg0 < 0) {
        return 1;
    }
    return 0;
}

// Populates the 96 cached Noise Report records from gSaveData.
// Suggested name: NoiseReport_LoadNoiseRecordsFromSave
void func_ov028_020e8278(void* arg0) {
    // gSaveData keeps the three small entry fields in a 4-byte-stride table at 0x1FCA,
    // and the best times in a separate 4-byte-stride table at 0x214C.
    for (s32 i = 0; i < 0x60; i++) {
        u8* src                          = (u8*)&gSaveData + i * 4;
        data_ov028_020ed524[i].killCount = *(s8*)(src + 0x1FCA);
        data_ov028_020ed524[i].dropFlags = *(s8*)(src + 0x1FCB);
        data_ov028_020ed524[i].seenFlag  = *(s16*)(src + 0x1FCC);
        data_ov028_020ed524[i].bestTime  = *(u32*)(src + 0x214C);
    }
}

const OverlayProcess OvlProc_NoiseReport = {
    .init = func_ov028_020e8878,
    .main = func_ov028_020e89d0,
    .exit = func_ov028_020e8b68,
};

// Overlay process dispatcher entry point for the Noise Report menu (overlay 28).
// Suggested name: ProcessOverlay_NoiseReport
void func_ov028_020e82d0(NoiseReportState* state) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        func_ov028_020e8b68(state);
        return;
    }
    OvlProc_NoiseReport.funcs[stage](state);
}

// VBlank interrupt callback for the Noise Report menu: commits display, DMA, OAM, and palette buffers.
// Suggested name: NoiseReport_VBlankCallback
void func_ov028_020e8310(void) {
    if (SystemStatusFlags.vblank != FALSE) {
        Display_Commit();
        DMA_Flush();
        OamMgr_Commit();
        DC_PurgeRange(&data_02066aec, 1024);
        GX_LoadBgPltt(&data_02066aec, 0, 0x200);
        GX_LoadObjPltt(&data_02066cec, 0, 0x200);
        DC_PurgeRange(&data_02066eec, 1024);
        GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
        GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
        func_02001b44(2, NULL, &data_020672ec, 0x400);
    }
}

// Initializes hardware graphics, VRAM banks, BG layers, OAM managers, and VBlank callback for Noise Report.
// Suggested name: NoiseReport_InitHardware
// Nonmatching
void func_ov028_020e83f8(void) {
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    func_0202b878();
    DMA_Init(256);
    Display_Init();
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1ff);
    GX_SetBankForTex(1);
    GX_SetBankForTexPltt(0x40);
    GX_SetBankForBg(2);
    GX_SetBankForObj(0x10);
    GX_SetBankForBgExtPltt(0);
    GX_SetBankForObjExtPltt(0);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    GX_SetBankForSubBgExtPltt(0);
    GX_SetBankForSubObjExtPltt(0);
    MI_CpuFill(0, 0x06800000, 0xa4000);
    MI_CpuFill(0, 0x6000000, 0x80000);
    MI_CpuFill(0, 0x06200000, 0x20000);
    MI_CpuFill(0, 0x06400000, 0x40000);
    MI_CpuFill(0, 0x06600000, 0x20000);
    REG_POWER_CNT &= ~0x8000;
    Display_CommitSynced();

    g_DisplaySettings.controls[DISPLAY_MAIN].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[DISPLAY_MAIN].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[DISPLAY_MAIN].dimension = GX2D3D_MODE_3D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_3D);

    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 1, 0, 0x4);
    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 1, 3, 1, 0x10C);
    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 5, 1, 0x214);

    g_DisplaySettings.engineState[0].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[0].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic = 0;

    g_DisplaySettings.controls[DISPLAY_MAIN].objTileMode = GX_OBJTILEMODE_1D_32K;
    g_DisplaySettings.controls[DISPLAY_MAIN].objBmpMode  = GX_OBJBMPMODE_1D_128K;

    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode  = GX_OBJBMPMODE_1D_128K;

    data_0206aa78 = 0x300010;
    data_0206aa7c = 0x400040;

    Display_SetMainLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    g_DisplaySettings.controls[DISPLAY_MAIN].brightness = 16;
    g_DisplaySettings.engineState[0].blendMode          = 1;
    g_DisplaySettings.engineState[0].blendLayer0        = 1;
    g_DisplaySettings.engineState[0].blendLayer1        = 62;
    g_DisplaySettings.engineState[0].blendCoeff0        = 6;
    g_DisplaySettings.engineState[0].blendCoeff1        = 10;
    g_DisplaySettings.controls[DISPLAY_SUB].bgMode      = GX_BGMODE_0;
    GXs_SetGraphicsMode(0);

    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 1, 1, 0, 0x104);
    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 3, 1, 0x20C);
    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 3, 5, 1, 0x314);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 2;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[0].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic   = 0;
    g_DisplaySettings.engineState[1].bgSettings[3].mosaic   = 0;

    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    Display_SetSubLayers(LAYER_BG0 | LAYER_BG1 | LAYER_BG2 | LAYER_BG3 | LAYER_OBJ);
    g_DisplaySettings.controls[DISPLAY_SUB].brightness = 16;

    OamMgr_Init3DSpritePipeline();
    OamMgr_Swap3DBuffers();
    OamMgr_Init();
    OamMgr_InitEngine(0, DISPLAY_EXTENDED);
    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);
    Interrupts_RegisterVBlankCallback(func_ov028_020e8310, TRUE);
}

// Unregisters the VBlank interrupt callback when leaving the Noise Report menu.
// Suggested name: NoiseReport_ClearVBlankCallback
void func_ov028_020e87f4(void) {
    Interrupts_RegisterVBlankCallback(NULL, TRUE);
}

// Fade-in stage callback for DebugOvlDisp: fades both screens in and pops when finished.
// Suggested name: NoiseReport_StageFadeIn
void func_ov028_020e8808(void* unused) {
    EasyFade_FadeBothDisplays(FADER_LINEAR, 0, 0x1000);
    if (EasyFade_IsFading() == FALSE) {
        DebugOvlDisp_Pop();
    }
}

// Main active stage callback for DebugOvlDisp: waits until the exit flag is set, then pops to fade out.
// Suggested name: NoiseReport_StageMain
void func_ov028_020e8830(void* state) {
    if (((NoiseReportState*)state)->unk_2164C != 0) {
        DebugOvlDisp_Pop();
    }
}

// Fade-out stage callback for DebugOvlDisp: fades both screens to black and pops when finished.
// Suggested name: NoiseReport_StageFadeOut
void func_ov028_020e8850(void* unused) {
    EasyFade_FadeBothDisplays(FADER_LINEAR, 16, 0x1000);
    if (EasyFade_IsFading() == FALSE) {
        DebugOvlDisp_Pop();
    }
}

const char* data_ov028_020ed3e4 = "Seq_Nrep()";

// Overlay init callback: allocates state, initializes hardware/managers/tasks, and pushes fade stages.
// Suggested name: NoiseReport_Init
void func_ov028_020e8878(NoiseReportState* state) {
    if (state == NULL) {
        const char* sequence = data_ov028_020ed3e4;
        state                = Mem_AllocHeapTail(&gDebugHeap, sizeof(NoiseReportState));
        Mem_SetSequence(&gDebugHeap, state, sequence);
        MainOvlDisp_SetCbArg(state);
    }

    func_ov028_020e83f8();

    state->unk_11580 = ResourceMgr_ReinitManagers(&state->unk_00000);
    TouchInput_Init();
    Mem_InitializeHeap(&state->heap, &state->memBlock, 0x10000);
    EasyTask_InitializePool(&state->taskPool, &state->heap, 512, NULL, NULL);
    data_02066aec = 0;
    data_02066eec = 0;
    EasyTask_CreateTask(&state->taskPool, &Task_EasyFade, NULL, 0, NULL, 0);
    EasyFade_FadeBothDisplays(FADER_SMOOTH, 16, 0x1000);
    func_ov028_020e7d08(&state->taskPool, &state->unk_21610);
    DebugOvlDisp_Init();
    DebugOvlDisp_Push(func_ov028_020e8850, state, 0);
    DebugOvlDisp_Push(func_ov028_020e8830, state, 0);
    DebugOvlDisp_Push(func_ov028_020e8808, state, 0);
    MainOvlDisp_NextProcessStage();

    state->unk_2164C = 0;
}

// Overlay main loop callback: updates touch/OAM/tasks, flushes graphics, and transitions to the next overlay on exit.
// Suggested name: NoiseReport_Main
void func_ov028_020e89d0(NoiseReportState* state) {
    TouchInput_Update();
    OamMgr_Reset3DState();
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);
    DebugOvlDisp_Run();

    if (state->unk_2164C == 0) {
        state->unk_2164C = func_ov028_020e7dbc(&state->taskPool, &state->unk_21610);
    }

    EasyTask_ProcessPendingTasks(&state->taskPool);
    EasyTask_UpdateActiveTasks(&state->taskPool);

    BOOL isAtBase = DebugOvlDisp_IsStackAtBase();

    OamMgr_Swap3DBuffers();
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_SUB]);
    PaletteMgr_Flush(g_PaletteManagers[DISPLAY_MAIN], NULL);
    PaletteMgr_Flush(g_PaletteManagers[DISPLAY_SUB], NULL);
    PaletteMgr_Flush(g_PaletteManagers[DISPLAY_EXTENDED], NULL);

    if (isAtBase) {
        switch (state->unk_2164C) {
            case 1: {
                OverlayTag tag;
                MainOvlDisp_ReplaceTop(&tag, (s32)&OVERLAY_43_ID, ProcessOverlay_MenuTop, NULL, 0);
            } break;

            case 2: {
                OverlayTag tag;
                MainOvlDisp_ReplaceTop(&tag, (s32)&OVERLAY_30_ID, func_ov030_020ae92c, NULL, 0);
            } break;

            default: {
                OverlayTag tag;
                MainOvlDisp_ReplaceTop(&tag, (s32)&OVERLAY_43_ID, ProcessOverlay_MenuTop, NULL, 0);
            } break;
        }
    }
}

// Overlay exit callback: cleans up the controller, task pool, resource managers, and state heap allocation.
// Suggested name: NoiseReport_Exit
void func_ov028_020e8b68(NoiseReportState* state) {
    func_ov028_020e7f00(&state->taskPool, &state->unk_21610);
    EasyTask_DestroyPool(&state->taskPool);
    ResourceMgr_ReinitManagers(NULL);
    func_ov028_020e87f4();
    Mem_Free(&gDebugHeap, state);
}

// Spawns the background management task (Tsk_NrepBG).
// Suggested name: NrepBG_CreateTask
void func_ov028_020e8bb0(void) {

    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &data_ov028_020ecf38, NULL, 0, NULL, 0);
}

// Initializes Tsk_NrepBG: loads all four background layers, spawns Tsk_Font, and sets initial blend parameters.
// Suggested name: NrepBG_Init
s32 func_ov028_020e8bec(NrepBG* arg0, s32 arg1) {
    const NrepBgConfig* src = data_ov028_020ecf44;
    NrepBgLayer*        dst = arg0->layers;

    MI_CpuSet(arg0, 0, sizeof(NrepBG));
    for (s32 i = 0; i < 4; i++) {
        dst->engine   = src->engine;
        dst->layer    = src->layer;
        dst->priority = src->priority;
        dst->hOffset  = 0;
        dst->vOffset  = 0;
        dst->visible  = 1;
        func_ov028_020e776c(dst, src);
        dst++;
        src++;
    }
    func_ov028_020e8da0();
    arg0->layers[1].blendCoeff0 = 6;
    arg0->layers[1].blendCoeff1 = 0xA;
    arg0->layers[1].blendLayer1 = 0x3F;
    arg0->layers[2].blendCoeff0 = 0x1F;
    arg0->layers[2].blendCoeff1 = 0x1F;
    arg0->layers[2].blendLayer1 = 0x30;
    return 1;
}

// Updates Tsk_NrepBG layer priorities, visibility, and blend coefficients based on whether help is open.
// Suggested name: NrepBG_Update
s32 func_ov028_020e8c94(NrepBG* arg0) {
    if (data_ov028_020ed3e0->unk_0C != 0) {
        arg0->layers[3].priority    = 1;
        arg0->layers[1].visible     = 1;
        arg0->layers[2].blendCoeff0 = 6;
        arg0->layers[2].blendCoeff1 = 0;
        arg0->layers[2].priority    = 1;
    } else {
        arg0->layers[3].priority    = 3;
        arg0->layers[1].visible     = 0;
        arg0->layers[2].blendCoeff0 = 0x1F;
        arg0->layers[2].blendCoeff1 = 0x1F;
        arg0->layers[2].priority    = 2;
    }
    return 1;
}

// Renders/commits Tsk_NrepBG background layer settings to g_DisplaySettings.
// Suggested name: NrepBG_Render
s32 func_ov028_020e8cfc(NrepBG* arg0) {
    func_ov028_020e7a50(&arg0->layers[3]);
    func_ov028_020e7a50(&arg0->layers[2]);
    func_ov028_020e7a50(&arg0->layers[1]);
    return 1;
}

// Cleans up Tsk_NrepBG by releasing all four background layer resources.
// Suggested name: NrepBG_Destroy
s32 func_ov028_020e8d24(NrepBG* arg0) {
    NrepBgLayer* layer = arg0->layers;
    for (s32 i = 0; i < 4; i++) {
        func_ov028_020e7be8(layer);
        layer++;
    }
    return 1;
}

// Task dispatcher for Tsk_NrepBG.
// Suggested name: NrepBG_RunTask
s32 func_ov028_020e8d50(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    NrepBG* data = (NrepBG*)task->data;
    switch (arg3) {
        case 0:
            return func_ov028_020e8bec(data, arg2);
        case 1:
            return func_ov028_020e8c94(data);
        case 2:
            return func_ov028_020e8cfc(data);
        case 3:
            return func_ov028_020e8d24(data);
        default:
            return 0;
    }
}

// Spawns the text rendering task (Tsk_Font).
// Suggested name: NrepFont_CreateTask
void func_ov028_020e8da0(void) {

    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &data_ov028_020ed030, NULL, 0, NULL, 0);
}

// Draws a static label message from the font item config directly to the screen buffer.
// Suggested name: NrepFont_DrawStaticLabel
void func_ov028_020e8ddc(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    SysFont_SetMsg(arg1, (u16)arg2->msgId);
    SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
}

// Draws the selected Noise's name centered around the configured X coordinate, or a placeholder if locked.
// Suggested name: NrepFont_DrawNoiseName
void func_ov028_020e8e18(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    s16 idx = data_ov028_020ed3e0->unk_16;
    s32 width;

    if (idx >= 0x60) {
        return;
    }

    func_ov028_020e8230(idx);
    if (func_ov028_020e8240(idx) != 0) {
        SysFont_SetMsg(arg1, (u16)arg2->msgId);
        width = SysFont_MeasureCurrentWidth(arg1);
        SysFont_SetPos(arg1, arg2->x - (width >> 1), arg2->y);
        SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
    } else {
        SysFont_SetMsg(arg1, (u16)(idx + 0x2DC5));
        width = SysFont_MeasureCurrentWidth(arg1);
        SysFont_SetPos(arg1, arg2->x - (width >> 1), arg2->y);
        SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
    }
}

// Formats and draws the 1-based Noise entry number ("No. XX") to the screen buffer.
// Suggested name: NrepFont_DrawNoiseNumber
void func_ov028_020e8f0c(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    u16  fmt[3];
    u16  buf[21];
    s16  idx    = data_ov028_020ed3e0->unk_16;
    u16* msgBuf = SysFont_GetMsgBuf(arg1, (u16)arg2->msgId);
    s16  num    = idx + 1;

    if (num >= 0x61) {
        fmt[0] = data_ov028_020ecff4[4];
        fmt[1] = data_ov028_020ecff4[5];
        SysFont_Format(buf, fmt, msgBuf);
    } else {
        fmt[0] = data_ov028_020ecff4[6];
        fmt[1] = data_ov028_020ecff4[7];
        fmt[2] = data_ov028_020ecff4[8];
        SysFont_Format(buf, fmt, msgBuf, num / 10, num % 10);
    }

    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
    Mem_Free(&gDebugHeap, msgBuf);
}

// Formats and draws the selected Noise's kill count centered at the configured position.
// Suggested name: NrepFont_DrawKillCount
void func_ov028_020e8fe0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    u16              fmt[2];
    u16              buf[20];
    s16              idx = data_ov028_020ed3e0->unk_16;
    NrepNoiseRecord* entry;
    s32              val;

    fmt[0] = data_ov028_020ecff4[0];
    fmt[1] = data_ov028_020ecff4[1];

    if (idx >= 0x60) {
        return;
    }

    entry = func_ov028_020e8230(idx);
    if (func_ov028_020e8240(idx) != 0) {
        val = 0;
    } else {
        val = entry->killCount;
    }

    SysFont_Format(buf, fmt, (u16)val);
    val = SysFont_MeasureWidth(arg1, buf);
    SysFont_SetPos(arg1, arg2->x - (val >> 1), arg2->y);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
}

// Formats and draws the selected Noise's seen/total encounter count centered at the configured position.
// Suggested name: NrepFont_DrawSeenCount
void func_ov028_020e90b0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    u16              fmt[2];
    u16              buf[20];
    s16              idx = data_ov028_020ed3e0->unk_16;
    NrepNoiseRecord* entry;
    s32              width;

    if (idx >= 0x60) {
        return;
    }

    entry  = func_ov028_020e8230(idx);
    fmt[0] = data_ov028_020ecff4[2];
    fmt[1] = data_ov028_020ecff4[3];
    SysFont_Format(buf, fmt, entry->seenFlag);
    width = SysFont_MeasureWidth(arg1, buf);
    SysFont_SetPos(arg1, arg2->x - (width >> 1), arg2->y);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
}

// Draws the selected Noise's flavor/help description text if the entry is unlocked.
// Suggested name: NrepFont_DrawNoiseDescription
void func_ov028_020e9160(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    s16 idx = data_ov028_020ed3e0->unk_16;

    if (func_ov028_020e8240(idx) != 0) {
        return;
    }

    func_ov028_020e8230(idx);
    SysFont_SetMsg(arg1, (u16)(arg2->msgId + idx));
    SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
}

// Draws the dropped pin name for one of the four difficulty slots (Easy, Normal, Hard, Ultimate), or placeholder if
// unobtained. Suggested name: NrepFont_DrawDropPinName
void func_ov028_020e91cc(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5) {
    u16              buf[22];
    NrepNoiseRecord* entry;
    NrepEnemyData*   p;
    u16*             msgBuf = NULL;
    s16              idx    = data_ov028_020ed3e0->unk_16;
    s32              v;

    if (idx >= 0x60) {
        return;
    }

    entry = func_ov028_020e8230(idx);
    p     = func_ov028_020e7360((s16)idx);

    switch (arg5) {
        case 4: {
            s32 m = 0x2E28;
            v     = p->drops[3];
            if (entry->dropFlags & 1) {
                if ((v >= 0 && v < 0x130) || (v >= 0x3E8 && v < 0x518)) {
                    if (v >= 0x3E8) {
                        v -= 0x3E8;
                    }
                    m = v + 0x27C1;
                }
            }
            msgBuf = SysFont_GetMsgBuf(arg1, (u16)m);
            break;
        }
        case 7: {
            s32 m = 0x2E28;
            v     = p->drops[2];
            if (entry->dropFlags & 2) {
                if ((v >= 0 && v < 0x130) || (v >= 0x3E8 && v < 0x518)) {
                    if (v >= 0x3E8) {
                        v -= 0x3E8;
                    }
                    m = v + 0x27C1;
                }
            }
            msgBuf = SysFont_GetMsgBuf(arg1, (u16)m);
            break;
        }
        case 10: {
            s32 m = 0x2E28;
            v     = p->drops[1];
            if (entry->dropFlags & 4) {
                if ((v >= 0 && v < 0x130) || (v >= 0x3E8 && v < 0x518)) {
                    if (v >= 0x3E8) {
                        v -= 0x3E8;
                    }
                    m = v + 0x27C1;
                }
            }
            msgBuf = SysFont_GetMsgBuf(arg1, (u16)m);
            break;
        }
        case 13: {
            s32 m = 0x2E28;
            v     = p->drops[0];
            if (entry->dropFlags & 8) {
                if ((v >= 0 && v < 0x130) || (v >= 0x3E8 && v < 0x518)) {
                    if (v >= 0x3E8) {
                        v -= 0x3E8;
                    }
                    m = v + 0x27C1;
                }
            }
            msgBuf = SysFont_GetMsgBuf(arg1, (u16)m);
            break;
        }
    }

    SysFont_Format(buf, msgBuf);
    v = SysFont_MeasureWidth(arg1, buf);
    SysFont_SetPos(arg1, arg2->x - (v >> 1), arg2->y);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
    Mem_Free(&gDebugHeap, msgBuf);
}

// Formats the integer portion of a drop rate percentage (100, 10..99, or 0..9) into the output buffer.
// Suggested name: NrepFont_FormatDropRateInt
void func_ov028_020e9464(NrepFont* arg0, SysFont* arg1, u16* arg2, s32 arg3) {
    if (arg3 >= 100) {
        u16 fmt[5];
        fmt[0] = data_ov028_020ecff4[14];
        fmt[1] = data_ov028_020ecff4[15];
        fmt[2] = data_ov028_020ecff4[16];
        fmt[3] = data_ov028_020ecff4[17];
        fmt[4] = data_ov028_020ecff4[18];
        SysFont_Format(arg2, fmt, 1, 0, 0);
    } else if (arg3 >= 10) {
        u16 fmt[5];
        fmt[0] = data_ov028_020ecff4[24];
        fmt[1] = data_ov028_020ecff4[25];
        fmt[2] = data_ov028_020ecff4[26];
        fmt[3] = data_ov028_020ecff4[27];
        fmt[4] = data_ov028_020ecff4[28];
        SysFont_Format(arg2, fmt, arg3 / 10, arg3 % 10);
    } else {
        u16 fmt[5];
        fmt[0] = data_ov028_020ecff4[9];
        fmt[1] = data_ov028_020ecff4[10];
        fmt[2] = data_ov028_020ecff4[11];
        fmt[3] = data_ov028_020ecff4[12];
        fmt[4] = data_ov028_020ecff4[13];
        SysFont_Format(arg2, fmt, arg3);
    }
}

// Computes the effective drop rate (in hundredths of a percent, clamped to 0..10000) for a Noise and difficulty slot.
// Suggested name: NrepFont_GetEffectiveDropRate
s32 func_ov028_020e957c(NrepFont* arg0, s32 arg1, s32 arg2) {
    s32            r4;
    NrepEnemyData* p;

    if (func_ov028_020e8240(arg1) != 0) {
        return 0;
    }
    p = func_ov028_020e7360(arg1);
    switch (arg2) {
        case 0x2:
        case 0x3:
            r4 = p->dropRates[3];
            break;
        case 0x5:
        case 0x6:
            r4 = p->dropRates[2];
            break;
        case 0x8:
        case 0x9:
            r4 = p->dropRates[1];
            break;
        case 0xB:
        case 0xC:
            r4 = p->dropRates[0];
            break;
        default:
            r4 = 0;
            break;
    }
    if (data_ov028_020ed3e0->unk_12 == 0) {
        r4 *= gSaveData.playerStats.dropRate;
    }
    if (r4 > 0x2710) {
        r4 = 0x2710;
    } else if (r4 < 0) {
        r4 = 0;
    }
    return r4;
}

// Draws the integer portion of a difficulty slot's drop rate percentage.
// Suggested name: NrepFont_DrawDropRateInt
void func_ov028_020e9650(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5) {
    u16 buf[22];
    s16 idx = data_ov028_020ed3e0->unk_16;
    u8  val;
    s32 r;

    if (idx >= 0x60) {
        return;
    }

    if (data_ov028_020ed3e0->unk_12 == 0) {
        val = arg2->color;
    } else {
        val = 4;
    }

    r = func_ov028_020e957c(arg0, idx, arg5);
    func_ov028_020e9464(arg0, arg1, buf, r / 100);
    SysFont_SetColor(arg1, val);
    SysFont_SetSpacing(arg1, 1, 1);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
}

// Draws the two-digit fractional portion of a difficulty slot's drop rate percentage.
// Suggested name: NrepFont_DrawDropRateFrac
void func_ov028_020e9704(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4, s32 arg5) {
    u16 fmt[5];
    u16 buf[22];
    s16 idx = data_ov028_020ed3e0->unk_16;
    u8  val;
    s32 r;

    fmt[0] = data_ov028_020ecff4[19];
    fmt[1] = data_ov028_020ecff4[20];
    fmt[2] = data_ov028_020ecff4[21];
    fmt[3] = data_ov028_020ecff4[22];
    fmt[4] = data_ov028_020ecff4[23];

    if (idx >= 0x60) {
        return;
    }

    if (data_ov028_020ed3e0->unk_12 == 0) {
        val = arg2->color;
    } else {
        val = 4;
    }

    r = func_ov028_020e957c(arg0, idx, arg5);
    SysFont_Format(buf, fmt, (r / 10) % 10, r % 10);
    SysFont_SetColor(arg1, val);
    SysFont_SetSpacing(arg1, 1, 1);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
}

// Formats and draws the best clear time (MM'SS"CS) for the selected Boss Noise entry if non-zero.
// Suggested name: NrepFont_DrawBestTime
void func_ov028_020e9808(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    u16  buf[21];
    u16  fmt[9];
    s32  i;
    u32  t;
    u32  hours, minutes, seconds, rem;
    u16* src = data_ov028_020ed04c;
    u16* dst = fmt;

    i = 4;
    do {
        dst[0] = src[0];
        dst[1] = src[1];
        dst += 2;
        src += 2;
    } while (--i != 0);
    dst[0] = src[0];

    if (data_ov028_020ed3e0->unk_16 >= 0x60) {
        return;
    }

    t = func_ov028_020e8230(data_ov028_020ed3e0->unk_16)->bestTime;
    if (t == 0) {
        return;
    }

    hours   = t / 3600;
    rem     = t - hours * 3600;
    minutes = rem / 60;
    seconds = (rem - minutes * 60) * 100 / 60;

    // The display is MM'SS"CC; anything over 99 minutes is clamped to a "time up" value.
    if (hours > 0x63) {
        hours   = 0x63;
        minutes = 0x3B;
        seconds = 0x63;
    }

    SysFont_Format(buf, fmt, hours / 10, hours % 10, minutes / 10, minutes % 10, seconds / 10, seconds % 10);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
}

// Draws the current help page's title heading on the lower screen.
// Suggested name: NrepFont_DrawHelpTitle
void func_ov028_020e99a4(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    SysFont_SetSpacing(arg1, 1, 0);
    SysFont_SetMsg(arg1, (u16)(arg2->msgId + data_ov028_020ed3e0->unk_10 - 1));
    SysFont_SetHAlign(arg1, 1, 0xE0);
    SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
}

// Formats and draws the help page indicator ("page/5") right-aligned on the lower screen.
// Suggested name: NrepFont_DrawHelpPageNum
void func_ov028_020e9a1c(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    u16  buf[30];
    u16* fmt;
    SysFont_SetSpacing(arg1, 1, 0);
    fmt = SysFont_GetMsgBuf(arg1, (u16)arg2->msgId);
    SysFont_Format(buf, fmt, data_ov028_020ed3e0->unk_10, 5);
    SysFont_SetHAlign(arg1, 2, 0xDC);
    SysFont_DrawToScreen(arg1, buf, arg3 + 2, arg4 + 2, 0);
    Mem_Free(&gDebugHeap, fmt);
}

// Draws the current help page's body text on the lower screen.
// Suggested name: NrepFont_DrawHelpBody
void func_ov028_020e9ac0(NrepFont* arg0, SysFont* arg1, const NrepFontItemConfig* arg2, u16* arg3, u16* arg4) {
    SysFont_SetSpacing(arg1, 1, 0);
    SysFont_SetMsg(arg1, (u16)(arg2->msgId + data_ov028_020ed3e0->unk_10 - 1));
    SysFont_SetHAlign(arg1, 1, 0xE0);
    SysFont_DrawCurrentToScreen(arg1, arg3 + 2, arg4 + 2, 0);
}

// Checks whether the selected Noise entry has changed since last frame and updates the cached value.
// Suggested name: NrepFont_HasSelectedNoiseChanged
s32 func_ov028_020e9b38(NrepFont* arg0) {
    s16 a = arg0->prevSelectedNoise;
    s16 b = data_ov028_020ed3e0->unk_16;
    if (a != b) {
        arg0->prevSelectedNoise = b;
        return 1;
    }
    return 0;
}

// Checks whether the drop rate mode, help page, or help open state has changed and updates cached values.
// Suggested name: NrepFont_HasViewStateChanged
s32 func_ov028_020e9b64(NrepFont* arg0) {
    s16 b = data_ov028_020ed3e0->unk_12;

    if (arg0->prevDropRateMode == b && arg0->prevHelpPage == data_ov028_020ed3e0->unk_10 &&
        arg0->prevHelpOpen == data_ov028_020ed3e0->unk_0C)
    {
        return 0;
    }
    arg0->prevDropRateMode = b;
    arg0->prevHelpPage     = data_ov028_020ed3e0->unk_10;
    arg0->prevHelpOpen     = data_ov028_020ed3e0->unk_0C;
    return 1;
}

// Reloads one of the two text canvas BG layers (0 = upper screen, 1 = lower screen) and caches its screen/char pointers.
// Suggested name: NrepFont_ReloadCanvasLayer
void func_ov028_020e9bcc(NrepFont* arg0, s32 arg1) {
    NrepBgLayer*        dst = &arg0->bgLayers[arg1];
    const NrepBgConfig* src = &data_ov028_020ed0a8[arg1];

    if (dst->data != NULL) {
        func_ov028_020e7be8(dst);
        arg0->screenBuf[arg1] = NULL;
        arg0->charBuf[arg1]   = NULL;
    }
    dst->engine   = src->engine;
    dst->layer    = src->layer;
    dst->priority = src->priority;
    dst->vOffset  = 0;
    dst->hOffset  = 0;
    dst->visible  = 1;
    func_ov028_020e776c(dst, src);

    arg0->screenBuf[arg1] = Data_GetPackEntryData(dst->data, data_ov028_020ed0a8[arg1].screenEntry);
    arg0->charBuf[arg1]   = Data_GetPackEntryData(dst->data, data_ov028_020ed0a8[arg1].charEntry);
}

// Redraws all text on the lower screen (either the help page text or the 20 Noise detail fields).
// Suggested name: NrepFont_RedrawLowerScreen
void func_ov028_020e9cd8(NrepFont* arg0) {
    s16                       idx = data_ov028_020ed3e0->unk_16;
    SysFont*                  ptr;
    const NrepFontItemConfig* tab;
    s32                       i;

    func_ov028_020e9bcc(arg0, 1);

    if (data_ov028_020ed3e0->unk_0C != 0) {
        func_ov028_020e99a4(arg0, &arg0->lowerFonts[17], &data_ov028_020ed1cc, arg0->screenBuf[1], arg0->charBuf[1]);
        func_ov028_020e9a1c(arg0, &arg0->lowerFonts[18], &data_ov028_020ed1d8, arg0->screenBuf[1], arg0->charBuf[1]);
        func_ov028_020e9ac0(arg0, &arg0->lowerFonts[19], &data_ov028_020ed1e4, arg0->screenBuf[1], arg0->charBuf[1]);
        return;
    }

    if (idx >= 0x60) {
        return;
    }

    ptr = arg0->lowerFonts;
    tab = data_ov028_020ed100;
    for (i = 0; i < 0x14; i++) {
        switch (i) {
            case 0:
                func_ov028_020e8f0c(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                break;
            case 1:
                func_ov028_020e8e18(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                break;
            case 2:
            case 5:
            case 8:
            case 11:
                func_ov028_020e9650(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1], i);
                break;
            case 3:
            case 6:
            case 9:
            case 12:
                func_ov028_020e9704(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1], i);
                break;
            case 4:
            case 7:
            case 10:
            case 13:
                func_ov028_020e91cc(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1], i);
                break;
            case 14:
                if (func_ov028_020e8230(idx)->bestTime != 0) {
                    func_ov028_020e8ddc(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                }
                break;
            case 15:
                func_ov028_020e9808(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                break;
            case 16:
                func_ov028_020e9160(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                break;
            default:
                func_ov028_020e8ddc(arg0, ptr, tab, arg0->screenBuf[1], arg0->charBuf[1]);
                break;
        }
        ptr++;
        tab++;
    }
}

// Redraws all 6 text fields on the upper screen for the currently selected Noise entry.
// Suggested name: NrepFont_RedrawUpperScreen
void func_ov028_020e9f44(NrepFont* arg0) {
    s16                       idx = data_ov028_020ed3e0->unk_16;
    s32                       i;
    SysFont*                  ptr;
    const NrepFontItemConfig* tab;

    func_ov028_020e9bcc(arg0, 0);

    if (idx >= 0x60) {
        return;
    }

    ptr = arg0->upperFonts;
    tab = data_ov028_020ed060;
    for (i = 0; i < 6; i++) {
        switch (i) {
            case 0:
                func_ov028_020e8f0c(arg0, ptr, tab, arg0->screenBuf[0], arg0->charBuf[0]);
                break;
            case 1:
                func_ov028_020e8e18(arg0, ptr, tab, arg0->screenBuf[0], arg0->charBuf[0]);
                break;
            case 3:
                func_ov028_020e8fe0(arg0, ptr, tab, arg0->screenBuf[0], arg0->charBuf[0]);
                break;
            case 5:
                func_ov028_020e90b0(arg0, ptr, tab, arg0->screenBuf[0], arg0->charBuf[0]);
                break;
            default:
                func_ov028_020e8ddc(arg0, ptr, tab, arg0->screenBuf[0], arg0->charBuf[0]);
                break;
        }
        ptr++;
        tab++;
    }
}

// Initializes Tsk_Font: sets up the 6 upper and 20 lower SysFont instances and performs initial text rendering.
// Suggested name: NrepFont_Init
s32 func_ov028_020ea058(NrepFont* arg0, void* arg1) {
    s32                       i;
    SysFont*                  dst;
    const NrepFontItemConfig* src;

    MI_CpuSet(arg0, 0, sizeof(NrepFont));

    dst = arg0->upperFonts;
    src = data_ov028_020ed060;
    for (i = 0; i < 6; i++) {
        SysFont_Init(dst);
        SysFont_SetColor(dst, src->color);
        SysFont_SetPos(dst, src->x, src->y);
        SysFont_SetSpacing(dst, 1, 0);
        dst++;
        src++;
    }

    dst = arg0->lowerFonts;
    src = data_ov028_020ed100;
    for (i = 0; i < 0x14; i++) {
        SysFont_Init(dst);
        SysFont_SetColor(dst, src->color);
        SysFont_SetPos(dst, src->x, src->y);
        SysFont_SetSpacing(dst, 1, 0);
        dst++;
        src++;
    }

    func_ov028_020e9cd8(arg0);
    func_ov028_020e9f44(arg0);
    func_ov028_020e9b38(arg0);
    func_ov028_020e9b64(arg0);
    return 1;
}

// Updates Tsk_Font: redraws upper and/or lower screen text when selection or view state changes.
// Suggested name: NrepFont_Update
s32 func_ov028_020ea15c(NrepFont* arg0) {
    if (func_ov028_020e9b38(arg0)) {
        func_ov028_020e9cd8(arg0);
        func_ov028_020e9f44(arg0);
    } else {
        if (func_ov028_020e9b64(arg0) != 0) {
            func_ov028_020e9cd8(arg0);
        }
    }
    return 1;
}

// Render callback for Tsk_Font (no-op since text is drawn directly into BG screen/char buffers).
// Suggested name: NrepFont_Render
s32 func_ov028_020ea1a0(NrepFont* arg0) {
    return 1;
}

// Cleans up Tsk_Font by destroying all 26 SysFont instances and releasing both text canvas BG layers.
// Suggested name: NrepFont_Destroy
s32 func_ov028_020ea1a8(NrepFont* arg0) {
    s32      i;
    SysFont* ptr = arg0->lowerFonts;
    for (i = 0; i < 0x14; i++) {
        SysFont_Destroy(ptr);
        ptr++;
    }
    ptr = arg0->upperFonts;
    for (i = 0; i < 6; i++) {
        SysFont_Destroy(ptr);
        ptr++;
    }
    NrepBgLayer* layer = arg0->bgLayers;
    for (i = 0; i < 2; i++) {
        func_ov028_020e7be8(layer);
        layer++;
    }
    return 1;
}

// Task dispatcher for Tsk_Font.
// Suggested name: NrepFont_RunTask
s32 func_ov028_020ea21c(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    NrepFont* data = (NrepFont*)task->data;
    switch (arg3) {
        case 0:
            return func_ov028_020ea058(data, arg2);
        case 1:
            return func_ov028_020ea15c(data);
        case 2:
            return func_ov028_020ea1a0(data);
        case 3:
            return func_ov028_020ea1a8(data);
        default:
            return 0;
    }
}

// Spawns the top-right corner icon buttons task (Tsk_NrepIcon).
// Suggested name: NrepIcon_CreateTask
void func_ov028_020ea26c(void) {

    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &data_ov028_020ed204, NULL, 0, NULL, 0);
}

// Hit-tests touch coordinates (arg0, arg1) against the two top-right icon buttons (0 = Help, 1 = Exit, -1 = none).
// Suggested name: NrepIcon_HitTest
s16 func_ov028_020ea2a8(s16 arg0, s16 arg1) {
    s16 boxes[2][2];
    s16 w = data_ov028_020ed1f0.width;
    s16 h = data_ov028_020ed1f0.height;
    s16 i;

    boxes[0][0] = data_ov028_020ed1f0.boxX[0];
    boxes[0][1] = data_ov028_020ed1f0.boxY[0];
    boxes[1][0] = data_ov028_020ed1f0.boxX[1];
    boxes[1][1] = data_ov028_020ed1f0.boxY[1];

    for (i = 0; i < 2; i++) {
        if (func_ov028_020e8158(arg0, arg1, boxes[i][0], boxes[i][1], w, h) != 0) {
            return i;
        }
    }
    return -1;
}

// Frame-info callback for Tsk_NrepIcon sprites: fills g_SpriteFrameInfo for update and render modes.
// Suggested name: NrepIcon_GetSpriteFrameInfo
SpriteFrameInfo* func_ov028_020ea350(void* arg0, s32 arg1, s32 arg2) {
    Sprite* sp = (Sprite*)arg0;
    Sprite_FrameInfoCallback(sp, arg2);
}

// Loads the two top-right icon button sprites (Help and Exit) for Tsk_NrepIcon.
// Suggested name: NrepIcon_LoadSprites
void func_ov028_020ea3ec(NrepIcon* arg0) {
    SpriteAnimation anim = data_ov028_020ed220;

    anim.dataType  = data_ov028_020ed3e0->unk_00 & 0xF;
    anim.animIndex = 3;
    anim.posX      = 0xDA;
    anim.posY      = 0x0C;
    _Sprite_Load(&arg0->sprites[0], &anim);
    anim.animIndex = 5;
    anim.posX      = 0xF3;
    anim.posY      = 0x0C;
    _Sprite_Load(&arg0->sprites[1], &anim);
}

// Initializes Tsk_NrepIcon: clears task data, loads the Help and Exit button sprites, and updates them once.
// Suggested name: NrepIcon_Init
s32 func_ov028_020ea49c(TaskPool* arg0, Task* arg1, void* arg2) {
    NrepIcon* r5 = (NrepIcon*)arg1->data;
    MI_CpuSet(r5, 0, sizeof(NrepIcon));
    r5->pressed[0] = 0;
    r5->pressed[1] = 0;
    func_ov028_020ea3ec(r5);
    Sprite* sp = r5->sprites;
    for (s32 i = 0; i < 2; i++) {
        Sprite_Update(sp);
        sp++;
    }
    return 1;
}

// Updates Tsk_NrepIcon: handles touch presses on the Help and Exit buttons and updates their animations.
// Suggested name: NrepIcon_Update
s32 func_ov028_020ea4ec(TaskPool* arg0, Task* arg1, void* arg2) {
    NrepIcon* r4 = (NrepIcon*)arg1->data;

    if (data_ov028_020ed3e0->unk_2C != 0 && data_ov028_020ed3e0->unk_0C == 0) {
        s16 result = func_ov028_020ea2a8((s16)data_ov028_020ed3e0->touchCoord.x, (s16)data_ov028_020ed3e0->touchCoord.y);
        if (result != -1) {
            if (r4->pressed[result] == 1) {
                r4->pressed[result] = 0;
            } else {
                r4->pressed[result] = 1;
            }
            if (result == 1) {
                data_ov028_020ed520->exitFlag = 1;
                func_ov028_020e81fc(&r4->sprites[result], (r4->pressed[result] == 1) ? 6 : 5);
                SndMgr_StartPlayingSE(0x11B);
                SndMgr_StartPlayingSE(0x117);
            } else {
                data_ov028_020ed520->helpOpen = 1;
                data_ov028_020ed520->helpPage = 1;
                func_ov028_020e81fc(&r4->sprites[result], (r4->pressed[result] == 1) ? 4 : 3);
                r4->pressTimer = 0xC;
                SndMgr_StartPlayingSE(0x11C);
            }
        }
    }

    if (r4->pressed[0] == 1) {
        r4->pressTimer--;
        if (r4->pressTimer < 0) {
            func_ov028_020e81fc(&r4->sprites[0], 3);
            r4->pressed[0] = 0;
        }
    }

    Sprite* sp = r4->sprites;
    for (s32 i = 0; i < 2; i++) {
        Sprite_Update(sp);
        sp++;
    }
    return 1;
}

// Renders both top-right icon button sprites for Tsk_NrepIcon.
// Suggested name: NrepIcon_Render
s32 func_ov028_020ea654(TaskPool* arg0, Task* arg1, void* arg2) {
    NrepIcon* icon = (NrepIcon*)arg1->data;
    Sprite*   r5   = icon->sprites;
    for (s32 i = 0; i < 2; i++) {
        Sprite_RenderFrame(r5);
        r5++;
    }
    return 1;
}

// Releases both top-right icon button sprites for Tsk_NrepIcon.
// Suggested name: NrepIcon_Destroy
s32 func_ov028_020ea680(TaskPool* arg0, Task* arg1, void* arg2) {
    NrepIcon* icon = (NrepIcon*)arg1->data;
    Sprite*   r5   = icon->sprites;
    for (s32 i = 0; i < 2; i++) {
        Sprite_Release(r5);
        r5++;
    }
    return 1;
}

// Task dispatcher for Tsk_NrepIcon.
// Suggested name: NrepIcon_RunTask
s32 func_ov028_020ea6ac(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    NrepTaskFuncs t = data_ov028_020ed210;
    return t.f[arg3](pool, task, arg2);
}

// Spawns the lower-screen button/cursor task (Tsk_NrepInterD).
// Suggested name: NrepInterD_CreateTask
void func_ov028_020ea6f4(void) {

    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &data_ov028_020ed254, NULL, 0, NULL, 0);
}

// Scrolls the entry grid to follow a drag on the scroll knob; the knob sprite's own position doubles as the drag anchor.
// Suggested name: NrepInterD_DragScrollKnob
s32 func_ov028_020ea730(NrepInterD* arg0) {
    if (arg0->draggingScroll == 0) {
        if (func_ov028_020e8158((s16)data_ov028_020ed3e0->touchCoord.x, (s16)data_ov028_020ed3e0->touchCoord.y, 0xF5, 0x31,
                                0x28, 0x5E) == 0)
        {
            return 0;
        }
    }

    if (arg0->draggingScroll == 0) {
        if (func_ov028_020e8158((s16)data_ov028_020ed3e0->touchCoord.x, (s16)data_ov028_020ed3e0->touchCoord.y,
                                arg0->sprites[9].posX - 3, arg0->sprites[9].posY - 0x25, 0x28, 0x25) != 0)
        {
            arg0->scrollDragOffset = (s16)(data_ov028_020ed3e0->touchCoord.y - (arg0->sprites[9].posY - 0x13));
            if (arg0->scrollDragOffset == 0) {
                arg0->scrollDragOffset = 1;
            }
        }
    }

    if (arg0->draggingScroll == 0 && arg0->scrollDragOffset == 0) {
        arg0->scrollKnobY      = (s16)(data_ov028_020ed3e0->touchCoord.y + 0x12);
        arg0->scrollDragOffset = 1;
    }

    func_ov028_020ea960(arg0);
    arg0->draggingScroll = 1;
    return 1;
}

// Converts between the scroll knob pixel position and the row index, then shifts the selection one page left.
// Suggested name: NrepInterD_SyncScrollKnob
void func_ov028_020ea888(NrepInterD* arg0, s32 arg1) {
    s16 val4a = arg0->scrollRow;

    if (arg1 != 0) {
        arg0->scrollRow = (s16)(((arg0->scrollKnobY - 0x56) * 100 / 0x39 * 9) / 100);
    } else {
        arg0->scrollKnobY = (s16)(val4a * 100 / 9 * 0x39 / 100 + 0x56);
    }

    arg0->cursorCol = (s16)(arg0->cursorCol + ((data_ov028_020ed3e0->unk_14 - arg0->scrollRow) << 3));
    if (arg0->scrollRow != val4a) {
        SndMgr_StartPlayingSE(0x119);
    }
}

// Eases the scroll knob toward the finger, clamps it, then resyncs the row index.
// Suggested name: NrepInterD_UpdateScrollKnob
void func_ov028_020ea960(NrepInterD* arg0) {
    s32 t = arg0->scrollDragOffset - 0x13;

    if (arg0->scrollDragOffset != 0) {
        s32 delta = data_ov028_020ed3e0->touchCoord.y - (arg0->scrollKnobY + t);

        if (delta > 1 || delta < -1) {
            if (delta > 3) {
                delta = 3;
            } else if (delta < -3) {
                delta = -3;
            }
            arg0->scrollKnobY = (s16)(arg0->scrollKnobY + delta);
        }
    }

    {
        s16 v = arg0->scrollKnobY;
        if (v > 0x8F) {
            v = 0x8F;
        } else if (v < 0x56) {
            v = 0x56;
        }
        arg0->scrollKnobY = v;
    }

    func_ov028_020ea888(arg0, 1);
}

// On the shoulder-button rows, steps the row index and shifts the selection one page left.
// Suggested name: NrepInterD_StepScrollRow
void func_ov028_020eaa00(NrepInterD* arg0, s32 arg1) {
    s16 val4a = arg0->scrollRow;

    if (arg1 == 7) {
        arg0->scrollRow = (s16)(val4a - 1);
        if (arg0->scrollRow < 0) {
            arg0->scrollRow = 0;
        }
        func_ov028_020ea888(arg0, 0);
    } else if (arg1 == 8) {
        arg0->scrollRow = (s16)(val4a + 1);
        if (arg0->scrollRow > 9) {
            arg0->scrollRow = 9;
        }
        func_ov028_020ea888(arg0, 0);
    }

    if (arg0->scrollRow != val4a) {
        SndMgr_StartPlayingSE(0x119);
    }
}

// On the drop-rate toggle buttons, flips the raw/adjusted drop-rate display mode.
// Suggested name: NrepInterD_ToggleDropRateMode
void func_ov028_020eaa90(NrepInterD* arg0, s32 arg1) {
    s16 old = data_ov028_020ed3e0->unk_12;

    if (arg1 == 5) {
        data_ov028_020ed520->dropRateMode = 0;
    } else if (arg1 == 6) {
        data_ov028_020ed520->dropRateMode = 1;
    }
    if (old != data_ov028_020ed520->dropRateMode) {
        SndMgr_StartPlayingSE(0x11A);
    }
}

// On the help page up/down buttons, changes the page and animates the pressed button.
// Suggested name: NrepInterD_ChangeHelpPage
void func_ov028_020eab04(NrepInterD* arg0, s32 arg1) {
    s16 old = data_ov028_020ed520->helpPage;

    if (arg1 == 0xA) {
        data_ov028_020ed520->helpPage = (s16)(old - 1);
        if (data_ov028_020ed520->helpPage < 1) {
            data_ov028_020ed520->helpPage = 1;
        }
        func_ov028_020e81fc(&arg0->sprites[arg1], 2);
    } else if (arg1 == 0xB) {
        data_ov028_020ed520->helpPage = (s16)(old + 1);
        if (data_ov028_020ed520->helpPage >= 5) {
            data_ov028_020ed520->helpPage = 5;
        }
        func_ov028_020e81fc(&arg0->sprites[arg1], 4);
    }
    if (old != data_ov028_020ed520->helpPage) {
        SndMgr_StartPlayingSE(0x11A);
    }
}

// On touch press, selects the entry under the finger in the 3x8 visible grid.
// Suggested name: NrepInterD_SelectEntryAtTouch
s16 func_ov028_020eaba4(NrepInterD* arg0) {
    s32 boxX = 0x26;
    s32 s10  = 0;
    s32 row;
    s32 col;

    for (row = 0; row < 3; row++) {
        for (col = 0; col < 8; col++) {
            if (func_ov028_020e8158((s16)data_ov028_020ed3e0->touchCoord.x, (s16)data_ov028_020ed3e0->touchCoord.y, (s16)boxX,
                                    (s16)(5 + col * 0x1E), 0x1B, 0x1B) != 0)
            {
                s16 idx = (s16)(arg0->scrollRow * 8 + col + s10);
                func_ov028_020e8230(idx);
                if (func_ov028_020e8240(idx) == 0) {
                    s16 old         = arg0->cursorCol;
                    arg0->cursorCol = (s16)(col + s10);
                    if (old != arg0->cursorCol) {
                        SndMgr_StartPlayingSE(0x11A);
                        arg0->selectedNoise = (s16)(arg0->cursorCol + (arg0->scrollRow << 3));
                    }
                    return arg0->cursorCol;
                }
            }
        }
        s10 += 8;
        boxX += 0x2C;
    }
    return -1;
}

// Per-frame input handler for Tsk_NrepInterD: resolves the hit button and moves the cursor/row.
// Suggested name: NrepInterD_UpdateInput
void func_ov028_020eacf8(NrepInterD* arg0) {
    s16 oldX = arg0->cursorCol;
    s16 oldY = arg0->scrollRow;
    u16 pad  = data_ov028_020ed3e0->unk_2A;
    u16 keys;

    if ((pad & 0xC03) == 0 && (pad & 0xF0) == 0) {
        arg0->keyRepeatTimer = 0;
    }

    keys = data_ov028_020ed3e0->unk_28;
    if ((keys & 0x200) || (keys & 0x100)) {
        data_ov028_020ed520->dropRateMode = (data_ov028_020ed3e0->unk_12 == 0) ? 1 : 0;
        SndMgr_StartPlayingSE(0x11A);
    } else if (arg0->keyRepeatTimer > 0) {
        arg0->keyRepeatTimer = (s16)(arg0->keyRepeatTimer - 1);
        return;
    } else {
        if ((keys & 0xC03) || (keys & 0xF0)) {
            arg0->keyRepeatTimer = 0x14;
        } else if ((data_ov028_020ed3e0->unk_2A & 0xC03) || (data_ov028_020ed3e0->unk_2A & 0xF0)) {
            arg0->keyRepeatTimer = 1;
        }

        pad = data_ov028_020ed3e0->unk_2A;
        if ((pad & 0x40) || (pad & 0x400)) {
            if (arg0->sprites[4].posY >= 0xC0) {
                arg0->cursorCol = 0;
            } else if (arg0->cursorCol - 8 >= 0) {
                arg0->cursorCol = (s16)(arg0->cursorCol - 8);
            } else {
                func_ov028_020eaa00(arg0, 7);
                if (arg0->cursorCol - 8 >= 0) {
                    arg0->cursorCol = (s16)(arg0->cursorCol - 8);
                }
            }
            arg0->selectedNoise = (s16)(arg0->cursorCol + (arg0->scrollRow << 3));
        } else if ((pad & 0x80) || (pad & 0x2)) {
            if (arg0->sprites[4].posY >= 0xC0) {
                arg0->cursorCol = 0;
            } else if (arg0->cursorCol + 8 < 0x18) {
                arg0->cursorCol = (s16)(arg0->cursorCol + 8);
            } else {
                func_ov028_020eaa00(arg0, 8);
                if (arg0->cursorCol + 8 < 0x18) {
                    arg0->cursorCol = (s16)(arg0->cursorCol + 8);
                }
            }
            arg0->selectedNoise = (s16)(arg0->cursorCol + (arg0->scrollRow << 3));
        } else if ((pad & 0x20) || (pad & 0x800)) {
            if (arg0->sprites[4].posY >= 0xC0) {
                arg0->cursorCol = 0;
            } else {
                arg0->cursorCol = (s16)(arg0->cursorCol - 1);
                if (arg0->cursorCol < 0) {
                    arg0->scrollRow = (s16)(arg0->scrollRow - 1);
                    if (arg0->scrollRow < 0) {
                        arg0->scrollRow = 9;
                        func_ov028_020ea888(arg0, 0);
                        arg0->cursorCol = 0x17;
                    } else {
                        func_ov028_020ea888(arg0, 0);
                        arg0->cursorCol = 7;
                    }
                }
            }
            arg0->selectedNoise = (s16)(arg0->cursorCol + (arg0->scrollRow << 3));
        } else if ((pad & 0x10) || (pad & 0x1)) {
            if (arg0->sprites[4].posY >= 0xC0) {
                arg0->cursorCol = 0;
            } else {
                arg0->cursorCol = (s16)(arg0->cursorCol + 1);
                if (arg0->cursorCol >= 0x18) {
                    arg0->scrollRow = (s16)(arg0->scrollRow + 1);
                    if (arg0->scrollRow > 9) {
                        arg0->scrollRow = 0;
                        func_ov028_020ea888(arg0, 0);
                        arg0->cursorCol = 0;
                    } else {
                        func_ov028_020ea888(arg0, 0);
                        arg0->cursorCol = 0x10;
                    }
                }
            }
            arg0->selectedNoise = (s16)(arg0->cursorCol + (arg0->scrollRow << 3));
        }
    }

    if (oldX != arg0->cursorCol || oldY != arg0->scrollRow) {
        SndMgr_StartPlayingSE(0x11A);
    }
}

// Keeps the cursor sprite pinned to the selected entry inside the 3-row visible window.
// Suggested name: NrepInterD_UpdateCursorPos
void func_ov028_020eb054(NrepInterD* arg0) {
    s16 val16 = data_ov028_020ed3e0->unk_16;
    s16 val14 = data_ov028_020ed3e0->unk_14;
    s32 a     = val16 % 8;
    s32 b     = (val16 / 8) - val14;

    if (b < 0 || b > 2) {
        b = 5;
    }

    arg0->sprites[4].posX = (s16)(a * 0x1E + 5) + 0xE;
    arg0->sprites[4].posY = (s16)(b * 0x2C + 0x26) + 0xE;
}

// Returns the index of the button hitbox under the touch point, or -1 if none.
// Suggested name: NrepInterD_HitTestButtons
s16 func_ov028_020eb0d4(NrepInterD* arg0) {
    const NrepInterDHitbox* table = data_ov028_020ed260;
    s32                     i;

    for (i = 0; i < 0xD; i++) {
        s16 w = (s16)(table[i].posX + table[i].hitOffsetX);
        s16 h = (s16)(table[i].posY + table[i].hitOffsetY);
        if (func_ov028_020e8158((s16)data_ov028_020ed3e0->touchCoord.x, (s16)data_ov028_020ed3e0->touchCoord.y, w, h,
                                table[i].hitWidth, table[i].hitHeight) != 0)
        {
            return i;
        }
    }
    return -1;
}

// Initializes Tsk_NrepInterD: loads the button sprites, restores the cursor/row from the save, and spawns Tsk_NrepSeal.
// Suggested name: NrepInterD_Init
s32 func_ov028_020eb174(NrepInterD* param_1, void* param_2) {
    NrepInterD*             base = param_1;
    SpriteAnimation         anim;
    Sprite*                 sp;
    const NrepInterDHitbox* tbl;
    s32                     i;

    func_ov028_020e7fc8(&anim, 0, data_ov028_020ed3e0->unk_00, &data_ov028_020ed24c, 1, 2, 3, 0xA);
    anim.unk_22 = 2;
    MI_CpuSet(base, 0, sizeof(NrepInterD));

    tbl = data_ov028_020ed260;
    sp  = base->sprites;
    for (i = 0; i < 0xD; i++) {
        if (i != 0xA && i != 0xB && i != 0xC) {
            _Sprite_Load(sp, &anim);
            func_ov028_020e81fc(sp, tbl->animIndex);
            sp->posX       = tbl->posX;
            sp->posY       = tbl->posY;
            sp->unk_0A.raw = (sp->unk_0A.raw & ~0xC00) | 0x400;
        }
        sp++;
        tbl++;
    }

    base->sprites[0].unk_0A.raw &= ~0xC00;
    base->sprites[4].unk_0A.raw &= ~0xC00;

    func_ov028_020e7fc8(&anim, 0, data_ov028_020ed3e0->unk_00, &data_ov028_020ed24c, 7, 8, 9, 0xA);
    anim.unk_22 = 2;

    tbl = data_ov028_020ed260;
    sp  = base->sprites;
    for (i = 0; i < 0xD; i++) {
        if (i == 0xA || i == 0xB || i == 0xC) {
            _Sprite_Load(sp, &anim);
            func_ov028_020e81fc(sp, tbl->animIndex);
            sp->posX = tbl->posX;
            sp->posY = tbl->posY;
        }
        sp++;
        tbl++;
    }

    base->selectedNoise            = gSaveData.unk_243A;
    base->scrollRow                = gSaveData.unk_243C;
    data_ov028_020ed520->scrollRow = base->scrollRow;
    func_ov028_020ec4f8(&data_ov028_020ed24c);

    base->selectedNoise = (s16)(base->selectedNoise + (base->scrollRow << 3));
    func_ov028_020eb054(base);

    base->scrollKnobY     = (s16)(((s32)base->scrollRow * 100 / 9) * 57 / 200 + 0x56);
    base->sprites[9].posY = base->scrollKnobY;

    sp = base->sprites;
    for (i = 0; i < 0xD; i++) {
        Sprite_Update(sp);
        sp++;
    }

    base->touchActive = 0;
    return 1;
}

// Per-frame update for Tsk_NrepInterD: routes touch/button input and keeps the 13 sprites current.
// Suggested name: NrepInterD_Update
s32 func_ov028_020eb3e8(NrepInterD* param_1) {
    NrepInterD* base = param_1;
    s32         active;

    base->sprites[9].posY = base->scrollKnobY;

    if (data_ov028_020ed3e0->unk_0C == 0) {
        if (TouchInput_WasTouchReleased()) {
            base->scrollDragOffset = 0;
            base->draggingScroll   = 0;
        }
        if (data_ov028_020ed3e0->unk_2C != 0 && base->draggingScroll == 0) {
            s32 which = func_ov028_020eb0d4(base);
            switch (which) {
                case 5:
                case 6:
                    func_ov028_020eaa90(base, which);
                    break;
                case 7:
                case 8:
                    func_ov028_020eaa00(base, which);
                    break;
                default:
                    func_ov028_020eaba4(base);
                    break;
            }
        } else if (TouchInput_IsTouchActive()) {
            func_ov028_020ea730(base);
        } else {
            func_ov028_020eacf8(base);
        }
        func_ov028_020eb054(base);
    } else {
        if (base->helpBtnTimer > 0) {
            base->helpBtnTimer = (s16)(base->helpBtnTimer - 1);
            if (base->helpBtnTimer <= 0) {
                base->helpBtnTimer = 0;
                func_ov028_020e81fc(&base->sprites[10], 1);
                func_ov028_020e81fc(&base->sprites[11], 3);
                func_ov028_020e81fc(&base->sprites[12], 5);
                if (base->closingHelp != 0) {
                    base->closingHelp             = 0;
                    data_ov028_020ed520->helpOpen = 0;
                }
            }
        } else if (data_ov028_020ed3e0->unk_2C != 0) {
            s32 which = func_ov028_020eb0d4(base);
            if (which == 0xA || which == 0xB) {
                func_ov028_020eab04(base, which);
                base->helpBtnTimer = 0xC;
            } else if (which == 0xC) {
                base->closingHelp  = 1;
                base->helpBtnTimer = 0xC;
                func_ov028_020e81fc(&base->sprites[which], 6);
                SndMgr_StartPlayingSE(0x11B);
                Inventory_SetHelpSeen(2);
            }
        }
    }

    if (TouchInput_IsTouchActive()) {
        TouchCoord coord;
        TouchInput_GetCoord(&coord);
        *(s16*)(base + 0xC) = coord.x;
        *(s16*)(base + 0xE) = coord.y;
        active              = 1;
    } else {
        active = 0;
    }

    base->touchActive = active;
    {
        Sprite* sp = base->sprites;
        s32     i;
        for (i = 0; i < 0xD; i++) {
            Sprite_Update(sp);
            sp++;
        }
    }

    data_ov028_020ed520->scrollRow     = base->scrollRow;
    data_ov028_020ed520->selectedNoise = base->selectedNoise;
    return 1;
}

// Renders the visible Tsk_NrepInterD sprites, hiding the ones the current mode/row excludes.
// Suggested name: NrepInterD_Render
s32 func_ov028_020eb654(NrepInterD* param_1) {
    NrepInterD* base = param_1;
    s32         r5;
    Sprite*     r6;

    if (base->touchActive == 1) {
        Sprite_RenderFrame(&base->sprites[0]);
    }

    r6 = &base->sprites[1];
    for (r5 = 1; r5 < 0xD; r5++) {
        s32 state = data_ov028_020ed3e0->unk_12;

        if ((state == 0 && r5 == 3) || (state == 1 && r5 == 2)) {
            goto skip;
        }
        if (data_ov028_020ed3e0->unk_0C == 0) {
            if (r5 == 0xA || r5 == 0xB || r5 == 0xC) {
                goto skip;
            }
        } else {
            if (r5 == 2 || r5 == 3 || r5 == 4) {
                goto skip;
            }
            if (r5 == 0xA) {
                if (data_ov028_020ed3e0->unk_10 <= 1) {
                    goto skip;
                }
            }
            if (r5 == 0xB) {
                if (data_ov028_020ed3e0->unk_10 >= 5) {
                    goto skip;
                }
            }
        }
        Sprite_RenderFrame(r6);
    skip:
        r6++;
    }
    return 1;
}

// Writes the cursor/row back to the save and releases all Tsk_NrepInterD sprites.
// Suggested name: NrepInterD_Destroy
s32 func_ov028_020eb718(NrepInterD* param_1) {
    NrepInterD* base = param_1;

    gSaveData.unk_243A = base->cursorCol;
    gSaveData.unk_243C = base->scrollRow;

    Sprite* r5 = base->sprites;
    for (s32 i = 0; i < 0xD; i++) {
        Sprite_Release(r5);
        r5++;
    }
    return 1;
}

// Task dispatcher for Tsk_NrepInterD.
// Suggested name: NrepInterD_RunTask
s32 func_ov028_020eb760(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    switch (arg3) {
        case 0:
            return func_ov028_020eb174(task->data, arg2);
        case 1:
            return func_ov028_020eb3e8(task->data);
        case 2:
            return func_ov028_020eb654(task->data);
        case 3:
            return func_ov028_020eb718(task->data);
        default:
            return 0;
    }
}

// Spawns the upper-screen sprite task (Tsk_NrepInterU).
// Suggested name: NrepInterU_CreateTask
void func_ov028_020eb7b0(void) {
    static const TaskHandle Tsk_NrepInterU = {"Tsk_NrepInterU", func_ov028_020eb9dc, sizeof(NrepInterU)};

    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &Tsk_NrepInterU, NULL, 0, NULL, 0);
}

// Initializes Tsk_NrepInterU: loads the 15 upper-screen sprites from the layout table and spawns Tsk_NrepPreview.
// Suggested name: NrepInterU_Init
s32 func_ov028_020eb7ec(NrepInterU* param_1, void* param_2) {
    NrepInterU*            base = param_1;
    SpriteAnimation        anim;
    const NrepInterUEntry* tbl = data_ov028_020ed32c;
    Sprite*                sp;
    s32                    i;

    func_ov028_020e7fc8(&anim, 0, data_ov028_020ed3e0->unk_00, &data_ov028_020ed318, 1, 2, 3, 4);
    MI_CpuSet(base, 0, sizeof(NrepInterU));

    sp = base->sprites;
    for (i = 0; i < 0xF; i++) {
        _Sprite_Load(sp, &anim);
        func_ov028_020e81fc(sp, (s16)tbl->animIndex);
        sp->posX       = tbl->posX;
        sp->posY       = tbl->posY;
        sp->unk_0A.raw = (sp->unk_0A.raw & ~0xC00) | 0x800;
        sp++;
        tbl++;
    }

    base->sprites[13].unk_0A.raw = (base->sprites[13].unk_0A.raw & ~0xC00) | 0x400;
    base->sprites[14].unk_0A.raw = (base->sprites[14].unk_0A.raw & ~0xC00) | 0x400;
    func_ov028_020eba2c(&data_ov028_020ed318);
    return 1;
}

// Updates all 15 Tsk_NrepInterU sprites.
// Suggested name: NrepInterU_Update
s32 func_ov028_020eb8ec(NrepInterU* arg0) {
    Sprite* sp = arg0->sprites;
    for (s32 i = 0; i < 15; i++) {
        Sprite_Update(sp);
        sp++;
    }
    return 1;
}

// Renders the 15 Tsk_NrepInterU sprites, hiding the ones the current mode/page excludes.
// Suggested name: NrepInterU_Render
s32 func_ov028_020eb918(NrepInterU* param_1) {
    NrepInterU* base = param_1;
    Sprite*     sp   = base->sprites;

    for (s32 i = 0; i < 0xF; i++) {
        if ((u32)(i - 0xD) <= 1) {
            if (data_ov028_020ed3e0->unk_0C == 0) {
                goto skip;
            }
            if (i == 0xD && data_ov028_020ed3e0->unk_10 <= 1) {
                goto skip;
            }
            if (i == 0xE && data_ov028_020ed3e0->unk_10 >= 5) {
                goto skip;
            }
        }
        if (i == 0xC && func_ov028_020e8230(data_ov028_020ed3e0->unk_16)->bestTime != 0) {
            goto skip;
        }
        Sprite_RenderFrame(sp);
    skip:
        sp++;
    }
    return 1;
}

// Releases all 15 Tsk_NrepInterU sprites.
// Suggested name: NrepInterU_Destroy
s32 func_ov028_020eb9b0(NrepInterU* arg0) {
    Sprite* sp = arg0->sprites;
    for (s32 i = 0; i < 15; i++) {
        Sprite_Release(sp);
        sp++;
    }
    return 1;
}

// Task dispatcher for Tsk_NrepInterU.
// Suggested name: NrepInterU_RunTask
s32 func_ov028_020eb9dc(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    switch (arg3) {
        case 0:
            return func_ov028_020eb7ec(task->data, arg2);
        case 1:
            return func_ov028_020eb8ec(task->data);
        case 2:
            return func_ov028_020eb918(task->data);
        case 3:
            return func_ov028_020eb9b0(task->data);
        default:
            return 0;
    }
}

// Spawns the enemy preview sprite task (Tsk_NrepPreview).
// Suggested name: NrepPreview_CreateTask
void func_ov028_020eba2c(const BinIdentifier* arg0) {
    s32                     stack[1];
    static const TaskHandle Tsk_NrepPreview = {"Tsk_NrepPreview", func_ov028_020ec4a8, sizeof(NrepPreview)};

    MI_CpuSet(&stack, 0, 4);
    stack[0] = (s32)arg0;
    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &Tsk_NrepPreview, NULL, 0, NULL, &stack);
}

// Lays out the Tsk_NrepPreview sprite rows: the stat readouts, the drop-rate digits, the body icons, and the boss-time banner.
// Suggested name: NrepPreview_LoadSprites
void func_ov028_020eba84(NrepPreview* arg0, Sprite* arg1, s32* arg2) {
    Sprite*         base = arg1;
    SpriteAnimation anim;
    Sprite*         s;
    s32             i;

    func_ov028_020e7fc8(&anim, 0, data_ov028_020ed3e0->unk_00, (const BinIdentifier*)arg2[0], 1, 2, 3, 4);

    // Five drop-rate digits, right-aligned.
    s = &base[4];
    for (i = 4; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[i].posX = (s16)((4 - i) * 5 + 0x15);
        base[i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // Four ATK digits.
    s = &base[8];
    for (i = 3; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[4 + i].posX = (s16)((3 - i) * 5 + 0x46);
        base[4 + i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // Three HP digits.
    s = &base[11];
    for (i = 2; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[8 + i].posX = (s16)((2 - i) * 5 + 0x6C);
        base[8 + i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // Three positive-resistance digits.
    s = &base[15];
    for (i = 2; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[12 + i].posX = (s16)((2 - i) * 5 + 0x93);
        base[12 + i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // Negative-resistance header, then its three digits.
    _Sprite_Load(&base[16], &anim);
    base[16].posX = 0x7B;
    base[16].posY = 0x15;
    func_ov028_020e81fc(&base[16], 0x17);
    base[16].unk_0A.raw = (base[16].unk_0A.raw & ~0xC00) | 0x800;

    s = &base[19];
    for (i = 2; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[16 + i].posX = (s16)((2 - i) * 5 + 0xBA);
        base[16 + i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // Yen header, then its five digits.
    _Sprite_Load(&base[20], &anim);
    base[20].posX = 0xA2;
    base[20].posY = 0x15;
    func_ov028_020e81fc(&base[20], 0x17);
    base[20].unk_0A.raw = (base[20].unk_0A.raw & ~0xC00) | 0x800;

    s = &base[24];
    for (i = 4; i >= 0; i--) {
        _Sprite_Load(s, &anim);
        base[20 + i].posX = (s16)((4 - i) * 5 + 0xE1);
        base[20 + i].posY = 0x15;
        func_ov028_020e81fc(s, 0xD);
        s->unk_0A.raw = (s->unk_0A.raw & ~0xC00) | 0x800;
        s--;
    }

    // The four body/limb icons, laid out vertically alongside the banner row.
    _Sprite_Load(&base[28], &anim);
    base[28].posX = 0x3F;
    base[28].posY = 0x3C;
    func_ov028_020e81fc(&base[28], 0x18);
    base[28].unk_0A.raw = (base[28].unk_0A.raw & ~0xC00) | 0x800;

    _Sprite_Load(&base[27], &anim);
    base[27].posX = 0x3F;
    base[27].posY = 0x56;
    func_ov028_020e81fc(&base[27], 0x18);
    base[27].unk_0A.raw = (base[27].unk_0A.raw & ~0xC00) | 0x800;

    _Sprite_Load(&base[26], &anim);
    base[26].posX = 0x3F;
    base[26].posY = 0x70;
    func_ov028_020e81fc(&base[26], 0x18);
    base[26].unk_0A.raw = (base[26].unk_0A.raw & ~0xC00) | 0x800;

    _Sprite_Load(&base[25], &anim);
    base[25].posX = 0x3F;
    base[25].posY = 0x8A;
    func_ov028_020e81fc(&base[25], 0x18);
    base[25].unk_0A.raw = (base[25].unk_0A.raw & ~0xC00) | 0x800;
}

// Per-frame update for Tsk_NrepPreview: fills the stat/drop digit sprites and the body-part icon sprites.
// Suggested name: NrepPreview_Update
void func_ov028_020ebe9c(NrepPreview* arg0) {
    Sprite* puVar5 = &arg0->sprites[0];
    s32     iVar6  = data_ov028_020ed3e0->unk_16;
    s32     iVar4  = 29;

    if ((iVar6 >= 0x60) || (arg0->unk_000 == 0)) {
        func_ov028_020e80a0(&arg0->sprites[0], 13, 0, 5, 0);
        func_ov028_020e80a0(&arg0->sprites[5], 13, 0, 4, 0);
        func_ov028_020e80a0(&arg0->sprites[9], 13, 0, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[12], 13, 0, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[16], 13, 0, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[20], 13, 0, 5, 0);
        func_ov028_020e81fc(&arg0->sprites[25], 0);
        func_ov028_020e81fc(&arg0->sprites[26], 0);
        func_ov028_020e81fc(&arg0->sprites[27], 0);
        func_ov028_020e81fc(&arg0->sprites[28], 0);
    } else {
        NrepNoiseRecord* iVar1  = func_ov028_020e8230(iVar6);
        NrepEnemyData*   iVar6e = func_ov028_020e7360(iVar6);

        s32 iVar2 = 200 - iVar6e->resistPos;
        s32 iVar7 = 200 - iVar6e->resistNeg;

        if (iVar2 > 200) {
            iVar2 = 200;
        } else if (iVar2 < 0) {
            iVar2 = 0;
        }

        if (iVar7 > 200) {
            iVar7 = 200;
        } else if (iVar7 < 0) {
            iVar7 = 0;
        }

        func_ov028_020e80a0(&arg0->sprites[0], 13, (int)iVar6e->hp, 5, 0);
        func_ov028_020e80a0(&arg0->sprites[5], 13, (int)iVar6e->atk, 4, 0);
        func_ov028_020e80a0(&arg0->sprites[9], 13, iVar2, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[12], 13, (s16)iVar7, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[16], 13, (u32)iVar6e->taboo, 3, 0);
        func_ov028_020e80a0(&arg0->sprites[20], 13, (u32)iVar6e->yen, 5, 0);

        s16 sVar3 = ((iVar1->dropFlags & 8) != 0) ? 0x18 : 0;
        func_ov028_020e81fc(&arg0->sprites[25], sVar3);

        sVar3 = ((iVar1->dropFlags & 4) != 0) ? 0x18 : 0;
        func_ov028_020e81fc(&arg0->sprites[26], sVar3);

        sVar3 = ((iVar1->dropFlags & 2) != 0) ? 0x18 : 0;
        func_ov028_020e81fc(&arg0->sprites[27], sVar3);

        sVar3 = ((iVar1->dropFlags & 1) != 0) ? 0x18 : 0;
        func_ov028_020e81fc(&arg0->sprites[28], sVar3);
    }

    do {
        Sprite_Update(puVar5);
        iVar4--;
        puVar5++;
    } while (0 < iVar4);
}

// Renders all 29 Tsk_NrepPreview sprites.
// Suggested name: NrepPreview_Render
void func_ov028_020ec16c(NrepPreview* arg0) {
    int     iVar1  = 29;
    Sprite* puVar2 = &arg0->sprites[0];
    do {
        Sprite_RenderFrame(puVar2);
        iVar1--;
        puVar2++;
    } while (0 < iVar1);
}

// Releases all 29 Tsk_NrepPreview sprites.
// Suggested name: NrepPreview_Destroy
void func_ov028_020ec194(NrepPreview* arg0) {
    int     iVar1  = 29;
    Sprite* puVar2 = &arg0->sprites[0];
    do {
        Sprite_Release(puVar2);
        iVar1--;
        puVar2++;
    } while (0 < iVar1);
}

// Checks whether the current entry is one of the three entries whose preview art differs from the default.
// Suggested name: NrepPreview_HasAlternateArt
s32 func_ov028_020ec1bc(void) {
    s32 iVar1 = 0;
    while (TRUE) {
        if (data_ov028_020ed3a4[iVar1] == 96) {
            return 0;
        }
        if (data_ov028_020ed3a4[iVar1] == data_ov028_020ed3e0->unk_16) {
            return 1;
        }

        iVar1++;
    }
    return 1;
}

// Reloads the two banner sprites (entry number, and the alternate-art marker) for the current entry.
// Suggested name: NrepPreview_LoadBanners
void func_ov028_020ec204(NrepPreview* arg0) {
    SpriteAnimation anim;
    short           sStack_1e;

    if (arg0->unk_7C4 == data_ov028_020ed3e0->unk_16) {
        return;
    }

    arg0->unk_7C4 = data_ov028_020ed3e0->unk_16;
    if (arg0->unk_004.resourceData != NULL) {
        Sprite_Release(&arg0->unk_004);
    }
    if (arg0->unk_044.resourceData != NULL) {
        Sprite_Release(&arg0->unk_044);
    }
    func_ov028_020e7fc8(&anim, 1, data_ov028_020ed3e0->unk_00, &data_ov028_020ed3ac, 1, 2, 3, 4);
    sStack_1e = data_ov028_020ed3e0->unk_16 + 1;
    if (sStack_1e < 0x61) {
        _Sprite_Load(&arg0->unk_004, &anim);
        func_ov028_020e81fc(&arg0->unk_004, 1);
        arg0->unk_004.posX       = 190;
        arg0->unk_004.posY       = 88;
        arg0->unk_004.unk_0A.raw = arg0->unk_004.unk_0A.raw & ~0xC00 | 0x800;
        if (func_ov028_020ec1bc()) {
            _Sprite_Load(&arg0->unk_044, &anim);
            func_ov028_020e81fc(&arg0->unk_044, 2);
            arg0->unk_044.posX       = 190;
            arg0->unk_044.posY       = 88;
            arg0->unk_044.unk_0A.raw = arg0->unk_044.unk_0A.raw & ~0xC00 | 0x800;
        }
    }
}

// Initializes Tsk_NrepPreview: clears the state, lays out the sprite rows, and loads the banners.
// Suggested name: NrepPreview_Init
s32 func_ov028_020ec358(NrepPreview* arg0, s32* arg1) {
    MI_CpuSet(arg0, 0, sizeof(NrepPreview));
    arg0->unk_7C4 = -1;
    func_ov028_020eba84(arg0, &arg0->sprites[0], arg1);
    func_ov028_020ec204(arg0);
    return 1;
}

// Per-frame update for Tsk_NrepPreview: refreshes the unlocked flag, banners, and stat sprites.
// Suggested name: NrepPreview_Update
s32 func_ov028_020ec3a0(NrepPreview* arg0) {
    s16 uVar1 = data_ov028_020ed3e0->unk_16;
    func_ov028_020e8230(uVar1);
    arg0->unk_000 = func_ov028_020e8240(uVar1) == 0;
    func_ov028_020ec204(arg0);
    func_ov028_020ebe9c(arg0);
    if (arg0->unk_000 != 0) {
        if (arg0->unk_004.resourceData != NULL) {
            Sprite_Update(&arg0->unk_004);
        }
        if (arg0->unk_044.resourceData != NULL) {
            Sprite_Update(&arg0->unk_044);
        }
    }
    return 1;
}

// Renders Tsk_NrepPreview: the 29 stat sprites plus the banners when the entry is unlocked.
// Suggested name: NrepPreview_Render
s32 func_ov028_020ec424(NrepPreview* arg0) {
    func_ov028_020ec16c(arg0);
    if (arg0->unk_000 != 0) {
        if (arg0->unk_004.resourceData != NULL) {
            Sprite_RenderFrame(&arg0->unk_004);
        }
        if (arg0->unk_044.resourceData != NULL) {
            Sprite_RenderFrame(&arg0->unk_044);
        }
    }
    return 1;
}

// Releases Tsk_NrepPreview: the 29 stat sprites plus the banners.
// Suggested name: NrepPreview_Destroy
s32 func_ov028_020ec46c(NrepPreview* arg0) {
    func_ov028_020ec194(arg0);
    if (arg0->unk_004.resourceData != NULL) {
        Sprite_Release(&arg0->unk_004);
    }
    if (arg0->unk_044.resourceData != NULL) {
        Sprite_Release(&arg0->unk_044);
    }
    return 1;
}

// Task dispatcher for Tsk_NrepPreview.
// Suggested name: NrepPreview_RunTask
s32 func_ov028_020ec4a8(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    NrepPreview* puVar1 = (NrepPreview*)task->data;
    switch (arg3) {
        case 0:
            return func_ov028_020ec358(puVar1, arg2);
        case 1:
            return func_ov028_020ec3a0(puVar1);
        case 2:
            return func_ov028_020ec424(puVar1);
        case 3:
            return func_ov028_020ec46c(puVar1);
        default:
            return 0;
    }
}

// Spawns the Noise seal task (Tsk_NrepSeal).
// Suggested name: NrepSeal_CreateTask
void func_ov028_020ec4f8(const BinIdentifier* arg0) {
    s32                     stack[1];

    MI_CpuSet(&stack, 0, 4);
    stack[0] = (s32)arg0;
    EasyTask_CreateTask(data_ov028_020ed3e0->pool, &Tsk_NrepSeal, NULL, 0, NULL, &stack);
}

// Writes out the grid X/Y for entry slot arg2 (column = arg2 % 8, row = arg2 / 8).
// Suggested name: NrepSeal_GetSlotPos
void func_ov028_020ec550(s16* arg0, s16* arg1, s32 arg2) {
    *arg0 = (s16)((arg2 % 8) * 0x1E + 5);
    *arg1 = (s16)((arg2 / 8) * 0x2C + 0x26);
}

// Rebuilds the 24 Noise seal tiles whenever the visible row changes; releases the old sprites and reloads them from the seal
// pack. Suggested name: NrepSeal_RebuildTiles
void func_ov028_020ec590(NrepSealTaskData* arg0) {
    NrepSeal* seals = arg0->seals;
    s32       i;

    if (arg0->loadedRow == data_ov028_020ed3e0->unk_14) {
        return;
    }

    for (i = 0; i < 0x18; i++) {
        seals->unk_000 = 0;
        if (seals->unk_004.resourceData != NULL) {
            Sprite_Release(&seals->unk_004);
        }
        seals++;
    }

    arg0->loadedRow = data_ov028_020ed3e0->unk_14;

    {
        SpriteAnimation anim;
        s32             j;

        func_ov028_020e7fc8(&anim, 0, data_ov028_020ed3e0->unk_00, &data_ov028_020ed3c0, 1, 2, 3, 4);
        anim.animIndex = 2;

        seals = arg0->seals;
        for (j = 0; j < 0x18; j++) {
            seals->unk_000 = 1;
            if (seals->unk_004.resourceData == NULL) {
                s16 idx;

                anim.bits_7_9  = (j < 0xF) ? 5 : 6;
                idx            = (s16)(data_ov028_020ed3e0->unk_14 * 8 + j) + 1;
                anim.packIndex = idx;
                if (idx >= 0x61) {
                    seals->unk_000 = 0;
                } else {
                    _Sprite_Load(&seals->unk_004, &anim);
                    func_ov028_020e8194(&seals->unk_004, 1, 3, 2);
                    {
                        s16 sx = 0;
                        s16 sy = 0;
                        func_ov028_020ec550(&sx, &sy, j);
                        seals->unk_004.posX = sx + 0xE;
                        seals->unk_004.posY = sy + 0xE;
                    }
                    seals->unk_004.unk_0A.raw = (seals->unk_004.unk_0A.raw & ~0xC) | 4;
                }
            }
            seals++;
        }
    }
}

// Initializes Tsk_NrepSeal: loads one 9-sprite column per visible entry and arms it.
// Suggested name: NrepSeal_Init
s32 func_ov028_020ec73c(NrepSealTaskData* arg0, u32* arg1) {
    SpriteAnimation anim;
    NrepSeal*       seals = arg0->seals;
    s16             sx;
    s16             rowY;
    s16             sy;
    s32             i;

    func_ov028_020e7fc8(&anim, 2, data_ov028_020ed3e0->unk_00, (const BinIdentifier*)*arg1, 4, 5, 6, 0xA);
    MI_CpuSet(arg0, 0, sizeof(NrepSealTaskData));
    arg0->loadedRow = -1;
    func_ov028_020ec590(arg0);

    for (i = 0; i < 0x18; i++) {
        func_ov028_020ec550(&sx, &sy, i);
        // Live copy of sy: mwcc keeps it in a callee-saved register so the row Y
        // survives across the eight Sprite_Load calls in this loop.
        rowY = sy;

        _Sprite_Load(&seals->unk_044, &anim);
        seals->unk_044.posX = sx + 0x18;
        seals->unk_044.posY = rowY + 0x20;
        func_ov028_020e8194(&seals->unk_044, 0xC, 6, 5);
        seals->unk_044.unk_0A.raw = (seals->unk_044.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_084, &anim);
        seals->unk_084.posX = sx + 0x11;
        seals->unk_084.posY = rowY + 0x20;
        func_ov028_020e8194(&seals->unk_084, 0xC, 6, 5);
        seals->unk_084.unk_0A.raw = (seals->unk_084.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_0C4, &anim);
        seals->unk_0C4.posX = sx + 0xA;
        seals->unk_0C4.posY = rowY + 0x20;
        func_ov028_020e8194(&seals->unk_0C4, 0xC, 6, 5);
        seals->unk_0C4.unk_0A.raw = (seals->unk_0C4.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_104, &anim);
        seals->unk_104.posX = sx + 0x3;
        seals->unk_104.posY = rowY + 0x20;
        func_ov028_020e8194(&seals->unk_104, 0xC, 6, 5);
        seals->unk_104.unk_0A.raw = (seals->unk_104.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_144, &anim);
        seals->unk_144.posX = sx;
        seals->unk_144.posY = rowY + 0x28;
        func_ov028_020e8194(&seals->unk_144, 1, 6, 5);
        seals->unk_144.unk_0A.raw = (seals->unk_144.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_184, &anim);
        seals->unk_184.posX = sx + 0x19;
        seals->unk_184.posY = rowY + 0x28;
        func_ov028_020e8194(&seals->unk_184, 2, 6, 5);
        seals->unk_184.unk_0A.raw = (seals->unk_184.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_1C4, &anim);
        seals->unk_1C4.posX = sx + 0x14;
        seals->unk_1C4.posY = rowY + 0x28;
        func_ov028_020e8194(&seals->unk_1C4, 2, 6, 5);
        seals->unk_1C4.unk_0A.raw = (seals->unk_1C4.unk_0A.raw & ~0xC) | 4;

        _Sprite_Load(&seals->unk_204, &anim);
        seals->unk_204.posX = sx + 0xF;
        seals->unk_204.posY = rowY + 0x28;
        func_ov028_020e8194(&seals->unk_204, 3, 6, 5);
        seals->unk_204.unk_0A.raw = (seals->unk_204.unk_0A.raw & ~0xC) | 4;

        seals++;
    }

    return 1;
}

// Per-frame update for Tsk_NrepSeal: refreshes the seal tiles and animates the earned-drop icons plus the kill count.
// Suggested name: NrepSeal_Update
s32 func_ov028_020eca54(NrepSealTaskData* arg0) {
    NrepSeal* seal = arg0->seals;

    func_ov028_020ec590(arg0);

    for (s32 i = 0; i < 24; i++) {
        s16              iVar3 = (data_ov028_020ed3e0->unk_14 * 8 + (s16)i);
        NrepNoiseRecord* rec   = func_ov028_020e8230(iVar3);

        if (func_ov028_020e8240(iVar3) != 0) {
            seal->unk_000 = 0;
        }

        func_ov028_020e81fc(&seal->unk_044, ((rec->dropFlags & 8) != 0) ? 0xC : 0);
        func_ov028_020e81fc(&seal->unk_084, ((rec->dropFlags & 4) != 0) ? 0xC : 0);
        func_ov028_020e81fc(&seal->unk_0C4, ((rec->dropFlags & 2) != 0) ? 0xC : 0);
        func_ov028_020e81fc(&seal->unk_104, ((rec->dropFlags & 1) != 0) ? 0xC : 0);

        if (rec->killCount == 0) {
            func_ov028_020e81fc(&seal->unk_144, 0);
            func_ov028_020e81fc(&seal->unk_184, 0);
            func_ov028_020e81fc(&seal->unk_1C4, 0);
            func_ov028_020e81fc(&seal->unk_204, 0);
        } else {
            func_ov028_020e81fc(&seal->unk_144, 1);
            func_ov028_020e80a0(&seal->unk_184, 2, (u32)(u8)rec->killCount, 3, 0);
        }

        Sprite_Update(&seal->unk_004);
        Sprite_Update(&seal->unk_044);
        Sprite_Update(&seal->unk_084);
        Sprite_Update(&seal->unk_0C4);
        Sprite_Update(&seal->unk_104);
        Sprite_Update(&seal->unk_144);
        Sprite_Update(&seal->unk_184);
        Sprite_Update(&seal->unk_1C4);
        Sprite_Update(&seal->unk_204);
        seal++;
    }

    return 1;
}

// Renders Tsk_NrepSeal: only the seals whose slot is currently armed.
// Suggested name: NrepSeal_Render
s32 func_ov028_020ecbf0(NrepSealTaskData* arg0) {
    NrepSeal* seal = arg0->seals;
    for (s32 i = 0; i < 24; i++) {
        if (seal->unk_000 != 0) {
            data_0206a890.unk_0C = (data_ov028_020ed3e0->unk_0C != 0) ? 0xF : 0x1F;
            data_0206a890.unk_0C &= 0x1F;
            Sprite_RenderFrame(&seal->unk_004);
            Sprite_RenderFrame(&seal->unk_044);
            Sprite_RenderFrame(&seal->unk_084);
            Sprite_RenderFrame(&seal->unk_0C4);
            Sprite_RenderFrame(&seal->unk_104);
            Sprite_RenderFrame(&seal->unk_144);
            Sprite_RenderFrame(&seal->unk_184);
            Sprite_RenderFrame(&seal->unk_1C4);
            Sprite_RenderFrame(&seal->unk_204);
        }
        seal++;
    }
    return 1;
}

// Releases all 9 sprites of each of the 24 Tsk_NrepSeal slots.
// Suggested name: NrepSeal_Destroy
s32 func_ov028_020ecc9c(NrepSealTaskData* arg0) {
    NrepSeal* seal = arg0->seals;
    for (s32 i = 0; i < 24; i++) {
        Sprite_Release(&seal->unk_004);
        Sprite_Release(&seal->unk_044);
        Sprite_Release(&seal->unk_084);
        Sprite_Release(&seal->unk_0C4);
        Sprite_Release(&seal->unk_104);
        Sprite_Release(&seal->unk_144);
        Sprite_Release(&seal->unk_184);
        Sprite_Release(&seal->unk_1C4);
        Sprite_Release(&seal->unk_204);
        seal++;
    }
    return 1;
}

// Task dispatcher for Tsk_NrepSeal.
// Suggested name: NrepSeal_RunTask
s32 func_ov028_020ecd08(TaskPool* pool, Task* task, void* arg2, s32 arg3) {
    NrepSealTaskData* seal = (NrepSealTaskData*)task->data;

    switch (arg3) {
        case 0:
            return func_ov028_020ec73c(seal, arg2);
        case 1:
            return func_ov028_020eca54(seal);
        case 2:
            return func_ov028_020ecbf0(seal);
        case 3:
            return func_ov028_020ecc9c(seal);
        default:
            return 0;
    }
}
