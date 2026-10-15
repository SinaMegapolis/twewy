#include "ShopPart.h"

// ---------------------------------------------------------------------------
// Shop_p6: item-table construction, category strip toggles, and the save
// commit paths for the Shop menu.
//
// Each function carries its own block-scope mirror of the tail of the
// ShopObject ("ShopP6") so that the file compiles both standalone and as part
// of the combined Shop.c unit.
//
// NOTE: the shared header declares func_ov043_020af624..af700 with an extra
// unused s32 parameter; the retail callees take a single pointer, so the call
// sites below use a function-pointer cast to keep the emitted bl 1-argument.
// ---------------------------------------------------------------------------

s32 func_ov043_020afbd4(ShopObject *arg0, u16 arg1) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ char         pad_000[0x7B8];
        /* 0x7B8 */ u8           unk_7B8;
        /* 0x7B9 */ u8           unk_7B9;
        /* 0x7BA */ char         pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP6Entry  unk_7BC[30];
        /* 0x834 */ char         pad_834[0x84C - 0x834];
        /* 0x84C */ u16          unk_84C;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r9;
    u16 var_r6;
    s32 temp_r7;
    s32 temp_r8;

    for (var_r9 = arg1; (s32) var_r9 < (s32) shop->unk_7B9; var_r9++) {
        temp_r7 = shop->unk_7BC[var_r9].unk_00;
        temp_r8 = Inventory_GetCategorizedIndex(temp_r7);
        for (var_r6 = 0; (u32) var_r6 < 0x1D8U; var_r6++) {
            if ((temp_r7 == gSaveData.inventoryItems[var_r6].itemID) &&
                ((u32) shop->unk_7B8 >= (u32) shop->unk_7BC[var_r9].unk_03) &&
                (func_02023b7c(temp_r7) == 0)) {
                gSaveData.unk_1EB2[temp_r8] =
                    (u8) ((gSaveData.unk_1EB2[temp_r8] & ~1) | 1);
                func_ov043_020afb60(arg0);
                shop->unk_84C = temp_r7;
                return 1;
            }
        }
    }
    return 0;
}

s32 func_ov043_020afcb0(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ char pad_02[0x2C - 0x02];
    } ShopP6Slot;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ ShopP6Slot  slots[42];
        /* 0x738 */ char        pad_738[0x7B8 - 0x738];
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP6Entry unk_7BC[30];
        /* 0x834 */ char        pad_834[0x848 - 0x834];
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ char        pad_84A[0x84C - 0x84A];
        /* 0x84C */ u16         unk_84C;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r4;
    s32 temp_r5;
    u16 temp_r6;

    temp_r5 = shop->slots[shop->unk_848].unk_00;
    temp_r6 = Inventory_GetCategorizedIndex(temp_r5);
    for (var_r4 = 0; (s32) shop->unk_7B9 > (s32) var_r4; var_r4++) {
        if ((temp_r5 == shop->unk_7BC[var_r4].unk_00) &&
            ((u32) shop->unk_7B8 >= (u32) shop->unk_7BC[var_r4].unk_03) &&
            (func_02023b7c(temp_r5) == 0)) {
            gSaveData.unk_1EB2[temp_r6] =
                (u8) ((gSaveData.unk_1EB2[temp_r6] & ~1) | 1);
            func_ov043_020afb60(arg0);
            shop->unk_84C = temp_r5;
            return 1;
        }
    }
    return 0;
}

