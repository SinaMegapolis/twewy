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

void func_ov043_020af724(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x86C];
        /* 0x86C */ s32  unk_86C;
    } ShopP5View724;
    ShopP5View724 *v = (ShopP5View724 *)arg0;
    u8  mac[8];
    u16 i;
    s32 acc;

    func_0203a96c(mac);
    i = 0;
    v->unk_86C = 0;
    do {
        acc = v->unk_86C + mac[i];
        i += 1;
        v->unk_86C = acc;
    } while ((u32)i < 6U);
}

u16 func_ov043_020af770(u16 arg0, u32 arg1) {
    u16 t;

    if (arg0 == 0xCE) {
        t = arg1 & 1;
        if (t == 0) {
            return 0xCE;
        }
        return 0xCF;
    }
    if (arg0 == 0xD0) {
        t = arg1 & 3;
        switch (t) {
        case 0:
            return 0xD0;
        case 1:
            return 0xD1;
        case 2:
            return 0xD2;
        }
        return 0xD3;
    }
    if (arg0 == 0xD4) {
        t = arg1 % 6;
        switch (t) {
        case 0:
            return 0xD4;
        case 1:
            return 0xD5;
        case 2:
            return 0xD6;
        case 3:
            return 0xD7;
        case 4:
            return 0xD8;
        }
        return 0xD9;
    }
    if (arg0 == 0xDA) {
        t = arg1 % 3;
        switch (t) {
        case 0:
            return 0xDA;
        case 1:
            return 0xDB;
        }
        return 0xDC;
    }
    return arg0;
}

u16 func_ov043_020af8a8(ShopObject *arg0, u32 arg1, u32 arg2) {
    u64 q;

    q = (u64)arg1 * 1000 / arg2;
    if (q > 1000) {
        q = 1000;
    }
    return (u16)q;
}

u16 func_ov043_020af8e4(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x788];
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ char pad_78A[0x790 - 0x78A];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View8e4;
    ShopP5View8e4 *v = (ShopP5View8e4 *)arg0;
    u16 temp_lr = v->unk_788;
    u16 var_ip = 0;
    s32 temp_r3;

    do {
        temp_r3 = v->unk_790[var_ip] * 10;
        if ((s32)temp_lr < temp_r3) {
            return var_ip + 1;
        }
        if (temp_lr == temp_r3) {
            return var_ip + 2;
        }
        var_ip += 1;
    } while ((u32)var_ip < 9U);
    return 10;
}

u16 func_ov043_020af950(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x790];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View950;
    ShopP5View950 *v = (ShopP5View950 *)arg0;
    u16 var_r2 = 0;

    do {
        if (v->unk_790[var_r2] == 100) {
            return var_r2 + 2;
        }
        var_r2 += 1;
    } while ((u32)var_r2 < 9U);
    return 10;
}

u32 func_ov043_020af994(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x778];
        /* 0x778 */ u16  unk_778;
        /* 0x77A */ u16  unk_77A;
        /* 0x77C */ char pad_77C[0x784 - 0x77C];
        /* 0x784 */ u32  unk_784;
        /* 0x788 */ char pad_788[0x790 - 0x788];
        /* 0x790 */ u16  unk_790[9];
    } ShopP5View994;
    ShopP5View994 *v = (ShopP5View994 *)arg0;
    u64 var_lr;

    if ((u32)v->unk_778 >= (u32)v->unk_77A) {
        var_lr = 100;
    } else {
        var_lr = v->unk_790[v->unk_778 - 1];
    }
    return (u64)v->unk_784 * var_lr / 100;
}

u16 func_ov043_020af9e8(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x788];
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ char pad_78A[0x78E - 0x78A];
        /* 0x78E */ u16  unk_78E;
    } ShopP5View9e8;
    ShopP5View9e8 *v = (ShopP5View9e8 *)arg0;
    u16 temp_r2 = v->unk_788;

    if ((u32)temp_r2 >= 0x3E8U) {
        return 2;
    }
    if ((s32)temp_r2 >= (s32)(v->unk_78E * 10)) {
        return 1;
    }
    return 0;
}

