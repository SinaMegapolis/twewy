#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatCutin.h"
#include "Combat/Core/CombatEnemyGroup.h"
#include "EasyFade.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/EasyTask.h"
#include "Engine/Math/Random.h"
#include "Engine/Math/Rect.h"
#include "Save.h"
#include "SndMgr.h"
#include "main_02026528.h"

#include <nitro/fx.h>
#include <nitro/mi/cpumem.h>

// ---------------------------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------------------------

/// The 3D motion track of a noise pair: an engine id plus two 20.12 positions.
/// `BtlCollision_Add` seeds it, `BtlCollision_SetPosition` retargets it and
/// `BtlCollision_ApplyPush` integrates the offsets at 0x20/0x24 into the actor position.

/// The per-actor tuning record fetched from `CombatEnemyParam_Table[group][sprite]`.

/// Four entries of `data_ov003_020e071c`/`data_ov003_020e0720` (0xC bytes per actor group).
typedef struct Ov003CutinCmdIds {
    /* 0x0 */ s32 main;
    /* 0x4 */ s32 sub;
    /* 0x8 */ s32 extra;
} Ov003CutinCmdIds; // Size: 0xC

/// The four adjacent .bss objects at 0x020e71e4..0x020e7204, as one block.

/// The engine-wide block at 0x02071cf0; only the field this unit polls is named.
typedef struct Ov02071cf0State {
    /* 0x000 */ u8  unk_000[0x130];
    /* 0x130 */ s32 unk_130;
} Ov02071cf0State;

// ---------------------------------------------------------------------------------------------
// Data owned by this unit
// ---------------------------------------------------------------------------------------------

static s32 func_ov003_020c59cc(Ov003EnemyActor* actor);
static s32 func_ov003_020c59e0(Ov003EnemyActor* actor);

static const u32 data_ov003_020e071c[1]    = {0x56C};
static const u32 data_ov003_020e0720[1]    = {0x18F};
static const u32 data_ov003_020e0724[0xB5] = {
    0x191, 0x181, 0x188, 0x189, 0x192, 0x199, 0x19A, 0x19B, 0x1A0, 0x1A1, 0x1B1, 0x1A9, 0x1AA, 0x1C6, 0x1C7, 0x1C8, 0x1D2,
    0x1D4, 0x1D5, 0x1E6, 0x1E7, 0x1E8, 0x1F6, 0x1F7, 0x1F8, 0x204, 0x206, 0x207, 0x215, 0x216, 0x217, 0x21F, 0x221, 0x222,
    0x229, 0x22A, 0x22B, 0x238, 0x23A, 0x23B, 0x56C, 0x243, 0x244, 0x254, 0x255, 0x256, 0x266, 0x267, 0x268, 0x275, 0x276,
    0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275,
    0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277, 0x275, 0x276, 0x277,
    0x275, 0x276, 0x277, 0x297, 0x56C, 0x56C, 0x282, 0x283, 0x284, 0x56C, 0x1A9, 0x1AA, 0x56C, 0x1B9, 0x1BA, 0x56C, 0x56C,
    0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2,
    0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2F2, 0x56C, 0x56C, 0x2D6, 0x56C, 0x56C, 0x2D6, 0x56C, 0x56C, 0x2ED, 0x56C, 0x56C,
    0x2EE, 0x56C, 0x56C, 0x2ED, 0x56C, 0x56C, 0x56C, 0x56C, 0x2C5, 0x56C, 0x56C, 0x56C, 0x56C, 0x56C, 0x56C, 0x56C, 0x56C,
    0x56C, 0x56C, 0x56C, 0x56C, 0x2B7, 0x56C, 0x56C, 0x304, 0x56C, 0x56C, 0x29E, 0x56C, 0x56C, 0x29E, 0x56C, 0x56C, 0x2AC,
    0x56C, 0x56C, 0x2AC, 0x56C, 0x56C, 0x165, 0x56C, 0x56C, 0x56C, 0x56C, 0x56C,
};

u32 data_ov003_020e7204[0xA];
u32 data_ov003_020e722c[0xA];

// ---------------------------------------------------------------------------------------------
// Data owned by neighbouring units / arm9
// ---------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------------------

/// Resets a noise to its idle defaults (a fresh 0x1C0-byte actor block).
void func_ov003_020c41e8(Ov003CutinActor* actor) {
    MI_CpuSet(actor, 0U, 0x1C0U);
    BtlActor_Init(&actor->base.actor, ACTOR_KIND_ENEMY);
    actor->base.actor.flags |= ACTOR_FLAG_NO_HIT_EFFECT;
    actor->base.actor.maxHp     = 1;
    actor->base.actor.currentHp = 1;
    actor->base.group           = 0x3C;
    actor->base.unk_192         = 0x64;
    actor->base.unk_194         = 0x64;
    actor->base.flags196.raw    = (u16)((actor->base.flags196.raw & ~1) | 1);
    actor->base.flags196.raw    = (u16)(actor->base.flags196.raw | 0x3C);
    actor->base.unk_1B8         = 0x1000;
    actor->base.unk_1BC         = 0x1000;
}

static void func_ov003_020c4264(Ov003EnemyActor* actor) {}

/// Releases the main sprite's resources.
void func_ov003_020c4268(Ov003CutinActor* actor) {
    BtlSprite_SetPaletteMode(&actor->base.sprite, BTL_SPRITE_PALETTE_OWN);
}

void func_ov003_020c427c(Ov003EnemyActor* actor) {
    func_ov003_020c37bc(actor);
}

static void func_ov003_020c4288(Ov003EnemyActor* actor) {
    actor->actor.flags &= ~ACTOR_FLAG_HIDDEN;
    if (actor->sprite.paletteMode == BTL_SPRITE_PALETTE_FLASH) {
        BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_OWN);
    }
    actor->flags18C &= ~1;
    actor->unk_1B8 = 0x1000;
    actor->unk_1BC = 0x1000;
}

void func_ov003_020c42d4(Ov003CutinActor* actor) {
    MI_CpuSet(&actor->base.actor.hit.sourceX, 0U, 8U);
}

/// Random rise speed for the sub (0x19E) or main (0x198) partner.
s16 func_ov003_020c42ec(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        return (s16)(actor->unk_19E + RNG_Next((u32)actor->unk_1A0));
    }
    return (s16)(actor->unk_198 + RNG_Next((u32)actor->unk_19A));
}

/// Rolls the noise's hit chance for the active partner.
s32 func_ov003_020c4348(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        if ((s32)RNG_Next(0x64U) < (s32)actor->unk_1A2) {
            return 1;
        }
        return 0;
    }
    if ((s32)RNG_Next(0x64U) < (s32)actor->unk_19C) {
        return 1;
    }
    return 0;
}

/// Fills the sprite-load template `anim` from the actor's load parameters.
void func_ov003_020c439c(Ov003EnemyActor* actor, SpriteAnimationEx* anim) {
    const BattleSpriteDesc* params = func_ov003_020c3c6c(actor);
    const SpriteAnimEntry*  entry;

    BtlSprite_InitAnim(&anim->anim, actor->sprite.sprite.bits_0_1, params->iden);
    entry                 = &params->table[params->entry];
    anim->anim.bits_10_11 = 1;
    anim->anim.unk_1C     = entry->charDataIndex;
    anim->anim.unk_1E     = (s16)(params->charTiles << 5);
    anim->anim.unk_26     = entry->cellDataIndex;
    anim->anim.unk_28     = entry->animDataIndex;
    anim->anim.unk_20     = params->paletteEntry;
    anim->anim.animIndex  = (s16)(entry->animIndex + 1);
    anim->animTable       = params->table;
}

