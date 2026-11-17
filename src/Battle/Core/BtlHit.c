#include "Battle/Core/BtlHit.h"
#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlAttackInfo.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/BtlDbg.h"
#include "Combat/Core/BtlEff.h"
#include "Combat/Core/BtlPop.h"
#include "Engine/Math/Random.h"
#include "Save.h"

#include <nitro/fx.h>
#include <nitro/gx.h>
#include <nitro/mi/cpumem.h>

/// How a hit picks its target(s).
typedef enum BattleHitKind {
    BTL_HIT_KIND_ACTOR  = 1, // a given actor, `x` = the attack's origin x
    BTL_HIT_KIND_POINT  = 2, // a hit box around a field position
    BTL_HIT_KIND_SCREEN = 3, // a hit box around a screen position, 20.12 pixels
} BattleHitKind;

/// A hit being resolved.
typedef struct BattleHit {
    /* 0x00 */ const BattleAttackInfo* info;
    /* 0x04 */ BattleAttackArgs*       args;
    /* 0x08 */ u16                     kind;  // BattleHitKind
    /* 0x0A */ u16                     pad;
    /* 0x0C */ BattleActor*            actor; // the actor hit, for BTL_HIT_KIND_ACTOR and the single-character hits
    /* 0x10 */ s32                     x;
    /* 0x14 */ s32                     y;
    /* 0x18 */ s32                     z;
} BattleHit; // Size: 0x1C

/// A status effect an attack may inflict.
typedef struct BattleStatusEffect {
    /* 0x00 */ u16 kind;     // periodic effect mode: 1 = damage over time, 2 = attack scale, 3 = damage-taken scale, 4 = stun
    /* 0x02 */ s16 amount;   // percent: of max HP per tick (1), or the scale (2, 3)
    /* 0x04 */ s16 duration; // frames
    /* 0x06 */ s16 chance;   // percent
} BattleStatusEffect;        // Size: 0x8

/// Damage percentages for one difficulty.
typedef struct BattleDifficultyDamage {
    /* 0x00 */ s16 toEnemies; // dealt to enemies
    /* 0x02 */ s16 toTeam;    // dealt to the player team
} BattleDifficultyDamage;     // Size: 0x4

/// A knockback velocity
typedef struct BattleKnockback {
    /* 0x00 */ s32 xSpeed; // away from the knockback source
    /* 0x04 */ s32 zSpeed; // negative is upward
} BattleKnockback;         // Size: 0x8

/// The running special attack's level and character
extern u8 data_ov003_020e71d8;
extern u8 data_ov003_020e71d9;

/// Damage percentages per difficulty.
static const BattleDifficultyDamage BtlHit_DifficultyDamagePercents[4] = {
    {200,  50},
    {100, 100},
    { 61, 200},
    { 40, 200}
};
/// Damage percentage dealt to the player team per battle of the chain.
static const s16 BtlHit_ChainDamagePercents[16] = {100, 120, 140, 160, 180, 200, 220, 240,
                                                   260, 280, 300, 300, 300, 300, 300, 300};
/// Knockback per BattleAttackFlags.knockback strength.
static const BattleKnockback BtlHit_Knockbacks[16] = {
    {     0,       0},
    {0x2333,       0},
    {0x4800,       0},
    {0x1000,       0},
    {     0, -0x3000},
    {0x1000,       0},
    {0x1000, -0x3800},
    {0x1800, -0x5000},
    {     0, -0x7000},
    {     0, -0x4000},
    {0xA000,       0},
    {0x0800, -0x2000},
    {0x1000, -0x5000},
    {0x1000,  0x0800},
    {0x2000, -0x8000},
    {0x1000, -0x3800},
};
/// The status effects attacks inflict, indexed by BattleAttackFlags.statusEffect.
static const BattleStatusEffect BtlHit_StatusEffects[32] = {
    {2, 100,    0,   0},
    {2,  50, 1800, 100},
    {2,  50, 1800,  40},
    {2,  50,  900, 100},
    {2,  50,  900,  40},
    {3, 150,  600, 100},
    {3, 150,  600,  40},
    {3, 150,  300, 100},
    {3, 150,  300,  40},
    {1,   5, 1800,  50},
    {1,   5, 1800,  10},
    {1,   5,  900,  50},
    {1,   5,  900,  10},
    {4,   0,  360,  60},
    {4,   0,  360,  15},
    {4,   0,  180,  60},
    {4,   0,  180,  15},
    {2,  20, 1000, 100},
    {2,  20, 1000,  70},
    {2,  20, 1000,  50},
    {2,  20, 1000,  20},
    {3, 120,  500, 100},
    {3, 120,  500,  70},
    {3, 120,  500,  50},
    {3, 120,  500,  20},
    {1,   5, 1000, 100},
    {1,   5, 1000,  80},
    {1,   5, 1000,  50},
    {1,   5, 1000,  20},
    {4,   0,  360, 100},
    {4,   0,  360,  70},
    {4,   0,  360,  20},
};

static void            BtlHit_StartStatusEffect(BattleSide side, BattleActor* actor, u16 kind, s16 amount, s16 duration);
static BattleHitResult BtlHit_Resolve(BattleHit* hit);

/// Maps a hit-box radius in pixels to one of the six spark sizes.
static s32 BtlHit_GetSparkSize(s32 radius) {
    if (radius < 8) {
        return 0;
    } else if (radius < 12) {
        return 1;
    } else if (radius < 24) {
        return 2;
    } else if (radius < 32) {
        return 3;
    } else if (radius < 64) {
        return 4;
    } else {
        return 5;
    }
}

void BtlHit_LoadSpark(DisplayEngine engine, BattleSprite* sprite, s32 size) {
    if (engine == DISPLAY_MAIN) {
        BtlSprite_LoadDirectNoProto(0, sprite, &Iden_BtlMenu, 13, 15, 14, 61, size);
    } else {
        BtlSprite_LoadDirectNoProto(1, sprite, &Iden_BtlMenu, 13, 15, 14, 63, size);
    }
    BtlSprite_SetSortKey(sprite, 0x7FFFFFFF);
    sprite->flags |= BTL_SPRITE_FLAG_TRANSLUCENT;
}

