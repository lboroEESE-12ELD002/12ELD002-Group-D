// *************************************************************************************************
// Description: Module to create functionality of countdown timer
// Group: ELD002 Direct Current
// Name: Ed Culpin
// Date: 07/12/2012
// Revision: 2.3
//
// Notes:
// - Values can be edited as the user wishes in the section immediately following this
// - Functionality of each function is explained in their respective headers
// - 'countdown_tick' and 'elapsed_tick' are called from timer.c line 385
//
// *************************************************************************************************
// User value to configure countdown

// timings
#define TILT_RETURN_DELAY 5			// The time in seconds before speed returns to 1Hz, after accelerometers drop below thresholds
#define DELAY_2HZ_TO_4HZ 5			// The time in seconds before speed increases to 4Hz, after increasing to 2Hz
#define DELAY_4HZ_TO_ZERO 5			// The time in seconds before countdown drops to zero, after increasing to 4Hz

// thresholds
#define X_THRESHOLD 20				// The x-axis angle, in degrees from horizontal, that causes the speed to increase
#define Y_THRESHOLD	20				// The y-axis angle, in degrees from horizontal, that causes the speed to increase

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "countdown.h"
#include "acceleration.h"
#include "buzzer.h"
#include "ports.h"
#include "vti_as.h"
#include "display.h"
#include "timer.h"

// logic
#include "simpliciti.h"
#include "user.h"
#include "string.h"
#include "menu.h"

// ******************************************
// Prototypes section
void start_countdown(void);
void stop_countdown(void);
void reset_countdown(void);
void reaches_zero(void);
void mx_countdown(u8 line);
void sx_countdown(u8 line);
void display_countdown(u8 line, u8 update);
u8 is_countdown_active(void);
void countdown_tick(void);
void elapsed_tick(void);
void test_axis(void);

// ******************************************
// Global Variable section
struct countdown sCountdown;
int heartcount, tiltreturn, tilttozero, finishedcounter, sw1use, totalcounter, countmultiplier;
enum flag countdownfinished, countdownbegun, countdownaudible;

// ******************************************
// @fn          start_countdown
// @brief       Sets countdown state to on.
// @param       none
// @return      none
// ******************************************
void start_countdown(void)
{
    // Set countdown run flag
	sCountdown.mode = COUNTDOWN_MODE_ON;
	sCountdown.mode2 = ELAPSED_MODE_ON;
	countdownbegun = true;
}

// ******************************************
// @fn          stop_countdown
// @brief       Sets countdown state to off.
//              Does not reset countdown.
// @param       none
// @return      none
// ******************************************
void stop_countdown(void)
{
    // Clear countdown run flag
    sCountdown.mode = COUNTDOWN_MODE_OFF;
    display_symbol(LCD_ICON_HEART, SEG_OFF);
}

// ******************************************
// @fn          reset_countdown
// @brief       Resets all initial countdown data.
// @param       none
// @return      none
// ******************************************
void reset_countdown(void)
{
	// stops both counters
	sCountdown.mode = COUNTDOWN_MODE_OFF;
	sCountdown.mode2 = ELAPSED_MODE_OFF;

	// resets switch usage and clears all flags
	sw1use = finishedcounter = heartcount = tilttozero = totalcounter = tiltreturn = 0;
	countdownaudible = countdownfinished = countdownbegun = false;
	countmultiplier = 4;
	sAccel.datax = 0;
	sAccel.datay = 0;

	// resets buzzer
	stop_buzzer();
	reset_buzzer();

	// clears counter values
	sCountdown.minute = 10;
    sCountdown.second = 00;
    sCountdown.secondflow = 00;
    sCountdown.secondflow--;
	sCountdown.elapminute = 00;
    sCountdown.elapsecond = 00;

    // updates display
    display_symbol(LCD_ICON_HEART, SEG_OFF);
    display_countdown(1, DISPLAY_LINE_UPDATE_FULL);
}

// ******************************************
// @fn          reaches_zero
// @brief       countdown reaches zero
// @param       none
// @return      none
// ******************************************
void reaches_zero(void)
{
	// calls buzzer for one minute
	reset_buzzer();
	start_buzzer(60, CONV_MS_TO_TICKS(1000), CONV_MS_TO_TICKS(1));

	// stops counter from running
	stop_countdown();
	countdownfinished = true;

	// updates display
	sCountdown.minute = 00;
    sCountdown.second = 00;
    display_countdown(1, DISPLAY_LINE_UPDATE_FULL);
}

// ******************************************
// @fn          mx_countdown
// @brief       Button DOWN has been pushed
// @param       u8 line		LINE1 or LINE2
// @return      none
// ******************************************
void mx_countdown(u8 line)
{
	// resets countdown
	reset_countdown();
}

// ******************************************
// @fn          sx_countdown
// @brief       Button UP has been pushed
// @param       u8 line		LINE1 or LINE2
// @return      none
// ******************************************
void sx_countdown(u8 line)
{
	// toggles countdown on/off
	if(sCountdown.mode == COUNTDOWN_MODE_OFF && countdownfinished == 0)
	{
		start_countdown();
	}
	else
	{
		stop_countdown();
	}
}

