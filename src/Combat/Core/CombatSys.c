#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatEffectActor.h"
#include "Combat/Core/CombatObsSpawn.h"
#include "Combat/Friend/Private/Beat.h"
#include "Combat/Friend/Private/Shiki.h"
#include "Combat/Friend/Shiki.h"
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

#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi/cpumem.h>

// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------

/// One pooled charge-gauge entry; `CombatSys_InitPartnerActions` seeds 16 of them per gauge state.

/// Sync/charge gauge state at `Ov003Global + 0x3D8E4`. The 16 entries shuttle between the two
/// lists: `unk_0C` holds the ones waiting to be consumed and `unk_14` the ones in flight.

/// State of the running effect-command task at `Ov003Global + 0x3D8E8`. The `queue` holds the
/// shuffled animation ids that `func_ov003_02086d24` hands out one at a time.

/// Parameters `CombatSys_SpawnObsTask` hands to a freshly created effect task.

/// The sprite half of a battle effect actor (0x38 bytes), driven by the ov003 effect helpers.

/// A pooled battle effect actor. It is linked into `Ov003Global.unk3D8B4` through the trailing
/// node, whose item pointer is the actor itself.

/// One entry of the per-engine "action effect" pool at `Ov003Global + 0x3D3CC` (8 entries,
/// `unk_04`/`unk_08`/`unk_0C` are the position the linked actor is pushed to).

/// Header of the `Ov003ActionEffect` pool: two lists (free, in use) followed by the entries.

/// The special effect task, owned by overlay 18.
extern const TaskHandle data_ov018_020ff3a0;
/// The charge/effect task handles owned by the battle setup unit.

/// Enemy-id -> command-table index, and the per-command effect/SE id table.
extern const BinIdentifier data_ov003_020d7820;

// ---------------------------------------------------------------------------------------------
// Helpers over the delinked initialisation table
// ---------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------

/// The battle runtime block; allocated by the battle manager, this unit owns the pointer slot.
Ov003Global* g_CombatGlobal;

/// A sprite-animation set: the entry table plus the ids/scale used to build a frame.

/// Per-actor initialisation record, indexed by the id in `CombatSys_SetupObs`'s argument.
typedef struct Ov003ActorConfig {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 zVelocity;
    /* 0x10 */ s32 linearDampingStep;
    /* 0x14 */ s32 dampingScale;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s16 maxHp;
    /* 0x1E */ s16 unk_1E;
} Ov003ActorConfig; // Size: 0x20

static char         Path_BtlTch[0x18] = "Apl_Fur/Grp_BtlTch.bin";
const BinIdentifier Iden_BtlTch       = {3, Path_BtlTch};

