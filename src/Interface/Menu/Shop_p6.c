#include "ShopPart.h"

void func_ov043_020afbd4(void *arg0, u16 arg1) {
    s32 temp_gt;
    s32 temp_r8;
    u16 temp_r0;
    u16 temp_r7;
    u16 var_r6;
    u16 var_r9;
    void *temp_r2;

    var_r9 = arg1;
    if ((s32) var_r9 < (s32) arg0->unk_7B9) {
loop_2:
        temp_r7 = (arg0 + (var_r9 * 4))->unk_7BC;
        temp_r8 = Inventory_GetCategorizedIndex(temp_r7);
        var_r6 = 0;
loop_3:
        if ((temp_r7 == (&gSaveData + (var_r6 * 4))->unk_F70) && ((u32) arg0->unk_7B8 >= (u32) (arg0 + (var_r9 * 4))->unk_7BF) && (func_02023b7c(temp_r7) == 0)) {
            temp_r2 = &gSaveData + 0x1EB2;
            *(temp_r2 + temp_r8) = (*(temp_r2 + temp_r8) & ~1) | 1;
            func_ov043_020afb60(arg0);
            arg0->unk_84C = temp_r7;
            return;
        }
        var_r6 += 1;
        if ((u32) var_r6 >= 0x1D8U) {
            temp_r0 = var_r9 + 1;
            temp_gt = (s32) arg0->unk_7B9 > (s32) temp_r0;
            var_r9 = temp_r0;
            if (!temp_gt) {

            } else {
                goto loop_2;
            }
        } else {
            goto loop_3;
        }
    }
}

void func_ov043_020afcb0(void *arg0) {
    s32 temp_gt;
    s32 temp_r6;
    u16 temp_r0;
    u16 temp_r5;
    u16 var_r4;
    void *temp_r2;
    void *temp_r2_2;

    temp_r5 = *(arg0 + (arg0->unk_848 * 0x2C));
    temp_r6 = Inventory_GetCategorizedIndex(temp_r5);
    var_r4 = 0;
    if ((s32) arg0->unk_7B9 > 0) {
loop_1:
        temp_r2 = arg0 + (var_r4 * 4);
        if ((temp_r5 == temp_r2->unk_7BC) && ((u32) arg0->unk_7B8 >= (u32) temp_r2->unk_7BF) && (func_02023b7c(temp_r5) == 0)) {
            temp_r2_2 = &gSaveData + 0x1EB2;
            *(temp_r2_2 + temp_r6) = (*(temp_r2_2 + temp_r6) & ~1) | 1;
            func_ov043_020afb60(arg0);
            arg0->unk_84C = temp_r5;
            return;
        }
        temp_r0 = var_r4 + 1;
        temp_gt = (s32) arg0->unk_7B9 > (s32) temp_r0;
        var_r4 = temp_r0;
        if (!temp_gt) {

        } else {
            goto loop_1;
        }
    }
}

void func_ov043_020afd6c(void *arg0) {
    u16 sp0;
    u16 sp8;
    u16 sp1C;
    u8 sp34;
    u16 sp50;
    s16 temp_r0_2;
    s32 temp_r1_4;
    s32 temp_r1_6;
    s32 temp_r1_8;
    s32 temp_r2_2;
    s32 temp_r2_3;
    s32 temp_r4;
    s32 temp_r6;
    s32 temp_r8;
    s32 var_r1;
    u16 temp_r0;
    u16 temp_r1;
    u16 temp_r4_2;
    u16 temp_r5;
    u16 temp_r9;
    u16 var_r3;
    u16 var_r4;
    u16 var_r8;
    u16 var_r8_2;
    u8 temp_r2_4;
    void *temp_r0_3;
    void *temp_r1_2;
    void *temp_r1_3;
    void *temp_r1_5;
    void *temp_r1_7;
    void *temp_r1_9;
    void *temp_r2;
    void *temp_r5_2;

    temp_r1 = arg0->unk_778;
    temp_r5 = arg0->unk_860;
    if ((u32) temp_r1 < 2U) {
        return;
    }
    temp_r4 = temp_r1 - 2;
    temp_r0 = (arg0 + (temp_r4 * 2))->unk_7A2;
    if ((u32) temp_r0 >= 0x3D4U) {
        return;
    }
    if ((u32) temp_r0 < 0x320U) {
        OS_WaitForever();
    }
    temp_r4_2 = (arg0 + (temp_r4 * 2))->unk_7A2 - 0x320;
    func_ov043_020af5cc(&sp84[0], (s32) temp_r4_2);
    func_ov043_020af770((void *) sp86, arg0->unk_86C);
    temp_r9 = M2C_ERROR(/* Read from unset register $r0 */);
    arg0->unk_84E = temp_r9;
    temp_r8 = Inventory_GetCategorizedIndex(arg0->unk_84E);
    temp_r0_2 = Inventory_GetCategory(arg0->unk_84E);
    temp_r6 = temp_r5 * 0x2C;
    *(arg0 + temp_r6) = temp_r9;
    temp_r2 = arg0 + temp_r6;
    temp_r2->unk_4 = temp_r0_2;
    temp_r2->unk_8 = 0;
    switch (temp_r0_2) {
    default:
        break;
    case 0:
        func_ov043_020af4c4(&sp50, temp_r8);
        func_ov043_020af5f8(&sp34, temp_r8);
        temp_r2_2 = temp_r5 * 0x2C;
        temp_r1_2 = arg0 + temp_r2_2;
        temp_r1_2->unk_2 = sp50;
        temp_r1_2->unk_6 = (s16) sp55;
        temp_r1_2->unk_20 = 0;
        temp_r0_3 = arg0 + 0x29;
        temp_r1_2->unk_24 = (s8) (sp38 + 1);
        temp_r1_2->unk_25 = (s8) (((u32) sp3A >> 0x1F) + ((s32) (sp3A + MULT_HI(0xA03C1689, sp3A)) >> 8));
        temp_r1_2->unk_26 = (s8) ((sp4C >> 0x1F) + ((s32) (sp4C + MULT_HI(0x88888889, sp4C)) >> 5));
        temp_r1_2->unk_27 = sp34;
        temp_r1_2->unk_28 = sp35;
        *(temp_r0_3 + temp_r2_2) = sp36;
        *(temp_r0_3 + temp_r2_2) = sp37;
        break;
    case 1:
        func_ov043_020af4f0(&sp1C, temp_r8);
        temp_r2_3 = (temp_r5 * 0x2C) + (s32) arg0;
        var_r3 = 0;
        temp_r2_3->unk_2 = sp1C;
        temp_r2_3->unk_6 = (s16) sp1E;
        if (((u32) (*(&gSaveData + 0x1EB2 + temp_r8) << 0x1F) >> 0x1F) == 1) {
            var_r1 = 1;
        } else {
            var_r1 = 0;
        }
        (arg0 + temp_r6)->unk_20 = var_r1;
        do {
            temp_r1_3 = arg0 + temp_r6 + var_r3;
            var_r3 += 1;
            temp_r1_3->unk_24 = 0;
        } while ((u32) var_r3 < 7U);
        break;
    case 2:
        func_ov043_020af51c(&sp8, temp_r8);
        temp_r1_4 = (temp_r5 * 0x2C) + (s32) arg0;
        var_r8 = 0;
        temp_r1_4->unk_2 = sp8;
        temp_r1_4->unk_6 = 0xE;
        temp_r1_4->unk_20 = 0;
        do {
            temp_r1_5 = arg0 + temp_r6 + var_r8;
            var_r8 += 1;
            temp_r1_5->unk_24 = 0;
        } while ((u32) var_r8 < 7U);
        break;
    case 3:
        func_ov043_020af548(&sp0, temp_r8);
        temp_r1_6 = (temp_r5 * 0x2C) + (s32) arg0;
        var_r8_2 = 0;
        temp_r1_6->unk_2 = sp0;
        temp_r1_6->unk_6 = 0xE;
        temp_r1_6->unk_20 = 0;
        do {
            temp_r1_7 = arg0 + temp_r6 + var_r8_2;
            var_r8_2 += 1;
            temp_r1_7->unk_24 = 0;
        } while ((u32) var_r8_2 < 7U);
        break;
    }
    temp_r1_8 = (temp_r5 * 0x2C) + (s32) arg0;
    temp_r1_8->unk_10 = 1;
    temp_r1_8->unk_14 = temp_r4_2;
    temp_r5_2 = arg0 + temp_r6;
    var_r4 = 0;
    temp_r1_8->unk_C = sp91;
    do {
        (temp_r5_2 + (var_r4 * 2))->unk_16 = (u16) (&sp84[0])[var_r4].unk_4;
        temp_r2_4 = (&sp84[0] + var_r4)->unk_A;
        temp_r1_9 = temp_r5_2 + var_r4;
        var_r4 += 1;
        temp_r1_9->unk_1C = temp_r2_4;
    } while ((u32) var_r4 < 3U);
    arg0->unk_860 = (u16) (arg0->unk_860 + 1);
}

