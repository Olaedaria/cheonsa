#include "cheonsa_input_manager.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <cassert>
#endif

namespace cheonsa
{

	boolean_c input_manager_c::_is_any_mouse_key_on()
	{
		return ( _mouse_keys_state[ input_mouse_key_e_left ] & input_key_state_bit_e_on ) || ( _mouse_keys_state[ input_mouse_key_e_right ] & input_key_state_bit_e_on ) || ( _mouse_keys_state[ input_mouse_key_e_middle ] & input_key_state_bit_e_on ) || ( _mouse_keys_state[ input_mouse_key_e_extra1 ] & input_key_state_bit_e_on ) || ( _mouse_keys_state[ input_mouse_key_e_extra2 ] & input_key_state_bit_e_on );
	}

	void_c input_manager_c::_update_keys_states( input_key_state_bit_e * states, sint32_c count )
	{
		for ( sint32_c i = 1; i < count; i++ )
		{
			if ( ( states[ i ] & input_key_state_bit_e_released ) != 0 )
			{
				states[ i ] = input_key_state_bit_e_off;
			}
			else if ( ( states[ i ] & input_key_state_bit_e_pressed ) != 0 )
			{
				states[ i ] = input_key_state_bit_e_on;
			}
		}
	}

	input_event_c * input_manager_c::_emplace_input_event()
	{
		input_event_c * input_event = _next_events->emplace_at_end();
		ops::memory_zero( input_event, sizeof( input_event_c ) );
		return input_event;
	}

