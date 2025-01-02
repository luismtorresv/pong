#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "stdlib.h"

#define PALLET_WIDTH 20
#define PALLET_HEIGHT 100
#define PALLET_HORIZONTAL_SEPARATION 100

#define FRAME_ADJUSTMENT (GetFrameTime() / (1.0f / 60.0f))

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum
{
    LEFT_PALLET = 0,
    RIGHT_PALLET = 1,
} PalletID;

typedef enum
{
    MOVE_UP = 0,
    MOVE_DOWN = 1,
} Direction;

typedef Rectangle Pallet;

typedef Rectangle Ball;

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
int counter_1 = 0;
int counter_2 = 0;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int finishScreen = 0;

static bool new_round = true;
static bool player_1_starts = true;
static Vector2 ball_speed = { 0.0f, 0.0f };

static Pallet pallet_1 = { .width = PALLET_WIDTH, .height = PALLET_HEIGHT };
static Pallet pallet_2 = { .width = PALLET_WIDTH, .height = PALLET_HEIGHT };
static Ball ball = { .x = 0, .y = 0, .width = 10, .height = 10 };

static const Vector2 ball_max_speed = { 100.0, 140.0 };
static const float pallet_vertical_speed = 8.0f;
static int ai_error_offset = 0;

static Sound hit_sound, score_sound;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------

static void
handle_keyboard_input();

static void
handle_collisions();

static void
ball_set_speed(Vector2 new_ball_speed);

static PalletID
pallet_which(Pallet* pallet);

static void
_pallet_move(Rectangle* pallet, Direction direction, int pixels);

static void
pallet_move(Rectangle* pallet, Direction direction);

static void
ai_pallet_move_towards(int y);

static void
ai_update_error_offset();

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void
InitGameplayScreen(void)
{
    finishScreen = 0;

    counter_1 = counter_2 = 0;
    new_round = player_1_starts = true;

    pallet_1.x = PALLET_HORIZONTAL_SEPARATION;
    pallet_1.y = (float)GetScreenHeight() / 2 - pallet_1.height / 2;

    pallet_2.x = GetScreenWidth() - PALLET_WIDTH - PALLET_HORIZONTAL_SEPARATION;
    pallet_2.y = (float)GetScreenHeight() / 2 - pallet_2.height / 2;

    ai_update_error_offset();

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
        ball.y = (GetRandomValue(0, 1)) ? (float)GetScreenHeight() : 0;

        // Set initial ball speed depending on round.
        ball_speed.x = (player_1_starts) ? -6.0f : 6.0f;
        ball_speed.y = (ball.y == (float)GetScreenHeight())
                         ? GetRandomValue(2, 4)
                         : GetRandomValue(-2, -4);
        player_1_starts = !player_1_starts;

        ai_update_error_offset();

        new_round = false;
    }

    ball.x += ball_speed.x * FRAME_ADJUSTMENT;
    ball.y -= ball_speed.y * FRAME_ADJUSTMENT;

    handle_keyboard_input();
    handle_collisions();

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

    // Counters.
    int font_size = 30;
    int y_pos = 40;
    float width = GetScreenWidth();
    DrawText(
      TextFormat("%d", counter_1), width * 1 / 4, y_pos, font_size, WHITE);
    DrawText(
      TextFormat("%d", counter_2), width * 3 / 4, y_pos, font_size, WHITE);

    // Middle lines.
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

    DrawRectangleRec(ball, WHITE);
    DrawRectangleRec(pallet_1, WHITE);
    DrawRectangleRec(pallet_2, WHITE);

    // Draw border so that the limits of the field are visible on Web.
    DrawRectangleLines(1, 0, GetScreenWidth()-1, GetScreenHeight()-1, WHITE);
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

//----------------------------------------------------------------------------------
// Gameplay
//----------------------------------------------------------------------------------

static void
handle_keyboard_input()
{
    // Player 1.
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_Q))
        pallet_move(&pallet_1, MOVE_UP);
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_A))
        pallet_move(&pallet_1, MOVE_DOWN);

    switch (gameMode) {
        case SINGLE_PLAYER:
            if (ball_speed.x > 0)
                ai_pallet_move_towards(ball.y);
            else
                ai_pallet_move_towards((float)GetScreenHeight() / 2);
            break;
        case DOUBLE_PLAYER:
            if (IsKeyDown(KEY_P))
                pallet_move(&pallet_2, MOVE_UP);
            if (IsKeyDown(KEY_L))
                pallet_move(&pallet_2, MOVE_DOWN);
            break;
        default:
            break;
    }
}

