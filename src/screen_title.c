#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Sound start_sound;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void
InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    start_sound = LoadSound("resources/start.ogg");
    PlaySound(start_sound);
}

// Title Screen Update logic
void
UpdateTitleScreen(void)
{
    if (TimerDone(timer)) {
        StartTimer(&timer, 2);
        finishScreen = 1;
    }
}

// Title Screen Draw logic
void
DrawTitleScreen(void)
{
    draw_centered_text("pong!", 30, 0, -50);
    draw_centered_text("programmed by L T", 20, 0, 0);
    draw_centered_text("powered by raylib", 20, 0, 50);
}

// Title Screen Unload logic
void
UnloadTitleScreen(void)
{
    UnloadSound(start_sound);
}

// Title Screen should finish?
int
FinishTitleScreen(void)
{
    return finishScreen;
}
