#include "Engine/Text.h"
#include "Engine/Core/OamMgr.h"
#include "Engine/File/BinMgr.h"

s32 func_020107a4(TextObject* obj, s32 param_2, s32 param_3);

void func_02010870(TextObject* obj);

void  func_0200fa28(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fa2c(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fb88(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fad4(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
void  func_0200fc70(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
s32   func_02010808(TextObject* arg0, s32 arg1, s32 arg2);
void  func_020105b8(TextObject* obj);
void  func_02010adc(TextObject* obj, u16* arg1);
void  func_020105d4(TextObject* obj, u32 arg1, u32 arg2);
void  func_02011bec(TextObject* obj);
BOOL  func_02011c04(TextObject* obj);
void  func_02011c1c(TextObject* obj, u16 arg1);
void  func_02011d48(void);
void  func_02011494(TextObject* obj);
void* func_02011d8c(TextObject* obj, u8* arg1);
void  func_02011e94(TextObject* obj, s32 arg1, s32 arg2, u8* arg3);
void  func_02011e58(TextObject* obj, u16* arg1);
void  func_02011ecc(TextObject* obj);
void  func_02011df8(TextObject* obj);
u16   func_02011d4c(TextObject* obj);
s32   func_02011c48(TextObject* obj);
void  func_020119b0(TextObject* obj, s32 arg1, s32 arg2);
u16   func_020111f0(TextObject* obj);
s32   func_0201122c(TextObject*, s32);
s32   func_02011358(TextObject*, s32, s32);
void  func_02011474(TextObject*, u32, s32);
void  func_020119b0(TextObject* obj, s32 arg1, s32 arg2);
void* func_02011b7c(TextObject* obj, s32 arg1, s32 arg2);

typedef void (*FuncPtr)(u32*, u8*, u32*, s32, s32, u32, u32*, u32*);
const FuncPtr data_0205aea4[20] = {
    func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa2c, func_0200fb88, func_0200fa28,
    func_0200fa28, func_0200fad4, func_0200fc70, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28,
    func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28, func_0200fa28,
};

s32 data_0205ae64[0x10]  = {0xF};
s32 data_0205b454[0x100] = {
    0,          0,          1,          1,          0,          0,          -1,         -1,         0,          0,
    0,          0,          0,          0,          2,          1,          1,          0,          -1,         -1,
    0,          0,          0,          0,          0,          0,          4,          1,          1,          1,
    -1,         -1,         0,          0,          0,          0,          0,          0,          4,          1,
    1,          1,          4,          0,          4,          1,          1,          1,          0,          0,
    1,          2,          2,          0,          -1,         -1,         0,          0,          0,          0,
    0,          0,          2,          2,          0,          1,          -1,         -1,         0,          0,
    0,          0,          0,          0,          4,          2,          1,          2,          -1,         -1,
    0,          0,          0,          0,          0,          0,          4,          2,          1,          2,
    4,          0,          4,          2,          1,          2,          0,          0,          1,          4,
    2,          1,          -1,         -1,         0,          0,          0,          0,          0,          0,
    2,          4,          2,          2,          -1,         -1,         0,          0,          0,          0,
    0,          0,          4,          4,          0,          2,          -1,         -1,         0,          0,
    0,          0,          0,          0,          8,          4,          1,          3,          -1,         -1,
    0,          0,          0,          0,          0,          0,          1,          4,          2,          1,
    0,          4,          1,          4,          2,          1,          0,          0,          2,          4,
    2,          2,          0,          4,          2,          4,          2,          2,          0,          0,
    4,          8,          2,          3,          -1,         -1,         0,          0,          0,          0,
    0,          0,          8,          8,          0,          3,          -1,         -1,         0,          0,
    0,          0,          1075920923, 1076576293, 1077887025, 1078280259, 1080115293, 1080901735, 1083261063, 1084833955,
    1085751473, 1088110781, 1089159391, 1090076919, 1091256585, 1091911953, 1093878049, 1094402357, 1096368447, 1097548133,
    1098596727, 1101742483, 1102922167, 1103839679, 1106199015, 1106854387, 1107771909, 1109344793, 1110000163, 1111704111,
    1112883795, 1113670235, 1115505267, 1116029571, 1116816009, 1117602455, 1120223925, 1121141451, 1122190045, 1124549361,
    1126122255, 1126646565, 1127695155, 1129268025, 1130971991, 1133331339, 1134904211, 1135559593, 1136477109, 1137656775,
    1139229665, 1139622891, 1140409329, 1141589001, 1143161879, 1144734761, 1145390143, 1146176587, 1146700883, 1148142693,
    1150239875, 1151681697, 1152205995, 1153385661, 1154958537, 1157186779,
};
s32 data_0205ae24;
s32 data_0205b0f4;

const u16 data_0205aef4[256] = {
    0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xA,    0xB,    0xC,    0xD,    0xE,
    0xF,    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,
    0x1E,   0x1F,   0x8140, 0x8149, 0x8160, 0x818C, 0x8190, 0x818B, 0x8195, 0x818C, 0x8169, 0x816A, 0x8196, 0x817B, 0x8143,
    0x817C, 0x8144, 0x815E, 0x824F, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256, 0x8257, 0x8258, 0x8146, 0x8147,
    0x8183, 0x8181, 0x8184, 0x8148, 0x8197, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266, 0x8267, 0x8268, 0x8269,
    0x826A, 0x826B, 0x826C, 0x826D, 0x826E, 0x826F, 0x8270, 0x8271, 0x8272, 0x8273, 0x8274, 0x8275, 0x8276, 0x8277, 0x8278,
    0x8279, 0x816D, 0x818F, 0x816E, 0x814F, 0x8151, 0x824D, 0x8281, 0x8282, 0x8283, 0x8284, 0x8285, 0x8286, 0x8287, 0x8288,
    0x8289, 0x828A, 0x828B, 0x828C, 0x828D, 0x828E, 0x828F, 0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296, 0x8297,
    0x8298, 0x8299, 0x829A, 0x8167, 0x8162, 0x8168, 0x8164, 0x8140, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159, 0x8159,
    0x8159,
};

u16 func_0200f94c(s32 idx) {
    return data_0205aef4[idx];
}

u16 func_0200f960(u32 arg0, u8* arg1) {
    u16 offset      = ((arg0 & 0x7F00) >> 6);
    u16 base_offset = arg1[offset] | (arg1[offset + 1] << 8);
    u16 width       = (arg1[offset + 3] << 8) | arg1[offset + 2];
    u16 result      = 0;

    if (base_offset != 0xFFFF) {
        u8 char_width = arg1[(u16)(base_offset + ((arg0 & 0xFF) - 0x40))];
        if (char_width != 0xFF) {
            result = char_width + width;
        }
    }

    return result;
}

u16 func_0200f9e0(s32 arg0, u8* arg1, u8* arg2, u32 arg3, u16 arg4) {
    u16 var_r4 = 0;

    if (arg2 != 0) {
        u32 idx = func_0200f960(arg0, arg1);
        if (idx < arg3) {
            var_r4 = arg2[idx];
        }
    }
    if (var_r4 == 0) {
        var_r4 = arg4;
    }
    return var_r4;
}

u32 func_0200fa10(u32 param_1, u8* param_2, u32 param_3, u32 param_4) {
    return param_4 * func_0200f960(param_1, param_2) + param_3;
}

void func_0200fa28(u32*, u8*, u32*, s32, s32, u32, u32*, u32*) {
    return;
}

void func_0200fa2c(u32* arg0, u8* arg1, u32* arg2, s32 arg3, s32 arg4, u32 arg5, u32* arg6, u32* arg7) {
    s32 temp_r3 = arg3 * 4;
    s32 var_r7  = 0;
    if (arg4 <= 0) {
        return;
    }
    u32 var_r9 = 0;
    do {
        u32 var_r6 = 0;
        s32 var_r8 = 0;
        s32 var_r5 = arg1[var_r9];
        if (temp_r3 > 0) {
            do {
                u32 temp_r11 = arg2[var_r5 & 1];
                var_r5       = var_r5 >> 1;
                var_r6 |= temp_r11 << var_r8;
                var_r8 += 4;
            } while (var_r8 < temp_r3);
        }
        u32 temp_r11_2 = arg0[var_r7] & arg6[0];
        arg0[var_r7]   = temp_r11_2;
        var_r9 += arg5;
        arg0[var_r7]   = temp_r11_2 | (var_r6 << arg7[0]);
        u32 temp_r11_3 = arg0[var_r7 + 8];
        var_r7 += 1;
        u32 temp_r11_4       = temp_r11_3 & arg6[1];
        arg0[var_r7 - 1 + 8] = temp_r11_4;
        arg0[var_r7 - 1 + 8] = temp_r11_4 | (var_r6 >> arg7[1]);
    } while (var_r7 < arg4);
}

void func_0200fad4(u32* arg0, u8* arg1, u32* arg2, s32 arg3, s32 arg4, u32 arg5, u32* arg6, u32* arg7) {
    s32 temp_r3 = arg3 * 4;
    s32 var_r7  = 0;
    if (arg4 <= 0) {
        return;
    }
    u32 var_r9 = 0;
    do {
        u32 var_r6 = 0;
        s32 var_r8 = 0;
        s32 var_r5 = arg1[var_r9] | (arg1[var_r9 + 1] << 8);
        if (temp_r3 > 0) {
            do {
                u32 temp_r11 = arg2[var_r5 & 3];
                var_r5       = var_r5 >> 2;
                var_r6 |= temp_r11 << var_r8;
                var_r8 += 4;
            } while (var_r8 < temp_r3);
        }
        u32 temp_r11_2 = arg0[var_r7] & arg6[0];
        arg0[var_r7]   = temp_r11_2;
        var_r9 += arg5;
        arg0[var_r7]   = temp_r11_2 | (var_r6 << arg7[0]);
        u32 temp_r11_3 = arg0[var_r7 + 8];
        var_r7 += 1;
        u32 temp_r11_4       = temp_r11_3 & arg6[1];
        arg0[var_r7 - 1 + 8] = temp_r11_4;
        arg0[var_r7 - 1 + 8] = temp_r11_4 | (var_r6 >> arg7[1]);
    } while (var_r7 < arg4);
}

void func_0200fb88(u32* arg0, u8* arg1, u32* arg2, s32 arg3, s32 arg4, u32 arg5, u32* arg6, u32* arg7) {
    s32 temp_r3 = arg3 * 4;
    s32 var_r5  = 0;
    if (arg4 <= 0) {
        return;
    }
    s32 var_r11 = 0;
    do {
        u32 var_ip = 0;
        u32 var_lr = 0;
        s32 var_r6 = 0;
        s32 var_r4 = arg1[var_r11];
        if (temp_r3 > 0) {
            do {
                s32 temp_r8 = var_r4 & 1;
                var_ip |= arg2[temp_r8] << var_r6;
                var_lr |= data_0205ae64[temp_r8] << var_r6;
                var_r4 = var_r4 >> 1;
                var_r6 += 4;
            } while (var_r6 < temp_r3);
        }
        u32* temp_r6   = arg0 + var_r5;
        u32  temp_r8_2 = temp_r6[0] & (arg6[0] | (var_lr << arg7[0]));
        temp_r6[0]     = temp_r8_2;
        var_r11 += arg5;
        temp_r6[0]  = temp_r8_2 | (var_ip << arg7[0]);
        u32 temp_r4 = temp_r6[8];
        var_r5 += 1;
        u32 temp_r8_3 = temp_r4 & (arg6[1] | (var_lr >> arg7[1]));
        temp_r6[8]    = temp_r8_3;
        temp_r6[8]    = temp_r8_3 | (var_ip >> arg7[1]);
    } while (var_r5 < arg4);
}

void func_0200fc70(u32* arg0, u8* arg1, u32* arg2, s32 arg3, s32 arg4, u32 arg5, u32* arg6, u32* arg7) {
    s32 temp_r3 = arg3 * 4;
    s32 var_r5  = 0;
    if (arg4 <= 0) {
        return;
    }
    s32 var_r7 = 0;
    do {
        u32 var_ip = 0;
        u32 var_lr = 0;
        s32 var_r4 = arg1[var_r7] | (arg1[var_r7 + 1] << 8);
        s32 var_r6 = 0;
        if (temp_r3 > 0) {
            do {
                s32 temp_r9 = var_r4 & 3;
                var_ip |= arg2[temp_r9] << var_r6;
                var_lr |= data_0205ae64[temp_r9] << var_r6;
                var_r4 = var_r4 >> 2;
                var_r6 += 4;
            } while (var_r6 < temp_r3);
        }
        u32* temp_r6   = arg0 + var_r5;
        u32  temp_r9_2 = temp_r6[0] & (arg6[0] | (var_lr << arg7[0]));
        temp_r6[0]     = temp_r9_2;
        var_r7 += arg5;
        temp_r6[0]  = temp_r9_2 | (var_ip << arg7[0]);
        u32 temp_r4 = temp_r6[8];
        var_r5 += 1;
        u32 temp_r9_3 = temp_r4 & (arg6[1] | (var_lr >> arg7[1]));
        temp_r6[8]    = temp_r9_3;
        temp_r6[8]    = temp_r9_3 | (var_ip >> arg7[1]);
    } while (var_r5 < arg4);
}

s32 func_0200fd64(UnkMedText* arg0) {
    s32 temp_r5;
    s32 var_ip;

    s32 var_r4 = arg0->unk_10;
    if (var_r4 > 8) {
        var_r4 = 8;
    }
    var_ip  = arg0->unk_14;
    temp_r5 = arg0->unk_0C;
    if (var_ip > 8) {
        var_ip = 8;
    }
    data_0205aea4[(arg0->unk_04 + (arg0->unk_02 * 4))](arg0->unk_1C, arg0->unk_18, arg0->unk_20, var_r4, var_ip, arg0->unk_08,
                                                       ((var_r4 * 0x60) + (s32)&data_0205b0f4) + (temp_r5 * 0xC),
                                                       (u8*)&data_0205ae24 + (temp_r5 * 8));

    return ((var_r4 + temp_r5 + 7) & ~7) >> 3;
}

s32 func_0200fe08(TextObject* obj, s32 xOffset, s32 yOffset, s32 arg3, s32 arg4, s32 arg5) {
    s32 sp0 = xOffset;
    s32 sp4 = yOffset;
    s32 sp8 = arg3;
    s32 spC;
    s32 sp10;
    s32 sp14;
    u16 temp_r4 = obj->unk_20;
    if ((s32)(xOffset + arg4) > (s32)temp_r4) {
        arg4 = temp_r4 - xOffset;
    }
    u16 temp_r2 = obj->unk_22;
    if ((s32)(sp4 + arg5) > (s32)temp_r2) {
        arg5 = (s32)temp_r2 - sp4;
    }
    if (arg4 <= 0 || arg5 <= 0) {
        return 0;
    }
    {
        spC             = (s32)(u16)(obj->unk_36 + (((sp4 >> 3) * obj->unk_18[0]) + (sp0 >> 3)));
        s32   temp_r2_2 = sp4 & 7;
        void* temp_r4_2 = (u8*)obj + 0x310;
        obj->unk_88     = (void*)((u8*)temp_r4_2 + (temp_r2_2 * 4));
        obj->unk_78     = sp0 & 7;
        obj->unk_80     = 8 - temp_r2_2;
        sp14            = 0;
        if (arg5 > 0) {
            do {
                u16*  var_r6 = (u16*)func_020107a4(obj, sp0, sp4 + sp14);
                void* var_r7 = (void*)func_02010808(obj, sp0, sp4 + sp14);
                if (var_r6 == NULL && var_r7 == NULL) {
                    goto block_14;
                }
                if (var_r6 != NULL && *var_r6 == obj->unk_34) {
                block_14:
                    MI_CpuFill(obj->unk_30, temp_r4_2, 0x20);
                } else {
                    MI_CpuCopy(var_r7, temp_r4_2, 0x20);
                }
                s32 var_r5  = 0;
                obj->unk_84 = (sp14 * obj->unk_74) + sp8;
                obj->unk_7C = arg4;
                if (arg4 > 0) {
                    s32 var_r11 = 0;
                    do {
                        u16*  var_r8;
                        void* var_r9;
                        if (var_r6 != NULL) {
                            var_r8 = var_r6 + 2;
                        } else {
                            var_r8 = (u16*)func_020107a4(obj, sp0 + var_r5 + 8, sp4 + sp14);
                        }
                        if (var_r7 != NULL) {
                            var_r9 = (void*)((u8*)var_r7 + 0x20);
                        } else {
                            var_r9 = (void*)func_02010808(obj, sp0 + var_r5 + 8, sp4 + sp14);
                        }
                        if (var_r8 == NULL && var_r9 == NULL) {
                            goto block_29;
                        }
                        if (var_r8 != NULL && *var_r8 == obj->unk_34) {
                        block_29:
                            MI_CpuFill(obj->unk_30, (u8*)temp_r4_2 + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(var_r9, (u8*)temp_r4_2 + 0x20, 0x20);
                        }
                        sp10 = func_0200fd64((UnkMedText*)((u8*)obj + 0x6C));
                        if (var_r6 != NULL) {
                            *var_r6 = (u16)(spC + var_r11);
                        }
                        if (var_r7 != NULL) {
                            MI_CpuCopy(temp_r4_2, var_r7, 0x20);
                        }
                        if (sp10 > 1) {
                            if (var_r8 != NULL) {
                                *var_r8 = (u16)(spC + var_r11 + 1);
                            }
                            if (var_r9 != NULL) {
                                MI_CpuCopy((u8*)temp_r4_2 + 0x20, var_r9, 0x20);
                            }
                        }
                        var_r6 = var_r8;
                        var_r7 = var_r9;
                        MI_CpuCopy((u8*)temp_r4_2 + 0x20, temp_r4_2, 0x20);
                        var_r11 += 1;
                        obj->unk_84 = obj->unk_84 + obj->unk_6E;
                        var_r5 += 8;
                        obj->unk_7C = obj->unk_7C - 8;
                    } while (obj->unk_7C > 0);
                }
                obj->unk_88   = temp_r4_2;
                s32 temp_r0_2 = sp14 + obj->unk_80;
                sp14          = temp_r0_2;
                s32 temp_r1   = (s32)arg5 - temp_r0_2;
                obj->unk_80   = temp_r1;
                spC           = (s32)(u16)(spC + obj->unk_18[0]);
                if (temp_r1 > 8) {
                    obj->unk_80 = 8;
                }
            } while (sp14 < arg5);
        }
        return (s32)(((sp0 + (s32)arg4 + 6) - (sp0 & ~7)) & ~7) >> 3;
    }
    return 0;
}

u16 func_02010170(TextObject* obj) {
    if (obj->unk_64 != obj->unk_66) {
        u16 val = obj->unk_68[obj->unk_66];
        obj->unk_66++;
        obj->unk_66 = obj->unk_66 & 0xff;
        return val;
    }
    return 0;
}

BOOL func_020101ac(TextObject* obj, u32 arg1) {
    u32 iVar1 = func_0200fa10(arg1, obj->unk_50, obj->unk_54, obj->unk_46);
    u32 uVar1 = func_0200f9e0(arg1, obj->unk_50, obj->unk_58, obj->unk_5C, obj->unk_60);

    func_0200fe08(obj, obj->xOffset, obj->yOffset, iVar1, uVar1, obj->unk_4E);
    if (obj->unk_00 == 0) {
        obj->xOffset += uVar1;
    } else {
        obj->yOffset += obj->unk_4E;
    }
    if ((obj->unk_10[0] != 0) && (obj->unk_70 == 0)) {
        if (obj->unk_00 == 0) {
            func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_10[0], obj->unk_4E);
            obj->xOffset += obj->unk_10[0];
        } else {
            func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_4C, obj->unk_10[1]);
            obj->yOffset += obj->unk_10[1];
        }
    }
    return TRUE;
}

static inline void SetOffsets(TextObject* obj, u32 x, u32 y) {
    obj->xOffset = x;
    obj->yOffset = y;
}

BOOL func_020102d8(TextObject* obj, u32 arg1, u32 arg2) {
    BOOL BVar1 = FALSE;

    switch (arg1) {
        case 0:
            break;
        case 10:
            if (obj->unk_00 == 0) {
                SetOffsets(obj, 0, obj->yOffset + (obj->unk_4E + obj->unk_10[1]));
            } else {
                SetOffsets(obj, obj->xOffset - (obj->unk_4C + obj->unk_10[0]), 0);
            }
            break;
        case 9:
            if (obj->unk_00 == 0) {
                if (arg2 == 1) {
                    func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, 8 - (obj->xOffset & 7U), obj->unk_4E);
                }
                SetOffsets(obj, (obj->xOffset + 8) & ~7, obj->yOffset);
            } else {
                if (arg2 == 1) {
                    func_0200fe08(obj, obj->xOffset, obj->yOffset, obj->unk_2C, obj->unk_4C, 8 - (obj->yOffset & 7U));
                }
                SetOffsets(obj, obj->xOffset, (obj->yOffset + 8) & ~7);
            }
            BVar1 = TRUE;
            break;
    }

    return BVar1;
}

void func_020103f4(TextObject* arg0, s32 arg1, s32 unused) {
    if (arg1 & 0xFF00) {
        func_020101ac(arg0, arg1);
        return;
    }
    func_020102d8(arg0, arg1, 1);
}

void func_02010414(TextObject* obj) {
    if (obj->unk_18[0] == 0 && obj->unk_18[1] == 0) {
        return;
    }

    u16 i;
    s32 iVar3 = func_020107a4(obj, 0, 0);

    if (iVar3 != 0) {
        for (i = 0; i < obj->unk_18[1]; i++) {
            MI_CpuFillU16(obj->unk_34, iVar3, obj->unk_18[0] << 1);
            iVar3 += obj->unk_04 * 2;
        }
    } else {
        MI_CpuFill(obj->unk_30, func_02010808(obj, 0, 0), obj->unk_18[0] * obj->unk_18[1] * 0x20);
    }
}

void func_020104c0(TextObject* obj, UnkSmallText* param_2) {
    obj->unk_00 = param_2->unk_0C;
    obj->unk_3C = param_2->unk_08;
    obj->data   = NULL;
    obj->unk_04 = 0x20;
    obj->unk_06 = 0x20;

    obj->unk_10 = param_2->unk_18;
    obj->unk_14 = param_2->unk_10;
    obj->unk_18 = param_2->unk_14;

    obj->unk_1C = param_2->unk_10[0] << 3;
    obj->unk_1E = param_2->unk_10[1] << 3;
    obj->unk_20 = param_2->unk_14[0] << 3;
    obj->unk_22 = param_2->unk_14[1] << 3;

    obj->unk_24 = param_2->unk_1C;
    obj->unk_28 = param_2->unk_20;
    obj->unk_34 = param_2->unk_2C;
    obj->unk_36 = param_2->unk_2E;

    func_020105d4(obj, param_2->unk_24, param_2->unk_28);
    obj->unk_38 = param_2->unk_24;
    obj->unk_2C = func_0200fa10(0x8140, obj->unk_50, obj->unk_54, obj->unk_46);
    func_02010adc(obj, &param_2->unk_30);
    func_02010870(obj);
}

void func_020105b8(TextObject* obj) {
    DatMgr_ReleaseData(obj->data);
    obj->data = NULL;
}

void func_020105d4(TextObject* obj, u32 arg1, u32 arg2) {
    obj->unk_38   = (s32)arg1;
    void* var_r0  = obj->data;
    void* temp_r4 = (void*)((u8*)(u32)arg1 + arg2 * 0x24);
    if (var_r0 == NULL) {
        if (var_r0 != NULL) {
            DatMgr_ReleaseData((Data*)var_r0);
        }
        s32 var_r6 = 0;
        s32 temp_r7;
        if (*(s32*)((u8*)temp_r4 + 8) == 1) {
            var_r6 = 1;
        }
        s32 temp_r8 = *(s32*)temp_r4;
        s32 temp_r9 = *(s32*)((u8*)temp_r4 + 4);
        temp_r7     = obj->unk_3C;
        if (BinMgr_FindById(temp_r8) == NULL) {
            var_r0 = DatMgr_LoadPackEntry(temp_r7, NULL, 0, (BinIdentifier*)temp_r8, temp_r9, (BOOL)var_r6);
        } else {
            var_r0 = DatMgr_LoadPackEntryDirect(temp_r7, (BinIdentifier*)temp_r8, temp_r9, var_r6);
        }
    }
    obj->data     = (Data*)var_r0;
    obj->unk_44   = (u16) * (u16*)((u8*)temp_r4 + 12);
    obj->unk_46   = (u16) * (u16*)((u8*)temp_r4 + 14);
    u16 temp_w2   = *(u16*)((u8*)temp_r4 + 16);
    u16 temp_w1   = *(u16*)((u8*)temp_r4 + 18);
    obj->unk_4C   = temp_w2;
    obj->unk_4E   = temp_w1;
    s32   temp_r3 = *(s32*)((u8*)temp_r4 + 20);
    void* var_r1  = NULL;
    if (var_r0 == NULL || temp_r3 <= 0) {
        var_r1 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1        = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3 * 8);
    }
    obj->unk_50     = (u8*)var_r1;
    s32   temp_r3_2 = *(s32*)((u8*)temp_r4 + 24);
    void* var_r1_2  = NULL;
    if (var_r0 == NULL || temp_r3_2 <= 0) {
        var_r1_2 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1_2      = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3_2 * 8);
    }
    obj->unk_54     = (u32)var_r1_2;
    s32   temp_r3_3 = *(s32*)((u8*)temp_r4 + 28);
    void* var_r1_3  = NULL;
    if (var_r0 == NULL || temp_r3_3 <= 0) {
        var_r1_3 = NULL;
    } else {
        void* temp_r2 = (u8*)((Data*)var_r0)->buffer + 0x20;
        var_r1_3      = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3_3 * 8);
    }
    obj->unk_58 = (u8*)var_r1_3;
    obj->unk_5C = *(s32*)((u8*)((Data*)var_r0)->buffer + (*(s32*)((u8*)temp_r4 + 28) * 8) + 0x24);
    // Actually m2c: arg0->unk5C = (s32)(var_r0->buffer + (temp_r4->unk1C * 8))->unk24;
    obj->unk_60  = *(u16*)((u8*)temp_r4 + 32);
    obj->unk_48  = (((*(u16*)((u8*)temp_r4 + 16)) + 7) & ~7) >> 3;
    obj->unk_4A  = (((*(u16*)((u8*)temp_r4 + 18)) + 7) & ~7) >> 3;
    obj->unk_6C  = 0;
    obj->unk_6E  = obj->unk_44;
    obj->unk_74  = (((obj->unk_4C * obj->unk_44) + 7) & ~7) >> 3;
    obj->unk_8C  = (void*)((u8*)obj + 0x90);
    obj->unk_70  = 0;
    s32 temp_y   = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset = *(s32*)((u8*)obj + 0x08);
    obj->yOffset = temp_y;
}

