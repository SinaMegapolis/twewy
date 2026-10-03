/*
 * Band files.
 *
 * OtuFieldAccess.c #includes these rather than each being its own translation
 * unit. That is not a stylistic choice -- it is forced by the delinks format:
 *
 *   - a file may claim a `.text` range only once;
 *   - two claimed ranges may not overlap;
 *   - every remaining function in this overlay already sits inside
 *     OtuFieldAccess.c's claim of 0x02088400-0x02098b8c, and there is no free
 *     address space anywhere in the overlay.
 *
 * So no new translation unit can be created here at all, and OtuFieldAccess.c's
 * own 55 functions cannot be split off, because the two groups alternate across
 * the range twenty-two times and any partition of the range strands some of
 * them outside their own file's claim.
 *
 * #include sidesteps that without touching the build system: the band files are
 * preprocessed into OtuFieldAccess.c, become part of its object, and land at
 * their target addresses because the delinker places objects by symbol. Each
 * band file is edited by exactly one person, so they merge without conflict.
 *
 * The bands are disjoint address ranges. Keep it that way.
 *
 *   OtuPinSprites.inc  0x0208f000 - 0x02090000
 *   OtuCounters.inc  0x02093000 - 0x02094000
 *   OtuEntryTasks.inc  0x02095000 - 0x02096000
 *   OtuTaskStages.inc  0x02096000 - 0x02097000
 *   OtuSpriteTasks.inc  0x02097000 - 0x02098000
 *   OtuObstacles.inc  0x02092484 - 0x020926f0  (Tsk_OtosuGame_obstacle)
 *   OtuMeters.inc  0x0209003c - 0x02090e9c  (the cursor family, then
 *                                             Tsk_OtosuGame_hammer)
 *   OtuHammerSpawn.inc  0x02098394               (the simulation tasks' child spawner)
 *
 * The numbering is not the address order: band 8 went in before band 7 because
 * the hammer task's CreateTask calls into it, and band 7 in turn opens with the
 * cursor family at 0x0209003c because its Init and Update call all eight of
 * those. Nothing else in band 7 links until they exist.
 *
 * Band 7 is the second band that is a whole task rather than a slice of a call
 * graph. Like band 6 it carries its own state struct, TaskHandle reference and
 * stage dispatcher, and for the same reason it sits before band 1 in the include
 * order and nowhere else.
 *
 * Undefined callees are not a blocker. Band 7 still calls 0x02090390,
 * 0x0209041c, 0x0208e998 and 0x0208e9ac, none of which are in the build, and it
 * links anyway: dsd resolves an undefined symbol against the original overlay
 * instead of failing. An earlier note here claimed the band could not link
 * without its four "missing" callees; that was wrong about the mechanism.
 *
 * Band 6 is the one band that is a whole task rather than a slice of a call
 * graph: it carries its own state struct, TaskHandle reference and stage
 * dispatcher, and calls nothing the other bands define. That is why it can sit
 * anywhere in the include order -- it is placed before band 1 only because band
 * 1 is the one include with a hard ordering requirement.
 *
 * The .inc extension is load-bearing: `configure.py` walks `src/` and compiles
 * every `.c` it finds, so a band named `.c` gets compiled twice and collides
 * with itself.
 *
 * A WARNING THAT WILL COST YOU AN HOUR IF YOU SKIP IT
 * --------------------------------------------------
 * mwcc is invoked with `-ipa file`, and its incremental cache is keyed on the
 * *including* file's mtime. Editing a band file therefore does NOT invalidate
 * the cache: ninja rebuilds OtuFieldAccess.o, mwcc hands back the cached
 * preprocessed result, and the object comes out with the band file's previous
 * contents. No error, no warning -- just an object silently missing whatever you
 * just wrote.
 *
 * It does not show up in the ROM either. The sha1 still passes and
 * `verify_rom.py` still reports `link confirmed`, because the ROM really was
 * rebuilt -- from a stale object.
 *
 * The only symptom is objdiff reporting `fuzzy_match_percent: null` for
 * functions you can plainly see in your own source.
 *
 * So, after editing a band file:
 *
 *     rm build/usa/src/Debug/Sugata/TinPinSlammer/OtuFieldAccess.o
 *     ninja build/usa/src/Debug/Sugata/TinPinSlammer/OtuFieldAccess.o
 *
 * or touch OtuFieldAccess.c so its mtime moves. `python tools/ov039_docstatus.py`
 * reports the matched total; if a function you just wrote is not in it, this is
 * why.
 */