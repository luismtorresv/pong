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
    if (middle_of_pallet < desired_y) {
        move_pallet_down(pallet);
    } else if (middle_of_pallet > desired_y) {
        move_pallet_up(pallet);
    }
}

void
move_pallet_up(Rectangle* pallet)
{
    float dy = -(pallet_vertical_speed * FRAME_ADJUSTMENT);
    if (pallet->y + dy > 0)
        pallet->y += dy;
    else
        pallet->y = 0;
}

void
move_pallet_down(Rectangle* pallet)
{
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
