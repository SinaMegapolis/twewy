#include "ShopPart.h"

void func_ov043_020b1fc4(void *arg0, u8 arg1, s32 arg2) {
    ? sp18;
    ? *var_lr;
    ? *var_r6;
    s32 temp_r0;
    s32 temp_r1;
    s32 temp_r2;
    s32 temp_r3;
    s32 var_ip;
    s32 var_r4;
    u16 var_r1;
    u32 temp_r0_2;
    u32 temp_r0_3;
    u32 temp_r0_4;
    u32 temp_r1_2;
    u32 temp_r1_3;
    u32 temp_ret;

    var_r6 = &data_ov043_020ca2c0;
    var_lr = &sp18;
    var_r4 = arg2;
    var_ip = 0xA;
    do {
        temp_r0 = var_r6->unk_0;
        temp_r1 = var_r6->unk_4;
        temp_r2 = var_r6->unk_8;
        temp_r3 = var_r6->unk_C;
        var_r6 += 0x10;
        var_lr->unk_0 = temp_r0;
        var_lr->unk_4 = temp_r1;
        var_lr->unk_8 = temp_r2;
        var_lr->unk_C = temp_r3;
        var_lr += 0x10;
        var_ip -= 1;
    } while (var_ip != 0);
    sp0[0].unk_0 = data_ov043_020ca2a8.unk_0;
    sp0[0].unk_4 = (s32) data_ov043_020ca2a8.unk_4;
    sp0[0].unk_8 = (s32) data_ov043_020ca2a8.unk_8;
    sp0[0].unk_C = (s32) data_ov043_020ca2a8.unk_C;
    sp0[0].unk_10 = (s32) data_ov043_020ca2a8.unk_10;
    (&sp0[0] + 0x10)->unk_4 = (s32) (&data_ov043_020ca2a8 + 0x10)->unk_4;
    if (arg1 == 0xB) {
        return;
    }
    if (arg1 == 0xA) {
        if ((u32) var_r4 > 5U) {
            OS_WaitForever();
        }
        if (var_r4 == 0) {
            temp_ret = RNG_Next(0xFFFF);
            temp_r0_2 = temp_ret;
            var_r1 = temp_r0_2 - (3 * ((temp_ret / 3) + (temp_r0_2 >> 0x1F)));
        } else if (var_r4 != 2) {
            var_r1 = var_r4 + 2;
        } else {
            temp_r0_3 = RNG_Next(0xFFFF);
            temp_r1_2 = temp_r0_3 >> 0x1F;
            var_r1 = temp_r1_2 + ROTATE_RIGHT((temp_r0_3 << 0x1F) - temp_r1_2, 0x1F) + 4;
        }
        SndMgr_StartPlayingSE((&sp0[0])[var_r1]);
        return;
    }
    if ((u32) var_r4 > 3U) {
        OS_WaitForever();
    }
    if (var_r4 == 2) {
        temp_r0_4 = RNG_Next(0xFFFF);
        temp_r1_3 = temp_r0_4 >> 0x1F;
        var_r4 = (s32) (u16) (var_r4 + (temp_r1_3 + ROTATE_RIGHT((temp_r0_4 << 0x1F) - temp_r1_3, 0x1F)));
    }
    SndMgr_StartPlayingSE(*(&sp18 + (arg1 * 0x10) + (var_r4 * 4)));
}

