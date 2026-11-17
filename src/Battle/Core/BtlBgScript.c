#include "Battle/Core/BtlBgScript.h"
#include "Combat/Core/CombatAnim.h"
#include "Combat/Core/CombatBgAnimSlot.h"
#include "Combat/Core/CombatBgTiles.h"
#include "Engine/Core/DMA.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/BgResMgr.h"
#include "Engine/Resources/PaletteMgr.h"

#include <nitro/g2.h>

void BtlBgScript_Init(BattleBgScriptState* state, DisplayEngine engine, BinIdentifier* iden) {
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

void BtlBgScript_Run(BattleBgScriptState* state, const BattleBgScriptWord* script, void* arg) {
    state->cursor.word = script;
    state->scriptArg   = arg;
    state->data        = NULL;
    while (state->cursor.cmd->run(state)) {
    }
}

void BtlBgScript_Update(BattleBgScriptState* state) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (state->slots[i].anims != NULL) {
            CombatAnim_SlotUpdate(&state->slots[i]);
        }
    }
}

void BtlBgScript_Release(BattleBgScriptState* state) {
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

s32 BtlBgScript_CmdEnd(BattleBgScriptState* state) {
    return 0;
}

s32 BtlBgScript_CmdLoadPalette(BattleBgScriptState* state) {
    const BattleBgScriptCmd* cmd       = state->cursor.cmd;
    u16                      packIndex = (cmd->operands0 & 0xFFFF0000) >> 16;
    u16                      srcIndex  = (u16)cmd->operands0;
    u16                      palStart  = (cmd->operands1 & 0xFFFF0000) >> 16;
    u16                      palCount  = (u16)cmd->operands1;
    void*                    source;

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
    state->cursor.cmd++;
    return 1;
}

s32 BtlBgScript_CmdLoadChar(BattleBgScriptState* state) {
    const BattleBgScriptCmd* cmd       = state->cursor.cmd;
    u16                      bgIndex   = (u16)cmd->operands0;
    u16                      packIndex = (cmd->operands0 & 0xFFFF0000) >> 16;
    u16                      srcIndex  = (u16)cmd->operands1;
    u16                      charTile  = (cmd->operands1 & 0xFFFF0000) >> 16;
    void*                    charDest;
    s32                      size;
    void*                    source;

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
    func_02001b44(7, (s32)charDest, ((const CombatBgTiles*)source)->pixels, size);
    state->cursor.cmd++;
    return 1;
}

s32 BtlBgScript_CmdLoadSlot(BattleBgScriptState* state) {
    const BattleBgScriptSlotCmd* cmd       = state->cursor.slotCmd;
    u32                          slotWord  = cmd->slot;
    u16                          palWord   = cmd->palette;
    u16                          bgIndex   = cmd->bg;
    u16                          slotIndex = cmd->slot;
    u16                          charTile  = (slotWord & 0xFFFF0000) >> 16;
    const Ov003BgAnimFrame*      anims     = cmd->anims;
    u8                           palStart;
    u8                           palCount = palWord;
    void*                        charDest;

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

    Ov003BgAnimSlot* slot = &state->slots[slotIndex];
    CombatAnim_SlotInit(slot, state->iden, anims, charDest, 1, 0);
    if (palCount != 0) {
        CombatAnim_SlotSetPalette(slot, 0, state->iden, state->engine, palStart, palCount);
    }
    state->cursor.slotCmd++;
    return 1;
}
