/*******************************************************************************************
 *
 *   pong
 *
 ********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define PALLET_LEFT 1
#define PALLET_RIGHT 2

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Rectangle pallet_1 = (Rectangle){ .x = 10, .y = 0, .width = 20, .height = 100 };
Rectangle pallet_2 =
  (Rectangle){ .x = 770, .y = 0, .width = 20, .height = 100 };
float speed = 5.0f;
char counter_1 = '0';
char counter_2 = '0';
Rectangle ball = (Rectangle){ .x = 0, .y = 0, .width = 10, .height = 10 };
bool new_round = true;
Vector2 ball_speed = { 0.0f, 0.0f };
bool player_1_starts = true;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static bool
UpdateDrawFrame(void); // Update and draw one frame

static Vector2
get_text_size(const char* text, int font_size);

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
// Handle collisions for both pallets
//----------------------------------------------------------------------------------
static void
handle_collisions(Rectangle* ball, Rectangle* pallet, int pallet_id)
{
  if (CheckCollisionRecs(*ball, *pallet)) {
    Rectangle collision_rec = GetCollisionRec(*ball, *pallet);
    TraceLog(LOG_DEBUG,
             TextFormat("x=%f, y=%f, width=%f, height=%f",
                        collision_rec.x,
                        collision_rec.y,
                        collision_rec.width,
                        collision_rec.height));

    // Handle case where the ball hits from below.
    if (collision_rec.width >= collision_rec.height) {
      ball_speed.y = -ball_speed.y;
      if (collision_rec.y < pallet->y + pallet->height / 2)
        ball->y = pallet->y - ball->height;
      else
        ball->y = pallet->y + pallet->height;
    }

    // Zero out velocity if the ball hits right in the middle of the pallet.
    if (ball->y == pallet->y + pallet->height / 2 - ball->height / 2)
      ball_speed.y = 0;
    else
      ball_speed.y += 1;

    // Correct x position and increase horizontal velocity.
    if (pallet_id == PALLET_LEFT) {
      ball->x = pallet->x + pallet->width;
      ball_speed.x -= 1;
    } else {
      ball->x = pallet->x - ball->width;
      ball_speed.x += 1;
    }

    // Bump.
    ball_speed.x = -ball_speed.x;
  }
}

//----------------------------------------------------------------------------------
// Draw centered text on the screen.
//----------------------------------------------------------------------------------
static void
draw_centered_text(const char* text,
                   int font_size,
                   float x_offset,
                   float y_offset)
{
  Vector2 text_size = get_text_size(text, font_size);

  BeginDrawing();
  {
    ClearBackground(BLACK);
    DrawText(text,
             (float)GetScreenWidth() / 2 - text_size.x / 2 + x_offset,
             (float)GetScreenHeight() / 2 - text_size.y / 2 + y_offset,
             font_size,
             WHITE);
  }
  EndDrawing();
}

//----------------------------------------------------------------------------------
// Display the name of the game.
//----------------------------------------------------------------------------------
static void
show_welcome_message()
{
  draw_centered_text("pong!", 30, 0, 0);
  Timer timer;
  StartTimer(&timer, 2);
  while (!TimerDone(timer)) {
  }
}

//----------------------------------------------------------------------------------
// Utility function to center text on the screen
// See also
// <https://old.reddit.com/r/raylib/comments/1c8wcqd/comment/l0hk1g1/>
//----------------------------------------------------------------------------------
static Vector2
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
  SetTraceLogLevel(LOG_ALL);

  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "pong");
  SetWindowMinSize(screenWidth, screenHeight);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  show_welcome_message();

  // Main game loop
  bool has_game_ended = false;
  while (!WindowShouldClose() // Detect window close button or ESC key
         && !has_game_ended) {
    has_game_ended = UpdateDrawFrame();
  }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

// Update and draw game frame
static bool
UpdateDrawFrame(void)
{
  if (new_round) {
    if (counter_1 > '9' || counter_2 > '9') {
      draw_centered_text("end of game!", 30, 0, 0);
      Timer timer;
      StartTimer(&timer, 2);
      while (!TimerDone(timer)) {
      }
      return true; // Game has ended
    }

    ball.x = (float)GetScreenWidth() / 2 - ball.width / 2;
    ball.y = (float)GetScreenHeight() / 2 - ball.height / 2;

    pallet_1.y = (float)GetScreenHeight() / 2 - pallet_1.height / 2;
    pallet_2.y = (float)GetScreenHeight() / 2 - pallet_2.height / 2;

    ball_speed.x = player_1_starts ? -3.0f : 3.0f;
    ball_speed.y = 0;
    player_1_starts = !player_1_starts;

    new_round = false;
  }

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  {
    ClearBackground(BLACK);

    DrawFPS(10, 10);

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

    ball.x += ball_speed.x;
    ball.y -= ball_speed.y;

    if (IsKeyDown(KEY_Q) && pallet_1.y >= 0)
      pallet_1.y -=
        speed; // Paradoxical, but that's the way the coordinate system works.
    if (IsKeyDown(KEY_A) && (pallet_1.y + pallet_1.height) <= GetScreenHeight())
      pallet_1.y += speed;

    if (IsKeyDown(KEY_P) && pallet_2.y >= 0)
      pallet_2.y -= speed;
    if (IsKeyDown(KEY_L) && (pallet_2.y + pallet_2.height) <= GetScreenHeight())
      pallet_2.y += speed;

    handle_collisions(&ball, &pallet_1, PALLET_LEFT);
    handle_collisions(&ball, &pallet_2, PALLET_RIGHT);

    if (ball.y < 0) {
      ball.y = 0;
      ball_speed.y = -ball_speed.y;
    } else if (ball.y + ball.height > GetScreenHeight()) {
      ball.y = GetScreenHeight() - ball.height;
      ball_speed.y = -ball_speed.y;
    }

    if (ball.x + ball.width < 0) {
      ++counter_2;
      new_round = true;
    } else if (ball.x > GetScreenWidth()) {
      ++counter_1;
      new_round = true;
    }

    DrawRectangleRec(ball, WHITE);
    DrawRectangleRec(pallet_1, WHITE);
    DrawRectangleRec(pallet_2, WHITE);
  }
  EndDrawing();
  //----------------------------------------------------------------------------------
  return false;
}