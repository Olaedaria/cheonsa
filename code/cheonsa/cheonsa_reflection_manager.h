#pragma once

#include "cheonsa_reflection_types.h"

namespace cheonsa
{



	/*
	// lets the program define the available options dynamically at run time.
	// useful in the case that we want options to be data driven, such as options for character customization.
	class reflection_dynamic_property_c
	{
	public:
		enum type_e
		{
			type_e_none,
			type_e_list, // dynamic property lets user select one item from a list of items.
			type_e_slider, // dynamic property lets user select a float in the range of 0 to 1.
		};

	public:
		string8_c key;
		type_e type;
		core_list_c< string8_c > list_items;

	public:
		reflection_dynamic_property_c()
			: key()
			, type( type_e_none )
			, list_items()
		{
		}

		reflection_dynamic_property_c( reflection_dynamic_property_c const & other )
		{
			*this = other;
		}

		reflection_dynamic_property_c & operator = ( reflection_dynamic_property_c const & other )
		{
			key = other.key;
			type = other.type;
			list_items = other.list_items;
			return *this;
		}

	};
	*/


	// manages enumeration and class registry.
	// there is a single global instance of this class.
	class reflection_manager_c
	{
		friend class engine_c;

	private:
		core_list_c< reflection_enumeration_c * > _enumeration_list;
		core_list_c< reflection_class_c * > _class_list;

		reflection_manager_c();
		~reflection_manager_c();

		boolean_c start();

	public:
		// adds a new enumeration with name to the global enumeration registry.
		// asserts if enumeration already exists.
		// the game should add all of the enumerations at start up.
		// is_flags means that the enumeration values should be treated like bit flags. this will allow the user to toggle select on and off more than one value at a time. otherwise the user can only select one value at a time.
		reflection_enumeration_c * add_enumeration( string8_c const & name, string8_c const & description, boolean_c is_for_bit_flags );
		// looks up an enumeration with name.
		reflection_enumeration_c * find_enumeration( string8_c const & name );

		// adds a new class with name to the global class registry.
		// asserts if enumeration already exists.
		// the game should add all of the classes at start up.
		reflection_class_c * add_class( string8_c const & name, string8_c const & description, reflection_class_c * base_class );
		// looks up a class with name.
		reflection_class_c * find_class( string8_c const & name );

	};

}
