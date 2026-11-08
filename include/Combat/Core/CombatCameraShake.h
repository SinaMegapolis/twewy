#ifndef TWEWY_COMBAT_CORE_COMBATCAMERASHAKE_H
#define TWEWY_COMBAT_CORE_COMBATCAMERASHAKE_H

#include "Combat/Core/Combat.h"

/// Binds camera `index` to the animation driven by `obj`.
void CombatCameraShake_Bind(Ov003CameraAnim* obj, s32 index, s32 arg2);
void CombatCameraShake_Start(Ov003CameraAnim* obj, s32 period, s32 offset);
/// Steps the camera shake animation; returns 1 when it hit a limit this frame.
s32  CombatCameraShake_Update(Ov003CameraAnim* obj, s32 delta, s32 min, s32 max);
void CombatCameraShake_Reset(Ov003CameraAnim* obj);

#endif // TWEWY_COMBAT_CORE_COMBATCAMERASHAKE_H
