#include "cheonsa_database_record_schema.h"
#include "cheonsa_database_field_schema.h"
#include <cassert>

namespace cheonsa
{

	database_record_schema_c::database_record_schema_c()
		: _fields()
	{
		assert( sizeof( database_record_header_c ) == 13 );
	}

	void_c database_record_schema_c::reset()
	{
		_fields.remove_all();
	}

	core_list_c< database_field_schema_c > const & database_record_schema_c::get_fields() const
	{
		return _fields;
	}

	database_field_schema_c * database_record_schema_c::add_field( string8_c const & name, data_type_e type, uint8_c type_count )
	{
		// verify.
		assert( _fields.get_length() + 1 < constants< uint16_c >::maximum() );
		assert( get_field( name ) == nullptr );
		assert( type >= data_type_e_string8 && type <= data_type_e_float64 );
		if ( type == data_type_e_string8 || type == data_type_e_string16 )
		{
			assert( type_count == 1 );
		}
		else
		{
			assert( type_count >= 1 && type_count <= 4 );
		}

		// calculate data offset and size.
		uint16_c data_offset = get_size(); // start at current record size.
		uint16_c data_size = 1 + ( database_get_data_type_size( type ) * type_count ); // each field is prefixed by a 1 byte header for flags.

		// add to our collection.
		database_field_schema_c * field = _fields.emplace_at_end();
		field->record_schema = this;
		field->_index = static_cast< uint16_c >( _fields.get_length() - 1 );
		field->_data_offset = data_offset;
		field->_data_size = data_size;
		return field; // return so caller can set reflection parameters.
	}

	database_field_schema_c const * database_record_schema_c::get_field( string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _fields[ i ];
			if ( field.name == name )
			{
				return &field;
			}
		}
		return nullptr;
	}

	database_field_schema_c const * database_record_schema_c::get_field( string8_c const & name, data_type_e type, uint8_c type_count ) const
	{
		for ( sint32_c i = 0; i < _fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _fields[ i ];
			if ( field.name == name )
			{
				if ( field.type == type && field.type_count == type_count )
				{
					return &field;
				}
				break;
			}
		}
		return nullptr;
	}

	uint16_c database_record_schema_c::get_size() const
	{
		if ( _fields.get_length() > 0 )
		{
			database_field_schema_c const & last_field = _fields[ _fields.get_length() - 1 ];
			return last_field._data_offset + last_field._data_size;
		}
		return sizeof( database_record_header_c );
	}

	database_record_schema_c & database_record_schema_c::operator = ( database_record_schema_c const & other )
	{
		_fields = other._fields;
		return *this;
	}

	boolean_c database_record_schema_c::operator == ( database_record_schema_c const & other ) const
	{
		if ( _fields.get_length() != other._fields.get_length() )
		{
			return false;
		}
		for ( sint32_c i = 0; i < _fields.get_length(); i++ )
		{
			database_field_schema_c const & this_field = _fields[ i ];
			database_field_schema_c const & that_field = other._fields[ i ];
			if ( this_field.name != that_field.name || this_field.type != that_field.type || this_field.type_count != that_field.type_count )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c database_record_schema_c::operator != ( database_record_schema_c const & other ) const
	{
		return !( operator == ( other ) );
	}

}
