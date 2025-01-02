#include "raygui.h"
#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;
static bool can_init_sound = false;

//----------------------------------------------------------------------------------
// Start Screen Functions Definition
//----------------------------------------------------------------------------------

// Start Screen Initialization logic
void
InitStartScreen(void)
{
    finishScreen = 0;
}

// Start Screen Update logic
void
UpdateStartScreen(void)
{
    if (can_init_sound) {
        InitAudioDevice();
        finishScreen = 1;
    }
}

// Start Screen Draw logic
void
DrawStartScreen(void)
{
    if (GuiButton((Rectangle){ ((float)GetScreenWidth() - 200.0) / 2,
                               ((float)GetScreenHeight() - 50.0) / 2,
                               200.0,
                               50.0 },
                  "#131#Click to start game")) {
        can_init_sound = true;
    };
}

// Start Screen Unload logic
void
UnloadStartScreen(void)
{
}

// Start Screen should finish?
int
FinishStartScreen(void)
{
    return finishScreen;
}
