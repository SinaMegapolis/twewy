#ifndef TWEWY_COMBAT_CORE_BTLPOP_H
#define TWEWY_COMBAT_CORE_BTLPOP_H

#include "Combat/Core/CombatActor.h"
#include "Engine/EasyTask.h"

/**
 * @brief A popup displaying the current hit combo count.
 * It flashes whenever the combo increases.
 */
extern const TaskHandle Tsk_BtlPop_Combo;

/// Posts a combo increase to the combo popup and records the longest combo of the battle.
void func_ov003_0209c150(s16 combo);

/// Spawns a number popup for `value` above `actor` on `engine`'s screen, unless ten popups of
/// that screen are already alive.
void func_ov003_0209b564(s32 engine, s32 value, CombatActor* actor, s32 element);

/// Spawns a number popup for the damage `value` dealt to `actor`, picking the element and the
/// critical/element icons out of the actor's hit flags.
void func_ov003_0209b61c(s32 engine, s32 value, CombatActor* actor);

/// Spawns a damage-rate popup over `engine`'s level-up gauge actor when the rate changed and is
/// 200% or more.
void func_ov003_0209c568(s32 engine);

/// Spawns a message popup on `engine`'s screen, unless ten popups of that screen are already alive.
void func_ov003_0209c984(s32 engine, s32 animBase, s32 x, s32 y, s32 z);

/// Spawns the status popup on `engine` following `actor`.
s32 func_ov003_0209ccc0(s32 engine, CombatActor* actor);

/// Spawns a star popup showing `value` at (`x`, `y`).
s32 func_ov003_0209d038(s32 value, s16 x, s32 y);

/// Spawns the tech popup if a tech was used this frame.
void func_ov003_0209d4b0(void);

#endif // TWEWY_COMBAT_CORE_BTLPOP_H
