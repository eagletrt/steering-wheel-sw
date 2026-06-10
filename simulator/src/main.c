/*!
 * \file main.c
 * \date 2026-06-10
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Host-side simulator for the steering-wheel firmware.
 *
 * \details Keyboard map:
 *       q/w  knob FRONT_LEFT  (POWER)        -/+
 *       a/s  knob FRONT_RIGHT (REGEN)        -/+
 *       z/x  knob SIDE_LEFT   (TORQUE)       -/+
 *       e    button BOTTOM_LEFT  (TELEMETRY LOG toggle)
 *       d    button BOTTOM_RIGHT (LAUNCH CONTROL toggle)
 *       c    paddle TOP_LEFT  (PTT, hold-to-activate)
 *       v    paddle TOP_RIGHT (PTT, hold-to-activate)
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "fsm.h"
#include "input-events-api.h"
#include "post.h"
#include "raster.h"
#include "screen-api.h"

#include "inputs-api.h"
#include "leds-api.h"
#include "parameters-api.h"

#include "inputs-shared.h"
#include "ipc-queue-api.h"
#include "ipc-ui-data-api.h"

#include "tigr.h"

#define SIMULATOR_WIDTH  (800)
#define SIMULATOR_HEIGHT (480)

static Tigr *window;

static int16_t simulator_knob_positions[INPUTS_SHARED_KNOB_ID_COUNT];
static bool simulator_paddle_held[INPUTS_SHARED_BUTTON_ID_COUNT];

/*!
 * \brief Raster rectangle callback backed by the Tigr framebuffer.
 *
 * \details Clamps to the window size so an off-by-one in some upstream
 *     layout cannot bleed into invalid memory.
 *
 * \param x Top-left corner X coordinate.
 * \param y Top-left corner Y coordinate.
 * \param w Rectangle width in pixels.
 * \param h Rectangle height in pixels.
 * \param color Fill color, including alpha for blending with the existing
 *     pixel color.
 *
 * \retval RASTER_RC_OK if the rectangle was drawn successfully
 * \retval RASTER_RC_ERROR if an error occurred (e.g. invalid parameters)
 */
static enum RasterReturnCode simulator_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, struct Color color) {
    if (x >= SIMULATOR_WIDTH || y >= SIMULATOR_HEIGHT) {
        return RASTER_RC_OK;
    }
    const uint16_t x_end = ((uint32_t)x + w > SIMULATOR_WIDTH) ? SIMULATOR_WIDTH : (uint16_t)(x + w);
    const uint16_t y_end = ((uint32_t)y + h > SIMULATOR_HEIGHT) ? SIMULATOR_HEIGHT : (uint16_t)(y + h);
    for (uint16_t row = y; row < y_end; row++) {
        for (uint16_t col = x; col < x_end; col++) {
            TPixel *dst = &window->pix[(row * window->w) + col];
            const uint8_t a = color.a;
            dst->r = (uint8_t)(((color.r * a) + (dst->r * (255U - a))) / 255U);
            dst->g = (uint8_t)(((color.g * a) + (dst->g * (255U - a))) / 255U);
            dst->b = (uint8_t)(((color.b * a) + (dst->b * (255U - a))) / 255U);
            dst->a = 255;
        }
    }
    return RASTER_RC_OK;
}

/*!
 * \brief No-op WS2812B transmit callback for the simulator.
 *
 * \param buffer Array of duty cycles for each LED in the strip.
 * \param length Number of duty cycles in \c buffer (length of the buffer).
 */
static enum LedsReturnCode simulator_leds_transmit(const enum WS2812BDutyCycle *buffer, uint16_t length) {
    (void)buffer;
    (void)length;
    return LEDS_RC_OK;
}

/*!
 * \brief CM4 parameters on-change hook: feeds shared parameters back to CM7.
 *
 * \details Mirrors the production CM4 main_on_parameter_change: only
 *     parameters flagged as shared end up on the IPC queue. CM4-local
 *     parameters such as PTT are still acknowledged (true) so the local
 *     side effect (LED pattern, future CAN broadcast) runs normally.
 *
 * \param parameter_id ID of the parameter that changed.
 * \param value New value of the parameter.
 *
 * \retval true if the parameter change was handled successfully (e.g. valid parameter_id)
 * \retval false if the parameter change was invalid and should be rejected
 */
static bool simulator_on_parameter_change(enum InputsSharedParameterID parameter_id, uint8_t value) {
    if (!parameters_api_is_shared(parameter_id)) {
        return true;
    }
    struct InputsSharedEvent ev = {
        .parameter_id = parameter_id,
        .value = value,
    };
    return ipc_queue_api_push_event(ev, NULL);
}

/*!
 * \brief Monotonic ms tick.
 *
 * \details CLOCK_MONOTONIC keeps ticking while the process sleeps between
 *     frames, which is what the popup timeout expects (clock() is CPU time
 *     and would freeze the timer when the process is idle).
 *
 * \returns Current tick in milliseconds.
 */
static uint32_t simulator_tick_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)((uint64_t)ts.tv_sec * 1000U + (uint64_t)ts.tv_nsec / 1000000U);
}

/*!
 * \brief Fire a one-frame button click (press + release on the same tick).
 *
 * \details Used by the toggle-on-press buttons (telemetry log, launch
 *     control). The release in the same call is harmless: parameters-api
 *     ignores it for non-paddle buttons.
 *
 * \param button_id ID of the button to click.
 * \param tick Current tick in milliseconds, used for both press and release events.
 */
