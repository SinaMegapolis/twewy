#ifndef COMBAT_CORE_COMBATAPI_H
#define COMBAT_CORE_COMBATAPI_H

/// Functions of overlay 3 that are called from other translation units.
/// Grouped by the unit that owns them.

#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatAttack.h"
#include "Combat/Core/CombatBgRes.h"
#include "Combat/Core/CombatBossExp.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Combat/Core/CombatEnemyShot.h"
#include "Combat/Core/CombatPsychEffects.h"
#include "Combat/Core/CombatSprite.h"
#include "Combat/Core/CombatSpriteSlot.h"
#include "Combat/Core/CombatTouch.h"
#include "Field/FieldRegion.h"

/// The 8-byte payload an attack entry point allocates for its on-fire handler.
typedef struct Ov003AttackData {
    /* 0x00 */ Ov003EffectState* effect;
    /* 0x04 */ CombatActor*      target;
} Ov003AttackData; // Size: 0x8

/// An attack entry point: the handler the battle calls when the attack lands, plus its payload.
typedef struct Ov003AttackCmd {
    /* 0x00 */ void (*fire)(Ov003AttackData*);
    /* 0x04 */ s32              unk_04;
    /* 0x08 */ Ov003AttackData* data;
} Ov003AttackCmd; // Size: 0xC

/// Playback cursor over a 4-byte-per-entry animation script (`func_ov003_020d38ac`).
typedef struct Ov003AnimScript {
    /* 0x00 */ const struct Ov003AnimScriptStep* steps;
    /* 0x04 */ u16                               index;
    /* 0x06 */ u16                               loop;
    /* 0x08 */ u16                               flags;
} Ov003AnimScript; // Size: 0xA

typedef struct Ov003AnimScriptStep {
    /* 0x00 */ s16 dataId; // -1 terminates the script
    /* 0x02 */ u16 entry;
} Ov003AnimScriptStep;     // Size: 0x4

/// Sprite-loading command shared by `func_ov003_020d36fc`/`func_ov003_020d37a0`. The tables are
/// arrays of 8-byte `SpriteAnimEntry` records; only `table` is actually read by the loader.
typedef struct Ov003SpriteLoadCmd {
    /* 0x00 */ const SpriteAnimEntry* entries;
    /* 0x04 */ const SpriteAnimEntry* table;
    /* 0x08 */ s32                    unk_08;
    /* 0x0C */ u16                    unk_0C;
} Ov003SpriteLoadCmd; // Size: 0x10

/// Scripted translation stepping shared with overlays 017/020/021 (owned by those units).
typedef struct Ov003Shake {
    /* 0x00 */ s16                          frame; // frames spent on the current step
    /* 0x02 */ s16                          index;
    /* 0x04 */ const struct Ov003ShakeStep* steps;
    /* 0x08 */ s32                          offsetX;
    /* 0x0C */ s32                          offsetY;
} Ov003Shake; // Size: 0x10

// Each 12-byte step holds a duration and position. On rollover, the runtime reads
// the next step's position while advancing the current index.
typedef struct Ov003ShakeStep {
    /* 0x00 */ s16 duration;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s32 offsetX;
    /* 0x08 */ s32 offsetY;
} Ov003ShakeStep; // Size: 0xC

// ov003_02083300.c
void func_ov003_02083300(Ov003BgResState* state, s32 engine, BinIdentifier* iden);
void func_ov003_0208335c(Ov003BgResState* state, const Ov003ResCmd* script, void* arg);
void func_ov003_02083390(Ov003BgResState* state);
void func_ov003_020833cc(Ov003BgResState* state);
s32  func_ov003_02083470(void);
s32  func_ov003_02083478(Ov003BgResState* state);
s32  func_ov003_020835d4(Ov003BgResState* state);
s32  func_ov003_02083700(Ov003BgResState* state);
void func_ov003_0208380c(void);
void func_ov003_020838d8(void);

