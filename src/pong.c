/*******************************************************************************************
 *
 *   pong
 *
 ********************************************************************************************/

#include "pong.h"

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Rectangle pallet_1 = (Rectangle){ .x = 10, .y = 0, .width = 20, .height = 100 };
Rectangle pallet_2 =
  (Rectangle){ .x = 770, .y = 0, .width = 20, .height = 100 };
Rectangle ball = (Rectangle){ .x = 0, .y = 0, .width = 10, .height = 10 };

const Vector2 max_speed = { 100.0, 140.0 };
const float pallet_vertical_speed = 5.0f;

int counter_1 = 0;
int counter_2 = 0;
bool new_round = true;
bool player_1_starts = true;

Vector2 ball_speed = { 0.0f, 0.0f };

GameScreen current_screen = TITLE;
Timer timer;

//----------------------------------------------------------------------------------
// Ad hoc implementation of a timer from the wiki
//----------------------------------------------------------------------------------
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
    /* if (collision_rec.width >= collision_rec.height) { */
    /*   ball_speed.y = -ball_speed.y; */
    /*   if (collision_rec.y < pallet->y + pallet->height / 2) */
    /*     ball->y = pallet->y - ball->height; */
    /*   else */
    /*     ball->y = pallet->y + pallet->height; */
    /* } */

    // Zero out velocity if the ball hits right in the middle of the pallet.
    if (ball->y == pallet->y + pallet->height / 2 - ball->height / 2)
      ball_speed.y = 0;
    else if (GetRandomValue(0, 1))
      set_ball_speed((Vector2){ ball_speed.x, ball_speed.y + 1 });
    else
      set_ball_speed((Vector2){ ball_speed.x, ball_speed.y - 1 });

    // Correct x position and increase horizontal velocity.
    if (pallet_id == PALLET_LEFT) {
      ball->x = pallet->x + pallet->width;
      set_ball_speed((Vector2){ ball_speed.x - 1, ball_speed.y });
    } else {
      ball->x = pallet->x - ball->width;
      set_ball_speed((Vector2){ ball_speed.x + 1, ball_speed.y });
    }

    // Bump.
    ball_speed.x = -ball_speed.x;
  }
}

//----------------------------------------------------------------------------------
// Only allow changing the ball speed if it does not surpass the maximum speed.
//----------------------------------------------------------------------------------
static void
set_ball_speed(Vector2 new_ball_speed)
{
  TraceLog(
    LOG_DEBUG, "new_ball_speed: %4f, %4f", new_ball_speed.x, new_ball_speed.y);
  ball_speed = Vector2Min(ball_speed, new_ball_speed);
  TraceLog(
    LOG_DEBUG, "actual new_ball_speed: %4f, %4f", ball_speed.x, ball_speed.y);
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

  DrawText(text,
           (float)GetScreenWidth() / 2 - text_size.x / 2 + x_offset,
           (float)GetScreenHeight() / 2 - text_size.y / 2 + y_offset,
           font_size,
           WHITE);
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

static void
handle_keyboard_input()
{
  // Player 1.
  if (IsKeyDown(KEY_Q) && pallet_1.y >= 0)
    pallet_1.y -= pallet_vertical_speed; // Paradoxical, but that's the way
                                         // the coordinate system works.
  if (IsKeyDown(KEY_A) && (pallet_1.y + pallet_1.height) <= GetScreenHeight())
    pallet_1.y += pallet_vertical_speed;

  // Player 2.
  if (IsKeyDown(KEY_P) && pallet_2.y >= 0)
    pallet_2.y -= pallet_vertical_speed;
  if (IsKeyDown(KEY_L) && (pallet_2.y + pallet_2.height) <= GetScreenHeight())
    pallet_2.y += pallet_vertical_speed;
}

static void
draw_counters()
{
  int font_size = 30;
  int y_pos = 40;
  float width = GetScreenWidth();
  DrawText(TextFormat("%d", counter_1), width * 1 / 4, y_pos, font_size, WHITE);
  DrawText(TextFormat("%d", counter_2), width * 3 / 4, y_pos, font_size, WHITE);
}

static void
draw_middle_lines()
{
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
}

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int
main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  SetRandomSeed(671);
  SetTraceLogLevel(LOG_ALL);

  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "pong");
  SetWindowMinSize(screenWidth, screenHeight);

  StartTimer(
    &timer,
    2); // TODO: Temporal fix. Will probably use frames later on, we'll see.
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  pallet_1.y = (float)GetScreenHeight() / 2 - pallet_1.height / 2;
  pallet_2.y = (float)GetScreenHeight() / 2 - pallet_2.height / 2;
  // Main game loop
  while (!WindowShouldClose()) { // Detect window close button or ESC key
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
  // Update
  //---------------------------------------------------------------------------
  switch (current_screen) {
    case TITLE: {
      if (TimerDone(timer)) {
        current_screen = INSTRUCTIONS;
        StartTimer(&timer, 2);
      }
    } break;
    case INSTRUCTIONS: {
      if (TimerDone(timer)) {
        current_screen = GAMEPLAY;
      }
    } break;
    case GAMEPLAY: {
      if (new_round) {
        if (counter_1 >= 9 || counter_2 >= 9) {
          current_screen = ENDING;
          StartTimer(&timer, 2);
        }

        ball.x = (float)GetScreenWidth() / 2 - ball.width / 2;
        ball.y = (float)GetScreenHeight() / 2 - ball.height / 2;

        // Set initial ball speed depending on round.
        ball_speed.x = player_1_starts ? -6.0f : 6.0f;
        ball_speed.y = GetRandomValue(-2, 2);
        player_1_starts = !player_1_starts;

        new_round = false;
      }
    } break;
    case ENDING: {
      if (TimerDone(timer)) {
        current_screen = TITLE;
      }
    } break;
    default:
      break;
  }
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  {
    ClearBackground(BLACK);
    switch (current_screen) {
      case TITLE: {
        draw_centered_text("pong!", 30, 0, 0);
        draw_centered_text("programmed by L T", 20, 0, 50);
        draw_centered_text("powered by raylib", 20, 0, 150);
      } break;
      case INSTRUCTIONS: {
        draw_centered_text("Instructions", 30, 0, -150);
        draw_centered_text("Move up and down with", 30, 0, -50);
        draw_centered_text("Player 1: q-a", 30, 0, 0);
        draw_centered_text("Player 2: p-l", 30, 0, 50);
        draw_centered_text("First to score 9 wins", 30, 0, 200);
      } break;
      case GAMEPLAY: {
        DrawFPS(10, 10);
        draw_counters();
        draw_middle_lines();

        ball.x += ball_speed.x;
        ball.y -= ball_speed.y;

        handle_keyboard_input();

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
      } break;
      case ENDING: {
        draw_centered_text("end of game!", 30, 0, 0);
      } break;
      default:
        break;
    }
  }
  EndDrawing();
  //----------------------------------------------------------------------------------
}
