#include "ShopPart.h"

void func_ov043_020b16c4(void *arg0, u16 arg1) {
    s32 temp_r5;
    u16 temp_r4;
    void *temp_r1;

    temp_r1 = (arg0 + (arg1 * 4))->unk_738;
    temp_r4 = temp_r1->unk_0;
    temp_r5 = temp_r1->unk_10;
    if (temp_r1->unk_4 == 0) {
        if (Inventory_AddItem(temp_r4, 1) == 1) {
            if (temp_r5 == 1) {
                func_ov043_020b01d8(arg0);
                arg0->unk_834 = (u16) (arg0->unk_834 | 0x40);
            }
        }
    } else if ((Inventory_GetOwnedCount() < 9U) && (Inventory_AddItem(temp_r4, 0) == 1)) {
        if (temp_r5 == 1) {
            func_ov043_020b01d8(arg0);
            arg0->unk_834 = (u16) (arg0->unk_834 | 0x40);
        }
    }
}

void func_ov043_020b1780(s32 arg0, s32 arg1) {
    u16 temp_r0_2;
    void *temp_r0;

    temp_r0 = (arg0 + (arg1 * 4))->unk_738;
    temp_r0_2 = temp_r0->unk_0;
    if (temp_r0->unk_4 == 0) {
        if (Inventory_CanAddPin(temp_r0_2, 1) == 1) {

        }
    } else if (Inventory_CanAddPin(temp_r0_2, 0) == 1) {

    }
}

void func_ov043_020b17d0(s16 arg0, s16 arg1) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;

    sp8 = data_ov043_020ca260.unk_10;
    spC = data_ov043_020ca260.unk_8;
    spA = data_ov043_020ca260.unk_12;
    spE = data_ov043_020ca260.unk_A;
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

    }
}

void func_ov043_020b1830(void *arg0) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;
    u16 sp10;
    u16 sp12;
    u16 sp14;
    s16 var_r10;

    spA = data_ov043_020ca260.unk_6;
    sp8 = data_ov043_020ca260.unk_4;
    sp14 = data_ov043_020ca260.unk_44;
    sp12 = data_ov043_020ca260.unk_42;
    spC = data_ov043_020ca260.unk_3C;
    spE = data_ov043_020ca260.unk_3E;
    sp10 = data_ov043_020ca260.unk_40;
    var_r10 = 0;
loop_1:
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        return;
    }
    var_r10 += 1;
    if ((s32) var_r10 >= 5) {
        return;
    }
    goto loop_1;
}

void func_ov043_020b18e4(s16 arg0, s16 arg1) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;

    sp8 = data_ov043_020ca260.unk_24;
    spC = data_ov043_020ca260.unk_28;
    spA = data_ov043_020ca260.unk_26;
    spE = data_ov043_020ca260.unk_2A;
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

    }
}

void func_ov043_020b1944(s16 arg0, s16 arg1) {
    u16 sp8;
    u16 spA;
    s32 var_r0;
    u32 var_r6;

    sp8 = data_ov043_020ca260.unk_20;
    spA = data_ov043_020ca260.unk_22;
    var_r6 = 0;
loop_1:
    if ((s16) (((var_r6 - (6 * ((var_r6 / 6) + (var_r6 >> 0x1F)))) * 0x21) + 6) == 0xAB) {
        var_r0 = 2;
    } else {
        var_r0 = 0;
    }
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) ((s16) sp8 - var_r0), /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        return;
    }
    var_r6 = (u32) (s16) (var_r6 + 1);
    if ((s32) var_r6 >= 0xC) {
        return;
    }
    goto loop_1;
}

void func_ov043_020b1a1c(void *arg0) {
    s32 temp_r1;
    u16 var_r5;
    u8 temp_r6;

    var_r5 = 0;
loop_1:
    temp_r1 = (arg0 + (M2C_ERROR(/* Read from unset register $r1 */) * 4))->unk_738;
    temp_r6 = (temp_r1 + var_r5)->unk_1C;
    if (temp_r6 != 0) {
        func_ov043_020b0088((void *) (temp_r1 + (var_r5 * 2))->unk_16);
        if ((s32) M2C_ERROR(/* Read from unset register $r0 */) < (s32) temp_r6) {
            return;
        }
    }
    var_r5 += 1;
    if ((u32) var_r5 >= 3U) {
        return;
    }
    goto loop_1;
}

