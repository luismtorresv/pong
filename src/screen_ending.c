#include "helpers.h"
#include "raylib.h"
#include "screens.h"
#include "timer.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;
static Sound end_sound;
static Timer timer = { 0 };

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void
InitEndingScreen(void)
{
    StartTimer(&timer, 3);
    finishScreen = 0;
    end_sound = LoadSound("resources/screens/ending/end.ogg");
    PlaySound(end_sound);
}

// Ending Screen Update logic
void
UpdateEndingScreen(void)
{
    if (TimerDone(timer)) {
        finishScreen = 1;
    }
}

// Ending Screen Draw logic
void
DrawEndingScreen(void)
{
    draw_centered_text("end of game!", 30, 0, -50);
    switch (gameMode) {
        case SINGLE_PLAYER:
            draw_centered_text((counter_1 > counter_2) ? "You win!"
                                                       : "The machine won",
                               50,
                               0,
                               0);
            break;
        case DOUBLE_PLAYER:
            draw_centered_text(
              TextFormat("Player %d wins!", (counter_1 > counter_2) ? 1 : 2),
              50,
              0,
              0);
            break;
        default:
            break;
    }
}

// Ending Screen Unload logic
void
UnloadEndingScreen(void)
{
    UnloadSound(end_sound);
}

// Ending Screen should finish?
int
FinishEndingScreen(void)
{
    return finishScreen;
}
