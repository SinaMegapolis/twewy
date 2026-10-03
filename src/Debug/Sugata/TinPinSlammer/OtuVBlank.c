#include "Debug/Sugata/TinPinSlammer.h"
#include "Display.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "common_data.h"
#include <nitro/gx.h>

/**
 * @file OtuVBlank.c
 * @brief The scene's VBlank handlers and the setup wrappers that install them.
 *
 * One handler per scene variant. Both commit the frame and push both engines'
 * OAM and both palettes to VRAM; the wireless one runs the two wireless stack
 * routines first, since they have to finish before the frame is committed.
 */

/// The plain scene's VBlank handler.
void func_ov039_02083750(void) {
    if (SystemStatusFlags.vblank) {
        Display_Commit();
        DMA_Flush();
        DC_PurgeRange(&g_OamMgr[DISPLAY_MAIN].oam, 0x400);
        GX_LoadOam(g_OamMgr[DISPLAY_MAIN].oam, 0, 0x400);
        DC_PurgeRange(&g_OamMgr[DISPLAY_SUB].oam, 0x400);
        GXs_LoadOam(g_OamMgr[DISPLAY_SUB].oam, 0, 0x400);
        DC_PurgeRange(&data_02066aec, 0x400);
        GX_LoadBgPltt(&data_02066aec, 0, 0x200);
        GX_LoadObjPltt(&data_02066cec, 0, 0x200);
        DC_PurgeRange(&data_02066eec, 0x400);
        GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
        GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
        func_02001b44(2, 0, &data_020672ec, 0x400);
    }
}

/// The wireless scene's VBlank handler.
void func_ov039_02083838(void) {
    if (SystemStatusFlags.vblank) {
        func_ov039_02088454();
        func_ov039_020885cc();
        Display_Commit();
        DMA_Flush();
        DC_PurgeRange(&g_OamMgr[DISPLAY_MAIN].oam, 0x400);
        GX_LoadOam(g_OamMgr[DISPLAY_MAIN].oam, 0, 0x400);
        DC_PurgeRange(&g_OamMgr[DISPLAY_SUB].oam, 0x400);
        GXs_LoadOam(g_OamMgr[DISPLAY_SUB].oam, 0, 0x400);
        DC_PurgeRange(&data_02066aec, 0x400);
        GX_LoadBgPltt(&data_02066aec, 0, 0x200);
        GX_LoadObjPltt(&data_02066cec, 0, 0x200);
        DC_PurgeRange(&data_02066eec, 0x400);
        GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
        GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
        func_02001b44(2, 0, &data_020672ec, 0x400);
    }
}

/**
 * @brief Bring the GX/display hardware up, then install the plain VBlank pair.
 *
 * Shared by both scene entry points; only the VBlank callback installed
 * afterwards differs, which is why there are two near-identical wrappers.
 */
void func_ov039_02083928(void) {
    func_ov039_020832c0();
    Interrupts_RegisterVBlankCallback(func_ov039_02083750, TRUE);
}

/// Bring the hardware up and install the wireless VBlank pair.
void func_ov039_02083944(void) {
    func_ov039_020832c0();
    Interrupts_RegisterVBlankCallback(func_ov039_02083838, TRUE);
}

/// Detach whatever VBlank callback the scene installed.
void func_ov039_02083960(void) {
    Interrupts_RegisterVBlankCallback(NULL, TRUE);
}
