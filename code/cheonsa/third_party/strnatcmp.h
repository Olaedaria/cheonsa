//
// THIS IS AN ALTERED SOURCE VERSION.
//

/* -*- mode: c; c-file-style: "k&r" -*-

  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

namespace strnatcmp
{

	template< typename char_type_c >
	static inline int nat_isdigit( char_type_c a )
	{
		return a >= '0' && a <= '9';
	}

	template< typename char_type_c >
	static inline int nat_isspace( char_type_c a )
	{
		return a == ' ';
	}

	template< typename char_type_c >
	static inline char_type_c nat_toupper( char_type_c a )
	{
		if ( a >= 'a' && a <= 'z' )
		{
			return 'A' + ( a - 'a' );
		}
		return a;
	}

	template< typename char_type_c >
	static int compare_right( char_type_c const * a, char_type_c const * b, int ai, int bi )
	{
		int bias = 0;

		// The longest run of digits wins.  That aside, the greatest value wins, but we can't know that it will until we've scanned both numbers to know that they have the same magnitude, so we remember it in BIAS.
		while ( true )
		{
			ai++;
			bi++;
			if ( !nat_isdigit( a[ ai ] ) && !nat_isdigit( b[ bi ] ) )
			{
				return bias;
			}
			else if ( !nat_isdigit( a[ ai ] ) )
			{
				return -1;
			}
			else if ( !nat_isdigit( b[ bi ] ) )
			{
				return +1;
			}
			else if ( a[ ai ] < b[ bi ] )
			{
				if ( !bias )
				{
					bias = -1;
				}
			}
			else if ( a[ ai ] > b[ bi ] )
			{
				if ( !bias )
				{
					bias = +1;
				}
			}
			else if ( !a[ ai ] && !b[ bi ] )
			{
				return bias;
			}
		}

		return 0;
	}

	template< typename char_type_c >
	static int compare_left( char_type_c const * a, char_type_c const * b, int ai, int bi )
	{
		// Compare two left-aligned numbers: the first to have a different value wins.
		while ( true )
		{
			ai++;
			bi++;
			if ( !nat_isdigit( a[ ai ] ) && !nat_isdigit( b[ bi ] ) )
			{
				return 0;
			}
			else if ( !nat_isdigit( a[ ai ] ) )
			{
				return -1;
			}
			else if ( !nat_isdigit( b[ bi ] ) )
			{
				return +1;
			}
			else if ( a[ ai ] < b[ bi ] )
			{
				return -1;
			}
			else if ( a[ ai ] > b[ bi ] )
			{
				return +1;
			}
		}

		return 0;
	}

	template< typename char_type_c >
	int strnatcmp0( char_type_c const * a, char_type_c const * b, bool case_insensitive )
	{
		int ai = 0;
		int bi = 0;
		char_type_c ca;
		char_type_c cb;
		int fractional;
		int result;

		while ( true )
		{
			ca = a[ ai ];
			cb = b[ bi ];

			// skip over leading spaces or zeros
			while ( nat_isspace< char_type_c >( ca ) )
			{
				ca = a[ ++ai ];
			}

			while ( nat_isspace< char_type_c >( cb ) )
			{
				cb = b[ ++bi ];
			}

			// process run of digits
			if ( nat_isdigit< char_type_c >( ca ) && nat_isdigit< char_type_c >( cb ) )
			{
				fractional = ( ca == '0' || cb == '0' );

				if ( fractional )
				{
					if ( ( result = compare_left< char_type_c >( a, b, ai, bi ) ) != 0 )
					{
						return result;
					}
				}
				else
				{
					if ( ( result = compare_right< char_type_c >( a, b, ai, bi ) ) != 0 )
					{
						return result;
					}
				}
			}

			if ( !ca && !cb )
			{
				/* The strings compare the same.  Perhaps the caller
						will want to call strcmp to break the tie. */
				return 0;
			}

			if ( case_insensitive )
			{
				ca = nat_toupper< char_type_c >( ca );
				cb = nat_toupper< char_type_c >( cb );
			}
	  
			if ( ca < cb )
			{
				return -1;
			}
			else if ( ca > cb )
			{
				return +1;
			}

			++ai;
			++bi;
		}
	}

}
