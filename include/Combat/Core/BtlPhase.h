#ifndef TWEWY_COMBAT_CORE_BTLPHASE_H
#define TWEWY_COMBAT_CORE_BTLPHASE_H

#include "Engine/EasyTask.h"

/// The "ready" banner shown when a battle starts.
extern const TaskHandle Tsk_BtlReady;

/// The victory banner shown when a battle is won.
extern const TaskHandle Tsk_BtlWin;

/// The game over banner shown when a battle is lost.
extern const TaskHandle Task_BtlOver;

/// The pause menu shown while the battle is paused with START.
extern const TaskHandle Tsk_BtlPause;

#endif // TWEWY_COMBAT_CORE_BTLPHASE_H
