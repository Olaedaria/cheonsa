#include "cheonsa_menu_element.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_element_c * menu_element_c::make_new_instance( string8_c const & type )
	{
		if ( type == menu_element_frame_c::get_type_name_static() )
		{
			return new menu_element_frame_c();
		}
		else if ( type == menu_element_text_c::get_type_name_static() )
		{
			return new menu_element_text_c();
		}
		return nullptr;
	}

	void_c menu_element_c::_on_local_box_modified()
	{
	}

	menu_element_c::menu_element_c()
		: _mother_control( nullptr )
		, _name()
		, _local_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _local_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom )
		, _local_box_anchor_measures( 0.0f, 0.0f, 0.0f, 0.0f )
		, _local_box( 0.0f, 0.0f, 100.0f, 100.0f )
		, _is_showing_from_style( true )
		, _is_showing( true )
		//, _is_showing_weight( 1.0f )
		, _is_enabled( true )
		//, _is_enabled_weight( 1.0f )
		, _is_selected( false )
		//, _is_selected_weight( 0.0f )
		, _is_pressed( false )
		//, _is_pressed_weight( 0.0f )
	{
	}

	void_c menu_element_c::update_animations( float32_c time_step )
	{
		//float32_c transition_step = global_engine_instance.interfaces.menu_style_manager->shared_transition_speed * static_cast< float32_c >( time_step );
		//_is_showing_weight = ops::math_saturate( _is_showing_weight + ( _is_showing ? transition_step : -transition_step ) );
		//_is_selected_weight = ops::math_saturate( _is_selected_weight + ( _is_selected ? transition_step : -transition_step ) );
		//_is_pressed_weight = ops::math_saturate( _is_pressed_weight + ( _is_pressed ? transition_step : -transition_step ) );
		//_is_enabled_weight = ops::math_saturate( _is_enabled_weight + ( _is_enabled ? transition_step : -transition_step ) );
	}

	void_c menu_element_c::set_layout_simple( box32x2_c const & local_box )
	{
		_local_box_anchor = menu_anchor_e_none;
		_local_box_anchor_measures = box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f );
		if ( _local_box != local_box )
		{
			_local_box = local_box;
			_on_local_box_modified();
		}
	}

	void_c menu_element_c::set_layout_box_anchor( menu_anchor_e local_box_anchor, box32x2_c local_box_anchor_insets )
	{
		_local_box_anchor = local_box_anchor;
		_local_box_anchor_measures = local_box_anchor_insets;
	}

	void_c menu_element_c::update_layout( box32x2_c const & mother_local_box )
	{
		if ( _local_box_anchor != menu_anchor_e_none )
		{
			box32x2_c new_local_box;
			if ( ( _local_box_anchor & menu_anchor_e_left ) && ( _local_box_anchor & menu_anchor_e_right ) )
			{
				// anchor left and right edges
				new_local_box.minimum.a = mother_local_box.minimum.a + _local_box_anchor_measures.minimum.a;
				new_local_box.maximum.a = mother_local_box.maximum.a - _local_box_anchor_measures.maximum.a;
			}
			else if ( _local_box_anchor & menu_anchor_e_left )
			{
				// anchor left edge
				new_local_box.minimum.a = mother_local_box.minimum.a + _local_box_anchor_measures.minimum.a;
				new_local_box.maximum.a = new_local_box.minimum.a + _local_box_anchor_measures.maximum.a;
			}
			else if ( _local_box_anchor & menu_anchor_e_left )
			{
				// anchor right edge
				new_local_box.maximum.a = mother_local_box.maximum.a - _local_box_anchor_measures.maximum.a;
				new_local_box.minimum.a = new_local_box.maximum.a - _local_box_anchor_measures.minimum.a;
			}
			if ( ( _local_box_anchor & menu_anchor_e_top ) && ( _local_box_anchor & menu_anchor_e_bottom ) )
			{
				// anchor top and bottom edges
				new_local_box.minimum.b = mother_local_box.minimum.b + _local_box_anchor_measures.minimum.b;
				new_local_box.maximum.b = mother_local_box.maximum.b - _local_box_anchor_measures.maximum.b;
			}
			else if ( _local_box_anchor & menu_anchor_e_top )
			{
				// anchor top edge
				new_local_box.minimum.b = mother_local_box.minimum.b + _local_box_anchor_measures.minimum.b;
				new_local_box.maximum.b = new_local_box.minimum.b + _local_box_anchor_measures.maximum.b;
			}
			else if ( _local_box_anchor & menu_anchor_e_bottom )
			{
				// anchor bottom edge
				new_local_box.maximum.b = mother_local_box.maximum.b - _local_box_anchor_measures.maximum.b;
				new_local_box.minimum.b = new_local_box.maximum.b - _local_box_anchor_measures.minimum.b;
			}
			if ( _local_box != new_local_box )
			{
				_local_box = new_local_box;
				_on_local_box_modified();
			}
		}
	}

	box32x2_c const & menu_element_c::get_local_box() const
	{
		return _local_box;
	}

	menu_control_c * menu_element_c::get_mother_control() const
	{
		return _mother_control;
	}

	//void_c menu_element_c::set_mother_control( menu_control_c * value )
	//{
	//	_mother_control = value;
	//}

	string8_c const & menu_element_c::get_name() const
	{
		return _name;
	}

	void_c menu_element_c::set_name( string8_c const & value )
	{
		_name = value;
	}

	vector32x4_c const & menu_element_c::get_local_color() const
	{
		return _local_color;
	}

	void_c menu_element_c::set_local_color( vector32x4_c const & value )
	{
		_local_color = value;
	}

	boolean_c menu_element_c::get_is_showing() const
	{
		return _is_showing;
	}

	void_c menu_element_c::set_is_showing( boolean_c value )
	{
		_is_showing = value;
	}

	//float32_c menu_element_c::get_is_showing_weight() const
	//{
	//	return _is_showing_weight;
	//}

	//void_c menu_element_c::set_is_showing_weight( float32_c value )
	//{
	//	_is_showing_weight = value;
	//}

	boolean_c menu_element_c::get_is_enabled() const
	{
		return _is_enabled;
	}

	void_c menu_element_c::set_is_enabled( boolean_c value )
	{
		_is_enabled = value;
	}

	//float32_c menu_element_c::get_is_enabled_weight() const
	//{
	//	return _is_enabled_weight;
	//}

	//void_c menu_element_c::set_is_enabled_weight( float32_c value )
	//{
	//	_is_enabled_weight = value;
	//}

	boolean_c menu_element_c::get_is_selected() const
	{
		return _is_selected;
	}

	void_c menu_element_c::set_is_selected( boolean_c value )
	{
		_is_selected = value;
	}

	//float32_c menu_element_c::get_is_selected_weight() const
	//{
	//	return _is_selected_weight;
	//}

	//void_c menu_element_c::set_is_selected_weight( float32_c value )
	//{
	//	_is_selected_weight = value;
	//}

	boolean_c menu_element_c::get_is_pressed() const
	{
		return _is_pressed;
	}

	void_c menu_element_c::set_is_pressed( boolean_c value )
	{
		_is_pressed = value;
	}

	//float32_c menu_element_c::get_is_pressed_weight() const
	//{
	//	return _is_pressed_weight;
	//}

	//void_c menu_element_c::set_is_pressed_weight( float32_c value )
	//{
	//	_is_pressed_weight = value;
	//}

	menu_draw_list_c & menu_element_c::get_draw_list() const
	{
		return const_cast< menu_draw_list_c & >( _draw_list );
	}

	menu_state_e menu_element_c::get_state() const
	{
		if ( !_is_enabled )
		{
			return menu_state_e_disabled;
		}
		else if ( _is_pressed )
		{
			return menu_state_e_pressed;
		}
		else if ( _is_selected )
		{
			return menu_state_e_selected;
		}
		return menu_state_e_normal;
	}

	//void_c menu_element_c::save_element_properties( data_scribe_structure_c * structure )
	//{
	//	data_scribe_structure_c::property_c * property;

	//	property = structure->add_property( "local_box_anchor", data_scribe_structure_c::property_c::type_e_uint8, 1 );
	//	property->get_uint8() = static_cast< uint8_c >( _local_box_anchor );

	//	property = structure->add_property( "local_box_anchor_insets", data_scribe_structure_c::property_c::type_e_float32, 4 );
	//	property->get_rectangle32() = _local_box_anchor_measures;

	//	property = structure->add_property( "local_box", data_scribe_structure_c::property_c::type_e_float32, 4 );
	//	property->get_rectangle32() = _local_box;
	//}

	//void_c menu_element_c::load_element_properties( data_scribe_structure_c * structure )
	//{
	//	data_scribe_structure_c::property_c * property;

	//	_local_box_anchor = menu_anchor_e_none;
	//	_local_box_anchor_measures = box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f );
	//	_local_box = box32x2_c( 0.0f, 0.0f, 100.0f, 100.0f );

	//	property = structure->find_property( "local_box_anchor", data_scribe_structure_c::property_c::type_e_uint8, 1 );
	//	if ( property != nullptr )
	//	{
	//		_local_box_anchor = static_cast< menu_anchor_e >( property->get_uint8() );
	//	}

	//	property = structure->find_property( "local_box_anchor_insets", data_scribe_structure_c::property_c::type_e_float32, 4 );
	//	if ( property != nullptr )
	//	{
	//		_local_box_anchor_measures = property->get_rectangle32();
	//	}

	//	property = structure->find_property( "local_box", data_scribe_structure_c::property_c::type_e_float32, 4 );
	//	if ( property != nullptr )
	//	{
	//		_local_box = property->get_rectangle32();
	//	}
	//}

	void_c menu_element_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		sint32_c dummy = 0;
		_local_box_anchor = menu_anchor_e_none;
		attribute = node->find_attribute( "local_box_anchor" );
		if ( attribute )
		{
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "left" ), dummy ) )
			{
				_local_box_anchor = static_cast< menu_anchor_e >( _local_box_anchor | menu_anchor_e_left );
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "top" ), dummy ) )
			{
				_local_box_anchor = static_cast< menu_anchor_e >( _local_box_anchor | menu_anchor_e_top );
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "right" ), dummy ) )
			{
				_local_box_anchor = static_cast< menu_anchor_e >( _local_box_anchor | menu_anchor_e_right );
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( mode_e_static, "bottom" ), dummy ) )
			{
				_local_box_anchor = static_cast< menu_anchor_e >( _local_box_anchor | menu_anchor_e_bottom );
			}
		}

		attribute = node->find_attribute( "local_box_anchor_measures" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( mode_e_static, _local_box_anchor_measures.as_array(), 4 ) );
		}

		attribute = node->find_attribute( "local_box" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( mode_e_static, _local_box.as_array(), 4 ) );
		}

		string8_c style_key;
		attribute = node->find_attribute( "style_key" );
		if ( attribute )
		{
			style_key = attribute->get_value();
		}
		set_style_key( style_key );
	}

}
