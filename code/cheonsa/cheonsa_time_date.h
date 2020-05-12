#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	class time_date_c
	{
	public:
		virtual void_c from_milliseconds_since_epoch( sint64_c value ) = 0;
		virtual sint64_c to_milliseconds_since_epoch() const = 0;
		virtual boolean_c from_string( string8_c const & value ) = 0;
		virtual string8_c to_string( string8_c const & format ) const = 0;

	};

}
