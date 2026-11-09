#include "Combat/Badge/BtlBadgeActor.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatActor.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatMic.h"
#include "Combat/Core/CombatSprite.h"
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
#include "SndMgr.h"
#include "common_data.h"

#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/math.h>
#include <nitro/mi/cpumem.h>

// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------

/// The 0x3C-byte effect block at 0xCC, driven by `CombatPsych.c`.

/// The psych trail effect object: a point trail plus the two sprites it draws.

/// Defined below; the state machine calls itself through these.
static s32                func_ov003_020a4908(Ov003PsychParams*, u16);
static s32                func_ov003_020a49b4(Ov003PsychParams*, u16);
static s32                func_ov003_020a49c8(Ov003PsychParams*, u16);
static const EquippedPin* BtlBadgeActor_GetDeckLayout(s32 deck);

// ---------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------

extern const BinIdentifier data_ov003_020dc044;
static const EquippedPin   data_ov003_020dc084[6];
static const EquippedPin   data_ov003_020dc0c0[6];
static const EquippedPin   data_ov003_020dc0fc[6];
static const EquippedPin   data_ov003_020dc138[6];
static const EquippedPin   data_ov003_020dc174[6];
static const EquippedPin   data_ov003_020dc1b0[6];
static const EquippedPin   data_ov003_020dc1ec[6];
static char                data_ov003_020e4868[0x14] = "Data/BadgeData.bin";

static char data_ov003_020e487c[0x18] = "Apl_Tak/Grp_Badge.bin";

static char                data_ov003_020e4894[0x1C] = "Apl_Hir/Grp_BtlBadge.bin";
static const BinIdentifier data_ov003_020dc03c       = {3, (char*)&data_ov003_020e4868};

const BinIdentifier data_ov003_020dc044 = {3, (char*)&data_ov003_020e4894};

static const BinIdentifier data_ov003_020dc04c = {3, (char*)&data_ov003_020e487c};

static const EquippedPin* const data_ov003_020dc054[12] = {
    data_ov003_020dc0c0, data_ov003_020dc174, data_ov003_020dc174, data_ov003_020dc174,
    data_ov003_020dc174, data_ov003_020dc174, data_ov003_020dc138, data_ov003_020dc0fc,
    data_ov003_020dc1b0, data_ov003_020dc0c0, data_ov003_020dc084, data_ov003_020dc1ec,
};

