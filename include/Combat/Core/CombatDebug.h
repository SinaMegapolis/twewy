#ifndef TWEWY_COMBAT_CORE_COMBATDEBUG_H
#define TWEWY_COMBAT_CORE_COMBATDEBUG_H

#include "Combat/Core/Combat.h"

void CombatDebug_InitSettings(void);
void CombatDebug_ValidateSettings(void);
s32  CombatDebug_GetSetting(s32);
void CombatDebug_SetCurrent(s32 value);
s32  CombatDebug_GetCurrent(void);
void CombatDebug_DrawCollision(void);
void CombatDebug_ShowSyncValues(void);

#endif // TWEWY_COMBAT_CORE_COMBATDEBUG_H
