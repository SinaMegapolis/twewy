#include "Display.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/Str.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Field/Event/EventFlags.h"
#include "Field/Scene/FieldSeq.h"
#include "Field/Tasks/Fld_Wall.h"
#include "Interface/Debug/Field/FieldDebug.h"
#include "Save.h"
#include "common_data.h"

extern void func_02025b68(void* textObj, UnkStruct_usedby_02025b68* params);
extern void func_02025e30(void* textObj);

extern s32  Field_GetCurrentTrendZone(void);
extern s32  func_ov030_020b7b00(void);
extern s32  func_ov030_020b7b40(void);
extern s32  func_ov030_020b7b60(void);
extern void func_ov030_020b7c58(s32 area, s32* arg1, s32* arg2, s32* arg3);
extern void Fld_Wall_SetAllExitStates(s32 arg0);
extern s16  FieldNoise_GetTotalQuota(void);
extern void func_ov030_020ae96c(s32 arg0);
extern void ProcessOverlay_Flash(void);

// Per chapter setup routines, indexed by MainData::chapter.
extern void (*const data_ov030_020dbe30[44])(void);

const char* data_ov030_0210033c[48] = {
    [7]  = "no004 ���}�n���Y",
    [1]  = "ce001 �X�N�����u�������_",
    [2]  = "no001 HWV�O",
    [3]  = "ce002 109",
    [37] = "mk014 �}�[�N�V�e�B����",
    [14] = "ea001 �{������",
    [47] = "ce004 �n�`�����Ȃ�",
    [22] = "we004 ���}�{�X�O",
    [19] = "we001 ������",
    [9]  = "no006 �F�c�쒬�H�n��",
    [39] = "ex002 �a�J���O",
    [11] = "no008 ����",
    [12] = "no009 �ۈ�",
    [13] = "no010 �^�����R",
    [5]  = "ce004 �n�`���O",
    [6]  = "no002 ���Ƃ���ّO",
    [15] = "ea002 �K�[�h���i���j",
    [16] = "ea003 �L���b�g�X�g���[�g",
    [17] = "so001 �a�J�w����",
    [18] = "so002 �w��K�[�h��",
    [35] = "mk012 �}�[�N�V�e�B12�K",
    [10] = "no007 ��X�،���",
    [23] = "we005 �I���G�A�C�[�X�g����",
    [24] = "mk001 �}�[�N�V�e�B1�K",
    [25] = "mk002 �}�[�N�V�e�B2�K",
    [26] = "mk003 �}�[�N�V�e�B3�K",
    [8]  = "no005 �p���R",
    [27] = "mk004 �}�[�N�V�e�B4�K",
    [28] = "mk005 �}�[�N�V�e�B5�K",
    [29] = "mk006 �}�[�N�V�e�B6�K",
    [30] = "mk007 �}�[�N�V�e�B7�K",
    [31] = "mk008 �}�[�N�V�e�B8�K",
    [32] = "mk009 �}�[�N�V�e�B9�K",
    [4]  = "ce003 �Z���^�[�X�����",
    [33] = "mk010 �}�[�N�V�e�B10�K",
    [34] = "mk011 �}�[�N�V�e�B11�K",
    [21] = "we003 �I���G�A�C�[�X�g",
    [36] = "mk013 �}�[�N�V�e�B13�K",
    [38] = "ex001 �a�J��ʘHA",
    [0]  = "no003 �X�y�C����",
    [40] = "ex003 �a�J��ʘHB",
    [41] = "ex004 �a�J��㋉�@�̕���",
    [42] = "ex005 �a�J��ʘHC",
    [43] = "ex006 �a�J��R���|�[�U�[�̕���",
    [44] = "tmp",
    [45] = "tmp",
    [46] = "tmp",
    [20] = "we002 �}�[�N�V�e�B",
};

static const char* data_ov030_02100338 = "Seq_FldSel()";

void func_ov030_020b04ac(FieldSelectObject* object) {
    UnkStruct_usedby_02025b68 params;

    params.unk_00 = 1;
    params.unk_04 = 0;
    params.unk_08 = object->unk_11584;
    params.unk_0C = &data_0205c9b0;
    params.unk_10 = 0;
    params.unk_14 = NULL;
    params.unk_18 = 0;
    params.unk_1A = 0;
    params.unk_1C = 0x20;
    params.unk_1E = 0x18;
    func_02025b68(&object->unk_21594, &params);
    Text_RenderToScreen(&object->text, 0, 0, "field select");
}

