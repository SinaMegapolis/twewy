#ifndef TWEWY_COMBAT_CORE_BTLEFF_H
#define TWEWY_COMBAT_CORE_BTLEFF_H

#include "Combat/Core/CombatBossExp.h"
#include "Combat/Core/CombatTouch.h"
#include "Engine/EasyTask.h"

/// The level-up effect shown over the partner when the battle level rises.
extern const TaskHandle Tsk_BtlEff_LvUp;

/// One sprite that flies out of a defeated boss.
extern const TaskHandle BtlEff_SideMove;

/// Spawns a hit effect of id `id` on the display engine `mode` at world position (`x`, `y`, `z`),
/// flipped to match `flip`. Ignored once ten effects are already live on that screen.
void func_ov003_0209e9f8(s32 mode, s32 id, s32 x, s32 y, s32 z, s32 flip);

/// Spawns hit effect `id` at world position (`x`, `y`, `z`), flipped to match `flip`. Ignored once
/// ten effects are already live on the screen the record belongs to.
void func_ov003_0209ed70(u8 id, s32 x, s32 y, s32 z, s32 flip);

/// Spawns the sword-slash effect playing `animIndex`.
void func_ov003_0209f000(s16 animIndex);

/// Spawns the death effect on `engine`'s screen at the given world position.
void func_ov003_0209f444(s32 engine, s32 x, s32 y, s32 z, s32 hasPartner);

/// Spawns the warp effect that trails the display actor from (`x`, `y`, `z`) to its new position.
void func_ov003_0209f770(s32 mode, s32 x, s32 y, s32 z);

/// Spawns a splash of `count` particles at (`x`, `y`, `z`) on `engine`. An `independent` splash
/// always spawns; any other splash only spawns once per arming of battle flag 0x80.
void func_ov003_0209fc88(s32 engine, s32 x, s32 y, s32 z, s16 count, s32 independent);

/// Spawns the star effect of charge level `level` around `owner`.
s32 func_ov003_020a0244(s32 level, Ov003TouchActor* owner);

/// Takes over the four animation slots of the boss defeat burst.
void func_ov003_020c9018(const BossExp1Anims* anims);

/// Fills all four animation slots of the boss defeat burst with `anim`.
void func_ov003_020c9064(const SpriteAnimationEx* anim);

/// Fills the animation slots of the boss defeat burst from the cut-in actor and its partner.
void func_ov003_020c9118(Ov003EnemyActor* actor);

/// Starts the boss defeat burst task and waits for it. Returns 0 once the burst is over.
s32 func_ov003_020c9338(Ov003EnemyActor* actor);

#endif // TWEWY_COMBAT_CORE_BTLEFF_H