s32 func_020107a4(TextObject* obj, s32 param_2, s32 param_3) {
    if (param_2 < 0 || param_2 >= obj->unk_20 || param_3 < 0 || param_3 >= obj->unk_22) {
        return 0;
    }
    s32 row = ((int)(param_3 + obj->unk_1E) >> 3) * obj->unk_04;
    s32 col = (int)(param_2 + obj->unk_1C) >> 3;
    return obj->unk_28 + row * 2 + col * 2;
}

s32 func_02010808(TextObject* arg0, s32 arg1, s32 arg2) {
    register u32 var_lr = 0;
    s32          var_ip = var_lr - 1;

    if ((arg1 >= 0) && (arg1 < arg0->unk_20) && (arg2 >= 0) && (arg2 < arg0->unk_22)) {
        s32 var_r3 = (arg2 >> 3) * arg0->unk_18[0];
        u32 var_r2 = (arg2 << 0x1D);
        var_r3 += arg1 >> 3;
        var_r3 <<= 5;
        var_r2 >>= 0x1B;
        var_ip = var_r3 + var_r2;
    }

    if (var_ip >= 0) {
        var_lr = arg0->unk_24 + (var_ip & ~0x1F);
    }

    return var_lr;
}

void func_02010870(TextObject* obj) {
    obj->unk_68 = &obj->unk_110;
    obj->unk_64 = 0;
    obj->unk_66 = 0;
}

