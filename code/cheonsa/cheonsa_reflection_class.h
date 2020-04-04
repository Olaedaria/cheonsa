#pragma once

#include "cheonsa_reflection_types.h"

namespace cheonsa
{

	// defines properties for a reflection_object_c.
	// supports single inheritance (optional) through _base_class.
	class reflection_class_c
	{
	private:
		friend class reflection_manager_c;
		reflection_manager_c * _reflection_manager;
		string8_c _name;
		string8_c _description;
		reflection_class_c * _base_class;
		core_list_c< reflection_property_c const * > _property_list;

	public:
		reflection_class_c( reflection_manager_c * reflection_manager, string8_c const & name, string8_c const & description, reflection_class_c * base_class );
		~reflection_class_c();

		string8_c const & get_name() const;
		string8_c const & get_description() const;

		reflection_class_c * get_base_class() const;

		// adds a new property to the class and returns it.
		// then you must call one of the _initialize functions on the returned property.
		reflection_property_c * initialize_property( string8_c const & name, string8_c const & description );

		sint32_c get_property_count() const;
		reflection_property_c const * get_property( sint32_c index ) const;
		reflection_property_c const * get_property_with_name( string8_c const & name ) const;

	};

}
