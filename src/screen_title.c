#include "helpers.h"
#include "raygui.h"
#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Sound start_sound;

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GameMode gameMode = NONE;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void
InitTitleScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    gameMode = NONE;
    start_sound = LoadSound("resources/screens/title/start.ogg");
    PlaySound(start_sound);
}

// Title Screen Update logic
void
UpdateTitleScreen(void)
{
    if (framesCounter < 120)
        ++framesCounter;

    if (gameMode != NONE)
        finishScreen = 1;
}

// Title Screen Draw logic
void
DrawTitleScreen(void)
{
    draw_centered_text("pung!", 30, 0, -50);
    draw_centered_text("programmed by L T", 20, 0, 0);
    draw_centered_text("powered by raylib", 20, 0, 50);

    if (framesCounter == 120) {
        const float button_width = 180.0f;
        const float button_height = 50.0f;

        const float separation = button_width / 2 + 20.0f;
        const float y_offset = 140.0f;

        float center_x = ((float)GetScreenWidth() - button_width) / 2;
        float center_y = ((float)GetScreenHeight() - button_height) / 2;

        if (GuiButton((Rectangle){ center_x - separation,
                                   center_y + y_offset,
                                   button_width,
                                   button_height },
                      "Single player\n(against the machine)"))
            gameMode = SINGLE_PLAYER;
        else if (GuiButton((Rectangle){ center_x + separation,
                                        center_y + y_offset,
                                        button_width,
                                        button_height },
                           "Two players"))
            gameMode = DOUBLE_PLAYER;
    }
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
