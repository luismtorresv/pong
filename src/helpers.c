#include "raylib.h"
#include "raymath.h"

static void
draw_centered_text(const char* text,
                   int font_size,
                   float x_offset,
                   float y_offset);

static Vector2
get_text_size(const char* text, int font_size);

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