// Nonmatching: register allocation: arg register and push set differ
void func_ov043_020afd6c(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ char       pad_738[0x778 - 0x738];
        /* 0x778 */ u16        unk_778;
        /* 0x77A */ char       pad_77A[0x7A2 - 0x77A];
        /* 0x7A2 */ u16        unk_7A2[9];
        /* 0x7B4 */ char       pad_7B4[0x84E - 0x7B4];
        /* 0x84E */ u16        unk_84E;
        /* 0x850 */ char       pad_850[0x860 - 0x850];
        /* 0x860 */ u16        unk_860;
        /* 0x862 */ char       pad_862[0x86C - 0x862];
        /* 0x86C */ u32        unk_86C;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  pad_02[6];
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u8  pad_0A[0x12];
        /* 0x1C */ u16 unk_1C;
        /* 0x1E */ u8  unk_1E;
        /* 0x1F */ u8  pad_1F[0x15];
        /* 0x34 */ u8  unk_34;
        /* 0x35 */ u8  unk_35;
        /* 0x36 */ u8  unk_36;
        /* 0x37 */ u8  unk_37;
        /* 0x38 */ u8  unk_38;
        /* 0x39 */ u8  pad_39;
        /* 0x3A */ s16 unk_3A;
        /* 0x3C */ u8  pad_3C[0x10];
        /* 0x4C */ u16 unk_4C;
        /* 0x4E */ u8  pad_4E[2];
        /* 0x50 */ u16 unk_50;
        /* 0x52 */ u8  pad_52[3];
        /* 0x55 */ u8  unk_55;
        /* 0x56 */ u8  pad_56[0x84 - 0x56];
        /* 0x84 */ u16 unk_84_00;
        /* 0x86 */ u16 unk_84_02;
        /* 0x88 */ u16 unk_84_04[3];
        /* 0x8E */ u8  unk_84_0A[3];
        /* 0x91 */ u8  unk_84_0D;
    } ShopP6Frame;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Frame sp;
    u16 temp_r1;
    u16 temp_r5;
    s32 temp_r4;
    u16 temp_r4_2;
    s32 temp_r9;
    s32 temp_r8;
    s16 temp_r0_2;
    s32 temp_r6;
    ShopP6Slot *slot;
    u16 var;

    temp_r1 = shop->unk_778;
    temp_r5 = shop->unk_860;
    if ((u32) temp_r1 < 2U) {
        return;
    }
    temp_r4 = temp_r1 - 2;
    if ((u32) shop->unk_7A2[temp_r4] >= 0x3D4U) {
        return;
    }
    if ((u32) shop->unk_7A2[temp_r4] < 0x320U) {
        OS_WaitForever();
    }
    temp_r4_2 = shop->unk_7A2[temp_r4] - 0x320;
    func_ov043_020af5cc(&sp.unk_84_00, (s32) temp_r4_2);
    temp_r9 = func_ov043_020af770(sp.unk_84_02, shop->unk_86C);
    shop->unk_84E = temp_r9;
    temp_r8 = Inventory_GetCategorizedIndex(shop->unk_84E);
    temp_r0_2 = Inventory_GetCategory(shop->unk_84E);
    temp_r6 = temp_r5 * 0x2C;
    slot = &shop->slots[temp_r5];
    slot->unk_00 = temp_r9;
    slot->unk_04 = temp_r0_2;
    slot->unk_08 = 0;
    switch (temp_r0_2) {
    default:
        break;
    case 0:
        func_ov043_020af4c4(&sp.unk_50, temp_r8);
        func_ov043_020af5f8(&sp.unk_34, temp_r8);
        slot->unk_02 = sp.unk_50;
        slot->unk_06 = (s16) sp.unk_55;
        slot->unk_20 = 0;
        slot->unk_24[0] = (s8) (sp.unk_38 + 1);
        slot->unk_24[1] = (s8) ((s32) sp.unk_3A / 409);
        slot->unk_24[2] = (s8) ((s32) sp.unk_4C / 60);
        slot->unk_24[3] = sp.unk_34;
        slot->unk_24[4] = sp.unk_35;
        slot->unk_24[5] = sp.unk_36;
        slot->unk_24[5] = sp.unk_37;
        break;
    case 1:
        func_ov043_020af4f0(&sp.unk_1C, temp_r8);
        slot->unk_02 = sp.unk_1C;
        slot->unk_06 = (s16) sp.unk_1E;
        if (((u32) (gSaveData.unk_1EB2[temp_r8] << 0x1F) >> 0x1F) == 1) {
            slot->unk_20 = 1;
        } else {
            slot->unk_20 = 0;
        }
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    case 2:
        func_ov043_020af51c(&sp.unk_08, temp_r8);
        slot->unk_02 = sp.unk_08;
        slot->unk_06 = 0xE;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    case 3:
        func_ov043_020af548(&sp.unk_00, temp_r8);
        slot->unk_02 = sp.unk_00;
        slot->unk_06 = 0xE;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        break;
    }
    slot->unk_10 = 1;
    slot->unk_14 = temp_r4_2;
    slot->unk_0C = sp.unk_84_0D;
    for (var = 0; (u32) var < 3U; var++) {
        slot->unk_16[var] = sp.unk_84_04[var];
        slot->unk_1C[var] = sp.unk_84_0A[var];
    }
    shop->unk_860 = (u16) (shop->unk_860 + 1);
}

s32 func_ov043_020b0088(u16 arg0) {
    s32 cat = Inventory_GetCategory(arg0);
    u16 var_r1 = 0;
    u16 var;

    if ((u32) arg0 >= 0x308U) {
        return 0;
    }
    if (cat == 0) {
        for (var = 0; (u32) var < 0x130U; var++) {
            if (arg0 == gSaveData.masteredPins[var].pinID) {
                var_r1 += gSaveData.masteredPins[var].count;
                break;
            }
        }
    } else {
        for (var = 0; (u32) var < 0x1D8U; var++) {
            if (arg0 == gSaveData.inventoryItems[var].itemID) {
                var_r1 = gSaveData.inventoryItems[var].flags.bits.count;
                break;
            }
        }
    }
    return var_r1;
}

void func_ov043_020b0154(s32 arg0, s32 arg1) {
    gSaveData.masteredPins[arg0].count -= arg1;
    if (gSaveData.masteredPins[arg0].count != 0) {
        return;
    }
    gSaveData.masteredPins[arg0].pinID = 0xFFFF;
    gSaveData.masteredPins[arg0].flags.bits.level = 0x64;
    gSaveData.masteredPins[arg0].flags.bits.unk_07 = 1;
}

void func_ov043_020b01ac(s32 arg0, s32 arg1) {
    u8 v = gSaveData.inventoryItems[arg0].flags.raw;

    gSaveData.inventoryItems[arg0].flags.raw =
        (v & ~0xF) | ((u8) (((u32) (v << 0x1C) >> 0x1C) - arg1) & 0xF);
}

