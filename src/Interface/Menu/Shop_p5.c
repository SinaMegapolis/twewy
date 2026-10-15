#include "ShopPart.h"

void func_ov043_020af6b4(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x11D0, &data_ov043_020ca240));
}

void func_ov043_020af6dc(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0xBA0, &data_ov043_020ca248));
}

void func_ov043_020af700(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x2140, &data_ov043_020ca250));
}

void func_ov043_020af724(void *arg0, s32 arg1) {
    ? sp0;
    s32 temp_r1;
    u16 var_r0;

    func_0203a96c(&sp0);
    var_r0 = 0;
    arg0->unk_86C = 0;
    do {
        temp_r1 = arg0->unk_86C + *(&sp0 + var_r0);
        var_r0 += 1;
        arg0->unk_86C = temp_r1;
    } while ((u32) var_r0 < 6U);
}

void func_ov043_020af770(void *arg0, s32 arg1) {
    u16 temp_r0;
    u16 temp_r0_2;
    u16 temp_r0_3;

    switch (arg0) {                                 /* switch 1; irregular */
    case 0xCE:                                      /* switch 1 */
        if ((u16) (arg1 & 1) == 0) {

        }
        return;
    case 0xD0:                                      /* switch 1 */
        temp_r0 = arg1 & 3;
        switch (temp_r0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            return;
        case 1:                                     /* switch 2 */
            return;
        case 2:                                     /* switch 2 */
            return;
        }
        break;
    case 0xD4:                                      /* switch 1 */
        temp_r0_2 = arg1 - (6 * ((u32) MULTU_HI(arg1, 0xAAAAAAAB) >> 2));
        switch (temp_r0_2) {                        /* switch 3 */
        default:                                    /* switch 3 */
            return;
        case 0:                                     /* switch 3 */
            return;
        case 1:                                     /* switch 3 */
            return;
        case 2:                                     /* switch 3 */
            return;
        case 3:                                     /* switch 3 */
            return;
        case 4:                                     /* switch 3 */
            return;
        }
        break;
    case 0xDA:                                      /* switch 1 */
        temp_r0_3 = arg1 - (3 * ((u32) MULTU_HI(arg1, 0xAAAAAAAB) >> 1));
        switch (temp_r0_3) {                        /* switch 4; irregular */
        case 0:                                     /* switch 4 */
            return;
        case 1:                                     /* switch 4 */
            return;
        }
        break;
    }
}

void func_ov043_020af8a8(void *arg0, s32 arg1) {
    s32 var_hi;
    u32 temp_r1;
    u32 temp_ret;

    temp_ret = _ll_udiv(arg1 * 0x3E8, (0 * 0x3E8) + ((arg1 * 0) + (arg1 / 4294967)));
    temp_r1 = SECOND_REG(temp_ret);
    var_hi = temp_r1 > 0U;
    if (temp_r1 == 0) {
        var_hi = temp_ret > 0x3E8U;
    }
    if (var_hi) {

    }
}

void func_ov043_020af8e4(void *arg0, s32 arg1) {
    s32 temp_r3;
    u16 temp_lr;
    u16 var_ip;

    temp_lr = arg0->unk_788;
    var_ip = 0;
loop_1:
    temp_r3 = (arg0 + (var_ip * 2))->unk_790 * 0xA;
    if ((s32) temp_lr < temp_r3) {
        return;
    }
    if (temp_lr == temp_r3) {
        return;
    }
    var_ip += 1;
    if ((u32) var_ip >= 9U) {
        return;
    }
    goto loop_1;
}

void func_ov043_020af950(void *arg0, s32 arg1) {
    u16 var_r2;

    var_r2 = 0;
loop_1:
    if ((arg0 + (var_r2 * 2))->unk_790 == 0x64) {
        return;
    }
    var_r2 += 1;
    if ((u32) var_r2 >= 9U) {
        return;
    }
    goto loop_1;
}

