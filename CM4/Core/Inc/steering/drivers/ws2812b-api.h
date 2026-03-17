#ifndef WS2812B_API_H
#define WS2812B_API_H

#include "ws2812b.h"

enum WS2812BReturnCode ws2812b_encode(const uint8_t *input_rgb, uint16_t *output_pwm, size_t num_leds);

#endif // WS2812B_API_H
