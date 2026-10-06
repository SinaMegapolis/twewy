#include "OtosuMenuShared.h"

#include "OtosuMenuShared.h"

static PrcStepFn data_ov002_02092cb0[] = {
    func_ov002_020875c8,
    OtosuPrcStep_FadeStart_Neutral,
    OtosuPrcStep_FadeWait_Neutral,
    func_ov002_02087728,
    OtosuPrcStep_FadeStart_BrightImmediate,
    OtosuPrcStep_FadeWait_Immediate,
    func_ov002_020878f8,
    PrcStep_PopFrame,
};

PrcFrameDesc data_ov002_02092c9c = {
    .enter     = func_ov002_02087524,
    .stepTable = data_ov002_02092cb0,
    .update    = func_ov002_020875c0,
    .render    = func_ov002_020875c4,
    .exit      = func_ov002_0208757c,
};

static const Ov002_U16_2 data_ov002_02091c3c[6] = {
    {0x0002, 0x0002},
    {0x009D, 0x0001},
    {0x003F, 0x004D},
    {0x0001, 0x003F},
    {0x0075, 0x0002},
    {0x00A4, 0x009D}
}; /* const */
static const Ov002_U16_25 data_ov002_02091c54 = {
    0,    8,    0xA8, 0x58, 0xB8, 1,    0x4C, 0x58,   0xB8,   0x68,   2,      0x4C,   0x80,
    0xB8, 0x90, 3,    0xA5, 0xA0, 0xF5, 0xB8, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
}; /* const */
static const Ov002_U16_30 data_ov002_02091c86 = {
    0x23F0, 0x0008, 0x0008, 0x00F8, 0x0040, 0x23EB, 0x0018, 0x00A6, 0x0058, 0x00B6, 0x23F1, 0x0058, 0x0056, 0x00B8, 0x0066,
    0x23F2, 0x0058, 0x007E, 0x00B8, 0x008E, 0x23F3, 0x00C5, 0x00A6, 0x00F5, 0x00B6, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,
}; /* const */

void func_ov002_02087524(PrcCtx* ctx, OtosuMenuObj* menuObj) {
    u16 i;

    for (i = 0; i < 4; i++) {
        data_02071d10.unk3414[i] = 0;
    }
    data_02072d10.unkD88 = 0;
    data_02072d10.unkD84 = 0;
    menuObj->unk_41FE9   = 0;
    func_ov002_02085a44(menuObj);
}

void func_ov002_0208757c(PrcCtx* ctx, void* arg1) {
    func_ov002_02085710(arg1);
    PrcMaster_UnregisterContext(arg1 + 0x41804, arg1 + 0x474E8);
    PrcMaster_UnregisterContext(arg1 + 0x41804, arg1 + 0x476D0);
}

void func_ov002_020875c0(void) {}

void func_ov002_020875c4(void) {}

PrcStepResult func_ov002_020875c8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u16           posX;
    Ov002_U16_30  layout = data_ov002_02091c86;

    SysFont_SetSpacing(&menuObj->font, TRUE, 0);
    func_ov002_02082f18(menuObj, 0x15, 0x14, layout.data);
    CriSndMgr_PlayFile(ADX_B11);
    menuObj->unk_474C8 = 0xFFFF;
    posX               = 0x60;
    PrcCtx_Init(&menuObj->unk_474E8, data_ov002_02092cd0, sizeof(OtosuMenuObj));
    PrcCtx_ReplaceFrame(&menuObj->unk_474E8, &data_ov002_02093020, &posX);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_474E8);
    posX = 0x88;
    PrcCtx_Init(&menuObj->unk_476D0, data_ov002_02092cd0, sizeof(OtosuMenuObj));
    PrcCtx_ReplaceFrame(&menuObj->unk_476D0, &data_ov002_02093020, &posX);
    PrcMaster_RegisterContext(&menuObj->prcMaster, &menuObj->unk_476D0);
    SystemStatusFlags;
    SystemStatusFlags.unk_06 = 1;
    SystemStatusFlags;
    SystemStatusFlags.unk_07 = 1;
    PrcCtx_AdvanceStep(ctx);
    return PRC_STEP_CONTINUE;
}

