#include "cheonsa__ops.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // silly windows, leave us alone.
#include <Windows.h>
#endif

namespace cheonsa
{
	namespace ops
	{

		sint64_c time_get_high_resolution_timer_frequency()
		{
#if defined( cheonsa_platform_windows )
			static LARGE_INTEGER result = {};
			if ( result.QuadPart == 0 )
			{
				QueryPerformanceFrequency( &result );
			}
			return result.QuadPart;
#else
#error ops::time_get_high_resolution_timer_frequency() is not implemented.
#endif
		}

		sint64_c time_get_high_resolution_timer_count()
		{
#if defined( cheonsa_platform_windows )
			LARGE_INTEGER result = {};
			QueryPerformanceCounter( &result );
			return result.QuadPart;
#else
#error ops::time_get_high_resolution_timer_count() is not implemented.
#endif
		}

		sint64_c time_get_milliseconds_since_epoch()
		{
#if defined( cheonsa_platform_windows )
			FILETIME file_time = {};
			GetSystemTimeAsFileTime( &file_time );
			ULARGE_INTEGER time_large_integer;
			time_large_integer.LowPart = file_time.dwLowDateTime;
			time_large_integer.HighPart = file_time.dwHighDateTime;
			return time_convert_time_format_from_windows_to_cheonsa( time_large_integer.QuadPart );
#else
#error ops::time_get_milliseconds_since_epoch() is not implemented.
#endif
		}

		sint64_c time_get_local_time_zone_offset()
		{
#if defined( cheonsa_platform_windows )
			DYNAMIC_TIME_ZONE_INFORMATION dynamic_time_zone_information = {};
			if ( GetDynamicTimeZoneInformation( &dynamic_time_zone_information ) != 0 )
			{
				// Bias contains the current local timezone offset plus daylight savings offset.
				// Bias units are originally in minutes so we need to convert to milliseconds.
				return ( dynamic_time_zone_information.Bias * 60 * 1000 ); // 60 seconds per minute, 1000 milliseconds per second.
			}
			return 0;
#else
#error ops::time_get_local_time_zone_offset() is not implemented.
#endif
		}

#if defined( cheonsa_platform_windows )

		// windows measures time in 100 nanosecond intervals.
		// cheonsa measures time in 1 millisecond intervals.
		// 1 nanosecond = 1/1000000000 = 0.000000001 second.
		// 100 nanosecond = 0.0000001 second
		// 1 millisecond  = 1/1000 = 0.0001 second
		// 0.0001 / 0.0000001 = 10000
		//   10000 (100 nanosecond intervals per millisecond)
#define cheonsa_windows_interval_to_cheonsa_interval 10000

		// windows' epoch is 1601 January 1 00:00 GMT.
		// cheonsa's epoch is 2000 January 1 12:00 GMT. (Julain years J2000).
		// these are 12591201600 seconds apart.
		// calculated the difference between the two epochs using python:
		//   >>> windows_epoch = datetime.datetime( 1601, 1, 1, 0 )
		//   >>> cheonsa_epoch = datetime.datetime( 2000, 1, 1, 12 )
		//   >>> ( cheonsa_epoch - windows_epoch ).total_seconds()
#define cheonsa_windows_epoch_to_cheonsa_epoch 12591201600

		sint64_c time_convert_time_format_from_windows_to_cheonsa( sint64_c windows_time )
		{
			// times 10000000 to convert from 1 second intervals to 100 nanosecond intervals.
			return ( windows_time - ( cheonsa_windows_epoch_to_cheonsa_epoch * 10000000 ) ) / cheonsa_windows_interval_to_cheonsa_interval;
		}

		sint64_c time_convert_time_format_from_cheonsa_to_windows( sint64_c cheonsa_time )
		{
			// times 1000 to convert from 1 second intervals to 1 millisecond intervals.
			return ( cheonsa_time + ( cheonsa_windows_epoch_to_cheonsa_epoch * 1000 ) ) * cheonsa_windows_interval_to_cheonsa_interval;
		}

#else
#error is not implemented.
#endif

	}
}
