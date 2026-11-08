#ifndef TWEWY_COMBAT_CORE_BTLOBSSPAWN_H
#define TWEWY_COMBAT_CORE_BTLOBSSPAWN_H

#include "nitro/types.h"

/// One step of a field-object spawn list: spawn `count` objects of `kind`; `kind == 0xFF` ends the list.
typedef struct CombatObsSpawnCmd {
    /* 0x0 */ u8 kind;
    /* 0x1 */ s8 count;
} CombatObsSpawnCmd; // Size: 0x2

extern const u8                 CombatObsSpawn_EncounterWeights[1275][4];
extern const CombatObsSpawnCmd* CombatObsSpawn_FieldLists[23][4];

#endif // TWEWY_COMBAT_CORE_BTLOBSSPAWN_H
