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
 * Inspired by the simple graphics library written by the folks at
 * AdaFruit this code is targeted for higher capacity microprocessors
 * such as the Cortex M series from ARM. The concept remains the same
 * however, provide a solid basic set of routines for throwing stuff up
 * on the screen.
 *
 * This code is reasonably portable, does assume 32 bit integers and
 * includes both a 7x9 as well as a 5x7 font so uses more "ROM" or flash
 * space. 
 * --Chuck McManis
 *
 */

#include <stdint.h>
#include <stdarg.h>
#include "gfx.h"

#ifdef __cplusplus
  extern "C" {
#endif
  	
extern struct gfx_font small_font;
extern struct gfx_font large_font;


/*
 * Define some helper routines, the first
 * swaps two ints using the XOR function.
 */
#define swap(a, b) { a ^= b; b ^= a; a ^= b; }

/* return the lowest of three numbers */
#define min(x, y, z) ((((x) < (y)) && ((x) < (z))) ? (x) :\
			(((y) < (z)) ? (y) : (z)))

/* return the largest of three numbers */
#define max(x, y, z) ((((x) > (y)) && ((x) > (z))) ? (x) :\
			(((y) > (z)) ? (y) : (z)))

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

/*
 * This maintains the 'state' of the graphics context
 */
static struct {
	void (*drawpixel)(int, int, uint16_t);	/* user supplied pixel writer */
	int16_t	disp_width, disp_height;	/* actual display dimensions */
	int16_t width, height;			/* dimensions based on rotation */
	struct gfx_font *font;			/* Current font in use */
	uint16_t	flags;			/* State flags for library */
	enum gfx_rotate rotation;		/* Display rotation */
	struct {
		int16_t x, y;			/* Current "cursor" X/Y location */
		uint16_t bg, fg;		/* Background and foreground colors */
		int16_t size;			/* Text "size" */
		enum gfx_rotate rotation;	/* Text rotation */
	} text;
} gfx_state;

/*
 * This function 'clips' to the current display
 * and then calls the user supplied function to
 * set the pixel.
 *
 * It also implements display rotation based on
 * parameters in the state structure.
 */
void
gfx_drawPixel(int x, int y, uint16_t color) {
	int	dx, dy;

	if ((x < 0) || (x >= gfx_state.width) ||
	    (y < 0) || (y >= gfx_state.height)) {
		return; // off screen so don't draw it
	}
	/*
	 * mirror the screen if requested
	 */
	if ((gfx_state.flags & GFX_DISPLAY_INVERT) != 0) {
			x = (gfx_state.width - 1) - x;
	}
			
	switch (gfx_state.rotation) {
		case GFX_ROT_90:
			dy = x;
			dx = (gfx_state.disp_width - 1) - y;
			break;
		case GFX_ROT_180:
			dx = (gfx_state.disp_width - 1) - x;
			dy = (gfx_state.disp_height - 1) - y;
			break;
		case GFX_ROT_270:
			dy = (gfx_state.disp_height - 1) - x;
			dx = y;
			break;
		default:
			dx = x;
			dy = y;
	}

	/* invoke user's callback in display co-ordinates */
	(gfx_state.drawpixel)(dx, dy, color);
}

/*
 * Must be called first. Sets up the graphics context and
 * sets the display size.
 */
void
gfx_init(void (*pixel_func)(int, int, uint16_t), int width, int height, int font_size)
{
	gfx_state.width     = gfx_state.disp_width = width;
	gfx_state.height    = gfx_state.disp_height = height;
	gfx_state.rotation  = GFX_ROT_0;
	gfx_state.text.y    = gfx_state.text.x = 0;
	gfx_state.text.size  = 1;
	gfx_state.text.fg = 0;
	gfx_state.text.bg = 0xFFFF;
	gfx_state.text.rotation = GFX_ROT_0;
	if (font_size == GFX_FONT_SMALL) {
		gfx_state.flags = GFX_FONT_SMALL;
		gfx_state.font = &small_font;
	} else {
		gfx_state.font = &large_font;
		gfx_state.flags = GFX_FONT_LARGE;
	}
	gfx_state.drawpixel = pixel_func;
}

