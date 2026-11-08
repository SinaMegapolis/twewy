#ifndef TWEWY_COMBAT_CORE_COMBATCOLLISION_H
#define TWEWY_COMBAT_CORE_COMBATCOLLISION_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatList.h"

void        CombatCollision_Add(Ov003Collider*, s32, s32, s32);
void        CombatCollision_Remove(Ov003Collider*);
void        CombatCollision_SetDisabled(Ov003Collider*, s32);
void        CombatCollision_SetPosition(Ov003Collider*, s32, s32, s32);
void        CombatCollision_ApplyPush(Ov003Collider*, s32*, s32*, s32*);
CombatList* CombatCollision_GetSideList(s32);
void        CombatCollision_CollideLists(CombatList*, CombatList*);
void        CombatCollision_Reset(void);
void        CombatCollision_Update(void);

#endif // TWEWY_COMBAT_CORE_COMBATCOLLISION_H
