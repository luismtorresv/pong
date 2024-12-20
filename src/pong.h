#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define PALLET_LEFT 1
#define PALLET_RIGHT 2

typedef enum GameScreen
{
  TITLE = 0,
  INSTRUCTIONS,
  GAMEPLAY,
  ENDING
} GameScreen;

typedef struct Timer
{
  double startTime; // Start time (seconds)
  double lifeTime;  // Lifetime (seconds)
} Timer;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void
UpdateDrawFrame(void); // Update and draw one frame

static Vector2
get_text_size(const char* text, int font_size);

static void
set_ball_speed(Vector2 new_ball_speed);
