#ifndef TWEWY_COMBAT_CORE_COMBATPSYTRAIL_H
#define TWEWY_COMBAT_CORE_COMBATPSYTRAIL_H

#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/CombatPsych.h"

typedef struct Ov003TrailEffect {
    /* 0x000 */ s32          state;
    /* 0x004 */ s32          count;
    /* 0x008 */ s16          points[0x78][2];
    /* 0x1E8 */ BattleSprite spriteA;
    /* 0x248 */ BattleSprite spriteB;
    /* 0x2A8 */ s32          scale; // 20.12 fixed point
    /* 0x2AC */ s16          pivotX;
    /* 0x2AE */ s16          pivotY;
} Ov003TrailEffect; // Size: 0x2B0

typedef struct Ov003PsychTrail {
    /* 0x000 */ Ov003EffectState* effect;
    /* 0x004 */ Ov003TrailEffect  trail;
} Ov003PsychTrail; // Size: 0x2B4

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
