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
char counter_1 = '0';
char counter_2 = '0';

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void
UpdateDrawFrame(void); // Update and draw one frame

//----------------------------------------------------------------------------------
// Ad hoc implementation of a timer from the wiki
//----------------------------------------------------------------------------------
typedef struct Timer
{
  double startTime; // Start time (seconds)
  double lifeTime;  // Lifetime (seconds)
} Timer;

static void
StartTimer(Timer* timer, double lifetime)
{
  timer->startTime = GetTime();
  timer->lifeTime = lifetime;
}

static bool
TimerDone(Timer timer)
{
  return GetTime() - timer.startTime >= timer.lifeTime;
}

static double
GetElapsed(Timer timer)
{
  return GetTime() - timer.startTime;
}

//----------------------------------------------------------------------------------
// Utility function to center text on the screen
// See also <https://old.reddit.com/r/raylib/comments/1c8wcqd/comment/l0hk1g1/>
//----------------------------------------------------------------------------------
Vector2
get_text_size(const char* text, int font_size)
{
  int defaultFontSize = 10; // From rtext.c
  int spacing = font_size / defaultFontSize;
  return MeasureTextEx(
    GetFontDefault(), text, (float)font_size, (float)spacing);
}

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
  SetWindowMinSize(screenWidth, screenHeight);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Show welcome message
  char* title = "pong!";
  Vector2 text_size = get_text_size(title, 30);

  Timer timer;
  StartTimer(&timer, 2);
  while (!TimerDone(timer)) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawText(title,
               (float)GetScreenWidth() / 2 - text_size.x / 2,
               (float)GetScreenHeight() / 2 - text_size.y / 2,
               30,
               WHITE);
    }
    EndDrawing();
  }

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
    ClearBackground(BLACK);

    DrawFPS(10, 10);

    DrawRectangleRec(pallet_1, WHITE);
    DrawRectangleRec(pallet_2, WHITE);

    int line_separation = 10;
    int line_height = 20;
    for (int y_dashed = 0; y_dashed + line_height <= GetScreenHeight();
         y_dashed += (line_height + line_separation)) {
      DrawLine(GetScreenWidth() / 2,
               y_dashed,
               GetScreenWidth() / 2,
               y_dashed + line_height,
               WHITE);
    }

    DrawTextCodepoint(GetFontDefault(),
                      counter_1,
                      (Vector2){ GetScreenWidth() * 1 / 4, 40 },
                      30,
                      WHITE);
    DrawTextCodepoint(GetFontDefault(),
                      counter_2,
                      (Vector2){ GetScreenWidth() * 3 / 4, 40 },
                      30,
                      WHITE);

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