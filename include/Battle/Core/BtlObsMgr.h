#ifndef TWEWY_BATTLE_CORE_BTLOBSMGR_H
#define TWEWY_BATTLE_CORE_BTLOBSMGR_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Engine/File/BinMgr.h"

/// The touch cursor graphics.
extern const BinIdentifier Iden_BtlTch;

void BtlObsMgr_DropAtRandom(u16 kind);
void BtlObsMgr_DropGroup(s32 kind, s32 count);
s32  BtlObsMgr_LoadSprite(s32 engine, BattleSprite* sprite, u32 id);
void BtlObsMgr_LoadHitEffect(s32 engine, BattleSprite* sprite, s32 id);
void BtlObsMgr_InitActor(BattleActor* actor, u16 id);
/// Plays the field object's sound on the main (`engine == 0`) or sub screen.
void BtlObsMgr_PlaySe(s32 engine, s32 id, s32 x);
s32  func_ov003_0208656c(void);
void BtlObsMgr_Spawn(u16 id, s32 x, s32 y);
/// Spawns the field-object task for config `id`, writing the task id through `out`.
s32  BtlObsMgr_SpawnTask(u16 id, s32 x, s32 y, s32* out);
void BtlObsMgr_Defeat(Ov003EffectActor* effect);
void BtlObsMgr_Setup(Ov003EffectActor* effect, Ov003EffectSpawn* spawn);
void BtlObsMgr_Update(Ov003EffectActor* effect);
void BtlObsMgr_Remove(Ov003EffectActor* effect);
void BtlObsMgr_ResetList(void);
/// Rolls the encounter's field-object drop and spawns the objects of the chosen list.
void BtlObsMgr_RollDrop(void);

#endif // TWEWY_BATTLE_CORE_BTLOBSMGR_H