void func_ov043_020b0088(void *arg0) {
    s16 temp_r0;
    u16 var_ip;

    temp_r0 = Inventory_GetCategory();
    if ((u32) arg0 >= 0x308U) {
        return;
    }
    var_ip = 0;
    if (temp_r0 == 0) {
loop_4:
        if (arg0 == (&gSaveData + (var_ip * 4))->unk_AB0) {

        } else {
            var_ip += 1;
            if ((u32) var_ip >= 0x130U) {

            } else {
                goto loop_4;
            }
        }
    } else {
loop_9:
        if (arg0 == (&gSaveData + (var_ip * 4))->unk_F70) {

        } else {
            var_ip += 1;
            if ((u32) var_ip < 0x1D8U) {
                goto loop_9;
            }
        }
    }
}

void func_ov043_020b0154(void *arg0) {
    s32 temp_r3;
    s8 temp_r1;
    void *temp_ip;
    void *temp_r2;

    temp_ip = &gSaveData + 0xAB2;
    temp_r3 = (s32) arg0 * 4;
    temp_r1 = *(temp_ip + ((s32) arg0 * 4)) - M2C_ERROR(/* Read from unset register $r1 */);
    *(temp_ip + ((s32) arg0 * 4)) = temp_r1;
    if (temp_r1 & 0xFF) {
        return;
    }
    temp_r2 = &gSaveData + 0xAB3;
    *(&gSaveData + 0xAB0 + temp_r3) = 0xFFFF;
    *(temp_r2 + temp_r3) = (u8) ((*(temp_r2 + temp_r3) & ~0x7F) | 0x64) | 0x80;
}

void func_ov043_020b01ac(void *arg0) {
    u8 temp_r3;
    void *temp_ip;

    temp_ip = &gSaveData + 0xF72;
    temp_r3 = *(temp_ip + ((s32) arg0 * 4));
    *(temp_ip + ((s32) arg0 * 4)) = (temp_r3 & ~0xF) | ((u8) (((u32) (temp_r3 << 0x1C) >> 0x1C) - M2C_ERROR(/* Read from unset register $r1 */)) & 0xF);
}

void func_ov043_020b01d8(void *arg0) {
    s16 temp_r0_2;
    s32 temp_r0;
    u16 temp_r5;
    u16 var_r0;
    u16 var_r4;
    u8 temp_r6;

    var_r4 = 0;
    do {
        temp_r0 = (arg0 + (M2C_ERROR(/* Read from unset register $r1 */) * 4))->unk_738;
        temp_r5 = (temp_r0 + (var_r4 * 2))->unk_16;
        temp_r6 = (temp_r0 + var_r4)->unk_1C;
        temp_r0_2 = Inventory_GetCategory(temp_r5);
        if (temp_r6 != 0) {
            var_r0 = 0;
            if (temp_r0_2 == 0) {
loop_4:
                if (temp_r5 == (&gSaveData + (var_r0 * 4))->unk_AB0) {
                    func_ov043_020b0154((void *) var_r0);
                } else {
                    var_r0 += 1;
                    if ((u32) var_r0 >= 0x130U) {

                    } else {
                        goto loop_4;
                    }
                }
            } else {
loop_9:
                if (temp_r5 == (&gSaveData + (var_r0 * 4))->unk_F70) {
                    func_ov043_020b01ac((void *) var_r0);
                } else {
                    var_r0 += 1;
                    if ((u32) var_r0 < 0x1D8U) {
                        goto loop_9;
                    }
                }
            }
        }
        var_r4 += 1;
    } while ((u32) var_r4 < 3U);
}

