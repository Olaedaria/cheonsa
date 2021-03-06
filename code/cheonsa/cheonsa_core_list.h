#pragma once

#include "cheonsa__types.h"
#include <cassert>

namespace cheonsa
{

	// list (and by extension strings) have a layer of complexity called modes.
	// this is used to avoid allocating memory on the heap, in cases where the block of memory can be referenced from somewhere else (be it the stack or heap).
	// when copying a core_list_mode_e_static mode list, we copy the array reference, which is cheap.
	// when copying core_list_mode_e_static_volatile and core_list_mode_e_dynamic, we have to allocate a new array on the heap and then copy the values from the original to it, which is not as cheap.
	enum core_list_mode_e
	{
		core_list_mode_e_static_volatile = -2, // this list instance is constructed around a reference to a stack|heap allocated string|array that we can not rely on to remain in scope for the life time of any copies made, so any copies that are made via assignment operator will create their copy on the heap.
		core_list_mode_e_static = -1, // this list instance is constructed around a reference to a stack|heap allocated string|array that we can rely on to remain in scope for the life time of the list instance, so if the list instance is ever copied via assignment operator then the reference is copied to the new instance rather than allocating a new copy on the heap.
		core_list_mode_e_dynamic = 0, // this list instance is volatile and manages its own heap allocated string|array so if the list instance is ever copied via assignment operator then a new copy will be allocated on the heap.
	};

	// a collection that manages an internal heap allocated array of consecutive values.
	// ideal for random element access by index, and for inserting and removing values at the end of the list.
	// not as ideal for inserting and removing values in the middle of the list.
	// try to use the list to only store things that can be copied by value.
	// if storing objects, prefer to store them by their pointers instead of by value, and manage that memory yourself, because constructors of emplaced values and destructors of removed values are not called.
	template< typename value_type_c >
	class core_list_c
	{
	private:
		value_type_c * _array; // heap allocated array of values.
		sint32_c _array_length_used; // number of value slots used in the array, which is less than or equal to _array_length_allocated.
		sint32_c _array_length_allocated; // this is the number of value slots allocated in the array. if >= 0 then indicates that this list is dynamic mode. if set to -1 (core_list_mode_e_static) or -2 (core_list_mode_e_static_volatile) then indicates that this is a static mode list.

		// if this function is defined, then it will be used instead of the type's assignment operator, during internal array resizes/reallocations.
		// this function is useful if your value_type_c is a class value type (non-pointer), and object instances of that class in turn allocate stuff on the heap (for example if that class contains core_list_c, string8_c, or string16_c instances).
		// this function lets you define a function that transfers heap ownerships from the old instance to the new instance (which is cheap), which avoids allocating more memory from the heap and doing copies (which is not as cheap).
		void_c (*_move_list_item_function)( value_type_c & from, value_type_c & to );

	private:
		// resizes the internal _array to length_to_allocate, existing values up to _array_length_used is preserved.
		// if length_to_allocate is 0 then the internal array is deleted.
		// if length_to_allocate is greater than 0 then the internal array is reallocated.
		// can be used to convert from static mode to dynamic mode.
		// if length_to_allocate is 0 then _array_length_used's value is set to 0.
		// if length_to_allocate is greater than 0 then _array_length_used must be less than or equal to length_to_allocate and _array_length_used's value is preserved.
		void_c _reallocate_to_exact_length( sint32_c length_to_allocate )
		{
			assert( length_to_allocate >= 0 );
			if ( length_to_allocate > 0 )
			{
				//assert( _array_length_used <= length_to_allocate ); // if caller is down sizing, let them clip the count before they call us, for paranoia's sake.
				value_type_c * new_array = new value_type_c[ length_to_allocate ];
				if ( _move_list_item_function )
				{
					for ( sint32_c i = 0; i < _array_length_used; i++ )
					{
						(*_move_list_item_function)( _array[ i ], new_array[ i ] );
					}
				}
				else
				{
					for ( sint32_c i = 0; i < _array_length_used; i++ )
					{
						new_array[ i ] = _array[ i ];
					}
				}
				if ( _array_length_allocated > 0 )
				{
					delete[] _array;
				}
				_array = new_array;
				_array_length_allocated = length_to_allocate;
			}
			else
			{
				if ( _array_length_allocated > 0 )
				{
					delete[] _array;
				}
				_array = nullptr;
				_array_length_allocated = 0;
				_array_length_used = 0;
			}
		}

