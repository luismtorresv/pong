/*******************************************************************************************
 *
 *   pong
 *
 ********************************************************************************************/

#include "screens.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
const int screenWidth = 1000;
const int screenHeight = 600;

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GameScreen currentScreen = START;
Timer timer = { 0 };

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void
UpdateDrawFrame(void); // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int
main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  SetTraceLogLevel(LOG_ALL);

  InitWindow(screenWidth, screenHeight, "pong");
  GuiLoadStyle("resources/style_terminal.rgs");
  SetWindowMinSize(screenWidth, screenHeight);

  currentScreen = START;
  InitStartScreen();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) { // Detect window close button or ESC key
    UpdateDrawFrame();
  }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------------------
  switch (currentScreen)
  {
    case START: UnloadStartScreen(); break;
    case TITLE: UnloadTitleScreen(); break;
    case INSTRUCTIONS: UnloadInstructionsScreen(); break;
    case GAMEPLAY: UnloadGameplayScreen(); break;
    case ENDING: UnloadEndingScreen(); break;
    default: break;
  }

  CloseAudioDevice();

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen
static void ChangeToScreen(int screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case START: UnloadStartScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case INSTRUCTIONS: UnloadInstructionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case START: InitStartScreen(); break;
        case TITLE: InitTitleScreen(); break;
        case INSTRUCTIONS: InitInstructionsScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case ENDING: InitEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
  switch(currentScreen)
  {
    case START:
      {
        UpdateStartScreen();

        if (FinishStartScreen()) ChangeToScreen(TITLE);

      } break;
    case TITLE:
      {
        UpdateTitleScreen();

        if (FinishTitleScreen()) ChangeToScreen(INSTRUCTIONS);

      } break;
    case INSTRUCTIONS:
      {
        UpdateInstructionsScreen();

        if (FinishInstructionsScreen()) ChangeToScreen(GAMEPLAY);

      } break;
    case GAMEPLAY:
      {
        UpdateGameplayScreen();

        if (FinishGameplayScreen()) ChangeToScreen(ENDING);
      } break;
    case ENDING:
      {
        UpdateEndingScreen();

        if (FinishEndingScreen()) ChangeToScreen(TITLE);

      } break;
    default: break;
  }
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  {
    ClearBackground(BLACK);

    switch(currentScreen)
    {
      case START: DrawStartScreen(); break;
      case TITLE: DrawTitleScreen(); break;
      case INSTRUCTIONS: DrawInstructionsScreen(); break;
      case GAMEPLAY: DrawGameplayScreen(); break;
      case ENDING: DrawEndingScreen(); break;
      default: break;
    }
  }
  EndDrawing();
  //----------------------------------------------------------------------------------
}
