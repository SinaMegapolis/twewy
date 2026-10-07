#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_0209301c[] = {func_ov002_0208caa0};

PrcFrameDesc OtosuMenu_Icon_FrameDesc = {
    .enter     = func_ov002_0208c9c8,
    .stepTable = data_ov002_0209301c,
    .update    = func_ov002_0208ca80,
    .render    = func_ov002_0208ca90,
    .exit      = func_ov002_0208ca70,
};

SpriteFrameInfo* func_ov002_0208c92c(Sprite* sprite, s32 arg, s32 mode) {
    Sprite_FrameInfoCallback(sprite, mode);
}

static const SpriteAnimation data_ov002_0209219c = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = {.raw = 0x0400},
    .posX              = -13,
    .posY              = 0x000C,
    .frameInfoCallback = func_ov002_0208c92c,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov002_02091acc,
    .unk_18            = 0x0000,
    .packIndex         = 0x0000,
    .unk_1C            = 0x0015,
    .unk_1E            = 0x0000,
    .unk_20            = 0x0018,
    .unk_22            = 0x0002,
    .unk_24            = 0x0000,
    .unk_26            = 0x0016,
    .unk_28            = 0x0017,
    .animIndex         = 0x0001,
};

void func_ov002_0208c9c8(void* arg0, void* arg1, s16* arg2) {
    SpriteAnimation anim = data_ov002_0209219c;

    anim.bits_0_1  = 0;
    anim.animIndex = 1;
    anim.posX      = 0x80;
    anim.posY      = *arg2;
    anim.unk_1C    = 0x11;
    anim.unk_20    = 0x14;
    anim.unk_22    = 2;
    anim.unk_26    = 0x12;
    anim.unk_28    = 0x13;

    if (_Sprite_Load((Sprite*)arg1, &anim) == 0) {
        OS_WaitForever();
    }
}

void func_ov002_0208ca70(PrcCtx* ctx, void* arg1) {
    Sprite_Destroy(arg1);
}

void func_ov002_0208ca80(PrcCtx* ctx, void* arg1) {
    Sprite_UpdateAndCheck(arg1);
}

void func_ov002_0208ca90(PrcCtx* ctx, void* arg1) {
    Sprite_Render(arg1);
}

PrcStepResult func_ov002_0208caa0(PrcCtx* ctx, void* unused) {
    return PRC_STEP_CONTINUE;
}

void func_ov002_0208caa8(OtosuMenuObj* menuObj) {
    PrcCtx* icons[4] = {&menuObj->objCtx[0], &menuObj->objCtx[1], &menuObj->objCtx[2], &menuObj->objCtx[3]};
    u16     i;

    for (i = 0; i < gSaveData.otosuPlayerCount; i++) {
        PrcMaster_UnregisterContext(&menuObj->prcMaster, icons[i]);
    }
    PrcMaster_UnregisterContext(&menuObj->prcMaster, &menuObj->objCtx[4]);
}

void func_ov002_0208cb50(OtosuMenuObj* menuObj) {
    u16 placed    = 0;
    u16 prevScore = 0xFFFF;
    u16 place;
    u16 rank = 0;

    menuObj->firstPlaceUnique = FALSE;
    for (place = 0; place < gSaveData.otosuPlayerCount; place++) {
        u16 i;
        u16 player = 0xFFFF;
        u16 best   = 0;

        for (i = 0; i < gSaveData.otosuPlayerCount; i++) {
            if (!(placed & (1 << i)) && best <= gSaveData.otosuScores[i]) {
                best   = gSaveData.otosuScores[i];
                player = i;
            }
        }
        if (prevScore != 0xFFFF && best != prevScore) {
            if (place == 1) {
                menuObj->firstPlaceUnique = TRUE;
            }
            rank++;
        }
        prevScore                    = best;
        menuObj->placeRanks[place]   = rank;
        menuObj->playerRanks[player] = rank;
        placed |= 1 << player;
        menuObj->placePlayers[place] = player;
    }
}

void func_ov002_0208cc5c(OtosuMenuObj* menuObj) {
    u16 placed    = 0;
    u16 place     = 0;
    u16 rank      = 0;
    u16 prevScore = 0xFFFF;
    u8  winner;

    menuObj->firstPlaceUnique = TRUE;
    for (place = 0; place < gSaveData.otosuPlayerCount; place++) {
        u16 i      = 0;
        u16 player = 0xFFFF;
        u16 best   = 0;

        for (i = 0; i < gSaveData.otosuPlayerCount; i++) {
            if (!(placed & (1 << i)) && best <= gSaveData.otosuScores[i]) {
                best   = gSaveData.otosuScores[i];
                player = i;
            }
        }
        if (prevScore != 0xFFFF && best != prevScore) {
            rank++;
        } else if (place == 1 && player == 0) {
            rank++;
        }
        menuObj->placeRanks[place]   = rank;
        menuObj->playerRanks[player] = rank;
        menuObj->placePlayers[place] = player;
        prevScore                    = best;
        placed |= 1 << player;
    }
    if (menuObj->placePlayers[1] != 0) {
        return;
    }
    winner = menuObj->placePlayers[0];
    if (gSaveData.otosuScores[0] == gSaveData.otosuScores[winner]) {
        menuObj->placePlayers[1] = winner;
        menuObj->placePlayers[0] = 0;
    }
}

s32 func_ov002_0208cdb0(OtosuMenuObj* menuObj, u16 arg1) {
    u8  player    = menuObj->placePlayers[arg1];
    u32 unused[5] = {0, 0, 0, 0, 0};

    return (s32)menuObj->players[player].name;
}
