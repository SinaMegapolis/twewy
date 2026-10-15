# Shop.c part-decompilation brief

You are decompiling functions of the TWEWY (Nintendo DS) decompilation project at `E:\Git\twewy`.
Goal: **C that compiles to object code byte-identical to the retail ROM**, verified with a
target-vs-current instruction diff.

## Your assignment
A single part file, e.g. `src/Interface/Menu/Shop_p3.c`. It already contains an **m2c draft**
of every function you own (a decompiler's best-effort C). Your job: turn each function into
correct, matching C. Only edit **your** part file. Do NOT edit `src/Interface/Menu/Shop.c`,
`include/ShopPart.h`, or anything under `include/` — see "Header changes" below.

## Fast loop (Windows PowerShell, run from the repo root)

Per-function, works even while sibling functions in your file are broken (preferred):
```
python tools/probe.py src/Interface/Menu/Shop_p3.c func_ov043_020ae670
python tools/probe.py src/Interface/Menu/Shop_p3.c          # probe the whole file
```
It compiles just that function in isolation and prints either `MATCH <fn>` or `N/M rows differ`.
Diff rows are `M` = target disassembly rows; when `N == 0` the function is byte-identical.

Once the whole part file compiles, you can also do the real thing:
```
pwsh -NoProfile -File tools\partdiff.ps1 Shop_p3 func_ov043_020ae670
```
(compiles the whole `Shop_p3.c` and diffs one function against the target `Shop.o`).

Target assembly for a function (read it before guessing):
```
pwsh -NoProfile -File tools\tgt.ps1 Interface/Menu/Shop func_ov043_020ae670
```

Compile a whole part file (to see all errors):
```
pwsh -NoProfile -File tools\mwcc.ps1 -src src/Interface/Menu/Shop_p3.c -obj build/usa/tmp/Shop_p3.o
```

## Shared context
- `include/ShopPart.h` — all includes, the root `ShopState` struct, and the data externs.
- `include/Interface/Menu/Shop.h` — `ShopObject`, `ShopItemSlot`, `ShopItemTableEntry`.
- `include/Player/Inventory/Pins.h` — `RawPinData` (0x34) and friends.
- `include/SpriteMgr.h`, `include/Engine/Core/OamMgr.h`, `include/Engine/EasyTask.h`.
- Sibling TUs that are already 100 % — the best style reference: `Shop_item.c`,
  `Shop_window0.c`, `Shop_textScr.c`, `Shop_tab.c`, and the fully decompiled
  `src/Interface/Menu/MenuEquip.c` (the closest analogue — same `MenuStateBase` root,
  same `0x21618`-ish task-id layout).

## Fixing the m2c draft — known artifacts
- `? sp0;` / bare `sp0`, `sp4`, `sp1C` … — m2c's unknown stack slots. Declare the real local
  variables (check the target's stack frame) or a `struct { … } sp;`. Never leave a bare `spNN`.
- `M2C_ERROR(/* Read from unset register $r0 */)` — m2c failed to model a value; read the
  target asm and write the real expression.
- `MULTU_HI(a, b)`, `_ll_udiv(...)`, `SECOND_REG(...)`, `SIGNED_HI_...` — write plain C:
  `a / b`, `a % b`, and correct signed/unsigned types. See
  `.opencode/skills/objdiff-decomp/references/mwcc-emission.md`.
- `x->unk_XXX` where the base is `void*`/`int*` — retype the parameter/local to a struct.
- `x + <N>` on a typed pointer — m2c emits byte offsets as pointer arithmetic; fix by using the
  real field or a `(u8*)` cast as appropriate (the offending `resourcePool`-anchor forms are
  already rewritten to `base.taskPool` / `->shop`).
- `(arg0 + (var * 4))->unk_2163C` — index a real array member (`arg0->taskId_Item[var]`).
- Wrong parameter count/type — read the target prologue (what it loads/stores at the arg slots).

## Header changes
Do not edit shared headers. If a needed field is missing from `ShopObject`/`ShopState`/etc.,
define a **local mirror** struct in your part file and cast, e.g.
```c
typedef struct {
    /* 0x000 */ char pad_0[0x774];
    /* 0x774 */ u16  unk_774;
    /* 0x776 */ u16  unk_776;
    /* 0x778 */ u16  unk_778;
    /* 0x77A */ char pad_77A[0x788 - 0x77A];
    /* 0x788 */ u16  unk_788;
    /* 0x78A */ u16  unk_78A;
} MyShopView;
```
Then use `((MyShopView*)&arg0->shop)->unk_788`. Give the typedef a part-unique name. In your
final report, list every field you needed so it can be added to the real header centrally.

## Style
Match the surrounding code: 4-space indent, `/* 0x00 */ name;` field comments, `static` for
file-local helpers, `BOOL`/`TRUE`/`FALSE`, existing naming. Keep signatures consistent with how
the target calls/returns.

## What to report back
- For each function: MATCH, or `N/M rows differ` with a one-line reason if stuck.
- Any header field you had to mirror locally (offset + type + name).
- Whether the whole part file compiles.

Get as many functions to `MATCH` as you can. Prefer finishing easy functions over perfecting one.
