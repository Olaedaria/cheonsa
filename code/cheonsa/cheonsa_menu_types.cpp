#include "cheonsa_menu_types.h" 
#include "cheonsa_menu_control.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{


	//
	//
	// menu_color_style_c::reference_c
	//
	//

	core_linked_list_c< menu_color_style_c::reference_c * > menu_color_style_c::reference_c::_global_list;

	menu_color_style_c::reference_c::reference_c()
		: _global_list_node( this )
		, _key()
		, _value( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	menu_color_style_c::reference_c::~reference_c()
	{
		_global_list.remove( &_global_list_node );
	}

	string8_c const & menu_color_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_color_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		resolve();
	}

	menu_color_style_c const * menu_color_style_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_color_style_c::reference_c::resolve()
	{
		menu_color_style_c const * new_value = engine.get_menu_style_manager()->find_color_style( _key );
		if ( _value != new_value )
		{
			_value = new_value;
		}
	}

	void_c menu_color_style_c::reference_c::unresolve()
	{
		_value = nullptr;
	}




	//
	//
	// menu_color_style_c
	//
	//

	menu_color_style_c::menu_color_style_c()
		: key()
		, value( 1.0f, 1.0f, 1.0f, 1.0f )
	{
	}

	void_c menu_color_style_c::reset()
	{
		key = string8_c();
		value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	void_c menu_color_style_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "value" );
		if ( attribute )
		{
			ops::convert_string8_to_rgba( attribute->get_value(), value );
		}
	}

	void_c menu_color_style_c::initialize( string8_c const & key, vector32x4_c const & value )
	{
		this->key = key;
		this->value = value;
	}




	//
	//
	// menu_frame_style_c::state_c
	//
	//

	menu_frame_style_c::state_c::state_c()
	{
		reset();
	}

	void_c menu_frame_style_c::state_c::reset()
	{
		show = true;
		swap_shared_colors = false;
		saturation = 1.0f;
		apparent_scale = 1.0f;
		texture_map[ 0 ] = 0;
		texture_map[ 1 ] = 0;
		texture_map[ 2 ] = 0;
		texture_map[ 3 ] = 0;
		texture_map_edges[ 0 ] = 0;
		texture_map_edges[ 1 ] = 0;
		texture_map_edges[ 2 ] = 0;
		texture_map_edges[ 3 ] = 0;
	}




	//
	//
	// menu_frame_style_c::reference_c
	//
	//

	core_linked_list_c< menu_frame_style_c::reference_c * > menu_frame_style_c::reference_c::_global_list;

	menu_frame_style_c::reference_c::reference_c()
		: _global_list_node( this )
		, _key( core_list_mode_e_static, "" )
		, _value( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	menu_frame_style_c::reference_c::~reference_c()
	{
		_global_list.remove( &_global_list_node );
	}

	string8_c const & menu_frame_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_frame_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		resolve();
	}

	menu_frame_style_c const * menu_frame_style_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_frame_style_c::reference_c::set_value( menu_frame_style_c const * value )
	{
		if ( _value != value )
		{
			if ( _value )
			{
				//on_unloaded.invoke( this );
			}
			_value = value;
			if ( _value )
			{
				_key = _value->key;
				//on_loaded.invoke( this );
			}
			else
			{
				_key = string8_c();
			}
		}
	}

	void_c menu_frame_style_c::reference_c::resolve()
	{
		menu_frame_style_c const * new_value = engine.get_menu_style_manager()->find_frame_style( _key );
		if ( _value != new_value )
		{
			_value = new_value;
		}
	}

	void_c menu_frame_style_c::reference_c::unresolve()
	{
		_value = nullptr;
	}




	//
	//
	// menu_frame_style_c
	//
	//

	menu_frame_style_c::menu_frame_style_c()
		: key()
		, texture( nullptr )
		, texture_map_mode( texture_map_mode_e_stretch )
		, texture_map_fill_middle( true )
		, pixel_shader_reference()
		, state_list()
	{
	}

	void_c menu_frame_style_c::reset()
	{
		key = string8_c();
		texture = nullptr;
		texture_map_mode = texture_map_mode_e_stretch;
		texture_map_fill_middle = true;
		pixel_shader_reference = nullptr;
		for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
		{
			state_list[ i ].reset();
		}
	}

	void_c menu_frame_style_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "texture" );
		if ( attribute )
		{
			texture = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
		}

		boolean_c texture_map_origin_is_defined = false;
		sint16_c texture_map_origin[ 2 ] = {};
		attribute = node->find_attribute( "texture_map_origin" );
		if ( attribute )
		{
			texture_map_origin_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( core_list_mode_e_static, texture_map_origin, 2 ) );
		}

		boolean_c texture_map_edges_is_defined = false;
		sint16_c texture_map_edges[ 4 ] = {};
		attribute = node->find_attribute( "texture_map_edges" );
		if ( attribute )
		{
			texture_map_edges_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( core_list_mode_e_static, texture_map_edges, 4 ) );
		}

		boolean_c texture_map_size_is_defined = false;
		sint16_c texture_map_size[ 2 ] = {};
		attribute = node->find_attribute( "texture_map_size" );
		if ( attribute )
		{
			texture_map_size_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( core_list_mode_e_static, texture_map_size, 2 ) );
		}

		attribute = node->find_attribute( "texture_map_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "stretch" )
			{
				texture_map_mode = texture_map_mode_e_stretch;
			}
			else if ( attribute->get_value() == "scale_to_fit" )
			{
				texture_map_mode = texture_map_mode_e_scale_to_fit;
			}
			else if ( attribute->get_value() == "scale_to_fill" )
			{
				texture_map_mode = texture_map_mode_e_scale_to_fill;
			}
			else if ( attribute->get_value() == "nine_slice_stretch" )
			{
				texture_map_mode = texture_map_mode_e_nine_slice_stretch;
			}
			else if ( attribute->get_value() == "nine_slice_tile" )
			{
				texture_map_mode = texture_map_mode_e_nine_slice_tile;
			}
		}

		attribute = node->find_attribute( "texture_map_fill_middle" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), texture_map_fill_middle );
		}

		attribute = node->find_attribute( "pixel_shader" );
		if ( attribute )
		{
			pixel_shader_reference = engine.get_video_renderer_shader_manager()->load_ps( string16_c( attribute->get_value() ) );
		}

		boolean_c show = true;
		attribute = node->find_attribute( "show" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), show );
		}

		boolean_c swap_shared_colors = false;
		attribute = node->find_attribute( "swap_shared_colors" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), swap_shared_colors );
		}

		// apply defaults if they were defined.
		if ( texture_map_origin_is_defined && texture_map_size_is_defined && texture_map_edges_is_defined )
		{
			for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
			{
				state_c & state = state_list[ i ];
				state.show = show;
				state.swap_shared_colors = swap_shared_colors;
				state.texture_map[ 0 ] = texture_map_origin[ 0 ];
				state.texture_map[ 1 ] = texture_map_origin[ 1 ];
				state.texture_map[ 2 ] = texture_map_origin[ 0 ] + texture_map_size[ 0 ];
				state.texture_map[ 3 ] = texture_map_origin[ 1 ] + texture_map_size[ 1 ];
				state.texture_map_edges[ 0 ] = texture_map_edges[ 0 ];
				state.texture_map_edges[ 1 ] = texture_map_edges[ 1 ];
				state.texture_map_edges[ 2 ] = texture_map_edges[ 2 ];
				state.texture_map_edges[ 3 ] = texture_map_edges[ 3 ];
			}
		}

		sint32_c state_index = 0;
		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "state" )
			{
				// optionally, state tags may define their key (index).
				// otherwise state index starts at 0.
				attribute = sub_node->find_attribute( "key" );
				if ( attribute )
				{
					if ( attribute->get_value() == "normal" )
					{
						state_index = menu_state_e_normal;
					}
					else if ( attribute->get_value() == "selected" )
					{
						state_index = menu_state_e_selected;
					}
					else if ( attribute->get_value() == "pressed" )
					{
						state_index = menu_state_e_pressed;
					}
					else if ( attribute->get_value() == "disabled" )
					{
						state_index = menu_state_e_disabled;
					}
				}

				// if state index is valid, then continue.
				if ( state_index >= 0 && state_index < menu_state_e_count_ )
				{
					state_c & state = state_list[ state_index ];

					state.show = true;

					attribute = sub_node->find_attribute( "show" );
					if ( attribute )
					{
						ops::convert_string8_to_boolean( attribute->get_value(), state.show );
					}

					attribute = sub_node->find_attribute( "swap_shared_colors" );
					if ( attribute )
					{
						ops::convert_string8_to_boolean( attribute->get_value(), state.swap_shared_colors );
					}

					attribute = sub_node->find_attribute( "saturation" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.saturation );
					}

					attribute = sub_node->find_attribute( "apparent_scale" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.apparent_scale );
					}

					attribute = sub_node->find_attribute( "texture_map_origin" );
					if ( attribute )
					{
						sint16_c state_texture_map_origin[ 2 ] = {};
						if ( ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( core_list_mode_e_static, state_texture_map_origin, 2 ) ) )
						{
							texture_map_origin[ 0 ] = state_texture_map_origin[ 0 ];
							texture_map_origin[ 1 ] = state_texture_map_origin[ 1 ];
							state.texture_map[ 0 ] = texture_map_origin[ 0 ];
							state.texture_map[ 1 ] = texture_map_origin[ 1 ];
							state.texture_map[ 2 ] = texture_map_origin[ 0 ] + texture_map_size[ 0 ];
							state.texture_map[ 3 ] = texture_map_origin[ 1 ] + texture_map_size[ 1 ];
						}
					}

					attribute = sub_node->find_attribute( "texture_map_edges" );
					if ( attribute )
					{
						ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( core_list_mode_e_static, state.texture_map_edges, 4 ) );
					}

					// advance to next state index by default.
					state_index++;
				}
			}
			sub_node = sub_node->get_next_sister();
		}
	}




	//
	//
	// menu_text_style_c
	//
	//

	core_linked_list_c< menu_text_style_c::reference_c * > menu_text_style_c::reference_c::_global_list;

	menu_text_style_c::reference_c::reference_c()
		: _global_list_node( this )
		, _key( core_list_mode_e_static, "" )
		, _value( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	menu_text_style_c::reference_c::~reference_c()
	{
		_global_list.remove( &_global_list_node );
	}

	string8_c const & menu_text_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_text_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		resolve();
	}

	menu_text_style_c const * menu_text_style_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_text_style_c::reference_c::set_value( menu_text_style_c const * value )
	{
		if ( _value != value )
		{
			if ( _value )
			{
				on_unresolved.invoke( this );
			}
			_value = value;
			if ( _value )
			{
				_key = _value->key;
				on_resolved.invoke( this );
			}
			else
			{
				_key = string8_c();
			}
		}
	}

	void_c menu_text_style_c::reference_c::resolve()
	{
		menu_text_style_c const * new_value = engine.get_menu_style_manager()->find_text_style( _key );
		if ( _value != new_value )
		{
			if ( _value )
			{
				on_unresolved.invoke( this );
			}
			_value = new_value;
			if ( _value )
			{
				on_resolved.invoke( this );
			}
		}
	}

	void_c menu_text_style_c::reference_c::unresolve()
	{
		if ( _value )
		{
			on_unresolved.invoke( this );
			_value = nullptr;
		}
	}

	menu_text_style_c::state_c::state_c()
	{
		reset();
	}

	void_c menu_text_style_c::state_c::reset()
	{
		show = true;
		swap_shared_colors = false;
		saturation = 1.0f;
		apparent_scale = 1.0f;
	}

	menu_text_style_c::menu_text_style_c()
	{
		reset();
	}

	void_c menu_text_style_c::reset()
	{
		key = string8_c();
		font_is_defined = false;
		font = nullptr;
		size_is_defined = false;
		size = 0;
		color_is_defined = false;
		color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		skew_is_defined = false;
		skew = 0;
		weight_is_defined = false;
		weight = 0;
		softness_is_defined = false;
		softness = 0;
		underline_is_defined = false;
		underline = 0;
		paragraph_spacing_is_defined = false;
		paragraph_spacing = 0.0f;
		line_spacing_is_defined = false;
		line_spacing = 0.0f;
		glyph_spacing_is_defined = false;
		glyph_spacing = 0.0f;
		text_align_horizontal_is_defined = false;
		text_align_horizontal = menu_text_align_horizontal_e_left;
		text_align_vertical_is_defined = false;
		text_align_vertical = menu_text_align_vertical_e_center;
		margin_is_defined = false;
		margin = box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f );
		for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
		{
			state_list[ i ].reset();
		}
	}

	void_c menu_text_style_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "font" );
		if ( attribute )
		{
			font = engine.get_resource_manager()->load_font( string16_c( attribute->get_value() ), true, true );
			font_is_defined = font.get_is_value_set_and_loaded();
		}

		attribute = node->find_attribute( "size" );
		if ( attribute )
		{
			size_is_defined = ops::convert_string8_to_float32( attribute->get_value(), size );
		}

		attribute = node->find_attribute( "color" );
		if ( attribute )
		{
			color_is_defined = ops::convert_string8_to_rgba( attribute->get_value(), color );
		}

		attribute = node->find_attribute( "skew" );
		if ( attribute )
		{
			skew_is_defined = ops::convert_string8_to_float32( attribute->get_value(), skew );
		}

		attribute = node->find_attribute( "weight" );
		if ( attribute )
		{
			weight_is_defined = ops::convert_string8_to_float32( attribute->get_value(), weight );
		}

		attribute = node->find_attribute( "softness" );
		if ( attribute )
		{
			softness_is_defined = ops::convert_string8_to_float32( attribute->get_value(), softness );
		}

		attribute = node->find_attribute( "underline" );
		if ( attribute )
		{
			underline_is_defined = ops::convert_string8_to_float32( attribute->get_value(), underline );
		}

		attribute = node->find_attribute( "paragraph_spacing" );
		if ( attribute )
		{
			paragraph_spacing_is_defined = ops::convert_string8_to_float32( attribute->get_value(), paragraph_spacing );
		}

		attribute = node->find_attribute( "line_spacing" );
		if ( attribute )
		{
			line_spacing_is_defined = ops::convert_string8_to_float32( attribute->get_value(), line_spacing );
		}

		attribute = node->find_attribute( "glyph_spacing" );
		if ( attribute )
		{
			glyph_spacing_is_defined = ops::convert_string8_to_float32( attribute->get_value(), glyph_spacing );
		}

		attribute = node->find_attribute( "align_horizontal" );
		if ( attribute )
		{
			text_align_horizontal_is_defined = true;
			if ( attribute->get_value() == "left" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_left;
			}
			else if ( attribute->get_value() == "center" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_center;
			}
			else if ( attribute->get_value() == "right" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_right;
			}
			else if ( attribute->get_value() == "justify" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_justify;
			}
		}

		attribute = node->find_attribute( "align_vertical" );
		if ( attribute )
		{
			text_align_vertical_is_defined = true;
			if ( attribute->get_value() == "top" )
			{
				text_align_vertical = menu_text_align_vertical_e_top;
			}
			else if ( attribute->get_value() == "center" )
			{
				text_align_vertical = menu_text_align_vertical_e_center;
			}
			else if ( attribute->get_value() == "bottom" )
			{
				text_align_vertical = menu_text_align_vertical_e_bottom;
			}
		}

		attribute = node->find_attribute( "margin" );
		if ( attribute )
		{
			margin_is_defined = ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, margin.as_array(), 4 ) );
		}

		boolean_c show = true;
		attribute = node->find_attribute( "show" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), show );
		}

		boolean_c swap_shared_colors = false;
		attribute = node->find_attribute( "swap_shared_colors" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), swap_shared_colors );
		}

		// apply defaults.
		for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
		{
			state_c & state = state_list[ i ];
			state.show = show;
			state.swap_shared_colors = swap_shared_colors;
		}

		sint32_c state_index = 0;
		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "state" )
			{
				attribute = sub_node->find_attribute( "key" );
				if ( attribute )
				{
					if ( attribute->get_value() == "normal" )
					{
						state_index = 0;
					}
					else if ( attribute->get_value() == "selected" )
					{
						state_index = 1;
					}
					else if ( attribute->get_value() == "pressed" )
					{
						state_index = 2;
					}
					else if ( attribute->get_value() == "disabled" )
					{
						state_index = 3;
					}
				}
				
				if ( state_index < menu_state_e_count_ )
				{
					state_c & state = state_list[ state_index ];

					attribute = sub_node->find_attribute( "show" );
					if ( attribute )
					{
						ops::convert_string8_to_boolean( attribute->get_value(), state.show );
					}

					attribute = sub_node->find_attribute( "swap_shared_colors" );
					if ( attribute )
					{
						ops::convert_string8_to_boolean( attribute->get_value(), state.swap_shared_colors );
					}

					attribute = sub_node->find_attribute( "saturation" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.saturation );
					}

					attribute = sub_node->find_attribute( "apparent_scale" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.apparent_scale );
					}

					state_index++;
				}
			}
			sub_node = sub_node->get_next_sister();
		}
	}




	//
	//
	// menu_style_map_c::reference_c
	//
	//

	core_linked_list_c< menu_style_map_c::reference_c * > menu_style_map_c::reference_c::_global_list;

	menu_style_map_c::reference_c::reference_c()
		: _global_list_node( this )
		, _key()
		, _value( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	menu_style_map_c::reference_c::~reference_c()
	{
		_global_list.remove( &_global_list_node );
	}

	void_c menu_style_map_c::reference_c::clear()
	{
		if ( _value )
		{
			on_unresolved.invoke( this );
			_value = nullptr;
			_key = string8_c();
		}
	}

	string8_c const & menu_style_map_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_style_map_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		resolve();
	}

	menu_style_map_c const * menu_style_map_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_style_map_c::reference_c::set_value( menu_style_map_c const * value )
	{
		if ( _value != value )
		{
			if ( _value )
			{
				on_unresolved.invoke( this );
			}
			_value = value;
			if ( _value )
			{
				_key = _value->get_key();
				on_resolved.invoke( this );
			}
			else
			{
				_key = string8_c();
			}
		}
	}

	void_c menu_style_map_c::reference_c::resolve()
	{
		menu_style_map_c const * new_value = engine.get_menu_style_manager()->find_style_map( _key );
		if ( _value != new_value )
		{
			if ( _value )
			{
				on_unresolved.invoke( this );
			}
			_value = new_value;
			if ( _value )
			{
				on_resolved.invoke( this );
			}
		}
	}

	void_c menu_style_map_c::reference_c::unresolve()
	{
		if ( _value )
		{
			on_unresolved.invoke( this );
			_value = nullptr;
		}
	}




	//
	//
	// menu_style_map_c::entry_c
	//
	//

	menu_style_map_c::entry_c::entry_c()
		: _element_name()
		, _style_key()
	{
	}

	string8_c const & menu_style_map_c::entry_c::get_element_name() const
	{
		return _element_name;
	}

	string8_c const & menu_style_map_c::entry_c::get_style_key() const
	{
		return _style_key;
	}




	//
	//
	// menu_style_map_c
	//
	//

	menu_style_map_c::menu_style_map_c()
		: _key()
		, _entry_list()
		//, property_list()
	{
	}

	menu_style_map_c::~menu_style_map_c()
	{
		_entry_list.remove_and_delete_all();
		//property_list.remove_and_delete_all();
	}

	void_c menu_style_map_c::load( data_scribe_markup_c::node_c const * node )
	{
		data_scribe_markup_c::attribute_c const * attribute;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			_key = attribute->get_value();
		}

		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "item" )
			{
				entry_c * entry = new entry_c();
				_entry_list.insert( -1, entry );

				attribute = sub_node->find_attribute( "element_name" );
				if ( attribute )
				{
					entry->_element_name = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "style_key" );
				if ( attribute )
				{
					entry->_style_key = attribute->get_value();
				}

				//attribute = sub_node->find_attribute( "anchor" );
				//if ( attribute )
				//{
				//	entry->anchor = menu_anchor_e_none;
				//	sint32_c index_of = 0;
				//	if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "left" ), index_of ) )
				//	{
				//		entry->anchor |= menu_anchor_e_left;
				//	}
				//	if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "top" ), index_of ) )
				//	{
				//		entry->anchor |= menu_anchor_e_top;
				//	}
				//	if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "right" ), index_of ) )
				//	{
				//		entry->anchor |= menu_anchor_e_right;
				//	}
				//	if ( ops::string8_find_index_of( attribute->get_value(), string8_c( core_list_mode_e_static, "bottom" ), index_of ) )
				//	{
				//		entry->anchor |= menu_anchor_e_bottom;
				//	}
				//}

				//attribute = sub_node->find_attribute( "anchor_measures" );
				//if ( attribute )
				//{
				//	ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, entry->anchor_measures.as_array(), 4 ) );
				//}
			}
			//else if ( sub_node->get_value() == "property" )
			//{
			//	property_c * property = new property_c();
			//	property_list.insert_at_end( property );

			//	attribute = sub_node->find_attribute( "name" );
			//	if ( attribute )
			//	{
			//		property->_name = attribute->get_value();
			//	}

			//	attribute = sub_node->find_attribute( "value" );
			//	if ( attribute )
			//	{
			//		property->_value = attribute->get_value();
			//	}
			//}
			sub_node = sub_node->get_next_sister();
		}
	}

	string8_c const & menu_style_map_c::get_key() const
	{
		return _key;
	}

	menu_style_map_c::entry_c const * menu_style_map_c::find_entry( string8_c const & element_name ) const
	{
		for ( sint32_c i = 0; i < _entry_list.get_length(); i++ )
		{
			if ( _entry_list[ i ]->_element_name == element_name )
			{
				return _entry_list[ i ];
			}
		}
		return nullptr;
	}

	//menu_style_map_c::property_c const * menu_style_map_c::find_property( string8_c const & name ) const
	//{
	//	for ( sint32_c i = 0; i < property_list.get_length(); i++ )
	//	{
	//		if ( property_list[ i ]->_name == name )
	//		{
	//			return property_list[ i ];
	//		}
	//	}
	//	return nullptr;
	//}

	//boolean_c menu_style_map_c::find_property_as_float32( string8_c const & name, float32_c & property_value_as_float32 ) const
	//{
	//	property_c const * property = find_property( name );
	//	if ( property )
	//	{
	//		return ops::convert_string8_to_float32( property->_value, property_value_as_float32 );
	//	}
	//	return false;
	//}




	//
	//
	// menu_text_glyph_style_c
	//
	//

	menu_text_glyph_style_c::menu_text_glyph_style_c()
		: font( nullptr )
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, size( 1.0f )
		, weight( 0.0f )
		, skew( 0.0f )
		, softness( 0.0f )
	{
	}

	menu_text_glyph_style_c::menu_text_glyph_style_c( menu_text_glyph_style_c const & other )
	{
		*this = other;
	}

	menu_text_glyph_style_c & menu_text_glyph_style_c::operator = ( menu_text_glyph_style_c const & other )
	{
		font = other.font;
		color = other.color;
		size = other.size;
		weight = other.weight;
		skew = other.skew;
		softness = other.softness;
		return *this;
	}

	boolean_c menu_text_glyph_style_c::operator == ( menu_text_glyph_style_c const & other ) const
	{
		return ( font == other.font ) && ( size == other.size ) && ( color == other.color ) && ( skew == other.skew ) && ( weight == other.weight ) && ( softness == other.softness );
	}




	//
	//
	// menu_event_information_c
	//
	//

	menu_event_information_c::menu_event_information_c()
		: control( nullptr )
		, next_control( nullptr )
		, input_event( nullptr )
	{
	}

	menu_event_information_c::menu_event_information_c( menu_control_c * control, menu_control_c * next_control, input_event_c * input_event )
		: control( control )
		, next_control( next_control )
		, input_event( input_event )
	{
	}




	//
	//
	// menu_draw_list_c
	//
	//

	menu_draw_list_c::draw_c::draw_c()
		: pixel_shader( nullptr )
		, texture( nullptr )
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, shared_colors{ vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) }
		, vertex_start( 0 )
		, vertex_count( 0 )
		, index_start( 0 )
		, index_count( 0 )
	{
	}

	menu_draw_list_c::menu_draw_list_c()
		: vertex_list()
		, index_list()
		, draw_list()
		, vertex_base( 0 )
		, index_base( 0 )
	{
	}

	void_c menu_draw_list_c::reset()
	{
		clip_planes[ 0 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, -1.0f );
		clip_planes[ 1 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, -1.0f );
		clip_planes[ 2 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, -1.0f );
		clip_planes[ 3 ] = vector32x4_c( 0.0f, 0.0f, 0.0f, -1.0f );
		vertex_list.remove_all();
		index_list.remove_all();
		draw_list.remove_all();
		vertex_base = 0;
		index_base = 0;
	}

	void_c menu_draw_list_c::set_clip_planes( box32x2_c const box, matrix32x2x2_c const & control_group_basis, vector32x2_c const & control_group_origin )
	{
			vector32x2_c minimum = ops::rotate_and_scale_vector32x2( box.minimum, control_group_basis ) + control_group_origin;
			vector32x2_c maximum = ops::rotate_and_scale_vector32x2( box.maximum, control_group_basis ) + control_group_origin;
			vector32x2_c normal = ops::normal_vector32x2( ops::rotate_and_scale_vector32x2( vector32x2_c( 1.0f, 0.0f ), control_group_basis ) );
			vector32x4_c * plane = &clip_planes[ 0 ];
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::dot_product_float32( normal, minimum );
			plane->d = 0.0f;
			normal = ops::normal_vector32x2( ops::rotate_and_scale_vector32x2( vector32x2_c( 0.0f, 1.0f ), control_group_basis ) );
			plane = &clip_planes[ 1 ];
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::dot_product_float32( normal, minimum );
			plane->d = 0.0f;
			normal = ops::normal_vector32x2( ops::rotate_and_scale_vector32x2( vector32x2_c( -1.0f, 0.0f ), control_group_basis ) );
			plane = &clip_planes[ 2 ];
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::dot_product_float32( normal, maximum );
			plane->d = 0.0f;
			normal = ops::normal_vector32x2( ops::rotate_and_scale_vector32x2( vector32x2_c( 0.0f, -1.0f ), control_group_basis ) );
			plane = &clip_planes[ 3 ];
			plane->a = normal.a;
			plane->b = normal.b;
			plane->c = -ops::dot_product_float32( normal, maximum );
			plane->d = 0.0f;
	}

	void_c menu_draw_list_c::append_rectangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color, vector32x4_c const shared_colors[ 3 ] )
	{
		// add draw.
		draw_c * draw = draw_list.emplace( -1, 1 );
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->color = color;
		if ( shared_colors )
		{
			draw->shared_colors[ 0 ] = shared_colors[ 0 ];
			draw->shared_colors[ 1 ] = shared_colors[ 1 ];
			draw->shared_colors[ 2 ] = shared_colors[ 2 ];
		}
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = input_vertex_list.get_length();
		draw->index_start = index_list.get_length();
		draw->index_count = input_vertex_list.get_length() / 4 * 6;

		// add indices.
		assert( vertex_list.get_length() + 4 < 0xFFFF );
		sint16_c base_vertex = static_cast< sint16_c >( vertex_list.get_length() );
		sint32_c rectangle_count = input_vertex_list.get_length() / 4;
		for ( sint32_c i = 0; i < rectangle_count; i++ )
		{
			index_list.insert( -1, base_vertex ); // top left.
			index_list.insert( -1, base_vertex + 1 ); // bottom left.
			index_list.insert( -1, base_vertex + 2 ); // top right.
			index_list.insert( -1, base_vertex + 2 ); // top right.
			index_list.insert( -1, base_vertex + 1 ); // bottom left.
			index_list.insert( -1, base_vertex + 3 ); // bottom right.
			base_vertex += 4;
		}

		// add vertices.
		vertex_list.insert( -1, input_vertex_list.get_internal_array(), input_vertex_list.get_length() );
	}

	void_c menu_draw_list_c::append_rectangle( box32x2_c const & box, box32x2_c const & map, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color, vector32x4_c const shared_colors[ 3 ] )
	{
		// add draw.
		draw_c * draw = draw_list.emplace( -1, 1 );
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->color = color;
		if ( shared_colors )
		{
			draw->shared_colors[ 0 ] = shared_colors[ 0 ];
			draw->shared_colors[ 1 ] = shared_colors[ 1 ];
			draw->shared_colors[ 2 ] = shared_colors[ 2 ];
		}
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = 4; // 4 vertices which make up 1 quad.
		draw->index_start = index_list.get_length();
		draw->index_count = 6; // 6 indices which make up 2 triangles which make up 1 quad.

		// add indices.
		assert( index_list.get_length() < 0xFFFF - 6 ); // if this trips then we should try using 32-bit indices.
		uint16_c base_vertex = static_cast< uint16_c >( vertex_list.get_length() );
		index_list.insert( -1, base_vertex ); // top left.
		index_list.insert( -1, base_vertex + 1 ); // bottom left.
		index_list.insert( -1, base_vertex + 2 ); // top right.
		index_list.insert( -1, base_vertex + 2 ); // bottom right.
		index_list.insert( -1, base_vertex + 1 ); // top right.
		index_list.insert( -1, base_vertex + 3 ); // bottom left.

		// add vertices.
		video_renderer_vertex_menu_c * vertex = vertex_list.emplace( -1, 1 ); // top left.
		vertex->position = vector32x3_c( box.minimum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.minimum.b, 0.0f );
		vertex->color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		vertex = vertex_list.emplace( -1, 1 ); // bottom left
		vertex->position = vector32x3_c( box.minimum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.maximum.b, 0.0f );
		vertex->color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		vertex = vertex_list.emplace( -1, 1 ); // top right
		vertex->position = vector32x3_c( box.maximum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.minimum.b, 0.0f );
		vertex->color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		vertex = vertex_list.emplace( -1, 1 ); // bottom right
		vertex->position = vector32x3_c( box.maximum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.maximum.b, 0.0f );
		vertex->color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	void_c menu_draw_list_c::append_triangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, core_list_c< uint16_c > const & input_index_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture )
	{
		draw_c * draw = draw_list.emplace( -1, 1 );
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = input_vertex_list.get_length();
		draw->index_start = index_list.get_length();
		draw->index_count = input_index_list.get_length();
		uint16_c base_vertex = static_cast< uint16_c >( vertex_list.get_length() );
		for ( sint32_c i = 0; i < input_index_list.get_length(); i++ )
		{
			index_list.insert( -1, base_vertex + input_index_list[ i ] );
		}
		vertex_list.insert( -1, input_vertex_list.get_internal_array(), input_vertex_list.get_length() );
	}

}