void BtlHit_DrawSpark(BattleSprite* sprite, s32 x, s32 y) {
    s32 screenX;
    s32 screenY;

    if (sprite->sprite.bits_0_1 == 0) {
        screenX = BtlStage_ToScreenX(DISPLAY_MAIN, x);
        screenY = BtlStage_ToScreenY(DISPLAY_MAIN, y, 0);
    } else {
        screenX = BtlStage_ToScreenX(DISPLAY_SUB, x);
        screenY = BtlStage_ToScreenY(DISPLAY_SUB, y, 0);
    }
    BtlSprite_SetPosition(sprite, screenX, screenY);
    if (BtlHit_CanDrawSpark()) {
        BtlSprite_Render(sprite);
    }
}

void BtlHit_LoadSparkForActor(DisplayEngine engine, BattleSprite* sprite, BattleActor* actor) {
    BtlHit_LoadSpark(engine, sprite, BtlHit_GetSparkSize(actor->radius));
}

void BtlHit_DrawSparkOnActor(BattleSprite* sprite, BattleActor* actor) {
    if ((actor->flags & ACTOR_FLAG_HIDDEN) == FALSE) {
        BtlHit_DrawSpark(sprite, actor->position.x, actor->position.y);
    }
}

BOOL BtlHit_CanDrawSpark(void) {
    BOOL result = TRUE;

    if (gBattle->flags & BATTLE_FLAG_SPECIAL) {
        switch (data_ov003_020e71d9) {
            case 0:
                if (data_ov003_020e71d8 == 1 || data_ov003_020e71d8 == 2) {
                    result = FALSE;
                }
                break;
            case 1:
                result = FALSE;
                break;
            case 2:
                if (data_ov003_020e71d8 == 2) {
                    result = FALSE;
                }
                break;
        }
    }
    return result;
}

/// Posts a player attack's hit box to the debug overlay.
// Nonmatching: register allocation of the info/hit pointers
static void BtlHit_PostDebugBox(BattleHit* hit) {
    const BattleAttackInfo* info   = hit->info;
    DisplayEngine           engine = info->flags.bits.screen;

    if (engine == DISPLAY_MAIN) {
        u8 halfW = info->halfWidth;
        u8 halfH = info->halfHeight;
        if (hit->kind == BTL_HIT_KIND_SCREEN) {
            BtlDbg_PostRect(BTLDBG_KIND_SCREEN, 0, hit->x - I2F(halfW), hit->y - I2F(halfH), halfW * 2, halfH * 2);
        }
        BtlDbg_PostRect(BTLDBG_KIND_ATTACK, 0, hit->x - I2F(halfW), hit->y - I2F(halfH), halfW * 2, halfH * 2);
    }
}

/// Rolls ability 0x38 (while the side has lost half the team's maximum HP) to negate the damage.
static void BtlHit_RollDamageNegate(BattleSide side, s16* damage) {
    s16 taken;
    if (side != BATTLE_SIDE_PARTNER) {
        taken = gBattle->playerDamage;
    } else {
        taken = gBattle->partnerDamage;
    }

    if (taken >= gBattle->teamMaxHp / 2 && BtlMgr_HasAbility(side, 0x38)) {
        s16 chance = BtlMgr_GetAbilityValue(side, 0x38);
        if (RNG_Next(100) < chance) {
            *damage = 0;
        }
    }
}

/// Applies ability 0x3D (while the side has lost half the team's maximum HP): raises the damage dealt by its percentage.
static void BtlHit_ApplyPinchAttackUp(BattleSide side, s16* damage) {
    s16 taken;
    if (side != BATTLE_SIDE_PARTNER) {
        taken = gBattle->playerDamage;
    } else {
        taken = gBattle->partnerDamage;
    }

    if (taken >= gBattle->teamMaxHp / 2 && BtlMgr_HasAbility(side, 0x3D)) {
        s16 bonus = BtlMgr_GetAbilityValue(side, 0x3D);
        *damage += (s16)(*damage * bonus / 100);
    }
}

/// Applies ability 0x3E (while the side has lost half the team's maximum HP): lowers the damage taken by its percentage.
static void BtlHit_ApplyPinchDefenseUp(BattleSide side, s16* damage) {
    s16 taken;
    if (side != BATTLE_SIDE_PARTNER) {
        taken = gBattle->playerDamage;
    } else {
        taken = gBattle->partnerDamage;
    }

    if (taken >= gBattle->teamMaxHp / 2 && BtlMgr_HasAbility(side, 0x3E)) {
        s16 bonus = BtlMgr_GetAbilityValue(side, 0x3E);
        *damage -= (s16)(*damage * bonus / 100);
    }
}

#define BTL_HIT_PERCENT(value, pct) ((value) * (pct) / 100)

/// Applies the status-effect abilities: resistances on a player target, longer and likelier effects otherwise.
// Nonmatching: jump-table layout of the two variant switches
static void BtlHit_ApplyStatusAbilities(BattleSide side, s16* duration, s16* chance, u32 kind, BOOL onPlayer) {
    if (onPlayer) {
        switch (kind) {
            case 1:
                if (BtlMgr_HasAbility(side, 0x32)) {
                    *chance -= BtlMgr_GetAbilityValue(side, 0x32);
                }
                return;
            case 2:
                if (BtlMgr_HasAbility(side, 0x33)) {
                    *chance -= BtlMgr_GetAbilityValue(side, 0x33);
                }
                return;
            case 3:
                if (BtlMgr_HasAbility(side, 0x34)) {
                    *chance -= BtlMgr_GetAbilityValue(side, 0x34);
                }
                return;
            case 4:
                if (BtlMgr_HasAbility(side, 0x35)) {
                    *chance -= BtlMgr_GetAbilityValue(side, 0x35);
                }
                return;
            default:
                return;
        }
    } else {
        switch (kind) {
            case 1:
                if (BtlMgr_HasAbility(side, 0x14)) {
                    *duration *= BtlMgr_GetAbilityValue(side, 0x14);
                }
                if (BtlMgr_HasAbility(side, 0x18)) {
                    *chance += BtlMgr_GetAbilityValue(side, 0x18);
                }
                return;
            case 2:
                if (BtlMgr_HasAbility(side, 0x15)) {
                    *duration *= BtlMgr_GetAbilityValue(side, 0x15);
                }
                if (BtlMgr_HasAbility(side, 0x19)) {
                    *chance += BtlMgr_GetAbilityValue(side, 0x19);
                }
                return;
            case 3:
                if (BtlMgr_HasAbility(side, 0x16)) {
                    *duration *= BtlMgr_GetAbilityValue(side, 0x16);
                }
                if (BtlMgr_HasAbility(side, 0x1A)) {
                    *chance += BtlMgr_GetAbilityValue(side, 0x1A);
                }
                return;
            case 4:
                if (BtlMgr_HasAbility(side, 0x17)) {
                    *duration *= BtlMgr_GetAbilityValue(side, 0x17);
                }
                if (BtlMgr_HasAbility(side, 0x1B)) {
                    *chance += BtlMgr_GetAbilityValue(side, 0x1B);
                }
                return;
            default:
                return;
        }
    }
}

