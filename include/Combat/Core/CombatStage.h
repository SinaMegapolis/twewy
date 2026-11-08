#ifndef TWEWY_COMBAT_CORE_COMBATSTAGE_H
#define TWEWY_COMBAT_CORE_COMBATSTAGE_H

#include "Combat/Core/Combat.h"

void CombatStage_PlaceMainCamera(void);
void CombatStage_PlaceSubCamera(void);
void CombatStage_SetCamera(s32, s32, s32, s32, s32);
void CombatStage_SetCameraTarget(s32, s32, s32, s32);
s32  CombatStage_IsOutOfBounds(s32 x, s32 y, s32 marginX, s32 marginY);
s32  CombatStage_ClampToFloorTop(s32* value, s32 margin, s32 scale);
s32  CombatStage_ClampY(s32*, s32, s32);
s32  CombatStage_ClampX(s32*, s32);
BOOL CombatStage_ClampXY(s32*, s32*, s32, s32);
s32  CombatStage_ClampYToBox(s32* value, s32 margin);
void CombatStage_SetupCameras(void);
void CombatStage_StartFieldTasks(s32);
void CombatStage_StartSandstorm(void);
void CombatStage_WorldToScreen(s32, s16*, s16*, s32, s32, s32);
s16  CombatStage_ToScreenX(s32, s32);
s16  CombatStage_ToScreenY(s32, s32, s32);
s32  CombatStage_ToWorldX(s32, s32);
s32  CombatStage_ToWorldY(s32, s32, s32);

#endif // TWEWY_COMBAT_CORE_COMBATSTAGE_H
