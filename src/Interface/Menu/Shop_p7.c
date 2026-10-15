#include "ShopPart.h"

// NOTE: mirrors for the data_ov043_020ca260 layout table are declared locally in
// each function that reads it (the header only exposes it as a bare `int`).

s32  func_ov043_020af3f0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s16 arg4, s16 arg5);
void func_ov043_020b01d8(void *arg0, s32 arg1);
u16  func_ov043_020b0088(u16 itemID);
s32  func_02023d88(u32 arg0);

s32 func_ov043_020b16c4(ShopObject *arg0, s32 arg1) {
    ShopItemSlot *slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    u16           itemID;
    s32           unk_10;

    itemID = slot->itemID;
    unk_10 = slot->unk_10;
    if (slot->unk_04 == 0) {
        if (Inventory_AddItem(itemID, 1) == 1) {
            if (unk_10 == 1) {
                func_ov043_020b01d8(arg0, arg1);
                arg0->unk_834 = arg0->unk_834 | 0x40;
            }
            return 1;
        }
    } else if ((Inventory_GetOwnedCount(itemID) < 9U) && (Inventory_AddItem(itemID, 0) == 1)) {
        if (unk_10 == 1) {
            func_ov043_020b01d8(arg0, arg1);
            arg0->unk_834 = arg0->unk_834 | 0x40;
        }
        return 1;
    }
    return 0;
}

s32 func_ov043_020b1780(ShopObject *arg0, s32 arg1) {
    ShopItemSlot *slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    u16           itemID;

    itemID = slot->itemID;
    if (slot->unk_04 == 0) {
        if (Inventory_CanAddPin(itemID, 1) == 1) {
            return 1;
        }
    } else if (Inventory_CanAddPin(itemID, 0) == 1) {
        return 1;
    }
    return 0;
}

// Nonmatching: the four loaded values are scheduled w,x,y,h (target) vs w,x,h,y,
// with a register rotation (base/last load lands in r2 in the target).
s32 func_ov043_020b17d0(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x08];
        /* 0x08 */ u16  unk_8;
        /* 0x0A */ u16  unk_A;
        /* 0x0C */ char pad_0C[0x10 - 0x0C];
        /* 0x10 */ u16  unk_10;
        /* 0x12 */ u16  unk_12;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_10;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_8;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_12;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_A;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

// Nonmatching: array-fill load/store pairing and register allocation differ.
s32 func_ov043_020b1830(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x04];
        /* 0x04 */ u16  unk_4;
        /* 0x06 */ u16  unk_6;
        /* 0x08 */ char pad_08[0x3C - 0x08];
        /* 0x3C */ u16  unk_3C;
        /* 0x3E */ u16  unk_3E;
        /* 0x40 */ u16  unk_40;
        /* 0x42 */ u16  unk_42;
        /* 0x44 */ u16  unk_44;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    u16            sp8;
    u16            spA;
    s16            spC[5];
    s16            i;

    spA = t->unk_6;
    sp8 = t->unk_4;
    spC[4] = t->unk_44;
    spC[3] = t->unk_42;
    spC[0] = t->unk_3C;
    spC[1] = t->unk_3E;
    spC[2] = t->unk_40;
    for (i = 0; i < 5; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i], 0xB0, (s16) sp8, (s16) spA) == 1) {
            return i;
        }
    }
    return -1;
}

// Nonmatching: see func_ov043_020b17d0 (load scheduling + register rotation).
s32 func_ov043_020b18e4(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x24];
        /* 0x24 */ u16  unk_24;
        /* 0x26 */ u16  unk_26;
        /* 0x28 */ u16  unk_28;
        /* 0x2A */ u16  unk_2A;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_24;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_28;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_26;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_2A;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

