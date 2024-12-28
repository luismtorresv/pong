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

typedef struct Pallet
{
    Rectangle data;
    PalletID id;
} Pallet;

typedef Rectangle Ball;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

int counter_1 = 0;
int counter_2 = 0;
static bool new_round = true;
static bool player_1_starts = true;
static Vector2 ball_speed = { 0.0f, 0.0f };

static Pallet pallet_1 = { .data = { .width = PALLET_WIDTH,
                                     .height = PALLET_HEIGHT },
                           .id = LEFT_PALLET };
static Pallet pallet_2 = { .data = { .width = PALLET_WIDTH,
                                     .height = PALLET_HEIGHT },
                           .id = RIGHT_PALLET };
static Ball ball = { .x = 0, .y = 0, .width = 10, .height = 10 };

static const Vector2 ball_max_speed = { 100.0, 140.0 };
static const float pallet_vertical_speed = 5.0f;
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

static void
_pallet_move(Rectangle* pallet, int desired_y);

static void
pallet_move_up(Rectangle* pallet, int pixels);

static void
pallet_move_down(Rectangle* pallet, int pixels);

static void
ai_update_error_offset();

static void
ai_move_right_pallet_towards(int y);

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

    pallet_1.data.x = PALLET_HORIZONTAL_SEPARATION;
    pallet_1.data.y = (float)GetScreenHeight() / 2 - pallet_1.data.height / 2;

    pallet_2.data.x =
      GetScreenWidth() - PALLET_WIDTH - PALLET_HORIZONTAL_SEPARATION;
    pallet_2.data.y = (float)GetScreenHeight() / 2 - pallet_2.data.height / 2;

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
    DrawRectangleRec(pallet_1.data, WHITE);
    DrawRectangleRec(pallet_2.data, WHITE);
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
        pallet_move_up(&pallet_1.data, -1);
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_A))
        pallet_move_down(&pallet_1.data, -1);

    // Player 2.
    /* if (IsKeyDown(KEY_P)) */
    /*   move_pallet_up(&pallet_2); */
    /* if (IsKeyDown(KEY_L)) */
    /*   move_pallet_down(&pallet_2); */

    // TODO: Create single-player mode.
    if (ball_speed.x > 0)
        ai_move_right_pallet_towards(ball.y);
    else
        _pallet_move(&pallet_2.data, (float)GetScreenHeight() / 2);
}

// Handle collisions for both pallets
static void
handle_collisions()
{
    static Pallet* pallets[2] = { &pallet_1, &pallet_2 };
    for (int i = 0; i < 2; ++i) {
        if (CheckCollisionRecs(ball, pallets[i]->data)) {
            // Zero out velocity if the ball hits right in the middle of the
            // pallet.
            if (ball.y == pallets[i]->data.y + pallets[i]->data.height / 2 -
                            ball.height / 2)
                ball_speed.y = 0;
            else if (GetRandomValue(0, 1))
                ball_set_speed((Vector2){ ball_speed.x, ball_speed.y + 1 });
            else
                ball_set_speed((Vector2){ ball_speed.x, ball_speed.y - 1 });

            // Correct x position and increase horizontal velocity.
            switch (pallets[i]->id) {
                case LEFT_PALLET:
                    ball.x = pallets[i]->data.x + pallets[i]->data.width;
                    ball_set_speed((Vector2){ ball_speed.x - 1, ball_speed.y });
                    break;
                case RIGHT_PALLET:
                    ball.x = pallets[i]->data.x - ball.width;
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
    }
}

//----------------------------------------------------------------------------------
// Pallet
//----------------------------------------------------------------------------------

// Move a pallet without overflowing.
static void
_pallet_move(Rectangle* pallet, int desired_y)
{
    int middle_of_pallet = pallet->y + pallet->height / 2;
    int difference = abs(middle_of_pallet - desired_y);
    int pixels = (difference < pallet_vertical_speed) ? difference : -1;
    if (middle_of_pallet < desired_y) {
        pallet_move_down(pallet, pixels);
    } else if (middle_of_pallet > desired_y) {
        pallet_move_up(pallet, pixels);
    }
}

static void
pallet_move_up(Rectangle* pallet, int pixels)
{
    if (pixels < 0)
        pixels = pallet_vertical_speed;
    float dy = -(pixels * FRAME_ADJUSTMENT);
    if (pallet->y + dy > 0)
        pallet->y += dy;
    else
        pallet->y = 0;
}

static void
pallet_move_down(Rectangle* pallet, int pixels)
{
    if (pixels < 0)
        pixels = pallet_vertical_speed;
    int bottom_of_pallet = pallet->y + pallet->height;
    float dy = pallet_vertical_speed * FRAME_ADJUSTMENT;
    if (bottom_of_pallet + dy < GetScreenHeight())
        pallet->y += dy;
    else
        pallet->y = GetScreenHeight() - pallet->height;
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

static void
ai_move_right_pallet_towards(int y)
{
    const int tolerance = 10;
    bool within_tolerance =
      abs((int)(pallet_2.data.y + pallet_2.data.height / 2 - y)) < tolerance;

    if (!within_tolerance) {
        _pallet_move(&pallet_2.data, y + ai_error_offset);
    }
}
