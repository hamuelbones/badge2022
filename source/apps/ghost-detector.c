/*********************************************

Ghost Detector. This is a very silly program, but it works perfectly.
It never detects any ghosts, and there's no such thing as ghosts.
 
Author: Stephen M. Cameron <stephenmcameron@gmail.com>

**********************************************/
#include <string.h>
#include "colors.h"
#include "menu.h"
#include "button.h"
#include "trig.h"
#include "framebuffer.h"

static int radar_angle = 0;

/* Program states.  Initial state is GHOSTDETECTOR_INIT */
enum ghostdetector_state_t {
	GHOSTDETECTOR_INIT,
	GHOSTDETECTOR_RUN,
	GHOSTDETECTOR_EXIT,
};

static enum ghostdetector_state_t ghostdetector_state = GHOSTDETECTOR_INIT;

static void ghostdetector_init(void)
{
	FbClear();
	ghostdetector_state = GHOSTDETECTOR_RUN;
}

static void check_buttons()
{
    int down_latches = button_down_latches();
	if (down_latches & (1<<BADGE_BUTTON_SW)) {
		/* Pressing the button exits the program. You probably want to change this. */
		ghostdetector_state = GHOSTDETECTOR_EXIT;
	} else if (down_latches & (1<<BADGE_BUTTON_LEFT)) {
	} else if (down_latches & (1<<BADGE_BUTTON_RIGHT)) {
	} else if (down_latches & (1<<BADGE_BUTTON_UP)) {
	} else if (down_latches & (1<<BADGE_BUTTON_DOWN)) {
	}
}

static void draw_radar_beam(int color)
{
	const int cx = 64;
	const int cy = 64;
	int ex, ey;
	ex = cx + (cosine(radar_angle) * 64) / 256;
	ey = cy + (sine(radar_angle) * 64) / 256;
	FbColor(color);
	FbLine(cx, cy, ex, ey);
}

static void draw_reticle_line(int distance, int width)
{
	const int cx = 64;
	const int cy = 64;
	FbHorizontalLine(cx - width / 2, cy - distance, cx + width / 2, cy - distance);
	FbHorizontalLine(cx - width / 2, cy + distance, cx + width / 2, cy + distance);
	FbVerticalLine(cx - distance, cy - width / 2, cx - distance, cy + width / 2);
	FbVerticalLine(cx + distance, cy - width / 2, cx + distance, cy + width / 2);
}

static void draw_reticle(int color)
{
	FbColor(color);
	FbVerticalLine(64, 1, 64, 127);
	FbHorizontalLine(1, 64, 127, 64);
	draw_reticle_line(63, 16);
	draw_reticle_line(43, 8);
	draw_reticle_line(23, 8);
}

static void draw_screen()
{
	draw_reticle(RED);
	draw_radar_beam(BLACK);
	radar_angle = (radar_angle + 1) % 128;
	draw_radar_beam(WHITE);
	FbSwapBuffers();
}

static void ghostdetector_run()
{
	check_buttons();
	draw_screen();
}

static void ghostdetector_exit()
{
	ghostdetector_state = GHOSTDETECTOR_INIT; /* So that when we start again, we do not immediately exit */
}

/* You will need to rename ghostdetector_cb() something else. */
int ghostdetector_cb(void)
{
	switch (ghostdetector_state) {
	case GHOSTDETECTOR_INIT:
		ghostdetector_init();
		break;
	case GHOSTDETECTOR_RUN:
		ghostdetector_run();
		break;
	case GHOSTDETECTOR_EXIT:
		ghostdetector_exit();
        return 1;
	default:
		break;
	}
	return 0;
}

#ifdef __linux__
int main(int argc, char *argv[])
{
        start_gtk(&argc, &argv, ghostdetector_cb, 30);
        return 0;
}
#endif
