#pragma once

#include "raylib.h"

void
draw_centered_text(const char* text,
                   int font_size,
                   float x_offset,
                   float y_offset);

Vector2
get_text_size(const char* text, int font_size);
