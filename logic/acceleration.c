	// *************************************************************************************************
	//
	//      Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
	//
	//
	//        Redistribution and use in source and binary forms, with or without
	//        modification, are permitted provided that the following conditions
	//        are met:
	//
	//          Redistributions of source code must retain the above copyright
	//          notice, this list of conditions and the following disclaimer.
	//
	//          Redistributions in binary form must reproduce the above copyright
	//          notice, this list of conditions and the following disclaimer in the
	//          documentation and/or other materials provided with the
	//          distribution.
	//
	//          Neither the name of Texas Instruments Incorporated nor the names of
	//          its contributors may be used to endorse or promote products derived
	//          from this software without specific prior written permission.
	//
	//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	//
	// *************************************************************************************************
	// Include section

	// system
	#include "project.h"

	// driver
	#include "display.h"
	#include "countdown.h"
	#include "vti_as.h"

	// logic
	#include "acceleration.h"
	#include "simpliciti.h"
	#include "user.h"

	// ******************************************
	// Prototypes section
	void do_acceleration_measurement(void);
	void test_axis(void);
	// *************************************************************************************************
	// Global Variable section
	struct accel sAccel;
	// Conversion values from data to mgrav taken from CMA3000-D0x datasheet (rev 0.4, table 4)
	const u16 mgrav_per_bit[7] = { 18, 36, 71, 143, 286, 571, 1142 };

	// *************************************************************************************************
	// Extern section
	// Global flag for proper acceleration sensor operation
	extern u8 as_ok;

	// *************************************************************************************************
	// @fn          reset_acceleration
	// @brief       Reset acceleration variables.
	// @param       none
	// @return      none
	// *************************************************************************************************
	void reset_acceleration(void)
	{
		// Start with Y-axis display
		sAccel.view_style = DISPLAY_ACCEL_Y;

		// Clear timeout counter
		sAccel.timeout = 0;

		// Default mode is off
		sAccel.mode = ACCEL_MODE_OFF;
	}

	// *************************************************************************************************
	// @fn          sx_acceleration
	// @brief       Acceleration direct function. Button UP switches between X/Y/Z values.
	// @param       u8 line         LINE2
	// @return      none
	// *************************************************************************************************
	void sx_acceleration(u8 line)
	{
		if (++sAccel.view_style > 2)
			sAccel.view_style = 0;

		// Reset current acceleration value
		sAccel.datax = 0;
		sAccel.datay = 0;

		// Get data from sensor
		as_get_data(sAccel.xyz);
	}

	// *************************************************************************************************
	// @fn          acceleration_value_is_positive
	// @brief       Returns 1 if 2's complement number is positive
	// @param       u8 value        2's complement number
	// @return      u8                      1 = number is positive, 0 = number is negavtive
	// *************************************************************************************************
	u8 acceleration_value_is_positive(u8 value)
	{
		return ((value & BIT7) == 0);
	}

	// *************************************************************************************************
	// @fn          convert_acceleration_value_to_mgrav
	// @brief       Converts measured value to mgrav units
	// @param       u8 value        g data from sensor
	// @return      u16                     Acceleration (mgrav)
	// *************************************************************************************************
	u16 convert_acceleration_value_to_mgrav(u8 value)
	{
		u16 result;
		u8 i;

		if (!acceleration_value_is_positive(value))
		{
			// Convert 2's complement negative number to positive number
			value = ~value;
			value += 1;
		}

		result = 0;
		for (i = 0; i < 7; i++)
		{
			result += ((value & (BIT(i))) >> i) * mgrav_per_bit[i];
		}

		return (result);
	}

	// *************************************************************************************************
	// @fn          is_acceleration_measurement
	// @brief       Returns 1 if acceleration is currently measured.
	// @param       none
	// @return      u8              1 = acceleration measurement ongoing
	// *************************************************************************************************
	u8 is_acceleration_measurement(void)
	{
		return ((sAccel.mode == ACCEL_MODE_ON) && (sAccel.timeout > 0));
	}

	// *************************************************************************************************
	// @fn          do_acceleration_measurement
	// @brief       Get sensor data and store in sAccel struct
	// @param       none
	// @return      none
	// *************************************************************************************************
	void do_acceleration_measurement(void)
	{
		reset_acceleration();

		// Get data from sensor
		as_get_data(sAccel.xyz);

		// Set display update flag
		display.flag.update_acceleration = 1;

		// If countdown is active, test x & y axes
		if (sCountdown.mode == COUNTDOWN_MODE_ON)
		{
			test_axis();
		}
	}

	// *************************************************************************************************
	// @fn          display_acceleration
	// @brief       Display routine.
	// @param       u8 line                 LINE1
	//                              u8 update               DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_CLEAR
	// @return      none
	// *************************************************************************************************
	void display_acceleration(u8 line, u8 update)
	{

	}