/*
 * Draw one to four circle 'quadrants' using a differential.
 * Quadrants are defined as:
 *		- top left (1)
 *		- top right (2)
 *		- bottom right (3)
 *		- bottom left (4)
 * Calling it with 0xf (all four "slices" gets you a full circle)
 */
void
gfx_drawCircleHelper( int x0, int y0, int r, uint8_t slice, uint16_t color) {
	int f     = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x     = 0;
	int y     = r;

	if (slice & 0x4){	// bottom right
		gfx_drawPixel(x0     , y0+r, color);
	}
	if (slice & 0x2){	// upper right
		gfx_drawPixel(x0 + r, y0, color);
	}
	if (slice & 0x8){	// lower left
		gfx_drawPixel(x0 - r, y0  , color);
	}
	if (slice & 0x1){	// upper left
		gfx_drawPixel(x0    , y0 - r  , color);
	}

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (slice & 0x4) {
			gfx_drawPixel(x0 + x, y0 + y, color);
			gfx_drawPixel(x0 + y, y0 + x, color);
		}
		if (slice & 0x2) {
			gfx_drawPixel(x0 + x, y0 - y, color);
			gfx_drawPixel(x0 + y, y0 - x, color);
		}
		if (slice & 0x8) {
			gfx_drawPixel(x0 - y, y0 + x, color);
			gfx_drawPixel(x0 - x, y0 + y, color);
		}
		if (slice & 0x1) {
			gfx_drawPixel(x0 - y, y0 - x, color);
			gfx_drawPixel(x0 - x, y0 - y, color);
		}
	}
}

/*
 * Generate a full circle using the helper.
 */
void
gfx_drawCircle(int x0, int y0, int r, uint16_t color) {
	gfx_drawCircleHelper(x0, y0, r, 0xf, color);
}


/*
 * Draw a filled circle.
 */
void
gfx_fillCircle(int x0, int y0, int r, uint16_t color) {
	gfx_drawFastVLine(x0, y0-r, 2*r+1, color);
	gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
}

/*
 * Filled versions of a circle
 *
 * This code has a code 'delta' which "extends" the
 * right hand side of the circle, such that it could
 * be the left and right sides of a rounded rectangle
 * (used below).
 *
 * It gets away with this because it is filling from
 * the counter part slice on the other side of the drawing.
 */
