//#include <vld.h>

#include "cheonsa_engine.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_data_scribe_text.h"
#include "cheonsa_network.h"
#include "cheonsa_time_date_gregorian.h"
#include "cheonsa_ops.h"
#include "cheonsa_debug.h"
#include "cheonsa_resource_object_font.h" // to manage global glyph cache.

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN // tells windows.h to not include every other windows related header for us, which solves the conflicts that when we include winsock2.h a couple lines later.
#include <Windows.h> // for most of windows.
//#include <Windowsx.h> // for GET_X_LPARAM and GET_Y_LPARAM. no longer included because for some reason intellisense could see GET_X_LPARAM and GET_Y_LPARAM but when compiling they were not resolving. so we just bit manipulate by hand.
#include <dwmapi.h>
#define cheonsa_window_style_normal ( WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN )
#define cheonsa_window_style_borderless ( WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN )
//#define cheonsa_window_style ( WS_OVERLAPPEDWINDOW )
//#define cheonsa_window_style ( WS_POPUP | WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX )
//#define cheonsa_window_style_windowed ( WS_THICKFRAME | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX )
//#define cheonsa_window_style_full_screen ( WS_POPUP | WS_VISIBLE )
#endif

// if this file exists in executable_folder_path, then it will be opened and read.
// if it contains a single line of text that resolves as a valid folder path then that is what will be used as the engine_data_folder_path.
//#define defined_engine_data_file_name "engine_data.txt"

// if this file exists in the same folder as the executable, and if the engine is started in client mode, then it will be opened anr ead.
// if it contains a single line of text that resolves as a valid folder path then that is what will be used as the game_data_folder_path.
//#define defined_game_data_file_name "game_data.txt"

// if this file exists in the same folder as the executable, and if the engine is started in client mode, then it will be opened and read.
// it is scanned for a couple lines of text:
// engine_data=[engine data folder path]
// game_data=[game data folder path]
#define defined_data_file_name "data.txt"

// this is the name of the settings file.
#define defined_settings_file_name "settings.ini"

#define window_width_minimum 1440
#define window_height_minimum 810

namespace cheonsa
{



	engine_c global_engine_instance;



#if defined( cheonsa_platform_windows )
	input_keyboard_key_e get_keyboard_key( WPARAM wParam, LPARAM lParam )
	{
		UINT scan_code = ( lParam & 0x00ff0000 ) >> 16;
		UINT extended = ( lParam & 0x01000000 );
		if ( wParam == VK_SHIFT )
		{
			return input_convert_windows_virtual_key_code_to_cheonsa_keyboard_key_code( MapVirtualKey( scan_code, MAPVK_VSC_TO_VK_EX ) );
		}
		else if ( wParam == VK_CONTROL )
		{
			return extended ? input_keyboard_key_e_r_ctrl : input_keyboard_key_e_l_ctrl;
		}
		else if ( wParam == VK_MENU )
		{
			return extended ? input_keyboard_key_e_r_alt : input_keyboard_key_e_l_alt;
		}
		else
		{
			return input_convert_windows_virtual_key_code_to_cheonsa_keyboard_key_code( wParam );
		}
	}

	// hidden engine members class, which contains platform specific stuff.
	class engine_members_c
	{
	public:
		HINSTANCE process_handle; // application process handle (passed to us from Windows when our program starts).
		HWND window_handle; // application window handle (created by us when we create our window).

	public:
		engine_members_c( engine_c * engine )
			: process_handle( 0 )
			, window_handle( 0 )
		{
		}

		static LRESULT __stdcall WndProc2( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
		{
			engine_c * engine = reinterpret_cast< engine_c * >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

			if ( !engine || !engine->_is_running )
			{
				return DefWindowProc( hWnd, msg, wParam, lParam );
			}

			switch ( msg )
			{
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_keyboard_key_pressed( get_keyboard_key( wParam, lParam ) );
					}
					//static BYTE KeyboardState[256];
					//GetKeyboardState( KeyboardState );
					//char16_c CharBuffer[32];
					//uint32_c ScanCode = ( lParam & 0xF0000 ) >> 16;
					//sint32_c count = ToUnicode( wParam, ScanCode, KeyboardState, CharBuffer, 32, 0 );
					//Input.Text.Update_AddChar( count ? CharBuffer[0] : 0, KeyID, lParam & 0x000000FF, ( _ShiftL || _ShiftR ), ( _ControlL || _ControlR ), ( _AltL || _AltR ) );
					//if ( wParam == VK_F4 && ( lParam & ( 1 << 29 ) ) != 0 ) // handle alt+f4 on our own. or maybe we should process the message but also just default to DefWindowProc.
					//{
					//	PostQuitMessage(0);
					//}
					if ( msg == WM_SYSKEYDOWN )
					{
						return DefWindowProc( hWnd, msg, wParam, lParam );
					}
					return 0;
				}

				case WM_KEYUP:
				case WM_SYSKEYUP:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_keyboard_key_released( get_keyboard_key( wParam, lParam ) );
					}
					if ( msg == WM_SYSKEYUP )
					{
						return DefWindowProc( hWnd, msg, wParam, lParam );
					}
					return 0;
				}

				case WM_CHAR:
				{
					// only pass through messages for visible characters.
					// ignore all messages for characters less than 32, as these are for control characters that we can ignore.
					if ( wParam >= 32 && engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_character( static_cast< char16_c >( wParam ), static_cast< uint8_c >( lParam & 0xFFFF ) );
					}
					return 0;
				}