void func_ov043_020b01d8(ShopObject *arg0, s32 arg1) {
    typedef struct {
        /* 0x00 */ char pad_00[0x16];
        /* 0x16 */ u16  unk_16[3];
        /* 0x1C */ u8   unk_1C[3];
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ char        pad_000[0x738];
        /* 0x738 */ ShopP6Slot *unk_738[12];
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u16 var_r4;

    for (var_r4 = 0; (u32) var_r4 < 3U; var_r4++) {
        ShopP6Slot *slot = shop->unk_738[arg1];
        u16 temp_r5 = slot->unk_16[var_r4];
        u8  temp_r6 = slot->unk_1C[var_r4];
        s32 temp_r0 = Inventory_GetCategory(temp_r5);

        if (temp_r6 != 0) {
            u16 var_r0;

            if (temp_r0 == 0) {
                for (var_r0 = 0; (u32) var_r0 < 0x130U; var_r0++) {
                    if (temp_r5 == gSaveData.masteredPins[var_r0].pinID) {
                        func_ov043_020b0154(var_r0, temp_r6);
                        break;
                    }
                }
            } else {
                for (var_r0 = 0; (u32) var_r0 < 0x1D8U; var_r0++) {
                    if (temp_r5 == gSaveData.inventoryItems[var_r0].itemID) {
                        func_ov043_020b01ac(var_r0, temp_r6);
                        break;
                    }
                }
            }
        }
    }
}

u16 func_ov043_020b02ac(u16 arg0) {
    if ((u32) arg0 < 0xCU) {
        return 12;
    }
    if ((s32) arg0 % 6 == 0) {
        return arg0;
    }
    return (u16) ((((s32) arg0 / 6) + 1) * 6);
}

// Nonmatching: register/spill allocation across the 2.5 KB body
void func_ov043_020b02fc(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
    } ShopP6Entry;
    typedef struct {
        /* 0x000 */ ShopP6Slot  slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ ShopP6Entry unk_7BC[30];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
    } ShopP6Sale;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04[3];
        /* 0x0A */ u8  unk_0A[3];
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u8  pad_0E[2];
    } ShopP6ShopItem;
    typedef struct {
        /* 0x00 */ u8  unk_00;
        /* 0x01 */ u8  unk_01;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
        /* 0x04 */ u8  unk_04;
        /* 0x05 */ u8  unk_05;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u8  pad_08[0x10];
        /* 0x18 */ u16 unk_18;
        /* 0x1A */ u8  pad_1A[2];
    } ShopP6PinInfo;

    ShopP6 *shop = (ShopP6 *) arg0;
    RawPinData      *pinData;
    RawItemData     *itemData;
    RawFoodData     *foodData;
    RawTreasureData *treasureData;
    ShopP6Sale      *sale;
    ShopP6ShopItem  *shopItem;
    ShopP6PinInfo   *pinInfo;
    u16              var_r4;
    u16              var_r7;
    u16              var_r8;
    u16              var_r5;
    u16              var;
    s32              temp_r10;
    u16              temp_r4_2;
    ShopP6Slot      *slot;
    s32              catIdx;
    s32              category;
    u32              item;

    pinData = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    Mem_SetSequence(&gDebugHeap, pinData, &data_ov043_020cc82c);
    itemData = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    Mem_SetSequence(&gDebugHeap, itemData, &data_ov043_020cc838);
    foodData = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    Mem_SetSequence(&gDebugHeap, foodData, &data_ov043_020cc844);
    treasureData = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    Mem_SetSequence(&gDebugHeap, treasureData, &data_ov043_020cc850);
    sale = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    Mem_SetSequence(&gDebugHeap, sale, &data_ov043_020cc860);
    shopItem = Mem_AllocHeapTail(&gDebugHeap, 0xBA0);
    Mem_SetSequence(&gDebugHeap, shopItem, &data_ov043_020cc870);
    pinInfo = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, pinInfo, &data_ov043_020cc880);
    ((void (*)(void *)) func_ov043_020af624)(pinData);
    ((void (*)(void *)) func_ov043_020af648)(itemData);
    ((void (*)(void *)) func_ov043_020af66c)(foodData);
    ((void (*)(void *)) func_ov043_020af690)(treasureData);
    ((void (*)(void *)) func_ov043_020af6b4)(sale);
    ((void (*)(void *)) func_ov043_020af6dc)(shopItem);
    ((void (*)(void *)) func_ov043_020af700)(pinInfo);

    var_r4 = 0;
    var_r7 = 0;
    do {
        slot = &shop->slots[var_r4];
        slot->unk_00 = 0xFFFF;
        slot->unk_02 = 0;
        slot->unk_04 = 0;
        slot->unk_06 = 0xE;
        slot->unk_08 = 0;
        slot->unk_0C = 1;
        slot->unk_10 = 0;
        slot->unk_14 = 0xFFFF;
        slot->unk_20 = 0;
        for (var = 0; (u32) var < 7U; var++) {
            slot->unk_24[var] = 0;
        }
        for (var = 0; (u32) var < 3U; var++) {
            slot->unk_16[var] = 0xFFFF;
            slot->unk_1C[var] = 0;
        }
        var_r4 += 1;
    } while ((u32) var_r4 < 0x2AU);

    var_r8 = 0;
    do {
        if (shop->unk_768 == sale[var_r8].unk_00) {
            item = func_ov043_020af770(sale[var_r8].unk_02, shop->unk_86C);
            temp_r10 = sale[var_r8].unk_04;
            if ((Inventory_GetOwnedCount(0x2AE) != 0) || ((s32) (gSaveData.chapter + 1) >= (s32) sale[var_r8].unk_06)) {
                if (item < 0x320) {
                    catIdx = Inventory_GetCategorizedIndex((u16) item);
                    category = Inventory_GetCategory((u16) item);
                    slot = &shop->slots[var_r7];
                    slot->unk_00 = (u16) item;
                    slot->unk_04 = category;
                    switch (category) {
                    default:
                        break;
                    case 0:
                        slot->unk_02 = pinData[catIdx].iconIndex;
                        slot->unk_06 = (s16) pinData[catIdx].brand;
                        slot->unk_08 = (u32) (temp_r10 * pinData[catIdx].price) / 100;
                        slot->unk_20 = 0;
                        slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                        slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                        slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                        slot->unk_24[3] = pinInfo[catIdx].unk_00;
                        slot->unk_24[4] = pinInfo[catIdx].unk_01;
                        slot->unk_24[5] = pinInfo[catIdx].unk_02;
                        slot->unk_24[6] = pinInfo[catIdx].unk_03;
                        break;
                    case 1:
                        slot->unk_02 = itemData[catIdx].unk_00;
                        slot->unk_06 = (s16) itemData[catIdx].brand;
                        slot->unk_08 = (u32) (temp_r10 * itemData[catIdx].unk_04) / 100;
                        if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                            slot->unk_20 = 1;
                        } else {
                            slot->unk_20 = 0;
                        }
                        break;
                    case 2:
                        slot->unk_02 = *(u16 *) (foodData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_08 = (u32) (temp_r10 * foodData[catIdx].unk_04) / 100;
                        slot->unk_20 = 0;
                        break;
                    case 3:
                        slot->unk_02 = *(u16 *) (treasureData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_08 = (u32) (temp_r10 * treasureData[catIdx].unk_04) / 100;
                        slot->unk_20 = 0;
                        break;
                    }
                    slot->unk_0C = 1;
                    slot->unk_10 = 0;
                    slot->unk_14 = 0xFFFF;
                } else {
                    temp_r4_2 = (u16) (item - 0x320);
                    item = shopItem[temp_r4_2].unk_02;
                    catIdx = Inventory_GetCategorizedIndex((u16) item);
                    category = Inventory_GetCategory((u16) item);
                    slot = &shop->slots[var_r7];
                    slot->unk_00 = (u16) item;
                    slot->unk_04 = category;
                    slot->unk_08 = 0;
                    switch (category) {
                    default:
                        break;
                    case 0:
                        slot->unk_02 = pinData[catIdx].iconIndex;
                        slot->unk_06 = (s16) pinData[catIdx].brand;
                        slot->unk_20 = 0;
                        slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                        slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                        slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                        slot->unk_24[3] = pinInfo[catIdx].unk_00;
                        slot->unk_24[4] = pinInfo[catIdx].unk_01;
                        slot->unk_24[5] = pinInfo[catIdx].unk_02;
                        slot->unk_24[6] = pinInfo[catIdx].unk_03;
                        break;
                    case 1:
                        slot->unk_02 = itemData[catIdx].unk_00;
                        slot->unk_06 = (s16) itemData[catIdx].brand;
                        if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                            slot->unk_20 = 1;
                        } else {
                            slot->unk_20 = 0;
                        }
                        break;
                    case 2:
                        slot->unk_02 = *(u16 *) (foodData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_20 = 0;
                        break;
                    case 3:
                        slot->unk_02 = *(u16 *) (treasureData + catIdx);
                        slot->unk_06 = 0xE;
                        slot->unk_20 = 0;
                        break;
                    }
                    slot->unk_10 = 1;
                    slot->unk_14 = temp_r4_2;
                    slot->unk_0C = shopItem[temp_r4_2].unk_0D;
                    for (var = 0; (u32) var < 3U; var++) {
                        slot->unk_16[var] = shopItem[temp_r4_2].unk_04[var];
                        slot->unk_1C[var] = shopItem[temp_r4_2].unk_0A[var];
                    }
                }
                var_r7 += 1;
                if ((u32) var_r7 >= 0x2AU) {
                    OS_WaitForever();
                }
            }
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x23AU);

    var_r5 = 0;
    if ((s32) (shop->unk_778 - 1) > 0) {
        do {
            if ((u32) shop->unk_7A2[var_r5] < 0x3D4U) {
                if ((u32) shop->unk_7A2[var_r5] < 0x320U) {
                    OS_WaitForever();
                }
                temp_r4_2 = (u16) (shop->unk_7A2[var_r5] - 0x320);
                item = func_ov043_020af770(shopItem[temp_r4_2].unk_02, shop->unk_86C);
                catIdx = Inventory_GetCategorizedIndex((u16) item);
                category = Inventory_GetCategory((u16) item);
                slot = &shop->slots[var_r7];
                slot->unk_00 = (u16) item;
                slot->unk_04 = category;
                slot->unk_08 = 0;
                switch (category) {
                default:
                    break;
                case 0:
                    slot->unk_02 = pinData[catIdx].iconIndex;
                    slot->unk_06 = (s16) pinData[catIdx].brand;
                    slot->unk_20 = 0;
                    slot->unk_24[0] = (s8) (pinInfo[catIdx].unk_04 + 1);
                    slot->unk_24[1] = (s8) ((s32) pinInfo[catIdx].unk_06 / 409);
                    slot->unk_24[2] = (s8) ((s32) pinInfo[catIdx].unk_18 / 60);
                    slot->unk_24[3] = pinInfo[catIdx].unk_00;
                    slot->unk_24[4] = pinInfo[catIdx].unk_01;
                    slot->unk_24[5] = pinInfo[catIdx].unk_02;
                    slot->unk_24[6] = pinInfo[catIdx].unk_03;
                    break;
                case 1:
                    slot->unk_02 = itemData[catIdx].unk_00;
                    slot->unk_06 = (s16) itemData[catIdx].brand;
                    if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                        slot->unk_20 = 1;
                    } else {
                        slot->unk_20 = 0;
                    }
                    break;
                case 2:
                    slot->unk_02 = *(u16 *) (foodData + catIdx);
                    slot->unk_06 = 0xE;
                    slot->unk_20 = 0;
                    break;
                case 3:
                    slot->unk_02 = *(u16 *) (treasureData + catIdx);
                    slot->unk_06 = 0xE;
                    slot->unk_20 = 0;
                    break;
                }
                slot->unk_10 = 1;
                slot->unk_14 = temp_r4_2;
                slot->unk_0C = shopItem[temp_r4_2].unk_0D;
                for (var = 0; (u32) var < 3U; var++) {
                    slot->unk_16[var] = shopItem[temp_r4_2].unk_04[var];
                    slot->unk_1C[var] = shopItem[temp_r4_2].unk_0A[var];
                }
                var_r7 += 1;
            }
            var_r5 += 1;
        } while ((s32) (shop->unk_778 - 1) > (s32) var_r5);
    }
    shop->unk_860 = var_r7;
    func_ov043_020b02ac(shop->unk_860);
    shop->unk_862 = (u16) func_ov043_020b02ac(shop->unk_860);
    shop->unk_870 = (u16) ((shop->unk_862 / 6) - 2);
    shop->unk_872 = shop->unk_870;
    var_r7 = 0;
    do {
        shop->unk_7BC[var_r7].unk_00 = 0xFFFF;
        shop->unk_7BC[var_r7].unk_02 = 0;
        shop->unk_7BC[var_r7].unk_03 = 0xA;
        var_r7 += 1;
    } while ((u32) var_r7 < 0x1EU);
    var_r7 = 0;
    var_r8 = 0;
    do {
        if (shop->unk_774 == itemData[var_r7].unk_12) {
            shop->unk_7BC[var_r8].unk_00 = (s16) (var_r7 + 0x130);
            shop->unk_7BC[var_r8].unk_02 = itemData[var_r7].unk_10;
            shop->unk_7BC[var_r8].unk_03 = itemData[var_r7].unk_14;
            var_r8 += 1;
        }
        var_r7 += 1;
    } while ((u32) var_r7 < 0x118U);
    shop->unk_7B9 = (s8) var_r8;
    Mem_Free(&gDebugHeap, pinData);
    Mem_Free(&gDebugHeap, itemData);
    Mem_Free(&gDebugHeap, foodData);
    Mem_Free(&gDebugHeap, treasureData);
    Mem_Free(&gDebugHeap, sale);
    Mem_Free(&gDebugHeap, shopItem);
    Mem_Free(&gDebugHeap, pinInfo);
}

