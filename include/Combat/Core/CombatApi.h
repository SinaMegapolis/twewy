#ifndef COMBAT_CORE_COMBATAPI_H
#define COMBAT_CORE_COMBATAPI_H

/// Functions of overlay 3 that are called from other translation units.
/// Grouped by the unit that owns them.

#include "Combat/Badge/BtlBadgeActor.h"
#include "Combat/Badge/BtlBadgeMgr.h"
#include "Combat/Core/BtlMgr.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatAnim.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatBgRes.h"
#include "Combat/Core/CombatBossExp.h"
#include "Combat/Core/CombatCameraShake.h"
#include "Combat/Core/CombatCollision.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatDebug.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Combat/Core/CombatEnemy.h"
#include "Combat/Core/CombatEnemyAi.h"
#include "Combat/Core/CombatEnemyGroup.h"
#include "Combat/Core/CombatEnemyParam.h"
#include "Combat/Core/CombatEnemyShot.h"
#include "Combat/Core/CombatHit.h"
#include "Combat/Core/CombatPsyAnim.h"
#include "Combat/Core/CombatPsyTrail.h"
#include "Combat/Core/CombatPsyUtil.h"
#include "Combat/Core/CombatPsych.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Combat/Core/CombatSe.h"
#include "Combat/Core/CombatSprite.h"
#include "Combat/Core/CombatSpriteSlot.h"
#include "Combat/Core/CombatStage.h"
#include "Combat/Core/CombatSys.h"
#include "Combat/Core/CombatTouch.h"
#include "Field/FieldRegion.h"

// BtlMic.c
void BtlMic_CreateTask(void);
s32  func_ov003_020a50cc(void);
s32  func_ov003_020a51c0(void);

// BtlBadge.c
void func_ov003_020a6d24(Ov003PinAction* action);

// BtlBadge_ExecEffect.c
void func_ov003_020a7284(Ov003TouchActor*);

// BtlBadge_Gauge.c
void func_ov003_020a7640(Ov003PinAction* action);

// BtlBadge_Stock.c
void func_ov003_020a7e68(Ov003PinAction* action);

// BtlBadge_RankingEffect.c
void func_ov003_020a7ba8(Ov003TouchActor*);

// BtlBadge_TimeBoot.c
void func_ov003_020a8300(Ov003TouchActor*, s32, s32);

// BtlPsy_FldShake.c
void func_ov003_020a8768(void);

// BtlPsy_Blur.c
void func_ov003_020acd04(void);

// BtlPsy_DragAttack_Effect.c
s32 func_ov003_020acd6c(TaskPool* pool, Task* task, void* args);
s32 func_ov003_020acdc0(TaskPool* pool, Task* task, void* args);
s32 func_ov003_020ace54(TaskPool* pool, Task* task, void* args);
s32 func_ov003_020acebc(TaskPool* pool, Task* task, void* args);

// SubTsk_BtlPsy_Psycho_T.c
void func_ov003_020b4e48(Ov003EnemyActor* unit, Ov003PsychoHitContext* context);

// BtlPsy_Heal.c
s32  func_ov003_020b6368(Ov003EffectContext* context, Ov003EffectState* effect);
s32  func_ov003_020b641c(Ov003EffectContext* context, Ov003EffectState* effect);
s32  func_ov003_020b6498(Ov003EffectContext* context, Ov003EffectState* effect);
s32  func_ov003_020b6644(Ov003EffectContext* context, Ov003EffectState* effect);
s32  func_ov003_020b66b4(Ov003EffectContext* context, Ov003EffectState* effect);
void func_ov003_020b67dc(Ov003EffectState* effect, CombatActor* actor);
s32  func_ov003_020b68ec(Ov003EffectState* effect, CombatActor* actor);

// BtlPsy_ShockWave_Body.c
s32  func_ov003_020b6b20(Ov003ShockWaveArgs* args);
u32  func_ov003_020b6b50(u32 angle, u32 limit);
s32  func_ov003_020b6ba4(TaskPool* pool, Task* task, void* args, s32 stage);
void func_ov003_020b7b34(s32 x, s32 y, s32 z, s32 kind, s32 flip, u32* out);

// BtlPrizeItem.c
void func_ov003_020bb190(s32, u32, s32);
s32  func_ov003_020bb47c(void);
void func_ov003_020bb570(Ov003CutinActor* actor);
void func_ov003_020bb580(void*);

s32 func_ov003_020bb48c(s32, s8, u16, s32, s32, s32, s32);

// BtlActSp00.c
void func_ov003_020c1f18(TaskPool* pool, u16 level);
s32  func_ov003_020c1f94(TaskPool* pool, s32 character);
s32  func_ov003_020c2008(TaskPool* pool);

// BossExp1.c
s32 func_ov003_020c9008(void);