/// With the Technical EXP sticker, adds tech bonus points for hitting an airborne actor during a long combo.
static void BtlHit_AddAirComboBonus(BattleActor* actor) {
    if (Inventory_GetOwnedCount(ITEM_STICKER_TECHNICAL_EXP) && actor->position.z < -0x10000) {
        if (gBattle->comboCount >= 10) {
            if (gBattle->comboCount < 20) {
                gBattle->techBonus++;
            } else if (gBattle->comboCount < 30) {
                gBattle->techBonus += 2;
            } else {
                gBattle->techBonus += 3;
            }
        }
    }
}

/// Whether `actor` cannot be hit by this attack: it is impacting, defeated, intangible or (unless the attack pierces)
/// invincible.
static BOOL BtlHit_IsImmune(BattleActor* actor, const BattleAttackInfo* info) {
    if ((actor->flags & ACTOR_FLAG_IMPACTING) || (actor->flags & ACTOR_FLAG_DEFEATED) ||
        (actor->flags & ACTOR_FLAG_INTANGIBLE))
    {
        return TRUE;
    } else if (actor->invincibleTimer > 0 && info->flags.bits.piercing != 1) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/// If `actor` ignores hits, tells it a hit was ignored. Returns whether it ignores hits.
static BOOL BtlHit_CheckIgnoresHits(BattleActor* actor) {
    if ((actor->flags & ACTOR_FLAG_IGNORES_HITS) == FALSE) {
        return FALSE;
    } else {
        BtlActor_SetPendingCommand(actor, ACTOR_CMD_HIT_IGNORED);
        return TRUE;
    }
}

/// Whether the actor's hit volume overlaps the box of half-extents `halfW`/`halfH`/`halfD` around (`x`, `y`, `z`).
// Nonmatching: register allocation of the hit-volume bounds
static BOOL BtlHit_OverlapsBox(BattleActor* actor, s32 x, s32 y, s32 z, s32 halfW, s32 halfH, s32 halfD) {
    s16 radius  = actor->radius;
    s16 zRadius = actor->height;
    s32 posX    = actor->position.x;
    s32 posY    = actor->position.y;
    s32 posZ    = actor->position.z;

    if (posX - (radius << 12) > x + halfW) {
        return FALSE;
    }
    if (posX + (radius << 12) < x - halfW) {
        return FALSE;
    }
    if (posY - ((radius / 2) << 12) > y + halfH) {
        return FALSE;
    }
    if (posY + ((radius / 2) << 12) < y - halfH) {
        return FALSE;
    }
    if (posZ < z - halfD) {
        return FALSE;
    }
    if (posZ - (zRadius << 12) <= z + halfD) {
        return TRUE;
    }
    return FALSE;
}

/// Whether the actor's on-screen box overlaps the attack's hit box around the hit's screen position.
// Nonmatching: register allocation of the info/hit pointers
static BOOL BtlHit_OverlapsScreenBox(BattleActor* actor, BattleHit* hit) {
    const BattleAttackInfo* info = hit->info;
    s32                     hitX = hit->x * 16;
    s16                     screenY;
    s16                     hitY = hit->y >> 12;

    hitX >>= 16;
    if (actor->screenX - actor->radius > info->halfWidth + hitX) {
        return FALSE;
    }
    if (actor->screenX + actor->radius < hitX - info->halfWidth) {
        return FALSE;
    }
    screenY = actor->screenY;
    if (screenY - actor->height > info->halfHeight + hitY) {
        return FALSE;
    }
    if (screenY >= hitY - info->halfHeight) {
        return TRUE;
    }
    return FALSE;
}

/// Gives the actor its hit reaction: knocked down for strong knockback or when already down or airborne, otherwise
/// flinches unless `noFlinch`, the attack or the actor forbids it. Then starts its hit stun.
static void BtlHit_ApplyReaction(BattleActor* actor, const BattleAttackInfo* info, BOOL noFlinch) {
    if (actor->position.z < -0x10000 || actor->pendingCommand == ACTOR_CMD_KNOCKDOWN || (actor->flags & ACTOR_FLAG_LAUNCHED) ||
        info->flags.bits.knockback >= 6)
    {
        if (actor->periodicEffectMode == 4) {
            BtlActor_ClearPeriodicEffect(actor);
        }
        BtlActor_SetPendingCommand(actor, ACTOR_CMD_KNOCKDOWN);
    } else if (!noFlinch && info->flags.bits.noFlinch != 1 && !(actor->flags & ACTOR_FLAG_NO_FLINCH)) {
        BtlActor_SetPendingCommand(actor, ACTOR_CMD_HIT);
    }
    actor->hit.stunTimer = info->stunFrames;
    actor->hit.finisher  = info->flags.bits.finisher;
    actor->flags |= ACTOR_FLAG_HIT_FLASH;
}

/// For a finisher, asks for the aura to be passed to the other side, if the attacking side holds it (or no aura is
/// active yet).
static void BtlHit_PassAura(const BattleAttackInfo* info) {
    if (info->flags.bits.finisher != 0) {
        if (info->flags.bits.screen == 0) {
            if (gBattle->auraPasses == 0 || gBattle->auraOnPlayer != 0) {
                gBattle->auraPass = 1;
                gBattle->flags |= BATTLE_FLAG_AURA_SPLASH;
            }
        } else if (gBattle->auraPasses == 0 || gBattle->auraOnPartner != 0) {
            gBattle->auraPass = 2;
            gBattle->flags |= BATTLE_FLAG_AURA_SPLASH;
        }
    }
}

/// Records whether an enemy attack hit the partner from behind.
static void BtlHit_CheckFromBehind(BattleActor* actor, const BattleAttackInfo* info, s32 originX) {
    BOOL behind = FALSE;

    if (info->flags.bits.screen == 1 && info->flags.bits.hitsPlayers == 1) {
        if (actor->isFlipped == 0) {
            if (actor->position.x < originX) {
                behind = TRUE;
            }
        } else if (actor->position.x > originX) {
            behind = TRUE;
        }
    }
    actor->hit.fromBehind = behind ? TRUE : FALSE;
}

/// Knocks a hit or knocked-down actor away from the attack's origin (or from the screen's player).
static void BtlHit_ApplyKnockback(BattleActor* actor, const BattleAttackInfo* info, s32 originX) {
    s32 sourceX;
    u32 strength;
    s32 speed;
    s32 zSpeed;

    if (actor->pendingCommand != ACTOR_CMD_HIT && actor->pendingCommand != ACTOR_CMD_KNOCKDOWN) {
        return;
    }

    strength = info->flags.bits.knockback;
    speed    = BtlHit_Knockbacks[strength].xSpeed;
    zSpeed   = BtlHit_Knockbacks[strength].zSpeed;
    sourceX  = originX;
    if (info->flags.bits.pushFromPlayer == 1) {
        if (info->flags.bits.screen == 0) {
            sourceX = gBattle->player->position.x;
        } else {
            sourceX = gBattle->partner->position.x;
        }
    }
    if (actor->position.x < sourceX) {
        speed = -speed;
    }
    actor->xVelocity   = speed;
    actor->zVelocity   = zSpeed;
    actor->hit.sourceX = sourceX;
}

/// Rolls a status effect on a flinching actor, unless the enemy is immune to its kind.
static void BtlHit_RollStatusEffect(BattleSide side, BattleActor* actor, u16 statusEffect) {

    if ((actor->pendingCommand != ACTOR_CMD_HIT) || (statusEffect == 0)) {
        return;
    }

    const BattleStatusEffect* effect = &BtlHit_StatusEffects[statusEffect];
    if (actor->kind == ACTOR_KIND_ENEMY) {
        Ov003EnemyActor* enemy  = (Ov003EnemyActor*)actor;
        BOOL             immune = FALSE;

        switch (effect->kind) {
            default:
                break;
            case 1:
                if (enemy->flags196.bits.b2 == 1) {
                    immune = TRUE;
                }
                break;
            case 2:
                if (enemy->flags196.bits.b4 == 1) {
                    immune = TRUE;
                }
                break;
            case 3:
                if (enemy->flags196.bits.b5 == 1) {
                    immune = TRUE;
                }
                break;
            case 4:
                if (enemy->flags196.bits.b3 == 1) {
                    immune = TRUE;
                }
                break;
        }
        if (immune) {
            return;
        }
    }

    s16 chance;
    s16 duration = effect->duration;
    chance       = effect->chance;

    BtlHit_ApplyStatusAbilities(side, &duration, &chance, effect->kind, actor->kind == ACTOR_KIND_PLAYER);
    if (RNG_Next(10000) < chance * 100) {
        BtlHit_StartStatusEffect(side, actor, effect->kind, effect->amount, duration);
    }
}

/// Rolls the attack's status effect on the actor.
static void BtlHit_ApplyStatusEffect(BattleActor* actor, const BattleAttackInfo* info) {
    if (info->flags.bits.screen == 0) {
        BtlHit_RollStatusEffect(BATTLE_SIDE_PLAYER, actor, info->flags.bits.statusEffect);
    } else {
        BtlHit_RollStatusEffect(BATTLE_SIDE_PARTNER, actor, info->flags.bits.statusEffect);
    }
}

/// Spawns the attack's hit effect on the actor, placed per BattleAttackFlags.effectPlacement and facing away from the
/// attack's origin.
// Nonmatching: callee-saved register rotation of the position temporaries
static void BtlHit_SpawnHitEffect(BattleActor* actor, BattleHit* hit) {
    Vec pos;
    s32 targetX;

    if ((actor->flags & ACTOR_FLAG_NO_HIT_EFFECT) || (hit->info->flags.bits.effect == 26)) {
        return;
    }

    switch (hit->kind) {
        case BTL_HIT_KIND_SCREEN:
            if (hit->info->flags.bits.screen == 0) {
                pos.x = BtlStage_ToWorldX(DISPLAY_MAIN, (hit->x * 16) >> 16);
                pos.y = BtlStage_ToWorldY(DISPLAY_MAIN, (hit->y * 16) >> 16, actor->position.z);
            } else {
                pos.x = BtlStage_ToWorldX(DISPLAY_SUB, (hit->x * 16) >> 16);
                pos.y = BtlStage_ToWorldY(DISPLAY_SUB, (hit->y * 16) >> 16, actor->position.z);
            }
            pos.z   = actor->position.z;
            targetX = pos.x;
            break;

        case BTL_HIT_KIND_ACTOR:
            pos.x   = actor->position.x;
            pos.z   = actor->position.z - I2F(actor->centerHeight);
            pos.y   = actor->position.y;
            targetX = hit->x;
            break;

        case BTL_HIT_KIND_POINT:
            pos.x   = hit->x;
            pos.y   = hit->y;
            pos.z   = hit->z;
            targetX = pos.x;
            break;

        default:
            pos.x   = 0;
            pos.y   = 0;
            pos.z   = 0;
            targetX = pos.x;
            break;
    }

    switch (hit->info->flags.bits.effectPlacement) {
        case 1:
            break;
        case 2: {
            s32 radius = actor->radius;
            if (actor->position.x < pos.x) {
                pos.x = ((actor->position.x + I2F(radius)) + (pos.x - I2F(hit->info->halfWidth))) / 2;
            } else {
                pos.x = ((actor->position.x - I2F(radius)) + (pos.x + I2F(hit->info->halfWidth))) / 2;
            }

            s32 halfRadius = radius / 2;
            if (actor->position.y < pos.y) {
                pos.y = ((actor->position.y + I2F(halfRadius)) + (pos.y - I2F(hit->info->halfHeight))) / 2;
            } else {
                pos.y = ((actor->position.y - I2F(halfRadius)) + (pos.y + I2F(hit->info->halfHeight))) / 2;
            }

            pos.z = (pos.z + (actor->position.z - I2F(actor->centerHeight))) / 2;
        } break;

        default:
        case 0:
            pos.x = actor->position.x;
            pos.y = actor->position.y;
            pos.z = actor->position.z - I2F(actor->centerHeight);
            break;
    }

    if (hit->info->flags.bits.pushFromPlayer == TRUE) {
        if (hit->info->flags.bits.screen == 0) {
            targetX = gBattle->player->position.x;
        } else {
            targetX = gBattle->partner->position.x;
        }
    }

    s32 jitterX = pos.x + (I2F(RNG_Next(17) - 8));
    s32 jitterZ = pos.z + (I2F(RNG_Next(17) - 8));
    s32 flipped = (actor->position.x >= targetX) ? 1 : 0;
    func_ov003_0209ed70(hit->info->flags.bits.effect, jitterX, pos.y, jitterZ, flipped);
}

/// If the actor is guarding, blocks the attack: turns to face it, starts its invincibility, plays the guard effect
/// and knockback and freezes the attacker's screen. Returns whether the attack was guarded.
static BOOL BtlHit_TryGuard(BattleActor* actor, const BattleAttackInfo* info, s32 originX) {
    if (!(actor->flags & ACTOR_FLAG_GUARDING)) {
        return FALSE;
    }
    BtlActor_SetPendingCommand(actor, ACTOR_CMD_GUARD);
    if (!(gBattle->flags & BATTLE_FLAG_NO_GUARD_TURN)) {
        actor->isFlipped = (actor->position.x < originX) ? TRUE : FALSE;
    }
    actor->invincibleTimer = info->invincibleFrames;
    if (info->flags.bits.screen == 0) {
        func_ov003_0209e9f8(0, 0, actor->position.x, actor->position.y, actor->position.z - (actor->centerHeight << 12),
                            (actor->isFlipped != 0) ? 1 : 0);
        func_ov003_0209c984(DISPLAY_MAIN, 0, actor->position.x, actor->position.y,
                            actor->position.z - ((actor->height / 2) << 12));
    } else {
        func_ov003_0209e9f8(1, 0, actor->position.x, actor->position.y, actor->position.z - (actor->centerHeight << 12),
                            (actor->isFlipped != 0) ? 1 : 0);
        func_ov003_0209c984(DISPLAY_SUB, 0, actor->position.x, actor->position.y,
                            actor->position.z - ((actor->height / 2) << 12));
    }
    BtlHit_ApplyKnockback(actor, info, originX);
    if (info->flags.bits.screen == 0) {
        gBattle->hitStopMainNext = info->hitStop;
    } else {
        gBattle->hitStopSubNext = info->hitStop;
    }
    return TRUE;
}

/// The enemy's damage percentage against the attack's variant, recording the variant and the resulting affinity in
/// its hit state.
static s32 BtlHit_GetAffinityPercent(Ov003EnemyActor* enemy, const BattleAttackInfo* info) {
    s16 multiplier = 100;

    enemy->actor.hit.variant = info->flags.bits.variant;
    if (info->flags.bits.variant == 1) {
        multiplier = enemy->unk_192;
    }
    if (info->flags.bits.variant == 2) {
        multiplier = enemy->unk_194;
    }
    if (multiplier < 0) {
        enemy->actor.hit.affinity = HIT_AFFINITY_ABSORB;
    } else if (multiplier < 100) {
        enemy->actor.hit.affinity = HIT_AFFINITY_RESIST;
    } else if (multiplier > 100) {
        enemy->actor.hit.affinity = HIT_AFFINITY_WEAK;
    }
    return multiplier;
}

/// The damage an attack deals to an enemy, after its affinity, the aura, abilities and difficulty.
static s16 BtlHit_CalcEnemyDamage(Ov003EnemyActor* enemy, const BattleAttackInfo* info, const BattleAttackArgs* args) {
    s16 damage = BTL_HIT_PERCENT(args->power, info->power);
    s32 bonus;

    damage = BTL_HIT_PERCENT(damage, args->powerPercent);
    damage = BTL_HIT_PERCENT(damage, enemy->actor.damageTakenPercent);
    if (info->flags.bits.screen == 0) {
        if (gBattle->auraOnPlayer == 0 && info->flags.bits.piercing != 1 && enemy->flags196.bits.b9 == 1) {
            damage /= 4;
        }
        s16 multiplier = BtlHit_GetAffinityPercent(enemy, info);

        damage = BTL_HIT_PERCENT(damage, multiplier);
        if (info->flags.bits.finisher == 1 && gBattle->auraOnPlayer != 0) {
            damage = BTL_HIT_PERCENT(damage, gBattle->auraRate);
            func_ov003_0209c568(DISPLAY_MAIN);
            enemy->actor.hit.critical = TRUE;
        }
        BtlHit_ApplyPinchAttackUp(BATTLE_SIDE_PLAYER, &damage);
    } else {
        if (gBattle->auraOnPartner == 0 && info->flags.bits.piercing != 1 && enemy->flags196.bits.b9 == 1) {
            damage /= 4;
        }
        if (info->flags.bits.finisher == 1 && gBattle->auraOnPartner != 0) {
            damage = BTL_HIT_PERCENT(damage, gBattle->auraRate);
            func_ov003_0209c568(DISPLAY_SUB);
            enemy->actor.hit.critical = TRUE;
        }
        BtlHit_ApplyPinchAttackUp(BATTLE_SIDE_PARTNER, &damage);
    }
    damage = BTL_HIT_PERCENT(damage, BtlHit_DifficultyDamagePercents[gSaveData.battleDifficulty].toEnemies);
    damage = BTL_HIT_PERCENT(damage, args->effectPercent);
    damage = BTL_HIT_PERCENT(damage, enemy->actor.effectDamageTakenPercent);
    return damage;
}

/// The damage an attack deals to the player team, after difficulty, the chain, defense and abilities.
static s16 BtlHit_CalcTeamDamage(BattleActor* actor, const BattleAttackInfo* info, const BattleAttackArgs* args) {
    s16 damage = args->power + info->power;

    damage = BTL_HIT_PERCENT(damage, args->powerPercent);
    damage = BTL_HIT_PERCENT(damage, BtlHit_DifficultyDamagePercents[gSaveData.battleDifficulty].toTeam);
    damage = BTL_HIT_PERCENT(damage, args->effectPercent);
    damage = BTL_HIT_PERCENT(damage, actor->effectDamageTakenPercent);
    damage = BTL_HIT_PERCENT(damage, BtlHit_ChainDamagePercents[gBattle->chainIndex]);
    damage -= actor->defense;
    if (info->flags.bits.screen == 0) {
        BtlHit_ApplyPinchDefenseUp(BATTLE_SIDE_PLAYER, &damage);
    } else {
        BtlHit_ApplyPinchDefenseUp(BATTLE_SIDE_PARTNER, &damage);
        if (actor->hit.fromBehind == TRUE) {
            damage = BTL_HIT_PERCENT(damage, 150);
        }
    }
    if (damage < 0) {
        damage = 0;
    }
    return damage;
}

/// Deals damage to an enemy, defeating it at 0 HP, otherwise giving it its hit reaction.
static void BtlHit_DamageEnemy(Ov003EnemyActor* enemy, const BattleAttackInfo* info, s32 damage, s16 comboFrames) {
    if (damage < 0) {
        enemy->actor.hit.affinity = HIT_AFFINITY_ABSORB;
    }
    if (info->flags.bits.screen == 0) {
        BtlMgr_AddUnitHp(BATTLE_SIDE_PLAYER, &enemy->actor, -damage);
        func_ov003_0209b61c(DISPLAY_MAIN, damage, &enemy->actor);
        if (damage > 0) {
            BtlMgr_RecordDamageDealt(BATTLE_SIDE_PLAYER, damage);
            func_ov003_0209c150(comboFrames);
            BtlHit_AddAirComboBonus(&enemy->actor);
        }
    } else {
        BtlMgr_AddUnitHp(BATTLE_SIDE_PARTNER, &enemy->actor, -damage);
        func_ov003_0209b61c(DISPLAY_SUB, damage, &enemy->actor);
        if (damage > 0) {
            BtlMgr_RecordDamageDealt(BATTLE_SIDE_PARTNER, damage);
        }
    }
    enemy->actor.invincibleTimer = info->invincibleFrames;
    if (enemy->actor.currentHp <= 0) {
        enemy->actor.flags |= ACTOR_FLAG_KILLED;
        BtlActor_ForceDefeat(&enemy->actor);
        BtlActor_ForceDefeat(enemy->actor.linkedActor);
    } else if (damage > 0) {
        BtlHit_ApplyReaction(&enemy->actor, info, enemy->flags196.bits.b1 == 1 ? TRUE : FALSE);
    }
}

/// Deals damage to the player team, defeating both characters at 0 HP, otherwise giving the actor its hit reaction.
static void BtlHit_DamageTeam(BattleActor* actor, const BattleAttackInfo* info, s16 damage) {
    if (info->flags.bits.screen == 0) {
        BtlHit_RollDamageNegate(BATTLE_SIDE_PLAYER, &damage);
        BtlMgr_AddTeamHp(BATTLE_SIDE_PLAYER, -damage);
        func_ov003_0209b564(DISPLAY_MAIN, damage, actor, 3);
    } else {
        BtlHit_RollDamageNegate(BATTLE_SIDE_PARTNER, &damage);
        BtlMgr_AddTeamHp(BATTLE_SIDE_PARTNER, -damage);
        func_ov003_0209b564(DISPLAY_SUB, damage, actor, 3);
    }
    actor->invincibleTimer = info->invincibleFrames;
    if (actor->currentHp <= 0) {
        actor->flags |= ACTOR_FLAG_KILLED;
        BtlActor_ForceDefeat(gBattle->player);
        BtlActor_ForceDefeat(gBattle->partner);
        gBattle->flags |= (BATTLE_FLAG_NO_PAUSE | BATTLE_FLAG_SPECIAL_LOCKED);
    } else if (damage > 0) {
        BtlHit_ApplyReaction(actor, info, FALSE);
        BtlMgr_CountTeamHit();
    }
}

/// Computes and deals an attack's damage to the actor.
static void BtlHit_ApplyDamage(BattleActor* actor, const BattleAttackInfo* info, const BattleAttackArgs* args) {
    actor->hit.critical = FALSE;
    actor->hit.affinity = HIT_AFFINITY_NORMAL;
    actor->hit.variant  = 0;
    if (info->flags.bits.hitsPlayers == 0) {
        Ov003EnemyActor* enemy = (Ov003EnemyActor*)actor;

        BtlHit_DamageEnemy(enemy, info, BtlHit_CalcEnemyDamage(enemy, info, args), args->comboFrames);
        BtlHit_PassAura(info);
    } else {
        BtlHit_DamageTeam(actor, info, BtlHit_CalcTeamDamage(actor, info, args));
    }
}

/// Resolves a BTL_HIT_KIND_ACTOR hit.
static BattleHitResult BtlHit_ResolveOnActor(BattleHit* hit) {
    if (hit->actor == NULL) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_IsImmune(hit->actor, hit->info)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_CheckIgnoresHits(hit->actor)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_TryGuard(hit->actor, hit->info, hit->x)) {
        return BTL_HIT_RESULT_GUARDED;
    }
    BtlHit_CheckFromBehind(hit->actor, hit->info, hit->x);
    BtlHit_ApplyDamage(hit->actor, hit->info, hit->args);
    BtlHit_SpawnHitEffect(hit->actor, hit);
    BtlHit_ApplyKnockback(hit->actor, hit->info, hit->x);
    BtlHit_ApplyStatusEffect(hit->actor, hit->info);
    if (hit->info->flags.bits.screen == 0) {
        gBattle->hitStopMainNext = hit->info->hitStop;
    } else {
        gBattle->hitStopSubNext = hit->info->hitStop;
    }
    return BTL_HIT_RESULT_LANDED;
}

