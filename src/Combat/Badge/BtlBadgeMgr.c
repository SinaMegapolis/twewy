#include "Combat/Badge/BtlBadgeMgr.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatBgRes.h"
#include "Combat/Core/CombatTouch.h"
#include "EasyFade.h"
#include "Engine/Color.h"
#include "Engine/Core/DMA.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/EasyTask.h"
#include "Engine/File/BinMgr.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/Input.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Math/Random.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Engine/Resources/PaletteMgr.h"
#include "Engine/Resources/ResourceMgr.h"
#include "Save.h"
#include "common_data.h"
#include <cri/adxt.h>
#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi/cpumem.h>

/// The start position of every badge slot (owned by arm9).
extern const u8 data_0205c19d[];
/// The badge cursor task.
extern const TaskHandle data_ov003_020d7d24;

static void BtlBadgeMgr_UpdateAll(void);

/// Drops every badge of the battle.
void BtlBadgeMgr_Clear(void) {
    g_CombatGlobal->touchActorCount = 0;
    CombatList_Init(&g_CombatGlobal->unk3D8BC);
}

/// Publishes the actor positions to the 3D layer's slot table.
void BtlBadgeMgr_PublishSlots(void) {
    Ov003TouchActor*     actor;
    Ov003TouchSlotTable* table = &gSaveData.battleTouchSlots;

    actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC);
    MI_CpuSet(table->kind, 0, 6);
    if (actor != NULL && actor->kind == 0xFFFE) {
        table->playerX = actor->x >> 12;
        table->playerY = actor->y >> 12;
        actor          = CombatList_GetNext(&actor->node);
    }
    if (actor == NULL) {
        return;
    }
    do {
        if (!(actor->effect.state == 0x3C && actor->state == 3)) {
            table->x[actor->slot] = actor->x >> 12;
            table->y[actor->slot] = actor->y >> 12;
        }
        if (g_CombatGlobal->unk3D890 != 0xD) {
            table->kind[actor->slot] = 0;
        } else {
            table->kind[actor->slot]   = actor->state;
            table->unk_14[actor->slot] = actor->unk_044;
            table->unk_20[actor->slot] = actor->unk_048;
        }
        actor = CombatList_GetNext(&actor->node);
    } while (actor != NULL);
}

/// Queues one queued ability for every equipped pin of the current deck (`arg0` selects the deck
/// of the active player).
void BtlBadgeMgr_QueueAbilities(s32 arg0) {
    RawPinData pin;
    u16        i;

    for (i = 0; i < 6; i++) {
        u16 pinId = BtlBadgeActor_GetEquippedPin(arg0, i)->pinID;

        if (pinId != 0xFFFF) {
            BtlBadgeActor_LoadPinData(pinId, &pin);
            if (func_ov003_020a6cc4(pin.brand) == 0 && pin.abilityId != 0xFF) {
                CombatAbility_Queue(pin.abilityId, -1, i);
            }
        }
    }
}

/// Spawns the touch icon task and queues one action per equipped pin of the current deck.
// Nonmatching: the psych-id flag switch lowers to a two-level table in the target.
void BtlBadgeMgr_SpawnBadges(s32 arg0) {
    Ov003PinAction action;
    RawPinData     pin;
    u16            i;

    EasyTask_CreateTask(&g_CombatGlobal->unk_10118, &data_ov003_020d7d24, NULL, 0, NULL, NULL);
    for (i = 0; i < 6; i++) {
        u16                pinId = BtlBadgeActor_GetEquippedPin(arg0, i)->pinID;
        const EquippedPin* equipped;

        if (pinId == 0xFFFF) {
            continue;
        }
        equipped = BtlBadgeActor_GetEquippedPin(arg0, i);
        BtlBadgeActor_LoadPinData(pinId, &pin);
        action.pinId  = pinId;
        action.level  = equipped->flags.bits.level;
        action.unk_04 = equipped->unk_08;
        action.pin    = &pin;
        action.slot   = i;
        switch (pin.durationType) {
            case 2:
                func_ov003_020a7e68(&action);
                break;
            case 1:
            case 3:
                func_ov003_020a7640(&action);
                break;
            default:
                func_ov003_020a6d24(&action);
                break;
        }
        switch (pin.psychId) {
            case 9:
            case 14:
            case 18:
            case 40:
                g_CombatGlobal->touchWork.flags |= 2;
                break;
            case 15:
            case 17:
            case 26:
                g_CombatGlobal->touchWork.flags |= 0x20;
                break;
        }
    }
    BtlBadgeMgr_UpdateAll();
}

/// Runs one update of every badge.
static void BtlBadgeMgr_UpdateAll(void) {
    Ov003TouchActor* actor;

    actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC);
    BtlBadgeActor_SeparateAll();
    while (actor != NULL) {
        if (actor->kind != 0xFFFE) {
            BtlBadgeActor_PickRequest(actor);
        }
        actor = CombatList_GetNext(&actor->node);
    }
}

/// TRUE while at least one non-player actor is alive.
s32 BtlBadgeMgr_AnyActive(void) {
    Ov003TouchActor* actor;

    for (actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); actor != NULL; actor = CombatList_GetNext(&actor->node)) {
        if (actor->kind != 0xFFFE && actor->unk_00E != 2) {
            return 1;
        }
    }
    return 0;
}

/// Snaps every actor back onto its starting slot position.
static void BtlBadgeMgr_ResetPositions(void) {
    Ov003TouchActor* actor;

    for (actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); actor != NULL; actor = CombatList_GetNext(&actor->node)) {
        actor->x = data_0205c19d[actor->slot * 2] << 12;
        actor->y = data_0205c19d[actor->slot * 2 + 1] << 12;
    }
}

/// Clears the "actor is being dragged" flag of every actor.
void BtlBadgeMgr_ClearDrag(void) {
    Ov003TouchActor* actor;

    for (actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); actor != NULL; actor = CombatList_GetNext(&actor->node)) {
        actor->unk_04C = 0;
    }
}

/// Picks the actor under the touch/stylus and moves it there; SELECT resets the positions.
void BtlBadgeMgr_DebugDrag(void) {
    Ov003TouchActor* actor;
    Ov003TouchActor* selected = NULL;

    if (InputStatus.buttonState.pressedButtons & INPUT_BUTTON_SELECT) {
        BtlBadgeMgr_ResetPositions();
        return;
    }
    for (actor = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); actor != NULL; actor = CombatList_GetNext(&actor->node)) {
        s32 active = func_ov003_020a3e50(actor);

        if (actor->unk_04C != 0) {
            if (active != 0 && CombatTouch_IsGesture(6) != 0) {
                selected = actor;
            } else {
                actor->unk_04C = 0;
            }
        }
        if (selected == NULL && CombatTouch_IsGesture(6) != 0 && active != 0) {
            if (BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y) != 0) {
                selected       = actor;
                actor->unk_04C = 1;
            }
        }
    }
    if (selected != NULL) {
        selected->x = g_CombatGlobal->touchWork.x << 12;
        selected->y = g_CombatGlobal->touchWork.y << 12;
    }
    BtlBadgeActor_SeparateAll();
}