PrcStepResult func_ov002_02087728(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj    = (OtosuMenuObj*)object;
    Ov002_U16_25  table_sp18 = data_ov002_02091c54;
    u16           table_sp0[0xC];
    u16           temp_r0_3;
    u16           i;

    for (i = 0; i < 6; i++) {
        table_sp0[i * 2]     = data_ov002_02091c3c[i].unk0;
        table_sp0[i * 2 + 1] = data_ov002_02091c3c[i].unk2;
    }

    temp_r0_3 = func_ov002_0208597c(table_sp18.data);
    if (temp_r0_3 == 0xFFFF) {
        return 0;
    }
    if (temp_r0_3 == 0xFFFE) {
        menuObj->unk_474C8      = 0xFFFF;
        menuObj->unk_46078.posX = 0U;
        menuObj->unk_46078.posY = 0xD2U;
        return 0;
    }
    if (menuObj->unk_474C8 == temp_r0_3) {
        switch (temp_r0_3) { /* switch 1 */
            default:         /* switch 1 */
                /* Duplicate return node #26. Try simplifying control flow for better match */
                return 0;
            case 0: /* switch 1 */
                SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CANCEL);
                PrcCtx_AdvanceStep(ctx);
                return PRC_STEP_CONTINUE;
            case 1: /* switch 1 */
            case 2: /* switch 1 */
            case 3: /* switch 1 */
                menuObj->unk_460C0.posX = 0x80;
                menuObj->unk_460C0.posY = 0x60;
                SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_EXECUTE);
                PrcCtx_AdvanceStep(ctx);
                return PRC_STEP_CONTINUE;
        }
    } else {
        u16 anim_index = (u16)(temp_r0_3 * 3);

        menuObj->unk_474C8 = temp_r0_3;
        SndMgr_StartPlayingSE(SEIDX_SE_BAYBADGE_MENU_CURSOR);
        switch (temp_r0_3) { /* switch 2 */
            default:         /* switch 2 */
                return PRC_STEP_CONTINUE;
            case 0:          /* switch 2 */
            case 1:          /* switch 2 */
            case 2:          /* switch 2 */
            case 3:          /* switch 2 */
                menuObj->unk_46078.posX = table_sp0[anim_index + 1];
                menuObj->unk_46078.posY = table_sp0[anim_index + 2];
                Sprite_ChangeAnimation(&menuObj->unk_46078, menuObj->unk_46078.animData, (s16)table_sp0[anim_index],
                                       menuObj->unk_46078.cellTable);
                return PRC_STEP_CONTINUE;
        }
    }
}

PrcStepResult func_ov002_020878f8(PrcCtx* ctx, void* object) {
    OtosuMenuObj* menuObj = (OtosuMenuObj*)object;
    u32           temp_ip;
    u32           temp_ip_2;

    switch (menuObj->unk_474C8) {
        case 0:
            menuObj->unk_46074 = 5;
            return PRC_STEP_CONTINUE;
        case 1:
            temp_ip = (*(u32*)&SystemStatusFlags) << 0x19;
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 0;
            menuObj->unk_46070       = (s16)(temp_ip >> 0x1F);
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 0;
            PrcCtx_ReplaceFrame(ctx, &data_ov002_02092ff0, NULL);
            break;
        case 2:
            temp_ip_2 = (*(u32*)&SystemStatusFlags) << 0x19;
            SystemStatusFlags;
            SystemStatusFlags.unk_06 = 0;
            menuObj->unk_46070       = (s16)(temp_ip_2 >> 0x1F);
            SystemStatusFlags;
            SystemStatusFlags.unk_07 = 0;
            PrcCtx_ReplaceFrame(ctx, &data_ov002_02092f44, NULL);
            break;
        case 3:
            data_02074d10.unk_41C = 0x1F;
            data_02074d10.unk_40B = 3;
            menuObj->unk_46074    = 1;
            break;
    }
    return PRC_STEP_CONTINUE;
}
