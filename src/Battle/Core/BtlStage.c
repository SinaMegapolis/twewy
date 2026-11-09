#include "Battle/Core/BtlStage.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Engine/EasyTask.h"
#include "FixedPoint.h"
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
// Nonmatching: the half-width is computed as `(512 - x) << 15 >> 16`, not a plain `/ 2`.
void BtlStage_PlaceMainCamera(void) {
    const BtlFieldRect* pos = BtlField_UG_Defs[g_CombatGlobal->unk3D7C0[0].pathId].rect;
    BattleCamera*       cam = &g_CombatGlobal->unk3D7C0[0];
    s16                 bottom;
    s16                 top;
    s16                 halfX;

    halfX  = (512 - pos->unk_00) / 2;
    bottom = pos->unk_04;
    top    = pos->unk_02;
    if (top <= bottom) {
        top = bottom;
    }
    BtlStage_SetCamera(0, halfX, 512 - top, 512 - halfX, 488);

    cam->rotation = FX_Atan2Idx(I2F(pos->unk_02 - pos->unk_04), cam->unk_0C);
    cam->posX     = I2F(512 - pos->unk_02) - cam->unk_08;
    cam->shakeY   = -I2F(40);
    cam->posY     = I2F(512 - pos->unk_06) - cam->unk_08;
    cam->shakeX   = cam->posY;
}

/// Places the sub engine's camera on its path entry.
void BtlStage_PlaceSubCamera(void) {
    const BtlFieldRectRG* pos  = BtlField_RG_Defs[g_CombatGlobal->unk3D7C0[1].pathId].rect;
    BattleCamera*         cam  = &g_CombatGlobal->unk3D7C0[1];
    s16                   left = 512 - pos->unk_00;

    BtlStage_SetCamera(1, 112, (s16)(left - pos->unk_02), 400, left);
    cam->shakeY = I2F(pos->unk_04);
}

/// The brand trend byte of save slot `index`.
static u8 BtlStage_GetLeadingBrand(s32 index) {
    BrandTrend* trend = &gSaveData.brandTrends[index];

    return trend->ranking[0];
}

/// Picks the battle index (the chapter's battle table row) from the mode and the save.
static void BtlStage_UpdateAreaBrand(void) {
    gSaveData.unk_3232 = 0;
    switch (gSaveData.unk_3138) {
        case 0:
        case 1:
            gSaveData.unk_3232 = BtlStage_GetLeadingBrand(0);
            break;
        case 2:
            if (gSaveData.chapter < 3 && Inventory_GetOwnedCount(ITEM_STICKER_GAME_CLEARED) == 0) {
                gSaveData.unk_3232 = 13;
                break;
            }
            gSaveData.unk_3232 = BtlStage_GetLeadingBrand(1);
            break;
        case 3:
            gSaveData.unk_3232 = BtlStage_GetLeadingBrand(0);
            break;
        case 4:
            if (func_0202599c(0) != 0) {
                gSaveData.unk_3232 = 1;
            }
            break;
    }
}

/// Places the camera of `index` over the field (`x0`,`y0`)-(`x1`,`y1`).
void BtlStage_SetCamera(s32 index, s32 x0, s32 y0, s32 x1, s32 y1) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];
    s32           width  = x1 - x0;

    camera->shakeX = 0;
    camera->unk_44 = 0;
    camera->unk_48 = 0;
    camera->unk_4C = 0;
    camera->unk_04 = I2F(x0);
    camera->unk_08 = I2F(y0);
    camera->unk_0C = I2F(width);
    camera->unk_10 = I2F(y1 - y0);
    camera->unk_20 = camera->unk_0C / 2;
    camera->unk_24 = camera->unk_10 / 2;
}

/// Sets the point the camera of `index` moves towards.
void BtlStage_SetCameraTarget(s32 index, s32 x, s32 y, s32 z) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];

    camera->unk_34 = x + camera->unk_04;
    camera->unk_38 = y + camera->unk_08;
    camera->unk_3C = z + camera->shakeY;
}

/// Whether a box around (`x`,`y`) is outside the camera's field.
BOOL BtlStage_IsOutOfBounds(s32 x, s32 y, s32 marginX, s32 marginY) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];
    s32           limit  = camera->posX + FX_Mul(FX_SinIdx(camera->rotation), x);

    if (y - marginY < limit) {
        return TRUE;
    }
    if (y + marginY > camera->unk_10) {
        return TRUE;
    }
    if (x - marginX < 0) {
        return TRUE;
    }
    if (x + marginX > camera->unk_0C) {
        return TRUE;
    }
    return FALSE;
}

