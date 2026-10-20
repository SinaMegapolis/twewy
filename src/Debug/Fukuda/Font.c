#include "Display.h"
#include "Engine/Core/Interrupts.h"
#include "Engine/Core/Memory.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/Core/System.h"
#include "Engine/File/DatMgr.h"
#include "Engine/IO/TouchInput.h"
#include "Engine/Overlay/OverlayDispatcher.h"
#include "Engine/Overlay/OverlayManager.h"
#include "Engine/Resources/ResourceMgr.h"
#include "SpriteMgr.h"
#include "Util/SysFont.h"
#include "common_data.h"
#include <nitro/types.h>

typedef struct {
    /* 0x00000 */ ResourceManager  unk_00000;
    /* 0x11580 */ ResourceManager* unk_11580;
    /* 0x11584 */ s32              unk_11584;
    /* 0x11588 */ char             unk_11588[0x10428];
    /* 0x219B0 */ SysFont          screenFont;
    /* 0x21A2C */ SysFont          spriteFont;
    /* 0x21AA8 */ void*            fontPalette;
    /* 0x21AAC */ PaletteResource* paletteResource;
    /* 0x21AB0 */ Sprite           windowSprite;
    /* 0x21AF0 */ Sprite           textSprite;
    /* 0x21B30 */ s16              unk_21B30;
    /* 0x21B32 */ char             unk_21B32[0xA];
    /* 0x21B3C */ u8*              screenMapEntry;
    /* 0x21B40 */ u8*              characterEntry;
} FontState; // Size: 0x21B44

const BinIdentifier data_ov001_02082f98 = {1, "Apl_Fuk/Grp_FldMessWin.bin"};
const BinIdentifier data_ov001_02082f90 = {1, "Apl_Fuk/Grp_ForTest.bin"};

void             FontTest_RegisterVBlank(void);
SpriteFrameInfo* func_ov001_020826e4(Sprite* sprite, s32 arg, s32 mode);

static const SpriteAnimation data_ov001_02082ff0 = {
    .bits_0_1          = 0,
    .dataType          = 0,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 1,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02            = {.raw = 0},
    .posX              = 50,
    .posY              = 50,
    .frameInfoCallback = func_ov001_020826e4,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = &data_ov001_02082f98,
    .unk_18            = 2,
    .packIndex         = 0,
    .unk_1C            = 0,
    .unk_1E            = 0,
    .unk_20            = 0,
    .unk_22            = 3,
    .unk_24            = 0,
    .unk_26            = 0,
    .unk_28            = 0,
    .animIndex         = 1,
};