const char* func_ov030_020b0534(s32 area) {
    return data_ov030_0210033c[area];
}

void func_ov030_020b0544(FieldSelectObject* object) {
    Text_ClearRect(&object->text, 0, 0x10, 0x100, 0x60);
    Text_RenderToScreen(&object->text, 8, 0x10, func_ov030_020b0534(gSaveData.fieldArea));
    Text_RenderToScreen(&object->text, 8, 0x18, Str_SPrintf("width:%d", func_ov030_020b7b00()));

    s32 building = func_ov030_020b7b40();
    if (building < 0) {
        building = 0;
    }
    Text_RenderToScreen(&object->text, 8, 0x20, Str_SPrintf("Building:%d", building));
    Text_RenderToScreen(&object->text, 8, 0x28, Str_SPrintf("Road    :%d", func_ov030_020b7b60()));

    s32 whole = gSaveData.unk_248C >> 12;
    Text_RenderToScreen(&object->text, 8, 0x30,
                        Str_SPrintf("topScale  :%03d.%02d", whole, ((gSaveData.unk_248C * 100) >> 12) - (whole * 100)));

    s32 whole2 = gSaveData.unk_2490 >> 12;
    Text_RenderToScreen(&object->text, 8, 0x38,
                        Str_SPrintf("botomScale:%03d.%02d", whole2, ((gSaveData.unk_2490 * 100) >> 12) - (whole2 * 100)));

    Text_RenderToScreen(&object->text, 8, 0x40, Str_SPrintf("NPC:%3d", gSaveData.areaPopulation[gSaveData.fieldArea]));

    if (gSaveData.unk_26F0 != 0) {
        Text_RenderToScreen(&object->text, 8, 0x48, "DEBUG WINDOW ON ");
    } else {
        Text_RenderToScreen(&object->text, 8, 0x48, "DEBUG WINDOW OFF");
    }

    if (gSaveData.unk_26EC != 0) {
        Text_RenderToScreen(&object->text, 8, 0x50, "MapJump ON ");
    } else {
        Text_RenderToScreen(&object->text, 8, 0x50, "MapJump OFF");
    }

    Text_RenderToScreen(&object->text, 8, 0x58, Str_SPrintf("partner:%d", gSaveData.playerStats.activePartner));
    Text_RenderToScreen(&object->text, 8, 0x60, Str_SPrintf("noize symbol:%d", gSaveData.noiseSymbolCount));
    Text_RenderToScreen(&object->text, 8, 0x68, Str_SPrintf("top brand:%d", object->unk_219B8));
    Text_RenderToScreen(&object->text, 0, (object->unk_219B4 * 8) + 0x10, ">");
}

void func_ov030_020b0820(FieldSelectObject* object) {
    func_02025e30(&object->unk_21594);
}

void func_ov030_020b0834(FieldSelectObject* object) {
    s32 index = Field_GetCurrentTrendZone();
    if (index < 0) {
        object->unk_219B8 = -1;
        return;
    }
    object->unk_219B8 = gSaveData.brandTrends[index].ranking[0];
}

void func_ov030_020b0878(FieldSelectObject* object) {
    s32 index = Field_GetCurrentTrendZone();
    if (index < 0) {
        return;
    }
    gSaveData.brandTrends[index].ranking[0] = object->unk_219B8;
}

void func_ov030_020b08b0(FieldSelectObject* object) {
    func_ov030_020b0834(object);
    if (gSaveData.unk_2484 != 0) {
        gSaveData.noiseSymbolCount = FieldNoise_GetTotalQuota();
    }
    Field_ResetDayProgress();
    func_ov030_020b04ac(object);
}

void func_ov030_020b08f0(FieldSelectObject* object) {
    func_ov030_020b0820(object);
}

// Cycles the active friend with the D-Pad, clamped to the three party members.
void func_ov030_020b08fc(void) {
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_RIGHT) {
        if (gSaveData.playerStats.activePartner < 2) {
            gSaveData.playerStats.activePartner++;
        }
        return;
    }
    if ((SysControl.buttonState.holdButtons & INPUT_BUTTON_LEFT) && gSaveData.playerStats.activePartner != 0) {
        gSaveData.playerStats.activePartner--;
    }
}