static const EquippedPin data_ov003_020dc084[6] = {
    {0x0105, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc0c0[6] = {
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc0fc[6] = {
    {0x010E, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0x010C, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc138[6] = {
    {0x010A, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc174[6] = {
    {0x0109, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc1b0[6] = {
    {0x0109, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0x010D, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

static const EquippedPin data_ov003_020dc1ec[6] = {
    {0x0022, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0x0063, 0x0000, 0x0000, 0x0000, 0x00, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
    {0xFFFF, 0x0000, 0x0000, 0x0000, 0x01, {.raw = 0x01}},
};

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Builds the badge described by `spawn` and links it into the actor list.
// Nonmatching: field store scheduling after CombatSprite_InitAnim.
void BtlBadgeActor_Init(Ov003TouchActor* actor, const Ov003ActorSpawn* spawn) {
    SpriteAnimationEx anim;
    s32               angle;

    actor->state   = 0;
    actor->request = 0;
    actor->slot    = spawn->slot;
    actor->kind    = spawn->kind;
    actor->unk_00C = spawn->unk_02;
    actor->unk_00E = spawn->unk_04;
    actor->params  = *spawn->params;
    actor->unk_044 = 0;
    actor->unk_046 = 0;
    actor->unk_04C = 0;
    actor->unk_050 = 0;
    actor->unk_052 = 0;
    actor->unk_054 = 0;
    CombatSprite_InitAnim(&anim.anim, 2, &data_ov003_020dc04c);
    anim.anim.bits_10_11 = 0;
    anim.anim.unk_1C     = 1;
    anim.anim.animIndex  = 1;
    anim.anim.unk_28     = 3;
    anim.anim.unk_20     = 4;
    anim.animTable       = NULL;
    anim.anim.unk_18     = 2;
    anim.anim.unk_26     = 2;
    anim.anim.packIndex  = actor->params.head.unk_00 + 1;
    CombatSprite_Load(&actor->sprite, &anim);
    func_ov003_02082738(&actor->sprite, 0);
    func_ov003_02082730(&actor->sprite, 0x3E8000);
    actor->radius    = 0x10;
    actor->x         = gSaveData.battleTouchSlots.x[actor->slot] << 12;
    actor->y         = gSaveData.battleTouchSlots.y[actor->slot] << 12;
    angle            = (RNG_Next(0x10000) >> 4) * 2;
    actor->velocityX = FX_Mul(FX_SinCosTable_[angle + 1], FX32_ONE);
    actor->velocityY = FX_Mul(FX_SinCosTable_[angle], FX32_ONE);
    func_ov003_020a58e4(&actor->effect, actor->params.head.unk_02, actor->params.unk_04);
    func_ov003_020a7284(actor);
    func_ov003_020a7ba8(actor);
    g_CombatGlobal->touchActorCount++;
    CombatList_Append(&g_CombatGlobal->unk3D8BC, &actor->node, actor);
}

void func_ov003_020a3d98(Ov003TouchActor* actor) {
    func_ov003_020a5540(&actor->effect, actor);
}

void func_ov003_020a3dac(Ov003TouchActor* actor) {
    func_ov003_020a563c(&actor->effect);
}

/// TRUE while the actor is allowed to run its action.
// Nonmatching: flag reload and register allocation.
s32 func_ov003_020a3dbc(Ov003TouchActor* actor) {
    s32 waiting;

    if (CombatDebug_GetSetting(0) == 1) {
        return 0;
    }
    waiting = g_CombatGlobal->unk3D878 & 6;
    if (waiting != 0 || g_CombatGlobal->unk3D884 != 0) {
        return 0;
    }
    if (actor->kind == 0xFFFE) {
        return actor->unk_044 > 0;
    }
    if (actor->state == 1 || actor->state == 2 || actor->state == 4) {
        return 0;
    }
    return 1;
}

/// TRUE while the actor can be pushed around by the other actors.
// Nonmatching: flag reload and register allocation.
s32 func_ov003_020a3e50(Ov003TouchActor* actor) {
    s32 waiting;

    if (CombatDebug_GetSetting(0) == 1) {
        return 0;
    }
    waiting = g_CombatGlobal->unk3D878 & 6;
    if (waiting != 0 || g_CombatGlobal->unk3D884 != 0) {
        return 0;
    }
    if (actor->kind == 0xFFFE) {
        return actor->unk_044 > 0;
    }
    return actor->state != 4;
}

/// TRUE while the actor's kind matches the current battle mode.
// Nonmatching: load of the battle-mode flag is not hoisted out of the branches the same way.
s32 func_ov003_020a3edc(Ov003TouchActor* actor) {
    u8  mode = actor->unk_00E;
    s32 flag = g_CombatGlobal->unk3D880;

    if (mode == 0) {
        return flag == 0;
    }
    if (mode == 1) {
        return flag;
    }
    return 1;
}

/// TRUE when a touch is inside one of the actor's active touch areas.
s32 func_ov003_020a3f20(Ov003TouchActor* actor) {
    if (CombatTouch_IsGesture(2) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y) != 0)
    {
        return 1;
    }
    if (CombatTouch_IsGesture(6) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.startX, g_CombatGlobal->touchWork.startY) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y) != 0)
    {
        return 1;
    }
    if (CombatTouch_IsGesture(3) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.endX, g_CombatGlobal->touchWork.endY) != 0)
    {
        return 1;
    }
    return 0;
}

/// TRUE when the release point of the current touch is inside the actor.
s32 func_ov003_020a401c(Ov003TouchActor* actor) {
    if (CombatTouch_IsGesture(2) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y) != 0)
    {
        return 1;
    }
    if (CombatTouch_IsGesture(3) != 0 &&
        BtlBadgeActor_HitTest(actor, g_CombatGlobal->touchWork.endX, g_CombatGlobal->touchWork.endY) != 0)
    {
        return 1;
    }
    return 0;
}

void func_ov003_020a40b0(Ov003TouchActor* actor) {
    actor->unk_050 = 0xF;
}

s16 func_ov003_020a40bc(Ov003TouchActor* actor) {
    return func_ov003_020a40cc(actor, 1);
}

/// Ticks the actor's lifetime counter down and returns it.
// Nonmatching: the s16 lifetime clamp is narrowed in a different order.
s16 func_ov003_020a40cc(Ov003TouchActor* actor, s32 amount) {
    if (CombatDebug_GetSetting(8) == 0 && actor->params.unk_12 != 0) {
        s16 value = actor->unk_044 - amount;

        if (value < 0) {
            value = 0;
        }
        actor->unk_044 = value;
    }
    return actor->unk_044;
}

/// TRUE when the screen point (x, y) is inside the actor's touch rectangle.
s32 BtlBadgeActor_HitTest(Ov003TouchActor* actor, s16 x, s16 y) {
    s16 radius = actor->radius;
    s32 posX   = actor->x >> 12;
    s32 posY   = actor->y >> 12;

    if (x >= posX - radius && x <= posX + radius && y >= posY - radius && y <= posY + radius) {
        return 1;
    }
    return 0;
}

/// Pushes overlapping actors apart, then keeps every actor inside the touch area.
// Nonmatching: pairwise push-back loop: bookkeeping registers and branch layout.
void BtlBadgeActor_SeparateAll(void) {
    Ov003TouchActor* a;
    Ov003TouchActor* b;

    for (a = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); a != NULL; a = CombatList_GetNext(&a->node)) {
        s32 aActive = func_ov003_020a3e50(a);

        for (b = CombatList_GetTail(&g_CombatGlobal->unk3D8BC); b != NULL; b = CombatList_GetPrev(&b->node)) {
            s32 bActive = func_ov003_020a3e50(b);
            s32 radius;
            s32 dx;
            s32 dy;
            s32 push;

            if (a == b) {
                continue;
            }
            if (aActive == 0 || bActive == 0) {
                continue;
            }
            radius = (a->radius + b->radius) - 6;
            dx     = a->x - b->x;
            dy     = a->y - b->y;
            if (ABS(dx) >= radius << 12 || ABS(dy) >= radius << 12) {
                continue;
            }
            push = (radius << 12) - FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy));
            if (push > 0) {
                u16 angle = FX_Atan2Idx(dy, dx);
                s32 pushX = FX_Mul(FX_CosIdx(angle), push);
                s32 pushY = FX_Mul(FX_SinIdx(angle), push);

                a->x += pushX;
                a->y += pushY;
                b->x -= pushX;
                b->y -= pushY;
            }
        }
    }
    for (a = CombatList_GetHead(&g_CombatGlobal->unk3D8BC); a != NULL; a = CombatList_GetNext(&a->node)) {
        s32 radius;

        if (func_ov003_020a3e50(a) == 0) {
            continue;
        }
        radius = a->radius;
        if (a->x < radius << 12) {
            a->x = radius << 12;
        } else if (a->x > (0x100 - radius) << 12) {
            a->x = (0x100 - radius) << 12;
        }
        if (a->y < radius << 12) {
            a->y = radius << 12;
        } else if (a->y > (0xC0 - radius) << 12) {
            a->y = (0xC0 - radius) << 12;
        }
    }
}

/// Requests state `request` from the actor; a "revive" request while the actor is still fading
/// turns into the death state instead.
void BtlBadgeActor_Request(Ov003TouchActor* actor, s32 request) {
    if (request == 5) {
        BtlBadgeActor_PickRequest(actor);
        CombatSe_Play(SEIDX_SE_PSY_FULL);
        return;
    }
    if (request == 2 && actor->params.unk_1E == 0) {
        actor->request = 6;
        return;
    }
    actor->request = request;
}

/// Runs the pending actor request and returns it (0 = none).
// Nonmatching: request dispatch: per-case reloads of the psych parameters.
u32 BtlBadgeActor_RunRequest(Ov003TouchActor* actor) {
    u32                  request = actor->request;
    Ov003TouchSlotTable* slots   = &gSaveData.battleTouchSlots;

    switch (request) {
        case 1:
            if (actor->params.unk_18 != 0) {
                if (actor->params.unk_18 == 1) {
                    s16 value = func_ov003_020a49b4(&actor->params, actor->unk_00C);

                    func_ov003_020a8300(actor, value, value);
                } else {
                    ((void (*)())func_ov003_020a6f90)(actor);
                }
            }
            actor->state = 1;
            break;
        case 8:
            if (actor->params.unk_18 != 0) {
                if (actor->params.unk_18 == 1) {
                    func_ov003_020a8300(actor, slots->unk_20[actor->slot],
                                        func_ov003_020a49b4(&actor->params, actor->unk_00C));
                } else {
                    ((void (*)())func_ov003_020a6f90)(actor);
                }
            }
            actor->state = 1;
            break;
        case 2:
            if (actor->params.unk_1E == 1) {
                s16 value = func_ov003_020a49c8(&actor->params, actor->unk_00C);

                func_ov003_020a8300(actor, value, value);
            }
            actor->state = 2;
            break;
        case 9:
            if (actor->params.unk_1E == 1) {
                func_ov003_020a8300(actor, slots->unk_20[actor->slot], func_ov003_020a49c8(&actor->params, actor->unk_00C));
            }
            actor->state = 2;
            break;
        case 3:
            actor->unk_046 = func_ov003_020a4908(&actor->params, actor->unk_00C);
            actor->unk_044 = actor->unk_046;
            actor->state   = 3;
            break;
        case 7:
            actor->unk_046 = func_ov003_020a4908(&actor->params, actor->unk_00C);
            actor->unk_044 = slots->unk_14[actor->slot];
            actor->state   = 3;
            break;
        case 4:
            actor->state = 4;
            break;
        case 6:
            actor->state = 5;
            break;
    }
    actor->request = 0;
    return request;
}

/// TRUE while both of the partner's psych actions are idle.
static s32 func_ov003_020a4594(void) {
    if (CombatAbility_IsActive(0x46) != 0 && CombatAbility_IsActive(0x47) != 0 && CombatAbility_IsActive(0x48) != 0) {
        return 1;
    }
    return 0;
}

/// Picks the next state request for the actor from the current battle state.
void BtlBadgeActor_PickRequest(Ov003TouchActor* actor) {
    u16 value = actor->params.unk_18;

    switch (value) {
        case 2:
            actor->request = 1;
            break;
        case 0:
            actor->request = 3;
            break;
        case 1:
            if (func_ov003_020a4594() != 0 || func_ov003_020a49b4(&actor->params, actor->unk_00C) == 0) {
                actor->request = 3;
            } else {
                actor->request = 1;
            }
            break;
    }
    if ((gSaveData.unk_3142 > 1) && g_CombatGlobal->unk3D890 == 0xD) {
        switch (gSaveData.battleTouchSlots.kind[actor->slot]) {
            case 3:
                actor->request = 7;
                break;
            case 1:
                actor->request = 8;
                break;
            case 2:
                actor->request = 9;
                break;
            case 5:
                actor->request = 6;
                break;
        }
    }
    if (g_CombatGlobal->unk3D88D == 5) {
        actor->request = 6;
    }
    if (func_ov003_020a6cc4(actor->params.unk_05) != 0) {
        actor->request = 6;
    }
    if (CombatDebug_GetSetting(4) == 0x3B) {
        actor->request = 6;
    }
}

/// The equipped pin of deck slot `slot` for the player `deck`.
const EquippedPin* BtlBadgeActor_GetEquippedPin(s32 deck, s32 slot) {
    if (deck == 0xD) {
        return &gSaveData.equippedPins[slot];
    }
    return &BtlBadgeActor_GetDeckLayout(deck)[slot];
}

/// Loads the raw pin record `pinId` into `out`.
s32 BtlBadgeActor_LoadPinData(u16 pinId, void* out) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, out, 0x34, &data_ov003_020dc03c, pinId * 0x34));
    return 1;
}

/// The scaled duration of psych kind `params->unk_05` at level `level`.
// Nonmatching: duration scaling: FX_Divide/FX_Mul operand order.
s32 func_ov003_020a4778(Ov003PsychParams* params, s32 level) {
    s32 value = (params->unk_10 * (level - 1)) + params->unk_0E;
    u8  kind  = params->unk_05;

    if (kind < 0xD && CombatAbility_IsActive(kind + 0x24) != 0) {
        s32 scale = FX_Divide(CombatAbility_GetValue(kind + 0x24) << 12, 0x64000) + 0x1000;

        value = FX_Mul(value << 12, scale) >> 12;
    }
    switch (func_ov003_020a6c14()) {
        case 1:
            if (kind < 0xD) {
                switch (func_ov003_020a6c7c(kind)) {
                    case 0:
                        value = FX_Mul(value << 12, 0x2000) >> 12;
                        break;
                    case 1:
                        value = FX_Mul(value << 12, 0x1800) >> 12;
                        break;
                    case 2:
                        value = FX_Mul(value << 12, 0x1333) >> 12;
                        break;
                    case 12:
                        value = FX_Mul(value << 12, 0x800) >> 12;
                        break;
                }
            }
            break;
    }
    return (s16)value;
}

/// The duration of one use of the actor's action at level `level`.
// Nonmatching: duration switch: case block layout.
static s32 func_ov003_020a4908(Ov003PsychParams* params, u16 level) {
    s32 value = (params->unk_16 * (level - 1)) + params->unk_14;

    switch (params->unk_12) {
        case 2:
            if (CombatAbility_IsActive(0x10) != 0) {
                value += CombatAbility_GetValue(0x10);
            }
            if (value > 0xA) {
                value = 0xA;
            }
            break;
        case 1:
        case 3:
            if (CombatAbility_IsActive(0x11) != 0) {
                s32 scale = FX_Mul(CombatAbility_GetValue(0x11) << 12, value << 12);

                scale = FX_Divide(scale, 0x64000);
                if (scale < 0x1000) {
                    scale = 0x1000;
                }
                value += scale >> 12;
            }
            break;
    }
    return (s16)value;
}

// Nonmatching: the target narrows the result to s16 at the return, mwcc keeps it s32.
static s32 func_ov003_020a49b4(Ov003PsychParams* params, u16 level) {
    return (params->unk_1C * (level - 1)) + params->unk_1A;
}

// Nonmatching: scaled-duration arithmetic shape (FX_Mul/FX_Divide nesting).
static s32 func_ov003_020a49c8(Ov003PsychParams* params, u16 level) {
    s16 value = (params->unk_22 * (level - 1)) + params->unk_20;

    if (CombatAbility_IsActive(0x49) != 0) {
        value -= FX_Divide(FX_Mul(value << 12, CombatAbility_GetValue(0x49) << 12), 0x64000) >> 12;
    }
    return value;
}

/// The equipped-pin layout of player `deck`.
static const EquippedPin* BtlBadgeActor_GetDeckLayout(s32 deck) {
    return data_ov003_020dc054[deck];
}
