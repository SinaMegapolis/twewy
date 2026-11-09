#ifndef BATTLE_CORE_BTLACTOR_H
#define BATTLE_CORE_BTLACTOR_H

#include "Combat/Core/CombatSprite.h"
#include <nitro/fx.h>

/// Which side an actor fights on
typedef enum {
    ACTOR_KIND_PLAYER = 0, // Neku, his partner, and their summons
    ACTOR_KIND_ENEMY  = 1, // Noise and their projectiles
    ACTOR_KIND_OBJECT = 2, // Grabbable and throwable objects
} BattleActorKind;

/// A reaction queued on an actor to be popped on Update
typedef enum {
    ACTOR_CMD_NONE        = 0,
    ACTOR_CMD_HIT         = 1, // flinch from a hit
    ACTOR_CMD_KNOCKDOWN   = 2, // knocked off its feet from heavy hits or hits while airborne
    ACTOR_CMD_DEFEAT      = 3,
    ACTOR_CMD_HOLD        = 4, // grabbed by a psych (Psychokinesis, Black Hole, etc.)
    ACTOR_CMD_RELEASE     = 5, // let go by the psych holding it, or thrown
    ACTOR_CMD_FEAT        = 6, // caught in a partner's Feat, frozen until the cut-in ends
    ACTOR_CMD_HIT_IGNORED = 7, // hit while ACTOR_FLAG_IGNORES_HITS is set, no damage taken
    ACTOR_CMD_LEVEL_UP    = 8, // the player's level went up
    ACTOR_CMD_GUARD       = 9, // hit while ACTOR_FLAG_GUARDING is set
} BattleActorCommand;

typedef enum {
    ACTOR_FLAG_TOUCHED          = (1 << 0),  // pressed on the touch screen this frame; cleared on render
    ACTOR_FLAG_STAGGERED        = (1 << 1),  // reeling from a hit; cancels the shots the actor is firing
    ACTOR_FLAG_DEFEATED         = (1 << 2),  // out of the fight: ignores commands, hits and targeting
    ACTOR_FLAG_GUARDING         = (1 << 3),  // hits are blocked (ACTOR_CMD_GUARD)
    ACTOR_FLAG_INTANGIBLE       = (1 << 4),  // cannot be hit, touched or targeted
    ACTOR_FLAG_NO_TARGET_MARKER = (1 << 5),  // the target reticle is not drawn over it
    ACTOR_FLAG_IGNORES_HITS     = (1 << 6),  // hits land without effect (ACTOR_CMD_HIT_IGNORED)
    ACTOR_FLAG_UNK_8            = (1 << 8),  // treated as busy by enemy shots
    ACTOR_FLAG_HELD             = (1 << 9),  // held by a psych
    ACTOR_FLAG_THROWN           = (1 << 10), // flying after a psych let go, dealing its impact attack
    ACTOR_FLAG_HIT_FLASH        = (1 << 12), // took a hit: flash the sprite's palette on the next render
    ACTOR_FLAG_KILLED           = (1 << 13), // took the fatal hit itself, rather than through its linked twin
    ACTOR_FLAG_LAUNCHED         = (1 << 14), // knocked into the air: no ground friction, every hit knocks down
    ACTOR_FLAG_IMPACTING        = (1 << 15), // its own impact attack is resolving, so it cannot hit itself
    ACTOR_FLAG_NO_RECOVERY      = (1 << 17), // cannot break a knockdown early
    ACTOR_FLAG_UNK_18           = (1 << 18), // Shiki: suppresses the charge gauge until her doll returns
    ACTOR_FLAG_BOSS_GAUGE_HP    = (1 << 27), // HP lives in the battle-wide boss gauge instead of currentHp
    ACTOR_FLAG_UNCLAMPED_X      = (1 << 28), // may leave the stage horizontally, only Y is clamped
    ACTOR_FLAG_NO_HIT_EFFECT    = (1 << 29), // hits spawn no spark or psych effect on it
    ACTOR_FLAG_NO_FLINCH        = (1 << 30), // light hits do not interrupt it (no ACTOR_CMD_HIT)
    ACTOR_FLAG_HIDDEN           = (1 << 31), // not drawn, and hit sparks are not shown on it
} BattleActorFlags;

