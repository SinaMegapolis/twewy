#ifndef TWEWY_COMBAT_CORE_COMBATSYS_H
#define TWEWY_COMBAT_CORE_COMBATSYS_H

#include "Battle/Core/BtlActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Combat/Core/CombatSprite.h"
#include "Engine/File/BinMgr.h"

/// The touch cursor graphics.
extern const BinIdentifier Iden_BtlTch;

void CombatSys_DropObsAtRandom(u16);
void CombatSys_DropObsGroup(s32, s32);
s32  CombatSys_LoadObsSprite(s32 engine, CombatSprite* sprite, u32 id);
void CombatSys_LoadObsHitEffect(s32 arg0, CombatSprite* arg1, s32 arg2);
/// Plays the field object's sound on the main (`arg0 == 0`) or sub screen.
void CombatSys_PlayObsSe(s32 arg0, s32 arg1, s32 arg2);
void CombatSys_SpawnObs(u16 id, s32 arg1, s32 arg2);
/// Spawns the field-object task for config `id`, writing the task id through `out`.
s32  CombatSys_SpawnObsTask(u16 id, s32 arg1, s32 arg2, s32* out);
void CombatSys_DefeatObs(Ov003EffectActor* effect);
void CombatSys_SetupObs(Ov003EffectActor* effect, Ov003EffectSpawn* spawn);
void CombatSys_UpdateObs(Ov003EffectActor* effect);
void CombatSys_RemoveObs(Ov003EffectActor* effect);
void CombatSys_ResetObsList(void);
/// Rolls the encounter's field-object drop and spawns the objects of the chosen list.
void CombatSys_RollObsDrop(void);
void CombatSys_PushPartnerAction(u16 value);
void CombatSys_ClearPartnerActions(void);
void CombatSys_SetPartnerDirection(s32 value);
u16  CombatSys_PopPartnerAction(void);
u16  CombatSys_TakePartnerDirection(void);
s32  func_ov003_02086a64(void);
s32  func_ov003_02086a80(void);
void func_ov003_02086aa8(s32 value);
void func_ov003_02086ac4(s32 enabled);
void func_ov003_02086af0(void);
void CombatSys_InitPartnerActions(Ov003ChargeState* state);
void CombatSys_ShuffleIds(u16* table, u32 count);
/// Refills the effect command's queue with a fresh shuffle.
void               func_ov003_02086cc0(void);
u16                func_ov003_02086d24(void);
void               func_ov003_02086d8c(CombatSprite* sprite, s32 animId, s32 flipped, s32 x, s16 y, s16 z);
void               func_ov003_02086e38(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32                func_ov003_02086f7c(void);
s8                 func_ov003_02086fec(void);
void               func_ov003_02087008(void);
void               func_ov003_020871cc(s32 arg0);
void               func_ov003_02087250(Ov003EffectState* state, BattleActor* target, s32 angle);
void               func_ov003_020872b4(Ov003ActionEffect* effect);
void               func_ov003_02087310(void);
Ov003ActionEffect* func_ov003_02087358(void);
void               func_ov003_02087378(void);
/// Retires the action effects whose target actor is gone or defeated.
void func_ov003_020873e0(void);
void func_ov003_02087458(s32 mode, s16* timer);
void func_ov003_02087540(s32 mode);
u16  func_ov003_0208765c(u16* state, s32 value, s32 arg2);
s32  func_ov003_020876c0(void);
s32  func_ov003_02087704(void);
/// Plays one of the two hit-reaction SE sets, picked at random.
void func_ov003_02087804(s32 sub);
/// Moves the player (`mode != 1`) or the partner to a new position, facing the way it moved.
void CombatSys_WarpCharacter(s32 mode, s32 x, s32 y, s32 z);

void CombatSys_InitObsActor(BattleActor*, u16);
s32  func_ov003_0208656c(void);
void CombatSys_EaseToSine(s32*, s16*);
s16* func_ov003_02086e94(void);
s32  func_ov003_02086f20(s32, s32);
void func_ov003_02087094(Ov003EffectCmd*, u8, s32);
void func_ov003_020871b0(Ov003EffectCmd*);
void func_ov003_0208722c(void);

#endif // TWEWY_COMBAT_CORE_COMBATSYS_H
