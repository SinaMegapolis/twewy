#include "Combat/Core/CombatBgRes.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatBgTiles.h"
#include "Combat/Core/CombatSpriteSlot.h"
#include "Engine/Core/DMA.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/BgResMgr.h"
#include "Engine/Resources/PaletteMgr.h"
#include "SpriteMgr.h"

#include <nitro/g2.h>

/// A longer command used by `CombatBgRes_CmdLoadSlot`; the extra operands select the
/// sprite slot and the palette window.
typedef struct Ov003ResSlotCmd {
    /* 0x00 */ s32 (*run)(void* state);
    /* 0x04 */ const void* anims;   // table of 6-byte animation records
    /* 0x08 */ u32         bg;      // { u16 bgIndex; u16 unk }
    /* 0x0C */ u32         slot;    // { u16 slotIndex; u16 charTile }
    /* 0x10 */ u32         palette; // { u8 count; u8 unk; u8 start; u8 unk }
} Ov003ResSlotCmd;                  // Size: 0x14

void CombatBgRes_Init(Ov003BgResState* state, s32 engine, BinIdentifier* iden) {
    s32 i;

    state->engine       = engine;
    state->iden         = iden;
    state->unk_88       = 0;
    state->paletteCount = 0;
    for (i = 0; i < 6; i++) {
        state->charRes[i] = NULL;
    }
    for (i = 0; i < 3; i++) {
        state->palettes[i] = NULL;
    }
    for (i = 0; i < 2; i++) {
        state->slots[i].anims = NULL;
    }
}

void CombatBgRes_RunScript(Ov003BgResState* state, const Ov003ResCmd* script, void* arg) {
    state->cursor    = script;
    state->scriptArg = arg;
    state->data      = NULL;
    while (state->cursor->run(state)) {
    }
}

void CombatBgRes_Update(Ov003BgResState* state) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (state->slots[i].anims != NULL) {
            CombatAnim_SlotUpdate(&state->slots[i]);
        }
    }
}

void CombatBgRes_Release(Ov003BgResState* state) {
    s32 i;

    if (state->data != NULL) {
        DatMgr_ReleaseData(state->data);
    }
    for (i = 0; i < 6; i++) {
        BgResMgr_ReleaseChar(g_BgResourceManagers[state->engine], state->charRes[i]);
    }
    for (i = 0; i < 3; i++) {
        PaletteMgr_ReleaseResource(g_PaletteManagers[state->engine], state->palettes[i]);
    }
    for (i = 0; i < 2; i++) {
        if (state->slots[i].anims != NULL) {
            CombatAnim_SlotRelease(&state->slots[i]);
        }
    }
}

s32 CombatBgRes_CmdEnd(void) {
    return 0;
}

/// Script command: load the pack that holds one palette, then copy it into the next palette slot.
s32 CombatBgRes_CmdLoadPalette(Ov003BgResState* state) {
    const Ov003ResCmd* cmd       = state->cursor;
    u16                packIndex = (cmd->operands0 & 0xFFFF0000) >> 16;
    u16                srcIndex  = (u16)cmd->operands0;
    u16                palStart  = (cmd->operands1 & 0xFFFF0000) >> 16;
    u16                palCount  = (u16)cmd->operands1;
    void*              source;

    if (state->data != NULL) {
        DatMgr_ReleaseData(state->data);
    }
    state->data = DatMgr_LoadPackEntry(1, NULL, 0, state->iden, packIndex, 0);
    source      = Data_GetPackEntryData(state->data, srcIndex);
    if (state->paletteCount < 3) {
        if (state->engine == 0) {
            state->palettes[state->paletteCount] =
                PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[0], source, 0, palStart, palCount);
            PaletteMgr_Flush(g_PaletteManagers[0], state->palettes[state->paletteCount]);
        } else {
            state->palettes[state->paletteCount] =
                PaletteMgr_AllocPaletteNoProto(g_PaletteManagers[1], source, 0, palStart, palCount);
            PaletteMgr_Flush(g_PaletteManagers[1], state->palettes[state->paletteCount]);
        }
        state->paletteCount++;
    }
    state->cursor = &state->cursor[1];
    return 1;
}

/// Script command: copy one character (tile) block of the current pack into BG VRAM.
s32 CombatBgRes_CmdLoadChar(Ov003BgResState* state) {
    const Ov003ResCmd* cmd       = state->cursor;
    u16                bgIndex   = (u16)cmd->operands0;
    u16                packIndex = (cmd->operands0 & 0xFFFF0000) >> 16;
    u16                srcIndex  = (u16)cmd->operands1;
    u16                charTile  = (cmd->operands1 & 0xFFFF0000) >> 16;
    s32                charDest;
    s32                size;
    void*              source;

    if (state->data != NULL) {
        DatMgr_ReleaseData(state->data);
    }
    state->data = DatMgr_LoadPackEntry(1, NULL, 0, state->iden, packIndex, 0);
    source      = Data_GetPackEntryData(state->data, srcIndex);
    if (state->engine == 0) {
        if (bgIndex == 1) {
            charDest = G2_GetBG1CharPtr() + (charTile << 5);
        } else {
            charDest = G2_GetBG2CharPtr() + (charTile << 5);
        }
    } else {
        if (bgIndex == 1) {
            charDest = G2S_GetBG1CharPtr() + (charTile << 6);
        } else {
            charDest = func_02036974() + (charTile << 6);
        }
    }
    size = (((const CombatBgTiles*)source)->header & ~0xFF) >> 8;
    if ((((const CombatBgTiles*)source)->format & 0xF0) == 0) {
        size -= 4;
    }
    func_02001b44(7, charDest, ((const CombatBgTiles*)source)->pixels, size);
    state->cursor = &state->cursor[1];
    return 1;
}

/// Script command: set up one of the two sprite slots and queue its palette.
u32 CombatBgRes_CmdLoadSlot(Ov003BgResState* state) {
    const Ov003ResSlotCmd* cmd       = (const Ov003ResSlotCmd*)state->cursor;
    u32                    slotWord  = cmd->slot;
    u16                    palWord   = cmd->palette;
    u16                    bgIndex   = cmd->bg;
    u16                    slotIndex = cmd->slot;
    u16                    charTile  = (slotWord & 0xFFFF0000) >> 16;
    const void*            anims     = cmd->anims;
    u8                     palStart;
    u8                     palCount = palWord;
    s32                    charDest;
    Ov003SpriteSlot*       slot;

    palStart = (cmd->palette & 0xFFFF0000) >> 16;
    if (state->engine == 0) {
        if (bgIndex == 1) {
            charDest = G2_GetBG1CharPtr() + (charTile << 5);
        } else {
            charDest = G2_GetBG2CharPtr() + (charTile << 5);
        }
    } else if (bgIndex == 1) {
        charDest = G2S_GetBG1CharPtr() + (charTile << 6);
    } else {
        charDest = func_02036974() + (charTile << 6);
    }
    slot = &state->slots[slotIndex];
    CombatAnim_SlotInit(slot, state->iden, anims, (void*)charDest, 1, 0);
    if (palCount != 0) {
        CombatAnim_SlotSetPalette(slot, 0, state->iden, state->engine, palStart, palCount);
    }
    state->cursor = (const Ov003ResCmd*)(&((const Ov003ResSlotCmd*)state->cursor)[1]);
    return 1;
}
