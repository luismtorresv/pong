#include "raylib.h"
#include "raymath.h"

void
move_pallet(Rectangle* pallet, int desired_y);

void
move_pallet_up(Rectangle* pallet, int pixels);

void
move_pallet_down(Rectangle* pallet, int pixels);

void
move_pallet_2_towards(int y);

void
ai_move_pallet(Rectangle* pallet, int desired_y);