BOOL func_02010888(TextObject* obj) {
    return obj->unk_64 == obj->unk_66;
}

void func_020108a0(TextObject* obj, u16 param_2) {
    obj->unk_68[obj->unk_64] = param_2;
    obj->unk_64++;
    obj->unk_64 &= 0xff;
}

s32 func_020108cc(TextObject* arg0) {
    u16 var_r2;
    u32 temp_r0;
    s32 temp_r5;

loop:
    s32 val = func_02010170(arg0);
    if (val == 0x40) {
        temp_r5 = func_02010170(arg0);
        temp_r0 = func_02010170(arg0);

        if ((temp_r0 >= 0x30) && (temp_r0 <= 0x39)) {
            var_r2 = temp_r0 - 0x30;
        } else if ((temp_r0 >= 0x61) && (temp_r0 <= 0x66)) {
            var_r2 = temp_r0 - 0x57;
        } else if ((temp_r0 >= 0x41) && (temp_r0 <= 0x46)) {
            var_r2 = temp_r0 - 0x37;
        } else {
            var_r2 = 0x10;
        }

        if (temp_r5 <= 0x46) {
            if (temp_r5 != 0x46) {
            } else {
                func_020105d4(arg0, arg0->unk_38, var_r2);
            }
        } else {
            switch (temp_r5 - 0x61) {
                case 1:
                    arg0->unk_90 = var_r2;
                    break;
                case 5:
                    arg0->unk_94 = var_r2;
                    break;
                case 0:
                    arg0->unk_98 = var_r2;
                    break;
                case 4:
                    arg0->unk_9C = var_r2;
                    break;
            }
        }
        goto loop;
    }
    return val;
}

