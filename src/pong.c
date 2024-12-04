/*******************************************************************************************
 *
 *   pong
 *
 ********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Rectangle pallet_1 =
  (Rectangle){ .x = 10, .y = 40, .width = 20, .height = 100 };
Rectangle pallet_2 =
  (Rectangle){ .x = 770, .y = 40, .width = 20, .height = 100 };
float speed = 5.0f;

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
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "pong");
  SetConfigFlags(FLAG_FULLSCREEN_MODE);
  SetWindowMinSize(screenWidth, screenHeight);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

// Update and draw game frame
static void
UpdateDrawFrame(void)
{
  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  {
  ClearBackground(RAYWHITE);

  DrawText("pong", 10, 40, 20, DARKGRAY);

  DrawFPS(10, 10);

  DrawRectangleRec(pallet_1, RED);
  DrawRectangleRec(pallet_2, RED);

  if (IsKeyDown(KEY_Q) && pallet_1.y >= 0)
      pallet_1.y -=
        speed; // Paradoxical, but that's the way the coordinate system works.
  if (IsKeyDown(KEY_A) && (pallet_1.y + pallet_1.height) <= GetScreenHeight())
    pallet_1.y += speed;

  if (IsKeyDown(KEY_P) && pallet_2.y >= 0)
      pallet_2.y -= speed;
  if (IsKeyDown(KEY_L) && (pallet_2.y + pallet_2.height) <= GetScreenHeight())
    pallet_2.y += speed;
  }
  EndDrawing();
  //----------------------------------------------------------------------------------
}