static const SpriteAnimEntry data_ov003_020d6fb4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6fc4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry data_ov003_020d6fd4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6fe4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6ff4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d7004[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d7014[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry data_ov003_020d7024[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d703c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7054[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d706c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7084[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d709c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70b4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70cc[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70e4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70fc[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7114[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d712c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7144[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d715c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7174[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d718c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d71a4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

const Ov003AnimSet CombatSys_ObsAnimSets[24] = {
    {(const SpriteAnimEntry*)&data_ov003_020d7084,  1, 4,  16, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d706c,  2, 4,  28, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d718c,  3, 4,  24, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7174,  4, 4,  40, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d715c,  5, 4,  48, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7144,  6, 4,  48, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d712c,  7, 4,  40, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d70b4,  8, 4,  80, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d70e4,  9, 4,  84, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d70fc, 10, 4, 128, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7004, 11, 4, 152, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d6fe4, 12, 4, 156, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d6fd4, 13, 4, 156, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d6ff4, 14, 4, 212, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d6fb4, 15, 4, 240, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d703c, 16, 4,  68, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d709c, 17, 4,  76, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d70cc, 18, 4,  96, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7114, 19, 4,  44, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d71a4, 20, 4,  52, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7024, 21, 4,  52, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7054, 22, 4,  52, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d6fc4, 23, 4,  36, 0},
    {(const SpriteAnimEntry*)&data_ov003_020d7014, 24, 4,  24, 0},
};

/// The field-object kinds: task kind, hit spark, sound, hit box, physics and HP.
static const Ov003ActorConfig CombatSys_ObsConfigs[25] = {
    {0, 2, 47, 12, 32, 696,   82, 3891, 3277,  0, 100},
    {2, 2, 48, 13, 32, 696,   90, 3891, 2458,  0, 170},
    {2, 2, 48, 14, 41, 696,   94, 3891, 2458,  0, 150},
    {2, 2, 48, 17, 53, 696,   98, 3891, 2458,  0, 210},
    {2, 2, 48, 25, 37, 696,  102, 3891, 2458,  0, 190},
    {2, 3, 48, 24, 37, 696,  115, 3891, 2458,  0, 200},
    {2, 3, 48, 16, 52, 696,  123, 3891, 2458,  0, 250},
    {2, 3, 48, 25, 69, 696,  131, 3891, 2458,  0, 300},
    {2, 3, 49, 25, 64, 696,  328, 3891, 2048, 16, 320},
    {2, 3, 50, 29, 64, 696,  410, 3277, 2048, 12, 380},
    {1, 5, 50, 61, 65, 819,  819, 2048, 1638, 10, 400},
    {1, 5, 50, 70, 66, 819,  901, 2048, 1638, 10, 430},
    {1, 5, 50, 72, 65, 819,  983, 2048, 1638, 10, 440},
    {1, 5, 50, 87, 72, 819, 1065, 2048, 1638, 10, 500},
    {1, 5, 49, 76, 89, 819, 1147, 2048, 1638, 10, 550},
    {2, 3, 49, 34, 50, 696,  164, 3277, 1638, 12, 270},
    {2, 3, 49, 42, 52, 696,  164, 3277, 1638, 12, 310},
    {2, 3, 48, 52, 54, 696,  205, 3277, 1638, 12, 350},
    {2, 2, 48, 32, 36, 696,  102, 3891, 2867, 16, 220},
    {2, 2, 48, 34, 43, 696,  102, 3891, 2867, 16, 225},
    {2, 2, 48, 34, 41, 696,  102, 3891, 2867, 16, 230},
    {2, 2, 48, 36, 48, 696,  102, 3891, 2867, 20, 260},
    {1, 2, 48, 12, 64, 696,   94, 3891, 3072,  0, 150},
    {1, 2, 48, 12, 45, 696,   82, 3891, 3277,  0, 100},
    {3, 4, 49, 32, 30, 655,  143, 3789, 2048,  0, 300},
};

/// Effect/SE id per battle-actor slot, indexed by the actor id `CombatSys_DropObsGroup` gets.
const u8 CombatSys_ObsDropKinds[0x18] = {
    0x0D, 0x0E, 0x0B, 0x0C, 0x0A, 0x0F, 0x10, 0x11, 0x07, 0x08, 0x12, 0x13,
    0x14, 0x15, 0x03, 0x06, 0x01, 0x05, 0x02, 0x04, 0x00, 0x16, 0x00, 0x00,
};

/// Frame counts of the three sync-gauge charge steps (one row per mode).
static const s8 data_ov003_020d7614[4][3] = {
    {4, 8, 16},
    {4, 8, 16},
    {4, 8, 16},
    {0, 0,  0},
};

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Drops a spawn of `kind` at a random place of the field.
void CombatSys_DropObsAtRandom(u16 kind) {
    Ov003Global* global = g_CombatGlobal;
    s32          height = global->stages[0].height;
    s32          width  = global->stages[0].width;
    s32          y      = RNG_Next((u32)(height << 4) >> 16);
    s32          x      = RNG_Next((u32)(width << 4) >> 16);

    CombatSys_SpawnObs(kind, x << 12, y << 12);
}

/// Spawns `count` things of the sort picked from `kind`.
void CombatSys_DropObsGroup(s32 kind, s32 count) {
    s32 index = 0x14;
    s32 i;
    u8  id;

    switch (kind) {
        case 0:
            index = RNG_Next(2);
            break;
        case 1:
            index = RNG_Next(2) + 2;
            break;
        case 2:
            index = 4;
            break;
        case 3:
            index = RNG_Next(3) + 5;
            break;
        case 4:
            index = RNG_Next(2) + 8;
            break;
        case 5:
            index = RNG_Next(4) + 10;
            break;
        case 6:
            index = 0xE;
            break;
        case 7:
            index = 0xF;
            break;
        case 8:
            index = 0x10;
            break;
        case 9:
            index = 0x11;
            break;
        case 10:
            index = 0x12;
            break;
        case 11:
            index = 0x13;
            break;
        case 12:
            index = 0x14;
            break;
        case 13:
            index = 0x15;
            break;
    }
    i = 0;
    if (count <= 0) {
        return;
    }
    id = CombatSys_ObsDropKinds[index];
    do {
        CombatSys_DropObsAtRandom(id);
        i++;
    } while (i < count);
}

// Nonmatching: regswaps
s32 CombatSys_LoadObsSprite(s32 engine, BattleSprite* sprite, u32 id) {
    SpriteAnimationEx anim;
    int               new_var2;
    if (id >= 24) {
        return 0;
    }
    const Ov003AnimSet* set = &CombatSys_ObsAnimSets[id];
    BtlSprite_InitAnim(&anim.anim, engine, &data_ov003_020d7820);
    new_var2                       = set->unk_04;
    anim.anim.bits_10_11           = 1;
    anim.anim.packIndex            = new_var2;
    anim.anim.unk_18               = 2;
    u16                    new_var = set->unk_06;
    const SpriteAnimEntry* entry   = set->entries;
    anim.anim.unk_1C               = entry->charDataIndex;
    anim.anim.unk_1E               = set->unk_08 << 5;
    anim.anim.unk_26               = entry->cellDataIndex;
    u16 new_var3                   = entry->animDataIndex;
    anim.anim.unk_20               = new_var;
    anim.anim.unk_28               = new_var3;
    anim.anim.animIndex            = entry->animIndex + 1;
    anim.animTable                 = set->entries;
    return BtlSprite_Load(sprite, &anim);
}

/// Reads a halfword out of the unmodelled part of `Ov003Global`.

void CombatSys_LoadObsHitEffect(s32 arg0, BattleSprite* arg1, s32 arg2) {
    CombatHit_LoadSpark(arg0, arg1, CombatSys_ObsConfigs[arg2].unk_02);
}

/// Initialises a battle effect actor's `BattleActor` from the per-actor record `id`.
void CombatSys_InitObsActor(BattleActor* actor, u16 id) {
    const Ov003ActorConfig* cfg = &CombatSys_ObsConfigs[id];
    s16                     maxHp;

    BtlActor_Init(actor, ACTOR_KIND_OBJECT);
    actor->radius            = cfg->unk_08;
    actor->height            = cfg->unk_0A;
    actor->zGravity          = cfg->zVelocity;
    actor->linearDampingStep = cfg->linearDampingStep;
    actor->dampingScale      = cfg->dampingScale;
    actor->bounceScale       = cfg->unk_18;
    actor->impactPercent     = cfg->unk_1E;
    maxHp                    = cfg->maxHp;
    actor->maxHp             = maxHp;
    actor->currentHp         = maxHp;
}

/// Plays the actor record's sound: the normal SE when `arg0 == 0`, the alternate one otherwise.
void CombatSys_PlayObsSe(s32 arg0, s32 arg1, s32 arg2) {
    s32 value = CombatSys_ObsConfigs[arg1].unk_04;

    if (arg0 == 0) {
        CombatSe_PlayAtMainX(value, arg2);
    } else {
        CombatSe_PlayAtSubX(value, arg2);
    }
}

/// TRUE while the battle is not in its "results" phase.
s32 func_ov003_0208656c(void) {
    if (!(g_CombatGlobal->unk3D878 & 4)) {
        return 1;
    }
    return 0;
}

void CombatSys_SpawnObs(u16 id, s32 arg1, s32 arg2) {
    s32 taskId;

    CombatSys_SpawnObsTask(id, arg1, arg2, &taskId);
}

/// Spawns the battle effect task for actor record `id`, writing the task id through `out`.
s32 CombatSys_SpawnObsTask(u16 id, s32 arg1, s32 arg2, s32* out) {
    Ov003EffectSpawn  spawn;
    const TaskHandle* handle;

    spawn.config = CombatSys_ObsConfigs[id].unk_00;
    spawn.id     = id;
    spawn.arg1   = arg1;
    spawn.arg2   = arg2;
    switch (spawn.config) {
        case 3:
            handle = &data_ov018_020ff3a0;
            break;
        case 0:
            handle = &Tsk_BtlObs;
            break;
        case 1:
            handle = &Tsk_BtlObs;
            break;
        case 2:
            handle = &Tsk_BtlObs;
            break;
        default:
            handle = &Tsk_BtlObs;
            break;
    }
    *out = EasyTask_CreateTask(&g_CombatGlobal->unk_00000, handle, NULL, 0, NULL, &spawn);
    return (s32)BtlList_GetTail(&g_CombatGlobal->unk3D8B4);
}

void CombatSys_DefeatObs(Ov003EffectActor* effect) {
    effect->actor.currentHp = 0;
    BtlActor_SetPendingCommand(&effect->actor, ACTOR_CMD_DEFEAT);
    effect->actor.flags |= ACTOR_FLAG_DEFEATED;
}

/// Fills in a freshly created effect actor from the spawn parameters `CombatSys_SpawnObsTask` passed
/// to its task.
void CombatSys_SetupObs(Ov003EffectActor* effect, Ov003EffectSpawn* spawn) {
    CombatSys_InitObsActor(&effect->actor, spawn->id);
    effect->id               = spawn->config;
    effect->config           = spawn->id;
    effect->actor.position.x = spawn->arg1;
    effect->actor.position.y = spawn->arg2;
    effect->actor.position.z = 0;
    BtlCollision_Add(&effect->sprite.collider, 3, effect->actor.radius, effect->actor.height);
    BtlList_Append(&g_CombatGlobal->unk3D8B4, &effect->node, effect);
}

void CombatSys_UpdateObs(Ov003EffectActor* effect) {
    s16 range;
    s32 half;

    BtlActor_UpdatePhysics(&effect->actor);
    if (!(effect->actor.flags & ACTOR_FLAG_HELD)) {
        range = effect->actor.radius;
        half  = (range / 2) << 12;
        if (BtlStage_ClampXToField(&effect->actor.position.x, range << 12) != 0) {
            effect->actor.xVelocity = 0;
        }
        if (BtlStage_ClampYToFloor(&effect->actor.position.y, half, effect->actor.position.x) != 0) {
            effect->actor.yVelocity = 0;
        }
    }
    BtlCollision_SetPosition(&effect->sprite.collider, effect->actor.position.x, effect->actor.position.y,
                             effect->actor.position.z);
}

void CombatSys_RemoveObs(Ov003EffectActor* effect) {
    BtlCollision_Remove(&effect->sprite.collider);
    BtlList_Remove(&g_CombatGlobal->unk3D8B4, &effect->node);
}

void CombatSys_ResetObsList(void) {
    BtlList_Init(&g_CombatGlobal->unk3D8B4);
}

/// Picks the hit effect/SE combo for whatever the main engine's camera is currently showing.
void CombatSys_RollObsDrop(void) {
    s32                      index;
    const CombatObsSpawnCmd* commands = NULL;
    BattleStage*             camera   = &g_CombatGlobal->stages[0];
    const u8*                weights;
    s16                      roll;

    if (CombatAbility_IsActive(5) != 0) {
        return;
    }
    switch (g_CombatGlobal->unk3D88E) {
        case 7:
        case 8:
            CombatSys_DropObsAtRandom(0x18);
            return;
    }
    roll    = RNG_Next(100);
    index   = 0;
    weights = CombatObsSpawn_EncounterWeights[g_CombatGlobal->unk3D88E];
    do {
        roll -= *weights;
        if (roll < 0) {
            break;
        }
        index++;
        weights++;
    } while (index < 4);
    if (index >= 4) {
        return;
    }
    if ((u16)camera->unk_00 < 0x29) {
        commands = CombatObsSpawn_FieldLists[BtlField_UG_Defs[(u16)camera->unk_00].obsGroup][index];
    }
    if (commands != NULL && commands->kind != 0xFF) {
        do {
            CombatSys_DropObsGroup(commands->kind, commands->count);
            commands++;
        } while (commands->kind != 0xFF);
    }
}

void CombatSys_PushPartnerAction(u16 value) {
    Ov003ChargeState* state = g_CombatGlobal->chargeState;
    Ov003ChargeEntry* entry;

    state->unk_08 = 0;
    entry         = BtlList_GetHead(&state->unk_0C);
    if (entry != NULL) {
        entry->value = value;
        BtlList_Remove(&state->unk_0C, &entry->node);
        BtlList_Append(&state->unk_14, &entry->node, entry);
        return;
    }
    entry = BtlList_GetTail(&state->unk_14);
    if (entry != NULL) {
        entry->value = value;
    }
}

static void CombatSys_FreePartnerAction(Ov003ChargeEntry* entry) {
    Ov003ChargeState* state = g_CombatGlobal->chargeState;

    if (entry == NULL) {
        return;
    }
    entry->value = 0;
    BtlList_Remove(&state->unk_14, &entry->node);
    BtlList_Append(&state->unk_0C, &entry->node, entry);
}

void CombatSys_ClearPartnerActions(void) {
    Ov003ChargeEntry* entry = BtlList_GetHead(&g_CombatGlobal->chargeState->unk_14);

    if (entry == NULL) {
        return;
    }
    do {
        Ov003ChargeEntry* next = BtlList_GetNext(&entry->node);
        CombatSys_FreePartnerAction(entry);
        entry = next;
    } while (entry != NULL);
}

void CombatSys_SetPartnerDirection(s32 value) {
    g_CombatGlobal->chargeState->unk_00 = value;
}

u16 CombatSys_PopPartnerAction(void) {
    Ov003ChargeState* state = g_CombatGlobal->chargeState;
    u16               value = 0;
    Ov003ChargeEntry* entry = BtlList_GetHead(&state->unk_14);

    if (entry != NULL) {
        value        = entry->value;
        entry->value = 0;
        BtlList_Remove(&state->unk_14, &entry->node);
        BtlList_Append(&state->unk_0C, &entry->node, entry);
    }
    return value;
}

u16 CombatSys_TakePartnerDirection(void) {
    Ov003ChargeState* state = g_CombatGlobal->chargeState;
    u16               value = state->unk_00;

    state->unk_00 = 0;
    return value;
}

s32 func_ov003_02086a64(void) {
    return g_CombatGlobal->chargeState->unk_08;
}

s32 func_ov003_02086a80(void) {
    if (g_CombatGlobal->chargeState->flags & 4) {
        return 1;
    }
    return 0;
}

void func_ov003_02086aa8(s32 value) {
    g_CombatGlobal->chargeState->unk_08 = value;
}

void func_ov003_02086ac4(s32 enabled) {
    Ov003ChargeState* state = g_CombatGlobal->chargeState;

    if (enabled != 0) {
        state->flags |= 4;
    } else {
        state->flags &= ~4;
    }
}

void func_ov003_02086af0(void) {
    g_CombatGlobal->chargeState->flags &= ~2;
}

/// Resets a sync gauge state and seeds its 16 pooled entries into the free list.
void CombatSys_InitPartnerActions(Ov003ChargeState* state) {
    s32               i;
    Ov003ChargeEntry* entry;

    g_CombatGlobal->chargeState = state;
    state->flags                = 0;
    state->unk_04               = 0;
    state->unk_06               = 0;
    state->unk_08               = 0;
    state->unk_00               = 0;
    BtlList_Init(&state->unk_0C);
    BtlList_Init(&state->unk_14);
    entry = state->entries;
    for (i = 0; i < 16; i++) {
        entry->value = 0;
        BtlList_Append(&state->unk_0C, &entry->node, entry);
        entry++;
    }
}

/// Advances the charge step until it covers the accumulated progress. Returns TRUE if it moved.
static s32 func_ov003_02086b88(Ov003EffectCmd* cmd) {
    s32 advanced = 0;

    while (cmd->progress >= cmd->next) {
        if (cmd->current >= cmd->count) {
            break;
        }
        cmd->current++;
        cmd->next = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
        advanced  = 1;
    }
    return advanced;
}

/// Shuffles `table[0..count-1]` into a random permutation (insertion shuffle).
void CombatSys_ShuffleIds(u16* table, u32 count) {
    u16 value = 0;
    s32 i     = 0;

    if (count == 0) {
        return;
    }
    do {
        s32 target = RNG_Next(i + 1);
        s32 j      = i;

        while (j > target) {
            table[j] = table[j - 1];
            j--;
        }
        table[target] = value;
        value++;
        i = (s16)(i + 1);
    } while (count > value);
}

/// Eases `*value` towards the sine table entry `*index` by an eighth of the difference.
void CombatSys_EaseToSine(s32* value, s16* index) {
    *value += ((SIN2(FX_SinCosTable_, (u16)(*index << 9)) * 4) - *value) / 8;
    *index += 1;
}

/// Refills the effect command's queue with a fresh shuffle.
void func_ov003_02086cc0(void) {
    Ov003EffectCmd* cmd = g_CombatGlobal->effectCmd;

    switch (cmd->mode) {
        case 1:
            cmd->remaining = func_ov005_020e74fc(&cmd->queue[0], cmd->capacity, cmd->unk_2C);
            break;
        case 0:
        case 2:
        default:
            CombatSys_ShuffleIds(&cmd->queue[0], cmd->capacity);
            cmd->remaining = cmd->capacity;
            break;
    }
}

u16 func_ov003_02086d24(void) {
    Ov003EffectCmd* cmd = g_CombatGlobal->effectCmd;
    u16             value;

    if (cmd->mode == 2) {
        return func_ov006_020e7538(cmd->unk_2C);
    }
    if (cmd->remaining == 0) {
        func_ov003_02086cc0();
    }
    cmd->remaining--;
    value                      = cmd->queue[cmd->remaining];
    cmd->queue[cmd->remaining] = 0xFFFF;
    return value;
}

/// Draws one battle effect frame: a sprite animation for the charge effects, a delegated draw for
/// the special ones.
void func_ov003_02086d8c(BattleSprite* sprite, s32 animId, s32 flipped, s32 x, s16 y, s16 z) {
    switch (g_CombatGlobal->effectCmd->mode) {
        case 0:
        case 1: {
            u16 anim = animId * 2;

            if (flipped != 0) {
                anim++;
            }
            BtlSprite_SetAnim(sprite, anim, 0);
            BtlSprite_Update(sprite);
            BtlSprite_SetPosition(sprite, x, y);
            BtlSprite_Render(sprite);
            break;
        }
        case 2:
            func_ov006_020e73b8(sprite, animId, flipped, x, y, z);
            break;
    }
}

/// Fires the effect command: the charge effects add a card attack, the special ones spawn their
/// own task.
void func_ov003_02086e38(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Ov003EffectCmd* cmd = g_CombatGlobal->effectCmd;
    s32             value;

    if (cmd->flags & 1) {
        return;
    }
    value = arg1 + (arg3 << 12);
    switch (cmd->mode) {
        case 2:
            func_ov006_020ea6c0(arg0, value, arg2);
            break;
        case 1:
            func_ov005_020eb29c(arg0, value, arg2);
            break;
        case 0:
        default:
            BtlCmd_Esp_CreateTask(arg0, value, arg2);
            break;
    }
}

s16* func_ov003_02086e94(void) {
    return (s16*)g_CombatGlobal->effectCmd->unk_2C;
}

/// Number of charge steps the active partner's sticker grants, clamped to 1..3.
static u8 func_ov003_02086eb0(void) {
    u32 count = 0;

    switch (g_CombatGlobal->unk3D88D) {
        case 0:
            count = Inventory_GetOwnedCount(0x2B0);
            break;
        case 1:
            count = Inventory_GetOwnedCount(0x2B8);
            break;
        case 2:
            count = Inventory_GetOwnedCount(0x2C0);
            break;
        default:
            break;
    }
    if (count < 1) {
        count = 1;
    }
    if (count > 3) {
        count = 3;
    }
    return count;
}

s32 func_ov003_02086f20(s32 arg0, s32 arg1) {
    Ov003EffectCmd* cmd = g_CombatGlobal->effectCmd;

    if (cmd->current >= cmd->count) {
        return 0;
    }
    func_ov003_0209d038(arg0, arg1 + 7, 0x12);
    cmd->progress += arg0;
    return func_ov003_02086b88(cmd);
}

s32 func_ov003_02086f7c(void) {
    s32 done = 0;

    switch (g_CombatGlobal->effectCmd->mode) {
        case 1:
            if (*func_ov003_02086e94() >= 5) {
                done = 1;
            }
            break;
        case 2:
            done = 1;
            break;
        default:
        case 0:
            if (*func_ov003_02086e94() >= 3) {
                done = 1;
            }
            break;
    }
    return done;
}

s8 func_ov003_02086fec(void) {
    return g_CombatGlobal->effectCmd->current;
}

/// Starts the next charge step: advances the stage, re-shuffles the queue and restarts the effect.
void func_ov003_02087008(void) {
    Ov003EffectCmd* cmd      = g_CombatGlobal->effectCmd;
    s8              stage    = cmd->current;
    u16             previous = stage - 1;

    cmd->stage++;
    cmd->current  = 0;
    cmd->progress = 0;
    cmd->next     = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
    func_ov003_020871cc(1);
    func_ov003_020c1f18(&g_CombatGlobal->unk_34230, previous);
}

void func_ov003_02087094(Ov003EffectCmd* cmd, u8 mode, s32 arg2) {
    g_CombatGlobal->effectCmd = cmd;
    cmd->mode                 = mode;
    cmd->unk_2C               = arg2;
    cmd->flags                = 0;
    cmd->current              = 0;
    cmd->count                = func_ov003_02086eb0();
    cmd->stage                = gSaveData.battleChargeStage;
    cmd->progress             = gSaveData.battleChargeProgress;
    cmd->next                 = cmd->stage + data_ov003_020d7614[cmd->mode][cmd->current];
    if (g_CombatGlobal->unk3D88C == 0 && gSaveData.battleTimer == 0) {
        if (CombatAbility_IsActive(7) != 0) {
            cmd->progress += (s8)CombatAbility_GetValue(7);
        }
    }
    func_ov003_02086b88(cmd);
    switch (cmd->mode) {
        case 1:
            cmd->capacity = 10;
            break;
        case 2:
            cmd->capacity = 4;
            break;
        default:
        case 0:
            cmd->capacity = func_ov004_020e737c();
            break;
    }
    func_ov003_02086cc0();
}

void func_ov003_020871b0(Ov003EffectCmd* cmd) {
    gSaveData.battleChargeProgress = cmd->progress;
    gSaveData.battleChargeStage    = cmd->stage;
}

void func_ov003_020871cc(s32 arg0) {
    Ov003EffectCmd* cmd = g_CombatGlobal->effectCmd;

    switch (cmd->mode) {
        case 1:
            func_ov005_020e740c(cmd->unk_2C, arg0);
            break;
        case 2:
            func_ov006_020e74b0(cmd->unk_2C, arg0);
            break;
        default:
        case 0:
            func_ov004_020e739c(cmd->unk_2C, arg0);
            break;
    }
}

void func_ov003_0208722c(void) {
    g_CombatGlobal->effectCmd->flags |= 1;
}

/// Pushes a battle actor towards `x`/`y`/`z` through the shared action-effect pool.
void func_ov003_02087250(Ov003EffectState* state, BattleActor* target, s32 angle) {
    Ov003ActionEffectPool* pool = &g_CombatGlobal->actionEffects;
    Ov003ActionEffect*     effect;

    if (target == NULL) {
        return;
    }
    effect = BtlList_GetHead(&pool->free);
    if (effect == NULL) {
        return;
    }
    effect->state  = state;
    effect->target = target;
    effect->angle  = angle;
    BtlList_Remove(&pool->free, &effect->node);
    BtlList_Append(&pool->active, &effect->node, effect);
}

void func_ov003_020872b4(Ov003ActionEffect* effect) {
    Ov003ActionEffectPool* pool = &g_CombatGlobal->actionEffects;

    if (effect == NULL || effect->target == NULL) {
        return;
    }
    effect->unk_00 = 0;
    effect->state  = NULL;
    effect->target = NULL;
    effect->angle  = 0;
    BtlList_Remove(&pool->active, &effect->node);
    BtlList_Append(&pool->free, &effect->node, effect);
}

void func_ov003_02087310(void) {
    Ov003ActionEffect* effect = BtlList_GetHead(&g_CombatGlobal->actionEffects.active);

    if (effect == NULL) {
        return;
    }
    do {
        Ov003ActionEffect* next = BtlList_GetNext(&effect->node);
        func_ov003_020872b4(effect);
        effect = next;
    } while (effect != NULL);
}

Ov003ActionEffect* func_ov003_02087358(void) {
    return BtlList_GetHead(&g_CombatGlobal->actionEffects.active);
}

void func_ov003_02087378(void) {
    Ov003ActionEffectPool* pool = &g_CombatGlobal->actionEffects;
    s32                    i;
    Ov003ActionEffect*     effect;

    BtlList_Init(&pool->free);
    BtlList_Init(&pool->active);
    effect = pool->entries;
    for (i = 0; i < 8; i++) {
        effect->unk_00 = 0;
        effect->state  = NULL;
        effect->target = NULL;
        effect->angle  = 0;
        BtlList_Append(&pool->free, &effect->node, effect);
        effect++;
    }
}

/// Retires the action effects whose target actor is gone or defeated.
void func_ov003_020873e0(void) {
    Ov003ActionEffect* effect = BtlList_GetHead(&g_CombatGlobal->actionEffects.active);

    if (effect == NULL) {
        return;
    }
    do {
        s32 flags = effect->target->flags;

        if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_INTANGIBLE) || (flags & ACTOR_FLAG_LAUNCHED)) {
            Ov003ActionEffect* next = BtlList_GetNext(&effect->node);
            func_ov003_020872b4(effect);
            effect = next;
        } else {
            effect = BtlList_GetNext(&effect->node);
        }
    } while (effect != NULL);
}

/// Ticks the level-up gauge: every 300 frames it banks a percentage of the gauge maximum.
void func_ov003_02087458(s32 mode, s16* timer) {
    BattleActor* actor;
    s16          amount;

    if (g_CombatGlobal->unk3D874 != 2) {
        return;
    }
    if (BtlMgr_HasAbility(mode, 0x3C) == 0) {
        return;
    }
    *timer += 1;
    if (*timer <= 0x12C) {
        return;
    }
    *timer = 0;
    amount = (g_CombatGlobal->unk3D8D2 * BtlMgr_GetAbilityValue(mode, 0x3C)) / 100;
    if (amount <= 0) {
        amount = 1;
    }
    BtlMgr_AddTeamHp(mode, amount);
    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    func_ov003_0209b564(mode, amount, actor, 1);
}

/// Advances the on-screen level-up gauge timer when every attack pin of the engine has been used.
void func_ov003_02087540(s32 mode) {
    BattleActor* actor;
    s32          ready;

    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    ready = 0;
    if (mode != 1) {
        if (g_CombatGlobal->unk3D8F4 != 0 && CombatAbility_IsActiveForPlayer(0x41) != 0 &&
            CombatAbility_IsActiveForPlayer(0x42) != 0 && CombatAbility_IsActiveForPlayer(0x43) != 0 &&
            CombatAbility_IsActiveForPlayer(0x44) != 0 && CombatAbility_IsActiveForPlayer(0x45) != 0)
        {
            ready = 1;
        }
    } else {
        if (g_CombatGlobal->unk3D8F0 != 0 && CombatAbility_IsActiveForPartner(0x41) != 0 &&
            CombatAbility_IsActiveForPartner(0x42) != 0 && CombatAbility_IsActiveForPartner(0x43) != 0 &&
            CombatAbility_IsActiveForPartner(0x44) != 0 && CombatAbility_IsActiveForPartner(0x45) != 0)
        {
            ready = 1;
        }
    }
    if (ready == 0) {
        return;
    }
    if (actor->invincibleTimer <= 0) {
        actor->invincibleTimer++;
    }
}

u16 func_ov003_0208765c(u16* state, s32 value, s32 arg2) {
    u16 previous = *state;
    s32 half     = g_CombatGlobal->unk3D8D2 / 2;

    if (value >= half) {
        *state = 2;
    } else if (arg2 >= 0xF0) {
        *state = 3;
    } else {
        *state = 1;
    }
    if (arg2 != 0 && *state == previous) {
        return 0;
    }
    return *state;
}

s32 func_ov003_020876c0(void) {
    s32 flags = g_CombatGlobal->player->flags;

    if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_STAGGERED) || (g_CombatGlobal->unk3D878 & 8)) {
        return 1;
    }
    return 0;
}

/// TRUE when the main display actor is far enough from the projected sub-screen position.
s32 func_ov003_02087704(void) {
    BattleActor* actor;
    s32          x;
    s32          y;
    s32          dx;
    s32          dy;

    if (BtlTouch_IsGesture(BTL_TOUCH_GESTURE_DRAG) == 0 || BtlTouch_IsTargetKind(BTL_TOUCH_TARGET_PLAYER) == 0) {
        return 0;
    }
    x = BtlStage_ToWorldX(0, g_CombatGlobal->touchWork.x);
    y = BtlStage_ToWorldY(0, g_CombatGlobal->touchWork.y, 0);
    BtlStage_ClampToFloor(&x, &y, 0x8000, 0x8000);
    actor = g_CombatGlobal->player;
    dx    = x - actor->position.x;
    dy    = y - actor->position.y;
    if ((FX_Sqrt(FX_Mul(dx, dx) + FX_Mul(dy, dy)) >> 12) > 0x50) {
        return 1;
    }
    return 0;
}

/// Plays one of the two hit-reaction SE sets, picked at random.
void func_ov003_02087804(s32 sub) {
    u16 roll;

    if (sub != 0) {
        roll = RNG_Next(0xFFFF) % 9;
        switch (roll) {
            case 0:
                CombatSe_PlayAtMainX(0x35A, g_CombatGlobal->player->position.x);
                return;
            case 1:
                CombatSe_PlayAtMainX(0x35B, g_CombatGlobal->player->position.x);
                return;
            case 2:
                CombatSe_PlayAtMainX(0x35C, g_CombatGlobal->player->position.x);
                return;
            case 3:
                CombatSe_PlayAtMainX(0x35D, g_CombatGlobal->player->position.x);
                return;
            case 4:
                CombatSe_PlayAtMainX(0x35E, g_CombatGlobal->player->position.x);
                return;
            case 5:
                CombatSe_PlayAtMainX(0x35F, g_CombatGlobal->player->position.x);
                return;
            case 6:
                CombatSe_PlayAtMainX(0x360, g_CombatGlobal->player->position.x);
                return;
            case 7:
                CombatSe_PlayAtMainX(0x361, g_CombatGlobal->player->position.x);
                return;
            case 8:
                CombatSe_PlayAtMainX(0x362, g_CombatGlobal->player->position.x);
                return;
        }
    }
    roll = RNG_Next(0xFFFF) % 8;
    switch (roll) {
        case 0:
            CombatSe_PlayAtMainX(0x352, g_CombatGlobal->player->position.x);
            return;
        case 1:
            CombatSe_PlayAtMainX(0x353, g_CombatGlobal->player->position.x);
            return;
        case 2:
            CombatSe_PlayAtMainX(0x354, g_CombatGlobal->player->position.x);
            return;
        case 3:
            CombatSe_PlayAtMainX(0x355, g_CombatGlobal->player->position.x);
            return;
        case 4:
            CombatSe_PlayAtMainX(0x356, g_CombatGlobal->player->position.x);
            return;
        case 5:
            CombatSe_PlayAtMainX(0x357, g_CombatGlobal->player->position.x);
            return;
        case 6:
            CombatSe_PlayAtMainX(0x358, g_CombatGlobal->player->position.x);
            return;
        case 7:
            CombatSe_PlayAtMainX(0x359, g_CombatGlobal->player->position.x);
            return;
    }
}

/// Moves the display actor of one engine to a new position and tells the HUD about it.
void CombatSys_WarpCharacter(s32 mode, s32 x, s32 y, s32 z) {
    BattleActor* actor;
    s32          oldY;
    s32          oldZ;
    s32          oldX;

    if (mode != 1) {
        actor = g_CombatGlobal->player;
    } else {
        actor = g_CombatGlobal->partner;
    }
    oldX = actor->position.x;
    oldY = actor->position.y;
    oldZ = actor->position.z;
    if (oldX > x) {
        actor->isFlipped = 0;
    }
    if (actor->position.x < x) {
        actor->isFlipped = 1;
    }
    actor->position.x = x;
    actor->position.y = y;
    actor->position.z = z;
    func_ov003_0209f770(mode, oldX, oldY, oldZ);
}
