#include "cheonsa_reflection_property.h"
#include "cheonsa_reflection_enumeration.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	reflection_property_c::reflection_property_c( reflection_class_c * reflection_class, string8_c const & name, string8_c const & description )
		: _reflection_class( reflection_class )
		, _name( name )
		, _description( description )
		, _type( data_type_e_none )
		, _type_count( 0 )
		, _view( data_view_e_default )
		, _additional_options()
		, _enumeration( nullptr )
		, _defaults_and_limits()
		, _accessors()
	{
		assert( reflection_class != nullptr );
		ops::memory_zero( &_defaults_and_limits, sizeof( _defaults_and_limits ) ); // i'm not sure if this is needed in modern c++.
		ops::memory_zero( &_accessors, sizeof( _accessors ) ); // i'm not sure if this is needed in modern c++.
	}

	reflection_property_c::~reflection_property_c()
	{
	}

	void_c reflection_property_c::initialize_clone( reflection_property_c const * original )
	{
		_name = original->_name;
		_description = original->_description;
		_type = original->_type;
		_type_count = original->_type_count;
		_view = original->_view;
		_additional_options = original->_additional_options;
		_defaults_and_limits = original->_defaults_and_limits;
		_accessors = original->_accessors;
	}

	void_c reflection_property_c::initialize_category_label()
	{
		assert( _type == data_type_e_none );
		_type = data_type_e_category_label;
	}

	void_c reflection_property_c::initialize_basic( data_type_e type, uint8_c type_count, value_getter_f value_getter, value_setter_f value_setter )
	{
		assert( _type == data_type_e_none );
		assert( type >= data_type_e_string8 && type <= data_type_e_float64 );
		assert( type_count >= 1 && type_count <= 4 );
		if ( type == data_type_e_string8 || type == data_type_e_string16 )
		{
			assert( type_count == 1 );
		}
		_type = type;
		_type_count = type_count;
		_accessors._value_getter = value_getter;
		_accessors._value_setter = value_setter;
	}

	void_c reflection_property_c::initialize_basic_default( void_c const * default )
	{
		assert( _type >= data_type_e_uint8 && _type <= data_type_e_float64 );
		assert( _type_count >= 1 && _type_count <= 4 );
		assert( default );
		uint8_c * target = _defaults_and_limits._uint8_default;
		ops::memory_copy( default, target, ops::get_data_type_size( _type ) * _type_count );
	}

	void_c reflection_property_c::initialize_basic_limits( void_c * minimum, void_c * maximum )
	{
		assert( _type >= data_type_e_uint8 && _type <= data_type_e_float64 );
		assert( _type_count == 1 );
		assert( minimum != nullptr && maximum != nullptr );
		sint32_c type_size = ops::get_data_type_size( _type );
		uint8_c * target = _defaults_and_limits._uint8_default;
		ops::memory_copy( minimum, target + ( type_size * 4 ), type_size );
		ops::memory_copy( maximum, target + ( type_size * 4 ) + type_size, type_size );
	}

	void_c reflection_property_c::initialize_basic_view_boolean()
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_uint8 );
		_view = data_view_e_boolean;
	}

	void_c reflection_property_c::initialize_basic_view_color()
	{
		assert( _view == data_view_e_default );
		assert( 
			( _type == data_type_e_uint32 && _type_count == 1 ) ||
			( ( _type == data_type_e_uint8 || _type == data_type_e_float32 || _type == data_type_e_float64 ) && ( _type_count == 3 || _type_count == 4 ) ) );
		if ( _type == data_type_e_uint32 )
		{
			assert( _defaults_and_limits._uint32_minimum == 0 && _defaults_and_limits._uint32_maximum == 0 );
		}
		// we could check limits for other configurations, but we don't need to because if the programmer tried to call _initialize_basic_limits when _type_count != 1 then it would have asserted there.
		_view = data_view_e_color;
	}

	void_c reflection_property_c::initialize_basic_view_scroll_bar()
	{
		assert( _view == data_view_e_default );
		assert( ( _type == data_type_e_float32 || _type == data_type_e_float64 ) && ( _type_count == 1 ) );
		if ( _type == data_type_e_float32 )
		{
			assert( _defaults_and_limits._float32_minimum < _defaults_and_limits._float32_maximum );
		}
		else if ( _type == data_type_e_float64 )
		{
			assert( _defaults_and_limits._float64_minimum < _defaults_and_limits._float64_maximum );
		}
		_view = data_view_e_scroll_bar;
	}

	void_c reflection_property_c::initialize_basic_view_scrub_bar()
	{
		assert( _view == data_view_e_default );
		_view = data_view_e_scrub_bar;
	}

	void_c reflection_property_c::initialize_basic_view_text()
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_string8 || _type == data_type_e_string16 );
		_view = data_view_e_text;
	}

	void_c reflection_property_c::initialize_basic_view_euler_angles()
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_float32 || _type == data_type_e_float64 );
		assert( _type_count == 3 || _type_count == 4 );
		_view = data_view_e_euler_angles;
	}

	void_c reflection_property_c::initialize_basic_view_enumeration( reflection_enumeration_c const * enumeration )
	{
		assert( _view == data_view_e_default );
		assert( _type >= data_type_e_uint8 && _type <= data_type_e_sint64 );
		assert( _type_count == 1 );
		assert( enumeration );
		_view = data_view_e_enumeration;
		_enumeration = enumeration;
	}

	void_c reflection_property_c::initialize_basic_view_file_path( string8_c file_type_filter )
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_string8 || _type == data_type_e_string16 );
		assert( _type_count == 1 );
		_view = data_view_e_file_path;
		_additional_options = file_type_filter;
	}

	void_c reflection_property_c::initialize_basic_view_folder_path()
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_string8 || _type == data_type_e_string16 );
		assert( _type_count == 1 );
		_view = data_view_e_folder_path;
	}

	void_c reflection_property_c::initialize_object( reflection_class_c const * _class, object_getter_f object_getter )
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_none );
		assert( _class );
		assert( object_getter );
		_type = data_type_e_object;
		_type_count = 1;
		this->_class = _class;
		_accessors._object_getter = object_getter;
	}

	void_c reflection_property_c::initialize_object_list( reflection_class_c const * _class, object_list_item_count_getter_f object_list_item_count_getter, object_list_item_getter_f object_list_item_getter, object_list_item_adder_f object_list_item_adder, object_list_item_remover_f object_list_item_remover, object_list_item_mover_f object_list_item_mover, object_list_item_sorter_f object_list_item_sorter )
	{
		assert( _view == data_view_e_default );
		assert( _type == data_type_e_none );
		assert( _class );
		assert( object_list_item_count_getter );
		assert( object_list_item_getter );
		assert( object_list_item_adder );
		assert( object_list_item_remover );
		_type = data_type_e_object_list;
		_type_count = 1;
		this->_class = _class;
		_accessors._object_list_item_count_getter = object_list_item_count_getter;
		_accessors._object_list_item_getter = object_list_item_getter;
		_accessors._object_list_item_adder = object_list_item_adder;
		_accessors._object_list_item_remover = object_list_item_remover;
		_accessors._object_list_item_mover = object_list_item_mover;
		_accessors._object_list_item_sorter = object_list_item_sorter;
	}

	string8_c const & reflection_property_c::get_name() const
	{
		return _name;
	}

	string8_c const & reflection_property_c::get_description() const
	{
		return _description;
	}

	data_type_e reflection_property_c::get_type() const
	{
		return _type;
	}

	uint8_c reflection_property_c::get_type_count() const
	{
		return _type_count;
	}

}
