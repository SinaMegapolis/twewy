#ifndef OTOSUMENU_H
#define OTOSUMENU_H

#include "Engine/Core/Memory.h"
#include "Engine/EasyTask.h"
#include "Engine/Resources/ResourceMgr.h"
#include "Engine/Resources/ScreenMapper.h"
#include "PrcMaster.h"
#include "SpriteMgr.h"
#include "Util/SysFont.h"

#include "common_data.h"

/** @brief Header of a board in data/BeBadge_MapData.bin. */
typedef struct {
    /* 0x0 */ u8 unk_0;
    /* 0x1 */ u8 unk_1;
    /* 0x2 */ u8 width;
    /* 0x3 */ u8 height;
    /* 0x4 */ u8 unk_4[0xC];
} OtosuMenuMapHeader; // Size: 0x10

/** @brief One Tin Pin Slammer participant as the lobby knows them. */
typedef struct {
    /* 0x00 */ u16 name[11];
    /* 0x16 */ u8  unk_16[0x2A - 0x16];
    /* 0x2A */ u8  bssid[6]; // MAC address of the player's console
} OtosuMenuPlayer;           // Size: 0x30

typedef struct {
    /* 0x00000 */ ResourceManager    unk_00000;
    /* 0x11580 */ ResourceManager*   unk_11580;
    /* 0x11584 */ s32                unk_11584;
    /* 0x11588 */ s32                unk_11588;
    /* 0x1158C */ s32                unk_1158C;
    /* 0x11590 */ Heap               heap;
    /* 0x1159C */ u8                 heapBuffer[0x30000];
    /* 0x4159C */ TaskPool           taskPool;
    /* 0x4161C */ PrcCtx             mainCtx;
    /* 0x41804 */ PrcMaster          prcMaster;
    /* 0x4180C */ s32                unk_4180C;
    /* 0x41810 */ s32                unk_41810;
    /* 0x41814 */ s32                unk_41814;
    /* 0x41818 */ s32                unk_41818;
    /* 0x4181C */ u16                ownName[11];
    /* 0x41832 */ u8                 ownPlayer;  // this console's index in players[]
    /* 0x41834 */ u16                playerMask; // bit per connected player
    /* 0x41836 */ u16                unk_41836;
    /* 0x41838 */ OtosuMenuPlayer    players[4];
    /* 0x418F8 */ char               unk_418F8[0x41950 - 0x418F8];
    /* 0x41950 */ s32                unk_41950;
    /* 0x41954 */ s32                unk_41954;
    /* 0x41958 */ u16                unk_41958;
    /* 0x4195A */ u16                unk_4195A[4]; // per player
    /* 0x41962 */ u16                unk_41962[4]; // per player
    /* 0x4196A */ u16                unk_4196A;
    /* 0x4196C */ u16                unk_4196C;
    /* 0x4196E */ s16                unk_4196E;
    /* 0x41970 */ char               unk_41970[0x4198A - 0x41970];
    /* 0x4198A */ u16                unk_4198A;
    /* 0x4198C */ s32                unk_4198C;
    /* 0x41990 */ s32                unk_41990;
    /* 0x41994 */ s32                unk_41994;
    /* 0x41998 */ u16                unk_41998;
    /* 0x4199A */ u16                unk_4199A;
    /* 0x4199C */ u16                unk_4199C;
    /* 0x4199E */ u16                unk_4199E;
    /* 0x419A0 */ s32                unk_419A0;
    /* 0x419A4 */ char               unk_419A4[0x419F0 - 0x419A4];
    /* 0x419F0 */ u16*               unk_419F0;
    /* 0x419F4 */ char               unk_419F4[0x41E38 - 0x419F4];
    /* 0x41E38 */ s32                unk_41E38;
    /* 0x41E3C */ char               unk_41E3C[0x41EEC - 0x41E3C];
    /* 0x41EEC */ u32                unk_41EEC;
    /* 0x41EF0 */ s32                unk_41EF0;
    /* 0x41EF4 */ char               unk_41EF4[0x41FB6 - 0x41EF4];
    /* 0x41FB6 */ u8                 connectedBssids[4][6]; // players[].bssid of the connected players, packed in order
    /* 0x41FCE */ char               unk_41FCE[0x41FD0 - 0x41FCE];
    /* 0x41FD0 */ s32                unk_41FD0;
    /* 0x41FD4 */ u8                 unk_41FD4;
    /* 0x41FD5 */ u8                 placePlayers[4];  // player in each finishing place, best score first
    /* 0x41FD9 */ u8                 placeRanks[4];    // rank shown for each place (ties share a rank)
    /* 0x41FDD */ u8                 playerRanks[4];   // rank of each player
    /* 0x41FE1 */ char               unk_41FE1[0x41FE4 - 0x41FE1];
    /* 0x41FE4 */ BOOL               firstPlaceUnique; // no other player tied the winner's score
    /* 0x41FE8 */ u8                 unk_41FE8;
    /* 0x41FE9 */ u8                 unk_41FE9;
    /* 0x41FEA */ char               unk_41FEA[0x41FF0 - 0x41FEA];
    /* 0x41FF0 */ s32                unk_41FF0;
    /* 0x41FF4 */ char               unk_41FF4[0x45FF4 - 0x41FF4];
    /* 0x45FF4 */ SysFont            font;
    /* 0x46070 */ s16                unk_46070;
    /* 0x46072 */ char               unk_46072[0x46074 - 0x46072];
    /* 0x46074 */ u32                nextScene; // 0 stays in the menu; otherwise which overlay OtosuMenu_Update switches to
    /* 0x46078 */ Sprite             cursor;
    /* 0x460B8 */ BOOL               cursorActive;
    /* 0x460BC */ s32                unk_460BC;
    /* 0x460C0 */ Sprite             linkLevelIcon;
    /* 0x46100 */ PrcCtx             linkLevelCtx;
    /* 0x462E8 */ s32                unk_462E8;
    /* 0x462EC */ s32                unk_462EC;
    /* 0x462F0 */ Data*              packs[4];
    /* 0x46300 */ s32                unk_46300;
    /* 0x46304 */ OtosuMenuMapHeader mapHeader; // board map from data/BeBadge_MapData.bin
    /* 0x46314 */ u8*                mapCells;  // mapHeader.width * mapHeader.height cells, two bytes each
    /* 0x46318 */ void*              mapExtra1;
    /* 0x4631C */ void*              mapExtra2;
    /* 0x46320 */ u16                tilemap[32][64]; // generated screen for a text layer
    /* 0x47320 */ BgResource*        subChars[4];     // indexed by BG layer
    /* 0x47330 */ BgResource*        mainChars[4];
    /* 0x47340 */ void*              subPalette;
    /* 0x47344 */ void*              mainPalette;
    /* 0x47348 */ ScreenMapEntry     subMaps[4];
    /* 0x473E8 */ ScreenMapEntry     mainMaps[4];
    /* 0x47488 */ void*              subScreens[4][2]; // screen blocks each map is built from
    /* 0x474A8 */ void*              mainScreens[4][2];
    /* 0x474C8 */ u16                unk_474C8;
    /* 0x474CA */ u16                unk_474CA;
    /* 0x474CC */ u16                unk_474CC;
    /* 0x474CE */ u16                unk_474CE;
    /* 0x474D0 */ u16                unk_474D0;
    /* 0x474D2 */ u16                unk_474D2;
    /* 0x474D4 */ char               unk_474D4[0x474D8 - 0x474D4];
    /* 0x474D8 */ s32                unk_474D8;
    /* 0x474DC */ s32                unk_474DC;
    /* 0x474E0 */ s32                unk_474E0;
    /* 0x474E4 */ s32                unk_474E4;
    /* 0x474E8 */ PrcCtx             objCtx[6]; // contexts of the screen's own objects (icons, title enemies, rank boards...)
    /* 0x48058 */ s32                unk_48058;
    /* 0x4805C */ char               unk_4805C[0x48068 - 0x4805C];
} OtosuMenuObj; // Size: 0x48068

/**
 * @brief Launcher for displaying a test screen that shows pin symbols
 * and a cyclable text field containing either battle tutorials or generic samples
 */
void ProcessOverlay_OtosuMenu_FontList(void* menuObj);

/**
 * @brief Launcher for starting single player mode of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_SinglePlayerEnter(void* menuObj);

/**
 * @brief Launcher for displaying single player rankings of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_SinglePlayerRanking(void* menuObj);

/**
 * @brief Launcher for starting multiplayer mode of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_MultiplayerEnter(void* menuObj);

/**
 * @brief Launcher for displaying multiplayer rankings of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_MultiplayerRanking(void* menuObj);

/**
 * @brief Launcher for displaying connection error screen of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_ConnectionError(void* menuObj);

/**
 * @brief Launcher for the role selection screen of Tin Pin Slammer
 */
void ProcessOverlay_OtosuMenu_RoleSelection(void* menuObj);

#endif // OTOSUMENU_H