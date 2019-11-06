#include "cheonsa_resource_file_texture.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	boolean_c resource_file_texture_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		static uint8_c png_file_signature[ 8 ] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
		static uint8_c jpg_file_signature[ 4 ] = { 0xFF, 0xD8, 0xFF, 0xE0 };

		assert( _is_wrapper == false );
		assert( _is_loaded == false );

		// load first 8 bytes, to determine file type.
		uint8_c loaded_file_signature[ 8 ] = {};
		if ( !stream->load( loaded_file_signature, 8 ) )
		{
			return false;
		}

		stream->set_position( stream->get_position() - 8 );
		core_list_c< uint8_c > file_data;
		file_data.construct_mode_dynamic( stream->get_size() - stream->get_position() );
		stream->load( file_data.get_internal_array(), file_data.get_length() );

		if (
			loaded_file_signature[ 0 ] == png_file_signature[ 0 ] &&
			loaded_file_signature[ 1 ] == png_file_signature[ 1 ] &&
			loaded_file_signature[ 2 ] == png_file_signature[ 2 ] &&
			loaded_file_signature[ 3 ] == png_file_signature[ 3 ] &&
			loaded_file_signature[ 4 ] == png_file_signature[ 4 ] &&
			loaded_file_signature[ 5 ] == png_file_signature[ 5 ] &&
			loaded_file_signature[ 6 ] == png_file_signature[ 6 ] &&
			loaded_file_signature[ 7 ] == png_file_signature[ 7 ] )
		{
			image_c image;
			if ( image_load_from_png( file_data, image, nullptr ) )
			{
				create_video_texture_from_image( image, _video_texture );
			}
		}
		else if ( 
			loaded_file_signature[ 0 ] == jpg_file_signature[ 0 ] &&
			loaded_file_signature[ 1 ] == jpg_file_signature[ 1 ] &&
			loaded_file_signature[ 2 ] == jpg_file_signature[ 2 ] &&
			loaded_file_signature[ 3 ] == jpg_file_signature[ 3 ] )
		{
			image_c image;
			if ( image_load_from_jpg( file_data, image ) )
			{
				create_video_texture_from_image( image, _video_texture );
			}
		}
		else
		{
			return false;
		}

		_is_loaded = _video_texture != nullptr;

		if ( _is_loaded )
		{
			on_load.invoke( this );
		}

		return _is_loaded;
	}

	void_c resource_file_texture_c::_unload()
	{
		if ( _is_wrapper )
		{
			return;
		}

		assert( _is_loaded == true );

		_is_loaded = false;

		on_unload.invoke( this );

		if ( _video_texture )
		{
			delete _video_texture;
			_video_texture = nullptr;
		}
	}

	resource_file_texture_c::resource_file_texture_c()
		: resource_file_c()
		, _video_texture( nullptr )
		, _is_wrapper( false )
	{
	}

	resource_file_texture_c::resource_file_texture_c( boolean_c is_wrapper )
		: resource_file_c()
		, _video_texture( nullptr )
		, _is_wrapper( true )
	{
		assert( is_wrapper );
	}

	resource_file_texture_c::~resource_file_texture_c()
	{
		assert( _is_loaded == false );
	}

	video_texture_c * resource_file_texture_c::get_video_texture() const
	{
		return _video_texture;
	}

	void_c resource_file_texture_c::set_video_texture( video_texture_c * video_texture )
	{
		assert( _is_wrapper );
		_video_texture = video_texture;
		_is_loaded = _video_texture != nullptr;
	}

	boolean_c resource_file_texture_c::create_video_texture_from_image(
			image_c const & image,
			video_texture_c * & video_texture
		)
	{
		// validate inputs.
		assert( video_texture == nullptr );
		assert( image.data.get_length() > 0 );
		assert( image.width >= 1 && image.width <= 4096 );
		assert( image.height >= 1 && image.height <= 4096 );
		assert( image.channel_count >= 1 && image.channel_count <= 4 );
		assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );
		assert( image.data.get_length() == image.width * image.height * image.channel_count * ( image.channel_bit_depth / 8 ) );

		// if needed, convert the input image format to a format that is compatible with the gpu.
		sint32_c pixel_count = image.width * image.height;
		video_texture_format_e video_texture_format = video_texture_format_e_none;
		uint32_c video_texture_data_size = 0; // size in bytes of the raw image, if ImageDataConverted is present then this is the size of ImageDataConverted, otherwise it is the size of ImageDataRaw.
		uint8_c * video_texture_data = nullptr; // optional, if present then this contains the image converted to a certain pixel format that is compatible with Direct3D.

		if ( image.channel_count == 1 )
		{
			if ( image.channel_bit_depth == 8 )
			{
				video_texture_format = video_texture_format_e_r8_unorm;
			}
			else if ( image.channel_bit_depth == 16 )
			{
				video_texture_format = video_texture_format_e_r16_unorm;
				video_texture_data_size = image.width * image.height * 2;
			}
			else
			{
				return false;
			}
		}
		else if ( image.channel_count == 2 )
		{
			if ( image.channel_bit_depth == 8 )
			{
				video_texture_format = video_texture_format_e_r8g8_unorm;
				video_texture_data_size = image.width * image.height * 2;
			}
			else if ( image.channel_bit_depth == 16 )
			{
				video_texture_format = video_texture_format_e_r16g16_unorm;
				video_texture_data_size = image.width * image.height * 4;
			}
			else
			{
				return false;
			}
		}
		else if ( image.channel_count == 3 )
		{
			// Direct3D doesn't support RGB textures, so we have to convert it to RGBA.
			if ( image.channel_bit_depth == 8 )
			{
				video_texture_format = video_texture_format_e_r8g8b8a8_unorm;
				video_texture_data_size = image.width * image.height * 4;
				video_texture_data = new uint8_c[ video_texture_data_size ];
				uint8_c const * in_data_pointer = reinterpret_cast< uint8_c const * >( image.data.get_internal_array() );
				uint8_c * out_data_pointer = reinterpret_cast< uint8_c * >( video_texture_data );
				for ( sint32_c i = 0; i < pixel_count; i++ )
				{
					*out_data_pointer++ = *in_data_pointer++;	// r
					*out_data_pointer++ = *in_data_pointer++;	// g
					*out_data_pointer++ = *in_data_pointer++;	// b
					*out_data_pointer++ = 0xFF;					// a
				}
			}
			else if ( image.channel_bit_depth == 16 )
			{
				video_texture_format = video_texture_format_e_r16g16b16a16_unorm;
				video_texture_data_size = image.width * image.height * 8;
				video_texture_data = new uint8_c[ video_texture_data_size ];
				uint16_c const * in_data_pointer = reinterpret_cast< uint16_c const * >( image.data.get_internal_array() );
				uint16_c * out_data_pointer = reinterpret_cast< uint16_c * >( video_texture_data );
				for ( sint32_c i = 0; i < pixel_count; i++ )
				{
					*out_data_pointer++ = *in_data_pointer++;	// r
					*out_data_pointer++ = *in_data_pointer++;	// g
					*out_data_pointer++ = *in_data_pointer++;	// b
					*out_data_pointer++ = 0xFFFF;				// a
				}
			}
			else
			{
				return false;
			}
		}
		else if ( image.channel_count == 4 )
		{
			if ( image.channel_bit_depth == 8 )
			{
				video_texture_format = video_texture_format_e_r8g8b8a8_unorm;
				video_texture_data_size = image.width * image.height * 4;
			}
			else if ( image.channel_bit_depth == 16 )
			{
				video_texture_format = video_texture_format_e_r16g16b16a16_unorm;
				video_texture_data_size = image.width * image.height * 8;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		video_texture = global_engine_instance.interfaces.video_interface->create_texture( video_texture_format, image.width, image.height, 1, 1, video_texture_data ? video_texture_data : image.data.get_internal_array(), video_texture_data ? video_texture_data_size : image.data.get_length(), false, false, false, true );

		if ( video_texture_data )
		{
			delete[] video_texture_data;
			video_texture_data = nullptr;
			video_texture_data_size = 0;
		}

		return video_texture != nullptr;
	}

	boolean_c resource_file_texture_c::get_image_from_video_texture(
		video_texture_c const * video_texture,
		image_c & image
	)
	{
		assert( video_texture != nullptr );
		if ( image.channel_count != 0 || image.channel_bit_depth != 0 )
		{
			assert( image.channel_count >= 1 && image.channel_count <= 4 );
			assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );
		}
		image.width = video_texture->get_width();
		image.height = video_texture->get_height();
		sint32_c image_pixel_count = image.width * image.height;

		image_c video_texture_image;
		video_texture_image.width = image.width;
		video_texture_image.height = image.height;
		video_texture_format_e video_texture_format = video_texture->get_texture_format();
		if ( video_texture_format == video_texture_format_e_r8_unorm )
		{
			video_texture_image.channel_count = 1;
			video_texture_image.channel_bit_depth = 8;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 1;
				image.channel_bit_depth = 8;
			}
		}
		else if ( video_texture_format == video_texture_format_e_r8g8_unorm )
		{
			video_texture_image.channel_count = 2;
			video_texture_image.channel_bit_depth = 8;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 2;
				image.channel_bit_depth = 8;
			}
		}
		else if ( video_texture_format == video_texture_format_e_r8g8b8a8_unorm )
		{
			video_texture_image.channel_count = 4;
			video_texture_image.channel_bit_depth = 8;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 4;
				image.channel_bit_depth = 8;
			}
		}
		else if ( video_texture_format == video_texture_format_e_r16_unorm )
		{
			video_texture_image.channel_count = 1;
			video_texture_image.channel_bit_depth = 16;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 1;
				image.channel_bit_depth = 16;
			}
		}
		else if ( video_texture_format == video_texture_format_e_r16g16_unorm )
		{
			video_texture_image.channel_count = 2;
			video_texture_image.channel_bit_depth = 16;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 2;
				image.channel_bit_depth = 16;
			}
		}
		else if ( video_texture_format == video_texture_format_e_r16g16b16a16_unorm )
		{
			video_texture_image.channel_count = 4;
			video_texture_image.channel_bit_depth = 16;
			if ( image.channel_count == 0 )
			{
				image.channel_count = 4;
				image.channel_bit_depth = 16;
			}
		}
		else
		{
			assert( false ); // not supported.
			return false;
		}
		sint32_c video_texture_pixel_size = video_texture_format_size_get( video_texture_format );
		video_texture_image.data.construct_mode_dynamic_from_array( video_texture_image.data.get_internal_array(), video_texture_image.data.get_length() );

		return image_copy_and_convert( video_texture_image, image );
	}

	
}