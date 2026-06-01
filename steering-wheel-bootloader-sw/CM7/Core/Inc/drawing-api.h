#ifndef DRAWING_API_H
#define DRAWING_API_H

#include <stdint.h>
#include "colors.h"
#include "raster.h"

enum RasterReturnCode draw_rectangle_callback(uint16_t x, uint16_t y, uint16_t width, uint16_t height, struct Color color);

#endif // DRAWING_API_H
