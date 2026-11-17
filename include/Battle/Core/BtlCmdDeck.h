#ifndef TWEWY_BATTLE_CORE_BTLCMDDECK_H
#define TWEWY_BATTLE_CORE_BTLCMDDECK_H

#include "Battle/Core/BtlSprite.h"

/// BattleCmdDeck flags.
typedef enum BattleCmdDeckFlag {
    BTL_CMD_DECK_FLAG_DISABLED = (1 << 0), // the card game is switched off for this battle
} BattleCmdDeckFlag;

/// The partner's card game: the shuffled deck dealt onto the board, and the fusion stars the matched cards earn.
typedef struct BattleCmdDeck {
    /* 0x00 */ u8    partner;     // ActivePartner whose card game this is
    /* 0x01 */ s8    stars;       // fusion stars earned
    /* 0x02 */ s8    maxStars;    // fusion stars the partner's Fusion Boost stickers allow, 1..3
    /* 0x03 */ s8    progress;    // cards matched so far, kept between chained battles
    /* 0x04 */ s8    threshold;   // progress needed for the next star
    /* 0x05 */ s8    fusionsUsed; // fusions used, raising every threshold; kept between chained battles
    /* 0x06 */ u16   flags;       // BattleCmdDeckFlag
    /* 0x08 */ u8    unk_08[0x8];
    /* 0x10 */ s16   remaining;   // cards left in the deck
    /* 0x12 */ u16   capacity;    // cards in a full deck
    /* 0x14 */ u16   cards[12];
    /* 0x2C */ void* partnerData; // the partner's own board state
} BattleCmdDeck;                  // Size: 0x30

/**
 * @brief Fills `table` with a random permutation of 0..count-1
 *
 * @param table The table to fill
 * @param count Number of entries to fill
 */
void BtlCmdDeck_Shuffle(u16* table, u32 count);

/**
 * @brief Eases a value an eighth of the way towards a point on a sine wave, then advances the wave
 *
 * @param value The value to ease
 * @param index The wave position, in 1/128ths of a turn; incremented by one
 */
void BtlCmdDeck_EaseToSine(s32* value, s16* index);

/**
 * @brief Refills the deck with a fresh shuffle of the partner's cards
 */
void BtlCmdDeck_Refill(void);

/**
 * @brief Deals the top card of the deck, refilling it first if it is empty
 *
 * @return The card dealt
 */
u16 BtlCmdDeck_DealCard(void);

/**
 * @brief Draws a card of the partner's card game
 *
 * @param sprite The card sprite
 * @param card The card to draw
 * @param flipped TRUE to draw the card's other face
 * @param x Horizontal screen position, pixels
 * @param y Vertical screen position, pixels
 * @param width Board cell width, pixels (negative for a chain running left)
 */
void BtlCmdDeck_RenderCard(BattleSprite* sprite, s32 card, BOOL flipped, s32 x, s16 y, s16 width);

/**
 * @brief Plays a card, starting the partner's attack for it unless the card game is disabled
 *
 * @param card The card played
 * @param x Horizontal screen position of the card, pixels
 * @param y Vertical screen position of the card, pixels
 * @param width Board cell width, pixels (negative for a chain running left)
 */
void BtlCmdDeck_UseCard(s32 card, s32 x, s32 y, s32 width);

/**
 * @brief Returns the partner's own board state
 *
 * @return The board state passed to BtlCmdDeck_Init; it starts with the number of cards matched this round
 */
s16* BtlCmdDeck_GetPartnerData(void);

/**
 * @brief Adds matched cards to the fusion progress, popping up the amount, and awards any stars reached
 *
 * Does nothing once every star allowed has been earned.
 *
 * @param amount Cards matched
 * @param x Horizontal screen position of the popup, pixels
 * @return TRUE if a star was awarded
 */
BOOL BtlCmdDeck_AddProgress(s32 amount, s32 x);

/**
 * @brief Whether the current board round has been played out
 *
 * @return TRUE once 3 (Shiki) or 5 (Joshua) cards were matched this round; always TRUE for Beat
 */
BOOL BtlCmdDeck_IsRoundComplete(void);

/**
 * @brief Returns the fusion stars earned
 *
 * @return The number of stars, 0..3
 */
s8 BtlCmdDeck_GetStars(void);

/**
 * @brief Spends the earned stars on a fusion attack and deals a new board
 *
 * Raises every star threshold by one for the rest of the chain.
 */
void BtlCmdDeck_StartFusion(void);

/**
 * @brief Installs the card game for a partner, restoring the progress carried over from the previous battle
 *
 * @param deck The deck to install as the battle's deck
 * @param partner The ActivePartner whose cards to use
 * @param partnerData The partner's own board state
 */
void BtlCmdDeck_Init(BattleCmdDeck* deck, u8 partner, void* partnerData);

/**
 * @brief Saves the fusion progress for the next chained battle
 *
 * @param deck The deck to save
 */
void BtlCmdDeck_Save(BattleCmdDeck* deck);

/**
 * @brief Deals the partner's board
 *
 * @param redeal FALSE for the first deal of the battle, TRUE to replace a played-out board
 */
void BtlCmdDeck_DealBoard(BOOL redeal);

/**
 * @brief Switches the card game off for the rest of the battle
 */
void BtlCmdDeck_Disable(void);

#endif // TWEWY_BATTLE_CORE_BTLCMDDECK_H
