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

Status: 102 of 561 functions decompiled, 53 of them at 100%.
`OtuMenuText` is 100% matched (860 bytes of `.data`, 268 of `.rodata`, 8 of
`.bss`); `OtuVBlank` is 100% across all five functions; `OtuSceneEntry` holds the
two plain entry points, the teardown, its thunk and both stage dispatches at
100%. `OtuScoreRow` is now the overlay's largest unit and holds eleven row
builders plus the countdown steppers that drive them. Partial:
`func_ov039_02083bb0` at 99.97%, `OtuGxInit` at 95.9%, both per-frame updates at
91.0% and 95.4%, the clamp helper at 94.5%, the place stepper at 92.3%, the
pin-count row at 83.8%, the countdown at 76.0%, the wireless teardown at 72.8%.
The remaining 459 functions are stubs.

The four countdown steppers (`02084458`, `020845dc`, `020846c4` and the
place-stepper they all call) share one unresolved gap, below. Two sit at ~89%
and one at 76%; decomp-permuter was run over the first two and could not push
either past 90%, so they are left as they are rather than contorted further.

### Row layouts, and why the tables keep the build's names

The result screens have a dozen or so numeric rows, and the `d.ddd` family has
one column-blanking table per row width: 4, 8, 12 and 16 entries for one
through four groups. There are three separate 4-entry tables and two separate
12-entry ones with identical contents, so a row's width does not determine its
table -- the tables are per-row, not per-width. With the nine-entry pin-count
table and the eighteen-entry three-digit one there are nine in all.

All are word-indexed (`ldr [base, index, lsl #2]`); declaring one as `u8` emits
`ldrb` and silently costs a match percentage. That is how the first of them was
wrong for a while, at `u8[7]` when it should have been `s32[9]`.

They are declared with the build's own `data_ov039_*` names rather than invented
ones, and that is deliberate. Declaring an array and letting dsd place it does
not work here: the claimed `.rodata` range is largely gap-filled from the
original overlay, so a declared array lands wherever the linker puts it rather
than where the target has it. The bytes came out right anyway -- it was the
constant-pool word that was wrong, so the defect was invisible until a function
was diffed. Naming them as the build does puts the pool word back, which is
what takes the three-digit row from 99.95% to a full match. The same applies to
three `.data` anchor constants that sit in a gap-filled range.

`tools/ov039_table.py <address>` recovers any table's contents;
`tools/ov039_anchor.py` reports which declared constant sits at which address,
and marks same-valued pairs AMBIGUOUS rather than guessing.

### Three stepper shapes

The countdown steppers come in three shapes, and they are not
interchangeable:

  * a bare `ldreq` with no `else` arm (`02084944`, `02084d20`), which leaves the
    column uninitialised when the quotient is non-zero. Defaulting it to a fixed
    address instead adds a `movne` the target does not have -- 84.3% against
    72.5%;
  * a three-way `if`/`else if` chain (`020846c4`, `020847ec`, `02084a3c`) with
    no jump table;
  * a four-way `switch` with a jump table (`02084458`, `02084b94`).

For the three-way shape the target branches to a block for `== 0` and to a
second for `== 1`, with `== 2` as the conditional load that falls through.
mwcc's ordering of the arms does not reproduce that exactly; putting the
fall-through case last gets closest (74.9%) without reaching it.

### Two row-body shapes

The `d.ddd` row builders are near-identical but differ by one instruction, and
the two shapes are not interchangeable:

  * `020849a4` and `02084d80` write the trailing space *before* the blanking
    check and the NUL *after* it, two separate stores with the blanking between.
    Written adjacently, mwcc deletes the first as dead.
  * `02084738`, `02084874`, `02084ac4` and `02084c34` write no trailing space at
    all, relying on `func_ov039_02083a20`'s own and writing only the NUL.

Adding the redundant store where the target omits it costs a register and about
seven points of match.


### Two divide idioms

Signed `x / 4` compiles to one of two three-instruction sequences, and mwcc picks
between them itself:

    lsr  r3, r2, #0x1f            rsb  r2, r3, r2, lsl #0x1e
    rsb  r2, r3, r2, lsl #0x1e     add  r2, r3, r2, ror #0x1e
    add  r2, r3, r2, ror #0x1e

Both truncate toward zero and both are three instructions. The target uses the
rotate form for a value passed to a call and the shift form for a switch selector
— `func_ov039_02084458` emits both from the same register — while this build
emits the shift form for either. Casting the operand on either side of the
division, including to the callee's own parameter type, changes which one mwcc
picks but never reaches the target's.

decomp-permuter was run over `02084458` and `020845dc` for this. Neither its
"wins" are real: on `02084458` the best candidate (580 → 530) replaced
`table->flags = 0` with `table->flags = value; table->flags = table->flags * 0`,
a dead store the byte-level scorer rewards and mwcc folds away; on `020845dc` the
best candidate (615 → 425) cached the row pointer and divided unsigned, which
reaches 89.35% only by replacing the target's three-instruction divide with a
single `lsr #2`. Its byte score and objdiff's match percentage disagree often
enough here that candidates have to be checked by hand before being applied.

### Verifying the ROM actually rebuilt

**`ninja` reporting `twewy_usa.nds: OK` does not mean the ROM was rebuilt.**

The `delink` edge in `build.ninja` lists only config files and `dsd.exe` as
inputs — the compiled `build/usa/src/**/*.o` objects are *not* among them. So
after editing a source file, ninja rebuilds the object, considers the delinked
objects up to date, skips the delink, and goes straight to the sha1 step. The
sha1 then passes, because it is hashing the ROM from the previous link.