void func_ov043_020b2100(void *arg0, s32 arg4, u32 arg5) {
    Data *temp_r0;
    Data *temp_r0_2;
    Data *temp_r0_3;
    s32 *temp_r1_4;
    s32 *var_r0;
    s32 temp_r2;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5;
    u32 var_r2;
    u32 var_r2_2;
    void *temp_r1;
    void *temp_r1_2;
    void *temp_r1_3;
    void *var_r0_2;
    void *var_r0_3;

    temp_r5 = M2C_ERROR(/* Read from unset register $r2 */);
    temp_r0 = DatMgr_LoadRawData(1, NULL, 0, &data_ov043_020ca198 + (M2C_ERROR(/* Read from unset register $r3 */) * 8));
    arg0->unk_0 = temp_r0;
    if (temp_r0 == NULL) {
        var_r0 = NULL;
    } else {
        temp_r1 = temp_r0->buffer + 0x20;
        var_r0 = temp_r1 + temp_r1->unk_8;
    }
    arg0->unk_10 = var_r0;
    temp_r0_2 = arg0->unk_0;
    if (temp_r0_2 == NULL) {
        var_r0_2 = NULL;
    } else {
        temp_r1_2 = temp_r0_2->buffer + 0x20;
        var_r0_2 = temp_r1_2 + temp_r1_2->unk_10;
    }
    arg0->unk_14 = var_r0_2;
    temp_r0_3 = arg0->unk_0;
    if (temp_r0_3 == NULL) {
        var_r0_3 = NULL;
    } else {
        temp_r1_3 = temp_r0_3->buffer + 0x20;
        var_r0_3 = temp_r1_3 + temp_r1_3->unk_18;
    }
    arg0->unk_18 = var_r0_3;
    temp_r1_4 = arg0->unk_10;
    temp_r2 = *temp_r1_4;
    if (M2C_ERROR(/* Read from unset register $r1 */) == 0) {
        var_r2 = (u32) (temp_r2 & ~0xFF) >> 8;
        temp_r4 = temp_r5 * 0x3C;
        if (!((u8) *temp_r1_4 & 0xF0)) {
            var_r2 -= 4;
        }
        arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers->unk_0, temp_r1_4, *(&g_DisplaySettings.engineState[0].bgSettings[0].charBase + temp_r4), 0U, var_r2);
        arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers->unk_0, arg0->unk_14, *(&g_DisplaySettings.engineState[0].bgSettings[0].screenBase + temp_r4), *(&g_DisplaySettings.engineState[0].bgSettings[0].screenSizeText + temp_r4));
        arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers->unk_0, arg0->unk_18, 0U, (s16) arg4, arg5);
        PaletteMgr_Flush(g_PaletteManagers->unk_0, arg0->unk_C);
        return;
    }
    var_r2_2 = (u32) (temp_r2 & ~0xFF) >> 8;
    temp_r4_2 = temp_r5 * 0x3C;
    if (!((u8) *temp_r1_4 & 0xF0)) {
        var_r2_2 -= 4;
    }
    arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers[1], temp_r1_4, *(&g_DisplaySettings.engineState[1].bgSettings[0].charBase + temp_r4_2), 0U, var_r2_2);
    arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers[1], arg0->unk_14, *(&g_DisplaySettings.engineState[1].bgSettings[0].screenBase + temp_r4_2), *(&g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText + temp_r4_2));
    arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers[1], arg0->unk_18, 0U, (s16) arg4, arg5);
    PaletteMgr_Flush(g_PaletteManagers[1], arg0->unk_C);
}

