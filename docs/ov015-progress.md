# Overlay 15 (`Tsk_BtlEnm026_*`) decompilation — progress handoff

**Date:** 2026-09-25
**Branch:** `decomp-ov015` (off `decomp-ov013`)
**Worktree:** `E:\Git\twewy-ov015` (session working directory; `extract/` is a junction to `E:\Git\twewy-ov013\extract`)
**Source:** `src/Combat/Noise/BtlEnm026/BtlEnm026.c`
**Header:** `include/Combat/Noise/Private/BtlEnm026.h`
**Config:** `config/usa/arm9/overlays/ov015/{delinks,symbols}.txt`

## Overlay summary

Overlay 15 is the enemy-026 (Raven/Reaper-type twin Noise) combat overlay. It provides three EasyTask handlers:

| Task | Handler | Data size |
|------|---------|-----------|
| `Tsk_BtlEnm026_Icon` | `func_ov015_02126860` | 0x134 |
| `Tsk_BtlEnm026_RG` | `func_ov015_02126f44` | 0x1F4 |
| `Tsk_BtlEnm026_UG` | `func_ov015_021275fc` | 0x1F4 |

57 functions, `.text` = `0x021256c0`–`0x02127fd8` (0x2918 bytes).
Original disassembly: `E:\Git\twewy\build\usa\asm\ov015_4.s` (3296 lines).

## Workflow

From `E:\Git\twewy-ov015`:

```powershell
ninja build/usa/src/Combat/Noise/BtlEnm026/BtlEnm026.o
ninja build/usa/delinks/src/Combat/Noise/BtlEnm026/BtlEnm026.o   # also regenerates objdiff.yml targets
.\objdiff-cli.exe diff -p . -u "src/Combat/Noise/BtlEnm026/BtlEnm026" -o build/ov_diff.json --format json
```

Parse `build/ov_diff.json`: `left.symbols[]` with `kind == "SYMBOL_FUNCTION"` have `match_percent`.
In the per-function instruction diff, `left` = original (delinked from ROM), `right` = ours.
`DIFF_DELETE` = in the original only; `DIFF_INSERT` = in ours only.
Helper scripts: `build/scratch/ov.ps1 -end 0x...` (extend delink prefix + rebuild + report) and
`build/scratch/disfn.py <symbol>` (disassemble one function from our .o with capstone).

## Status (objdiff)

**45 of 57 functions are exact (100%), average 98.31%; `.text` 95.30%, `.rodata` 66.9%,
`.data` 100%.** The `.text` delink covers the full range.
Remaining:

| Function | % | Notes |
|----------|---|-------|
| `func_ov015_021261d8` | 80.6 | Icon init; anim-build register allocation/scheduling (unk_2C store position etc.) |
| `func_ov015_0212699c` | 88.0 | RG state fn; atan2/magnitude fixed-mul reg colors |
| `func_ov015_02126fd8` | 91.6 | UG state fn; same atan2 cluster |
| `func_ov015_021257f8` | 91.8 | shared pre-update; result10 store scheduling |
| `func_ov015_02126464` | 92.5 | Icon update; hp division reg colors |
| `func_ov015_02127e7c` | 94.5 | twin cooldown; push/pop reg set + reload scheduling |
| `func_ov015_0212786c` | 83.8 | twin state transfer; early-guard form loads twin before unk14 check |
| `func_ov015_02125d0c` | 83.4 | particle spawner; param-block store scheduling |
| `func_ov015_02127ac4` | 99.1 | palette mode; one `beq` vs predicated-epilogue slot left |
| `func_ov015_02127c18` | 98.8 | counter compare reg colors |
| `func_ov015_02125f80` | 99.8 | one `.word data_ov015_0212851a` literal (ours targets `data_ov015_02128500+0x1a`) |
| `func_ov015_021256f8` | 99.9 | byte-identical; literal reloc accounting |
| `func_ov015_021256c0` | 99.3 | byte-identical; second `.word` literal is `data_ov015_02128130+0x4` in ours |

## WARNING: stale `objdiff.json` silently invalidates measurements