This was found the hard way: writing a sentinel value into a table inside the
claimed `.rodata` range left the ROM byte-identical, and the sentinel appeared
nowhere in `build/usa/build/arm9_ov039.bin`. The object had been rebuilt; it had
simply never been relinked.

`tools/verify_rom.py [module]` deletes the module's delinked objects, runs
ninja, checks the sha1, and then asserts the ROM is newer than the objects it
was built from — that last check is what distinguishes "rebuilt and correct"
from "never rebuilt". Use it instead of bare `ninja`.

A related quirk: a claimed `.rodata` range is not fully sourced from the claiming
TU. dsd places the TU's objects that match target symbols and fills the rest of
the range from the original overlay, so `OtuMenuText.c`'s two stage-dispatch
tables land at 0x02098e30/0x02098e3c while everything after them in that range
comes from the extracted overlay. Object *values* there are therefore not
load-bearing — but the C *type* still is, because it decides whether a use emits
`ldr` or `ldrb`.

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

### Check emitted sizes, not just match percentages

**`python tools/permsize.py` is the first thing to run on this overlay.** It
compares each function's real symbol size in the delinked target object against
the current build. `report.json` is not a substitute: its per-function `size` is
the *target's* size, and objdiff's fuzzy match tolerates a length difference — so
a function missing six instructions still scored 87%, and the mismatch was
invisible until the sizes were read out of the objects directly.

A non-zero delta is the most useful thing to know before reading a diff: every
instruction after the divergence point is shifted, and the diff view shows far
more damage than actually exists. Nine functions still differ; the largest is
`func_ov039_020827d0`, 48 bytes short.

### Text helpers

`OtuText.c` covers 0x02083974-0x02084020 — seven functions, three at 100%. They
are the result screens' number formatting: digit extraction
(`func_ov039_02083974`), `d.ddd` fixed-point output (`func_ov039_02083a20`), a
range clamp (`func_ov039_020839d4`), the decimal-place stepper
(`func_ov039_02083af4`), the scrollbar (`func_ov039_02083bb0`), the countdown
animation (`func_ov039_02083cbc`) and the pin-count row (`func_ov039_02083ed4`).

Four things worth knowing before writing the rest of this block:

- **This overlay's source does not cache computed addresses.** The target
  re-derives the row pointer (`ldr [r4,#0]` then `add r1, r4, r0, lsl #4`) at
  every field access, and re-derives `scene + 0x44000 + menuIndex * 0x34` before
  every countdown read. Writing a local to hold the pointer is the natural thing
  to do and costs the multiply-and-add that mwcc then never emits again — that
  alone was the whole gap on `func_ov039_02083bb0` (87% → 99.97%) and most of it
  on `func_ov039_02083ed4` and `func_ov039_02083cbc`. Suspect a cached local
  first when a function here is short.
- Digit extraction is plain `% 10` and `/ 10` in source. The `smull`/shift
  sequences in the target are just mwcc's expansion, with `0x66666667` in the
  literal pool being its divide-by-ten reciprocal. No asm needed.
- Where a small integer selects one of several fixed values, the target uses a
  real `switch` with a jump table — `func_ov039_02083af4` has one at 0x02083b38
  and `func_ov039_02083cbc` a nine-way one at 0x02083cf4, both behind an
  `addls pc, pc, rX, lsl #2` bounds check. An `x == 0 || x == 1 || ...` chain
  folds to a range check instead, the same trap as the scene's 0x41AC4 case.
- **Stack-buffer layouts must be read out of the disassembly, not derived from
  the field widths.** `func_ov039_02083ed4`'s six digit groups are 2, 1, 3, 1, 1,
  1 chars wide, which tiles 0x0..0xC — but the target places them at 0x0, 0x3,
  0x5, 0x9, 0xB, 0xD with the terminator at 0xE. The `add r0, sp, #N` sequence
  is the only reliable source.
- The row table is walked at a **16-byte stride** (`add r0, r4, r0, lsl #4`) by
  a struct that is 0x38 wide, and reaches fields past the stride. Array
  subscripting makes mwcc derive the stride from `sizeof`, so the indexing goes
  through `OTU_TEXT_ROW` instead.

`func_ov039_02083cbc` has an open question worth resolving before trusting it:
its later arms call the five-argument clamp, but only ever push the fourth
argument. The clamp reads its fifth — the upper bound — from `[sp, #0x10]`, where
these callers set nothing, so the bound this source passes is a guess.

### Permuting here

decomp-permuter is **not installed on this box** (its documented path is a Linux
checkout, and there is no ARM assembler for its target object). What exists
instead, all committed under `tools/`:

| | |
|---|---|
| `perm_score.py` | scores a function from both objects' bytes; ~100× faster than `fndiff.py`, which reloads the whole project |
| `perm_harness.py` | builds a standalone TU with one function kept, so struct layouts and file-local tables match the real build |
| `perm_run.py` | mutate / compile / score / hill-climb |
| `permsize.py` | the size comparison above — run this first |

Validated by scoring the known-100% functions at 0/0. Note the substitution
scores **bytes** and has no notion of semantics, so it is easy to game: every
apparent improvement it found was a duplicated call, which lines the word stream
up while making the code wrong. `perm_run.py` now rejects candidates that
increase the call count; with that guard it reports zero improvements honestly.
Treat a win from it as a lead to check, not a result to commit.

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
