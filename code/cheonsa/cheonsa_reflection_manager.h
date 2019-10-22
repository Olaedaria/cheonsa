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
	private:
		core_list_c< reflection_enumeration_c * > _enumeration_list;
		core_list_c< reflection_class_c * > _class_list;

	public:
		reflection_manager_c();
		~reflection_manager_c();

		boolean_c start();

		reflection_enumeration_c * _add_enumeration( string8_c const & name, string8_c const & description ); // adds a new enumeration with name to the global enumeration registry. asserts if enumeration already exists.
		reflection_enumeration_c * find_enumeration( string8_c const & name ); // looks up an enumeration with name.

		reflection_class_c * _add_class( string8_c const & name, string8_c const & description, reflection_class_c * base_class ); // adds a new class with name to the global class registry. asserts if enumeration already exists.
		reflection_class_c * find_class( string8_c const & name ); // looks up a class with name.

	};

}