`objdiff.json` is generated and can go stale. When the requested unit is missing from it,
`objdiff-cli diff -u <unit>` **fails with `Unit not found` and does not write its output
file** — so the previous run's `build/ov_diff.json` is still sitting there, and the harness
prints entirely plausible, completely unchanged numbers. This cost a lot of time here: a
large `const` rewrite plus a full reordering appeared to have had *zero* effect, and the
"reordering does not change the layout" conclusion drawn from it was wrong.

Two guards, both now in `build/scratch/ov.ps1`:

- `ninja objdiff.json` is run first, so the unit list is regenerated.
- `build/ov_diff.json` is deleted before the diff and the harness exits non-zero if it is
  not recreated, so a failed diff can never masquerade as a result.

Sanity check whenever a number refuses to move: confirm the unit is present in
`objdiff.json` before believing the score. `objdiff-cli` resolving the unit at all is the
minimum bar.

## Data sections: the ov012/ov013 recipe mostly applies, with one hard difference

`.data` went 77.3% → **100%** and `.rodata` 4.8% → **66.9%**. The changes:

- **`const` on every read-only table** — the ten `Enm026Variant`s, the twelve
  `BinIdentifier`s, both `SpriteAnimEntry` tables, `Enm026IdPair` and `Enm026StateFns`.
- **Padded sizes on the `char[]` arrays**, from the gap to the next symbol in `symbols.txt`:
  `[28] [28] [28] [4] [24] [28] [28] [28] [28] [28] [20] [8] [20] [32] [32] [20] [44]`.
  The last one runs to the end of `.data`.
- **Keep the `char[]` arrays non-**`const`** so they stay in `.data`.
- **Two literals are split mid-string in the original** and are reproduced by splitting the
  array: `0x02128334` `"Apl_"` + `0x02128338` `"Hor/Grp_BtlEnm026d.bin"`, and `0x021283dc`
  `"Apl_Hor/Grp_BtlEnm02"` + `0x021283f0` `"6b.bin"`. Both `BinIdentifier`s still point at
  the first half, so the runtime paths are unchanged. This is the same pattern as ov012's
  `Tsk_BtlEnm014_RG` and ov013's `Grp_BtlEnm015b.bin`.

### Why `.rodata` stops at 66.9% when ov012/ov013 reached 100%

Two independent reasons, neither fixable in C:

1. **MWCC does not emit `.rodata` in declaration order — it groups objects by size.** Our
   object comes out as 8-byte, then 12-byte, then 16-byte, then 104-byte, then 144-byte
   objects, and within each group the order is not even the declaration order. The original's
   `.rodata` order (`160, 80, 104, 104, 144, 8, 8, 104, 36`) follows no size pattern at all.
   Reordering the declarations has **no effect** on the emitted layout. ov012 and ov013
   reached 100% only because their size-grouped order happened to coincide with the
   original's; ov015 does not, and it has far more relocations for objdiff to mispair
   (74 in `.rodata` alone). This is luck of the draw, not a property of the recipe.
2. **`symbols.txt` declares two spurious 4-byte objects** that sit *inside* arrays and so
   cannot be reproduced in C at all: `data_ov015_02128134` (the `unk_04` field of the first
   `Enm026IdPair` entry, inside `data_ov015_02128130[13]`) and `data_ov015_0212819c` (inside
   `data_ov015_02128198[18]`). Both are 4 bytes into a struct array, so no C declaration can
   produce them as separate objects. Unlike ov012's `0x02127a18` and ov013's `0x0212756c`
   they are **not** flagged `ambiguous`, which is why they were not caught earlier. They are
   the direct cause of the two `.word` literal mismatches in `func_ov015_021256c0` (99.3%)
   and `func_ov015_02125f80` (99.8%), where the original has distinct symbols and ours
   renders `data_ov015_02128130+0x4`.

Fixing (2) means editing `symbols.txt` to drop or mark the two phantom objects. That is a
ground-truth change rather than a source change, so it has not been done unilaterally.

