#include "Battle/Core/BtlStage.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/Combat.h"
#include "Engine/EasyTask.h"
#include "FixedPoint.h"
#include "Player/Inventory.h"
#include "Save.h"
#include <nitro/fx.h>

extern const TaskHandle Tsk_BtlSandstorm;
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

void BtlStage_InitMain(void) {
    const BtlFieldRect* rect  = BtlField_UG_Defs[g_CombatGlobal->stages[0].fieldId].rect;
    BattleStage*        stage = &g_CombatGlobal->stages[0];
    s16                 rightHeight;
    s16                 topHeight;
    s16                 margin;

    margin      = (512 - rect->width) / 2;
    rightHeight = rect->floorTopRight;
    topHeight   = rect->floorTopLeft;
    if (topHeight <= rightHeight) {
        topHeight = rightHeight;
    }
    BtlStage_SetFieldRect(DISPLAY_MAIN, margin, 512 - topHeight, 512 - margin, 488);

    stage->floorSlope    = FX_Atan2Idx(I2F(rect->floorTopLeft - rect->floorTopRight), stage->width);
    stage->floorTop      = I2F(512 - rect->floorTopLeft) - stage->top;
    stage->targetOffsetZ = -I2F(40);
    stage->viewTop       = I2F(512 - rect->viewTop) - stage->top;
    stage->panOffsetY    = stage->viewTop;
}

void BtlStage_InitSub(void) {
    const BtlFieldRectRG* rect   = BtlField_RG_Defs[g_CombatGlobal->stages[1].fieldId].rect;
    BattleStage*          stage  = &g_CombatGlobal->stages[1];
    s16                   bottom = 512 - rect->floorBottom;

    BtlStage_SetFieldRect(DISPLAY_SUB, 112, (s16)(bottom - rect->floorHeight), 400, bottom);
    stage->targetOffsetZ = I2F(rect->targetOffsetZ);
}

/// The brand leading the trend in area `area`.
static u8 BtlStage_GetLeadingBrand(s32 area) {
    BrandTrend* trend = &gSaveData.brandTrends[area];

    return trend->ranking[0];
}

/// Picks which background script the battle's field runs; the first fields show the brand leading their area's trend.
static void BtlStage_PickBgScript(void) {
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

void BtlStage_SetFieldRect(DisplayEngine engine, s32 x0, s32 y0, s32 x1, s32 y1) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];
    s32          width = x1 - x0;

    stage->panOffsetY = 0;
    stage->shakeX     = 0;
    stage->shakeY     = 0;
    stage->shakeZ     = 0;
    stage->left       = I2F(x0);
    stage->top        = I2F(y0);
    stage->width      = I2F(width);
    stage->height     = I2F(y1 - y0);
    stage->centerX    = stage->width / 2;
    stage->centerY    = stage->height / 2;
}

void BtlStage_SetCameraTarget(DisplayEngine engine, s32 x, s32 y, s32 z) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    stage->targetX = x + stage->left;
    stage->targetY = y + stage->top;
    stage->targetZ = z + stage->targetOffsetZ;
}

BOOL BtlStage_IsOutsideFloor(s32 x, s32 y, s32 marginX, s32 marginY) {
    BattleStage* stage = &g_CombatGlobal->stages[0];
    s32          limit = stage->floorTop + FX_Mul(FX_SinIdx(stage->floorSlope), x);

    if (y - marginY < limit || y + marginY > stage->height || x - marginX < 0 || x + marginX > stage->width) {
        return TRUE;
    }
    return FALSE;
}

