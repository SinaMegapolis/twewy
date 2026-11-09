#include "Combat/Core/CombatApi.h"
#include "Combat/Core/CombatPsychEffects.h"
#include <nitro/mi/cpumem.h>

static const TaskHandle             data_ov003_020dd1dc;
static const Ov003StreetLiveSpeaker data_ov003_020dd1e8[5];
static void                         func_ov003_020ba68c(Ov003StreetLive* self);

s32 func_ov003_020ba2e8(Ov003EffectContext* context, Ov003EffectState* effect) {
    if (func_ov003_020a5b18(0x36))
        return 0;
    if (!func_ov003_020a50cc())
        return 0;
    Ov003PsychArgs* arg = func_ov003_020a5d2c(sizeof(*arg), data_ov003_020dd1dc.taskName);
    arg->effect         = effect;
    context->handle     = &data_ov003_020dd1dc;
    context->arg        = arg;
    context->post       = NULL;
    return 1;
}

static s32 func_ov003_020ba350(TaskPool* pool, Task* task, void* args, s32 stage) {
    Ov003PsychArgs*  arg = args;
    Ov003StreetLive* self;
    switch (stage) {
        default:
        case 0: {
            self                = task->data;
            BattleActor* player = func_ov003_020a6438();
            MI_CpuSet(self, 0, sizeof(*self));
            self->effect = arg->effect;
            self->effect->unk_0A++;
            func_ov003_020a6464(1);
            func_ov003_020a64ac();
            CombatPsyAnim_Load(&self->sprite, 2, 1, self->effect->index + 0xDC, 0);
            self->speakerPhase = 0;
            for (u32 i = 0; i < 5; i++) {
                const Ov003StreetLiveSpeaker* speaker = &data_ov003_020dd1e8[i];
                func_ov003_020b9be0(self->effect, player->position.x + speaker->x * 0x1000,
                                    player->position.y + speaker->y * 0x1000, speaker->kind, &self->speakerPhase);
            }
            CombatCameraShake_Bind(&self->camera, 0, 0);
            CombatCameraShake_Start(&self->camera, 4, 0x1800);
            self->phase = 0;
            self->timer = 0;
            CombatSe_PlayAtMainX(0x5A, player->position.x - 0x1E000);
            func_ov003_02087804(0);
            return 1;
        }
        case 1:
            self = task->data;
            func_ov003_020a5a94(self->effect);
            switch (self->phase) {
                case 0:
                    if (++self->timer == 10) {
                        self->phase        = 1;
                        self->quietFrames  = 0;
                        self->timer        = 0;
                        self->speakerPhase = 1;
                        switch (self->effect->index) {
                            case 0:
                                CombatPsyAnim_SetAnim(&self->sprite, 0xE0, 0);
                                break;
                            case 1:
                                CombatPsyAnim_SetAnim(&self->sprite, 0xE1, 1);
                                break;
                            case 2:
                                CombatPsyAnim_SetAnim(&self->sprite, 0xE2, 0);
                                break;
                            case 3:
                                CombatPsyAnim_SetAnim(&self->sprite, 0xE3, 0);
                                break;
                        }
                        func_ov003_020ba68c(self);
                    }
                    break;
                case 1:
                    if (func_ov003_020a50cc()) {
                        self->quietFrames = 0;
                    } else {
                        if (++self->quietFrames >= 60) {
                            self->phase        = 2;
                            self->speakerPhase = 2;
                        }
                    }
                    CombatCameraShake_Update(&self->camera, 0, 0x1800, 0x1800);
                    s32 interval = self->effect->params[0];
                    if (++self->timer == interval) {
                        self->timer = 0;
                        func_ov003_020ba68c(self);
                    }
                    break;
                case 2:
                    return 0;
            }
            if (!func_ov003_020a5aa4(self->effect) || func_ov003_020a63a8(self->effect)) {
                self->phase        = 2;
                self->speakerPhase = 2;
            }
            if (SpriteMgr_IsAnimationFinished(&self->sprite.sprite))
                CombatPsyAnim_SetAnim(&self->sprite, 0xE4, 0);
            BtlSprite_Update(&self->sprite);
            return 1;
        case 2:
            self = task->data;
            BtlSprite_SetPosition(&self->sprite, 128, 96);
            BtlSprite_SetSortKey(&self->sprite, 0x385000);
            BtlSprite_Render(&self->sprite);
            return 1;
        case 3:
            self = task->data;
            CombatCameraShake_Reset(&self->camera);
            BtlSprite_Release(&self->sprite);
            func_ov003_020a6464(0);
            self->effect->unk_0A--;
            return 1;
    }
}

static void func_ov003_020ba68c(Ov003StreetLive* self) {
    BattleActor* player = func_ov003_020a6438();
    func_ov003_020ba08c(self->effect, player->position.x - 0x1E000, player->position.y - 0x52000);
    CombatSe_PlayAtMainX(0x5A, player->position.x - 0x1E000);
}

static char                         data_ov003_020e5048[24] = "Tsk_BtlPsy_StreetLive";
static const TaskHandle             data_ov003_020dd1dc = {data_ov003_020e5048, func_ov003_020ba350, sizeof(Ov003StreetLive)};
static const Ov003StreetLiveSpeaker data_ov003_020dd1e8[5] = {
    {-30, -58, 0},
    { 10, -55, 1},
    { 44, -42, 1},
    {-57, -36, 1},
    {-71, -12, 1}
};
