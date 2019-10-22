#include "cheonsa_data_scribe_structure.h"
#include "cheonsa_ops.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	sint32_c data_scribe_structure_c::property_c::get_type_base_size( type_e type )
	{
		if ( type == type_e_sint8 || type == type_e_uint8 )
		{
			return 1;
		}
		else if ( type == type_e_sint16 || type == type_e_uint16 || type == type_e_float16 )
		{
			return 2;
		}
		else if ( type == type_e_sint32 || type == type_e_uint32 || type == type_e_float32 )
		{
			return 4;
		}
		else if ( type == type_e_sint64 || type == type_e_uint64 || type == type_e_float64 )
		{
			return 8;
		}
		return -1;
	}

	string16_c & data_scribe_structure_c::property_c::get_name()
	{
		return _name;
	}

	data_scribe_structure_c::property_c::type_e data_scribe_structure_c::property_c::get_type()
	{
		return _type;
	}

	void_c data_scribe_structure_c::property_c::set_buffer( sint32_c array_count, void_c * data, sint32_c data_size )
	{
		cheonsa_assert( data );
		cheonsa_assert( data_size >= 0 );
		cheonsa_assert( sizeof( buffer_c ) == 8 );

		delete[] _value;
		_value = nullptr;

		sint32_c size_of_header_and_data = sizeof( buffer_c ) + data_size;
		_value = new uint8_c[ size_of_header_and_data ];

		property_c::buffer_c * buffer = reinterpret_cast< property_c::buffer_c * >( _value );
		buffer->array_count = array_count;
		buffer->data_size = data_size;
		uint8_c * buffer_data = reinterpret_cast< uint8_c * >( buffer ) + sizeof( buffer_c );
		if ( data_size > 0 )
		{
			ops::memory_copy( data, buffer_data, data_size );
		}
	}

	sint32_c data_scribe_structure_c::property_c::get_buffer_array_count()
	{
		cheonsa_assert( _type == type_e_buffer );
		return reinterpret_cast< property_c::buffer_c * >( _value )->array_count;
	}

	sint32_c data_scribe_structure_c::property_c::get_buffer_data_size()
	{
		cheonsa_assert( _type == type_e_buffer );
		return reinterpret_cast< property_c::buffer_c * >( _value )->data_size;
	}

	void_c * data_scribe_structure_c::property_c::get_buffer_data()
	{
		cheonsa_assert( _type == type_e_buffer );
		return reinterpret_cast< uint8_c * >( _value ) + sizeof( buffer_c );
	}

	string8_c & data_scribe_structure_c::property_c::get_string8()
	{
		cheonsa_assert( _type == type_e_string8 );
		return *reinterpret_cast< string8_c * >( _value );
	}

	string16_c & data_scribe_structure_c::property_c::get_string16()
	{
		cheonsa_assert( _type == type_e_string16 );
		return *reinterpret_cast< string16_c * >( _value );
	}

	data_scribe_structure_c * data_scribe_structure_c::property_c::get_structure()
	{
		cheonsa_assert( _type == type_e_structure );
		return reinterpret_cast< data_scribe_structure_c * >( _value );
	}

	sint32_c data_scribe_structure_c::property_c::get_structure_list_count()
	{
		cheonsa_assert( _type == type_e_structure_list );
		return reinterpret_cast< core_list_c< data_scribe_structure_c * > * >( _value )->get_length();
	}

	data_scribe_structure_c * data_scribe_structure_c::property_c::add_structure_to_structure_list()
	{
		cheonsa_assert( _type == type_e_structure_list );
		data_scribe_structure_c * result = new data_scribe_structure_c();
		reinterpret_cast< core_list_c< data_scribe_structure_c * > * >( _value )->insert_at_end( result );
		return result;
	}

	data_scribe_structure_c * data_scribe_structure_c::property_c::get_structure_in_structure_list_at_index( sint32_c index )
	{
		cheonsa_assert( _type == type_e_structure_list );
		return (*reinterpret_cast< core_list_c< data_scribe_structure_c * > * >( _value ))[ index ];
	}

	sint8_c & data_scribe_structure_c::property_c::get_sint8()
	{
		cheonsa_assert( _type == type_e_sint8 && _type_count == 1 );
		return *reinterpret_cast< sint8_c * >( _value );
	}

	sint16_c & data_scribe_structure_c::property_c::get_sint16()
	{
		cheonsa_assert( _type == type_e_sint16 && _type_count == 1 );
		return *reinterpret_cast< sint16_c * >( _value );
	}

	sint32_c & data_scribe_structure_c::property_c::get_sint32()
	{
		cheonsa_assert( _type == type_e_sint32 && _type_count == 1 );
		return *reinterpret_cast< sint32_c * >( _value );
	}

	sint64_c & data_scribe_structure_c::property_c::get_sint64()
	{
		cheonsa_assert( _type == type_e_sint64 && _type_count == 1 );
		return *reinterpret_cast< sint64_c * >( _value );
	}

	uint8_c & data_scribe_structure_c::property_c::get_uint8()
	{
		cheonsa_assert( _type == type_e_uint8 && _type_count == 1 );
		return *reinterpret_cast< uint8_c * >( _value );
	}

	uint16_c & data_scribe_structure_c::property_c::get_uint16()
	{
		cheonsa_assert( _type == type_e_uint16 && _type_count == 1 );
		return *reinterpret_cast< uint16_c * >( _value );
	}

	uint32_c & data_scribe_structure_c::property_c::get_uint32()
	{
		cheonsa_assert( _type == type_e_uint32 && _type_count == 1 );
		return *reinterpret_cast< uint32_c * >( _value );
	}

	uint64_c & data_scribe_structure_c::property_c::get_uint64()
	{
		cheonsa_assert( _type == type_e_uint64 && _type_count == 1 );
		return *reinterpret_cast< uint64_c * >( _value );
	}

	float32_c & data_scribe_structure_c::property_c::get_float32()
	{
		cheonsa_assert( _type == type_e_float32 && _type_count == 1 );
		return *reinterpret_cast< float32_c * >( _value );
	}

	vector32x2_c & data_scribe_structure_c::property_c::get_float32x2()
	{
		cheonsa_assert( _type == type_e_float32 && _type_count == 2 );
		return *reinterpret_cast< vector32x2_c * >( _value );
	}

	vector32x3_c & data_scribe_structure_c::property_c::get_float32x3()
	{
		cheonsa_assert( _type == type_e_float32 && _type_count == 3 );
		return *reinterpret_cast< vector32x3_c * >( _value );
	}

	vector32x4_c & data_scribe_structure_c::property_c::get_float32x4()
	{
		cheonsa_assert( _type == type_e_float32 && _type_count == 4 );
		return *reinterpret_cast< vector32x4_c * >( _value );
	}

	box32x2_c & data_scribe_structure_c::property_c::get_rectangle32()
	{
		cheonsa_assert( _type == type_e_float32 && _type_count == 4 );
		return *reinterpret_cast< box32x2_c * >( _value );
	}

	float64_c & data_scribe_structure_c::property_c::get_float64()
	{
		cheonsa_assert( _type == type_e_float64 && _type_count == 1 );
		return *reinterpret_cast< float64_c * >( _value );
	}

	vector64x2_c & data_scribe_structure_c::property_c::get_float64x2()
	{
		cheonsa_assert( _type == type_e_float64 && _type_count == 2 );
		return *reinterpret_cast< vector64x2_c * >( _value );
	}

	vector64x3_c & data_scribe_structure_c::property_c::get_float64x3()
	{
		cheonsa_assert( _type == type_e_float64 && _type_count == 3 );
		return *reinterpret_cast< vector64x3_c * >( _value );
	}

	vector64x4_c & data_scribe_structure_c::property_c::get_float64x4()
	{
		cheonsa_assert( _type == type_e_float64 && _type_count == 4 );
		return *reinterpret_cast< vector64x4_c * >( _value );
	}

	box64x2_c & data_scribe_structure_c::property_c::get_rectangle64()
	{
		cheonsa_assert( _type == type_e_float64 && _type_count == 4 );
		return *reinterpret_cast< box64x2_c * >( _value );
	}

	data_scribe_structure_c::property_c::property_c( string16_c const & name, type_e type, uint8_c type_count )
		: _name()
		, _type( type )
		, _type_count( type_count )
		, _value( nullptr )
	{
		cheonsa_assert( type > type_e_none && type < type_e_count_ );
		if ( type == type_e_buffer )
		{
			cheonsa_assert( _type_count == 0 );
			_value = nullptr;
		}
		else if ( type == type_e_string8 )
		{
			cheonsa_assert( _type_count == 0 );
			_value = new string8_c();
		}
		else if ( type == type_e_string16 )
		{
			cheonsa_assert( _type_count == 0 );
			_value = new string16_c();
		}
		else if ( type == type_e_structure )
		{
			cheonsa_assert( _type_count == 0 );
			_value = new data_scribe_structure_c();
		}
		else if ( type == type_e_structure_list )
		{
			cheonsa_assert( _type_count == 0 );
			_value = new core_list_c< data_scribe_structure_c * >();
		}
		else
		{
			// plain old data.
			cheonsa_assert( _type_count > 0 );
			sint32_c property_type_base_size = get_type_base_size( _type );
			cheonsa_assert( property_type_base_size > 0 );
			_value = new uint8_c[ property_type_base_size * _type_count ];
		}
	}

	data_scribe_structure_c::property_c::~property_c()
	{
		if ( _type == type_e_buffer )
		{
			delete[] _value;
		}
		else if ( _type == type_e_string8 )
		{
			delete reinterpret_cast< string8_c * >( _value );
		}
		else if ( _type == type_e_string16 )
		{
			delete reinterpret_cast< string16_c * >( _value );
		}
		else if ( _type == type_e_structure )
		{
			delete reinterpret_cast< data_scribe_structure_c * >( _value );
		}
		else if ( _type == type_e_structure_list )
		{
			core_list_c< data_scribe_structure_c * > * struct_list = reinterpret_cast< core_list_c< data_scribe_structure_c * > * >( _value );
			for ( sint32_c i = 0; i < struct_list->get_length(); i++ )
			{
				delete (*struct_list)[ i ];
			}
			delete struct_list;
		}
		else
		{
			// plain old data deleter.
			delete[] _value;
		}
		_value = nullptr;
	}

	data_scribe_structure_c::property_c * data_scribe_structure_c::find_property( string16_c const & name, property_c::type_e type, uint8_c type_count )
	{
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			property_c * property = _property_list[ i ];
			if ( property->_name == name )
			{
				if ( type == property_c::type_e_none )
				{
					return property;
				}
				else if ( property->_type == type && property->_type_count == type_count )
				{
					return property;
				}
			}
		}
		return nullptr;
	}

	void_c data_scribe_structure_c::remove_property( property_c * property )
	{
		cheonsa_assert( _property_list.remove( property ) );
		delete property;
	}

	data_scribe_structure_c::property_c * data_scribe_structure_c::add_property( string16_c const & name, property_c::type_e type, uint8_c type_count )
	{
		property_c * result = new property_c( name, type, type_count );
		_property_list.insert_at_end( result );
		return result;
	}

	void_c data_scribe_structure_c::save( data_scribe_binary_c * scribe_binary )
	{
		scribe_binary->save_uint32( _property_list.get_length() );
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			property_c * property = _property_list[ i ];
			scribe_binary->save_string8( string8_c( property->_name ) );
			scribe_binary->save_uint8( property->_type );
			if ( property->_type == property_c::type_e_buffer )
			{
				if ( property->_value == nullptr )
				{
					scribe_binary->save_sint32( 0 );
					scribe_binary->save_sint32( 0 );
				}
				else
				{
					scribe_binary->save_sint32( property->get_buffer_array_count() );
					scribe_binary->save_sint32( property->get_buffer_data_size() );
					scribe_binary->get_stream()->save( property->get_buffer_data(), property->get_buffer_data_size() );
				}
			}
			else if ( property->_type == property_c::type_e_string8 )
			{
				scribe_binary->save_string8( property->get_string8() );
			}
			else if ( property->_type == property_c::type_e_string16 )
			{
				scribe_binary->save_string16( property->get_string16() );
			}
			else if ( property->_type == property_c::type_e_structure )
			{
				property->get_structure()->save( scribe_binary );
			}
			else if ( property->_type == property_c::type_e_structure_list )
			{
				scribe_binary->save_sint32( property->get_structure_list_count() );
				for ( sint32_c j = 0; j < property->get_structure_list_count(); j++ )
				{
					property->get_structure_in_structure_list_at_index( j )->save( scribe_binary );
				}
			}
			else
			{
				cheonsa_assert( property->_type_count > 0 );
				scribe_binary->save_uint8( property->_type_count );
				sint32_c property_type_base_size = property_c::get_type_base_size( property->_type );
				cheonsa_assert( property_type_base_size > 0 );
				uint8_c const * value = reinterpret_cast< uint8_c const * >( property->_value );
				for ( sint32_c j = 0; j < property->_type_count; j++ )
				{
					scribe_binary->save_generic( value, property_type_base_size );
					value += property_type_base_size;
				}
			}
		}
	}

	void_c data_scribe_structure_c::load( data_scribe_binary_c * scribe_binary )
	{
		_property_list.construct_mode_dynamic( scribe_binary->load_sint32() );
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			string16_c property_name = string16_c( scribe_binary->load_string8() );
			property_c::type_e property_type = static_cast< property_c::type_e >( scribe_binary->load_uint8() );
			uint32_c property_type_base_size = property_c::get_type_base_size( property_type );
			uint8_c property_type_count = property_type_base_size > 0 ? scribe_binary->load_uint8() : 0;
			property_c * property = new property_c( property_name, property_type, property_type_count );
			_property_list[ i ] = property;
			if ( property->_type == property_c::type_e_buffer )
			{
				sint32_c array_count = scribe_binary->load_sint32();
				sint32_c data_size = scribe_binary->load_sint32();
				property->set_buffer( array_count, scribe_binary->get_stream(), data_size );
			}
			else if ( property_type == property_c::type_e_string8 )
			{
				property->get_string8() = scribe_binary->load_string8();
			}
			else if ( property_type == property_c::type_e_string16 )
			{
				property->get_string16() = scribe_binary->load_string16();
			}
			else if ( property_type == property_c::type_e_structure )
			{
				property->get_structure()->load( scribe_binary );
			}
			else if ( property_type == property_c::type_e_structure_list )
			{
				sint32_c count = scribe_binary->load_sint32();
				for ( sint32_c j = 0; j < count; j++ )
				{
					property->add_structure_to_structure_list()->load( scribe_binary );
				}
			}
			else
			{
				cheonsa_assert( property_type_count > 0 );
				cheonsa_assert( property_type_base_size > 0 );
				uint8_c * value = reinterpret_cast< uint8_c * >( property->_value );
				for ( uint32_c i = 0; i < property_type_count; i++ )
				{
					scribe_binary->load_generic( value, property_type_base_size );
					value += property_type_base_size;
				}
			}
		}
	}

	data_scribe_structure_c::data_scribe_structure_c()
		: _property_list()
	{
	}

	data_scribe_structure_c::~data_scribe_structure_c()
	{
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			delete _property_list[ i ];
		}
	}

}