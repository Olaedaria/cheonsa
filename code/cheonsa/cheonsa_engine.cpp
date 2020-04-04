//#include <vld.h>

#include "cheonsa_engine.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_ini.h"
#include "cheonsa_data_scribe_text.h"
#include "cheonsa_network.h"
#include "cheonsa_time_date_gregorian.h"
#include "cheonsa_resource_file_font.h" // to manage global glyph cache.
#include "cheonsa__ops.h"

#if defined( cheonsa_platform_windows )
#include <Windows.h>
#endif

namespace cheonsa
{



	engine_c engine_c::_instance; // global instance.



	//
	//
	//
	// engine
	//
	//
	//

	void_c engine_c::_update( float32_c time_step )
	{
		_input_manager->update(); // update input states.
		_user_interface->update( time_step ); // respond to user input and update animations of active scene and menus.
		_game->update( time_step );
	}

	void_c engine_c::_render( float32_c time_step )
	{
		_user_interface->render_and_present( time_step );
	}

	engine_c::engine_c()
		: _game( nullptr )
		, _debug_manager( nullptr )
		, _content_manager( nullptr )
		, _window_manager( nullptr )
		, _resource_manager( nullptr )
		, _audio_interface( nullptr )
		, _video_interface( nullptr )
		, _video_renderer_interface( nullptr )
		, _video_renderer_shader_manager( nullptr )
		, _glyph_manager( nullptr )
		, _menu_style_manager( nullptr )
		, _input_manager( nullptr )
		, _script_manager( nullptr )
		, _reflection_manager( nullptr )
		, _user_interface( nullptr )
		, _is_running( true )
	{
		network_start();
		ops::random_start();
	}

	engine_c::~engine_c()
	{
	}
	