/// Same as `func_ov003_020c439c`, but without the frame offset/animation index overrides.
static void func_ov003_020c4428(Ov003CutinActor* actor, SpriteAnimationEx* anim) {
    const BattleSpriteDesc* params = func_ov003_020c3c6c(&actor->base);
    const SpriteAnimEntry*  entry  = &params->table[params->entry];

    BtlSprite_InitAnim(&anim->anim, actor->base.sprite.sprite.bits_0_1, params->iden);
    anim->anim.bits_10_11 = 0;
    anim->anim.unk_1C     = entry->charDataIndex;
    anim->anim.unk_26     = entry->cellDataIndex;
    anim->anim.unk_28     = entry->animDataIndex;
    anim->anim.unk_20     = params->paletteEntry;
    anim->anim.animIndex  = (s16)(entry->animIndex + 1);
    anim->animTable       = NULL;
}

static s32 func_ov003_020c44a4(Ov003EnemyActor* actor) {
    return 1;
}

/// Loads the main sprite; returns the sprite-load result (0 on failure).
s32 func_ov003_020c44ac(Ov003EnemyActor* actor) {
    SpriteAnimationEx anim;
    s32               result;

    func_ov003_020c439c(actor, &anim);
    anim.anim.bits_0_1 = 1;
    result             = BtlSprite_Load(&actor->sprite, &anim);
    if (result == 0) {
        return 0;
    }
    func_ov003_020c4264(actor);
    CombatHit_LoadSparkForUnit(1, &actor->subSprite, &actor->actor);
    func_ov003_020c4988(actor);
    func_ov003_020c44a4(actor);
    return result;
}

/// Loads the main sprite and seeds the actor's motion track.
s32 func_ov003_020c4520(Ov003EnemyActor* actor) {
    Ov003ActorParams* params = func_ov003_020c3c88(actor);
    SpriteAnimationEx anim;
    s32               result;

    func_ov003_020c439c(actor, &anim);
    anim.anim.bits_0_1 = 0;
    result             = BtlSprite_Load(&actor->sprite, &anim);
    if (result == 0) {
        return 0;
    }
    CombatHit_LoadSparkForUnit(0, &actor->subSprite, &actor->actor);
    func_ov003_020c49c8(actor);
    BtlCollision_Add(&actor->collider, 1, params->unk_03, params->unk_04);
    func_ov003_020c44a4(actor);
    return result;
}

/// Same as `func_ov003_020c4520`, using the secondary load template.
s32 func_ov003_020c45a4(Ov003CutinActor* actor) {
    Ov003ActorParams* params = func_ov003_020c3c88(&actor->base);
    SpriteAnimationEx anim;
    s32               result;

    func_ov003_020c4428(actor, &anim);
    anim.anim.bits_0_1 = 0;
    result             = BtlSprite_Load(&actor->base.sprite, &anim);
    if (result == 0) {
        return 0;
    }
    CombatHit_LoadSparkForUnit(0, &actor->base.subSprite, &actor->base.actor);
    func_ov003_020c49c8(&actor->base);
    BtlCollision_Add(&actor->base.collider, 1, params->unk_03, params->unk_04);
    func_ov003_020c44a4(&actor->base);
    return result;
}

void func_ov003_020c4628(Ov003EnemyActor* actor) {
    BtlActor_UpdateEffects(1, &actor->actor);
    if (actor->actor.zGravity == 0) {
        actor->actor.zVelocity = 0;
    }
    BtlActor_UpdatePhysics(&actor->actor);
    BtlSprite_Update(&actor->sprite);
    BtlSprite_Update(&actor->subSprite);
}

void func_ov003_020c4668(Ov003EnemyActor* actor) {
    s16 half;

    BtlActor_UpdateEffects(0, &actor->actor);
    if (actor->actor.zGravity == 0) {
        actor->actor.zVelocity = 0;
    }
    BtlActor_UpdatePhysics(&actor->actor);
    if (!(actor->flags18C & 4) && (actor->collider.flags != 0)) {
        BtlCollision_ApplyPush(&actor->collider, &actor->actor.position.x, &actor->actor.position.y, &actor->actor.position.z);
    }
    if (func_ov003_020c59f4(actor) == 0) {
        if (actor->actor.flags & ACTOR_FLAG_UNCLAMPED_X) {
            s16 v = actor->actor.radius;
            BtlStage_ClampYToFloor(&actor->actor.position.y, (v / 2) << 0xC, actor->actor.position.x);
        } else {
            s16 v = actor->actor.radius;
            BtlStage_ClampToFloor(&actor->actor.position.x, &actor->actor.position.y, v << 0xC, (v / 2) << 0xC);
        }
    }
    half = 0;
    BtlCollision_SetPosition(&actor->collider, actor->actor.position.x, actor->actor.position.y, actor->actor.position.z);
    BtlSprite_Update(&actor->sprite);
    BtlSprite_Update(&actor->subSprite);
}

/// Registers (or re-budgets) the sprite's OAM affine group from the actor's scale fields.
void func_ov003_020c4748(Ov003EnemyActor* actor) {
    s32 scaleX = actor->unk_1B8;
    s32 flip;

    if ((scaleX != 0x1000) || (actor->unk_1BC != 0x1000)) {
        flip                            = (actor->sprite.flags & BTL_SPRITE_FLAG_FLIPPED) ? 1 : 0;
        actor->sprite.sprite.unk_0A.raw = (u16)((actor->sprite.sprite.unk_0A.raw & ~0x3E0) |
                                                ((u32)(u16)OamMgr_AllocAffineGroup(&g_OamMgr[actor->sprite.sprite.bits_0_1],
                                                                                   0U, scaleX, actor->unk_1BC, flip)
                                                 << 0x1B) >>
                                                    0x16);
        actor->sprite.sprite.unk_0A.raw = (u16)((actor->sprite.sprite.unk_0A.raw & ~1) | 1);
        actor->sprite.sprite.unk_0A.raw = (u16)(actor->sprite.sprite.unk_0A.raw & ~2);
        return;
    }
    actor->sprite.sprite.unk_0A.raw &= ~0x3E0;
    actor->sprite.sprite.unk_0A.raw &= ~1;
    if (actor->sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        BtlSprite_SetFlip(&actor->sprite, 1);
    } else {
        BtlSprite_SetFlip(&actor->sprite, 0);
    }
}

/// True while the actor is hidden or either scale is (nearly) zero.
s32 func_ov003_020c4830(Ov003EnemyActor* actor) {
    s32 scale;

    if (actor->flags18C & 1) {
        return 1;
    }
    scale = actor->unk_1B8;
    if (scale < 0) {
        scale = -scale;
    }
    if (scale < 0x10) {
        return 1;
    }
    scale = actor->unk_1BC;
    if (scale < 0) {
        scale = -scale;
    }
    if (scale < 0x10) {
        return 1;
    }
    return 0;
}

void func_ov003_020c4878(void* arg) {
    Ov003CutinActor* actor = arg;

    if (func_ov003_020c4830(&actor->base) != 0) {
        return;
    }
    func_ov003_020c4748(&actor->base);
    BtlActor_Render(&actor->base.actor, &actor->base.sprite);
    CombatHit_DrawSparkOnActor(&actor->base.subSprite, &actor->base.actor);
}

void func_ov003_020c48b0(Ov003EnemyActor* actor) {
    s32 flags = actor->actor.flags;

    if (flags & ACTOR_FLAG_TOUCHED) {
        actor->actor.flags = flags & ~ACTOR_FLAG_TOUCHED;
    }
    if (func_ov003_020c4830(actor) != 0) {
        return;
    }
    func_ov003_020c4748(actor);
    BtlActor_Render(&actor->actor, &actor->sprite);
    CombatHit_DrawSparkOnActor(&actor->subSprite, &actor->actor);
}

void func_ov003_020c48fc(void* arg) {
    Ov003CutinActor* actor = arg;

    func_ov003_020c4ee0(&actor->base);
    func_ov003_020c4a34(&actor->base);
    func_ov003_020c4268(actor);
    BtlSprite_Release(&actor->base.sprite);
    BtlSprite_Release(&actor->base.subSprite);
}

void func_ov003_020c492c(Ov003EnemyActor* actor) {
    func_ov003_020c4f00(actor);
    func_ov003_020c4a74(actor);
    BtlSprite_Release(&actor->sprite);
    BtlSprite_Release(&actor->subSprite);
    BtlCollision_Remove(&actor->collider);
}

