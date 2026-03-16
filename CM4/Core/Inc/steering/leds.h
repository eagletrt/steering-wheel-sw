#ifndef LEDS_H
#define LEDS_H

#include "ws2812b.h"

#define LEDS_COUNT 9
#define LEDS_PWM_BUFFER_SIZE (LEDS_COUNT * 24 + 3)
#define LEDS_PWM_DUTY_0 35
#define LEDS_PWM_DUTY_1 70

/*!
 * \brief Return codes for LED operations.
 */
enum LedsReturnCode {
    LEDS_RC_OK,        /*!< Operation successful. */
    LEDS_DRIVER_ERROR, /*!< Error initializing or operating the LED driver. */
};

/*!
 * \brief Handler structure for managing the LED system, including the WS2812B handler.
 */
struct LedsHandler {
    struct WS2812BHandler ws2812b_handler; /*!< Handler for the WS2812B LED strip. */
};

#endif // LEDS_H