	boolean_c engine_c::start( char16_c const * const * arguments, sint32_c arguments_count, game_c * game )
	{
		assert( game != nullptr );
		
#if defined( cheonsa_platform_windows )
		// resolve executable path, executable folder path, and executable file name.
		//_members->process_handle = reinterpret_cast< HINSTANCE >( GetModuleHandle( NULL ) );
		char16_c module_file_path[ MAX_PATH + 1 ];
		GetModuleFileNameW( NULL, module_file_path, MAX_PATH + 1 );
		module_file_path[ MAX_PATH ] = 0;
		string16_c executable_file_path;
		ops::path_format_for_cheonsa( string16_c( mode_e_static, module_file_path ), executable_file_path, false );
		string16_c executable_folder_path = ops::path_get_mother( executable_file_path );
		string16_c executable_file_name = ops::path_get_file_name( executable_file_path );
		
		//// resolved logical processor count.
		//SYSTEM_INFO system_info;
		//GetSystemInfo( &system_info );
		//environment._logical_processor_count = system_info.dwNumberOfProcessors;

		// base data folder path is the folder that also contains a sub-folder named "engine/", and which may also contain a sub-folder named "game/".
		// default to "data/engine/" folder in executable folder path.
		string16_c engine_data_folder_path;
		engine_data_folder_path += executable_folder_path;
		engine_data_folder_path += "data/engine/";

		// if data folder is not in executable folder, then assume that running out of "x64/Debug/", so go up a few folders and dig down again.
#if defined( _DEBUG )
		if ( !ops::file_system_does_folder_exist( engine_data_folder_path ) )
		{
			engine_data_folder_path = executable_folder_path;
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
		if ( !ops::file_system_does_folder_exist( engine_data_folder_path ) )
		{
			string16_c message;
			message += "engine_data_folder_path does not exist:\n";
			message += "\"";
			message += engine_data_folder_path;
			message += "\"";
			debug_annoy( L"error", message.character_list.get_internal_array() );
			return false;
		}

		// start sub-systems.
		// this has to be done in a particular order because of how there are interdependencies between them.

		// 1. start debug manager.
		_debug_manager = new debug_manager_c();
		if ( !_debug_manager->start( executable_file_path ) )
		{
			debug_annoy( L"error", L"debug_manager_c::start() failed." );
			goto cancel;
		}

		// 2. start window manager.
		_window_manager = new window_manager_c();
		if ( !_window_manager->open_client_window() )
		{
			debug_annoy( L"error", L"window_manager_c::open_client_window() failed." );
			goto cancel;
		}

		// 3. start content manager.
		_content_manager = new content_manager_c();
		if ( !_content_manager->start( engine_data_folder_path, executable_folder_path, executable_file_name ) )
		{
			debug_annoy( L"error", L"content_manager_c::start() failed." );
			goto cancel;
		}

		// 4. start resource manager.
		_resource_manager = new resource_manager_c();
		if ( !_resource_manager->start() )
		{
			debug_annoy( L"error", L"resource_manager_c::start() failed." );
			goto cancel;
		}

		// 6. start audio interface.
		_audio_interface = new audio2_interface_c();
		if ( !_audio_interface->start() )
		{
			debug_annoy( L"error", L"audio2_interface_c::start() failed." );
			goto cancel;
		}

		// 7. start the video interface.
		#if defined( cheonsa_video_d3d11 )
		_video_interface = new video_interface_d3d11_c();
		if ( !_video_interface->start() )
		{
			debug_annoy( L"error", L"video_interface_d3d11_c::start() failed." );
			goto cancel;
		}
		#endif

		// 8. start the video renderer shader manager.
		_video_renderer_shader_manager = new video_renderer_shader_manager_c();
		if ( !_video_renderer_shader_manager->start() )
		{
			debug_annoy( L"error", L"video_renderer_shader_manager_c::start() failed." );
			goto cancel;
		}

		// 9. start the video renderer manager.
		_video_renderer_interface = new video_renderer_interface_c();
		if ( !_video_renderer_interface->start() )
		{
			debug_annoy( L"error", L"video_renderer_interface_c::start() failed." );
			goto cancel;
		}

		// 10. start the glyph manager.
		_glyph_manager = new glyph_manager_c();
		if ( !_glyph_manager->start() )
		{
			debug_annoy( L"error", L"glyph_manager_c::start() failed." );
			goto cancel;
		}

		// 11. start menu style manager.
		_menu_style_manager = new menu_style_manager_c();
		if ( !_menu_style_manager->start() )
		{
			debug_annoy( L"error", L"menu_style_manager_c::start() failed." );
			goto cancel;
		}

		// 12. start input manager.
		_input_manager = new input_manager_c();
		if ( !_input_manager->start() )
		{
			debug_annoy( L"error", L"input_manager_c::start() failed." );
			goto cancel;
		}

		// 13. start script manager.
		_script_manager = new script_manager_c();
		if ( !_script_manager->start() )
		{
			debug_annoy( L"error", L"script_manager_c::start() failed." );
			goto cancel;
		}

		// 14. start reflection manager.
		_reflection_manager = new reflection_manager_c();
		if ( !_reflection_manager->start() )
		{
			debug_annoy( L"error", L"reflection_manager_c::start() failed." );
			goto cancel;
		}

		// 15. start user interface.
		_user_interface = new user_interface_c();
		if ( !_user_interface->start( _window_manager->get_window_handle() ) )
		{
			debug_annoy( L"error", L"user_interface_c::start() failed." );
			goto cancel;
		}

		// situate.
		_content_manager->apply_changes();
		_audio_interface->_load_settings( _content_manager->get_settings_file() );
		_video_renderer_interface->_load_settings( _content_manager->get_settings_file() );
		_glyph_manager->load_from_disk();

		// start the game.
		_game = game;
		_game->start( arguments, arguments_count );

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
		_game->stop();
		_game = nullptr;

		// save settings.
		_content_manager->get_settings_file().reset();
		_content_manager->_save_settings( _content_manager->get_settings_file() );
		_audio_interface->_save_settings( _content_manager->get_settings_file() );
		_video_renderer_interface->_save_settings( _content_manager->get_settings_file() );
		_content_manager->save_settings_file();

		// save glyph cache.
		_glyph_manager->save_to_disk();

	cancel:
		// stop and delete sub-systems.
		if ( _user_interface != nullptr )
		{
			delete _user_interface;
			_user_interface = nullptr;
		}

		if ( _glyph_manager != nullptr )
		{
			delete _glyph_manager;
			_glyph_manager = nullptr;
		}

		if ( _menu_style_manager != nullptr )
		{
			delete _menu_style_manager;
			_menu_style_manager = nullptr;
		}

		if ( _video_renderer_interface != nullptr )
		{
			delete _video_renderer_interface;
			_video_renderer_interface = nullptr;
		}

		if ( _video_renderer_shader_manager != nullptr )
		{
			delete _video_renderer_shader_manager;
			_video_renderer_shader_manager = nullptr;
		}

		if ( _input_manager != nullptr )
		{
			delete _input_manager;
			_input_manager = nullptr;
		}

		if ( _resource_manager != nullptr )
		{
			delete _resource_manager;
			_resource_manager = nullptr;
		}

		if ( _script_manager != nullptr )
		{
			delete _script_manager;
			_script_manager = nullptr;
		}

		if ( _reflection_manager != nullptr )
		{
			delete _reflection_manager;
			_reflection_manager = nullptr;
		}

		if ( _content_manager != nullptr )
		{
			delete _content_manager;
			_content_manager = nullptr;
		}

		if ( _video_interface != nullptr )
		{
			delete _video_interface;
			_video_interface = nullptr;
		}

		if ( _audio_interface != nullptr )
		{
			delete _audio_interface;
			_audio_interface = nullptr;
		}

		if ( _window_manager != nullptr )
		{
			delete _window_manager;
			_window_manager = nullptr;
		}

		if ( _debug_manager != nullptr )
		{
			delete _debug_manager;
			_debug_manager = nullptr;
		}

		return true;
	}

	void_c engine_c::stop()
	{
		_is_running = false;
	}

	boolean_c engine_c::get_is_running() const
	{
		return _is_running;
	}

}
