# Name conversion suggestions - `Events.c` / `EventManager.c` (overlay 030)

**Status: applied to the USA tree (JP deliberately not updated).** Applied: every `FldEvent_*` rename in sections 3-4 for chapters 0-20 (194 functions), the section 5 data / typedef renames, the section 6 `FieldEventManager` field renames, and the step-0 signature fixes. **Not applied** (still proposals): all `PostGameNN` names (chapters 21-43), the `SelectNextEvent_*` selectors and `return 0;` stubs, the two `(leave unnamed)` functions in section 3 (`a68f4`, `aac28`), `data_ov030_020d9588`/`958a`, the `gSaveData.unk_24C8`/`unk_24C4` save fields, and `unk_21D1C`. Verified: USA `ninja check sha1` passes; the five non-linked TUs (`EventManager`, `FieldBg`, `FieldMain`, `FieldEncounter`, `FieldBattleSetup`) were compiled and their code/data sections are byte-identical to HEAD (only debug names differ).

Generated from the current `src/` state; every function in both files is listed, not just the notable ones.

---

## 0. Summary

| file | functions | referenced | orphaned |
|---|---|---|---|
| `EventManager.c` | 51 unnamed + 6 named | 57 | 0 |
| `Events.c` | 243 | 133 | 110 |

`Events.c` breakdown by role:

