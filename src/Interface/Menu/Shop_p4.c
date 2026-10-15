#include "ShopPart.h"

void func_ov043_020af3f0(ShopState *arg0, s16 arg4, s16 arg5) {
    if (((s32) arg0 >= (s32) M2C_ERROR(/* Read from unset register $r2 */)) && ((s32) arg0 <= (s32) (M2C_ERROR(/* Read from unset register $r2 */) + arg4)) && ((s32) M2C_ERROR(/* Read from unset register $r1 */) >= (s32) M2C_ERROR(/* Read from unset register $r3 */)) && ((s32) M2C_ERROR(/* Read from unset register $r1 */) <= (s32) (M2C_ERROR(/* Read from unset register $r3 */) + arg5))) {

    }
}

void func_ov043_020af42c(ShopState *arg0) {
    SpriteCell *var_r3;
    void *temp_ip;
    void *temp_ip_2;
    void *temp_r3;
    void *var_r1;

    temp_ip = arg0->unk_28;
    if (temp_ip == NULL) {
        var_r1 = NULL;
    } else {
        temp_r3 = temp_ip->unk_8 + 0x20;
        var_r1 = temp_r3 + temp_r3->unk_18;
    }
    if (temp_ip == NULL) {
        var_r3 = NULL;
    } else {
        temp_ip_2 = temp_ip->unk_8 + 0x20;
        var_r3 = temp_ip_2 + temp_ip_2->unk_10;
    }
    Sprite_ChangeAnimation((Sprite *) arg0, var_r1, M2C_ERROR(/* Read from unset register $r1 */), var_r3);
}

void func_ov043_020af478(ShopState *arg0) {
    SpriteCell *var_r3;
    void *temp_ip;
    void *temp_ip_2;
    void *temp_r3;
    void *var_r1;

    temp_ip = arg0->unk_28;
    if (temp_ip == NULL) {
        var_r1 = NULL;
    } else {
        temp_r3 = temp_ip->unk_8 + 0x20;
        var_r1 = temp_r3 + temp_r3->unk_20;
    }
    if (temp_ip == NULL) {
        var_r3 = NULL;
    } else {
        temp_ip_2 = temp_ip->unk_8 + 0x20;
        var_r3 = temp_ip_2 + temp_ip_2->unk_18;
    }
    Sprite_ChangeAnimation((Sprite *) arg0, var_r1, M2C_ERROR(/* Read from unset register $r1 */), var_r3);
}

void func_ov043_020af4c4(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x34, &data_ov043_020ca210, arg1 * 0x34));
}

void func_ov043_020af4f0(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x18, &data_ov043_020ca218, arg1 * 0x18));
}

void func_ov043_020af51c(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x14, &data_ov043_020ca220, arg1 * 0x14));
}

void func_ov043_020af548(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 8, &data_ov043_020ca228, arg1 * 8));
}

void func_ov043_020af574(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0xC, &data_ov043_020ca230, arg1 * 0xC));
}

void func_ov043_020af5a0(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x2C, &data_ov043_020ca238, arg1 * 0x2C));
}

void func_ov043_020af5cc(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x10, &data_ov043_020ca248, arg1 * 0x10));
}

void func_ov043_020af5f8(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawDataWithOffset(1, arg0, 0x1C, &data_ov043_020ca250, arg1 * 0x1C));
}

void func_ov043_020af624(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x3DC0, &data_ov043_020ca210));
}

void func_ov043_020af648(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x1A40, &data_ov043_020ca218));
}

void func_ov043_020af66c(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x348, &data_ov043_020ca220));
}

void func_ov043_020af690(void *arg0, s32 arg1) {
    DatMgr_ReleaseData(DatMgr_LoadRawData(1, arg0, 0x4B0, &data_ov043_020ca228));
}
