#include "Combat/Core/CombatStage.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "Player/Inventory.h"
#include "Save.h"
#include <nitro/fx.h>

extern const TaskHandle Tsk_BtlSandstorm;

// Boss background task handles of other overlays.
extern const TaskHandle data_ov018_020feb70;
extern const TaskHandle data_ov018_020fec54;
extern const TaskHandle data_ov017_021416f4;
extern const TaskHandle data_ov017_02141700;
extern const TaskHandle data_ov017_02141da8;
extern const TaskHandle data_ov017_02141ed4;
extern const TaskHandle data_ov017_02141ef8;
extern const TaskHandle data_ov019_020fa764;
extern const TaskHandle data_ov019_020fa780;
extern const TaskHandle data_ov020_020f6fc0;
extern const TaskHandle data_ov020_020f701c;
extern const TaskHandle data_ov021_020f7824;
extern const TaskHandle data_ov021_020f7880;
extern const TaskHandle data_ov021_020f78b4;
extern const TaskHandle data_ov021_020f798c;
extern const TaskHandle data_ov008_020e7a68;

/// Places the main engine's camera on its path entry and aims it at the battle centre.
// Nonmatching: the target computes the half-width as `(0x200 - x) << 15 >> 16` (a fixed-point
// scale) where mwcc folds our `/ 2` into a single `asr #1`.
void CombatStage_PlaceMainCamera(void) {
    const BtlFieldRect* pos = BtlField_UG_Defs[g_CombatGlobal->unk3D7C0[0].pathId].rect;
    Ov003Camera*        cam = &g_CombatGlobal->unk3D7C0[0];
    s16                 bottom;
    s16                 top;
    s16                 halfX;

    halfX  = (0x200 - pos->unk_00) / 2;
    bottom = pos->unk_04;
    top    = pos->unk_02;
    if (top <= bottom) {
        top = bottom;
    }
    CombatStage_SetCamera(0, halfX, 0x200 - top, 0x200 - halfX, 0x1E8);

    cam->rotation = FX_Atan2Idx((pos->unk_02 - pos->unk_04) << 12, cam->unk_0C);
    cam->posX     = ((0x200 - pos->unk_02) << 12) - cam->unk_08;
    cam->shakeY   = -0x28000;
    cam->posY     = ((0x200 - pos->unk_06) << 12) - cam->unk_08;
    cam->shakeX   = cam->posY;
}

/// Places the sub engine's camera on its path entry.
void CombatStage_PlaceSubCamera(void) {
    const BtlFieldRectRG* pos  = BtlField_RG_Defs[g_CombatGlobal->unk3D7C0[1].pathId].rect;
    Ov003Camera*          cam  = &g_CombatGlobal->unk3D7C0[1];
    s16                   left = 0x200 - pos->unk_00;

    CombatStage_SetCamera(1, 0x70, (s16)(left - pos->unk_02), 0x190, left);
    cam->shakeY = pos->unk_04 << 12;
}

/// The brand trend byte of save slot `index`.
static u8 CombatStage_GetLeadingBrand(s32 index) {
    BrandTrend* trend = &gSaveData.brandTrends[index];

    return trend->ranking[0];
}

/// Picks the battle index (the chapter's battle table row) from the mode and the save.
static void CombatStage_UpdateAreaBrand(void) {
    gSaveData.unk_3232 = 0;
    switch (gSaveData.unk_3138) {
        case 0:
        case 1:
            gSaveData.unk_3232 = CombatStage_GetLeadingBrand(0);
            break;
        case 2:
            if (gSaveData.chapter < 3 && Inventory_GetOwnedCount(0x2AE) == 0) {
                gSaveData.unk_3232 = 0xD;
                break;
            }
            gSaveData.unk_3232 = CombatStage_GetLeadingBrand(1);
            break;
        case 3:
            gSaveData.unk_3232 = CombatStage_GetLeadingBrand(0);
            break;
        case 4:
            if (func_0202599c(0) != 0) {
                gSaveData.unk_3232 = 1;
            }
            break;
    }
}

