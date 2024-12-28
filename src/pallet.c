#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "stdlib.h"

#include "pallet.h"

extern Rectangle pallet_1;
extern Rectangle pallet_2;
extern const float pallet_vertical_speed;
extern int ai_error_offset;

//----------------------------------------------------------------------------------
// Move a pallet without overflowing.
//----------------------------------------------------------------------------------
void
ai_move_pallet(Rectangle* pallet, int desired_y)
{
    int middle_of_pallet = pallet->y + pallet->height / 2;
    int difference = abs(middle_of_pallet - desired_y);
    int pixels = (difference < pallet_vertical_speed) ? difference : -1;
    if (middle_of_pallet < desired_y) {
        move_pallet_down(pallet, pixels);
    } else if (middle_of_pallet > desired_y) {
        move_pallet_up(pallet, pixels);
    }
}

void
move_pallet_up(Rectangle* pallet, int pixels)
{
    if (pixels < 0)
        pixels = pallet_vertical_speed;
    float dy = -(pixels * FRAME_ADJUSTMENT);
    if (pallet->y + dy > 0)
        pallet->y += dy;
    else
        pallet->y = 0;
}

void
move_pallet_down(Rectangle* pallet, int pixels)
{
    if (pixels < 0)
        pixels = pallet_vertical_speed;
    int bottom_of_pallet = pallet->y + pallet->height;
    float dy = pallet_vertical_speed * FRAME_ADJUSTMENT;
    if (bottom_of_pallet + dy < GetScreenHeight())
        pallet->y += dy;
    else
        pallet->y = GetScreenHeight() - pallet->height;
}

void
move_pallet_2_towards(int y)
{
    const int tolerance = 10;
    bool within_tolerance =
      abs((int)(pallet_2.y + pallet_2.height / 2 - y)) < tolerance;

    if (!within_tolerance) {
        ai_move_pallet(&pallet_2, y + ai_error_offset);
    }
}
