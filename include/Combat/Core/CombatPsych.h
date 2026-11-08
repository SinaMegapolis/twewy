#ifndef TWEWY_COMBAT_CORE_COMBATPSYCH_H
#define TWEWY_COMBAT_CORE_COMBATPSYCH_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatTouch.h"

void func_ov003_020a5540(Ov003EffectState* effect, Ov003TouchActor* actor);
void func_ov003_020a563c(Ov003EffectState* effect);
s32  func_ov003_020a58c4(void);
s32  func_ov003_020a58d4(void);
void func_ov003_020a58e4(Ov003EffectState* effect, u16 arg1, u8 arg2);
/// Records the touch-slot origins used by the effect selection.
void func_ov003_020a593c(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_ov003_020a59dc(void);
u8   func_ov003_020a5a1c(void);
/// Finds the badge that owns the effect state at `effect`.
Ov003TouchActor* func_ov003_020a5a34(Ov003EffectState* effect);
void             func_ov003_020a5a94(Ov003EffectState* effect);
s32              func_ov003_020a5aa4(Ov003EffectState* effect);
s32              func_ov003_020a5ab4(Ov003EffectState* effect);
/// Sums the per-actor amount of every actor running effect `state`.
s32   func_ov003_020a5b18(s32 state);
void* func_ov003_020a5d2c(s32 size, const char* name);
/// Staggers the countdown timers of `count` sprite slots.
void func_ov003_020a5d88(Ov003AnimSprite* sprites, s32 count, s32 step);

#endif // TWEWY_COMBAT_CORE_COMBATPSYCH_H