/// Places the camera of `index` over the field (`x0`,`y0`)-(`x1`,`y1`).
void CombatStage_SetCamera(s32 index, s32 x0, s32 y0, s32 x1, s32 y1) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];
    s32          width  = x1 - x0;

    camera->shakeX = 0;
    camera->unk_44 = 0;
    camera->unk_48 = 0;
    camera->unk_4C = 0;
    camera->unk_04 = x0 << 12;
    camera->unk_08 = y0 << 12;
    camera->unk_0C = width << 12;
    camera->unk_10 = (y1 - y0) << 12;
    camera->unk_20 = camera->unk_0C / 2;
    camera->unk_24 = camera->unk_10 / 2;
}

/// Sets the point the camera of `index` moves towards.
void CombatStage_SetCameraTarget(s32 index, s32 x, s32 y, s32 z) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];

    camera->unk_34 = camera->unk_04 + x;
    camera->unk_38 = camera->unk_08 + y;
    camera->unk_3C = camera->shakeY + z;
}

/// Whether a box around (`x`,`y`) is outside the camera's field.
s32 CombatStage_IsOutOfBounds(s32 x, s32 y, s32 marginX, s32 marginY) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32          limit  = camera->posX + FX_Mul(FX_SinIdx(camera->rotation), x);

    if (y - marginY < limit) {
        return 1;
    }
    if (y + marginY > camera->unk_10) {
        return 1;
    }
    if (x - marginX < 0) {
        return 1;
    }
    if (x + marginX > camera->unk_0C) {
        return 1;
    }
    return 0;
}

s32 CombatStage_ClampToFloorTop(s32* value, s32 margin, s32 scale) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32          moved  = 0;
    s32          limit  = camera->posX + FX_Mul(FX_SinIdx(camera->rotation), scale);

    if (*value - margin < limit) {
        *value = limit + margin;
        moved  = 1;
    }
    return moved;
}

s32 CombatStage_ClampY(s32* value, s32 margin, s32 scale) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32          moved  = 0;

    if (CombatStage_ClampToFloorTop(value, margin, scale) != 0) {
        moved = 1;
    }
    if (*value + margin > camera->unk_10) {
        *value = camera->unk_10 - margin;
        moved  = 1;
    }
    return moved;
}

s32 CombatStage_ClampX(s32* value, s32 margin) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32          moved  = 0;

    if (*value - margin < 0) {
        *value = margin;
        moved  = 1;
    } else if (*value + margin > camera->unk_0C) {
        *value = camera->unk_0C - margin;
        moved  = 1;
    }
    return moved;
}

BOOL CombatStage_ClampXY(s32* x, s32* y, s32 marginX, s32 marginY) {
    BOOL moved = FALSE;

    if (CombatStage_ClampX(x, marginX) != 0) {
        moved = TRUE;
    }
    if (CombatStage_ClampY(y, marginY, *x) != 0) {
        moved = TRUE;
    }
    return moved;
}

s32 CombatStage_ClampYToBox(s32* value, s32 margin) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32          moved  = 0;

    if (*value - margin < 0) {
        *value = margin;
        moved  = 1;
    } else if (*value + margin > camera->unk_10) {
        *value = camera->unk_10 - margin;
        moved  = 1;
    }
    return moved;
}

