#include <stdio.h>
#include "micron-steer-sdram-api.h"

enum MicronSteerSdramReturnCode micron_steer_sdram_api_init(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_delay_callback delay_callback, micron_steer_sdram_api_send_command_callback send_command_callback, micron_steer_sdram_api_program_refresh_rate_callback program_refresh_rate_callback) {
    enum MicronSteerSdramReturnCode ret = MICRON_STEER_SDRAM_ERROR;

    if (ctx == NULL || delay_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    /* Step 1: Configure a clock configuration enable command */
    if (micron_steer_sdram_api_clock_enable(ctx, send_command_callback) == MICRON_STEER_SDRAM_OK) {
        /* Step 2: Insert 100 us minimum delay */
        /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
        delay_callback(1);

        /* Step 3: Configure a PALL (precharge all) command */
        if (micron_steer_sdram_api_precharge(ctx, send_command_callback) == MICRON_STEER_SDRAM_OK) {
            /* Step 4: Configure a Refresh command */
            if (micron_steer_sdram_api_refresh_mode(ctx, send_command_callback) == MICRON_STEER_SDRAM_OK) {
                /* Step 5: Program the external memory mode register */
                if (micron_steer_sdram_api_mode_reg_config(ctx, send_command_callback) == MICRON_STEER_SDRAM_OK) {
                    /* Step 6: Set the refresh rate counter */
                    if (micron_steer_sdram_api_refresh_rate(ctx, program_refresh_rate_callback) == MICRON_STEER_SDRAM_OK) {
                        ret = MICRON_STEER_SDRAM_OK;
                    }
                }
            }
        }
    }
    return ret;
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_clock_enable(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_CLK_ENABLE_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 1;
    cmd.mode_register_definition = 0;

    return send_command_callback(&cmd);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_precharge(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL || send_command_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_PALL_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 1;
    cmd.mode_register_definition = 0;

    return send_command_callback(&cmd);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_mode_reg_config(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL || send_command_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    uint32_t mode_register_definition = (uint32_t)ctx->burst_length | ctx->burst_type | ctx->cas_latency | ctx->operation_mode | ctx->write_burst_mode;

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_LOAD_MODE_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 1;
    cmd.mode_register_definition = mode_register_definition;

    return send_command_callback(&cmd);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_refresh_mode(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL || send_command_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_AUTOREFRESH_MODE_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 8;
    cmd.mode_register_definition = 0;

    return send_command_callback(&cmd);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_refresh_rate(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_program_refresh_rate_callback program_refresh_rate_func) {
    if (ctx == NULL || program_refresh_rate_func == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    return program_refresh_rate_func(ctx->refresh_rate);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_enter_power_mode(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL || send_command_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_POWERDOWN_MODE_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 1;
    cmd.mode_register_definition = 0;

    return send_command_callback(&cmd);
}

enum MicronSteerSdramReturnCode micron_steer_sdram_api_exit_power_mode(struct MicronSteerSdramContext *ctx, micron_steer_sdram_api_send_command_callback send_command_callback) {
    if (ctx == NULL || send_command_callback == NULL) {
        return MICRON_STEER_SDRAM_ERROR;
    }

    struct MicronSteerSdramCommand cmd;
    cmd.command_mode = MICRON_STEER_SDRAM_NORMAL_MODE_CMD;
    cmd.target_bank = ctx->target_bank;
    cmd.auto_refresh_number = 1;
    cmd.mode_register_definition = 0;

    return send_command_callback(&cmd);
}
