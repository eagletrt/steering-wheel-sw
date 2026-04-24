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

enum ScreenReturnCode screen_init(font_draw_line_callback draw_line, raster_draw_rectangle_callback draw_rectangle);
enum ScreenReturnCode screen_update(uint32_t tick);

#endif // SCREEN_API_H