void func_ov003_020c495c(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        func_ov003_020c4988(actor);
        return;
    }
    func_ov003_020c49c8(actor);
}

void func_ov003_020c4988(Ov003EnemyActor* actor) {
    if (actor->flags18C & 2) {
        return;
    }
    actor->flags18C = (u16)(actor->flags18C | 2);
    BtlList_Append(&g_CombatGlobal->unk3D8AC, &actor->node, actor);
}

void func_ov003_020c49c8(Ov003EnemyActor* actor) {
    if (actor->flags18C & 2) {
        return;
    }
    actor->flags18C = (u16)(actor->flags18C | 2);
    BtlList_Append(&g_CombatGlobal->unk3D8A4, &actor->node, actor);
}

void func_ov003_020c4a08(Ov003CutinActor* actor) {
    if (func_ov003_020c37f8(&actor->base.sprite) != 0) {
        func_ov003_020c4a34(&actor->base);
        return;
    }
    func_ov003_020c4a74(&actor->base);
}

s32 func_ov003_020c4a34(Ov003EnemyActor* actor) {
    if (!(actor->flags18C & 2)) {
        return 0;
    }
    actor->flags18C = (u16)(actor->flags18C & ~2);
    return (s32)BtlList_Remove(&g_CombatGlobal->unk3D8AC, &actor->node);
}

s32 func_ov003_020c4a74(Ov003EnemyActor* actor) {
    if (!(actor->flags18C & 2)) {
        return 0;
    }
    actor->flags18C = (u16)(actor->flags18C & ~2);
    return (s32)BtlList_Remove(&g_CombatGlobal->unk3D8A4, &actor->node);
}

/// Faces the main sprite towards (1) or away from (0) the sub screen.
void func_ov003_020c4ab4(Ov003EnemyActor* actor, s32 arg1) {
    if (arg1 != 0) {
        BtlSprite_SetFlip(&actor->sprite, 1);
        actor->actor.isFlipped = 1;
        return;
    }
    BtlSprite_SetFlip(&actor->sprite, 0);
    actor->actor.isFlipped = 0;
}

void func_ov003_020c4af0(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        func_ov003_020c4b1c(actor);
        return;
    }
    func_ov003_020c4b5c(actor);
}

/// Declared `int`: noise callers treat it like an undeclared (implicitly `int`) function.
s32 func_ov003_020c4b1c(Ov003EnemyActor* actor) {
    if (actor->actor.position.x < g_CombatGlobal->partner->position.x) {
        func_ov003_020c4ab4(actor, 1);
        return;
    }
    func_ov003_020c4ab4(actor, 0);
}

void func_ov003_020c4b5c(Ov003EnemyActor* actor) {
    if (actor->actor.position.x < g_CombatGlobal->player->position.x) {
        func_ov003_020c4ab4(actor, 1);
        return;
    }
    func_ov003_020c4ab4(actor, 0);
}

void func_ov003_020c4b9c(Ov003CutinActor* actor) {
    if (actor->base.actor.position.x < g_CombatGlobal->partner->position.x) {
        func_ov003_020c4ab4(&actor->base, 0);
        return;
    }
    func_ov003_020c4ab4(&actor->base, 1);
}

void func_ov003_020c4bdc(Ov003CutinActor* actor) {
    if (actor->base.actor.position.x < g_CombatGlobal->player->position.x) {
        func_ov003_020c4ab4(&actor->base, 0);
        return;
    }
    func_ov003_020c4ab4(&actor->base, 1);
}

void func_ov003_020c4c1c(Ov003EnemyActor* actor) {
    if (actor->actor.position.x < g_CombatGlobal->stages[1].width / 2) {
        func_ov003_020c4ab4(actor, 1);
        return;
    }
    func_ov003_020c4ab4(actor, 0);
}

void func_ov003_020c4c5c(Ov003EnemyActor* actor) {
    if (actor->actor.position.x < g_CombatGlobal->stages[0].width / 2) {
        func_ov003_020c4ab4(actor, 1);
        return;
    }
    func_ov003_020c4ab4(actor, 0);
}

void func_ov003_020c4c9c(Ov003EnemyActor* actor) {
    if (actor->sprite.flags & BTL_SPRITE_FLAG_FLIPPED) {
        func_ov003_020c4ab4(actor, 0);
        return;
    }
    func_ov003_020c4ab4(actor, 1);
}

/// Plays a noise sound effect, ignoring the "no command" sentinel 0x56C.
void func_ov003_020c4cc4(Ov003EnemyActor* actor, s32 command) {
    if (command == 0x56C) {
        return;
    }
    func_ov003_020c3acc(actor->sprite.sprite.bits_0_1, command, actor->actor.position.x);
}

/// Returns 1 if `value` is already in `table`.
static s32 func_ov003_020c4cf0(s32 value, s32* table, s32 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        if (value == table[i]) {
            return 1;
        }
    }
    return 0;
}

/// Stores `value` into the first free slot of `table`.
static s32 func_ov003_020c4d20(s32 value, s32* table, s32 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        if (table[i] == 0) {
            table[i] = value;
            return 1;
        }
    }
    return 0;
}

/// Clears every slot of `table` holding `value`.
static void func_ov003_020c4d54(s32 value, s32* table, s32 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        if (value == table[i]) {
            table[i] = 0;
        }
    }
}

void func_ov003_020c4d84(void) {
    func_ov003_020c4cf0(0, (s32*)data_ov003_020e722c, (s32)g_Cutin->unk_02);
}

void func_ov003_020c4da8(void) {
    func_ov003_020c4cf0(0, (s32*)data_ov003_020e7204, (s32)g_Cutin->unk_03);
}

static s32 func_ov003_020c4dcc(s32 value) {
    func_ov003_020c4cf0(value, (s32*)data_ov003_020e722c, (s32)g_Cutin->unk_02);
}

static s32 func_ov003_020c4dec(s32 value) {
    return func_ov003_020c4cf0(value, (s32*)data_ov003_020e7204, (s32)g_Cutin->unk_03);
}

s32 func_ov003_020c4e0c(s32 value) {
    u8 partner = g_CombatGlobal->unk3D88D;

    if ((partner != 0) && (partner != 1) && (partner != 2)) {
        return 0;
    }
    if (func_ov003_020c4dcc(value) != 0) {
        return 1;
    }
    return func_ov003_020c4d20(value, (s32*)data_ov003_020e722c, (s32)g_Cutin->unk_02);
}

s32 func_ov003_020c4e7c(s32 value) {
    if (func_ov003_020c4dec(value) != 0) {
        return 1;
    }
    return func_ov003_020c4d20(value, (s32*)data_ov003_020e7204, (s32)g_Cutin->unk_03);
}

void func_ov003_020c4eb4(Ov003CutinActor* actor) {
    if (func_ov003_020c37f8(&actor->base.sprite) != 0) {
        func_ov003_020c4ee0(&actor->base);
        return;
    }
    func_ov003_020c4f00(&actor->base);
}

/// Retires the actor's sub-screen command slot.
void func_ov003_020c4ee0(Ov003EnemyActor* actor) {
    func_ov003_020c4d54((s32)actor, (s32*)data_ov003_020e722c, (s32)g_Cutin->unk_02);
}

/// Retires the actor's main-screen command slot.
void func_ov003_020c4f00(Ov003EnemyActor* actor) {
    func_ov003_020c4d54((s32)actor, (s32*)data_ov003_020e7204, (s32)g_Cutin->unk_03);
}

/// True if any actor on `list` has noise instance id `id`.
static s32 func_ov003_020c4f20(s32 id, BattleList* list) {
    Ov003CutinActor* entry = BtlList_GetHead(list);

    while (entry != NULL) {
        if (entry->base.group == id) {
            return 1;
        }
        entry = BtlList_GetNext(&entry->base.node);
    }
    return 0;
}

void func_ov003_020c4f60(s32 id) {
    func_ov003_020c4f20(id, &g_CombatGlobal->unk3D8AC);
}