void func_ov043_020b2300(void *arg0, s32 arg4, s32 arg5, u32 arg6) {
    Data *temp_r0;
    Data *temp_r0_2;
    Data *temp_r1_2;
    s32 *temp_r1_5;
    s32 *var_r0;
    s32 temp_r2;
    s32 temp_r2_2;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5;
    u32 var_r2;
    u32 var_r2_2;
    void *temp_r1;
    void *temp_r1_3;
    void *temp_r1_4;
    void *var_r0_2;
    void *var_r0_3;

    temp_r5 = M2C_ERROR(/* Read from unset register $r2 */);
    temp_r0 = DatMgr_LoadRawData(1, NULL, 0, &data_ov043_020ca198 + (M2C_ERROR(/* Read from unset register $r3 */) * 8));
    arg0->unk_0 = temp_r0;
    if (temp_r0 == NULL) {
        var_r0 = NULL;
    } else {
        temp_r1 = temp_r0->buffer + 0x20;
        var_r0 = temp_r1 + temp_r1->unk_8;
    }
    arg0->unk_10 = var_r0;
    temp_r1_2 = arg0->unk_0;
    if ((temp_r1_2 == NULL) || (temp_r2 = arg4 + 2, (temp_r2 <= 0))) {
        var_r0_2 = NULL;
    } else {
        temp_r1_3 = temp_r1_2->buffer + 0x20;
        var_r0_2 = temp_r1_3 + *(temp_r1_3 + (temp_r2 * 8));
    }
    arg0->unk_14 = var_r0_2;
    temp_r0_2 = arg0->unk_0;
    if (temp_r0_2 == NULL) {
        var_r0_3 = NULL;
    } else {
        temp_r1_4 = temp_r0_2->buffer + 0x20;
        var_r0_3 = temp_r1_4 + temp_r1_4->unk_18;
    }
    arg0->unk_18 = var_r0_3;
    temp_r1_5 = arg0->unk_10;
    temp_r2_2 = *temp_r1_5;
    if (M2C_ERROR(/* Read from unset register $r1 */) == 0) {
        var_r2 = (u32) (temp_r2_2 & ~0xFF) >> 8;
        temp_r4 = temp_r5 * 0x3C;
        if (!((u8) *temp_r1_5 & 0xF0)) {
            var_r2 -= 4;
        }
        arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers->unk_0, temp_r1_5, *(&g_DisplaySettings.engineState[0].bgSettings[0].charBase + temp_r4), 0U, var_r2);
        arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers->unk_0, arg0->unk_14, *(&g_DisplaySettings.engineState[0].bgSettings[0].screenBase + temp_r4), *(&g_DisplaySettings.engineState[0].bgSettings[0].screenSizeText + temp_r4));
        arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers->unk_0, arg0->unk_18, 0U, (s16) arg5, arg6);
        PaletteMgr_Flush(g_PaletteManagers->unk_0, arg0->unk_C);
        return;
    }
    var_r2_2 = (u32) (temp_r2_2 & ~0xFF) >> 8;
    temp_r4_2 = temp_r5 * 0x3C;
    if (!((u8) *temp_r1_5 & 0xF0)) {
        var_r2_2 -= 4;
    }
    arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers[1], temp_r1_5, *(&g_DisplaySettings.engineState[1].bgSettings[0].charBase + temp_r4_2), 0U, var_r2_2);
    arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers[1], arg0->unk_14, *(&g_DisplaySettings.engineState[1].bgSettings[0].screenBase + temp_r4_2), *(&g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText + temp_r4_2));
    arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers[1], arg0->unk_18, 0U, (s16) arg5, arg6);
    PaletteMgr_Flush(g_PaletteManagers[1], arg0->unk_C);
}

void func_ov043_020b2514(void *arg0, s32 arg4) {
    s32 temp_ip;
    s32 temp_r1;
    void *var_r0;

    if ((M2C_ERROR(/* Read from unset register $r1 */) == 0) || (arg4 <= 0)) {
        var_r0 = NULL;
    } else {
        temp_r1 = M2C_ERROR(/* Read from unset register $r1 */)->unk_8 + 0x20;
        var_r0 = temp_r1 + *(temp_r1 + (arg4 * 8));
    }
    arg0->unk_14 = var_r0;
    temp_ip = M2C_ERROR(/* Read from unset register $r3 */) * 0x3C;
    if (M2C_ERROR(/* Read from unset register $r2 */) == 0) {
        arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers->unk_0, arg0->unk_14, *(&g_DisplaySettings.engineState[0].bgSettings[0].screenBase + temp_ip), *(&g_DisplaySettings.engineState[0].bgSettings[0].screenSizeText + temp_ip));
        return;
    }
    arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers[1], arg0->unk_14, *(&g_DisplaySettings.engineState[1].bgSettings[0].screenBase + temp_ip), *(&g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText + temp_ip));
}

