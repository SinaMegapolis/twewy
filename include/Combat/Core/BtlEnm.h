#ifndef TWEWY_COMBAT_CORE_BTLENM_H
#define TWEWY_COMBAT_CORE_BTLENM_H

#include "Combat/Core/CombatCutin.h"

/// Starts the main-screen enemy aura of `owner` and its partner.
void func_ov003_020ca66c(Ov003CutinActor* owner);

/// Sets the enemy aura state to `mode` unless the aura is already engaged this frame.
void func_ov003_020ca6a8(Ov003CutinActor* unit);

/// The unit's attack value, scaled by the aura's per-cent scale while the aura is engaged.
s16 func_ov003_020ca728(Ov003CutinActor* unit);

/// Spawns the sub-screen enemy aura at world position (`x`, `y`).
void func_ov003_020cac38(s32 x, s32 y);

/// Starts the blink streak of `owner` along `dir` in five lengths, 2 to 6 steps long.
void func_ov003_020cb0b4(Ov003CutinActor* owner, Vec* dir, s32 reverse);

#endif // TWEWY_COMBAT_CORE_BTLENM_H