void func_ov003_020c4f80(s32 id) {
    func_ov003_020c4f20(id, &g_CombatGlobal->unk3D8A4);
}

/// Knocks the actor out: zero HP, defeat command, "defeated" flag.
static void func_ov003_020c4fa0(Ov003EnemyActor* actor) {
    actor->actor.currentHp = 0;
    BtlActor_SetPendingCommand(&actor->actor, ACTOR_CMD_DEFEAT);
    actor->actor.flags |= ACTOR_FLAG_DEFEATED;
}

void func_ov003_020c4fc8(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        func_ov003_020c4ff4(actor);
        return;
    }
    func_ov003_020c5058(actor);
}

s32 func_ov003_020c4ff4(Ov003EnemyActor* actor) {
    if (actor->actor.flags & ACTOR_FLAG_DEFEATED) {
        return 0;
    }
    func_ov003_020c4fa0(actor);
    func_ov003_020c4ee0(actor);
    if (actor->partner != NULL) {
        func_ov003_020c4fa0(actor->partner);
        func_ov003_020c4f00(actor->partner);
    }
    actor->actor.invincibleTimer = 5;
    g_CombatGlobal->unk3D90A     = 5;
    return 1;
}

s32 func_ov003_020c5058(Ov003EnemyActor* actor) {
    if (actor->actor.flags & ACTOR_FLAG_DEFEATED) {
        return 0;
    }
    func_ov003_020c4fa0(actor);
    func_ov003_020c4f00(actor);
    if (actor->partner != NULL) {
        func_ov003_020c4fa0(actor->partner);
        func_ov003_020c4ee0(actor->partner);
    }
    actor->actor.invincibleTimer = 5;
    g_CombatGlobal->unk3D906     = 5;
    return 1;
}

void func_ov003_020c50bc(void) {
    Ov003CutinActor* entry;
    Ov003CutinActor* next;

    entry = BtlList_GetHead(&g_CombatGlobal->unk3D8A4);
    while (entry != NULL) {
        func_ov003_020c5058(&entry->base);
        entry = BtlList_GetNext(&entry->base.node);
    }
    next = BtlList_GetHead(&g_CombatGlobal->unk3D8AC);
    while (next != NULL) {
        func_ov003_020c4ff4(&next->base);
        next = BtlList_GetNext(&next->base.node);
    }
}

/// Pushes the actor's hit effects to the combo counters.
void func_ov003_020c5130(Ov003EnemyActor* actor) {
    if (gSaveData.unk_3130 == 1) {
        return;
    }
    BtlMgr_GainExp(actor->sprite.sprite.bits_0_1, actor->unk_18E);
    BtlMgr_GainPP(actor->sprite.sprite.bits_0_1, actor->unk_190);
    if (BtlMgr_HasAbility(actor->sprite.sprite.bits_0_1, 0x1D) != 0) {
        return;
    }
    func_ov003_020bb580(actor);
}

/// Nudges the actor one eighth of the way towards (x, y), clamped to +/-0x8000.
void func_ov003_020c51a4(Ov003CutinActor* actor, s32 x, s32 y) {
    s32 stepX = (x - actor->base.actor.position.x) / 8;
    s32 stepY = (y - actor->base.actor.position.y) / 8;

    if (stepX > 0x8000) {
        stepX = 0x8000;
    } else if (stepX < -0x8000) {
        stepX = -0x8000;
    }
    if (stepY > 0x8000) {
        stepY = 0x8000;
    } else if (stepY < -0x8000) {
        stepY = -0x8000;
    }
    actor->base.actor.position.x += stepX;
    actor->base.actor.position.y += stepY;
}

/// True if the actor and its partner swapped sides of the sub-screen centre.
s32 func_ov003_020c5220(Ov003EnemyActor* actor) {
    s32 centre = g_CombatGlobal->stages[1].width / 2;
    s32 x      = actor->actor.position.x;
    s32 lower  = centre - 0x10000;

    if ((x < lower) && (actor->unk_1AC > centre + 0x10000)) {
        return 1;
    }
    if ((x > centre + 0x10000) && (actor->unk_1AC < lower)) {
        return 1;
    }
    return 0;
}

/// True if the actor's box does not overlap any actor on the sub-screen list.
static s32 func_ov003_020c5288(Ov003EnemyActor* actor, s32 x, s32 y) {
    s32              hitbox[4];
    s32              other[4];
    s32              result;
    Ov003CutinActor* entry;
    s16              width;
    s16              width2;

    width     = actor->actor.radius;
    hitbox[3] = FX32_CONVERT(width);
    hitbox[2] = FX32_CONVERT(width) * 2;
    hitbox[1] = y - FX32_CONVERT(width) / 2;
    hitbox[0] = x - FX32_CONVERT(width);

    entry = BtlList_GetHead(&g_CombatGlobal->unk3D8AC);
    while (entry != NULL) {
        width2   = entry->base.actor.radius;
        other[3] = FX32_CONVERT(width2);
        other[2] = FX32_CONVERT(width2) * 2;
        other[1] = entry->base.unk_1B0 - FX32_CONVERT(width2) / 2;
        other[0] = entry->base.unk_1AC - FX32_CONVERT(width2);
        if (Rect_Intersect(&result, hitbox, other) != 0) {
            return 0;
        }
        entry = BtlList_GetNext(&entry->base.node);
    }
    return 1;
}

/// Counts the sub-screen noise, and how many of them are "large" (width > 0x40).
static void func_ov003_020c54e0(s32* over, s32* total) {
    Ov003CutinActor* entry;

    *over  = 0;
    *total = 0;
    entry  = BtlList_GetHead(&g_CombatGlobal->unk3D8AC);
    while (entry != NULL) {
        if (entry->base.actor.height > 0x40) {
            (*over)++;
        }
        (*total)++;
        entry = BtlList_GetNext(&entry->base.node);
    }
}

/// Picks a randomised rise speed for the noise, scaled by how crowded the sub screen is.
static void func_ov003_020c554c(Ov003EnemyActor* actor, s32* outX, s32* outY, u16 arg3) {
    s32 over;
    s32 total;
    s32 maxSpeed;
    s32 minSpeed;
    s32 speed;
    s32 centre;
    s32 size;
    u16 flags196;

    if (arg3 != 0) {
        maxSpeed = (g_CombatGlobal->stages[1].width / 4) >> 12;
    } else {
        maxSpeed = (g_CombatGlobal->stages[1].width / 4 * 3) >> 12;
    }
    flags196 = actor->flags196.raw;
    if ((((u32)flags196 << 0x19) >> 0x1F) != 0 && (((u32)flags196 << 0x18) >> 0x1F) == 0) {
        if (arg3 != 0) {
            *outX = FX32_CONVERT(maxSpeed + RNG_Next(0x14U));
        } else {
            *outX = FX32_CONVERT(maxSpeed - RNG_Next(0x14U));
        }
    } else if ((((u32)flags196 << 0x19) >> 0x1F) == 0 && (((u32)actor->flags196.raw << 0x18) >> 0x1F) != 0) {
        if (arg3 != 0) {
            *outX = FX32_CONVERT(maxSpeed - RNG_Next(0x14U));
        } else {
            *outX = FX32_CONVERT(maxSpeed + RNG_Next(0x14U));
        }
    } else {
        *outX = FX32_CONVERT((maxSpeed + RNG_Next(0x28U)) - 0x14);
    }

    func_ov003_020c54e0(&over, &total);
    size     = g_CombatGlobal->stages[1].height;
    centre   = actor->actor.radius / 2;
    speed    = ((size * over) / total) >> 0xC;
    minSpeed = size >> 0xC;
    if (speed < centre) {
        speed = centre;
    }
    if (speed > minSpeed) {
        speed = minSpeed;
    }
    if (actor->actor.height > 0x40) {
        *outY = (centre + RNG_Next((speed - centre) + 1)) << 0xC;
        return;
    }
    *outY = (speed + RNG_Next((minSpeed - speed) + 1)) << 0xC;
}