void func_ov043_020af994(void *arg0) {
    s32 temp_r3;
    u16 temp_r2;
    u16 var_lr;

    temp_r2 = arg0->unk_778;
    if ((u32) temp_r2 >= (u32) arg0->unk_77A) {
        var_lr = 0x64;
    } else {
        var_lr = (arg0 + ((temp_r2 - 1) * 2))->unk_790;
    }
    temp_r3 = arg0->unk_784;
    _ll_udiv(temp_r3 * var_lr, (0 * var_lr) + ((temp_r3 * 0) + MULTU_HI(temp_r3, var_lr)), 0x64, 0);
}

void func_ov043_020af9e8(void *arg0) {
    u16 temp_r2;

    temp_r2 = arg0->unk_788;
    if ((u32) temp_r2 >= 0x3E8U) {
        return;
    }
    if ((s32) temp_r2 >= (s32) (arg0->unk_78E * 0xA)) {

    }
}

void func_ov043_020afa20(void *arg0) {
    func_ov043_020af8a8(arg0, arg0->unk_77C);
    arg0->unk_788 = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af8a8(arg0, arg0->unk_780);
    arg0->unk_78A = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af9e8(arg0);
    arg0->unk_78C = (s16) M2C_ERROR(/* Read from unset register $r0 */);
}

void func_ov043_020afa68(void *arg0) {
    s32 temp_gt;
    s32 temp_ip;
    u16 temp_r1;
    u16 var_r3;

    var_r3 = 0;
    temp_ip = arg0->unk_778 - 1;
    if (temp_ip > 0) {
        do {
            if ((u32) (arg0 + (var_r3 * 2))->unk_7A2 >= 0x3D4U) {

            }
            temp_r1 = var_r3 + 1;
            temp_gt = temp_ip > (s32) temp_r1;
            var_r3 = temp_r1;
        } while (temp_gt);
    }
}

void func_ov043_020afabc(void *arg0) {
    u16 temp_r0_2;
    u16 var_r4;
    u32 temp_r0;

    var_r4 = 0;
    if ((s32) arg0->unk_7B9 > 0) {
        do {
            if (func_02023b7c((arg0 + (var_r4 * 4))->unk_7BC) == 1) {

            }
            temp_r0_2 = var_r4 + 1;
            temp_r0 = temp_r0_2 << 0x10;
            var_r4 = temp_r0_2;
        } while ((s32) arg0->unk_7B9 > (s32) (temp_r0 >> 0x10));
    }
}

void func_ov043_020afb14(void *arg0) {
    s32 temp_gt;
    u16 temp_r1;
    u16 var_r3;
    u8 temp_lr;

    temp_lr = arg0->unk_7B9;
    var_r3 = 0;
    if ((s32) temp_lr > 0) {
        do {
            if ((u32) arg0->unk_7B8 >= (u32) (arg0 + (var_r3 * 4))->unk_7BF) {

            }
            temp_r1 = var_r3 + 1;
            temp_gt = (s32) temp_lr > (s32) temp_r1;
            var_r3 = temp_r1;
        } while (temp_gt);
    }
}

void func_ov043_020afb60(void *arg0) {
    s32 temp_hi;
    s32 temp_r9;
    u16 temp_r0;
    u16 var_r8;

    var_r8 = 0;
    if ((u32) arg0->unk_860 <= 0U) {
        return;
    }
    do {
        temp_r9 = (var_r8 * 0x2C) + (s32) arg0;
        if ((temp_r9->unk_4 == 1) && (((u32) (*(&gSaveData + 0x1EB2 + Inventory_GetCategorizedIndex(temp_r9->unk_0)) << 0x1F) >> 0x1F) == 1)) {
            temp_r9->unk_20 = 1;
        }
        temp_r0 = var_r8 + 1;
        temp_hi = (u32) arg0->unk_860 > (u32) temp_r0;
        var_r8 = temp_r0;
    } while (temp_hi);
}
