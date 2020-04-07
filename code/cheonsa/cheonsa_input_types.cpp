#include "cheonsa_input_types.h"
#include "cheonsa_engine.h"
#include <cassert>

namespace cheonsa
{

	input_shortcut_c::input_shortcut_c()
	{
	}

	void_c input_shortcut_c::clear()
	{
		type = type_e_none;
		keyboard.key = input_keyboard_key_e_none;
		keyboard.modifier = input_modifier_flag_e_none;
	}

	void_c input_shortcut_c::set_keyboard_shortcut( input_keyboard_key_e key, input_modifier_flag_e modifier )
	{
		type = type_e_keyboard;
		keyboard.key = key;
		keyboard.modifier = modifier;
	}

	void_c input_shortcut_c::set_mouse_shortcut( input_mouse_key_e key )
	{
		type = type_e_mouse;
		mouse.key = key;
	}

	void_c input_shortcut_c::set_gamepad_shortcut( input_gamepad_key_e key )
	{
		type = type_e_gamepad;
		gamepad.key = key;
	}

	boolean_c input_shortcut_c::operator == ( input_shortcut_c const & other ) const
	{
		if ( type != other.type )
		{
			return false;
		}
		if ( type == type_e_keyboard )
		{
			return keyboard.key == other.keyboard.key && keyboard.modifier == other.keyboard.modifier;
		}
		else if ( type == type_e_mouse )
		{
			return mouse.key == other.mouse.key;
		}
		else if ( type == type_e_gamepad )
		{
			return gamepad.key == other.gamepad.key;
		}
		return false;
	}

	input_action_c::reference_c::reference_c()
		: _key()
		, _value( nullptr )
	{
	}

	void_c input_action_c::reference_c::clear()
	{
		_key = string8_c();
		_value = nullptr;
		on_refreshed.invoke( this );
	}

