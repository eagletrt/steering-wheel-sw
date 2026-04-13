#ifndef IPC_UI_DATA_H
#define IPC_UI_DATA_H

#include <stdint.h>

struct IPCUIData {
    volatile uint32_t soc;
    volatile uint32_t hv_temp;
    volatile uint32_t motor_fl_temp;
    volatile uint32_t motor_fr_temp;
    volatile uint32_t motor_rl_temp;
    volatile uint32_t motor_rr_temp;
    // ... more fields as needed
};

#endif // IPC_UI_DATA_H