void func_ov043_020b02ac(u16 arg0) {
    if ((u32) arg0 < 0xCU) {
        return;
    }
    if ((arg0 - (6 * ((arg0 / 6) + (arg0 >> 0x1F)))) == 0) {

    }
}

void func_ov043_020b02fc(void *arg0) {
    void *sp0;
    void *sp4;
    void *sp8;
    void *spC;
    void *sp10;
    s16 temp_r0_12;
    s16 temp_r0_19;
    s16 temp_r0_9;
    s16 temp_r4_3;
    s16 temp_r4_7;
    s16 temp_r5_5;
    s32 temp_gt;
    s32 temp_r0_10;
    s32 temp_r0_13;
    s32 temp_r0_14;
    s32 temp_r0_15;
    s32 temp_r0_20;
    s32 temp_r0_21;
    s32 temp_r0_22;
    s32 temp_r1;
    s32 temp_r1_2;
    s32 temp_r1_3;
    s32 temp_r1_5;
    s32 temp_r1_7;
    s32 temp_r1_9;
    s32 temp_r3;
    s32 temp_r3_2;
    s32 temp_r4;
    s32 temp_r4_2;
    s32 temp_r4_5;
    s32 temp_r4_6;
    s32 temp_r4_9;
    s32 temp_r5;
    u16 temp_r0_17;
    u16 temp_r0_18;
    u16 temp_r0_24;
    u16 temp_r10;
    u16 temp_r10_2;
    u16 temp_r10_3;
    u16 temp_r10_4;
    u16 temp_r4_4;
    u16 temp_r4_8;
    u16 temp_r5_6;
    u16 temp_r8;
    u16 var_r10;
    u16 var_r1;
    u16 var_r1_2;
    u16 var_r2;
    u16 var_r3;
    u16 var_r4;
    u16 var_r5;
    u16 var_r7;
    u16 var_r7_2;
    u16 var_r8;
    u16 var_r8_2;
    u32 temp_r0_8;
    u32 temp_r5_3;
    void *temp_r0;
    void *temp_r0_11;
    void *temp_r0_16;
    void *temp_r0_23;
    void *temp_r0_25;
    void *temp_r0_2;
    void *temp_r0_3;
    void *temp_r0_4;
    void *temp_r0_5;
    void *temp_r0_6;
    void *temp_r0_7;
    void *temp_r1_4;
    void *temp_r1_6;
    void *temp_r1_8;
    void *temp_r2;
    void *temp_r2_2;
    void *temp_r2_3;
    void *temp_r4_10;
    void *temp_r5_2;
    void *temp_r5_4;

    temp_r0 = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    sp10 = temp_r0;
    Mem_SetSequence(&gDebugHeap, sp10, &data_ov043_020cc82c);
    temp_r0_2 = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    spC = temp_r0_2;
    Mem_SetSequence(&gDebugHeap, spC, &data_ov043_020cc838);
    temp_r0_3 = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    sp8 = temp_r0_3;
    Mem_SetSequence(&gDebugHeap, sp8, &data_ov043_020cc844);
    temp_r0_4 = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    sp4 = temp_r0_4;
    Mem_SetSequence(&gDebugHeap, sp4, &data_ov043_020cc850);
    temp_r0_5 = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    sp0 = temp_r0_5;
    Mem_SetSequence(&gDebugHeap, sp0, &data_ov043_020cc860);
    temp_r0_6 = Mem_AllocHeapTail(&gDebugHeap, 0xBA0);
    Mem_SetSequence(&gDebugHeap, temp_r0_6, &data_ov043_020cc870);
    temp_r0_7 = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, temp_r0_7, &data_ov043_020cc880);
    func_ov043_020af624(temp_r0, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af648(temp_r0_2, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af66c(temp_r0_3, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af690(temp_r0_4, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af6b4(temp_r0_5, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af6dc(temp_r0_6, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af700(temp_r0_7, M2C_ERROR(/* Read from unset register $r1 */));
    var_r4 = 0;
    var_r7 = 0;
    do {
        temp_r5 = var_r4 * 0x2C;
        *(arg0 + temp_r5) = 0xFFFF;
        temp_r5_2 = arg0 + temp_r5;
        temp_r5_2->unk_2 = 0;
        temp_r5_2->unk_4 = 0;
        temp_r5_2->unk_6 = 0xE;
        temp_r5_2->unk_8 = 0;
        temp_r5_2->unk_C = 1;
        temp_r5_2->unk_10 = 0;
        temp_r5_2->unk_14 = 0xFFFF;
        temp_r5_2->unk_20 = 0;
        var_r10 = 0;
loop_2:
        (temp_r5_2 + var_r10)->unk_24 = 0;
        var_r10 += 1;
        if ((u32) var_r10 < 7U) {
            goto loop_2;
        }
        var_r8 = 0;
loop_4:
        (temp_r5_2 + (var_r8 * 2))->unk_16 = 0xFFFF;
        (temp_r5_2 + var_r8)->unk_1C = 0;
        var_r8 += 1;
        if ((u32) var_r8 < 3U) {
            goto loop_4;
        }
        var_r4 += 1;
    } while ((u32) var_r4 < 0x2AU);
    var_r8_2 = 0;
    do {
        temp_r1 = var_r8_2 * 8;
        if ((arg0->unk_768 == *(sp0 + temp_r1)) && ((func_ov043_020af770((void *) (sp0 + temp_r1)->unk_2, arg0->unk_86C), temp_r5_3 = M2C_ERROR(/* Read from unset register $r0 */), temp_r10 = (sp0 + (var_r8_2 * 8))->unk_4, (Inventory_GetOwnedCount(0x2AE) != 0)) || ((s32) (gSaveData.unk_1AB0 + 1) >= (s32) (sp0 + (var_r8_2 * 8))->unk_6))) {
            if (temp_r5_3 < 0x320U) {
                temp_r4 = Inventory_GetCategorizedIndex((u16) temp_r5_3);
                temp_r0_9 = Inventory_GetCategory((u16) temp_r5_3);
                temp_r1_2 = var_r7 * 0x2C;
                *(arg0 + temp_r1_2) = (s16) temp_r5_3;
                temp_r2 = arg0 + temp_r1_2;
                temp_r2->unk_4 = temp_r0_9;
                switch (temp_r0_9) {                /* switch 1 */
                default:                            /* switch 1 */
                    break;
                case 0:                             /* switch 1 */
                    temp_r1_3 = temp_r4 * 0x34;
                    temp_r5_4 = sp10 + temp_r1_3;
                    temp_r2->unk_2 = (u16) *(sp10 + temp_r1_3);
                    temp_r0_10 = temp_r4 * 0x1C;
                    temp_r2->unk_6 = (s16) temp_r5_4->unk_5;
                    temp_r1_4 = temp_r0_7 + temp_r0_10;
                    temp_r2->unk_8 = (u32) ((u32) MULTU_HI((temp_r10 * temp_r5_4->unk_8), 0x51EB851F) >> 5);
                    temp_r2->unk_20 = 0;
                    temp_r2->unk_24 = (s8) (temp_r1_4->unk_4 + 1);
                    temp_r5_5 = temp_r1_4->unk_6;
                    temp_r2->unk_25 = (s8) (((u32) temp_r5_5 >> 0x1F) + ((s32) (temp_r5_5 + MULT_HI(0xA03C1689, temp_r5_5)) >> 8));
                    temp_r10_2 = temp_r1_4->unk_18;
                    temp_r2->unk_26 = (s8) ((temp_r10_2 >> 0x1F) + ((s32) (temp_r10_2 + MULT_HI(0x88888889, temp_r10_2)) >> 5));
                    temp_r2->unk_27 = (u8) *(temp_r0_7 + temp_r0_10);
                    temp_r2->unk_28 = (u8) temp_r1_4->unk_1;
                    temp_r2->unk_29 = (u8) temp_r1_4->unk_2;
                    temp_r2->unk_2A = (u8) temp_r1_4->unk_3;
                    break;
                case 1:                             /* switch 1 */
                    temp_r3 = temp_r4 * 0x18;
                    temp_r0_11 = spC + temp_r3;
                    temp_r2->unk_2 = (u16) *(spC + temp_r3);
                    temp_r2->unk_6 = (s16) temp_r0_11->unk_2;
                    temp_r2->unk_8 = (u32) ((u32) MULTU_HI((temp_r10 * temp_r0_11->unk_4), 0x51EB851F) >> 5);
                    if (((u32) (*(&gSaveData + 0x1EB2 + temp_r4) << 0x1F) >> 0x1F) == 1) {
                        temp_r2->unk_20 = 1;
                    } else {
                        temp_r2->unk_20 = 0;
                    }
                    break;
                case 2:                             /* switch 1 */
                    temp_r3_2 = temp_r4 * 0x14;
                    temp_r2->unk_2 = (u16) *(sp8 + temp_r3_2);
                    temp_r2->unk_6 = 0xE;
                    temp_r2->unk_8 = (u32) ((u32) MULTU_HI((temp_r10 * (sp8 + temp_r3_2)->unk_4), 0x51EB851F) >> 5);
                    temp_r2->unk_20 = 0;
                    break;
                case 3:                             /* switch 1 */
                    temp_r2->unk_2 = (u16) *(sp4 + (temp_r4 * 8));
                    temp_r2->unk_6 = 0xE;
                    temp_r2->unk_8 = (u32) ((u32) MULTU_HI((temp_r10 * (sp4 + (temp_r4 * 8))->unk_4), 0x51EB851F) >> 5);
                    temp_r2->unk_20 = 0;
                    break;
                }
                temp_r2->unk_C = 1;
                temp_r2->unk_10 = 0;
                temp_r2->unk_14 = 0xFFFF;
            } else {
                temp_r10_3 = temp_r5_3 - 0x320;
                temp_r5_6 = (temp_r0_6 + (temp_r10_3 * 0x10))->unk_2;
                temp_r4_2 = Inventory_GetCategorizedIndex(temp_r5_6);
                temp_r0_12 = Inventory_GetCategory(temp_r5_6);
                temp_r1_5 = var_r7 * 0x2C;
                *(arg0 + temp_r1_5) = temp_r5_6;
                temp_r2_2 = arg0 + temp_r1_5;
                temp_r2_2->unk_4 = temp_r0_12;
                temp_r2_2->unk_8 = 0;
                switch (temp_r0_12) {               /* switch 2 */
                default:                            /* switch 2 */
                    break;
                case 0:                             /* switch 2 */
                    temp_r0_13 = temp_r4_2 * 0x34;
                    temp_r0_14 = temp_r4_2 * 0x1C;
                    temp_r2_2->unk_2 = (u16) *(sp10 + temp_r0_13);
                    temp_r1_6 = temp_r0_7 + temp_r0_14;
                    temp_r2_2->unk_6 = (s16) (sp10 + temp_r0_13)->unk_5;
                    temp_r2_2->unk_20 = 0;
                    temp_r2_2->unk_24 = (s8) (temp_r1_6->unk_4 + 1);
                    temp_r4_3 = temp_r1_6->unk_6;
                    temp_r2_2->unk_25 = (s8) (((u32) temp_r4_3 >> 0x1F) + ((s32) (temp_r4_3 + MULT_HI(0xA03C1689, temp_r4_3)) >> 8));
                    temp_r4_4 = temp_r1_6->unk_18;
                    temp_r2_2->unk_26 = (s8) ((temp_r4_4 >> 0x1F) + ((s32) (temp_r4_4 + MULT_HI(0x88888889, temp_r4_4)) >> 5));
                    temp_r2_2->unk_27 = (u8) *(temp_r0_7 + temp_r0_14);
                    temp_r2_2->unk_28 = (u8) temp_r1_6->unk_1;
                    temp_r2_2->unk_29 = (u8) temp_r1_6->unk_2;
                    temp_r2_2->unk_2A = (u8) temp_r1_6->unk_3;
                    break;
                case 1:                             /* switch 2 */
                    temp_r0_15 = temp_r4_2 * 0x18;
                    temp_r2_2->unk_2 = (u16) *(spC + temp_r0_15);
                    temp_r2_2->unk_6 = (s16) (spC + temp_r0_15)->unk_2;
                    if (((u32) (*(&gSaveData + 0x1EB2 + temp_r4_2) << 0x1F) >> 0x1F) == 1) {
                        temp_r2_2->unk_20 = 1;
                    } else {
                        temp_r2_2->unk_20 = 0;
                    }
                    break;
                case 2:                             /* switch 2 */
                    temp_r2_2->unk_2 = (u16) *(sp8 + (temp_r4_2 * 0x14));
                    temp_r2_2->unk_6 = 0xE;
                    temp_r2_2->unk_20 = 0;
                    break;
                case 3:                             /* switch 2 */
                    temp_r2_2->unk_2 = (u16) *(sp4 + (temp_r4_2 * 8));
                    temp_r2_2->unk_6 = 0xE;
                    temp_r2_2->unk_20 = 0;
                    break;
                }
                temp_r4_5 = (var_r7 * 0x2C) + (s32) arg0;
                temp_r4_5->unk_10 = 1;
                temp_r4_5->unk_14 = temp_r10_3;
                temp_r0_16 = temp_r0_6 + (temp_r10_3 * 0x10);
                var_r1 = 0;
                temp_r4_5->unk_C = (u8) temp_r0_16->unk_D;
                do {
                    (temp_r2_2 + (var_r1 * 2))->unk_16 = (u16) (temp_r0_16 + (var_r1 * 2))->unk_4;
                    (temp_r2_2 + var_r1)->unk_1C = (u8) (var_r1 + temp_r0_16)->unk_A;
                    var_r1 += 1;
                } while ((u32) var_r1 < 3U);
            }
            var_r7 += 1;
            if ((u32) var_r7 >= 0x2AU) {
                OS_WaitForever();
            }
        }
        temp_r0_17 = var_r8_2 + 1;
        temp_r0_8 = temp_r0_17 << 0x10;
        var_r8_2 = temp_r0_17;
    } while ((u32) (temp_r0_8 >> 0x10) < 0x23AU);
    var_r5 = 0;
    if ((s32) (arg0->unk_778 - 1) > 0) {
        do {
            temp_r0_18 = (arg0 + (var_r5 * 2))->unk_7A2;
            if ((u32) temp_r0_18 < 0x3D4U) {
                if ((u32) temp_r0_18 < 0x320U) {
                    OS_WaitForever();
                }
                temp_r10_4 = (arg0 + (var_r5 * 2))->unk_7A2 - 0x320;
                func_ov043_020af770((void *) (temp_r0_6 + (temp_r10_4 * 0x10))->unk_2, arg0->unk_86C);
                temp_r8 = M2C_ERROR(/* Read from unset register $r0 */);
                temp_r4_6 = Inventory_GetCategorizedIndex();
                temp_r0_19 = Inventory_GetCategory(temp_r8);
                temp_r1_7 = var_r7 * 0x2C;
                *(arg0 + temp_r1_7) = temp_r8;
                temp_r2_3 = arg0 + temp_r1_7;
                temp_r2_3->unk_4 = temp_r0_19;
                temp_r2_3->unk_8 = 0;
                switch (temp_r0_19) {               /* switch 3 */
                default:                            /* switch 3 */
                    break;
                case 0:                             /* switch 3 */
                    temp_r0_20 = temp_r4_6 * 0x34;
                    temp_r0_21 = temp_r4_6 * 0x1C;
                    temp_r2_3->unk_2 = (u16) *(sp10 + temp_r0_20);
                    temp_r1_8 = temp_r0_7 + temp_r0_21;
                    temp_r2_3->unk_6 = (s16) (sp10 + temp_r0_20)->unk_5;
                    temp_r2_3->unk_20 = 0;
                    temp_r2_3->unk_24 = (s8) (temp_r1_8->unk_4 + 1);
                    temp_r4_7 = temp_r1_8->unk_6;
                    temp_r2_3->unk_25 = (s8) (((u32) temp_r4_7 >> 0x1F) + ((s32) (temp_r4_7 + MULT_HI(0xA03C1689, temp_r4_7)) >> 8));
                    temp_r4_8 = temp_r1_8->unk_18;
                    temp_r2_3->unk_26 = (s8) ((temp_r4_8 >> 0x1F) + ((s32) (temp_r4_8 + MULT_HI(0x88888889, temp_r4_8)) >> 5));
                    temp_r2_3->unk_27 = (u8) *(temp_r0_7 + temp_r0_21);
                    temp_r2_3->unk_28 = (u8) temp_r1_8->unk_1;
                    temp_r2_3->unk_29 = (u8) temp_r1_8->unk_2;
                    temp_r2_3->unk_2A = (u8) temp_r1_8->unk_3;
                    break;
                case 1:                             /* switch 3 */
                    temp_r0_22 = temp_r4_6 * 0x18;
                    temp_r2_3->unk_2 = (u16) *(spC + temp_r0_22);
                    temp_r2_3->unk_6 = (s16) (spC + temp_r0_22)->unk_2;
                    if (((u32) (*(&gSaveData + 0x1EB2 + temp_r4_6) << 0x1F) >> 0x1F) == 1) {
                        temp_r2_3->unk_20 = 1;
                    } else {
                        temp_r2_3->unk_20 = 0;
                    }
                    break;
                case 2:                             /* switch 3 */
                    temp_r2_3->unk_2 = (u16) *(sp8 + (temp_r4_6 * 0x14));
                    temp_r2_3->unk_6 = 0xE;
                    temp_r2_3->unk_20 = 0;
                    break;
                case 3:                             /* switch 3 */
                    temp_r2_3->unk_2 = (u16) *(sp4 + (temp_r4_6 * 8));
                    temp_r2_3->unk_6 = 0xE;
                    temp_r2_3->unk_20 = 0;
                    break;
                }
                temp_r4_9 = (var_r7 * 0x2C) + (s32) arg0;
                temp_r4_9->unk_10 = 1;
                temp_r4_9->unk_14 = temp_r10_4;
                temp_r0_23 = temp_r0_6 + (temp_r10_4 * 0x10);
                var_r1_2 = 0;
                temp_r4_9->unk_C = (u8) temp_r0_23->unk_D;
                do {
                    (temp_r2_3 + (var_r1_2 * 2))->unk_16 = (u16) (temp_r0_23 + (var_r1_2 * 2))->unk_4;
                    (temp_r2_3 + var_r1_2)->unk_1C = (u8) (var_r1_2 + temp_r0_23)->unk_A;
                    var_r1_2 += 1;
                } while ((u32) var_r1_2 < 3U);
                var_r7 += 1;
            }
            temp_r0_24 = var_r5 + 1;
            temp_gt = (s32) (arg0->unk_778 - 1) > (s32) temp_r0_24;
            var_r5 = temp_r0_24;
        } while (temp_gt);
    }
    arg0->unk_860 = var_r7;
    func_ov043_020b02ac(arg0->unk_860);
    arg0->unk_862 = (u16) M2C_ERROR(/* Read from unset register $r0 */);
    var_r3 = 0;
    arg0->unk_870 = (u16) (((arg0->unk_862 / 6) + ((u16) arg0->unk_862 >> 0x1F)) - 2);
    var_r7_2 = 0;
    arg0->unk_872 = (u16) arg0->unk_870;
    var_r2 = 0;
    do {
        temp_r4_10 = arg0 + (var_r7_2 * 4);
        temp_r4_10->unk_7BC = 0xFFFF;
        temp_r4_10->unk_7BE = 0;
        temp_r4_10->unk_7BF = 0xA;
        var_r7_2 += 1;
    } while ((u32) var_r7_2 < 0x1EU);
    do {
        temp_r1_9 = (var_r2 * 0x18) + (s32) spC;
        if (arg0->unk_774 == temp_r1_9->unk_12) {
            temp_r0_25 = arg0 + (var_r3 * 4);
            temp_r0_25->unk_7BC = (s16) (var_r2 + 0x130);
            temp_r0_25->unk_7BE = (u8) temp_r1_9->unk_10;
            var_r3 += 1;
            temp_r0_25->unk_7BF = (s8) temp_r1_9->unk_14;
        }
        var_r2 += 1;
    } while ((u32) var_r2 < 0x118U);
    arg0->unk_7B9 = (s8) var_r3;
    Mem_Free(&gDebugHeap, sp10);
    Mem_Free(&gDebugHeap, spC);
    Mem_Free(&gDebugHeap, sp8);
    Mem_Free(&gDebugHeap, sp4);
    Mem_Free(&gDebugHeap, sp0);
    Mem_Free(&gDebugHeap, temp_r0_6);
    Mem_Free(&gDebugHeap, temp_r0_7);
}

void func_ov043_020b0d18(void) {

}

void func_ov043_020b0d1c(void *arg0) {
    u16 sp0;
    ? sp2C;
    s32 var_ip;
    s32 var_r2;
    s8 var_r0;
    u16 *temp_r2;
    u16 var_r1;
    u32 temp_r1;
    void *temp_r0;
    void *temp_r0_2;
    void *var_r1_2;

    func_ov043_020af724(arg0, M2C_ERROR(/* Read from unset register $r1 */));
    arg0->unk_858 = (s32) gSaveData.unk_C;
    if (((u32) (gSaveData.unk_19 << 0x19) >> 0x1F) == 1) {
        var_r1 = arg0->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    arg0->unk_834 = var_r1;
    arg0->unk_768 = (u16) gSaveData.unk_1AB2;
    func_ov043_020af574(&sp2C, (s32) arg0->unk_768);
    arg0->unk_76A = (s16) (sp2E + 0x3F);
    arg0->unk_76C = (s16) (sp30 + 0x1F);
    arg0->unk_76E = sp32;
    arg0->unk_770 = sp36;
    arg0->unk_774 = sp34;
    func_ov043_020af5a0(&sp0, (s32) sp34);
    arg0->unk_776 = sp0;
    var_ip = 0;
    arg0->unk_77C = (u32) *(&gSaveData + 0x1A18 + (arg0->unk_774 * 4));
    arg0->unk_784 = sp4;
    arg0->unk_78E = sp2;
    temp_r1 = arg0->unk_784;
    if ((u32) arg0->unk_77C > temp_r1) {
        arg0->unk_77C = temp_r1;
    }
    do {
        temp_r2 = &(&sp0)[var_ip];
        temp_r0 = arg0 + (var_ip * 2);
        temp_r0->unk_790 = (u16) temp_r2->unk_8;
        var_ip += 1;
        temp_r0->unk_7A2 = (u16) temp_r2->unk_1A;
    } while (var_ip < 9);
    func_ov043_020af8a8(arg0, (s32) arg0->unk_77C);
    arg0->unk_788 = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af8e4(arg0, (s32) (arg0 + 0x700));
    arg0->unk_778 = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af950(arg0, (s32) (arg0 + 0x700));
    arg0->unk_77A = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af994(arg0);
    arg0->unk_780 = (s32) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af9e8(arg0);
    arg0->unk_78C = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af8a8(arg0, arg0->unk_780);
    arg0->unk_78A = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020b1e2c(arg0);
    if (M2C_ERROR(/* Read from unset register $r0 */) == 0) {
        var_r0 = 0;
    } else {
        var_r0 = 1;
    }
    arg0->unk_7B6 = var_r0;
    func_ov043_020b1ecc(arg0, arg0->unk_774);
    arg0->unk_7B7 = (s8) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020b02fc(arg0);
    func_ov043_020afa68(arg0);
    arg0->unk_7B8 = (s8) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020afabc(arg0);
    arg0->unk_7B4 = (s8) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020afb14(arg0);
    var_r1_2 = arg0;
    arg0->unk_7B5 = (s8) M2C_ERROR(/* Read from unset register $r0 */);
    var_r2 = 0;
    do {
        temp_r0_2 = arg0 + (var_r2 * 4);
        var_r2 += 1;
        temp_r0_2->unk_738 = var_r1_2;
        var_r1_2 += 0x2C;
    } while (var_r2 < 0xC);
    func_ov043_020b0d18();
}

void func_ov043_020b0f20(void *arg0) {
    void *sp0;
    void *sp4;
    void *sp8;
    void *spC;
    s16 temp_ip;
    s16 temp_r0_9;
    s32 temp_r0_10;
    s32 temp_r0_12;
    s32 temp_r0_14;
    s32 temp_r0_7;
    s32 temp_r0_8;
    s32 temp_r11;
    s32 temp_r1;
    s32 temp_r1_2;
    s32 temp_r2;
    s32 temp_r2_3;
    s32 temp_r3_2;
    s32 temp_r4;
    s32 temp_r9;
    u16 temp_ip_2;
    u16 temp_r8;
    u16 temp_r9_3;
    u16 var_ip;
    u16 var_r11;
    u16 var_r5;
    u16 var_r8;
    u16 var_r8_2;
    u16 var_r9;
    void *temp_r0;
    void *temp_r0_11;
    void *temp_r0_13;
    void *temp_r0_15;
    void *temp_r0_2;
    void *temp_r0_3;
    void *temp_r0_4;
    void *temp_r0_5;
    void *temp_r0_6;
    void *temp_r2_2;
    void *temp_r3;
    void *temp_r9_2;

    temp_r0 = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    spC = temp_r0;
    Mem_SetSequence(&gDebugHeap, spC, &data_ov043_020cc82c);
    temp_r0_2 = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    sp8 = temp_r0_2;
    Mem_SetSequence(&gDebugHeap, sp8, &data_ov043_020cc838);
    temp_r0_3 = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    sp4 = temp_r0_3;
    Mem_SetSequence(&gDebugHeap, sp4, &data_ov043_020cc844);
    temp_r0_4 = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    Mem_SetSequence(&gDebugHeap, temp_r0_4, &data_ov043_020cc850);
    temp_r0_5 = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    sp0 = temp_r0_5;
    Mem_SetSequence(&gDebugHeap, sp0, &data_ov043_020cc860);
    temp_r0_6 = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, temp_r0_6, &data_ov043_020cc880);
    func_ov043_020af624(temp_r0, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af648(temp_r0_2, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af66c(temp_r0_3, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af690(temp_r0_4, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af6b4(temp_r0_5, M2C_ERROR(/* Read from unset register $r1 */));
    func_ov043_020af700(temp_r0_6, M2C_ERROR(/* Read from unset register $r1 */));
    var_r8 = 0;
    var_r5 = 0;
    do {
        temp_r9 = var_r8 * 0x2C;
        *(arg0 + temp_r9) = 0xFFFF;
        temp_r9_2 = arg0 + temp_r9;
        temp_r9_2->unk_2 = 0;
        temp_r9_2->unk_4 = 0;
        temp_r9_2->unk_6 = 0xE;
        temp_r9_2->unk_8 = 0;
        temp_r9_2->unk_C = 1;
        temp_r9_2->unk_10 = 0;
        var_ip = 0;
loop_2:
        (temp_r9_2 + var_ip)->unk_24 = 0;
        var_ip += 1;
        if ((u32) var_ip < 7U) {
            goto loop_2;
        }
        var_r11 = 0;
loop_4:
        (temp_r9_2 + (var_r11 * 2))->unk_16 = 0xFFFF;
        (temp_r9_2 + var_r11)->unk_1C = 0;
        var_r11 += 1;
        if ((u32) var_r11 < 3U) {
            goto loop_4;
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x2AU);
    var_r9 = 0;
    do {
        temp_r8 = (&gSaveData + (var_r9 * 2))->unk_1AF6;
        temp_r0_7 = Inventory_GetCategorizedIndex(temp_r8);
        if (temp_r8 != 0xFFFF) {
            temp_r11 = (temp_r0_7 * 0x34) + (s32) spC;
            temp_r2 = temp_r11->unk_8;
            if ((temp_r2 != 0) && (temp_r2 != 0x989680)) {
                temp_r1 = temp_r0_7 * 0x1C;
                temp_r0_8 = var_r5 * 0x2C;
                *(arg0 + temp_r0_8) = temp_r8;
                temp_r3 = arg0 + temp_r0_8;
                temp_r3->unk_2 = (u16) temp_r11->unk_0;
                temp_r3->unk_4 = 0;
                var_r5 += 1;
                temp_r2_2 = temp_r0_6 + temp_r1;
                temp_r3->unk_6 = (s16) temp_r11->unk_5;
                temp_r3->unk_8 = (s32) temp_r11->unk_8;
                temp_r3->unk_20 = 0;
                temp_r3->unk_24 = (s8) (temp_r2_2->unk_4 + 1);
                temp_ip = temp_r2_2->unk_6;
                temp_r3->unk_25 = (s8) (((u32) temp_ip >> 0x1F) + ((s32) (temp_ip + MULT_HI(0xA03C1689, temp_ip)) >> 8));
                temp_ip_2 = temp_r2_2->unk_18;
                temp_r3->unk_26 = (s8) ((temp_ip_2 >> 0x1F) + ((s32) (temp_ip_2 + MULT_HI(0x88888889, temp_ip_2)) >> 5));
                temp_r3->unk_27 = (u8) *(temp_r0_6 + temp_r1);
                temp_r3->unk_28 = (u8) temp_r2_2->unk_1;
                temp_r3->unk_29 = (u8) temp_r2_2->unk_2;
                temp_r3->unk_2A = (u8) temp_r2_2->unk_3;
            }
        }
        var_r9 += 1;
    } while ((u32) var_r9 < 6U);
    var_r8_2 = 0;
    do {
        temp_r9_3 = (&gSaveData + (var_r8_2 * 2))->unk_1B02;
        temp_r4 = Inventory_GetCategorizedIndex(temp_r9_3);
        temp_r0_9 = Inventory_GetCategory(temp_r9_3);
        if (temp_r9_3 != 0xFFFF) {
            temp_r1_2 = (temp_r4 * 0x18) + (s32) sp8;
            temp_r3_2 = temp_r1_2->unk_4;
            if ((temp_r3_2 != 0) && (temp_r3_2 != 0x989680)) {
                switch (temp_r0_9) {                /* irregular */
                case 1:
                    temp_r0_10 = var_r5 * 0x2C;
                    *(arg0 + temp_r0_10) = temp_r9_3;
                    temp_r0_11 = arg0 + temp_r0_10;
                    temp_r0_11->unk_2 = (u16) temp_r1_2->unk_0;
                    temp_r0_11->unk_4 = 1;
                    temp_r0_11->unk_6 = (s16) temp_r1_2->unk_2;
                    temp_r0_11->unk_8 = (s32) temp_r1_2->unk_4;
                    if (((u32) (*(&gSaveData + 0x1EB2 + temp_r4) << 0x1F) >> 0x1F) == 1) {
                        temp_r0_11->unk_20 = 1;
                    } else {
                        temp_r0_11->unk_20 = 0;
                    }
                    break;
                case 2:
                    temp_r2_3 = temp_r4 * 0x14;
                    temp_r0_12 = var_r5 * 0x2C;
                    *(arg0 + temp_r0_12) = temp_r9_3;
                    temp_r0_13 = arg0 + temp_r0_12;
                    temp_r0_13->unk_2 = (u16) *(sp4 + temp_r2_3);
                    temp_r0_13->unk_4 = 2;
                    temp_r0_13->unk_6 = 0xE;
                    temp_r0_13->unk_8 = (s32) (sp4 + temp_r2_3)->unk_4;
                    temp_r0_13->unk_20 = 0;
                    break;
                case 3:
                    temp_r0_14 = var_r5 * 0x2C;
                    *(arg0 + temp_r0_14) = temp_r9_3;
                    temp_r0_15 = arg0 + temp_r0_14;
                    temp_r0_15->unk_2 = (u16) *(temp_r0_4 + (temp_r4 * 8));
                    temp_r0_15->unk_4 = 3;
                    temp_r0_15->unk_6 = 0xE;
                    temp_r0_15->unk_8 = (s32) (temp_r0_4 + (temp_r4 * 8))->unk_4;
                    temp_r0_15->unk_20 = 0;
                    break;
                default:
                    OS_WaitForever();
                    break;
                }
                var_r5 += 1;
            }
        }
        var_r8_2 += 1;
    } while ((u32) var_r8_2 < 0x10U);
    arg0->unk_860 = var_r5;
    func_ov043_020b02ac(arg0->unk_860);
    arg0->unk_862 = (u16) M2C_ERROR(/* Read from unset register $r0 */);
    arg0->unk_870 = (u16) (((arg0->unk_862 / 6) + ((u16) arg0->unk_862 >> 0x1F)) - 2);
    arg0->unk_872 = (u16) arg0->unk_870;
    Mem_Free(&gDebugHeap, spC);
    Mem_Free(&gDebugHeap, sp8);
    Mem_Free(&gDebugHeap, sp4);
    Mem_Free(&gDebugHeap, temp_r0_4);
    Mem_Free(&gDebugHeap, sp0);
    Mem_Free(&gDebugHeap, temp_r0_6);
}

void func_ov043_020b1404(void *arg0) {
    u16 sp0;
    ? sp2C;
    s32 var_ip;
    s32 var_r2;
    u16 *temp_r2;
    u16 var_r1;
    void *temp_r0;
    void *temp_r0_2;
    void *var_r1_2;

    arg0->unk_858 = (s32) gSaveData.unk_C;
    if (((u32) (gSaveData.unk_19 << 0x19) >> 0x1F) == 1) {
        var_r1 = arg0->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    arg0->unk_834 = var_r1;
    arg0->unk_768 = (u16) ((u32) (gSaveData.unk_1AF2 << 0x1A) >> 0x1A);
    func_ov043_020af574(&sp2C, (s32) arg0->unk_768);
    arg0->unk_76A = (s16) (sp2E + 0x3F);
    arg0->unk_76C = (s16) (((u32) (gSaveData.unk_1AF2 << 0x11) >> 0x1D) + 0x1F);
    arg0->unk_76E = (u16) gSaveData.unk_1AF4;
    arg0->unk_770 = sp36;
    arg0->unk_774 = (u16) ((u32) (gSaveData.unk_1AF2 << 0x14) >> 0x1A);
    func_ov043_020af5a0(&sp0, (s32) arg0->unk_774);
    var_ip = 0;
    arg0->unk_776 = sp0;
    arg0->unk_77C = sp4;
    arg0->unk_784 = sp4;
    arg0->unk_78E = sp2;
    do {
        temp_r2 = &(&sp0)[var_ip];
        temp_r0 = arg0 + (var_ip * 2);
        temp_r0->unk_790 = (u16) temp_r2->unk_8;
        var_ip += 1;
        temp_r0->unk_7A2 = (u16) temp_r2->unk_1A;
    } while (var_ip < 9);
    func_ov043_020af8a8(arg0, arg0->unk_77C);
    arg0->unk_788 = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af8e4(arg0, (s32) (arg0 + 0x700));
    arg0->unk_778 = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af950(arg0, (s32) (arg0 + 0x700));
    arg0->unk_77A = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af994(arg0);
    arg0->unk_780 = (s32) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af9e8(arg0);
    arg0->unk_78C = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020af8a8(arg0, arg0->unk_780);
    arg0->unk_78A = (s16) M2C_ERROR(/* Read from unset register $r0 */);
    arg0->unk_7B4 = 0;
    arg0->unk_7B5 = 0;
    arg0->unk_7B6 = 0;
    func_ov043_020b1ecc(arg0, arg0->unk_774);
    arg0->unk_7B7 = (s8) M2C_ERROR(/* Read from unset register $r0 */);
    func_ov043_020b0f20(arg0);
    var_r2 = 0;
    var_r1_2 = arg0;
    do {
        temp_r0_2 = arg0 + (var_r2 * 4);
        var_r2 += 1;
        temp_r0_2->unk_738 = var_r1_2;
        var_r1_2 += 0x2C;
    } while (var_r2 < 0xC);
}

void func_ov043_020b15d0(void *arg0) {
    u8 var_r0;

    gSaveData.unk_C = (s32) arg0->unk_858;
    *(&gSaveData + 0x1A18 + (arg0->unk_774 * 4)) = arg0->unk_77C;
    if (arg0->unk_834 & 4) {
        var_r0 = gSaveData.unk_19 | 0x40;
    } else {
        var_r0 = gSaveData.unk_19 & ~0x40;
    }
    gSaveData.unk_19 = var_r0;
}

void func_ov043_020b1618(void *arg0) {
    s32 temp_ip;
    s32 temp_r2;
    s32 temp_r4;
    u32 var_r2;
    u32 var_r3;
    void *temp_lr;

    gSaveData.unk_C = (s32) arg0->unk_858;
    temp_r4 = arg0->unk_868;
    if (temp_r4 != 0) {
        temp_ip = gSaveData.unk_1AB9 * 0xC;
        temp_lr = &gSaveData + 0x1B2C;
        temp_r2 = *(temp_lr + temp_ip);
        var_r3 = (temp_r2 & ~0xFF000000) + temp_r4;
        if (var_r3 > 0x98967FU) {
            var_r3 = 0x98967F;
        }
        var_r2 = ((u32) (temp_r2 & 0xFF000000) >> 0x18) + 1;
        if (var_r2 > 0xFFU) {
            var_r2 = 0;
        }
        *(temp_lr + temp_ip) = (var_r3 & ~0xFF000000) + (var_r2 << 0x18);
    }
    if (!(arg0->unk_834 & 4)) {
        gSaveData.unk_19 = (u8) (gSaveData.unk_19 & ~0x40);
        return;
    }
    gSaveData.unk_19 = (u8) (gSaveData.unk_19 | 0x40);
}
