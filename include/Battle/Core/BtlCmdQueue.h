#ifndef TWEWY_BATTLE_CORE_BTLCMDQUEUE_H
#define TWEWY_BATTLE_CORE_BTLCMDQUEUE_H

#include "Battle/Core/BtlList.h"

/// One partner action waiting in the command queue.
typedef struct BattleCmdQueueEntry {
    /* 0x00 */ u16            value; // the action, 0 = none
    /* 0x02 */ u16            unk_02;
    /* 0x04 */ BattleListNode node;
} BattleCmdQueueEntry; // Size: 0x10

/// The actions the partner command UI has queued for the partner to act out. The 16 entries move between
/// the free list and the queued list.
typedef struct BattleCmdQueue {
    /* 0x000 */ u16                 direction; // facing to apply next, 0 = unchanged
    /* 0x002 */ u16                 flags;
    /* 0x004 */ u16                 unk_04;
    /* 0x006 */ u16                 unk_06;
    /* 0x008 */ s32                 unk_08;
    /* 0x00C */ BattleList          free;
    /* 0x014 */ BattleList          queued;
    /* 0x01C */ BattleCmdQueueEntry entries[16];
} BattleCmdQueue; // Size: 0x11C

void BtlCmdQueue_Push(u16 value);
void BtlCmdQueue_Clear(void);
void BtlCmdQueue_SetDirection(s32 value);
u16  BtlCmdQueue_Pop(void);
u16  BtlCmdQueue_TakeDirection(void);
s32  func_ov003_02086a64(void);
s32  func_ov003_02086a80(void);
void func_ov003_02086aa8(s32 value);
void func_ov003_02086ac4(s32 enabled);
void func_ov003_02086af0(void);
void BtlCmdQueue_Init(BattleCmdQueue* queue);

#endif // TWEWY_BATTLE_CORE_BTLCMDQUEUE_H