				//case WM_CHAR:
				//	if ( ( Input.keyboard.InputImmediate[Input::EKey_LControl].State & Input::State_Held ) || ( Input.keyboard.InputImmediate[Input::EKey_RControl].State & Input::State_Held ) || ( Input.keyboard.InputImmediate[Input::EKey_LAlt].State & Input::State_Held ) || ( Input.keyboard.InputImmediate[Input::EKey_RAlt].State & Input::State_Held ) )
				//	{
				//		// this is not a 100% effective way to filter out input modifiers (such as ctrl and alt)
				//		// if ctrl + anykey is pressed at nearly the same time, the resulting character will still be passed through
				//		// i have to find a better way to deal with this
				//		// WM_CHAR really kind of sucks
				//		// more info:
				//		// http://blog.ngedit.com/2005/06/13/whats-broken-in-the-wm_keydownwm_char-input-model/
				//	}
				//	else
				//	{
				//		Input.Text.Update_AddCharacter( wParam, ( uint8_c )( lParam & 0xFFFF ) );
				//	}
				//	return 0;

				case WM_MOUSEMOVE:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_move( (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) );
					}
					return 0;
				}

				case WM_MOUSEWHEEL:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_wheel( static_cast< float32_c >( GET_WHEEL_DELTA_WPARAM( wParam ) ) / 120.0f );
					}
					return 0;
				}

				case WM_LBUTTONDOWN:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_pressed( input_mouse_key_e_left );
					}
					return 0;
				}

				case WM_LBUTTONUP:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_released( input_mouse_key_e_left );
					}
					return 0;
				}

				case WM_RBUTTONDOWN:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_pressed( input_mouse_key_e_right );
					}
					return 0;
				}

				case WM_RBUTTONUP:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_released( input_mouse_key_e_right );
					}
					return 0;
				}

				case WM_MBUTTONDOWN:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_pressed( input_mouse_key_e_middle );
					}
					return 0;
				}

				case WM_MBUTTONUP:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_released( input_mouse_key_e_middle );
					}
					return 0;
				}

				case WM_XBUTTONDOWN:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_pressed( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? input_mouse_key_e_extra1 : input_mouse_key_e_extra2 );
					}
					return 0;
				}

				case WM_XBUTTONUP:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->push_mouse_key_released( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? input_mouse_key_e_extra1 : input_mouse_key_e_extra2 );
					}
					return 0;
				}

				//case WM_INPUT:
				//	{
				//		RAWINPUT RawInput;
				//		UINT RawInputsize = sizeof( RawInput );
				//		GetRawInputData( reinterpret_cast< HRAWINPUT >( lParam ), RID_INPUT, & RawInput, & RawInputsize, sizeof( RAWINPUTHEADER ) );
				//		if ( RawInput.header.dwType == RIM_TYPEMOUSE )
				//		{
				//			global_engine_instance.input.mouse.position_delta_accumulator.a += RawInput.data.mouse.lLastX;
				//			global_engine_instance.input.mouse.position_delta_accumulator.b += RawInput.data.mouse.lLastY;
				//			if ( RawInput.data.mouse.usButtonFlags & RI_MOUSE_WHEEL )
				//			{
				//				global_engine_instance.input.mouse.wheel_delta_accumulator += static_cast< float32_c >( static_cast< short >( LOWORD( RawInput.data.mouse.usButtonData ) ) / static_cast< float32_c >( WHEEL_DELTA ) );
				//			}
				//		}
				//	}
				//	return 0;

				case WM_MOVING:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->_release_all_keys();
					}
					return 0;
				}

				case WM_SIZE:
				{
					RECT window_rectangle;
					GetClientRect( hWnd, &window_rectangle );
					if ( wParam == SIZE_MAXIMIZED )
					{
						engine->environment._window_state = window_state_e_maximized;
					}
					else if ( wParam == SIZE_MINIMIZED )
					{
						engine->environment._window_state = window_state_e_minimized;
					}
					else if ( wParam == SIZE_RESTORED )
					{
						engine->environment._window_state = window_state_e_normaled;
					}
					//if ( engine->interfaces._game != nullptr )
					//{
					//	engine->interfaces._game->on_window_state_changed();
					//}
					break;
				}

				case WM_SIZING:
				{
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->_release_all_keys();
					}

					LPRECT window_rect = reinterpret_cast< LPRECT >( lParam );

					// adjust constraints for client area
					RECT fixed_window_rect;
					fixed_window_rect.left = 0;
					fixed_window_rect.top = 0;
					fixed_window_rect.right = window_width_minimum;
					fixed_window_rect.bottom = window_height_minimum;
					//AdjustWindowRect( &fixed_window_rect, cheonsa_window_style_borderless, FALSE );

					uint32_c fixed_window_width = fixed_window_rect.right - fixed_window_rect.left;
					uint32_c fixed_window_height = fixed_window_rect.bottom - fixed_window_rect.top;

					if ( wParam == WMSZ_LEFT || wParam == WMSZ_TOPLEFT || wParam == WMSZ_BOTTOMLEFT )
					{
						// constrain left edge
						if ( window_rect->left > window_rect->right - static_cast< LONG >( fixed_window_width ) )
						{
							window_rect->left = window_rect->right - static_cast< LONG >( fixed_window_width );
						}
					}
					if ( wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT )
					{
						// constrain top edge
						if ( window_rect->top > window_rect->bottom - static_cast< LONG >( fixed_window_height ) )
						{
							window_rect->top = window_rect->bottom - static_cast< LONG >( fixed_window_height );
						}
					}
					if ( wParam == WMSZ_RIGHT || wParam == WMSZ_TOPRIGHT || wParam == WMSZ_BOTTOMRIGHT )
					{
						// constrain right edge
						if ( window_rect->right < window_rect->left + static_cast< LONG >( fixed_window_width ) )
						{
							window_rect->right = window_rect->left + static_cast< LONG >( fixed_window_width );
						}
					}
					if ( wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT )
					{
						// constrain bottom edge
						if ( window_rect->bottom < window_rect->top + static_cast< LONG >( fixed_window_height ) )
						{
							window_rect->bottom = window_rect->top + static_cast< LONG >( fixed_window_height );
						}
					}
					return 1;
				}

				case WM_SETFOCUS:
				{
					engine->environment._window_is_focused = true;
					return 0;
				}

				case WM_KILLFOCUS:
				{
					engine->environment._window_is_focused = false;
					if ( engine->interfaces.input_manager != nullptr )
					{
						engine->interfaces.input_manager->_release_all_keys();
					}
					return 0;
				}

				//case WM_SETCURSOR:
				//{
				//	if ( LOWORD( lParam ) == HTCLIENT )
				//	{
				//		if ( global_engine_instance._pointer )
				//		{
				//			SetCursor( reinterpret_cast<HCURSOR>( global_engine_instance._pointer->pointer_handle ) );
				//			return 1;
				//		}
				//	}
				//	break;
				//}

				case WM_NCCALCSIZE:
				{
					return 0; // this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
				}

				case WM_NCHITTEST:
				{
					sint32_c x = (int)(short)LOWORD( lParam );
					sint32_c y = (int)(short)HIWORD( lParam );
					RECT window_rectangle;
					GetWindowRect( hWnd, &window_rectangle );

					LRESULT result = 0;
					if ( global_engine_instance.interfaces.user_interface )
					{
						menu_non_client_type_e non_client_type = global_engine_instance.interfaces.user_interface->perform_non_client_hit_test( vector32x2_c( (float32_c)( x - window_rectangle.left ), (float32_c)( y - window_rectangle.top ) ) );
						if ( non_client_type == menu_non_client_type_e_none )
						{
							// do nothing, continue with regular non client hit test next.
						}
						else if ( non_client_type == menu_non_client_type_e_client )
						{
							// do nothing, let the engine handle the event for itself.
							break;
						}
						else if ( non_client_type == menu_non_client_type_e_title_bar )
						{
							result = HTCAPTION;
						}
						else if ( non_client_type == menu_non_client_type_e_size_left )
						{
							result = HTLEFT;
						}
						else if ( non_client_type == menu_non_client_type_e_size_top )
						{
							result = HTTOP;
						}
						else if ( non_client_type == menu_non_client_type_e_size_top_left )
						{
							result = HTTOPLEFT;
						}
						else if ( non_client_type == menu_non_client_type_e_size_top_right )
						{
							result = HTTOPRIGHT;
						}
						else if ( non_client_type == menu_non_client_type_e_size_right )
						{
							result = HTRIGHT;
						}
						else if ( non_client_type == menu_non_client_type_e_size_bottom )
						{
							result = HTBOTTOM;
						}
						else if ( non_client_type == menu_non_client_type_e_size_bottom_left )
						{
							result = HTBOTTOMLEFT;
						}
						else if ( non_client_type == menu_non_client_type_e_size_bottom_right )
						{
							result = HTBOTTOMRIGHT;
						}
						else if ( non_client_type == menu_non_client_type_e_button_close )
						{
							result = HTCLOSE;
						}
						else if ( non_client_type == menu_non_client_type_e_button_maximize_restore )
						{
							result = HTMAXBUTTON;
						}
						else if ( non_client_type == menu_non_client_type_e_button_minimize )
						{
							result = HTMINBUTTON;
						}
					}

					if ( x >= window_rectangle.left && x < window_rectangle.left + engine->environment._window_edge_thickness && y < window_rectangle.bottom && y >= window_rectangle.bottom - engine->environment._window_edge_thickness )
					{
						result = HTBOTTOMLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine->environment._window_edge_thickness && y < window_rectangle.bottom && y >= window_rectangle.bottom - engine->environment._window_edge_thickness )
					{
						result = HTBOTTOMRIGHT;
					}
					else if ( x >= window_rectangle.left && x < window_rectangle.left + engine->environment._window_edge_thickness && y >= window_rectangle.top && y < window_rectangle.top + engine->environment._window_edge_thickness )
					{
						result = HTTOPLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine->environment._window_edge_thickness && y >= window_rectangle.top && y < window_rectangle.top + engine->environment._window_edge_thickness )
					{
						result = HTTOPRIGHT;
					}
					else if ( x >= window_rectangle.left && x < window_rectangle.left + engine->environment._window_edge_thickness )
					{
						result = HTLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine->environment._window_edge_thickness )
					{
						result = HTRIGHT;
					}
					else if ( y < window_rectangle.bottom && y >= window_rectangle.bottom - engine->environment._window_edge_thickness )
					{
						result = HTBOTTOM;
					}
					else if ( y >= window_rectangle.top && y < window_rectangle.top + engine->environment._window_edge_thickness )
					{
						result = HTTOP;
					}
					else if ( x >= window_rectangle.left && x <= window_rectangle.right && y >= window_rectangle.top && y < window_rectangle.top + engine->environment._window_edge_thickness + engine->environment._window_title_bar_thickness + ( engine->environment._window_state == window_state_e_maximized ? 8 : 0 ) )
					{
						result = HTCAPTION;
					}

					if ( result != 0 )
					{
						global_engine_instance.interfaces.user_interface->let_go_of_mouse_overed();
						return result;
					}

					break;
				}

				case WM_DESTROY:
				{
					engine->stop();
					return 0;
				}

			}

			return DefWindowProc( hWnd, msg, wParam, lParam );
		}

		//struct monitor_c
		//{
		//	sint32_c index;
		//	sint32_c rectangle[ 4 ]; // left, top, right, bottom.
		//	HMONITOR handle;
		//};

		//core_list_c< engine_members_c::monitor_c > monitors;

		//static BOOL CALLBACK monitor_enumeration_process(
		//	_In_  HMONITOR hMonitor,
		//	_In_  HDC      hdcMonitor,
		//	_In_  LPRECT   lprcMonitor,
		//	_In_  LPARAM   dwData
		//)
		//{
		//	engine_members_c * members = reinterpret_cast< engine_members_c * >( dwData );
		//	engine_members_c::monitor_c monitor;
		//	monitor.index = members->monitors.get_length();
		//	monitor.rectangle[ 0 ] = lprcMonitor->left;
		//	monitor.rectangle[ 1 ] = lprcMonitor->top;
		//	monitor.rectangle[ 2 ] = lprcMonitor->right;
		//	monitor.rectangle[ 3 ] = lprcMonitor->bottom;
		//	monitor.handle = hMonitor;
		//	members->monitors.insert_at_end( monitor );
		//	return true;
		//}

	};