void func_ov043_020b0d18(void) {
}

// Nonmatching: if-conversion vs branch on the unk_834 flag update
void func_ov043_020b0d1c(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u16 unk_0A;
    } ShopP6Menu2C;
    typedef struct {
        /* 0x00 */ ShopP6Menu0  menu0;
        /* 0x2C */ ShopP6Menu2C menu2C;
    } ShopP6Scratch;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Scratch sp;
    s32          var_ip;
    s32          var_r2;
    s16          var_r1;
    ShopP6Slot  *var_r1_2;

    func_ov043_020af724(arg0);
    shop->unk_858 = gSaveData.playerStats.money;
    if (gSaveData.playerStats.unk_19_6 == 1) {
        var_r1 = shop->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    shop->unk_834 = var_r1;
    shop->unk_768 = gSaveData.unk_1AB2;
    func_ov043_020af574(&sp.menu2C, (s32) shop->unk_768);
    shop->unk_76A = (s16) (sp.menu2C.unk_02 + 0x3F);
    shop->unk_76C = (s16) (sp.menu2C.unk_04 + 0x1F);
    shop->unk_76E = sp.menu2C.unk_06;
    shop->unk_770 = sp.menu2C.unk_0A;
    shop->unk_774 = sp.menu2C.unk_08;
    func_ov043_020af5a0(&sp.menu0, (s32) sp.menu2C.unk_08);
    shop->unk_776 = sp.menu0.unk_00;
    shop->unk_77C = gSaveData.unk_1A18[shop->unk_774];
    shop->unk_784 = sp.menu0.unk_04;
    shop->unk_78E = sp.menu0.unk_02;
    if (shop->unk_77C > shop->unk_784) {
        shop->unk_77C = shop->unk_784;
    }
    for (var_ip = 0; var_ip < 9; var_ip++) {
        shop->unk_790[var_ip] = sp.menu0.unk_08[var_ip];
        shop->unk_7A2[var_ip] = sp.menu0.unk_1A[var_ip];
    }
    shop->unk_788 = func_ov043_020af8a8(arg0, shop->unk_77C, shop->unk_784);
    shop->unk_778 = func_ov043_020af8e4(arg0);
    shop->unk_77A = func_ov043_020af950(arg0);
    func_ov043_020af994(arg0);
    shop->unk_780 = func_ov043_020af994(arg0);
    shop->unk_78C = func_ov043_020af9e8(arg0);
    shop->unk_78A = func_ov043_020af8a8(arg0, shop->unk_780, shop->unk_784);
    if (func_ov043_020b1e2c(arg0) == 0) {
        shop->unk_7B6 = 0;
    } else {
        shop->unk_7B6 = 1;
    }
    shop->unk_7B7 = (s8) func_ov043_020b1ecc(arg0, shop->unk_774);
    func_ov043_020b02fc(arg0);
    shop->unk_7B8 = (s8) func_ov043_020afa68(arg0);
    shop->unk_7B4 = (s8) func_ov043_020afabc(arg0);
    shop->unk_7B5 = (s8) func_ov043_020afb14(arg0);
    var_r1_2 = shop->slots;
    var_r2 = 0;
    do {
        shop->unk_738[var_r2] = var_r1_2;
        var_r2 += 1;
        var_r1_2 = (ShopP6Slot *) ((u8 *) var_r1_2 + 0x2C);
    } while (var_r2 < 12);
    func_ov043_020b0d18();
}