/// Picks the battle background for the encounter and tells the battle manager about it.
void CombatStage_SetupCameras(void) {
    u16          id        = gSaveData.unk_3138;
    Ov003Global* global    = g_CombatGlobal;
    s32          encounter = global->unk3D88E;

    if (encounter <= 0xF) {
        if (encounter < 9) {
        } else if (encounter == 9) {
            id = 0x1A;
        } else if (encounter == 0xE) {
            id = 0x18;
        } else if (encounter == 0xF) {
            id = 0x28;
        }
    } else {
        switch (encounter - 0x14) {
            case 0:
                id = 0x2A;
                break;
            case 1:
                id = 0x2A;
                break;
            case 6:
                id = 0x10;
                break;
            case 8:
                id = 0xE;
                break;
            case 9:
                id = 6;
                break;
            case 11:
                id = 0xA;
                break;
        }
    }
    if (id >= 0x29) {
        id = 0;
    }
    global->unk3D7C0[0].pathId = id;
    global->unk3D7C0[0].unk_02 = 0;
    global->unk3D7C0[1].pathId = id;
    global->unk3D7C0[1].unk_02 = 0;
    CombatStage_PlaceMainCamera();
    CombatStage_PlaceSubCamera();
}

/// Starts the two background tasks (main and sub screen) of the current battle.
void CombatStage_StartFieldTasks(s32 flag) {
    if (flag == 0) {
        CombatStage_UpdateAreaBrand();
    }

    u16 args = flag;
    switch (g_CombatGlobal->unk3D88E) {
        case 7:
        case 8:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov018_020feb70, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov018_020fec54, NULL, 0, NULL, &args);
            return;
        case 10:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141700, NULL, 0, NULL, &args);
            return;
        case 11:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141da8, NULL, 0, NULL, &args);
            return;
        case 12:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141da8, NULL, 0, NULL, &args);
            return;
        case 13:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141ed4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141ef8, NULL, 0, NULL, &args);
            return;
        case 19:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov019_020fa764, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov019_020fa780, NULL, 0, NULL, &args);
            return;
        case 20:
        case 21:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov020_020f6fc0, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov020_020f701c, NULL, 0, NULL, &args);
            return;
        case 22:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f7824, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f7880, NULL, 0, NULL, &args);
            return;
        case 27:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f78b4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f798c, NULL, 0, NULL, &args);
            return;
        default:
            if (g_CombatGlobal->unk3D88D != 3) {
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_UG, NULL, 0, NULL, &args);
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov008_020e7a68, NULL, 0, NULL, &args);
            } else {
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_UG, NULL, 0, NULL, &args);
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_RG, NULL, 0, NULL, &args);
            }
            return;
    }
}

/// Starts the transition task, except in the last four battles.
void CombatStage_StartSandstorm(void) {
    switch (g_CombatGlobal->unk3D88E) {
        case 19:
        case 20:
        case 21:
        case 22:
            break;
        default:
            EasyTask_CreateTask(&g_CombatGlobal->unk_3C2BC, &Tsk_BtlSandstorm, NULL, 0, NULL, NULL);
            break;
    }
}

/// Projects the world point onto the screen of engine `index`.
void CombatStage_WorldToScreen(s32 index, s16* screenX, s16* screenY, s32 x, s32 y, s32 z) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];

    *screenX = (x - camera->unk_28 + 0x80000 + camera->unk_04) >> 12;
    *screenY = (y + z - camera->unk_40 + 0x60000 + camera->unk_08) >> 12;
}

s16 CombatStage_ToScreenX(s32 index, s32 x) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];

    return (s16)((x - camera->unk_28 + 0x80000 + camera->unk_04) * 0x10 >> 16);
}

s16 CombatStage_ToScreenY(s32 index, s32 y, s32 z) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];

    return (s16)((y + z - camera->unk_40 + 0x60000 + camera->unk_08) * 0x10 >> 16);
}

s32 CombatStage_ToWorldX(s32 index, s32 screenX) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[index];

    return camera->unk_28 + I2F(screenX) - 0x80000 - camera->unk_04;
}

s32 CombatStage_ToWorldY(s32 cameraIndex, s32 screenY, s32 height) {
    Ov003Camera* camera = &g_CombatGlobal->unk3D7C0[cameraIndex];

    return camera->unk_40 + I2F(screenY) - 0x60000 - camera->unk_08 - height;
}
