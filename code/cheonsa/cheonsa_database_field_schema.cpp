#include "cheonsa_database_field_schema.h"

namespace cheonsa
{

	database_field_schema_c::reflection_information_c::reflection_information_c()
		: view( data_view_e_default )
		, is_readable( false )
		, is_writable( false )
		, additional_options()
		, defaults_and_limits{}
	{
		// IDK if we need to do this in modern c++.
		defaults_and_limits.float64_default[ 0 ] = 0;
		defaults_and_limits.float64_default[ 1 ] = 0;
		defaults_and_limits.float64_default[ 2 ] = 0;
		defaults_and_limits.float64_default[ 3 ] = 0;
		defaults_and_limits.float64_minimum = 0;
		defaults_and_limits.float64_maximum = 0;
	}

	database_field_schema_c::reflection_information_c & database_field_schema_c::reflection_information_c::operator = ( reflection_information_c const & other )
	{
		view = other.view;
		is_readable = other.is_readable;
		is_writable = other.is_writable;
		additional_options = other.additional_options;
		defaults_and_limits = other.defaults_and_limits;
		return *this;
	}

	database_field_schema_c::database_field_schema_c()
		: _record_schema( nullptr )
		, _key()
		, _type( data_type_e_none )
		, _type_count( 0 )
		, _reflection_information()
		, _data_offset( 0 )
		, _data_size( 0 )
	{
	}

	database_field_schema_c & database_field_schema_c::operator = ( database_field_schema_c const & other )
	{
		_record_schema = other._record_schema;
		_key = other._key;
		_type = other._type;
		_type_count = other._type_count;
		_reflection_information = other._reflection_information;
		_data_offset = other._data_offset;
		_data_size = other._data_size;
		return *this;
	}

}