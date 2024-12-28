#include "pallet.h"
#include "raylib.h"
#include "screens.h"

#define PALLET_LEFT 1
#define PALLET_RIGHT 2

#define PALLET_WIDTH 20
#define PALLET_HEIGHT 100
#define PALLET_HORIZONTAL_SEPARATION 100

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

int counter_1 = 0;
int counter_2 = 0;
bool new_round = true;
bool player_1_starts = true;
Vector2 ball_speed = { 0.0f, 0.0f };

Rectangle pallet_1 = { .width = PALLET_WIDTH, .height = PALLET_HEIGHT };
Rectangle pallet_2 = { .width = PALLET_WIDTH, .height = PALLET_HEIGHT };
Rectangle ball = { .x = 0, .y = 0, .width = 10, .height = 10 };

const Vector2 max_speed = { 100.0, 140.0 };
const float pallet_vertical_speed = 5.0f;
int ai_error_offset;

Sound hit_sound, score_sound;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
Vector2
get_text_size(const char* text, int font_size);

static void
set_ball_speed(Vector2 new_ball_speed);

static void
update_ai_error_offset();

static void
handle_keyboard_input();

static void
handle_collisions(Rectangle* ball, Rectangle* pallet, int pallet_id);

static void
draw_counters();

static void
draw_middle_lines();

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void
InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    counter_1 = counter_2 = 0;
    new_round = player_1_starts = true;

    pallet_1.x = PALLET_HORIZONTAL_SEPARATION;
    pallet_1.y = (float)GetScreenHeight() / 2 - pallet_1.height / 2;

    pallet_2.x = GetScreenWidth() - PALLET_WIDTH - PALLET_HORIZONTAL_SEPARATION;
    pallet_2.y = (float)GetScreenHeight() / 2 - pallet_2.height / 2;

    update_ai_error_offset();

    hit_sound = LoadSound("resources/screens/gameplay/hit.ogg");
    score_sound = LoadSound("resources/screens/gameplay/score.ogg");
    SetSoundVolume(score_sound, 0.5);
}

// Gameplay Screen Update logic
void
UpdateGameplayScreen(void)
{
    if (new_round) {
        bool initial_round = counter_1 == 0 && counter_2 == 0;
        bool final_round = counter_1 >= 9 || counter_2 >= 9;

        if (final_round) {
            finishScreen = 1;
            return;
        } else if (!initial_round) {
            PlaySound(score_sound);
        }

        ball.x = (float)GetScreenWidth() / 2 - ball.width / 2;
        ball.y = GetRandomValue(0, 1) ? (float)GetScreenHeight() : 0;

        // Set initial ball speed depending on round.
        ball_speed.x = player_1_starts ? -6.0f : 6.0f;
        ball_speed.y = ball.y == (float)GetScreenHeight()
                         ? GetRandomValue(2, 4)
                         : GetRandomValue(-2, -4);
        player_1_starts = !player_1_starts;

        update_ai_error_offset();

        new_round = false;
    }

    ball.x += ball_speed.x * FRAME_ADJUSTMENT;
    ball.y -= ball_speed.y * FRAME_ADJUSTMENT;

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
}

// Gameplay Screen Draw logic
void
DrawGameplayScreen(void)
{
    DrawFPS(10, 10);
    draw_counters();
    draw_middle_lines();
    DrawRectangleRec(ball, WHITE);
    DrawRectangleRec(pallet_1, WHITE);
    DrawRectangleRec(pallet_2, WHITE);
}

// Gameplay Screen Unload logic
void
UnloadGameplayScreen(void)
{
    UnloadSound(hit_sound);
    UnloadSound(score_sound);
}

// Gameplay Screen should finish?
int
FinishGameplayScreen(void)
{
    return finishScreen;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
static void
handle_keyboard_input()
{
    // Player 1.
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_Q))
        move_pallet_up(&pallet_1, -1);
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_A))
        move_pallet_down(&pallet_1, -1);

    // Player 2.
    /* if (IsKeyDown(KEY_P)) */
    /*   move_pallet_up(&pallet_2); */
    /* if (IsKeyDown(KEY_L)) */
    /*   move_pallet_down(&pallet_2); */

    // TODO: Create single-player mode.
    if (ball_speed.x > 0)
        move_pallet_2_towards(ball.y);
    else
        ai_move_pallet(&pallet_2, (float)GetScreenHeight() / 2);
}

static void
draw_counters()
{
    int font_size = 30;
    int y_pos = 40;
    float width = GetScreenWidth();
    DrawText(
      TextFormat("%d", counter_1), width * 1 / 4, y_pos, font_size, WHITE);
    DrawText(
      TextFormat("%d", counter_2), width * 3 / 4, y_pos, font_size, WHITE);
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
// Handle collisions for both pallets
//----------------------------------------------------------------------------------
static void
handle_collisions(Rectangle* ball, Rectangle* pallet, int pallet_id)
{
    if (CheckCollisionRecs(*ball, *pallet)) {
        Rectangle collision_rec = GetCollisionRec(*ball, *pallet);
        /* TraceLog(LOG_DEBUG, */
        /*          TextFormat("x=%f, y=%f, width=%f, height=%f", */
        /*                     collision_rec.x, */
        /*                     collision_rec.y, */
        /*                     collision_rec.width, */
        /*                     collision_rec.height)); */

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

        PlaySound(hit_sound);

        update_ai_error_offset();
        /* TraceLog(LOG_DEBUG, "ai_error_offset: %d", ai_error_offset); */
    }
}

//----------------------------------------------------------------------------------
// Only allow changing the ball speed if it does not surpass the maximum speed.
//----------------------------------------------------------------------------------
static void
set_ball_speed(Vector2 new_ball_speed)
{
    /* TraceLog( */
    /*   LOG_DEBUG, "new_ball_speed: %4f, %4f", new_ball_speed.x,
     * new_ball_speed.y); */
    ball_speed = Vector2Min(ball_speed, new_ball_speed);
    /* TraceLog( */
    /*   LOG_DEBUG, "actual new_ball_speed: %4f, %4f", ball_speed.x,
     * ball_speed.y); */
}

static void
update_ai_error_offset()
{
    int half_amplitude = PALLET_HEIGHT / 2 + 5;
    ai_error_offset = GetRandomValue(-half_amplitude, half_amplitude);
}
