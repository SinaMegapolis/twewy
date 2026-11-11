#ifndef COMBAT_CORE_COMBATEFFECTACTOR_H
#define COMBAT_CORE_COMBATEFFECTACTOR_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlCollision.h"
#include "Battle/Core/BtlList.h"

typedef struct Ov003EffectSpawn {
    /* 0x00 */ u16 config;
    /* 0x02 */ u16 id;
    /* 0x04 */ s32 arg1;
    /* 0x08 */ s32 arg2;
} Ov003EffectSpawn; // Size: 0xC

// The existing cut-in motion record is also the prefix of a pooled effect visual.
typedef struct Ov003EffectSprite {
    /* 0x00 */ BtlCollider collider;
    /* 0x34 */ u16         flags;
    /* 0x36 */ u16         unk_36;
} Ov003EffectSprite; // Size: 0x38

typedef struct Ov003EffectActor {
    /* 0x000 */ u16               id;
    /* 0x002 */ u16               config;
    /* 0x004 */ BattleActor       actor;
    /* 0x080 */ Ov003EffectSprite sprite;
    /* 0x0B8 */ BattleListNode    node;
} Ov003EffectActor; // Size: 0xC4

#endif