#endif




	//
	//
	//
	// engine
	//
	//
	//

	engine_c::interfaces_c::interfaces_c()
		: game( nullptr )
		, content_manager( nullptr )
		, resource_manager( nullptr )
		, audio_interface( nullptr )
		, video_interface( nullptr )
		, video_renderer_interface( nullptr )
		, video_renderer_shader_manager( nullptr )
		, glyph_manager( nullptr )
		, menu_style_manager( nullptr )
		, input_manager( nullptr )
		, script_manager( nullptr )
		, reflection_manager( nullptr )
		, user_interface( nullptr )
	{
	}

	engine_c::environment_c::environment_c()
		: _window_state( window_state_e_minimized )
		, _window_is_focused( false )
		, _window_edge_thickness( 8 )
		, _window_title_bar_thickness( 30 )
		, _executable_folder_path()
		, _executable_file_name()
		, _logical_processor_count( 0 )
	{
	}

	void_c * engine_c::environment_c::get_window_handle()
	{
		return global_engine_instance._members->window_handle;
	}

	void_c engine_c::environment_c::set_window_title( string16_c const & value )
	{
#if defined( cheonsa_platform_windows )
		SetWindowTextW( global_engine_instance._members->window_handle, value.character_list.get_internal_array() );
#endif
	}

	void_c engine_c::environment_c::set_window_icon_big( platform_icon_c * value )
	{
#if defined( cheonsa_platform_windows )
		SendMessage( global_engine_instance._members->window_handle, WM_SETICON, ICON_BIG, reinterpret_cast< LPARAM >( value->icon_handle ) );
		//SetClassLong( _members->window_handle, GCL_HICON, (LONG)Icon->IconHandle );
#endif
	}

	void_c engine_c::environment_c::set_window_icon_small( platform_icon_c * value )
	{
#if defined( cheonsa_platform_windows )
		SendMessage( global_engine_instance._members->window_handle, WM_SETICON, ICON_SMALL, reinterpret_cast< LPARAM >( value->icon_handle ) );
		//SetClassLong( _members->window_handle, GCL_HICON, (LONG)Icon->IconHandle );
#endif
	}

	window_state_e engine_c::environment_c::get_window_state() const
	{
		return _window_state;
	}

	void_c engine_c::environment_c::set_window_state( window_state_e value )
	{
		if ( value == window_state_e_normaled )
		{
			ShowWindow( global_engine_instance._members->window_handle, SW_RESTORE );
		}
		else if ( value == window_state_e_maximized )
		{
			ShowWindow( global_engine_instance._members->window_handle, SW_MAXIMIZE );
		}
		else if ( value == window_state_e_minimized )
		{
			ShowWindow( global_engine_instance._members->window_handle, SW_MINIMIZE );
		}
	}

	sint32_c engine_c::environment_c::get_window_edge_thickness() const
	{
		return _window_edge_thickness;
	}

	sint32_c engine_c::environment_c::get_window_title_bar_thickness() const
	{
		return _window_title_bar_thickness;
	}

	engine_c::debug_c::console_line_c & engine_c::debug_c::console_line_c::operator = ( console_line_c const & other )
	{
		type = other.type;
		message = other.message;
		return *this;
	}

	engine_c::debug_c::debug_c()
		: _log_file_is_open( false )
		, _log_file()
		, _statistics_is_showing( false )
		, _statistics_text()
		, _console_is_showing( false )
		, _console_lines( 4096 )
	{
	}

	void_c engine_c::debug_c::_start()
	{
		string16_c log_file_path;
		log_file_path += global_engine_instance.environment._executable_folder_path;
		log_file_path += global_engine_instance.environment._executable_file_name;
		log_file_path += ".log";
		_log_file_is_open = _log_file.open( log_file_path, data_stream_mode_e_write );
	}

	void_c engine_c::debug_c::_stop()
	{
		if ( _log_file_is_open )
		{
			_log_file.close();
		}
	}

	void_c engine_c::debug_c::_log( log_type_e type, string16_c const & message )
	{
#if defined( _DEBUG )
		OutputDebugStringW( message.character_list.get_internal_array() );
#endif
		if ( _log_file_is_open && type != log_type_e_information )
		{
			core_list_c< char8_c > utf8;
			ops::convert_string16_to_string8( message.character_list, utf8 );
			_log_file.save( utf8.get_internal_array(), utf8.get_length() - 1 );
			_log_file.save( "\n", 1 );
		}

		if ( _console_lines.get_length() == _console_lines.get_capacity() )
		{
			_console_lines.pop_front();
		}
		console_line_c & console_line = _console_lines.emplace_back();
		console_line.type = type;
		console_line.message = message;
	}

	boolean_c engine_c::debug_c::get_statistics_is_showing()
	{
		return _statistics_is_showing;
	}

	void_c engine_c::debug_c::set_statistics_is_showing( boolean_c value )
	{
		_statistics_is_showing = value;
	}

	boolean_c engine_c::debug_c::get_console_is_showing()
	{
		return _console_is_showing;
	}

	void_c engine_c::debug_c::set_console_is_showing( boolean_c value )
	{
		_console_is_showing = value;
	}

	boolean_c engine_c::_load_settings_ini()
	{
		string16_c file_path;
		file_path += global_engine_instance.interfaces.content_manager->get_engine_data_folder_path();
		file_path += defined_settings_file_name;
		data_stream_file_c stream;
		if ( !stream.open( file_path, data_stream_mode_e_read ) )
		{
			return false;
		}
		if ( !_settings_ini.load( &stream ) )
		{
			return false;
		}
		return true;
	}

	boolean_c engine_c::_save_settings_ini()
	{
		string16_c file_path;
		file_path += global_engine_instance.interfaces.content_manager->get_engine_data_folder_path();
		file_path += defined_settings_file_name;
		data_stream_file_c stream;
		if ( !stream.open( file_path, data_stream_mode_e_write ) )
		{
			return false;
		}
		if ( !_settings_ini.save( &stream ) )
		{
			return false;
		}  
		return true;
	}

	void_c engine_c::_open_client_window()
	{
		WNDCLASSEX window_class_ex = { 0 };
		window_class_ex.cbSize = sizeof( WNDCLASSEX );
		window_class_ex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class_ex.lpfnWndProc = reinterpret_cast< WNDPROC >( engine_members_c::WndProc2 );
		window_class_ex.hInstance = _members->process_handle;
		window_class_ex.hCursor = LoadCursor( NULL, IDC_ARROW );
		window_class_ex.hbrBackground = reinterpret_cast< HBRUSH >( GetStockObject( COLOR_WINDOW + 1 ) );
		window_class_ex.lpszClassName = L"cheonsa";
		cheonsa_assert( SUCCEEDED( RegisterClassEx( &window_class_ex ) ) );

		//EnumDisplayMonitors( NULL, NULL, engine_members_c::monitor_enumeration_process, reinterpret_cast< LPARAM >( _members ) );
		//engine_members_c::monitor_c & monitor = _members->monitors[ 0 ]; // default to primary display, at index 0.
		//environment.window_size[ 0 ] = static_cast< sint32_c >( static_cast< float32_c >( monitor.rectangle[ 2 ] - monitor.rectangle[ 0 ] ) * 0.95f );
		//environment.window_size[ 1 ] = static_cast< sint32_c >( static_cast< float32_c >( monitor.rectangle[ 3 ] - monitor.rectangle[ 1 ] ) * 0.75f );
		//environment.window_position[ 0 ] = ( ( monitor.rectangle[ 2 ] - monitor.rectangle[ 0 ] ) / 2 ) - ( environment.window_size[ 0 ] / 2 );
		//environment.window_position[ 1 ] = ( ( monitor.rectangle[ 3 ] - monitor.rectangle[ 1 ] ) / 2 ) - ( environment.window_size[ 1 ] / 2 );

		_members->window_handle = CreateWindow( L"cheonsa", _game_name.character_list.get_internal_array(), cheonsa_window_style_borderless, 0, 0, window_width_minimum, window_height_minimum, NULL, NULL, _members->process_handle, nullptr );
		cheonsa_assert( _members->window_handle );
		SetWindowLongPtr( _members->window_handle, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( this ) ); // associate our global game engine instance with the window.

		//SetWindowLongPtr( _members->window_handle, GWL_STYLE, cheonsa_window_style_borderless );
		//const MARGINS shadow_off = { 0, 0, 0, 0 };
		//DwmExtendFrameIntoClientArea( _members->window_handle, &shadow_off );
		//SetWindowPos( _members->window_handle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE );
		//ShowWindow( _members->window_handle, SW_SHOW );

		_center_client_window();

#if defined( cheonsa_platform_windows )
		// register mouse for raw input so we can get reliable mouse deltas.
		// this enables us to receive WM_INPUT messages.
		RAWINPUTDEVICE raw_input_device_array[ 1 ];
		raw_input_device_array[ 0 ].usUsagePage = 0x01;
		raw_input_device_array[ 0 ].usUsage = 0x02;
		raw_input_device_array[ 0 ].dwFlags = 0;
		raw_input_device_array[ 0 ].hwndTarget = reinterpret_cast< engine_members_c * >( _members )->window_handle;
		cheonsa_assert( RegisterRawInputDevices( raw_input_device_array, 1, sizeof( RAWINPUTDEVICE ) ) );
#endif
	}

	void_c engine_c::_center_client_window()
	{
		RECT work_area_rectangle;
		if ( SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area_rectangle, 0 ) )
		{
			RECT window_rectangle;
			if ( GetWindowRect( _members->window_handle, &window_rectangle ) )
			{
				RECT window_rectangle_centered;
				window_rectangle_centered.left = ( ( work_area_rectangle.right - work_area_rectangle.left ) / 2 ) - ( ( window_rectangle.right - window_rectangle.left ) / 2 );
				window_rectangle_centered.top = ( ( work_area_rectangle.bottom - work_area_rectangle.top ) / 2 ) - ( ( window_rectangle.bottom - window_rectangle.top ) / 2 );
				window_rectangle_centered.right = window_rectangle_centered.left + ( window_rectangle.right - window_rectangle.left );
				window_rectangle_centered.bottom = window_rectangle_centered.top + ( window_rectangle.bottom - window_rectangle.top );
				SetWindowPos( _members->window_handle, 0, window_rectangle_centered.left, window_rectangle_centered.top, window_rectangle_centered.right - window_rectangle_centered.left, window_rectangle_centered.bottom - window_rectangle_centered.top, SWP_FRAMECHANGED | SWP_NOZORDER );
				ShowWindow( _members->window_handle, SW_SHOW );
			}

			//RECT client_rectangle;
			//if ( GetClientRect( _members->window_handle, &client_rectangle ) )
			//{
			//	RECT window_rect;
			//	window_rect.left = static_cast< LONG >( ( ( work_area.left + work_area.right ) / 2 ) - ( ( client_rectangle.right - client_rectangle.left ) / 2 ) );
			//	window_rect.top = static_cast< LONG >( ( ( work_area.top + work_area.bottom ) / 2 ) - ( ( client_rectangle.bottom - client_rectangle.top ) / 2 ) );
			//	window_rect.right = static_cast< LONG >( window_rect.left + ( client_rectangle.right - client_rectangle.left ) );
			//	window_rect.bottom = static_cast< LONG >( window_rect.top + ( client_rectangle.bottom - client_rectangle.top ) );
			//	RECT client_rectangle = window_rect;
			//	if ( AdjustWindowRect( &window_rect, cheonsa_window_style_borderless, FALSE ) )
			//	{
			//		if ( window_rect.top < 0 )
			//		{
			//			window_rect.bottom -= window_rect.top;
			//			window_rect.top = 0;
			//		}

			//		MoveWindow( _members->window_handle, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, TRUE );
			//		//SetWindowPos( _members->window_handle, 0, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, SWP_FRAMECHANGED | SWP_NOZORDER );
			//		//environment.window_state = environment_c::window_state_e_normaled; // i think this will be set appropriately after ShowWindow is called because of the message that our message pump will receive and process.
			//		ShowWindow( _members->window_handle, SW_SHOW );
			//	}
			//}
		}
	}

	void_c engine_c::_close_client_window()
	{
		DestroyWindow( _members->window_handle );
		_members->window_handle = 0;
	}

	void_c engine_c::_update( float32_c time_step )
	{
		interfaces.input_manager->update(); // update input states.
		interfaces.user_interface->update( time_step ); // respond to user input and update animations of active scene and menus.
		interfaces.game->update( time_step );
	}

	void_c engine_c::_render( float32_c time_step )
	{
		interfaces.user_interface->render_and_present( time_step );
	}

	engine_c::engine_c()
		: interfaces()
		, environment()
		, debug()
		, _members( nullptr )
		, _is_running( true )
	{
		cheonsa_assert( ops::memory_is_zero( &interfaces, sizeof( interfaces ) ) );
		ops::memory_zero( &interfaces, sizeof( interfaces ) );

		_members = new engine_members_c( this );

		network_start();
	}

	engine_c::~engine_c()
	{
	}
	
	boolean_c engine_c::start( char16_c const * const * arguments, sint32_c arguments_count, game_c * game )
	{
		cheonsa_assert( game != nullptr );
		
#if defined( cheonsa_platform_windows )
		// we could have passed this in, but we can also just ask windows for it.
		_members->process_handle = reinterpret_cast< HINSTANCE >( GetModuleHandle( NULL ) );

		// resolved logical processor count.
		SYSTEM_INFO system_info;
		GetSystemInfo( &system_info );
		environment._logical_processor_count = system_info.dwNumberOfProcessors;

		// resolve executable path, executable folder path, and executable file name.
		char16_c module_file_path[ MAX_PATH + 1 ];
		GetModuleFileNameW( NULL, module_file_path, MAX_PATH + 1 );
		module_file_path[ MAX_PATH ] = 0;
		string16_c executable_file_path;
		ops::path_format_for_cheonsa( string16_c( mode_e_static, module_file_path ), executable_file_path, false );
		environment._executable_folder_path = ops::path_get_mother( executable_file_path );
		environment._executable_file_name = ops::path_get_file_name( executable_file_path );
		
		// open debug log file.
		debug._start();

		// base data folder path is the folder that also contains a sub-folder named "engine/", and which may also contain a sub-folder named "game/".
		string16_c engine_data_folder_path;

		// default to "data/engine/" folder in executable folder path.
		engine_data_folder_path += environment._executable_folder_path;
		engine_data_folder_path += "data/engine/";

		// if data folder is not in executable folder, then assume that running out of "x64/Debug/", so go up a few folders and dig down again.
#if defined( _DEBUG )
		if ( !ops::data_does_folder_exist( engine_data_folder_path ) )
		{
			engine_data_folder_path = environment._executable_folder_path;
			engine_data_folder_path = ops::path_get_mother( engine_data_folder_path );
			engine_data_folder_path = ops::path_get_mother( engine_data_folder_path );
			engine_data_folder_path = ops::path_get_mother( engine_data_folder_path );
			engine_data_folder_path += "data/engine/";
		}
#endif
		
		// check for command line argument to override engine data folder path.
		// windows does not like trailing back slashes at the end of folder paths.
		// note that windows mangles the input because it tries to be helpful.
		// but windows' design is in conflict with itself because the back slash is used as a path node delimiter and is also used as an escape character when the parser is in "in quotes" mode.
		// passing a literal argument of [-data="C:\Data\"] will result in the program receiving the argument as [-data=C:\Data"] instead.
		char8_c const * arg_data = "-data=";
		sint32_c const arg_data_length = ops::string8_find_length( arg_data );
		for ( sint32_c i = 1; i < arguments_count; i++ ) // start at 1 to skip the first arg, it's just the file path of the exe.
		{
			string16_c arg = string16_c( mode_e_static, arguments[ i ] );
			if ( ops::string16_starts_with( arg.character_list.get_internal_array(), arg_data ) )
			{
				// extract value. note that if the argument contained a file path that was enclosed by double quotes, windows will remove those quotes before giving it to us.
				string16_c value = ops::string16_sub_string( arg, arg_data_length, arg.get_length() - arg_data_length );
				ops::path_format_for_cheonsa( value, engine_data_folder_path, true );
			}
		}
#else
		#error not implemented.
#endif

		// determine if engine data folder path is valid.
		if ( !ops::data_does_folder_exist( engine_data_folder_path ) )
		{
			string16_c message;
			message += "engine_data_folder_path does not exist:\n";
			message += "\"";
			message += engine_data_folder_path;
			message += "\"";
			cheonsa_annoy( L"error", message.character_list.get_internal_array() );
			return false;
		}

		// start sub-systems.
		// this has to be done in a particular order because of how there are interdependencies between them.
		const_cast< content_manager_c * >( interfaces.content_manager ) = new content_manager_c();
		if ( !interfaces.content_manager->start( engine_data_folder_path ) )
		{
			cheonsa_annoy( L"error", L"content_manager_c::start() failed." );
			goto clean_up;
		}

		_load_settings_ini(); // relies on content manager to resolve file path to ini.
		_open_client_window();
		interfaces.content_manager->_load_settings( _settings_ini ); // depends on environment.

		const_cast< resource_manager_c * >( interfaces.resource_manager ) = new resource_manager_c();
		if ( !interfaces.resource_manager->start() )
		{
			cheonsa_annoy( L"error", L"resource_manager_c::start() failed." );
			goto clean_up;
		}

		#if defined( cheonsa_platform_windows )
		const_cast< audio_interface_c * >( interfaces.audio_interface ) = new audio_interface_wave_out_c();
		if ( !interfaces.audio_interface->start() )
		{
			cheonsa_annoy( L"error", L"audio_interface_wave_out_c::start() failed." );
			goto clean_up;
		}
		#endif

		#if defined( cheonsa_video_d3d11 )
		const_cast< video_interface_c * >( interfaces.video_interface ) = new video_interface_d3d11_c();
		if ( !interfaces.video_interface->start() )
		{
			cheonsa_annoy( L"error", L"video_interface_d3d11_c::start() failed." );
			goto clean_up;
		}
		#endif

		const_cast< video_renderer_shader_manager_c * >( interfaces.video_renderer_shader_manager ) = new video_renderer_shader_manager_c();
		if ( !interfaces.video_renderer_shader_manager->start() )
		{
			cheonsa_annoy( L"error", L"video_renderer_shader_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< video_renderer_interface_c * >( interfaces.video_renderer_interface ) = new video_renderer_interface_c();
		if ( !interfaces.video_renderer_interface->start() )
		{
			cheonsa_annoy( L"error", L"video_renderer_interface_c::start() failed." );
			goto clean_up;
		}

		const_cast< glyph_manager_c * >( interfaces.glyph_manager ) = new glyph_manager_c();
		if ( !interfaces.glyph_manager->start() )
		{
			cheonsa_annoy( L"error", L"glyph_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< menu_style_manager_c * >( interfaces.menu_style_manager ) = new menu_style_manager_c();
		if ( !interfaces.menu_style_manager->start() )
		{
			cheonsa_annoy( L"error", L"menu_style_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< input_manager_c * >( interfaces.input_manager ) = new input_manager_c();
		if ( !interfaces.input_manager->start() )
		{
			cheonsa_annoy( L"error", L"input_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< script_manager_c * >( interfaces.script_manager ) = new script_manager_c();
		if ( !interfaces.script_manager->start() )
		{
			cheonsa_annoy( L"error", L"script_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< reflection_manager_c * >( interfaces.reflection_manager ) = new reflection_manager_c();
		if ( !interfaces.reflection_manager->start() )
		{
			cheonsa_annoy( L"error", L"reflection_manager_c::start() failed." );
			goto clean_up;
		}

		const_cast< user_interface_c * >( interfaces.user_interface ) = new user_interface_c();
		if ( !interfaces.user_interface->start( _members->window_handle ) )
		{
			cheonsa_annoy( L"error", L"user_interface_c::start() failed." );
			goto clean_up;
		}
		cheonsa_assert( interfaces.user_interface != nullptr );

		// situate.
		interfaces.content_manager->apply_changes();

		ops::random_initialize();
		interfaces.audio_interface->_load_settings( _settings_ini );
		interfaces.video_renderer_interface->_load_settings( _settings_ini );
		interfaces.glyph_manager->load_from_disk();

		// start the game.
		const_cast< game_c * >( interfaces.game ) = game;
		interfaces.game->start( arguments, arguments_count );

		// the main loop!
		_is_running = true;
		float32_c time_step = 1.0f / 60.0f;
		float64_c timer_frequency = static_cast< float64_c >( ops::time_get_high_resolution_timer_frequency() );
		float64_c time_remaining = 0.0f;
		sint64_c timer_count_last = ops::time_get_high_resolution_timer_count();
		sint64_c timer_count_now = timer_count_last;
		MSG msg;
		PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
		while ( _is_running )
		{
			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				timer_count_now = ops::time_get_high_resolution_timer_count();
				time_remaining += ( static_cast< float64_c >( timer_count_now - timer_count_last ) / timer_frequency );
				timer_count_last = timer_count_now;
				
				boolean_c do_render = false;
				// cap accumulated time to not more than 1 second to compensate for slow machines.
				// if weak machine can't ever catch up, then this compensation results in the game running in slow motion.
				if ( time_remaining > 1.0f )
				{
					time_remaining = 1.0f;
				}
				// update as many times as needed with the fixed time step to keep up with real time passed.
				while ( time_remaining > time_step )
				{
					_update( time_step );
					time_remaining -= time_step;
					do_render = true;
				}
				// render once.
				if ( do_render )
				{
					_render( time_step );
				}
			}
		}

		// stop the game.
		interfaces.game->stop();
		const_cast< game_c * >( interfaces.game ) = nullptr;

		// save settings.
		_settings_ini.reset();
		interfaces.content_manager->_save_settings( _settings_ini );
		interfaces.audio_interface->_save_settings( _settings_ini );
		interfaces.video_renderer_interface->_save_settings( _settings_ini );
		_save_settings_ini();

		// save glyph cache.
		interfaces.glyph_manager->save_to_disk();

	clean_up:
		// stop and delete sub-systems.
		if ( interfaces.user_interface != nullptr )
		{
			delete interfaces.user_interface;
			const_cast< user_interface_c * >( interfaces.user_interface ) = nullptr;
		}

		if ( interfaces.glyph_manager != nullptr )
		{
			delete interfaces.glyph_manager;
			const_cast< glyph_manager_c * >( interfaces.glyph_manager ) = nullptr;
		}

		if ( interfaces.video_renderer_interface != nullptr )
		{
			delete interfaces.video_renderer_interface;
			const_cast< video_renderer_interface_c * >( interfaces.video_renderer_interface ) = nullptr;
		}

		if ( interfaces.video_renderer_shader_manager != nullptr )
		{
			delete interfaces.video_renderer_shader_manager;
			const_cast< video_renderer_shader_manager_c * >( interfaces.video_renderer_shader_manager ) = nullptr;
		}

		if ( interfaces.input_manager != nullptr )
		{
			delete interfaces.input_manager;
			const_cast< input_manager_c * >( interfaces.input_manager ) = nullptr;
		}

		if ( interfaces.menu_style_manager != nullptr )
		{
			delete interfaces.menu_style_manager;
			const_cast< menu_style_manager_c * >( interfaces.menu_style_manager ) = nullptr;
		}

		if ( interfaces.resource_manager != nullptr )
		{
			delete interfaces.resource_manager;
			const_cast< resource_manager_c * >( interfaces.resource_manager ) = nullptr;
		}

		if ( interfaces.script_manager != nullptr )
		{
			delete interfaces.script_manager;
			const_cast< script_manager_c * >( interfaces.script_manager ) = nullptr;
		}

		if ( interfaces.reflection_manager != nullptr )
		{
			delete interfaces.reflection_manager;
			const_cast< reflection_manager_c * >( interfaces.reflection_manager ) = nullptr;
		}

		if ( interfaces.content_manager != nullptr )
		{
			delete interfaces.content_manager;
			const_cast< content_manager_c * >( interfaces.content_manager ) = nullptr;
		}

		if ( interfaces.video_interface != nullptr )
		{
			delete interfaces.video_interface;
			const_cast< video_interface_c * >( interfaces.video_interface ) = nullptr;
		}

		if ( interfaces.audio_interface != nullptr )
		{
			delete interfaces.audio_interface;
			const_cast< audio_interface_c * >( interfaces.audio_interface ) = nullptr;
		}

		debug._stop();

		_close_client_window();

		return true;
	}

	void_c engine_c::stop()
	{
		_is_running = false;
	}

	string16_c const & engine_c::get_game_name() const
	{
		return _game_name;
	}

	void_c engine_c::set_game_name( string16_c const & value )
	{
		_game_name = value;
		if ( _members->window_handle )
		{
			SetWindowTextW( _members->window_handle, _game_name.character_list.get_internal_array() );
		}
	}

}
