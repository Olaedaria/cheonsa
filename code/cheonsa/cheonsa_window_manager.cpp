#include "cheonsa_window_manager.h"
#include "cheonsa_input_types.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )
#include <windows.h>
//#include <dwmapi.h>
#define cheonsa_window_style_normal ( WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN )
#define cheonsa_window_style_borderless ( WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN )
#define cheonsa_window_width_minimum 1440
#define cheonsa_window_height_minimum 810
#endif

namespace cheonsa
{
	
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
		return input_convert_windows_virtual_key_code_to_cheonsa_keyboard_key_code( wParam );
	}

	class window_manager_members_c
	{
	public:
		static LRESULT __stdcall WndProc2( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
		{
			//engine_c * engine = engine_c::get_instance(); //reinterpret_cast< engine_c * >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

			if ( !engine.get_is_running() )
			{
				return DefWindowProc( hWnd, msg, wParam, lParam );
			}

			switch ( msg )
			{
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_keyboard_key_pressed( get_keyboard_key( wParam, lParam ) );
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
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_keyboard_key_released( get_keyboard_key( wParam, lParam ) );
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
					if ( wParam >= 32 && engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_character( static_cast< char16_c >( wParam ), static_cast< uint8_c >( lParam & 0xFFFF ) );
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
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_move( (int)(short)LOWORD( lParam ), (int)(short)HIWORD( lParam ) );
					}
					return 0;
				}

				case WM_MOUSEWHEEL:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_wheel( static_cast< float32_c >( GET_WHEEL_DELTA_WPARAM( wParam ) ) / 120.0f );
					}
					return 0;
				}

				case WM_LBUTTONDOWN:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_pressed( input_mouse_key_e_left );
					}
					return 0;
				}

				case WM_LBUTTONUP:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_released( input_mouse_key_e_left );
					}
					return 0;
				}

				case WM_RBUTTONDOWN:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_pressed( input_mouse_key_e_right );
					}
					return 0;
				}

				case WM_RBUTTONUP:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_released( input_mouse_key_e_right );
					}
					return 0;
				}

				case WM_MBUTTONDOWN:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_pressed( input_mouse_key_e_middle );
					}
					return 0;
				}

				case WM_MBUTTONUP:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_released( input_mouse_key_e_middle );
					}
					return 0;
				}

				case WM_XBUTTONDOWN:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_pressed( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? input_mouse_key_e_extra1 : input_mouse_key_e_extra2 );
					}
					return 0;
				}

				case WM_XBUTTONUP:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_push_mouse_key_released( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? input_mouse_key_e_extra1 : input_mouse_key_e_extra2 );
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
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_release_all_keys();
					}
					return 0;
				}

				case WM_SIZE:
				{
					//RECT window_rectangle;
					//GetClientRect( hWnd, &window_rectangle );
					if ( wParam == SIZE_MAXIMIZED )
					{
						engine.get_window_manager()->_window_state = window_state_e_maximized;
					}
					else if ( wParam == SIZE_MINIMIZED )
					{
						engine.get_window_manager()->_window_state = window_state_e_minimized;
					}
					else if ( wParam == SIZE_RESTORED )
					{
						engine.get_window_manager()->_window_state = window_state_e_normaled;
					}
					//if ( engine->interfaces._game )
					//{
					//	engine->interfaces._game->on_window_state_changed();
					//}
					break;
				}

				case WM_SIZING:
				{
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_release_all_keys();
					}

					LPRECT window_rect = reinterpret_cast< LPRECT >( lParam );

					// adjust constraints for client area
					RECT fixed_window_rect;
					fixed_window_rect.left = 0;
					fixed_window_rect.top = 0;
					fixed_window_rect.right = cheonsa_window_width_minimum;
					fixed_window_rect.bottom = cheonsa_window_height_minimum;
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
					engine.get_window_manager()->_window_is_focused = true;
					return 0;
				}

				case WM_KILLFOCUS:
				{
					engine.get_window_manager()->_window_is_focused = false;
					if ( engine.get_input_manager() )
					{
						engine.get_input_manager()->_release_all_keys();
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
					if ( engine.get_user_interface() )
					{
						menu_non_client_type_e non_client_type = engine.get_user_interface()->perform_non_client_hit_test( vector32x2_c( (float32_c)( x - window_rectangle.left ), (float32_c)( y - window_rectangle.top ) ) );
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

					if ( x >= window_rectangle.left && x < window_rectangle.left + engine.get_window_manager()->get_window_edge_thickness() && y < window_rectangle.bottom && y >= window_rectangle.bottom - engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTBOTTOMLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine.get_window_manager()->get_window_edge_thickness() && y < window_rectangle.bottom && y >= window_rectangle.bottom - engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTBOTTOMRIGHT;
					}
					else if ( x >= window_rectangle.left && x < window_rectangle.left + engine.get_window_manager()->get_window_edge_thickness() && y >= window_rectangle.top && y < window_rectangle.top + engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTTOPLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine.get_window_manager()->get_window_edge_thickness() && y >= window_rectangle.top && y < window_rectangle.top + engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTTOPRIGHT;
					}
					else if ( x >= window_rectangle.left && x < window_rectangle.left + engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTLEFT;
					}
					else if ( x < window_rectangle.right && x >= window_rectangle.right - engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTRIGHT;
					}
					else if ( y < window_rectangle.bottom && y >= window_rectangle.bottom - engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTBOTTOM;
					}
					else if ( y >= window_rectangle.top && y < window_rectangle.top + engine.get_window_manager()->get_window_edge_thickness() )
					{
						result = HTTOP;
					}
					else if ( x >= window_rectangle.left && x <= window_rectangle.right && y >= window_rectangle.top && y < window_rectangle.top + engine.get_window_manager()->get_window_edge_thickness() + engine.get_window_manager()->get_window_title_bar_thickness() + ( engine.get_window_manager()->get_window_state() == window_state_e_maximized ? 8 : 0 ) )
					{
						result = HTCAPTION;
					}

					if ( result != 0 )
					{
						engine.get_user_interface()->let_go_of_mouse_overed();
						return result;
					}

					break;
				}

				case WM_DESTROY:
				{
					engine.stop();
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

	window_manager_c::window_manager_c()
		: _process_handle( 0 )
		, _window_handle( 0 )
		, _window_is_focused( false )
		, _window_edge_thickness( 8 )
		, _window_title_bar_thickness( 30 )
	{
		_process_handle = GetModuleHandle( NULL );
	}

	window_manager_c::~window_manager_c()
	{
		if ( _window_handle != 0 )
		{
			close_client_window();
		}
	}

	boolean_c window_manager_c::open_client_window()
	{
#if defined( cheonsa_platform_windows )
		WNDCLASSEX window_class_ex = { 0 };
		window_class_ex.cbSize = sizeof( WNDCLASSEX );
		window_class_ex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class_ex.lpfnWndProc = &window_manager_members_c::WndProc2;
		window_class_ex.hInstance = static_cast< HINSTANCE >( _process_handle );
		window_class_ex.hCursor = LoadCursor( NULL, IDC_ARROW );
		window_class_ex.hbrBackground = reinterpret_cast< HBRUSH >( GetStockObject( COLOR_WINDOW + 1 ) );
		window_class_ex.lpszClassName = L"cheonsa";
		assert( SUCCEEDED( RegisterClassEx( &window_class_ex ) ) );

		//EnumDisplayMonitors( NULL, NULL, engine_members_c::monitor_enumeration_process, reinterpret_cast< LPARAM >( _members ) );
		//engine_members_c::monitor_c & monitor = _members->monitors[ 0 ]; // default to primary display, at index 0.
		//environment.window_size[ 0 ] = static_cast< sint32_c >( static_cast< float32_c >( monitor.rectangle[ 2 ] - monitor.rectangle[ 0 ] ) * 0.95f );
		//environment.window_size[ 1 ] = static_cast< sint32_c >( static_cast< float32_c >( monitor.rectangle[ 3 ] - monitor.rectangle[ 1 ] ) * 0.75f );
		//environment.window_position[ 0 ] = ( ( monitor.rectangle[ 2 ] - monitor.rectangle[ 0 ] ) / 2 ) - ( environment.window_size[ 0 ] / 2 );
		//environment.window_position[ 1 ] = ( ( monitor.rectangle[ 3 ] - monitor.rectangle[ 1 ] ) / 2 ) - ( environment.window_size[ 1 ] / 2 );
		
		_window_handle = CreateWindow( L"cheonsa", L"cheonsa", cheonsa_window_style_borderless, 0, 0, cheonsa_window_width_minimum, cheonsa_window_height_minimum, NULL, NULL, static_cast< HINSTANCE >( _process_handle ), nullptr );
		assert( _window_handle );
		SetWindowLongPtr( static_cast< HWND >( _window_handle ), GWLP_USERDATA, reinterpret_cast< LONG_PTR >( this ) ); // associate our global game engine instance with the window.

		//SetWindowLongPtr( _members->window_handle, GWL_STYLE, cheonsa_window_style_borderless );
		//const MARGINS shadow_off = { 0, 0, 0, 0 };
		//DwmExtendFrameIntoClientArea( _members->window_handle, &shadow_off );
		//SetWindowPos( _members->window_handle, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE );
		//ShowWindow( _members->window_handle, SW_SHOW );

		//center_client_window();
		maximize_client_window();

		// register mouse for raw input so we can get reliable mouse deltas.
		// this enables us to receive WM_INPUT messages.
		RAWINPUTDEVICE raw_input_device_array[ 1 ];
		raw_input_device_array[ 0 ].usUsagePage = 0x01;
		raw_input_device_array[ 0 ].usUsage = 0x02;
		raw_input_device_array[ 0 ].dwFlags = 0;
		raw_input_device_array[ 0 ].hwndTarget = static_cast< HWND >( _window_handle );
		assert( RegisterRawInputDevices( raw_input_device_array, 1, sizeof( RAWINPUTDEVICE ) ) );

		return true;
#else
#error not implemented.
#endif
	}

	void_c window_manager_c::maximize_client_window()
	{
#if defined( cheonsa_platform_windows )
		ShowWindow( static_cast< HWND >( _window_handle ), SW_SHOWMAXIMIZED );
#else
#error not implemented.
#endif
	}

	void_c window_manager_c::center_client_window()
	{
#if defined( cheonsa_platform_windows )
		RECT work_area_rectangle;
		if ( SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area_rectangle, 0 ) )
		{
			RECT window_rectangle;
			if ( GetWindowRect( static_cast< HWND >( _window_handle ), &window_rectangle ) )
			{
				RECT window_rectangle_centered;
				window_rectangle_centered.left = ( ( work_area_rectangle.right - work_area_rectangle.left ) / 2 ) - ( ( window_rectangle.right - window_rectangle.left ) / 2 );
				window_rectangle_centered.top = ( ( work_area_rectangle.bottom - work_area_rectangle.top ) / 2 ) - ( ( window_rectangle.bottom - window_rectangle.top ) / 2 );
				window_rectangle_centered.right = window_rectangle_centered.left + ( window_rectangle.right - window_rectangle.left );
				window_rectangle_centered.bottom = window_rectangle_centered.top + ( window_rectangle.bottom - window_rectangle.top );
				SetWindowPos( static_cast< HWND >( _window_handle ), 0, window_rectangle_centered.left, window_rectangle_centered.top, window_rectangle_centered.right - window_rectangle_centered.left, window_rectangle_centered.bottom - window_rectangle_centered.top, SWP_FRAMECHANGED | SWP_NOZORDER );
				ShowWindow( static_cast< HWND >( _window_handle ), SW_SHOW );
			}
		}
#else
#error not implemented.
#endif
	}

	void_c window_manager_c::close_client_window()
	{
#if defined( cheonsa_platform_windows )
		if ( _window_handle != 0 )
		{
			DestroyWindow( static_cast< HWND >( _window_handle ) );
			_window_handle = 0;
		}
#else
#error not implemented.
#endif
	}

	void_c * window_manager_c::get_window_handle()
	{
		return _window_handle;
	}

	void_c window_manager_c::set_window_title( string16_c const & value )
	{
#if defined( cheonsa_platform_windows )
		SetWindowTextW( static_cast< HWND >( _window_handle ), value.character_list.get_internal_array() );
#else
#error not implemented.
#endif
	}

	void_c window_manager_c::set_window_icon_big( platform_icon_c * value )
	{
#if defined( cheonsa_platform_windows )
		SendMessage( static_cast< HWND >( _window_handle ), WM_SETICON, ICON_BIG, reinterpret_cast< LPARAM >( value->icon_handle ) );
		//SetClassLong( _members->window_handle, GCL_HICON, (LONG)Icon->IconHandle );
#else
#error not implemented.
#endif
	}

	void_c window_manager_c::set_window_icon_small( platform_icon_c * value )
	{
#if defined( cheonsa_platform_windows )
		SendMessage( static_cast< HWND >( _window_handle ), WM_SETICON, ICON_SMALL, reinterpret_cast< LPARAM >( value->icon_handle ) );
		//SetClassLong( _members->window_handle, GCL_HICON, (LONG)Icon->IconHandle );
#else
#error not implemented.
#endif
	}

	window_state_e window_manager_c::get_window_state() const
	{
		return _window_state;
	}

	void_c window_manager_c::set_window_state( window_state_e value )
	{
#if defined( cheonsa_platform_windows )
		// ShowWindow() should inadvertently set _window_state, when windows sends us a WM_SIZE message to process.
		if ( value == window_state_e_normaled )
		{
			ShowWindow( static_cast< HWND >( _window_handle ), SW_RESTORE );
		}
		else if ( value == window_state_e_maximized )
		{
			ShowWindow( static_cast< HWND >( _window_handle ), SW_MAXIMIZE );
		}
		else if ( value == window_state_e_minimized )
		{
			ShowWindow( static_cast< HWND >( _window_handle ), SW_MINIMIZE );
		}
#else
#error not implemented.
#endif
	}

	sint32_c window_manager_c::get_window_edge_thickness() const
	{
		return _window_edge_thickness;
	}

	sint32_c window_manager_c::get_window_title_bar_thickness() const
	{
		return _window_title_bar_thickness;
	}

}
