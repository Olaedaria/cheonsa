#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	class reflection_property_c;
	class reflection_enumeration_c;
	class reflection_class_c;
	class reflection_manager_c;
	class reflection_object_c;

	// used to hold before/after value states of a single property, regardless of the underlying type.
	class reflection_value_container_c
	{
	public:
		union
		{
			boolean_c boolean;
			uint8_c uint8[ 4 ];
			sint8_c sint8[ 4 ];
			uint16_c uint16[ 4 ];
			sint16_c sint16[ 4 ];
			uint32_c uint32[ 4 ];
			sint32_c sint32[ 4 ];
			uint64_c uint64[ 4 ];
			sint64_c sint64[ 4 ];
			float32_c float32[ 4 ];
			float64_c float64[ 4 ];
		};
		string8_c string8;
		string16_c string16;

	public:
		reflection_value_container_c();

		reflection_value_container_c & operator = ( reflection_value_container_c const & other );

	};

	class reflection_property_value_changed_information_c
	{
	public:
		reflection_object_c * reflection_object;
		reflection_property_c const * reflection_property;
		reflection_value_container_c before_value;
		reflection_value_container_c after_value;

	public:
		reflection_property_value_changed_information_c();
		reflection_property_value_changed_information_c( reflection_property_value_changed_information_c const & other );

		reflection_property_value_changed_information_c & operator = ( reflection_property_value_changed_information_c const & other );

	};

	// generic get value accessor. only works with string and numeric types, asserts for other types.
	boolean_c reflection_get_object_property_value( reflection_object_c * object, reflection_property_c const * property, reflection_value_container_c & value );
	// generic set value accessor. only works with string and numeric types, asserts for other types.
	boolean_c reflection_set_object_property_value( reflection_object_c * object, reflection_property_c const * property, reflection_value_container_c const & value );

	// to interoperate with text boxes. takes into account property data view to convert as needed.
	string16_c reflection_convert_value_to_string16( reflection_property_c const * property, reflection_value_container_c const & value );
	// to interoperate with text boxes. takes into account property data view to convert as needed.
	boolean_c reflection_convert_string16_to_value( reflection_property_c const * property, reflection_value_container_c & value, string16_c const & value_as_string16 );

	vector64x4_c reflection_convert_value_to_color( reflection_property_c const * property, reflection_value_container_c const & value );
	void_c reflection_convert_color_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector64x4_c const & value_as_color );

	vector32x3_c reflection_convert_value_to_euler_angles( reflection_property_c const * property, reflection_value_container_c const & value );
	void_c reflection_convert_euler_angles_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector32x3_c const & value_as_euler_angles );

	boolean_c reflection_compare_values( reflection_property_c const * property, reflection_value_container_c const & a, reflection_value_container_c const & b );

}
