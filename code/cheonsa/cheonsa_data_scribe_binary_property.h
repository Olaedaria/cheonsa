#pragma once

#include "cheonsa_data_scribe_binary.h"

namespace cheonsa
{

	// provides a way to save and load named properties as a binary format.
	class data_scribe_binary_property_c
	{
	protected:
		static const sint32_c maximum_atomic_length = 16;

		data_scribe_binary_c * _scribe_binary; // binary stream that we are reading from or writing to.

		string8_c _loaded_key; // currently loaded key.

		uint8_c _loaded_type; // currently loaded value type.
		uint8_c _loaded_type_count; // number of consecutive elements in value.
		uint64_c _loaded_value_buffer[ maximum_atomic_length ]; // although this is typed as uint64_c, it is used to hold atomic values of all types.
		sint32_c _loaded_value_list_length; // currently loaded number of items in core_list_c.
		sint32_c _loaded_value_list_size; // currently loaded size in bytes of core_list_c.
		string8_c _loaded_value_string8;
		string16_c _loaded_value_string16;

		class list_instance_c
		{
		public:
			boolean_c handled;
			sint32_c offset;
			inline list_instance_c()
				: handled( false )
				, offset( 0 )
			{
			}
			inline list_instance_c( boolean_c handled, sint32_c offset )
				: handled( handled )
				, offset( offset )
			{
			}
		};
		core_list_c< list_instance_c > _list_instance_stack;

	public:
		data_scribe_binary_property_c();

		void_c open( data_scribe_binary_c * scribe_binary ); // initializes the scribe for reading from and/or writing to a stream.
		void_c close(); // resets this scribe's scribe state so that open can be called again.

		data_scribe_binary_c * get_scribe_binary() const;

		boolean_c load(); // loads the next property from the stream. returns false when the end of the property stream was reached.
		boolean_c loaded_check( char8_c const * expected_key, data_type_e expected_type, uint8_c expected_type_count = 1 ); // expected_length should be set to 1 if expected_type is list or string.

		string8_c const & loaded_key(); // gets the currently loaded property key.
		data_type_e loaded_type(); // gets the currently loaded property type.
		uint8_c loaded_type_length(); // gets the number of elements in type.

		void_c loaded_list_enter(); // call when the current property is a list to tell the property stream not to skip to the next property during next load() so that you can handle the properties in the core_list_c using this property stream.
		void_c loaded_list_exit(); // call you are done loading the list to position the property stream at the next property after the list.
		sint32_c loaded_list_length(); // gets the number of entries in the list.
		sint32_c loaded_list_size(); // gets the size in bytes of the current list.
		string8_c const & loaded_string8() const; // gets the value of the loaded utf-8 encoded string.
		string16_c const & loaded_string16() const; // gets the value of the loaded char16_c string.
		uint8_c loaded_uint8() const; // equivalent to calling loaded_uint8_buffer()[ 0 ] when loaded_type_length() == 1.
		sint8_c loaded_sint8() const; // equivalent to calling loaded_sint8_buffer()[ 0 ] when loaded_type_length() == 1.
		uint16_c loaded_uint16() const; // equivalent to calling loaded_uint16_buffer()[ 0 ] when loaded_type_length() == 1.
		sint16_c loaded_sint16() const; // equivalent to calling loaded_sint16_buffer()[ 0 ] when loaded_type_length() == 1.
		uint32_c loaded_uint32() const; // equivalent to calling loaded_uint32_buffer()[ 0 ] when loaded_type_length() == 1.
		sint32_c loaded_sint32() const; // equivalent to calling loaded_sint32_buffer()[ 0 ] when loaded_type_length() == 1.
		uint64_c loaded_uint64() const; // equivalent to calling loaded_uint64_buffer()[ 0 ] when loaded_type_length() == 1.
		sint64_c loaded_sint64() const; // equivalent to calling loaded_sint64_buffer()[ 0 ] when loaded_type_length() == 1.
		float32_c loaded_float32() const; // equivalent to calling loaded_float32_buffer()[ 0 ] when loaded_type_length() == 1.
		float64_c loaded_float64() const; // equivalent to calling loaded_float64_buffer()[ 0 ] when loaded_type_length() == 1.
		uint8_c const * loaded_uint8_buffer() const;
		sint8_c const * loaded_sint8_buffer() const;
		uint16_c const * loaded_uint16_buffer() const;
		sint16_c const * loaded_sint16_buffer() const;
		uint32_c const * loaded_uint32_buffer() const;
		sint32_c const * loaded_sint32_buffer() const;
		uint64_c const * loaded_uint64_buffer() const;
		sint64_c const * loaded_sint64_buffer() const;
		float32_c const * loaded_float32_buffer() const;
		float64_c const * loaded_float64_buffer() const;