void func_ov001_020824a0(FontState* arg0) {
    // Unused retail test vectors still occupy the shared constant pool.
    SysCode glyphOrder[] = {1, 2, 4, 6, 3, 5, 22, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
    s32     origin[2]    = {-64, -64};
    data_02066eec        = 0;
    SysFont_InitWithFont(&arg0->screenFont, 1, TRUE);
    SysFont_SetPos(&arg0->screenFont, 0, 0);
    arg0->screenFont.widthScale = 1;
    SysFont_SetPos(&arg0->screenFont, 0, 0);
    SysFont_SetMsg(&arg0->screenFont, SYSMSG_FONT_CHECK_TEXT);
    // Grp_ForTest entries include a four-byte graphics header before their payloads.
    SysFont_DrawCurrentToScreen(&arg0->screenFont, arg0->screenMapEntry + 4, arg0->characterEntry + 4, 0);
    SysFont_Destroy(&arg0->screenFont);
}

void func_ov001_0208254c(FontState* arg0) {
    // Preserve the second retail sample initializer even though it is unused.
    SysCode unusedSample[] = {4, SYSFONT_CODE_LINEBREAK, 5, 6, 7, SYSFONT_CODE_STR_END};
    SysFont_InitWithFont(&arg0->spriteFont, 1, TRUE);
    SysCode sample[] = {4, SYSFONT_CODE_LINEBREAK, 5, 6, 7, SYSFONT_CODE_STR_END};
    SysFont_SetPos(&arg0->spriteFont, 20, 10);
    SysFont_SetMsgPtr(&arg0->spriteFont, sample);
    SysFont_DrawCurrentToSprite(&arg0->spriteFont, &arg0->textSprite, TRUE);
    SysFont_Destroy(&arg0->spriteFont);
}

void func_ov001_020825f4(FontState* arg0) {
    PaletteMgr_ReleaseResource(g_PaletteManagers[1], arg0->paletteResource);
    Mem_Free(&gDebugHeap, arg0->fontPalette);
}

void func_ov001_0208262c(FontState* arg0) {
    s32   dataSlot = DatMgr_AllocateSlot();
    Data* graphics;
    u8*   characters;
    u8*   screenMap;

    arg0->fontPalette = SysFont_GetAllocPal(0);
    // The retail routine discards this return value; preserve that behavior.
    PaletteMgr_AllocPalette(g_PaletteManagers[1], arg0->fontPalette, 0, 0, 1);
    graphics             = DatMgr_LoadPackEntry(dataSlot, NULL, 0, &data_ov001_02082f90, 1, TRUE);
    characters           = Data_GetPackEntryData(graphics, 1);
    screenMap            = Data_GetPackEntryData(graphics, 2);
    arg0->characterEntry = characters;
    arg0->screenMapEntry = screenMap;
}

SpriteFrameInfo* func_ov001_020826e4(Sprite* sprite, s32 arg, s32 mode) {
    SpriteFrameInfo* result = NULL;
    switch (mode) {
        case SPRITE_FRAME_UPDATE:
            g_SpriteFrameInfo.updateSteps = 1;
            return &g_SpriteFrameInfo;
        case SPRITE_FRAME_RENDER: {
            SpriteFrameInfo* info = &g_SpriteFrameInfo;
            Sprite_FillFrameInfo(info, sprite);
            result = info;
        } break;
    }
    return result;
}

void func_ov001_02082780(FontState* arg0) {
    SpriteAnimation animation = data_ov001_02082ff0;

    animation.packIndex = 22;
    animation.unk_1C    = 2;
    animation.unk_20    = 1;
    animation.unk_26    = 3;
    animation.unk_28    = 4;
    animation.animIndex = 1;
    animation.dataType  = DatMgr_AllocateSlot();
    _Sprite_Load(&arg0->windowSprite, &animation);

    animation.packIndex = 1;
    animation.animIndex = 1;
    animation.posX      = 200;
    _Sprite_Load(&arg0->textSprite, &animation);

    arg0->windowSprite.cellIndex = 1;
    arg0->textSprite.cellIndex   = 1;
    // Both retail stores address the same field; the second overwrites the first.
    arg0->unk_21B30 = arg0->windowSprite.charData->bitmapIndex / 4;
    arg0->unk_21B30 = arg0->textSprite.charData->bitmapIndex / 4;
}

void func_ov001_0208288c(FontState* arg0) {
    FS_LoadOverlay(0, &OVERLAY_31_ID);
    func_ov001_0208262c(arg0);
    func_ov001_02082780(arg0);
    Sprite_Update(&arg0->windowSprite);
    Sprite_RenderFrame(&arg0->windowSprite);
    Sprite_Update(&arg0->textSprite);
    Sprite_RenderAltPalette(&arg0->textSprite, 0, 0, 2);
    OamMgr_CommitMain();
    func_ov001_020824a0(arg0);
    DC_PurgeAll();

    s32 val = (s32)arg0->screenMapEntry + 4;
    func_0203ab7c(3, val, G2S_GetBG0ScrPtr(), 0x800);

    val = (s32)arg0->characterEntry + 4;
    func_0203ab7c(3, val, G2S_GetBG0CharPtr(), 0x8000);

    PaletteMgr_Flush(g_PaletteManagers[0], 0);
    PaletteMgr_Flush(g_PaletteManagers[1], 0);
}

void func_ov001_02082984(FontState* arg0) {
    FS_UnloadOverlay(0, &OVERLAY_31_ID);
    Sprite_Release(&arg0->windowSprite);
    Sprite_Release(&arg0->textSprite);
    func_ov001_020825f4(arg0);
}

static const char* data_ov001_02083020 = "Seq_Text()";

// Nonmatching: the display and sub-palette literal-pool entries are reversed.
void FontTest_Init(FontState* arg0) {
    char* sequence = data_ov001_02083020;
    arg0           = Mem_AllocHeapTail(&gDebugHeap, sizeof(FontState));
    Mem_SetSequence(&gDebugHeap, arg0, sequence);
    MainOvlDisp_SetCbArg(arg0);

    arg0->unk_11584 = DatMgr_AllocateSlot();
    FontTest_RegisterVBlank();
    arg0->unk_11580 = ResourceMgr_ReinitManagers(&arg0->unk_00000);

    *(volatile u16*)&data_02066aec                                       = 0;
    data_02066eec                                                        = 0;
    *(volatile s32*)&g_DisplaySettings.controls[DISPLAY_MAIN].brightness = 0;
    g_DisplaySettings.controls[DISPLAY_SUB].brightness                   = 0;

    func_ov001_0208288c(arg0);
    MainOvlDisp_NextProcessStage();
}

void FontTest_Update(FontState* arg0) {
    TouchInput_Update();
    Input_PollState(&InputStatus);
    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_A) {
        func_ov001_0208254c(arg0);
    }

    if (func_02041458() == FALSE) {
        SysControl.buttonState.pressedButtons = 0;
        SysControl.buttonState.currButtons    = 0;
        TouchInput_Init();
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_SELECT) {
        OverlayTag tag;
        MainOvlDisp_Pop(&tag);
    }

    if (SysControl.buttonState.pressedButtons & INPUT_BUTTON_R) {
        Mem_ValidateSequences(&gDebugHeap);
        Mem_ValidateSequences(&gMainHeap);
    }
}

