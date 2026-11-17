#ifndef TWEWY_BATTLE_CORE_BTLATTACKINFO_H
#define TWEWY_BATTLE_CORE_BTLATTACKINFO_H

#include "nitro/types.h"

/// How an attack behaves when it lands.
typedef struct BattleAttackFlags {
    u32 effect          : 5; // hit effect id spawned at the hit point, 26 = none
    u32 effectPlacement : 2; // 0 = on the target, 1 = at the hit point, 2 = halfway between them
    u32 hitsPlayers     : 1; // 0 = hits the enemies, 1 = hits the player or partner
    u32 screen          : 1; // 0 = the player's (main) screen, 1 = the partner's (sub) screen
    u32 variant         : 2; // enemy damage-multiplier variant, copied to `BattleActorHit.variant`
    u32 pushFromPlayer  : 1; // knock the target away from the screen's player instead of the hit point
    u32 finisher        : 1; // combo finisher, multiplied by the sync bonus while it is active
    u32 piercing        : 1; // ignores the target's invincibility and guard damage reduction
    u32 noFlinch        : 1; // the target takes damage without the hit reaction
    u32 knockback       : 4; // knockback strength index, 6 and up knocks the target down
    u32 statusEffect    : 5; // status effect rolled on a plain hit, 0 = none
    u32 unused          : 8;
} BattleAttackFlags;

/// One attack's damage, reaction timings, hit box and behaviour.
typedef struct BattleAttackInfo {
    /* 0x00 */ s16 power;            // base damage
    /* 0x02 */ u8  hitStop;          // frames the attacker's screen freezes when the attack lands
    /* 0x03 */ u8  stunFrames;       // frames the target stays stunned
    /* 0x04 */ u8  invincibleFrames; // frames the target is invincible afterwards
    /* 0x05 */ u8  halfWidth;        // hit box half-extents, pixels
    /* 0x06 */ u8  halfHeight;
    /* 0x07 */ u8  halfDepth;
    /* 0x08 */ union {
        u32               raw;
        BattleAttackFlags bits;
    } flags;
} BattleAttackInfo; // Size: 0xC

/// Every attack's info, indexed by attack id.
extern const BattleAttackInfo BtlAttackInfo_Table[401];

#endif // TWEWY_BATTLE_CORE_BTLATTACKINFO_H