		// resizes internal array if needed, and probably with extra slots so there is room for insertions to happen before another resize is forced.
		// if the list is currently in static mode, then this will convert it to dynamic mode.
		inline void_c _reallocate_to_fuzzy_length( sint32_c length_required )
		{
			if ( length_required > 0 && length_required > _array_length_allocated )
			{
				// this allocation strategy is taken from python's list growth pattern.
				// growth pattern: 0, 4, 8, 16, 24, 32, 40, 52, 64, 76, ...
				uint32_c new_length = ( (uint32_c)length_required + ( length_required >> 3 ) + 6 ) & ~(uint32_c)3;
				_reallocate_to_exact_length( new_length );
			}
		}

		// shifts element values at index by amount.
		// if amount is positive then elements will be shifted towards the end of the list. used to make room for new values in the middle of the list.
		// if amount is negative then elements will be shifted towards the start of the list. used to remove values from the middle of the list.
		void_c _shift( sint32_c index, sint32_c amount )
		{
			assert( index >= 0 && index < _array_length_used );
			if ( amount > 0 )
			{
				assert( index + amount < _array_length_allocated );
				value_type_c * a = &_array[ _array_length_used - 1 ]; // from.
				value_type_c * b = &_array[ _array_length_used - 1 + amount ]; // to.
				sint32_c count = _array_length_used - index;
				for ( sint32_c i = 0; i < count; i++ )
				{
					*b-- = *a--;
				}
			}
			else if ( amount < 0 )
			{
				assert( index + amount >= 0 );
				value_type_c * a = &_array[ index ];
				value_type_c * b = &_array[ index + amount ];
				sint32_c count = _array_length_used - index;
				for ( sint32_c i = 0; i < count; i++ )
				{
					*b++ = *a++;
				}
			}
		}

	public:
		// initializes an empty dynamic mode list.
		core_list_c()
			: _array( nullptr )
			, _array_length_used( 0 )
			, _array_length_allocated( 0 )
			, _move_list_item_function( nullptr )
		{
		}

		// initializes as a copy of the other instance.
		core_list_c( core_list_c const & other )
			: _array( nullptr )
			, _array_length_used( 0 )
			, _array_length_allocated( 0 )
			, _move_list_item_function( nullptr )
		{
			operator = ( other );
		}

		// constructs this list as static mode or dynamic mode.
		// array may be nullptr and array_length may be 0.
		core_list_c( core_list_mode_e mode, value_type_c const * array, sint32_c array_length )
			: _array( nullptr )
			, _array_length_used( 0 )
			, _array_length_allocated( 0 )
			, _move_list_item_function( nullptr )
		{
			assert( mode == core_list_mode_e_static_volatile || mode == core_list_mode_e_static || mode == core_list_mode_e_dynamic );
			assert( array_length >= 0 );
			if ( array_length == 0 )
			{
				assert( array == nullptr );
			}
			else
			{
				assert( array );
			}
			if ( mode < 0 )
			{
				_array = const_cast< value_type_c * >( array );
				_array_length_used = array_length;
				_array_length_allocated = static_cast< sint32_c >( mode );
			}
			else if ( mode >= 0 )
			{
				_reallocate_to_fuzzy_length( array_length );
				_array_length_used = array_length;
				for ( sint32_c i = 0; i < array_length; i++ )
				{
					_array[ i ] = array[ i ];
				}
			}
		}

		// release memory if needed.
		~core_list_c()
		{
			_reallocate_to_exact_length( 0 );
			_array = nullptr;
			_array_length_used = 0;
			_array_length_allocated = 0;
		}

		void_c set_list_item_move_function( void_c (*list_item_move_function)( value_type_c & in, value_type_c & out ) )
		{
			_move_list_item_function = list_item_move_function;
		}

		core_list_mode_e get_mode()
		{
			return _array_length_allocated >= 0 ? core_list_mode_e_dynamic : static_cast< mode_e >( _array_length_allocated );
		}

		boolean_c get_mode_is_static() const
		{
			return _array_length_allocated < 0;
		}