// BtlEnmShot_Blossom.c
void             func_ov003_020cb128(s32*, s32, s32);
void             func_ov003_020cb130(BtlBinSet*, const BtlBinEntry*, u16, s32, const char*);
s32              func_ov003_020cb150(BtlBinSet*);
void             func_ov003_020cb194(BtlBinSet*);
u16              func_ov003_020cb3c4(s32, s32);
Ov003EnemyActor* func_ov003_020cb42c(s32, s32, s32);
s32              func_ov003_020cb498(s32, s32, BtlActorFilter, void*);
void             func_ov003_020cb594(Ov003EnemyActor*, s32);
s32              func_ov003_020cb764(s32);
s32              func_ov003_020cb7c4(s32 index, s32 x, s32 y);
s16              func_ov003_020cb888(u16*, s32, s32);
void             func_ov003_020cb8cc(s32* vx, s32* vy, s32 x0, s32 y0, s32 x1, s32 y1, s32 steps);
s32              func_ov003_020cb910(s32*, s32*, s32*, s32, s32, s32, s32, s32, s32, s32);
s32              func_ov003_020cba14(s32, s32, s32, s32);
s32              func_ov003_020cba2c(s32, s32, s32, s32);
s32              func_ov003_020cba54(s32, s32, s32, s32, s32, s32);
void             func_ov003_020cbab8(s32* vx, s32* vy, s32 x0, s32 y0, s32 x1, s32 y1, s32 speed);
void             func_ov003_020cbbd4(s32* mx, s32* my, s32 x0, s32 y0, s32 x1, s32 y1);
void             func_ov003_020cbd30(BtlVec2*, BtlVec2, BtlVec2, BtlVec2, s32);
void             func_ov003_020cbdd8(Vec* out, Vec p0, Vec p1, Vec p2, s32 t);
void             func_ov003_020cbf54(BtlVec2* out, BtlVec2 p0, BtlVec2 p1, BtlVec2 p2, BtlVec2 p3, s32 t);
void             func_ov003_020cc020(Vec* out, Vec p0, Vec p1, Vec p2, Vec p3, s32 t);
s32              func_ov003_020cc2d0(CombatActor* actor, s32 index);
void*            func_ov003_020cc644(s32 side, s32 id, s32 n);
s32              func_ov003_020cc6a4(s32 unused, s32 id, BtlItemFilter filter, s32 arg);
void             func_ov003_020cc71c(CombatActor* actor, s32 flag);
void             func_ov003_020cc834(Ov003EnemyShotSource* source, Ov003CutinActor* owner, const void* visual, u16 attackId);
s32              func_ov003_020ccc60(s32 side, s32 id);
s32              func_ov003_020cd134(Ov003EnemyShotSource* source, u16 angle, s32 speed, s32 curveSpeed, s32 yScale);

s32   func_ov003_020cb200(BtlBinSet*, u16);
s32   func_ov003_020cb264(BtlBinSet*, u32);
void  func_ov003_020cb304(BtlBinSet*, u16);
s32   func_ov003_020cb32c(BtlBinSet*, BinIdentifier*);
s32   func_ov003_020cb348(BtlBinSet*, u16);
s32   func_ov003_020cb368(BinIdentifier*);
void  func_ov003_020cb520(CombatActor*, s32);
s32   func_ov003_020cb55c(Ov003EnemyActor*);
void  func_ov003_020cb578(CombatActor*, s32);
void  func_ov003_020cb5b0(Ov003EnemyActor*, s16, s32);
s32   func_ov003_020cb648(Ov003EnemyActor*, s32);
s32   func_ov003_020cb744(s32);
s32   func_ov003_020cb784(s32);
s32   func_ov003_020cb7a4(s32);
s32   func_ov003_020cbafc(s32*, s32*, s32*, s32, s32, s32, s32, s32, s32, s32);
void  func_ov003_020cbc50(s32*, s32*, u16, s32);
void  func_ov003_020cbcb4(s32*, s32*, u16, s32, s32);
s32   func_ov003_020cc300(Ov003EnemyActor*, s32);
s32   func_ov003_020cc354(CombatActor*);
s32   func_ov003_020cc38c(Ov003CutinActor*, s32, s32, s32, s32, s32, s32);
u32   func_ov003_020cc430(Ov003CutinActor*, s32, s32, s32);
s32   func_ov003_020cc5d0(CombatActor*);
s32   func_ov003_020cc5fc(s32, s32);
void* func_ov003_020cc750(s32);
void  func_ov003_020cc784(CombatActor*, s32);
s32   func_ov003_020cc7c0(s32, s32, s32);
s32   func_ov003_020cc7f4(s32, s32, s32, s32);
void  func_ov003_020ccea8(CombatActor*, CombatSprite*);
void  func_ov003_020ccec0(CombatActor*, s32);
s32   func_ov003_020ccedc(s32);
s32   func_ov003_020ccefc(s32);
void  func_ov003_020ccf1c(s32*, s32);
void  func_ov003_020ccf58(s32, Ov003EnemyActor*);
void  func_ov003_020ccf94(s32, CombatSprite*, const BtlSpriteDesc*);
s32   func_ov003_020ccfc8(Ov003EnemyActor*, u16);
s32   func_ov003_020ccfec(Ov003EnemyActor*);
s32   func_ov003_020cd010(Ov003EnemyActor*);
s32   func_ov003_020cd11c(s32);
// BtlBadge_ConditionBoot.c
void func_ov003_020a6f90(Ov003TouchActor* actor, s32 arg1);

void func_0202fbf4(s32 arg0);
void func_ov005_020e740c(s32, s32);
s32  func_ov005_020e74fc(void* queue, s32 capacity, s32 arg2);
void func_ov005_020eb29c(s32, s32, s32);

#endif // COMBAT_CORE_COMBATAPI_H
