#pragma once

#include "cheonsa_reflection_types.h"

namespace cheonsa
{

	// reflection properties are added to reflection classes to define the properties of that class.
	// these provide a route for the reflection system to edit an object's properties, by calling static functions on the object's class that in turn access the object instance's data.
	// this allows the class|object to respond|react to edits as they are made.
	// this is in contrast to a direct memory access model, which is an alternative way to do things.
	class reflection_property_c
	{
	public:
		// value type must point to a buffer|instance of the same type that is defined by the property type and type count.
		// the property's value_type and value_count are passed in each time just so that the programmer can verify that they're casting to the same data type that they said they would when they initialized the reflection property.
		// so if property _type is data_type_e_float32 and _type_count is 4, then value must point to a buffer of 4 float32_c, or an instance of vector32x4_c.
		// if property _type is data_type_e_string8 and _type_count is 1, then value must point to a string8_c instance.
		// if property _type is data_type_e_string16 and _type_count is 1, then value must point to a string16_c instance.
		// if property _type is data_type_e_object, then use object_getter_f instead.
		// if property _type is data_type_e_object_list, then use the set of object_list_*_f functions instead.
		typedef boolean_c (*value_getter_f)( reflection_object_c * instance, void_c * value, data_type_e value_type, uint8_c value_type_count );
		typedef boolean_c (*value_setter_f)( reflection_object_c * instance, void_c const * value, data_type_e value_type, uint8_c value_type_count );

		typedef reflection_object_c * (*object_getter_f)( reflection_object_c * instance ); // for type_e_object.

		typedef sint32_c (*object_list_item_count_getter_f)( reflection_object_c * instance ); // for type_e_object_list.
		typedef reflection_object_c * (*object_list_item_getter_f)( reflection_object_c * instance, sint32_c index );
		typedef reflection_object_c * (*object_list_item_adder_f)( reflection_object_c * instance, sint32_c index ); // instantiates a new instance of an object and inserts it in the list at index.
		typedef boolean_c (*object_list_item_remover_f)( reflection_object_c * instance, sint32_c index );
		typedef boolean_c (*object_list_item_mover_f)( reflection_object_c * instance, sint32_c old_index, sint32_c new_index ); // swaps items at old_index and new_index.
		typedef boolean_c (*object_list_item_sorter_f)( reflection_object_c * instance );

