#include "cheonsa_user_interface.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	boolean_c user_interface_c::_pick_control( input_event_c * input_event, menu_control_c * & result )
	{
		// track result.
		menu_control_c * picked_control = nullptr;
		float64_c picked_control_distance = 0.0f;
		menu_layer_e picked_control_layer = menu_layer_e_undefined;

		// fist pass, try to pick 2d.
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			if ( control->_scene_component_menu_control == nullptr )
			{
				menu_control_c * candidate_control = nullptr;
				float64_c candidate_control_distance = 0.0;
				menu_layer_e candidate_control_layer = menu_layer_e_undefined;
				if ( _pick_control_level_2( input_event, control, candidate_control_layer, candidate_control, candidate_control_distance ) )
				{
					if ( control->get_expressed_layer() > picked_control_layer ) // 2d prioritizes layers and disregards distance, which is different from 3d.
					{
						picked_control = candidate_control;
						picked_control_distance = candidate_control_distance;
						picked_control_layer = candidate_control_layer;
					}
				}
			}
		}

		// second pass, try to pick 3d.
		if ( picked_control == nullptr ) // it's only valid to try to pick 3d if 2d didn't hit anything.
		{
			for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
			{
				menu_control_c * control = _control_list[ i ];
				if ( control->_scene_component_menu_control != nullptr )
				{
					menu_control_c * candidate_control = nullptr;
					float64_c candidate_control_distance = 0.0;
					menu_layer_e candidate_control_layer = menu_layer_e_undefined;
					if ( _pick_control_level_2( input_event, control, candidate_control_layer, candidate_control, candidate_control_distance ) )
					{
						if ( picked_control == nullptr || candidate_control_distance < picked_control_distance ) // 3d prioritizes distance and disregards layers, which is different from 2d.
						{
							picked_control = candidate_control;
							picked_control_distance = candidate_control_distance;
							picked_control_layer = candidate_control_layer;
						}
					}
				}
			}
		}

		if ( _mouse_overed != picked_control )
		{
			if ( _mouse_overed != nullptr )
			{
				_mouse_overed->_is_mouse_overed = false;
				_mouse_overed->_on_mouse_over_lost();
			}
			_mouse_overed = picked_control;
			if ( _mouse_overed != nullptr )
			{
				_mouse_overed->_is_mouse_overed = true;
				_mouse_overed->_on_mouse_over_gained();
			}
		}

		result = picked_control;

		return picked_control != nullptr;
	}

	boolean_c user_interface_c::_pick_control_level_2( input_event_c * input_event, menu_control_c * window, menu_layer_e minimum_layer, menu_control_c * & result, float64_c & result_distance )
	{
		if ( window->_is_showing == true )
		{
			float64_c control_distance = 0.0; // for 3d menus, this is the distance from the camera to the point of intersection with the menu. for 2d menus this is unused and will always be 0.
			if ( window->_scene_component_menu_control == nullptr )
			{
				control_distance = 0.0f;
				//input_event->menu_mouse_position = input_event->mouse_position; // - _canvas_size * 0.5f; // local 2D menu space mouse position is just the mouse position from the input manager.
			}
			else
			{
				assert( _scene != nullptr );
				space_transform_c world_space_window_transform = window->get_scene_component()->get_scene_object()->get_world_space_transform();
				world_space_window_transform.scale *= 0.001f; // convert size of pixels from meters to millimeters.
				matrix32x3x3_c world_space_window_basis = world_space_window_transform.get_scaled_basis();
				space_transform_c world_space_window_transform_inverted = world_space_window_transform.get_inverted();
				plane64_c world_space_window_plane = ops::make_plane64_from_normal_and_point( vector64x3_c( world_space_window_basis.c / world_space_window_transform.scale ), world_space_window_transform.position );
				ray64_c world_space_mouse_ray = _scene->get_scene_camera().build_ray_through_canvas( input_event->mouse_position, static_cast< sint32_c >( _local_box.get_width() ), static_cast< sint32_c >( _local_box.get_height() ) ); // ASSUMING that _local_box is the same size as the canvas.
				if ( ops::sweep_ray_vs_plane( world_space_mouse_ray, world_space_window_plane, control_distance ) ) // find if there is an intersection and if so find out the distance to intersection
				{
					vector64x3_c world_space_intersection_point = world_space_mouse_ray.position + ( world_space_mouse_ray.normal * control_distance );
					space_transform_c world_space_window_transform_inverted = window->get_scene_component()->get_scene_object()->get_world_space_transform().get_inverted();
					vector64x3_c menu_space_intersection_point = ops::make_vector64x3_transformed_point( world_space_intersection_point, world_space_window_transform_inverted );
					input_event->menu_mouse_position.a = static_cast< float32_c >( menu_space_intersection_point.a );
					input_event->menu_mouse_position.b = static_cast< float32_c >( menu_space_intersection_point.b );
				}
			}
			menu_control_c * candidate = window->pick_control_with_global_point( input_event->menu_mouse_position, minimum_layer );
			if ( candidate != nullptr && ( result == nullptr || control_distance < result_distance ) )
			{
				_is_mouse_inside = true;
				result = candidate;
				result_distance = control_distance;
				return true;
			}
		}
		return false;
	}

	void_c user_interface_c::_bubble_input_event( menu_control_c * control, input_event_c * input_event )
	{
		while ( control && !input_event->processed )
		{
			control->_on_input( input_event );
			control = control->_mother_control;
		}
	}

	void_c user_interface_c::_process_input_event( input_event_c * input_event )
	{
		// reset things.
		_is_mouse_inside = false;
		//_is_modal_window_open = false;

		// default to 2d, until we have to pick 3d.
		input_event->menu_mouse_position = input_event->mouse_position;

		// determine if there is a top level modal window that is showing.
		// if a modal window is open then it has exclusive access to input events.
		//menu_control_c * modal_window = nullptr;
		//for ( sint32_c i = 0; i < _modal_public_daughter_control_list.get_length(); i++ )
		//{
		//	if ( _modal_public_daughter_control_list[ i ]->_is_showing )
		//	{
		//		_is_modal_window_open = true;
		//		modal_window = _modal_public_daughter_control_list[ i ];
		//		break;
		//	}
		//}

		// route mouse move event to appropriate control.
		// do this before picking controls under the mouse, because this lets controls update in response to drag and drop operations.
		// which can change the mouse pick result.
		if ( input_event->type == input_event_c::type_e_mouse_move )
		{
			if ( _mouse_focused )
			{
				_bubble_input_event( _mouse_focused, input_event );
			}
		}

		// update mouse overed and mouse focused.
		_pick_control( input_event, _mouse_overed );
		boolean_c mouse_keys_are_up = ( input_event->mouse_keys_state[ input_mouse_key_e_left ] & input_key_state_bit_e_on ) == 0 && ( input_event->mouse_keys_state[ input_mouse_key_e_right ] & input_key_state_bit_e_on ) == 0 && ( input_event->mouse_keys_state[ input_mouse_key_e_middle ] & input_key_state_bit_e_on ) == 0;
		if ( mouse_keys_are_up )
		{
			_pick_control( input_event, _mouse_overed );
			if ( _mouse_focused != _mouse_overed )
			{
				if ( _mouse_focused )
				{
					_mouse_focused->_is_pressed = false;
					_mouse_focused->_is_mouse_focused = false;
					_mouse_focused->_on_mouse_focus_lost();
				}
				_mouse_focused = _mouse_overed;
				if ( _mouse_focused )
				{
					_mouse_focused->_is_mouse_focused = true;
					_mouse_focused->_on_mouse_focus_gained();
				}
			}
		}

		// route other events to appropriate control.
		if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			boolean_c do_multi_click = false;
			if ( input_event->mouse_key == input_mouse_key_e_left || input_event->mouse_key == input_mouse_key_e_right || input_event->mouse_key == input_mouse_key_e_middle )
			{
				if ( _text_focused != _mouse_focused )
				{
					set_text_focused( _mouse_focused );
				}
			}
			if ( _mouse_focused )
			{
				if ( input_event->mouse_key == input_mouse_key_e_left )
				{
					_mouse_focused->_is_pressed = true;
				}
				_bubble_input_event( _mouse_focused, input_event );
				if ( input_event->mouse_key_multi_click_count == 2 )
				{
					_mouse_focused->_on_multi_clicked( input_event );
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			if ( _mouse_focused )
			{
				if ( input_event->mouse_key == input_mouse_key_e_left )
				{
					_mouse_focused->_is_pressed = false;
				}
				_bubble_input_event( _mouse_focused, input_event );
			}
			if ( input_event->mouse_key == input_mouse_key_e_left || input_event->mouse_key == input_mouse_key_e_right || input_event->mouse_key == input_mouse_key_e_middle )
			{
				if ( _mouse_focused && _mouse_focused == _mouse_overed && input_event->mouse_key == input_mouse_key_e_left )
				{
					_mouse_focused->_on_clicked( input_event );
				}
			}
		}
		else // if ( input_event->event_type == input_event_type_e_character )
		{
			if ( _text_focused )
			{
				_bubble_input_event( _text_focused, input_event );
			}
		}

		// do mouse pick, potentially for the second time.
		//mouse_hit_was_blocked |= _pick_control_level_1( input_event, modal_window );

		input_event->processed |= _mouse_focused != nullptr || _mouse_overed != nullptr;
	}

	user_interface_c::user_interface_c()
		: _canvas_and_output( nullptr )
		, _scene( nullptr )
		, _control_list()
		, _mouse_overed( nullptr )
		, _mouse_focused( nullptr )
		, _text_focused( nullptr )
		, _is_mouse_inside( false )
		, _is_set_text_focus( false )
		, _local_box()

	{
	}

	user_interface_c::~user_interface_c()
	{
		delete _canvas_and_output;
		_canvas_and_output = nullptr;
	}

	boolean_c user_interface_c::start( void_c * window_handle )
	{
		assert( window_handle != nullptr );

		_canvas_and_output = new video_renderer_canvas_c( true, true, window_handle );

		update_canvas();

		return true;
	}

	void_c user_interface_c::update_canvas()
	{
		// update canvas and size.
		_canvas_and_output->update();
		sint32_c width = _canvas_and_output->get_apparent_width();
		sint32_c height = _canvas_and_output->get_apparent_height();
		if ( width < 100 )
		{
			width = 100;
		}
		if ( height < 100 )
		{
			height = 100;
		}
		box32x2_c local_box = box32x2_c( 0.0f, 0.0f, static_cast< float32_c >( width ), static_cast< float32_c >( height ) );
		if ( global_engine_instance.environment.get_window_state() == window_state_e_maximized )
		{
			sint32_c compensate = 8; // for some reason, when windows are maximized in windows, their area is virtually 8 pixels larger than the screen.
			local_box.minimum.a += compensate;
			local_box.minimum.b += compensate;
			local_box.maximum.a -= compensate;
			local_box.maximum.b -= compensate;
		}
		if ( _local_box != local_box )
		{
			_local_box = local_box;
			on_local_box_changed.invoke( this );
			for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
			{
				menu_control_c * control = _control_list[ i ];
				if ( control->_scene_component_menu_control == nullptr )
				{
					control->update_transform_and_layout();
				}
			}
		}
	}

	void_c user_interface_c::update( float32_c time_step )
	{
		// update canvas.
		update_canvas();

		// process input events.
		boolean_c wants_to_refresh_resources = false;
		core_list_c< input_event_c > & input_events = global_engine_instance.interfaces.input_manager->get_events();
		for ( sint32_c i = 0; i < input_events.get_length(); i++ )
		{
			input_event_c & input_event = input_events[ i ];

			// check for engine hot keys.
			if ( input_event.type == input_event_c::type_e_keyboard_key_pressed )
			{
				if ( input_event.keyboard_key == debug_key_toggle_console )
				{
					global_engine_instance.debug.set_console_is_showing( !global_engine_instance.debug.get_console_is_showing() );
				}
				else if ( input_event.keyboard_key == debug_key_toggle_stats )
				{
					global_engine_instance.debug.set_statistics_is_showing( !global_engine_instance.debug.get_statistics_is_showing() );
				}
				else if ( input_event.keyboard_key == debug_key_toggle_menu_bounds )
				{
					global_engine_instance.debug.draw_menu_bounds = !global_engine_instance.debug.draw_menu_bounds;
				}
				else if ( input_event.keyboard_key == debug_key_refresh_resources )
				{
					wants_to_refresh_resources = true;
				}
			}

			// let the menus process the input event.
			_process_input_event( &input_event );

			// let the game process the input event.
			global_engine_instance.interfaces.game->process_input_event( &input_event );
		}

		// update animations of controls, and delete any that want to be deleted.
		for ( sint32_c i = 0; i < _control_list.get_length(); i++ )
		{
			menu_control_c * control = _control_list[ i ];
			control->_index = i;
			control->update_animations( time_step );
			if ( control->_wants_to_be_deleted && control->_is_showing_weight <= 0.0f )
			{
				_control_list.remove_at_index( i );
				delete control;
				i--;
			}
		}

		// reload resources.
		if ( wants_to_refresh_resources )
		{
			global_engine_instance.interfaces.video_renderer_shader_manager->refresh();
			global_engine_instance.interfaces.resource_manager->refresh();
			global_engine_instance.interfaces.menu_style_manager->refresh();
		}
	}

	void_c user_interface_c::render_and_present( float32_c time_step )
	{
		// clear the canvas.
		_canvas_and_output->clear();

		// pre-render menus.
		global_engine_instance.interfaces.video_renderer_interface->pre_render_menus( this );

		// render 3d scene and 3d menu controls to canvas.
		if ( _scene )
		{
			global_engine_instance.interfaces.video_renderer_interface->render_scene( _scene, &_scene->get_scene_camera(), _canvas_and_output );
		}

		// render 2d menus to canvas.
		global_engine_instance.interfaces.video_renderer_interface->render_2d_menus( this );

		// flip canvas back buffer to output window.
		_canvas_and_output->present();
	}

	//sint32_c user_interface_c::get_width() const
	//{
	//	return static_cast< sint32_c >( _local_box.get_size_a() );
	//}

	//sint32_c user_interface_c::get_height() const
	//{
	//	return static_cast< sint32_c >( _local_box.get_size_b() );
	//}

	box32x2_c const & user_interface_c::get_local_box() const
	{
		return _local_box;
	}

	scene_c * user_interface_c::get_scene() const
	{
		return _scene;
	}

	void_c user_interface_c::set_scene( scene_c * scene )
	{
		// remove controls from old scene.
		if ( _scene )
		{
			global_engine_instance.interfaces.audio_interface->remove_audio_scene( _scene->get_audio_scene() );
			core_linked_list_c< scene_object_c * >::node_c const * scene_object_list_node = _scene->get_scene_object_list().get_first();
			while ( scene_object_list_node )
			{
				core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = scene_object_list_node->get_value()->get_scene_component_list()->get_first();
				while ( scene_component_list_node )
				{
					if ( scene_component_list_node->get_value()->get_type_code() == scene_component_menu_control_c::get_type_code_static() )
					{
						scene_component_menu_control_c * scene_component_menu_control = static_cast< scene_component_menu_control_c * >( scene_component_list_node->get_value() );
						if ( scene_component_menu_control->_control )
						{
							_control_list.remove( scene_component_menu_control->_control );
						}
					}
				}
				scene_object_list_node = scene_object_list_node->get_next();
			}
		}

		// set new scene and forget old one.
		_scene = scene;

		// add controls from new scene.
		if ( _scene )
		{
			core_linked_list_c< scene_object_c * >::node_c const * scene_object_list_node = _scene->get_scene_object_list().get_first();
			while ( scene_object_list_node )
			{
				core_linked_list_c< scene_component_c * >::node_c const * scene_component_list_node = scene_object_list_node->get_value()->get_scene_component_list()->get_first();
				while ( scene_component_list_node )
				{
					if ( scene_component_list_node->get_value()->get_type_code() == scene_component_menu_control_c::get_type_code_static() )
					{
						scene_component_menu_control_c * scene_component_menu_control = static_cast< scene_component_menu_control_c * >( scene_component_list_node->get_value() );
						if ( scene_component_menu_control->_control )
						{
							_control_list.insert_at_end( scene_component_menu_control->_control );
						}
					}
				}
			}
			global_engine_instance.interfaces.audio_interface->add_audio_scene( _scene->get_audio_scene() );
		}
	}

	void_c user_interface_c::add_control( menu_control_c * control, sint32_c index )
	{
		assert( control != nullptr );
		assert( control->_user_interface == nullptr );
		assert( control->_mother_control == nullptr );
		assert( control->_index == -1 );
		assert( index >= -1 && index <= _control_list.get_length() );
		assert( control->_name.get_length() > 0 );
		control->_user_interface = this;
		if ( index == -1 )
		{
			index = _control_list.get_length();
		}
		_control_list.insert_at_index( index, control );
		for ( sint32_c i = index; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_index = index;
		}
		assert( _local_box.maximum.a > _local_box.minimum.a && _local_box.maximum.b > _local_box.minimum.b );
		control->update_transform_and_layout();
		control->_on_added_to_user_interface();
	}

	void_c user_interface_c::remove_control( menu_control_c * control )
	{
		assert( control != nullptr );
		assert( control->_user_interface == this );
		assert( control->_mother_control == nullptr );
		assert( control->_index >= 0 && control->_index < _control_list.get_length() );
		assert( control == _control_list[ control->_index ] );
		_suspend_control( control );
		_control_list.remove_at_index( control->_index );
		for ( sint32_c i = control->_index; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_index = i;
		}
		control->_user_interface = nullptr;
		control->_index = -1;
		control->_on_removed_from_user_interface();
	}

	void_c user_interface_c::bring_control_to_front( menu_control_c * control )
	{
		assert( control != nullptr );
		assert( control->_user_interface == this );
		_control_list.remove_at_index( control->_index );
		_control_list.insert_at_end( control );
		for ( sint32_c i = control->_index; i < _control_list.get_length(); i++ )
		{
			_control_list[ i ]->_index = i;
		}
	}

	core_list_c< menu_control_c * > const & user_interface_c::get_control_list() const
	{
		return _control_list;
	}

	//boolean_c user_interface_c::is_mouse_inside()
	//{
	//	return _is_mouse_inside || _mouse_focused != nullptr;
	//}

	boolean_c user_interface_c::has_text_focus()
	{
		return _text_focused != nullptr;
	}

	menu_control_c * user_interface_c::get_mouse_overed() const
	{
		return _mouse_overed;
	}

	menu_control_c * user_interface_c::get_mouse_focused() const
	{
		return _mouse_focused;
	}

	menu_control_c * user_interface_c::get_text_focused() const
	{
		return _text_focused;
	}

	void_c user_interface_c::set_text_focused( menu_control_c * menu_control )
	{
		assert( _is_set_text_focus == false );
		_is_set_text_focus = true;

		if ( menu_control )
		{
			assert( menu_control->get_user_interface() == this );
		}

		// clear text focus.
		menu_control_c * previous = _text_focused;
		if ( _text_focused )
		{
			assert( _text_focused->_is_text_focused );
			_text_focused->_is_text_focused = false;
			_text_focused->_on_text_focus_lost(); // this has a possibility to nest and cascade.
			_text_focused = nullptr;
		}

		// increment _is_deep_text_focused first, so that we don't send _on_deep_text_focus_lost() to controls that won't be affected.
		menu_control_c * deep_text_focused = menu_control;
		while ( deep_text_focused )
		{
			assert( deep_text_focused->_is_deep_text_focused == 0 || deep_text_focused->_is_deep_text_focused == 1 );
			deep_text_focused->_is_deep_text_focused++;
			deep_text_focused = deep_text_focused->get_mother_control();
		}
		// then decrement, and send out _on_deep_text_focus_lost() to affected controls.
		deep_text_focused = previous;
		while ( deep_text_focused )
		{
			assert( deep_text_focused->_is_deep_text_focused == 1 || deep_text_focused->_is_deep_text_focused == 2 );
			deep_text_focused->_is_deep_text_focused--;
			if ( deep_text_focused->_is_deep_text_focused == 0 )
			{
				deep_text_focused->_on_deep_text_focus_lost();
			}
			deep_text_focused = deep_text_focused->get_mother_control();
		}

		// set text focus.
		_text_focused = menu_control;
		if ( _text_focused )
		{
			assert( _text_focused->_is_text_focused == 0 );
			_text_focused->_is_text_focused++;
			_text_focused->_on_text_focus_gained();
		}

		deep_text_focused = menu_control;
		while ( deep_text_focused )
		{
			assert( deep_text_focused->_is_deep_text_focused == 0 || deep_text_focused->_is_deep_text_focused == 1 );
			if ( deep_text_focused->_is_deep_text_focused == 1 )
			{
				deep_text_focused->_on_deep_text_focus_gained();
			}
			deep_text_focused = deep_text_focused->get_mother_control();
		}

		_is_set_text_focus = false;
	}

	void_c user_interface_c::_suspend_control( menu_control_c * control )
	{
		if ( _text_focused && _text_focused->_is_text_focused ) // check _text_focused->_is_text_focused because it's possible that the control is already in the process of losing focus, and we are here via a response to an event (_on_text_focus_lost(), _on_general_focus_lost()).
		{
			if ( _text_focused == control || _text_focused->is_descendant_of( control ) )
			{
				_text_focused->_is_text_focused = false;
				_text_focused->_on_text_focus_lost();
				menu_control_c * deep_text_focused = _text_focused;
				while ( deep_text_focused )
				{
					assert ( deep_text_focused->_is_deep_text_focused == 1 );
					deep_text_focused->_is_deep_text_focused--;
					deep_text_focused->_on_deep_text_focus_lost();
					deep_text_focused = deep_text_focused->get_mother_control();
				}
				_text_focused = nullptr;
			}
		}
		if ( _mouse_focused )
		{
			if ( _mouse_focused == control || _mouse_focused->is_descendant_of( control ) )
			{
				_mouse_focused->_is_pressed = false;
				_mouse_focused->_is_mouse_focused = false;
				_mouse_focused->_on_mouse_focus_lost();
				_mouse_focused = nullptr;
			}
		}
		if ( _mouse_overed )
		{
			if ( _mouse_overed == control || _mouse_overed->is_descendant_of( control ) )
			{
				_mouse_overed->_is_mouse_overed = false;
				_mouse_overed = false;
			}
		}
	}

	menu_non_client_type_e user_interface_c::perform_non_client_hit_test( vector32x2_c const & point )
	{
		menu_layer_e layer = menu_layer_e_base;
		for ( sint32_c i = _control_list.get_length() - 1; i >= 0; i-- )
		{
			menu_control_c * control = _control_list[ i ]->pick_control_with_global_point( point, layer );
			if ( control != nullptr )
			{
				return control->get_non_client_type();
			}
		}
		return menu_non_client_type_e_none;
	}

	void_c user_interface_c::let_go_of_mouse_overed()
	{
		if ( _mouse_focused )
		{
			_mouse_focused->_is_pressed = false;
			_mouse_focused->_is_mouse_focused = false;
			_mouse_focused->_on_mouse_focus_lost();
			_mouse_focused = nullptr;
		}
		if ( _mouse_overed )
		{
			_mouse_overed->_is_mouse_overed = false;
			_mouse_overed = nullptr;
		}
	}

	void_c make_popup_rectangle( box32x2_c const & around, vector32x2_c const & size, menu_anchor_e towards_x, menu_anchor_e towards_y, box32x2_c & result )
	{
		if ( towards_x == menu_anchor_e_left )
		{
			result.maximum.a = around.minimum.a;
			result.minimum.a = result.maximum.a - size.a;
		}
		else if ( towards_x == menu_anchor_e_right )
		{
			result.minimum.a = around.maximum.a;
			result.maximum.a = result.minimum.a + size.a;
		}
		else
		{
			// no towards_x preference defined, result will be centered horizontally with around (used by combo box drop downs).
			result.minimum.a = ( ( around.minimum.a + around.maximum.a ) * 0.5f ) - ( size.a * 0.5f );
			result.maximum.a = ( ( around.minimum.a + around.maximum.a ) * 0.5f ) + ( size.a * 0.5f );
		}

		if ( towards_y == menu_anchor_e_top )
		{
			result.maximum.b = around.minimum.b;
			result.minimum.b = result.maximum.b - size.b;
		}
		else if ( towards_y == menu_anchor_e_bottom )
		{
			result.minimum.b = around.maximum.b;
			result.maximum.b = result.minimum.b + size.b;
		}
		else
		{
			// no towards_y preference defined, result height will be same as around height (not used by anything in practice at the moment).
			result.minimum.b = ( ( around.minimum.b + around.maximum.b ) * 0.5f ) - ( size.b * 0.5f );
			result.maximum.b = ( ( around.minimum.b + around.maximum.b ) * 0.5f ) + ( size.b * 0.5f );
		}
	}

	/*
	box32x2_c user_interface_c::find_popup_box( menu_popup_type_e popup_type, vector32x2_c const & global_origin, matrix32x2x2_c const & global_basis, box32x2_c const & local_around, vector32x2_c const & local_size )
	{
		// build parameters.
		polygon32x2_c local_polygon; // polygon version of _local_box.
		ops::make_polygon32x2_from_rectangle32( _local_box, local_polygon );
		menu_anchor_e towards_x = popup_type == menu_popup_type_e_menu_list ? menu_anchor_e_right : menu_anchor_e_none; // context menu popup type favors opening towards right.
		menu_anchor_e towards_y = menu_anchor_e_bottom; // all popup types favor opening towards down.
		box32x2_c popup_rectangle; // the result.
		polygon32x2_c popup_polygon; // the intermediate result. we use polygons because they work with angles.
		popup_polygon.origin = global_origin;
		popup_polygon.basis = global_basis;

		// we go through at most 4 iterations to try to find an acceptable layout.
		// each iteration we flip either the x and/or y towards.
		sint32_c intersection = 0; // tracks how popup_polygon intersects with local_polygon.
		for ( sint32_c i = 0; i < 4; i++ )
		{
			// flip one or both of the towards axes if on a subsequent iteration.
			if ( i >= 1 )
			{
				if ( towards_x != 0 )
				{
					towards_x = towards_x == menu_anchor_e_left ? menu_anchor_e_right : menu_anchor_e_left; // flip x.
				}
				if ( i == 2 )
				{
					if ( towards_y != 0 )
					{
						towards_y = towards_y == menu_anchor_e_top ? menu_anchor_e_bottom : menu_anchor_e_top; // flip y.
					}
				}
			}

			// build popup_rectangle, convert it to popup_polygon, and test if it intersects with local_polygon.
			make_popup_rectangle( local_around, local_size, towards_x, towards_y, popup_rectangle );
			ops::make_polygon32x2_from_rectangle32( popup_rectangle, popup_polygon );
			intersection = ops::intersect_polygon_vs_polygon( local_polygon, popup_polygon );
			if ( intersection == 2 )
			{
				break; // popup_polygon is fully contained within local_polygon, so we found a layout that works, break now.
			}

			// determine if we can skip some interations.
			if ( towards_x == 0 )
			{
				if ( towards_y == 0 )
				{
					// centered on both axes, so there's nothing to flip on subsequent iteration, break now.
					break;
				}
				i++; // can't flip x on the next iteration, so this will count from 0 to 2 (and skip 1).
			}
		}

		// if popup_polygon is still not fully contained in local_polygon, then translate it so that it fits.
		// if popup_polygon is too large to fit completely within local_polygon, then we will center it.
		if ( intersection != 2 )
		{
			// find distances that popup polygon lies out of bounds of for each edge of local rectangle.
			float32_c distances[ 4 ] = { 0, 0, 0, 0 }; // longest distances over each edge: left, bottom, right, top.
			vector32x2_c popup_polygon_points[ 4 ]; // popup polygon points in same space as local_polygon.
			for ( sint32_c i = 0; i < 4; i++ )
			{
				popup_polygon_points[ i ] = popup_polygon.get_point( i );
			}

			for ( sint32_c i = 0; i < local_polygon.points_count; i++ )
			{
				line32_c local_polygon_edge_line = local_polygon.get_edge_line( i );
							
				for ( sint32_c j = 0; j < popup_polygon.points_count; j++ )
				{
					float32_c t = ops::distance_between_point_and_line( popup_polygon_points[ j ], local_polygon_edge_line );
					if ( t > distances[ i ] )
					{
						distances[ i ] = t;
					}
				}
			}

			// construct translation quantity that will nudge popup rectangle back into bounds of local rectangle.
			vector32x2_c translate;
			translate.a = distances[ 0 ] - distances[ 2 ]; // distance over left minus distance over right.
			if ( distances[ 0 ] != 0.0f && distances[ 2 ] != 0.0f ) // center if out of bounds of both left and right.
			{
				translate.a *= 0.5f;
			}
			translate.b = distances[ 3 ] - distances[ 1 ]; // distance over top minus distance over bottom.
			if ( distances[ 3 ] != 0.0f && distances[ 1 ] != 0.0f ) // center if out of bounds of both top and bottom.
			{
				translate.b *= 0.0f;
			}

			// translate the popup rectangle and localize the coordinates.
			matrix32x2x2_c popup_polygon_basis_inverted = ops::make_matrix32x2x2_inverted( popup_polygon.basis );
			for ( sint32_c i = 0; i < 4; i++ )
			{
				popup_polygon_points[ i ] += translate; // nudge back in bounds.
				popup_polygon_points[ i ] -= popup_polygon.origin; // translate to local space.
				popup_polygon_points[ i ] = ops::make_vector32x2_transformed_point( popup_polygon_points[ i ], popup_polygon_basis_inverted ); // transform (orientation and scale)into local space.
			}

			// now we have the minimum and maximum bounds of the result.
			popup_rectangle.minimum = popup_polygon_points[ 0 ]; // top left.
			popup_rectangle.maximum = popup_polygon_points[ 2 ]; // bottom right.
		}

		return popup_rectangle;
	}
	*/

}
