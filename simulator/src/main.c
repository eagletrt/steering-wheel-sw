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
#include "can-communications-router-api.h"

#include "inputs-api.h"
#include "leds-api.h"
#include "parameters-api.h"

#include "inputs-shared.h"
#include "ipc-queue-api.h"
#include "ipc-ui-data-api.h"

#include "cm4-fsm.h"
#include "simulator-leds.h"

#include "tigr.h"
#include "eagletrt-api.h"

#define SIMULATOR_DASHBOARD_WIDTH (800)
#define SIMULATOR_DASHBOARD_HEIGHT (480)
#define SIMULATOR_LED_STRIP_HEIGHT (60)
#define SIMULATOR_WIDTH (SIMULATOR_DASHBOARD_WIDTH)
#define SIMULATOR_HEIGHT (SIMULATOR_DASHBOARD_HEIGHT + SIMULATOR_LED_STRIP_HEIGHT)

EAGLETRT_STATIC Tigr *window;

EAGLETRT_STATIC int16_t simulator_knob_positions[INPUTS_SHARED_KNOB_ID_COUNT];
EAGLETRT_STATIC bool simulator_paddle_held[INPUTS_SHARED_BUTTON_ID_COUNT];

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
EAGLETRT_STATIC enum RasterReturnCode simulator_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, struct Color color) {
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
 * \brief WS2812B "transmit" for the simulator.
 *
 * \param buffer Array of duty cycles for each LED in the strip.
 * \param length Number of duty cycles in \c buffer (length of the buffer).
 *
 * \retval LEDS_RC_OK always, since the simulator does not have a real LED strip to fail on.
 */
EAGLETRT_STATIC enum LedsReturnCode simulator_leds_transmit(const enum WS2812BDutyCycle *buffer, uint16_t length) {
    simulator_leds_decode(buffer, length);
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
EAGLETRT_STATIC bool simulator_on_parameter_change(enum InputsSharedParameterID parameter_id, uint8_t value) {
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
EAGLETRT_STATIC uint32_t simulator_tick_ms(void) {
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
EAGLETRT_STATIC void simulator_click_button(enum InputsSharedButtonID button_id, uint32_t tick) {
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
EAGLETRT_STATIC void simulator_bump_knob(enum InputsSharedKnobID knob_id, int16_t delta) {
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
EAGLETRT_STATIC void simulator_track_paddle(enum InputsSharedButtonID button_id, bool held, uint32_t tick) {
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
EAGLETRT_STATIC void simulator_handle_keyboard(uint32_t tick) {
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
 * \brief Position and identity of each LED in the simulator strip.
 *
 * \details Laid out left-to-right exactly the way the LEDs sit on the
 *     steering wheel: outer-left pair, the five centre LEDs, the outer-right
 *     pair. The x coordinates fit the 800px-wide window with comfortable
 *     spacing; the y coordinate lives in the strip below the dashboard.
 */
struct SimulatorLedSlot {
    enum LedsIndex index;
    uint16_t x;
};

EAGLETRT_STATIC const struct SimulatorLedSlot simulator_led_slots[LEDS_INDEX_COUNT] = {
    { LEDS_INDEX_TOP_LEFT_1, 60U },
    { LEDS_INDEX_TOP_LEFT_0, 160U },
    { LEDS_INDEX_CENTER_0, 260U },
    { LEDS_INDEX_CENTER_1, 330U },
    { LEDS_INDEX_CENTER_2, 400U },
    { LEDS_INDEX_CENTER_3, 470U },
    { LEDS_INDEX_CENTER_4, 540U },
    { LEDS_INDEX_TOP_RIGHT_0, 640U },
    { LEDS_INDEX_TOP_RIGHT_1, 740U },
};

#define SIMULATOR_LED_RADIUS (18)
#define SIMULATOR_LED_CENTER_Y (SIMULATOR_DASHBOARD_HEIGHT + (SIMULATOR_LED_STRIP_HEIGHT / 2))
#define SIMULATOR_LED_STRIP_COLOR_R (10)
#define SIMULATOR_LED_STRIP_COLOR_G (10)
#define SIMULATOR_LED_STRIP_COLOR_B (10)

/*!
 * \brief Paint a uniform dark background under the LED strip.
 */
EAGLETRT_STATIC void simulator_clear_led_strip(void) {
    const TPixel bg = {
        .r = SIMULATOR_LED_STRIP_COLOR_R,
        .g = SIMULATOR_LED_STRIP_COLOR_G,
        .b = SIMULATOR_LED_STRIP_COLOR_B,
        .a = 255,
    };
    for (int y = SIMULATOR_DASHBOARD_HEIGHT; y < SIMULATOR_HEIGHT; y++) {
        for (int x = 0; x < SIMULATOR_WIDTH; x++) {
            window->pix[(y * window->w) + x] = bg;
        }
    }
}

/*!
 * \brief Render each LED as a filled circle.
 */
EAGLETRT_STATIC void simulator_draw_led_strip(void) {
    simulator_clear_led_strip();

    const struct LedColor *colors = simulator_leds_get_colors();

    for (size_t i = 0; i < (sizeof(simulator_led_slots) / sizeof(simulator_led_slots[0])); i++) {
        const struct SimulatorLedSlot slot = simulator_led_slots[i];
        const struct LedColor color = colors[slot.index];
        const TPixel pixel = {
            .r = color.r,
            .g = color.g,
            .b = color.b,
            .a = 255,
        };
        const int center_x = (int)slot.x;
        const int center_y = SIMULATOR_LED_CENTER_Y;
        for (int dy = -SIMULATOR_LED_RADIUS; dy <= SIMULATOR_LED_RADIUS; dy++) {
            for (int dx = -SIMULATOR_LED_RADIUS; dx <= SIMULATOR_LED_RADIUS; dx++) {
                if ((dx * dx + dy * dy) > (SIMULATOR_LED_RADIUS * SIMULATOR_LED_RADIUS)) {
                    continue;
                }
                const int px = center_x + dx;
                const int py = center_y + dy;
                if (px < 0 || px >= window->w || py < 0 || py >= window->h) {
                    continue;
                }
                window->pix[(py * window->w) + px] = pixel;
            }
        }
    }
}

/*!
 * \brief Seed a few telemetry fields so the dashboard does not show all dashes
 *     when the window first appears.
 */
EAGLETRT_STATIC void simulator_seed_ui_snapshot(void) {
    struct IPCUIData *ui = ipc_ui_data_api_get();
    ui->vehicle_state = IPC_UI_VEHICLE_STATE_IDLE;
    ui->soc = 80U;
    ui->hv_temp = 35;
    ui->inverter_temp = 22;
    ui->lap_current = 1U;
    ui->lap_total = 10U;
}

enum CanCommunicationReturnCode simulator_can_send_primary(const struct CanCommunicationFrame *frame) {
    EAGLETRT_API_UNUSED(frame);
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode simulator_can_send_secondary(const struct CanCommunicationFrame *frame) {
    EAGLETRT_API_UNUSED(frame);
    return CAN_COMMUNICATION_RC_OK;
}

int main(void) {
    window = tigrWindow(SIMULATOR_WIDTH, SIMULATOR_HEIGHT, "Steering wheel simulator", 0);

    fsm_state_t cm7_state = FSM_STATE_INIT;
    struct PostInitData cm7_post = {
        .draw_rectangle = simulator_draw_rectangle,
    };
    cm7_state = fsm_run_state(cm7_state, &cm7_post);
    if (cm7_state == FSM_STATE_ERROR) {
        fprintf(stderr, "CM7 init failed\n");
        tigrFree(window);
        return 1;
    }

    fsm_state_t cm4_state = FSM_STATE_INIT;
    struct CM4PostInitData cm4_post = {
        .leds_transmit = simulator_leds_transmit,
        .parameters_on_change = simulator_on_parameter_change,
        .can_network_configs = {
            [CAN_COMMUNICATION_NETWORK_PRIMARY] = {
                .cs_enter = NULL,
                .cs_exit = NULL,
                .on_receive = can_communications_router_api_receive_primary,
                .send = simulator_can_send_primary,
            },
            [CAN_COMMUNICATION_NETWORK_SECONDARY] = {
                .cs_enter = NULL,
                .cs_exit = NULL,
                .on_receive = can_communications_router_api_receive_secondary,
                .send = simulator_can_send_secondary,
            },
        },
    };
    cm4_state = cm4_fsm_run_state(cm4_state, &cm4_post);
    if (cm4_state == FSM_STATE_ERROR) {
        fprintf(stderr, "CM4 init failed\n");
        tigrFree(window);
        return 1;
    }

    simulator_seed_ui_snapshot();

    struct FsmData fsm_data;

    while (!tigrClosed(window)) {
        const uint32_t tick = simulator_tick_ms();
        fsm_data.tick = tick;

        simulator_handle_keyboard(tick);

        /* CM4 idle: poll long-press + push LED buffer (no-op transmit). */
        cm4_state = cm4_fsm_run_state(cm4_state, &fsm_data);
        if (cm4_state == FSM_STATE_ERROR) {
            fprintf(stderr, "CM4 FSM entered ERROR state\n");
            break;
        }

        ipc_queue_api_read_and_process_all(input_events_api_handle_event);

        cm7_state = fsm_run_state(cm7_state, &fsm_data);
        if (cm7_state == FSM_STATE_ERROR) {
            fprintf(stderr, "CM7 FSM entered ERROR state\n");
            break;
        }

        simulator_draw_led_strip();
        tigrUpdate(window);
    }

    tigrFree(window);
    return 0;
}