s32 func_ov003_020c5864(Ov003CutinActor* actor, s32* outX, s32* outY) {
    s32 attempts = 0;

    while (1) {
        func_ov003_020c554c(&actor->base, outX, outY, RNG_Next(2U));
        if (func_ov003_020c5288(&actor->base, *outX, *outY) != 0) {
            return 1;
        }
        attempts++;
        if (attempts >= 0x64) {
            return 0;
        }
    }
}

s32 func_ov003_020c58c8(Ov003CutinActor* actor, s32* outX, s32* outY, u16 arg3) {
    s32 attempts = 0;

    while (1) {
        func_ov003_020c554c(&actor->base, outX, outY, arg3);
        if (func_ov003_020c5288(&actor->base, *outX, *outY) != 0) {
            return 1;
        }
        attempts++;
        if (attempts >= 0x64) {
            return 0;
        }
    }
}

s32 func_ov003_020c5924(Ov003EnemyActor* actor, s32* out) {
    s32 attempts = 0;
    u16 dir;

    while (1) {
        dir = actor->unk_1AC > g_CombatGlobal->stages[1].width / 2;
        func_ov003_020c554c(actor, out, out + 1, dir);
        if (func_ov003_020c5288(actor, out[0], out[1]) != 0) {
            return 1;
        }
        attempts++;
        if (attempts >= 0x64) {
            return 0;
        }
    }
}

s32 func_ov003_020c59a0(Ov003EnemyActor* actor) {
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        return func_ov003_020c59cc(actor);
    }
    return func_ov003_020c59e0(actor);
}

static s32 func_ov003_020c59cc(Ov003EnemyActor* actor) {
    return actor->actor.flags & (ACTOR_FLAG_STAGGERED | ACTOR_FLAG_DEFEATED | ACTOR_FLAG_LAUNCHED);
}

static s32 func_ov003_020c59e0(Ov003EnemyActor* actor) {
    return actor->actor.flags &
           (ACTOR_FLAG_STAGGERED | ACTOR_FLAG_DEFEATED | ACTOR_FLAG_HELD | ACTOR_FLAG_THROWN | ACTOR_FLAG_LAUNCHED);
}

s32 func_ov003_020c59f4(Ov003EnemyActor* actor) {
    return actor->actor.flags & ACTOR_FLAG_HELD;
}

s32 func_ov003_020c5a00(BattleActor* target) {
    target->flags |= ACTOR_FLAG_HELD;
    return target->flags;
}

s32 func_ov003_020c5a14(BattleActor* target) {
    target->flags &= ~ACTOR_FLAG_HELD;
    return target->flags;
}

s32 func_ov003_020c5a28(Ov003EnemyActor* self, BattleActor* target) {
    if ((target->invincibleTimer > 0) || (target->flags & ACTOR_FLAG_HELD) || (target->flags & ACTOR_FLAG_DEFEATED) ||
        (target->flags & ACTOR_FLAG_INTANGIBLE))
    {
        return 0;
    }
    return 1;
}

s32 func_ov003_020c5a60(Ov003EnemyActor* self, BattleActor* target) {
    if ((target->invincibleTimer > 0) || (target->flags & ACTOR_FLAG_HELD) || (target->flags & ACTOR_FLAG_DEFEATED) ||
        (target->flags & ACTOR_FLAG_INTANGIBLE))
    {
        return 0;
    }
    return 1;
}

void func_ov003_020c5a98(Ov003EnemyActor* self, BattleActor* target) {
    target->holderId = 0;
    BtlActor_SetPendingCommand(target, ACTOR_CMD_RELEASE);
    BtlActor_ResetAttack(target);
}

void func_ov003_020c5ac0(Ov003EnemyActor* self, u16 attackId, BattleActor* target, s32 xVelocity, s32 yVelocity,
                         s32 zVelocity) {
    target->xVelocity = xVelocity;
    target->yVelocity = yVelocity;
    target->zVelocity = zVelocity;
    target->holderId  = 0;
    BtlActor_SetPendingCommand(target, ACTOR_CMD_RELEASE);
    CombatHit_ArmActor(attackId, &self->actor.baseArgs, target);
}

s32 func_ov003_020c5b0c(u16 id, Ov003EnemyActor* owner, s32 arg2) {
    return CombatHit_AttackCharacter(CombatHit_GetAttackInfo(id), &owner->actor.baseArgs, arg2);
}

s32 func_ov003_020c5b2c(u16 id, Ov003EnemyActor* owner, s32 x, s32 y, s32 z) {
    return CombatHit_AttackAt(CombatHit_GetAttackInfo(id), &owner->actor.baseArgs, x, y, z);
}

s32 func_ov003_020c5b64(u16 id, Ov003EnemyActor* owner, s32 x, s32 y) {
    return CombatHit_AttackAtScreen(CombatHit_GetAttackInfo(id), &owner->actor.baseArgs, x, y);
}

s32 func_ov003_020c5b8c(const Ov003AttackInfo* info, Ov003EnemyActor* owner, s32 x, s32 y, s32 z) {
    return CombatHit_AttackAt(info, &owner->actor.baseArgs, x, y, z);
}

s32 func_ov003_020c5ba4(u16 id, BattleAttackArgs* args, s32 arg2) {
    return CombatHit_AttackCharacter(CombatHit_GetAttackInfo(id), args, arg2);
}

s32 func_ov003_020c5bc4(u16 id, BattleAttackArgs* args, s32 x, s32 y, s32 z) {
    return CombatHit_AttackAt(CombatHit_GetAttackInfo(id), args, x, y, z);
}

/// Noise state machine: the actor swoops in, alternates facing, then fires its command.
s32 func_ov003_020c5bfc(Ov003EnemyActor* actor) {
    switch (actor->unk_1A8) {
        default:
            return 1;

        case 0:
            if (actor->unk_1A4 == 0) {
                actor->sprite.flags |= BTL_SPRITE_FLAG_OBJ_WINDOW;
                actor->actor.flags |= ACTOR_FLAG_HIDDEN;
                if ((func_ov003_020c37f8(&actor->sprite) != 0) && (actor->partner != NULL)) {
                    actor->unk_1A6 = actor->partner->unk_1A6;
                } else {
                    actor->unk_1A6 = RNG_Next(0x33U) + 0x64;
                }
                actor->unk_1B8 = 0;
                actor->unk_1BC = 0x1000;
                if (((actor->flags196.raw << 0x17) >> 0x1F) != 0) {
                    func_ov003_020c3ec0(&actor->actor);
                    actor->actor.flags |= ACTOR_FLAG_BOSS_GAUGE_HP;
                }
            }
            if (actor->unk_1A4 < actor->unk_1A6) {
                actor->unk_1A4++;
            } else {
                actor->unk_1A4 = 0;
                actor->unk_1A6 = 0x14;
                actor->unk_1A8++;
                func_ov003_020c4cc4(actor, ((Ov003CutinCmdIds*)data_ov003_020e071c)[actor->group].main);
            }
            return 1;

        case 1:
            if (actor->unk_1A6 > 0) {
                func_02026590(&actor->unk_1B8, 0x1000, (u16)actor->unk_1A6);
                actor->unk_1A6--;
            }
            if (actor->unk_1A6 <= 0) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            }
            return 1;

        case 2:
            if (actor->unk_1A4 < 0xA) {
                actor->actor.flags ^= ACTOR_FLAG_HIDDEN;
                actor->sprite.flags ^= BTL_SPRITE_FLAG_OBJ_WINDOW;
            } else {
                s32 flags = actor->actor.flags;
                if (actor->unk_1A4 % 3 == 0) {
                    actor->actor.flags = flags | ACTOR_FLAG_HIDDEN;
                    actor->sprite.flags |= BTL_SPRITE_FLAG_OBJ_WINDOW;
                } else {
                    actor->actor.flags = flags & ~ACTOR_FLAG_HIDDEN;
                    actor->sprite.flags &= ~BTL_SPRITE_FLAG_OBJ_WINDOW;
                }
            }
            if (actor->unk_1A4 <= 0x14) {
                actor->unk_1A4++;
            } else {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
                actor->actor.flags &= ~ACTOR_FLAG_HIDDEN;
                actor->sprite.flags &= ~BTL_SPRITE_FLAG_OBJ_WINDOW;
            }
            return 1;

        case 3:
            if (g_CombatGlobal->unk3D874 == 2) {
                if (CombatAbility_IsActive(0xF) != 0) {
                    CombatHit_ApplyAttack(actor->sprite.sprite.bits_0_1, &actor->actor, (u16)CombatAbility_GetValue(0xF));
                }
                return 0;
            }
            return 1;
    }
}

