#ifndef TWEWY_COMBAT_CORE_COMBATPSYUTIL_H
#define TWEWY_COMBAT_CORE_COMBATPSYUTIL_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatEnemyActor.h"

/// Advances the spin/fade timers of `count` sprite slots and restarts their animations on demand.
void func_ov003_020a5db8(Ov003AnimSprite* sprites, s32 count, s32 animSet, s32 arg3, s32 arg4, s32 arg5);
/// Spins the live sprite slots around (x, y) and renders them.
void func_ov003_020a5ed4(Ov003AnimSprite* sprites, s32 count, s32 x, s32 y);
/// Releases the live sprite slots.
void  func_ov003_020a5fc0(Ov003AnimSprite* sprites, s32 count);
BOOL  func_ov003_020a603c(u32* id);
void  func_ov003_020a6060(u32 id);
void* func_ov003_020a6084(u32 id);
/// Whether `unit` can take the pending command tagged `arg1`.
s32 func_ov003_020a60a8(Ov003EnemyActor* unit, s32 arg1);
/// Whether `unit` can be given `arg1`; the busy/idle and status bits gate it.
s32 func_ov003_020a60ec(Ov003EnemyActor* unit, s32 arg1);
/// Applies pending command 4 to `unit` and records `arg1`.
s32 func_ov003_020a6164(Ov003EnemyActor* unit, u16 arg1);
/// Applies pending command 5 to `unit` and clears its velocity.
void func_ov003_020a6198(Ov003EnemyActor* unit, s32 arg1);
/// Applies pending command 5 and launches `unit` at `speed` in direction `angle`.
void func_ov003_020a61e4(Ov003EnemyActor* unit, u16 arg1, s32 attackId, Ov003AttackArgs* args, s32 angle, s32 speed,
                         s32 zVelocity);
s32  func_ov003_020a62b8(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32  func_ov003_020a62d0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32  func_ov003_020a62ec(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32  func_ov003_020a632c(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
/// Whether a (0..0x100, 0..0xC0) point lies inside a square of half-size `arg2`.
s32 func_ov003_020a6374(s32 arg0, s32 arg1, s32 arg2);
/// Whether the current battle lets the effect run.
s32 func_ov003_020a63a8(Ov003EffectState* effect);
/// Whether the battle is in the post-boss state that unlocks the extra effects.
s32 func_ov003_020a63e4(void);
/// The player's battle actor.
CombatActor* func_ov003_020a6438(void);
void         func_ov003_020a6450(void);
void         func_ov003_020a6464(s32 arg0);
void         func_ov003_020a647c(s32 arg0);
void         func_ov003_020a6494(s16 arg0);
void         func_ov003_020a64ac(void);
/// Initialises the effect's sprite anchor from the owning touch actor.
void func_ov003_020a64d0(Ov003EffectState* effect, s16 arg1);
/// Queues a sweep attack against `unit`, resetting the streak when the info record says so.
s32 func_ov003_020a6568(Ov003AttackStreak*, u16, Ov003EnemyActor*, s32, s32, u16);
/// Queues a point attack at (x, y, z) and resets the streak when the info record says so.
s32 func_ov003_020a6628(Ov003AttackStreak*, u16, s32, s32, s32, s32, u16);
s32 func_ov003_020a66f4(Ov003AttackStreak* streak, u16 attackId, s32 x, s32 y, s32 force, u16 amount);
/// Queues a sweep attack against every unit whose box overlaps (x1, y1)-(x2, y2).
s32 func_ov003_020a6acc(Ov003AttackStreak*, u16, s32, s32, s32, s32, s32);
/// Story-progress gate for the pin effects: 0 = none, 1 = early, 2 = late.
s32 func_ov003_020a6c14(void);
s32 func_ov003_020a6c7c(s32 arg0);
s32 func_ov003_020a6cc4(s32 arg0);

#endif // TWEWY_COMBAT_CORE_COMBATPSYUTIL_H
