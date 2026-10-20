# Overlay 13 (`Tsk_BtlEnm015_*`) decompilation — progress handoff

**Date:** 2026-09-25
**Branch:** `decomp-ov013` (off `decomp-ov008`)
**Worktree:** `E:\Git\twewy-ov013`
**Source:** `src/Combat/Noise/BtlEnm015/BtlEnm015.c`
**Header:** `include/Combat/Noise/Private/BtlEnm015.h`
**Config:** `config/usa/arm9/overlays/ov013/{delinks,symbols}.txt`

## Overlay summary

Overlay 13 is the enemy-015 combat overlay. It provides six EasyTask handlers:

| Task | Handler | Data size |
|------|---------|-----------|
| `Tsk_BtlEnm015_Eff` | `func_ov013_021260e8` | 0x70 |
| `Tsk_BtlEnm015_EffStamp` | `func_ov013_02126700` | 0x0C |
| `Tsk_BtlEnm015_EffStampSub` | `func_ov013_02126a30` | 0x74 |
| `Tsk_BtlEnm015_RG` | `func_ov013_02126dd8` | 0x1E8 |
| `Tsk_BtlEnm015_Shake` | `func_ov013_02126ff4` | 0x10 |
| `Tsk_BtlEnm015_UG` | `func_ov013_02127370` | 0x1EC |

38 functions total, `.text` = `0x021256c0`–`0x02127444`.

The original disassembly is at `E:\Git\twewy\build\usa\asm\ov013_4.s` (57 KB).

## Workflow

From `E:\Git\twewy-ov013`:

```powershell
ninja build/usa/src/Combat/Noise/BtlEnm015/BtlEnm015.o
ninja objdiff.json build/usa/delinks/src/Combat/Noise/BtlEnm015/BtlEnm015.o
.\objdiff-cli.exe diff -p . -u "src/Combat/Noise/BtlEnm015/BtlEnm015" -o build/ov_diff.json --format json
```

Parse `build/ov_diff.json`: `left.symbols[]` entries with `kind == "SYMBOL_FUNCTION"` have `match_percent`.
In the per-function instruction diff, `left` = the original (delinked from ROM), `right` = our build.
`DIFF_DELETE` = present in the original but missing in ours; `DIFF_INSERT` = the reverse.

**All 38 functions are implemented.** The `.text` delink covers the full range
(`0x021256c0`–`0x02127444`), so any new edit affects all of them at once.

## Status (objdiff)

**28 of 38 functions are exact (100%), average 98.13%; `.text` 97.00%, `.rodata` 100%,
`.data` 100%.** The rest:

| Function | % | Notes |
|----------|---|-------|
| `func_ov013_02126960` | 76.5 | EffStampSub init; anim-build register allocation/scheduling |
| `func_ov013_02125fd0` | 83.2 | Eff task init; same class of reg-alloc diffs |
| `func_ov013_02125b8c` | 85.8 | RG/UG init; anim-build register allocation/scheduling |
| `func_ov013_02126254` | 95.2 | spark ring; loop register assignment (count/conv/owner colors) |
| `func_ov013_021265b0` | 95.9 | single-spark spawn; reg-alloc swaps (magic/global/owner) |
| `func_ov013_02127370` | 96.5 | UG handler; 0x19000 constant materialization position |
| `func_ov013_02126788` | 96.8 | EffStampSub update; reg swaps around the pc-rel loads |
| `func_ov013_02126ff4` | 99.4 | Shake handler; `r0`/`r1` swap on a conditional-pointer store |
| `func_ov013_02125838` | 99.8 | single `ldrne`/`moveq` pair |
| `func_ov013_02125a04` | 99.9 | instruction-identical; forward intra-TU `bl` shown as `branch_dest` |

`02125a04` and `02125838` are objdiff accounting artifacts, not code differences — after
normalising branch and literal-pool targets, `02125a04`'s 88 instructions are identical.
`021256c0`/`021256e4` reached 100% as a side effect of the `const` change below.

## Data sections now match (the ov012 recipe applies verbatim)

`.rodata` went 30% → 100% and `.data` 75.9% → 100%. Four changes, all mechanical:

- **`const` on every read-only table** — the three `Enm015Variant`s, the three
  `SpriteAnimEntry` tables and the three `BinIdentifier`s. Without this MWCC puts them
  in `.data`. This alone took `.rodata` to 94% and fixed `func_ov013_021256c0`/`021256e4`.