// Handle collisions for any of the pallets.
static void
handle_collisions()
{
    // It's impossible for both pallets to collide with the ball, so we only
    // need to know which one, if any, is colliding.
    Pallet* collision_pallet = NULL;
    if (CheckCollisionRecs(ball, pallet_1))
        collision_pallet = &pallet_1;
    else if (CheckCollisionRecs(ball, pallet_2))
        collision_pallet = &pallet_2;
    else // No collisions.
        return;

    // Zero out velocity if the ball hits right in the middle of the pallet.
    if (ball.y ==
        collision_pallet->y + collision_pallet->height / 2 - ball.height / 2)
        ball_speed.y = 0;
    else if (GetRandomValue(0, 1))
        ball_set_speed((Vector2){ ball_speed.x, ball_speed.y + 1 });
    else
        ball_set_speed((Vector2){ ball_speed.x, ball_speed.y - 1 });

    // Correct x position and increase horizontal velocity.
    switch (pallet_which(collision_pallet)) {
        case LEFT_PALLET:
            ball.x = collision_pallet->x + collision_pallet->width;
            ball_set_speed((Vector2){ ball_speed.x - 1, ball_speed.y });
            break;
        case RIGHT_PALLET:
            ball.x = collision_pallet->x - ball.width;
            ball_set_speed((Vector2){ ball_speed.x + 1, ball_speed.y });
            break;
        default:
            break;
    }

    // Bump.
    ball_speed.x = -ball_speed.x;

    PlaySound(hit_sound);

    ai_update_error_offset();
}

//----------------------------------------------------------------------------------
// Pallet
//----------------------------------------------------------------------------------

// Identify pallet for collision handling.
static PalletID
pallet_which(Pallet* pallet)
{
    if (pallet->x < (float)GetScreenWidth() / 2)
        return LEFT_PALLET;
    else
        return RIGHT_PALLET;
}

// Move a pallet without overflowing.
// Note: This is the "internal" version for use of both human players and AI.
static void
_pallet_move(Rectangle* pallet, Direction direction, int pixels)
{
    float dy;
    switch (direction) {
        case MOVE_UP: {
            dy = -(pixels * FRAME_ADJUSTMENT);
            if (pallet->y + dy > 0)
                pallet->y += dy;
            else
                pallet->y = 0;
        } break;
        case MOVE_DOWN: {
            dy = pixels * FRAME_ADJUSTMENT;
            float bottom_of_pallet = pallet->y + pallet->height;
            if (bottom_of_pallet + dy < GetScreenHeight())
                pallet->y += dy;
            else
                pallet->y = GetScreenHeight() - pallet->height;
        } break;
        default:
            break;
    }
}

static void
pallet_move(Rectangle* pallet, Direction direction)
{
    _pallet_move(pallet, direction, pallet_vertical_speed);
}

//----------------------------------------------------------------------------------
// Ball
//----------------------------------------------------------------------------------

// Only allow changing the ball speed if it does not surpass the maximum speed.
static void
ball_set_speed(Vector2 new_ball_speed)
{
    ball_speed = Vector2Min(ball_max_speed, new_ball_speed);
}

//----------------------------------------------------------------------------------
// AI (a.k.a single-player mode :)
//----------------------------------------------------------------------------------

static void
ai_update_error_offset()
{
    int half_amplitude = PALLET_HEIGHT / 2 + 5;
    ai_error_offset = GetRandomValue(-half_amplitude, half_amplitude);
}

// Move right pallet towards a certain vertical coordinate.
static void
ai_pallet_move_towards(int y)
{
    y += ai_error_offset;

    int middle_of_pallet = pallet_2.y + pallet_2.height / 2;
    int difference = abs(middle_of_pallet - y);
    int pixels =
      (difference < pallet_vertical_speed) ? difference : pallet_vertical_speed;

    ///
    // Honestly, I don't know whether this is a good or bad idea. I'll leave it
    // here in case I want to review it someday (probably never :)
    //
    /* const int tolerance = 10; */
    /* bool within_tolerance = difference < tolerance; */

    /* if (!within_tolerance) { */
    /*     _pallet_move(&pallet_2, y + ai_error_offset); */
    /* } */
    ///

    if (middle_of_pallet < y) {
        _pallet_move(&pallet_2, MOVE_DOWN, pixels);
    } else if (middle_of_pallet > y) {
        _pallet_move(&pallet_2, MOVE_UP, pixels);
    }
}
