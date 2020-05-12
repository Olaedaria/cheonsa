#pragma once

#include "cheonsa_types.h"
#include <cassert>

namespace cheonsa
{

	// provides a first in first out rolling window structure.
	// this is useful when you want to feed a work load from one thread to another thread without worrying about [critical sections|mutually exclusive (mutex) access|thread locking].
	// as long as the worker thread keeps up with the work load then the buffer will never fill up.
	// if the buffer does fill up, then the feeder thread should be able to deal with it appropriately.
	template< typename value_type_c >
	class core_circle_buffer_c
	{
	public:
		value_type_c * _array;
		sint32_c _capacity;
		sint32_c _push_index; // index within _array that is where the next value will be pushed to.
		sint32_c _pop_index; // index within _array that is where the next value will be popped from.
		sint32_c _length;

	public:
		core_circle_buffer_c( sint32_c capacity )
		{
			assert( capacity > 0 );
			_array = new value_type_c[ capacity ];
			_capacity = capacity;
			_push_index = 0;
			_pop_index = 0;
			_length = 0;
		}

		// gets the maximum number of items that can be stored in the buffer.
		sint32_c get_capacity() const
		{
			return _capacity;
		}

		// gets the number of items that are currently stored in the buffer.
		sint32_c get_length() const
		{
			return _length;
		}

		// pushes a value to the end of the buffer and returns a reference to it.
		// asserts if the buffer is already full.
		value_type_c & emplace_back()
		{
			assert( _length < _capacity );
			_length++;
			sint32_c push_index = _push_index;
			_push_index = ( _push_index + 1 ) % _capacity;
			return _array[ push_index ];
		}

		// pushes a value to the end of the buffer.
		// asserts if the buffer is already full.
		void_c push_back( value_type_c value )
		{
			assert( _length < _capacity );
			_length++;
			_array[ _push_index ] = value;
			_push_index = ( _push_index + 1 ) % _capacity;
		}

		// removes the value at the start of the circle buffer (the oldest value) and returns a copy of it.
		// asserts if the buffer is empty.
		value_type_c pop_front()
		{
			assert( _length > 0 );
			value_type_c result = _array[ _pop_index ];
			_pop_index = ( _pop_index + 1 ) % _capacity;
			_length--;
			return result;
		}

		// index is a value between 0 and _length.
		// value at index 0 is the oldest value, the value at _pop_index.
		value_type_c & operator [] ( sint32_c index )
		{
			assert( index >= 0 && index < _length );
			return _array[ ( _pop_index + index ) % _capacity ];
		}

	};

}