// Nonmatching: loop-counter/point registers rotated (i/h/w land in r8/r6/r7
// instead of the target's r6/r7/r8); otherwise identical.
s32 func_ov043_020b1944(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x20];
        /* 0x20 */ u16  unk_20;
        /* 0x22 */ u16  unk_22;
    } ShopP7_Layout;
    volatile u16   spA;
    volatile u16   sp8;
    s16            h;
    s16            w;
    s16            i;
    s16            x;
    s16            y;
    s16            w2;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_20;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_22;
    w = sp8;
    h = spA;
    for (i = 0; i < 12; i++) {
        x = (i % 6) * 33 + 6;
        y = (i / 6) * 41 + 23;
        w2 = (s16) w - ((x == 0xAB) ? 2 : 0);
        if (func_ov043_020af3f0(arg0, arg1, x, y, w2, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

s32 func_ov043_020b1a1c(ShopObject *arg0, s32 arg1) {
    u16 i;
    u8  count;
    u16 itemID;

    for (i = 0; i < 3; i++) {
        count = ((ShopItemSlot *) (arg0->unk_738[arg1]))->tradeInCounts[i];
        itemID = ((ShopItemSlot *) (arg0->unk_738[arg1]))->tradeInItemIDs[i];
        if (count != 0) {
            if ((s32) func_ov043_020b0088(itemID) < (s32) count) {
                return 0;
            }
        }
    }
    return 1;
}

s32 func_ov043_020b1a70(ShopObject *arg0, s16 arg1) {
    ShopItemSlot *slot;

    if (arg1 == -1) {
        return 0;
    }
    slot = (ShopItemSlot *) (arg0->unk_738[arg1]);
    if (slot->unk_0C[0] == 1) {
        if (slot->unk_10 == 0) {
            if (slot->cost <= arg0->unk_858) {
                return 1;
            }
        } else {
            if (func_ov043_020b1a1c(arg0, arg1) == 1) {
                return 1;
            }
        }
    } else if ((slot->unk_10 == 1) && (func_02023d88(slot->unk_14) == 0)) {
        if (func_ov043_020b1a1c(arg0, arg1) == 1) {
            return 1;
        }
    }
    return 0;
}

// Nonmatching: see func_ov043_020b17d0 (load scheduling + register rotation).
s32 func_ov043_020b1b14(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x18];
        /* 0x18 */ u16  unk_18;
        /* 0x1A */ u16  unk_1A;
        /* 0x1C */ u16  unk_1C;
        /* 0x1E */ u16  unk_1E;
    } ShopP7_Layout;
    volatile s16   spE;
    volatile s16   spC;
    volatile s16   spA;
    volatile s16   sp8;
    s16            a;
    s16            b;
    s16            c;
    s16            d;

    sp8 = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_18;
    a = sp8;
    spC = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1C;
    spA = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1A;
    spE = ((ShopP7_Layout *) &data_ov043_020ca260)->unk_1E;
    b = spA;
    c = spC;
    d = spE;
    return func_ov043_020af3f0(arg0, arg1, c, d, a, b) == 1;
}

s32 func_ov043_020b1b74(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    typedef struct {
        /* 0x00 */ u16 unk_0;
        /* 0x02 */ u16 unk_2;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            a;
    s16            b;

    sp8 = t->unk_0;
    a = sp8;
    spA = t->unk_2;
    b = spA;
    return func_ov043_020af3f0(arg0, arg1, (s16) (arg2 - 8), (s16) (arg3 - 21), a, b) == 1;
}

// Nonmatching: array-fill load pairing and register allocation differ.
s32 func_ov043_020b1bd4(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x0C];
        /* 0x0C */ u16  unk_C;
        /* 0x0E */ u16  unk_E;
        /* 0x10 */ char pad_10[0x2C - 0x10];
        /* 0x2C */ u16  unk_2C;
        /* 0x2E */ u16  unk_2E;
        /* 0x30 */ u16  unk_30;
        /* 0x32 */ u16  unk_32;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            w;
    s16            h;
    s16            spC[2][2];
    s16            i;

    spC[0][0] = t->unk_2C;
    spC[0][1] = t->unk_2E;
    spC[1][0] = t->unk_30;
    spC[1][1] = t->unk_32;
    spA = t->unk_E;
    sp8 = t->unk_C;
    w = sp8;
    h = spA;
    for (i = 0; i < 2; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i][0], spC[i][1], (s16) w, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

// Nonmatching: array-fill load pairing and register allocation differ.
s32 func_ov043_020b1c7c(s16 arg0, s16 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x14];
        /* 0x14 */ u16  unk_14;
        /* 0x16 */ u16  unk_16;
        /* 0x18 */ char pad_18[0x34 - 0x18];
        /* 0x34 */ u16  unk_34;
        /* 0x36 */ u16  unk_36;
        /* 0x38 */ u16  unk_38;
        /* 0x3A */ u16  unk_3A;
    } ShopP7_Layout;
    ShopP7_Layout *t = (ShopP7_Layout *) &data_ov043_020ca260;
    volatile u16   spA;
    volatile u16   sp8;
    s16            w;
    s16            h;
    s16            spC[2][2];
    s16            i;

    spC[0][0] = t->unk_34;
    spC[0][1] = t->unk_36;
    spC[1][0] = t->unk_38;
    spC[1][1] = t->unk_3A;
    spA = t->unk_16;
    sp8 = t->unk_14;
    h = spA;
    w = sp8;
    for (i = 0; i < 2; i++) {
        if (func_ov043_020af3f0(arg0, arg1, spC[i][0], spC[i][1], (s16) w, (s16) h) == 1) {
            return i;
        }
    }
    return -1;
}

s32 func_ov043_020b1d24(u8 arg0) {
    switch (arg0) {
    case 1:
    case 2:
    case 3:
    case 4:
        return 0;
    case 6:
    case 7:
    case 8:
    case 9:
        return 5;
    case 14:
    case 15:
    case 16:
    case 17:
        return 13;
    case 19:
    case 20:
    case 21:
    case 22:
        return 18;
    case 26:
    case 27:
    case 28:
    case 29:
        return 25;
    case 31:
    case 32:
    case 33:
    case 34:
        return 30;
    case 43:
    case 44:
    case 45:
    case 46:
        return 42;
    default:
        return -1;
    }
}

s32 func_ov043_020b1e2c(ShopObject *arg0) {
    switch (arg0->unk_776) {
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 22:
    case 29:
        return 0;
    default:
        return arg0->unk_788 >= 0x3E8U;
    }
}

s32 func_ov043_020b1ecc(void *arg0, u32 arg1) {
    switch (arg1) {
    case 18:
    case 28:
        return 0;
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
        return 1;
    case 15:
    case 16:
    case 17:
    case 26:
        return 2;
    case 29:
        return 3;
    case 20:
        return 4;
    case 5:
    case 12:
    case 19:
    case 27:
        return 5;
    case 3:
    case 4:
    case 6:
    case 21:
    case 32:
        return 6;
    case 0:
    case 1:
    case 13:
    case 14:
        return 7;
    case 7:
    case 33:
        return 8;
    case 25:
        return 9;
    case 24:
        return 10;
    default:
        return 11;
    }
}
