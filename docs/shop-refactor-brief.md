# Shop.c struct/pointer-arithmetic cleanup brief

You are cleaning up decompiled C in the TWEWY (Nintendo DS) decompilation repo at `E:\Git\twewy`.
The byte match must **not** regress: every function you touch must still report `MATCH` from
`tools/probe.py` afterwards.

## Context

`src/Interface/Menu/Shop.c` (overlay ov043) is one big translation unit of 118 functions. It is
currently split into `src/Interface/Menu/Shop_p1.c` … `Shop_p8.c`, which `Shop.c` `#include`s.
Each part starts with `#include "ShopPart.h"` (the internal header: includes, the `ShopState`
root struct, data externs, and inter-function prototypes).

`ShopObject` in `include/Interface/Menu/Shop.h` was just expanded so that **every** padded/unnamed
offset now has a named scalar field (e.g. `unk_78C` is now `s16`, `unk_7B0`… etc.). Because of that,
most functions no longer need their local "view" typedefs at all.

## Your job

For every function in your assigned part file:

1. **Delete local mirror typedefs of `ShopObject`.** A typedef is a `ShopObject` mirror if it has
   `slots[42]` at 0x000, or `unk_738[12]`, or a `pad_000`/`pad_0` reaching offset ≥ 0x700, or
   otherwise reproduces offsets 0x700–0x954. Replace `SomeMirror *shop = (SomeMirror *)arg0;`
   (and the `((SomeMirror *)shop)->field` / `((SomeMirror *)arg0)->field` casts) with plain
   `ShopObject *shop = arg0;` and `shop->field` (or `arg0->field`).  Use the real field types from
   `Shop.h` — do not keep a second definition.
2. **Delete local mirrors of the other header types**: a typedef with `unk_00; unk_02; … unk_24[7]`
   is `ShopItemSlot` (from `Shop.h`); one with `unk_00; unk_08; unk_10; unk_14` is
   `ShopItemTableEntry`; the 0x7BC record `{u16 unk_00; u8 unk_02; u8 unk_03;}` is `ShopObjectEntry`.
3. **Replace pointer arithmetic with struct/array access.** Examples seen in this file:
   - `((ShopP2 *) ((u8 *) shop + (shop->unk_778 - 2) * 2))->unk_7A2`  →  `shop->unk_7A2[shop->unk_778 - 2]`
   - `(ShopP6Slot *) ((u8 *) var_r1_2 + 0x2C)` (walking `slots[]`)  →  `&shop->slots[i]`
   - `func_ov043_020b2af4((MenuBgResource *) ((u8 *) arg0 + 0x900))`  →  `&((ShopState *)arg0)->shop.resources[?]`
     (compute the right index from the offset; `resources` starts at 0x874)
   - `*(u16 *) ((u8 *) &gSaveData + 0x1AF6 + var_r9 * 2)` → use the `gSaveData` member if it exists,
     otherwise `gSaveData.someArray[var_r9]`.
   Prefer a real member / array index whenever the offsets line up; only keep a cast if the target
   really is an opaque byte buffer.
4. **Record structs** (`ShopP6Sale`, `ShopP6ShopItem`, `ShopP6PinInfo`, `ShopP6Menu0`, `ShopP6Menu2C`,
   `ShopP6Scratch`, `ShopP6Frame`, `ShopSeRow`, `ShopSeTable`, `ShopSeList`, `ShopP7_Layout`, and any
   `typedef struct` inside a *loop* or a *heap record*) are **not** `ShopObject`. Keep them, but:
   - if the same struct is defined in more than one function in your file, define it **once** in the
     file-level prelude (just after `#include "ShopPart.h"`, before the first function) and delete the
     inner copies;
   - check the existing headers (`include/Player/Inventory/*.h`, `include/SpriteMgr.h`,
     `include/Engine/*.h`) for a type that already describes the same record; if one exists, use it
     and delete the local one.  `RawPinData`/`RawItemData`/`RawFoodData`/`RawTreasureData` are real
     records — prefer them where the layout matches.
   - **Exception**: the hand-tuned `ShopP7_Layout` views in `Shop_p7.c` are load-scheduling/register
     tricks.  Leave those functions' local views and `volatile` spill structure exactly as they are
     (you may leave the `// Nonmatching:` comments).

## Verifying

Per function (works even when a sibling is broken):
```
python tools/probe.py src/Interface/Menu/Shop_p1.c func_ov043_020ac5d8
```
It compiles that function in isolation (with the file's prelude) and prints `MATCH` / `NN.NN%`.
Whole file:
```
python tools/probe.py src/Interface/Menu/Shop_p1.c
```
Compile the whole part:
```
pwsh -NoProfile -File tools\mwcc.ps1 -src src/Interface/Menu/Shop_p1.c -obj build/usa/tmp/Shop_p1.o
```
Target assembly for a function:
```
pwsh -NoProfile -File tools\tgt.ps1 Interface/Menu/Shop func_ov043_020ac5d8
```

## Hard rules

- Edit **only** your assigned `Shop_pN.c`.  Do **not** touch `Shop.c`, `include/Shop.h`,
  `include/ShopPart.h`, or any other file, and do not commit.
- A function that matched before must still match.  If deleting a mirror changes codegen (usually a
  signed-vs-unsigned load, e.g. `ldrsh` vs `ldrh`), fix it with an explicit cast in the expression
  (`(s16) shop->unk_XXX` or `(u16) shop->unk_XXX`) so the instruction sequence is preserved.
- Functions already marked `// Nonmatching: …` do not have to reach 100%, but their score must not
  get worse.
- Prefer `ShopObject *shop = arg0;` (or `ShopState *state = arg0;`) over repeated `arg0->base…`.
- Keep declarations/statement order identical unless a change is needed for codegen; the point is
  readability, not rewriting.

## Report back

- per-function status (MATCH / % / unchanged Nonmatching),
- the list of local typedefs you deleted and the header type each was replaced by,
- any struct that duplicated an existing header type (and which header),
- any function where you had to keep a cast or a local view (and why).
