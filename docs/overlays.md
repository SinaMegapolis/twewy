# Overlays

TWEWY contains 48 overlays, each being loaded on demand to provide specified functionality at runtime, as detailed below.

- [00 - Mini108](#overlay-00---mini108)
- [01 - Font](#overlay-01---font)
- [02 - OtosuMenu](#overlay-02---otosu-menu)
- [03](#overlay-03)
- [04 - Shiki](#overlay-04---shiki)
- [05](#overlay-05)
- [06](#overlay-06)
- [07 - Fusion / Last Attacks](#overlay-07---fusion--last-attacks)
- [08 - Tutorial Battles](#overlay-08---tutorial-battles)
- [09 - Noise](#overlay-09---noise)
- [10](#overlay-10)
- [11](#overlay-11)
- [12](#overlay-12)
- [13](#overlay-13)
- [14](#overlay-14)
- [15](#overlay-15)
- [16 - Boss01](#overlay-16---boss01)
- [17 - Boss02 and Boss03](#overlay-17---boss02-and-boss03)
- [18 - Boss00](#overlay-18---boss00)
- [19](#overlay-19)
- [20](#overlay-20)
- [21](#overlay-21)
- [22](#overlay-22)
- [23](#overlay-23)
- [24 - Boss15 and Boss16](#overlay-24---boss15-and-boss16)
- [25 - Continue](#overlay-25---continue)
- [26 - Tutorial](#overlay-26---tutorial)
- [27](#overlay-27)
- [28 - Noise Report](#overlay-28---noise-report)
- [29 - Sound Tests](#overlay-29---sound-tests)
- [30](#overlay-30)
- [31 - Font / PRC](#overlay-31---font--prc)
- [32](#overlay-32)
- [33](#overlay-33)
- [34](#overlay-34)
- [35](#overlay-35)
- [36](#overlay-36)
- [37 - OpenEnd](#overlay-37---openend)
- [38 - Graphics Checks](#overlay-38---graphics-checks)
- [39 - Tin Pin Slammer](#overlay-39---tin-pin-slammer)
- [40 - Wireless Communications](#overlay-40---wireless-communications)
- [41 - StreetPass Test](#overlay-41---streetpass-test)
- [42 - StaffRoll](#overlay-42---staffroll)
- [43 - Menus](#overlay-43---menus)
- [44 - Results Screen](#overlay-44---results-screen)
- [45 - Mingle Mode and Friend List UI](#overlay-45---mingle-mode-and-friend-list-ui)
- [46 - Debug Menu Launcher](#overlay-46---debug-menu-launcher)
- [47 - Empty Overlay](#overlay-47---unknown-empty-overlay)

## Overlay 00 - Mini108

**Files:**
[Mini108](../src/Debug/Abe/Mini108.c)

TODO: Document

## Overlay 01 - Font

**Files:**
[Font](../src/Debug/Fukuda/Font.c)

TODO: Document

## Overlay 02 - Otosu Menu

**Files:**
[OtosuMenu](../src/Debug/Fukuda/OtosuMenu.c)

TODO: Document

## Overlay 03

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: main battle system logic

## Overlay 04 - Shiki

**Files:**
[Shiki](../src/Combat/Friend/Shiki/)

TODO: Document

## Overlay 05

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Joshua

## Overlay 06

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Beat

## Overlay 07 - Fusion / Last Attacks

**Category:** Combat

**Files:**
[BtlFusion](../src/Combat/Fusion/BtlFusion.c)

Provides the `Tsk_BtlPlayerLast` and `Tsk_BtlAuraLast` combat tasks. `Tsk_BtlPlayerLast` drives the
player's finishing attack (screen-space reprojection plus an actor update callback); `Tsk_BtlAuraLast`
manages a pool of 16 aura sprites used for the attack's visual effects, interpolating and spawning them
from active/free lists.

Status: 22 of 25 functions match byte-for-byte. `func_ov007_020e76dc`, `func_ov007_020e7a8c`, and
`func_ov007_020e7da4` are Nonmatching due to MWCC register-allocation differences. The TU cannot be
marked `complete` until those are resolved.

## Overlay 08 - Tutorial Battles

**Category:** Combat

**Files:**
[BtlTutorial](../src/Combat/Tutorial/BtlTutorial.c)

Provides the `Tsk_BtlPlayerNone` and `Tsk_BtlTutorial` combat tasks. `Tsk_BtlTutorial` loads the tutorial
graphics pack (`Apl_Fur/Grp_Tutorial.bin`), configures sub-engine BG1/BG2 for the tutorial overlay, and
draws a combat sprite; `Tsk_BtlPlayerNone` is the stub partner task used while the tutorial runs. The
active variant is selected from the combat context.

TODO: Document the exact tutorial variants (`data_ov008_020e7a38`).

## Overlay 09 - Noise

**Files:**
[BtlEnm003](../src/Combat/Noise/BtlEnm003),
[BtlEnm012](../src/Combat/Noise/BtlEnm012),
[BtlEnm019](../src/Combat/Noise/BtlEnm019),
[BtlEnm028](../src/Combat/Noise/BtlEnm028),
[BtlEnm044](../src/Combat/Noise/BtlEnm044),

TODO: Decompile and document

## Overlay 10

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 11

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 12

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 13

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 14

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 15

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Noise

## Overlay 16 - Boss01

**Files:**
[Boss01](../src/Combat/Noise/Boss01.c)

Contains logic for "Boss01". TODO: Determine which boss this is and document its functionality.

## Overlay 17 - Boss02 and Boss03

**Files:**
[Boss02](../src/Combat/Noise/Boss02.c),
[Boss03](../src/Combat/Noise/Boss03.c)

TODO: Document

## Overlay 18 - Boss00

**Files:**
[Boss00](../src/Combat/Noise/Boss00/)

TODO: Decompile and document

## Overlay 19

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Boss fight

## Overlay 20

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Boss fight

## Overlay 21

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Boss fight

## Overlay 22

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Boss fight

## Overlay 23

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: Boss fight

## Overlay 24 - Boss15 and Boss16

**Files:**
[Boss15](../src/Combat/Noise/Boss15),
[Boss16](../src/Combat/Noise/Boss16)

TODO: Decompile and document

## Overlay 25 - Continue

**Files:**
[Continue](../src/Debug/Furukawa/Continue.c)

TODO: Document

## Overlay 26 - Tutorial

**Files:**
[Tutorial](../src/Debug/Furukawa/Tutorial.c)

TODO: Document

## Overlay 27

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: battle selection menu for chain battles and encounter selection

## Overlay 28 - Noise Report

**Files:**
[NoiseReport](../src/Debug/Horii/NoiseReport.c)

The Noise Report screen, which displays the Noise enemies and various statistics about them including their item drops and drop chances at various difficulties.

## Overlay 29 - Sound Tests

**Files:**
[SoundTest](../src/Debug/Kitawaki/SoundTest.c)

Accessible through the hidden debugging menu. Contains a display from which all of the game's sound effects and music tracks can be played. Options are provided to adjust various parameters of the audio playback.

## Overlay 30

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

Speculated purpose: field exploration, various debugging menus, brand and region information, etc. Extremely large and varied in functionality.

## Overlay 31 - Font / PRC

**Files:**
[PrcMaster](../src/PrcMaster.c)

TODO: Document

## Overlay 32

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

## Overlay 33

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

## Overlay 34

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

## Overlay 35

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

## Overlay 36 - MsgXls

**Files:**
[MsgXls](../src/Util/MsgXls.c)

TODO: Document

## Overlay 37 - OpenEnd

**Files:**
[OpenEnd](../src/Debug/Bul/OpenEnd.c)

TODO: Document

## Overlay 38 - Graphics Checks

**Files:**
[GrpCheck](../src/Debug/Mori/GrpCheck.c)

Contains a graphics testing module for viewing art assets and their relevant information such as category, filename, color grading, scaling, etc. Accessible through the hidden debugging menu. Option M1 configures the module to display static images such as backgrounds. Option M2 configures the module to display animated sprites.

## Overlay 39 - Tin Pin Slammer

**Category:** Debug Menu

**Files:**
[TinPinSlammer.h](../include/Debug/Sugata/TinPinSlammer.h),
[OtuScene](../src/Debug/Sugata/TinPinSlammer/OtuScene.c),
[OtuSceneEntry](../src/Debug/Sugata/TinPinSlammer/OtuSceneEntry.c),
[OtuMenuText](../src/Debug/Sugata/TinPinSlammer/OtuMenuText.c)

Jun Sugata's Otosu ("pin throw") minigame, reached from the debug menu's S key.
It is the largest overlay in the ROM: **561 functions, 92.5 KB of `.text`**.

The whole overlay runs off one large scene object that `MainOvlDisp` allocates
out of `gDebugHeap` and passes to every entry point in `r0`. Its sub-objects sit
at very large fixed offsets (`0x11000`, `0x41000`, `0x416A8`, `0x44000`, ...),
so `TinPinSlammer.h` models the block as one struct with padding between
members — every field past `0xFFF` is too far for a `ldr` displacement, which is
why the target re-derives the containing pointer on each access rather than
hoisting it.

Status: 26 of 561 functions decompiled, 18 of them at 100%. `OtuMenuText` is
100% matched (860 bytes of `.data`, 24 of `.rodata`, 8 of `.bss`); `OtuVBlank`
is 100% across all five functions; `OtuSceneEntry` holds the two plain entry
points, the teardown, its thunk and both stage dispatches at 100%. Partial:
`OtuGxInit` at 95.9%, both per-frame updates at 90.9% and 95.4%, the clamp
helper at 94.5%, the place stepper at 91.5%, the wireless teardown at 72.8%. The
remaining 535 functions are stubs.

### Data ownership

The overlay's `.data` (0x0209a140-0x0209a49c) and `.bss` (0x0209ad00) are
reachable only as a single range each, and the split that *looks* natural — one
group per consuming code block — produces a dsd link-order cycle, because dsd
sorts TUs by `.text` and the groups' `.data` ranges ascend in the opposite
order. The resolution is a data-only TU (`OtuMenuText.c`) that claims the whole
range, with the code TUs claiming `.text` only.

`OtuMenuText.c` is generated, not hand-written: `tools/ov039_strgen.py` emits
the Shift-JIS menu labels from `build/usa/asm/ov039_4.s`, and
`tools/ov039_datagen.py` prepends the stage descriptors, companion lists,
coordinate tables and anchor constants. Declaration order is load-bearing —
mwcc sorts `.data` objects by size ascending, ties broken by creation order.

### mwcc emission notes

- `ResourceManager` is 0x11580 bytes and sits at `scene+0`, so `prevResMgr`
  lands at 0x11580. Reaching it as `scene + 0x11000 + 0x580` is two steps, not
  a distinct sub-object.
- The heap and its buffer are reached through `scene->base.heap` /
  `scene->base.heapBuffer`. Spelling them as offset macros instead lets mwcc
  CSE the address into a callee-saved register, where the target recomputes it
  at each use — that alone was worth ~7% on `func_ov039_02082978`.
- The overlay's slot state is one 8-byte object cleared with `str` at +4 and
  `strb` at +0, not two 4-byte words.
- The two per-frame updates (`0x02082e98` plain, `0x02082ff8` wireless) are
  near-twins with identical OAM/palette prologues and identical sizes (352 and
  364). They differ only in the task work — the wireless one skips it while
  `OtuScene_SlotState.count` is set — and in where the exit overlay is chosen
  from: the save record for the plain one, the scene's own latched status for
  the wireless one.
- Both updates pass the overlay id as a literal `2`, but the target loads it
  from a literal-pool word where mwcc folds it to `mov r1, #2`. That one word
  shifts every later pool displacement and is the entire remaining diff in
  `func_ov039_02082ff8`. Seven spellings of the id were tried, plus a `volatile`
  local; all score equal or worse. Note the other update has the same issue at
  `+0x41C` — a `switch` or range test there is the likely original shape.
- A thunk that must be a pure tail call needs a *declared parameter*, not an
  explicit argument. `func_ov039_020831cc(scene)` emits a bare `ldr ip, [pc]` +
  `bx ip`; passing `0` instead costs a `mov r0, #0` and drops 100% to 66%.
- `tools/sizecheck.py <unit>` lists each function's size against the target's.
  A function whose instructions are all correct can still report a wall of
  pool-displacement mismatches because an earlier function in the same TU
  differs in size, so check the deltas before concluding a gap is register
  allocation. `tools/moveblock.py` keeps a TU in address order after a batch of
  edits.
- `func_ov039_02082c50` latches `SystemStatusFlags.unk_06` and `unk_07` into the
  scene (as `state.linkStatus` / `state.wirelessStatus`) and clears them, so
  they are wireless-*relevant* — but that is all that is known, and **neither
  bit is a wireless flag**:
  - `unk_07` is set by `Savefile_InitNewGameDefaults` beside `unk_05`, and
    `main.c` runs its main loop `while (reset == FALSE || unk_07 == FALSE)`. It
    is a save/boot "new game initialised" flag.
  - `unk_06` is a tri-state power/hinge mode. `main.c:173` branches three ways on
    it: 1 gates `Input_IsSystemHingeClosed()` lid handling, any other non-zero
    value polls two peripheral slots. `Tusin.c` sets it when the wireless stack
    is *busy*, so reading it as "link established" inverts its meaning.

  Genuine names need the JP overlay (not buildable here — see below) or the
  hardware docs. They are deliberately left as `unk_NN`; a confident-sounding
  guess here would be worse than the placeholder, since both bits are read from
  power-management and save code far from this overlay. The scene-state field
  names (`linkStatus`/`wirelessStatus`) describe the *overlay's* use of them and
  make no claim about the flags themselves.
- The wireless save record is based at `gSaveData + 0x3000`, not `gSaveState`.
- `func_ov039_02082c50` (the menus/results entry point) reached only 66% and
  resisted 14 source variants. Every instruction is semantically right; the whole
  gap is mwcc placing literal-pool loads one or two instructions later than the
  target does. It is the natural next thing to attack with the permuter.

### Scene lifecycle

The scene's whole lifecycle is four functions plus two dispatch tables, all now
in source:

| Role | Function | |
|---|---|---|
| entry (plain) | `func_ov039_02082978` | 100% |
| entry (wireless) | `func_ov039_02082ae0` | 100% |
| entry (menus/results) | `func_ov039_02082c50` | not written; 66% when tried |
| update (plain) | `func_ov039_02082e98` | 90.9% |
| update (wireless) | `func_ov039_02082ff8` | 95.4% |
| teardown | `func_ov039_02083164` | 100% |
| teardown thunk | `func_ov039_020831cc` | 100% |
| teardown (wireless) | `func_ov039_020831d8` | 72.8% |
| stage dispatch | `func_ov039_02083240` | 100% |

`OtuScene_PlainHandlers` and `OtuScene_WirelessHandlers` (0x02098e30, 24 bytes of
`.rodata`) map `MainOvlDisp_GetProcessStage`'s result to a handler, one per scene
variant. Their element type is an untyped `void (*)()`: the dispatcher passes the
scene in r0 but the two entry-point slots ignore it, and mwcc rejects a
function-pointer initialiser for a differently-typed function.

### Display setup

`OtuGxInit.c` holds `func_ov039_020832c0` (95.9%), the 1168-byte straight-line
GX/display bring-up; `OtuVBlank.c` holds both per-variant VBlank handlers and
the three setup wrappers, all at 100%. Between them these close out the
0x02082e98-0x02083974 block.

**Check `Display.h` before hand-writing display setup.** It already provides
`Display_InitMainBG0..3` and `Display_InitSubBG0..3`, each of which writes a BG
layer's six settings fields and then pokes that layer's BG control register with
a `regMask` argument — exactly the field-then-poke pattern the target uses.
Writing the eight layers out by hand scored 77.5% on `func_ov039_020832c0`;
switching to the helpers took it to 95.9%. `Display_GetBGnSettings`,
`Display_SetMainLayers` and `Display_SetSubLayers` cover the rest. The helpers
are not discoverable from the overlay's own function names, so the instinct to
write fields directly is easy to follow and expensive.

Two naming traps in that function: the sub-engine BG control registers are
`REG_BGxCNT_SUB` (not `_S`), and the third OAM engine is `DISPLAY_EXTENDED`
(not `DISPLAY_SUBOBJ`), initialised through `OamMgr_InitEngine`.

### Text helpers

`OtuText.c` opens the 0x02083974 text-helpers block with the result screens'
number formatters: decimal digit extraction (`func_ov039_02083974`, 100%),
`d.ddd` fixed-point formatting (`func_ov039_02083a20`, 100%), a range clamp
(`func_ov039_020839d4`, 94.5%) and the decimal-place stepper
(`func_ov039_02083af4`, 91.5%).

Two things worth knowing before writing the rest of this block:

- Digit extraction is plain `% 10` and `/ 10` in source. The `smull`/shift
  sequences in the target are just mwcc's expansion, with `0x66666667` in the
  literal pool being its divide-by-ten reciprocal. No asm needed.
- Where a small integer selects one of several fixed values, the target uses a
  real `switch` with a jump table — `func_ov039_02083af4` has one at 0x02083b38
  behind a `cmp r2, #3 ; addls pc, pc, r2, lsl #2` bounds check. An
  `x == 0 || x == 1 || ...` chain folds to a range check instead, which is the
  same trap as the scene's 0x41AC4 case.

Useful notes for continuing:

- Drafts come from m2c, which reads the repo's own `build/usa/asm/ov039_4.s`
  directly — no hand transcription. Use `python tools/m2cdraft.py --range
  0x02082724:0x020827d0`, which wraps `tools/m2ctx.py` + `m2c.py` and caches the
  preprocessed context.
- The pin tray is indexed as a flat `u16` array off the scene pointer at slot
  `TIN_PIN_SLAMMER_TRAY_SLOT` (0x22026), not through a sub-object pointer;
  writing it as `((OtuSceneTables*)(scene + 0x44000))->tray[i]` strength-reduces
  the base into a register and costs ~33% of the match.
- `python tools/ov039_data_owners.py <lo> <hi>` maps a `.data` range to the
  functions referencing it. That is what decides whether a range can be claimed
  by the TU holding its consumers' `.text`.
- `python tools/ov039_plan.py` prints all 30 task TUs with ranges and function
  counts; `tools/sizeprobe.py` pins real struct sizes (mwcc rejects `sizeof` in
  constant expressions).
- The equipped-pin array is walked with a bare byte cursor (stride `0xA`, id at
  `+0x74`), not through `EquippedPin`.
- `.text` layout is contiguous, so a TU's `delinks.txt` range must be filled
  completely; use `python .opencode/skills/objdiff-decomp/scripts/ov039_survey.py
  [--calls]` to see function boundaries and intra-overlay call targets.
- JP is **not** the same module: `config/jp/arm9/overlays/ov039` has 121
  functions against the USA overlay's 561, so USA-only code has no JP counterpart
  and must not be treated as a regression.

## Overlay 40 - Wireless Communications

**Files:** N/A (Not yet decompiled)

TODO: Decompile and document

## Overlay 41 - StreetPass Test

**Files:**
[STSample](../src/Debug/Sugata/STSample.c)

Only accessible from the hidden debugging menu. Appears to have been used for testing StreetPass functionality.

## Overlay 42 - StaffRoll

**Files:**
[StaffRoll](../src/Debug/Suyama/StaffRoll.c)

End credits.

## Overlay 43 - Menus

**Files:**
[TakTest](../src/Debug/Takami/TakTest.c),
[MenuTop](../src/Debug/Takami/MenuTop.c),
[MenuBadge](../src/Debug/Takami/MenuBadge.c),
[MenuScena](../src/Debug/Takami/MenuScena.c),
[MenuEquip](../src/Debug/Takami/MenuEquip.c),
[Shop](../src/Debug/Takami/Shop.c),
[Depart](../src/Debug/Takami/Depart.c),
[Save](../src/Debug/Takami/Save.c),
[NRep](../src/Debug/Takami/NRep.c)

Several menus, such as the main phone menu, pins, equipment, post-game scenario selection, shops, department stores, and the save screen.

MenuTop provides the primary phone screen. The top display provides a map of Shibuya indicating the dominant brands for each region and detailed breakdown of the top 3 and lowest brand of the region the player currently is within. The bottom screen provides buttons to open other menus, a slider for player level, toggles for combat difficulty and partner behavior, and displays for currency and drop rates.

MenuBadge provides a screen for viewing and equipping the player's collected pins.

MenuScena provides a screen for selecting chapters/days to revisit after completing the game.

MenuEquip provides a screen for equipping the player's collected accessories, consuming food, and viewing the player's stats.

Shop provides a screen for purchasing items from shops.

Depart provides a screen for selecting different shops to visit.

Save provides a screen for saving the game.

TakTest appears to be primarily a basic template to test UI rendering but crashes the game immediately upon opening.

NRep appeared to have been intended as a Noise Report screen but appeared to be abandoned, left only displaying the static background of the save screen overlaid with artwork for the first 24 pins.

## Overlay 44 - Results Screen

**Files:**
[Result](../src/Debug/Takami/Result.c)

The results screen displayed at the conclusion of battles. Displays the player's performance in the battle, such as score, time taken, and various other statistics.

## Overlay 45 - Mingle Mode and Friend List UI

**Files:**
[Tusin](../src/Interface/Menu/Tusin.c),
[TusinSet](../src/Interface/Menu/TusinSet.c),
[FriendList](../src/Debug/Takami/FriendList.c)

Elements related to Mingle Mode. Includes the UI for configuring Mingle Mode, the Friends menu, and option to launch the game into Mingle Mode.

## Overlay 46 - Debug Menu Launcher

**Files:** [Launcher](../src//Debug/Launcher.c)

The main screen for accessing the various debugging modules. Displays options organized under icons of their respective developers' initials, such as "H" for Horii and "M" for Mori. Each option then provides sub-options for the various modules, such as "M1" for GrpCheck and "H1" for NoiseReport. Descriptions of each option are provided at the bottom of the screen when highlighted.

## Overlay 47 - Unknown (Empty Overlay)

**Files:** N/A (Not yet decompiled)

This particular overlay contains no code or data except for a compiler-generated contructor reference. Any usage of relevance has not yet been discovered. It is possible this overlay was intended for testing purposes, as a template for other overlays, or was simply accidentally included, but these ideas are purely speculative.
