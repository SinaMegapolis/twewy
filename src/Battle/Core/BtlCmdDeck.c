#include "Battle/Core/BtlCmdDeck.h"
#include "Battle/Core/BtlObsMgr.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/CombatAbility.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Friend/Private/Beat.h"
#include "Combat/Friend/Private/Shiki.h"
#include "Combat/Friend/Shiki.h"
#include "Engine/Math/Random.h"
#include "Player/Inventory.h"
#include "Save.h"

#include <nitro/fx.h>
#include <nitro/math.h>

/// Matched cards needed for each fusion star, on top of `fusionsUsed`, one row per partner.
static const s8 BtlCmdDeck_StarThresholds[4][3] = {
    {4, 8, 16},
    {4, 8, 16},
    {4, 8, 16},
    {0, 0,  0},
};

/// Awards every fusion star the progress has reached. Returns TRUE if any was awarded.
static BOOL BtlCmdDeck_AdvanceStars(BattleCmdDeck* deck) {
    BOOL advanced = FALSE;

    while (deck->progress >= deck->threshold) {
        if (deck->stars >= deck->maxStars) {
            break;
        }
        deck->stars++;
        deck->threshold = deck->fusionsUsed + BtlCmdDeck_StarThresholds[deck->partner][deck->stars];
        advanced        = TRUE;
    }
    return advanced;
}

void BtlCmdDeck_Shuffle(u16* table, u32 count) {
    u16 value;

    s16 i = 0;
    value = 0;
    while (count > value) {
        s32 target = RNG_Next(i + 1);
        s32 j      = i;
        while (j > target) {
            table[j] = table[j - 1];
            j--;
        }

        table[target] = value;
        i++;
        value++;
    }
}

void BtlCmdDeck_EaseToSine(s32* value, s16* index) {
    *value += ((SIN2(FX_SinCosTable_, (u16)(*index << 9)) * 4) - *value) / 8;
    *index += 1;
}

void BtlCmdDeck_Refill(void) {
    BattleCmdDeck* deck = gBattle->cmdDeck;

    switch (deck->partner) {
        case PARTNER_JOSHUA:
            deck->remaining = func_ov005_020e74fc(&deck->cards[0], deck->capacity, deck->partnerData);
            break;
        case PARTNER_SHIKI:
        case PARTNER_BEAT:
        default:
            BtlCmdDeck_Shuffle(&deck->cards[0], deck->capacity);
            deck->remaining = deck->capacity;
            break;
    }
}

u16 BtlCmdDeck_DealCard(void) {
    BattleCmdDeck* deck = gBattle->cmdDeck;
    u16            card;

    if (deck->partner == PARTNER_BEAT) {
        return func_ov006_020e7538(deck->partnerData);
    }
    if (deck->remaining == 0) {
        BtlCmdDeck_Refill();
    }
    deck->remaining--;
    card                         = deck->cards[deck->remaining];
    deck->cards[deck->remaining] = 0xFFFF;
    return card;
}

void BtlCmdDeck_RenderCard(BattleSprite* sprite, s32 card, BOOL flipped, s32 x, s16 y, s16 width) {
    switch (gBattle->cmdDeck->partner) {
        case PARTNER_SHIKI:
        case PARTNER_JOSHUA: {
            u16 anim = card * 2;

            if (flipped) {
                anim++;
            }
            BtlSprite_SetAnim(sprite, anim, 0);
            BtlSprite_Update(sprite);
            BtlSprite_SetPosition(sprite, x, y);
            BtlSprite_Render(sprite);
            break;
        }
        case PARTNER_BEAT:
            func_ov006_020e73b8(sprite, card, flipped, x, y, width);
            break;
    }
}

void BtlCmdDeck_UseCard(s32 card, s32 x, s32 y, s32 width) {
    BattleCmdDeck* deck = gBattle->cmdDeck;

    if (!(deck->flags & BTL_CMD_DECK_FLAG_DISABLED)) {
        s32 position = x + (width << 12);

        switch (deck->partner) {
            case PARTNER_BEAT:
                func_ov006_020ea6c0(card, position, y);
                break;
            case PARTNER_JOSHUA:
                func_ov005_020eb29c(card, position, y);
                break;
            case PARTNER_SHIKI:
            default:
                BtlCmd_Esp_CreateTask(card, position, y);
                break;
        }
    }
}

s16* BtlCmdDeck_GetPartnerData(void) {
    return gBattle->cmdDeck->partnerData;
}

