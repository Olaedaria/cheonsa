#include "cheonsa_resource_file_sprite_set.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	boolean_c resource_file_sprite_set_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( stream ) )
		{
			return false;
		}

		core_list_c< data_scribe_markup_c::node_c const * > sprite_tags;
		core_list_c< data_scribe_markup_c::node_c const * > frame_tags;
		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		markup.get_node( 0 )->find_tags( "sprite", sprite_tags );
		_sprite_list.set_length_absolute( sprite_tags.get_length() );
		for ( sint32_c i = 0; i < sprite_tags.get_length(); i++ )
		{
			data_scribe_markup_c::node_c const * sprite_tag = sprite_tags[ i ];
			sprite_c * sprite = &_sprite_list[ i ];

			attribute = sprite_tag->find_attribute( "name" );
			if ( attribute )
			{
				sprite->name = attribute->get_value();
			}

			attribute = sprite_tag->find_attribute( "texture" );
			if ( attribute )
			{
				sprite->texture_resource = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
			}

			attribute = sprite_tag->find_attribute( "font_size" );
			if ( attribute )
			{
				ops::convert_string8_to_float32( attribute->get_value(), sprite->font_size );
				sprite->font_size = ops::math_clamp( sprite->font_size, 1.0f, 1000.0f );
			}

			attribute = sprite_tag->find_attribute( "font_horizontal_advance" );
			if ( attribute )
			{
				ops::convert_string8_to_float32( attribute->get_value(), sprite->font_horizontal_advance );
				sprite->font_horizontal_advance = ops::math_clamp( sprite->font_horizontal_advance, 0.0f, 1000.0f );
			}

			frame_tags.remove_all();
			sprite_tag->find_tags( "frame", frame_tags );
			sprite->frame_list.set_length_absolute( frame_tags.get_length() );
			for ( sint32_c j = 0; j < frame_tags.get_length(); j++ )
			{
				data_scribe_markup_c::node_c const * frame_tag = frame_tags[ j ];
				frame_c * frame = &sprite->frame_list[ j ];
	
				attribute = frame_tag->find_attribute( "box" );
				if ( attribute )
				{
					ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( mode_e_static, frame->box.as_array(), 4 ) );
				}

				attribute = frame_tag->find_attribute( "map" );
				if ( attribute )
				{
					ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( mode_e_static, frame->map.as_array(), 4 ) );
				}

				attribute = frame_tag->find_attribute( "duration" );
				if ( attribute )
				{
					ops::convert_string8_to_float32( attribute->get_value(), frame->duration );
					frame->duration = ops::math_clamp( frame->duration, 0.0f, 1000000.0f );
				}
			}
		}

		_is_loaded = true;
		return true;
	}

	void_c resource_file_sprite_set_c::_unload()
	{
		assert( _is_loaded == true );
		_sprite_list.remove_all();
		_is_loaded = false;
	}

	resource_file_sprite_set_c::resource_file_sprite_set_c()
		: _sprite_list()
	{
	}

	resource_file_sprite_set_c::~resource_file_sprite_set_c()
	{
	}

	resource_file_sprite_set_c::sprite_c const * resource_file_sprite_set_c::find_sprite( string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _sprite_list.get_length(); i++ )
		{
			if ( _sprite_list[ i ].name == name )
			{
				return &_sprite_list[ i ];
			}
		}
		return nullptr;
	}

}
