#ifndef TWEWY_COMBAT_CORE_BTLTCH_H
#define TWEWY_COMBAT_CORE_BTLTCH_H

#include "Battle/Core/BtlSprite.h"
#include "Engine/EasyTask.h"

// Shared with the battle manager's cursor state machine.
typedef struct BtlIcon {
    /* 0x00 */ BattleSprite sprite;
    /* 0x60 */ u16          animState;
    /* 0x62 */ s16          x;
    /* 0x64 */ s16          y;
    /* 0x66 */ u16          pad_66;
    /* 0x68 */ void (*update)(struct BtlIcon*);
    /* 0x6C */ s32 unk_6C;
} BtlIcon; // Size: 0x70

/// The stylus cursor icon that follows the touch position.
extern const TaskHandle Task_BtlTch;

/// The burst shown where the stylus was tapped, sized by the tap count.
extern const TaskHandle Tsk_BtlTch_Click;

/// The streak drawn along a stylus slash.
extern const TaskHandle Tsk_BtlTch_Slash;

#endif // TWEWY_COMBAT_CORE_BTLTCH_H