s32 func_020109d0(TextObject* arg0) {
    u16 temp_r4;
    u16 temp_r5;

    temp_r4 = arg0->unk_64;
    temp_r5 = arg0->unk_66;
loop_1:
    s32 val = func_02010170(arg0);
    if (val == 0x40) {
        func_02010170(arg0);
        func_02010170(arg0);
        goto loop_1;
    }
    arg0->unk_64 = temp_r4;
    arg0->unk_66 = temp_r5;
    return val;
}

void* func_02010a10(TextObject* arg0, u8* arg1) {
    u8  var_r2;
    u8* var_r4;

    var_r4 = arg1;
    var_r2 = *var_r4;
    if (var_r2 != 0) {
        do {
            if (var_r2 & 0x80) {
                func_020108a0(arg0, (u16)(var_r4[1] | (var_r2 << 8)));
                var_r4 += 2;
            } else {
                func_020108a0(arg0, func_0200f94c(var_r2));
                var_r4 += 1;
            }
            var_r2 = *var_r4;
        } while (var_r2 != 0);
    }
    return var_r4 + 1;
}

void func_02010a7c(TextObject* arg0) {
    while (func_02010888(arg0) == FALSE) {
        s32 charCode = func_020108cc(arg0);
        s32 result   = func_020109d0(arg0);
        s32 val      = 0;

        if (result == 0 || result == 0xA) {
            val = 1;
        }

        func_020103f4(arg0, charCode, val);
    }
}

