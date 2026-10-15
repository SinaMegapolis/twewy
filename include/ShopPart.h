#ifndef INTERFACE_MENU_SHOPPART_H
#define INTERFACE_MENU_SHOPPART_H

#include "Interface/Menu/Shop.h"
#include "Interface/Menu/MenuCommon.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/Core/HBlank.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/ResourceMgr.h"
#include "Player/Inventory.h"
#include "Save.h"
#include "SndMgr.h"
#include "Display.h"
#include "EasyFade.h"
#include "common_data.h"
#include <nitro/fs/overlay.h>
#include <nitro/mi/cpumem.h>
#include <nitro/reg.h>

// Root state object for src/Interface/Menu/Shop.c (the Shop menu dispatcher).
typedef struct {
    /* 0x00000 */ MenuStateBase base;
    /* 0x21618 */ s32           taskId_NameD;
    /* 0x2161C */ s32           taskId_Exit;
    /* 0x21620 */ s32           unk_21620;
    /* 0x21624 */ s32           taskId_NumMoneyNek;
    /* 0x21628 */ s32           taskId_NumMoneyBdg;
    /* 0x2162C */ s32           taskId_Sbar;
    /* 0x21630 */ s32           taskId_Check;
    /* 0x21634 */ s32           taskId_Tab;
    /* 0x21638 */ s32           taskId_TextScr;
    /* 0x2163C */ s32           taskId_Item[12];
    /* 0x2166C */ s32           taskId_ItemPlate[12];
    /* 0x2169C */ s32           taskId_ItemPrice[12];
    /* 0x216CC */ s32           taskId_ItemAbl[12];
    /* 0x216FC */ s32           taskId_ItemMask[12];
    /* 0x2172C */ s32           taskId_ItemStar[12];
    /* 0x2175C */ s32           taskId_ItemCur;
    /* 0x21760 */ s32           taskId_ItemTotal;
    /* 0x21764 */ s32           taskId_ItemTotalNum;
    /* 0x21768 */ s32           unk_21768;
    /* 0x2176C */ s32           unk_2176C;
    /* 0x21770 */ s32           taskId_BrdLogo;
    /* 0x21774 */ s32           unk_21774;
    /* 0x21778 */ s32           unk_21778;
    /* 0x2177C */ s32           unk_2177C;
    /* 0x21780 */ s32           taskId_Pointer;
    /* 0x21784 */ s32           taskId_NameU;
    /* 0x21788 */ s32           unk_21788;
    /* 0x2178C */ s32           taskId_NumLvU;
    /* 0x21790 */ s32           taskId_NumAblU;
    /* 0x21794 */ s32           taskId_NumFsgU;
    /* 0x21798 */ s32           taskId_FsgGaugeU;
    /* 0x2179C */ s32           taskId_TextScrU;
    /* 0x217A0 */ s32           taskId_WindowU;
    /* 0x217A4 */ s32           taskId_CastU;
    /* 0x217A8 */ s32           taskId_BoardU;
    /* 0x217AC */ s32           unk_217AC;
    /* 0x217B0 */ u16           unk_217B0;
    /* 0x217B2 */ u16           unk_217B2;
    /* 0x217B4 */ ShopObject    shop;
} ShopState; // Size: 0x22108

extern ShopState* data_ov043_020cd288;
extern int data_02066cec;
extern int data_020670ec;
extern int data_020672ec;
extern int data_ov043_020ca18c;
extern int data_ov043_020ca198;
extern BinIdentifier data_ov043_020ca210;
extern BinIdentifier data_ov043_020ca218;
extern BinIdentifier data_ov043_020ca220;
extern BinIdentifier data_ov043_020ca228;
extern BinIdentifier data_ov043_020ca230;
extern BinIdentifier data_ov043_020ca238;
extern BinIdentifier data_ov043_020ca240;
extern BinIdentifier data_ov043_020ca248;
extern BinIdentifier data_ov043_020ca250;
extern int data_ov043_020ca260;
extern int data_ov043_020ca2a8;
extern int data_ov043_020ca2c0;
extern s8 *data_ov043_020cc5bc;
extern s8 data_ov043_020cc82c;
extern s8 data_ov043_020cc838;
extern s8 data_ov043_020cc844;
extern s8 data_ov043_020cc850;
extern s8 data_ov043_020cc860;
extern s8 data_ov043_020cc870;
extern s8 data_ov043_020cc880;
extern int func_ov043_020bd414;

