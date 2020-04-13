#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_check.h"
#include "cheonsa_menu_control_collection.h"
#include "cheonsa_menu_control_color_picker.h"
#include "cheonsa_menu_control_combo.h"
#include "cheonsa_menu_control_file_picker.h"
#include "cheonsa_menu_control_image.h"
#include "cheonsa_menu_control_label.h"
#include "cheonsa_menu_control_list.h"
#include "cheonsa_menu_control_menu.h"
#include "cheonsa_menu_control_panel.h"
#include "cheonsa_menu_control_property_inspector.h"
#include "cheonsa_menu_control_scene.h"
#include "cheonsa_menu_control_scroll_bar_x.h"
#include "cheonsa_menu_control_scroll_bar_y.h"
#include "cheonsa_menu_control_scrub_bar_x.h"
#include "cheonsa_menu_control_scrub_bar_y.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_element.h"
#include "cheonsa_user_interface.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	core_linked_list_c< menu_control_c * > menu_control_c::_global_list;

	menu_control_c * menu_control_c::make_new_instance( string8_c const & type )
	{
		if ( type == menu_control_button_c::get_type_name_static() )
		{
			return new menu_control_button_c();
		}
		else if ( type == menu_control_check_c::get_type_name_static() )
		{
			return new menu_control_check_c();
		}
		else if ( type == menu_control_collection_c::get_type_name_static() )
		{
			return new menu_control_collection_c();
		}
		else if ( type == menu_control_color_picker_c::get_type_name_static() )
		{
			return new menu_control_color_picker_c();
		}
		else if ( type == menu_control_combo_c::get_type_name_static() )
		{
			return new menu_control_combo_c();
		}
		else if ( type == menu_control_file_picker_c::get_type_name_static() )
		{
			return new menu_control_file_picker_c();
		}
		else if ( type == menu_control_image_c::get_type_name_static() )
		{
			return new menu_control_image_c();
		}
		else if ( type == menu_control_label_c::get_type_name_static() )
		{
			return new menu_control_label_c();
		}
		else if ( type == menu_control_list_c::get_type_name_static() )
		{
			return new menu_control_list_c();
		}
		else if ( type == menu_control_menu_list_c::get_type_name_static() )
		{
			return new menu_control_menu_list_c();
		}
		else if ( type == menu_control_property_inspector_c::get_type_name_static() )
		{
			return new menu_control_property_inspector_c( nullptr, nullptr );
		}
		else if ( type == menu_control_scene_c::get_type_name_static() )
		{
			return new menu_control_scene_c();
		}
		else if ( type == menu_control_scroll_bar_x_c::get_type_name_static() )
		{
			return new menu_control_scroll_bar_x_c();
		}
		else if ( type == menu_control_scroll_bar_y_c::get_type_name_static() )
		{
			return new menu_control_scroll_bar_y_c();
		}
		else if ( type == menu_control_scrub_bar_x_c::get_type_name_static() )
		{
			return new menu_control_scrub_bar_x_c();
		}
		else if ( type == menu_control_scrub_bar_y_c::get_type_name_static() )
		{
			return new menu_control_scrub_bar_y_c();
		}
		else if ( type == menu_control_text_c::get_type_name_static() )
		{
			return new menu_control_text_c();
		}
		
		return nullptr;
	}

	void_c menu_control_c::_handle_style_map_reference_on_refreshed( menu_style_map_c::reference_c const * value )
	{
		if ( _style_map_reference.get_value() )
		{
			// resolves styles for elements.
			for ( sint32_c i = 0; i < _style_map_reference.get_value()->_entry_list.get_length(); i++ )
			{
				menu_style_map_c::entry_c * entry = _style_map_reference.get_value()->_entry_list[ i ];
				core_list_c< menu_element_c * > element_list;
				_find_elements_with_name( entry->get_element_name(), element_list );
				for ( sint32_c j = 0; j < element_list.get_length(); j++ )
				{
					menu_element_c * element = element_list[ j ];
					if ( entry->get_style_key() == "" )
					{
						element->_is_showed_from_style = false;
						element->set_style_key( string8_c() );
					}
					else
					{
						element->_is_showed_from_style = true;
						element->set_style_key( entry->get_style_key() );
					}
				}
			}
		}
		else
		{
			// release style references from daughter elements.
			for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
			{
				menu_element_c * element = _element_list[ i ];
				element->set_style_key( string8_c() );
			}
		}
	}

	sint32_c menu_control_c::_give_control( menu_control_c * control, sint32_c index )
	{
		assert( control );
		assert( control != this );
		assert( control->_user_interface == nullptr );
		assert( control->_mother_control == nullptr );
		assert( control->_index == -1 );
		assert( index >= -1 && index <= _control_list.get_length() );

		// update relationships.
		if ( index == -1 )
		{
			index = _control_list.get_length();
		}
		control->_mother_control = this;
		control->_index = index;
		_control_list.insert_at_index( index, control );
		for ( sint32_c i = index + 1; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_index = i;
		}

		// update style map if possible.
		if ( _style_map_reference.get_value() )
		{
			if ( control->get_name().get_length() > 0 )
			{
				string8_c target;
				target += "control:";
				target += control->get_name();
				menu_style_map_c::entry_c const * style_map_entry = _style_map_reference.get_value()->find_entry( target );
				if ( style_map_entry )
				{
					control->set_style_map_key( style_map_entry->get_style_key() );
				}
			}
		}

		// update layout.
		//_content_bounds_is_dirty = true;
		control->_update_transform_and_layout();

		return index;
	}

	menu_control_c * menu_control_c::_take_control( sint32_c control_index )
	{
		assert( control_index >= 0 && control_index < _control_list.get_length() );
		user_interface_c * user_interface = get_user_interface_root();
		menu_control_c * control = _control_list[ control_index ];
		assert( control->_user_interface == nullptr );
		assert( control->_mother_control == this );
		assert( control->_index == control_index );
		if ( user_interface )
		{
			user_interface->_suspend_control( control );
		}
		_control_list.remove( control );
		control->_mother_control = nullptr;
		control->_index = -1;
		for ( sint32_c i = control_index; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_index = i;
		}
		//_content_bounds_is_dirty = true;
		return control;
	}

	void_c menu_control_c::_remove_and_delete_all_controls()
	{
		user_interface_c * user_interface = get_user_interface_root();
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * daughter_control = _control_list[ i ];
			if ( user_interface )
			{
				user_interface->_suspend_control( daughter_control );
			}
			delete daughter_control;
		}
		_control_list.remove_all();
		//_content_bounds_is_dirty = true;
	}

	menu_control_c * menu_control_c::_find_control( string8_c const & name, string8_c const & type )
	{
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			if ( type == control->get_type_name() && name == control->get_name() )
			{
				return control;
			}
		}
		return nullptr;
	}

	void_c menu_control_c::_add_element( menu_element_c * element )
	{
		assert( element );
		assert( element->_mother_control == nullptr );

		// update relationships.
		element->_mother_control = this;
		_element_list.insert_at_end( element );

		// update layout.
		element->update_layout( _local_box );

		// assign style from style map if possible.
		if ( _style_map_reference.get_value() )
		{
			if ( element->get_name().get_length() > 0 )
			{
				menu_style_map_c::entry_c const * style_map_entry = _style_map_reference.get_value()->find_entry( element->get_name() );
				if ( style_map_entry )
				{
					element->set_style_key( style_map_entry->get_style_key() );
				}
			}
		}
	}

	void_c menu_control_c::_remove_element( menu_element_c * element )
	{
		assert( element != nullptr );
		assert( element->_mother_control == this );
		_element_list.remove( element );
		element->_mother_control = nullptr;
	}

	menu_element_c * menu_control_c::_find_element( string8_c const & name, string8_c const & type )
	{
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			if ( type == element->get_type_name() )
			{
				if ( name.get_length() == 0 || name == element->get_name() )
				{
					return element;
				}
			}
		}
		return nullptr;
	}

	void_c menu_control_c::_find_elements_with_name( string8_c const & name, core_list_c< menu_element_c * > & result )
	{
		result.construct_mode_dynamic( 0 );
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * daughter = _element_list[ i ];
			if ( daughter->get_name() == name )
			{
				result.insert_at_end( daughter );
			}
		}
	}

	void_c menu_control_c::_update_global_space_properties()
	{
		// update _global_origin and _global_basis and _global_basis_inverted.
		if ( _mother_control != nullptr )
		{
			_global_origin = ops::make_vector32x2_transformed_point( _local_origin + _mother_control->_content_offset, _mother_control->_global_basis ) + _mother_control->_global_origin;
			_global_basis = _mother_control->_global_basis * _local_basis;
			_global_color = _mother_control->_global_color * _local_color;
		}
		else
		{
			_global_origin = _local_origin;
			_global_basis = _local_basis;
			_global_color = _local_color;
		}
		_global_basis_inverted = ops::make_matrix32x2x2_inverted( _global_basis );
	}

	void_c menu_control_c::_handle_resource_file_menu_on_load( resource_file_c * resource_file )
	{
		assert( resource_file != nullptr );
		assert( resource_file->get_is_loaded() );
		assert( _resource_file_menu == resource_file );
		load_static_data_properties_recursive( _resource_file_menu->get_markup().get_node( 1 ) );
	}

	void_c menu_control_c::_handle_resource_file_menu_on_unload( resource_file_c * resource_file )
	{
		assert( resource_file != nullptr );
		assert( resource_file->get_is_loaded() );
		load_static_data_properties_recursive( nullptr );
	}

	void_c menu_control_c::_on_user_interface_association_changed( user_interface_c * user_interface )
	{
		on_user_interface_association_changed.invoke( user_interface );
	}

	void_c menu_control_c::_on_is_showed_changed()
	{
		on_is_showed_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_enabled_changed()
	{
		on_is_enabled_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_deep_text_focused_changed()
	{
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_text_focused_changed()
	{
		on_is_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_mouse_overed_changed()
	{
		on_is_mouse_overed_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_mouse_focused_changed()
	{
		on_is_mouse_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_is_pressed_changed()
	{
		on_is_pressed_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_c::_on_clicked( input_event_c * input_event )
	{
		get_user_interface_root()->reset_multi_click_detection();
		on_clicked.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_c::_on_multi_clicked( input_event_c * input_event )
	{
		on_multi_clicked.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_c::_on_input( input_event_c * input_event )
	{
	}

	void_c menu_control_c::_update_transform_and_layout()
	{
		user_interface_c * user_interface = get_user_interface_root();
		if ( user_interface == nullptr )
		{
			return;
		}

		// update _local_basis.
		_local_basis = ops::make_matrix32x2x2_transform( _local_angle, _local_scale );

		// layout _local_box.
		if ( _layout_mode == menu_layout_mode_e_box_anchor )
		{
			box32x2_c mother_rectangle = _mother_control ? _mother_control->_local_box : user_interface->get_local_box();
			if ( ( _local_anchor & menu_anchor_e_left ) && ( _local_anchor & menu_anchor_e_right ) )
			{
				// anchor left and right edges.
				_local_box.minimum.a = mother_rectangle.minimum.a + _local_anchor_measures.minimum.a;
				_local_box.maximum.a = mother_rectangle.maximum.a - _local_anchor_measures.maximum.a;
			}
			else if ( _local_anchor & menu_anchor_e_left )
			{
				// anchor left edge.
				_local_box.minimum.a = mother_rectangle.minimum.a + _local_anchor_measures.minimum.a;
				_local_box.maximum.a = _local_box.minimum.a + _local_anchor_measures.maximum.a;
			}
			else if ( _local_anchor & menu_anchor_e_right )
			{
				// anchor right edge.
				_local_box.maximum.a = mother_rectangle.maximum.a - _local_anchor_measures.maximum.a;
				_local_box.minimum.a = _local_box.maximum.a - _local_anchor_measures.minimum.a;
			}
			if ( ( _local_anchor & menu_anchor_e_top ) && ( _local_anchor & menu_anchor_e_bottom ) )
			{
				// anchor top and bottom edges.
				_local_box.minimum.b = mother_rectangle.minimum.b + _local_anchor_measures.minimum.b;
				_local_box.maximum.b = mother_rectangle.maximum.b - _local_anchor_measures.maximum.b;
			}
			else if ( _local_anchor & menu_anchor_e_top )
			{
				// anchor top edge.
				_local_box.minimum.b = mother_rectangle.minimum.b + _local_anchor_measures.minimum.b;
				_local_box.maximum.b = _local_box.minimum.b + _local_anchor_measures.maximum.b;
			}
			else if ( _local_anchor & menu_anchor_e_bottom )
			{
				// anchor bottom edge.
				_local_box.maximum.b = mother_rectangle.maximum.b - _local_anchor_measures.maximum.b;
				_local_box.minimum.b = _local_box.maximum.b - _local_anchor_measures.minimum.b;
			}

			// constrain dimensions such that width and height don't become smaller than zero.
			if ( _local_box.maximum.a < _local_box.minimum.a )
			{
				_local_box.maximum.a = _local_box.minimum.a;
			}
			if ( _local_box.maximum.b < _local_box.minimum.b )
			{
				_local_box.maximum.b = _local_box.minimum.b;
			}

			//// if we are scaled differently from our mother then we can factor in corrective adjustments here.
			//// this keeps the effect of anchor offsets constant even as our local scale changes.
			//if ( _local_scale != 1.0f )
			//{
			//	float32_c local_scale_inverted = 1.0f / _local_scale;
			//	_local_box.minimum.a *= local_scale_inverted;
			//	_local_box.minimum.b *= local_scale_inverted;
			//	_local_box.maximum.a *= local_scale_inverted;
			//	_local_box.maximum.b *= local_scale_inverted;
			//}

			// set result_local_origin to center of result_local_box, then translate the result_local_box by the inverted.
			_local_origin = _local_box.get_center();
			_local_box.minimum -= _local_origin;
			_local_box.maximum -= _local_origin;
		}
		else if ( _layout_mode == menu_layout_mode_e_point_anchor )
		{
			box32x2_c mother_rectangle = _mother_control ? _mother_control->_local_box : user_interface->get_local_box();
			_local_origin = vector32x2_c();
			if ( _local_anchor & menu_anchor_e_left )
			{
				_local_origin.a = mother_rectangle.minimum.a + _local_anchor_measures.minimum.a;
			}
			else if ( _local_anchor & menu_anchor_e_right )
			{
				_local_origin.a = mother_rectangle.maximum.a - _local_anchor_measures.minimum.a;
			}
			if ( _local_anchor & menu_anchor_e_top )
			{
				_local_origin.b = mother_rectangle.minimum.b + _local_anchor_measures.minimum.b;
			}
			else if ( _local_anchor & menu_anchor_e_bottom )
			{
				_local_origin.b = mother_rectangle.maximum.b - _local_anchor_measures.minimum.b;
			}
		}

		// update _global_origin and _global_basis and _global_basis_inverted.
		_update_global_space_properties();

		// determine if local layout changed.
		if ( _old_local_box != _local_box )
		{
			_old_local_box = _local_box;

			// invalidate content bounds.
			//_content_bounds_is_dirty = true;
		}

		// layout daughter elements.
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->update_layout( _local_box );
		}

		// layout daughter controls.
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			control->_update_transform_and_layout();
		}
	}

	menu_control_c::menu_control_c()
		: _global_list_node( this )
		, _style_map_reference()
		, _name()
		, _user_interface( nullptr )
		, _mother_control( nullptr )
		, _index( -1 )
		, _content_offset( 0.0f, 0.0f )
		, _content_bounds( 0.0f, 0.0f, 0.0f, 0.0f )
		, _content_bounds_is_dirty( true )
		, _element_list()
		, _control_list()
		, _layer( menu_layer_e_undefined )
		, _select_mode( menu_select_mode_e_mouse )
		, _wants_to_be_deleted( false )
		, _is_showed( true )
		, _is_showed_weight( 0.0f )
		, _is_enabled( true )
		, _is_deep_text_focused( 0 )
		, _is_text_focused( 0 )
		, _is_mouse_overed( 0 )
		, _is_mouse_focused( 0 )
		, _is_pressed( 0 )
		, _layout_mode( menu_layout_mode_e_simple )
		, _local_origin( 0.0f, 0.0f )
		, _local_anchor( menu_anchor_e_none )
		, _local_anchor_measures( 0.0f, 0.0f, 0.0f, 0.0f )
		, _local_box( 0.0f, 0.0f, 100.0f, 100.0f )
		, _old_local_box( _local_box )
		, _local_angle( 0.0f )
		, _local_scale( 1.0f )
		, _local_basis()
		, _local_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _global_origin( 0.0f, 0.0f )
		, _global_basis()
		, _global_basis_inverted()
		, _global_color()
		, _control_group_is_root( false )
		, _control_group_mother( nullptr )
		, _control_group_daughter_list()
		, _control_group_texture( nullptr )
		, _control_group_texture_wrapper( true )
		, _control_group_origin()
		, _control_group_basis()
		, _control_group_draw_list()
		, _scene_component( nullptr )
		, _non_client_type( menu_non_client_type_e_none )
		, _user_pointer( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
		_style_map_reference.on_refreshed.subscribe( this, &menu_control_c::_handle_style_map_reference_on_refreshed );
	}

	menu_control_c::~menu_control_c()
	{
		_global_list.remove( &_global_list_node );

		if ( _control_group_texture != nullptr )
		{
			_control_group_texture_wrapper.set_video_texture( nullptr );
			delete _control_group_texture;
			_control_group_texture = nullptr;
		}

		//assert( get_user_interface() == nullptr );

		_control_list.remove_and_delete_all();
	}

	void_c menu_control_c::update_animations( float32_c time_step )
	{
		float32_c transition_step = engine_c::get_instance()->get_menu_style_manager()->get_shared_transition_speed() * time_step;
		_is_showed_weight = ops::math_saturate( _is_showed_weight + ( _is_showed ? transition_step : -transition_step ) );

		boolean_c is_selected = is_ascendant_of( get_user_interface_root()->get_mouse_overed() );
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->set_is_enabled( _is_enabled );
			element->set_is_selected( is_selected );
			element->set_is_pressed( _is_pressed && _is_mouse_overed );
			element->update_animations( time_step );
		}

		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			assert( control->get_index() == i );
			control->update_animations( time_step );
			if ( control->get_wants_to_be_deleted() && control->get_is_showed_weight() <= 0.0f )
			{
				_take_control( i );
				delete control;
				i--;
			}
		}
	}

	void_c menu_control_c::load_static_data_properties_recursive( data_scribe_markup_c::node_c const * node )
	{
		// exit early if nothing else to do.
		if ( node == nullptr )
		{
			return;
		}

		// load properties of this control.
		load_static_data_properties( node );

		// daughter controls.
		core_list_c< data_scribe_markup_c::node_c const * > sub_nodes;
		node->find_tags( "control", sub_nodes );
		for ( sint32_c i = 0; i < sub_nodes.get_length(); i++ )
		{
			data_scribe_markup_c::node_c const * sub_node = sub_nodes[ i ];
			if ( sub_node->get_type() == data_scribe_markup_c::node_c::type_e_tag )
			{
				data_scribe_markup_c::attribute_c const * type_attribute = sub_node->find_attribute( "type" );
				data_scribe_markup_c::attribute_c const * name_attribute = sub_node->find_attribute( "name" );
				if ( type_attribute && name_attribute )
				{
					menu_control_c * control = nullptr;
					control = _find_control( name_attribute->get_value(), type_attribute->get_value() );
					if ( control != nullptr )
					{
						control->load_static_data_properties_recursive( sub_node );
					}
				}
			}
		}
	}

	void_c menu_control_c::reset_static_data_properties()
	{
		set_style_map_key( string8_c() );
		_layout_mode = menu_layout_mode_e_simple;
		_local_origin = vector32x2_c( 0.0f, 0.0f );
		_local_anchor = menu_anchor_e_none;
		_local_box = box32x2_c( 0.0f, 0.0f, 100.0f, 100.0f );
		_local_angle = 0.0f;
		_local_scale = 1.0f;
		_local_color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_update_transform_and_layout();
	}

	void_c menu_control_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		//data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "style_map_key" );
		//_style_map_reference.set_key( attribute ? attribute->get_value() : string8_c() );

		data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "layout_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "box_anchor" )
			{
				_layout_mode = menu_layout_mode_e_box_anchor;
			}
			else if ( attribute->get_value() == "point_anchor" )
			{
				_layout_mode = menu_layout_mode_e_point_anchor;
			}
			else if ( attribute->get_value() == "simple" )
			{
				_layout_mode = menu_layout_mode_e_simple;
			}
		}

		attribute = node->find_attribute( "local_origin" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, _local_origin.as_array(), 2 ) );
		}

		attribute = node->find_attribute( "local_anchor" );
		if ( attribute )
		{
			sint32_c dummy = 0;
			_local_anchor = menu_anchor_e_none;
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "left" ), dummy ) )
			{
				_local_anchor |= menu_anchor_e_left;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "top" ), dummy ) )
			{
				_local_anchor |= menu_anchor_e_top;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "right" ), dummy ) )
			{
				_local_anchor |= menu_anchor_e_right;
			}
			if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "bottom" ), dummy ) )
			{
				_local_anchor |= menu_anchor_e_bottom;
			}
		}

		attribute = node->find_attribute( "local_anchor_measures" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, _local_anchor_measures.as_array(), 4 ) );
		}

		attribute = node->find_attribute( "local_box" );
		if ( attribute )
		{
			ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, _local_box.as_array(), 4 ) );
		}

		attribute = node->find_attribute( "local_angle" );
		if ( attribute )
		{
			ops::convert_string8_to_float32( attribute->get_value(), _local_angle );
		}

		attribute = node->find_attribute( "local_scale" );
		if ( attribute )
		{
			ops::convert_string8_to_float32( attribute->get_value(), _local_scale );
		}

		_update_transform_and_layout();
	}

	resource_file_menu_layout_c * menu_control_c::get_menu_layout() const
	{
		return _resource_file_menu;
	}

	void_c menu_control_c::set_menu_layout( resource_file_menu_layout_c * value )
	{
		if ( _resource_file_menu.is_reference_set() )
		{
			if ( _resource_file_menu->get_is_loaded() )
			{
				_handle_resource_file_menu_on_unload( _resource_file_menu );
			}
			_resource_file_menu->on_load.unsubscribe( this, &menu_control_c::_handle_resource_file_menu_on_load );
			_resource_file_menu->on_unload.unsubscribe( this, &menu_control_c::_handle_resource_file_menu_on_unload );
		}
		_resource_file_menu = value;
		if ( _resource_file_menu.is_reference_set() )
		{
			_resource_file_menu->on_load.subscribe( this, &menu_control_c::_handle_resource_file_menu_on_load );
			_resource_file_menu->on_unload.subscribe( this, &menu_control_c::_handle_resource_file_menu_on_unload );
			if ( _resource_file_menu->get_is_loaded() )
			{
				_handle_resource_file_menu_on_load( _resource_file_menu );
			}
		}
	}

	menu_control_c * menu_control_c::pick_control_with_global_point( vector32x2_c const & global_point, menu_layer_e & layer )
	{
		// ignore controls that are hiding or in the process of hiding.
		if ( _is_showed == false )
		{
			return nullptr;
		}

		// ignore this control if the point doesn't even touch us.
		if ( !contains_global_point( global_point ) )
		{
			return nullptr;
		}

		// test daughter controls.
		menu_control_c * best_pick = nullptr;
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * daughter = _control_list[ i ];
			menu_control_c * pick = daughter->pick_control_with_global_point( global_point, layer );
			if ( pick != nullptr )
			{
				best_pick = pick;
			}
		}

		// return daughter if we found one.
		if ( best_pick != nullptr )
		{
			return best_pick;
		}

		// finally, test against this.
		menu_layer_e this_layer = get_expressed_layer();
		if ( _select_mode != menu_select_mode_e_none && this_layer >= layer )
		{
			layer = this_layer;
			return this;
		}

		return nullptr;
	}

	vector32x2_c menu_control_c::transform_global_point_to_local_point( vector32x2_c const & global_point ) const
	{
		return ops::make_vector32x2_transformed_point( global_point - _global_origin, _global_basis_inverted );
	}

	vector32x2_c menu_control_c::transform_local_point_to_global_point( vector32x2_c const & local_point ) const
	{
		return ops::make_vector32x2_transformed_point( local_point, _global_basis ) + _global_origin;
	}

	boolean_c menu_control_c::contains_global_point( vector32x2_c const & global_point ) const
	{
		vector32x2_c local_point = transform_global_point_to_local_point( global_point );
		return ops::intersect_box_vs_point( _local_box, local_point );
	}

	boolean_c menu_control_c::contains_local_point( vector32x2_c const & local_point ) const
	{
		return ops::intersect_box_vs_point( _local_box, local_point );
	}

	string8_c const & menu_control_c::get_style_map_key() const
	{
		return _style_map_reference.get_key();
	}

	void_c menu_control_c::set_style_map_key( string8_c const & value )
	{
		_style_map_reference.set_key( value );
	}

	string8_c const & menu_control_c::get_name() const
	{
		return _name;
	}

	void_c menu_control_c::set_name( string8_c const & value )
	{
		_name = value;
	}

	sint32_c menu_control_c::get_index() const
	{
		return _index;
	}

	user_interface_c * menu_control_c::get_user_interface_root()
	{
		return get_root_mother_control()->_user_interface;
	}

	menu_control_c * menu_control_c::get_root_mother_control()
	{
		menu_control_c * control = this;
		while ( control->_mother_control )
		{
			control = control->_mother_control;
		}
		return control;
	}

	menu_control_c * menu_control_c::get_mother_control()
	{
		return _mother_control;
	}

	sint32_c menu_control_c::get_control_count() const
	{
		return _control_list.get_length();
	}

	menu_control_c * menu_control_c::get_control( sint32_c control_index ) const
	{
		return _control_list[ control_index ];
	}

	boolean_c menu_control_c::is_ascendant_of( menu_control_c * control )
	{
		while ( control != nullptr )
		{
			if ( control == this )
			{
				return true;
			}
			control = control->_mother_control;
		}
		return false;
	}

	menu_layer_e menu_control_c::get_expressed_layer()
	{
		menu_control_c * control = this;
		while ( control != nullptr )
		{
			if ( control->_layer != menu_layer_e_undefined )
			{
				return control->_layer;
			}
			control = control->_mother_control;
		}
		return menu_layer_e_base;
	}

	menu_select_mode_e menu_control_c::get_select_mode() const
	{
		return _select_mode;
	}

	void_c menu_control_c::set_select_mode( menu_select_mode_e value )
	{
		_select_mode = value;
	}

	boolean_c menu_control_c::get_wants_to_be_deleted() const
	{
		return _wants_to_be_deleted;
	}

	boolean_c menu_control_c::get_is_showed() const
	{
		return _is_showed;
	}

	float32_c menu_control_c::get_is_showed_weight() const
	{
		return _is_showed_weight;
	}

	void_c menu_control_c::set_is_showed( boolean_c value, boolean_c and_wants_to_be_deleted )
	{
		if ( value )
		{
			if ( _is_showed == false )
			{
				_is_showed = true;
				_wants_to_be_deleted = false;
				_on_is_showed_changed();
			}
		}
		else
		{
			if ( _is_showed == true )
			{
				_is_showed = false;
				_wants_to_be_deleted = and_wants_to_be_deleted;
				_on_is_showed_changed();
				if ( user_interface_c * user_interface = get_user_interface_root() )
				{
					user_interface->_suspend_control( this );
				}
			}
		}
	}

	void_c menu_control_c::set_is_showed_immediately( boolean_c value )
	{
		if ( value )
		{
			_is_showed = true;
			_is_showed_weight = 1.0f;
			_wants_to_be_deleted = false;
			_on_is_showed_changed();
		}
		else
		{
			_is_showed = false;
			_is_showed_weight = 0.0f;
			_wants_to_be_deleted = false;
			_on_is_showed_changed();
			if ( user_interface_c * user_interface = get_user_interface_root() )
			{
				user_interface->_suspend_control( this );
			}
		}
	}

	boolean_c menu_control_c::get_is_enabled() const
	{
		return _is_enabled;
	}

	void_c menu_control_c::set_is_enabled( boolean_c value )
	{
		_is_enabled = value;
		_on_is_enabled_changed();
		if ( value == false )
		{
			if ( user_interface_c * user_interface = get_user_interface_root() )
			{
				user_interface->_suspend_control( this );
			}
		}
	}

	sint32_c menu_control_c::get_is_deep_text_focused() const
	{
		return _is_deep_text_focused;
	}

	boolean_c menu_control_c::get_is_mouse_overed() const
	{
		return _is_mouse_overed;
	}

	boolean_c menu_control_c::get_is_mouse_focused() const
	{
		return _is_mouse_focused;
	}

	boolean_c menu_control_c::get_is_text_focused() const
	{
		return _is_text_focused;
	}

	boolean_c menu_control_c::get_is_pressed() const
	{
		return _is_pressed;
	}

	menu_state_e menu_control_c::get_state() const
	{
		if ( _is_enabled )
		{
			if ( _is_pressed )
			{
				return menu_state_e_pressed;
			}
			else if ( _is_mouse_focused || _is_text_focused )
			{
				return menu_state_e_selected;
			}
			else
			{
				return menu_state_e_normal;
			}
		}
		else
		{
			return menu_state_e_disabled;
		}
	}

	void_c menu_control_c::set_layout_simple( vector32x2_c const & local_origin, box32x2_c const & local_box_around_origin, float32_c local_angle, float32_c local_scale )
	{
		_layout_mode = menu_layout_mode_e_simple;
		_local_anchor = menu_anchor_e_none;
		_local_origin = local_origin;
		_local_box = local_box_around_origin;
		_local_angle = local_angle;
		_local_scale = local_scale;
		_update_transform_and_layout();
	}

	void_c menu_control_c::set_layout_simple( box32x2_c const & local_box, float32_c local_angle, float32_c local_scale )
	{
		_layout_mode = menu_layout_mode_e_simple;
		_local_anchor = menu_anchor_e_none;
		_local_origin = local_box.get_center();
		_local_box = local_box;
		_local_angle = local_angle;
		_local_scale = local_scale;
		_local_box.minimum -= _local_origin;
		_local_box.maximum -= _local_origin;
		_update_transform_and_layout();
	}

	void_c menu_control_c::set_layout_box_anchor( menu_anchor_e local_anchor, box32x2_c const & local_anchor_measures, float32_c local_angle, float32_c local_scale )
	{
		_layout_mode = menu_layout_mode_e_box_anchor;
		_local_anchor = local_anchor;
		_local_anchor_measures = local_anchor_measures;
		_local_angle = local_angle;
		_local_scale = local_scale;
		_update_transform_and_layout();
	}

	void_c menu_control_c::set_layout_point_anchor( menu_anchor_e local_anchor, vector32x2_c const & local_anchor_measures, box32x2_c const & local_box, float32_c local_angle, float32_c local_scale )
	{
		_layout_mode = menu_layout_mode_e_point_anchor;
		_local_anchor = local_anchor;
		_local_anchor_measures.minimum = local_anchor_measures;
		_local_anchor_measures.maximum = vector32x2_c();
		_local_box = local_box;
		_local_angle = local_angle;
		_local_scale = local_scale;
		_update_transform_and_layout();
	}

	vector32x2_c const & menu_control_c::get_local_origin() const
	{
		return _local_origin;
	}

	void_c menu_control_c::set_local_origin( vector32x2_c const & value )
	{
		_local_origin = value;
		_update_transform_and_layout();
	}

	box32x2_c const & menu_control_c::get_local_box() const
	{
		return _local_box;
	}

	box32x2_c const & menu_control_c::get_local_anchor_measures() const
	{
		return _local_anchor_measures;
	}

	void_c menu_control_c::set_local_anchor_measures( box32x2_c const & value )
	{
		_local_anchor_measures = value;
		_update_transform_and_layout();
	}

	float32_c menu_control_c::get_local_angle() const
	{
		return _local_angle;
	}

	void_c menu_control_c::set_local_angle( float32_c value )
	{
		_local_angle = value;
		_update_transform_and_layout();
	}

	float32_c menu_control_c::get_local_scale() const
	{
		return _local_scale;
	}

	void_c menu_control_c::set_local_scale( float32_c value )
	{
		_local_scale = value;
		_update_transform_and_layout();
	}

	vector32x4_c const & menu_control_c::get_local_color() const
	{
		return _local_color;
	}

	void_c menu_control_c::set_local_color( vector32x4_c const & value )
	{
		_local_color = value;
		_update_transform_and_layout();
	}

	vector32x2_c const & menu_control_c::get_global_origin() const
	{
		return _global_origin;
	}

	matrix32x2x2_c const & menu_control_c::get_global_basis() const
	{
		return _global_basis;
	}

	matrix32x2x2_c const & menu_control_c::get_global_basis_inverted() const
	{
		return _global_basis_inverted;
	}

	vector32x4_c const & menu_control_c::get_global_color() const
	{
		return _global_color;
	}

	scene_component_menu_control_c * menu_control_c::get_scene_component() const
	{
		return _scene_component;
	}

	void_c menu_control_c::set_scene_component( scene_component_menu_control_c * value )
	{
		_scene_component = value;
	}

	menu_non_client_type_e menu_control_c::get_non_client_type() const
	{
		return _non_client_type;
	}

	void_c menu_control_c::set_non_client_type( menu_non_client_type_e value )
	{
		_non_client_type = value;
	}

	void_c * menu_control_c::get_user_pointer() const
	{
		return _user_pointer;
	}

	void_c menu_control_c::set_user_pointer( void_c * value )
	{
		_user_pointer = value;
	}

	void_c menu_control_c::bring_to_front()
	{
		if ( _user_interface )
		{
			_user_interface->bring_control_to_front( this );
		}
		else if ( _mother_control )
		{
			assert( _mother_control->_control_list[ _index ] == this );
			_mother_control->_control_list.remove_at_index( _index );
			_mother_control->_control_list.insert_at_index( 0, this );
			for ( sint32_c i = 0; i < _mother_control->_control_list.get_length(); i++ )
			{
				_mother_control->_control_list[ i ]->_index = i;
			}
		}
	}

	void_c menu_control_c::give_text_focus()
	{
		user_interface_c * user_interface = get_user_interface_root();
		if ( user_interface )
		{
			user_interface->set_text_focused( this );
		}
	}

	box32x2_c const & menu_control_c::get_content_bounds()
	{
		if ( _content_bounds_is_dirty )
		{
			_content_bounds_is_dirty = false;
			_content_bounds = box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f );
			boolean_c first = true;
			for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
			{
				menu_control_c const * control = _control_list[ i ];
				box32x2_c control_box = ops::make_aabb_from_obb( control->_local_box, vector32x2_c(), control->_local_basis );
				if ( first )
				{
					first = false;
					_content_bounds = control_box;
				}
				else
				{
					_content_bounds.accumulate_bounds( control_box );
				}
			}
			for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
			{
				menu_element_c const * element = _element_list[ i ];
				box32x2_c const & element_box = element->_local_box;
				if ( first )
				{
					first = false;
					_content_bounds = element_box;
				}
				else
				{
					_content_bounds.accumulate_bounds( element_box );
				}
			}
		}
		return _content_bounds;
	}

	vector32x2_c const & menu_control_c::get_content_offset() const
	{
		return _content_offset;
	}

	void_c menu_control_c::set_content_offset( vector32x2_c const & value )
	{
		_content_offset = value;
		_update_transform_and_layout();
	}

	void_c menu_control_c::set_content_offset_horizontal( float32_c value )
	{
		_content_offset.a = value;
		_update_transform_and_layout();
	}

	void_c menu_control_c::set_content_offset_vertical( float32_c value )
	{
		_content_offset.b = value;
		_update_transform_and_layout();
	}

	void_c menu_control_c::get_control_group_clip_planes( core_list_c< vector32x4_c > & result )
	{
		menu_control_c * control = this;
		while ( control != nullptr && !control->_control_group_is_root )
		{
			matrix32x2x2_c control_group_basis = control->get_control_group_basis();
			vector32x2_c control_group_origin = control->get_control_group_origin();
			vector32x2_c minimum = ops::make_vector32x2_transformed_point( control->_local_box.minimum, control_group_basis ) + control_group_origin;
			vector32x2_c maximum = ops::make_vector32x2_transformed_point( control->_local_box.maximum, control_group_basis ) + control_group_origin;
			vector32x2_c normal = ops::make_vector32x2_normalized( ops::make_vector32x2_transformed_point( vector32x2_c( 1.0f, 0.0f ), control_group_basis ) );
			vector32x4_c * plane = result.emplace_at_end();
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::make_float32_dot_product( normal, minimum );
			plane->d = 0.0f;
			normal = ops::make_vector32x2_normalized( ops::make_vector32x2_transformed_point( vector32x2_c( 0.0f, 1.0f ), control_group_basis ) );
			plane = result.emplace_at_end();
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::make_float32_dot_product( normal, minimum );
			plane->d = 0.0f;
			normal = ops::make_vector32x2_normalized( ops::make_vector32x2_transformed_point( vector32x2_c( -1.0f, 0.0f ), control_group_basis ) );
			plane = result.emplace_at_end();
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::make_float32_dot_product( normal, maximum );
			plane->d = 0.0f;
			normal = ops::make_vector32x2_normalized( ops::make_vector32x2_transformed_point( vector32x2_c( 0.0f, -1.0f ), control_group_basis ) );
			plane = result.emplace_at_end();
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::make_float32_dot_product( normal, maximum );
			plane->d = 0.0f;
			control = control->_mother_control;
		}
	}

	matrix32x2x2_c menu_control_c::get_control_group_basis() const
	{
		return _control_group_basis;
	}

	vector32x2_c menu_control_c::get_control_group_origin() const
	{
		return _control_group_origin;
	}

	void_c menu_control_c::_compile_control_groups( core_list_c< menu_control_c * > & control_group_list, core_list_c< menu_draw_list_c * > & draw_list_list )
	{
		if ( _scene_component != nullptr )
		{
			// todo: control is 3d, we can return here if it is not visible to the primary (camera) view.
		}

		if ( _is_showed_weight <= 0.0f || _local_color.d <= 0.0f )
		{
			return;
		}

		// determine control group root state of this control.
		_control_group_is_root = ( _mother_control == nullptr || _is_showed_weight < 1.0f || _local_color.d < 1.0f );
		_control_group_mother = nullptr;
		_control_group_daughter_list.remove_all();
		_control_group_draw_list.reset();
		if ( _mother_control != nullptr )
		{
			_control_group_mother = _mother_control;
			while ( _control_group_mother->_control_group_is_root == false )
			{
				_control_group_mother = _control_group_mother->_mother_control;
				assert( _control_group_mother != nullptr );
			}
			if ( _control_group_is_root )
			{
				_control_group_mother->_control_group_daughter_list.insert_at_end( this );
			}
		}
		if ( _control_group_is_root )
		{
			control_group_list.insert_at_end( this );

			// create or recreate control group texture target.
			sint32_c needed_width = ops::math_maximum( 16, static_cast< sint32_c >( _local_box.get_width() + 0.5f ) );
			sint32_c needed_height = ops::math_maximum( 16, static_cast< sint32_c >( _local_box.get_height() + 0.5f ) );
			if ( _control_group_texture == nullptr || ( _control_group_texture->get_width() < needed_width || _control_group_texture->get_height() < needed_height ) )
			{
				if ( _control_group_texture != nullptr )
				{
					delete _control_group_texture;
					_control_group_texture = nullptr;
				}
				assert( needed_width <= 4096 && needed_height <= 4096 );
				_control_group_texture = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r8g8b8a8_unorm, needed_width, needed_height, 1, 1, nullptr, 0, false, false, true, false );
				assert( _control_group_texture != nullptr );
				_control_group_texture_wrapper.set_video_texture( _control_group_texture );
			}

			// control group spatial transform.
			_control_group_basis = matrix32x2x2_c( 1.0f, 0.0f, 0.0f, 1.0f );
			_control_group_origin = vector32x2_c( 0.0f, 0.0f );
			_control_group_color = _local_color;

			// compile draw list that will copy the _control_group_texture to the mother control group.
			box32x2_c map;
			map.minimum.a = 0.0f;
			map.minimum.b = 0.0f;
			map.maximum.a = static_cast< float32_c >( _local_box.get_width() ) / static_cast< float32_c >( _control_group_texture->get_width() );
			map.maximum.b = static_cast< float32_c >( _local_box.get_height() ) / static_cast< float32_c >( _control_group_texture->get_height() );
			box32x2_c box = _local_box;
			_control_group_draw_list.append_rectangle( box, map, engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu_ps_frame(), &_control_group_texture_wrapper, vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ), nullptr );
			draw_list_list.insert_at_end( &_control_group_draw_list );
		}
		else
		{
			assert( _mother_control != nullptr );

			// release control group texture target.
			if ( _control_group_texture != nullptr )
			{
				delete _control_group_texture;
				_control_group_texture = nullptr;
				_control_group_texture_wrapper.set_video_texture( nullptr );
			}

			// control group spatial transform.
			_control_group_basis = _mother_control->_control_group_basis * _local_basis;
			_control_group_origin = ops::make_vector32x2_transformed_point( _local_origin, _mother_control->_control_group_basis ) + _mother_control->_control_group_origin;
			_control_group_color = _mother_control->_control_group_color * _local_color;
		}

		// compile daughter elements.
		for ( sint32_c i = 0; i < _element_list.get_length(); i++ )
		{
			menu_element_c * element = _element_list[ i ];
			element->_draw_list_is_dirty = true; // for now always force rebuild.
			if ( element->_draw_list_is_dirty )
			{
				element->_build_draw_list();
			}
			if ( element->_is_showed && element->_is_showed_from_style )
			{
				draw_list_list.insert_at_end( &element->_draw_list );
			}
		}

		// compile daughter controls.
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_compile_control_groups( control_group_list, draw_list_list );
		}
	}

}