	void_c input_manager_c::_finalize_input_event( input_event_c * input_event )
	{
		if ( input_event->type == input_event_c::type_e_mouse_move )
		{
			input_event->mouse_position_delta = input_event->mouse_position - _mouse_position;

			_mouse_position = input_event->mouse_position;
		}
		else if ( input_event->type == input_event_c::type_e_keyboard_key_pressed )
		{
			assert( input_event->keyboard_key >= 0 && input_event->keyboard_key < input_keyboard_key_e_count_ );
			if ( input_event->keyboard_key > 0 && input_event->keyboard_key < input_keyboard_key_e_count_ )
			{
				_keyboard_keys_state[ input_event->keyboard_key ] = input_key_state_bit_e_on | input_key_state_bit_e_pressed;
			}
		}
		else if ( input_event->type == input_event_c::type_e_keyboard_key_released )
		{
			assert( input_event->keyboard_key >= 0 && input_event->keyboard_key < input_keyboard_key_e_count_ );
			if ( input_event->keyboard_key > 0 && input_event->keyboard_key < input_keyboard_key_e_count_ )
			{
				_keyboard_keys_state[ input_event->keyboard_key ] = input_key_state_bit_e_released;
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			assert( input_event->mouse_key >= 0 && input_event->mouse_key < input_mouse_key_e_count_ );
			boolean_c previous_any_mouse_key_state = _is_any_mouse_key_on();
			if ( input_event->mouse_key > 0 && input_event->mouse_key < input_mouse_key_e_count_ )
			{
				_mouse_keys_state[ input_event->mouse_key ] = input_key_state_bit_e_on | input_key_state_bit_e_pressed;
			}

			if ( previous_any_mouse_key_state == false )
			{
				SetCapture( static_cast< HWND >( engine_c::get_instance()->get_window_manager()->get_window_handle() ) );
			}

			sint64_c pressed_time = ops::time_get_high_resolution_timer_count();
			if ( ops::make_float32_length_squared( _mouse_position - _mouse_position_when_key_pressed ) < get_double_click_space() )
			{
				if ( static_cast< float32_c >( static_cast< float64_c >( pressed_time - _mouse_key_multi_click_times[ input_event->mouse_key ] ) / static_cast< float64_c >( ops::time_get_high_resolution_timer_frequency() ) ) < get_double_click_time() && _mouse_key_multi_click_counts[ input_event->mouse_key ] < 3 )
				{
					if ( _mouse_key_multi_click_counts[ input_event->mouse_key ] < 100 )
					{
						_mouse_key_multi_click_counts[ input_event->mouse_key ]++;
					}
				}
				else
				{
					_mouse_key_multi_click_counts[ input_event->mouse_key ] = 1;
				}
			}
			else
			{
				for ( sint32_c i = 0; i < input_mouse_key_e_count_; i++ )
				{
					_mouse_key_multi_click_counts[ i ] = 1;
				}
			}
			_mouse_position_when_key_pressed = _mouse_position;
			_mouse_key_multi_click_times[ input_event->mouse_key ] = pressed_time;
			input_event->mouse_key_multi_click_count = _mouse_key_multi_click_counts[ input_event->mouse_key ];
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			assert( input_event->mouse_key >= 0 && input_event->mouse_key < input_mouse_key_e_count_ );
			if ( input_event->mouse_key > 0 && input_event->mouse_key < input_mouse_key_e_count_ )
			{
				_mouse_keys_state[ input_event->mouse_key ] = input_key_state_bit_e_released;
			}
			if ( _is_any_mouse_key_on() == false )
			{
				ReleaseCapture();
			}
		}

		if ( input_event->type == input_event_c::type_e_keyboard_key_pressed || input_event->type == input_event_c::type_e_keyboard_key_released )
		{
			_modifier_keys_state[ input_modifier_key_e_shift ] = ( ( _keyboard_keys_state[ input_keyboard_key_e_l_shift ] & input_key_state_bit_e_on ) != 0 || ( _keyboard_keys_state[ input_keyboard_key_e_r_shift ] & input_key_state_bit_e_on ) != 0 ) ? input_key_state_bit_e_on : input_key_state_bit_e_off;
			_modifier_keys_state[ input_modifier_key_e_ctrl ] = ( ( _keyboard_keys_state[ input_keyboard_key_e_l_ctrl ] & input_key_state_bit_e_on ) != 0 || ( _keyboard_keys_state[ input_keyboard_key_e_r_ctrl ] & input_key_state_bit_e_on ) != 0 ) ? input_key_state_bit_e_on : input_key_state_bit_e_off;
			_modifier_keys_state[ input_modifier_key_e_alt ] = ( ( _keyboard_keys_state[ input_keyboard_key_e_l_alt ] & input_key_state_bit_e_on ) != 0 || ( _keyboard_keys_state[ input_keyboard_key_e_r_alt ] & input_key_state_bit_e_on ) != 0 ) ? input_key_state_bit_e_on : input_key_state_bit_e_off;
		}

		input_event->mouse_position = _mouse_position;
		assert( sizeof( input_event->modifier_keys_state ) == sizeof ( _modifier_keys_state ) );
		ops::memory_copy( _modifier_keys_state, input_event->modifier_keys_state, sizeof( _modifier_keys_state ) );
		assert( sizeof( input_event->mouse_keys_state ) == sizeof( _mouse_keys_state ) );
		ops::memory_copy( _mouse_keys_state, input_event->mouse_keys_state, sizeof( _mouse_keys_state ) );
		assert( sizeof( input_event->keyboard_keys_state ) == sizeof( _keyboard_keys_state ) );
		ops::memory_copy( _keyboard_keys_state, input_event->keyboard_keys_state, sizeof( _keyboard_keys_state ) );
	}

	input_manager_c::input_manager_c()
		: _events( nullptr )
		, _next_events( nullptr )
		, _mouse_position()
		, _modifier_keys_state{}
		, _mouse_keys_state{}
		, _keyboard_keys_state{}
		, _events_a()
		, _events_b()
		, _mouse_pointer_visibility( true )
		, _mouse_position_when_key_pressed()
		, _mouse_key_multi_click_times{ 0 }
		, _mouse_key_multi_click_counts{ 1, 1, 1, 1, 1, 1 }
	{	
	}

	input_manager_c::~input_manager_c()
	{
	}

	boolean_c input_manager_c::start()
	{
		_events = &_events_a;
		_next_events = &_events_b;
		return true;
	}

	void_c input_manager_c::update()
	{
		_update_keys_states( _modifier_keys_state, input_modifier_key_e_count_ );
		_update_keys_states( _mouse_keys_state, input_mouse_key_e_count_ );
		_update_keys_states( _keyboard_keys_state, input_keyboard_key_e_count_ );
		core_list_c< input_event_c > * temp = _events;
		_events = _next_events;
		_next_events = temp;
		_next_events->remove_all();
	}

	float32_c input_manager_c::get_double_click_time()
	{
#if defined( cheonsa_platform_windows )
		return static_cast< float32_c >( GetDoubleClickTime() ) * 0.001f;
#endif
	}

	float32_c input_manager_c::get_double_click_space()
	{
		return 3.0f;
	}

	boolean_c input_manager_c::get_mouse_pointer_visibility()
	{
		return _mouse_pointer_visibility;
	}

	void_c input_manager_c::set_mouse_pointer_visibility( boolean_c value )
	{
#if defined( cheonsa_platform_windows )
		if ( _mouse_pointer_visibility != value )
		{
			_mouse_pointer_visibility = value;
			ShowCursor( value ? TRUE : FALSE );
		}
#endif
	}

	vector32x2_c input_manager_c::get_mouse_pointer_position()
	{
#if defined( cheonsa_platform_windows )
		//POINT cursor_position;
		//GetCursorPos( &cursor_position );
		//RECT window_client_rectangle;
		//GetClientRect( static_cast< HWND >( global_engine_instance.environment.get_window_handle() ), &window_client_rectangle );
		//return vector32x2_c( static_cast< float32_c >( cursor_position.x - window_client_rectangle.left ), static_cast< float32_c >( cursor_position.y - window_client_rectangle.top ) );
#endif
		return _mouse_position;
	}

	void_c input_manager_c::set_mouse_pointer_position( vector32x2_c value )
	{
#if defined( cheonsa_platform_windows )
		_mouse_position = value;
		RECT window_client_rectangle;
		GetWindowRect( static_cast< HWND >( engine_c::get_instance()->get_window_manager()->get_window_handle() ), &window_client_rectangle );
		SetCursorPos( static_cast< int >( value.a + window_client_rectangle.left ), static_cast< int >( value.b + window_client_rectangle.top ) );
#endif
	}

	void_c input_manager_c::_release_all_keys()
	{
		for ( sint32_c i = 0; i < input_mouse_key_e_count_; i++ )
		{
			if ( ( _mouse_keys_state[ i ] & input_key_state_bit_e_on ) != 0 )
			{
				push_mouse_key_released( static_cast< input_mouse_key_e >( i ) );
			}
		}
		for ( sint32_c i = 0; i < input_keyboard_key_e_count_; i++ )
		{
			if ( ( _keyboard_keys_state[ i ] & input_key_state_bit_e_on ) != 0 )
			{
				push_keyboard_key_released( static_cast< input_keyboard_key_e >( i ) );
			}
		}
	}

	void_c input_manager_c::push_keyboard_key_pressed( input_keyboard_key_e keyboard_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_keyboard_key_pressed;
		input_event->keyboard_key = keyboard_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_keyboard_key_released( input_keyboard_key_e keyboard_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_keyboard_key_released;
		input_event->keyboard_key = keyboard_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_character( char16_c character, uint8_c character_repeat_count )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_character;
		input_event->character = character;
		input_event->character_repeat_count = character_repeat_count;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_mouse_move( sint32_c mouse_position_x, sint32_c mouse_position_y )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_mouse_move;
		input_event->mouse_position.a = static_cast< float32_c >( mouse_position_x );
		input_event->mouse_position.b = static_cast< float32_c >( mouse_position_y );
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_mouse_wheel( float32_c mouse_wheel_delta )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_mouse_wheel;
		input_event->mouse_wheel_delta = mouse_wheel_delta;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_mouse_key_pressed( input_mouse_key_e mouse_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_mouse_key_pressed;
		input_event->mouse_key = mouse_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::push_mouse_key_released( input_mouse_key_e mouse_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->type = input_event_c::type_e_mouse_key_released;
		input_event->mouse_key = mouse_key;
		_finalize_input_event( input_event );
	}

	core_list_c< input_event_c > & input_manager_c::get_events() const
	{
		return *_events;
	}

	input_key_state_bit_e input_manager_c::get_keyboard_key_state( input_keyboard_key_e key ) const
	{
		assert( key >= 0 && key < input_keyboard_key_e_count_ );
		return _keyboard_keys_state[ key ];
	}

	boolean_c input_manager_c::get_keyboard_key_is_pressed( input_keyboard_key_e key ) const
	{
		assert( key >= 0 && key < input_keyboard_key_e_count_ );
		return ( _keyboard_keys_state[ key ] & input_key_state_bit_e_on ) != 0;
	}

	input_action_c * input_manager_c::create_action( string8_c const & key, string8_c const & context )
	{
		input_action_c * result = new input_action_c( key, context );
		_action_list.insert_at_end( result );
		return result;
	}

	input_action_c * input_manager_c::find_action( string8_c const & key ) const
	{
		for ( sint32_c i = 0; i < _action_list.get_length(); i++ )
		{
			if ( _action_list[ i ]->get_key() == key )
			{
				return _action_list[ i ];
			}
		}
		return nullptr;
	}

	string8_c const & input_manager_c::get_action_context() const
	{
		return _action_context;
	}

	void_c input_manager_c::set_action_context( string8_c const & value )
	{
		_action_context = value;
	}

	input_action_c * input_manager_c::invoke_action_with_shortcut( input_shortcut_c const & shortcut )
	{
		if ( shortcut.type != input_shortcut_c::type_e_none )
		{
			for ( sint32_c i = _action_list.get_length() - 1; i >= 0; i-- )
			{
				input_action_c * action = _action_list[ i ];
				if ( ops::string8_starts_with( _action_context.character_list.get_internal_array(), action->get_context().character_list.get_internal_array() ) )
				{
					if ( action->get_shortcut().type != input_shortcut_c::type_e_none )
					{
						if ( action->get_shortcut() == shortcut )
						{
							action->on_invoked.invoke( action );
							return action;
						}
					}
				}
			}
		}
		return nullptr;
	}

	boolean_c input_manager_c::is_shortcut_pressed( input_shortcut_c const & shortcut ) const
	{
		if ( shortcut.type == input_shortcut_c::type_e_keyboard )
		{
			if ( !( _keyboard_keys_state[ shortcut.keyboard.key ] & input_key_state_bit_e_on ) )
			{
				return false;
			}
			if ( ( shortcut.keyboard.modifier & input_modifier_flag_e_shift ) && !( _modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) )
			{
				return false;
			}
			if ( ( shortcut.keyboard.modifier & input_modifier_flag_e_ctrl ) && !( _modifier_keys_state[ input_modifier_key_e_ctrl ] & input_key_state_bit_e_on ) )
			{
				return false;
			}
			if ( ( shortcut.keyboard.modifier & input_modifier_flag_e_alt ) && !( _modifier_keys_state[ input_modifier_key_e_alt ] & input_key_state_bit_e_on ) )
			{
				return false;
			}
			return true;
		}
		else if ( shortcut.type == input_shortcut_c::type_e_mouse )
		{
			return ( _mouse_keys_state[ shortcut.mouse.key ] & input_key_state_bit_e_on );
		}
		else if ( shortcut.type == input_shortcut_c::type_e_gamepad )
		{
			return ( _gamepad_keys_state[ shortcut.gamepad.key ] & input_key_state_bit_e_on );
		}
		return false;
	}

	boolean_c input_manager_c::clip_board_get_plain_text( string16_c & value )
	{
		boolean_c result = false;
		if ( IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		{
			if ( OpenClipboard( static_cast< HWND >( engine_c::get_instance()->get_window_manager()->get_window_handle() ) ) )
			{
				HGLOBAL hglobalunculus = GetClipboardData( CF_UNICODETEXT );
				if ( hglobalunculus )
				{
					LPWSTR omgwtfbbqlolwowwin32lpwstr = reinterpret_cast< LPWSTR >( GlobalLock( hglobalunculus ) );
					if ( omgwtfbbqlolwowwin32lpwstr )
					{
						value = reinterpret_cast< char16_c * >( omgwtfbbqlolwowwin32lpwstr );
						GlobalUnlock( hglobalunculus );
						result = value.character_list.get_length() - 1 > 0;
					}
				}
				CloseClipboard();
			}
		}
		return result;
	}

	boolean_c input_manager_c::clip_board_set_plain_text( string16_c const & value )
	{
		boolean_c result = false;
		if ( OpenClipboard( static_cast< HWND >( engine_c::get_instance()->get_window_manager()->get_window_handle() ) ) )
		{
			EmptyClipboard();
			HGLOBAL h_what_global_what_tho_hghghghg_win32unnngg = GlobalAlloc( GMEM_MOVEABLE, value.character_list.get_internal_array_size_used() );
			if ( h_what_global_what_tho_hghghghg_win32unnngg )
			{
				LPVOID buffer = GlobalLock( h_what_global_what_tho_hghghghg_win32unnngg );
				if ( buffer )
				{
					ops::memory_copy( value.character_list.get_internal_array(), buffer, value.character_list.get_internal_array_size_used() );
					GlobalUnlock( h_what_global_what_tho_hghghghg_win32unnngg );
					SetClipboardData( CF_UNICODETEXT, h_what_global_what_tho_hghghghg_win32unnngg );
					result = true;
				}
			}
			CloseClipboard();
		}
		return result;
	}

}
