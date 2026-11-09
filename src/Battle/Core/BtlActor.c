#include "Battle/Core/BtlActor.h"
#include "Combat/Core/BtlMgr.h"
#include "Combat/Core/BtlPop.h"
#include "Combat/Core/Combat.h"
#include "Combat/Core/CombatDebug.h"
#include "Combat/Core/CombatStage.h"
#include <nitro/fx.h>

static void BtlActor_ClearHitStun(BattleActor* actor) {
    actor->hit.stunTimer = 0;
    actor->hit.finisher  = FALSE;
}

static void BtlActor_ApplyPlanarDamping(BattleActor* actor) {
    if ((actor->flags & ACTOR_FLAG_LAUNCHED) == 0) {
        if (actor->xVelocity > 0) {
            actor->xVelocity -= actor->linearDampingStep;
            if (actor->xVelocity < 0) {
                actor->xVelocity = 0;
            }
        } else if (actor->xVelocity < 0) {
            actor->xVelocity += actor->linearDampingStep;
            if (actor->xVelocity > 0) {
                actor->xVelocity = 0;
            }
        }

        if (actor->yVelocity > 0) {
            actor->yVelocity -= actor->linearDampingStep;
            if (actor->yVelocity < 0) {
                actor->yVelocity = 0;
            }
        } else if (actor->yVelocity < 0) {
            actor->yVelocity += actor->linearDampingStep;
            if (actor->yVelocity > 0) {
                actor->yVelocity = 0;
            }
        }
        if (actor->position.z >= 0) {
            actor->xVelocity = FX_Mul(actor->xVelocity, actor->dampingScale);
            actor->yVelocity = FX_Mul(actor->yVelocity, actor->dampingScale);
        }
    }
}

static void BtlActor_ApplyPeriodicEffectTick(s32 engine, BattleActor* actor) {
    if (actor->periodicEffectTimer % 180 != 0) {
        return;
    }

    s32 val = actor->periodicEffectAmount;

    if (actor->kind == ACTOR_KIND_PLAYER) {
        BtlMgr_AddTeamHp(engine, -val);
        func_ov003_0209b564(engine, val, actor, 3);
        if (actor->currentHp <= 0) {
            actor->flags |= ACTOR_FLAG_KILLED;
            BtlActor_ForceDefeat(g_CombatGlobal->player);
            BtlActor_ForceDefeat(g_CombatGlobal->partner);
            g_CombatGlobal->unk3D878 |= 0x30000;
        }
    } else {
        BtlMgr_AddUnitHp(engine, actor, -val);
        func_ov003_0209b61c(engine, val, actor);

        if (actor->currentHp <= 0) {
            actor->flags |= ACTOR_FLAG_KILLED;
            BtlActor_ForceDefeat(actor);
            BtlActor_ForceDefeat(actor->linkedActor);
        }
    }
}

void BtlActor_SetPendingCommand(BattleActor* actor, BattleActorCommand command) {
    if ((actor->flags & ACTOR_FLAG_DEFEATED) == 0) {
        actor->pendingCommand = command;
    }
}

BattleActorCommand BtlActor_PopPendingCommand(BattleActor* actor) {
    BattleActorCommand prev = actor->pendingCommand;

    actor->pendingCommand = ACTOR_CMD_NONE;
    if (actor->invincibleTimer > 0) {
        actor->invincibleTimer--;
    }
    return prev;
}

void BtlActor_ForceDefeat(BattleActor* actor) {
    if (actor != NULL) {
        actor->currentHp = 0;
        BtlActor_SetPendingCommand(actor, ACTOR_CMD_DEFEAT);
        actor->flags |= ACTOR_FLAG_DEFEATED;
    }
}

void BtlActor_SetJumpDuration(BattleActor* actor, s32 frames) {
    s32 var_r0 = 0;
    s32 var_r2 = 0;
    if (frames > 0) {
        s32 var_r3 = 0;
        do {
            var_r2++;
            var_r0 += var_r3;
            var_r3 += actor->zGravity;
        } while (var_r2 < frames);
    }
    actor->zVelocity = -(var_r0 / frames);
}

void BtlActor_ClearPeriodicEffect(BattleActor* actor) {
    actor->periodicEffectMode       = 0;
    actor->periodicEffectTimer      = 0;
    actor->periodicEffectAmount     = 0;
    actor->effectDamageTakenPercent = 100;
    actor->baseArgs.effectPercent   = 100;
}

void BtlActor_ResetAttack(BattleActor* actor) {
    actor->impactAttackId           = 10;
    actor->impactArgs.power         = 0;
    actor->impactArgs.powerPercent  = 100;
    actor->impactArgs.effectPercent = 100;
}

