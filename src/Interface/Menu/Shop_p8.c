#include "ShopPart.h"

// Views of the read-only SE tables data_ov043_020ca2c0 / data_ov043_020ca2a8.
typedef struct {
    /* 0x00 */ s32 v[4];
} ShopSeRow; // Size: 0x10

typedef struct {
    /* 0x00 */ ShopSeRow rows[10];
} ShopSeTable; // Size: 0xA0

typedef struct {
    /* 0x00 */ s32 v[6];
} ShopSeList; // Size: 0x18

void func_ov043_020b1fc4(void *arg0, u8 arg1, s32 arg2) {
    ShopSeTable table = *(ShopSeTable *)&data_ov043_020ca2c0;
    ShopSeList  list  = *(ShopSeList *)&data_ov043_020ca2a8;
    u16         idx;

    if (arg1 == 0xB) {
        return;
    }
    if (arg1 == 0xA) {
        if ((u32)arg2 > 5U) {
            OS_WaitForever();
        }
        if (arg2 == 0) {
            idx = RNG_Next(0xFFFF) % 3;
        } else if (arg2 == 2) {
            idx = (u16)((RNG_Next(0xFFFF) % 2) + 4);
        } else {
            idx = (u16)(arg2 + 2);
        }
        SndMgr_StartPlayingSE((SndMgrSeIdx)list.v[idx]);
        return;
    }
    if ((u32)arg2 > 3U) {
        OS_WaitForever();
    }
    if (arg2 == 2) {
        arg2 = (u16)(arg2 + RNG_Next(0xFFFF) % 2);
    }
    SndMgr_StartPlayingSE((SndMgrSeIdx)table.rows[arg1].v[arg2]);
}

void func_ov043_020b2100(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count) {
    res->data        = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8));
    res->charData    = Data_GetPackEntryData(res->data, 1);
    res->screenMap   = Data_GetPackEntryData(res->data, 2);
    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b2300(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 screenIndex, s32 start, u32 count) {
    s32  index;
    u16 *screenMap;

    res->data     = DatMgr_LoadRawData(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8));
    res->charData = Data_GetPackEntryData(res->data, 1);

    if (res->data == NULL || (index = screenIndex + 2) <= 0) {
        screenMap = NULL;
    } else {
        u8 *base  = (u8 *)res->data->buffer + 0x20;
        screenMap = (u16 *)(base + *(s32 *)(base + index * 8));
    }
    res->screenMap = screenMap;

    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b2514(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex) {
    u16 *screenMap;

    if (data == NULL || screenIndex <= 0) {
        screenMap = NULL;
    } else {
        u8 *base   = (u8 *)data->buffer + 0x20;
        screenMap = (u16 *)(base + *(s32 *)(base + screenIndex * 8));
    }
    res->screenMap = screenMap;

    if (engine == 0) {
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        return;
    }
    res->screenResource = BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                               g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                               g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
}

void func_ov043_020b25c0(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, u16 packIndex, s32 start, u32 count) {
    res->data        = DatMgr_LoadPackEntry(1, NULL, 0, (BinIdentifier *)((u8 *)&data_ov043_020ca198 + binIndex * 8), packIndex, FALSE);
    res->charData    = Data_GetPackEntryData(res->data, 1);
    res->screenMap   = Data_GetPackEntryData(res->data, 2);
    res->paletteData = Data_GetPackEntryData(res->data, 3);

    if (engine == 0) {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[0], res->charData,
                                                 g_DisplaySettings.engineState[0].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[0], res->screenMap,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[0].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[0], res->paletteResource);
    } else {
        u32 charSize = (*(u32 *)res->charData & ~0xFF) >> 8;
        if ((*(u8 *)res->charData & 0xF0) == 0) {
            charSize -= 4;
        }
        res->charResource = BgResMgr_AllocChar32(g_BgResourceManagers[1], res->charData,
                                                 g_DisplaySettings.engineState[1].bgSettings[layer].charBase, 0, charSize);
        res->screenResource =
            BgResMgr_AllocScreen(g_BgResourceManagers[1], res->screenMap,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenBase,
                                 g_DisplaySettings.engineState[1].bgSettings[layer].screenSizeText);
        res->paletteResource = PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], res->paletteData, 0, start, count);
        PaletteMgr_Flush(g_PaletteManagers[1], res->paletteResource);
    }
}