void func_02010adc(TextObject* obj, u16* arg1) {
    s32 var_ip = 0;
    do {
        u16 temp_r3                = arg1[var_ip];
        ((u32*)obj)[0x24 + var_ip] = temp_r3;
        ((u32*)obj)[0x34 + var_ip] = temp_r3;
        var_ip += 1;
    } while (var_ip < 0x10);
    u32 temp_r1 = *(u32*)((u8*)obj + 0x90);
    temp_r1 |= temp_r1 * 0x10;
    temp_r1 |= temp_r1 << 8;
    *(u32*)((u8*)obj + 0x30) = temp_r1 | (temp_r1 << 0x10);
}

void Text_RenderToScreen(TextObject* obj, s32 xOffset, s32 yOffset, const char* string) {
    func_02010a7c(obj);
    obj->xOffset = xOffset;
    obj->yOffset = yOffset;
    func_02010a10(obj, string);
    func_02010a7c(obj);
}

void func_02010b50(TextObject* obj) {
    s32 sVar1;

    func_02010414(obj);
    if (obj->unk_00 == 0) {
        sVar1 = 0;
    } else {
        sVar1 = obj->unk_20 - obj->unk_4C;
    }
    obj->xOffset = sVar1;
    obj->yOffset = 0;
}

void func_02010b84(TextObject* obj, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 sp8 = arg1;
    s32 spC = arg2;
    s32 sp10;
    s32 sp14;
    s32 sp18;
    s32 sp1C;
    s32 sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    s32 var_r3 = arg3;
    if (arg1 < 0) {
        var_r3 += arg1;
        sp8 = 0;
    }
    s32 var_r0 = arg4;
    if (spC < 0) {
        var_r0 += spC;
        spC = 0;
    }
    u16 temp_r2 = obj->unk_20;
    if ((s32)(sp8 + var_r3) > (s32)temp_r2) {
        var_r3 = temp_r2 - sp8;
    }
    u16 temp_r2_2 = obj->unk_22;
    if ((s32)(spC + var_r0) > (s32)temp_r2_2) {
        var_r0 = temp_r2_2 - spC;
    }
    if ((var_r3 > 0) && (var_r0 > 0)) {
        sp30         = (s32)obj->unk_70;
        obj->unk_70  = NULL;
        s32 temp_r1  = sp8 + var_r3;
        sp18         = temp_r1;
        obj->xOffset = temp_r1;
        s32 temp_r0v = spC + var_r0;
        sp14         = temp_r0v;
        obj->yOffset = temp_r0v;
        s32 temp_r4  = spC & 7;
        if (temp_r4 != 0) {
            s32 temp_r0_2 = sp8 & 7;
            if (temp_r0_2 != 0) {
                func_0200fe08(obj, sp8, spC, obj->unk_2C, 8 - temp_r0_2, 8 - temp_r4);
            }
            s32 var_r7  = (sp8 + 7) & ~7;
            s32 temp_r5 = sp18 & ~7;
            if (var_r7 < temp_r5) {
                do {
                    func_0200fe08(obj, var_r7, spC, obj->unk_2C, 8, 8 - temp_r4);
                    var_r7 += 8;
                } while (var_r7 < temp_r5);
            }
            s32 temp_r1_2 = sp18 & 7;
            if ((temp_r1_2 != 0) && ((s32)(sp8 & ~7) < temp_r5)) {
                func_0200fe08(obj, temp_r5, spC, obj->unk_2C, temp_r1_2, 8 - temp_r4);
            }
        }
        s32 var_r8   = (spC + 7) & ~7;
        s32 temp_r6  = sp18 & ~7;
        s32 var_r7_2 = (sp8 + 7) & ~7;
        sp10         = sp14 & ~7;
        if (func_020107a4(obj, 0, 0) != 0) {
            if (var_r8 < sp10) {
                s32 temp_r0_3 = sp8 & 7;
                sp1C          = temp_r0_3;
                s32 temp_r5_2 = sp18 & 7;
                sp34          = var_r7_2 >> 3;
                do {
                    if (sp1C != 0) {
                        func_0200fe08(obj, sp8, var_r8, obj->unk_2C, 8 - temp_r0_3, 8);
                    }
                    u16* temp_r0_4 = (u16*)func_020107a4(obj, 0, var_r8);
                    s32  var_r3_2  = var_r7_2;
                    if (var_r7_2 < temp_r6) {
                        s32 var_r11 = sp34;
                        do {
                            s32 temp_r1_3 = var_r11 * 2;
                            var_r11 += 1;
                            temp_r0_4[temp_r1_3] = obj->unk_34;
                            var_r3_2 += 8;
                        } while (var_r3_2 < temp_r6);
                    }
                    if ((temp_r5_2 != 0) && ((s32)(sp8 & ~7) < temp_r6)) {
                        func_0200fe08(obj, temp_r6, var_r8, obj->unk_2C, temp_r5_2, 8);
                    }
                    var_r8 += 8;
                } while (var_r8 < sp10);
            }
        } else if (var_r8 < sp10) {
            s32 temp_r0_5 = sp8 & 7;
            sp2C          = temp_r0_5;
            sp28          = 8 - temp_r0_5;
            sp24          = sp18 & 7;
            sp20          = sp8 & ~7;
            sp38          = var_r7_2 >> 3;
            do {
                if (sp2C != 0) {
                    func_0200fe08(obj, var_r7_2, var_r8, obj->unk_2C, sp28, 8);
                }
                void* temp_r4_2 = (void*)func_02010808(obj, var_r7_2, var_r8);
                s32   var_r9    = var_r7_2;
                if (var_r7_2 < temp_r6) {
                    s32 var_r5 = sp38;
                    do {
                        MI_CpuFill(obj->unk_30, (u8*)temp_r4_2 + (var_r5 << 5), 0x20);
                        var_r5 += 1;
                        var_r9 += 8;
                    } while (var_r9 < temp_r6);
                }
                if ((sp24 != 0) && (sp20 < temp_r6)) {
                    func_0200fe08(obj, temp_r6, var_r8, obj->unk_2C, sp24, 8);
                }
                var_r8 += 8;
            } while (var_r8 < sp10);
        }
        s32 temp_r4_3 = sp14 & 7;
        if ((temp_r4_3 != 0) && ((s32)(spC & ~7) < sp10)) {
            s32 temp_r0_6 = sp8 & 7;
            if (temp_r0_6 != 0) {
                func_0200fe08(obj, sp8, sp10, obj->unk_2C, 8 - temp_r0_6, temp_r4_3);
            }
            if (var_r7_2 < temp_r6) {
                do {
                    func_0200fe08(obj, var_r7_2, sp10, obj->unk_2C, 8, temp_r4_3);
                    var_r7_2 += 8;
                } while (var_r7_2 < temp_r6);
            }
            s32 temp_r1_4 = sp18 & 7;
            if ((temp_r1_4 != 0) && ((s32)(sp8 & ~7) < temp_r6)) {
                func_0200fe08(obj, temp_r6, sp10, obj->unk_2C, temp_r1_4, temp_r4_3);
            }
        }
        obj->unk_70 = (u16*)sp30;
    }
}

