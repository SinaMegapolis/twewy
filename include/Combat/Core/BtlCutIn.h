#ifndef TWEWY_COMBAT_CORE_BTLCUTIN_H
#define TWEWY_COMBAT_CORE_BTLCUTIN_H

#include "Combat/Core/CombatCutin.h"
#include "Engine/EasyTask.h"

/// The partner's cut-in.
extern const TaskHandle Tsk_BtlCutIn00;

/// The first fusion cut-in.
extern const TaskHandle Tsk_BtlCutIn01;

/// The second fusion cut-in.
extern const TaskHandle Tsk_BtlCutIn02;

/// Spawns the Kariya and Yashiro variant of the partner's cut-in in `pool`.
s32 BtlCutIn00_CreateKariyaYashiroTask(TaskPool* pool);

#endif // TWEWY_COMBAT_CORE_BTLCUTIN_H