// Nonmatching: register/spill allocation through the item-table build
void func_ov043_020b0f20(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
    } ShopP6Sale;
    typedef struct {
        /* 0x00 */ u8  unk_00;
        /* 0x01 */ u8  unk_01;
        /* 0x02 */ u8  unk_02;
        /* 0x03 */ u8  unk_03;
        /* 0x04 */ u8  unk_04;
        /* 0x05 */ u8  unk_05;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u8  pad_08[0x10];
        /* 0x18 */ u16 unk_18;
        /* 0x1A */ u8  pad_1A[2];
    } ShopP6PinInfo;

    ShopP6 *shop = (ShopP6 *) arg0;
    RawPinData      *pinData;
    RawItemData     *itemData;
    RawFoodData     *foodData;
    RawTreasureData *treasureData;
    ShopP6Sale      *sale;
    ShopP6PinInfo   *pinInfo;
    u16              var_r8;
    u16              var_r5;
    u16              var_r9;
    u16              var_ip;
    s32              catIdx;
    s32              item;
    s32              category;
    s32              cost;
    ShopP6Slot      *slot;
    ShopP6PinInfo   *info;

    pinData = Mem_AllocHeapTail(&gDebugHeap, 0x3DC0);
    Mem_SetSequence(&gDebugHeap, pinData, &data_ov043_020cc82c);
    itemData = Mem_AllocHeapTail(&gDebugHeap, 0x1A40);
    Mem_SetSequence(&gDebugHeap, itemData, &data_ov043_020cc838);
    foodData = Mem_AllocHeapTail(&gDebugHeap, 0x348);
    Mem_SetSequence(&gDebugHeap, foodData, &data_ov043_020cc844);
    treasureData = Mem_AllocHeapTail(&gDebugHeap, 0x4B0);
    Mem_SetSequence(&gDebugHeap, treasureData, &data_ov043_020cc850);
    sale = Mem_AllocHeapTail(&gDebugHeap, 0x11D0);
    Mem_SetSequence(&gDebugHeap, sale, &data_ov043_020cc860);
    pinInfo = Mem_AllocHeapTail(&gDebugHeap, 0x2140);
    Mem_SetSequence(&gDebugHeap, pinInfo, &data_ov043_020cc880);
    ((void (*)(void *)) func_ov043_020af624)(pinData);
    ((void (*)(void *)) func_ov043_020af648)(itemData);
    ((void (*)(void *)) func_ov043_020af66c)(foodData);
    ((void (*)(void *)) func_ov043_020af690)(treasureData);
    ((void (*)(void *)) func_ov043_020af6b4)(sale);
    ((void (*)(void *)) func_ov043_020af700)(pinInfo);

    var_r8 = 0;
    var_r5 = 0;
    do {
        slot = &shop->slots[var_r8];
        slot->unk_00 = 0xFFFF;
        slot->unk_02 = 0;
        slot->unk_04 = 0;
        slot->unk_06 = 0xE;
        slot->unk_08 = 0;
        slot->unk_0C = 1;
        slot->unk_10 = 0;
        for (var_ip = 0; (u32) var_ip < 7U; var_ip++) {
            slot->unk_24[var_ip] = 0;
        }
        for (var_ip = 0; (u32) var_ip < 3U; var_ip++) {
            slot->unk_16[var_ip] = 0xFFFF;
            slot->unk_1C[var_ip] = 0;
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x2AU);

    var_r9 = 0;
    do {
        item = *(u16 *) ((u8 *) &gSaveData + 0x1AF6 + var_r9 * 2);
        catIdx = Inventory_GetCategorizedIndex((u16) item);
        if (item != 0xFFFF) {
            cost = pinData[catIdx].price;
            if ((cost != 0) && (cost != 0x989680)) {
                info = &pinInfo[catIdx];
                slot = &shop->slots[var_r5];
                slot->unk_00 = (u16) item;
                slot->unk_02 = pinData[catIdx].iconIndex;
                slot->unk_04 = 0;
                slot->unk_06 = (s16) pinData[catIdx].brand;
                slot->unk_08 = pinData[catIdx].price;
                slot->unk_20 = 0;
                slot->unk_24[0] = (s8) (info->unk_04 + 1);
                slot->unk_24[1] = (s8) ((s32) info->unk_06 / 409);
                slot->unk_24[2] = (s8) ((s32) info->unk_18 / 60);
                slot->unk_24[3] = info->unk_00;
                slot->unk_24[4] = info->unk_01;
                slot->unk_24[5] = info->unk_02;
                slot->unk_24[6] = info->unk_03;
                var_r5 += 1;
            }
        }
        var_r9 += 1;
    } while ((u32) var_r9 < 6U);

    var_r8 = 0;
    do {
        item = *(u16 *) ((u8 *) &gSaveData + 0x1B02 + var_r8 * 2);
        catIdx = Inventory_GetCategorizedIndex((u16) item);
        category = Inventory_GetCategory((u16) item);
        if (item != 0xFFFF) {
            cost = itemData[catIdx].unk_04;
            if ((cost != 0) && (cost != 0x989680)) {
                switch (category) {
                case 1:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = itemData[catIdx].unk_00;
                    slot->unk_04 = 1;
                    slot->unk_06 = (s16) itemData[catIdx].brand;
                    slot->unk_08 = itemData[catIdx].unk_04;
                    if (((u32) (gSaveData.unk_1EB2[catIdx] << 0x1F) >> 0x1F) == 1) {
                        slot->unk_20 = 1;
                    } else {
                        slot->unk_20 = 0;
                    }
                    break;
                case 2:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = *(u16 *) (foodData + catIdx);
                    slot->unk_04 = 2;
                    slot->unk_06 = 0xE;
                    slot->unk_08 = foodData[catIdx].unk_04;
                    slot->unk_20 = 0;
                    break;
                case 3:
                    slot = &shop->slots[var_r5];
                    slot->unk_00 = (u16) item;
                    slot->unk_02 = *(u16 *) (treasureData + catIdx);
                    slot->unk_04 = 3;
                    slot->unk_06 = 0xE;
                    slot->unk_08 = treasureData[catIdx].unk_04;
                    slot->unk_20 = 0;
                    break;
                default:
                    OS_WaitForever();
                    break;
                }
                var_r5 += 1;
            }
        }
        var_r8 += 1;
    } while ((u32) var_r8 < 0x10U);

    shop->unk_860 = var_r5;
    func_ov043_020b02ac(shop->unk_860);
    shop->unk_862 = (u16) func_ov043_020b02ac(shop->unk_860);
    shop->unk_870 = (u16) ((shop->unk_862 / 6) - 2);
    shop->unk_872 = shop->unk_870;
    Mem_Free(&gDebugHeap, pinData);
    Mem_Free(&gDebugHeap, itemData);
    Mem_Free(&gDebugHeap, foodData);
    Mem_Free(&gDebugHeap, treasureData);
    Mem_Free(&gDebugHeap, sale);
    Mem_Free(&gDebugHeap, pinInfo);
}