- **Padded sizes on the `char[]` arrays**, derived from the gap to the next symbol in
  `symbols.txt`: `[28] [4] [24] [28]` for the asset names and `[20] [24] [28] [20] [20]
  [48]` for the task names. The last one runs to the end of `.data`.
- **Keep the `char[]` arrays non-const** so they stay in `.data`. Adding `const` here
  drops `.data` to 0%.
- **`data_ov013_02127568` is split mid-literal** in the original: a 4-byte `"Apl_"` object
  plus a 24-byte remainder at `0x0212756c` (marked `ambiguous` in `symbols.txt`). The
  `BinIdentifier` still points at `0x02127568`, so splitting the array reproduces both
  extents with an identical byte image.

Two real bugs fell out of this:

- The three `Enm015Variant` initializers carried a spurious extra `0`, putting
  `unk_0A`/`unk_0C` one slot late — `unk_0A` read `0` instead of `0x19` and `unk_0C` read
  `0x19` instead of `0x2BC`. Confirmed against the original bytes at `0x02127444`
  (`... 0000 1900 bc02 0000`). The code reads these by name, so `.text` was unaffected
  and this was purely a data/behaviour bug.
- The `IsFrameFinished` frame-diff ternary in `func_ov013_02125d24` had the `+1` on the
  wrong arm. The original adds 1 when the frame *is* finished. Spelling it as an explicit
  `if`/`else` with the `+1` in the then-arm also stops MWCC if-converting both arms, which
  was costing three predicated instructions. 98.6% → 100%.

## Key codegen recipes discovered (MWCC 2.0 sp1p5, -O4,p)

- The recurring `bl _fflt / _fadd|_fsub / _ffix` dance with `0x3F000000` (0.5f) is a
  `round()`-style macro on an integer:
  `#define ROUND(v) ((s32)((v) > 0 ? (f32)((v) * 0x1000) + 0.5f : (f32)((v) * 0x1000) - 0.5f))`.
  `* 0x1000` (not `<< 12`) is required so the hoisted copy becomes `q2 * 0x28000`.
- The 12.12 fixed multiply helper: `static inline s32 Mth_MulFixed(s32 a, s32 b) {
  return (s32)(((s64)a * b + 0x800) >> 12); }` produces the register-form
  `mov rX,#0x800 / adds / mov rX,#0 / adc` sequence. Calling it as
  `Mth_MulFixed(tbl[a], ROUND(b))` (table value first) puts the table value in `rn` of `smull`.
- Div-by-60 chains: write `t % 60` and `t / 60` as separate expressions (no shared local) to get
  the two `smull` chains the original emits.
- Sparse 2-3 case switches compile to `cmp`/`beq` chains with the case bodies placed after the
  dispatch; 4 dense cases compile to an `addls pc, pc, rX, lsl #2` jump table.
- `if (A || B) return 0;` produces the branched shape used by the task handlers' case 1;
  separate ifs produce predicated returns (both shapes exist in the original — Eff uses separate
  ifs, EffStamp/EffStampSub/Shake use `||`).
- `angle = i * 0x200` inside the loop body blocks IVSR (matching the original's per-iteration
  `lsl` + `add rX, rX, #0x200`), unlike an accumulating `angle += 0x200` variable.
- `data->unk_00 = args->unk_00;` must re-read `args->unk_00` after `MI_CpuSet` (the local is used
  for later reads) — this reproduces the double `ldr` after the call.
- MWCC strength-reduces `x * 0x1000` into `lsl #0xC` for register values but reassociates
  `(q * 0x28) * 0x1000` into `q * 0x28000` when hoisting.
- Calls to `0x020824a0` must use the ov000 symbol name `Mini108_VBlank` (the original binary's
  linker name), likewise `CombatSprite_SetFlip` (0x02082750), `CombatSprite_SetPaletteMode`
  (0x020827c0) and `CombatActor_Render` (0x020831e4); using the old `func_ov003_*` spellings costs
  relocation-name mismatches in objdiff.
- Reading a table pointer into a **named local** before computing `&table[idx]` keeps the pointer in
  a single register across the anim build (no rematerialized reload before the `unk_2C` store).
- Assigning a **single `s16` local from both branches** of an if/else (then storing it once)
  produces the `lsl #0x10 / asr #0x10` sign-extend sequence; separate per-branch locals fold the
  truncation into the `strh` instead.
