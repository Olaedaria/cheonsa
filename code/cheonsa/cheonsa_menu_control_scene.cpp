#include "cheonsa_menu_control_scene.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_control_scene_c::_on_input( input_event_c * input_event )
	{
		on_input.invoke( menu_event_information_c( this, input_event ) );
	}

	void_c menu_control_scene_c::_update_transform_and_layout()
	{
		menu_control_c::_update_transform_and_layout();
		sint32_c width = static_cast< sint32_c >( _local_box.get_width() );
		sint32_c height = static_cast< sint32_c >( _local_box.get_height() );
		_canvas->set_apparent_size( width, height );
		if ( _canvas->update() )
		{
			_element_frame_style.state_list[ 0 ].texture_map[ 0 ] = 0;
			_element_frame_style.state_list[ 0 ].texture_map[ 1 ] = 1;
			_element_frame_style.state_list[ 0 ].texture_map[ 2 ] = static_cast< sint16_c >( _canvas->get_apparent_width() );
			_element_frame_style.state_list[ 0 ].texture_map[ 3 ] = static_cast< sint16_c >( _canvas->get_apparent_height() );
			_canvas_wrapper.set_video_texture( _canvas->get_target_color_final() );
		}
	}

	menu_control_scene_c::menu_control_scene_c( string8_c const & name )
		: menu_control_c( name )
		, _element_frame( string8_c( core_list_mode_e_static, "frame" ) )
		, _element_frame_style()
		, _scene( nullptr )
		, _canvas( nullptr )
		, _canvas_wrapper( true )
	{
		_canvas = new video_renderer_canvas_c( true, true, nullptr );

		_canvas_wrapper.set_video_texture( _canvas->get_target_color_final() );

		_element_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_stretch;
		_element_frame_style.texture = &_canvas_wrapper;
		_element_frame.set_override_style( &_element_frame_style );
		_add_daughter_element( &_element_frame );
	}

	menu_control_scene_c::~menu_control_scene_c()
	{
		_canvas_wrapper.set_video_texture( nullptr );
		delete _canvas;
		_canvas = nullptr;
	}

	menu_control_scene_c * menu_control_scene_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_scene_c( name );
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