void
gfx_fillCircleHelper(int x0, int y0, int r, uint8_t slice, int delta, uint16_t color) {

	int f     = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x     = 0;
	int y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (slice & 0x1) {
			gfx_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (slice & 0x2) {
			gfx_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

/*
 * Draw a line from Point x0,y0 to x1,y1 using
 * Bresenham's algorithm.
 */
void gfx_drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
	int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int err = dx / 2;
	int ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			gfx_drawPixel(y0, x0, color);
		} else {
			gfx_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

/*
 * Optimized line drawing for vertical lines.
 */
void
gfx_drawFastVLine(int x, int y, int h, uint16_t color) {
	int i;
	for (i = 0; i < h; i++) {
		gfx_drawPixel(x, y+i, color);
	}
}

/*
 * Optimized line drawing for horizontal lines.
 */
void
gfx_drawFastHLine(int x, int y, int w, uint16_t color) {
	int i;
	for (i = 0; i < w; i++) {
		gfx_drawPixel(x + i, y, color);
	}
}

/*
 * Draw a basic rectangle
 */
void
gfx_drawRect(int x, int y, int w, int h, uint16_t color) {
	gfx_drawFastHLine(x        , y    , w, color);
	gfx_drawFastHLine(x        , y+h-1, w, color);
	gfx_drawFastVLine(x        , y    , h, color);
	gfx_drawFastVLine(x + w - 1, y    , h, color);
}

/*
 * Draw a filled rectangle.
 */
void
gfx_fillRect(int x, int y, int w, int h, uint16_t color) {
	int i;

	for (i = x; i < x + w; i++) {
		gfx_drawFastVLine(i, y, h, color);
	}
}

/*
 * Clear the screen by writing a rectangle the size of
 * the screen.
 */
void
gfx_fillScreen(uint16_t color) {
	gfx_fillRect(0, 0, gfx_state.width, gfx_state.height, color);
}

/*
 * Draw a rectangle with Rounded corners.
 *	This takes advantage of our 'quadrant' drawing code in circles
 *	to put rounded corners on a rectangle.
 */
void
gfx_drawRoundRect(int x, int y, int w, int h, int r, uint16_t color) {

	gfx_drawFastHLine(x+r  , y    , w-2*r, color); // Top
	gfx_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	gfx_drawFastVLine(x    , y+r  , h-2*r, color); // Left
	gfx_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right

	// draw four corners
	gfx_drawCircleHelper(x+r    , y+r    , r, 1, color);
	gfx_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	gfx_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	gfx_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

/*
 * Draw a filled rectangle with rounded corners
 */
void
gfx_fillRoundRect(int x, int y, int w, int h, int r, uint16_t color) {

	/* this part is just a rectangle */
	gfx_fillRect(x+r, y, w-2*r, h, color);

	/* this part uses the corner helper to fill in the top an bottom */
	gfx_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	gfx_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

/*
 * Draw a triangle.
 */
void
gfx_drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
	gfx_drawLine(x0, y0, x1, y1, color);
	gfx_drawLine(x1, y1, x2, y2, color);
	gfx_drawLine(x2, y2, x0, y0, color);
}

/*
 * Draw a filled triangle.
 *
 * This is the code from devmaster.net/posts/6145/advanced-rasterization
 *
 * It has 33 local ints (so 132 bytes of additional stack space) and
 * a couple dozen instructions. It is pretty fast!
 */
void
gfx_fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
	int x, y;
	int cross;

	/*
	 * This algorithm depends on the vertices being in CCW order. This
 	 * is also known as their "winding". We can compute if they are in
	 * order by computing the "Z" component of a cross product of two
 	 * vectors from a common point (assume z is 0). If it is postive
	 * they are, if not you need to swap the order of the second and
	 * third vertex.
	 * NB: This can overflow if you're using a "big" space (larger
 	 *     than 64K x 64K points.
	 */
	cross = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - y0);
	if (cross == 0) {
		// they are co-linear so just draw a line
		gfx_drawLine(min(x0, x1, x2), min(y0, y1, y2),
					 max(x0, x1, x2), max(y0, y1, y2), color);
		return;
	}

	/* 28.4 fixed point */
	const int Y1 = y0 << 4;
	const int Y2 = ((cross < 0) ? y1 : y2) << 4;
	const int Y3 = ((cross < 0) ? y2 : y1) << 4;

	const int X1 = x0 << 4;
	const int X2 = ((cross < 0) ? x1 : x2) << 4;
	const int X3 = ((cross < 0) ? x2 : x1) << 4;

	/* Deltas */
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	/* Fixed point Deltas */
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	/* Bounding rectangle */
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	/* Half-edge constants */
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	/* Correct for fill convention */
	if ((DY12 < 0) || ((DY12 == 0) && (DX12 > 0))) {
		C1++;
	}
	if ((DY23 < 0) || ((DY23 == 0) && (DX23 > 0))) {
		C2++;
	}
	if ((DY31 < 0) || ((DY31 == 0) && (DX31 > 0))) {
		C3++;
	}

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	for (y = miny; y < maxy; y++) {
		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;
		for (x = minx; x < maxx; x++) {
			if ((CX1 > 0) && (CX2 > 0) && (CX3 > 0)) {
				gfx_drawPixel(x, y, color);
			}
			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
		}
		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;
	}
}

/*
 * This code puts a text glyph onto the screen.
 */
void
gfx_drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, uint16_t color) {
	int i, j, byteWidth = (w + 7) / 8;

	for(j=0; j<h; j++) {
		for(i=0; i<w; i++ ) {
			if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				gfx_drawPixel(x+i, y+j, color);
			}
		}
  	}
}

/*
 * gfx_drawChar() does the heavy lifting of putting a font glyph associated
 * with an ASCII value, as the base level function it takes all of the
 * arguments in the call including foreground and background color,
 * size, and character rotation. The only parameter not passed is
 * the font to use, that comes from the internal state and can be
 * queried or set with gfx_{get|set}Font() call.
 */