/// Resolves a player's BTL_HIT_KIND_POINT hit against every enemy on the attacker's screen.
static BattleHitResult BtlHit_ResolvePointOnEnemies(BattleHit* hit) {
    BattleHitResult  result = BTL_HIT_RESULT_MISSED;
    Ov003EnemyActor* enemy;

    BtlHit_PostDebugBox(hit);

    s32 halfW = I2F(hit->info->halfWidth);
    s32 halfH = I2F(hit->info->halfHeight);
    s32 halfD = I2F(hit->info->halfDepth);
    if (hit->info->flags.bits.screen == 0) {
        enemy = BtlList_GetHead(&gBattle->enemiesMain);
    } else {
        enemy = BtlList_GetHead(&gBattle->enemiesSub);
    }
    while (enemy != NULL) {
        if (!BtlHit_IsImmune(&enemy->actor, hit->info) &&
            BtlHit_OverlapsBox(&enemy->actor, hit->x, hit->y, hit->z, halfW, halfH, halfD) &&
            !BtlHit_CheckIgnoresHits(&enemy->actor))
        {
            if (BtlHit_TryGuard(&enemy->actor, hit->info, hit->x)) {
                if (result == BTL_HIT_RESULT_MISSED) {
                    result = BTL_HIT_RESULT_GUARDED;
                }
            } else {
                BtlHit_ApplyDamage(&enemy->actor, hit->info, hit->args);
                BtlHit_SpawnHitEffect(&enemy->actor, hit);
                BtlHit_ApplyKnockback(&enemy->actor, hit->info, hit->x);
                BtlHit_ApplyStatusEffect(&enemy->actor, hit->info);
                result = BTL_HIT_RESULT_LANDED;
            }
        }
        enemy = BtlList_GetNext(&enemy->node);
    }
    if (result == BTL_HIT_RESULT_LANDED) {
        if (hit->info->flags.bits.screen == 0) {
            gBattle->hitStopMainNext = hit->info->hitStop;
        } else {
            gBattle->hitStopSubNext = hit->info->hitStop;
        }
    }
    return result;
}

