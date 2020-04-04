#include "cheonsa_database_record_schema.h"
#include "cheonsa_database_field_schema.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	database_record_schema_c::database_record_schema_c()
		: _fields()
	{
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

	boolean_c database_record_schema_c::save_record_buffer( data_scribe_binary_c & scribe, core_list_c< uint8_c > const & record_buffer, sint32_c record_count )
	{
		assert( scribe.get_byte_order() != ops::get_native_byte_order() ); // there's no reason to use this function if byte order flipping isn't needed.
		sint32_c record_size = get_size();
		assert( record_size * record_count == record_buffer.get_length() );
		for ( sint32_c i = 0; i < record_count; i++ )
		{
			uint8_c const * record_data = &record_buffer[ record_size * i ];
			for ( sint32_c j = 0; j < _fields.get_length(); j++ )
			{
				database_field_schema_c & field = _fields[ j ];
				assert( field.type_count > 0 && field.type_count <= 4 );
				for ( sint32_c k = 0; k < field.type_count; k++ )
				{
					uint8_c const * field_data = &record_data[ field._data_offset ];
					// save field header.
					if ( !scribe.save_uint8( field_data[ 0 ] ) )
					{
						return false;
					}
					// save field value.
					if ( field.type == data_type_e_string8 || field.type == data_type_e_string16 )
					{
						assert( field.type_count == 1 );
						assert( 1 + 4 == field._data_size );
						if ( !scribe.save_generic( &field_data[ 1 ], 4 ) )
						{
							return false;
						}
					}
					else
					{
						assert( 1 + ops::get_data_type_size( field.type ) == field._data_size );
						if ( !scribe.save_generic( &field_data[ 1 ], field._data_size - 1 ) )
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	boolean_c database_record_schema_c::load_record_buffer( data_scribe_binary_c & scribe, core_list_c< uint8_c > & record_buffer, sint32_c record_count )
	{
		assert( scribe.get_byte_order() != ops::get_native_byte_order() ); // there's no reason to use this function if byte order flipping isn't needed.
		sint32_c record_size = get_size();
		record_buffer.set_length( record_size * record_count );
		for ( sint32_c i = 0; i < record_count; i++ )
		{
			uint8_c * record_data = &record_buffer[ record_size * i ];
			for ( sint32_c j = 0; j < _fields.get_length(); j++ )
			{
				database_field_schema_c & field = _fields[ j ];
				assert( field.type_count > 0 && field.type_count <= 4 );
				for ( sint32_c k = 0; k < field.type_count; k++ )
				{
					uint8_c * field_data = &record_data[ field._data_offset ];
					// load field header.
					if ( !scribe.load_uint8( field_data[ 0 ] ) )
					{
						return false;
					}
					// load field data.
					if ( field.type == data_type_e_string8 || field.type == data_type_e_string16 )
					{
						assert( field.type_count == 1 );
						assert( 1 + 4 == field._data_size );
						if ( !scribe.load_generic( &field_data[ 1 ], 4 ) )
						{
							return false;
						}
					}
					else
					{
						assert( 1 + ops::get_data_type_size( field.type ) == field._data_size );
						if ( !scribe.load_generic( &record_data[ field._data_offset ], field._data_size - 1 ) )
						{
							return false;
						}
					}
				}
			}
		}
		return true;
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
