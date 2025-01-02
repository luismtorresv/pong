#include "helpers.h"
#include "raylib.h"
#include "screens.h"
#include "timer.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;
static Timer timer = { 0 };

//----------------------------------------------------------------------------------
// Instructions Screen Functions Definition
//----------------------------------------------------------------------------------

// Instructions Screen Initialization logic
void
InitInstructionsScreen(void)
{
    finishScreen = 0;
    StartTimer(&timer, 2);
}

// Instructions Screen Update logic
void
UpdateInstructionsScreen(void)
{
    if (TimerDone(timer)) {
        finishScreen = 1;
    }
}

// Instructions Screen Draw logic
void
DrawInstructionsScreen(void)
{
    draw_centered_text("Instructions", 30, 0, -150);
    switch (gameMode) {
        case SINGLE_PLAYER:
            draw_centered_text("Move with UP and DOWN", 30, 0, -50);
            break;
        case DOUBLE_PLAYER:
            draw_centered_text(
              "Player 1: UP and DOWN (or Q and A)", 30, 0, -50);
            draw_centered_text("Player 2: P and L", 30, 0, 0);
            break;
        default:
            gameMode = SINGLE_PLAYER;
            TraceLog(LOG_WARNING, "Game mode with NONE value");
            break;
    }
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
