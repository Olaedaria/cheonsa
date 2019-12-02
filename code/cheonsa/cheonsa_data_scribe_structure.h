#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_scribe_binary.h"

namespace cheonsa
{

	// the structure data container type is used to hold all of an object's state in an internal heap memory list of properties.
	// this allows us to get around some limitations with the requirement of serial linearity with saving/loading properties for inherited classes.
	// if we have a base class a_c and a derived class b_c, and we want to write save/load functions for each one, where the save/load functions in b_c saves/loads all the same properties from a_c plus its own...
	// if we were to do this with a linear scribe, we would have to copy and paste the same code from a_c in b_c.
	// but with a scribe such as this one, we can nest the load/save code, such that b_c calls the save/load function in a_c to save/load properties of the base class, and then b_c can save/load its own properties afterward. 
	// all of the engine specific file formats will probably use this container format to store data because it allows for pretty simple forward compatability.
	// just don't use this to work with large data, because it all has to fit in memory.
	class data_scribe_structure_c
	{
	public:
		class property_c
		{
		public:
			enum type_e : uint8_c
			{
				type_e_none,

				type_e_buffer, // a blob of arbitrary binary data or an array of items.

				type_e_string8, // the property value is stored in the file as a uint16_c length-prefixed char8_c utf-8 encoded string, but during run-time it is converted into a string16_c.
				type_e_string16, // the property value is stored in the file as a uint16_c length-prefixed char16_c string.

				type_e_structure, // the property is another data_scribe_structure_c.
				type_e_structure_list, // the property is an array of other data_scribe_structure_c's.

				type_e_sint8, // plain old data type, can have 1 to 255 elements.
				type_e_sint16, // plain old data type, can have 1 to 255 elements.
				type_e_sint32, // plain old data type, can have 1 to 255 elements.
				type_e_sint64, // plain old data type, can have 1 to 255 elements.
				type_e_uint8, // plain old data type, can have 1 to 255 elements.
				type_e_uint16, // plain old data type, can have 1 to 255 elements.
				type_e_uint32, // plain old data type, can have 1 to 255 elements.
				type_e_uint64, // plain old data type, can have 1 to 255 elements.
				type_e_float16, // plain old data type, can have 1 to 255 elements.
				type_e_float32, // plain old data type, can have 1 to 255 elements.
				type_e_float64, // plain old data type, can have 1 to 255 elements.

				type_e_count_
			};

			static sint32_c get_type_base_size( type_e type );

			// should be 8 bytes.
			// a buffer of sizeof( buffer_c ) + data_size is allocated.
			// the first 8 bytes are reinterpreted as buffer_c to describe the data.
			// the remaining bytes that follow are the data itself.
			class buffer_c
			{
			public:
				sint32_c array_count; // optional number of elements in the buffer in the case that the buffer is used to store an array of elements.
				sint32_c data_size; // size in bytes of data.

			};

		private:
			friend class data_scribe_structure_c;
			string16_c _name; // the name that is used to identify this property.
			type_e _type; // the value type, which determines how _value is allocated and interpreted.
			uint8_c _type_count; // for plain old data types, this defines the number of elements of the given _type that are in _value.
			void_c * _value; // raw value allocated on the heap. how this value is allocated and interpreted depends on the value of _type.

		public:
			string16_c & get_name();

			type_e get_type();

			void_c set_buffer( sint32_c array_count, void_c * data, sint32_c data_size ); // array_count is optional (pass in 0 if you don't need it), but it lets us treat the buffer like an array of elements, so when we load the buffer we can know how many elements of whatever thing we can expect to find in it.
			sint32_c get_buffer_array_count(); // gets the number of items in the buffer, in the case that it is being used like an array of items.
			sint32_c get_buffer_data_size(); // gets the size of the buffer data in bytes.
			void_c * get_buffer_data(); // gets a pointer to the raw buffer.

			string8_c & get_string8(); // gets a reference to the string8_c value, which can be set because it is a reference.
			string16_c & get_string16(); // gets a reference to the string16_c value, which can be set because it is a reference.

			data_scribe_structure_c * get_structure(); // gets a pointer to the sub-structure.

			sint32_c get_structure_list_count(); // gets the number of sub-structures in the sub-structure list.
			data_scribe_structure_c * add_structure_to_structure_list(); // adds a sub-structure to the sub-structure list and returns its pointer.
			data_scribe_structure_c * get_structure_in_structure_list_at_index( sint32_c index ); // gets the sub-structure in the sub-structure list at index and returns its pointer.

			sint8_c & get_sint8(); // gets a reference to the sint8_c value, which can be set because it is a reference.
			sint16_c & get_sint16(); // gets a reference to the sint16_c value, which can be set because it is a reference.
			sint32_c & get_sint32(); // gets a reference to the sint32_c value, which can be set because it is a reference.
			sint64_c & get_sint64(); // gets a reference to the sint64_c value, which can be set because it is a reference.

			uint8_c & get_uint8(); // gets a reference to the uint8_c value, which can be set because it is a reference.
			uint16_c & get_uint16(); // gets a reference to the uint16_c value, which can be set because it is a reference.
			uint32_c & get_uint32(); // gets a reference to the uint32_c value, which can be set because it is a reference.
			uint64_c & get_uint64(); // gets a reference to the uint64_c value, which can be set because it is a reference.

			float32_c & get_float32(); // gets a reference to the float32_c value, which can be set because it is a reference.
			vector32x2_c & get_float32x2(); // gets a reference to the float32x2_c value, which can be set because it is a reference.
			vector32x3_c & get_float32x3(); // gets a reference to the float32x3_c value, which can be set because it is a reference.
			vector32x4_c & get_float32x4(); // gets a reference to the float32x4_c value, which can be set because it is a reference.
			box32x2_c & get_rectangle32();

			float64_c & get_float64(); // gets a reference to the float64_c value, which can be set because it is a reference.
			vector64x2_c & get_float64x2(); // gets a reference to the float64x2_c value, which can be set because it is a reference.
			vector64x3_c & get_float64x3(); // gets a reference to the float64x3_c value, which can be set because it is a reference.
			vector64x4_c & get_float64x4(); // gets a reference to the float64x4_c value, which can be set because it is a reference.
			box64x2_c & get_rectangle64();

		public:
			property_c( string16_c const & name, type_e type, uint8_c type_count = 0 );
			~property_c();

		};

	private:
		core_list_c< property_c * > _property_list;

	public:
		boolean_c save( data_scribe_binary_c & scribe );
		boolean_c load( data_scribe_binary_c & scribe );

		property_c * find_property( string16_c const & name, property_c::type_e type = property_c::type_e_none, uint8_c type_count = 0 ); // finds and returns the property with name and type, or returns nullptr.

		void_c remove_property( property_c * property ); // removes and deletes the property from this struct's property core_list_c.

		property_c * add_property( string16_c const & name, property_c::type_e type, uint8_c type_count = 0 ); // creates a new property and adds it to this structs property list and returns it.

	public:
		data_scribe_structure_c();
		~data_scribe_structure_c();

	};

}