/// Resolves an enemy's BTL_HIT_KIND_POINT hit against the screen's player character.
static BattleHitResult BtlHit_ResolvePointOnCharacter(BattleHit* hit) {
    BattleActor* actor;

    BtlHit_PostDebugBox(hit);
    if (hit->info->flags.bits.screen == 0) {
        actor = gBattle->player;
    } else {
        actor = gBattle->partner;
    }
    hit->actor = actor;

    s32 halfW = I2F(hit->info->halfWidth);
    s32 halfH = I2F(hit->info->halfHeight);
    s32 halfD = I2F(hit->info->halfDepth);
    if (BtlHit_IsImmune(actor, hit->info) || !BtlHit_OverlapsBox(hit->actor, hit->x, hit->y, hit->z, halfW, halfH, halfD)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_CheckIgnoresHits(hit->actor)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_TryGuard(hit->actor, hit->info, hit->x)) {
        return BTL_HIT_RESULT_GUARDED;
    }
    BtlHit_CheckFromBehind(hit->actor, hit->info, hit->x);
    BtlHit_ApplyDamage(hit->actor, hit->info, hit->args);
    BtlHit_SpawnHitEffect(hit->actor, hit);
    BtlHit_ApplyKnockback(hit->actor, hit->info, hit->x);
    BtlHit_ApplyStatusEffect(hit->actor, hit->info);
    if (hit->info->flags.bits.screen == 0) {
        gBattle->hitStopMainNext = hit->info->hitStop;
    } else {
        gBattle->hitStopSubNext = hit->info->hitStop;
    }
    return BTL_HIT_RESULT_LANDED;
}

