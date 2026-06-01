#ifndef SCREEN_API_H
#define SCREEN_API_H

#include "input-events.h"
#include "screen.h"

/*!
 * \brief Handle a parameter change event by showing the popup overlay.
 *
 * \details Stamps the current tick and refreshes the popup labels. The
 *     raster interface swap is deferred to screen_update so that rendering
 *     stays off the IPC ISR path.
 *
 * \param parameter_id The parameter that was changed.
 * \param value The new value of the parameter.
 *
 * \retval INPUT_EVENTS_RC_OK on success.
 * \retval INPUT_EVENTS_RC_ERROR on failure.
 */
enum InputEventsReturnCode screen_on_parameter_change(enum InputsSharedParameterID parameter_id, uint8_t value);

/*!
 * \brief Initialize the screen interface.
 *
 * \details Sets up the raster interface and the popup overlay. The popup starts
 *     in the inactive state.
 *
 * \param draw_rectangle Raster rendering callback provided by the caller.
 *
 * \retval SCREEN_RC_OK if initialization was successful.
 * \retval SCREEN_RC_ERROR if initialization failed.
 */
enum ScreenReturnCode screen_init(raster_draw_rectangle_callback draw_rectangle);

/*!
 * \brief Update the screen state, including popup visibility and raster interface.
 *
 * \details Checks whether the popup should still be active based on the current
 *     tick, and updates the raster interface accordingly.
 *
 * \param tick Current tick count in milliseconds, used for popup timeout checks.
 *
 * \retval SCREEN_RC_OK if the screen was updated successfully.
 * \retval SCREEN_RC_ERROR if an error occurred during update.
 */
enum ScreenReturnCode screen_update(uint32_t tick);

#endif // SCREEN_API_H