/// Second noise state machine: launch, rise, track the partner, then finish.
s32 func_ov003_020c5e98(Ov003EnemyActor* actor) {
    s32 target[2];
    u8  partner;

    if (((g_Cutin->flags.raw << 0x1E) >> 0x1F) != 0) {
        return 0;
    }
    partner = g_CombatGlobal->unk3D88D;
    if ((partner != 0) && (partner != 1) && (partner != 2)) {
        return 0;
    }
    switch (actor->unk_1A8) {
        default:
            return 1;

        case 0:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c4cc4(actor, 0x19);
            }
            if (actor->unk_1A4 <= 0x14) {
                actor->unk_1A4++;
            } else {
                actor->unk_1A4 = 0;
                actor->unk_1A6 = 0xA;
                actor->unk_1A8++;
                actor->actor.flags |= ACTOR_FLAG_INTANGIBLE | ACTOR_FLAG_HIDDEN;
            }
            return 1;

        case 1:
            if (actor->unk_1A6 > 0) {
                func_02026590(&actor->unk_1B8, 0x40, (u16)(actor->unk_1A6 / 5));
                func_02026590(&actor->unk_1BC, 0, (u16)actor->unk_1A6);
                actor->unk_1A6--;
            }
            if (actor->unk_1A6 <= 0) {
                actor->unk_1A4 = 0;
                actor->unk_1A6 = 0x14;
                actor->unk_1A8++;
                actor->flags18C |= 0x11;
            }
            return 1;

        case 2:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c5924(actor, target);
                actor->unk_1AC = target[0];
                actor->unk_1B0 = target[1];
                EasyTask_CreateTask(&g_CombatGlobal->taskPool, &BtlEff_SideMove, NULL, 0, NULL, actor);
                func_ov003_020c4cc4(actor, 0x1A);
            }
            if (actor->unk_1A4 < actor->unk_1A6) {
                func_02026590(&actor->actor.position.x, actor->unk_1AC, (u16)actor->unk_1A6);
                func_02026590(&actor->actor.position.y, actor->unk_1B0, (u16)actor->unk_1A6);
                SndMgr_UpdateSEPan(SEIDX_SE_QUICKJUMP02, BtlStage_ToScreenX(1, actor->actor.position.x));
                actor->unk_1A4++;
                return 1;
            }
            actor->actor.position.x = actor->unk_1AC;
            actor->actor.position.y = actor->unk_1B0;
            func_ov003_020c4b1c(actor);
            actor->unk_1A4 = 0;
            actor->unk_1A6 = 0xA;
            actor->unk_1A8++;
            actor->unk_1B8 = 0;
            actor->unk_1BC = 0;
            actor->flags18C &= ~0x10;
            if (actor->flags18C & 0x20) {
                return 0;
            }
            func_ov003_020c4cc4(actor, 0x1B);
            return 1;

        case 3:
            if (actor->unk_1A6 > 0) {
                func_02026590(&actor->unk_1BC, 0x1000, (u16)actor->unk_1A6);
                func_02026590(&actor->unk_1B8, 0x1000, (u16)actor->unk_1A6);
                actor->flags18C &= ~1;
                actor->unk_1A6--;
            }
            if (actor->unk_1A6 <= 0) {
                actor->actor.flags &= ~ACTOR_FLAG_INTANGIBLE;
                actor->actor.flags &= ~ACTOR_FLAG_HIDDEN;
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            }
            return 1;

        case 4:
            if (actor->unk_1A4 <= 0x14) {
                actor->unk_1A4++;
            } else {
                BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_OWN);
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            }
            return 1;

        case 5:
            if (g_CombatGlobal->unk3D874 == 2) {
                return 0;
            }
            return 1;
    }
}

/// Blinks the main sprite while the noise counts down.
s32 func_ov003_020c6230(Ov003EnemyActor* actor) {
    if (actor->unk_1A4 == 0) {
        func_ov003_020c4cc4(actor, 0x1C);
    }
    if ((actor->unk_1A4 / 5) % 2 == 0) {
        BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_FLASH);
    } else {
        BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_OWN);
    }
    if (actor->unk_1A4 > 0x13) {
        BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_OWN);
        return 0;
    }
    actor->unk_1A4++;
    return 1;
}

/// Attack noise state machine (main actor).
s32 func_ov003_020c62c4(Ov003EnemyActor* actor, u16 anim) {
    s32               target[2];
    Ov003ActorParams* params;
    s32               scale;

    params = func_ov003_020c3c88(actor);
    switch (actor->unk_1A8) {
        default:
            return 1;

        case 0:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c4cc4(actor, ((Ov003CutinCmdIds*)data_ov003_020e0720)[actor->group].main);
                func_ov003_020c4288(actor);
                func_ov003_020c38b8(&actor->sprite, anim, 1);
                BtlSprite_Restart(&actor->sprite);
                actor->actor.flags |= ACTOR_FLAG_STAGGERED;
                func_ov003_020c4ee0(actor);
                if (func_ov003_020c5220(actor) != 0) {
                    func_ov003_020c5924(actor, target);
                    actor->unk_1AC = target[0];
                    actor->unk_1B0 = target[1];
                }
                actor->unk_1A4++;
            }
            scale = params->unk_19 - actor->unk_1A4;
            if (scale < 0) {
                scale = 0;
            }
            func_02026590(&actor->actor.position.x, actor->unk_1AC, (u16)scale);
            func_02026590(&actor->actor.position.y, actor->unk_1B0, (u16)scale);
            func_02026590(&actor->actor.position.z, actor->unk_1B4, (u16)scale);
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 1:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)(anim + 1), 1);
            }
            scale = params->unk_19 - actor->unk_1A4;
            if (scale < 0) {
                scale = 0;
            }
            func_02026590(&actor->actor.position.x, actor->unk_1AC, (u16)scale);
            func_02026590(&actor->actor.position.y, actor->unk_1B0, (u16)scale);
            func_02026590(&actor->actor.position.z, actor->unk_1B4, (u16)scale);
            if ((actor->actor.position.x == actor->unk_1AC) && (actor->actor.position.y == actor->unk_1B0) &&
                (actor->actor.position.z == actor->unk_1B4) && (actor->actor.hit.stunTimer == 0))
            {
                actor->unk_1A4 = 0;
                if (actor->actor.periodicEffectMode == 4) {
                    actor->unk_1A8++;
                } else {
                    actor->unk_1A8 += 2;
                }
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 2:
            if (actor->unk_1A4 == 0) {
                actor->actor.flags &= ~ACTOR_FLAG_STAGGERED;
            }
            if (actor->actor.periodicEffectMode != 4) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 3:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)(anim + 2), 1);
            }
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                actor->actor.flags &= ~ACTOR_FLAG_STAGGERED;
                return 0;
            }
            actor->unk_1A4++;
            return 1;
    }
}

