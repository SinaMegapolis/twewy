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
 *   OtuBand1.c   0x0208f000 - 0x02090000
 *   OtuBand2.c   0x02093000 - 0x02094000
 *   OtuBand3.c   0x02095000 - 0x02096000
 *   OtuBand4.c   0x02096000 - 0x02097000
 *   OtuBand5.c   0x02097000 - 0x02098000
 */