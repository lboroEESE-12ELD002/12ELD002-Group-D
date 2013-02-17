// *************************************************************************************************
#ifndef COUNTDOWN_H_
#define COUNTDOWN_H_

// *************************************************************************************************
// Include section
#include <project.h>


// ************************************************************************************************
// Extern section
extern void start_countdown(void);
extern void stop_countdown(void);
extern void reset_countdown(void);
extern void reaches_zero(void);

extern void mx_countdown(u8 line);
extern void sx_countdown(u8 line);

extern void display_countdown(u8 line, u8 update);
extern u8 is_countdown_active(void);
extern void countdown_tick(void);
extern void elapsed_tick(void);
extern void test_axis(void);

// ************************************************************************************************
// Defines section
#define COUNTDOWN_STD_TICK                      (32768 / 1)
#define COUNTDOWN_MODE_OFF		(0u)
#define COUNTDOWN_MODE_ON		(1u)
#define ELAPSED_MODE_OFF		(0u)
#define ELAPSED_MODE_ON			(1u)

// ************************************************************************************************
// Global Variable section

enum flag {false, true};

struct countdown
{
	u8 mode;				// COUNTDOWN_MODE_OFF, COUNTDOWN_MODE_ON
	u8 mode2;				// ELAPSED_MODE_OFF, ELAPSED_MODE_ON
    u8 minute;              // Remaining minutes
    u8 second;              // Remaining seconds
    u8 secondflow;          // Remaining seconds overflow test
    u8 elapsecond;			// Elapsed time seconds
    u8 elapminute;			// Elapsed time minutes
};
extern struct countdown sCountdown;

#endif /*COUNTDOWN_H_*/
