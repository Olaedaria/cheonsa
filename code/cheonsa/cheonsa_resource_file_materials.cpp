#include "cheonsa_resource_file_materials.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	resource_file_materials_c::thing_c::thing_c()
		: _target_names()
		, _material()
	{
	}

	string8_c const & resource_file_materials_c::thing_c::get_target_names() const
	{
		return _target_names;
	}

	scene_material_c const & resource_file_materials_c::thing_c::get_material() const
	{
		return _material;
	}

	void_c resource_file_materials_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( stream ) )
		{
			return;
		}

		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		data_scribe_markup_c::node_c const * node = markup.get_node( 0 )->find_tag( "materials" );
		if ( node == nullptr )
		{
			return;
		}

		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "material" )
			{
				thing_c * thing = new thing_c();
				_thing_list.insert( -1, thing );
	
				attribute = sub_node->find_attribute( "target_names" );
				if ( attribute )
				{
					thing->_target_names = attribute->get_value();
				}

				attribute = sub_node->find_attribute( "pixel_shader" );
				if ( attribute )
				{
					thing->_material.pixel_shader = engine.get_video_renderer_shader_manager()->load_ps( string16_c( attribute->get_value() ) );
				}

				attribute = sub_node->find_attribute( "is_waved" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->_material.is_waved );
				}

				attribute = sub_node->find_attribute( "is_masked" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->_material.is_masked );
				}

				attribute = sub_node->find_attribute( "is_overlay" );
				if ( attribute )
				{
					ops::convert_string8_to_boolean( attribute->get_value(), thing->_material.is_overlay );
				}

				attribute = sub_node->find_attribute( "cull_mode" );
				if ( attribute )
				{
					if ( attribute->get_value() == "none" )
					{
						thing->_material.cull_mode = video_cull_type_e_none;
					}
					else if ( attribute->get_value() == "front" )
					{
						thing->_material.cull_mode = video_cull_type_e_front;
					}
					else if ( attribute->get_value() == "back" )
					{
						thing->_material.cull_mode = video_cull_type_e_back;
					}
				}

				attribute = sub_node->find_attribute( "fill_mode" );
				if ( attribute )
				{
					if ( attribute->get_value() == "face" )
					{
						thing->_material.fill_mode = video_fill_type_e_face;
					}
					else if ( attribute->get_value() == "edge" )
					{
						thing->_material.fill_mode = video_fill_type_e_edge;
					}
					else if ( attribute->get_value() == "point" )
					{
						thing->_material.fill_mode = video_fill_type_e_point;
					}
				}

				attribute = sub_node->find_attribute( "blend_type" );
				if ( attribute )
				{
					if ( attribute->get_value() == "set" )
					{
						thing->_material.blend_type = video_blend_type_e_set;
					}
					else if ( attribute->get_value() == "mix" )
					{
						thing->_material.blend_type = video_blend_type_e_mix;
					}
					else if ( attribute->get_value() == "add" )
					{
						thing->_material.blend_type = video_blend_type_e_add;
					}
					else if ( attribute->get_value() == "sub" )
					{
						thing->_material.blend_type = video_blend_type_e_sub;
					}
				}

				attribute = sub_node->find_attribute( "color_0" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->_material.colors[ 0 ] );
				}

				attribute = sub_node->find_attribute( "color_1" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->_material.colors[ 1 ] );
				}

				attribute = sub_node->find_attribute( "color_2" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->_material.colors[ 2 ] );
				}

				attribute = sub_node->find_attribute( "color_3" );
				if ( attribute )
				{
					ops::convert_string8_to_rgba( attribute->get_value(), thing->_material.colors[ 3 ] );
				}

				attribute = sub_node->find_attribute( "texture_0" );
				if ( attribute )
				{
					thing->_material.textures[ 0 ] = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
				}

				attribute = sub_node->find_attribute( "texture_1" );
				if ( attribute )
				{
					thing->_material.textures[ 1 ] = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
				}

				attribute = sub_node->find_attribute( "texture_2" );
				if ( attribute )
				{
					thing->_material.textures[ 2 ] = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
				}

				attribute = sub_node->find_attribute( "texture_3" );
				if ( attribute )
				{
					thing->_material.textures[ 3 ] = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
				}
			}
			sub_node = sub_node->get_next_sister();
		}

		_is_loaded = true;

		on_loaded.invoke( this );
	}

	void_c resource_file_materials_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_thing_list.remove_and_delete_all();
	}

	resource_file_materials_c::resource_file_materials_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _thing_list()
	{
	}

	core_list_c< resource_file_materials_c::thing_c * > const & resource_file_materials_c::get_thing_list() const
	{
		return _thing_list;
	}

}