void BtlActor_UpdateEffects(s32 engine, BattleActor* actor) {
    if ((g_CombatGlobal->unk3D878 & 6) == 0) {
        if (actor->periodicEffectMode == 1) {
            BtlActor_ApplyPeriodicEffectTick(engine, actor);
        }

        actor->periodicEffectTimer--;
        if (actor->periodicEffectTimer <= 0) {
            BtlActor_ClearPeriodicEffect(actor);
        }

        actor->hit.stunTimer--;
        if (actor->hit.stunTimer <= 0) {
            actor->hit.stunTimer = 0;
        }
    }
}

void BtlActor_UpdatePhysics(BattleActor* actor) {
    actor->position.z += actor->zVelocity;

    if (actor->position.z < 0) {
        actor->zVelocity += actor->zGravity;
        if (actor->zVelocity > 0x5000) {
            actor->zVelocity = 0x5000;
        }
    } else if (actor->position.z > 0) {
        actor->position.z = 0;

        s32 var_r1 = -FX_Mul(actor->zVelocity, actor->bounceScale);

        actor->zVelocity = var_r1;
        if (var_r1 < 0) {
            var_r1 = -var_r1;
        }
        if (var_r1 < 0x1000) {
            actor->zVelocity = 0;
        }
    }
    actor->position.x += actor->xVelocity;
    actor->position.y += actor->yVelocity;
    BtlActor_ApplyPlanarDamping(actor);
}

void BtlActor_Init(BattleActor* actor, BattleActorKind kind) {
    actor->kind                     = kind;
    actor->position.x               = 0;
    actor->position.y               = 0;
    actor->position.z               = 0;
    actor->zVelocity                = 0;
    actor->zGravity                 = 0x400;
    actor->bounceScale              = 0;
    actor->xVelocity                = 0;
    actor->yVelocity                = 0;
    actor->linearDampingStep        = 410;
    actor->dampingScale             = 0x1000;
    actor->pendingCommand           = ACTOR_CMD_NONE;
    actor->isFlipped                = FALSE;
    actor->flags                    = 0;
    actor->holderId                 = 0;
    actor->invincibleTimer          = 0;
    actor->radius                   = 16;
    actor->height                   = 40;
    actor->centerOffsetX            = 0;
    actor->centerHeight             = 0;
    actor->attack                   = 0;
    actor->defense                  = 0;
    actor->impactPercent            = 100;
    actor->baseArgs.power           = 0;
    actor->baseArgs.comboFrames     = 50;
    actor->baseArgs.powerPercent    = 100;
    actor->baseArgs.effectPercent   = 100;
    actor->damageTakenPercent       = 100;
    actor->effectDamageTakenPercent = 100;
    BtlActor_ClearPeriodicEffect(actor);
    BtlActor_ResetAttack(actor);
    BtlActor_ClearHitStun(actor);
    actor->maxHp       = 100;
    actor->currentHp   = 100;
    actor->linkedActor = 0;
}

void BtlActor_Render(BattleActor* actor, BattleSprite* sprite) {
    if ((CombatDebug_GetSetting(0) == 1) && (actor->kind != ACTOR_KIND_PLAYER)) {
        actor->flags |= ACTOR_FLAG_HIDDEN;
        return;
    }

    if (actor->flags & ACTOR_FLAG_HIT_FLASH) {
        actor->flags &= ~ACTOR_FLAG_HIT_FLASH;
        sprite->paletteTimer = 12;
        sprite->flags |= BTL_SPRITE_FLAG_PALETTE_FLASH;
        BtlSprite_SetPaletteMode(sprite, BTL_SPRITE_PALETTE_FLICKER);
    }

    s32 posX;
    s32 posY;
    if (sprite->sprite.bits_0_1 == 0) {
        posX = CombatStage_ToScreenX(0, actor->position.x);
        posY = CombatStage_ToScreenY(0, actor->position.y, actor->position.z);
    } else {
        posX = CombatStage_ToScreenX(1, actor->position.x);
        posY = CombatStage_ToScreenY(1, actor->position.y, actor->position.z);
    }
    if (actor->isFlipped == TRUE) {
        BtlSprite_SetFlip(sprite, TRUE);
    } else {
        BtlSprite_SetFlip(sprite, FALSE);
    }

    BtlSprite_SetPosition(sprite, posX, posY);
    BtlSprite_SetSortKey(sprite, 0x7FFFFFFE - actor->position.y);
    BtlSprite_Render(sprite);
    actor->screenX = posX;
    actor->screenY = posY;
}
