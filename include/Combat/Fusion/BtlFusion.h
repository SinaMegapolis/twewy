#ifndef COMBAT_FUSION_BTLFUSION_H
#define COMBAT_FUSION_BTLFUSION_H

#include "Battle/Core/BtlActor.h"
#include "Battle/Core/BtlSprite.h"
#include "Combat/Core/CombatList.h"
#include "Engine/EasyTask.h"

/// Task data for `Tsk_BtlPlayerLast`.
typedef struct BtlPlayerLast {
    /* 0x00 */ BattleActor actor;
    /* 0x7C */ void (*unk_7C)(struct BtlPlayerLast*);
    /* 0x80 */ s16 unk_80;
    /* 0x82 */ s16 unk_82;
} BtlPlayerLast; // Size: 0x84

/// A pooled aura sprite, linked through the trailing node.
typedef struct BtlAuraSprite {
    /* 0x00 */ BattleSprite   sprite;
    /* 0x60 */ CombatListNode node;
} BtlAuraSprite; // Size: 0x6C

/// Task data for `Tsk_BtlAuraLast`.
typedef struct BtlAuraLast {
    /* 0x000 */ BattleSprite  unk_000;
    /* 0x060 */ BattleSprite  unk_060;
    /* 0x0C0 */ BattleSprite  unk_0C0;
    /* 0x120 */ CombatList    freeSprites;
    /* 0x128 */ CombatList    activeSprites;
    /* 0x130 */ BtlAuraSprite sprites[16];
    /* 0x7F0 */ s16           unk_7F0;
    /* 0x7F2 */ s16           unk_7F2;
    /* 0x7F4 */ void (*unk_7F4)(struct BtlAuraLast*);
    /* 0x7F8 */ s16 unk_7F8;
    /* 0x7FA */ s16 unk_7FA;
    /* 0x7FC */ s32 unk_7FC;
    /* 0x800 */ s32 unk_800;
    /* 0x804 */ s32 unk_804;
    /* 0x808 */ s32 unk_808;
    /* 0x80C */ s16 unk_80C;
    /* 0x80E */ s16 unk_80E;
} BtlAuraLast; // Size: 0x810

extern const TaskHandle Tsk_BtlPlayerLast;
extern const TaskHandle Tsk_BtlAuraLast;

s32  func_ov021_020f1174(void);
void func_ov021_020f11bc(s32, s32*, s32*, s32*);
void func_ov021_020f13b0(void);

#endif // COMBAT_FUSION_BTLFUSION_H