void func_ov043_020afa20(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x77C];
        /* 0x77C */ s32  unk_77C;
        /* 0x780 */ s32  unk_780;
        /* 0x784 */ u32  unk_784;
        /* 0x788 */ u16  unk_788;
        /* 0x78A */ u16  unk_78A;
        /* 0x78C */ u16  unk_78C;
    } ShopP5Viewa20;
    ShopP5Viewa20 *v = (ShopP5Viewa20 *)arg0;

    v->unk_788 = func_ov043_020af8a8(arg0, v->unk_77C, v->unk_784);
    v->unk_78A = func_ov043_020af8a8(arg0, v->unk_780, v->unk_784);
    v->unk_78C = func_ov043_020af9e8(arg0);
}

u16 func_ov043_020afa68(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x778];
        /* 0x778 */ u16  unk_778;
        /* 0x77A */ char pad_77A[0x7A2 - 0x77A];
        /* 0x7A2 */ u16  unk_7A2[9];
    } ShopP5Viewa68;
    ShopP5Viewa68 *v = (ShopP5Viewa68 *)arg0;
    u16 var_r2 = 0;
    u16 var_r3 = 0;
    s32 temp_ip = v->unk_778 - 1;

    if (temp_ip > 0) {
        do {
            if ((u32)v->unk_7A2[var_r3] >= 0x3D4U) {
                var_r2 += 1;
            }
            var_r3 += 1;
        } while (temp_ip > (s32)var_r3);
    }
    return var_r2;
}

u16 func_ov043_020afabc(ShopObject *arg0) {
    typedef struct {
        /* 0x0 */ u16 unk_0;
        /* 0x2 */ u8  pad_2;
        /* 0x3 */ u8  unk_3;
    } ShopP5Recabc;
    typedef struct {
        /* 0x000 */ char        pad_000[0x7B9];
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP5Recabc unk_7BC[17];
    } ShopP5Viewabc;
    ShopP5Viewabc *v = (ShopP5Viewabc *)arg0;
    u16 var_r4;
    u8  var_r5 = 0;

    var_r4 = 0;
    if ((s32)v->unk_7B9 > 0) {
        do {
            if (func_02023b7c(v->unk_7BC[var_r4].unk_0) == 1) {
                var_r5 += 1;
            }
            var_r4 += 1;
        } while ((s32)v->unk_7B9 > (s32)var_r4);
    }
    return var_r5;
}

u16 func_ov043_020afb14(ShopObject *arg0) {
    typedef struct {
        /* 0x0 */ u16 unk_0;
        /* 0x2 */ u8  pad_2;
        /* 0x3 */ u8  unk_3;
    } ShopP5Recb14;
    typedef struct {
        /* 0x000 */ char        pad_000[0x7B8];
        /* 0x7B8 */ u8          unk_7B8;
        /* 0x7B9 */ u8          unk_7B9;
        /* 0x7BA */ char        pad_7BA[0x7BC - 0x7BA];
        /* 0x7BC */ ShopP5Recb14 unk_7BC[17];
    } ShopP5Viewb14;
    ShopP5Viewb14 *v = (ShopP5Viewb14 *)arg0;
    u8  temp_lr = v->unk_7B9;
    u8  var_r2 = 0;
    u16 var_r3 = 0;
    u8  ip;

    if ((s32)temp_lr > 0) {
        ip = v->unk_7B8;
        do {
            if (ip >= v->unk_7BC[var_r3].unk_3) {
                var_r2 += 1;
            }
            var_r3 += 1;
        } while ((s32)temp_lr > (s32)var_r3);
    }
    return var_r2;
}

void func_ov043_020afb60(ShopObject *arg0) {
    typedef struct {
        /* 0x000 */ char pad_000[0x860];
        /* 0x860 */ u16  unk_860;
    } ShopP5Viewb60;
    typedef struct {
        u8 unk_0 : 1;
    } ShopP5Flag;
    ShopP5Viewb60 *v = (ShopP5Viewb60 *)arg0;
    ShopP5Flag *flags;
    u16 var_r8;

    var_r8 = 0;
    if ((u32)v->unk_860 <= (u32)var_r8) {
        return;
    }
    flags = (ShopP5Flag *)&gSaveData.unk_1EB2;
    do {
        ShopItemSlot *slot = &arg0->slots[var_r8];
        if (slot->unk_04 == 1 && flags[Inventory_GetCategorizedIndex(slot->itemID)].unk_0 == 1) {
            slot->validFlag = 1;
        }
        var_r8 += 1;
    } while ((u32)v->unk_860 > (u32)var_r8);
}
