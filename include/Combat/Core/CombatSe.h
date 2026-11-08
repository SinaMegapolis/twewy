#ifndef TWEWY_COMBAT_CORE_COMBATSE_H
#define TWEWY_COMBAT_CORE_COMBATSE_H

#include "SndMgrSeIdx.h"

void CombatSe_Play(SndMgrSeIdx);
void CombatSe_PlayPanned(SndMgrSeIdx seIdx, s32 sePan);
void CombatSe_PlayAtMainX(SndMgrSeIdx, s32);
void CombatSe_PlayAtSubX(SndMgrSeIdx, s32);

#endif // TWEWY_COMBAT_CORE_COMBATSE_H