	private:
		friend boolean_c reflection_get_object_property_value( reflection_object_c *, reflection_property_c const *, reflection_value_container_c & );
		friend boolean_c reflection_set_object_property_value( reflection_object_c *, reflection_property_c const *, reflection_value_container_c const & );
		friend string16_c reflection_convert_value_to_string16( reflection_property_c const * property, reflection_value_container_c const & property_value );
		friend boolean_c reflection_convert_string16_to_value( reflection_property_c const * property, reflection_value_container_c & property_value, string16_c const & property_value_as_string16 );
		friend vector64x4_c reflection_convert_value_to_color( reflection_property_c const * property, reflection_value_container_c const & value );
		friend void_c reflection_convert_color_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector64x4_c const & value_as_color );
		friend vector32x3_c reflection_convert_value_to_euler_angles( reflection_property_c const * property, reflection_value_container_c const & value );
		friend void_c reflection_convert_euler_angles_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector32x3_c const & value_as_euler_angles );
		friend boolean_c reflection_compare_values( reflection_property_c const * property, reflection_value_container_c const & a, reflection_value_container_c const & b );
		friend class menu_control_property_inspector_c;
		friend class reflection_class_c;
		friend class reflection_manager_c;

		reflection_class_c const * _reflection_class; // the reflection class that owns this property.
		string8_c _name; // name of property. currently not localizable.
		string8_c _description; // description of property. currently not localizable.
		data_type_e _type; // the type of data.
		uint8_c _type_count; // a value between 1 and 4. must be 1 for non-numeric types. must be 1 for int type with enum view.
		data_view_e _view; // what kind of data editor user interface to use.

		// how this is interpreted depends on the data type.
		// if view is file path, then this is a vertical slash separated list of file extensions to filter ".txt|.log".
		// (not stored in _defaults_and_limits because that's a union and this is not a base type so it wouldn't play well with the union).
		string8_c _additional_options;
		reflection_enumeration_c const * _enumeration; // for use with _types of int with _type_count of 1, and with _view of enumeration.
		reflection_class_c const * _class; // for use with _type of data_type_e_object and data_type_e_object_list.

		union
		{
			struct
			{
				uint8_c _uint8_default[ 4 ];
				uint8_c _uint8_minimum;
				uint8_c _uint8_maximum;
			};
			struct
			{
				sint8_c _sint8_default[ 4 ];
				sint8_c _sint8_minimum;
				sint8_c _sint8_maximum;
			};
			struct
			{
				uint16_c _uint16_default[ 4 ];
				uint16_c _uint16_minimum;
				uint16_c _uint16_maximum;
			};
			struct
			{
				sint16_c _sint16_default[ 4 ];
				sint16_c _sint16_minimum;
				sint16_c _sint16_maximum;
			};
			struct
			{
				uint32_c _uint32_default[ 4 ];
				uint32_c _uint32_minimum;
				uint32_c _uint32_maximum;
			};
			struct
			{
				sint32_c _sint32_default[ 4 ];
				sint32_c _sint32_minimum;
				sint32_c _sint32_maximum;
			};
			struct
			{
				uint64_c _uint64_default[ 4 ];
				uint64_c _uint64_minimum;
				uint64_c _uint64_maximum;
			};
			struct
			{
				sint64_c _sint64_default[ 4 ];
				sint64_c _sint64_minimum;
				sint64_c _sint64_maximum;
			};
			struct
			{
				float32_c _float32_default[ 4 ];
				float32_c _float32_minimum;
				float32_c _float32_maximum;
			};
			struct
			{
				float64_c _float64_default[ 4 ];
				float64_c _float64_minimum;
				float64_c _float64_maximum;
			};
			struct
			{
				sint32_c _string_character_limit; // for use with string8 and string16 types.
			};
			struct
			{
				boolean_c _auto_sort_enable; // for use with type_e_object_list. if true then the editor will resort the list any time any property in any of the list items is modified.
			};
		} _defaults_and_limits;

		union
		{
			struct
			{
				union
				{
					value_getter_f _value_getter;
					object_getter_f _object_getter;
				};
				union
				{
					value_setter_f _value_setter;
				};
			};
			struct
			{
				object_list_item_count_getter_f _object_list_item_count_getter;
				object_list_item_getter_f _object_list_item_getter;
				object_list_item_adder_f _object_list_item_adder;
				object_list_item_remover_f _object_list_item_remover;
				object_list_item_mover_f _object_list_item_mover;
				object_list_item_sorter_f _object_list_item_sorter;
			};
		} _accessors;

	public:
		reflection_property_c( reflection_class_c * reflection_class, string8_c const & name, string8_c const & description );
		~reflection_property_c();

		void_c initialize_clone( reflection_property_c const * original ); // initializes this property as a clone of original, used to duplicate properties from base class(es).

		void_c initialize_category_label();

		void_c initialize_basic( data_type_e type, uint8_c type_count, value_getter_f value_getter, value_setter_f value_setter ); // initializes reflector for numeric and string types. returns this in case you want to daisy chain.

		void_c initialize_basic_value_default( void_c const * default ); // default must point to a buffer of appropriate type and count that contains the default value of the property. returns this in case you want to daisy chain.
		void_c initialize_basic_value_limits( void_c * minimum, void_c * maximum ); // minimum and maximum must point to a values of appropriate type. returns this in case you want to daisy chain.

		void_c initialize_basic_view_color(); // type must be uint32x1 (rgba), or uint8x3 (rgb), or uint8x4 (rgba), or float32x3 (rgb), or float32x4 (rgba), or float64x3 (rgb), or float64x4 (rgba).
		void_c initialize_basic_view_scroll_bar(); // type must be float32x1 or float64x1. limits must be set so that the slider has a range to slide around in.
		void_c initialize_basic_view_scrub_bar(); // type must be float32x1 or float64x1.
		void_c initialize_basic_view_text(); // type must be string8x1 or string16x1.
		void_c initialize_basic_view_euler_angles(); // type must be float32x3 (euler), float32x4 (quaternion), float64x3 (euler), or float64x4 (quaternion). if the type is a quaternion then it does the conversion to and from euler angles.
		void_c initialize_basic_view_enumeration( reflection_enumeration_c const * enumeration ); // type must be any int type with count of 1: uint8x1, sint8x1, uint16x1, sint16x1, uint32x1, sint32x1, uint64x1, sint64x1. will display a combo box that allows the user to select at most one item.
		void_c initialize_basic_view_enumeration_flags( reflection_enumeration_c const * enumeration ); // type must be any int type with count of 1: uint8x1, sint8x1, uint16x1, sint16x1, uint32x1, sint32x1, uint64x1, sint64x1. will display a combo box that allows the user to check on and off each item in the enumeration.
		void_c initialize_basic_view_file_path( string8_c file_type_filter ); // enables file picker. file_type_filter is a vertical slash separated list of file extensions, for example "*.txt|*.log".

		void_c initialize_object( reflection_class_c const * _class, object_getter_f object_getter );

		void_c initialize_object_list( reflection_class_c const * _class, object_list_item_count_getter_f object_list_item_count_getter, object_list_item_getter_f object_list_item_getter, object_list_item_adder_f object_list_item_adder, object_list_item_remover_f object_list_item_remover, object_list_item_mover_f object_list_item_mover, object_list_item_sorter_f object_list_item_sorter ); // mover and sorter functions are optional.

	public:
		string8_c const & get_name() const;
		string8_c const & get_description() const;
		data_type_e get_type() const;
		uint8_c get_type_count() const;

	};

}
