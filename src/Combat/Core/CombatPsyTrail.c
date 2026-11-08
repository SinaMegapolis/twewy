#include "Combat/Core/CombatPsyTrail.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatApi.h"
#include "SndMgr.h"
#include <nitro/fx.h>

/// Sets up the two sprite slots and the trail of the psych effect object.
void CombatPsyTrail_Init(Ov003TrailEffect* effect) {
    effect->state = 0;
    effect->count = 0;
    effect->scale = 0x1F000;
    CombatPsyAnim_Load(&effect->spriteA, 2, 1, 0xB5, 1);
    func_ov003_02082730(&effect->spriteA, 0x3E7);
    CombatPsyAnim_Load(&effect->spriteB, 2, 1, 0xB6, 0);
    func_ov003_02082730(&effect->spriteB, 0x3E8);
}

/// Updates the psych effect trail; returns 0 when it is finished.
// Nonmatching: trail update: duplicated effect-position loads are scheduled differently.
s32 CombatPsyTrail_Update(Ov003TrailEffect* effect) {
    s32 count;

    CombatSprite_Update(&effect->spriteA);
    CombatSprite_Update(&effect->spriteB);
    switch (effect->state) {
        case 0:
            effect->points[effect->count][0] = g_CombatGlobal->touchWork.x;
            effect->points[effect->count][1] = g_CombatGlobal->touchWork.y;
            effect->count++;
            if (CombatTouch_IsGesture(6) == 0) {
                effect->state = 3;
                return 1;
            }
            count = effect->count;
            if (count > 2) {
                if (func_ov003_020a632c(effect->points[count - 1][0], effect->points[count - 1][1],
                                        g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y) > 0x6000)
                {
                    if (effect->count < 0x78) {
                        s16 x = effect->points[count - 1][0];
                        s16 y = effect->points[count - 1][1];

                        effect->points[count - 1][0]     = x + (g_CombatGlobal->touchWork.x - x) / 2;
                        effect->points[count - 1][1]     = y + (g_CombatGlobal->touchWork.y - y) / 2;
                        effect->points[effect->count][0] = g_CombatGlobal->touchWork.x;
                        effect->points[effect->count][1] = g_CombatGlobal->touchWork.y;
                        effect->count++;
                    }
                }
                if (CombatPsyTrail_IsValidStroke(effect) != 0) {
                    CombatSe_PlayPanned(SEIDX_SE_PSY_FIREBALL01_02, effect->pivotX);
                    effect->state = 1;
                    return 1;
                }
                if (effect->count >= 0x78) {
                    effect->state = 3;
                    return 1;
                }
            }
            if (SndMgr_IsSEPlaying(SEIDX_SE_PSY_FIREBALL01_01) == 0) {
                CombatSe_PlayPanned(SEIDX_SE_PSY_FIREBALL01_01, g_CombatGlobal->touchWork.x);
            }
            return 1;
        case 1:
            effect->scale -= 0x1800;
            if (effect->scale < 0x1000) {
                effect->state = 2;
            }
            return 1;
        case 2:
            return 0;
        case 3:
            return 2;
    }
    return 1;
}

/// Renders the psych trail.
// Nonmatching: trail render loop register allocation.
void CombatPsyTrail_Render(Ov003TrailEffect* effect) {
    s32 i;

    for (i = 0; i < effect->count; i++) {
        CombatSprite_SetPosition(&effect->spriteA, effect->points[i][0], effect->points[i][1]);
        func_ov003_02082730(&effect->spriteA, 0x3B6000);
        if (effect->scale >= 0x1000) {
            if (effect->scale < 0x1F000) {
                CombatSprite_RenderWithPalette(&effect->spriteA, effect->scale >> 12);
            } else {
                CombatSprite_Render(&effect->spriteA);
            }
        }
    }
    if (effect->state != 0) {
        return;
    }
    CombatSprite_SetPosition(&effect->spriteB, g_CombatGlobal->touchWork.x, g_CombatGlobal->touchWork.y);
    func_ov003_02082730(&effect->spriteB, 0x3B7000);
    CombatSprite_Render(&effect->spriteB);
}

// Nonmatching: tail-call register allocation.
void CombatPsyTrail_Release(Ov003TrailEffect* effect) {
    CombatSprite_Release(&effect->spriteA);
    CombatSprite_Release(&effect->spriteB);
}

/// Checks that the trail is long and straight enough to fire the psych.
s32 CombatPsyTrail_IsValidStroke(Ov003TrailEffect* effect) {
    s16(*points)[2] = effect->points;
    s32 last        = effect->count - 1;
    s32 mid         = last / 2;
    s32 midX;
    s32 midY;
    s32 min = 0x3E8000;
    s32 max = 0;
    s32 dist;
    s32 i;

    if (func_ov003_020a632c(points[0][0], points[0][1], points[last][0], points[last][1]) > 0x10000) {
        return 0;
    }
    dist = func_ov003_020a632c(points[0][0], points[0][1], points[mid][0], points[mid][1]);
    if (dist < 0x30000 || dist > 0x100000) {
        return 0;
    }
    midX = points[0][0] + (points[mid][0] - points[0][0]) / 2;
    midY = points[0][1] + (points[mid][1] - points[0][1]) / 2;
    for (i = 0; i < last; i++) {
        dist = func_ov003_020a632c(midX, midY, points[i][0], points[i][1]);
        if (min > dist) {
            min = dist;
        }
        if (max < dist) {
            max = dist;
        }
    }
    if (max - min > 0x30000) {
        return 0;
    }
    effect->pivotX = midX;
    effect->pivotY = midY;
    return 1;
}