// ov003_02083950.c
/// Sets up the camera projection/position for one engine from a camera path entry.
void         func_ov003_02083a2c(s32, s32, s32, s32, s32);
void         func_ov003_02083ab0(s32, s32, s32, s32);
s32          func_ov003_02083af8(s32 x, s32 y, s32 marginX, s32 marginY);
s32          func_ov003_02083b8c(s32* value, s32 margin, s32 scale);
s32          func_ov003_02083bfc(s32*, s32, s32);
s32          func_ov003_02083c50(s32*, s32);
BOOL         func_ov003_02083ca0(s32*, s32*, s32, s32);
s32          func_ov003_02083ce4(s32* value, s32 margin);
void         func_ov003_02083d34(void);
void         func_ov003_02083e50(s32);
void         func_ov003_020842e0(void);
void         func_ov003_02084678(Ov003Collider*);
void         func_ov003_020846ac(Ov003Collider*, s32, s32, s32);
void         func_ov003_020846c0(Ov003Collider*, s32*, s32*, s32*);
void         func_ov003_020847f0(void);
void         func_ov003_0208481c(void);
void*        func_ov003_02084970(CombatList* list);
void*        func_ov003_02084998(CombatListNode*);
void         func_ov003_02084a08(CombatList* list, CombatListNode* before, CombatListNode* node, void* item);
void         func_ov003_02084b88(void);
void         func_ov003_02084bdc(void);
s32          func_ov003_02084d80(s32);
void         func_ov003_02084da0(s32 value);
s32          func_ov003_02084db0(void);
void         func_ov003_02085054(void);
void         func_ov003_0208508c(void);
void         func_ov003_02085c78(void);
BOOL         func_ov003_02085cfc(u16);
BOOL         func_ov003_02085d3c(s32);
BOOL         func_ov003_02085d60(CombatActor*);
CombatActor* func_ov003_02085e78(s32);
CombatActor* func_ov003_02085ec0(s32);
CombatActor* func_ov003_02086014(s32);
void         func_ov003_020860c4(void);
void         func_ov003_02086130(void);
void         func_ov003_020862a4(u16);
void         func_ov003_020862f8(s32, s32);

void         func_ov003_02084348(s32, s16*, s16*, s32, s32, s32);
s16          func_ov003_020843b0(s32, s32);
s16          func_ov003_020843ec(s32, s32, s32);
s32          func_ov003_0208442c(s32, s32);
s32          func_ov003_02084460(s32, s32, s32);
void         func_ov003_02084634(Ov003Collider*, s32, s32, s32);
void         func_ov003_02084694(Ov003Collider*, s32);
CombatList*  func_ov003_020846f0(s32);
void         func_ov003_020846fc(CombatList*, CombatList*);
void*        func_ov003_0208495c(CombatList*);
void*        func_ov003_02084984(CombatListNode*);
void         func_ov003_020849ac(CombatList*);
void         func_ov003_020849bc(CombatList*, CombatListNode*, void*);
void*        func_ov003_020849dc(CombatList*, CombatListNode*);
CombatActor* func_ov003_02085dc8(s32);

