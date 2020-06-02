#include "cheonsa_user_interface.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_frame.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_object.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

	float32_c user_interface_c::_get_multi_click_time() const
	{
#if defined( cheonsa_platform_windows )
		return static_cast< float32_c >( GetDoubleClickTime() ) * 0.001f; // GetDoubleClickTime() returns value in milliseconds, but we want seconds, so we convert it to seconds.
#else
#error
#endif
	}

	float32_c user_interface_c::_get_multi_click_space() const
	{
		return 3.0f;
	}

	boolean_c user_interface_c::_pick_control( input_event_c * input_event, menu_control_c * & result )
	{
		// track result.
		menu_control_c * picked_control = nullptr;
		float64_c picked_control_distance = 0.0f;
		menu_layer_e picked_control_layer = menu_layer_e_undefined;

		// fist pass, try to pick 2d.
		{
			core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_first();
			while ( control_list_node )
			{
				menu_control_c * control = control_list_node->get_value();
				if ( control->_scene_component == nullptr )
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
				control_list_node = control_list_node->get_next();
			}
		}

		// second pass, try to pick 3d.
		if ( picked_control == nullptr ) // it's only valid to try to pick 3d if 2d didn't hit anything.
		{
			core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_first();
			while ( control_list_node )
			{
				menu_control_c * control = control_list_node->get_value();
				if ( control->_scene_component )
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
				control_list_node = control_list_node->get_next();
			}
		}

		if ( _mouse_overed != picked_control )
		{
			if ( _mouse_overed )
			{
				_mouse_overed->_is_mouse_overed = false;
				_mouse_overed->_on_is_mouse_overed_changed();
			}
			_mouse_overed = picked_control;
			if ( _mouse_overed )
			{
				_mouse_overed->_is_mouse_overed = true;
				_mouse_overed->_on_is_mouse_overed_changed();
			}
		}

		result = picked_control;

		return picked_control;
	}

	boolean_c user_interface_c::_pick_control_level_2( input_event_c * input_event, menu_control_c * window, menu_layer_e minimum_layer, menu_control_c * & result, float64_c & result_distance )
	{
		if ( window->_is_showed == true )
		{
			float64_c control_distance = 0.0; // for 3d menus, this is the distance from the camera to the point of intersection with the menu. for 2d menus this is unused and will always be 0.
			if ( window->_scene_component == nullptr )
			{
				control_distance = 0.0f;
				//input_event->menu_global_mouse_position = input_event->mouse_position; // - _canvas_size * 0.5f; // local 2D menu space mouse position is just the mouse position from the input manager.
			}
			else
			{
				assert( _scene );
				transform3d_c world_space_window_transform = window->get_scene_component()->get_scene_object()->get_world_space_transform();
				world_space_window_transform.scale *= 0.001f; // convert size of pixels from meters to millimeters.
				matrix32x3x3_c world_space_window_basis = world_space_window_transform.get_scaled_basis();
				transform3d_c world_space_window_transform_inverted = world_space_window_transform.get_inverted();
				plane64_c world_space_window_plane = ops::make_plane64_from_normal_and_point( vector64x3_c( world_space_window_basis.c / world_space_window_transform.scale ), world_space_window_transform.position );
				ray64_c world_space_mouse_ray = _scene->get_camera().build_ray_through_canvas( input_event->get_mouse_position(), static_cast< sint32_c >( _local_box.get_width() ), static_cast< sint32_c >( _local_box.get_height() ) ); // ASSUMING that _local_box is the same size as the canvas.
				if ( ops::sweep_ray_vs_plane( world_space_mouse_ray, world_space_window_plane, control_distance ) ) // find if there is an intersection and if so find out the distance to intersection
				{
					vector64x3_c world_space_intersection_point = world_space_mouse_ray.position + ( world_space_mouse_ray.normal * control_distance );
					transform3d_c world_space_window_transform_inverted = window->get_scene_component()->get_scene_object()->get_world_space_transform().get_inverted();
					vector64x3_c menu_space_intersection_point = ops::rotate_scale_and_translate_vector64x3( world_space_intersection_point, world_space_window_transform_inverted );
					input_event->_menu_mouse_position.a = static_cast< float32_c >( menu_space_intersection_point.a );
					input_event->_menu_mouse_position.b = static_cast< float32_c >( menu_space_intersection_point.b );
				}
			}
			menu_control_c * candidate = window->pick_control_with_global_point( input_event->get_menu_mouse_position(), minimum_layer );
			if ( candidate && ( result == nullptr || control_distance < result_distance ) )
			{
				_is_mouse_overed = true;
				result = candidate;
				result_distance = control_distance;
				return true;
			}
		}
		return false;
	}

	void_c user_interface_c::_bubble_input_event( menu_control_c * control, input_event_c * input_event )
	{
		assert( input_event->get_was_handled() == false );
		while ( control )
		{
			control->_on_input( input_event );
			control = control->_mother_control;
			if ( input_event->get_was_handled() )
			{
				return;
			}
		}
		on_input.invoke( input_event );
	}

	void_c user_interface_c::_process_input_event( input_event_c * input_event )
	{
		// reset things.
		_is_mouse_overed = false;

		// process input actions.
		engine.get_input_manager()->_invoke_action_with_input_event( input_event );

		// default to 2d, until we have to pick 3d.
		input_event->_menu_mouse_position = input_event->get_mouse_position();

		// route mouse move event to appropriate control.
		// do this before picking controls under the mouse, because this lets controls update in response to drag and drop operations.
		// which can change the mouse pick result.
		if ( input_event->get_type() == input_event_c::type_e_mouse_move )
		{
			if ( _mouse_focused )
			{
				_bubble_input_event( _mouse_focused, input_event );
			}
		}

		// update mouse overed and mouse focused.
		menu_control_c * original_mouse_focused = _mouse_focused;
		_pick_control( input_event, _mouse_overed );
		boolean_c is_any_mouse_key_down = input_event->get_mouse_key_state( input_mouse_key_e_left ) || input_event->get_mouse_key_state( input_mouse_key_e_right ) || input_event->get_mouse_key_state( input_mouse_key_e_middle );
		if ( !is_any_mouse_key_down )
		{
			_pick_control( input_event, _mouse_overed );
			if ( _mouse_focused != _mouse_overed )
			{
				if ( _mouse_focused )
				{
					if ( _mouse_focused->_is_pressed )
					{
						_mouse_focused->_is_pressed = false;
						_mouse_focused->_on_is_pressed_changed();
					}
					_mouse_focused->_is_mouse_focused = false;
					_mouse_focused->_on_is_mouse_focused_changed();
				}
				_mouse_focused = _mouse_overed;
				if ( _mouse_focused )
				{
					_mouse_focused->_is_mouse_focused = true;
					_mouse_focused->_on_is_mouse_focused_changed();
				}
			}
		}

		// route other events to appropriate control.
		if ( input_event->get_type() == input_event_c::type_e_mouse_key_pressed )
		{
			boolean_c do_multi_click = false;
			if ( input_event->get_mouse_key() == input_mouse_key_e_left || input_event->get_mouse_key() == input_mouse_key_e_right || input_event->get_mouse_key() == input_mouse_key_e_middle )
			{
				if ( _text_focused != _mouse_focused )
				{
					set_text_focused( _mouse_focused );
				}
			}
			if ( _mouse_focused )
			{
				if ( input_event->get_mouse_key() == input_mouse_key_e_left )
				{
					_mouse_focused->_is_pressed = true;
					_mouse_focused->_on_is_pressed_changed();
					if ( _multi_click_control != _mouse_focused ||
						ops::length_squared_float32( input_event->get_mouse_position() - _multi_click_position ) > _get_multi_click_space() ||
						( static_cast< float64_c >( input_event->get_time() - _multi_click_time ) / static_cast< float64_c >( ops::time_get_high_resolution_timer_frequency() ) ) > _get_multi_click_time() )
					{
						_multi_click_count = 1;
					}
					else
					{
						_multi_click_count++;
						if ( _multi_click_count > 3 )
						{
							_multi_click_count = 1;
						}
					}
					_multi_click_control = _mouse_focused;
					_multi_click_time = input_event->get_time();
					_multi_click_position = input_event->get_mouse_position();
					if ( _multi_click_count != 1 )
					{
						input_event->_menu_multi_click_count = _multi_click_count;
						_mouse_focused->_on_multi_clicked( input_event );
					}
					else
					{
						_bubble_input_event( _mouse_focused, input_event );
					}
				}
			}
		}
		else if ( input_event->get_type() == input_event_c::type_e_mouse_key_released )
		{
			if ( _mouse_focused )
			{
				if ( input_event->get_mouse_key() == input_mouse_key_e_left )
				{
					_mouse_focused->_is_pressed = false;
					_mouse_focused->_on_is_pressed_changed();
				}
				_bubble_input_event( _mouse_focused, input_event );
			}
			if ( input_event->get_mouse_key() == input_mouse_key_e_left || input_event->get_mouse_key() == input_mouse_key_e_right || input_event->get_mouse_key() == input_mouse_key_e_middle )
			{
				if ( original_mouse_focused && original_mouse_focused == _mouse_overed && input_event->get_mouse_key() == input_mouse_key_e_left )
				{
					if ( _multi_click_count == 1 )
					{
						original_mouse_focused->_on_clicked( input_event );
					}
				}
			}
		}
		else if ( input_event->get_type() == input_event_c::type_e_mouse_wheel )
		{
			_bubble_input_event( _mouse_overed, input_event );
		}
		else //if ( input_event->type == input_event_c::type_e_character )
		{
			if ( _text_focused )
			{
				_bubble_input_event( _text_focused, input_event );
			}
		}

		// do mouse pick, potentially for the second time.
		//mouse_hit_was_blocked |= _pick_control_level_1( input_event, modal_window );
		//input_event->was_handled |= _mouse_focused || _mouse_overed;
	}

	user_interface_c::user_interface_c()
		: _local_box()
		, _canvas_and_output( nullptr )
		, _scene( nullptr )
		, _daughter_control_list()
		, _mouse_overed( nullptr )
		, _mouse_focused( nullptr )
		, _text_focused( nullptr )
		, _is_mouse_overed( false )
		, _is_set_text_focused_changing( false )
		, _multi_click_control( nullptr )
		, _multi_click_time( 0 )
		, _multi_click_position()
	{
	}

	user_interface_c::~user_interface_c()
	{
		core_list_c< menu_control_c * > daughter_controls_to_remove; // take stock of the non-supplemental controls to remove.
		core_linked_list_c< menu_control_c * >::node_c const * daughter_control_list_node = _daughter_control_list.get_first();
		while ( daughter_control_list_node )
		{
			if ( !daughter_control_list_node->get_value()->_is_supplemental )
			{
				daughter_controls_to_remove.insert( -1, daughter_control_list_node->get_value() );
			}
			daughter_control_list_node = daughter_control_list_node->get_next();
		}
		for ( sint32_c i = 0; i < daughter_controls_to_remove.get_length(); i++ ) // remove the non-supplemental controls.
		{
			remove_daughter_control( daughter_controls_to_remove[ i ] );
		}
		assert( _daughter_control_list.get_length() == 0 ); // all controls (non-supplemental and supplemental) should be removed now.

		delete _canvas_and_output;
		_canvas_and_output = nullptr;
		_scene = nullptr;
		_mouse_overed = nullptr;
		_mouse_focused = nullptr;
		_text_focused = nullptr;
		_is_mouse_overed = false;
		_is_set_text_focused_changing = false;
	}

	boolean_c user_interface_c::start( void_c * window_handle )
	{
		assert( window_handle );

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
		if ( engine.get_window_manager()->get_window_state() == window_state_e_maximized )
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

			core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_first();
			while ( control_list_node )
			{
				menu_control_c * control = control_list_node->get_value();
				if ( control->_scene_component == nullptr )
				{
					control->_update_transform_and_layout();
				}
				control_list_node = control_list_node->get_next();
			}
		}
	}

	void_c user_interface_c::update( float32_c time_step )
	{
		// update canvas.
		update_canvas();

		// process input events.
		boolean_c wants_to_refresh_resources = false;
		core_list_c< input_event_c > & input_events = engine.get_input_manager()->get_events();
		for ( sint32_c i = 0; i < input_events.get_length(); i++ )
		{
			input_event_c & input_event = input_events[ i ];

			// check for engine hot keys.
			if ( input_event.get_type() == input_event_c::type_e_keyboard_key_pressed )
			{
				if ( input_event.get_keyboard_key() == debug_key_toggle_console )
				{
					engine.get_debug_manager()->set_console_is_showing( !engine.get_debug_manager()->get_console_is_showing() );
				}
				else if ( input_event.get_keyboard_key() == debug_key_toggle_stats )
				{
					engine.get_debug_manager()->set_statistics_is_showing( !engine.get_debug_manager()->get_statistics_is_showing() );
				}
				else if ( input_event.get_keyboard_key() == debug_key_toggle_menu_bounds )
				{
					engine.get_debug_manager()->set_draw_menu_bounds( !engine.get_debug_manager()->get_draw_menu_bounds() );
				}
				else if ( input_event.get_keyboard_key() == debug_key_refresh_resources )
				{
					engine.get_content_manager()->queue_refresh();
				}
			}

			// let the menus process the input event.
			_process_input_event( &input_event );

			// let the game process the input event.
			engine.get_game()->process_input_event( &input_event );
		}

		// update animations of controls, and remove any that want to be removed.
		sint32_c index = 0;
		core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_first();
		while ( control_list_node )
		{
			core_linked_list_c< menu_control_c * >::node_c const * next = control_list_node->get_next();
			menu_control_c * control = control_list_node->get_value();
			control->update_animations( time_step );
			if ( control->get_wants_to_be_deleted() && control->get_is_showed_weight() <= 0.0f )
			{
				remove_daughter_control( control );
			}
			else
			{
				control->_index = index;
				index++;
			}
			control_list_node = next;
		}
	}

	void_c user_interface_c::render_and_present( float32_c time_step )
	{
		// pre-render menus.
		// this has to be done before rendering the 3d scene, in case some of the menus are 3d.
		engine.get_video_renderer_interface()->pre_render_menus( this );

		// clear the canvas.
		vector32x4_c clear_color = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
		if ( _scene )
		{
			clear_color = _scene->get_clear_color();
		}
		_canvas_and_output->clear( clear_color );

		// render 3d scene and 3d menu controls to canvas.
		if ( _scene )
		{
			engine.get_video_renderer_interface()->render_scene( _scene, &_scene->get_camera(), _canvas_and_output );
		}

		// render 2d menus to canvas.
		engine.get_video_renderer_interface()->render_2d_menus( this );

		// flip canvas back buffer to output window.
		_canvas_and_output->present();
	}

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
		if ( _scene != scene )
		{
			if ( _scene )
			{
				_scene->_set_user_interface( nullptr );
				engine.get_audio_interface()->remove_scene( _scene->get_audio_scene() );
			}

			_scene = scene;

			if ( _scene )
			{
				engine.get_audio_interface()->add_scene( _scene->get_audio_scene() );
				_scene->_set_user_interface( this );
			}
		}
	}

	void_c user_interface_c::add_daughter_control( menu_control_c * control, sint32_c index )
	{
		assert( _local_box.maximum.a > _local_box.minimum.a && _local_box.maximum.b > _local_box.minimum.b );
		assert( control );
		assert( control->_mother_user_interface == nullptr );
		assert( control->_mother_control == nullptr );
		assert( control->_index == -1 );
		assert( index >= -1 && index <= _daughter_control_list.get_length() );
		assert( control->_name.get_length() > 0 );

		// update relationships, insert in list.
		control->add_reference();
		control->_mother_control = nullptr;
		control->_index = index < 0 ? _daughter_control_list.get_length() : index;
		_daughter_control_list.insert_at_index( &control->_daughter_control_list_node, control->_index );

		// reindex controls after insertion point.
		sint32_c reindex_index = control->_index + 1;
		core_linked_list_c< menu_control_c * >::node_c const * reindex_node = control->_daughter_control_list_node.get_next();
		while ( reindex_node )
		{
			reindex_node->get_value()->_index = reindex_index;
			reindex_index++;
			reindex_node = reindex_node->get_next();
		}

		// update layout.
		control->_handle_added_to_user_interface( this );
		control->_update_transform_and_layout();
	}

	void_c user_interface_c::remove_daughter_control( menu_control_c * control )
	{
		assert( control );
		assert( control->_mother_user_interface == this );
		assert( control->_mother_control == nullptr );
		assert( control->_index >= 0 && control->_index < _daughter_control_list.get_length() );

		// suspend control from user interface.
		_suspend_control( control );
		control->_handle_removed_from_user_interface();

		// reindex controls after removal point.
		sint32_c reindex_index = control->_index;
		core_linked_list_c< menu_control_c * >::node_c const * reindex_node = control->_daughter_control_list_node.get_next();
		while ( reindex_node )
		{
			reindex_node->get_value()->_index = reindex_index;
			reindex_index++;
			reindex_node = reindex_node->get_next();
		}

		// update relationships and remove control from list.
		control->_mother_control = nullptr;
		control->_index = -1;
		_daughter_control_list.remove( &control->_daughter_control_list_node );
		control->remove_reference();
	}

	void_c user_interface_c::bring_daughter_control_to_front( menu_control_c * control )
	{
		assert( control );
		assert( control->_mother_user_interface == this );
		sint32_c reindex_index = control->_index;
		_daughter_control_list.remove( &control->_daughter_control_list_node );
		_daughter_control_list.insert_at_end( &control->_daughter_control_list_node );
		core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_at_index( reindex_index );
		while ( control_list_node )
		{
			control_list_node->get_value()->_index = reindex_index;
			reindex_index++;
			control_list_node = control_list_node->get_next();
		}
	}

	void_c user_interface_c::reset_multi_click_detection()
	{
		_multi_click_control = nullptr;
		_multi_click_time = 0;
		_multi_click_position.a = 0.0f;
		_multi_click_position.b = 0.0f;
		_multi_click_count = 0;
	}

	boolean_c user_interface_c::has_text_focus()
	{
		return _text_focused;
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
		assert( _is_set_text_focused_changing == false );
		_is_set_text_focused_changing = true;

		if ( menu_control )
		{
			assert( menu_control->_mother_user_interface == this );
		}

		// clear text focus.
		menu_control_c * previous = _text_focused;
		if ( _text_focused )
		{
			assert( _text_focused->_is_text_focused );
			_text_focused->_is_text_focused = false;
			_text_focused->_on_is_text_focused_changed(); // this has a possibility to nest and cascade.
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
				deep_text_focused->_on_is_deep_text_focused_changed();
			}
			deep_text_focused = deep_text_focused->get_mother_control();
		}

		// set text focus.
		_text_focused = menu_control;
		if ( _text_focused )
		{
			assert( _text_focused->_is_text_focused == 0 );
			_text_focused->_is_text_focused++;
			_text_focused->_on_is_text_focused_changed();
		}

		deep_text_focused = menu_control;
		while ( deep_text_focused )
		{
			assert( deep_text_focused->_is_deep_text_focused == 0 || deep_text_focused->_is_deep_text_focused == 1 );
			if ( deep_text_focused->_is_deep_text_focused == 1 )
			{
				deep_text_focused->_on_is_deep_text_focused_changed();
			}
			deep_text_focused = deep_text_focused->get_mother_control();
		}

		_is_set_text_focused_changing = false;
	}

	void_c user_interface_c::_suspend_control( menu_control_c * control )
	{
		if ( _text_focused && _text_focused->_is_text_focused ) // check _text_focused->_is_text_focused because it's possible that the control is already in the process of losing focus, and we are here via a cascading response to an event (_on_text_focus_lost(), _on_general_focus_lost()).
		{
			if ( control->is_ascendant_of( _text_focused ) )
			{
				_text_focused->_is_text_focused = false;
				_text_focused->_on_is_text_focused_changed();
				menu_control_c * deep_text_focused = _text_focused;
				while ( deep_text_focused )
				{
					assert ( deep_text_focused->_is_deep_text_focused == 1 );
					deep_text_focused->_is_deep_text_focused--;
					deep_text_focused->_on_is_deep_text_focused_changed();
					deep_text_focused = deep_text_focused->get_mother_control();
				}
				_text_focused = nullptr;
			}
		}
		if ( _mouse_focused )
		{
			if ( control->is_ascendant_of( _mouse_focused ) )
			{
				_mouse_focused->_is_pressed = false;
				_mouse_focused->_on_is_pressed_changed();
				_mouse_focused->_is_mouse_focused = false;
				_mouse_focused->_on_is_mouse_focused_changed();
				_mouse_focused = nullptr;
			}
		}
		if ( _mouse_overed )
		{
			if ( control->is_ascendant_of( _mouse_overed ) )
			{
				_mouse_overed->_is_mouse_overed = false;
				_mouse_overed->_on_is_mouse_overed_changed();
				_mouse_overed = false;
			}
		}
	}

	menu_non_client_type_e user_interface_c::perform_non_client_hit_test( vector32x2_c const & point )
	{
		menu_layer_e layer = menu_layer_e_base;
		core_linked_list_c< menu_control_c * >::node_c const * control_list_node = _daughter_control_list.get_last();
		while ( control_list_node )
		{
			menu_control_c * control = control_list_node->get_value()->pick_control_with_global_point( point, layer );
			if ( control )
			{
				return control->get_non_client_type();
			}
			control_list_node = control_list_node->get_previous();
		}
		return menu_non_client_type_e_none;
	}

	void_c user_interface_c::let_go_of_mouse_overed()
	{
		if ( _mouse_focused )
		{
			_mouse_focused->_is_pressed = false;
			_mouse_focused->_on_is_pressed_changed();
			_mouse_focused->_is_mouse_focused = false;
			_mouse_focused->_on_is_mouse_focused_changed();
			_mouse_focused = nullptr;
		}
		if ( _mouse_overed )
		{
			_mouse_overed->_is_mouse_overed = false;
			_mouse_overed->_on_is_mouse_overed_changed();
			_mouse_overed = nullptr;
		}
	}

	box32x2_c user_interface_c::_make_pop_up_box_for_iteration( menu_pop_up_type_e pop_up_type, sint32_c iteration, box32x2_c const & around, vector32x2_c const & pop_up_size )
	{
		box32x2_c result;
		if ( pop_up_type == menu_pop_up_type_e_right_bottom )
		{
			if ( iteration == 0 )
			{
				// towards right bottom.
				result.minimum.a = around.maximum.a;
				result.maximum.a = result.minimum.a + pop_up_size.a;
				result.minimum.b = around.minimum.b;
				result.maximum.b = result.minimum.b + pop_up_size.b;
			}
			else if ( iteration == 1 )
			{
				// towards right top.
				result.minimum.a = around.maximum.a;
				result.maximum.a = around.minimum.a + pop_up_size.a;
				result.maximum.b = around.maximum.b;
				result.minimum.b = result.maximum.b - pop_up_size.b;
			}
			else if ( iteration == 2 )
			{
				// towards left bottom.
				result.maximum.a = around.minimum.a;
				result.minimum.a = result.maximum.a - pop_up_size.a;
				result.minimum.b = around.minimum.b;
				result.maximum.b = result.minimum.b + pop_up_size.b;
			}
			else if ( iteration == 3 )
			{
				// towards left top.
				result.maximum.a = around.minimum.a;
				result.minimum.a = result.maximum.a - pop_up_size.a;
				result.maximum.b = around.maximum.b;
				result.minimum.b = result.maximum.b - pop_up_size.b;
			}
		}
		else if ( pop_up_type == menu_pop_up_type_e_bottom_right )
		{
			if ( iteration == 0 )
			{
				// towards bottom right.
				result.minimum.a = around.minimum.a;
				result.maximum.a = result.minimum.a + pop_up_size.a;
				result.minimum.b = around.maximum.b;
				result.maximum.b = result.minimum.b + pop_up_size.b;
			}
			else if ( iteration == 1 )
			{
				// towards top right.
				result.minimum.a = around.minimum.a;
				result.maximum.a = result.minimum.a + pop_up_size.a;
				result.maximum.b = around.minimum.b;
				result.minimum.b = result.maximum.b - pop_up_size.b;
			}
			else if ( iteration == 2 )
			{
				// towards bottom left.
				result.maximum.a = around.maximum.a;
				result.minimum.a = result.maximum.a - pop_up_size.a;
				result.minimum.b = around.maximum.b;
				result.maximum.b = result.minimum.b + pop_up_size.b;
			}
			else if ( iteration == 3 )
			{
				// towards top left.
				result.maximum.a = around.maximum.a;
				result.minimum.a = result.maximum.a - pop_up_size.a;
				result.maximum.b = around.minimum.b;
				result.minimum.b = result.maximum.b - pop_up_size.b;
			}
		}
		return result;
	}

	box32x2_c user_interface_c::_find_pop_up_box( box32x2_c const & around_box, matrix32x2x2_c const & around_global_basis, vector32x2_c const & around_global_origin, menu_pop_up_type_e pop_up_type, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space )
	{
		box32x2_c result;
		polygon32x2_c screen_box_polygon;
		box32x2_c pop_up_box;
		polygon32x2_c pop_up_box_polygon;
		sint32_c intersection;

		// find a pop up box that fits in the screen box.
		screen_box_polygon = ops::make_polygon32x2_from_box32x2( _local_box );
		for ( sint32_c i = 0; i < 4; i++ )
		{
			pop_up_box = _make_pop_up_box_for_iteration( pop_up_type, i, around_box, pop_up_size ); // create local space pop up box.
			pop_up_box_polygon = ops::make_polygon32x2_from_box32x2( pop_up_box ); // create polygon of box.
			pop_up_box_polygon = ops::make_polygon32x2_transformed( pop_up_box_polygon, around_global_basis, around_global_origin ); // transform from local space to global space.
			intersection = ops::intersect_polygon_vs_polygon( screen_box_polygon, pop_up_box_polygon ); // check for intersection against screen polygon.
			if ( intersection == 2 )
			{
				// pop up is fully contained in screen, so we have a result that will work.
				goto finish_up;
			}
		}

		// we have a preliminary result, but it's not completely inside the screen.
		// we will try to get it to fit.
		// find the largest amount of overlap between each edge of the screen and all of the points in the pop up polygon, so that we can nudge the pop up polygon to fit within the bounds of the screen.
		// a generic implementation would measuring distances of pop up polygon points against screen polygon edges, but since the screen rectangle is always going to be a rectangle and axis aligned i will keep it simple.
		float32_c distances[ 4 ]; // distances over left, top, right, and bottom edges of screen. values will be positive if there is overlap (pop up goes outside of that side of screen), negative if there is underlap (pop up is within that side of screen).
		distances[ 0 ] = _local_box.minimum.a - pop_up_box_polygon.points[ 0 ].a;
		distances[ 1 ] = _local_box.minimum.b - pop_up_box_polygon.points[ 0 ].b;
		distances[ 2 ] = pop_up_box_polygon.points[ 0 ].a - _local_box.maximum.a;
		distances[ 3 ] = pop_up_box_polygon.points[ 0 ].b - _local_box.maximum.b;
		for ( sint32_c i = 1; i < pop_up_box_polygon.points_count; i++ )
		{
			float32_c d = _local_box.minimum.a - pop_up_box_polygon.points[ i ].a;
			if ( distances[ 0 ] < d )
			{
				distances[ 0 ] = d;
			}

			d = _local_box.minimum.b - pop_up_box_polygon.points[ i ].b;
			if ( distances[ 1 ] < d )
			{
				distances[ 1 ] = d;
			}

			d = pop_up_box_polygon.points[ i ].a - _local_box.maximum.a;
			if ( distances[ 2 ] < d )
			{
				distances[ 2 ] = d;
			}

			d = pop_up_box_polygon.points[ i ].b - _local_box.maximum.b;
			if ( distances[ 3 ] < d )
			{
				distances[ 3 ] = d;
			}
		}
		// nudge horizontally if needed.
		if ( distances[ 0 ] > 0.0f || distances[ 2 ] > 0.0f )
		{
			// the pop up is overlapping the left or right side(s) of the screen.
			float32_c pop_up_width_in_screen = distances[ 2 ] - distances[ 0 ];
			if ( pop_up_width_in_screen <= _local_box.get_width() )
			{
				// the pop up horizontal cross section is less than or equal to the width of the screen, so nudge the pop up over to fit in the screen.
				if ( distances[ 0 ] > 0.0f )
				{
					// nudge right.
					for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
					{
						pop_up_box_polygon.points[ i ].a += distances[ 0 ];
					}
				}
				else if ( distances[ 2 ] > 0.0f )
				{
					// nudge left.
					for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
					{
						pop_up_box_polygon.points[ i ].a -= distances[ 2 ];
					}
				}
			}
			else
			{
				// the pop up horizontal cross section is larger than the width of the screen, so center the pop up horizontally.
				float32_c nudge_amount = distances[ 0 ] - ( ( pop_up_width_in_screen - _local_box.get_width() ) * 0.5f );
				for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
				{
					pop_up_box_polygon.points[ i ].a += nudge_amount;
				}
			}
		}
		// nudge vertically if needed.
		if ( distances[ 1 ] > 0.0f || distances[ 3 ] > 0.0f )
		{
			// the pop up is overlapping the top or bottom side(s) of the screen.
			float32_c pop_up_height_in_screen = distances[ 3 ] - distances[ 1 ];
			if ( pop_up_height_in_screen <= _local_box.get_height() )
			{
				// the pop up vertical cross section is less than or equal to the height of the screen, so nudge the pop up over to fit in the screen.
				if ( distances[ 1 ] > 0.0f )
				{
					// nudge down.
					for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
					{
						pop_up_box_polygon.points[ i ].b += distances[ 1 ];
					}
				}
				else if ( distances[ 3 ] > 0.0f )
				{
					// nudge up.
					for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
					{
						pop_up_box_polygon.points[ i ].b -= distances[ 3 ];
					}
				}
			}
			else
			{
				// the pop up vertical cross section is larger than the height of the screen, so center the pop up vertically.
				float32_c nudge_amount = distances[ 1 ] - ( ( pop_up_height_in_screen - _local_box.get_height() ) * 0.5f );
				for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
				{
					pop_up_box_polygon.points[ i ].b += nudge_amount;
				}
			}
		}

	finish_up:
		// transform the pop up polygon from global space back to the local space relative to around_global_basis and around_global_origin.
		// this should produce a rectangular polygon that is axis aligned, then we can convert it to a box by simply grabbing a couple of points.
		if ( !give_result_in_global_space )
		{
			matrix32x2x2_c around_global_basis_inverted = ops::invert_matrix32x2x2( around_global_basis );
			for ( sint32_c i = 0; i < pop_up_box_polygon.points_count; i++ )
			{
				pop_up_box_polygon.points[ i ] = ops::rotate_and_scale_vector32x2( pop_up_box_polygon.points[ i ] - around_global_origin, around_global_basis_inverted );
			}
		}
		result.minimum = pop_up_box_polygon.points[ 0 ]; // top left point.
		result.maximum = pop_up_box_polygon.points[ 2 ]; // bottom right point.
		return result;
	}

	box32x2_c user_interface_c::find_context_menu_pop_up_box( vector32x2_c screen_space_point_to_spawn_pop_up_around, vector32x2_c const & pop_up_size )
	{
		box32x2_c around_box = box32x2_c( screen_space_point_to_spawn_pop_up_around, screen_space_point_to_spawn_pop_up_around );
		matrix32x2x2_c around_global_basis;
		vector32x2_c around_global_origin;
		return _find_pop_up_box( around_box, around_global_basis, around_global_origin, menu_pop_up_type_e_right_bottom, pop_up_size, true );
	}

	box32x2_c user_interface_c::find_sub_menu_pop_up_box( menu_control_c * menu_item_to_spawn_pop_up_around, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space )
	{
		box32x2_c around_box = menu_item_to_spawn_pop_up_around->get_local_box();
		matrix32x2x2_c around_global_basis = menu_item_to_spawn_pop_up_around->get_global_basis();
		vector32x2_c around_global_origin = menu_item_to_spawn_pop_up_around->get_global_origin();
		return _find_pop_up_box( around_box, around_global_basis, around_global_origin, menu_pop_up_type_e_right_bottom, pop_up_size, give_result_in_global_space );
	}

	box32x2_c user_interface_c::find_combo_list_pop_up_box( menu_control_c * combo_to_spawn_pop_up_around, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space )
	{
		box32x2_c around_box = combo_to_spawn_pop_up_around->get_local_box();
		matrix32x2x2_c around_global_basis = combo_to_spawn_pop_up_around->get_global_basis();
		vector32x2_c around_global_origin = combo_to_spawn_pop_up_around->get_global_origin();
		return _find_pop_up_box( around_box, around_global_basis, around_global_origin, menu_pop_up_type_e_bottom_right, pop_up_size, give_result_in_global_space );
	}

	menu_control_c * user_interface_c::open_modal_screen()
	{
		menu_control_frame_c * result = menu_control_frame_c::make_new_instance( string8_c( core_list_mode_e_static, "modal_screen" ) );
		result->set_style_map_key( string8_c( core_list_mode_e_static, "e_modal" ) );
		result->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( -10.0f, -10.0f, -10.0f, -10.0f ) );
		add_daughter_control( result );
		return result;
	}

}
