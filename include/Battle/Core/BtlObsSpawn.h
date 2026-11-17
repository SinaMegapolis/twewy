#ifndef TWEWY_BATTLE_CORE_BTLOBSSPAWN_H
#define TWEWY_BATTLE_CORE_BTLOBSSPAWN_H

#include "nitro/types.h"

/// One step of a field-object spawn list: drop `count` objects of group `kind`; `kind == 0xFF` ends the list.
typedef struct BattleObsSpawnCmd {
    /* 0x0 */ u8 kind; // field-object group, see BtlObsMgr_DropGroup
    /* 0x1 */ s8 count;
} BattleObsSpawnCmd;   // Size: 0x2

/// Per encounter, the weights (in percent) of the four spawn lists it may roll; the remainder drops nothing.
extern const u8 BtlObsSpawn_EncounterWeights[1275][4];

/// The spawn lists of each field's object group (`BtlFieldDef.obsGroup`), one per weighted roll.
extern const BattleObsSpawnCmd* BtlObsSpawn_FieldLists[23][4];

#endif // TWEWY_BATTLE_CORE_BTLOBSSPAWN_H
