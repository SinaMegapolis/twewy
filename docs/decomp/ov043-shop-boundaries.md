# OV043 (Menus) — TU-boundary check for the Shop units

This note records what the TU-boundary analysis (`docs/tu-boundaries/`, run with the ov043 delinks
hidden) says about the Shop units, and whether re-splitting `src/Interface/Menu/Shop.c` helps the
match.

## Method

The analysis (`tools/tu_boundaries/{existing,pipeline}.py`, delinks hidden) proposes source-file
boundaries from the ROM's data order (mwcc sorts a TU's `.rodata`/`.data` objects by ascending size,
then appends tail literals in source order), task descriptors, and pooled data blocks. Ran on
`ov043`; the cut list is reproduced below.

## Result

| existing TU | inner cut | confidence | evidence |
|---|---|---|---|
| `src/Interface/Menu/Shop.c` `0x020ac5d8-0x020b2b48` | **(none)** | — | its `.rodata`/`.data` runs are single ascending size runs → one TU |
| `src/Interface/Menu/Shop/Shop_textScr.c` `0x020b5198-0x020b699c` | `0x020b6668` | **B** | `data ro(0x020ca610)`: a `.rodata` break snapped onto the `Tsk_Shop_textScr` creator |
| `src/Debug/Takami/MenuScena.c` | 9 cuts | A+ | one per `Tsk_MenuScena_*` task |
| `src/Debug/Takami/NRep.c` | 2 cuts | A+ | per `Tsk_Nrep_icon` / `Tsk_Nrep_textScr` |

So the Shop dispatcher unit (`Shop.c`, the 118-function unit) is **a single TU** by every available
signal; the only flagged Shop split is inside `Shop_textScr.c`:

- TU A: `0x020b5198 - 0x020b6668` (`func_ov043_020b5198` … `func_ov043_020b63fc`)
- TU B: `0x020b6668 - 0x020b699c` (`func_ov043_020b6668` … `Shop_textScr_CreateTask`)

with `.rodata` split at `0x020ca610`.

## Does splitting `Shop.c` help the match? No.

Controlled test (`tools/mwcc.ps1`, same flags): compile `Shop.c` as one TU, then as two TUs
(`Shop_p1..4` and `Shop_p5..8`), and compare the full disassembly of every non-matching function
(addresses/symbols normalised).

All 12 non-matching functions were **byte-identical** between the one-TU and two-TU builds:

```
func_ov043_020acbd0  identical   func_ov043_020b0d1c  identical
func_ov043_020aeb14  identical   func_ov043_020b0f20  identical
func_ov043_020aef20  identical   func_ov043_020b1404  identical
func_ov043_020afd6c  identical   func_ov043_020b1618  identical
func_ov043_020b02fc  identical   func_ov043_020b17d0  identical
func_ov043_020b1830  identical   func_ov043_020b1bd4  identical
```

Register allocation and instruction scheduling are per-function, and `-ipa file` does not rewrite
the code of these non-`static` functions, so moving the TU boundary changes nothing.

## Data ownership

`Shop.c`'s `.rodata` (0x020ca18c-0x020ca360), `.data` (0x020cc5bc-0x020cc890) and `.bss`
(0x020cd288) are now defined by the unit itself in `src/Interface/Menu/Shop_data.c` (included by
`Shop.c`) instead of being `extern`:

- `.data`: the bin path/name string objects plus the `Seq_Shop()` pointer.
- `.rodata`: the `OverlayCB[3]` reload-callback table, the 25-entry `BinIdentifier[]` bin table
  (the delinker split it at reference points; the address labels are reproduced as separate objects),
  the menu layout table (0x020ca260) and the two sound-effect tables (0x020ca2a8 / 0x020ca2c0).
  All are `const`, which is what puts them in `.rodata` (the previous non-const `extern`s resolved to
  `.data`).
- `.bss`: the Shop root `ShopState*` (`data_ov043_020cd288`).

With those definitions the unit's `.rodata`, `.data` and `.bss` all match the target 100%.

## Conclusion

- Keep `src/Interface/Menu/Shop.c` as one TU; there is no evidence for splitting it and splitting
  does not change codegen.
- `Shop_textScr.c` is the one Shop unit that the data-order evidence says is two TUs; split it at
  `0x020b6668` (with `.rodata` at `0x020ca610`) the next time that file is worked on. Its remaining
  non-matching functions are register/scheduling ties, so the split is expected to fix data
  ownership (and let the halves be marked `complete`) rather than the instructions.
