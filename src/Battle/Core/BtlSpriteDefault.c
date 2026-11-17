#include "Battle/Core/BtlSpriteDefault.h"
#include "Battle/Core/BtlSprite.h"

SpriteAnimation BtlSpriteDefault_Anim = {
    .bits_0_1          = 0,
    .dataType          = 1,
    .bit_6             = 0,
    .bits_7_9          = 5,
    .bits_10_11        = 0,
    .bits_12_13        = 1,
    .bits_14_15        = 0,
    .unk_02.raw        = 0x800,
    .posX              = 0,
    .posY              = 0,
    .frameInfoCallback = BtlSprite_GetFrameInfo,
    .callbackArg       = 0,
    .owner             = NULL,
    .binIden           = NULL,
    .unk_18            = 0,
    .packIndex         = 0,
    .unk_1C            = 0,
    .unk_1E            = 0,
    .unk_20            = 0,
    .unk_22            = 1,
    .unk_24            = 0,
    .unk_26            = 0,
    .unk_28            = 0,
    .animIndex         = 1,
};