- `(s32)(f32)` conversions and `(s16)` locals aside, most remaining mismatches are pure register
  color permutations that follow from allocation order; small source reorderings (declaration order,
  statement order inside the anim-field block) shift them.

## Remaining functions to implement

None — all implemented. Remaining work is register-allocation/schedule matching for the four
anim-build/init functions and small scheduling diffs listed above.

## Useful symbol resolutions

Many ov003 helpers are unnamed; these are used by the implemented code:

- `0x02082f2c` = `CombatActor_PopPendingCommand` (declared in `CombatActor.h`)
- `0x02082b00` = `CombatSprite_Init`; `0x02082b0c` = `CombatSprite_Update`; `0x02082b64` = `CombatSprite_Render`
- `0x02082cc4` = `CombatSprite_Release`; `0x02082724` = `CombatSprite_SetPosition`; `0x02082730` (unnamed)
- `0x02082940` = `CombatSprite_InitAnim`; `0x02082998` = `CombatSprite_Load`; `0x02082a04` = `CombatSprite_LoadFromTable`
- `0x020824a0` = `CombatSprite_SetAnimFromTable` (multi-module alias; referenced as `Mini108_VBlank`)
- `0x02082750` = `CombatSprite_SetFlip`; `0x020827c0` = `CombatSprite_SetPaletteMode`; `0x020831e4` = `CombatActor_Render`
- `0x02082a04` projection helper cluster: `func_ov003_02084348`, `020843b0`, `020843ec`, `02084634`, `02084694`
- `0x020ccedc` / `0x020ccefc` take an engine index (0/1) and return a fixed-point screen metric.
- `0x0208a114` / `0x0208a164` / `0x0208a08c` are the 3D-SE helpers (`a164`'s first arg is
  `a114`'s return value).
- `data_0205e4e0` is a 12.12 fixed-point sin/cos table (pairs of s16).

## Notes

- `.rodata`/`.data` now both match, so the earlier "known gap" caveat is gone. That was the
  prerequisite for ever marking this TU `complete`.
- The ROM `arm9_ov013.bin` has **unapplied intra-module relocations**, so pointer *values* in
  `.rodata`/`.data` cannot be read from it (e.g. `0x021274ec` holds `0x02127180`, not the
  task-name pointer). String contents, padding and object extents can be read from it; use
  `symbols.txt` and `relocs.txt` for anything pointer-shaped.
- Overlay TUs are only linked from source when marked `complete` in `delinks.txt`; this one is
  not, so the ROM still links the delinked original (i.e. `ninja sha1` passing would not
  validate this source). `ninja check_modules` and `ninja check_symbols` both pass.
- `data_ov013_02127640` / `data_ov013_02127644` are two separate `s32` BSS task-id slots.
- The `Tsk_BtlEnm015_*` symbols were renamed in `symbols.txt` to match the source task-handle names.
- `Enm015Variant.unk_08` and `.unk_0C` are `u16` (the original loads them with `ldrh`).
- `BtlEnm015.unk_1E8` is `u16`; `BtlEnm015Shake.unk_0C` is `u16`.
- The RG task's `MI_CpuSet` clears `0x1E8` bytes (not `sizeof(BtlEnm015)` = 0x1EC).

## Remaining work

Ten functions, all register-allocation or scheduling decisions. Source shapes already tried and
rejected are listed so they are not retried:

- `func_ov013_02126960` (76.5%): the seven `SpriteAnimationEx` stores are emitted in a different
  order, which cascades into the `ldrh [r4, #..]` hoisting. Tried and rejected: reordering the
  seven stores to the original's emission order (73.0%), spelling `table[3] + 1` inline instead of
  via the `animData` local (74.1%), and both together (68.0%). The committed order is the best of six.
- `func_ov013_02126ff4` (99.4%): a pure `r0`/`r1` swap on
  `*((cond) ? &a : &b) = -1`. Tried and rejected: selecting into an `s32*` local, an
  `if`/`else` assigning each slot directly (90.3%), and a pointer local with the condition
  tested the other way round. All produce 99.4%; the register choice is not steerable.
- `func_ov013_02125a04` (99.9%) / `func_ov013_02125838` (99.8%): instruction-identical or
  near-identical; objdiff relocation-index artifacts. Not fixable in source.
- `func_ov013_02126254`, `021265b0`, `02127370`, `02126788`, `02125b8c`, `02125fd0`: the
  register-colour and scheduling differences described in the table above; consistent with
  ov012's experience these have not responded to source restructuring.

