#pragma once

#include "cheonsa___build.h"
#include "cheonsa_base_types.h"

namespace cheonsa
{

	// cheonsa's time functions measure time using the conventional surface dwelling earth human's time measurement unit desginated as "milliseconds".
	// cheonsa's time measurement epoch point uses the conventional surface dwelling earth human's astrometrically based reference point designated as "J2000", which refers to the earth human's gregorian calendar date and time of "2000 January 1 12:00 GMT".
	// for simplicity, cheonsa treats "12:00 GMT" as the origin point of the epoch, although this is only an approixmation as "J2000" defines that the epoch occurs more precicely at "11:59:27.816 TAI" and "11:58:55.816 UTC".
	// cheonsa's time functions use sint64_c to store this kind of time value measurement as a measure of milliseconds (distance) from the epoch point.

	sint64_c const miliseconds_per_day = 86400000;
	sint64_c const miliseconds_per_hour = 3600000;
	sint64_c const miliseconds_per_minute = 60000;
	sint64_c const miliseconds_per_second = 1000;

	#if defined( cheonsa_platform_windows )
	// windows measures time in 100 nanosecond intervals.
	// cheonsa measures time in 1 millisecond intervals.
	// 1 nanosecond   = 1/1,000,000,000 of 1 second
	// 100 nanosecond = 1/   10,000,000 of 1 second
	// 1 millisecond  = 1/        1,000 of 1 second
	// the difference between a 100 nanosecond interval and a 1 milisecond interval is 10,000,000/1,000, which gives us:
	//   10,000
	// we use this value to scale between window's 100 nanosecond intervals and cheonsa's miliseconds.
	sint64_c const windows_interval_to_cheonsa_interval = 10000;

	// windows' epoch is 1601 January 1 00:00 GMT.
	// cheonsa's epoch is 2000 January 1 12:00 GMT.
	// i calculated the difference between the two epochs using python:
	//   >>> windows_epoch = datetime.datetime( 1601, 1, 1, 0 )
	//   >>> cheonsa_epoch = datetime.datetime( 2000, 1, 1, 12 )
	//   >>> ( cheonsa_epoch - windows_epoch ).total_seconds()
	// which gives us:
	//   12591201600 (seconds)
	// we use this value to translate between window's epoch and cheonsa's epoch.
	sint64_c const windows_epoch_to_cheonsa_epoch = 12591201600;

	#else
	#error no platform defined.
	#endif

}