void FontTest_Destroy(FontState* arg0) {
    func_ov001_02082984(arg0);
    ResourceMgr_ReinitManagers(NULL);
    DatMgr_ClearSlot(arg0->unk_11584);
    Mem_Free(&gDebugHeap, arg0);
}

static const OverlayProcess OvlProc_FontTest = {
    .init = FontTest_Init,
    .main = FontTest_Update,
    .exit = FontTest_Destroy,
};

void ProcessOverlay_FontTest(FontState* arg0) {
    s32 stage = MainOvlDisp_GetProcessStage();
    if (stage == PROCESS_STAGE_EXIT) {
        FontTest_Destroy(arg0);
    } else {
        OvlProc_FontTest.funcs[stage](arg0);
    }
}

void func_ov001_02082b74(void) {
    Interrupts_Init();
    HBlank_Init();
    GX_Init();
    func_0202b878();
    DMA_Init(256);
    Display_Init();
    GX_DisableBankForLcdc();
    GX_SetBankForLcdc(0x1ff);
    GX_SetBankForTex(3);
    GX_SetBankForTexPltt(0x40);
    GX_SetBankForBg(0x20);
    GX_SetBankForObj(0x10);
    GX_SetBankForBgExtPltt(0);
    GX_SetBankForObjExtPltt(0);
    GX_SetBankForSubBg(4);
    GX_SetBankForSubObj(8);
    GX_SetBankForSubBgExtPltt(0x80);
    GX_SetBankForSubObjExtPltt(0x100);
    MI_CpuFill(0, 0x6800000, 0xa4000);
    MI_CpuFill(0, 0x6000000, 0x80000);
    MI_CpuFill(0, 0x6200000, 0x20000);
    MI_CpuFill(0, 0x6400000, 0x40000);
    MI_CpuFill(0, 0x6600000, 0x20000);
    MI_CpuFill(0, 0x6800000, 0xa4000);
    Display_CommitSynced();

    g_DisplaySettings.controls[DISPLAY_MAIN].dispMode  = GX_DISPMODE_GRAPHICS;
    g_DisplaySettings.controls[DISPLAY_MAIN].bgMode    = GX_BGMODE_0;
    g_DisplaySettings.controls[DISPLAY_MAIN].dimension = GX2D3D_MODE_3D;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX2D3D_MODE_3D);

    Display_InitMainBG1(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 16, 0, 0, 0x1000);

    g_DisplaySettings.engineState[0].bgSettings[0].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[1].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[2].priority = 1;
    g_DisplaySettings.engineState[0].bgSettings[3].priority = 1;

    g_DisplaySettings.engineState[0].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[2].mosaic = 0;
    g_DisplaySettings.engineState[0].bgSettings[3].mosaic = 0;

    g_DisplaySettings.controls[DISPLAY_MAIN].objTileMode = GX_OBJTILEMODE_1D_32K;
    g_DisplaySettings.controls[DISPLAY_MAIN].objBmpMode  = GX_OBJBMPMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode  = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode   = GX_OBJBMPMODE_1D_128K;
    Display_SetMainLayers(LAYER_BG0 | LAYER_OBJ);
    data_0206aa78                                  = 0x300010;
    data_0206aa7c                                  = 0x400040;
    g_DisplaySettings.controls[DISPLAY_SUB].bgMode = GX_BGMODE_0;

    GXs_SetGraphicsMode(0);

    Display_InitSubBG0(DISPLAY_BGMODE_TEXT, GX_BG_SIZE_TEXT_256x256, GX_BG_COLORS_16, 0, 1, 0, 0x4);

    g_DisplaySettings.engineState[1].bgSettings[0].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[1].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[2].priority = 3;
    g_DisplaySettings.engineState[1].bgSettings[3].priority = 3;

    g_DisplaySettings.engineState[1].bgSettings[0].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[1].mosaic = 0;
    g_DisplaySettings.engineState[1].bgSettings[2].mosaic = 0;
    // Carry the cleared value into the layer mask to preserve retail code generation.
    u32 emptyLayers = (g_DisplaySettings.engineState[1].bgSettings[3].mosaic = 0);

    g_DisplaySettings.controls[DISPLAY_SUB].objTileMode = GX_OBJTILEMODE_1D_128K;
    g_DisplaySettings.controls[DISPLAY_SUB].objBmpMode  = GX_OBJBMPMODE_1D_128K;

    g_DisplaySettings.controls[DISPLAY_SUB].layers = emptyLayers | LAYER_BG0 | LAYER_OBJ;

    REG_POWER_CNT &= ~0x8000;
    OamMgr_Init3DSpritePipeline();
    g_DisplaySettings.engineState[0].blendMode   = 1;
    g_DisplaySettings.engineState[0].blendLayer0 = 1;
    g_DisplaySettings.engineState[0].blendLayer1 = 63;
    g_DisplaySettings.engineState[0].blendCoeff0 = 10;
    g_DisplaySettings.engineState[0].blendCoeff1 = 6;
    OamMgr_InitExtended();
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_MAIN]);
    OamMgr_ResetCommandQueues(&g_OamMgr[DISPLAY_SUB]);
}

void FontTest_VBlank(void) {
    if (SystemStatusFlags.vblank) {
        Display_Commit();
        DMA_Flush();
        OamMgr_Commit();
        DC_PurgeRange(&data_02066aec, 0x400);
        GX_LoadBgPltt(&data_02066aec, 0, 0x200);
        GX_LoadObjPltt(&data_02066cec, 0, 0x200);
        DC_PurgeRange(&data_02066eec, 0x400);
        GXs_LoadBgPltt(&data_02066eec, 0, 0x200);
        GXs_LoadObjPltt(&data_020670ec, 0, 0x200);
    }
}

void FontTest_RegisterVBlank(void) {
    func_ov001_02082b74();
    Interrupts_RegisterVBlankCallback(FontTest_VBlank, TRUE);
}
