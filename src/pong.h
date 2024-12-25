#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"

#define PALLET_LEFT 1
#define PALLET_RIGHT 2

#define PALLET_WIDTH 20
#define PALLET_HEIGHT 100
#define PALLET_HORIZONTAL_SEPARATION 100

typedef enum GameScreen
{
  START = 0,
  TITLE,
  INSTRUCTIONS,
  GAMEPLAY,
  ENDING
} GameScreen;

typedef struct Timer
{
  double startTime; // Start time (seconds)
  double lifeTime;  // Lifetime (seconds)
} Timer;

inline static double
GetElapsed(Timer timer);

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void
UpdateDrawFrame(void); // Update and draw one frame

static Vector2
get_text_size(const char* text, int font_size);

static void
set_ball_speed(Vector2 new_ball_speed);
