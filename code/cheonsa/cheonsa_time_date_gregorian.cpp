#include "cheonsa_time_date_gregorian.h"
#include "cheonsa_time_types.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	time_date_gregorian_c::time_date_gregorian_c()
		: time( 0 )
		, year( 0 )
		, month_of_year( 0 )
		, day_of_year( 0 )
		, day_of_month( 0 )
		, day_of_week( 0 )
		, hour_of_day( 0 )
		, minute_of_hour( 0 )
		, second_of_minute( 0 )
		, miliseconds_of_second( 0 )
	{
	}

	void_c time_date_gregorian_c::from_miliseconds_since_epoch( sint64_c value )
	{
		sint64_c days_since_epoch = value / miliseconds_per_day;
		convert_days_since_epoch_to_date( days_since_epoch, year, month_of_year, day_of_month, day_of_year );
		convert_days_since_epoch_to_day_of_week( days_since_epoch, day_of_week );
		sint32_c miliseconds_left_over = static_cast<sint32_c>( value - ( days_since_epoch * miliseconds_per_day ) );
		hour_of_day = miliseconds_left_over / miliseconds_per_hour;
		miliseconds_left_over -= hour_of_day * miliseconds_per_hour;
		minute_of_hour = miliseconds_left_over / miliseconds_per_minute;
		miliseconds_left_over -= minute_of_hour * miliseconds_per_minute;
		second_of_minute = miliseconds_left_over / miliseconds_per_second;
		miliseconds_of_second = miliseconds_left_over - ( second_of_minute * miliseconds_of_second );
	}

	sint64_c time_date_gregorian_c::to_miliseconds_since_epoch() const
	{
		sint64_c days_since_epoch;
		convert_date_to_days_since_epoch( days_since_epoch, year, month_of_year, day_of_month );
		return ( days_since_epoch * miliseconds_per_day ) + ( hour_of_day * miliseconds_per_hour ) + ( minute_of_hour * miliseconds_per_minute ) + ( second_of_minute * miliseconds_per_second ) + ( miliseconds_of_second );
	}

	enum time_date_code_e
	{
		time_date_code_e_none,
		time_date_code_e_Y,
		time_date_code_e_YY, // strong dislike, much disdain.
		time_date_code_e_MM,
		time_date_code_e_M,
		time_date_code_e_month,
		time_date_code_e_Month,
		time_date_code_e_MONTH,
		time_date_code_e_mon,
		time_date_code_e_Mon,
		time_date_code_e_MON,
		time_date_code_e_DD,
		time_date_code_e_D,
		time_date_code_e_weekday,
		time_date_code_e_Weekday,
		time_date_code_e_WEEKDAY,
		time_date_code_e_wkd,
		time_date_code_e_Wkd,
		time_date_code_e_WKD,
		time_date_code_e_hh,
		time_date_code_e_h,
		time_date_code_e_mm,
		time_date_code_e_ss
	};

	boolean_c time_scan_for_date_code( char8_c * string, time_date_code_e & code, sint32_c & code_length )
	{
		if ( string[ 0 ] )
		{
			if ( string[ 0 ] == '{' )
			{
				if ( string[ 1 ] == 'Y' )
				{
					if ( ops::string8_compare( &string[ 2 ], "}" ) )
					{
						// {Y}
						code = time_date_code_e_Y;
						code_length = 3;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "Y}" ) )
					{
						// {YY}
						code = time_date_code_e_YY;
						code_length = 4;
						return true;
					}
				}
				else if ( string[ 1 ] == 'M' )
				{
					if ( ops::string8_compare( &string[ 2 ], "}" ) )
					{
						// {M}
						code = time_date_code_e_M;
						code_length = 3;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "M}" ) )
					{
						// {MM}
						code = time_date_code_e_MM;
						code_length = 4;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "onth}" ) )
					{
						// {Month}
						code = time_date_code_e_Month;
						code_length = 7;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "ONTH}" ) )
					{
						// {MONTH}
						code = time_date_code_e_MONTH;
						code_length = 7;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "on}" ) )
					{
						// {Mon}
						code = time_date_code_e_Mon;
						code_length = 5;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "ON}" ) )
					{
						// {MON}
						code = time_date_code_e_MON;
						code_length = 5;
						return true;
					}
				}
				else if ( string[ 1 ] == 'm' )
				{
					if ( ops::string8_compare( &string[ 2 ], "on}" ) )
					{
						// {mon}
						code = time_date_code_e_mon;
						code_length = 5;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "onth}" ) )
					{
						// {month}
						code = time_date_code_e_month;
						code_length = 5;
						return true;
					}
				}
				else if ( string[ 1 ] == 'D' )
				{
					if ( ops::string8_compare( &string[ 2 ], "}" ) )
					{
						// {D}
						code = time_date_code_e_D;
						code_length = 3;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "D}" ) )
					{
						// {DD}
						code = time_date_code_e_DD;
						code_length = 4;
						return true;
					}
				}
				else if ( string[ 1 ] == 'W' )
				{
					if ( ops::string8_compare( &string[ 2 ], "kd}" ) )
					{
						// {Wkd}
						code = time_date_code_e_Wkd;
						code_length = 5;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "KD}" ) )
					{
						// {WKD}
						code = time_date_code_e_WKD;
						code_length = 5;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "eekday}" ) )
					{
						// {Weekday}
						code = time_date_code_e_Weekday;
						code_length = 9;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "EEKDAY}" ) )
					{
						// {WEEKDAY}
						code = time_date_code_e_WEEKDAY;
						code_length = 9;
						return true;
					}
				}
				else if ( string[ 1 ] == 'w' )
				{
					if ( ops::string8_compare( &string[ 2 ], "eekday}" ) )
					{
						// {weekday}
						code = time_date_code_e_weekday;
						code_length = 9;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "kd}" ) )
					{
						// {wkd}
						code = time_date_code_e_wkd;
						code_length = 5;
						return true;
					}
				}
				else if ( string[ 1 ] == 'h' )
				{
					if ( ops::string8_compare( &string[ 2 ], "}" ) )
					{
						// {h}
						code = time_date_code_e_h;
						code_length = 3;
						return true;
					}
					else if ( ops::string8_compare( &string[ 2 ], "h}" ) )
					{
						// {hh}
						code = time_date_code_e_hh;
						code_length = 5;
						return true;
					}
				}
				else if ( string[ 1 ] == 'm' )
				{
					if ( ops::string8_compare( &string[ 2 ], "m}" ) )
					{
						// {mm}
						code = time_date_code_e_mm;
						code_length = 5;
						return true;
					}
				}
				else if ( string[ 1 ] == 's' )
				{
					if ( ops::string8_compare( &string[ 2 ], "s}" ) )
					{
						// {ss}
						code = time_date_code_e_ss;
						code_length = 5;
						return true;
					}
				}
			}
		}
		code = time_date_code_e_none;
		code_length = 0;
		return false;
	}

	boolean_c time_date_gregorian_c::from_string( string8_c const & value )
	{
		return false;
	}

	string8_c time_date_gregorian_c::to_string( string8_c const & format ) const
	{
		string8_c result;

		static char8_c const * day_of_week_full_names[] = {
			"Sunday",
			"Monday",
			"Tuesday",
			"Wednesday",
			"Thursday",
			"Friday",
			"Saturday",
		};

		static char8_c const * day_of_week_short_names[] = {
			"Sun",
			"Mon",
			"Tues",
			"Wed",
			"Thurs",
			"Fri",
			"Sat",
		};

		static char8_c const * month_of_year_full_names[] = {
			"January",
			"February",
			"March",
			"April",
			"May",
			"June",
			"July",
			"August",
			"September",
			"October",
			"November",
			"December"
		};

		static char8_c const * month_of_year_short_names[] = {
			"Jan",
			"Feb",
			"Mar",
			"Apr",
			"May",
			"Jun",
			"Jul",
			"Aug",
			"Sept",
			"Oct",
			"Nov",
			"Dec"
		};

		time_date_code_e code;
		sint32_c code_length;
		char8_c * string = format.character_list.get_internal_array();
		while ( string[ 0 ] )
		{
			if ( time_scan_for_date_code( string, code, code_length ) )
			{
				string8_c temp;
				switch ( code )
				{
					case time_date_code_e_Y:
						ops::convert_sint32_to_string8( year, temp );
						break;
					case time_date_code_e_YY:
						ops::convert_sint32_to_string8( year, temp );
						if ( temp.get_length() < 2 )
						{
							ops::string8_pad_with_leading_zeros( temp, 2 );
						}
						else if ( temp.get_length() > 2 )
						{
							temp.character_list.remove_range_at_index( 0, temp.character_list.get_length() - 3 );
						}
						break;
					case time_date_code_e_MM:
						ops::convert_sint32_to_string8( month_of_year, temp );
						ops::string8_pad_with_leading_zeros( temp, 2 );
						break;
					case time_date_code_e_M:
						ops::convert_sint32_to_string8( month_of_year, temp );
						break;
					case time_date_code_e_month:
						temp = month_of_year_full_names[ month_of_year ];
						ops::string8_to_lower( temp );
						break;
					case time_date_code_e_Month:
						temp = month_of_year_full_names[ month_of_year ];
						break;
					case time_date_code_e_MONTH:
						temp = month_of_year_full_names[ month_of_year ];
						ops::string8_to_upper( temp );
						break;
					case time_date_code_e_mon:
						temp = month_of_year_short_names[ month_of_year ];
						ops::string8_to_lower( temp );
						break;
					case time_date_code_e_Mon:
						temp = month_of_year_short_names[ month_of_year ];
						break;
					case time_date_code_e_MON:
						temp = month_of_year_short_names[ month_of_year ];
						ops::string8_to_upper( temp );
						break;
					case time_date_code_e_DD:
						ops::convert_sint32_to_string8( day_of_month, temp );
						ops::string8_pad_with_leading_zeros( temp, 2 );
						break;
					case time_date_code_e_D:
						ops::convert_sint32_to_string8( day_of_month, temp );
						break;
					case time_date_code_e_weekday:
						temp = day_of_week_full_names[ day_of_week ];
						ops::string8_to_lower( temp );
						break;
					case time_date_code_e_Weekday:
						temp = day_of_week_full_names[ day_of_week ];
						break;
					case time_date_code_e_WEEKDAY:
						temp = day_of_week_full_names[ day_of_week ];
						ops::string8_to_upper( temp );
						break;
					case time_date_code_e_wkd:
						temp = day_of_week_short_names[ day_of_week ];
						ops::string8_to_lower( temp );
						break;
					case time_date_code_e_Wkd:
						temp = day_of_week_short_names[ day_of_week ];
						break;
					case time_date_code_e_WKD:
						temp = day_of_week_short_names[ day_of_week ];
						ops::string8_to_upper( temp );
						break;
					case time_date_code_e_hh:
						ops::convert_sint32_to_string8( hour_of_day, temp );
						ops::string8_pad_with_leading_zeros( temp, 2 );
						break;
					case time_date_code_e_h:
						ops::convert_sint32_to_string8( hour_of_day, temp );
						break;
					case time_date_code_e_mm:
						ops::convert_sint32_to_string8( minute_of_hour, temp );
						ops::string8_pad_with_leading_zeros( temp, 2 );
						break;
					case time_date_code_e_ss:
						ops::convert_sint32_to_string8( second_of_minute, temp );
						ops::string8_pad_with_leading_zeros( temp, 2 );
						break;
				}
				string += code_length;
				result += temp;
			}
			else
			{
				result += string[ 0 ];
				string++;
			}
		}
		return result;
	}

	boolean_c time_date_gregorian_c::is_leap_year( sint32_c year )
	{
		// taken from: http://howardhinnant.github.io/date_algorithms.html.
		// a year is a leap year if it is divisible by 4,
		// but century years are not leap years unless they are divisible by 400.
		// So, the years 1700, 1800, and 1900 were not leap years, but the year 2000 was.
		return year % 4 == 0 && ( year % 100 != 0 || year % 400 == 0 );
	}

	void_c time_date_gregorian_c::convert_date_to_days_since_epoch( sint64_c & days, sint32_c year, sint32_c month_of_year, sint32_c day_of_month )
	{
		// taken from: http://howardhinnant.github.io/date_algorithms.html.
		// epoch that this function works with is at 0000-03-01, which is 60 days after 0000-00-00.
		// days difference between cheonsa's epoch of 2000-1-1 and this function's epoch of 0000-03-01 is 730485 days.
		// algorithm originally expected day_of_month to be in range [1, 31], however we removed the - 1 in the algorithm, so the function expects to to be in the range [0, 30].
		month_of_year++; // algorithm expects this to be in range [1, 12].
		year -= month_of_year <= 2;
		sint32_c const era = ( year >= 0 ? year : year - 399 ) / 400; // every 400 years is an era.
		uint32_c const yoe = static_cast< uint32_c >( year - era * 400 ); // year of era [0, 399]
		uint32_c const doy = ( 153 * ( month_of_year + ( month_of_year > 2 ? -3 : 9 ) ) + 2 ) / 5 + day_of_month; // day of year [0, 365]
		uint32_c const doe = yoe * 365 + yoe / 4 - yoe / 100 + doy; // day of era [0, 146096], factors in leap years, factors out century years.
		days = static_cast< sint64_c >( era ) * 146097 + static_cast< sint64_c >( doe ) - 730485; // - 730485 shifts from days since 0000-03-01 to days since 2000-01-01.
	}

	void_c time_date_gregorian_c::convert_days_since_epoch_to_date( sint64_c days, sint32_c & year, sint32_c & month_of_year, sint32_c & day_of_month, sint32_c & day_of_year )
	{
		// taken from: http://howardhinnant.github.io/date_algorithms.html.
		days += 730485; // shift from days since 2000-01-01 to days since 0000-03-01.
		sint32_c const era = static_cast< sint32_c >( ( days >= 0 ? days : days - 146096 ) / 146097 );
		uint32_c const doe = static_cast< uint32_c >( days - era * 146097 ); // [0, 146096]
		uint32_c const yoe = ( doe - doe / 1460 + doe / 36524 - doe / 146096 ) / 365; // [0, 399]
		year = static_cast< sint32_c >( yoe ) + era * 400;
		day_of_year = doe - ( 365 * yoe + yoe / 4 - yoe / 100 ); // [0, 365]
		uint32_c const mp = ( 5 * day_of_year + 2 ) / 153; // [0, 11]
		day_of_month = day_of_year - ( 153 * mp + 2 ) / 5; // [0, 30]
		month_of_year = mp + ( mp < 10 ? 3 : -9 ) - 1; // [0, 11]
		year += month_of_year <= 2;
	}

	void_c time_date_gregorian_c::convert_days_since_epoch_to_day_of_week( sint64_c days, sint32_c & day_of_week )
	{
		// taken from: http://howardhinnant.github.io/date_algorithms.html.
		day_of_week = static_cast< sint32_c >( days >= -4 ? ( days + 4 ) % 7 : ( days + 5 ) % 7 + 6 );
	}

}
