#ifndef TWEWY_COMBAT_CORE_COMBATENEMY_H
#define TWEWY_COMBAT_CORE_COMBATENEMY_H

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatCutin.h"

void func_ov003_020c2064(Ov003CutinTracks* tracks);
void func_ov003_020c20b8(Ov003CutinTracks* tracks, Vec* data, u16 count);
void func_ov003_020c20d4(Ov003CutinTracks* tracks, Ov003CutinActor* actor);
/// Starts noise `index` (0 = the default layout) and kicks off its actors.
void func_ov003_020c2a34(s32 index);
/// (Re)initialises the shared save-resident noise slot table.
void func_ov003_020c2af0(void);
u32  func_ov003_020c2b3c(s32 index);
void func_ov003_020c2b5c(Ov003CutinActor* actor);
void func_ov003_020c2b68(Ov003CutinActor* actor);
/// True while the actor is safely inside the camera bounds.
s32 func_ov003_020c2bbc(Ov003CutinActor* actor);
/// Three-phase approach/attack/leave state machine used by the normal noise attack.
s32 func_ov003_020c2c30(Ov003CutinActor* actor, s32 animBase, s32 targetX, s32 targetY, s32 targetZ, s32 arg5);
/// Projectile-like noise attack state machine (fly toward the target, then leave).
s32 func_ov003_020c2f68(Ov003CutinActor* actor, s32 animBase, s32 speed, s32 spread);
/// Third-actor entrance/exit state machine.
s32  func_ov003_020c3570(Ov003CutinActor* actor, u16 animBase);
void func_ov003_020c3700(Ov003EnemyActor* actor);
void func_ov003_020c37bc(Ov003EnemyActor* actor);
/// Fills an animation descriptor from a noise parameter table and loads it into a sprite.
void func_ov003_020c3814(s32 engine, BattleSprite* sprite, const BinIdentifier* iden, const u8* table, u16 index, u16 arg4,
                         u16 arg5, s16 arg6, u16 arg7, s16 arg8);
s32  func_ov003_020c38b8(BattleSprite* sprite, u16 anim, s32 arg2);
s32  func_ov003_020c3920(BattleSprite* sprite);
/// Clamps `*value` to the camera's horizontal bounds.
s32 func_ov003_020c3960(s32* value, s32 margin);
/// Rotates a 2D offset so it points along the vector between the noise and its target.
void      func_ov003_020c39b0(Vec* src, Vec* dst);
TaskPool* func_ov003_020c3a10(u32 index);
void      func_ov003_020c3acc(s32 engine, void* arg1, s32 arg2);
void      func_ov003_020c3b1c(u16* tracks);
/// Returns the first index whose cumulative weight exceeds `target`.
s32                     func_ov003_020c3b30(u16* weights, s32 count, s32 target);
void                    func_ov003_020c3ba8(s32 x, s32 y);
s32                     func_ov003_020c3bf0(void* actor);
TaskPool*               func_ov003_020c3ca4(Ov003CutinActor* actor);
void                    func_ov003_020c3ccc(Ov003CutinActor* actor);
void                    func_ov003_020c3cdc(Ov003CutinActor* actor);
void                    func_ov003_020c3e64(Ov003EnemyActor*);
void                    func_ov003_020c3e88(Ov003CutinActor* actor, s32 arg1);
s32                     func_ov003_020c37f8(BattleSprite*);
s32                     func_ov003_020c38e8(BattleSprite*);
s32                     func_ov003_020c3908(BattleSprite*);
u32                     func_ov003_020c3aac(u32);
s32                     func_ov003_020c3c28(void);
void                    func_ov003_020c3c44(Ov003CutinScript*, void*, s16);
const BattleSpriteDesc* func_ov003_020c3c6c(Ov003EnemyActor*);
Ov003ActorParams*       func_ov003_020c3c88(Ov003EnemyActor*);
void func_ov003_020c3cec(const TaskHandle* mainHandle, const TaskHandle* subHandle, void* mainData, void* subData);
s32 func_ov003_020c3d28(const TaskHandle* subHandle, const TaskHandle* mainHandle, void* subData, void* mainData, s32* mainOut,
                        s32* subOut);
s32 func_ov003_020c3ec0(BattleActor*);
s32 func_ov003_020c3efc(Ov003EnemyActor*, Ov003CutinCmdData*);

#endif // TWEWY_COMBAT_CORE_COMBATENEMY_H