| role | count | callers |
|---|---|---|
| script-end handler (`data_ov030_020da618[]`) | 43 | the dispatch table (+1 more handler, `table[43]`, defined in `FieldBg.c`) |
| choice-window callback | 84 | 72 to `func_ov030_020a9f54` (= `FldEvent_CreateChoiceWindow`), 12 to `func_ov030_020aa0c8` (= `FldEvent_RunScanPrompt`) (verified, disjoint) |
| trade / post-script choice callback (`unk_21D1C`) | 6 | stored in `unk_21D24`, driven by `unk_21D18 == 5` in `FldEvent_HandleScriptEnd` |
| chapter initializer `void f(void)` (41) + `void f(mgr)` (1: `func_ov030_020a2208`) | 42 | **none - caller not decompiled** (+1 more init in `FieldBg.c`: `func_ov030_02084980` = chapter 0's, see 4.2) |
| area trigger checker | 26 | **none - caller not decompiled; several prototypes say `s32 f(s32)` but pass the arg to `FldEvent_BeginScript` as `mgr*` - fix prototype to `(FieldEventManager*)` before renaming** |
| event selector `s32 f(void)` / stub | 42 | **none - caller not decompiled** |

**Recommended application order** (highest naming leverage, least churn first):

0. Fix signatures first - `func_ov030_020a6960` (no `return`; return type must become `u32*` and the body must `return eventMgr->unk_21CD0`; most callers cast the result to `u32*`, but `EventManager.c:369` (`== arg1`), `:408` and `:1526` (2nd arg of `EasyTask_GetTaskById`, whose parameter is `u32` - `src/Engine/EasyTask.c:275`) treat it as a scalar, while `:1583` does `*(u32*)...` for the same operation as `:408` - those three sites are missing the deref, so fix them together), `func_ov030_020a6d34` (returns the `unk_21C48[i]` value `-1`; must be `&unk_21C48[i]` - see `EventManager.c:815,824,835` dereferencing it; the `s32` wrappers `func_ov030_020a6d80`/`func_ov030_020a6d94` must become `u32*` too, and the no-free-slot `return 0` needs a defined value), `func_ov030_020a75c0` (declared `s32`, no `return`), the `s32 f(s32)` trigger prototypes, and `func_ov030_020aa0c8`'s callback parameter / `unk_21D08` (declared `void (*)(void*)` but holding `s32 (*)(FieldEventManager*)`). Renaming before this bakes the bug into the name.
1. `func_ov030_020a7b50` -> `FldEvent_RunOpcode` - forces the opcode vocabulary, which then names ~half the helpers in section 3.
2. The bust-shot-slot family (`a6960` -> `GetBustShotSlotPtr`, `a69ac` -> `PollBustShotSlot`, `a7550` -> `IsAnyBustShotSlotAlive`) - pins down `unk_21BBC` / `unk_21CD0` (to become `bustShotSlotTaskIds` / `activeBustShotSlotPtr`).
3. The window-slot family (`a6a1c`, `a6a68`, `a6ab4` -> `PollWindowSlots`, `a6d34`, `a6d80`, `a6d94`, `a6b6c`/`a6c4c` + wrappers -> `CloseOrFade...`) - pins down `unk_21C48` / `unk_21CD4` (to become `windowSlotTaskIds` / `activeWindowSlotIndex`).
4. `func_ov030_020aa4bc` -> `FldEvent_HandleScriptEnd` - the bridge into `Events.c` (`return 1` = block/retry, `return 0` = done; `Finish` would imply terminal).
5. The chapter initializers (41x `void f(void)` + 1x `void f(mgr)` in `Events.c`, plus `func_ov030_02084980` in `FieldBg.c` for chapter 0) - mechanical once section 2 is confirmed.
6. Only then the script-end handlers and choice callbacks (largest volume, lowest risk).

---

## 1. How the table was produced

1. Every function definition was extracted from `src/Field/Events.c` (243) and `src/Field/EventManager.c` (51 unnamed, plus 6 already-named `FldEvent_*`).
2. Each was classified by **body shape**, not by name:

   - **script-end** - the function appears in `data_ov030_020da618[44]` (`Events.c:13135`)
   - **chapter-init** - `void f(void)` (41x) or `void f(mgr)` (1x: `func_ov030_020a2208`) writing `gSaveData.currentStoryEvent` / `fieldPos` / `unk_3124` directly or via `func_ov030_020ae96c`. A 43rd init, `func_ov030_02084980`, lives in `FieldBg.c:743` and is chapter 0's (see 4.2) - it fell outside the `Events.c`-only census.
   - **trigger** - `s32 f(...)` whose body contains `FldEvent_BeginScript`. NOTE: several are currently prototyped `s32 f(s32 arg0)` but forward `arg0` as `FieldEventManager*` - the prototype is wrong, fix to `(FieldEventManager*)` before renaming.
   - **choice** - `s32 f(...)` whose body is a `switch` on `unk_2176C[unk_2177C]` or `gSaveData.unk_24C8[gSaveData.unk_24C4]`
   - **trade choice** - `s32 f(mgr)` whose body is a `switch` on `unk_21D1C` (post-script overlay choice, driven via `unk_21D24` when `unk_21D18 == 5`)
   - **event-selector** - `s32 f(void)` returning an event id, gated on `gSaveData.unk_2458` + `FldGate_*`
   - **stub** - body contains no `gSaveData` access at all
3. Script-end handlers were taken directly from the dispatch table (entry *i* is chapter *i*, because `gSaveData.chapter` indexes it - `EventManager.c:1684`, clamped `chapter >= 44 -> 0` at `:1681`). Every other function was bracketed by address between consecutive script-end handlers. That bracketing only holds for `Events.c` bodies: chapter 43's handler is in `FieldBg.c` (`0x02084978`, below the first `Events.c` function `0x02084a2c`), so the 24 functions after PostGame22's handler have no *closing* bracket - they are marked `?` in section 4.6 but are the natural chapter 43 block. The same layout rule places `func_ov030_02084980` (`FieldBg.c`) inside chapter 0's block, i.e. it is chapter 0's initializer (see 4.2).
4. Reference counts were computed against `src/**` and `include/**` (worktrees under `.claude/worktrees/` and `permtmp/` excluded). The headers change nothing in practice: no `Events.c` function is declared in any header, and all 57 `EventManager.c` functions are referenced from `src/`.

**Confidence legend:** `high` = derived from unambiguous call-site / structural evidence; `med` = classification is solid, the specific name is a reasonable guess; `low` = speculative, verify before applying.

---

## 2. Chapter model (verify this before applying section 4)

The 44-entry table is indexed by `gSaveData.chapter`. `include/Save/MainData.h:74` documents that field as:

> *0-6 Shiki's week, 7-13 Joshua's, 14-20 Beat's, 21+ post-game*


The labels below follow `MainData.h:74` and hold for chapters 0-20; `PostGame01`..`PostGame23` are placeholders (see the note at the end of this section - chapter 21 is very likely Another Day). Within most chapters the source layout is init -> trigger -> choice -> trade -> script-end, and a chapter's block sits *before* its script-end handler. Address ordering alone does **not** validate the mapping at the tail: chapter 43's handler `func_ov030_02084978` is at `0x02084978`, below the first `Events.c` function (`0x02084a2c`), so it cannot bracket its block by address.


| chapter | label (proposed) | script-end handler |
|---|---|---|
| 0 | ShikiDay1 | `func_ov030_02084eb4` |
| 1 | ShikiDay2 | `func_ov030_02085cf8` |
| 2 | ShikiDay3 | `func_ov030_020876c8` |
| 3 | ShikiDay4 | `func_ov030_02089300` |
| 4 | ShikiDay5 | `func_ov030_0208aaf4` |
| 5 | ShikiDay6 | `func_ov030_0208c8a0` |
| 6 | ShikiDay7 | `func_ov030_0208ddfc` |
| 7 | JoshuaDay1 | `func_ov030_0208ec3c` |
| 8 | JoshuaDay2 | `func_ov030_0208faa4` |
| 9 | JoshuaDay3 | `func_ov030_02091bc8` |
| 10 | JoshuaDay4 | `func_ov030_02093db8` |
| 11 | JoshuaDay5 | `func_ov030_0209585c` |
| 12 | JoshuaDay6 | `func_ov030_0209716c` |
| 13 | JoshuaDay7 | `func_ov030_02097f4c` |
| 14 | BeatDay1 | `func_ov030_02098a30` |
| 15 | BeatDay2 | `func_ov030_02099514` |
| 16 | BeatDay3 | `func_ov030_0209a564` |
| 17 | BeatDay4 | `func_ov030_0209bacc` |
| 18 | BeatDay5 | `func_ov030_0209c680` |
| 19 | BeatDay6 | `func_ov030_0209cff0` |
| 20 | BeatDay7 | `func_ov030_0209dafc` |
| 21 | PostGame01 (**likely AnotherDay1** - see note below) | `func_ov030_0209f0e8` |
| 22 | PostGame02 | `func_ov030_020a206c` |
| 23 | PostGame03 | `func_ov030_020a21a4` |
| 24 | PostGame04 | `func_ov030_020a2628` |
| 25 | PostGame05 | `func_ov030_020a2c20` |
| 26 | PostGame06 | `func_ov030_020a2de8` |
| 27 | PostGame07 | `func_ov030_020a306c` |
| 28 | PostGame08 | `func_ov030_020a30d4` |
| 29 | PostGame09 | `func_ov030_020a317c` |
| 30 | PostGame10 | `func_ov030_020a31bc` |
| 31 | PostGame11 | `func_ov030_020a333c` |
| 32 | PostGame12 | `func_ov030_020a3718` |
| 33 | PostGame13 | `func_ov030_020a3750` |
| 34 | PostGame14 | `func_ov030_020a3788` |
| 35 | PostGame15 | `func_ov030_020a37c0` |
| 36 | PostGame16 | `func_ov030_020a37f8` |
| 37 | PostGame17 | `func_ov030_020a3830` |
| 38 | PostGame18 | `func_ov030_020a3858` |
| 39 | PostGame19 | `func_ov030_020a3890` |
| 40 | PostGame20 | `func_ov030_020a38c8` |
| 41 | PostGame21 | `func_ov030_020a3900` |
| 42 | PostGame22 | `func_ov030_020a3938` |
| 43 | PostGame23 | `func_ov030_02084978` (**defined in `FieldBg.c`, not `Events.c`**) |

> **Reviewers please check:** the labels for chapters 22-43 are placeholders (`PostGame02`..`PostGame23`). There is
> in-tree evidence for chapter 21: every `EVENT_ANOTHERDAY_*` case (`Events.c:10614`, `11128`-`11482`) lies inside
> `func_ov030_0209f0e8` = `data_ov030_020da618[21]`, so chapter 21 is very likely **Another Day**, not "PostGame01".
> The rest still need names: `func_ov030_020a2f74` (chapter 27's init) sets `currentStoryEvent = 0x8A`, the same id
> as the Shiki Day 3 init `func_ov030_02086700` (`EVENTFLAG_NOBGM | 0x8A`), which is consistent with a secret-report
> style chapter revisiting a main-story day. Substitute the real names before applying section 4.
>
> Chapter 43's handler is a `return 0;` stub (`func_ov030_02084978`, `FieldBg.c:738`); its block in `Events.c` is the
> 24-function tail of section 4.6 (rows 19-42) - those follow chapter 42's handler and have no closing bracket, so
> they are the natural chapter 43 block, not unassigned functions. Chapter 0's init is `func_ov030_02084980`
> (`FieldBg.c:743`): symbol sizes make it exactly contiguous with chapter 0's block (`func_ov030_02084978` size `0x8`,
> `func_ov030_02084980` size `0xAC` ending exactly at `func_ov030_02084a2c`, which ends exactly at
> `func_ov030_02084eb4`), it precedes (never follows) its own chapter's script-end handler, and its
> `currentStoryEvent = 0x8056` is `EVENTFLAG_NOBGM | 0x56` = 86 = `EVENT_SHIKI1_BEGIN_GAME` (`Events.h:213`) - the
> natural start of Shiki Day 1.

---

## 3. `EventManager.c` - the script VM

All 57 functions here are referenced from somewhere in `src/`, so renames are pure churn. Suggested order is top-to-bottom by address.

> **Vocabulary caveat:** the `Poll*` prefix is used below with two opposite return conventions - `PollBustShotSlot` returns `1` = done/free, while `PollWindowSlots` and `IsAnyBustShotSlotAlive` return `1` = still busy/block. Keep the two meanings distinct when applying these names.

| # | line | bytes | current | suggested | conf | rationale |
|---|---|---|---|---|---|---|
| 1 | 109 | 80 | `func_ov030_020a66e8` | `FldEvent_ShouldSuppressAutoAdvance` | high | Polarity: both consumers gate on the opposite sense. `FldEvent_AdvanceScript` runs its skip-ahead loop only when this returns **0** (`EventManager.c:124`), and `FldEvent_HandleScriptEnd` runs the post-script overlay machine only when it returns **1** (`:1781`). Returns 1 while `currentStoryEvent` is 5..39 or `unk_21D22 != 0`. A previous `IsFastForwardEligible` read backwards. |
| 2 | 155 | 76 | `func_ov030_020a68a8` | `FldEvent_AllocRingSlot` | med | 280-entry ring counter on unk_22E2C (`arg == -1` -> `id = counter % 280`). Previous `Resource` guess dropped - the allocated thing varies (object ids, slot ids). |
| 3 | 166 | 108 | `func_ov030_020a68f4` | (leave unnamed) | low | Body is entirely commented out; the `// Not yet implemented` comment sits directly above it at `EventManager.c:165`. Leave unnamed until implemented. |
| 4 | 210 | 76 | `func_ov030_020a6960` | `FldEvent_GetBustShotSlotPtr` | high | Bust-shot resource index (an index into the 280-entry `data_ov030_020df6e4[]` table - `Fld_BustShot.c:461`) -> `&unk_21BBC[]` slot pointer, cached in unk_21CD0. FIX REQUIRED: return type must become `u32*` and the body must `return eventMgr->unk_21CD0` (it currently has no `return` at all). Most callers cast the result to `u32*`, but `EventManager.c:369` (`== arg1`), `:408` and `:1526` (2nd arg of `EasyTask_GetTaskById`, whose parameter is `u32` - `src/Engine/EasyTask.c:275`) use it as a scalar; `:1583` does `*(u32*)...` for the same operation as `:408`, so those three sites are missing the deref - fix them together. `0x118` (= 280, the table size, i.e. the out-of-table sentinel) selects the base slot; what that slot represents (default/player bust) is unconfirmed. Depends on stubbed `a68f4`. |
| 5 | 220 | 112 | `func_ov030_020a69ac` | `FldEvent_PollBustShotSlot` | high | Poll one bust-shot slot (`TaskPool*`, slot `u32*`, arg); deletes + sets -1 when done, `1` = free/done, `0` = still busy. Previous `WaitOrReleaseSlot` renamed: it never blocks. |
| 6 | 233 | 76 | `func_ov030_020a6a1c` | `FldEvent_ReleaseAllWindowSlots` | high | Deletes all 32 unk_21C48[] entries. |
| 7 | 242 | 76 | `func_ov030_020a6a68` | `FldEvent_ReleaseWindowSlotsUpper` | high | Deletes unk_21C48[16..31] only. |
| 8 | 251 | 184 | `func_ov030_020a6ab4` | `FldEvent_PollWindowSlots` | high | Per-frame poll of all 32 window slots. When `func_ov030_020b0454` (fast-forward/skip held; see row 9) is active it deletes every slot except the active one (`unk_21CD4`) and pokes the active task; returns 1 while any window task is still alive (callers wait on that). Previous `Update` renamed: it polls, never blocks. |
| 9 | 271 | 184 | `func_ov030_020a6b6c` | `FldEvent_CloseOrFadeWindowSlotsSkippingActive` | high | Core: close-or-animate window slots in `[arg2, arg3)`, always skipping the active one. Both cores branch on `func_ov030_020b0454`, which is **not a fader check**: it is true while fast-forward/skip is held (`FieldBattleSetup.c:585`: script active + item `0x2AE` + L/R held), so skipping deletes instantly and otherwise animates (`020b4060` slides `posY`, `020b400c` sets the sprite render-mode bits). The `CloseOrFade` names are retained but the second operation is a slide/mode change, not a screen fade. |
| 10 | 285 | 20 | `func_ov030_020a6c24` | `FldEvent_CloseOrFadeWindowSlotsLowerSkippingActive` | high | Wrapper -> range 0..16, skipping-active variant. |
| 11 | 289 | 20 | `func_ov030_020a6c38` | `FldEvent_CloseOrFadeWindowSlotsUpperSkippingActive` | high | Wrapper -> range 16..32, skipping-active variant. |
| 12 | 293 | 172 | `func_ov030_020a6c4c` | `FldEvent_CloseOrFadeWindowSlotsInRangeIncludingActive` | high | Core: same close-or-fade op over `[arg2, arg3)`, but includes the active slot (no `unk_21CD4` check). That - not the range - is what distinguishes it from row 9, so the variant stays in the name like it does in the wrappers. |
| 13 | 307 | 20 | `func_ov030_020a6cf8` | `FldEvent_CloseOrFadeWindowSlotsAll` | high | Wrapper -> range 0..32, including-active variant. |
| 14 | 311 | 20 | `func_ov030_020a6d0c` | `FldEvent_CloseOrFadeWindowSlotsUpperIncludingActive` | high | Wrapper -> range 16..32, including-active variant. |
| 15 | 315 | 20 | `func_ov030_020a6d20` | `FldEvent_CloseOrFadeWindowSlotsLowerIncludingActive` | high | Wrapper -> range 0..16, including-active variant. |
| 16 | 320 | 76 | `func_ov030_020a6d34` | `FldEvent_AllocWindowSlot` | high | First free index in unk_21C48[from..to); records index in unk_21CD4. FIX REQUIRED: must `return &unk_21C48[i]` (`u32*`) - currently returns the `-1` value, so callers like `020a6f90`, opcode `0x1A-0x22` (`(u32*)6d80(...)`) dereference garbage. The `s32` wrappers `func_ov030_020a6d80`/`func_ov030_020a6d94` must become `u32*` too, and the no-free-slot `return 0` needs a defined value. NONMATCHING. |
| 17 | 330 | 20 | `func_ov030_020a6d80` | `FldEvent_AllocWindowSlotLower` | high | unk_21C48[0..16). |
| 18 | 334 | 20 | `func_ov030_020a6d94` | `FldEvent_AllocWindowSlotUpper` | high | unk_21C48[16..32). |
| 19 | 363 | 20 | `func_ov030_020a6e50` | `FldEvent_IsKeywordLayerVisible` | high | Takes `void`. Layer 8 of the sub display is the keyword-menu overlay (opened by row 34, cleared by `PollKeywordMenu`). `g_DisplaySettings.controls[DISPLAY_SUB].layers & 8`. |
| 20 | 368 | 72 | `func_ov030_020a6e64` | `FldEvent_ClearBustShotStateIfKeywordLayerVisible` | low | If the keyword layer is visible and the slot pointer matches, writes `0` to the bust-shot sprite field `unk_0A.unk_10` (`b3cd4`). The same setter is called with `1` on keyword decide / input advance, so this is a state flag rather than visibility - the earlier "hide" reading is unproven. Contains the scalar use of `a6960`'s return at `EventManager.c:369` (see row 4). NONMATCHING. |
| 21 | 374 | 228 | `func_ov030_020a6eac` | `FldEvent_SpawnBustShot` | med | Creates a bust-shot task for a resource id, or re-poses the existing one (`b3bdc` swaps the sprite config, `b3b70` positions it). Not limited to the talk target. The 3 trailing s16 args (arg4-arg6) are position/anim - unverified. |
| 22 | 390 | 140 | `func_ov030_020a6f90` | `FldEvent_SpawnBustShotWindow` | low | Allocates an upper window slot and spawns a message-window task (`b3d88`, winKind `0x16`, X = the bust shot's `posX`, Y = `0xD0`) for the bust shot referenced by `unk_21D00`. Called from `PollKeywordMenu` right after a keyword is decided. Not a clone of the bust shot, and not a cursor; purpose (name plate?) unconfirmed. |
| 23 | 398 | 396 | `func_ov030_020a701c` | `FldEvent_PollKeywordMenu` | high | Reads the keyword-menu task selection (selecting the same entry twice = decide); on decide it plays the confirm SE, clears layer 8, creates the decide / string-plate tasks and closes the upper windows. Returns 1 only while the menu is idle (selection `-1`). |
| 24 | 434 | 132 | `func_ov030_020a71a8` | `FldEvent_PollKeywordDecideTask` | med | Gate used by `PollAdvanceInput`. Returns 0 while the decide task runs (and fires `unk_21D08(eventMgr)` once it ends, `EventManager.c:439` - the only call site of the `func_ov030_020aa0c8` callback) or while the keyword menu holds a selection / cancel; returns 1 when there is no menu or it is idle. The earlier "waits for the keyword menu to close" had the polarity backwards. |
| 25 | 449 | 88 | `func_ov030_020a722c` | `FldEvent_PollAdvanceInput` | high | After the keyword-decide gate (`a71a8`), returns 1 on touch / X / Y / D-pad / START / B / A, or while skip is held (`:460`). |
| 26 | 466 | 120 | `func_ov030_020a7284` | `FldEvent_CrossfadeBustShotPose` | med | Opcode 0x4F: advances the script once the bust shot's pending flag is consumed (`b3b10`); otherwise builds a second sprite and a cross-fade task (`b39e0`, speed from arg3: 10 / 0x3C / 0 / else). |
| 27 | 480 | 128 | `func_ov030_020a72fc` | `FldEvent_SwapBustShotPose` | med | Opcodes 0x4D/0x4E: advances the script when the bust shot is missing or its swap-pending flag was consumed (`b3c84`); otherwise swaps the sprite config to the given pose index (`b3bdc`). Not a move. |
| 28 | 498 | 236 | `func_ov030_020a737c` | `FldEvent_FadeStepBothDisplays` | low | Selected by `RunFadeStep` when `unk_21CF4 != 0`: waits for the sub display to reach `targetBrightness` (fading both displays), then switches layout, enables BG1, creates the talk-event title task, advances the script and fades back. Direction (black or white) comes from the opcode (0x0-0x4), not from this function, so the old `FadeFromBlack` name was wrong. |
| 29 | 518 | 232 | `func_ov030_020a7468` | `FldEvent_FadeStepMainAndSub` | low | Selected by `RunFadeStep` when `unk_21CF4 == 0`: fades the main display and creates the underground-fade task, waits for the sub display, switches layout and advances. Same shape as row 28 but with the main/sub split; direction again comes from the opcode, so `FadeToBlack` was wrong. |
| 30 | 539 | 112 | `func_ov030_020a7550` | `FldEvent_IsAnyBustShotSlotAlive` | high | Polls all 35 bust-shot slots via `PollBustShotSlot`, then non-zero while any slot is still alive (`1` = busy/block, `0` = drained). Previous `WaitFor` inverted the usual `1 = done` poller convention. |
| 31 | 556 | 24 | `func_ov030_020a75c0` | `FldEvent_ReleaseWindowsAndPollBustShots` | high | Closes all window slots (`6a1c`), then polls the bust-shot slots. FIX REQUIRED: declared `s32` but has no `return` - must `return FldEvent_IsAnyBustShotSlotAlive(...)`. |
| 32 | 561 | 388 | `func_ov030_020a75d8` | `FldEvent_WaitAreaTransition` | med | Opcode 0x53: finishes the guard fade before committing the area change. Caution: case 0x53 passes `gSaveData.unk_24A0[gSaveData.unk_249C]` (the opcode byte, not its operand), which makes the `arg1 == 0x49` branch look unreachable - recheck the operand before trusting the parameter. |
| 33 | 598 | 180 | `func_ov030_020a775c` | `FldEvent_GetItemPlateResIds` | high | itemID -> (plate resID, message resID) per Inventory_GetCategory. |
| 34 | 621 | 128 | `func_ov030_020a7810` | `FldEvent_OpenKeywordOverlay` | med | Keyword-menu overlay, fade-in half: sets `layers \|= 8` and drives the layer-8 blend from (0, 1.0) toward (0.625, 0.5) over 30 frames; its callback `82e70` leaves layer 8 set. Called from opcode 0x36 and `RunScanPrompt`. NONMATCHING. |
| 35 | 640 | 64 | `func_ov030_020a7890` | `FldEvent_FadeOutKeywordOverlay` | med | Fade-out half of the *same* overlay as row 34, not a second overlay: seeds `unk_21AB0`/`unk_21AB4` and pushes `82f10`, which ends with `layers &= ~8; DebugOvlDisp_Pop()` (`FieldMain.c:378-396`). Called from opcode 0x37 and `func_ov030_020a9e48`. |
| 36 | 647 | 88 | `func_ov030_020a78d0` | `FldEvent_RunFadeOpcodeStep` | med | Shared tail of the opcode 0x0-0x4 fade handlers: plays the once-only sequence start, then dispatches on `unk_21CF4` to row 28 (nonzero) or row 29 (zero). |
| 37 | 659 | 108 | `func_ov030_020a7928` | `FldEvent_SetupBrightnessRamp` | med | Opcodes 0x33/0x34 only (not the 0x0-0x4 fade of row 36): captures the current main/sub brightness into `unk_21CDC`/`unk_21CE0`, stores the duration (`unk_21CE8`, min 1) and sets the step `unk_21CD8` to +0x10 / -0x10 / 0 from the operand; opcode 0x35 then steps it. |
| 38 | 678 | 268 | `func_ov030_020a7994` | `FldEvent_WaitForAdvance` | high | Opcode 0x10: spawns the "press to continue" task and waits for input. |
| 39 | 707 | 176 | `func_ov030_020a7aa0` | `FldEvent_HandleTextCompletion` | med | Branches on the text task result; installs the yes/no or choice sub-script from data_ov030_020d9586/88/8a. |
| 40 | 727 | 8452 | `func_ov030_020a7b50` | `FldEvent_RunOpcode` | high | 8452-byte opcode dispatcher; the largest function in EventManager.c (ov030's largest is 4.1's 11712-byte `func_ov030_0209f0e8`). Driven from FieldEncounter.c:203/607 and self-recursive for the blocking opcodes. |
| 41 | 1486 | 184 | `func_ov030_020a9d00` | `FldEvent_BeginScriptAfterFade` | high | Deferred script switch: drains bust-shot slots, waits for fades, then FldEvent_SetCurrent. |
| 42 | 1512 | 144 | `func_ov030_020a9db8` | `FldEvent_BeginScriptWithOverlays` | med | BeginScriptAfterFade + pushes the overlay stack keyed on `unk_21D22`/`unk_21D18`. Renamed off `StartCutscene` to match section 6's conclusion that these are field talk/keyword/scan overlays, not cinematics. |
| 43 | 1525 | 268 | `func_ov030_020a9e48` | `FldEvent_ShowScanPrompt` | med | Builds the "scan this object?" prompt tasks (`b44b8` + `c3654`, SE 0x122) and fades the keyword overlay out (calls row 35). Renamed off `ShowKeywordPrompt`: it closes the keyword overlay rather than showing it. |
| 44 | 1544 | 372 | `func_ov030_020a9f54` | `FldEvent_CreateChoiceWindow` | high | Already documented at Events.h:405. Builds the N-way choice window and invokes the callback. NONMATCHING. |
| 45 | 1575 | 712 | `func_ov030_020aa0c8` | `FldEvent_RunScanPrompt` | med | The scan / touch-to-inspect branch: prompt -> keyword menu -> accept/reject (imprint). Takes `(mgr, resourceId, acceptEvent, rejectEvent, cb)` - `arg1` is stored to `unk_21D00` (resource id, cf. `InitState: 280`), `arg2/arg3` become `SetCurrent` targets, `arg4` becomes `unk_21D08`. |
| 46 | 1630 | 300 | `func_ov030_020aa390` | `FldEvent_OnPostScriptChoice` | low | `unk_21D1C -> SetCurrent(6..14)` / overlay mapping, run when `unk_21D18 == 5` in `HandleScriptEnd`. Previous `DefaultChoiceCallback` dropped: it is only one of seven `unk_21D24` values (1 here + 6 in `Events.c`), installed for `currentStoryEvent == 0x36`. |
| 47 | 1677 | 1336 | `func_ov030_020aa4bc` | `FldEvent_HandleScriptEnd` | high | Opcode 0x55 tail only (case 0x56 does not call it; it is referenced nowhere else outside this file). Dispatches data_ov030_020da618[chapter], then reward/overlay state machine (`unk_21D18` 0..5, `unk_21D24` callback). `return 1` = block/retry, `0` = done. |
| 48 | 1815 | 228 | `func_ov030_020aa9f4` | `FldEvent_AddKeyword` | med | Adds a keyword id to `gSaveData.unk_24C8[16]` (deduplicated; `unk_24C8[unk_24C4]` is the entry the keyword menu selected) and to the pending-notification arrays `unk_21718..21748` (capped at 8). NONMATCHING. |
| 49 | 1840 | 32 | `func_ov030_020aaad8` | `FldEvent_GetTopBrandId` | med | Takes `void`, returns `u8`. Top-ranked brand id: `brandTrends[020af350()].ranking[0]`. `Id` added: it returns an index, not a struct. |
| 50 | 1844 | 120 | `func_ov030_020aaaf8` | `FldEvent_ShouldGrantItem` | high | Gate shared by the three Grant* helpers. Returns 1 (grant) unconditionally while `ITEM_STICKER_GAME_CLEARED` is not held; otherwise asks `func_02023be8(itemID, mode)`: 0 = no, `10000000` = grant only if the item is not already owned, anything else = grant. |
| 51 | 1876 | 92 | `func_ov030_020aac28` | (leave unnamed) | low | Third grant flavour: `Inventory_AddItem(id, 1)`. Do not encode the unresolved mode in a name: `GrantItem`/`GrantPin` are named by category and this one's category is unknown (cf. `GrantItem: mode 0`, `GrantPin: mode 2`). Also already declared at `Events.h:398`, so any rename touches the header. |

Already named in this file - no change proposed:

| name | note |
|---|---|
| `FldEvent_SetCurrent` | Selects the script for an event id (handles the >=2207 "reward script" bank). NONMATCHING. |
| `FldEvent_AdvanceScript` | Advances the PC by the opcode's operand width. NONMATCHING. |
| `FldEvent_InitState` | Resets the object / window slot arrays. |
| `FldEvent_BeginScript` | Public entry point: init state, strip EVENTFLAG_*, FldEvent_SetCurrent. NONMATCHING. |
| `FldEvent_GrantItem` | - |
| `FldEvent_GrantPin` | - |

---

## 4. `Events.c` - story content

Proposed naming convention:

```c
void  FldEvent_InitChapter_<Char>Day<D>(void)       // writes gSaveData; runs when a chapter starts (PostGame04's takes (mgr); chapter 0's lives in FieldBg.c - see 4.2)
s32   FldEvent_CheckTriggers_<Char>Day<D>(mgr)      // per-area trigger poll; calls FldEvent_BeginScript, 1 = dispatched/block, 0 = nothing. `TryDispatchTriggers` is an acceptable synonym. Do not apply until the `s32` prototypes are fixed to `(FieldEventManager*)`.
s32   FldEvent_OnScriptEnd_<Char>Day<D>(mgr)        // data_ov030_020da618[chapter] entry
s32   FldEvent_OnChoice_<Char>Day<D>_<NN>(mgr)      // callback passed to `func_ov030_020a9f54` (`FldEvent_CreateChoiceWindow`). The 12 passed to `func_ov030_020aa0c8` (`FldEvent_RunScanPrompt`) are named `FldEvent_OnKeywordSelected_<Char>Day<D>_<NN>` instead (they switch on the selected keyword, `unk_24C8[unk_24C4]`). Keep the positional `_NN`: the window's message id is a *caller-side* argument, is not unique within a chapter, and the 12 `aa0c8` callbacks have none (see 4.4).
s32   FldEvent_OnChoiceTrade_<Char>Day<D>_<NN>(mgr) // post-script choice callback stored in `unk_21D24`, run from `unk_21D18 == 5` (see 4.5)
s32   FldEvent_SelectNextEvent_<Char>Day<D>_<Area>(void) // area + gate gated event-id selector; global `_NN` below is placeholder only
// Do NOT name `return 0;` stubs. Leave them `func_ov030_*` until a caller is found.
// The `s32 f(s32)` prototype bug is not limited to the triggers: it also affects the choice callback
// `func_ov030_02086f10` and the selector `func_ov030_020a36f0`. Fix all of them to `(FieldEventManager*)` first.
// `func_ov030_020aa0c8`'s callback parameter and `unk_21D08` are `void (*)(void*)` but hold
// `s32 (*)(FieldEventManager*)` - fix those with the same pass (see 0.).
```

### 4.1 Script-end handlers

The 43 `Events.c` members of `data_ov030_020da618[]`. These are the highest-value names in the file - the switch arms are the story.

> Row 22 (`func_ov030_0209f0e8`, `table[21]`) is the only handler that switches on `EVENT_ANOTHERDAY_*`
> (`Events.c:10614`, `11128`-`11482`), so chapter 21 is very likely **Another Day** - `PostGame01` in this table is a
> placeholder pending confirmation.

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 121 | 1484 | ShikiDay1 | `func_ov030_02084eb4` | `FldEvent_OnScriptEnd_ShikiDay1` |
| 2 | 481 | 2568 | ShikiDay2 | `func_ov030_02085cf8` | `FldEvent_OnScriptEnd_ShikiDay2` |
| 3 | 1135 | 5012 | ShikiDay3 | `func_ov030_020876c8` | `FldEvent_OnScriptEnd_ShikiDay3` |
| 4 | 1813 | 1856 | ShikiDay4 | `func_ov030_02089300` | `FldEvent_OnScriptEnd_ShikiDay4` |
| 5 | 2468 | 3464 | ShikiDay5 | `func_ov030_0208aaf4` | `FldEvent_OnScriptEnd_ShikiDay5` |
| 6 | 3162 | 3688 | ShikiDay6 | `func_ov030_0208c8a0` | `FldEvent_OnScriptEnd_ShikiDay6` |
| 7 | 3688 | 2280 | ShikiDay7 | `func_ov030_0208ddfc` | `FldEvent_OnScriptEnd_ShikiDay7` |
| 8 | 4036 | 1316 | JoshuaDay1 | `func_ov030_0208ec3c` | `FldEvent_OnScriptEnd_JoshuaDay1` |
| 9 | 4371 | 2424 | JoshuaDay2 | `func_ov030_0208faa4` | `FldEvent_OnScriptEnd_JoshuaDay2` |
| 10 | 5204 | 4228 | JoshuaDay3 | `func_ov030_02091bc8` | `FldEvent_OnScriptEnd_JoshuaDay3` |
| 11 | 6012 | 3408 | JoshuaDay4 | `func_ov030_02093db8` | `FldEvent_OnScriptEnd_JoshuaDay4` |
| 12 | 6669 | 3892 | JoshuaDay5 | `func_ov030_0209585c` | `FldEvent_OnScriptEnd_JoshuaDay5` |
| 13 | 7268 | 2148 | JoshuaDay6 | `func_ov030_0209716c` | `FldEvent_OnScriptEnd_JoshuaDay6` |
| 14 | 7606 | 1460 | JoshuaDay7 | `func_ov030_02097f4c` | `FldEvent_OnScriptEnd_JoshuaDay7` |
| 15 | 7878 | 1504 | BeatDay1 | `func_ov030_02098a30` | `FldEvent_OnScriptEnd_BeatDay1` |
| 16 | 8147 | 836 | BeatDay2 | `func_ov030_02099514` | `FldEvent_OnScriptEnd_BeatDay2` |
| 17 | 8570 | 2540 | BeatDay3 | `func_ov030_0209a564` | `FldEvent_OnScriptEnd_BeatDay3` |
| 18 | 9023 | 1576 | BeatDay4 | `func_ov030_0209bacc` | `FldEvent_OnScriptEnd_BeatDay4` |
| 19 | 9322 | 932 | BeatDay5 | `func_ov030_0209c680` | `FldEvent_OnScriptEnd_BeatDay5` |
| 20 | 9526 | 488 | BeatDay6 | `func_ov030_0209cff0` | `FldEvent_OnScriptEnd_BeatDay6` |
| 21 | 9777 | 1648 | BeatDay7 | `func_ov030_0209dafc` | `FldEvent_OnScriptEnd_BeatDay7` |
| 22 | 10295 | 11712 | PostGame01 | `func_ov030_0209f0e8` | `FldEvent_OnScriptEnd_PostGame01` |
| 23 | 11544 | 140 | PostGame02 | `func_ov030_020a206c` | `FldEvent_OnScriptEnd_PostGame02` |
| 24 | 11579 | 100 | PostGame03 | `func_ov030_020a21a4` | `FldEvent_OnScriptEnd_PostGame03` |
| 25 | 11701 | 1052 | PostGame04 | `func_ov030_020a2628` | `FldEvent_OnScriptEnd_PostGame04` |
| 26 | 11863 | 236 | PostGame05 | `func_ov030_020a2c20` | `FldEvent_OnScriptEnd_PostGame05` |
| 27 | 11913 | 396 | PostGame06 | `func_ov030_020a2de8` | `FldEvent_OnScriptEnd_PostGame06` |
| 28 | 11974 | 56 | PostGame07 | `func_ov030_020a306c` | `FldEvent_OnScriptEnd_PostGame07` |
| 29 | 11991 | 136 | PostGame08 | `func_ov030_020a30d4` | `FldEvent_OnScriptEnd_PostGame08` |
| 30 | 12019 | 8 | PostGame09 | `func_ov030_020a317c` | `FldEvent_OnScriptEnd_PostGame09` |
| 31 | 12034 | 8 | PostGame10 | `func_ov030_020a31bc` | `FldEvent_OnScriptEnd_PostGame10` |
| 32 | 12068 | 924 | PostGame11 | `func_ov030_020a333c` | `FldEvent_OnScriptEnd_PostGame11` |
| 33 | 12192 | 8 | PostGame12 | `func_ov030_020a3718` | `FldEvent_OnScriptEnd_PostGame12` |
| 34 | 12206 | 8 | PostGame13 | `func_ov030_020a3750` | `FldEvent_OnScriptEnd_PostGame13` |
| 35 | 12220 | 8 | PostGame14 | `func_ov030_020a3788` | `FldEvent_OnScriptEnd_PostGame14` |
| 36 | 12234 | 8 | PostGame15 | `func_ov030_020a37c0` | `FldEvent_OnScriptEnd_PostGame15` |
| 37 | 12248 | 8 | PostGame16 | `func_ov030_020a37f8` | `FldEvent_OnScriptEnd_PostGame16` |
| 38 | 12262 | 8 | PostGame17 | `func_ov030_020a3830` | `FldEvent_OnScriptEnd_PostGame17` |
| 39 | 12275 | 8 | PostGame18 | `func_ov030_020a3858` | `FldEvent_OnScriptEnd_PostGame18` |
| 40 | 12289 | 8 | PostGame19 | `func_ov030_020a3890` | `FldEvent_OnScriptEnd_PostGame19` |
| 41 | 12303 | 8 | PostGame20 | `func_ov030_020a38c8` | `FldEvent_OnScriptEnd_PostGame20` |
| 42 | 12317 | 8 | PostGame21 | `func_ov030_020a3900` | `FldEvent_OnScriptEnd_PostGame21` |
| 43 | 12331 | 8 | PostGame22 | `func_ov030_020a3938` | `FldEvent_OnScriptEnd_PostGame22` |

### 4.2 Chapter initializers

All 42 are currently orphaned (no C caller). They write `gSaveData.currentStoryEvent`, `fieldPos`, `unk_3124`, partner, and story gates - some directly, some via `func_ov030_020ae96c`. 41 are `void f(void)`; `func_ov030_020a2208` (PostGame04) is `void f(FieldEventManager*)`.

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 304 | 136 | ShikiDay2 | `func_ov030_02085480` | `FldEvent_InitChapter_ShikiDay2` |
| 2 | 722 | 260 | ShikiDay3 | `func_ov030_02086700` | `FldEvent_InitChapter_ShikiDay3` |
| 3 | 1597 | 260 | ShikiDay4 | `func_ov030_02088a5c` | `FldEvent_InitChapter_ShikiDay4` |
| 4 | 1989 | 232 | ShikiDay5 | `func_ov030_02089a40` | `FldEvent_InitChapter_ShikiDay5` |
| 5 | 2769 | 256 | ShikiDay6 | `func_ov030_0208b87c` | `FldEvent_InitChapter_ShikiDay6` |
| 6 | 3506 | 260 | ShikiDay7 | `func_ov030_0208d708` | `FldEvent_InitChapter_ShikiDay7` |
| 7 | 3903 | 256 | JoshuaDay1 | `func_ov030_0208e6e4` | `FldEvent_InitChapter_JoshuaDay1` |
| 8 | 4169 | 296 | JoshuaDay2 | `func_ov030_0208f160` | `FldEvent_InitChapter_JoshuaDay2` |
| 9 | 4640 | 260 | JoshuaDay3 | `func_ov030_0209041c` | `FldEvent_InitChapter_JoshuaDay3` |
| 10 | 5585 | 260 | JoshuaDay4 | `func_ov030_02092c4c` | `FldEvent_InitChapter_JoshuaDay4` |
| 11 | 6339 | 244 | JoshuaDay5 | `func_ov030_02094b08` | `FldEvent_InitChapter_JoshuaDay5` |
| 12 | 7035 | 248 | JoshuaDay6 | `func_ov030_02096790` | `FldEvent_InitChapter_JoshuaDay6` |
| 13 | 7479 | 252 | JoshuaDay7 | `func_ov030_020979d0` | `FldEvent_InitChapter_JoshuaDay7` |
| 14 | 7754 | 260 | BeatDay1 | `func_ov030_02098500` | `FldEvent_InitChapter_BeatDay1` |
| 15 | 8036 | 252 | BeatDay2 | `func_ov030_02099010` | `FldEvent_InitChapter_BeatDay2` |
| 16 | 8235 | 312 | BeatDay3 | `func_ov030_02099858` | `FldEvent_InitChapter_BeatDay3` |
| 17 | 8802 | 264 | BeatDay4 | `func_ov030_0209af50` | `FldEvent_InitChapter_BeatDay4` |
| 18 | 9194 | 276 | BeatDay5 | `func_ov030_0209c0f4` | `FldEvent_InitChapter_BeatDay5` |
| 19 | 9416 | 332 | BeatDay6 | `func_ov030_0209ca24` | `FldEvent_InitChapter_BeatDay6` |
| 20 | 9578 | 320 | BeatDay7 | `func_ov030_0209d1d8` | `FldEvent_InitChapter_BeatDay7` |
| 21 | 9944 | 424 | PostGame01 | `func_ov030_0209e16c` | `FldEvent_InitChapter_PostGame01` |
| 22 | 11495 | 40 | PostGame02 | `func_ov030_020a1ea8` | `FldEvent_InitChapter_PostGame02` |
| 23 | 11559 | 40 | PostGame03 | `func_ov030_020a20f8` | `FldEvent_InitChapter_PostGame03` |
| 24 | 11586 | 96 | PostGame04 | `func_ov030_020a2208` | `FldEvent_InitChapter_PostGame04` |
| 25 | 11799 | 64 | PostGame05 | `func_ov030_020a2a44` | `FldEvent_InitChapter_PostGame05` |
| 26 | 11882 | 72 | PostGame06 | `func_ov030_020a2d0c` | `FldEvent_InitChapter_PostGame06` |
| 27 | 11948 | 80 | PostGame07 | `func_ov030_020a2f74` | `FldEvent_InitChapter_PostGame07` |
| 28 | 11981 | 40 | PostGame08 | `func_ov030_020a30a4` | `FldEvent_InitChapter_PostGame08` |
| 29 | 12010 | 24 | PostGame09 | `func_ov030_020a315c` | `FldEvent_InitChapter_PostGame09` |
| 30 | 12023 | 48 | PostGame10 | `func_ov030_020a3184` | `FldEvent_InitChapter_PostGame10` |
| 31 | 12038 | 88 | PostGame11 | `func_ov030_020a31c4` | `FldEvent_InitChapter_PostGame11` |
| 32 | 12180 | 24 | PostGame12 | `func_ov030_020a36d8` | `FldEvent_InitChapter_PostGame12` |
| 33 | 12196 | 40 | PostGame13 | `func_ov030_020a3720` | `FldEvent_InitChapter_PostGame13` |
| 34 | 12210 | 40 | PostGame14 | `func_ov030_020a3758` | `FldEvent_InitChapter_PostGame14` |
| 35 | 12224 | 40 | PostGame15 | `func_ov030_020a3790` | `FldEvent_InitChapter_PostGame15` |
| 36 | 12238 | 40 | PostGame16 | `func_ov030_020a37c8` | `FldEvent_InitChapter_PostGame16` |
| 37 | 12252 | 40 | PostGame17 | `func_ov030_020a3800` | `FldEvent_InitChapter_PostGame17` |
| 38 | 12266 | 24 | PostGame18 | `func_ov030_020a3838` | `FldEvent_InitChapter_PostGame18` |
| 39 | 12279 | 40 | PostGame19 | `func_ov030_020a3860` | `FldEvent_InitChapter_PostGame19` |
| 40 | 12293 | 40 | PostGame20 | `func_ov030_020a3898` | `FldEvent_InitChapter_PostGame20` |
| 41 | 12307 | 40 | PostGame21 | `func_ov030_020a38d0` | `FldEvent_InitChapter_PostGame21` |
| 42 | 12321 | 40 | PostGame22 | `func_ov030_020a3908` | `FldEvent_InitChapter_PostGame22` |

A 43rd init sits outside this file and was missed by the `Events.c`-only census:

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 43 | `FieldBg.c:743` | 172 | ShikiDay1 | `func_ov030_02084980` | `FldEvent_InitChapter_ShikiDay1` |

Evidence (high confidence): symbol sizes make it exactly contiguous with chapter 0's block (`func_ov030_02084978` size `0x8`, `func_ov030_02084980` size `0xAC` ending exactly at `func_ov030_02084a2c`, which ends exactly at chapter 0's handler `func_ov030_02084eb4`); under the init -> trigger -> ... -> script-end layout it precedes (never follows) its own chapter's script-end handler, so it cannot be chapter 43's; and its `currentStoryEvent = 0x8056` is `EVENTFLAG_NOBGM | 0x56` = 86 = `EVENT_SHIKI1_BEGIN_GAME` (`Events.h:213`), the natural start of Shiki Day 1. It also writes `unk_3124 = 2` and `activePartner = 0xFF` (`FieldBg.c:757,759`).

Chapter 43 / PostGame23 still has no init in `Events.c`; its block is the 24-function tail of section 4.6 (rows 19-42), which is all `s32 f(void)` selectors.

### 4.3 Area trigger checkers

All 26 are currently orphaned. Each is an `if (gSaveData.unk_2458 == AREA)` ladder that sets a FldGate and calls `FldEvent_BeginScript`. Fix the `s32 f(s32)` prototypes to `s32 f(FieldEventManager*)` before renaming; suggested `TryDispatchTriggers_` (or keep `CheckTriggers_`) below.

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 31 | 1160 | ShikiDay1 | `func_ov030_02084a2c` | `FldEvent_CheckTriggers_ShikiDay1` |
| 2 | 318 | 1548 | ShikiDay2 | `func_ov030_02085508` | `FldEvent_CheckTriggers_ShikiDay2` |
| 3 | 757 | 1804 | ShikiDay3 | `func_ov030_02086804` | `FldEvent_CheckTriggers_ShikiDay3` |
| 4 | 1633 | 1376 | ShikiDay4 | `func_ov030_02088b60` | `FldEvent_CheckTriggers_ShikiDay4` |
| 5 | 2021 | 1280 | ShikiDay5 | `func_ov030_02089b28` | `FldEvent_CheckTriggers_ShikiDay5` |
| 6 | 2807 | 2652 | ShikiDay6 | `func_ov030_0208b97c` | `FldEvent_CheckTriggers_ShikiDay6` |
| 7 | 3546 | 972 | ShikiDay7 | `func_ov030_0208d80c` | `FldEvent_CheckTriggers_ShikiDay7` |
| 8 | 3937 | 860 | JoshuaDay1 | `func_ov030_0208e7e4` | `FldEvent_CheckTriggers_JoshuaDay1` |
| 9 | 4210 | 1952 | JoshuaDay2 | `func_ov030_0208f288` | `FldEvent_CheckTriggers_JoshuaDay2` |
| 10 | 4680 | 3932 | JoshuaDay3 | `func_ov030_02090520` | `FldEvent_CheckTriggers_JoshuaDay3` |
| 11 | 5623 | 2408 | JoshuaDay4 | `func_ov030_02092d50` | `FldEvent_CheckTriggers_JoshuaDay4` |
| 12 | 6373 | 1896 | JoshuaDay5 | `func_ov030_02094bfc` | `FldEvent_CheckTriggers_JoshuaDay5` |
| 13 | 7072 | 1700 | JoshuaDay6 | `func_ov030_02096888` | `FldEvent_CheckTriggers_JoshuaDay6` |
| 14 | 7517 | 1152 | JoshuaDay7 | `func_ov030_02097acc` | `FldEvent_CheckTriggers_JoshuaDay7` |
| 15 | 7790 | 812 | BeatDay1 | `func_ov030_02098604` | `FldEvent_CheckTriggers_BeatDay1` |
| 16 | 8074 | 1032 | BeatDay2 | `func_ov030_0209910c` | `FldEvent_CheckTriggers_BeatDay2` |
| 17 | 8282 | 1532 | BeatDay3 | `func_ov030_02099990` | `FldEvent_CheckTriggers_BeatDay3` |
| 18 | 8837 | 1740 | BeatDay4 | `func_ov030_0209b058` | `FldEvent_CheckTriggers_BeatDay4` |
| 19 | 9234 | 884 | BeatDay5 | `func_ov030_0209c208` | `FldEvent_CheckTriggers_BeatDay5` |
| 20 | 9458 | 1152 | BeatDay6 | `func_ov030_0209cb70` | `FldEvent_CheckTriggers_BeatDay6` |
| 21 | 9619 | 1628 | BeatDay7 | `func_ov030_0209d318` | `FldEvent_CheckTriggers_BeatDay7` |
| 22 | 10005 | 3280 | PostGame01 | `func_ov030_0209e314` | `FldEvent_CheckTriggers_PostGame01` |
| 23 | 11501 | 268 | PostGame02 | `func_ov030_020a1ed0` | `FldEvent_CheckTriggers_PostGame02` |
| 24 | 11565 | 132 | PostGame03 | `func_ov030_020a2120` | `FldEvent_CheckTriggers_PostGame03` |
| 25 | 11597 | 420 | PostGame04 | `func_ov030_020a2268` | `FldEvent_CheckTriggers_PostGame04` |
| 26 | 12048 | 288 | PostGame11 | `func_ov030_020a321c` | `FldEvent_CheckTriggers_PostGame11` |

### 4.4 Choice-window callbacks

Referenced. The `_NN` below is positional. An earlier draft proposed replacing it with `_m<MsgId>` (the choice-window message id); that does not work:

- the ids span `0x35EE..0x36EA`, not `0x35xx`;
- `mesNo` is a *caller-side* argument (2nd argument to `func_ov030_020a9f54`), while the callback body switches on the *option* ids (`Events.c:417`, `:1741`), so the id is not visible in the function being named;
- `mesNo` is not unique within a chapter - 5 colliding groups: `0x36D4` (BeatDay4 x3), `0x3650` (JoshuaDay3 x2), `0x366D` (JoshuaDay4 x3), `0x3604` (PostGame04 x2), `0x361A` (ShikiDay4 x3) - so `_m<MsgId>` alone would produce duplicate identifiers.

Keep `_NN` (optionally disambiguated by the option ids each callback switches on); the real names should still come from what each menu asks. The 12 scan-prompt callbacks are already renamed `FldEvent_OnKeywordSelected_<Chapter>_<NN>` in the table below (they switch on the selected keyword, `unk_24C8[unk_24C4]`; see section 8 for the Keyword terminology). 72 of these are passed to `func_ov030_020a9f54` (= `FldEvent_CreateChoiceWindow`); the other 12 (`func_ov030_02086f10`, `86fdc`, `870ec`, `8a0a0`, `8a120`, `8c3d8`, `8c480`, `8c528`, `8c5d0`, `91a8c`, `a1fdc`, `a240c`) are passed to `func_ov030_020aa0c8` (= `FldEvent_RunScanPrompt`) instead - the scan / touch-to-inspect prompt, whose *callbacks* switch on `gSaveData.unk_24C8[gSaveData.unk_24C4]` (verified disjoint, no overlap). Those 12 ultimately run from `func_ov030_020a71a8` via `unk_21D08` (`EventManager.c:439`).

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 416 | 272 | ShikiDay2 | `func_ov030_02085b14` | `FldEvent_OnChoice_ShikiDay2_01` |
| 2 | 453 | 212 | ShikiDay2 | `func_ov030_02085c24` | `FldEvent_OnChoice_ShikiDay2_02` |
| 3 | 892 | 204 | ShikiDay3 | `func_ov030_02086f10` | `FldEvent_OnKeywordSelected_ShikiDay3_01` |
| 4 | 917 | 272 | ShikiDay3 | `func_ov030_02086fdc` | `FldEvent_OnKeywordSelected_ShikiDay3_02` |
| 5 | 950 | 332 | ShikiDay3 | `func_ov030_020870ec` | `FldEvent_OnKeywordSelected_ShikiDay3_03` |
| 6 | 990 | 176 | ShikiDay3 | `func_ov030_02087238` | `FldEvent_OnChoice_ShikiDay3_04` |
| 7 | 1012 | 120 | ShikiDay3 | `func_ov030_020872e8` | `FldEvent_OnChoice_ShikiDay3_05` |
| 8 | 1027 | 180 | ShikiDay3 | `func_ov030_02087360` | `FldEvent_OnChoice_ShikiDay3_06` |
| 9 | 1049 | 232 | ShikiDay3 | `func_ov030_02087414` | `FldEvent_OnChoice_ShikiDay3_07` |
| 10 | 1079 | 120 | ShikiDay3 | `func_ov030_020874fc` | `FldEvent_OnChoice_ShikiDay3_08` |
| 11 | 1094 | 120 | ShikiDay3 | `func_ov030_02087574` | `FldEvent_OnChoice_ShikiDay3_09` |
| 12 | 1109 | 220 | ShikiDay3 | `func_ov030_020875ec` | `FldEvent_OnChoice_ShikiDay3_10` |
| 13 | 1740 | 136 | ShikiDay4 | `func_ov030_020890c0` | `FldEvent_OnChoice_ShikiDay4_01` |
| 14 | 1757 | 204 | ShikiDay4 | `func_ov030_02089148` | `FldEvent_OnChoice_ShikiDay4_02` |
| 15 | 1783 | 236 | ShikiDay4 | `func_ov030_02089214` | `FldEvent_OnChoice_ShikiDay4_03` |
| 16 | 2129 | 120 | ShikiDay5 | `func_ov030_0208a028` | `FldEvent_OnChoice_ShikiDay5_01` |
| 17 | 2144 | 128 | ShikiDay5 | `func_ov030_0208a0a0` | `FldEvent_OnKeywordSelected_ShikiDay5_02` |
| 18 | 2159 | 124 | ShikiDay5 | `func_ov030_0208a120` | `FldEvent_OnKeywordSelected_ShikiDay5_03` |
| 19 | 3020 | 168 | ShikiDay6 | `func_ov030_0208c3d8` | `FldEvent_OnKeywordSelected_ShikiDay6_01` |
| 20 | 3039 | 168 | ShikiDay6 | `func_ov030_0208c480` | `FldEvent_OnKeywordSelected_ShikiDay6_02` |
| 21 | 3058 | 168 | ShikiDay6 | `func_ov030_0208c528` | `FldEvent_OnKeywordSelected_ShikiDay6_03` |
| 22 | 3077 | 168 | ShikiDay6 | `func_ov030_0208c5d0` | `FldEvent_OnKeywordSelected_ShikiDay6_04` |
| 23 | 3096 | 184 | ShikiDay6 | `func_ov030_0208c678` | `FldEvent_OnChoice_ShikiDay6_05` |
| 24 | 3118 | 184 | ShikiDay6 | `func_ov030_0208c730` | `FldEvent_OnChoice_ShikiDay6_06` |
| 25 | 3140 | 184 | ShikiDay6 | `func_ov030_0208c7e8` | `FldEvent_OnChoice_ShikiDay6_07` |
| 26 | 3622 | 184 | ShikiDay7 | `func_ov030_0208dbd8` | `FldEvent_OnChoice_ShikiDay7_01` |
| 27 | 3644 | 184 | ShikiDay7 | `func_ov030_0208dc90` | `FldEvent_OnChoice_ShikiDay7_02` |
| 28 | 3666 | 180 | ShikiDay7 | `func_ov030_0208dd48` | `FldEvent_OnChoice_ShikiDay7_03` |
| 29 | 4006 | 124 | JoshuaDay1 | `func_ov030_0208eb40` | `FldEvent_OnChoice_JoshuaDay1_01` |
| 30 | 4021 | 128 | JoshuaDay1 | `func_ov030_0208ebbc` | `FldEvent_OnChoice_JoshuaDay1_02` |
| 31 | 4356 | 124 | JoshuaDay2 | `func_ov030_0208fa28` | `FldEvent_OnChoice_JoshuaDay2_01` |
| 32 | 4978 | 216 | JoshuaDay3 | `func_ov030_0209147c` | `FldEvent_OnChoice_JoshuaDay3_01` |
| 33 | 5004 | 272 | JoshuaDay3 | `func_ov030_02091554` | `FldEvent_OnChoice_JoshuaDay3_02` |
| 34 | 5037 | 184 | JoshuaDay3 | `func_ov030_02091664` | `FldEvent_OnChoice_JoshuaDay3_03` |
| 35 | 5059 | 128 | JoshuaDay3 | `func_ov030_0209171c` | `FldEvent_OnChoice_JoshuaDay3_04` |
| 36 | 5074 | 184 | JoshuaDay3 | `func_ov030_0209179c` | `FldEvent_OnChoice_JoshuaDay3_05` |
| 37 | 5096 | 184 | JoshuaDay3 | `func_ov030_02091854` | `FldEvent_OnChoice_JoshuaDay3_06` |
| 38 | 5118 | 184 | JoshuaDay3 | `func_ov030_0209190c` | `FldEvent_OnChoice_JoshuaDay3_07` |
| 39 | 5140 | 200 | JoshuaDay3 | `func_ov030_020919c4` | `FldEvent_OnChoice_JoshuaDay3_08` |
| 40 | 5166 | 316 | JoshuaDay3 | `func_ov030_02091a8c` | `FldEvent_OnKeywordSelected_JoshuaDay3_09` |
| 41 | 5800 | 128 | JoshuaDay4 | `func_ov030_020936b8` | `FldEvent_OnChoice_JoshuaDay4_01` |
| 42 | 5815 | 140 | JoshuaDay4 | `func_ov030_02093738` | `FldEvent_OnChoice_JoshuaDay4_02` |
| 43 | 5832 | 188 | JoshuaDay4 | `func_ov030_020937c4` | `FldEvent_OnChoice_JoshuaDay4_03` |
| 44 | 5854 | 216 | JoshuaDay4 | `func_ov030_02093880` | `FldEvent_OnChoice_JoshuaDay4_04` |
| 45 | 5880 | 180 | JoshuaDay4 | `func_ov030_02093958` | `FldEvent_OnChoice_JoshuaDay4_05` |
| 46 | 5901 | 140 | JoshuaDay4 | `func_ov030_02093a0c` | `FldEvent_OnChoice_JoshuaDay4_06` |
| 47 | 5918 | 168 | JoshuaDay4 | `func_ov030_02093a98` | `FldEvent_OnChoice_JoshuaDay4_07` |
| 48 | 5937 | 160 | JoshuaDay4 | `func_ov030_02093b40` | `FldEvent_OnChoice_JoshuaDay4_08` |
| 49 | 5956 | 160 | JoshuaDay4 | `func_ov030_02093be0` | `FldEvent_OnChoice_JoshuaDay4_09` |
| 50 | 5975 | 184 | JoshuaDay4 | `func_ov030_02093c80` | `FldEvent_OnChoice_JoshuaDay4_10` |
| 51 | 5997 | 128 | JoshuaDay4 | `func_ov030_02093d38` | `FldEvent_OnChoice_JoshuaDay4_11` |
| 52 | 6516 | 180 | JoshuaDay5 | `func_ov030_02095364` | `FldEvent_OnChoice_JoshuaDay5_01` |
| 53 | 6538 | 184 | JoshuaDay5 | `func_ov030_02095418` | `FldEvent_OnChoice_JoshuaDay5_02` |
| 54 | 6560 | 184 | JoshuaDay5 | `func_ov030_020954d0` | `FldEvent_OnChoice_JoshuaDay5_03` |
| 55 | 6582 | 164 | JoshuaDay5 | `func_ov030_02095588` | `FldEvent_OnChoice_JoshuaDay5_04` |
| 56 | 6601 | 156 | JoshuaDay5 | `func_ov030_0209562c` | `FldEvent_OnChoice_JoshuaDay5_05` |
| 57 | 6620 | 156 | JoshuaDay5 | `func_ov030_020956c8` | `FldEvent_OnChoice_JoshuaDay5_06` |
| 58 | 6639 | 124 | JoshuaDay5 | `func_ov030_02095764` | `FldEvent_OnChoice_JoshuaDay5_07` |
| 59 | 6654 | 124 | JoshuaDay5 | `func_ov030_020957e0` | `FldEvent_OnChoice_JoshuaDay5_08` |
| 60 | 7199 | 136 | JoshuaDay6 | `func_ov030_02096f2c` | `FldEvent_OnChoice_JoshuaDay6_01` |
| 61 | 7215 | 136 | JoshuaDay6 | `func_ov030_02096fb4` | `FldEvent_OnChoice_JoshuaDay6_02` |
| 62 | 7231 | 124 | JoshuaDay6 | `func_ov030_0209703c` | `FldEvent_OnChoice_JoshuaDay6_03` |
| 63 | 7246 | 180 | JoshuaDay6 | `func_ov030_020970b8` | `FldEvent_OnChoice_JoshuaDay6_04` |
| 64 | 7848 | 128 | BeatDay1 | `func_ov030_02098930` | `FldEvent_OnChoice_BeatDay1_01` |
| 65 | 7863 | 128 | BeatDay1 | `func_ov030_020989b0` | `FldEvent_OnChoice_BeatDay1_02` |
| 66 | 8458 | 140 | BeatDay3 | `func_ov030_0209a1e4` | `FldEvent_OnChoice_BeatDay3_01` |
| 67 | 8475 | 184 | BeatDay3 | `func_ov030_0209a270` | `FldEvent_OnChoice_BeatDay3_02` |
| 68 | 8497 | 208 | BeatDay3 | `func_ov030_0209a328` | `FldEvent_OnChoice_BeatDay3_03` |
| 69 | 8523 | 244 | BeatDay3 | `func_ov030_0209a3f8` | `FldEvent_OnChoice_BeatDay3_04` |
| 70 | 8555 | 120 | BeatDay3 | `func_ov030_0209a4ec` | `FldEvent_OnChoice_BeatDay3_05` |
| 71 | 8961 | 216 | BeatDay4 | `func_ov030_0209b810` | `FldEvent_OnChoice_BeatDay4_01` |
| 72 | 8981 | 220 | BeatDay4 | `func_ov030_0209b8e8` | `FldEvent_OnChoice_BeatDay4_02` |
| 73 | 9001 | 264 | BeatDay4 | `func_ov030_0209b9c4` | `FldEvent_OnChoice_BeatDay4_03` |
| 74 | 9291 | 260 | BeatDay5 | `func_ov030_0209c57c` | `FldEvent_OnChoice_BeatDay5_01` |
| 75 | 9730 | 136 | BeatDay7 | `func_ov030_0209d974` | `FldEvent_OnChoice_BeatDay7_01` |
| 76 | 9746 | 124 | BeatDay7 | `func_ov030_0209d9fc` | `FldEvent_OnChoice_BeatDay7_02` |
| 77 | 9761 | 132 | BeatDay7 | `func_ov030_0209da78` | `FldEvent_OnChoice_BeatDay7_03` |
| 78 | 10264 | 136 | PostGame01 | `func_ov030_0209efe4` | `FldEvent_OnChoice_PostGame01_01` |
| 79 | 10280 | 124 | PostGame01 | `func_ov030_0209f06c` | `FldEvent_OnChoice_PostGame01_02` |
| 80 | 11527 | 144 | PostGame02 | `func_ov030_020a1fdc` | `FldEvent_OnKeywordSelected_PostGame02_01` |
| 81 | 11638 | 204 | PostGame04 | `func_ov030_020a240c` | `FldEvent_OnKeywordSelected_PostGame04_01` |
| 82 | 11662 | 144 | PostGame04 | `func_ov030_020a24d8` | `FldEvent_OnChoice_PostGame04_02` |
| 83 | 11679 | 192 | PostGame04 | `func_ov030_020a2568` | `FldEvent_OnChoice_PostGame04_03` |
| 84 | 11896 | 140 | PostGame06 | `func_ov030_020a2d5c` | `FldEvent_OnChoice_PostGame06_01` |

### 4.5 Trade / post-script choice callbacks

Stored in `unk_21D24` and driven when `unk_21D18 == 5` in `FldEvent_HandleScriptEnd`. Same `_NN` caveat as 4.4. `Reaper` dropped: only `ShikiDay5_01..04`/`BeatDay3_01` look trade-related; `PostGame05_01` is a generic `unk_21D1C 0/1` branch.

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 2174 | 596 | ShikiDay5 | `func_ov030_0208a19c` | `FldEvent_OnChoiceTrade_ShikiDay5_01` |
| 2 | 2248 | 600 | ShikiDay5 | `func_ov030_0208a3f0` | `FldEvent_OnChoiceTrade_ShikiDay5_02` |
| 3 | 2322 | 600 | ShikiDay5 | `func_ov030_0208a648` | `FldEvent_OnChoiceTrade_ShikiDay5_03` |
| 4 | 2396 | 596 | ShikiDay5 | `func_ov030_0208a8a0` | `FldEvent_OnChoiceTrade_ShikiDay5_04` |
| 5 | 8384 | 600 | BeatDay3 | `func_ov030_02099f8c` | `FldEvent_OnChoiceTrade_BeatDay3_01` |
| 6 | 11811 | 404 | PostGame05 | `func_ov030_020a2a8c` | `FldEvent_OnChoiceTrade_PostGame05_01` |

### 4.6 Event selectors and stubs

All 42 are currently orphaned. Rows below are sub-classified for review only: the 15 8-byte `return 0;` stub rows must be **left unnamed** (`func_ov030_*`) per the convention above, and so must two non-stubs that do not fit the class - `func_ov030_020a2fc4` (row 4, always returns `0` and only sets FldGates) and `func_ov030_020a36f0` (row 8, `s32 f(s32 arg0)`); their `suggested` cells read `(leave unnamed)` for those reasons. `FldEvent_SelectNextEvent_NN` global numbering is likewise placeholder only - use `FldEvent_SelectNextEvent_<Chapter>_<Area>` once callers are found. Rows 19-42 have no chapter because their block follows chapter 42's handler with no closing bracket; they are the natural chapter 43 / PostGame23 block (see section 2), not unassigned functions.

| # | line | bytes | chapter | current | suggested |
|---|---|---|---|---|---|
| 1 | 8941 | 236 | BeatDay4 | `func_ov030_0209b724` | `FldEvent_SelectNextEvent_01` |
| 2 | 11807 | 8 | PostGame05 | `func_ov030_020a2a84` | (leave unnamed) |
| 3 | 11892 | 8 | PostGame06 | `func_ov030_020a2d54` | (leave unnamed) |
| 4 | 11957 | 168 | PostGame07 | `func_ov030_020a2fc4` | (leave unnamed) |
| 5 | 11987 | 8 | PostGame08 | `func_ov030_020a30cc` | (leave unnamed) |
| 6 | 12015 | 8 | PostGame09 | `func_ov030_020a3174` | (leave unnamed) |
| 7 | 12030 | 8 | PostGame10 | `func_ov030_020a31b4` | (leave unnamed) |
| 8 | 12185 | 40 | PostGame12 | `func_ov030_020a36f0` | (leave unnamed - takes `s32`) |
| 9 | 12202 | 8 | PostGame13 | `func_ov030_020a3748` | (leave unnamed) |
| 10 | 12216 | 8 | PostGame14 | `func_ov030_020a3780` | (leave unnamed) |
| 11 | 12230 | 8 | PostGame15 | `func_ov030_020a37b8` | (leave unnamed) |
| 12 | 12244 | 8 | PostGame16 | `func_ov030_020a37f0` | (leave unnamed) |
| 13 | 12258 | 8 | PostGame17 | `func_ov030_020a3828` | (leave unnamed) |
| 14 | 12271 | 8 | PostGame18 | `func_ov030_020a3850` | (leave unnamed) |
| 15 | 12285 | 8 | PostGame19 | `func_ov030_020a3888` | (leave unnamed) |
| 16 | 12299 | 8 | PostGame20 | `func_ov030_020a38c0` | (leave unnamed) |
| 17 | 12313 | 8 | PostGame21 | `func_ov030_020a38f8` | (leave unnamed) |
| 18 | 12327 | 8 | PostGame22 | `func_ov030_020a3930` | (leave unnamed) |
| 19 | 12335 | 40 | ? | `func_ov030_020a3940` | `FldEvent_SelectNextEvent_19` |
| 20 | 12345 | 144 | ? | `func_ov030_020a3968` | `FldEvent_SelectNextEvent_20` |
| 21 | 12361 | 512 | ? | `func_ov030_020a39f8` | `FldEvent_SelectNextEvent_21` |
| 22 | 12395 | 1684 | ? | `func_ov030_020a3bf8` | `FldEvent_SelectNextEvent_22` |
| 23 | 12489 | 328 | ? | `func_ov030_020a428c` | `FldEvent_SelectNextEvent_23` |
| 24 | 12521 | 288 | ? | `func_ov030_020a43d4` | `FldEvent_SelectNextEvent_24` |
| 25 | 12546 | 996 | ? | `func_ov030_020a44f4` | `FldEvent_SelectNextEvent_25` |
| 26 | 12605 | 224 | ? | `func_ov030_020a48d8` | `FldEvent_SelectNextEvent_26` |
| 27 | 12623 | 124 | ? | `func_ov030_020a49b8` | `FldEvent_SelectNextEvent_27` |
| 28 | 12633 | 356 | ? | `func_ov030_020a4a34` | `FldEvent_SelectNextEvent_28` |
| 29 | 12658 | 1192 | ? | `func_ov030_020a4b98` | `FldEvent_SelectNextEvent_29` |
| 30 | 12739 | 1480 | ? | `func_ov030_020a5040` | `FldEvent_SelectNextEvent_30` |
| 31 | 12826 | 860 | ? | `func_ov030_020a5608` | `FldEvent_SelectNextEvent_31` |
| 32 | 12893 | 444 | ? | `func_ov030_020a5964` | `FldEvent_SelectNextEvent_32` |
| 33 | 12916 | 180 | ? | `func_ov030_020a5b20` | `FldEvent_SelectNextEvent_33` |
| 34 | 12931 | 148 | ? | `func_ov030_020a5bd4` | `FldEvent_SelectNextEvent_34` |
| 35 | 12943 | 324 | ? | `func_ov030_020a5c68` | `FldEvent_SelectNextEvent_35` |
| 36 | 12963 | 440 | ? | `func_ov030_020a5dac` | `FldEvent_SelectNextEvent_36` |
| 37 | 12996 | 408 | ? | `func_ov030_020a5f64` | `FldEvent_SelectNextEvent_37` |
| 38 | 13029 | 244 | ? | `func_ov030_020a60fc` | `FldEvent_SelectNextEvent_38` |
| 39 | 13045 | 332 | ? | `func_ov030_020a61f0` | `FldEvent_SelectNextEvent_39` |
| 40 | 13069 | 56 | ? | `func_ov030_020a633c` | `FldEvent_SelectNextEvent_40` |
| 41 | 13077 | 640 | ? | `func_ov030_020a6374` | `FldEvent_SelectNextEvent_41` |
| 42 | 13123 | 168 | ? | `func_ov030_020a65f4` | `FldEvent_SelectNextEvent_42` |

---

## 5. Header / table changes that should ride along

> **Symbol-map sync is required for the USA map.** Every symbol below appears in
> `config/usa/arm9/overlays/ov030/symbols.txt` at the line given in the `symbols.txt` column. `ninja check` runs
> `dsd check symbols --elf-path ... --fail` (`build.ninja:59,11592`; `check` is part of the default target at
> `build.ninja:12969`, and `.github/workflows/build.yml` configures both `usa` and `jp` before running `ninja`), so a
> C-side rename that is not mirrored in the symbol map fails the check even though the code still compiles.
>
> None of the five symbols below exists in `config/jp/arm9/overlays/ov030/symbols.txt` (verified by name and by address
> fragment): JP names its data symbols after *JP* addresses (e.g. `data_ov030_020da6bc`), and JP has no `EventScripts`
> symbol at all. These five rows therefore need **no** JP mirror edit. The per-version warning does apply to the
> section 3 function renames - both maps carry the per-version `func_ov030_*` names and the JP addresses differ (e.g.
> `FldEvent_SetCurrent` is `0x020a7078` in JP vs `0x020a669c` in USA). The `where` column lists the C/header sites.

| current | suggested | where (C/header) | symbols.txt | why |
|---|---|---|---|---|
| `FieldEventRewardFunc` | `FieldEventScriptEndFunc` | `include/Field/Events.h:201` | typedef only | Dispatched on script *end*, not only for rewards. |
| `data_ov030_020da618` | `gFieldEventScriptEndHandlers` | `Events.c:13135`, `Events.h:202`, `EventManager.c:1684` | `:1959` | Follows the typedef rename. `g` not `s`: it is a global `.rodata` table (`extern` across translation units in ov030, not "across overlays"). |
| `EventScripts` | `gFieldEventScripts` | `EventManager.c:25` | `:4400` (delinked data, `addr:0x020fdeb8`; no C definition) | Currently a bare `extern void*[2207]`; the elements are consumed as `s16*` (`FldEvent_SetCurrent` -> `gSaveData.unk_24A0`), so the type should be `extern s16* gFieldEventScripts[2207]`. The delinked data symbol must be renamed too. |
| `data_ov030_020dbdd8` | `gFieldEventOpcodeOperandWidths` | `EventManager.c:24` | `:2001` (delinked; no C definition) | 88-entry opcode -> halfword-count table used by `FldEvent_AdvanceScript`. `g` not `s`: `extern`. CAUTION: `FldEvent_AdvanceScript` indexes it two different ways - by opcode at `:123` but by *PC* at `:149` (`data_ov030_020dbdd8[gSaveData.unk_249C]`, unbounded against the 88 entries). That function is Nonmatching, so the body is not authoritative; confirm the `:149` indexing before baking "opcode -> width" into the name. |
| `data_ov030_020d9586/88/8a` | `gFieldEventScriptYesNo` (9586); leave 9588 / 958a unnamed for now | `EventManager.c:19-21` | `:1839-1841` (delinked; no C definitions) | Only `9586` (installed by opcode 0x37 at `EventManager.c:1050`) is plausibly the yes/no flow. `958a` is installed while text is still paging and `9588` when there is no text task (`EventManager.c:713-723`), i.e. both look like text-continuation sub-scripts - naming them `Choice`/`NoChoice`, as an earlier draft did, contradicts that. Prefixed when named: bare `sScript*` is too generic and collides. |

## 6. Struct fields named as a side effect

These fall out of naming the slot families in section 3:

| field | suggested | evidence |
|---|---|---|
| `unk_21BBC[35]` | `bustShotSlotTaskIds[]` | slots hold `BustShotSprite` tasks (`Fld_BustShot.c`), selected by bust-shot resource id; `0x118` (= 280, the table size) is special-cased as the base slot. `TaskIds`: slots hold task ids (`-1` = free) |
| `unk_21C48[32]` | `windowSlotTaskIds[]` | two banks, `0..15` and `16..31`, allocated by the AllocWindowSlot family. Same `TaskIds` fix |
| `unk_21CD0` | `activeBustShotSlotPtr` | written by `FldEvent_GetBustShotSlotPtr`; is `u32*` into `unk_21BBC`, not an index |
| `unk_21CD4` | `activeWindowSlotIndex` | compared against in the fade/close loops; records the free index in `AllocWindowSlot` |
| `unk_21D18` | `postScriptState` | 0..5 mini state machine driving the post-script overlay stack in `HandleScriptEnd`. Ex-`cutsceneState`: these are field talk/keyword/scan overlays, not cinematics |
| `unk_21D24` | `postScriptChoiceCallback` | only ever holds `func_ov030_020aa390` (`EventManager.c:1711`) or one of the 6 trade callbacks of 4.5, or `NULL`. The `FldEvent_CreateChoiceWindow` callbacks are passed as an argument and called directly (`EventManager.c:1566`), never stored here. Run when `postScriptState == 5` |
| `unk_21D28` | `postScriptMessageId` | message id paired with the above |
| `unk_216B0` | `talkWindowTaskId` | used with the string-plate / fade tasks |
| `unk_21D00` | `talkBustShotResourceId` | NOT a slot index: init to `280` (`InitState`), fed to `AllocRingSlot`/`GetBustShotSlotPtr` as a bust-shot resource id; overwritten by `RunScanPrompt(arg1)` |
| `unk_21D08` | `keywordSelectedCallback` | Holds `func_ov030_020aa0c8`'s 5th argument; invoked from `func_ov030_020a71a8` (`EventManager.c:439`), the only call site. Type is wrong in the header: `void (*)(void*)` (`Events.h:168`) vs the actual `s32 (*)(FieldEventManager*)` - fix with step 0 |
| `unk_22E2C` | `nextRingSlotId` | counter for the `%280` ring allocator `AllocRingSlot` |
| `gSaveData.unk_24C8[16]` | `keywords[]` | list of collected keyword ids (deduplicated by `AddKeyword`); the keyword menu selects an entry via `unk_24C4`. Proposal only - save-struct field, not renamed in the tree |
| `gSaveData.unk_24C4` | `selectedKeywordIndex` | set from the keyword-menu selection (`PollKeywordMenu`, `EventManager.c:412`); the 12 `OnKeywordSelected_*` callbacks switch on `keywords[selectedKeywordIndex]`. Proposal only |

`taskId_KeywordMenu` (offset `0x216B8`) is already named in the header and is what the code uses - no entry needed here.

## 7. Open questions for reviewers

1. **Chapters 22-43 labels** - chapter 21 is very likely **Another Day** (its handler `func_ov030_0209f0e8` is the only one touching `EVENT_ANOTHERDAY_*`), so `PostGame01` should become `AnotherDay1` once confirmed. The rest are still placeholders, and the tail block (4.6 rows 19-42) is the natural chapter 43 block. Do not apply any remaining `PostGameNN` name until they are resolved.
2. **The 111 orphaned functions** (42 inits in `Events.c` + chapter 0's init in `FieldBg.c` + 26 triggers + 42 selectors/stubs). Their callers are not decompiled, so parameter
   semantics are inferred from the body only. Fix the `s32 f(s32)` prototypes to `(FieldEventManager*)` - it affects the triggers,
   the choice callback `func_ov030_02086f10` and the selector `func_ov030_020a36f0` - the slot-family missing returns
   (`6960`, `6d34`, `75c0`, plus the `6d34` wrapper return types) and `unk_21D08`'s callback type before renaming.
3. **The mode-1 grant helper `func_ov030_020aac28`** - RESOLVED: `Inventory_AddItem` (`src/Inventory.c:424-469`) treats the second argument as a pin placement mode for pins: `1` = stockpile (`Inventory_AddStockpiledPin`, promoted to `2` when `pinData.maxLevel == 1`), `2` = mastered (`Inventory_AddMasteredPin`), `0` = plain inventory item (a PIN passed with `0` falls through to `return FALSE`). So `GrantItem: mode 0` = non-pin item, `GrantPin: mode 2` = mastered pin, and `func_ov030_020aac28` grants a **stockpiled (unmastered) pin** - `FldEvent_GrantPinStockpiled` is only partly accurate: the mode is promoted to 2 (mastered) when `pinData.maxLevel == 1`, and mode 1 on a non-pin item takes the plain-inventory branch, so prefer a neutral name (e.g. `FldEvent_GrantPinMode1`) unless the callers are confirmed to pass pins only; it is not "thread".
4. **`unk_21D1C` vs `unk_2176C[unk_2177C]`** - RESOLVED: they are two distinct choice indices. `unk_2176C[unk_2177C]` is the in-script choice-window selection, written by `func_ov030_020a9f54` (`EventManager.c:1553-1565`) and switched on by the 72 window callbacks. `unk_21D1C` (`Events.h:174`) is an *external* choice fed in from field/battle code through `func_ov030_020af708` (`FieldBattleSetup.c:197-200`, plays SE `0xC2` on set), switched on by the 6 trade callbacks and `func_ov030_020aa390`, and used directly as a `FldEvent_SetCurrent` target at `EventManager.c:1791`.
5. **`func_ov030_02084978`** - it is `data_ov030_020da618[43]` but is defined in `FieldBg.c`. Renaming it touches a third file. The same file holds chapter 0's init `func_ov030_02084980` (`FieldBg.c:743`), which section 4.2 row 43 now covers.
6. **Eight `// Nonmatching` functions in `EventManager.c`** (lines 101, 121, 319, 367, 620, 1456, 1542, 1814 - i.e. section 3 rows 16, 20, 34, 44, 48 plus the already-named `SetCurrent`/`AdvanceScript`/`BeginScript`). Renames are codegen-neutral,
   but if the goal is match percentage rather than readability, those are the real targets.
7. **Symbol-map sync** - `config/usa/...` and `config/jp/...` each carry their own `symbols.txt`, and the JP addresses differ (e.g. `FldEvent_SetCurrent` is `0x020a7078` in JP vs `0x020a669c` in USA), so the section 3 function renames need a mirrored edit per version. Section 5's five delinked-data symbols are USA-only - they do not exist in the JP map - so they need no JP edit.

## 8. Terminology: `Keyword` (kept)

The in-game "meme" (twewy.fandom.com/wiki/Meme) is the phone text the player receives and then uses to imprint people. The Japanese term is キーワード ("keyword"), NEO renamed memes to keywords, and the ROM itself uses `Keyword` (task name string `"Tsk_Fld_TalkEventKeywordMenu"`, `SE_get_keyword`, `message_KeyWord_w.xls`). To stay aligned with the ROM and the existing tree names (`Fld_TalkEventKeywordMenu_*`, `taskId_KeywordMenu`), this doc uses `Keyword` everywhere and the tree is unchanged. A trial rename of the tree identifiers to `Meme` was built and verified, then reverted in favour of this.
