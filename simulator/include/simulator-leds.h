/*!
 * \file simulator-leds.h
 * \date 2026-06-10
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Inverse of ws2812b_api_encode for the simulator.
 */

#ifndef SIMULATOR_LEDS_H
#define SIMULATOR_LEDS_H

#include "leds.h"
#include "ws2812b.h"
#include <stdint.h>

/*!
 * \brief Decode a WS2812B duty-cycle frame back into per-LED RGB values.
 *
 * \param buffer Encoded duty-cycle buffer (typically produced by
 *     \c ws2812b_api_encode and handed over by \c leds_api_show).
 * \param length Number of entries in \p buffer; the call is a no-op when
 *     shorter than a full frame.
 */
void simulator_leds_decode(const enum WS2812BDutyCycle *buffer, uint16_t length);

/*!
 * \brief Pointer to the most-recently-decoded LED state.
 *
 * \return Pointer to an array of \c LEDS_INDEX_COUNT \c LedColor entries.
 *     Valid until the next call to \c simulator_leds_decode.
 */
const struct LedColor *simulator_leds_get_colors(void);

#endif // SIMULATOR_LEDS_H