void func_ov043_020b25c0(void *arg0, u16 arg4, s32 arg5, u32 arg6) {
    Data *temp_r0;
    Data *temp_r0_2;
    Data *temp_r0_3;
    s32 *temp_r1_4;
    s32 *var_r0;
    s32 temp_r2;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r5;
    u32 var_r2;
    u32 var_r2_2;
    void *temp_r1;
    void *temp_r1_2;
    void *temp_r1_3;
    void *var_r0_2;
    void *var_r0_3;

    temp_r5 = M2C_ERROR(/* Read from unset register $r2 */);
    temp_r0 = DatMgr_LoadPackEntry(1, NULL, 0, &data_ov043_020ca198 + (M2C_ERROR(/* Read from unset register $r3 */) * 8), (s32) arg4, 0);
    arg0->unk_0 = temp_r0;
    if (temp_r0 == NULL) {
        var_r0 = NULL;
    } else {
        temp_r1 = temp_r0->buffer + 0x20;
        var_r0 = temp_r1 + temp_r1->unk_8;
    }
    arg0->unk_10 = var_r0;
    temp_r0_2 = arg0->unk_0;
    if (temp_r0_2 == NULL) {
        var_r0_2 = NULL;
    } else {
        temp_r1_2 = temp_r0_2->buffer + 0x20;
        var_r0_2 = temp_r1_2 + temp_r1_2->unk_10;
    }
    arg0->unk_14 = var_r0_2;
    temp_r0_3 = arg0->unk_0;
    if (temp_r0_3 == NULL) {
        var_r0_3 = NULL;
    } else {
        temp_r1_3 = temp_r0_3->buffer + 0x20;
        var_r0_3 = temp_r1_3 + temp_r1_3->unk_18;
    }
    arg0->unk_18 = var_r0_3;
    temp_r1_4 = arg0->unk_10;
    temp_r2 = *temp_r1_4;
    if (M2C_ERROR(/* Read from unset register $r1 */) == 0) {
        var_r2 = (u32) (temp_r2 & ~0xFF) >> 8;
        temp_r4 = temp_r5 * 0x3C;
        if (!((u8) *temp_r1_4 & 0xF0)) {
            var_r2 -= 4;
        }
        arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers->unk_0, temp_r1_4, *(&g_DisplaySettings.engineState[0].bgSettings[0].charBase + temp_r4), 0U, var_r2);
        arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers->unk_0, arg0->unk_14, *(&g_DisplaySettings.engineState[0].bgSettings[0].screenBase + temp_r4), *(&g_DisplaySettings.engineState[0].bgSettings[0].screenSizeText + temp_r4));
        arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers->unk_0, arg0->unk_18, 0U, (s16) arg5, arg6);
        PaletteMgr_Flush(g_PaletteManagers->unk_0, arg0->unk_C);
        return;
    }
    var_r2_2 = (u32) (temp_r2 & ~0xFF) >> 8;
    temp_r4_2 = temp_r5 * 0x3C;
    if (!((u8) *temp_r1_4 & 0xF0)) {
        var_r2_2 -= 4;
    }
    arg0->unk_8 = BgResMgr_AllocChar32(g_BgResourceManagers[1], temp_r1_4, *(&g_DisplaySettings.engineState[1].bgSettings[0].charBase + temp_r4_2), 0U, var_r2_2);
    arg0->unk_4 = BgResMgr_AllocScreen(g_BgResourceManagers[1], arg0->unk_14, *(&g_DisplaySettings.engineState[1].bgSettings[0].screenBase + temp_r4_2), *(&g_DisplaySettings.engineState[1].bgSettings[0].screenSizeText + temp_r4_2));
    arg0->unk_C = PaletteMgr_AllocPalette(g_PaletteManagers[1], arg0->unk_18, 0U, (s16) arg5, arg6);
    PaletteMgr_Flush(g_PaletteManagers[1], arg0->unk_C);
}

