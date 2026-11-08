#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"

static char Path_AbilityData[0x18] = "Data/AbilityData.bin";
static char Path_ItemData[0x18]    = "Apl_Tak/ItemData.bin";

const u32 data_ov003_020da738 = 4;

static const BinIdentifier Iden_AbilityData = {3, (char*)&Path_AbilityData};

const BinIdentifier Iden_ItemData = {3, (char*)&Path_ItemData};

/// Queues a queued ability for ability `id` unless the ability cannot be used by `touchIndex`
/// (a value of -1 or 0..2 selects "any touch" or one specific touch).
void CombatAbility_Queue(s32 id, s32 touchIndex, u32 slot) {
    u32              anyTouch = 1;
    Ov003TouchEvent* events;
    Ov003AbilityData ability;
    s32              i;

    events = g_CombatGlobal->touchEvents;
    func_020274ac(&ability, id, &Iden_AbilityData);
    if (touchIndex >= 0 && touchIndex <= 2) {
        anyTouch = 0;
        if (ability.flags.usable[touchIndex] == 0) {
            return;
        }
    } else if (ability.flags.valid == 0) {
        return;
    }
    for (i = 0; i < 0xE; i++) {
        if (events[i].unk_00 == 0xFF) {
            events[i].unk_00   = id;
            events[i].anyTouch = anyTouch;
            events[i].ability  = ability;
            events[i].slot     = slot;
            return;
        }
    }
}

/// TRUE when a queued ability queued for `touchMode` (0 = any touch, 1 = any-queued abilities,
/// 2 = single-queued abilities) applies to them.
static s32 CombatAbility_MatchesOwner(s32 touchMode, s32 anyTouch) {
    switch (touchMode) {
        case 0:
            break;
        case 1:
            if (anyTouch == 0) {
                return 0;
            }
            break;
        case 2:
            if (anyTouch != 0) {
                return 0;
            }
            break;
    }
    return 1;
}

/// TRUE unless the actor of touch slot `slot` is in a state that cannot accept a touch.
// Nonmatching: mwcc range-folds the `state == 4 || state == 5` test instead of the cmp/cmpne chain.
static s32 CombatAbility_IsSourceActive(s32 slot) {
    Ov003TouchActor* actor;

    if (slot == -1) {
        return 1;
    }
    for (actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); actor != NULL; actor = CombatList_GetNext(&actor->node)) {
        if (actor->slot != slot) {
            continue;
        }
        if (actor->state == 4 || actor->state == 5) {
            return 0;
        }
        if (actor->state == 0 && (actor->request == 4 || actor->request == 6)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

/// TRUE when `actionId` is one of the actions a queued ability grants.
static s32 CombatAbility_IsActiveFor(s32 actionId, s32 touchMode) {
    Ov003TouchEvent*        events = g_CombatGlobal->touchEvents;
    const Ov003AbilityData* ability;
    s32                     i;
    s32                     j;

    for (i = 0; i < 0xE; i++) {
        Ov003TouchEvent* event = &events[i];

        if (event->unk_00 == 0xFF || CombatAbility_MatchesOwner(touchMode, event->anyTouch) == 0 ||
            CombatAbility_IsSourceActive(event->slot) == 0)
        {
            continue;
        }
        ability = &event->ability;
        for (j = 0; j < 4; j++) {
            if (actionId == ability->slots[j].actionId) {
                return 1;
            }
        }
    }
    return 0;
}

s32 CombatAbility_IsActive(s32 actionId) {
    return CombatAbility_IsActiveFor(actionId, 0);
}

s32 CombatAbility_IsActiveForPlayer(s32 actionId) {
    return CombatAbility_IsActiveFor(actionId, 1);
}

s32 CombatAbility_IsActiveForPartner(s32 actionId) {
    return CombatAbility_IsActiveFor(actionId, 2);
}

/// The amount of `actionId` a queued ability grants: most actions add the recorded amount,
/// but a few (the multi-hit ones) report their per-action value directly.
static s32 CombatAbility_GetValueFor(s32 actionId, s32 touchMode) {
    Ov003TouchEvent*        events = g_CombatGlobal->touchEvents;
    const Ov003AbilityData* ability;
    s32                     i;
    s32                     j;
    s32                     total = 0;

    for (i = 0; i < 0xE; i++) {
        Ov003TouchEvent* event = &events[i];

        if (event->unk_00 == 0xFF || CombatAbility_MatchesOwner(touchMode, event->anyTouch) == 0 ||
            CombatAbility_IsSourceActive(event->slot) == 0)
        {
            continue;
        }
        ability = &event->ability;
        for (j = 0; j < 4; j++) {
            if (actionId != ability->slots[j].actionId) {
                continue;
            }
            switch (actionId) {
                case 6:
                case 10:
                case 11:
                case 15:
                case 33:
                    return ability->slots[j].amount;
                default:
                    total += ability->slots[j].amount;
                    break;
            }
        }
    }
    return total;
}

s32 CombatAbility_GetValue(s32 actionId) {
    return CombatAbility_GetValueFor(actionId, 0);
}

s32 CombatAbility_GetPlayerValue(s32 actionId) {
    return CombatAbility_GetValueFor(actionId, 1);
}

s32 CombatAbility_GetPartnerValue(s32 actionId) {
    return CombatAbility_GetValueFor(actionId, 2);
}