static void simulator_click_button(enum InputsSharedButtonID button_id, uint32_t tick) {
    (void)inputs_api_update_button(button_id, true, tick);
    (void)inputs_api_update_button(button_id, false, tick);
}

/*!
 * \brief Bump a knob by one detent in either direction.
 *
 * \details Encoders are relative, so the simulator tracks their position locally 
 *     and sends the new absolute position to CM4 on each change. The actual value is not 
 *     important as long as it is consistent across frames and changes in the expected 
 *     direction when the keys are pressed.
 *
 * \param knob_id ID of the knob to bump.
 * \param delta Amount to change the knob position by (positive or negative).
 */
static void simulator_bump_knob(enum InputsSharedKnobID knob_id, int16_t delta) {
    simulator_knob_positions[knob_id] = (int16_t)(simulator_knob_positions[knob_id] + delta);
    (void)inputs_api_update_knob(knob_id, simulator_knob_positions[knob_id]);
}

/*!
 * \brief Reflect "key X currently held" into the matching paddle button.
 *
 * \details Edge-triggered against the previous frame so press fires once
 *     when the key first goes down and release fires once when it comes
 *     back up — same observation model the inputs module uses on real GPIO.
 *
 * \param button_id ID of the paddle button to track.
 * \param held Whether the corresponding key is currently held down.
 * \param tick Current tick in milliseconds, used for the button event.
 */
static void simulator_track_paddle(enum InputsSharedButtonID button_id, bool held, uint32_t tick) {
    if (held == simulator_paddle_held[button_id]) {
        return;
    }
    simulator_paddle_held[button_id] = held;
    (void)inputs_api_update_button(button_id, held, tick);
}

/*!
 * \brief Translate one frame of keyboard input into inputs-api calls.
 *
 * \param tick Current tick in milliseconds, used for button events and long-press tracking.
 */
static void simulator_handle_keyboard(uint32_t tick) {
    if (tigrKeyDown(window, 'Q')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, -1);
    }
    if (tigrKeyDown(window, 'W')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, +1);
    }
    if (tigrKeyDown(window, 'A')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_FRONT_RIGHT, -1);
    }
    if (tigrKeyDown(window, 'S')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_FRONT_RIGHT, +1);
    }
    if (tigrKeyDown(window, 'Z')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_SIDE_LEFT, -1);
    }
    if (tigrKeyDown(window, 'X')) {
        simulator_bump_knob(INPUTS_SHARED_KNOB_ID_SIDE_LEFT, +1);
    }
    if (tigrKeyDown(window, 'E')) {
        simulator_click_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, tick);
    }
    if (tigrKeyDown(window, 'D')) {
        simulator_click_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT, tick);
    }
    simulator_track_paddle(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT, tigrKeyHeld(window, 'C') != 0, tick);
    simulator_track_paddle(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT, tigrKeyHeld(window, 'V') != 0, tick);
}

/*!
 * \brief Run CM4's init sequence the same way CM4/post-api would on hardware.
 *
 * \retval true if all CM4 APIs initialized successfully
 * \retval false if any CM4 API failed to initialize
 */
static bool simulator_init_cm4(void) {
    if (parameters_api_init(simulator_on_parameter_change) != PARAMETERS_RC_OK) {
        fprintf(stderr, "CM4 parameters init failed\n");
        return false;
    }
    if (inputs_api_init(parameters_api_handle_button, NULL, parameters_api_handle_button_release, parameters_api_handle_knob) != INPUTS_RC_OK) {
        fprintf(stderr, "CM4 inputs init failed\n");
        return false;
    }
    if (leds_api_init(simulator_leds_transmit) != LEDS_RC_OK) {
        fprintf(stderr, "CM4 leds init failed\n");
        return false;
    }
    return true;
}

/*!
 * \brief Seed a few telemetry fields so the dashboard does not show all dashes
 *     when the window first appears.
 */
static void simulator_seed_ui_snapshot(void) {
    struct IPCUIData *ui = ipc_ui_data_api_get();
    ui->vehicle_state = IPC_UI_VEHICLE_STATE_IDLE;
    ui->soc = 80U;
    ui->hv_temp = 35;
    ui->inverter_temp = 22;
    ui->lap_current = 1U;
    ui->lap_total = 10U;
}

int main(void) {
    window = tigrWindow(SIMULATOR_WIDTH, SIMULATOR_HEIGHT, "Steering wheel simulator", 0);

    fsm_state_t state = FSM_STATE_INIT;
    struct PostInitData post_init_data = {
        .draw_rectangle = simulator_draw_rectangle,
    };
    state = fsm_run_state(state, &post_init_data);
    if (state == FSM_STATE_ERROR) {
        fprintf(stderr, "CM7 init failed\n");
        tigrFree(window);
        return 1;
    }

    if (!simulator_init_cm4()) {
        tigrFree(window);
        return 1;
    }
    simulator_seed_ui_snapshot();

    struct FsmData fsm_data;

    while (!tigrClosed(window)) {
        const uint32_t tick = simulator_tick_ms();

        simulator_handle_keyboard(tick);
        (void)inputs_api_poll_for_long_press(tick);

        ipc_queue_api_read_and_process_all(input_events_api_handle_event);

        fsm_data.tick = tick;
        state = fsm_run_state(state, &fsm_data);
        if (state == FSM_STATE_ERROR) {
            fprintf(stderr, "FSM entered ERROR state\n");
            break;
        }
        tigrUpdate(window);
    }

    tigrFree(window);
    return 0;
}