// ******************************************
// @fn          display_countdown
// @brief       Display routine.
// @param       u8 line			LINE1
//				u8 update		DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_CLEAR
// @return      none
// ******************************************
void display_countdown(u8 line, u8 update)
{
	// update relevant display segments
	display_symbol(LCD_SEG_L1_COL, SEG_ON);
	display_symbol(LCD_SEG_L2_COL0, SEG_ON);

	display_chars(switch_seg(line, LCD_SEG_L1_3_2,LCD_SEG_L2_3_2),int_to_array(sCountdown.minute, 2,0), SEG_ON);
	display_chars(switch_seg(line, LCD_SEG_L1_1_0,LCD_SEG_L2_1_0), int_to_array(sCountdown.second, 2,0), SEG_ON);

	display_chars(switch_seg(line, LCD_SEG_L2_3_2,LCD_SEG_L2_3_2),int_to_array(sCountdown.elapminute, 2,0), SEG_ON);
	display_chars(switch_seg(line, LCD_SEG_L2_1_0,LCD_SEG_L2_1_0), int_to_array(sCountdown.elapsecond, 2,0), SEG_ON);

	// display_chars(LCD_SEG_L2_3_0, sCountdown.elaptime, SEG_ON);
}

// ******************************************
// @fn          is_countdown_active
// @brief       Returns 1 if countdown module is currently active, else 0
// @param       none
// @return      u8		1 = countdown module is active, else 0
// ******************************************
u8 is_countdown_active(void)
{
	if(sCountdown.mode == COUNTDOWN_MODE_ON)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// ******************************************
// @fn          countdown tick
// @brief       decrements countdown time
// @param		none
// @return		none
// ******************************************
void countdown_tick(void)
{
	// calls buzzer
	if(countdownaudible == true)
	{
		start_buzzer(1, CONV_MS_TO_TICKS(100), CONV_MS_TO_TICKS(100));
	}

	// toggles heart icon on and off every other tick
	if (heartcount%2)
	{
		display_symbol(LCD_ICON_HEART, SEG_OFF);
	}
	else
	{
		display_symbol(LCD_ICON_HEART, SEG_ON);
	}
	heartcount++;

	// decrementing counter
	sCountdown.second--;

	// prevents seconds from underflowing
    if (sCountdown.second == sCountdown.secondflow)
	{
    	sCountdown.second = 59;
    	sCountdown.minute--;
	}

    // check for end of countdown
    if (sCountdown.second == 00 && sCountdown.minute == 00)
    {
    	reaches_zero();
    }

	// Always set display update flag
	display.flag.update_countdown = 1;
}

// ******************************************
// @fn          elapsed tick
// @brief       increments elapsed time
// @param		none
// @return		none
// ******************************************
void elapsed_tick(void)
{
	do_acceleration_measurement();

	// incrementing counter
	sCountdown.elapsecond++;

	// prevents seconds from underflowing
    if (sCountdown.elapsecond == 60)
	{
    	sCountdown.elapsecond = 00;
    	sCountdown.elapminute++;
	}

	// Always set display update flag
	display.flag.update_countdown = 1;

	// test for end of countdown time
	if (countdownfinished == true)
	{
		finishedcounter++;
	}
	if (finishedcounter == 60)
	{
		sCountdown.mode2 = ELAPSED_MODE_OFF;
	}
}

// ******************************************
// @fn          tests accelerometer x & y axes
// @brief       %4 for 1Hz, %2 for 2Hz, %1 for 4Hz
// @param		none
// @return		none
// ******************************************
void test_axis(void)
{
    // the raw data straight from the accelerometers
	u8 raw_datax;
    u8 raw_datay;

    // the raw data convert into a displayable value
    u16 accel_datax;
    u16 accel_datay;

    // Start sensor
    as_start();
    sAccel.timeout = ACCEL_MEASUREMENT_TIMEOUT;
    sAccel.mode = ACCEL_MODE_ON;

    // manipulate raw data into displayable
	raw_datax = sAccel.xyz[0];
	raw_datay = sAccel.xyz[1];
	accel_datax = convert_acceleration_value_to_mgrav(raw_datax) / 10;
	accel_datay = convert_acceleration_value_to_mgrav(raw_datay) / 10;

	// perform filtering and store average
    accel_datax = (u16) ((accel_datax * 0.1) + (sAccel.datax * 0.9));
    accel_datay = (u16) ((accel_datay * 0.1) + (sAccel.datay * 0.9));
    sAccel.datax = accel_datax;
    sAccel.datay = accel_datay;

	// if accelerometer data is outside threshold values
    if ((accel_datax > X_THRESHOLD || accel_datay > Y_THRESHOLD))
	{
		tilttozero++;
		if (tilttozero == 1)
		{
			// if tilting goes above thresholds, set speed=2Hz
			countmultiplier = 2;
			// set tiltreturn to a value to stop 1Hz being reached
			tiltreturn = (TILT_RETURN_DELAY*8);
		}
		if (tilttozero == (DELAY_2HZ_TO_4HZ*8))
		{
			// if tilting continues, set speed=4Hz
			countmultiplier = 1;
		}
		if (tilttozero == ((DELAY_4HZ_TO_ZERO*16)+20))
		{
			// if tilting still continues, end countdown
			reaches_zero();
		}
	}

    // if accelerometer data is within threshold values
    else
	{
		if (tiltreturn == 0)
		{
			// if tilting stops, set speed=1Hz (provided tiltreturn =0)
			countmultiplier = 4;
			tilttozero = 0;
		}
		else
		{
			// otherwise decrement tiltreturn and wait until next time
			tiltreturn--;
		}
	}
}
