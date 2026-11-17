/* Compatibility helpers for building against the modern <fxcg/...> headers.
 * The old disp_tools.hpp / color.h / keyboard.hpp of the original PrizmSDK are not
 * available in this SDK, so the pieces the game needs are provided here. */
#ifndef CG50_COMPAT_H
#define CG50_COMPAT_H

#include <fxcg/display.h>

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif

/* the SDK's stddef.h has no ptrdiff_t (SH is a 32-bit target) */
#define ptrdiff_t int

#define LCD_W 384
#define LCD_H 216

static inline void plot(int x, int y, color_t c) {
	if (x < 0 || y < 0 || x >= LCD_W || y >= LCD_H) return;
	((color_t*)GetVRAMAddress())[y * LCD_W + x] = c;
}

static inline void fillArea(int x, int y, int w, int h, color_t c) {
	int i, j;
	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			plot(x + i, y + j, c);
}

static inline void filledCircle(int cx, int cy, int r, color_t c) {
	int x, y;
	for (y = -r; y <= r; y++)
		for (x = -r; x <= r; x++)
			if (x * x + y * y <= r * r)
				plot(cx + x, cy + y, c);
}

#endif
