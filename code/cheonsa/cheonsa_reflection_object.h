#pragma once

#include "cheonsa_reflection_types.h"

namespace cheonsa
{

	// all objects that implement reflection must inherit from this class.
	// references a reflection class, which enables reflection on the object instance.
	class reflection_object_c
	{
	protected:
		friend boolean_c reflection_get_object_property_value( reflection_object_c *, reflection_property_c const *, reflection_value_container_c & );
		friend boolean_c reflection_set_object_property_value( reflection_object_c *, reflection_property_c const *, reflection_value_container_c const & );
		friend class menu_control_property_inspector_c;

		reflection_object_c * _this; // the real this pointer? something goes screwy if we reference the "this" pointer in this class vs the "this" pointer in an inherited class. so "_this" this pointer is the "this" pointer from most leaf class.
		reflection_class_c const * _reflection_class; // the reflection class interface to use to make_reflected the properties of the class instance pointed to by reflection_object.

	public:
		reflection_object_c( reflection_object_c * this_in, reflection_class_c const * reflection_class );

		virtual string16_c get_reflection_display_name() const; // used to show the name of the object instance when it is displayed in a list box.
		reflection_class_c const * get_reflection_class() const;

	};

}