void
gfx_drawChar(int x, int y, unsigned char c, uint16_t fg, uint16_t bg, int size, enum gfx_rotate r) {
	const uint8_t	*glyph;
	int i, k;
	uint8_t descender, bit;
	int dx, dy;

	if ((int) c  >  gfx_state.font->chars) {
		return; // no glyph for this character.
	}

	glyph = gfx_state.font->raw + (gfx_state.font->size * (unsigned int) c);
	descender = ((*glyph & 0x80) != 0);
	for (k = 0; k < gfx_state.font->size+1; k++) {
		for (i = 0; i < 7; i++) {
			bit = ((*glyph & (0x40 >> i)) != 0);
			dx = ((gfx_state.font->byrow) ? (i) : (k));
			dy = (((gfx_state.font->byrow) ? (k) : (i)) + (descender * 3)) - gfx_state.font->baseline;
			switch (r) {
				case GFX_ROT_90:
					dy = -dy;
					swap(dx, dy);
					break;
				case GFX_ROT_180:
					dy = -dy;
					dx = -dx;
					break;
				case GFX_ROT_270:
					dx = -dx;
					swap(dx, dy);
					break;
				default:
					break;
			}
			dx *= size;
			dy *= size;
			if (bit && k < gfx_state.font->size) {
				if (size > 1) {
					gfx_fillRect(x + dx, y + dy, size, size, fg);
				} else {
					gfx_drawPixel(x + dx, y + dy, fg);
				}
			} else if (bg != fg) {
				if (size > 1) {
					gfx_fillRect(x + dx, y + dy, size, size, bg);
				} else {
					gfx_drawPixel(x + dx, y + dy, bg);
				}
			}
		}
		glyph++;
	}
}

/*
 * gfx_write() is somewhere between drawChar and a putc, it uses the internal
 * state to write the character to the display, it updates the x, y position
 * such that if it is called again it will put the next character in the correct
 * place.
 */
int
gfx_write(unsigned char c) {
	int	nx, ny;
	int dx, dy;

	nx = gfx_state.text.x;
	ny = gfx_state.text.y;
	if (c == '\n')
    {
        ny += gfx_state.font->width * gfx_state.text.size+2;
        nx = 0;
    }
    else if (c == '\r')
        nx = 0;
    else
    {
	if (((nx < gfx_state.width) && (nx >= 0)) &&
	    ((ny < gfx_state.height) && (ny >= 0))) {
		gfx_drawChar(nx, ny, c, gfx_state.text.fg, gfx_state.text.bg,
				gfx_state.text.size, gfx_state.text.rotation);
	}
	dx = gfx_state.font->width * gfx_state.text.size;
	dy = gfx_state.font->height * gfx_state.text.size;
	switch (gfx_state.text.rotation) {
		case GFX_ROT_90:
			ny += dx;
			dx = -dx;
			break;
		case GFX_ROT_180:
			nx -= dx;
			dy = -dy;
			break;
		case GFX_ROT_270:
			ny -= dx;
			break;
		default:
			nx += dx;
			break;
	}
}
	gfx_state.text.x = nx;
	gfx_state.text.y = ny;
}

void
gfx_putc(char c) {
	gfx_write(c);
}

/*
 * Print a string to the display.
 */
void
gfx_puts(const char* s) {
	int x, y;
	x = gfx_state.text.x;
	y = gfx_state.text.y;
	while (*s) {
		gfx_write(*s);
		if ((gfx_state.flags & GFX_TEXT_WRAP) != 0) {
			switch (gfx_state.text.rotation) {
				case GFX_ROT_0:
					if (gfx_state.text.x >= gfx_state.width) {
						gfx_state.text.x = x;
						gfx_state.text.y += gfx_state.font->height;
					}
					break;
				case GFX_ROT_180:
					if (gfx_state.text.x < 0) {
						gfx_state.text.x = x;
						gfx_state.text.y -= gfx_state.font->height;
					}
					break;
				case GFX_ROT_90:
					if (gfx_state.text.y >= gfx_state.height) {
						gfx_state.text.y = y;
						gfx_state.text.x -= gfx_state.font->height;
					}
					break;
				case GFX_ROT_270:
					if (gfx_state.text.y < 0) {
						gfx_state.text.y = y;
						gfx_state.text.x += gfx_state.font->height;
					}
					break;
			}
		}
		s++;
	}
}

