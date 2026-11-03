#ifndef TWEWY_COMBAT_CORE_BTLAURA_H
#define TWEWY_COMBAT_CORE_BTLAURA_H

#include "Engine/EasyTask.h"

/// The aura of the player's gauge: flames that drift up below the gauge actor while the partner sync rate is high.
extern const TaskHandle Tsk_BtlAura;

/// Spawns the sub-screen aura at world position (`x`, `y`).
void func_ov003_020974f8(s32 x, s32 y);

#endif // TWEWY_COMBAT_CORE_BTLAURA_H
