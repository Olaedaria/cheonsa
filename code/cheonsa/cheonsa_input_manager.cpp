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

	input_event_c * input_manager_c::_emplace_input_event()
	{
		input_event_c * input_event = _events_next->emplace( -1, 1 );
		input_event->reset();
		return input_event;
	}

	void_c input_manager_c::_finalize_input_event( input_event_c * input_event )
	{
		// update "living" snapshots of key states.
		if ( input_event->_type == input_event_c::type_e_mouse_move )
		{
			input_event->_mouse_position_delta = input_event->_mouse_position - _event_mouse_position;
			_event_mouse_position = input_event->_mouse_position;
		}
		else if ( input_event->_type == input_event_c::type_e_keyboard_key_pressed )
		{
			assert( input_event->_keyboard_key > 0 && input_event->_keyboard_key < input_keyboard_key_e_count_ );
			if ( input_event->_keyboard_key > 0 && input_event->_keyboard_key < input_keyboard_key_e_count_ )
			{
				_event_keyboard_keys_state[ input_event->_keyboard_key ] = true;
			}
		}
		else if ( input_event->_type == input_event_c::type_e_keyboard_key_released )
		{
			assert( input_event->_keyboard_key > 0 && input_event->_keyboard_key < input_keyboard_key_e_count_ );
			if ( input_event->_keyboard_key > 0 && input_event->_keyboard_key < input_keyboard_key_e_count_ )
			{
				_event_keyboard_keys_state[ input_event->_keyboard_key ] = false;
			}
		}
		else if ( input_event->_type == input_event_c::type_e_mouse_key_pressed )
		{
			assert( input_event->_mouse_key > 0 && input_event->_mouse_key < input_mouse_key_e_count_ );
			boolean_c was_any_mouse_key_down = _event_mouse_keys_state[ input_mouse_key_e_left ] || _event_mouse_keys_state[ input_mouse_key_e_right ] || _event_mouse_keys_state[ input_mouse_key_e_middle ];
			if ( input_event->_mouse_key > 0 && input_event->_mouse_key < input_mouse_key_e_count_ )
			{
				_event_mouse_keys_state[ input_event->_mouse_key ] = true;
			}
			if ( !was_any_mouse_key_down && ( input_event->_mouse_key == input_mouse_key_e_left || input_event->_mouse_key == input_mouse_key_e_right || input_event->_mouse_key == input_mouse_key_e_middle ) )
			{
				SetCapture( static_cast< HWND >( engine.get_window_manager()->get_window_handle() ) );
			}
		}
		else if ( input_event->_type == input_event_c::type_e_mouse_key_released )
		{
			assert( input_event->_mouse_key > 0 && input_event->_mouse_key < input_mouse_key_e_count_ );
			if ( input_event->_mouse_key > 0 && input_event->_mouse_key < input_mouse_key_e_count_ )
			{
				_event_mouse_keys_state[ input_event->_mouse_key ] = false;
			}
			boolean_c is_any_mouse_key_down = _event_mouse_keys_state[ input_mouse_key_e_left ] || _event_mouse_keys_state[ input_mouse_key_e_right ] || _event_mouse_keys_state[ input_mouse_key_e_middle ];
			if ( !is_any_mouse_key_down )
			{
				ReleaseCapture();
			}
		}
		else if ( input_event->_type == input_event_c::type_e_gamepad_key_pressed )
		{
			assert( input_event->_gamepad_key > 0 && input_event->_gamepad_key < input_gamepad_key_e_count_ );
			if ( input_event->_gamepad_key > 0 && input_event->_gamepad_key < input_gamepad_key_e_count_ )
			{
				_event_gamepad_keys_state[ input_event->_gamepad_key ] = true;
			}
		}
		else if ( input_event->_type == input_event_c::type_e_gamepad_key_released )
		{
			assert( input_event->_gamepad_key > 0 && input_event->_gamepad_key < input_gamepad_key_e_count_ );
			if ( input_event->_gamepad_key > 0 && input_event->_gamepad_key < input_gamepad_key_e_count_ )
			{
				_event_gamepad_keys_state[ input_event->_gamepad_key ] = false;
			}
		}
		if ( input_event->_type == input_event_c::type_e_keyboard_key_pressed || input_event->_type == input_event_c::type_e_keyboard_key_released )
		{
			_event_modifier_flags = input_modifier_flag_e_none;
			if ( _event_keyboard_keys_state[ input_keyboard_key_e_l_shift ] || _event_keyboard_keys_state[ input_keyboard_key_e_r_shift ] )
			{
				_event_modifier_flags = static_cast< input_modifier_flag_e >( _event_modifier_flags | input_modifier_flag_e_shift );
			}
			if ( _event_keyboard_keys_state[ input_keyboard_key_e_l_ctrl ] || _event_keyboard_keys_state[ input_keyboard_key_e_r_ctrl ] )
			{
				_event_modifier_flags = static_cast< input_modifier_flag_e >( _event_modifier_flags | input_modifier_flag_e_ctrl );
			}
			if ( _event_keyboard_keys_state[ input_keyboard_key_e_l_alt ] || _event_keyboard_keys_state[ input_keyboard_key_e_r_alt ] )
			{
				_event_modifier_flags = static_cast< input_modifier_flag_e >( _event_modifier_flags | input_modifier_flag_e_alt );
			}
		}

		// copy "living" snapshots to input event, so that the program has access to all key states at the time that the event occurred.
		// it might be too much but it can be nice to have sometimes.
		static_assert( sizeof( input_event->_mouse_keys_state ) == sizeof( _event_mouse_keys_state ), "just checking" );
		static_assert( sizeof( input_event->_keyboard_keys_state ) == sizeof( _event_keyboard_keys_state ), "just checking" );
		static_assert( sizeof( input_event->_gamepad_keys_state ) == sizeof( _event_gamepad_keys_state ), "just checking" );
		static_assert( sizeof( input_event->_mouse_keys_state ) == sizeof( boolean_c ) * input_mouse_key_e_count_, "just checking" );
		static_assert( sizeof( input_event->_keyboard_keys_state ) == sizeof( boolean_c ) * input_keyboard_key_e_count_, "just checking" );
		static_assert( sizeof( input_event->_gamepad_keys_state ) == sizeof( boolean_c ) * input_gamepad_key_e_count_, "just checking" );
		input_event->_mouse_position = _event_mouse_position;
		input_event->_modifier_flags = _event_modifier_flags;
		ops::memory_copy( _event_mouse_keys_state, input_event->_mouse_keys_state, sizeof( input_event->_mouse_keys_state ) );
		ops::memory_copy( _event_keyboard_keys_state, input_event->_keyboard_keys_state, sizeof( input_event->_keyboard_keys_state ) );
		ops::memory_copy( _event_gamepad_keys_state, input_event->_gamepad_keys_state, sizeof( input_event->_gamepad_keys_state ) );
	}

	input_manager_c::input_manager_c()
		: _events_current( nullptr )
		, _events_next( nullptr )
		, _events_ping()
		, _events_pong()
		, _event_modifier_flags( input_modifier_flag_e_none )
		, _event_mouse_position()
		, _event_mouse_keys_state{}
		, _event_keyboard_keys_state{}
		, _event_gamepad_keys_state{}
		, _frame_modifier_flags( input_modifier_flag_e_none )
		, _frame_mouse_position_last()
		, _frame_mouse_position()
		, _frame_mouse_position_delta()
		, _frame_mouse_keys_state{}
		, _frame_keyboard_keys_state{}
		, _frame_gamepad_state()
		, _mouse_pointer_visibility( true )
		, _drag_drop_is_in_progress( false )
		, _drag_drop_payload()
		, _action_scope()
		, _action_list()
	{
		_events_current = &_events_ping;
		_events_next = &_events_pong;
	}

	input_manager_c::~input_manager_c()
	{
	}

	boolean_c input_manager_c::start()
	{
		return true;
	}

	void_c input_manager_c::update()
	{
		static_assert( XUSER_MAX_COUNT == 4, "just checking, because _gamepad_states in header has its length set to 4, and does not have access to xinput.h." );
		XINPUT_KEYSTROKE xinput_keystroke;
		DWORD xinput_result;
		while ( true )
		{
			xinput_result = XInputGetKeystroke( 0, 0, &xinput_keystroke );
			if ( xinput_result != ERROR_SUCCESS )
			{
				break;
			}
			input_gamepad_key_e gamepad_key = input_convert_xinput_virtual_key_code_to_cheonsa_gamepad_key_code( xinput_keystroke.VirtualKey );
			if ( gamepad_key != 0 )
			{
				if ( xinput_keystroke.Flags == XINPUT_KEYSTROKE_KEYDOWN || xinput_keystroke.Flags == XINPUT_KEYSTROKE_REPEAT )
				{
					_push_gamepad_key_pressed( gamepad_key );
				}
				else if ( xinput_keystroke.Flags == XINPUT_KEYSTROKE_KEYUP )
				{
					_push_gamepad_key_released( gamepad_key );
				}
			}
		}

		XINPUT_STATE xinput_state;
		if ( XInputGetState( 0, &xinput_state ) == ERROR_SUCCESS )
		{
			_frame_gamepad_state._index = 0;
			_frame_gamepad_state._is_connected	= true;
			_frame_gamepad_state._button_states = 0;
			_frame_gamepad_state._button_states = xinput_state.Gamepad.wButtons; // cheonsa's gamepad button flags are currently the same as xinput's button flags, so we can just copy value without translating it.

			// convert analog input integer values to unit float values.
			// map triggers values to range 0 to 1.
			_frame_gamepad_state._left_trigger_state = static_cast< float32_c >( xinput_state.Gamepad.bLeftTrigger ) / 255.0f;
			_frame_gamepad_state._right_trigger_state = static_cast< float32_c >( xinput_state.Gamepad.bRightTrigger ) / 255.0f;

			// xinput documentation states that range of thumb stick values is between -32768 and 32767.
			// this is asymmetrical. there's one more point of negative values than there is of positive values.
			// map thumb stick values to range -1 to 1.
			_frame_gamepad_state._left_stick_state.a = static_cast< float32_c >( ops::math_clamp( xinput_state.Gamepad.sThumbLX, -32767, 32767 ) ) / 32767.0f;
			_frame_gamepad_state._left_stick_state.b = static_cast< float32_c >( ops::math_clamp( xinput_state.Gamepad.sThumbLY, -32767, 32767 ) ) / 32767.0f;
			_frame_gamepad_state._right_stick_state.a = static_cast< float32_c >( ops::math_clamp( xinput_state.Gamepad.sThumbRX, -32767, 32767 ) ) / 32767.0f;
			_frame_gamepad_state._right_stick_state.b = static_cast< float32_c >( ops::math_clamp( xinput_state.Gamepad.sThumbRY, -32767, 32767 ) ) / 32767.0f;
			_frame_gamepad_state._cap_length_and_make_dead_zone();
		}
		else
		{
			_frame_gamepad_state._is_connected = false;
			_frame_gamepad_state._button_states = 0;
			_frame_gamepad_state._left_trigger_state = 0.0f;
			_frame_gamepad_state._right_trigger_state = 0.0f;
			_frame_gamepad_state._left_stick_state = vector32x2_c( 0.0f, 0.0f );
			_frame_gamepad_state._right_stick_state = vector32x2_c( 0.0f, 0.0f );
			_frame_gamepad_state._left_stick_state_with_dead_zone = vector32x2_c( 0.0f, 0.0f );
			_frame_gamepad_state._right_stick_state_with_dead_zone = vector32x2_c( 0.0f, 0.0f );
		}

		_frame_modifier_flags = _event_modifier_flags;
		_frame_mouse_position_last = _frame_mouse_position;
		_frame_mouse_position = _event_mouse_position;
		_frame_mouse_position_delta = _frame_mouse_position - _frame_mouse_position_last;
		static_assert( sizeof( _event_mouse_keys_state ) == sizeof( _frame_mouse_keys_state ), "just checking" );
		static_assert( sizeof( _event_keyboard_keys_state ) == sizeof( _frame_keyboard_keys_state ), "just checking" );
		ops::memory_copy( _event_mouse_keys_state, _frame_mouse_keys_state, sizeof( _frame_mouse_keys_state ) );
		ops::memory_copy( _event_keyboard_keys_state, _frame_keyboard_keys_state, sizeof( _frame_keyboard_keys_state ) );

		core_list_c< input_event_c > * temp = _events_current;
		_events_current = _events_next;
		_events_next = temp;
		_events_next->remove_all();
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

	void_c input_manager_c::set_mouse_pointer_position( vector32x2_c value )
	{
#if defined( cheonsa_platform_windows )
		_event_mouse_position = value;
		RECT window_client_rectangle;
		GetWindowRect( static_cast< HWND >( engine.get_window_manager()->get_window_handle() ), &window_client_rectangle );
		SetCursorPos( static_cast< int >( value.a + window_client_rectangle.left ), static_cast< int >( value.b + window_client_rectangle.top ) );
#endif
	}

	void_c input_manager_c::_push_keyboard_key_pressed( input_keyboard_key_e keyboard_key )
	{
		if ( keyboard_key == input_keyboard_key_e_none )
		{
			return;
		}
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_keyboard_key_pressed;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_keyboard_key = keyboard_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_keyboard_key_released( input_keyboard_key_e keyboard_key )
	{
		if ( keyboard_key == input_keyboard_key_e_none )
		{
			return;
		}
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_keyboard_key_released;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_keyboard_key = keyboard_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_character( char16_c character, uint8_c character_repeat_count )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_character;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_character = character;
		input_event->_character_repeat_count = character_repeat_count;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_mouse_move( sint32_c mouse_position_x, sint32_c mouse_position_y )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_mouse_move;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_mouse_position.a = static_cast< float32_c >( mouse_position_x );
		input_event->_mouse_position.b = static_cast< float32_c >( mouse_position_y );
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_mouse_wheel( float32_c mouse_wheel_delta )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_mouse_wheel;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_mouse_wheel_delta = mouse_wheel_delta;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_mouse_key_pressed( input_mouse_key_e mouse_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_mouse_key_pressed;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_mouse_key = mouse_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_mouse_key_released( input_mouse_key_e mouse_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_mouse_key_released;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_mouse_key = mouse_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_gamepad_key_pressed( input_gamepad_key_e gamepad_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_gamepad_key_pressed;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_gamepad_key = gamepad_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_push_gamepad_key_released( input_gamepad_key_e gamepad_key )
	{
		input_event_c * input_event = _emplace_input_event();
		input_event->_type = input_event_c::type_e_gamepad_key_released;
		input_event->_time = ops::time_get_high_resolution_timer_count();
		input_event->_gamepad_key = gamepad_key;
		_finalize_input_event( input_event );
	}

	void_c input_manager_c::_release_all_keys()
	{
		for ( sint32_c i = 1; i < input_mouse_key_e_count_; i++ )
		{
			if ( _event_mouse_keys_state[ i ] )
			{
				_push_mouse_key_released( static_cast< input_mouse_key_e >( i ) );
			}
		}
		for ( sint32_c i = 1; i < input_keyboard_key_e_count_; i++ )
		{
			if ( _event_keyboard_keys_state[ i ] )
			{
				_push_keyboard_key_released( static_cast< input_keyboard_key_e >( i ) );
			}
		}
		for ( sint32_c i = 1; i < input_gamepad_key_e_count_; i++ )
		{
			if ( _event_gamepad_keys_state[ i ] )
			{
				_push_gamepad_key_released( static_cast< input_gamepad_key_e >( i ) );
			}
		}
	}

	core_list_c< input_event_c > & input_manager_c::get_events() const
	{
		return *_events_current;
	}

	input_modifier_flag_e input_manager_c::get_frame_modifier_flags() const
	{
		return _frame_modifier_flags;
	}

	vector32x2_c input_manager_c::get_frame_mouse_position() const
	{
		return _frame_mouse_position;
	}

	vector32x2_c input_manager_c::get_frame_mouse_position_delta() const
	{
		return _frame_mouse_position_delta;
	}

	boolean_c input_manager_c::get_frame_mouse_key_state( input_mouse_key_e mouse_key ) const
	{
		assert( mouse_key > input_mouse_key_e_none && mouse_key < input_mouse_key_e_count_ );
		return _frame_mouse_keys_state[ mouse_key ];
	}

	boolean_c input_manager_c::get_frame_keyboard_key_state( input_keyboard_key_e keyboard_key ) const
	{
		assert( keyboard_key > input_keyboard_key_e_none && keyboard_key < input_keyboard_key_e_count_ );
		return _frame_keyboard_keys_state[ keyboard_key ];
	}

	input_gamepad_state_c const & input_manager_c::get_frame_gamepad_state() const
	{
		return _frame_gamepad_state;
	}

	void_c input_manager_c::set_gamepad_vibration( float32_c left_motor_speed, float32_c right_motor_speed ) const
	{
		XINPUT_VIBRATION xinput_vibration;
		xinput_vibration.wLeftMotorSpeed = static_cast< WORD >( ops::math_saturate( left_motor_speed ) * 65535.0f );
		xinput_vibration.wRightMotorSpeed = static_cast< WORD >( ops::math_saturate( right_motor_speed ) * 65535.0f );
		XInputSetState( 0, &xinput_vibration );
	}

	void_c input_manager_c::add_action( input_action_c * action )
	{
		_action_list.insert( -1, action );
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

	string8_c const & input_manager_c::get_action_scope() const
	{
		return _action_scope;
	}

	void_c input_manager_c::set_action_scope( string8_c const & value )
	{
		_action_scope = value;
	}

	void_c input_manager_c::_invoke_action_with_input_event( input_event_c * input_event )
	{
		for ( sint32_c i = 0; i < _action_list.get_length(); i++ )
		{
			input_action_c * action = _action_list[ i ];
			if ( action->_scope == _action_scope )
			{
				if ( action->_shortcut.is_pressed_for_event( input_event ) )
				{
					action->on_invoked.invoke( action );
				}
			}
		}
	}

	//input_action_c * input_manager_c::invoke_action_with_shortcut( input_shortcut_c const & shortcut )
	//{
	//	if ( shortcut._mouse_key != 0 || shortcut._keyboard_key != 0 || shortcut._gamepad_button != 0 )
	//	{
	//		for ( sint32_c i = _action_list.get_length() - 1; i >= 0; i-- )
	//		{
	//			input_action_c * action = _action_list[ i ];
	//			if ( ops::string8_starts_with( _action_context.character_list.get_internal_array(), action->get_context().character_list.get_internal_array() ) )
	//			{
	//				if ( action->get_shortcut() == shortcut )
	//				{
	//					action->on_invoked.invoke( action );
	//					return action;
	//				}
	//			}
	//		}
	//	}
	//	return nullptr;
	//}

	boolean_c input_manager_c::get_plain_text_clip_board_value( string16_c & value )
	{
		boolean_c result = false;
		if ( IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		{
			if ( OpenClipboard( static_cast< HWND >( engine.get_window_manager()->get_window_handle() ) ) )
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

	boolean_c input_manager_c::set_plain_text_clip_board_value( string16_c const & value )
	{
		boolean_c result = false;
		if ( OpenClipboard( static_cast< HWND >( engine.get_window_manager()->get_window_handle() ) ) )
		{
			EmptyClipboard();
			HGLOBAL h_what_global_what_tho_hghghghg_win32unnngg = GlobalAlloc( GMEM_MOVEABLE, value.character_list.get_internal_array_size() );
			if ( h_what_global_what_tho_hghghghg_win32unnngg )
			{
				LPVOID buffer = GlobalLock( h_what_global_what_tho_hghghghg_win32unnngg );
				if ( buffer )
				{
					ops::memory_copy( value.character_list.get_internal_array(), buffer, value.character_list.get_internal_array_size() );
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
