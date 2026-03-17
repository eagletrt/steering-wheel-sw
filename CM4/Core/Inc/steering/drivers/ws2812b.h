#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stddef.h>

#define WS2812B_DUTY_0 35
#define WS2812B_DUTY_1 70
#define WS2812B_RESET_SLOTS 50

/*!
 * \brief Return codes for WS2812B functions.
 */
enum WS2812BReturnCode {
    WS2812B_RC_OK,           /*!< Operation successful. */
    WS2812B_RC_NULL_POINTER, /*!< A null pointer was passed to a function. */
};

#endif // WS2812B_H
