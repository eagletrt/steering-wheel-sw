#ifndef DRAWING_API_H
#define DRAWING_API_H

#include <stdint.h>
#include "colors.h"

void draw_line_callback(uint16_t x, uint16_t y, uint16_t width, struct Color color);

#endif // DRAWING_API_H