/// Resolves a player's BTL_HIT_KIND_SCREEN hit against every enemy on the attacker's screen.
static BattleHitResult BtlHit_ResolveScreenOnEnemies(BattleHit* hit) {
    Ov003EnemyActor* enemy;
    s32              originX;
    BattleHitResult  result = BTL_HIT_RESULT_MISSED;

    BtlHit_PostDebugBox(hit);
    if (hit->info->flags.bits.screen == 0) {
        originX = BtlStage_ToWorldX(DISPLAY_MAIN, (hit->x * 16) >> 16);
    } else {
        originX = BtlStage_ToWorldX(DISPLAY_SUB, (hit->x * 16) >> 16);
    }
    if (hit->info->flags.bits.screen == 0) {
        enemy = BtlList_GetHead(&gBattle->enemiesMain);
    } else {
        enemy = BtlList_GetHead(&gBattle->enemiesSub);
    }
    while (enemy != NULL) {
        if (!BtlHit_IsImmune(&enemy->actor, hit->info) && BtlHit_OverlapsScreenBox(&enemy->actor, hit) &&
            !BtlHit_CheckIgnoresHits(&enemy->actor))
        {
            if (BtlHit_TryGuard(&enemy->actor, hit->info, originX)) {
                if (result == BTL_HIT_RESULT_MISSED) {
                    result = BTL_HIT_RESULT_GUARDED;
                }
            } else {
                BtlHit_ApplyDamage(&enemy->actor, hit->info, hit->args);
                BtlHit_SpawnHitEffect(&enemy->actor, hit);
                BtlHit_ApplyKnockback(&enemy->actor, hit->info, originX);
                BtlHit_ApplyStatusEffect(&enemy->actor, hit->info);
                result = BTL_HIT_RESULT_LANDED;
            }
        }
        enemy = BtlList_GetNext(&enemy->node);
    }
    if (result == BTL_HIT_RESULT_LANDED) {
        if (hit->info->flags.bits.screen == 0) {
            gBattle->hitStopMainNext = hit->info->hitStop;
        } else {
            gBattle->hitStopSubNext = hit->info->hitStop;
        }
    }
    return result;
}