// Prototypes for the part files currently included in Shop.c: Shop_p2.c, Shop_p4.c, Shop_p5.c, Shop_p8.c
void func_ov043_020adaa4(ShopState *arg0);
void func_ov043_020adaf4(ShopState *arg0);
void func_ov043_020adbb0(ShopState *arg0);
void func_ov043_020add48(ShopState *arg0);
void func_ov043_020ade08(ShopState *arg0);
void func_ov043_020ade9c(ShopState *arg0);
void func_ov043_020adef8(ShopState *arg0);
void func_ov043_020adfcc(ShopState *arg0);
void func_ov043_020ae0b0(ShopState *arg0);
void func_ov043_020ae138(ShopState *arg0);
void func_ov043_020ae1b8(ShopState *arg0);
void func_ov043_020ae24c(ShopState *arg0);
void func_ov043_020ae284(ShopState *arg0);
void func_ov043_020ae580(ShopState *arg0);
void func_ov043_020ae614(ShopState *arg0);
BOOL func_ov043_020af3f0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s16 arg4, s16 arg5);
void func_ov043_020af42c(Sprite *sprite, s16 animFrame);
void func_ov043_020af478(Sprite *sprite, s16 animFrame);
void func_ov043_020af4c4(void *arg0, s32 arg1);
void func_ov043_020af4f0(void *arg0, s32 arg1);
void func_ov043_020af51c(void *arg0, s32 arg1);
void func_ov043_020af548(void *arg0, s32 arg1);
void func_ov043_020af574(void *arg0, s32 arg1);
void func_ov043_020af5a0(void *arg0, s32 arg1);
void func_ov043_020af5cc(void *arg0, s32 arg1);
void func_ov043_020af5f8(void *arg0, s32 arg1);
void func_ov043_020af624(void *arg0, s32 arg1);
void func_ov043_020af648(void *arg0, s32 arg1);
void func_ov043_020af66c(void *arg0, s32 arg1);
void func_ov043_020af690(void *arg0, s32 arg1);
void func_ov043_020af6b4(void *arg0, s32 arg1);
void func_ov043_020af6dc(void *arg0, s32 arg1);
void func_ov043_020af700(void *arg0, s32 arg1);
void func_ov043_020af724(ShopObject *arg0);
u16 func_ov043_020af770(u16 arg0, u32 arg1);
u16 func_ov043_020af8a8(ShopObject *arg0, u32 arg1, u32 arg2);
u16 func_ov043_020af8e4(ShopObject *arg0);
u16 func_ov043_020af950(ShopObject *arg0);
u32 func_ov043_020af994(ShopObject *arg0);
u16 func_ov043_020af9e8(ShopObject *arg0);
void func_ov043_020afa20(ShopObject *arg0);
u16 func_ov043_020afa68(ShopObject *arg0);
u16 func_ov043_020afabc(ShopObject *arg0);
u16 func_ov043_020afb14(ShopObject *arg0);
void func_ov043_020afb60(ShopObject *arg0);
void func_ov043_020b1fc4(void *arg0, u8 arg1, s32 arg2);
void func_ov043_020b2100(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count);
void func_ov043_020b2300(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 screenIndex, s32 start, u32 count);
void func_ov043_020b2514(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex);
void func_ov043_020b25c0(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, u16 packIndex, s32 start, u32 count);
void func_ov043_020b27cc(MenuBgResource *res, s32 engine);
void func_ov043_020b2854(MenuBgResource *res, s32 engine);
void func_ov043_020b288c(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count);
void func_ov043_020b28d4(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex);
void func_ov043_020b2910(MenuBgResource *res);
void func_ov043_020b2934(ShopObject *shop);
void func_ov043_020b2af0(void);
void func_ov043_020b2af4(s32 arg0);

#endif // INTERFACE_MENU_SHOPPART_H
