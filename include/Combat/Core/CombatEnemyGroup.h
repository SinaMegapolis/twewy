#ifndef TWEWY_COMBAT_CORE_COMBATENEMYGROUP_H
#define TWEWY_COMBAT_CORE_COMBATENEMYGROUP_H

#include "nitro/types.h"

/// The sound archive each noise group loads.
extern const s8 CombatEnemyGroup_SoundArc[0x40];

extern u8  data_ov003_020e6a30[8];
extern s32 data_ov003_020e6a38[3];

/// The sprite descriptor table of each noise group's variants, in the noise overlays.
extern void** CombatEnemyGroup_SpriteDescs[61];

#endif // TWEWY_COMBAT_CORE_COMBATENEMYGROUP_H