		boolean_c get_mode_is_dynamic() const
		{
			return _array_length_allocated >= 0;
		}

		// constructs this list as a static volatile mode list.
		// static means that it wraps an existing array rather than copying it.
		// volatile means that if the list is copied with the assignment operator then the copy will be a new dynamic mode copy.
		void_c construct_mode_static_volatile_from_array( value_type_c const * array, sint32_c array_length )
		{
			_reallocate_to_exact_length( 0 );
			_array = const_cast< value_type_c * >( array );
			_array_length_used = array_length;
			_array_length_allocated = static_cast< sint32_c >( core_list_mode_e_static_volatile );
		}

		// constructs this list as a static mode list.
		// static means that it wraps an existing array rather than copying it.
		void_c construct_mode_static_from_array( value_type_c const * array, sint32_c array_length )
		{
			_reallocate_to_exact_length( 0 );
			_array = const_cast< value_type_c * >( array );
			_array_length_used = array_length;
			_array_length_allocated = static_cast< sint32_c >( core_list_mode_e_static );
		}

		// constructs dynamic mode list of the given length and allocates the internal array to length_to_allocate.
		// if length <= _array_length_allocated and length_to_allocate < 0, then this function preserves the existing internal array allocation and simply sets _array_length_used to length.
		// if length > _array_length_allocated and length_to_allocate < 0, then this function reallocates the internal array to an ideal length with room to grow.
		// consider the contents of this list to be undefined and invalid after calling this function, so be sure to set the values if needed.
		// do not expect destructors of removed item object instances to be called.
		void_c construct_mode_dynamic( sint32_c length, sint32_c length_to_allocate = -1 )
		{
			assert( length >= 0 );
			assert( length_to_allocate < 0 || length_to_allocate >= length );
			if ( length_to_allocate < 0 )
			{
				_reallocate_to_exact_length( 0 );
				_reallocate_to_fuzzy_length( length );
			}
			else
			{
				_reallocate_to_exact_length( 0 );
				_reallocate_to_exact_length( length_to_allocate );
			}
			_array_length_used = length;
		}

		// converts this list to a dynamic mode list and populates its values from array and array_length.
		// reallocates if needed, otherwise uses exsisting allocation.
		void_c construct_mode_dynamic_from_array( value_type_c const * array, sint32_c array_length )
		{
			assert( array );
			assert( array_length > 0 );
			if ( array_length > _array_length_allocated )
			{
				_reallocate_to_fuzzy_length( array_length );
			}
			for ( sint32_c i = 0; i < array_length; i++ )
			{
				_array[ i ] = array[ i ];
			}
			_array_length_used = array_length;
		}

		// allocates a new internal array and preserves existing contents.
		// can only be called if this list is currently static mode, because it's pointless to do otherwise.
		void_c convert_from_static_to_dynamic()
		{
			assert( _array_length_allocated < 0 );
			_reallocate_to_exact_length( _array_length_used );
		}

		// copies the values from another list into this list.
		// ensure that value_type_c has an appropriate = operator.
		// if the other list was core_list_mode_e_static, then this list will be static mode wrap around the same data.
		// if the other list was core_list_mode_e_static_volatile, then this list will be a new dynamic mode copy.
		// if the other list was core_list_mode_e_dynamic, then this list will be a new dynamic mode copy.
		core_list_c< value_type_c > & operator = ( core_list_c< value_type_c > const & other )
		{
			if ( other._array_length_allocated == core_list_mode_e_static )
			{
				_reallocate_to_exact_length( 0 );
				_array = other._array;
				_array_length_used = other._array_length_used;
				_array_length_allocated = other._array_length_allocated;
			}
			else
			{
				_reallocate_to_fuzzy_length( other._array_length_used );
				_array_length_used = other._array_length_used;
				for ( sint32_c i = 0; i < _array_length_used; i++ )
				{
					_array[ i ] = other._array[ i ];
				}
			}
			return *this;
		}

		// compares each item in both lists for inequality, returns false if any are not equal, false if otherwise.
		boolean_c operator == ( core_list_c< value_type_c > const & other ) const
		{
			if ( _array_length_used != other._array_length_used )
			{
				return false;
			}
			for ( sint32_c i = 0; i < _array_length_used; i++ )
			{
				if ( _array[ i ] != other._array[ i ] )
				{
					return false;
				}
			}
			return true;
		}

