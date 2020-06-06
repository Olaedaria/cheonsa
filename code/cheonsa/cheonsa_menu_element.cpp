#include "cheonsa_menu_element.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_element_c::_on_local_box_modified()
	{
	}

	void_c menu_element_c::_load_properties( data_scribe_markup_c::node_c const * node )
	{
		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		sint32_c new_local_box_anchor = menu_anchor_e_none;
		box32x2_c new_local_box_anchor_measures;
		box32x2_c new_local_box;
		string8_c new_style_key;

		// load layout.
		attribute = node->find_attribute( "local_box_anchor" );
		if ( attribute )
		{
			sint32_c temp = 0;
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "left" ), temp ) )
			{
				new_local_box_anchor |= menu_anchor_e_left;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "top" ), temp ) )
			{
				new_local_box_anchor |= menu_anchor_e_top;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "right" ), temp ) )
			{
				new_local_box_anchor |= menu_anchor_e_right;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "bottom" ), temp ) )
			{
				new_local_box_anchor |= menu_anchor_e_bottom;
			}
		}
		attribute = node->find_attribute( "local_box_anchor_measures" );
		if ( attribute )
		{
			if ( ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, new_local_box_anchor_measures.as_array(), 4 ) ) )
			{
				set_layout_box_anchor( static_cast< menu_anchor_e >( new_local_box_anchor ), new_local_box_anchor_measures );
			}
		}
		attribute = node->find_attribute( "local_box" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, new_local_box.as_array(), 4 ) );
			set_layout_simple( new_local_box );
		}

		// load style.
		attribute = node->find_attribute( "style_key" );
		if ( attribute )
		{
			new_style_key = attribute->get_value();
		}
		set_style_key( new_style_key );
	}

	menu_element_c::menu_element_c( string8_c const & name )
		: _mother_control( nullptr )
		, _name( name )
		, _shared_color_class( menu_shared_color_class_e_window )
		, _shared_color_class_swapped( false )
		, _local_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _local_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom )
		, _local_box_anchor_measures( 0.0f, 0.0f, 0.0f, 0.0f )
		, _local_box( 0.0f, 0.0f, 100.0f, 100.0f )
		, _is_showed_from_style( true )
		, _is_showed( true )
		, _is_enabled( true )
		, _is_selected( false )
		, _is_pressed( false )
		, _is_overlay( false )
	{
	}

	void_c menu_element_c::set_layout_simple( box32x2_c const & local_box )
	{
		_local_box_anchor = menu_anchor_e_none;
		_local_box_anchor_measures = box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f );
		if ( _local_box != local_box )
		{
			_local_box = local_box;
			if ( _local_box.maximum.a < _local_box.minimum.a )
			{
				_local_box.maximum.a = _local_box.minimum.a;
			}
			if ( _local_box.maximum.b < _local_box.minimum.b )
			{
				_local_box.maximum.b = _local_box.minimum.b;
			}
			_on_local_box_modified();
		}
	}

	void_c menu_element_c::set_layout_box_anchor( menu_anchor_e local_box_anchor, box32x2_c const & local_box_anchor_measures )
	{
		_local_box_anchor = local_box_anchor;
		_local_box_anchor_measures = local_box_anchor_measures;
	}

	void_c menu_element_c::set_layout_box_anchor_measures( box32x2_c const & local_box_anchor_measures )
	{
		_local_box_anchor_measures = local_box_anchor_measures;
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

	void_c menu_element_c::set_mother_control( menu_control_c * value )
	{
		_mother_control = value;
	}

	string8_c const & menu_element_c::get_name() const
	{
		return _name;
	}

	//void_c menu_element_c::set_name( string8_c const & value )
	//{
	//	_name = value;
	//}

	vector32x4_c const & menu_element_c::get_local_color() const
	{
		return _local_color;
	}

	void_c menu_element_c::set_local_color( vector32x4_c const & value )
	{
		_local_color = value;
	}

	menu_shared_color_class_e menu_element_c::get_shared_color_class() const
	{
		return _shared_color_class;
	}

	void_c menu_element_c::set_shared_color_class( menu_shared_color_class_e value, boolean_c swapped )
	{
		_shared_color_class = value;
		_shared_color_class_swapped = swapped;
	}

	boolean_c menu_element_c::get_is_showed() const
	{
		return _is_showed;
	}

	void_c menu_element_c::set_is_showed( boolean_c value )
	{
		_is_showed = value;
	}

	boolean_c menu_element_c::get_is_enabled() const
	{
		return _is_enabled;
	}

	void_c menu_element_c::set_is_enabled( boolean_c value )
	{
		_is_enabled = value;
	}

	boolean_c menu_element_c::get_is_selected() const
	{
		return _is_selected;
	}

	void_c menu_element_c::set_is_selected( boolean_c value )
	{
		_is_selected = value;
	}

	boolean_c menu_element_c::get_is_pressed() const
	{
		return _is_pressed;
	}

	void_c menu_element_c::set_is_pressed( boolean_c value )
	{
		_is_pressed = value;
	}

	boolean_c menu_element_c::get_is_overlay() const
	{
		return _is_overlay;
	}

	void_c menu_element_c::set_is_overlay( boolean_c value )
	{
		_is_overlay = value;
	}

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

}