/// Resolves an enemy's BTL_HIT_KIND_SCREEN hit against the screen's player character.
static BattleHitResult BtlHit_ResolveScreenOnCharacter(BattleHit* hit) {
    BattleActor* actor;
    s32          originX;

    BtlHit_PostDebugBox(hit);
    if (hit->info->flags.bits.screen == 0) {
        originX = BtlStage_ToWorldX(DISPLAY_MAIN, (hit->x * 16) >> 16);
    } else {
        originX = BtlStage_ToWorldX(DISPLAY_SUB, (hit->x * 16) >> 16);
    }
    if (hit->info->flags.bits.screen == 0) {
        actor = gBattle->player;
    } else {
        actor = gBattle->partner;
    }
    hit->actor = actor;
    if (BtlHit_IsImmune(actor, hit->info) || !BtlHit_OverlapsScreenBox(hit->actor, hit)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_CheckIgnoresHits(hit->actor)) {
        return BTL_HIT_RESULT_MISSED;
    }
    if (BtlHit_TryGuard(hit->actor, hit->info, originX)) {
        return BTL_HIT_RESULT_GUARDED;
    }
    BtlHit_CheckFromBehind(hit->actor, hit->info, originX);
    BtlHit_ApplyDamage(hit->actor, hit->info, hit->args);
    BtlHit_SpawnHitEffect(hit->actor, hit);
    BtlHit_ApplyKnockback(hit->actor, hit->info, originX);
    BtlHit_ApplyStatusEffect(hit->actor, hit->info);
    if (hit->info->flags.bits.screen == 0) {
        gBattle->hitStopMainNext = hit->info->hitStop;
    } else {
        gBattle->hitStopSubNext = hit->info->hitStop;
    }
    return BTL_HIT_RESULT_LANDED;
}

/// Starts a status effect on the actor, replacing any running one; starts its visual effect if none was running.
static void BtlHit_StartStatusEffect(BattleSide side, BattleActor* actor, u16 kind, s16 amount, s16 duration) {
    u16 previous = actor->periodicEffectMode;

    BtlActor_ClearPeriodicEffect(actor);
    actor->periodicEffectMode  = kind;
    actor->periodicEffectTimer = duration;
    switch (actor->periodicEffectMode) {
        case 2:
            actor->baseArgs.effectPercent = amount;
            break;
        case 3:
            actor->effectDamageTakenPercent = amount;
            break;
        case 1:
            actor->periodicEffectAmount = actor->maxHp * amount / 100;
            break;
        case 0:
        case 4:
            break;
    }
    if (previous == 0) {
        func_ov003_0209ccc0(side, actor);
    }
}