BOOL BtlStage_ClampToFloorTop(s32* value, s32 margin, s32 x) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];
    BOOL          moved  = FALSE;
    s32           limit  = camera->posX + FX_Mul(FX_SinIdx(camera->rotation), x);

    if (*value - margin < limit) {
        *value = limit + margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampY(s32* value, s32 margin, s32 x) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];
    BOOL          moved  = FALSE;

    if (BtlStage_ClampToFloorTop(value, margin, x)) {
        moved = TRUE;
    }
    if (*value + margin > camera->unk_10) {
        *value = camera->unk_10 - margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampX(s32* value, s32 margin) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];
    BOOL          moved  = FALSE;

    if (*value - margin < 0) {
        *value = margin;
        moved  = TRUE;
    } else if (*value + margin > camera->unk_0C) {
        *value = camera->unk_0C - margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampXY(s32* x, s32* y, s32 marginX, s32 marginY) {
    BOOL moved = FALSE;

    if (BtlStage_ClampX(x, marginX)) {
        moved = TRUE;
    }
    if (BtlStage_ClampY(y, marginY, *x)) {
        moved = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampYToBox(s32* value, s32 margin) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[0];
    BOOL          moved  = FALSE;

    if (*value - margin < 0) {
        *value = margin;
        moved  = TRUE;
    } else if (*value + margin > camera->unk_10) {
        *value = camera->unk_10 - margin;
        moved  = TRUE;
    }
    return moved;
}

/// Picks the battle background for the encounter and tells the battle manager about it.
void BtlStage_SetupCameras(void) {
    u16          id        = gSaveData.unk_3138;
    Ov003Global* global    = g_CombatGlobal;
    s32          encounter = global->unk3D88E;

    if (encounter <= 15) {
        if (encounter < 9) {
        } else if (encounter == 9) {
            id = 26;
        } else if (encounter == 14) {
            id = 24;
        } else if (encounter == 15) {
            id = 40;
        }
    } else {
        switch (encounter - 20) {
            case 0:
                id = 42;
                break;
            case 1:
                id = 42;
                break;
            case 6:
                id = 16;
                break;
            case 8:
                id = 14;
                break;
            case 9:
                id = 6;
                break;
            case 11:
                id = 10;
                break;
        }
    }
    if (id >= 41) {
        id = 0;
    }
    global->unk3D7C0[0].pathId = id;
    global->unk3D7C0[0].unk_02 = 0;
    global->unk3D7C0[1].pathId = id;
    global->unk3D7C0[1].unk_02 = 0;
    BtlStage_PlaceMainCamera();
    BtlStage_PlaceSubCamera();
}

/// Starts the two background tasks (main and sub screen) of the current battle.
void BtlStage_StartFieldTasks(s32 flag) {
    if (flag == 0) {
        BtlStage_UpdateAreaBrand();
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
void BtlStage_StartSandstorm(void) {
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
void BtlStage_WorldToScreen(s32 index, s16* screenX, s16* screenY, s32 x, s32 y, s32 z) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];

    *screenX = F2I(x - camera->unk_28 + I2F(128) + camera->unk_04);
    *screenY = F2I(y + z - camera->unk_40 + I2F(96) + camera->unk_08);
}

s16 BtlStage_ToScreenX(s32 index, s32 x) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];

    return F2Ix(x - camera->unk_28 + I2F(128) + camera->unk_04);
}

s16 BtlStage_ToScreenY(s32 index, s32 y, s32 z) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];

    return F2Ix(y + z - camera->unk_40 + I2F(96) + camera->unk_08);
}

s32 BtlStage_ToWorldX(s32 index, s32 screenX) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[index];

    return camera->unk_28 + I2F(screenX) - I2F(128) - camera->unk_04;
}

s32 BtlStage_ToWorldY(s32 cameraIndex, s32 screenY, s32 height) {
    BattleCamera* camera = &g_CombatGlobal->unk3D7C0[cameraIndex];

    return camera->unk_40 + I2F(screenY) - I2F(96) - camera->unk_08 - height;
}
