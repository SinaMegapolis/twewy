#ifndef TWEWY_COMBAT_CORE_COMBATENEMYAI_H
#define TWEWY_COMBAT_CORE_COMBATENEMYAI_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatCutin.h"

/// Resets a noise to its idle defaults (a fresh 0x1C0-byte actor block).
void func_ov003_020c41e8(Ov003CutinActor* actor);
/// Releases the main sprite's resources.
void func_ov003_020c4268(Ov003CutinActor* actor);
void func_ov003_020c42d4(Ov003CutinActor* actor);
void func_ov003_020c439c(Ov003EnemyActor* actor, SpriteAnimationEx* anim);
/// Same as `func_ov003_020c4520`, using the secondary load template.
s32  func_ov003_020c45a4(Ov003CutinActor* actor);
void func_ov003_020c49c8(Ov003EnemyActor* actor);
void func_ov003_020c4a08(Ov003CutinActor* actor);
s32  func_ov003_020c4a34(Ov003EnemyActor* actor);
s32  func_ov003_020c4a74(Ov003EnemyActor* actor);
void func_ov003_020c4b9c(Ov003CutinActor* actor);
void func_ov003_020c4bdc(Ov003CutinActor* actor);
void func_ov003_020c4d84(void);
void func_ov003_020c4da8(void);
s32  func_ov003_020c4e0c(s32 value);
s32  func_ov003_020c4e7c(s32 value);
void func_ov003_020c4eb4(Ov003CutinActor* actor);
/// Retires the actor's main-screen command slot.
void func_ov003_020c4f00(Ov003EnemyActor* actor);
void func_ov003_020c4f60(s32 id);
void func_ov003_020c4f80(s32 id);
s32  func_ov003_020c4ff4(Ov003EnemyActor* actor);
s32  func_ov003_020c5058(Ov003EnemyActor* actor);
void func_ov003_020c50bc(void);
/// Pushes the actor's hit effects to the combo counters.
void func_ov003_020c5130(Ov003EnemyActor* actor);
/// Nudges the actor one eighth of the way towards (x, y), clamped to +/-0x8000.
void func_ov003_020c51a4(Ov003CutinActor* actor, s32 x, s32 y);
/// True if the actor and its partner swapped sides of the sub-screen centre.
s32  func_ov003_020c5220(Ov003EnemyActor* actor);
s32  func_ov003_020c5864(Ov003CutinActor* actor, s32* outX, s32* outY);
s32  func_ov003_020c58c8(Ov003CutinActor* actor, s32* outX, s32* outY, u16 arg3);
s32  func_ov003_020c59f4(Ov003EnemyActor* actor);
s32  func_ov003_020c5a00(BattleActor* target);
s32  func_ov003_020c5a14(BattleActor* target);
s32  func_ov003_020c5a28(Ov003EnemyActor* self, BattleActor* target);
s32  func_ov003_020c5a60(Ov003EnemyActor* self, BattleActor* target);
void func_ov003_020c5a98(Ov003EnemyActor* self, BattleActor* target);
void func_ov003_020c5ac0(Ov003EnemyActor* self, u16 attackId, BattleActor* target, s32 xVelocity, s32 yVelocity,
                         s32 zVelocity);
s32  func_ov003_020c5b0c(u16 id, Ov003EnemyActor* owner, s32 arg2);
s32  func_ov003_020c5b64(u16 id, Ov003EnemyActor* owner, s32 x, s32 y);
s32  func_ov003_020c5b8c(const BattleAttackInfo* info, Ov003EnemyActor* owner, s32 x, s32 y, s32 z);
s32  func_ov003_020c5ba4(u16 id, BattleAttackArgs* args, s32 arg2);
/// Third noise state machine: dash in, bounce and recover.
s32  func_ov003_020c6920(Ov003EnemyActor* actor, u16 anim, s32 arg2);
void func_ov003_020c6b9c(Ov003CutinActor* actor, u16 anim);
void func_ov003_020c6bbc(Ov003CutinActor* actor, u16 anim);
s32  func_ov003_020c6f40(Ov003EnemyActor*);
s32  func_ov003_020c6f68(Ov003EnemyActor*);
/// Drives the end-of-noise teardown task until the fade completes.
s32  func_ov003_020c6f90(Ov003EnemyActor* actor);
void func_ov003_020c427c(Ov003EnemyActor*);
s16  func_ov003_020c42ec(Ov003EnemyActor*);
s32  func_ov003_020c4348(Ov003EnemyActor*);
s32  func_ov003_020c44ac(Ov003EnemyActor*);
s32  func_ov003_020c4520(Ov003EnemyActor*);
void func_ov003_020c4628(Ov003EnemyActor*);
void func_ov003_020c4668(Ov003EnemyActor*);
void func_ov003_020c4748(Ov003EnemyActor*);
s32  func_ov003_020c4830(Ov003EnemyActor*);
void func_ov003_020c48b0(Ov003EnemyActor*);
void func_ov003_020c492c(Ov003EnemyActor*);
void func_ov003_020c495c(Ov003EnemyActor*);
void func_ov003_020c4988(Ov003EnemyActor*);
void func_ov003_020c4ab4(Ov003EnemyActor*, s32);
void func_ov003_020c4af0(Ov003EnemyActor*);
s32  func_ov003_020c4b1c(Ov003EnemyActor*);
void func_ov003_020c4b5c(Ov003EnemyActor*);
void func_ov003_020c4c1c(Ov003EnemyActor*);
void func_ov003_020c4c5c(Ov003EnemyActor*);
void func_ov003_020c4c9c(Ov003EnemyActor*);
void func_ov003_020c4cc4(Ov003EnemyActor*, s32);
void func_ov003_020c4ee0(Ov003EnemyActor*);
void func_ov003_020c4fc8(Ov003EnemyActor*);
s32  func_ov003_020c5924(Ov003EnemyActor*, s32*);
s32  func_ov003_020c59a0(Ov003EnemyActor*);
s32  func_ov003_020c5b2c(u16 id, Ov003EnemyActor* owner, s32 x, s32 y, s32 z);
s32  func_ov003_020c5bc4(u16, BattleAttackArgs*, s32, s32, s32);
s32  func_ov003_020c5bfc(Ov003EnemyActor*);
s32  func_ov003_020c5e98(Ov003EnemyActor*);
s32  func_ov003_020c6230(Ov003EnemyActor*);
s32  func_ov003_020c62c4(Ov003EnemyActor*, u16);
s32  func_ov003_020c65cc(Ov003EnemyActor*, u16);
s32  func_ov003_020c6b8c(Ov003EnemyActor*, u16);
s32  func_ov003_020c6bac(Ov003EnemyActor*, u16);
s32  func_ov003_020c6bc8(Ov003EnemyActor*, u16);
s32  func_ov003_020c6c2c(Ov003EnemyActor*, u16);

s32 func_ov003_020c703c(Ov003EnemyActor*);
s32 func_ov003_020c7070(Ov003EnemyActor*);
s32 func_ov003_020c70a4(Ov003CutinActor* actor, s32 anim);
s32 func_ov003_020c72b4(Ov003EnemyActor*, s32, s32);

#endif // TWEWY_COMBAT_CORE_COMBATENEMYAI_H