	string8_c const & input_action_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c input_action_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		_value = engine_c::get_instance()->get_input_manager()->find_action( _key );
		on_refreshed.invoke( this );
	}

	input_action_c const * input_action_c::reference_c::get_value() const
	{
		return _value;
	}

	input_action_c::input_action_c()
		: _key()
		, _context()
		, _name()
		, _description()
	{
	}

	input_action_c::input_action_c( string8_c const & key, string8_c const & context )
		: _key( key )
		, _context( context )
		, _name()
		, _description()
		, _references()
	{
	}

	string8_c const & input_action_c::get_key() const
	{
		return _key;
	}

	string8_c const & input_action_c::get_context() const
	{
		return _context;
	}

	input_shortcut_c & input_action_c::get_shortcut()
	{
		return _shortcut;
	}

	string_c::reference_c & input_action_c::get_name()
	{
		return _name;
	}

	string_c::reference_c & input_action_c::get_description()
	{
		return _description;
	}

	input_event_c::input_event_c()
		: type( type_e_none )
		, time( 0 )
		, mouse_key( input_mouse_key_e_none )
		, mouse_key_state( input_key_state_bit_e_off )
		, mouse_position( 0.0f, 0.0f )
		, mouse_position_delta( 0.0f, 0.0f )
		, mouse_wheel_delta( 0.0f )
		, multi_click_count( 0 )
		, keyboard_key( input_keyboard_key_e_none )
		, keyboard_key_state( input_key_state_bit_e_off )
		, character( 0 )
		, character_repeat_count( 0 )
		, menu_global_mouse_position( 0.0f, 0.0f )
		, modifier_keys_state{}
		, mouse_keys_state{}
		, keyboard_keys_state{}
		, processed( false )
	{
	}

	boolean_c input_event_c::check_modifier_key_states( boolean_c shift, boolean_c ctrl, boolean_c alt ) const
	{
		return ( ( ( modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 ) == shift ) && ( ( ( modifier_keys_state[ input_modifier_key_e_ctrl ] & input_key_state_bit_e_on ) != 0 ) == ctrl ) && ( ( ( modifier_keys_state[ input_modifier_key_e_alt ] & input_key_state_bit_e_on ) != 0 ) == alt );
	}

	input_drag_drop_payload_c::input_drag_drop_payload_c()
		: type()
		, data()
	{
	}

#if defined( cheonsa_platform_windows )

	input_keyboard_key_e input_convert_windows_virtual_key_code_to_cheonsa_keyboard_key_code( sint_native_c windows_virtual_key_code )
	{
		static uint8_c const map[ 256 ] = {
			0,	// 0x00
			0,	// 0x01	left mouse button
			0,	// 0x02 right mouse button
			0,	// 0x03
			0,	// 0x04	middle mouse button
			0,	// 0x05 extra button on mouse, usually mapped to browser backward
			0,	// 0x06 extra button on mouse, usually mapped to browser forward
			0,	// 0x07
			input_keyboard_key_e_back_space, //0x08
			input_keyboard_key_e_tab, //0x09
			0,	// 0x0A
			0,	// 0x0B
			0,	// 0x0C	clear
			input_keyboard_key_e_enter, // 0x0D
			0,	// 0x0E
			0,	// 0x0F
			0, // 0x10 shift
			0, // 0x11 control
			0, // 0x12 alt
			input_keyboard_key_e_pause, // 0x13
			0,	// 0x14	capital
			0,	// 0x15 kana, hangul
			0,	// 0x16
			0,	// 0x17 junja
			0,	// 0x18 hanja
			0,	// 0x19 kanji
			0,	// 0x1A
			input_keyboard_key_e_escape, // 0x1B
			0,	// 0x1C convert
			0,	// 0x1D nonconvert
			0,	// 0x1E accept
			0,	// 0x1F modechange
			input_keyboard_key_e_space, // 0x20
			input_keyboard_key_e_page_up, // 0x21
			input_keyboard_key_e_page_down, // 0x22
			input_keyboard_key_e_end, // 0x23
			input_keyboard_key_e_home, // 0x24
			input_keyboard_key_e_left, // 0x25
			input_keyboard_key_e_up, // 0x26
			input_keyboard_key_e_right, // 0x27
			input_keyboard_key_e_down, // 0x28
			0,	// 0x29 select
			0,	// 0x2A print
			0,	// 0x2B execute
			input_keyboard_key_e_print_screen, // 0x2C
			input_keyboard_key_e_insert, // 0x2D
			input_keyboard_key_e_delete, // 0x2E
			0,		// 0x2F help!
			input_keyboard_key_e_0, // 0x30
			input_keyboard_key_e_1, // 0x31
			input_keyboard_key_e_2, // 0x32
			input_keyboard_key_e_3, // 0x33
			input_keyboard_key_e_4, // 0x34
			input_keyboard_key_e_5, // 0x35
			input_keyboard_key_e_6, // 0x36
			input_keyboard_key_e_7, // 0x37
			input_keyboard_key_e_8, // 0x38
			input_keyboard_key_e_9, // 0x39
			0,	// 0x3A
			0,	// 0x3B
			0,	// 0x3C
			0,	// 0x3D
			0,	// 0x3E
			0,	// 0x3F
			0,	// 0x40
			input_keyboard_key_e_a, // 0x41
			input_keyboard_key_e_b, // 0x42
			input_keyboard_key_e_c, // 0x43
			input_keyboard_key_e_d, // 0x44
			input_keyboard_key_e_e, // 0x45
			input_keyboard_key_e_f, // 0x46
			input_keyboard_key_e_g, // 0x47
			input_keyboard_key_e_h, // 0x48
			input_keyboard_key_e_i, // 0x49
			input_keyboard_key_e_j, // 0x4a
			input_keyboard_key_e_k, // 0x4b
			input_keyboard_key_e_l, // 0x4c
			input_keyboard_key_e_m, // 0x4d
			input_keyboard_key_e_n, // 0x4e
			input_keyboard_key_e_o, // 0x4f
			input_keyboard_key_e_p, // 0x50
			input_keyboard_key_e_q, // 0x51
			input_keyboard_key_e_r, // 0x52
			input_keyboard_key_e_s, // 0x53
			input_keyboard_key_e_t, // 0x54
			input_keyboard_key_e_u, // 0x55
			input_keyboard_key_e_v, // 0x56
			input_keyboard_key_e_w, // 0x57
			input_keyboard_key_e_x, // 0x58
			input_keyboard_key_e_y, // 0x59
			input_keyboard_key_e_z, // 0x5a
			0,	// 0x5B
			0,	// 0x5C
			0,	// 0x5D
			0,	// 0x5E
			0,	// 0x5F
			input_keyboard_key_e_num_pad_0, // 0x60
			input_keyboard_key_e_num_pad_1, // 0x61
			input_keyboard_key_e_num_pad_2, // 0x62
			input_keyboard_key_e_num_pad_3, // 0x63
			input_keyboard_key_e_num_pad_4, // 0x64
			input_keyboard_key_e_num_pad_5, // 0x65
			input_keyboard_key_e_num_pad_6, // 0x66
			input_keyboard_key_e_num_pad_7, // 0x67
			input_keyboard_key_e_num_pad_8, // 0x68
			input_keyboard_key_e_num_pad_9, // 0x69
			input_keyboard_key_e_num_pad_multiply, // 0X6A
			input_keyboard_key_e_num_pad_add, // 0X6B
			input_keyboard_key_e_num_pad_enter, // 0x6C
			input_keyboard_key_e_num_pad_subtract, // 0x6D
			input_keyboard_key_e_num_pad_dot, // 0x6E
			input_keyboard_key_e_num_pad_divide, // 0x6F
			input_keyboard_key_e_f1, // 0x70
			input_keyboard_key_e_f2, // 0x71
			input_keyboard_key_e_f3, // 0x72
			input_keyboard_key_e_f4, // 0x73
			input_keyboard_key_e_f5, // 0x74
			input_keyboard_key_e_f6, // 0x75
			input_keyboard_key_e_f7, // 0x76
			input_keyboard_key_e_f8, // 0x77
			input_keyboard_key_e_f9, // 0x78
			input_keyboard_key_e_f10, // 0x79
			input_keyboard_key_e_f11, // 0x7A
			input_keyboard_key_e_f12, // 0x7B
			0,	// 0x7C	f13
			0,	// 0x7D f14
			0,	// 0x7E f15
			0,	// 0x7F f16
			0,	// 0x80 f17
			0,	// 0x81 f18
			0,	// 0x82 f19
			0,	// 0x83 f20
			0,	// 0x84 f21
			0,	// 0x85 f22
			0,	// 0x86 f23
			0,	// 0x87 f24
			0,	// 0x88
			0,	// 0x89
			0,	// 0x8A
			0,	// 0x8B
			0,	// 0x8C
			0,	// 0x8D
			0,	// 0x8E
			0,	// 0x8F
			input_keyboard_key_e_num_lock, // 0x90
			input_keyboard_key_e_scroll_lock, // 0x91
			0,	// 0x92 num pad equal
			0,	// 0x93
			0,	// 0x94
			0,	// 0x95
			0,	// 0x96
			0,	// 0x97
			0,	// 0x98
			0,	// 0x99
			0,	// 0x9A
			0,	// 0x9B
			0,	// 0x9C
			0,	// 0x9D
			0,	// 0x9E
			0,	// 0x9F
			input_keyboard_key_e_l_shift, // 0XA0
			input_keyboard_key_e_r_shift, // 0XA1
			input_keyboard_key_e_l_ctrl, // 0XA2
			input_keyboard_key_e_r_ctrl, // 0XA3
			input_keyboard_key_e_l_alt, // 0XA4
			input_keyboard_key_e_r_alt, // 0XA5

			input_keyboard_key_e_browser_back, // 0XA6
			input_keyboard_key_e_browser_forward, // 0XA7
			input_keyboard_key_e_browser_refresh, // 0XA8
			input_keyboard_key_e_browser_stop, // 0XA9
			input_keyboard_key_e_browser_search, // 0XAA
			input_keyboard_key_e_browser_favorites, // 0XAB
			input_keyboard_key_e_browser_home, // 0XAC

			input_keyboard_key_e_volume_mute, // 0XAD
			input_keyboard_key_e_volume_down, // 0XAE
			input_keyboard_key_e_volume_up, // 0XAF

			input_keyboard_key_e_media_next, // 0XB0
			input_keyboard_key_e_media_previous, // 0XB1
			input_keyboard_key_e_media_stop, // 0XB2
			input_keyboard_key_e_media_play_pause, // 0XB3

			0,	// 0xB4
			0,	// 0xB5
			0,	// 0xB6
			0,	// 0xB7
			0,	// 0xB8
			0,	// 0xB9
			input_keyboard_key_e_semi_colon, // 0xBA
			0,	// 0xbb
			input_keyboard_key_e_comma, // 0XBC
			input_keyboard_key_e_minus, // 0XBD
			input_keyboard_key_e_period, // 0XBE
			input_keyboard_key_e_slash, // 0XBF

			input_keyboard_key_e_grave, // 0XC0
			0,	// 0xC1
			0,	// 0xC2
			0,	// 0xC3
			0,	// 0xC4
			0,	// 0xC5
			0,	// 0xC6
			0,	// 0xC7
			0,	// 0xC8
			0,	// 0xC9
			0,	// 0xCA
			0,	// 0xCB
			0,	// 0xCC
			0,	// 0xCD
			0,	// 0xCE
			0,	// 0xCF

			0,	// 0xD0
			0,	// 0xD1
			0,	// 0xD2
			0,	// 0xD3
			0,	// 0xD4
			0,	// 0xD5
			0,	// 0xD6
			0,	// 0xD7
			0,	// 0xD8
			0,	// 0xD9
			0,	// 0xDA
			input_keyboard_key_e_l_bracket, // 0XDB
			input_keyboard_key_e_back_slash, // 0XDC
			input_keyboard_key_e_r_bracket, // 0XDD
			input_keyboard_key_e_apostrophe, // 0XDE
			0,	// 0xDF

			0,	// 0xE0
			0,	// 0xE1
			0,	// 0xE2
			0,	// 0xE3
			0,	// 0xE4
			0,	// 0xE5
			0,	// 0xE6
			0,	// 0xE7
			0,	// 0xE8
			0,	// 0xE9
			0,	// 0xEA
			0,	// 0xEB
			0,	// 0xEC
			0,	// 0xED
			0,	// 0xEE
			0,	// 0xEF

			0,	// 0xF0
			0,	// 0xF1
			0,	// 0xF2
			0,	// 0xF3
			0,	// 0xF4
			0,	// 0xF5
			0,	// 0xF6
			0,	// 0xF7
			0,	// 0xF8
			0,	// 0xF9
			0,	// 0xFA
			0,	// 0xFB
			0,	// 0xFC
			0,	// 0xFD
			0	// 0xFE
		};
		assert( windows_virtual_key_code >= 0 && windows_virtual_key_code < 256 );
		return static_cast< input_keyboard_key_e >( map[ windows_virtual_key_code ] );
	}

	input_gamepad_key_e input_convert_xinput_virtual_key_code_to_cheonsa_gamepad_key_code( sint16_c xinput_virtual_key_code )
	{
		static uint8_c const map[ 64 ] = {
			input_gamepad_key_e_a, // 0x5800 VK_PAD_A
			input_gamepad_key_e_b, // 0x5801 VK_PAD_B
			input_gamepad_key_e_x, // 0x5802 VK_PAD_X
			input_gamepad_key_e_y, // 0x5803 VK_PAD_Y
			input_gamepad_key_e_rb, // 0x5804 VK_PAD_RSHOULDER
			input_gamepad_key_e_lb, // 0x5805 VK_PAD_LSHOULDER
			input_gamepad_key_e_lt, // 0x5806 VK_PAD_LTRIGGER
			input_gamepad_key_e_rt, // 0x5807 VK_PAD_RTRIGGER
			0, // 0x5808
			0, // 0x5809
			0, // 0x580A
			0, // 0x580B
			0, // 0x580C
			0, // 0x580D
			0, // 0x580E
			0, // 0x580F

			input_gamepad_key_e_d_up, // 0x5810 VK_PAD_DPAD_UP
			input_gamepad_key_e_d_down, // 0x5811 VK_PAD_DPAD_DOWN
			input_gamepad_key_e_d_left, // 0x5812 VK_PAD_DPAD_LEFT
			input_gamepad_key_e_d_right, // 0x5813 VK_PAD_DPAD_RIGHT
			input_gamepad_key_e_menu, // 0x5814 VK_PAD_START
			input_gamepad_key_e_view, // 0x5815 VK_PAD_BACK
			input_gamepad_key_e_ls, // 0x5816 VK_PAD_LTHUMB_PRESS
			input_gamepad_key_e_rs, // 0x5817 VK_PAD_RTHUMB_PRESS
			0, // 0x5818
			0, // 0x5819
			0, // 0x581A
			0, // 0x581B
			0, // 0x581C
			0, // 0x581D
			0, // 0x581E
			0, // 0x581F

			0, //input_gamepad_key_e_LS_Up, // 0x5820 VK_PAD_LTHUMB_UP
			0, //input_gamepad_key_e_LS_Down, // 0x5821 VK_PAD_LTHUMB_DOWN
			0, //input_gamepad_key_e_LS_Right, // 0x5822 VK_PAD_LTHUMB_RIGHT
			0, //input_gamepad_key_e_LS_Left, // 0x5823 VK_PAD_LTHUMB_LEFT
			0, //input_gamepad_key_e_LS_UpLeft, // 0x5824 VK_PAD_LTHUMB_UPLEFT
			0, //input_gamepad_key_e_LS_UpRight, // 0x5825 VK_PAD_LTHUMB_UPRIGHT
			0, //input_gamepad_key_e_LS_DownRight, // 0x5826 VK_PAD_LTHUMB_DOWNRIGHT
			0, //input_gamepad_key_e_LS_DownLeft, // 0x5827 VK_PAD_LTHUMB_DOWNLEFT
			0, // 0x5828
			0, // 0x5829
			0, // 0x582A
			0, // 0x582B
			0, // 0x582C
			0, // 0x582D
			0, // 0x582E
			0, // 0x582F

			0, //input_gamepad_key_e_RS_Up, // 0x5830 VK_PAD_RTHUMB_UP
			0, //input_gamepad_key_e_RS_Down, // 0x5831 VK_PAD_RTHUMB_DOWN
			0, //input_gamepad_key_e_RS_Right, // 0x5832 VK_PAD_RTHUMB_RIGHT
			0, //input_gamepad_key_e_RS_Left, // 0x5833 VK_PAD_RTHUMB_LEFT
			0, //input_gamepad_key_e_RS_UpLeft, // 0x5834 VK_PAD_RTHUMB_UPLEFT
			0, //input_gamepad_key_e_RS_UpRight, // 0x5835 VK_PAD_RTHUMB_UPRIGHT
			0, //input_gamepad_key_e_RS_DownRight, // 0x5836 VK_PAD_RTHUMB_DOWNRIGHT
			0, //input_gamepad_key_e_RS_DownLeft, // 0x5837 VK_PAD_RTHUMB_DOWNLEFT
			0, // 0x5838
			0, // 0x5839
			0, // 0x583A
			0, // 0x583B
			0, // 0x583C
			0, // 0x583D
			0, // 0x583E
			0, // 0x583F
		};
		assert( xinput_virtual_key_code >= 0 && xinput_virtual_key_code < 64 );
		return static_cast< input_gamepad_key_e >( map[ xinput_virtual_key_code ] );
	}

#endif

}
