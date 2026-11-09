#ifndef TWEWY_BATTLE_CORE_BTLBGSCRIPT_H
#define TWEWY_BATTLE_CORE_BTLBGSCRIPT_H

#include "Combat/Core/CombatBgAnimSlot.h"
#include "Engine/File/BinMgr.h"
#include "Engine/File/DatMgr.h"
#include "Engine/Resources/BgResMgr.h"
#include "Engine/Resources/PaletteMgr.h"

struct BattleBgScriptState;

/**
 * A background script command. Reads its operands through `state->cursor`, moves `cursor` past its
 * own words, and returns nonzero to let BtlBgScript_Run run the next command (0 ends the script).
 */
typedef s32 (*BattleBgScriptCmdFunc)(struct BattleBgScriptState* state);

/**
 * One word of a background script, which is how scripts are defined and passed around: a flat array
 * of these mixing command functions, animation tables and packed operands, ending with
 * BtlBgScript_CmdEnd. A command is one or more consecutive words, starting with its `run` word.
 *
 * This is the `cursor.word` view. Commands never read words one at a time; they view the words at
 * the cursor through the command shape their own `run` expects (BattleBgScriptCmd or
 * BattleBgScriptSlotCmd).
 */
typedef union BattleBgScriptWord {
    u32                     value;
    BattleBgScriptCmdFunc   run;
    const Ov003BgAnimFrame* anims;
} BattleBgScriptWord; // Size: 0x4

/**
 * The words at `cursor` when `run` is BtlBgScript_CmdLoadPalette or BtlBgScript_CmdLoadChar: the
 * `cursor.cmd` view, which those commands advance with `cursor.cmd++`.
 *
 * BtlBgScript_CmdEnd is also dispatched through this view, but only its `run` word exists; it never
 * reads its operands or advances.
 */
typedef struct BattleBgScriptCmd {
    /* 0x00 */ BattleBgScriptCmdFunc run;
    /* 0x04 */ u32                   operands0; // packed halfwords, see the command's own doc
    /* 0x08 */ u32                   operands1; // packed halfwords, see the command's own doc
} BattleBgScriptCmd;                            // Size: 0xC

/**
 * The words at `cursor` when `run` is BtlBgScript_CmdLoadSlot: the `cursor.slotCmd` view, which that
 * command advances with `cursor.slotCmd++`. Longer than BattleBgScriptCmd because it carries the
 * slot's animation table as well as its packed operands.
 */
typedef struct BattleBgScriptSlotCmd {
    /* 0x00 */ BattleBgScriptCmdFunc   run;
    /* 0x04 */ const Ov003BgAnimFrame* anims;
    /* 0x08 */ u32                     bg;      // 1 = BG1, otherwise BG2; the high halfword is unused
    /* 0x0C */ u32                     slot;    // slot index | first char tile << 16
    /* 0x10 */ u32                     palette; // palette count | first palette << 16, both read as u8
} BattleBgScriptSlotCmd;                        // Size: 0x14

/// The interpreter state for the background scripts of one screen engine, and what those scripts
/// left loaded: the palettes, the last pack entry they read, and up to two animated tile slots.
typedef struct BattleBgScriptState {
    /* 0x00 */ s32            engine; // 0 = main engine, 1 = sub engine
    /* 0x04 */ BinIdentifier* iden;   // pack the graphics come from
    /* 0x08 */ union {
        const BattleBgScriptWord*    word;
        const BattleBgScriptCmd*     cmd;
        const BattleBgScriptSlotCmd* slotCmd;
    } cursor;                                 // next command of the running script: `word` is how the script was passed in,
                                              // `cmd`/`slotCmd` are how a command reads and steps past its own words
    /* 0x0C */ void*            scriptArg;    // the `arg` of BtlBgScript_Run; no command reads it
    /* 0x10 */ Data*            data;         // pack entry the last palette/char command loaded
    /* 0x14 */ BgResource*      charRes[6];   // released by BtlBgScript_Release, but no command fills them
    /* 0x2C */ PaletteResource* palettes[3];  // filled in load order, see `paletteCount`
    /* 0x38 */ Ov003BgAnimSlot  slots[2];     // animated BG tiles, unused while `anims` is NULL
    /* 0x88 */ s16              unk_88;
    /* 0x8A */ s16              paletteCount; // palettes loaded so far, at most 3
} BattleBgScriptState;                        // Size: 0x8C

/**
 * @brief Sets up an empty script state for one screen engine
 *
 * @param state The script state to set up
 * @param engine The screen engine its scripts load into: 0 = main, 1 = sub
 * @param iden The pack the scripts load their graphics from
 */
void BtlBgScript_Init(BattleBgScriptState* state, s32 engine, BinIdentifier* iden);

/**
 * @brief Runs a background script to its end
 *
 * Calls each command in turn until one returns 0 (BtlBgScript_CmdEnd). Forgets the pack entry a
 * previous script left in `data` without releasing it.
 *
 * @param state The script state the commands load into
 * @param script The script's words, starting with its first command
 * @param arg Kept in `scriptArg`; no command reads it
 */
void BtlBgScript_Run(BattleBgScriptState* state, const BattleBgScriptWord* script, void* arg);

/**
 * @brief Advances the animated tile slots the scripts set up
 *
 * @param state The script state to animate
 */
void BtlBgScript_Update(BattleBgScriptState* state);

/**
 * @brief Frees everything the scripts left loaded
 *
 * Releases the last loaded pack entry, the palettes and the animated tile slots (and `charRes`,
 * which no command fills).
 *
 * @param state The script state to release
 */
void BtlBgScript_Release(BattleBgScriptState* state);

/**
 * @brief Script command: ends the script
 *
 * Takes no operands.
 *
 * @param state The script state running the script (unused)
 * @return 0, which stops BtlBgScript_Run
 */
s32 BtlBgScript_CmdEnd(BattleBgScriptState* state);

/**
 * @brief Script command: loads a palette into the next palette slot
 *
 * Operands: `operands0` = pack index << 16 | entry of the palette data; `operands1` = first
 * palette << 16 | palette count. The pack entry is always loaded (and kept in `data`), but no
 * palette is allocated once three are loaded.
 *
 * @param state The script state running the script
 * @return 1, so the script continues
 */
s32 BtlBgScript_CmdLoadPalette(BattleBgScriptState* state);

/**
 * @brief Script command: copies a block of BG tiles straight into BG1 or BG2 char VRAM
 *
 * Operands: `operands0` = pack index << 16 | BG (1 = BG1, otherwise BG2); `operands1` = first
 * tile << 16 | entry of the tile data. The tiles are written by DMA at that tile offset; no
 * BgResource is allocated for them. The pack entry stays loaded in `data`.
 *
 * @param state The script state running the script
 * @return 1, so the script continues
 */
s32 BtlBgScript_CmdLoadChar(BattleBgScriptState* state);

/**
 * @brief Script command: sets up one of the two animated tile slots
 *
 * A longer command of five words: the run function, the slot's animation table, the BG (1 = BG1,
 * otherwise BG2), first tile << 16 | slot index, and first palette << 16 | palette count (0 = the
 * slot keeps no palette of its own). The slot loops its animation; nothing is uploaded until the
 * first BtlBgScript_Update.
 *
 * @param state The script state running the script
 * @return 1, so the script continues
 */
s32 BtlBgScript_CmdLoadSlot(BattleBgScriptState* state);

#endif // TWEWY_BATTLE_CORE_BTLBGSCRIPT_H
