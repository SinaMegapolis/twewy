#ifndef TWEWY_COMBAT_CORE_BTLDBG_H
#define TWEWY_COMBAT_CORE_BTLDBG_H

/** Battle debug overlay for visualizing hitboxes and other combat-related rectangles. */

#include <nitro/types.h>

enum {
    BTLDBG_KIND_FOOTPRINT = 0, // White: Actor footprint in world space
    BTLDBG_KIND_ATTACK    = 1, // Red: Attack hit area in world space
    BTLDBG_KIND_SCREEN    = 2, // Pink: Attack hit area in screen space
    BTLDBG_KIND_HITBOX    = 3, // Blue: Actor hitbox in world space
};

/**
 * Creates a debugging task which overlays rectangles over various actions in the combat scene.
 */
void BtlDbg_CreateTask(void);

/**
 * Post a debug rectangle to be rendered.
 *
 * @param kind   The kind of debug rectangle
 * @param life   The lifetime of the rectangle in frames
 * @param x      The x-coordinate of the rectangle's top-left corner
 * @param y      The y-coordinate of the rectangle's top-left corner
 * @param width  The width of the rectangle
 * @param height The height of the rectangle
 */
void BtlDbg_PostRect(u16 kind, u16 life, s32 x, s32 y, s16 width, s16 height);

#endif // TWEWY_COMBAT_CORE_BTLDBG_H