		// compares each item in both lists for inequality, returns true if any are not equal, false if otherwise.
		boolean_c operator != ( core_list_c< value_type_c > const & other ) const
		{
			if ( _array_length_used != other._array_length_used )
			{
				return true;
			}
			for ( sint32_c i = 0; i < _array_length_used; i++ )
			{
				if ( _array[ i ] != other._array[ i ] )
				{
					return true;
				}
			}
			return false;
		}

		// const index accessor.
		inline value_type_c const & operator [] ( sint32_c index ) const
		{
			assert( index >= 0 && index < _array_length_used );
			return _array[ index ];
		}

		// index accessor.
		inline value_type_c & operator [] ( sint32_c index )
		{
			assert( index >= 0 && index < _array_length_used );
			return _array[ index ];
		}

		// inserts a value at end of list.
		void_c insert_at_end( value_type_c const & value )
		{
			assert( _array_length_allocated >= 0 );
			_reallocate_to_fuzzy_length( _array_length_used + 1 );
			_array_length_used++;
			_array[ index ] = value;
		}

		// inserts a range of values at end of list.
		void_c insert_at_end( value_type_c const * values, sint32_c values_count )
		{
			assert( _array_length_allocated >= 0 );
			if ( values_count > 0 )
			{
				assert( values );
				_reallocate_to_fuzzy_length( _array_length_used + values_count );
				for ( sint32_c i = 0; i < values_count; i++, _array_length_used++ )
				{
					_array[ _array_length_used ] = values[ i ];
				}
			}
		}

		// inserts a value at index.
		// if index == -1, then inserts the value at the end of the list.
		void_c insert( sint32_c index, value_type_c const & value )
		{
			assert( _array_length_allocated >= 0 );
			assert( index >= -1 && index <= _array_length_used );
			if ( index == -1 )
			{
				index = _array_length_used;
			}
			_reallocate_to_fuzzy_length( _array_length_used + 1 );
			if ( index < _array_length_used )
			{
				_shift( index, 1 );
			}
			_array_length_used += 1;
			_array[ index ] = value;
		}

		// inserts a range of values at index and returns the pointer to the first inserted value.
		// if index == -1, then inserts the values at the end of the list.
		void_c insert( sint32_c index, value_type_c const * values, sint32_c values_count )
		{
			assert( _array_length_allocated >= 0 );
			assert( index >= -1 && index <= _array_length_used );
			if ( values_count > 0 )
			{
				assert( values );
				if ( index == -1 )
				{
					index = _array_length_used;
				}
				_reallocate_to_fuzzy_length( _array_length_used + values_count );
				if ( index < _array_length_used )
				{
					_shift( index, values_count );
				}
				_array_length_used += values_count;
				for ( sint32_c i = 0; i < values_count; i++ )
				{
					_array[ i + index ] = values[ i ];
				}
			}
		}

		// inserts a number of values at index and returns the pointer to the first inserted value.
		// if index == -1, then inserts the values at the end of the list.
		value_type_c * emplace( sint32_c index, sint32_c values_count )
		{
			assert( _array_length_allocated >= 0 );
			assert( index >= -1 && index <= _array_length_used );
			assert( values_count > 0 );
			if ( index == -1 )
			{
				index = _array_length_used;
			}
			_reallocate_to_fuzzy_length( _array_length_used + values_count );
			if ( index < _array_length_used )
			{
				_shift( index, values_count );
			}
			_array_length_used += values_count;
			return &_array[ index ];
		}

		// returns the index of the first occurrence of value.
		sint32_c find_index_of( value_type_c const & value ) const
		{
			for ( sint32_c i = 0; i < _array_length_used; i++ )
			{
				if ( _array[ i ] == value )
				{
					return i;
				}
			}
			return -1;
		}

		// removes the first occurrence of value.
		// do not expect destructors of removed item object instances to be called.
		boolean_c remove_value( value_type_c const value )
		{
			assert( _array_length_allocated >= 0 );
			sint32_c index = find_index_of( value );
			if ( index >= 0 )
			{
				remove( index, 1 );
				return true;
			}
			return false;
		}