		boolean_c save_list_begin( char8_c const * key ); // begins saving of a list type of property. the caller can save any type of data to the stream in between a matched pair of calls to save_list_begin() and save_list_end().
		boolean_c save_list_end( sint32_c length ); // finalizes saving the list type property, which involves writing the item length and the size of the list data in bytes. we supply the length (number of items in the list) here in save_list_end() because some times we don't know how many items are going to be written to the list when save_list_begin() is called.
		boolean_c save_string8( char8_c const * key, string8_c const & value );
		boolean_c save_string16( char8_c const * key, string16_c const & value );
		boolean_c save_uint8( char8_c const * key, uint8_c value ); // equivalent to calling save_uint8_buffer() with length of 1.
		boolean_c save_sint8( char8_c const * key, sint8_c value ); // equivalent to calling save_sint8_buffer() with length of 1.
		boolean_c save_uint16( char8_c const * key, uint16_c value ); // equivalent to calling save_uint16_buffer() with length of 1.
		boolean_c save_sint16( char8_c const * key, sint16_c value ); // equivalent to calling save_sint16_buffer() with length of 1.
		boolean_c save_uint32( char8_c const * key, uint32_c value ); // equivalent to calling save_uint32_buffer() with length of 1.
		boolean_c save_sint32( char8_c const * key, sint32_c value ); // equivalent to calling save_sint32_buffer() with length of 1.
		boolean_c save_uint64( char8_c const * key, uint64_c value ); // equivalent to calling save_uint64_buffer() with length of 1.
		boolean_c save_sint64( char8_c const * key, sint64_c value ); // equivalent to calling save_sint64_buffer() with length of 1.
		boolean_c save_float32( char8_c const * key, float32_c value ); // equivalent to calling save_float32_buffer() with length of 1.
		boolean_c save_float64( char8_c const * key, float64_c value ); // equivalent to calling save_float64_buffer() with length of 1.
		boolean_c save_uint8_buffer( char8_c const * key, uint8_c const * value, sint32_c length );
		boolean_c save_sint8_buffer( char8_c const * key, sint8_c const * value, sint32_c length );
		boolean_c save_uint16_buffer( char8_c const * key, uint16_c const * value, sint32_c length );
		boolean_c save_sint16_buffer( char8_c const * key, sint16_c const * value, sint32_c length );
		boolean_c save_uint32_buffer( char8_c const * key, uint32_c const * value, sint32_c length );
		boolean_c save_sint32_buffer( char8_c const * key, sint32_c const * value, sint32_c length );
		boolean_c save_uint64_buffer( char8_c const * key, uint64_c const * value, sint32_c length );
		boolean_c save_sint64_buffer( char8_c const * key, sint64_c const * value, sint32_c length );
		boolean_c save_float32_buffer( char8_c const * key, float32_c const * value, sint32_c length );
		boolean_c save_float64_buffer( char8_c const * key, float64_c const * value, sint32_c length );
		boolean_c save_end(); // call to mark the end of the property stream.

	};

}
