#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Instructions Screen Functions Definition
//----------------------------------------------------------------------------------

// Instructions Screen Initialization logic
void
InitInstructionsScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
}

// Instructions Screen Update logic
void
UpdateInstructionsScreen(void)
{
    if (framesCounter++ == 120) {
        finishScreen = 1;
    }
}

// Instructions Screen Draw logic
void
DrawInstructionsScreen(void)
{
    draw_centered_text("Instructions", 30, 0, -150);
    draw_centered_text("Move with UP and DOWN", 30, 0, -50);
    /* draw_centered_text("Player 1: Q and A", 30, 0, 0); */
    /* draw_centered_text("Player 2: P and L", 30, 0, 50); */
    draw_centered_text("First to score 9 wins", 30, 0, 100);
}

// Instructions Screen Unload logic
void
UnloadInstructionsScreen(void)
{
}

// Instructions Screen should finish?
int
FinishInstructionsScreen(void)
{
    return finishScreen;
}
