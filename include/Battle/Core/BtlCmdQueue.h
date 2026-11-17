#ifndef TWEWY_BATTLE_CORE_BTLCMDQUEUE_H
#define TWEWY_BATTLE_CORE_BTLCMDQUEUE_H

#include "Battle/Core/BtlList.h"

/// BattleCmdQueue flags.
typedef enum BattleCmdQueueFlag {
    BTL_CMD_QUEUE_FLAG_UNK_1           = (1 << 1), // cleared when a combo chain starts, never set
    BTL_CMD_QUEUE_FLAG_PARTNER_JUMPING = (1 << 2), // the partner is mid-jump
} BattleCmdQueueFlag;

/// One partner action waiting in the command queue.
typedef struct BattleCmdQueueEntry {
    /* 0x00 */ u16            value; // the action, 0 = none
    /* 0x02 */ u16            unk_02;
    /* 0x04 */ BattleListNode node;
} BattleCmdQueueEntry; // Size: 0x10

/// The actions the partner command UI has queued for the partner to act out. The 16 entries move between
/// the free list and the queued list.
typedef struct BattleCmdQueue {
    /* 0x000 */ u16                 direction; // direction of the chain the player picked, 0 = none
    /* 0x002 */ u16                 flags;     // BattleCmdQueueFlag
    /* 0x004 */ u16                 unk_04;
    /* 0x006 */ u16                 unk_06;
    /* 0x008 */ BOOL                partnerReady; // the partner has finished its action and can take another
    /* 0x00C */ BattleList          free;
    /* 0x014 */ BattleList          queued;
    /* 0x01C */ BattleCmdQueueEntry entries[16];
} BattleCmdQueue; // Size: 0x11C

/**
 * @brief Queues an action for the partner
 *
 * When the queue is full the last queued action is replaced. Clears the partner's ready flag.
 *
 * @param value The action to queue
 */
void BtlCmdQueue_Push(u16 value);

/**
 * @brief Drops every queued action
 */
void BtlCmdQueue_Clear(void);

/**
 * @brief Records the direction of the combo chain the player picked
 *
 * @param direction The chain direction (2 = right), 0 = none
 */
void BtlCmdQueue_SetDirection(s32 direction);

/**
 * @brief Takes the oldest queued action off the queue
 *
 * @return The action, or 0 if the queue is empty
 */
u16 BtlCmdQueue_Pop(void);

/**
 * @brief Reads and clears the recorded chain direction
 *
 * @return The direction set by BtlCmdQueue_SetDirection, or 0 if none was recorded since the last call
 */
u16 BtlCmdQueue_TakeDirection(void);

/**
 * @brief Whether the partner has finished its action and can take another
 *
 * @return TRUE if the partner is ready
 */
BOOL BtlCmdQueue_IsPartnerReady(void);

/**
 * @brief Whether the partner is mid-jump
 *
 * @return TRUE while the partner is jumping
 */
BOOL BtlCmdQueue_IsPartnerJumping(void);

/**
 * @brief Sets whether the partner has finished its action and can take another
 *
 * @param ready TRUE when the partner is ready
 */
void BtlCmdQueue_SetPartnerReady(BOOL ready);

/**
 * @brief Sets whether the partner is mid-jump
 *
 * @param jumping TRUE while the partner is jumping
 */
void BtlCmdQueue_SetPartnerJumping(BOOL jumping);

/**
 * @brief Clears BTL_CMD_QUEUE_FLAG_UNK_1, done whenever a combo chain starts
 */
void func_ov003_02086af0(void);

/**
 * @brief Installs and resets the command queue, moving all 16 entries to the free list
 *
 * @param queue The queue to install as the battle's command queue
 */
void BtlCmdQueue_Init(BattleCmdQueue* queue);

#endif // TWEWY_BATTLE_CORE_BTLCMDQUEUE_H
