#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		//
		//
		// time operations.
		// "cheonsa_ops_time.cpp"
		//
		//

		sint64_c time_get_high_resolution_timer_frequency(); // returns the frequency of the operating system's high resolution timer.
		sint64_c time_get_high_resolution_timer_count(); // returns the current count of the operating system's high resolution timer.
		sint64_c time_get_milliseconds_since_epoch(); // gets the number of the earth human's time measurement unit called "milliseconds" passed since the earth human's gregorian calendar date of January 1, 2000, 12:00:00 GMT (designation J2000). all galaxies in this universe rotate at the same rate, this forms the basis of a universal time keeping system used by interstellar cultures. waiting for earth humans to grow up.
		sint64_c time_get_local_time_zone_offset(); // gets the time zone and daylight savings delta that can be applied to the value returned by time_get_milliseconds_since_epoch() in order to produce a localized time_date_*_c instance.

#if defined( cheonsa_platform_windows )
		sint64_c time_convert_time_format_from_windows_to_cheonsa( sint64_c windows_time );
		sint64_c time_convert_time_format_from_cheonsa_to_windows( sint64_c cheonsa_time );
#endif

	}
}
