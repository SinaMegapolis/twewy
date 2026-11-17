#ifndef COMBAT_CORE_COMBATEFFECTS_H
#define COMBAT_CORE_COMBATEFFECTS_H
#include "Battle/Core/BtlSprite.h"
#include "SndMgrSeIdx.h"
#include <nitro/fx.h>

typedef struct CombatEffect {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ Vec          position;
    /* 0x6C */ s32          active;
} CombatEffect; // Size: 0x70

typedef struct CombatHitEffect {
    /* 0x00 */ u8           kind;
    /* 0x01 */ u8           padding[3];
    /* 0x04 */ CombatEffect effect;
} CombatHitEffect; // Size: 0x74

typedef struct CombatDeadEffect {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ Vec          position;
    /* 0x6C */ s32          playVoice;
    /* 0x70 */ s32          active;
} CombatDeadEffect; // Size: 0x74

typedef struct CombatEffectParams {
    /* 0x00 */ SpriteAnimEntry anim;
    /* 0x08 */ u16             pack;
    /* 0x0A */ u16             charTiles;    // char VRAM to reserve, in 32-byte units
    /* 0x0C */ u16             paletteEntry; // palette data entry
    /* 0x0E */ u16             padding;
    /* 0x10 */ s32             unused;
} CombatEffectParams; // Size: 0x14

typedef struct CombatHitParams {
    /* 0x00 */ SpriteAnimEntry anim;
    /* 0x08 */ u16             engine;
    /* 0x0A */ u16             pack;
    /* 0x0C */ u16             charTiles;    // char VRAM to reserve, in 32-byte units
    /* 0x0E */ u16             paletteEntry; // palette data entry
    /* 0x10 */ SndMgrSeIdx     sound;
} CombatHitParams;                           // Size: 0x14
#endif