/// How a hit landed relative to the target's resistances; drives the damage popup's colour.
typedef enum {
    HIT_AFFINITY_NORMAL = 0,
    HIT_AFFINITY_ABSORB = 1, // the multiplier was negative: the hit healed
    HIT_AFFINITY_RESIST = 2, // below 100%
    HIT_AFFINITY_WEAK   = 3, // above 100%
} BattleHitAffinity;

/// What an actor's hits deal: copied into every attack it spawns and scaled by the target on impact.
typedef struct BattleAttackArgs {
    /* 0x00 */ s16 power;         // base damage
    /* 0x02 */ s16 comboFrames;   // how long a hit keeps the combo counter open
    /* 0x04 */ s16 powerPercent;  // strength scale: enemy parameters, aura, enrage
    /* 0x06 */ s16 effectPercent; // status-effect strength scale (periodic effect 2)
} BattleAttackArgs;               // Size: 0x8

/// The last hit the actor took.
typedef struct BattleActorHit {
    /* 0x0 */ s32 sourceX;        // where the hit came from: knockback points away from it
    /* 0x4 */ s16 stunTimer;      // frames of hit stun left
    /* 0x6 */ u16 finisher   : 1; // A combo finisher
    /* 0x6 */ u16 critical   : 1; // boosted by the sync bonus
    /* 0x6 */ u16 affinity   : 2; // BattleHitAffinity
    /* 0x6 */ u16 variant    : 2; // the attack's variant: picks which per-unit multiplier applied
    /* 0x6 */ u16 fromBehind : 1; // hit from behind
} BattleActorHit;                 // Size: 0x8

typedef struct BattleActor {
    /* 0x00 */ BattleActorKind     kind;
    /* 0x04 */ BattleAttackArgs    baseArgs;       // copied into the attacks the actor spawns
    /* 0x0C */ BattleActorHit      hit;
    /* 0x14 */ u16                 impactAttackId; // attack dealt when the actor is thrown into others
    /* 0x16 */ BattleAttackArgs    impactArgs;
    /* 0x20 */ BattleActorCommand  pendingCommand;
    /* 0x24 */ BOOL                isFlipped;
    /* 0x28 */ Vec                 position;
    /* 0x34 */ s16                 screenX;
    /* 0x36 */ s16                 screenY;
    /* 0x38 */ s32                 zVelocity;
    /* 0x3C */ s32                 zGravity;
    /* 0x40 */ s32                 xVelocity;
    /* 0x44 */ s32                 yVelocity;
    /* 0x48 */ s32                 linearDampingStep;
    /* 0x4C */ s32                 dampingScale;
    /* 0x50 */ s32                 bounceScale;     // share of the falling speed kept when bouncing off the ground
    /* 0x54 */ s32                 flags;
    /* 0x58 */ s16                 holderId;        // the psych holding the actor (ACTOR_CMD_HOLD)
    /* 0x5A */ s16                 invincibleTimer; // frames during which hits are ignored
    /* 0x5C */ u16                 periodicEffectMode;
    /* 0x5E */ s16                 periodicEffectTimer;
    /* 0x60 */ s16                 periodicEffectAmount;
    /* 0x62 */ s16                 damageTakenPercent;       // resistance to the players' attacks
    /* 0x64 */ s16                 effectDamageTakenPercent; // status-effect scale on damage taken (periodic effect 3)
    /* 0x66 */ s16                 attack;
    /* 0x68 */ s16                 defense;
    /* 0x6A */ s16                 impactPercent; // scales the impact attack's power
    /* 0x6C */ s16                 currentHp;
    /* 0x6E */ s16                 maxHp;
    /* 0x70 */ s16                 radius;        // half the body's width, in pixels
    /* 0x72 */ s16                 height;        // in pixels
    /* 0x74 */ s16                 centerOffsetX; // body centre relative to `position`, in pixels
    /* 0x76 */ s16                 centerHeight;
    /* 0x78 */ struct BattleActor* linkedActor;   // the twin on the other screen, shares HP and defeat
} BattleActor;                                    // Size: 0x7C