s32 func_02010fd8(TextObject* obj, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 sp0     = arg2;
    s32 sp4     = arg3;
    u16 temp_r1 = obj->unk_20;
    u16 temp_r2 = obj->unk_22;
    if ((s32)(arg1 + arg4) > (s32)temp_r1) {
        arg4 = temp_r1 - arg1;
    }
    if ((s32)(sp0 + arg5) > (s32)temp_r2) {
        arg5 = temp_r2 - sp0;
    }
    if (arg4 <= 0 || arg5 <= 0) {
        return 0;
    }
    {
        void* temp_r4   = (u8*)obj + 0x318;
        s32   temp_r1_2 = sp0 & 7;
        obj->unk_90     = (void*)((u8*)temp_r4 + (temp_r1_2 * 4));
        obj->unk_80     = arg1 & 7;
        obj->unk_88     = 8 - temp_r1_2;
        s32 var_r5      = 0;
        if (arg5 > 0) {
            do {
                void* temp_r0 = func_02011b7c(obj, arg1, sp0 + var_r5);
                void* var_r8  = temp_r0;
                if (var_r8 == NULL) {
                    MI_CpuFill(*(void**)((u8*)obj + 0x34), temp_r4, 0x20);
                } else {
                    MI_CpuCopy(temp_r0, temp_r4, 0x20);
                }
                s32 var_r6  = 0;
                obj->unk_8C = (var_r5 * obj->unk_7C) + sp4;
                obj->unk_84 = arg4;
                if (arg4 > 0) {
                    do {
                        void* temp_r0_3 = func_02011b7c(obj, arg1 + var_r6 + 8, sp0 + var_r5);
                        if (temp_r0_3 == NULL) {
                            MI_CpuFill(*(void**)((u8*)obj + 0x34), (u8*)temp_r4 + 0x20, 0x20);
                        } else {
                            MI_CpuCopy(temp_r0_3, (u8*)temp_r4 + 0x20, 0x20);
                        }
                        s32 temp_r7 = func_0200fd64((UnkMedText*)((u8*)obj + 0x74));
                        if (var_r8 != NULL) {
                            MI_CpuCopy(temp_r4, var_r8, 0x20);
                        }
                        if ((temp_r7 > 1) && (temp_r0_3 != NULL)) {
                            MI_CpuCopy((u8*)temp_r4 + 0x20, temp_r0_3, 0x20);
                        }
                        var_r8 = temp_r0_3;
                        MI_CpuCopy((u8*)temp_r4 + 0x20, temp_r4, 0x20);
                        var_r6 += 8;
                        obj->unk_8C = obj->unk_8C + *(u16*)((u8*)obj + 0x76);
                        obj->unk_84 = obj->unk_84 - 8;
                    } while (obj->unk_84 > 0);
                }
                obj->unk_90 = temp_r4;
                var_r5 += obj->unk_88;
                s32 temp_r0_4 = arg5 - var_r5;
                obj->unk_88   = temp_r0_4;
                if (temp_r0_4 > 8) {
                    obj->unk_88 = 8;
                }
            } while (var_r5 < arg5);
        }
        return (s32)(((arg1 + arg4 + 6) - (arg1 & ~7)) & ~7) >> 3;
    }
    return 0;
}
u16 func_020111f0(TextObject* obj) {
    if (obj->unk_6C != obj->unk_6E) {
        u16 val = obj->unk_70[obj->unk_6E];
        obj->unk_6E++;
        obj->unk_6E &= 0xff;
        return val;
    }
    return 0;
}

s32 func_0201122c(TextObject* obj, s32 arg1) {
    s32 temp_r6 = func_0200fa10(arg1, obj->unk_58, obj->unk_5C, obj->unk_4E);
    s32 temp_r0 = (s32)func_0200f9e0(arg1, obj->unk_58, (s32) * (s32*)((u8*)obj + 0x60), (s32) * (s32*)((u8*)obj + 0x64),
                                     *(u16*)((u8*)obj + 0x68));
    func_02010fd8(obj, obj->xOffset, obj->yOffset, temp_r6, temp_r0, (s32) * (u16*)((u8*)obj + 0x56));
    if (*(s32*)((u8*)obj + 0x04) == 0) {
        obj->xOffset = obj->xOffset + temp_r0;
    } else {
        obj->yOffset = obj->yOffset + *(u16*)((u8*)obj + 0x56);
    }
    u16 temp_r1 = obj->unk_10[0];
    if (temp_r1 != 0 && obj->unk_78 == 0) {
        if (*(s32*)((u8*)obj + 0x04) == 0) {
            func_02010fd8(obj, obj->xOffset, obj->yOffset, obj->unk_30, (s32)temp_r1, (s32) * (u16*)((u8*)obj + 0x56));
            obj->xOffset = obj->xOffset + obj->unk_10[0];
        } else {
            func_02010fd8(obj, obj->xOffset, obj->yOffset, obj->unk_30, (s32) * (u16*)((u8*)obj + 0x54), (s32)obj->unk_10[1]);
            obj->yOffset = obj->yOffset + obj->unk_10[1];
        }
    }
    return 1;
}

void func_02011474(TextObject* arg0, u32 arg1, s32 arg2) {
    if ((arg1 & 0xff00) != 0) {
        func_0201122c(arg0, (s32)arg1);
    } else {
        func_02011358(arg0, (s32)arg1, 1);
    }
}

s32 func_02011358(TextObject* obj, s32 arg1, s32 arg2) {
    s32 var_r3 = 0;
    switch (arg1) {
        case 0:
            break;
        case 10:
            if (*(s32*)((u8*)obj + 0x04) == 0) {
                *(s32*)((u8*)obj + 0x08) = 0;
                obj->yOffset             = obj->yOffset + (*(u16*)((u8*)obj + 0x56) + obj->unk_10[1]);
            } else {
                s32 temp_r1  = *(u16*)((u8*)obj + 0x54) + obj->unk_10[0];
                obj->xOffset = obj->xOffset - temp_r1;
                obj->yOffset = 0;
            }
            break;
        case 9:
            if (*(s32*)((u8*)obj + 0x04) == 0) {
                if (arg2 == 1) {
                    s32 temp_r1 = obj->xOffset;
                    func_02010fd8(obj, temp_r1, obj->yOffset, obj->unk_30, 8 - (temp_r1 & 7), (s32) * (u16*)((u8*)obj + 0x56));
                }
                s32 temp_y   = obj->yOffset;
                obj->xOffset = (obj->xOffset + 8) & ~7;
                obj->yOffset = temp_y;
            } else {
                if (arg2 == 1) {
                    s32 temp_r2 = obj->yOffset;
                    func_02010fd8(obj, obj->xOffset, temp_r2, obj->unk_30, (s32) * (u16*)((u8*)obj + 0x54), 8 - (temp_r2 & 7));
                }
                s32 temp_x   = *(s32*)((u8*)obj + 0x08);
                s32 temp_r1  = obj->yOffset + 8;
                obj->xOffset = temp_x;
                obj->yOffset = temp_r1 & ~7;
            }
            var_r3 = 1;
            break;
    }
    return var_r3;
}

void func_02011494(TextObject* obj) {
    if (obj->unk_18[0] == 0 && obj->unk_18[1] == 0) {
        return;
    }
    MI_CpuFill(*(s32*)((u8*)obj + 0x34), func_02011b7c(obj, 0, 0), obj->unk_24);
}

