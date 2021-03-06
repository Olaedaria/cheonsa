#include "cheonsa_resource_file_sprites.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	resource_file_sprites_c::frame_c::frame_c()
		: _box()
		, _map()
		, _duration( 0.1f )
	{
	}

	box32x2_c const & resource_file_sprites_c::frame_c::get_box() const
	{
		return _box;
	}

	box32x2_c const & resource_file_sprites_c::frame_c::get_map() const
	{
		return _map;
	}

	float32_c resource_file_sprites_c::frame_c::get_duration() const
	{
		return _duration;
	}

	resource_file_sprites_c::sprite_c::sprite_c()
		: _name()
		, _texture_resource()
		, _font_size( 10.0f )
		, _font_horizontal_advance( 10.0f )
		, _frame_list()
	{
	}

	string8_c const & resource_file_sprites_c::sprite_c::get_name() const
	{
		return _name;
	}

	resource_file_texture_c * resource_file_sprites_c::sprite_c::get_texture_resource() const
	{
		return _texture_resource;
	}

	float32_c resource_file_sprites_c::sprite_c::get_font_size() const
	{
		return _font_size;
	}

	float32_c resource_file_sprites_c::sprite_c::get_font_horizontal_advance() const
	{
		return _font_horizontal_advance;
	}

	core_list_c< resource_file_sprites_c::frame_c > const & resource_file_sprites_c::sprite_c::get_frame_list() const
	{
		return _frame_list;
	}

	void_c resource_file_sprites_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( stream ) )
		{
			return;
		}

		data_scribe_markup_c::node_c const * sprites_tag = markup.get_node( 0 )->find_tag( "sprites" );
		core_list_c< data_scribe_markup_c::node_c const * > sprite_tag_list;
		core_list_c< data_scribe_markup_c::node_c const * > frame_tag_list;
		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		if ( !sprites_tag )
		{
			return;
		}

		sprites_tag->find_tags( "sprite", sprite_tag_list );
		_sprite_list.set_length_absolute( sprite_tag_list.get_length() );
		for ( sint32_c i = 0; i < sprite_tag_list.get_length(); i++ )
		{
			data_scribe_markup_c::node_c const * sprite_tag = sprite_tag_list[ i ];
			sprite_c * sprite = &_sprite_list[ i ];

			attribute = sprite_tag->find_attribute( "name" );
			if ( attribute )
			{
				sprite->_name = attribute->get_value();
			}

			attribute = sprite_tag->find_attribute( "texture" );
			if ( attribute )
			{
				sprite->_texture_resource = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
			}

			attribute = sprite_tag->find_attribute( "font_size" );
			if ( attribute )
			{
				ops::convert_string8_to_float32( attribute->get_value(), sprite->_font_size );
				sprite->_font_size = ops::math_clamp( sprite->_font_size, 1.0f, 1000.0f );
			}

			attribute = sprite_tag->find_attribute( "font_horizontal_advance" );
			if ( attribute )
			{
				ops::convert_string8_to_float32( attribute->get_value(), sprite->_font_horizontal_advance );
				sprite->_font_horizontal_advance = ops::math_clamp( sprite->_font_horizontal_advance, 0.0f, 1000.0f );
			}

			frame_tag_list.remove_all();
			sprite_tag->find_tags( "frame", frame_tag_list );
			sprite->_frame_list.set_length_absolute( frame_tag_list.get_length() );
			for ( sint32_c j = 0; j < frame_tag_list.get_length(); j++ )
			{
				data_scribe_markup_c::node_c const * frame_tag = frame_tag_list[ j ];
				frame_c * frame = &sprite->_frame_list[ j ];
	
				attribute = frame_tag->find_attribute( "box" );
				if ( attribute )
				{
					ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, frame->_box.as_array(), 4 ) );
				}

				attribute = frame_tag->find_attribute( "map" );
				if ( attribute )
				{
					ops::convert_string8_to_float32xn( attribute->get_value(), core_list_c< float32_c >( core_list_mode_e_static, frame->_map.as_array(), 4 ) );
				}

				attribute = frame_tag->find_attribute( "duration" );
				if ( attribute )
				{
					ops::convert_string8_to_float32( attribute->get_value(), frame->_duration );
					frame->_duration = ops::math_clamp( frame->_duration, 0.0f, 1000000.0f );
				}
			}
		}

		_is_loaded = true;

		on_loaded.invoke( this );
	}

	void_c resource_file_sprites_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_sprite_list.remove_all();
	}

	resource_file_sprites_c::resource_file_sprites_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _sprite_list()
	{
	}

	resource_file_sprites_c::sprite_c const * resource_file_sprites_c::find_sprite( string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _sprite_list.get_length(); i++ )
		{
			if ( _sprite_list[ i ]._name == name )
			{
				return &_sprite_list[ i ];
			}
		}
		return nullptr;
	}

}