BOOL BtlHit_FlinchWithStatus(BattleSide side, BattleActor* actor, u16 statusEffect) {
    s32 flags = actor->flags;

    if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_INTANGIBLE) || (flags & ACTOR_FLAG_LAUNCHED) ||
        actor->pendingCommand == ACTOR_CMD_KNOCKDOWN)
    {
        return FALSE;
    }
    BtlActor_SetPendingCommand(actor, ACTOR_CMD_HIT);
    BtlHit_RollStatusEffect(side, actor, statusEffect);
    return TRUE;
}

BOOL BtlHit_Stagger(BattleSide side, BattleActor* actor, s16 stunFrames) {
    s32 flags = actor->flags;

    if ((flags & ACTOR_FLAG_DEFEATED) || (flags & ACTOR_FLAG_INTANGIBLE) || (flags & ACTOR_FLAG_LAUNCHED) ||
        actor->pendingCommand == ACTOR_CMD_KNOCKDOWN)
    {
        return FALSE;
    }
    actor->flags = flags & ~ACTOR_FLAG_GUARDING;
    BtlActor_SetPendingCommand(actor, ACTOR_CMD_HIT);
    if (stunFrames > 0) {
        BtlHit_StartStatusEffect(side, actor, 4, 0, stunFrames);
    }
    return TRUE;
}

const BattleAttackInfo* BtlHit_GetAttackInfo(u16 id) {
    return &BtlAttackInfo_Table[id];
}

BattleHitResult BtlHit_AttackActor(const BattleAttackInfo* info, BattleAttackArgs* args, BattleActor* actor, s32 originX) {
    BattleHit hit;

    hit.info  = info;
    hit.args  = args;
    hit.kind  = BTL_HIT_KIND_ACTOR;
    hit.actor = actor;
    hit.x     = originX;
    hit.y     = 0;
    hit.z     = 0;
    return BtlHit_Resolve(&hit);
}

BattleHitResult BtlHit_AttackAt(const BattleAttackInfo* info, BattleAttackArgs* args, s32 x, s32 y, s32 z) {
    BattleHit hit;

    hit.info  = info;
    hit.args  = args;
    hit.kind  = BTL_HIT_KIND_POINT;
    hit.actor = NULL;
    hit.x     = x;
    hit.y     = y;
    hit.z     = z;
    return BtlHit_Resolve(&hit);
}

BattleHitResult BtlHit_AttackAtScreen(const BattleAttackInfo* info, BattleAttackArgs* args, s32 x, s32 y) {
    BattleHit hit;

    hit.info  = info;
    hit.args  = args;
    hit.kind  = BTL_HIT_KIND_SCREEN;
    hit.actor = NULL;
    hit.x     = I2F(x);
    hit.y     = I2F(y);
    hit.z     = 0;
    return BtlHit_Resolve(&hit);
}

BattleHitResult BtlHit_AttackCharacter(const BattleAttackInfo* info, BattleAttackArgs* args, s32 originX) {
    if (info->flags.bits.screen == 0) {
        return BtlHit_AttackActor(info, args, gBattle->player, originX);
    } else {
        return BtlHit_AttackActor(info, args, gBattle->partner, originX);
    }
}

void BtlHit_ArmActor(u16 attackId, const BattleAttackArgs* args, BattleActor* actor) {
    actor->impactAttackId = attackId;
    actor->impactArgs     = *args;
}

BattleHitResult BtlHit_ActorImpact(BattleActor* actor) {
    if (actor->xVelocity == 0 && actor->yVelocity == 0) {
        if (actor->zVelocity < 0x5000) {
            return BTL_HIT_RESULT_MISSED;
        }
    }

    const BattleAttackInfo* info   = BtlHit_GetAttackInfo(actor->impactAttackId);
    BattleAttackInfo        attack = *info;
    BattleAttackArgs        args   = actor->impactArgs;

    if (attack.flags.bits.hitsPlayers == 0) {
        args.power = BTL_HIT_PERCENT(args.power, actor->impactPercent);
    } else {
        args.power   = BTL_HIT_PERCENT(actor->impactPercent, args.power + attack.power);
        attack.power = 0;
    }

    u8 radius         = actor->radius;
    attack.halfDepth  = actor->height / 2;
    attack.halfWidth  = radius;
    attack.halfHeight = radius;

    actor->flags |= ACTOR_FLAG_IMPACTING;

    BattleHitResult result =
        BtlHit_AttackAt(&attack, &args, actor->position.x, actor->position.y, actor->position.z - I2F(actor->height / 2));

    actor->flags &= ~ACTOR_FLAG_IMPACTING;
    if (actor->kind == ACTOR_KIND_OBJECT) {
        s16 hp = actor->currentHp;

        if (hp > 0 && result == BTL_HIT_RESULT_LANDED) {
            actor->currentHp = hp - 1;
            if (actor->currentHp <= 0) {
                BtlActor_SetPendingCommand(actor, ACTOR_CMD_HIT);
                actor->flags |= ACTOR_FLAG_DEFEATED;
            }
        }
    }
    return result;
}

/// Resolves a hit by its kind while the battle is running.
static BattleHitResult BtlHit_Resolve(BattleHit* hit) {
    if (gBattle->state != BATTLE_STATE_FIGHT) {
        return BTL_HIT_RESULT_MISSED;
    }

    switch (hit->kind) {
        case BTL_HIT_KIND_ACTOR:
            return BtlHit_ResolveOnActor(hit);
        case BTL_HIT_KIND_POINT:
            if (hit->info->flags.bits.hitsPlayers == FALSE) {
                return BtlHit_ResolvePointOnEnemies(hit);
            } else {
                return BtlHit_ResolvePointOnCharacter(hit);
            }
        case BTL_HIT_KIND_SCREEN:
            if (hit->info->flags.bits.hitsPlayers == FALSE) {
                return BtlHit_ResolveScreenOnEnemies(hit);
            } else {
                return BtlHit_ResolveScreenOnCharacter(hit);
            }
        default:
            return BTL_HIT_RESULT_MISSED;
    }
}
