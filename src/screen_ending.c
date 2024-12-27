#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Sound end_sound;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void
InitEndingScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    end_sound = LoadSound("resources/screens/ending/end.ogg");
    PlaySound(end_sound);
}

// Ending Screen Update logic
void
UpdateEndingScreen(void)
{
    if (TimerDone(timer)) {
        StartTimer(&timer, 2);
        finishScreen = 1;
    }
}

// Ending Screen Draw logic
void
DrawEndingScreen(void)
{
    draw_centered_text("end of game!", 30, 0, -50);
    draw_centered_text(
      TextFormat("Player %d wins!", counter_1 > counter_2 ? 1 : 2), 50, 0, 0);
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