/// Attack noise state machine with the trailing command/sound wiring.
s32 func_ov003_020c65cc(Ov003EnemyActor* actor, u16 anim) {
    Ov003ActorParams* params;
    s32               scale;

    params = func_ov003_020c3c88(actor);
    switch (actor->unk_1A8) {
        default:
            return 1;

        case 0:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c4cc4(actor, ((Ov003CutinCmdIds*)data_ov003_020e0720)[actor->group].main);
                func_ov003_020c38b8(&actor->sprite, anim, 1);
                BtlSprite_Restart(&actor->sprite);
                actor->actor.flags |= ACTOR_FLAG_STAGGERED;
                if (actor->actor.xVelocity < 0) {
                    func_ov003_020c4ab4(actor, 1);
                }
                if (actor->actor.xVelocity > 0) {
                    func_ov003_020c4ab4(actor, 0);
                }
                actor->unk_1B4 = actor->actor.position.z;
                if (actor->actor.zVelocity == 0) {
                    actor->actor.linearDampingStep = 0x19A;
                    actor->actor.zGravity          = 0x400;
                    if (params->unk_19 > 0) {
                        scale = params->unk_19 << 0xC;
                        scale = (s32)(scale > 0 ? 0.5f + (f32)scale : (f32)scale - 0.5f);
                        if (actor->actor.position.z >= -scale) {
                            actor->actor.zGravity = 0;
                        }
                    }
                } else {
                    actor->actor.linearDampingStep = 0;
                    actor->actor.zGravity          = 0x400;
                }
                func_ov003_020c4f00(actor);
                func_ov003_020c3700(actor);
            }
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 1:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)(anim + 1), 1);
            }
            if (actor->actor.linearDampingStep != 0) {
                if ((actor->actor.xVelocity != 0) || (actor->actor.yVelocity != 0)) {
                    goto advancing;
                }
                goto landed;
            }
            if (actor->actor.zGravity != 0) {
                scale = actor->actor.position.z;
                if (scale >= actor->unk_1B4) {
                    scale = params->unk_19 << 0xC;
                    scale = (s32)(scale > 0 ? 0.5f + (f32)scale : (f32)scale - 0.5f);
                    if (actor->actor.position.z >= -scale) {
                        goto grounded;
                    }
                }
                goto advancing;
            }
        grounded:
            if (actor->actor.hit.stunTimer == 0) {
                actor->unk_1A4 = 0;
                if (actor->actor.periodicEffectMode == 4) {
                    actor->unk_1A8++;
                } else {
                    actor->unk_1A8 += 2;
                }
            } else {
                goto advancing;
            }
            return 1;

        landed:
            if (actor->actor.zGravity != 0) {
                scale = actor->actor.position.z;
                if (scale >= actor->unk_1B4) {
                    scale = params->unk_19 << 0xC;
                    scale = (s32)(scale > 0 ? 0.5f + (f32)scale : (f32)scale - 0.5f);
                    if (actor->actor.position.z >= -scale) {
                        goto grounded;
                    }
                }
            }
        advancing:
            actor->unk_1A4++;
            return 1;

        case 2:
            if (actor->unk_1A4 == 0) {
                actor->actor.flags &= ~ACTOR_FLAG_STAGGERED;
            }
            if (actor->actor.periodicEffectMode != 4) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 3:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)(anim + 2), 1);
                actor->actor.linearDampingStep = 0x19A;
                if (params->unk_19 > 0) {
                    actor->actor.zGravity = 0;
                } else {
                    actor->actor.zGravity = 0x400;
                }
            }
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                actor->actor.flags &= ~ACTOR_FLAG_STAGGERED;
                return 0;
            }
            actor->unk_1A4++;
            return 1;
    }
}

/// Third noise state machine: dash in, bounce and recover.
s32 func_ov003_020c6920(Ov003EnemyActor* actor, u16 anim, s32 arg2) {
    switch (actor->unk_1A8) {
        case 0:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c4cc4(actor, ((Ov003CutinCmdIds*)data_ov003_020e0720)[actor->group].main);
                actor->actor.flags |= ACTOR_FLAG_STAGGERED | ACTOR_FLAG_LAUNCHED;
                if (actor->actor.position.x > actor->actor.hit.sourceX) {
                    actor->actor.isFlipped = 0;
                }
                if (actor->actor.position.x < actor->actor.hit.sourceX) {
                    actor->actor.isFlipped = 1;
                }
                if (actor->actor.xVelocity < 0) {
                    func_ov003_020c4ab4(actor, 1);
                }
                if (actor->actor.xVelocity > 0) {
                    func_ov003_020c4ab4(actor, 0);
                }
                func_ov003_020c38b8(&actor->sprite, anim, 1);
                BtlSprite_Restart(&actor->sprite);
                func_ov003_020c4f00(actor);
                func_ov003_020c3700(actor);
                actor->actor.linearDampingStep = 0x19A;
                actor->actor.zGravity          = 0x400;
            }
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)((anim + arg2) - 2), 0);
                actor->sprite.sprite.animationMode = 1;
            }
            if ((actor->unk_1A4 > 0) && (actor->actor.position.z >= 0)) {
                actor->unk_1A4 = 0;
                if (actor->actor.periodicEffectMode == 4) {
                    actor->unk_1A8++;
                } else {
                    actor->unk_1A8 += 2;
                }
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 1:
            if (actor->unk_1A4 == 0) {
                actor->actor.flags &= ~(ACTOR_FLAG_STAGGERED | ACTOR_FLAG_LAUNCHED);
            }
            if (actor->actor.periodicEffectMode != 4) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 2:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)((anim + arg2) - 1), 1);
                actor->actor.flags &= ~(ACTOR_FLAG_STAGGERED | ACTOR_FLAG_LAUNCHED);
            }
            if ((SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) && (actor->actor.xVelocity == 0) &&
                (actor->actor.yVelocity == 0))
            {
                return 0;
            }
            actor->unk_1A4++;
            return 1;

        default:
            return 1;
    }
}

s32 func_ov003_020c6b8c(Ov003EnemyActor* actor, u16 anim) {
    return func_ov003_020c6920(actor, anim, 2);
}

void func_ov003_020c6b9c(Ov003CutinActor* actor, u16 anim) {
    func_ov003_020c6920(&actor->base, anim, 2);
}

s32 func_ov003_020c6bac(Ov003EnemyActor* actor, u16 anim) {
    return func_ov003_020c6920(actor, anim, 3);
}

void func_ov003_020c6bbc(Ov003CutinActor* actor, u16 anim) {
    func_ov003_020c65cc(&actor->base, anim);
}

s32 func_ov003_020c6bc8(Ov003EnemyActor* actor, u16 anim) {
    if (actor->unk_1A4 == 0) {
        func_ov003_020c38b8(&actor->sprite, anim, 0);
        actor->actor.flags |= ACTOR_FLAG_HELD;
        func_ov003_020c4f00(actor);
        if (actor->actor.periodicEffectMode == 4) {
            BtlActor_ClearPeriodicEffect(&actor->actor);
        }
        actor->unk_1A4++;
    }
    return 1;
}

s32 func_ov003_020c6c2c(Ov003EnemyActor* actor, u16 anim) {
    Ov003ActorParams* params;

    params = func_ov003_020c3c88(actor);
    switch (actor->unk_1A8) {
        default:
            return 1;

        case 0:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, anim, 0);
                actor->actor.flags |= ACTOR_FLAG_THROWN;
                func_ov003_020c4f00(actor);
                func_ov003_020c3700(actor);
            }
            CombatHit_ActorImpact(&actor->actor);
            if ((actor->unk_1A4 > 0) && (actor->actor.position.z >= 0)) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 1:
            if (actor->unk_1A4 == 0) {
                func_ov003_020c38b8(&actor->sprite, (u16)(anim + 1), 1);
                actor->actor.linearDampingStep = 0x19A;
                actor->actor.zGravity          = 0x400;
            }
            CombatHit_ActorImpact(&actor->actor);
            if ((actor->actor.xVelocity == 0) && (actor->actor.yVelocity == 0)) {
                actor->actor.flags &= ~ACTOR_FLAG_THROWN;
                actor->unk_1A4 = 0;
                if (actor->actor.periodicEffectMode == 4) {
                    actor->unk_1A8++;
                } else {
                    actor->unk_1A8 += 2;
                }
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 2:
            if (actor->actor.periodicEffectMode != 4) {
                actor->unk_1A4 = 0;
                actor->unk_1A8++;
            } else {
                actor->unk_1A4++;
            }
            return 1;

        case 3:
            if (SpriteMgr_IsAnimationFinished(&actor->sprite.sprite) != 0) {
                if (params->unk_19 > 0) {
                    actor->actor.zGravity = 0;
                }
                return 0;
            }
            actor->unk_1A4++;
            return 1;
    }
}