void func_ov043_020b1a70(void *arg0, s16 arg1) {
    void *temp_r3;

    if (arg1 == -1) {
        return;
    }
    temp_r3 = (arg0 + (arg1 * 4))->unk_738;
    if (temp_r3->unk_C == 1) {
        if (temp_r3->unk_10 == 0) {
            if ((u32) temp_r3->unk_8 <= (u32) arg0->unk_858) {

            }
        } else {
            func_ov043_020b1a1c(arg0);
            if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

            }
        }
    } else if ((temp_r3->unk_10 == 1) && (func_02023d88(temp_r3->unk_14) == 0)) {
        func_ov043_020b1a1c(arg0);
        if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

        }
    }
}

void func_ov043_020b1b14(void *arg0) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;

    sp8 = data_ov043_020ca260.unk_18;
    spC = data_ov043_020ca260.unk_1C;
    spA = data_ov043_020ca260.unk_1A;
    spE = data_ov043_020ca260.unk_1E;
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

    }
}

void func_ov043_020b1b74(void *arg0) {
    u16 sp8;
    u16 spA;

    sp8 = data_ov043_020ca260.unk_0;
    spA = data_ov043_020ca260.unk_2;
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {

    }
}

void func_ov043_020b1bd4(void *arg0) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;
    u16 sp10;
    u16 sp12;
    s16 var_r6;

    spC = data_ov043_020ca260.unk_2C;
    spE = data_ov043_020ca260.unk_2E;
    var_r6 = 0;
    sp10 = data_ov043_020ca260.unk_30;
    sp12 = data_ov043_020ca260.unk_32;
    spA = data_ov043_020ca260.unk_E;
    sp8 = data_ov043_020ca260.unk_C;
loop_1:
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        return;
    }
    var_r6 += 1;
    if ((s32) var_r6 >= 2) {
        return;
    }
    goto loop_1;
}

void func_ov043_020b1c7c(s16 arg0, s16 arg1) {
    u16 sp8;
    u16 spA;
    u16 spC;
    u16 spE;
    u16 sp10;
    u16 sp12;
    s16 var_r6;

    spC = data_ov043_020ca260.unk_34;
    spE = data_ov043_020ca260.unk_36;
    var_r6 = 0;
    sp10 = data_ov043_020ca260.unk_38;
    sp12 = data_ov043_020ca260.unk_3A;
    spA = data_ov043_020ca260.unk_16;
    sp8 = data_ov043_020ca260.unk_14;
loop_1:
    func_ov043_020af3f0((ShopState *) arg0, /* extra? */ (s32) (s16) sp8, /* extra? */ (s32) (s16) spA);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 1) {
        return;
    }
    var_r6 += 1;
    if ((s32) var_r6 >= 2) {
        return;
    }
    goto loop_1;
}

void func_ov043_020b1d24(u8 arg0) {
    switch (arg0) {
    default:
    case 0:
    case 5:
    case 10:
    case 11:
    case 12:
    case 13:
    case 18:
    case 23:
    case 24:
    case 25:
    case 30:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
        return;
    case 1:
    case 2:
    case 3:
    case 4:
        return;
    case 6:
    case 7:
    case 8:
    case 9:
        return;
    case 14:
    case 15:
    case 16:
    case 17:
        return;
    case 19:
    case 20:
    case 21:
    case 22:
        return;
    case 26:
    case 27:
    case 28:
    case 29:
        return;
    case 31:
    case 32:
    case 33:
    case 34:
        return;
    case 43:
    case 44:
    case 45:
    case 46:
        return;
    }
}

void func_ov043_020b1e2c(void *arg0) {
    u16 temp_r1;

    temp_r1 = arg0->unk_776;
    switch ((s32) temp_r1) {                        /* irregular */
    case 29:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 22:
        return;
    default:
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        if ((u32) arg0->unk_788 >= 0x3E8U) {
            return;
        }
        return;
    }
}

void func_ov043_020b1ecc(void *arg0, u16 arg1) {
    switch (arg1) {
    default:
        return;
    case 18:
    case 28:
        return;
    case 2:
    case 8:
    case 9:
    case 10:
    case 11:
    case 22:
    case 23:
    case 30:
    case 31:
    case 34:
        return;
    case 15:
    case 16:
    case 17:
    case 26:
        return;
    case 29:
        return;
    case 20:
        return;
    case 5:
    case 12:
    case 19:
    case 27:
        return;
    case 3:
    case 4:
    case 6:
    case 21:
    case 32:
        return;
    case 0:
    case 1:
    case 13:
    case 14:
        return;
    case 7:
    case 33:
        return;
    case 25:
        return;
    case 24:
        return;
    }
}