		// removes a range of values starting at index.
		// if index == -1, then removes the values at the end of the list.
		// do not expect destructors of removed item object instances to be called.
		void_c remove( sint32_c index, sint32_c values_count )
		{
			assert( _array_length_allocated >= 0 );
			assert( index >= -1 && index + values_count <= _array_length_used );
			assert( values_count > 0 );
			if ( index == -1 )
			{
				index = _array_length_used - values_count;
			}
			if ( index + values_count < _array_length_used )
			{
				_shift( index + values_count, -values_count );
			}
			_array_length_used -= values_count;
		}

		// removes and deletes a range of values starting at index.
		// if index == -1, then removes the values at the end of the list.
		void_c remove_and_delete( sint32_c index, sint32_c values_count )
		{
			assert( _array_length_allocated >= 0 );
			assert( index >= -1 && index + values_count <= _array_length_used );
			assert( values_count > 0 );
			if ( index == -1 )
			{
				index = _array_length_used - values_count;
			}
			for ( sint32_c i = index; i < index + values_count; i++ )
			{
				delete _array[ i ];
				_array[ i ] = nullptr;
			}	
			if ( index + values_count < _array_length_used )
			{
				_shift( index + values_count, -values_count );
			}
			_array_length_used -= values_count;
		}

		// removes all values from this list, keeps the current allocated length.
		// if list is static mode then this converts it to dynamic mode.
		// destructors of removed values are not called.
		void_c remove_all()
		{
			if ( _array_length_allocated < 0 )
			{
				_reallocate_to_exact_length( 0 );
			}
			_array_length_used = 0;
		}

		// removes and deletes all values from this list, keeps the current allocated length.
		// if list is static mode then this converts it to dynamic mode.
		// a convenience function if you are using this list to store objects that were allocated on the heap with new.
		// destructors of removed values are called.
		void_c remove_and_delete_all()
		{
			for ( sint32_c i = 0; i < _array_length_used; i++ )
			{
				if ( _array[ i ] )
				{
					delete _array[ i ];
				}
			}
			if ( _array_length_allocated < 0 )
			{
				_reallocate_to_exact_length( 0 );
			}
			_array_length_used = 0;
		}

		// gets the internal array used to store values.
		inline value_type_c * get_internal_array()
		{
			return _array;
		}

		// gets the internal array used to store values.
		inline value_type_c const * get_internal_array() const
		{
			return _array;
		}

		// gets size in bytes of the used portion of the internal array, which is sizeof( value_type_c ) * _array_length_used.
		// this should only be called on dynamic mode lists.
		inline sint32_c get_internal_array_size() const
		{
			return sizeof( value_type_c ) * _array_length_used;
		}

		// gets the size in bytes of the total allocation for the internal array, which is sizeof( value_type_c ) * _array_length_allocated.
		// this should only be called on dynamic mode lists.
		inline sint32_c get_internal_array_size_allocated() const
		{
			assert( _array_length_allocated >= 0 );
			return sizeof( value_type_c ) * _array_length_allocated;
		}

		// gets the number of elements allocated by the internal array.
		// this should only be called on dynamic mode lists.
		// this may be greater than or equal to the number of values in the array.
		inline sint32_c get_length_allocated() const
		{
			assert( _array_length_allocated >= 0 );
			return _array_length_allocated;
		}

		// gets the number of values currently stored.
		inline sint32_c get_length() const
		{
			return _array_length_used;
		}

		// sets the length.
		// reallocates internal array to at least the provided length if needed (may allocate extra room for future growth).
		inline void_c set_length( sint32_c length )
		{
			assert( length >= 0 );
			if ( length > _array_length_allocated )
			{
				_reallocate_to_fuzzy_length( length );
			}
			_array_length_used = length;
		}

		// sets the length.
		// reallocates length of internal array to exactly length, and sets the length used to exactly length.
		// if sizing larger, then all existing values up to current length will be preserved.
		// if sizing smaller, then all existing values after the new length will be lost.
		inline void_c set_length_absolute( sint32_c length )
		{
			assert( length >= 0 );
			_reallocate_to_exact_length( length );
			_array_length_used = length;
		}