/**
 * @brief Queues a command to be handled on the actor's next Update, if the actor is not defeated
 *
 * @param actor The actor to command
 * @param command The command to queue, replacing any command still pending
 */
void BtlActor_SetPendingCommand(BattleActor* actor, BattleActorCommand command);

/**
 * @brief Takes the actor's queued command and ticks down its invincibility timer
 *
 * @param actor The actor to read the command from
 * @return The queued command, or ACTOR_CMD_NONE if there was none
 */
BattleActorCommand BtlActor_PopPendingCommand(BattleActor* actor);

/**
 * @brief Drops an actor to 0 HP, queues ACTOR_CMD_DEFEAT and marks it ACTOR_FLAG_DEFEATED
 *
 * @param actor The actor to defeat
 */
void BtlActor_ForceDefeat(BattleActor* actor);

/**
 * @brief Sets the upward velocity that brings the actor back to the ground after a given time
 *
 * @param actor The actor to launch; its zGravity must already be set
 * @param frames How long the jump lasts, in frames
 */
void BtlActor_SetJumpDuration(BattleActor* actor, s32 frames);

/**
 * @brief Ends the actor's periodic status effect and restores both status-effect scales to 100%
 *
 * @param actor The actor to clear
 */
void BtlActor_ClearPeriodicEffect(BattleActor* actor);

/**
 * @brief Restores the default impact attack (attack 10, no power, 100% scales)
 *
 * @param actor The actor to reset
 */
void BtlActor_ResetAttack(BattleActor* actor);

/**
 * @brief Runs one frame of the actor's periodic effect and hit stun
 *
 * Periodic effect mode 1 deals `periodicEffectAmount` damage every 3 seconds, to the team HP for
 * players or the actor's own HP otherwise; the damage can defeat the actor, along with its partner
 * or linked twin. The periodic effect ends when its timer runs out. Nothing ticks while the battle
 * is halted by a Fusion or a boss defeat.
 *
 * @param engine The screen the actor fights on (0 = Neku's, 1 = the partner's), for the HP change and damage popup
 * @param actor The actor to update
 */
void BtlActor_UpdateEffects(s32 engine, BattleActor* actor);

/**
 * @brief Runs one frame of the actor's movement
 *
 * Applies gravity while airborne (capped fall speed), bounces on landing by `bounceScale` and stops
 * small bounces, moves by the planar velocity, then applies friction unless ACTOR_FLAG_LAUNCHED is set.
 *
 * @param actor The actor to move
 */
void BtlActor_UpdatePhysics(BattleActor* actor);

/**
 * @brief Resets an actor to the defaults of a freshly spawned one
 *
 * Grounded at the origin with no velocity, 100 HP, 100% scales, no flags, no pending command,
 * the default body size and impact attack, and no linked twin.
 *
 * @param actor The actor to initialize
 * @param kind Which side the actor fights on
 */
void BtlActor_Init(BattleActor* actor, BattleActorKind kind);

/**
 * @brief Draws an actor's sprite at its position on the stage
 *
 * Starts the palette flash for a hit taken since the last render, places and flips the sprite to
 * match the actor, sorts it by depth and records the resulting `screenX`/`screenY`. With debug
 * setting 0 on, non-player actors are marked ACTOR_FLAG_HIDDEN and not drawn.
 *
 * @param actor The actor to draw
 * @param sprite The actor's sprite; the screen it is assigned to picks the stage projection
 */
void BtlActor_Render(BattleActor* actor, CombatSprite* sprite);

#endif // BATTLE_CORE_BTLACTOR_H