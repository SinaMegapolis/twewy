#include "Combat/Core/CombatCameraShake.h"
#include "Combat/Core/Combat.h"
#include "Engine/Math/Random.h"

/// Binds camera `index` to the animation driven by `obj`.
void CombatCameraShake_Bind(Ov003CameraAnim* obj, s32 index, s32 arg2) {
    obj->camera         = &gBattle->stages[index];
    obj->camera->shakeX = 0;
    obj->camera->shakeY = 0;
    obj->camera->shakeZ = 0;
    obj->timer          = -1;
    obj->index          = arg2;
}

void CombatCameraShake_Start(Ov003CameraAnim* obj, s32 period, s32 offset) {
    obj->period = period;
    obj->timer  = period;
    obj->dir    = 0;
    obj->offset = offset;
}

/// Steps the camera shake animation; returns 1 when it hit a limit this frame.
s32 CombatCameraShake_Update(Ov003CameraAnim* obj, s32 delta, s32 min, s32 max) {
    s32 hit = 0;
    s32 value;

    if (obj->timer == -1) {
        return 0;
    }
    if (--obj->timer == 0) {
        obj->timer = obj->period;
        obj->dir   = obj->dir == 0 ? 1 : 0;
        if (obj->index == 0) {
            obj->camera->shakeX = (RNG_Next(3) - 1) << 12;
        } else {
            obj->camera->shakeY = (RNG_Next(3) - 1) << 12;
        }
    }
    obj->offset += delta;
    if (obj->offset > max) {
        obj->offset = max;
        hit         = 1;
    } else if (obj->offset < min) {
        obj->offset = min;
        hit         = 1;
    }
    if (obj->dir != 0) {
        value = -obj->offset;
    } else {
        value = obj->offset;
    }
    if (obj->index == 0) {
        obj->camera->shakeY = value;
    } else {
        obj->camera->shakeX = value;
    }
    return hit;
}

void CombatCameraShake_Reset(Ov003CameraAnim* obj) {
    obj->camera->shakeX = 0;
    obj->camera->shakeY = 0;
    obj->camera->shakeZ = 0;
}