		// sets a range of values in this list. overwrites whatever is in those slots.
		void_c set_range_at_index( sint32_c index, sint32_c length, value_type_c * values )
		{
			assert( index + length < _array_length_used );
			sint32_c end = index + length;
			for ( sint32_c i = index; i < end; i++ )
			{
				_array[ i ] = *values;
				values++;
			}
		}

		// swaps values between index a and index b.
		void_c swap( sint32_c index_a, sint32_c index_b )
		{
			assert( index_a != index_b );
			assert( index_a >= 0 && index_a < _array_length_used );
			assert( index_b >= 0 && index_b < _array_length_used );
			value_type_c temp = _array[ index_a ];
			_array[ index_a ] = _array[ index_b ];
			_array[ index_b ] = temp;
		}

		// moves the value at index up (towards the beginning) of the list.
		boolean_c move_up_at_index( sint32_c index )
		{
			if ( index > 0 && index < _array_length_used )
			{
				swap( index, index - 1 );
				return true;
			}
			return false;
		}

		// moves the value at index down (towards the end) of the list.
		boolean_c move_down_at_index( sint32_c index )
		{
			if ( index >= 0 && index + 1 < _array_length_used )
			{
				swap( index, index + 1 );
				return true;
			}
			return false;
		}

		/*
		// sorts the values in this list using an insertion sort algorithm.
		// insertion sort is used for sorting strings since they don't have an absolute numeric value that we can sort by, they are compared to other strings and a relative valuation is calculated from that, so it's different depending on which strings are compared to each other.
		void_c insertion_sort( boolean_c (*is_b_less_than_a)( value_type_c const & a, value_type_c const & b ), boolean_c invert, sint32_c start = -1, sint32_c end = -1 )
		{
			if ( start == -1 )
			{
				start = 0;
			}
			if ( end == -1 )
			{
				end = _array_length_used;
			}
			assert( start >= 0 && end <= _array_length_used );
			for ( sint32_c i = start + 1; i < end; i++ )
			{
				value_type_c value = _array[ i ];
				sint32_c j = i - 1;
				if ( invert == false )
				{
					while ( j >= 0 && is_b_less_than_a( _array[ j ], value ) )
					{
						_array[ j + 1 ] = _array[ j ];
						j--;
					}
				}
				else
				{
					while ( j >= 0 && is_b_less_than_a( value, _array[ j ] ) )
					{
						_array[ j + 1 ] = _array[ j ];
						j--;
					}
				}
				_array[ j + 1 ] = value;
			}
		}
		*/

	public:
		template< typename sort_type_c >
		void_c quick_sort_1( sort_type_c (*value_function)( value_type_c const & a ), boolean_c invert, sint32_c start_index = -1, sint32_c end_index = -1 )
		{
			if ( start_index < 0 )
			{
				start_index = 0;
			}
			if ( end_index < 0 )
			{
				end_index = _array_length_used - 1;
			}
			if ( start_index < end_index )
			{
				sint32_c partition_border = _quick_sort_1_partition( value_function, invert, start_index, end_index );
				quick_sort_1( value_function, invert, start_index, partition_border );
				quick_sort_1( value_function, invert, partition_border + 1, end_index );
			}
		}

	private:
		template< typename sort_type_c >
		sint32_c _quick_sort_1_partition( sort_type_c (*value_function)( value_type_c const & a ), boolean_c invert, sint32_c lo_index, sint32_c hi_index )
		{
			assert( lo_index >= 0 && hi_index < _array_length_used );
			assert( lo_index < hi_index );
			sort_type_c pivot_value = value_function( _array[ ( lo_index + hi_index ) / 2 ] );
			sint32_c inner_lo_index = lo_index;
			sint32_c inner_hi_index = hi_index;
			while ( true )
			{
				while ( value_function( _array[ inner_lo_index ] ) < pivot_value )
				{
					inner_lo_index++;
				}
				while ( value_function( _array[ inner_hi_index ] ) > pivot_value )
				{
					inner_hi_index--	;
				}
				if ( inner_lo_index >= inner_hi_index )
				{
					return inner_hi_index;
				}
				value_type_c temp = _array[ inner_lo_index ];
				_array[ inner_lo_index ] = _array[ inner_hi_index ];
				_array[ inner_hi_index ] = temp;
				inner_lo_index++;
				inner_hi_index--;
			}
		}

