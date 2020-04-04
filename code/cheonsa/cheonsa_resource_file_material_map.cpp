#include "cheonsa_resource_file_material_map.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{
	
	boolean_c resource_file_material_map_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		sint32_c data_size = stream->get_size();
		string8_c data;
		data.character_list.construct_mode_dynamic( data_size + 1, data_size + 1 );
		data.character_list[ data_size ] = 0;
		stream->load( data.character_list.get_internal_array(), data_size );

		data_scribe_markup_c markup( '<' );
		markup.parse( data );

		data_scribe_markup_c::node_c const * node = markup.get_node( 0 )->find_tag( "materials" );
		if ( node == nullptr )
		{
			return false;
		}

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "material" )
			{
				thing_c * thing = new thing_c();
				things.insert_at_end( thing );

					
				attribute = sub_node->find_attribute( "target_name" );
				if ( attribute )
				{
					thing->target_name = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "pixel_shader" );
				if ( attribute )
				{
					thing->material.pixel_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->load_pixel_shader( string16_c( attribute->get_value() ) );
				}

				attribute = sub_node->find_attribute( "is_waved" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->material.is_waved );
				}

				attribute = sub_node->find_attribute( "is_masked" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->material.is_masked );
				}

				attribute = sub_node->find_attribute( "is_overlay" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->material.is_overlay );
				}

				attribute = sub_node->find_attribute( "cull_mode" );
				if ( attribute )
				{
					if ( attribute->get_value() == "none" )
					{
						thing->material.cull_mode = video_cull_type_e_none;
					}
					else if ( attribute->get_value() == "front" )
					{
						thing->material.cull_mode = video_cull_type_e_front;
					}
					else if ( attribute->get_value() == "back" )
					{
						thing->material.cull_mode = video_cull_type_e_back;
					}
				}

				attribute = sub_node->find_attribute( "fill_mode" );
				if ( attribute )
				{
					if ( attribute->get_value() == "face" )
					{
						thing->material.fill_mode = video_fill_type_e_face;
					}
					else if ( attribute->get_value() == "edge" )
					{
						thing->material.fill_mode = video_fill_type_e_edge;
					}
					else if ( attribute->get_value() == "point" )
					{
						thing->material.fill_mode = video_fill_type_e_point;
					}
				}

				attribute = sub_node->find_attribute( "blend_type" );
				if ( attribute )
				{
					if ( attribute->get_value() == "set" )
					{
						thing->material.blend_type = video_blend_type_e_set;
					}
					else if ( attribute->get_value() == "mix" )
					{
						thing->material.blend_type = video_blend_type_e_mix;
					}
					else if ( attribute->get_value() == "add" )
					{
						thing->material.blend_type = video_blend_type_e_add;
					}
					else if ( attribute->get_value() == "sub" )
					{
						thing->material.blend_type = video_blend_type_e_sub;
					}
				}

				attribute = sub_node->find_attribute( "color[0]" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->material.colors[ 0 ] );
				}

				attribute = sub_node->find_attribute( "color[1]" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->material.colors[ 1 ] );
				}

				attribute = sub_node->find_attribute( "color[2]" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->material.colors[ 2 ] );
				}

				attribute = sub_node->find_attribute( "color[3]" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->material.colors[ 3 ] );
				}

				attribute = sub_node->find_attribute( "texture[0]" );
				if ( attribute )
				{
					thing->material.textures[ 0 ] = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
				}

				attribute = sub_node->find_attribute( "texture[1]" );
				if ( attribute )
				{
					thing->material.textures[ 1 ] = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
				}

				attribute = sub_node->find_attribute( "texture[2]" );
				if ( attribute )
				{
					thing->material.textures[ 2 ] = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
				}

				attribute = sub_node->find_attribute( "texture[3]" );
				if ( attribute )
				{
					thing->material.textures[ 3 ] = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
				}
			}
			sub_node = sub_node->get_next_sister();
		}

		_is_loaded = true;

		on_load.invoke( this );

		return true;
	}

	void_c resource_file_material_map_c::_unload()
	{
		assert( _is_loaded == true );
		_is_loaded = false;
		on_unload.invoke( this );

		for ( sint32_c i = 0; i < things.get_length(); i++ )
		{
			delete things[ i ];
		}
		things.remove_all();
	}

	resource_file_material_map_c::resource_file_material_map_c()
		: resource_file_c()
		, things()
	{
	}

	resource_file_material_map_c::~resource_file_material_map_c()
	{
		assert( _is_loaded == false );
	}

}
