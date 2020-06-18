#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	// these are templated dictionary hash functions, used to convert a value to a hash that can be used to index buckets in the dictionary.
	// there was a time when i used function pointers to define custom hash functions, but this way is easier but less flexible.
	template< typename type_c > 
	struct core_hasher
	{
		static uint32_c hash( type_c const & value )
		{
			return ops::xxhash32( &value, sizeof( type_c ) );
		};
	};

	template<>
	struct core_hasher< string8_c >
	{
		static uint32_c hash( string8_c const & value )
		{
			return ops::xxhash32( value.character_list.get_internal_array(), value.character_list.get_internal_array_size() );
		}
	};

	template<>
	struct core_hasher< string16_c >
	{
		static uint32_c hash( string16_c const & value )
		{
			return ops::xxhash32( value.character_list.get_internal_array(), value.character_list.get_internal_array_size() );
		}
	};

	// dictionary, stores values mapped by keys, so that values can be looked up by their keys.
	// imlemented like an unordered map, uses buckets, hashes keys to find bucket indices.
	// if key_type_c is a class type then it needs to implement operator = and operator ==.
	// if value_type_c is a class type then it may need to implement operator =.
	template < typename key_type_c, typename value_type_c >
	class core_dictionary_c
	{
	public:
		class entry_c
		{
		public:
			key_type_c key;
			value_type_c value;

		public:
			entry_c()
			{
			}

			entry_c( key_type_c const & key )
			{
				// use copy operator instead of copy constructor
				this->key = key;
			}

			entry_c( key_type_c const & key, value_type_c const & value )
			{
				// use copy operator instead of copy constructor
				this->key = key;
				this->value = value;
			}

			entry_c & operator = ( entry_c const & other )
			{
				key = other.key;
				value = other.value;
				return *this;
			}

		};

		class iterator_c
		{
		public:
			core_dictionary_c const * _dictionary;
			sint32_c _current_bucket;
			sint32_c _current_entry;

		public:
			iterator_c()
				: _dictionary( nullptr )
				, _current_bucket( -1 )
				, _current_entry( -1 )
			{
			}

			boolean_c next()
			{
				if ( _current_bucket == -1 )
				{
					goto next_bucket;
				}
				else if ( _current_bucket == _dictionary->_buckets.get_length() )
				{
					return false;
				}

				// go to next entry.
				_current_entry++;
				if ( _current_entry == _dictionary->_buckets[ _current_bucket ].get_length() )
				{
					goto next_bucket;
				}
				return true;

			next_bucket:
				// go to next occupied bucket.
				_current_bucket++;
				while ( _current_bucket < _dictionary->_buckets.get_length() )
				{
					if ( _dictionary->_buckets[ _current_bucket ].get_length() )
					{
						_current_entry = 0;
						return true;
					}
					_current_bucket++;
				}
				return false;
			}

			key_type_c const & get_key() const
			{
				return _dictionary->_buckets[ _current_bucket ][ _current_entry ].key;
			}

			value_type_c const & get_value() const
			{
				return _dictionary->_buckets[ _current_bucket ][ _current_entry ].value;
			}

		};

		// returns an iterator placed before the first element.
		// use like:
		// core_dictionary_c< x, x >::iterator_c iterator = my_dictionary.get_iterator();
		// while ( iterator.next() ) { do stuff }
		iterator_c get_iterator() const
		{
			iterator_c result;
			result._dictionary = this;
			result._current_bucket = -1;
			result._current_entry = 0;
			return result;
		}

	private:
		core_list_c< core_list_c< entry_c > > _buckets;
		sint32_c _length; // number of entries stored in this dictionary.

		// resizes the buckets list.
		void_c _set_buckets_count( sint32_c count )
		{
			core_list_c< core_list_c< entry_c > > old_buckets;
			_buckets.transfer_to( old_buckets );
			_buckets.construct_mode_dynamic( count, count );
			for ( sint32_c i = 0; i < old_buckets.get_length(); i++ )
			{
				core_list_c< entry_c > & old_bucket = old_buckets[ i ];
				for ( sint32_c j = 0; j < old_bucket.get_length(); j++ )
				{
					entry_c const & entry = old_bucket[ j ];
					core_list_c< entry_c > & bucket = _buckets[ core_hasher< key_type_c >::hash( entry.key ) % _buckets.get_length() ];
					bucket.insert( -1, entry );
				}
			}
		}

	public:
		core_dictionary_c()
			: _buckets()
			, _length( 0 )
		{
			_buckets.set_length_absolute( 32 );
		}

		~core_dictionary_c()
		{
		}

		core_dictionary_c( core_dictionary_c const & other ) = delete;

		core_dictionary_c & operator = ( core_dictionary_c const & ) = delete;

		// finds an item in the dictionary, and optionally gets its specific address in the dictionary.
		boolean_c find_entry( key_type_c const & key, core_list_c< entry_c > * * out_bucket, sint32_c * out_entry_index, entry_c * * out_entry ) const
		{
			core_list_c< entry_c > const & bucket = _buckets[ core_hasher< key_type_c >::hash( key ) % _buckets.get_length() ];
			for ( sint32_c i = 0; i < bucket.get_length(); i++ )
			{
				if ( bucket[ i ].key == key )
				{
					if ( out_bucket )
					{
						*out_bucket = const_cast< core_list_c< entry_c > * >( &bucket );
					}
					if ( out_entry_index )
					{
						*out_entry_index = i;
					}
					if ( out_entry )
					{
						*out_entry = const_cast< entry_c * >( &bucket[ i ] );
					}
					return true;
				}
			}
			return false;
		}

		// inserts new or replaces existing value with key.
		boolean_c insert( key_type_c const & key, value_type_c const & value )
		{
			if ( _length / _buckets.get_length() > 16 )
			{
				_set_buckets_count( _buckets.get_length() * 2 );
			}

			entry_c * entry = nullptr;
			if ( find_entry( key, nullptr, nullptr, &entry ) )
			{
				const_cast< entry_c * >( entry )->value = value;
			}
			else
			{
				core_list_c< entry_c > & bucket = _buckets[ core_hasher< key_type_c >::hash( key ) % _buckets.get_length() ];
				entry_c * entry = bucket.emplace( -1, 1 );
				entry->key = key;
				entry->value = value;
				_length++;
			}
			return true;
		}

		// removes existing key and value with key.
		boolean_c remove( key_type_c const & key )
		{
			core_list_c< entry_c > * bucket = nullptr;
			sint32_c entry_index = 0;
			if ( !find_entry( key, &bucket, &entry_index, nullptr ) )
			{
				return false;
			}
			bucket->remove( entry_index, 1 );
			_length--;
			return true;
		}

		// removes existing key and value with key, and deletes value.
		boolean_c remove_and_delete( key_type_c const & key )
		{
			core_list_c< entry_c > * bucket = nullptr;
			sint32_c entry_index = 0;
			entry_c * entry = nullptr;
			if ( !find_entry( key, &bucket, &entry_index, &entry ) )
			{
				return false;
			}
			delete entry->value;
			bucket->remove( entry_index, 1 );
			_length--;
			return true;
		}

		// removes all values from this collection.
		void_c remove_all()
		{
			if ( _length > 0 )
			{
				for ( sint32_c i = 0; i < _buckets.get_length(); i++ )
				{
					_buckets[ i ].remove_all();
				}
				_length = 0;
			}
		}

		// removes and deletes all values from this collection.
		// stored values must have been allocated with new for this to work.
		void_c remove_and_delete_all()
		{
			if ( _length > 0 )
			{
				for ( sint32_c i = 0; i < _buckets.get_length(); i++ )
				{
					core_list_c< entry_c > & bucket = _buckets[ i ];
					for ( sint32_c j = 0; j < bucket.get_length(); j++ )
					{
						delete bucket[ j ].value;
					}
					bucket.remove_all();
				}
				_length = 0;
			}
		}

		// returns the value if it exists or nullptr if it doesn't.
		// intended for use if value_type_c is a pointer type (rather than a value type).
		value_type_c find_value_else_nullptr( key_type_c const & key ) const
		{
			if ( _length > 0 )
			{
				entry_c * entry = nullptr;
				if ( find_entry( key, nullptr, nullptr, &entry ) )
				{
					return entry->value;
				}
			}
			return nullptr;
		}

		// returns a pointer to the value if key exists or nullptr if it doesn't.
		// treat the returned pointer as temporary, because if/when the dictionary resizes then that pointer will become invalid.
		// intended for use if value_type_c is a value type (rather than a pointer type).
		value_type_c * find_value_pointer( key_type_c const & key ) const
		{
			if ( _length > 0 )
			{
				entry_c * entry = nullptr;
				if ( find_entry( key, nullptr, nullptr, &entry ) )
				{
					return &entry->value;
				}
			}
			return nullptr;
		}

		// returns a reference to the value if it exists or asserts if it doesn't.
		// treat the reference as volatile, use it for what you need and then discard it, when the dictionary resizes then that reference will be invalid.
		value_type_c & find_value_reference( key_type_c const & key ) const
		{
			entry_c * entry = nullptr;
			assert( find_entry( key, nullptr, nullptr, &entry ) );
			return entry->value;
		}

		// returns a copy of the value if it exists or asserts if it doesn't.
		value_type_c find_value( key_type_c const & key ) const
		{
			entry_c * entry = nullptr;
			assert( find_entry( key, nullptr, nullptr, &entry ) );
			return entry->value;
		}

		// returns true if this dictionary contains the given key.
		boolean_c contains( key_type_c const & key ) const
		{
			return find_entry( key, nullptr, nullptr, nullptr );
		}

		// gets the number of items stored in this collection.
		sint32_c get_length() const
		{
			return _length;
		}

		// gets a copy of all the keys in this dictionary (in the order that they were bucketed in which probably isn't useful).
		core_list_c< key_type_c > get_key_list() const
		{
			core_list_c< key_type_c > result;
			for ( sint32_c i = 0; i < _buckets.get_length(); i++ )
			{
				core_list_c< entry_c > const & bucket = _buckets[ i ];
				for ( sint32_c j = 0; j < bucket.get_length(); j++ )
				{
					result.insert( -1, bucket[ j ].key );
				}
			}
			return result;
		}

		// gets all of the keys in this dictionary.
		// in cases where pointers is cheaper than copying by value.
		// treat these pointers as volatile, as they will no longer be valid after the dictionary is modified.
		core_list_c< key_type_c const * > get_key_pointer_list() const
		{
			core_list_c< key_type_c const * > result;
			for ( sint32_c i = 0; i < _buckets.get_length(); i++ )
			{
				core_list_c< entry_c > const & bucket = _buckets[ i ];
				for ( sint32_c j = 0; j < bucket.get_length(); j++ )
				{
					result.insert( -1, &bucket[ j ].key );
				}
			}
			return result;
		}

		// gets a copy of all the values in this dictionary (in the order that they were bucketed in).
		core_list_c< value_type_c > get_value_list()
		{
			core_list_c< value_type_c > result;
			for ( sint32_c i = 0; i < _buckets.get_length(); i++ )
			{
				core_list_c< entry_c > const & bucket = _buckets[ i ];
				for ( sint32_c j = 0; j < bucket.get_length(); j++ )
				{
					result.insert( -1, bucket[ j ].value );
				}
			}
			return result;
		}

		// transfers ownership of internal arrays from this to other, converts this to an empty dictionary as a result.
		// the cases to use this are specific, but it's more optimal than the assignment operator because no heap reallocations or memory copies are performed.
		void_c transfer_to( core_dictionary_c< key_type_c, value_type_c > & other )
		{
			core_dictionary_c< key_type_c, value_type_c > & self = *this;
			self._buckets.transfer_to( other._buckets );
			other._length = self._length;
			self._buckets.set_length( 32 );
			self._length = 0;
		}

	};

}