// Steps the selected area with the D-Pad and refreshes everything derived from it.
void func_ov030_020b0948(FieldSelectObject* object) {
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_RIGHT) {
        gSaveData.fieldArea++;
    } else if (SysControl.buttonState.holdButtons & INPUT_BUTTON_LEFT) {
        gSaveData.fieldArea--;
    }

    if (gSaveData.fieldArea >= 0x2C) {
        gSaveData.fieldArea = 0x2B;
    }
    if (gSaveData.fieldArea < 0) {
        gSaveData.fieldArea = 0;
    }

    func_ov030_020b7c58(gSaveData.fieldArea, &gSaveData.unk_248C, &gSaveData.unk_2490, &gSaveData.unk_2494);
    func_ov030_020b0834(object);

    if (!(SysControl.buttonState.holdButtons & INPUT_BUTTON_RIGHT) &&
        !(SysControl.buttonState.holdButtons & INPUT_BUTTON_LEFT))
    {
        return;
    }
    gSaveData.noiseSymbolCount = FieldNoise_GetTotalQuota();
}

// Toggles a flag with the D-Pad.
void func_ov030_020b0a14(s32* flag) {
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_RIGHT) {
        *flag = 1;
        return;
    }
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_LEFT) {
        *flag = 0;
    }
}

// Steps a free running counter with the D-Pad.
void func_ov030_020b0a40(s16* value) {
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_RIGHT) {
        *value += 1;
        return;
    }
    if (SysControl.buttonState.holdButtons & INPUT_BUTTON_LEFT) {
        *value -= 1;
    }
}

void func_ov030_020b0a78(FieldSelectObject* object) {
    OverlayTag tag;
    OverlayTag tag2;

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_A) {
        func_ov030_020ae96c(gSaveData.fieldArea);
        gSaveData.unk_24B4 = 0;
        if (gSaveData.unk_26EC != 0) {
            Fld_Wall_SetAllExitStates(4);
        } else {
            Fld_Wall_SetAllExitStates(0);
        }
        func_ov030_020b0878(object);
        MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, PROCESS_STAGE_INIT);
        return;
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_SELECT) {
        MainOvlDisp_Pop(&tag2);
    } else if (SysControl.buttonState.holdButtons == INPUT_BUTTON_UP) {
        object->unk_219B4--;
    } else if (SysControl.buttonState.holdButtons == INPUT_BUTTON_DOWN) {
        object->unk_219B4++;
    }

    if (object->unk_219B4 < 0) {
        object->unk_219B4 = 0xB;
    } else if (object->unk_219B4 >= 0xC) {
        object->unk_219B4 = 0;
    }

    switch (object->unk_219B4) {
        case 0:
            func_ov030_020b0948(object);
            break;
        case 6:
            func_ov030_020b0a40(&gSaveData.areaPopulation[gSaveData.fieldArea]);
            break;
        case 7:
            func_ov030_020b0a14(&gSaveData.unk_26F0);
            break;
        case 8:
            func_ov030_020b0a14(&gSaveData.unk_26EC);
            break;
        case 9:
            func_ov030_020b08fc();
            break;
        case 10:
            func_ov030_020b0a40(&gSaveData.noiseSymbolCount);
            if (gSaveData.noiseSymbolCount < 0) {
                gSaveData.noiseSymbolCount = 0;
            } else if (gSaveData.noiseSymbolCount >= 0x80) {
                gSaveData.noiseSymbolCount = 0x7F;
            }
            break;
        case 11:
            if (object->unk_219B8 >= 0) {
                func_ov030_020b0a40(&object->unk_219B8);
                if (object->unk_219B8 < 0) {
                    object->unk_219B8 = 0;
                } else if (object->unk_219B8 >= 0xD) {
                    object->unk_219B8 = 0xC;
                }
            }
            break;
        default:
            break;
    }

    func_ov030_020ae96c(gSaveData.fieldArea);
    func_ov030_020b0544(object);
}

