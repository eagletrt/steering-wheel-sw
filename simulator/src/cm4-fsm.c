/*!
 * \file cm4-fsm.c
 * \date 2026-06-10
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Compile CM4's fsm.c and post-api.c with prefixed symbols.
 */

#define fsm_state_names cm4_fsm_state_names
#define fsm_state_table cm4_fsm_state_table
#define fsm_transition_table cm4_fsm_transition_table
#define fsm_fired_event cm4_fsm_fired_event
#define fsm_is_event_triggered cm4_fsm_is_event_triggered
#define fsm_event_trigger cm4_fsm_event_trigger
#define fsm_do_init cm4_fsm_do_init
#define fsm_do_idle cm4_fsm_do_idle
#define fsm_do_error cm4_fsm_do_error
#define fsm_do_flash cm4_fsm_do_flash
#define fsm_do_ts_on cm4_fsm_do_ts_on
#define fsm_do_autonomous cm4_fsm_do_autonomous
#define fsm_do_drive cm4_fsm_do_drive
#define fsm_init_done cm4_fsm_init_done
#define fsm_init_error cm4_fsm_init_error
#define fsm_start_flash cm4_fsm_start_flash
#define fsm_error_detected cm4_fsm_error_detected
#define fsm_ts_on_request cm4_fsm_ts_on_request
#define fsm_autonomous_enable cm4_fsm_autonomous_enable
#define fsm_flash_done cm4_fsm_flash_done
#define fsm_ts_cancel cm4_fsm_ts_cancel
#define fsm_ts_on_accepted cm4_fsm_ts_on_accepted
#define fsm_ts_off cm4_fsm_ts_off
#define fsm_autonomous_disable cm4_fsm_autonomous_disable
#define fsm_run_state cm4_fsm_run_state

#define post_api_do_init cm4_post_api_do_init
#define PostReturnCode CM4PostReturnCode
#define PostInitData CM4PostInitData
#define POST_RC_OK CM4_POST_RC_OK
#define POST_RC_ERROR CM4_POST_RC_ERROR

#include "../../CM4/Core/Inc/steering/fsm.h"
#include "../../CM4/Core/Inc/steering/post.h"
#include "../../CM4/Core/Inc/steering/post-api.h"

#include "../../CM4/Core/Src/steering/fsm.c"
#include "../../CM4/Core/Src/steering/post-api.c"