void func_020114d4(void* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    /* arg2 = width (sp6C), arg3 = height (sp6E) */
    s32  sp6C = arg2;
    s32  sp6E = arg3;
    s32* sp0  = (s32*)arg0;
    s32  sp4  = ((sp6C * sp6E * 2) + 0x1F) & ~0x1F;
    s32* sp18 = *(s32**)((u8*)&g_OamMgr + 8 + (arg1 * 0x108C));
    MI_CpuSet((void*)arg4, 0xFF, (u32)sp4);
    s32 var_r8 = 0;
    s32 sp1C   = 0;
    s32 var_r6 = 0;
    if (sp6E > 0) {
        s32 sp14 = 0;
        s32 sp2C = arg6;
        do {
            s32 sp8 = 0;
            if (sp6C > 0) {
                s32 sp10 = sp6E - var_r6;
                s32 sp30 = arg4 + (sp14 * 2);
                do {
                    if (*(s16*)((u8*)sp30 + (sp8 * 2)) == (s16)0xFFFF) {
                        s32 temp_r0_2 = sp6C - sp8;
                        s32 sp28;
                        if (temp_r0_2 >= 8)
                            sp28 = 3;
                        else if (temp_r0_2 >= 4)
                            sp28 = 2;
                        else if (temp_r0_2 >= 2)
                            sp28 = 1;
                        else if (temp_r0_2 >= 1)
                            sp28 = 0;
                        s32 sp24;
                        if (sp10 >= 8)
                            sp24 = 3;
                        else if (sp10 >= 4)
                            sp24 = 2;
                        else if (sp10 >= 2)
                            sp24 = 1;
                        else if (sp10 >= 1)
                            sp24 = 0;
                        /* var_r9 = (sp28*0x30) + (sp24*0xC0) + &data_0205b454 */
                        s32 var_r9 = (sp28 * 0x30) + (sp24 * 0xC0) + (s32)data_0205b454;
                        s32 sp20   = 0;
                        do {
                            s32 var_r1 = 0;
                            s32 var_r7 = 0;
                            /* var_r9->unkC */
                            s32 unkC = *(s32*)(var_r9 + 12);
                            if (unkC > 0) {
                                s16 var_r2 = (s16)(var_r8 << 5);
                                do {
                                    var_r7   = 0;
                                    s32 unk8 = *(s32*)(var_r9 + 8);
                                    if (unk8 > 0) {
                                        s32 unk0 = *(s32*)(var_r9 + 0);
                                        s32 unk4 = *(s32*)(var_r9 + 4);
                                        do {
                                            /* write to arg4 buffer */
                                            s32 idx = ((sp6C * (unk4 + (var_r6 + var_r1))) + (unk0 + var_r7)) * 2;
                                            *(s16*)((u8*)arg4 + (sp8 * 2) + idx) = var_r2;
                                            var_r7 += 1;
                                            var_r2 = (s16)(var_r2 + 0x20);
                                        } while (var_r7 < unk8);
                                    }
                                    var_r1 += 1;
                                } while (var_r1 < unkC);
                            }
                            if (var_r1 > 0) {
                                *(s16*)((u8*)arg5 + (sp1C * 2)) = (s16)(var_r8 | 0x8000);
                                void* temp_r3                   = (u8*)arg5 + (sp1C * 2);
                                s32   unk4v                     = *(s32*)(var_r9 + 4);
                                s32   unk0v                     = *(s32*)(var_r9 + 0);
                                s32   unk10                     = *(s32*)(var_r9 + 16);
                                s32   unk14                     = *(s32*)(var_r9 + 20);
                                *(s16*)((u8*)temp_r3 + 2)       = (s16)(((var_r6 + unk4v) * 8) | (unk10 << 14));
                                *(s16*)((u8*)temp_r3 + 4)       = (s16)(((sp8 + unk0v) * 8) | (unk14 << 14));
                                *(s16*)((u8*)temp_r3 + 6)       = (s16)(var_r8 | (sp2C << 12));
                                s32 temp_r11                    = *sp18 >> 5;
                                s32 var_r1v                     = var_r1;
                                s32 var_r7v                     = var_r7;
                                var_r8 += temp_r11 * (((var_r1v * var_r7v) + (temp_r11 - 1)) / temp_r11);
                                sp1C += 4;
                            }
                            var_r9 += 0x18;
                            sp20 = sp20 + 1;
                        } while (sp20 < 2);
                    }
                    sp8 = sp8 + 1;
                } while (sp8 < sp6C);
            }
            var_r6 += 1;
            sp14 += sp6C;
        } while (var_r6 < sp6E);
    }
    s32 sp38                        = var_r8;
    s32 temp_r1                     = (((((sp1C >> 2) + 1) * 8) + 0x1F) & ~0x1F) + sp4;
    s32 sp34                        = temp_r1;
    *(s16*)((u8*)arg5 + (sp1C * 2)) = (s16)0xFFFF;
    sp0[0]                          = temp_r1;
    sp0[1]                          = var_r8;
}

void func_02011810(TextObject* obj) {
    s32 spC[2];
    func_020114d4(spC, obj->unk_00, (s32)(obj->unk_14[0] | (obj->unk_14[1] << 16)),
                  (s32)(obj->unk_18[0] | (obj->unk_18[1] << 16)), obj->unk_38, obj->unk_3C, obj->unk_2C);
    obj->unk_24 = (s32)(spC[1] << 5);
}

void func_0201187c(TextObject* obj, void* arg1) {
    *(s32*)((u8*)obj + 0x04)   = *(s32*)((u8*)arg1 + 8);
    obj->unk_00                = *(s32*)arg1;
    *(s32*)((u8*)obj + 0x44)   = *(s32*)((u8*)arg1 + 4);
    *(Data**)((u8*)obj + 0x48) = NULL;
    u16 temp_wA0               = *(u16*)((u8*)arg1 + 20);
    u16 temp_wA1               = *(u16*)((u8*)arg1 + 22);
    obj->unk_10[0]             = temp_wA0;
    obj->unk_10[1]             = temp_wA1;
    u16 temp_wB0               = *(u16*)((u8*)arg1 + 12);
    u16 temp_wB1               = *(u16*)((u8*)arg1 + 14);
    obj->unk_14[0]             = temp_wB0;
    obj->unk_14[1]             = temp_wB1;
    u16 temp_wC0               = *(u16*)((u8*)arg1 + 16);
    u16 temp_wC1               = *(u16*)((u8*)arg1 + 18);
    obj->unk_18[0]             = temp_wC0;
    obj->unk_18[1]             = temp_wC1;
    obj->unk_1C                = (u16)(*(u16*)((u8*)arg1 + 12) * 8);
    obj->unk_1E                = (u16)(*(u16*)((u8*)arg1 + 14) * 8);
    obj->unk_20                = (u16)(*(u16*)((u8*)arg1 + 16) * 8);
    obj->unk_22                = (u16)(*(u16*)((u8*)arg1 + 18) * 8);
    obj->unk_28                = *(s32*)((u8*)arg1 + 24);
    obj->unk_2C                = *(s32*)((u8*)arg1 + 28);
    obj->unk_38                = *(s32*)((u8*)arg1 + 32);
    obj->unk_3C                = *(s32*)((u8*)arg1 + 32) + (((obj->unk_18[0] * obj->unk_18[1] * 2) + 0x1F) & ~0x1F);
    func_020119b0(obj, *(s32*)((u8*)arg1 + 36), *(s32*)((u8*)arg1 + 40));
    *(s32*)((u8*)obj + 0x40) = *(s32*)((u8*)arg1 + 36);
    obj->unk_30              = func_0200fa10(0x8140, obj->unk_58, obj->unk_5C, obj->unk_4E);
    func_02011e58(obj, (u8*)arg1 + 0x2C);
    func_02011bec(obj);
    func_02011810(obj);
}

void func_02011994(TextObject* obj) {
    DatMgr_ReleaseData(*(Data**)((u8*)obj + 0x48));
    *(Data**)((u8*)obj + 0x48) = NULL;
}

