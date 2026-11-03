#ifndef TWEWY_COMBAT_CORE_BTLINF_H
#define TWEWY_COMBAT_CORE_BTLINF_H

#include "Engine/EasyTask.h"

/// The charge-command panel: the current charge step, its progress, the next step's length and a star.
extern const TaskHandle Tsk_BtlInf_Star;

/// The exp panel, shown for three seconds whenever the exp needed for the next level changes.
extern const TaskHandle Tsk_BtlInf_Exp;

/// The "Auto" HUD label.
extern const TaskHandle Tsk_BtlInf_Auto;

/// The battle clock, showing the clear time as mm:ss.hh.
extern const TaskHandle Tsk_BtlInf_Timer;

/// Spawns the value display in `pool`. The variadic arguments are `const u16*` pairs (main
/// screen value, sub screen value), either of which may be NULL, terminated by a NULL pair.
s32 func_ov003_020a2f7c(TaskPool* pool, ...);

/// Frees every queued touch event.
void func_ov003_020a300c(void);

/// Queues the touch-event ability of each of the four equipped threads `items` for `touchIndex`.
void func_ov003_020a303c(const u16* items, s32 touchIndex);

#endif // TWEWY_COMBAT_CORE_BTLINF_H
