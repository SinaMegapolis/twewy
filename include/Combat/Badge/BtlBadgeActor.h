#ifndef TWEWY_COMBAT_BADGE_BTLBADGEACTOR_H
#define TWEWY_COMBAT_BADGE_BTLBADGEACTOR_H

#include "Combat/Core/Combat.h"
#include "Player/Inventory/Pins.h"

void BtlBadgeActor_Init(Ov003TouchActor* actor, const Ov003ActorSpawn* spawn);
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
s32  BtlBadgeActor_HitTest(Ov003TouchActor* actor, s16 x, s16 y);
/// Pushes overlapping actors apart, then keeps every actor inside the touch area.
void BtlBadgeActor_SeparateAll(void);
/// Requests state `request` from the actor; a "revive" request while the actor is still fading
/// turns into the death state instead.
void               BtlBadgeActor_Request(Ov003TouchActor* actor, s32 request);
u32                BtlBadgeActor_RunRequest(Ov003TouchActor* actor);
void               BtlBadgeActor_PickRequest(Ov003TouchActor* actor);
const EquippedPin* BtlBadgeActor_GetEquippedPin(s32 deck, s32 slot);
/// Loads the raw pin record `pinId` into `out`.
s32 BtlBadgeActor_LoadPinData(u16 pinId, void* out);
/// The scaled duration of psych kind `params->unk_05` at level `level`.
s32 func_ov003_020a4778(Ov003PsychParams* params, s32 level);

#endif // TWEWY_COMBAT_BADGE_BTLBADGEACTOR_H
