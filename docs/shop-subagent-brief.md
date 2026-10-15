You are decompiling functions of the TWEWY (Nintendo DS) decompilation project at `E:\Git\twewy`.
Target: **code that object-identical to the retail ROM**, verified with objdiff.

## Your assignment
Decompile the following non-matching functions:

{ASSIGNMENT}

Only edit the files listed above. Do NOT edit any other file (especially nothing under `include/`,
`src/Interface/Menu/Shop/Shop_textScr.c`, or `src/Interface/Menu/Shop/Shop.c`).

## Tooling (Windows PowerShell; run from the repo root)

1. **Target asm** for a function:
   `arm-none-eabi-objdump -dr build/usa/delinks/src/Interface/Menu/Shop/<File>.o --disassemble=<fn>`
   (there is also `pwsh -NoProfile -File tools\tgt.ps1 Interface/Menu/Shop/<File> <fn>`)
2. **Rebuild + normalised diff**:
   `pwsh -NoProfile -File tools\fndiff.ps1 Interface/Menu/Shop/<File> <fn>`
   - left column = target, right column = your build. `*` marks the differing rows.
   - `MATCH <fn>` means the function is byte-identical — you are done with that function.
   - The script rebuilds the single object, so each iteration takes ~1 s.
3. **Full TU status**: `python tools/shopreport.py <Substring>` after a full `ninja`.

IMPORTANT: never run a bare `ninja` with no target (that takes minutes and regenerates report.json).
Only `ninja build/usa/src/Interface/Menu/Shop/<File>.o`.

## How the target is built
The compiler is MetroWerks mwccarm 2.0 sp1p5 with `-O4,p -enum int -char signed -proc arm946e
-gccext,on -fp soft -inline noauto -RTTI off -interworking -sym on -gccinc -nolink -ipa file
-str noreuse -lang c99 -d REGION_USA`.

Known emission rules (learned the hard way — check these FIRST):
- `/` and `%` written directly as plain C reproduce the target's smull/umull magic-multiply.
  Replace any m2c magic-multiply blob with a real division.
- `stmdb sp!, {{r0,r1,r2,r3}}` as the first instruction ⇒ the function is **variadic**.
- Wrong first stack-arg offset ⇒ wrong parameter count (m2c drops unused params).
- `ldrh`/`ldrb` at an argument slot ⇒ the parameter is `u16`/`u8`.
- All compares first, bodies out-of-line ⇒ the source was a `switch`, not if/else-if.
  Switch case bodies are emitted in source order.
- `ldrh [pool_base,#k]` / `strh [sp,#m]` copy loop ⇒ a **local array initialiser**
  (`u16 t[2] = {{...}}`), which mwcc emits as an anonymous `.rodata` template.
- Stack slot order is **reverse declaration order**; the repo's `spNN` naming relies on that.
- `and rX, rY, #255` before a bitfield insert ⇒ a `u8` in the source.
- `ldrsh` of a `u16` struct field ⇒ the source cast `(s16)obj->field`.
- A path reaching `pop {{...,pc}}` without writing `r0` ⇒ the original had `return;` with no value.
- `add rN,rI,#1; lsl #16; lsr #16` on a loop counter ⇒ `u16 i`.
- A wrong `ldrh`/`ldr` *offset* from a pool base is a **data-layout** bug: mwcc emits one pool
  word per section (the first object's address) and reaches the rest by offset. Check the
  `.rodata` object order with
  `arm-none-eabi-objdump -t build/usa/src/Interface/Menu/Shop/<File>.o`
  against the delinked target. Objects sort by size ascending; ties follow a heapsort over the
  reversed creation order, so *moving a `static` definition earlier or later in the file* can
  flip a tie. This is often the whole fix.

## Cross-reference material
- `include/Interface/Menu/Shop.h` — `ShopObject`, `ShopItemSlot`, `ShopItemTableEntry`.
- `include/SpriteMgr.h` — `Sprite`, `SpriteAnimation`, `SpriteCell`, `Unk_Bitfield`.
- `include/Engine/Core/OamMgr.h` — `g_OamMgr[3]` (`OamManager` is `0x108C` bytes, so
  `g_OamMgr+0x108C` == `&g_OamMgr[1]`), `DISPLAY_MAIN`/`DISPLAY_SUB`/`DISPLAY_EXTENDED`.
- `include/Util/SysFont.h`, `include/Engine/File/DatMgr.h`, `include/Engine/File/BinMgr.h`.
- Sibling TUs in the same directory that are already 100 % — they are the best style/structure
  reference. Look at `Shop_item.c`, `Shop_item2.c`, `Shop_mov.c`, `Shop_window0.c`,
  `Shop_castU.c` and especially `MenuBadge_bpGaugeU.c` (the gauge analogue).

## Struct access vs pointer arithmetic
Prefer struct field access and array indexing that reflects what the assembly does.
Only use raw pointer arithmetic when the assembly genuinely walks a byte buffer
(e.g. the pack-entry char-data copy loops).

## Style
Match the surrounding code: 4-space indent, `/* 0x00 */ name;` field comments in local struct
typedefs, `static` for file-local helpers, `BOOL`/`TRUE`/`FALSE` for booleans, existing
`<param>`/`argN` naming conventions. Keep the existing function signatures and call sites
exactly as they are — the callers are already matching, so a signature change breaks them.

## What to report back
For each function you finished: the final `fndiff` verdict. If a function ends up with a pure
register-allocation mismatch and you cannot close it, add the repo's conventional marker
`// Nonmatching: <short reason>` directly above the function and say so in your summary.
Report any `ShopObject` field you needed but could not find, with the offset, so it can be added
to the header centrally.

Work until every assigned function either MATCHes or is marked Nonmatching.
