# Steering Wheel firmware

This is the firmware for the steering wheel of **Kraken**.
It works on a **STM32H747BIT6** microcontroller, which is a dual-core ARM Cortex-M7/M4 MCU, placed on a [custom PCB](https://github.com/eagletrt/steering-wheel-hw).

## Overview

The firmware is responsible for reading data from the **CAN bus** and displaying them on a **800x480 LCD** and **7 LEDs**.
It also handles **buttons** and **4 rotary encoders** for user input in real-time, allowing the driver to interact with the system while driving.

## Codebase structure

The codebase is splitted in 2 main folders:
- `CM4/`: contains the code for the Cortex-M4 core, which is responsible for reading data from the **CAN bus**, driving the **LEDs** and handling user input.
- `CM7/`: contains the code for the Cortex-M7 core, which is responsible for driving the **LCD** and displaying the data.

Each folder is a standalone project as **CubeMX** generates the code. Some shared code is placed in the `Shared/` folder.

## Development

The codebase is based on **PlatformIO** and can be build, flashed and tested using the following commands:

- First make sure you have **PlatformIO** installed:
    ```bash
    pip install platformio
    ```
- To build the firmware run this command:
    ```bash
    pio run -e cm7 -e cm4
    ```
- To flash the firmware on the device, connect it to your computer and run this command:
    ```bash
    pio run -e cm7 -e cm4 -t upload
    ```
- To run tests, run this command:
    ```bash
    pio test -e tests_common -e tests_cm4 -e tests_cm7
    ```

## Mantainers

- [Bridi Alessandro](https://github.com/bridiro)
