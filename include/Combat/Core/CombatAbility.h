#ifndef TWEWY_COMBAT_CORE_COMBATABILITY_H
#define TWEWY_COMBAT_CORE_COMBATABILITY_H

#include "Combat/Core/Combat.h"

void CombatAbility_Queue(s32 id, s32 touchIndex, u32 slot);
s32  CombatAbility_IsActiveForPlayer(s32 actionId);
s32  CombatAbility_IsActiveForPartner(s32 actionId);
s32  CombatAbility_GetPlayerValue(s32 actionId);
s32  CombatAbility_GetPartnerValue(s32 actionId);
s32  CombatAbility_IsActive(s32);
s32  CombatAbility_GetValue(s32);

#endif // TWEWY_COMBAT_CORE_COMBATABILITY_H