## Key codegen recipes (MWCC 2.0 sp1p5, -O4,p) — see also `docs/ov013-progress.md`

- `ROUND(v)` and `Mth_MulFixed(a,b)` helpers as in ov013 (register-form `mov #0x800/adds/mov #0/adc`).
- `Mth_MulFixed(x, 0x2000)` strength-reduces to `lsl #13` + carry handling; pass the table
  value first: `Mth_MulFixed(sin, mag)` puts `sin` in `Rm` of `smull`.
- The atan2→velocity cluster: `FX_Atan2Idx(dy, dx) >> 4`, index ×2, `mag = Mth_MulFixed(ABS(sin)+0x1000, 0x2000)`,
  then `Mth_MulFixed(sin, mag)` / `Mth_MulFixed(cos, mag)`.
- Sparse 2-3 case switches → `cmp/beq` chains; 4+ dense cases → `addls pc, pc, rX, lsl #2` table.
- `if (A || B) return x;` → branched form; separate `if`s → predicated returns. Both shapes occur.
- **Nesting a body inside `if (A == B)` makes MWCC branch over it; the original often
  predicates the epilogue instead** (`cmp / ldmneia sp!, {r3,r4,r5,pc}`). Writing
  `if (A != B) { return; } <body>` restores the predicated early return. Found in
  `func_ov015_02127ac4` (98.1% → 99.1%); the same inversion is worth trying wherever a
  function scores just under 100% on a `beq`/`bne`-versus-`ldm*ia` slot.
- Common-tail code shared by `case 0`/`case 1` via fallthrough is written as
  `case 0: ...; /* fallthrough */ case 1: ...common...; break;` (021268b0 pattern, now 100%).
- Flip ternaries emit `rsb` when the negative arm is first:
  `(flip ? 0x8000 : -0x8000)` → `rsbeq`; `(flip ? -0x8000 : 0x8000)` → `movgt/movle`.
- `state->unk_1E` etc. reload after `strh` (no store-to-load forwarding for sub-word stores).
- Calls to `0x020824a0` use the ov000 name `Mini108_VBlank`; `0x02082d04` is `Sprite_Restart`;
  `0x02082f1c` is `CombatActor_SetPendingCommand`; `0x020827c0` is `CombatSprite_SetPaletteMode`.
- `02082d04` takes the `Sprite*` inside `CombatSprite` (`&data->unk_084.sprite`).

## Structure notes

- Shared global state `data_ov015_02128500` (0x40 bytes of BSS): `{initialized, variant, flag08,
  counter0C(s8), result10, unk14, timer18(s16), seconds1A(s16), counter1C(s16), unk_1E(s16),
  unk_20, unk_24, unk_28(u16), pad}`. One struct symbol; all accesses use base+offset literals.
- `BtlEnm026.unk_184` = the twin task's data pointer; each task's `unk_188` = itself.
  `data->unk_184->unk_188` = the twin.
- `data_ov015_02128198[18]` = per-variant `{enter, update}` fn pairs (odd/even interleave in ROM).
- `data_ov015_02128424[18]` = per-variant `Enm026Variant*` table; `data_ov015_02128130[13]` maps
  enemy ids 0x11..0x1D to variant indices.
- `Tsk_BtlEnm026_Icon` params: `{owner*, kind(s16), pad, params*}`; the Icon task draws the HP
  badge bars (ones/tens digits from `SpriteAnimEntry` frames 3..12).
- RG boss pointer is `data_ov003_020e71b8->unk3D89C`, UG is `->unk3D898`.
- `ninja sha1` currently fails on 4 bytes of NDS header metadata (`0x6C/0x6D` + `0x15E/0x15F`
  — arm9 overlay-table length/pointer fields), because overlay 15 is *not* yet marked `complete`
  and the delinked ROM still contains the original bytes. This is expected and unrelated to the
  source; objdiff is the metric. (Same rule as ov013.)

## Remaining work

None implemented-outstanding — all 57 functions exist in source. Remaining is
register-allocation/schedule matching for the functions listed above, plus (optionally)
renaming the ov015 symbols beyond the three task handles.
