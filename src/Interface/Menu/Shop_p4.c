#include "ShopPart.h"

BOOL func_ov043_020af3f0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s16 arg4, s16 arg5) {
    return arg0 >= arg2 && arg0 <= arg2 + arg4 && arg1 >= arg3 && arg1 <= arg3 + arg5;
}

void func_ov043_020af42c(Sprite *sprite, s16 animFrame) {
    Data *data;
    u8 *base;
    void *animData;
    SpriteCell *cellTable;

    data = sprite->resourceData;
    if (data == NULL) {
        animData = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        animData = base + *(s32 *) (base + 0x18);
    }
    if (data == NULL) {
        cellTable = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        cellTable = (SpriteCell *) (base + *(s32 *) (base + 0x10));
    }
    Sprite_ChangeAnimation(sprite, animData, animFrame, cellTable);
}

void func_ov043_020af478(Sprite *sprite, s16 animFrame) {
    Data *data;
    u8 *base;
    void *animData;
    SpriteCell *cellTable;

    data = sprite->resourceData;
    if (data == NULL) {
        animData = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        animData = base + *(s32 *) (base + 0x20);
    }
    if (data == NULL) {
        cellTable = NULL;
    } else {
        base = (u8 *) data->buffer + 0x20;
        cellTable = (SpriteCell *) (base + *(s32 *) (base + 0x18));
    }
    Sprite_ChangeAnimation(sprite, animData, animFrame, cellTable);
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
