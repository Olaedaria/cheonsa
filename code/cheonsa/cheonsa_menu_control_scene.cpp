#include "cheonsa_menu_control_scene.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_scene_c::_update_transform_and_layout()
	{
		menu_control_c::_update_transform_and_layout();
		sint32_c width = static_cast< sint32_c >( _local_box.get_width() );
		sint32_c height = static_cast< sint32_c >( _local_box.get_height() );
		if ( _canvas->set_apparent_size( width, height ) )
		{
			_frame_style.states[ 0 ].texture_map[ 0 ] = 0;
			_frame_style.states[ 0 ].texture_map[ 1 ] = 0;
			_frame_style.states[ 0 ].texture_map[ 2 ] = static_cast< sint16_c >( _canvas->get_apparent_width() );
			_frame_style.states[ 0 ].texture_map[ 3 ] = static_cast< sint16_c >( _canvas->get_apparent_height() );
			_canvas_wrapper.set_video_texture( _canvas->get_target_color_final() );
		}
		else
		{
			assert( false );
		}
	}

	menu_control_scene_c::menu_control_scene_c()
		: menu_control_c()
		, _frame_element()
		, _frame_style()
		, _scene( nullptr )
		, _canvas( nullptr )
		, _canvas_wrapper( true )
	{
		_canvas = new video_renderer_canvas_c( true, true, nullptr );

		_canvas_wrapper.set_video_texture( _canvas->get_target_color_final() );

		_frame_style.texture_map_mode = menu_texture_map_mode_e_stretch;
		_frame_style.texture = &_canvas_wrapper;

		_frame_element.set_name( string8_c( "frame", core_list_mode_e_static ) );
		_frame_element.set_override_style( &_frame_style );
		_add_daughter_element( &_frame_element );
	}

	menu_control_scene_c::~menu_control_scene_c()
	{
		_canvas_wrapper.set_video_texture( nullptr );
		delete _canvas;
		_canvas = nullptr;
	}

	scene_c * menu_control_scene_c::get_scene() const
	{
		return _scene;
	}

	void_c menu_control_scene_c::set_scene( scene_c * value )
	{
		_scene = value;
	}

	video_renderer_canvas_c * menu_control_scene_c::get_canvas() const
	{
		return _canvas;
	}

}
