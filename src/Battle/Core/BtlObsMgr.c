#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlCollision.h"
#include "Battle/Core/BtlHit.h"
#include "Battle/Core/BtlList.h"
#include "Battle/Core/BtlMgr.h"
#include "Battle/Core/BtlObsSpawn.h"
#include "Battle/Core/BtlSe.h"
#include "Battle/Core/BtlStage.h"
#include "Combat/Core/BtlField.h"
#include "Combat/Core/BtlFieldData.h"
#include "Combat/Core/CombatAbility.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "SndMgrSeIdx.h"

/// The special effect task, owned by overlay 18.
extern const TaskHandle data_ov018_020ff3a0;

Battle* gBattle;

/// The sprite pack, animations and OBJ allocation of one field-object config.
typedef struct BattleObsAnimSet {
    /* 0x00 */ const SpriteAnimEntry* entries;
    /* 0x04 */ u16                    packIndex;
    /* 0x06 */ u16                    unk_06; // copied to SpriteAnimation.unk_20
    /* 0x08 */ u16                    unk_08; // copied to SpriteAnimation.unk_1E, in 32-byte units
    /* 0x0A */ u16                    unk_0A;
} BattleObsAnimSet;                           // Size: 0xC

/// The settings of one field-object config.
typedef struct BattleObsConfig {
    /* 0x00 */ u16         kind;      // task kind, 3 = the overlay 18 task
    /* 0x02 */ u16         sparkSize; // hit spark size, see BtlHit_LoadSpark
    /* 0x04 */ SndMgrSeIdx seIdx;     // landing sound
    /* 0x08 */ s16         radius;    // hit box, pixels
    /* 0x0A */ s16         height;
    /* 0x0C */ s32         gravity;
    /* 0x10 */ s32         linearDampingStep;
    /* 0x14 */ s32         dampingScale;
    /* 0x18 */ s32         bounceScale;
    /* 0x1C */ s16         maxHp;         // impacts the object survives
    /* 0x1E */ s16         impactPercent; // damage of the object's impact attack, percent
} BattleObsConfig;                        // Size: 0x20

