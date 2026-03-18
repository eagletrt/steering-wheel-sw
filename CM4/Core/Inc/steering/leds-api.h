/*!
 * \file leds-api.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines the API for controlling the LED strip on the steering wheel, including functions for setting individual LED colors, filling the strip with a color, clearing the strip, and transmitting data to the LEDs.
 */

#ifndef LEDS_API_H
#define LEDS_API_H

#include "leds.h"

/*!
 * \brief Initializes the LED system, clearing all LEDs and preparing the driver for operation.
 *
 * \param transmit Function pointer for transmitting data to the LEDs.
 *
 * \retval LEDS_RC_OK Initialization successful.
 * \retval LEDS_RC_NULL_POINTER A null pointer was passed for the transmit callback.
 */
enum LedsReturnCode leds_api_init(leds_transmit_callback transmit);

/*!
 * \brief Sets the color of a specific LED in the strip.
 *
 * \param index Index of the LED to be set (0-based).
 * \param color LedColor structure representing the desired color for the LED.
 *
 * \retval LEDS_RC_OK Color set successfully.
 * \retval LEDS_RC_INVALID_LED The specified LED index is out of range.
 */
enum LedsReturnCode leds_api_set_led(enum LedsIndex index, struct LedColor color);

/*!
 * \brief Fills the entire LED strip with a specific color.
 *
 * \param color LedColor structure representing the desired color for all LEDs.
 */
void leds_api_fill(struct LedColor color);

/*!
 * \brief Clears the LED strip by setting all LEDs to off (black).
 */
void leds_api_clear(void);

/*!
 * \brief Transmits the current LED color data to the LED strip, updating the displayed colors.
 *
 * \param brightness Brightness level for the LEDs (0-255), where 0 is off and 255 is full brightness (scaling is internal)
 *
 * \retval LEDS_RC_OK Data transmitted successfully.
 * \retval LEDS_RC_NULL_POINTER The transmit callback function is not set.
 * \retval LEDS_RC_TRANSMISSION_ERROR An error occurred during data transmission.
 */
enum LedsReturnCode leds_api_show(uint8_t brightness);

/*!
 * \brief Sets the LEDs to indicate the PTT (Push-To-Talk) status.
 *
 * This function can be used to visually indicate when the PTT is active.
 *
 * \details The middle 5 LEDs (indices 0-4) will be set to blue
 */
void leds_api_ptt();

/*!
 * \brief Sets the LEDs to indicate the current lap status.
 *
 * This function can be used to visually indicate the current lap number.
 *
 * \details The top left and right LEDs (indices 5-8) will be set to off (TODO: ask if green is better)
 */
void leds_api_target_lap(void);

/*!
 * \brief Sets the LEDs to indicate the current lap status.
 *
 * This function can be used to visually indicate the lap delta is improving.
 *
 * \details The top left and right LEDs (indices 5-8) will be set to green (TODO: ask if green or purple)
 */
void leds_api_fast_lap(void);

/*!
 * \brief Sets the LEDs to indicate the current lap status.
 *
 * This function can be used to visually indicate the lap delta is worsening.
 *
 * \details The top left and right LEDs (indices 5-8) will be set to yellow
 */
void leds_api_slow_lap(void);

/*!
 * \brief Sets the LEDs to indicate an error state.
 *
 * This function can be used to visually indicate that an error has occurred.
 *
 * \details The middle 5 LEDs (indices 0-4) will be set to red
 */
void leds_api_error(void);

/*!
 * \brief Sets the LEDs to indicate an OK state.
 *
 * This function can be used to visually indicate that everything is functioning correctly.
 *
 * \details The middle 5 LEDs (indices 0-4) will be set to off (TODO: ask if green is better)
 */
void leds_api_ok(void);

#endif // LEDS_API_H