	public:
		template< typename sort_type_c >
		void_c quick_sort_2( sort_type_c (*compare_function)( value_type_c const & a, value_type_c const & b ), boolean_c invert, sint32_c start_index = -1, sint32_c end_index = -1 )
		{
			if ( start_index < 0 )
			{
				start_index = 0;
			}
			if ( end_index < 0 )
			{
				end_index = _array_length_used - 1;
			}
			assert( start_index >= 0 && end_index < _array_length_used );
			if ( start_index < end_index )
			{
				sint32_c partition_border = _quick_sort_2_partition( compare_function, invert, start_index, end_index );
				quick_sort_2( compare_function, invert, start_index, partition_border );
				quick_sort_2( compare_function, invert, partition_border + 1, end_index );
			}
		}

	private:
		template< typename sort_type_c >
		sint32_c _quick_sort_2_partition( sort_type_c (*compare_function)( value_type_c const & a, value_type_c const & b ), boolean_c invert, sint32_c lo_index, sint32_c hi_index )
		{
			value_type_c const & pivot_value = _array[ ( lo_index + hi_index ) / 2 ];
			sint32_c inner_lo_index = lo_index;
			sint32_c inner_hi_index = hi_index;
			while ( true )
			{
				while ( compare_function( _array[ inner_lo_index ], pivot_value ) < 0 )
				{
					inner_lo_index++;
				}
				while ( compare_function( _array[ inner_hi_index ], pivot_value ) > 0 )
				{
					inner_hi_index--;
				}
				if ( inner_lo_index >= inner_hi_index )
				{
					return inner_hi_index;
				}
				value_type_c temp = _array[ inner_lo_index ];
				_array[ inner_lo_index ] = _array[ inner_hi_index ];
				_array[ inner_hi_index ] = temp;
				inner_lo_index++;
				inner_hi_index--;
			}
		}

		/*
		// recursive quick sort function.
		template< typename sort_type_c >
		void_c _quick_sort_internal( sort_type_c (*compare_function)( value_type_c const & a, value_type_c const & b ), boolean_c invet, sint32_c lo_index, sint32_c hi_index )
		{
			assert( lo_index >= 0 && hi_index < _array_length_used );
			assert( lo_index < hi_index );
			sint32_c inner_lo_index = lo_index;
			sint32_c inner_hi_index = hi_index;
			value_type_c pivot_value = _array[ lo_index + ( hi_index - lo_index ) / 2 ];
			while ( inner_lo_index < inner_hi_index )
			{
				while ( compare_function( _array[ inner_lo_index ], pivot_value ) < 0 )
				{
					inner_lo_index++;
				}
				while ( compare_function( _array[ inner_hi_index ], pivot_value ) > 0 )
				{
					inner_hi_index--;
				}
				if ( inner_lo_index <= inner_hi_index )
				{
					value_type_c temp = _array[ inner_lo_index ];
					_array[ inner_lo_index ] = _array[ inner_hi_index ];
					_array[ inner_hi_index ] = temp;
					inner_lo_index++;
					inner_hi_index--;
				}
				if ( lo_index < inner_hi_index )
				{
					_quick_sort_internal( compare_function, lo_index, inner_hi_index, invert );
				}
				if ( inner_lo_index < hi_index )
				{
					_quick_sort_internal( compare_function, inner_lo_index, hi_index, invert );
				}
			}
		}
		*/

	public:
		// transfers ownership of internal array from this to other, converts this to an empty dynamic mode list as a side effect of disassociating ownership.
		// the cases to use this are specific, but it's more optimal than the assignment operator because no heap reallocations or memory copies are performed.
		void_c transfer_to( core_list_c< value_type_c > & other )
		{
			core_list_c< value_type_c > & self = *this;
			if ( other._array_length_allocated > 0 )
			{
				delete[] other._array;
			}
			other._array = self._array; self._array = nullptr;
			other._array_length_used = self._array_length_used; self._array_length_used = 0;
			other._array_length_allocated = self._array_length_allocated; self._array_length_allocated = 0;
		}

	};

}
