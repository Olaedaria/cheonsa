#include "cheonsa_menu_types.h" 
#include "cheonsa_menu_control.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	string8_c const & menu_shared_color_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_shared_color_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		if ( _key.get_length() > 0 )
		{
			assert( engine_c::get_instance()->get_menu_style_manager() != nullptr );
			_value = engine_c::get_instance()->get_menu_style_manager()->get_shared_colors_map().find_value_else_nullptr( _key );
		}
		else
		{
			_value = nullptr;
		}
	}

	menu_shared_color_c const * menu_shared_color_c::reference_c::get_value() const
	{
		return _value;
	}

	menu_text_entity_c::menu_text_entity_c( sint32_c type_code )
		: type_code( type_code )
	{
	}

	menu_text_entity_sprite_c::menu_text_entity_sprite_c()
		: menu_text_entity_c( type_code_static() )
		, value()
	{
	}

	menu_event_info_c::menu_event_info_c()
		: control( nullptr )
		, event( nullptr )
	{
	}

	menu_event_info_c::menu_event_info_c( menu_control_c * control, input_event_c * event )
		: control( control )
		, event( event )
	{
	}

	menu_draw_list_c::draw_c::draw_c()
		: pixel_shader( nullptr )
		, texture( nullptr )
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
		vertex_list.remove_all();
		index_list.remove_all();
		draw_list.remove_all();
		vertex_base = 0;
		index_base = 0;
	}

	void_c menu_draw_list_c::append_rectangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture )
	{
		// add draw.
		draw_c * draw = draw_list.emplace_at_end();
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
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
			index_list.insert_at_end( base_vertex ); // top left.
			index_list.insert_at_end( base_vertex + 1 ); // bottom left.
			index_list.insert_at_end( base_vertex + 2 ); // top right.
			index_list.insert_at_end( base_vertex + 2 ); // top right.
			index_list.insert_at_end( base_vertex + 1 ); // bottom left.
			index_list.insert_at_end( base_vertex + 3 ); // bottom right.
			base_vertex += 4;
		}

		// add vertices.
		vertex_list.insert_range_at_end( input_vertex_list.get_internal_array(), input_vertex_list.get_length() );
	}

	void_c menu_draw_list_c::append_rectangle( box32x2_c const & box, box32x2_c const & map, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color )
	{
		// add draw.
		draw_c * draw = draw_list.emplace_at_end();
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = 4; // 4 vertices which make up 1 quad.
		draw->index_start = index_list.get_length();
		draw->index_count = 6; // 6 indices which make up 2 triangles which make up 1 quad.

		// add indices.
		assert( index_list.get_length() < 0xFFFF - 6 ); // if this trips then we should try using 32-bit indices.
		uint16_c base_vertex = static_cast< uint16_c >( vertex_list.get_length() );
		index_list.insert_at_end( base_vertex ); // top left
		index_list.insert_at_end( base_vertex + 1 ); // bottom left
		index_list.insert_at_end( base_vertex + 2 ); // top right
		index_list.insert_at_end( base_vertex + 2 ); // top right
		index_list.insert_at_end( base_vertex + 1 ); // bottom left
		index_list.insert_at_end( base_vertex + 3 ); // bottom right

		// add vertices.
		video_renderer_vertex_menu_c * vertex = vertex_list.emplace_at_end(); // top left
		vertex->position = vector32x3_c( box.minimum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.minimum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // bottom left
		vertex->position = vector32x3_c( box.minimum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.maximum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // top right
		vertex->position = vector32x3_c( box.maximum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.minimum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // bottom right
		vertex->position = vector32x3_c( box.maximum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.maximum.b, 0.0f );
		vertex->color = color;
	}

}
