#ifndef _SSD1306_H
#define _SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

#define OLED_BLACK 0
#define OLED_WHITE 1

void ssd1306_setup();
void ssd1306_drawpixel(int x, int y, uint16_t color);
void ssd1306_display(void);

#ifdef __cplusplus
}
#endif

#endif
