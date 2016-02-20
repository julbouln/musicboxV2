/* 
 * Simple Graphics
 *
 * Copyright (c) 2014-2015 Charles McManis, all rights reserved.
 *
 * This source code is licensed under a Creative Commons 4.0 
 * International Public license. 
 *
 * See: http://creativecommons.org/licenses/by/4.0/legalcode for
 * details.
 * 
 * Basic prototype definitions and flag #defines for the simple
 * graphics routines.
 *
 */
#ifndef _SIMPLE_GFX_H
#define _SIMPLE_GFX_H
#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif
      
#define GFX_FONT_LARGE		1
#define GFX_FONT_SMALL		2
#define GFX_TEXT_WRAP		4
#define GFX_DISPLAY_INVERT	8


enum gfx_rotate { GFX_ROT_0, GFX_ROT_90, GFX_ROT_180, GFX_ROT_270 };

void gfx_drawPixel(int x, int y, uint16_t color);
void gfx_drawLine(int x0, int y0, int x1, int y1, uint16_t color);
void gfx_drawFastVLine(int x, int y, int h, uint16_t color);
void gfx_drawFastHLine(int x, int y, int w, uint16_t color);
void gfx_drawRect(int x, int y, int w, int h, uint16_t color);
void gfx_fillRect(int x, int y, int w, int h, uint16_t color);
void gfx_fillScreen(uint16_t color);

void gfx_drawCircle(int x0, int y0, int r, uint16_t color);
void gfx_drawCircleHelper(int x0, int y0, int r, uint8_t cornername,
      uint16_t color);
void gfx_fillCircle(int x0, int y0, int r, uint16_t color);
void gfx_init(void (*draw)(int, int, uint16_t), int, int, int font_size);

void gfx_fillCircleHelper(int x0, int y0, int r, uint8_t cornername,
      int delta, uint16_t color);
void gfx_drawTriangle(int x0, int y0, int x1, int y1,
      int x2, int y2, uint16_t color);
void gfx_fillTriangle(int x0, int y0, int x1, int y1,
      int x2, int y2, uint16_t color);
void gfx_drawRoundRect(int x0, int y0, int w, int h,
      int radius, uint16_t color);
void gfx_fillRoundRect(int x0, int y0, int w, int h,
      int radius, uint16_t color);
void gfx_drawBitmap(int x, int y, const uint8_t *bitmap,
      int w, int h, uint16_t color);
void gfx_drawChar(int x, int y, unsigned char c, uint16_t fg,
      uint16_t bg, int size, enum gfx_rotate rotation);
void gfx_setTextRotate(enum gfx_rotate r);
enum gfx_rotate gfx_getTextRotate(void);
void gfx_setCursor(int x, int y);
void gfx_setTextColor(uint16_t c, uint16_t bg);
void gfx_setTextSize(int s);
void gfx_setTextWrap(int w);
int	gfx_getTextWidth(void);
int gfx_getTextHeight(void);
void gfx_setRotation(enum gfx_rotate r);
void gfx_setMirrored(int m);
int gfx_getMirrored(void);
void gfx_puts(const char*);
void gfx_printf(char *fmt, ...);
void gfx_putc(char);
int gfx_write(unsigned char );

int gfx_height(void);
int gfx_width(void);
enum gfx_rotate gfx_getTextRotation(void);
void gfx_setTextRotation(enum gfx_rotate);

int gfx_getFont(void);
void gfx_setFont(int font);
enum gfx_rotate gfx_getRotation(void);

/*
 * ASCII Font structure
 */
struct gfx_font {
	const uint8_t	*raw;	/* raw font data */
	int16_t			chars;	/* number of characters in font */
	int16_t			size;	/* bytes per glyph */
	uint8_t			byrow;	/* row oriented or column oriented flag */
	uint8_t			width;	/* spacing between characters. */
	uint8_t			height;	/* spacing between rows of characters. */
	uint8_t			baseline;	/* # of lines above 'y' to the top of glyph */
};

#define GFX_COLOR_WHITE          0xFFFF
#define GFX_COLOR_BLACK          0x0000
#define GFX_COLOR_GREY           0xF7DE
#define GFX_COLOR_BLUE           0x001F
#define GFX_COLOR_BLUE2          0x051F
#define GFX_COLOR_RED            0xF800
#define GFX_COLOR_MAGENTA        0xF81F
#define GFX_COLOR_GREEN          0x07E0
#define GFX_COLOR_CYAN           0x7FFF
#define GFX_COLOR_YELLOW         0xFFE0

#ifdef __cplusplus
  }
#endif

#endif  /* SIMPLE_GFX */
