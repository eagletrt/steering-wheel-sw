#include "display.h"
#include "drawing-api.h"
#include "fontutils-api.h"

void draw_boot_screen() {
    const char *boot_message = "Booting...";
    int x = 400;
    int y = 220;
    struct Color color = { .argb = 0xFFFFFFFF };

    font_api_draw(x, y, FONT_ALIGN_CENTER, FONT_KONEXY, boot_message, color, 40, draw_line_callback);
}