// Nonmatching: if-conversion vs branch on the unk_834 flag update
void func_ov043_020b1404(ShopObject *arg0) {
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ s16 unk_06;
        /* 0x08 */ u32 unk_08;
        /* 0x0C */ u8  unk_0C;
        /* 0x0D */ u8  unk_0D;
        /* 0x0E */ u16 unk_0E;
        /* 0x10 */ s32 unk_10;
        /* 0x14 */ u16 unk_14;
        /* 0x16 */ u16 unk_16[3];
        /* 0x1C */ u8  unk_1C[3];
        /* 0x1F */ u8  unk_1F;
        /* 0x20 */ s32 unk_20;
        /* 0x24 */ u8  unk_24[7];
        /* 0x2B */ u8  unk_2B;
    } ShopP6Slot;
    typedef struct {
        /* 0x000 */ ShopP6Slot slots[42];
        /* 0x738 */ ShopP6Slot *unk_738[12];
        /* 0x768 */ u16         unk_768;
        /* 0x76A */ s16         unk_76A;
        /* 0x76C */ s16         unk_76C;
        /* 0x76E */ u16         unk_76E;
        /* 0x770 */ u16         unk_770;
        /* 0x772 */ u16         unk_772;
        /* 0x774 */ u16         unk_774;
        /* 0x776 */ u16         unk_776;
        /* 0x778 */ u16         unk_778;
        /* 0x77A */ u16         unk_77A;
        /* 0x77C */ u32         unk_77C;
        /* 0x780 */ s32         unk_780;
        /* 0x784 */ u32         unk_784;
        /* 0x788 */ s16         unk_788;
        /* 0x78A */ s16         unk_78A;
        /* 0x78C */ s16         unk_78C;
        /* 0x78E */ u16         unk_78E;
        /* 0x790 */ u16         unk_790[9];
        /* 0x7A2 */ u16         unk_7A2[9];
        /* 0x7B4 */ u8          unk_7B4;
        /* 0x7B5 */ u8          unk_7B5;
        /* 0x7B6 */ u8          unk_7B6;
        /* 0x7B7 */ u8          unk_7B7;
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ u8          unk_7BA;
        /* 0x7BB */ u8          unk_7BB;
        /* 0x7BC */ char        pad_7BC[0x834 - 0x7BC];
        /* 0x834 */ u16         unk_834;
        /* 0x836 */ u8          unk_836;
        /* 0x837 */ u8          unk_837;
        /* 0x838 */ u8          unk_838;
        /* 0x839 */ u8          unk_839;
        /* 0x83A */ u16         unk_83A;
        /* 0x83C */ s16         unk_83C;
        /* 0x83E */ u16         unk_83E;
        /* 0x840 */ s16         unk_840;
        /* 0x842 */ s16         unk_842;
        /* 0x844 */ u16         unk_844;
        /* 0x846 */ u16         unk_846;
        /* 0x848 */ u16         unk_848;
        /* 0x84A */ u16         unk_84A;
        /* 0x84C */ u16         unk_84C;
        /* 0x84E */ u16         unk_84E;
        /* 0x850 */ u16         unk_850;
        /* 0x852 */ u16         unk_852;
        /* 0x854 */ u8          unk_854;
        /* 0x855 */ u8          unk_855;
        /* 0x856 */ u16         unk_856;
        /* 0x858 */ u32         unk_858;
        /* 0x85C */ u16         unk_85C;
        /* 0x85E */ u16         unk_85E;
        /* 0x860 */ u16         unk_860;
        /* 0x862 */ u16         unk_862;
        /* 0x864 */ u8          unk_864[3];
        /* 0x867 */ u8          unk_867;
        /* 0x868 */ u32         unk_868;
        /* 0x86C */ u32         unk_86C;
        /* 0x870 */ u16         unk_870;
        /* 0x872 */ u16         unk_872;
    } ShopP6;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u32 unk_04;
        /* 0x08 */ u16 unk_08[9];
        /* 0x1A */ u16 unk_1A[9];
    } ShopP6Menu0;
    typedef struct {
        /* 0x00 */ u16 unk_00;
        /* 0x02 */ u16 unk_02;
        /* 0x04 */ u16 unk_04;
        /* 0x06 */ u16 unk_06;
        /* 0x08 */ u16 unk_08;
        /* 0x0A */ u16 unk_0A;
    } ShopP6Menu2C;
    typedef struct {
        /* 0x00 */ ShopP6Menu0  menu0;
        /* 0x2C */ ShopP6Menu2C menu2C;
    } ShopP6Scratch;

    ShopP6 *shop = (ShopP6 *) arg0;
    ShopP6Scratch sp;
    s32          var_ip;
    s32          var_r2;
    s16          var_r1;
    ShopP6Slot  *var_r1_2;

    shop->unk_858 = gSaveData.playerStats.money;
    if (gSaveData.playerStats.unk_19_6 == 1) {
        var_r1 = shop->unk_834 | 4;
    } else {
        var_r1 = 0;
    }
    shop->unk_834 = var_r1;
    shop->unk_768 = (u16) (((u32) gSaveData.unk_1AF2.shopId));
    func_ov043_020af574(&sp.menu2C, (s32) shop->unk_768);
    shop->unk_76A = (s16) (sp.menu2C.unk_02 + 0x3F);
    shop->unk_76C = (s16) (gSaveData.unk_1AF2.musicId + 0x1F);
    shop->unk_76E = gSaveData.unk_1AF2.giftItemId;
    shop->unk_770 = sp.menu2C.unk_0A;
    shop->unk_774 = (u16) gSaveData.unk_1AF2.clerkId;
    func_ov043_020af5a0(&sp.menu0, (s32) shop->unk_774);
    shop->unk_776 = sp.menu0.unk_00;
    shop->unk_77C = sp.menu0.unk_04;
    shop->unk_784 = sp.menu0.unk_04;
    shop->unk_78E = sp.menu0.unk_02;
    for (var_ip = 0; var_ip < 9; var_ip++) {
        shop->unk_790[var_ip] = sp.menu0.unk_08[var_ip];
        shop->unk_7A2[var_ip] = sp.menu0.unk_1A[var_ip];
    }
    shop->unk_788 = func_ov043_020af8a8(arg0, shop->unk_77C, shop->unk_784);
    shop->unk_778 = func_ov043_020af8e4(arg0);
    shop->unk_77A = func_ov043_020af950(arg0);
    func_ov043_020af994(arg0);
    shop->unk_780 = func_ov043_020af994(arg0);
    shop->unk_78C = func_ov043_020af9e8(arg0);
    shop->unk_78A = func_ov043_020af8a8(arg0, shop->unk_780, shop->unk_784);
    shop->unk_7B4 = 0;
    shop->unk_7B5 = 0;
    shop->unk_7B6 = 0;
    shop->unk_7B7 = (s8) func_ov043_020b1ecc(arg0, shop->unk_774);
    func_ov043_020b0f20(arg0);
    var_r1_2 = shop->slots;
    var_r2 = 0;
    do {
        shop->unk_738[var_r2] = var_r1_2;
        var_r2 += 1;
        var_r1_2 = (ShopP6Slot *) ((u8 *) var_r1_2 + 0x2C);
    } while (var_r2 < 12);
}