void func_ov043_020b27cc(void *arg0, s32 arg1) {
    if (arg1 == 0) {
        BgResMgr_ReleaseChar(g_BgResourceManagers->unk_0, arg0->unk_8);
        BgResMgr_ReleaseScreen(g_BgResourceManagers->unk_0, arg0->unk_4);
        PaletteMgr_ReleaseResource(g_PaletteManagers->unk_0, arg0->unk_C);
    } else {
        BgResMgr_ReleaseChar(g_BgResourceManagers[1], arg0->unk_8);
        BgResMgr_ReleaseScreen(g_BgResourceManagers[1], arg0->unk_4);
        PaletteMgr_ReleaseResource(g_PaletteManagers[1], arg0->unk_C);
    }
    DatMgr_ReleaseData(arg0->unk_0);
}

void func_ov043_020b2854(void *arg0, s32 arg1) {
    if (arg1 == 0) {
        BgResMgr_ReleaseScreen(g_BgResourceManagers->unk_0, arg0->unk_4);
        return;
    }
    BgResMgr_ReleaseScreen(g_BgResourceManagers[1], arg0->unk_4);
}

void func_ov043_020b288c(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u32 arg5) {
    func_ov043_020b27cc(arg0, arg1);
    func_ov043_020b2100(arg0, /* extra? */ arg4, /* extra? */ arg5);
}

void func_ov043_020b28d4(void *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4) {
    func_ov043_020b2854(arg0, arg2);
    func_ov043_020b2514(arg0, /* extra? */ arg4);
}

void func_ov043_020b2910(void *arg0) {
    arg0->unk_0 = 0;
    arg0->unk_4 = 0;
    arg0->unk_8 = 0;
    arg0->unk_C = 0;
    arg0->unk_14 = 0;
    arg0->unk_10 = 0;
    arg0->unk_18 = 0;
}

void func_ov043_020b2934(void *arg0) {
    s16 temp_r0;
    s32 var_r5;
    void *var_r6;
    void *var_r7;

    var_r6 = arg0 + 0x874;
    var_r7 = arg0 + 0x8E4;
    var_r5 = 0;
    do {
        func_ov043_020b2910(var_r6);
        func_ov043_020b2910(var_r7);
        var_r5 += 1;
        var_r6 += 0x1C;
        var_r7 += 0x1C;
    } while (var_r5 < 4);
    func_ov043_020b2100(arg0 + 0x900, /* extra? */ 0xF, /* extra? */ 1);
    if (*(arg0 + (arg0->unk_848 * 0x2C)) == 0xFFFF) {
        func_ov043_020b2300(arg0 + 0x938, /* extra? */ 0, /* extra? */ 0, /* extra? */ 6);
    } else {
        temp_r0 = Inventory_GetCategory();
        if (temp_r0 == 0) {
            func_ov043_020b2300(arg0 + 0x938, /* extra? */ 8, /* extra? */ 0, /* extra? */ 6);
        } else if (temp_r0 == 1) {
            func_ov043_020b2300(arg0 + 0x938, /* extra? */ 0, /* extra? */ 0, /* extra? */ 6);
        } else {
            func_ov043_020b2300(arg0 + 0x938, /* extra? */ 2, /* extra? */ 0, /* extra? */ 6);
        }
    }
    func_ov043_020b2514(arg0 + 0x91C, /* extra? */ 9);
    func_ov043_020b2100(arg0 + 0x890, /* extra? */ 0xF, /* extra? */ 1);
    func_ov043_020b25c0(arg0 + 0x8C8, /* extra? */ (s32) arg0->unk_76A, /* extra? */ 0, /* extra? */ 0xE);
}

void func_ov043_020b2af0(void) {

}

void func_ov043_020b2af4(s32 arg0) {
    func_ov043_020b27cc(arg0 + 0x900, 0);
    func_ov043_020b2854(arg0 + 0x91C, 0);
    func_ov043_020b27cc(arg0 + 0x938, 0);
    func_ov043_020b27cc(arg0 + 0x890, 1);
    func_ov043_020b27cc(arg0 + 0x8C8, 1);
}
/* Warning: struct OamCellPiece is not defined (only forward-declared) */
/* Warning: struct OSMutex is not defined (only forward-declared) */
/* Warning: struct OamAffineParam is not defined (only forward-declared) */