BOOL BtlStage_ClampToFloorTop(s32* value, s32 margin, s32 x) {
    BattleStage* stage = &g_CombatGlobal->stages[0];
    BOOL         moved = FALSE;
    s32          limit = stage->floorTop + FX_Mul(FX_SinIdx(stage->floorSlope), x);

    if (*value - margin < limit) {
        *value = limit + margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampYToFloor(s32* value, s32 margin, s32 x) {
    BattleStage* stage = &g_CombatGlobal->stages[0];
    BOOL         moved = FALSE;

    if (BtlStage_ClampToFloorTop(value, margin, x)) {
        moved = TRUE;
    }
    if (*value + margin > stage->height) {
        *value = stage->height - margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampXToField(s32* value, s32 margin) {
    BattleStage* stage = &g_CombatGlobal->stages[0];
    BOOL         moved = FALSE;

    if (*value - margin < 0) {
        *value = margin;
        moved  = TRUE;
    } else if (*value + margin > stage->width) {
        *value = stage->width - margin;
        moved  = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampToFloor(s32* x, s32* y, s32 marginX, s32 marginY) {
    BOOL moved = FALSE;

    if (BtlStage_ClampXToField(x, marginX)) {
        moved = TRUE;
    }
    if (BtlStage_ClampYToFloor(y, marginY, *x)) {
        moved = TRUE;
    }
    return moved;
}

BOOL BtlStage_ClampYToField(s32* value, s32 margin) {
    BattleStage* stage = &g_CombatGlobal->stages[0];
    BOOL         moved = FALSE;

    if (*value - margin < 0) {
        *value = margin;
        moved  = TRUE;
    } else if (*value + margin > stage->height) {
        *value = stage->height - margin;
        moved  = TRUE;
    }
    return moved;
}

void BtlStage_Init(void) {
    u16 id = gSaveData.unk_3138;

    switch (g_CombatGlobal->unk3D88E) {
        case 9:
            id = 26;
            break;
        case 14:
            id = 24;
            break;
        case 15:
            id = 40;
            break;
        case 26:
            id = 16;
            break;
        case 29:
            id = 6;
            break;
        case 28:
            id = 14;
            break;
        case 31:
            id = 10;
            break;
        case 20:
            id = 42;
            break;
        case 21:
            id = 42;
            break;
    }
    if (id >= 41) {
        id = 0;
    }
    g_CombatGlobal->stages[0].fieldId = id;
    g_CombatGlobal->stages[0].flags   = 0;
    g_CombatGlobal->stages[1].fieldId = id;
    g_CombatGlobal->stages[1].flags   = 0;
    BtlStage_InitMain();
    BtlStage_InitSub();
}

void BtlStage_StartFieldTasks(s32 flag) {
    if (flag == 0) {
        BtlStage_PickBgScript();
    }

    u16 args = flag;
    switch (g_CombatGlobal->unk3D88E) {
        case 7:
        case 8:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov018_020feb70, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov018_020fec54, NULL, 0, NULL, &args);
            break;
        case 10:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141700, NULL, 0, NULL, &args);
            break;
        case 11:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141da8, NULL, 0, NULL, &args);
            break;
        case 12:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_021416f4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141da8, NULL, 0, NULL, &args);
            break;
        case 13:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141ed4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov017_02141ef8, NULL, 0, NULL, &args);
            break;
        case 19:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov019_020fa764, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov019_020fa780, NULL, 0, NULL, &args);
            break;
        case 20:
        case 21:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov020_020f6fc0, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov020_020f701c, NULL, 0, NULL, &args);
            break;
        case 22:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f7824, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f7880, NULL, 0, NULL, &args);
            break;
        case 27:
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f78b4, NULL, 0, NULL, &args);
            EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov021_020f798c, NULL, 0, NULL, &args);
            break;
        default:
            if (g_CombatGlobal->unk3D88D != 3) {
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_UG, NULL, 0, NULL, &args);
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov008_020e7a68, NULL, 0, NULL, &args);
            } else {
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_UG, NULL, 0, NULL, &args);
                EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &Tsk_BtlField_RG, NULL, 0, NULL, &args);
            }
            break;
    }
}

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

void BtlStage_WorldToScreen(DisplayEngine engine, s16* screenX, s16* screenY, s32 x, s32 y, s32 z) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    *screenX = F2I(x - stage->viewX + I2F(128) + stage->left);
    *screenY = F2I(y + z - stage->viewY + I2F(96) + stage->top);
}

s16 BtlStage_ToScreenX(DisplayEngine engine, s32 x) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    return F2I(x - stage->viewX + I2F(128) + stage->left);
}

s16 BtlStage_ToScreenY(DisplayEngine engine, s32 y, s32 z) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    return F2I(y + z - stage->viewY + I2F(96) + stage->top);
}

s32 BtlStage_ToWorldX(DisplayEngine engine, s32 screenX) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    return stage->viewX + I2F(screenX) - I2F(128) - stage->left;
}

s32 BtlStage_ToWorldY(DisplayEngine engine, s32 screenY, s32 height) {
    BattleStage* stage = &g_CombatGlobal->stages[engine];

    return stage->viewY + I2F(screenY) - I2F(96) - stage->top - height;
}