// ov003_020864c0.c
s32  func_ov003_02086404(s32 engine, CombatSprite* sprite, u32 id);
void func_ov003_020864c0(s32 arg0, CombatSprite* arg1, s32 arg2);
/// Plays the actor record's sound: the normal SE when `arg0 == 0`, the alternate one otherwise.
void func_ov003_02086540(s32 arg0, s32 arg1, s32 arg2);
void func_ov003_02086590(u16 id, s32 arg1, s32 arg2);
/// Spawns the battle effect task for actor record `id`, writing the task id through `out`.
s32  func_ov003_020865a0(u16 id, s32 arg1, s32 arg2, s32* out);
void func_ov003_02086654(Ov003EffectActor* effect);
void func_ov003_02086680(Ov003EffectActor* effect, Ov003EffectSpawn* spawn);
void func_ov003_020866f8(Ov003EffectActor* effect);
void func_ov003_02086770(Ov003EffectActor* effect);
void func_ov003_020867a0(void);
/// Picks the hit effect/SE combo for whatever the main engine's camera is currently showing.
void func_ov003_020867c0(void);
void func_ov003_020868c8(u16 value);
void func_ov003_0208697c(void);
void func_ov003_020869c8(s32 value);
u16  func_ov003_020869e4(void);
u16  func_ov003_02086a40(void);
s32  func_ov003_02086a64(void);
s32  func_ov003_02086a80(void);
void func_ov003_02086aa8(s32 value);
void func_ov003_02086ac4(s32 enabled);
void func_ov003_02086af0(void);
void func_ov003_02086b14(Ov003ChargeState* state);
void func_ov003_02086c04(u16* table, u32 count);
/// Refills the effect command's queue with a fresh shuffle.
void func_ov003_02086cc0(void);
u16  func_ov003_02086d24(void);
void func_ov003_02086d8c(CombatSprite* sprite, s32 animId, s32 flipped, s32 x, s16 y, s16 z);
void func_ov003_02086e38(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32  func_ov003_02086f7c(void);
s8   func_ov003_02086fec(void);
void func_ov003_02087008(void);
void func_ov003_020871cc(s32 arg0);
/// Pushes a battle actor towards `x`/`y`/`z` through the shared action-effect pool.
void               func_ov003_02087250(Ov003EffectState* state, CombatActor* target, s32 angle);
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
/// TRUE when the main display actor is far enough from the projected sub-screen position.
s32 func_ov003_02087704(void);
/// Plays one of the two hit-reaction SE sets, picked at random.
void func_ov003_02087804(s32 sub);
/// Moves the display actor of one engine to a new position and tells the HUD about it.
void func_ov003_02087b00(s32 mode, s32 x, s32 y, s32 z);

void func_ov003_020864dc(CombatActor*, u16);
s32  func_ov003_0208656c(void);
void func_ov003_02086c78(s32*, s16*);
s16* func_ov003_02086e94(void);
s32  func_ov003_02086f20(s32, s32);
void func_ov003_02087094(Ov003EffectCmd*, u8, s32);
void func_ov003_020871b0(Ov003EffectCmd*);
void func_ov003_0208722c(void);
// ov003_0208a488.c
void func_ov003_0208af9c(s32 time, s32 amount);
BOOL func_ov003_0208b1e8(u32 itemId);
void func_ov003_0208b278(u32 state);
void func_ov003_0208b2fc(s32* x, s32* y, s32 angle, s32 scale);
void func_ov003_0208b370(s16* screenX, s16* screenY, s32 x, s32 y, s32 z);
/// True for the two encounters that are allowed to take damage while the battle is paused.
BOOL             func_ov003_0208b690(u16 encounterId);
Ov003EnemyActor* func_ov003_0208c8c0(s32 x, s32 side, s32 airborne);
void             func_ov003_0208c970(s32 mode, s16 exp);
void             func_ov003_0208c9e0(s32 mode, s16 pp);
BOOL             func_ov003_0208ca04(s32 mode, u32 effect);
s16              func_ov003_0208ca24(s32 mode, u32 effect);
s32              func_ov003_0208ca54(s32 includePins);
s32              func_ov003_0208cae0(s32 includePins);
void             func_ov003_0208cb6c(s32 kind, s32 damage);
/// Bumps the battle's combo-length counter.
void func_ov003_0208cbac(void);
/// Extends the battle's longest combo.
void func_ov003_0208cbc8(void);

void             func_ov003_0208afcc(s32);
void             func_ov003_0208b3cc(s32, s16);
void             func_ov003_0208b518(s32, Ov003EnemyActor*, s16);
BOOL             func_ov003_0208c79c(Ov003EnemyActor*, s32);
Ov003EnemyActor* func_ov003_0208c810(s32, s32, s32);
void             func_ov003_0208ec74(BtlMgrWork*);

// ov003_020a30d4.c
void func_ov003_020a30d4(s32 id, s32 touchIndex, u32 slot);
s32  func_ov003_020a3340(s32 actionId);
s32  func_ov003_020a3350(s32 actionId);
s32  func_ov003_020a3478(s32 actionId);
s32  func_ov003_020a3488(s32 actionId);
s32  func_ov003_020a3498(CombatSprite* cSprite, s32 arg1, s32 arg2, s32 index, u16 arg4);
void func_ov003_020a35b8(CombatSprite* cSprite, s32 index, s32 arg2);
/// Drops every touch action actor of the battle.
void func_ov003_020a35d8(void);
/// Publishes the actor positions to the 3D layer's slot table.
void func_ov003_020a3608(void);
/// Queues one touch event for every equipped pin of the current deck (`arg0` selects the deck
/// of the active player).
void func_ov003_020a3728(s32 arg0);
/// Spawns the touch icon task and queues one action per equipped pin of the current deck.
void func_ov003_020a37a8(s32 arg0);
/// TRUE while at least one non-player actor is alive.
s32  func_ov003_020a3980(void);
void func_ov003_020a3a40(void);
/// Picks the actor under the touch/stylus and moves it there; SELECT resets the positions.
void func_ov003_020a3a80(void);

s32 func_ov003_020a3330(s32);
s32 func_ov003_020a3468(s32);
// ov003_020a3b9c.c
void func_ov003_020a3b9c(Ov003TouchActor* actor, const Ov003ActorSpawn* spawn);
void func_ov003_020a3d98(Ov003TouchActor* actor);
void func_ov003_020a3dac(Ov003TouchActor* actor);
s32  func_ov003_020a3dbc(Ov003TouchActor* actor);
/// TRUE while the actor can be pushed around by the other actors.
s32 func_ov003_020a3e50(Ov003TouchActor* actor);
/// TRUE while the actor's kind matches the current battle mode.
s32 func_ov003_020a3edc(Ov003TouchActor* actor);
/// TRUE when a touch is inside one of the actor's active touch areas.
s32 func_ov003_020a3f20(Ov003TouchActor* actor);
/// TRUE when the release point of the current touch is inside the actor.
s32  func_ov003_020a401c(Ov003TouchActor* actor);
void func_ov003_020a40b0(Ov003TouchActor* actor);
s16  func_ov003_020a40bc(Ov003TouchActor* actor);
s16  func_ov003_020a40cc(Ov003TouchActor* actor, s32 amount);
s32  func_ov003_020a410c(Ov003TouchActor* actor, s16 x, s16 y);
/// Pushes overlapping actors apart, then keeps every actor inside the touch area.
void func_ov003_020a4158(void);
/// Requests state `request` from the actor; a "revive" request while the actor is still fading
/// turns into the death state instead.
void               func_ov003_020a4390(Ov003TouchActor* actor, s32 request);
u32                func_ov003_020a43c8(Ov003TouchActor* actor);
void               func_ov003_020a45d4(Ov003TouchActor* actor);
const EquippedPin* func_ov003_020a4718(s32 deck, s32 slot);
/// Loads the raw pin record `pinId` into `out`.
s32 func_ov003_020a474c(u16 pinId, void* out);
/// The scaled duration of psych kind `params->unk_05` at level `level`.
s32 func_ov003_020a4778(Ov003PsychParams* params, s32 level);
/// Sets up the two sprite slots and the trail of the psych effect object.
void func_ov003_020a4a34(Ov003TrailEffect* effect);
/// Updates the psych effect trail; returns 0 when it is finished.
s32 func_ov003_020a4aac(Ov003TrailEffect* effect);
/// Renders the psych trail.
void func_ov003_020a4c8c(Ov003TrailEffect* effect);
void func_ov003_020a4d50(Ov003TrailEffect* effect);
/// Checks that the trail is long and straight enough to fire the psych.
s32 func_ov003_020a4d6c(Ov003TrailEffect* effect);
/// Binds camera `index` to the animation driven by `obj`.
void func_ov003_020a4e84(Ov003CameraAnim* obj, s32 index, s32 arg2);
void func_ov003_020a4ed0(Ov003CameraAnim* obj, s32 period, s32 offset);
/// Steps the camera shake animation; returns 1 when it hit a limit this frame.
s32  func_ov003_020a4ee8(Ov003CameraAnim* obj, s32 delta, s32 min, s32 max);
void func_ov003_020a4fd0(Ov003CameraAnim* obj);
/// Sets up the 3D layer the touch action sprites are rendered through.
void func_ov003_020a4ff0(void);

// BtlMic.c
void func_ov003_020a5080(void);
s32  func_ov003_020a50cc(void);
s32  func_ov003_020a51c0(void);

// ov003_020a5250.c
void func_ov003_020a5540(Ov003EffectState* effect, Ov003TouchActor* actor);
void func_ov003_020a563c(Ov003EffectState* effect);
s32  func_ov003_020a58c4(void);
s32  func_ov003_020a58d4(void);
void func_ov003_020a58e4(Ov003EffectState* effect, u16 arg1, u8 arg2);
/// Records the touch-slot origins used by the effect selection.
void func_ov003_020a593c(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_ov003_020a59dc(void);
u8   func_ov003_020a5a1c(void);
/// Finds the touch actor that owns the effect state at `effect`.
Ov003TouchActor* func_ov003_020a5a34(Ov003EffectState* effect);
void             func_ov003_020a5a94(Ov003EffectState* effect);
s32              func_ov003_020a5aa4(Ov003EffectState* effect);
s32              func_ov003_020a5ab4(Ov003EffectState* effect);
/// Sums the per-actor amount of every actor running effect `state`.
s32   func_ov003_020a5b18(s32 state);
void* func_ov003_020a5d2c(s32 size, const char* name);
/// Staggers the countdown timers of `count` sprite slots.
void func_ov003_020a5d88(Ov003AnimSprite* sprites, s32 count, s32 step);

// ov003_020a5db8.c
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

// ov003_020c2064.c
void func_ov003_020c2064(Ov003CutinTracks* tracks);
void func_ov003_020c20b8(Ov003CutinTracks* tracks, Vec* data, u16 count);
void func_ov003_020c20d4(Ov003CutinTracks* tracks, Ov003CutinActor* actor);
/// Starts cut-in `index` (0 = the default layout) and kicks off its actors.
void func_ov003_020c2a34(s32 index);
/// (Re)initialises the shared save-resident cut-in slot table.
void func_ov003_020c2af0(void);
u32  func_ov003_020c2b3c(s32 index);
void func_ov003_020c2b5c(Ov003CutinActor* actor);
void func_ov003_020c2b68(Ov003CutinActor* actor);
/// True while the actor is safely inside the camera bounds.
s32 func_ov003_020c2bbc(Ov003CutinActor* actor);
/// Three-phase approach/attack/leave state machine used by the normal cut-in attack.
s32 func_ov003_020c2c30(Ov003CutinActor* actor, s32 animBase, s32 targetX, s32 targetY, s32 targetZ, s32 arg5);
/// Projectile-like cut-in attack state machine (fly toward the target, then leave).
s32 func_ov003_020c2f68(Ov003CutinActor* actor, s32 animBase, s32 speed, s32 spread);
/// Third-actor entrance/exit state machine.
s32  func_ov003_020c3570(Ov003CutinActor* actor, u16 animBase);
void func_ov003_020c3700(Ov003EnemyActor* actor);
void func_ov003_020c37bc(Ov003EnemyActor* actor);
/// Fills an animation descriptor from a cut-in actor parameter table and loads it into a sprite.
void func_ov003_020c3814(s32 arg0, CombatSprite* sprite, s32 unused, const u8* table, u16 index, u16 arg4, u16 arg5, s16 arg6,
                         u16 arg7, s16 arg8);
s32  func_ov003_020c38b8(CombatSprite* sprite, u16 anim, s32 arg2);
s32  func_ov003_020c3920(CombatSprite* sprite);
/// Clamps `*value` to the camera's horizontal bounds.
s32 func_ov003_020c3960(s32* value, s32 margin);
/// Rotates a 2D offset so it points along the vector between the cut-in actor and its target.
void      func_ov003_020c39b0(Vec* src, Vec* dst);
TaskPool* func_ov003_020c3a10(u32 index);
void      func_ov003_020c3acc(s32 engine, void* arg1, s32 arg2);
void      func_ov003_020c3b1c(u16* tracks);
/// Returns the first index whose cumulative weight exceeds `target`.
s32       func_ov003_020c3b30(u16* weights, s32 count, s32 target);
void      func_ov003_020c3ba8(s32 x, s32 y);
s32       func_ov003_020c3bf0(void* actor);
TaskPool* func_ov003_020c3ca4(Ov003CutinActor* actor);
void      func_ov003_020c3ccc(Ov003CutinActor* actor);
void      func_ov003_020c3cdc(Ov003CutinActor* actor);
void      func_ov003_020c3e64(Ov003EnemyActor*);
void      func_ov003_020c3e88(Ov003CutinActor* actor, s32 arg1);

s32                  func_ov003_020c37f8(CombatSprite*);
s32                  func_ov003_020c38e8(CombatSprite*);
s32                  func_ov003_020c3908(CombatSprite*);
u32                  func_ov003_020c3aac(u32);
s32                  func_ov003_020c3c28(void);
void                 func_ov003_020c3c44(Ov003CutinScript*, void*, s16);
const BtlSpriteDesc* func_ov003_020c3c6c(Ov003EnemyActor*);
Ov003ActorParams*    func_ov003_020c3c88(Ov003EnemyActor*);
void func_ov003_020c3cec(const TaskHandle* mainHandle, const TaskHandle* subHandle, void* mainData, void* subData);
s32 func_ov003_020c3d28(const TaskHandle* subHandle, const TaskHandle* mainHandle, void* subData, void* mainData, s32* mainOut,
                        s32* subOut);
s32 func_ov003_020c3ec0(CombatActor*);
s32 func_ov003_020c3efc(Ov003EnemyActor*, Ov003CutinCmdData*);
// ov003_020c41e8.c
/// Resets a cut-in actor to its idle defaults (a fresh 0x1C0-byte actor block).
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
s32  func_ov003_020c5a00(Ov003CutinActor* actor);
s32  func_ov003_020c5a14(Ov003CutinActor* actor);
s32  func_ov003_020c5a28(Ov003CutinActor* actor);
s32  func_ov003_020c5a60(Ov003CutinActor* actor);
void func_ov003_020c5a98(Ov003CutinActor* actor);
void func_ov003_020c5ac0(s32 arg0, s32 arg1, Ov003CutinActor* actor, s32 arg3, s32 arg4, s32 arg5);
s32  func_ov003_020c5b0c(u16 id, void* owner, s32 arg2);
s32  func_ov003_020c5b64(u16 id, void* owner, s32 x, s32 y);
s32  func_ov003_020c5b8c(const Ov003AttackInfo* info, void* owner, s32 x, s32 y, s32 z);
s32  func_ov003_020c5ba4(u16 id, void* args, s32 arg2);
/// Third cut-in state machine: dash in, bounce and recover.
s32  func_ov003_020c6920(Ov003EnemyActor* actor, u16 anim, s32 arg2);
void func_ov003_020c6b9c(Ov003CutinActor* actor, u16 anim);
void func_ov003_020c6bbc(Ov003CutinActor* actor, u16 anim);
s32  func_ov003_020c6f40(Ov003EnemyActor*);
s32  func_ov003_020c6f68(Ov003EnemyActor*);
/// Drives the end-of-cut-in teardown task until the fade completes.
s32 func_ov003_020c6f90(Ov003EnemyActor* actor);

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
s32  func_ov003_020c5b2c(u16, void*, s32, s32, s32);
s32  func_ov003_020c5bc4(u16, Ov003AttackArgs*, s32, s32, s32);
s32  func_ov003_020c5bfc(Ov003EnemyActor*);
s32  func_ov003_020c5e98(Ov003EnemyActor*);
s32  func_ov003_020c6230(Ov003EnemyActor*);
s32  func_ov003_020c62c4(Ov003EnemyActor*, u16);
s32  func_ov003_020c65cc(Ov003EnemyActor*, u16);
s32  func_ov003_020c6b8c(Ov003EnemyActor*, u16);
s32  func_ov003_020c6bac(Ov003EnemyActor*, u16);
s32  func_ov003_020c6bc8(Ov003EnemyActor*, u16);
s32  func_ov003_020c6c2c(Ov003EnemyActor*, u16);
// BossExp1.c
s32 func_ov003_020c703c(Ov003EnemyActor*);
s32 func_ov003_020c7070(Ov003EnemyActor*);
s32 func_ov003_020c70a4(Ov003CutinActor* actor, s32 anim);
s32 func_ov003_020c72b4(Ov003EnemyActor*, s32, s32);
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
// ov003_02087b7c.c
s32                    func_ov003_02087c28(Ov003AttackCmd* cmd, Ov003EffectState* effect);
s32                    func_ov003_02087c98(Ov003AttackCmd* cmd, Ov003EffectState* effect);
s32                    func_ov003_02087d1c(Ov003AttackCmd* cmd, Ov003EffectState* effect);
s32                    func_ov003_02087e5c(Ov003AttackCmd* cmd, Ov003EffectState* effect);
void                   func_ov003_02087ed8(SndMgrSeIdx);
void                   func_ov003_02087f28(SndMgrSeIdx, s32);
void                   func_ov003_02087f64(SndMgrSeIdx, s32);
void                   func_ov003_02087fe0(s32, CombatSprite*, s32);
void                   func_ov003_02088064(CombatSprite*, s32, s32);
void                   func_ov003_020880e4(s32, CombatSprite*, Ov003EnemyActor*);
void                   func_ov003_0208810c(CombatSprite*, CombatActor*);
s32                    func_ov003_02088130(void);
s32                    func_ov003_0208a028(s32 mode, Ov003EnemyActor* unit, u16 attackId);
s32                    func_ov003_0208a08c(s32, Ov003EnemyActor*, s32);
const Ov003AttackInfo* func_ov003_0208a114(u16);
s32                    func_ov003_0208a128(const Ov003AttackInfo*, void*, void*, s32);
s32                    func_ov003_0208a164(const Ov003AttackInfo*, Ov003AttackArgs*, s32, s32, s32);
s32                    func_ov003_0208a1a4(const Ov003AttackInfo*, void*, s32, s32);
s32                    func_ov003_0208a1e8(const Ov003AttackInfo*, void*, s32);
void                   func_ov003_0208a234(s32 arg0, const Ov003AttackArgs* args, Ov003EnemyActor* unit);
s32                    func_ov003_0208a25c(CombatActor* actor);

// ov003_020d36fc.c
void func_ov003_020d3858(s32 engine, Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, void* paletteSrc, u16 palIdx, u16 flip);
s32  func_ov003_020d38ac(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script);
void func_ov003_020d3978(Ov003EnemyActor* owner, const Ov003SpriteLoadCmd* cmd, Ov003AnimScript* script,
                         const Ov003AnimScriptStep* steps, u16 loop);
s32  func_ov003_020d39ac(Ov003AnimScript* script);
void func_ov003_020d3b6c(Ov003AnimLayer*, Data*, s16, u32, u32, u32);
void func_ov003_020d3bec(Ov003AnimLayer*, BinIdentifier*, s32, void**, const Ov003AnimFrame*);
void func_ov003_020d3c2c(Ov003AnimLayer*, const Ov003AnimFrame*);
void func_ov003_020d3c98(Ov003AnimLayer*, Ov003AnimLayer*);
void func_ov003_020d3cac(Ov003AnimLayer*);
void func_ov003_020d3cf8(Ov003AnimLayer*);
PaletteResource* func_ov003_020d3dc0(Ov003SpriteSlot* slot);
void func_ov003_020d3eb4(Ov003SpriteSlot* slot, BinIdentifier* iden, const Ov003SpriteAnim* anims, void* charDest, u32 arg4,
                         u32 arg5);
void func_ov003_020d3f44(Ov003SpriteSlot* slot, PaletteResource* palette, BinIdentifier* iden, u32 engine, u8 palStart,
                         u8 palCount);
void func_ov003_020d3fb0(Ov003SpriteSlot* slot, const Ov003SpriteAnim* anims);
void func_ov003_020d3fd8(Ov003SpriteSlot* slot);
void func_ov003_020d40fc(Ov003SpriteSlot* slot);
void func_ov003_020d415c(Ov003Shake* shake, const Ov003ShakeStep* steps);
void func_ov003_020d4180(Ov003Shake* shake);
void func_ov003_020d41f0(Ov003Shake* shake, CombatActor* actor);
void func_ov003_020d4220(Ov003Shake* shake);

void func_0202fbf4(s32 arg0);
void func_ov005_020e740c(s32, s32);
s32  func_ov005_020e74fc(void* queue, s32 capacity, s32 arg2);
void func_ov005_020eb29c(s32, s32, s32);

// BtlBadge_ConditionBoot.c
void func_ov003_020a6f90(Ov003TouchActor* actor, s32 arg1);

#endif // COMBAT_CORE_COMBATAPI_H
