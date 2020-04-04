#include "cheonsa_data_stream_memory.h"
#include <cassert>

namespace cheonsa
{

	data_stream_memory_c::data_stream_memory_c()
		: _buffer()
		, _position( -1 )
	{
		_buffer.construct_mode_dynamic( 0, 1024 );
	}

	data_stream_memory_c::data_stream_memory_c( data_stream_memory_c const & other )
		: _buffer( other._buffer )
		, _position( other._position )
	{
		*this = other;
	}

	data_stream_memory_c::~data_stream_memory_c()
	{
		if ( _position >= 0 )
		{
			close();
		}
	}

	boolean_c data_stream_memory_c::get_is_open()
	{
		return _position != -1;
	}

	void_c data_stream_memory_c::open()
	{
		assert( _position == -1 );
		_buffer.remove_all();
		_position = 0;
	}

	void_c data_stream_memory_c::open_static( void_c const * data, sint32_c data_size )
	{
		assert( _position == -1 );
		_buffer.construct_mode_static_from_array( reinterpret_cast< uint8_c const * >( data ), data_size );
		_position = 0;
	}

	void_c data_stream_memory_c::close()
	{
		assert( _position >= 0 );
		_buffer.remove_all();
		_position = -1;
	}

	core_list_c< uint8_c > & data_stream_memory_c::get_internal_buffer()
	{
		return _buffer;
	}

	sint32_c data_stream_memory_c::get_size() const
	{
		return _buffer.get_length();
	}

	void_c data_stream_memory_c::set_size( sint32_c size )
	{
		_buffer.set_length( size );
	}

	sint32_c data_stream_memory_c::get_position() const
	{
		assert( _position >= 0 );
		return _position;
	}

	boolean_c data_stream_memory_c::set_position( sint32_c const position )
	{
		assert( _position >= 0 );
		assert( _position <= get_size() );
		if ( _position >= 0 && _position <= get_size() )
		{
			_position = position;
			return true;
		}
		return false;
	}

	boolean_c data_stream_memory_c::load( void_c * const data, sint32_c const size )
	{
		assert( _position >= 0 );
		assert( _position + size <= _buffer.get_length() );
		uint8_c const * in = &_buffer.get_internal_array()[ _position ];
		uint8_c * out = reinterpret_cast< uint8_c * >( data );
		for ( sint32_c i = 0; i < size; i++ )
		{
			*out++ = *in++;
		}
		_position += size;
		return true;
	}

	boolean_c data_stream_memory_c::save( void_c const * const data, sint32_c const size )
	{
		assert( _position >= 0 );
		assert( _buffer.get_mode_is_dynamic() );
		assert( _position <= _buffer.get_length() );
		sint32_c required_size = _position + size;
		if ( required_size > _buffer.get_length() )
		{
			_buffer.set_length( required_size );
		}
		uint8_c const * in = static_cast< uint8_c const * >( data );
		uint8_c * out = &_buffer.get_internal_array()[ _position ];
		for ( sint32_c i = 0; i < size; i++ )
		{
			*out++ = *in++;
		}
		_position += size;
		return true;
	}

}
