#ifndef TWEWY_COMBAT_CORE_COMBATPSYTRAIL_H
#define TWEWY_COMBAT_CORE_COMBATPSYTRAIL_H

#include "Combat/Core/CombatTouch.h"

/// Sets up the two sprite slots and the trail of the psych effect object.
void CombatPsyTrail_Init(Ov003TrailEffect* effect);
/// Updates the psych effect trail; returns 0 when it is finished.
s32 CombatPsyTrail_Update(Ov003TrailEffect* effect);
/// Renders the psych trail.
void CombatPsyTrail_Render(Ov003TrailEffect* effect);
void CombatPsyTrail_Release(Ov003TrailEffect* effect);
/// Checks that the trail is long and straight enough to fire the psych.
s32 CombatPsyTrail_IsValidStroke(Ov003TrailEffect* effect);

#endif // TWEWY_COMBAT_CORE_COMBATPSYTRAIL_H