void func_020119b0(TextObject* obj, s32 arg1, s32 arg2) {
    *(s32*)((u8*)obj + 0x40) = arg1;
    Data* var_r0             = *(Data**)((u8*)obj + 0x48);
    void* temp_r4            = (void*)((u8*)(u32)arg1 + arg2 * 0x24);
    if (var_r0 != NULL) {
        DatMgr_ReleaseData(var_r0);
    }
    s32 var_r6 = 0;
    s32 temp_r7;
    if (*(s32*)((u8*)temp_r4 + 8) == 1) {
        var_r6 = 1;
    }
    s32 temp_r8 = *(s32*)temp_r4;
    s32 temp_r9 = *(s32*)((u8*)temp_r4 + 4);
    temp_r7     = *(s32*)((u8*)obj + 0x44);
    if (BinMgr_FindById(temp_r8) == NULL) {
        var_r0 = DatMgr_LoadPackEntry(temp_r7, NULL, 0, (BinIdentifier*)temp_r8, temp_r9, (BOOL)var_r6);
    } else {
        var_r0 = DatMgr_LoadPackEntryDirect(temp_r7, (BinIdentifier*)temp_r8, temp_r9, var_r6);
    }
    *(Data**)((u8*)obj + 0x48) = var_r0;
    obj->unk_4C                = (u16) * (u16*)((u8*)temp_r4 + 12);
    obj->unk_4E                = (u16) * (u16*)((u8*)temp_r4 + 14);
    u16 temp_w1                = *(u16*)((u8*)temp_r4 + 18);
    u16 temp_w2                = *(u16*)((u8*)temp_r4 + 16);
    *(u16*)((u8*)obj + 0x54)   = temp_w2;
    *(u16*)((u8*)obj + 0x56)   = temp_w1;
    s32   temp_r3              = *(s32*)((u8*)temp_r4 + 20);
    void* var_r1               = NULL;
    if (var_r0 == NULL || temp_r3 <= 0) {
        var_r1 = NULL;
    } else {
        void* temp_r2 = (u8*)var_r0->buffer + 0x20;
        var_r1        = (u8*)temp_r2 + *(s32*)((u8*)temp_r2 + temp_r3 * 8);
    }
    obj->unk_58     = (u8*)var_r1;
    s32   temp_r3_2 = *(s32*)((u8*)temp_r4 + 24);
    void* var_r1_2  = NULL;
    if (var_r0 == NULL || temp_r3_2 <= 0) {
        var_r1_2 = NULL;
    } else {
        void* temp_r2_2 = (u8*)var_r0->buffer + 0x20;
        var_r1_2        = (u8*)temp_r2_2 + *(s32*)((u8*)temp_r2_2 + temp_r3_2 * 8);
    }
    obj->unk_5C     = (u32)var_r1_2;
    s32   temp_r3_3 = *(s32*)((u8*)temp_r4 + 28);
    void* var_r1_3  = NULL;
    if (var_r0 == NULL || temp_r3_3 <= 0) {
        var_r1_3 = NULL;
    } else {
        void* temp_r2_3 = (u8*)var_r0->buffer + 0x20;
        var_r1_3        = (u8*)temp_r2_3 + *(s32*)((u8*)temp_r2_3 + temp_r3_3 * 8);
    }
    *(u32*)((u8*)obj + 0x60) = (u32)var_r1_3;
    *(s32*)((u8*)obj + 0x64) = *(s32*)((u8*)var_r0->buffer + (*(s32*)((u8*)temp_r4 + 28) * 8) + 0x24);
    *(u16*)((u8*)obj + 0x68) = *(u16*)((u8*)temp_r4 + 32);
    *(s16*)((u8*)obj + 0x50) = (((*(u16*)((u8*)temp_r4 + 16)) + 7) & ~7) >> 3;
    *(s16*)((u8*)obj + 0x52) = (((*(u16*)((u8*)temp_r4 + 18)) + 7) & ~7) >> 3;
    *(u16*)((u8*)obj + 0x74) = 0;
    *(u16*)((u8*)obj + 0x76) = obj->unk_4C;
    obj->unk_7C              = ((((*(u16*)((u8*)obj + 0x54)) * obj->unk_4C) + 7) & ~7) >> 3;
    obj->unk_94              = (void*)((u8*)obj + 0x98);
    obj->unk_78              = 0;
    s32 temp_y               = *(s32*)((u8*)obj + 0x0C);
    obj->xOffset             = *(s32*)((u8*)obj + 0x08);
    obj->yOffset             = temp_y;
}

void* func_02011b7c(TextObject* obj, s32 arg1, s32 arg2) {
    void* var_lr = NULL;
    s32   var_ip = var_lr - 1;
    if (arg1 >= 0 && arg1 < obj->unk_20 && arg2 >= 0 && arg2 < obj->unk_22) {
        var_ip = *(u16*)((u8*)(u32)obj->unk_38 + ((((arg2 >> 3) * obj->unk_18[0]) + (arg1 >> 3)) * 2));
    }
    if (var_ip != 0xFFFF) {
        var_lr = (void*)(obj->unk_28 + (var_ip & ~0x1F));
    }
    return var_lr;
}

void func_02011bec(TextObject* obj) {
    obj->unk_70 = (u16*)((u8*)obj + 0x118);
    obj->unk_6C = 0;
    obj->unk_6E = 0;
}

BOOL func_02011c04(TextObject* obj) {
    if (obj->unk_6C == obj->unk_6E) {
        return TRUE;
    }
    return FALSE;
}

void func_02011c1c(TextObject* obj, u16 arg1) {
    obj->unk_70[obj->unk_6C] = arg1;
    obj->unk_6C++;
    obj->unk_6C = (u16)(u8)obj->unk_6C;
}

s32 func_02011c48(TextObject* obj) {
    s32 val;
    s32 temp_r5;
    u16 var_r2;
    u32 temp_r0;

    val = 0;
    do {
        val = func_020111f0(obj);
        if (val != 0x40) {
            break;
        }
        temp_r5 = func_020111f0(obj);
        temp_r0 = (u32)func_020111f0(obj);
        if ((temp_r0 >= 0x30U) && (temp_r0 <= 0x39U)) {
            var_r2 = (u16)(temp_r0 - 0x30);
        } else if ((temp_r0 >= 0x61U) && (temp_r0 <= 0x66U)) {
            var_r2 = (u16)(temp_r0 - 0x57);
        } else if ((temp_r0 >= 0x41U) && (temp_r0 <= 0x46U)) {
            var_r2 = (u16)(temp_r0 - 0x37);
        } else {
            var_r2 = 0x10;
        }
        if (temp_r5 > 0x46) {
            switch (temp_r5 - 0x61) {
                case 1:
                    obj->unk_98 = (s32)var_r2;
                    break;
                case 5:
                    obj->unk_9C = (s32)var_r2;
                    break;
                case 0:
                    *(s32*)((u8*)obj + 0xA0) = (s32)var_r2;
                    break;
                case 4:
                    *(s32*)((u8*)obj + 0xA4) = (s32)var_r2;
                    break;
            }
        } else if (temp_r5 == 0x46) {
            func_020119b0(obj, (s32)obj->data, var_r2);
        }
    } while (1);
    return val;
}

void func_02011d48(void) {}

u16 func_02011d4c(TextObject* obj) {
    u16 temp_r4 = obj->unk_6C;
    u16 temp_r5 = obj->unk_6E;
    u16 val;
loop_1:
    val = func_020111f0(obj);
    if (val == 0x40) {
        func_020111f0(obj);
        func_020111f0(obj);
        goto loop_1;
    }
    obj->unk_6C = temp_r4;
    obj->unk_6E = temp_r5;
    return val;
}

void* func_02011d8c(TextObject* obj, u8* arg1) {
    u8* var_r4 = arg1;
    u8  var_r2 = *var_r4;
    if (var_r2 != 0) {
        do {
            if (var_r2 & 0x80) {
                func_02011c1c(obj, (u16)(var_r4[1] | (var_r2 << 8)));
                var_r4 += 2;
            } else {
                func_02011c1c(obj, func_0200f94c(var_r2));
                var_r4 += 1;
            }
            var_r2 = *var_r4;
        } while (var_r2 != 0);
    }
    return var_r4 + 1;
}

void func_02011df8(TextObject* obj) {
    if (func_02011c04(obj)) {
        return;
    }
    do {
        s32 temp_r6 = func_02011c48(obj);
        u16 temp_r0 = func_02011d4c(obj);
        s32 var_r2  = 0;
        if (!(temp_r0 != 0 && temp_r0 != 0xA)) {
            var_r2 = 1;
        }
        func_02011474(obj, (u32)temp_r6, var_r2);
    } while (!func_02011c04(obj));
}

void func_02011e58(TextObject* obj, u16* arg1) {
    s32 var_ip = 0;
    do {
        u16 temp_r3                = arg1[var_ip];
        ((u32*)obj)[0x26 + var_ip] = temp_r3;
        ((u32*)obj)[0x36 + var_ip] = temp_r3;
        var_ip += 1;
    } while (var_ip < 0x10);
    u32 temp_r1 = *(u32*)((u8*)obj + 0x98);
    temp_r1 |= temp_r1 * 0x10;
    temp_r1 |= temp_r1 << 8;
    *(u32*)((u8*)obj + 0x34) = temp_r1 | (temp_r1 << 0x10);
}

void func_02011e94(TextObject* obj, s32 arg1, s32 arg2, u8* arg3) {
    func_02011df8(obj);
    obj->xOffset = arg1;
    obj->yOffset = arg2;
    func_02011d8c(obj, arg3);
    func_02011df8(obj);
}

void func_02011ecc(TextObject* obj) {
    s32 var_r0;
    func_02011494(obj);
    if (*(s32*)((u8*)obj + 0x04) == 0) {
        var_r0 = 0;
    } else {
        var_r0 = obj->unk_20 - (u16) * (u16*)((u8*)obj + 0x54);
    }
    obj->xOffset = var_r0;
    obj->yOffset = 0;
}