/// Fusion stars the active partner's Fusion Boost stickers allow, clamped to 1..3.
static u8 BtlCmdDeck_GetMaxStars(void) {
    u32 count = 0;

    switch (gBattle->partnerId) {
        case PARTNER_SHIKI:
            count = Inventory_GetOwnedCount(ITEM_STICKER_FUSION_BOOST_SHIKI);
            break;
        case PARTNER_JOSHUA:
            count = Inventory_GetOwnedCount(ITEM_STICKER_FUSION_BOOST_JOSHUA);
            break;
        case PARTNER_BEAT:
            count = Inventory_GetOwnedCount(ITEM_STICKER_FUSION_BOOST_BEAT);
            break;
        default:
            break;
    }
    if (count < 1) {
        count = 1;
    }
    if (count > 3) {
        count = 3;
    }
    return count;
}

BOOL BtlCmdDeck_AddProgress(s32 amount, s32 x) {
    BattleCmdDeck* deck = gBattle->cmdDeck;

    if (deck->stars >= deck->maxStars) {
        return FALSE;
    }
    func_ov003_0209d038(amount, x + 7, 18);
    deck->progress += amount;
    return BtlCmdDeck_AdvanceStars(deck);
}

BOOL BtlCmdDeck_IsRoundComplete(void) {
    BOOL done = FALSE;

    switch (gBattle->cmdDeck->partner) {
        case PARTNER_JOSHUA:
            if (*BtlCmdDeck_GetPartnerData() >= 5) {
                done = TRUE;
            }
            break;
        case PARTNER_BEAT:
            done = TRUE;
            break;
        default:
        case PARTNER_SHIKI:
            if (*BtlCmdDeck_GetPartnerData() >= 3) {
                done = TRUE;
            }
            break;
    }
    return done;
}

s8 BtlCmdDeck_GetStars(void) {
    return gBattle->cmdDeck->stars;
}

void BtlCmdDeck_StartFusion(void) {
    BattleCmdDeck* deck  = gBattle->cmdDeck;
    s8             stars = deck->stars;
    u16            level = stars - 1;

    deck->fusionsUsed++;
    deck->stars     = 0;
    deck->progress  = 0;
    deck->threshold = deck->fusionsUsed + BtlCmdDeck_StarThresholds[deck->partner][deck->stars];
    BtlCmdDeck_DealBoard(TRUE);
    func_ov003_020c1f18(&gBattle->specialPool, level);
}

void BtlCmdDeck_Init(BattleCmdDeck* deck, u8 partner, void* partnerData) {
    gBattle->cmdDeck  = deck;
    deck->partner     = partner;
    deck->partnerData = partnerData;
    deck->flags       = 0;
    deck->stars       = 0;
    deck->maxStars    = BtlCmdDeck_GetMaxStars();
    deck->fusionsUsed = gSaveData.battleChargeStage;
    deck->progress    = gSaveData.battleChargeProgress;
    deck->threshold   = deck->fusionsUsed + BtlCmdDeck_StarThresholds[deck->partner][deck->stars];
    if (gBattle->chainIndex == 0 && gSaveData.battleTimer == 0 && CombatAbility_IsActive(7)) {
        deck->progress += (s8)CombatAbility_GetValue(7);
    }
    BtlCmdDeck_AdvanceStars(deck);
    switch (deck->partner) {
        case PARTNER_JOSHUA:
            deck->capacity = 10;
            break;
        case PARTNER_BEAT:
            deck->capacity = 4;
            break;
        default:
        case PARTNER_SHIKI:
            deck->capacity = func_ov004_020e737c();
            break;
    }
    BtlCmdDeck_Refill();
}

void BtlCmdDeck_Save(BattleCmdDeck* deck) {
    gSaveData.battleChargeProgress = deck->progress;
    gSaveData.battleChargeStage    = deck->fusionsUsed;
}

void BtlCmdDeck_DealBoard(BOOL redeal) {
    BattleCmdDeck* deck = gBattle->cmdDeck;

    switch (deck->partner) {
        case PARTNER_JOSHUA:
            func_ov005_020e740c(deck->partnerData, redeal);
            break;
        case PARTNER_BEAT:
            func_ov006_020e74b0(deck->partnerData, redeal);
            break;
        default:
        case PARTNER_SHIKI:
            func_ov004_020e739c(deck->partnerData, redeal);
            break;
    }
}

void BtlCmdDeck_Disable(void) {
    gBattle->cmdDeck->flags |= BTL_CMD_DECK_FLAG_DISABLED;
}
