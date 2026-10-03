#include "Debug/Sugata/TinPinSlammer.h"
#include "Display.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/OamMgr.h"
#include "common_data.h"
#include "nitro/reg.h"
#include <nitro/gx.h>

/**
 * @file OtuGxInit.c
 * @brief The scene's GX/display bring-up.
 *
 * Brings up both display engines in text mode: four 512x256 16-colour text
 * layers on the main engine and four 256x256 ones on the sub engine, with a
 * fixed VRAM bank layout, the 3D pipeline's orthographic projection, blending
 * set up, and both OAM engines initialised.  Entirely straight-line -- there is
 * no branch in the function.
 *
 * The VRAM bank arguments and the three raw BG control-register pokes are
 * written as plain hex, matching the target.  The BG control writes go through
 * REG_BGxCNT because there is no Display.h helper that pokes the control
 * register directly rather than going through g_DisplaySettings.
 */

// Nonmatching: 95.9%. Prologue and size match exactly, and every call, VRAM
// bank, field value and constant is right. The remainder is register choice
// inside the g_DisplaySettings init block: the target keeps its base in r3 and
// folds the two data_0206aa7x stores between the field writes, while this build
// reuses lr and hoists them. Rewriting the seven BG blocks to use Display.h's
// Display_InitMainBG0..3 / Display_InitSubBG0..3 helpers took this from 77.5%.
void func_ov039_020832c0(void) {
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    func_0202b878();
    DMA_Init(0x100);
    Display_Init();

    // VRAM bank layout: textures in A, main BGs in B, sub BGs in C, sub OAM in
    // D, main OAM in E, tex pltt in G, and the two ext-pltt banks in H and I.
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(GX_VRAM_ALL);
    GX_SetBankForTex(GX_VRAM_A);
    GX_SetBankForTexPltt(GX_VRAM_G);
    GX_SetBankForBg(GX_VRAM_B);
    GX_SetBankForObj(GX_VRAM_E);
    GX_SetBankForSubObj(GX_VRAM_D);
    GX_SetBankForSubBg(GX_VRAM_C);
    GX_SetBankForSubObjExtPltt(GX_VRAM_I);
    GX_SetBankForSubBgExtPltt(GX_VRAM_H);

    // Clear each engine's VRAM region, blank the 3D layer, then clear again --
    // the second pass catches what the first display commit makes visible.
    MI_CpuFill(0, (void*)0x06800000, 0xA4000);
    MI_CpuFill(0, (void*)0x06000000, 0x80000);
    MI_CpuFill(0, (void*)0x06200000, 0x20000);
    MI_CpuFill(0, (void*)0x06400000, 0x40000);
    MI_CpuFill(0, (void*)0x06600000, 0x20000);
    *(vu16*)0x04000304 &= ~0x8000;
    MI_CpuFill(0, (void*)0x06800000, 0xA4000);
    Display_CommitSynced();

    g_DisplaySettings.controls[DISPLAY_MAIN].objTileMode = GX_OBJTILEMODE_1D_32K;
    g_DisplaySettings.controls[DISPLAY_MAIN].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode  = GX_OBJTILEMODE_1D_32K;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode   = GX_OBJBMPMODE_1D_128K;

    data_0206aa78 = 0x300010;
    data_0206aa7c = 0x400040;

    g_DisplaySettings.controls[DISPLAY_MAIN].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[DISPLAY_MAIN].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[DISPLAY_MAIN].dimension = GX2D3D_MODE_3D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_3D);

    // Main engine: four 512x256 16-colour text layers, then the control-register
    // pokes that set the same base/char/slot triples the fields above describe.
    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 0, 2, 0, 0x4008);

    Display_InitMainBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 2, 4, 1, 0x4210);

    Display_InitMainBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_512x256, GX_BG_COLORS_16, 6, 6, 1, 0x4618);

    // Priority runs back-to-front on the main engine; mosaic stays off.
    Display_GetBG0Settings(DISPLAY_MAIN)->priority  = 2;
    Display_GetBG1Settings(DISPLAY_MAIN)->priority  = 3;
    Display_GetBG2Settings(DISPLAY_MAIN)->priority  = 0;
    Display_GetBG3Settings(DISPLAY_MAIN)->priority  = 1;
    Display_GetBG0Settings(DISPLAY_MAIN)->mosaic    = 0;
    Display_GetBG1Settings(DISPLAY_MAIN)->mosaic    = 0;
    Display_GetBG2Settings(DISPLAY_MAIN)->mosaic    = 0;
    Display_GetBG3Settings(DISPLAY_MAIN)->mosaic    = 0;
    g_DisplaySettings.controls[DISPLAY_MAIN].layers = 0x13;
    g_DisplaySettings.controls[DISPLAY_MAIN].layers = 0x13 | 0xC;

    // Sub engine: four 256x256 text layers, same field-then-poke pattern.
    g_DisplaySettings.controls[DISPLAY_SUB].bgMode = GX_BGMODE_0;
    GXs_SetGraphicsMode(GX_BGMODE_0);

    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 2, 0, 0x8);

    Display_InitSubBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 1, 4, 0, 0x110);

    Display_InitSubBG2(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 2, 4, 1, 0x210);

    Display_InitSubBG3(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 3, 6, 1, 0x318);

    // Front-to-back on the sub engine.
    Display_GetBG0Settings(DISPLAY_SUB)->priority = 0;
    Display_GetBG1Settings(DISPLAY_SUB)->priority = 1;
    Display_GetBG2Settings(DISPLAY_SUB)->priority = 2;
    Display_GetBG3Settings(DISPLAY_SUB)->priority = 3;
    Display_GetBG0Settings(DISPLAY_SUB)->mosaic   = 0;
    Display_GetBG1Settings(DISPLAY_SUB)->mosaic   = 0;
    Display_GetBG2Settings(DISPLAY_SUB)->mosaic   = 0;
    Display_GetBG3Settings(DISPLAY_SUB)->mosaic   = 0;
    Display_SetSubLayers(0x11);

    // 3D layer: disable, then set the 3D-only display bits, then project.
    REG_DISP3DCNT = (REG_DISP3DCNT & ~0x302) | 2;
    REG_DISP3DCNT &= ~0xCFDF;
    OamMgr_Init3DSpritePipeline();
    G3i_OrthoW(0, 0xC0000, 0, 0x100000, 0xF0000000, 0x400000, 0x400000, 1, 0);

    // Alpha blending for the sprite layer.
    g_DisplaySettings.engineState[DISPLAY_MAIN].blendMode   = 0;
    g_DisplaySettings.engineState[DISPLAY_MAIN].blendLayer0 = 8;
    g_DisplaySettings.engineState[DISPLAY_MAIN].blendLayer1 = 0x23;
    g_DisplaySettings.engineState[DISPLAY_MAIN].blendCoeff0 = 5;
    g_DisplaySettings.engineState[DISPLAY_MAIN].blendCoeff1 = 0xA;

    // Both OAM engines.
    OamMgr_InitEngine(0, DISPLAY_MAIN);
    OamMgr_Reset(&g_OamMgr[DISPLAY_MAIN], 0, 0);
    DC_PurgeRange(&g_OamMgr[DISPLAY_MAIN].oam, 0x400);
    GX_LoadOam(g_OamMgr[DISPLAY_MAIN].oam, 0, 0x400);

    OamMgr_InitEngine(0, DISPLAY_SUB);
    OamMgr_Reset(&g_OamMgr[DISPLAY_SUB], 0, 0);
    DC_PurgeRange(&g_OamMgr[DISPLAY_SUB].oam, 0x400);
    GXs_LoadOam(g_OamMgr[DISPLAY_SUB].oam, 0, 0x400);

    OamMgr_InitEngine(0, DISPLAY_EXTENDED);
    OamMgr_SetAffineCount(&g_OamMgr[DISPLAY_EXTENDED], 0);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);
}