void func_ov043_020b27cc(MenuBgResource *res, s32 engine) {
    if (engine == 0) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[0], res->charResource);
        BgResMgr_ReleaseScreen(g_BgResourceManagers[0], res->screenResource);
        PaletteMgr_ReleaseResource(g_PaletteManagers[0], res->paletteResource);
    } else {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], res->charResource);
        BgResMgr_ReleaseScreen(g_BgResourceManagers[1], res->screenResource);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], res->paletteResource);
    }
    DatMgr_ReleaseData(res->data);
}

void func_ov043_020b2854(MenuBgResource *res, s32 engine) {
    if (engine == 0) {
        BgResMgr_ReleaseScreen(g_BgResourceManagers[0], res->screenResource);
        return;
    }
    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], res->screenResource);
}

void func_ov043_020b288c(MenuBgResource *res, s32 engine, s32 layer, s32 binIndex, s32 start, u32 count) {
    func_ov043_020b27cc(res, engine);
    func_ov043_020b2100(res, engine, layer, binIndex, start, count);
}

void func_ov043_020b28d4(MenuBgResource *res, Data *data, s32 engine, s32 layer, s32 screenIndex) {
    func_ov043_020b2854(res, engine);
    func_ov043_020b2514(res, data, engine, layer, screenIndex);
}

void func_ov043_020b2910(MenuBgResource *res) {
    res->data            = 0;
    res->screenResource  = 0;
    res->charResource    = 0;
    res->paletteResource = 0;
    res->screenMap       = 0;
    res->charData        = 0;
    res->paletteData     = 0;
}

void func_ov043_020b2934(ShopObject *shop) {
    s32             i;
    MenuBgResource *sub  = &shop->resources[0];
    MenuBgResource *main = &shop->resources[4];

    for (i = 0; i < 4; i++) {
        func_ov043_020b2910(sub);
        func_ov043_020b2910(main);
        sub++;
        main++;
    }
    func_ov043_020b2100(&shop->resources[5], 0, 1, 0, 0xF, 1);
    if (shop->slots[shop->unk_848].itemID == 0xFFFF) {
        func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 0, 0, 6);
    } else {
        s32 category = Inventory_GetCategory(shop->slots[shop->unk_848].itemID);
        if (category == 0) {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 8, 0, 6);
        } else if (category == 1) {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 0, 0, 6);
        } else {
            func_ov043_020b2300(&shop->resources[7], 0, 3, 1, 2, 0, 6);
        }
    }
    func_ov043_020b2514(&shop->resources[6], shop->resources[7].data, 0, 2, 9);
    func_ov043_020b2100(&shop->resources[1], 1, 1, 6, 0xF, 1);
    func_ov043_020b25c0(&shop->resources[3], 1, 3, 7, (u16)shop->unk_76A, 0, 0xE);
}

void func_ov043_020b2af0(void) {

}

// NOTE: the ShopPart.h prototype is `func_ov043_020b2af4(s32)`, and the header is out
// of scope for this cleanup, so the definition cannot take `ShopObject *` directly.
void func_ov043_020b2af4(s32 arg0) {
    ShopObject *shop = (ShopObject *)arg0;

    func_ov043_020b27cc(&shop->resources[5], 0);
    func_ov043_020b2854(&shop->resources[6], 0);
    func_ov043_020b27cc(&shop->resources[7], 0);
    func_ov043_020b27cc(&shop->resources[1], 1);
    func_ov043_020b27cc(&shop->resources[3], 1);
}
/* Warning: struct OamCellPiece is not defined (only forward-declared) */
/* Warning: struct OSMutex is not defined (only forward-declared) */
/* Warning: struct OamAffineParam is not defined (only forward-declared) */