static const SpriteAnimEntry BtlObsMgr_Anims11[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry BtlObsMgr_Anims12[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry BtlObsMgr_Anims10[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry BtlObsMgr_Anims13[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry BtlObsMgr_Anims14[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
};

static const SpriteAnimEntry BtlObsMgr_Anims23[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry BtlObsMgr_Anims22[2] = {
    {1, 3, 2, 0},
    {1, 3, 2, 0},
};

static const SpriteAnimEntry BtlObsMgr_Anims06[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims18[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims09[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims05[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims03[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims02[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims16[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims00[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims01[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims15[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims07[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims17[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims04[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims19[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims20[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims21[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

static const SpriteAnimEntry BtlObsMgr_Anims08[3] = {
    {1, 3, 2, 0},
    {1, 3, 2, 1},
    {1, 3, 2, 2},
};

/// Sprite setup per field-object config.
const BattleObsAnimSet BtlObsMgr_AnimSets[24] = {
    {BtlObsMgr_Anims00,  1, 4,  16, 0},
    {BtlObsMgr_Anims01,  2, 4,  28, 0},
    {BtlObsMgr_Anims02,  3, 4,  24, 0},
    {BtlObsMgr_Anims03,  4, 4,  40, 0},
    {BtlObsMgr_Anims04,  5, 4,  48, 0},
    {BtlObsMgr_Anims05,  6, 4,  48, 0},
    {BtlObsMgr_Anims06,  7, 4,  40, 0},
    {BtlObsMgr_Anims07,  8, 4,  80, 0},
    {BtlObsMgr_Anims08,  9, 4,  84, 0},
    {BtlObsMgr_Anims09, 10, 4, 128, 0},
    {BtlObsMgr_Anims10, 11, 4, 152, 0},
    {BtlObsMgr_Anims11, 12, 4, 156, 0},
    {BtlObsMgr_Anims12, 13, 4, 156, 0},
    {BtlObsMgr_Anims13, 14, 4, 212, 0},
    {BtlObsMgr_Anims14, 15, 4, 240, 0},
    {BtlObsMgr_Anims15, 16, 4,  68, 0},
    {BtlObsMgr_Anims16, 17, 4,  76, 0},
    {BtlObsMgr_Anims17, 18, 4,  96, 0},
    {BtlObsMgr_Anims18, 19, 4,  44, 0},
    {BtlObsMgr_Anims19, 20, 4,  52, 0},
    {BtlObsMgr_Anims20, 21, 4,  52, 0},
    {BtlObsMgr_Anims21, 22, 4,  52, 0},
    {BtlObsMgr_Anims22, 23, 4,  36, 0},
    {BtlObsMgr_Anims23, 24, 4,  24, 0},
};

/// Every field-object config.
static const BattleObsConfig BtlObsMgr_Configs[25] = {
    {0, 2, SEIDX_SE_OBJ_SS01, 12, 32, 696,   82, 3891, 3277,  0, 100},
    {2, 2,  SEIDX_SE_OBJ_S01, 13, 32, 696,   90, 3891, 2458,  0, 170},
    {2, 2,  SEIDX_SE_OBJ_S01, 14, 41, 696,   94, 3891, 2458,  0, 150},
    {2, 2,  SEIDX_SE_OBJ_S01, 17, 53, 696,   98, 3891, 2458,  0, 210},
    {2, 2,  SEIDX_SE_OBJ_S01, 25, 37, 696,  102, 3891, 2458,  0, 190},
    {2, 3,  SEIDX_SE_OBJ_S01, 24, 37, 696,  115, 3891, 2458,  0, 200},
    {2, 3,  SEIDX_SE_OBJ_S01, 16, 52, 696,  123, 3891, 2458,  0, 250},
    {2, 3,  SEIDX_SE_OBJ_S01, 25, 69, 696,  131, 3891, 2458,  0, 300},
    {2, 3,  SEIDX_SE_OBJ_M01, 25, 64, 696,  328, 3891, 2048, 16, 320},
    {2, 3,  SEIDX_SE_OBJ_L01, 29, 64, 696,  410, 3277, 2048, 12, 380},
    {1, 5,  SEIDX_SE_OBJ_L01, 61, 65, 819,  819, 2048, 1638, 10, 400},
    {1, 5,  SEIDX_SE_OBJ_L01, 70, 66, 819,  901, 2048, 1638, 10, 430},
    {1, 5,  SEIDX_SE_OBJ_L01, 72, 65, 819,  983, 2048, 1638, 10, 440},
    {1, 5,  SEIDX_SE_OBJ_L01, 87, 72, 819, 1065, 2048, 1638, 10, 500},
    {1, 5,  SEIDX_SE_OBJ_M01, 76, 89, 819, 1147, 2048, 1638, 10, 550},
    {2, 3,  SEIDX_SE_OBJ_M01, 34, 50, 696,  164, 3277, 1638, 12, 270},
    {2, 3,  SEIDX_SE_OBJ_M01, 42, 52, 696,  164, 3277, 1638, 12, 310},
    {2, 3,  SEIDX_SE_OBJ_S01, 52, 54, 696,  205, 3277, 1638, 12, 350},
    {2, 2,  SEIDX_SE_OBJ_S01, 32, 36, 696,  102, 3891, 2867, 16, 220},
    {2, 2,  SEIDX_SE_OBJ_S01, 34, 43, 696,  102, 3891, 2867, 16, 225},
    {2, 2,  SEIDX_SE_OBJ_S01, 34, 41, 696,  102, 3891, 2867, 16, 230},
    {2, 2,  SEIDX_SE_OBJ_S01, 36, 48, 696,  102, 3891, 2867, 20, 260},
    {1, 2,  SEIDX_SE_OBJ_S01, 12, 64, 696,   94, 3891, 3072,  0, 150},
    {1, 2,  SEIDX_SE_OBJ_S01, 12, 45, 696,   82, 3891, 3277,  0, 100},
    {3, 4,  SEIDX_SE_OBJ_M01, 32, 30, 655,  143, 3789, 2048,  0, 300},
};

/// The config each field-object group drops; groups with several pick one at random.
const u8 BtlObsMgr_DropKinds[24] = {
    0x0D, 0x0E, 0x0B, 0x0C, 0x0A, 0x0F, 0x10, 0x11, 0x07, 0x08, 0x12, 0x13,
    0x14, 0x15, 0x03, 0x06, 0x01, 0x05, 0x02, 0x04, 0x00, 0x16, 0x00, 0x00,
};

void BtlObsMgr_DropAtRandom(u16 configId) {
    Battle* global = gBattle;
    s32     height = global->stages[0].height;
    s32     width  = global->stages[0].width;
    s32     y      = RNG_Next((u32)(height << 4) >> 16);
    s32     x      = RNG_Next((u32)(width << 4) >> 16);

    BtlObsMgr_Spawn(configId, x << 12, y << 12);
}

void BtlObsMgr_DropGroup(s32 group, s32 count) {
    s32 index = 20;

    switch (group) {
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
            index = 14;
            break;
        case 7:
            index = 15;
            break;
        case 8:
            index = 16;
            break;
        case 9:
            index = 17;
            break;
        case 10:
            index = 18;
            break;
        case 11:
            index = 19;
            break;
        case 12:
            index = 20;
            break;
        case 13:
            index = 21;
            break;
    }

    s32 i = 0;
    if (count > 0) {
        u8 id = BtlObsMgr_DropKinds[index];
        do {
            BtlObsMgr_DropAtRandom(id);
            i++;
        } while (i < count);
    }
}

// Nonmatching: regswaps
s32 BtlObsMgr_LoadSprite(DisplayEngine engine, BattleSprite* sprite, u32 configId) {
    SpriteAnimationEx anim;
    int               packIndex;
    if (configId >= 24) {
        return 0;
    }
    const BattleObsAnimSet* set = &BtlObsMgr_AnimSets[configId];
    BtlSprite_InitAnim(&anim.anim, engine, &Iden_BtlObs);
    packIndex                     = set->packIndex;
    anim.anim.bits_10_11          = 1;
    anim.anim.packIndex           = packIndex;
    anim.anim.unk_18              = 2;
    u16                    unk_20 = set->unk_06;
    const SpriteAnimEntry* entry  = set->entries;
    anim.anim.unk_1C              = entry->charDataIndex;
    anim.anim.unk_1E              = set->unk_08 << 5;
    anim.anim.unk_26              = entry->cellDataIndex;
    u16 animData                  = entry->animDataIndex;
    anim.anim.unk_20              = unk_20;
    anim.anim.unk_28              = animData;
    anim.anim.animIndex           = entry->animIndex + 1;
    anim.animTable                = set->entries;
    return BtlSprite_Load(sprite, &anim);
}

void BtlObsMgr_LoadHitEffect(DisplayEngine engine, BattleSprite* sprite, s32 configId) {
    BtlHit_LoadSpark(engine, sprite, BtlObsMgr_Configs[configId].sparkSize);
}

void BtlObsMgr_InitActor(BattleActor* actor, u16 configId) {
    const BattleObsConfig* cfg = &BtlObsMgr_Configs[configId];

    BtlActor_Init(actor, ACTOR_KIND_OBJECT);
    actor->radius            = cfg->radius;
    actor->height            = cfg->height;
    actor->zGravity          = cfg->gravity;
    actor->linearDampingStep = cfg->linearDampingStep;
    actor->dampingScale      = cfg->dampingScale;
    actor->bounceScale       = cfg->bounceScale;
    actor->impactPercent     = cfg->impactPercent;
    actor->currentHp = actor->maxHp = cfg->maxHp;
}

void BtlObsMgr_PlaySe(DisplayEngine engine, s32 configId, s32 x) {
    SndMgrSeIdx seIdx = BtlObsMgr_Configs[configId].seIdx;

    if (engine == DISPLAY_MAIN) {
        BtlSe_PlayAtMainX(seIdx, x);
    } else {
        BtlSe_PlayAtSubX(seIdx, x);
    }
}

BOOL func_ov003_0208656c(void) {
    if ((gBattle->flags & BATTLE_FLAG_FEAT) == FALSE) {
        return TRUE;
    }
    return FALSE;
}

void BtlObsMgr_Spawn(u16 configId, s32 x, s32 y) {
    s32 taskId;

    BtlObsMgr_SpawnTask(configId, x, y, &taskId);
}

BattleObsActor* BtlObsMgr_SpawnTask(u16 configId, s32 x, s32 y, s32* taskId) {
    BattleObsSpawn    spawn;
    const TaskHandle* handle;

    spawn.kind     = BtlObsMgr_Configs[configId].kind;
    spawn.configId = configId;
    spawn.x        = x;
    spawn.y        = y;

    switch (spawn.kind) {
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
    *taskId = EasyTask_CreateTask(&gBattle->mainPool, handle, NULL, 0, NULL, &spawn);
    return BtlList_GetTail(&gBattle->obsList);
}

void BtlObsMgr_Defeat(BattleObsActor* obs) {
    obs->actor.currentHp = 0;
    BtlActor_SetPendingCommand(&obs->actor, ACTOR_CMD_DEFEAT);
    obs->actor.flags |= ACTOR_FLAG_DEFEATED;
}

void BtlObsMgr_Setup(BattleObsActor* obs, BattleObsSpawn* spawn) {
    BtlObsMgr_InitActor(&obs->actor, spawn->configId);
    obs->kind             = spawn->kind;
    obs->configId         = spawn->configId;
    obs->actor.position.x = spawn->x;
    obs->actor.position.y = spawn->y;
    obs->actor.position.z = 0;
    BtlCollision_Add(&obs->body.collider, BTLCOLLISION_LAYER_EFFECT, obs->actor.radius, obs->actor.height);
    BtlList_Append(&gBattle->obsList, &obs->node, obs);
}

void BtlObsMgr_Update(BattleObsActor* obs) {
    s16 radius;
    s32 halfRadius;

    BtlActor_UpdatePhysics(&obs->actor);
    if (!(obs->actor.flags & ACTOR_FLAG_HELD)) {
        radius     = obs->actor.radius;
        halfRadius = I2F(radius / 2);
        if (BtlStage_ClampXToField(&obs->actor.position.x, I2F(radius))) {
            obs->actor.xVelocity = 0;
        }
        if (BtlStage_ClampYToFloor(&obs->actor.position.y, halfRadius, obs->actor.position.x)) {
            obs->actor.yVelocity = 0;
        }
    }
    BtlCollision_SetPosition(&obs->body.collider, obs->actor.position.x, obs->actor.position.y, obs->actor.position.z);
}

void BtlObsMgr_Remove(BattleObsActor* obs) {
    BtlCollision_Remove(&obs->body.collider);
    BtlList_Remove(&gBattle->obsList, &obs->node);
}

void BtlObsMgr_ResetList(void) {
    BtlList_Init(&gBattle->obsList);
}

void BtlObsMgr_RollDrop(void) {
    const BattleObsSpawnCmd* commands = NULL;
    BattleStage*             stage    = &gBattle->stages[0];

    if (CombatAbility_IsActive(5)) {
        return;
    }

    switch (gBattle->encounterId) {
        case 7:
        case 8:
            BtlObsMgr_DropAtRandom(24);
            return;
    }

    s16       roll    = RNG_Next(100);
    s32       index   = 0;
    const u8* weights = BtlObsSpawn_EncounterWeights[gBattle->encounterId];
    do {
        roll -= *weights;
        if (roll < 0) {
            break;
        }
        index++;
        weights++;
    } while (index < 4);

    if (index < 4) {
        if (stage->fieldId < 41) {
            commands = BtlObsSpawn_FieldLists[BtlField_UG_Defs[stage->fieldId].obsGroup][index];
        }
        if (commands != NULL) {
            while (commands->kind != 0xFF) {
                BtlObsMgr_DropGroup(commands->kind, commands->count);
                commands++;
            }
        }
    }
}