void gfx_printf(char *fmt, ...)
{
	char str[128];
    va_list va;
    va_start(va,fmt);
    int len=vsnprintf(str,128,fmt,va);
    int i;
    for(i=0;i<len;i++) {
    	gfx_putc(str[i]);
    }
    va_end(va);
}

void
gfx_setCursor(int x, int y) {
	gfx_state.text.x = x;
	gfx_state.text.y = y;
}

void
gfx_setTextSize(int s) {
	gfx_state.text.size = (s > 0) ? s : 1;
}

int
gfx_getTextHeight(void) {
	return gfx_state.font->height;
}

int
gfx_getTextWidth(void) {
	return gfx_state.font->width;
}

/*
 * Set the foreground and background colors for the text.
 * If set to the same value then only the foreground color
 * is written (psuedo tranparent background)
 */
void
gfx_setTextColor(uint16_t fg, uint16_t bg) {
	gfx_state.text.fg = fg;
	gfx_state.text.bg = bg;
}

void
gfx_setTextWrap(int w) {
	if (w) {
		gfx_state.flags |= GFX_TEXT_WRAP;
	} else {
		gfx_state.flags &= ~GFX_TEXT_WRAP;
	}
}

/*
 * Set the text rotation. This is independent of the
 * screen rotation and supports four values
 *	GFX_ROT_0	: text flows left to right horizontally
 *	GFX_ROT_90	: text prints downward with the letter 'top'
 *			  pointing toward the right of the screen.
 *	GFX_ROT_180	: text prints right to left and upside
 *			  down.
 *		GFX_ROT_270 : text prints vertically going up with
 *					  character tops pointing leftward.
 */
void
gfx_setTextRotation(enum gfx_rotate r) {
	gfx_state.text.rotation = r;
}

enum gfx_rotate
gfx_getTextRotation(void) {
	return (gfx_state.text.rotation);
}

/*
 * Set the font to either GFX_FONT_SMALL
 * or GFX_FONT_LARGE
 */
void
gfx_setFont(int font) {
	if (font == GFX_FONT_SMALL) {
		gfx_state.font = &small_font;
		gfx_state.flags &= ~GFX_FONT_LARGE;
		gfx_state.flags |= GFX_FONT_SMALL;
	} else {
		gfx_state.font = &large_font;
		gfx_state.flags &= ~GFX_FONT_SMALL;
		gfx_state.flags |= GFX_FONT_LARGE;
	}
}

int
gfx_getFont(void) {
	if (gfx_state.flags & GFX_FONT_SMALL) {
		return GFX_FONT_SMALL;
	} else {
		return GFX_FONT_LARGE;
	}
}

int
gfx_textWidth(void) {
	return gfx_state.font->width;
}

int
gfx_textHeight(void) {
	return gfx_state.font->height;
}

void
gfx_setMirrored(int m) {
	if (m) {
		gfx_state.flags |= GFX_DISPLAY_INVERT;
	} else {
		gfx_state.flags &= ~GFX_DISPLAY_INVERT;
	}
}

int
gfx_getMirrored(void) {
	return ((gfx_state.flags & GFX_DISPLAY_INVERT) != 0);
}

enum gfx_rotate
gfx_getRotation(void) {
  return gfx_state.rotation;
}

/* implement simple rotations
 */
void
gfx_setRotation(enum gfx_rotate r) {
	gfx_state.rotation = r;
	switch(gfx_state.rotation) {
		case GFX_ROT_0:
		case GFX_ROT_180:
			gfx_state.width  = gfx_state.disp_width;
			gfx_state.height = gfx_state.disp_height;
			break;
		case GFX_ROT_90:
		case GFX_ROT_270:
			gfx_state.width  = gfx_state.disp_height;
			gfx_state.height = gfx_state.disp_width;
			break;
	}
}

/*
 * Return display width
 */
int
gfx_width(void) {
	return gfx_state.width;
}

int
gfx_height(void) {
	return gfx_state.height;
}

#ifdef __cplusplus
  }
#endif