static void func_ov003_020c6e0c(Ov003EnemyActor* actor) {
    actor->flags18C = (u16)((actor->flags18C | 0x40) & ~0x20);
    func_ov003_020c4ee0(actor);
    func_ov003_020c4a34(actor);
    if (g_CombatGlobal->unk3D8A0 == actor) {
        g_CombatGlobal->unk3D8A0 = NULL;
    }
}

static void func_ov003_020c6e58(Ov003EnemyActor* actor) {
    func_ov003_020c4f00(actor);
}

/// Final noise sequence: camera fade-out and sprite release.
static s32 func_ov003_020c6e64(Ov003EnemyActor* actor) {
    if (actor->unk_1A4 == 0) {
        if (!(actor->flags18C & 0x1000)) {
            s32 hasPartner = ((actor->actor.flags & ACTOR_FLAG_KILLED) && (actor->partner != NULL)) ? 1 : 0;
            func_ov003_0209f444(actor->sprite.sprite.bits_0_1, actor->actor.position.x, actor->actor.position.y,
                                actor->actor.position.z, hasPartner);
            actor->sprite.flags |= BTL_SPRITE_FLAG_OBJ_WINDOW;
        }
        if (actor->actor.flags & ACTOR_FLAG_KILLED) {
            func_ov003_020c5130(actor);
        }
        func_ov003_020c4cc4(actor, ((Ov003CutinCmdIds*)data_ov003_020e0724)[actor->group].main);
    }
    if (actor->unk_1A4 == 1) {
        actor->flags18C |= 1;
    }
    if (actor->unk_1A4 == 0x3C) {
        return 0;
    }
    actor->unk_1A4++;
    return 1;
}

s32 func_ov003_020c6f40(Ov003EnemyActor* actor) {
    if (actor->unk_1A4 == 0) {
        func_ov003_020c6e0c(actor);
    }
    return func_ov003_020c6e64(actor);
}

s32 func_ov003_020c6f68(Ov003EnemyActor* actor) {
    if (actor->unk_1A4 == 0) {
        func_ov003_020c6e58(actor);
    }
    return func_ov003_020c6e64(actor);
}

/// Drives the end-of-noise teardown task until the fade completes.
s32 func_ov003_020c6f90(Ov003EnemyActor* actor) {
    s32 result;

    if (actor->unk_1A4 == 0) {
        func_ov003_020c9118(actor);
        data_ov003_020e6a30[4] = 1;
    }
    result = func_ov003_020c9338(actor);
    if ((func_ov003_020c9008() != 0) && (data_ov003_020e6a30[4] != 0)) {
        BtlSprite_Release(&actor->sprite);
        BtlSprite_Release(&actor->partner->sprite);
        data_ov003_020e6a30[4] = 0;
    }
    if (result == 0) {
        func_ov003_020c50bc();
    }
    if ((result == 0) && (EasyFade_IsFading() == 0)) {
        return 0;
    }
    actor->unk_1A4++;
    return 1;
}
s32 func_ov003_020c703c(Ov003EnemyActor* actor) {
    if (actor->flags196.bits.b8 && (actor->actor.flags & ACTOR_FLAG_KILLED)) {
        return func_ov003_020c6f90(actor);
    } else {
        return func_ov003_020c6f40(actor);
    }
}

s32 func_ov003_020c7070(Ov003EnemyActor* actor) {
    if (actor->flags196.bits.b8 && (actor->actor.flags & ACTOR_FLAG_KILLED)) {
        return func_ov003_020c6f90(actor);
    } else {
        return func_ov003_020c6f68(actor);
    }
}

/// One frame of an enemy's defeat animation, which plays animations `anim`, `anim + 1` and `anim + 2` in turn.
s32 func_ov003_020c70a4(Ov003CutinActor* actor, s32 anim) {
    switch (actor->base.unk_1A8) {
        case 0:
            if (actor->base.unk_1A4 == 0) {
                BtlSprite_SetAnimFromTable(&actor->base.sprite, anim, 1);
                BtlSprite_Restart(&actor->base.sprite);
                if (actor->base.actor.hit.sourceX == 0) {
                    func_ov003_020c4af0(&actor->base);
                } else {
                    if (actor->base.actor.position.x > actor->base.actor.hit.sourceX) {
                        actor->base.actor.isFlipped = 0;
                    }
                    if (actor->base.actor.position.x < actor->base.actor.hit.sourceX) {
                        actor->base.actor.isFlipped = 1;
                    }
                }
                actor->base.actor.flags |= ACTOR_FLAG_GUARDING;
            }
            if (SpriteMgr_IsAnimationFinished(&actor->base.sprite.sprite)) {
                actor->base.unk_1A4 = 0;
                actor->base.unk_1A8++;
            } else {
                actor->base.unk_1A4++;
            }
            break;
        case 1:
            if (actor->base.unk_1A4 == 0) {
                BtlSprite_SetAnimFromTable(&actor->base.sprite, (u16)(anim + 1), 0);
            }
            if (actor->base.unk_1A4 <= actor->base.unk_1A6) {
                actor->base.unk_1A4++;
            } else {
                actor->base.unk_1A4 = 0;
                actor->base.unk_1A8++;
            }
            break;
        case 2:
            if (actor->base.unk_1A4 == 0) {
                BtlSprite_SetAnimFromTable(&actor->base.sprite, (u16)(anim + 2), 1);
                actor->base.actor.flags &= ~ACTOR_FLAG_GUARDING;
            }
            if (SpriteMgr_IsAnimationFinished(&actor->base.sprite.sprite)) {
                return 0;
            }
            actor->base.unk_1A4++;
            break;
    }
    return 1;
}

/// Puts an actor back into its idle state after its defeat animation.
static void func_ov003_020c7224(Ov003EnemyActor* actor) {
    Ov003ActorParams* params = func_ov003_020c3c88(actor);

    actor->flags18C &= ~1;
    actor->flags18C &= ~0x10;
    actor->unk_1B8 = 0x1000;
    actor->unk_1BC = 0x1000;
    BtlSprite_SetPaletteMode(&actor->sprite, BTL_SPRITE_PALETTE_OWN);
    actor->sprite.flags &= ~BTL_SPRITE_FLAG_OBJ_WINDOW;
    actor->actor.flags &= ~ACTOR_FLAG_INTANGIBLE;
    actor->actor.flags &= ~ACTOR_FLAG_HIDDEN;
    actor->actor.zVelocity = 0;
    if (params->unk_19 > 0) {
        actor->actor.zGravity = 0;
    }
    actor->actor.xVelocity         = 0;
    actor->actor.yVelocity         = 0;
    actor->actor.linearDampingStep = 0x19A;
}

/// Starts an actor's death animation `a` (or `b` in the second variant) and waits for the transition.
s32 func_ov003_020c72b4(Ov003EnemyActor* actor, s32 a, s32 b) {
    if (actor->unk_1A4 == 0) {
        switch (BtlCutIn_GetSpecialKind()) {
            case 1:
                func_ov003_020c38b8(&actor->sprite, a, 0);
                break;
            case 2:
                func_ov003_020c38b8(&actor->sprite, b, 0);
                break;
            default:
                func_ov003_020c38b8(&actor->sprite, a, 0);
                break;
        }
        func_ov003_020c7224(actor);
        actor->unk_1A4++;
    }
    if (g_CombatGlobal->unk3D878 & 2) {
        return 1;
    }
    func_ov003_020c7224(actor);
    if (func_ov003_020c37f8(&actor->sprite) != 0) {
        actor->actor.position.x = actor->unk_1AC;
        actor->actor.position.y = actor->unk_1B0;
        actor->actor.position.z = actor->unk_1B4;
    }
    func_ov003_020c4af0(actor);
    return 0;
}
