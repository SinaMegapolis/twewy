#ifndef TWEWY_BATTLE_CORE_BTLCMDDECK_H
#define TWEWY_BATTLE_CORE_BTLCMDDECK_H

#include "Battle/Core/BtlSprite.h"

/// The partner's command deck: a shuffled queue of command ids dealt to the command UI, plus the combo
/// stage progress.
typedef struct BattleCmdDeck {
    /* 0x00 */ u8  mode;     // 0 = charge (difficulty-sized), 1 = seal, 2 = special
    /* 0x01 */ s8  current;  // index into the charge-step table
    /* 0x02 */ s8  count;    // steps in the current charge step sequence
    /* 0x03 */ s8  progress; // charge accumulated so far
    /* 0x04 */ s8  next;     // step length of the entry at `current`
    /* 0x05 */ s8  stage;    // charge stage (per partner progress)
    /* 0x06 */ u16 flags;
    /* 0x08 */ u8  unk_08[0x8];
    /* 0x10 */ s16 remaining; // entries left in `queue`
    /* 0x12 */ u16 capacity;
    /* 0x14 */ u16 queue[12];
    /* 0x2C */ s32 unk_2C; // mode-specific handle (battle difficulty index, ...)
} BattleCmdDeck;           // Size: 0x30

void BtlCmdDeck_Shuffle(u16* table, u32 count);
void BtlCmdDeck_EaseToSine(s32* value, s16* index);
/// Refills the effect command's queue with a fresh shuffle.
void func_ov003_02086cc0(void);
u16  func_ov003_02086d24(void);
void func_ov003_02086d8c(BattleSprite* sprite, s32 animId, s32 flipped, s32 x, s16 y, s16 z);
void func_ov003_02086e38(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s16* func_ov003_02086e94(void);
s32  func_ov003_02086f20(s32 arg0, s32 arg1);
s32  func_ov003_02086f7c(void);
s8   func_ov003_02086fec(void);
void func_ov003_02087008(void);
void func_ov003_02087094(BattleCmdDeck* deck, u8 mode, s32 arg2);
void func_ov003_020871b0(BattleCmdDeck* deck);
void func_ov003_020871cc(s32 arg0);
void func_ov003_0208722c(void);

#endif // TWEWY_BATTLE_CORE_BTLCMDDECK_H