// Nonmatching: r1/r2 swapped for the g_DisplaySettings / data_02066eec pool loads
void func_ov030_020b0ca4(void) {
    const char*        sequenceName = data_ov030_02100338;
    FieldSelectObject* object       = Mem_AllocHeapTail(&gDebugHeap, 0x219BC);

    Mem_SetSequence(&gDebugHeap, object, sequenceName);
    MainOvlDisp_SetCbArg(object);
    object->unk_11584 = DatMgr_AllocateSlot();
    object->unk_219B4 = 0;
    func_ov030_020b20c0();
    Display_SetMainLayers(LAYER_NONE);
    object->prevResources = ResourceMgr_ReinitManagers(&object->resources);

    g_DisplaySettings.controls[DISPLAY_SUB].brightness = data_02066eec = g_DisplaySettings.controls[DISPLAY_MAIN].brightness =
        data_02066aec                                                  = 0;

    func_ov030_020b08b0(object);
    MainOvlDisp_NextProcessStage();
}

void func_ov030_020b0d58(FieldSelectObject* object) {
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);
    func_ov030_020b0a78(object);
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_FlushCommands(&g_OamMgr[DISPLAY_SUB]);
    PaletteMgr_Flush(g_PaletteManagers[DISPLAY_MAIN], 0);
    PaletteMgr_Flush(g_PaletteManagers[DISPLAY_SUB], 0);
}

void func_ov030_020b0dd8(FieldSelectObject* object) {
    func_ov030_020b08f0(object);
    ResourceMgr_ReinitManagers(NULL);
    DatMgr_ClearSlot(object->unk_11584);
    Mem_Free(&gDebugHeap, object);
}

static const OverlayProcess data_ov030_020dbee0 = {
    .init = func_ov030_020b0ca4,
    .main = func_ov030_020b0d58,
    .exit = func_ov030_020b0dd8,
};

void func_ov030_020b0e0c(void* object) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        func_ov030_020b0dd8(object);
    } else {
        data_ov030_020dbee0.funcs[stage](object);
    }
}

void func_ov030_020b0e4c(void) {
    MainOvlDisp_NextProcessStage();
}

void func_ov030_020b0e58(void) {
    gSaveData.unk_2484 = 1;
    Field_ResetDayProgress();
    gSaveData.unk_2488  = 1;
    gSaveData.unk_30FC  = 0;
    gSaveData.unk_30FE  = -1;
    gSaveData.unk_3100  = -1;
    gSaveData.mailEvent = 0;

    if (gSaveData.chapter <= 6) {
        gSaveData.playerStats.activePartner = PARTNER_SHIKI;
    } else if (gSaveData.chapter <= 13) {
        gSaveData.playerStats.activePartner = PARTNER_JOSHUA;
    } else if (gSaveData.chapter <= 20) {
        gSaveData.playerStats.activePartner = PARTNER_BEAT;
    } else if (gSaveData.chapter <= 28) {
        gSaveData.playerStats.activePartner = PARTNER_SHIKI;
    } else if (gSaveData.chapter <= 35) {
        gSaveData.playerStats.activePartner = PARTNER_JOSHUA;
    } else if (gSaveData.chapter <= 42) {
        gSaveData.playerStats.activePartner = PARTNER_BEAT;
    }

    func_ov030_020aec38(6);
    func_ov030_020aec38(9);
    gSaveData.noiseQuotaStep = 0;
    gSaveData.unk_310C       = NULL;
    gSaveData.unk_30FE       = 39;
    func_ov030_020aead0(2);
    func_ov030_020aead0(4);
    func_ov030_020aec38(10);
    func_ov030_020b7ac8();
    data_ov030_020dbe30[gSaveData.chapter]();

    if (gSaveData.flashSequence == -1) {
        OverlayTag tag;
        MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, func_ov030_020ae92c, NULL, PROCESS_STAGE_INIT);
    } else {
        OverlayTag tag;
        MainOvlDisp_ReplaceTop(&tag, &OVERLAY_30_ID, ProcessOverlay_Flash, NULL, PROCESS_STAGE_INIT);
    }
}

void func_ov030_020b0fe4(void* object) {}

static const OverlayProcess data_ov030_020dbeec = {
    .init = func_ov030_020b0e4c,
    .main = func_ov030_020b0e58,
    .exit = func_ov030_020b0fe4,
};

void func_ov030_020b0fe8(void* object) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        func_ov030_020b0fe4(object);
    } else {
        data_ov030_020dbeec.funcs[stage](object);
    }
}
