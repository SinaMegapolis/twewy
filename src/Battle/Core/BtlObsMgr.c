#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlCollision.h"
#include "Battle/Core/BtlHit.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/CombatObsSpawn.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"

/// The special effect task, owned by overlay 18.
extern const TaskHandle data_ov018_020ff3a0;

extern const BinIdentifier data_ov003_020d7820;

/// The battle runtime block; allocated by the battle manager, this unit owns the pointer slot.
Ov003Global* g_CombatGlobal;

/// The sprite pack, animations and OBJ allocation of one field-object kind.
typedef struct BattleObsAnimSet {
    /* 0x00 */ const SpriteAnimEntry* entries;
    /* 0x04 */ u16                    unk_04;
    /* 0x06 */ u16                    unk_06;
    /* 0x08 */ u16                    unk_08;
    /* 0x0A */ u16                    unk_0A;
} BattleObsAnimSet; // Size: 0xC

/// The settings of one field-object kind.
typedef struct BattleObsConfig {
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
} BattleObsConfig; // Size: 0x20

static char         Path_BtlTch[0x18] = "Apl_Fur/Grp_BtlTch.bin";
const BinIdentifier Iden_BtlTch       = {3, Path_BtlTch};

static const SpriteAnimEntry data_ov003_020d7014[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry data_ov003_020d6fe4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d7004[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6fb4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6fd4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6ff4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry data_ov003_020d6fc4[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry data_ov003_020d7114[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70cc[3] = {
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

static const SpriteAnimEntry data_ov003_020d7084[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d706c[3] = {
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

static const SpriteAnimEntry data_ov003_020d70b4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70e4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d715c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d71a4[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d7024[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d709c[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry data_ov003_020d70fc[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

const BattleObsAnimSet BtlObsMgr_AnimSets[24] = {
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
static const BattleObsConfig BtlObsMgr_Configs[25] = {
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

/// Effect/SE id per battle-actor slot, indexed by the actor id `BtlObsMgr_DropGroup` gets.
const u8 BtlObsMgr_DropKinds[0x18] = {
    0x0D, 0x0E, 0x0B, 0x0C, 0x0A, 0x0F, 0x10, 0x11, 0x07, 0x08, 0x12, 0x13,
    0x14, 0x15, 0x03, 0x06, 0x01, 0x05, 0x02, 0x04, 0x00, 0x16, 0x00, 0x00,
};

/// Drops a spawn of `kind` at a random place of the field.
void BtlObsMgr_DropAtRandom(u16 kind) {
    Ov003Global* global = g_CombatGlobal;
    s32          height = global->stages[0].height;
    s32          width  = global->stages[0].width;
    s32          y      = RNG_Next((u32)(height << 4) >> 16);
    s32          x      = RNG_Next((u32)(width << 4) >> 16);

    BtlObsMgr_Spawn(kind, x << 12, y << 12);
}

/// Spawns `count` things of the sort picked from `kind`.
void BtlObsMgr_DropGroup(s32 kind, s32 count) {
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
    id = BtlObsMgr_DropKinds[index];
    do {
        BtlObsMgr_DropAtRandom(id);
        i++;
    } while (i < count);
}

// Nonmatching: regswaps
s32 BtlObsMgr_LoadSprite(s32 engine, BattleSprite* sprite, u32 id) {
    SpriteAnimationEx anim;
    int               new_var2;
    if (id >= 24) {
        return 0;
    }
    const BattleObsAnimSet* set = &BtlObsMgr_AnimSets[id];
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

void BtlObsMgr_LoadHitEffect(s32 arg0, BattleSprite* arg1, s32 arg2) {
    BtlHit_LoadSpark(arg0, arg1, BtlObsMgr_Configs[arg2].unk_02);
}

/// Initialises a battle effect actor's `BattleActor` from the per-actor record `id`.
void BtlObsMgr_InitActor(BattleActor* actor, u16 id) {
    const BattleObsConfig* cfg = &BtlObsMgr_Configs[id];
    s16                    maxHp;

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
void BtlObsMgr_PlaySe(s32 arg0, s32 arg1, s32 arg2) {
    s32 value = BtlObsMgr_Configs[arg1].unk_04;

    if (arg0 == 0) {
        BtlSe_PlayAtMainX(value, arg2);
    } else {
        BtlSe_PlayAtSubX(value, arg2);
    }
}

/// TRUE while the battle is not in its "results" phase.
s32 func_ov003_0208656c(void) {
    if (!(g_CombatGlobal->unk3D878 & 4)) {
        return 1;
    }
    return 0;
}

void BtlObsMgr_Spawn(u16 id, s32 arg1, s32 arg2) {
    s32 taskId;

    BtlObsMgr_SpawnTask(id, arg1, arg2, &taskId);
}

/// Spawns the battle effect task for actor record `id`, writing the task id through `out`.
s32 BtlObsMgr_SpawnTask(u16 id, s32 arg1, s32 arg2, s32* out) {
    Ov003EffectSpawn  spawn;
    const TaskHandle* handle;

    spawn.config = BtlObsMgr_Configs[id].unk_00;
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

void BtlObsMgr_Defeat(Ov003EffectActor* effect) {
    effect->actor.currentHp = 0;
    BtlActor_SetPendingCommand(&effect->actor, ACTOR_CMD_DEFEAT);
    effect->actor.flags |= ACTOR_FLAG_DEFEATED;
}

/// Fills in a freshly created effect actor from the spawn parameters `BtlObsMgr_SpawnTask` passed
/// to its task.
void BtlObsMgr_Setup(Ov003EffectActor* effect, Ov003EffectSpawn* spawn) {
    BtlObsMgr_InitActor(&effect->actor, spawn->id);
    effect->id               = spawn->config;
    effect->config           = spawn->id;
    effect->actor.position.x = spawn->arg1;
    effect->actor.position.y = spawn->arg2;
    effect->actor.position.z = 0;
    BtlCollision_Add(&effect->sprite.collider, 3, effect->actor.radius, effect->actor.height);
    BtlList_Append(&g_CombatGlobal->unk3D8B4, &effect->node, effect);
}

void BtlObsMgr_Update(Ov003EffectActor* effect) {
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

void BtlObsMgr_Remove(Ov003EffectActor* effect) {
    BtlCollision_Remove(&effect->sprite.collider);
    BtlList_Remove(&g_CombatGlobal->unk3D8B4, &effect->node);
}

void BtlObsMgr_ResetList(void) {
    BtlList_Init(&g_CombatGlobal->unk3D8B4);
}

/// Picks the hit effect/SE combo for whatever the main engine's camera is currently showing.
void BtlObsMgr_RollDrop(void) {
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
            BtlObsMgr_DropAtRandom(0x18);
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
            BtlObsMgr_DropGroup(commands->kind, commands->count);
            commands++;
        } while (commands->kind != 0xFF);
    }
}
