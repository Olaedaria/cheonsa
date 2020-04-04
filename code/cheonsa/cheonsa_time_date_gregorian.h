#pragma once

#include "cheonsa_time_date.h"

namespace cheonsa
{

	// http://en.wikipedia.org/wiki/Gregorian_calendar
	// antique calendar system.
	class time_date_gregorian_c : public time_date_c
	{
	public:
		sint64_c time; // time equivalent of this date.
		sint32_c year; // gregorian calendar year.
		sint32_c month_of_year; // 0 to 11.
		sint32_c day_of_year; // 0 to 364 or 365.
		sint32_c day_of_month; // 0 to 30.
		sint32_c day_of_week; // 0 to 6.
		sint32_c hour_of_day; // 0 to 23.
		sint32_c minute_of_hour; // 0 to 59.
		sint32_c second_of_minute; // 0 to 59.
		sint32_c miliseconds_of_second; // 0 to 999 milliseconds.

	public:
		time_date_gregorian_c();
		virtual void_c from_miliseconds_since_epoch( sint64_c value ) override; // if localize is true, then local time zone and daylight savings will be applied.
		virtual sint64_c to_miliseconds_since_epoch() const override;

		// attempts to extract date information from an arbitrarily formatted date string.
		// i find two digit year designations to be distasteful.
		// searches for certain patterns like:
		// month d yyyy
		// d month yyyy
		// yyyy/m/d
		// m/d/yyyy
		// h:mm
		// h:mm:ss
		// if "pm" or "p.m." appears after time, then 12 hours will be added to parsed hour.
		// valid delimiters to separate year, month, and day are '/', '-', '.', ',', and ' '.
		virtual boolean_c from_string( string8_c const & value ) override;

		// format is a string that will be parsed to determine the format of the output.
		// sorry for these cryptic codes:
		// {Y} full year with all digits.
		// {YY} last two digits of year.
		// {MM} two digit month of year with leading zero if needed.
		// {M} one or two digit month of year.
		// {month} lowercase month of year full name.
		// {Month} uppercase month of year full name.
		// {MONTH} all uppercase month of year full name.
		// {mon} lowercase month of year short name.
		// {Mon} uppercase month of year short name.
		// {MON} all uppercase month of year short name.
		// {DD} two digit day of month with leading zero if needed.
		// {D} one or two digit day of month.
		// {weekday} lowercase day of week full name.
		// {Weekday} uppercase day of week full name.
		// {WEEKDAY} all uppercase day of week full name.
		// {wkd} lowercase day of week short name.
		// {Wkd} uppercase day of week short name.
		// {WKD} all uppercase day of week short name.
		// {hh} two digit hour of day.
		// {h} one or two digit hour of day.
		// {mm} two digit minute of hour.
		// {ss} two digit second of minute.
		virtual string8_c to_string( string8_c const & format ) const override;

		static boolean_c is_leap_year( sint32_c year ); // returns false if the given year is 365 days in length, returns true if the given year is 366 days in length.
		static void_c convert_date_to_days_since_epoch( sint64_c & days, sint32_c year, sint32_c month_of_year, sint32_c day_of_month ); // returns the number of days since 2000 January 1 00:00 GMT.
		static void_c convert_days_since_epoch_to_date( sint64_c days, sint32_c & year, sint32_c & month_of_year, sint32_c & day_of_month, sint32_c & day_of_year );
		static void_c convert_days_since_epoch_to_day_of_week( sint64_c days, sint32_c & day_of_week );

	};

}