void func_ov043_020b15d0(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x774];
        /* 0x774 */ u16  unk_774;
        /* 0x776 */ char pad_776[0x77C - 0x776];
        /* 0x77C */ u32  unk_77C;
        /* 0x780 */ char pad_780[0x834 - 0x780];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x858 - 0x836];
        /* 0x858 */ u32  unk_858;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;

    gSaveData.playerStats.money = shop->unk_858;
    gSaveData.unk_1A18[shop->unk_774] = shop->unk_77C;
    if (shop->unk_834 & 4) {
        ((u8 *) &gSaveData)[0x19] |= 0x40;
    } else {
        ((u8 *) &gSaveData)[0x19] &= ~0x40;
    }
}

// Nonmatching: pure register permutation
void func_ov043_020b1618(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x834];
        /* 0x834 */ u16  unk_834;
        /* 0x836 */ char pad_836[0x858 - 0x836];
        /* 0x858 */ u32  unk_858;
        /* 0x85C */ char pad_85C[0x868 - 0x85C];
        /* 0x868 */ u32  unk_868;
    } ShopP6;

    ShopP6 *shop = (ShopP6 *) arg0;
    u8 idx;
    u32 *p;
    u32 temp_r4;
    u32 temp_r2;
    u32 var_r3;
    u32 var_r2;

    idx = gSaveData.unk_1AB9;
    gSaveData.playerStats.money = shop->unk_858;
    temp_r4 = shop->unk_868;
    if (temp_r4 != 0) {
        p = &gSaveData.mingleFriends[idx].unk_8;
        temp_r2 = *p;
        var_r3 = (temp_r2 & ~0xFF000000) + temp_r4;
        if (var_r3 > 0x98967FU) {
            var_r3 = 0x98967F;
        }
        var_r2 = ((u32) (temp_r2 & 0xFF000000) >> 0x18) + 1;
        if (var_r2 > 0xFFU) {
            var_r2 = 0;
        }
        *p = (var_r3 & ~0xFF000000) + (var_r2 << 0x18);
    }
    if (shop->unk_834 & 4) {
        ((u8 *) &gSaveData)[0x19] |= 0x40;
        return;
    }
    ((u8 *) &gSaveData)[0x19] &= ~0x40;
}
