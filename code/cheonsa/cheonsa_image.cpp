#include "cheonsa_image.h"
#include "cheonsa__ops.h"
#include "cheonsa_data_stream_memory.h"
#include "cheonsa_data_scribe_binary.h"
#include "third_party/jpgd.h"
#include "third_party/jpge.h"
#include "third_party/lodepng.h"
#include <cassert>

namespace cheonsa
{

	uint8_c const image_png_signature[ 8 ] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	image_png_chunk_c::image_png_chunk_c()
		: _type{}
		, _data()
		, _crc( 0 )
	{
	}

	char8_c const * image_png_chunk_c::get_type() const
	{
		return _type;
	}

	void_c image_png_chunk_c::set_type( char8_c const * value )
	{
		_type[ 0 ] = value[ 0 ];
		_type[ 1 ] = value[ 1 ];
		_type[ 2 ] = value[ 2 ];
		_type[ 3 ] = value[ 3 ];
	}

	core_list_c< uint8_c > const & image_png_chunk_c::get_data() const
	{
		return _data;
	}

	core_list_c< uint8_c > & image_png_chunk_c::get_data()
	{
		return _data;
	}

	uint32_c image_png_chunk_c::get_crc() const
	{
		return _crc;
	}

	image_c::image_c()
		: _width( 0 )
		, _height( 0 )
		, _channel_count( 0 )
		, _channel_bit_depth( 0 )
		, _data()
	{
	}

	sint32_c image_c::get_width() const
	{
		return _width;
	}

	void_c image_c::set_width( sint32_c value )
	{
		assert( value >= 0 && value < 8192 );
		_width = value;
	}

	sint32_c image_c::get_height() const
	{
		return _height;
	}

	void_c image_c::set_height( sint32_c value )
	{
		assert( value >= 0 && value < 8192 );
		_height = value;
	}

	sint32_c image_c::get_channel_count() const
	{
		return _channel_count;
	}

	void_c image_c::set_channel_count( sint32_c value )
	{
		assert( value >= 0 && value <= 4 );
		_channel_count = value;
	}

	sint32_c image_c::get_channel_bit_depth() const
	{
		return _channel_bit_depth;
	}

	void_c image_c::set_channel_bit_depth( sint32_c value )
	{
		assert( value == 0 || value == 8 || value == 16 );
		_channel_bit_depth = value;
	}

	core_list_c< uint8_c > const & image_c::get_data() const
	{
		return _data;
	}

	core_list_c< uint8_c > & image_c::get_data()
	{
		return _data;
	}

	boolean_c image_c::copy_and_convert( image_c const & in, image_c & out )
	{
		// validate inputs.
		assert( in._width >= 1 && in._width <= 4096 );
		assert( in._height >= 1 && in._height <= 4096 );
		assert( in._channel_count >= 1 && in._channel_count <= 4 );
		assert( in._channel_bit_depth == 8 || in._channel_bit_depth == 16 );
		assert( in._data.get_length() == in._width * in._height * in._channel_count * ( in._channel_bit_depth / 8 ) );
		assert( out._width == in._width );
		assert( out._height == in._height );
		assert( out._channel_count >= 1 && out._channel_count <= 4 );
		assert( out._channel_bit_depth == 8 || out._channel_bit_depth == 16 );

		// copy and/or convert.
		sint32_c pixel_count = in._width * in._height;
		sint32_c out_data_size = out._width * out._height * out._channel_count * ( out._channel_bit_depth / 8 );
		out._data.construct_mode_dynamic( out_data_size, out_data_size );
		if ( in._channel_count == out._channel_count && in._channel_bit_depth == out._channel_bit_depth )
		{
			assert( in._data.get_length() == out._data.get_length() );
			ops::memory_copy( in._data.get_internal_array(), out._data.get_internal_array(), in._data.get_length() );
			return true;
		}
		else if ( in._channel_bit_depth == 8 && out._channel_bit_depth == 8 )
		{
			uint8_c out_pixel_default[ 4 ] = { 0x00, 0x00, 0x00, 0xFF };
			uint8_c const * in_data_pointer = in._data.get_internal_array();
			uint8_c * out_data_pointer = out._data.get_internal_array();
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out._channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in._channel_count ? in_data_pointer[ j ] : out_pixel_default[ j ];
				}
				in_data_pointer += in._channel_count;
				out_data_pointer += out._channel_count;
			}
			return true;
		}
		else if ( in._channel_bit_depth == 8 && out._channel_bit_depth == 16 )
		{
			uint16_c out_pixel_default[ 4 ] = { 0x0000, 0x0000, 0x0000, 0xFFFF };
			uint8_c const * in_data_pointer = in._data.get_internal_array();
			uint16_c * out_data_pointer = reinterpret_cast< uint16_c * >( out._data.get_internal_array() );
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out._channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in._channel_count ? static_cast< uint16_c >( in_data_pointer[ j ] ) << 8 : out_pixel_default[ j ];
				}
				in_data_pointer += in._channel_count;
				out_data_pointer += out._channel_count;
			}
			return true;
		}
		else if ( in._channel_bit_depth == 16 && out._channel_bit_depth == 8 )
		{
			uint8_c out_pixel_default[ 4 ] = { 0x00, 0x00, 0x00, 0xFF };
			uint16_c const * in_data_pointer = reinterpret_cast< uint16_c const * >( in._data.get_internal_array() );
			uint8_c * out_data_pointer = out._data.get_internal_array();
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out._channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in._channel_count ? static_cast< uint8_c >( in_data_pointer[ j ] >> 8 ) : out_pixel_default[ j ];
				}
				in_data_pointer += in._channel_count;
				out_data_pointer += out._channel_count;
			}
			return true;
		}
		else if ( in._channel_bit_depth == 16 && out._channel_bit_depth == 16 )
		{
			uint16_c out_pixel_default[ 4 ] = { 0x0000, 0x0000, 0x0000, 0xFFFF };
			uint16_c const * in_data_pointer = reinterpret_cast< uint16_c const * >( in._data.get_internal_array() );
			uint16_c * out_data_pointer = reinterpret_cast< uint16_c * >( out._data.get_internal_array() );
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out._channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in._channel_count ? in_data_pointer[ j ] : out_pixel_default[ j ];
				}
				in_data_pointer += in._channel_count;
				out_data_pointer += out._channel_count;
			}
			return true;
		}

		return false;
	}


	boolean_c image_c::load_from_png_just_the_extra_chunks(
		data_stream_c * png_file_stream,
		core_list_c< image_png_chunk_c * > & extra_chunk_list )
	{
		assert( png_file_stream );
		assert( extra_chunk_list.get_length() == 0 ); // we aren't responsible for deleting or forgetting pointers.

		data_stream_c * & stream = png_file_stream;

		uint8_c signature[ 8 ];
		if ( !stream->load( signature, 8 ) )
		{
			return false;
		}
		if ( !ops::memory_compare( signature, image_png_signature, 8 ) )
		{
			return false;
		}

		data_scribe_binary_c scribe;
		scribe.open( stream, byte_order_e_big );
		
		sint32_c stream_size = stream->get_size();
		while ( stream->get_position() + 8 <= stream_size );
		{
			image_png_chunk_c * chunk = new image_png_chunk_c();
			extra_chunk_list.insert( -1, chunk );
			uint32_c chunk_data_size = 0;
			if ( !scribe.load_uint32( chunk_data_size ) )
			{
				return false;
			}
			if ( !scribe.get_stream()->load( chunk->_type, 4 ) )
			{
				return false;
			}
			if ( chunk_data_size > 0 )
			{
				chunk->_data.set_length_absolute( chunk_data_size );
				if ( !stream->load( chunk->_data.get_internal_array(), chunk->_data.get_internal_array_size() ) )
				{
					return false;
				}
			}
			if ( !scribe.load_uint32( chunk->_crc ) )
			{
				return false;
			}
			// we won't verify the crc.
		}

		return true;
	}

	boolean_c image_c::load_from_png(
		core_list_c< uint8_c > const & png_file_data,
		image_c & image,
		core_list_c< image_png_chunk_c * > * extra_chunk_list )
	{
		// validate inputs.
		if ( image._channel_count == 0 || image._channel_bit_depth == 0 )
		{
			assert( image._channel_count == 0 && image._channel_bit_depth == 0 );
		}
		else
		{
			assert( image._channel_count >= 1 && image._channel_count <= 4 );
			assert( image._channel_bit_depth == 8 || image._channel_bit_depth == 16 );
		}

		if ( extra_chunk_list )
		{
			extra_chunk_list->remove_all();
		}

		// decode png into pixels.
		std::vector< unsigned char > decoded_image_data;
		unsigned int decoded_image_width;
		unsigned int decoded_image_height;
		lodepng::State decoded_lode_png_state;
		decoded_lode_png_state.decoder.color_convert = 0; // lodepng does support automatic conversion if we set decoded_lode_png_state.info_raw before calling lodepng::decode(), but i wrote an image class and function to do conversions for us so i don't want to rely on this.
		decoded_lode_png_state.decoder.remember_unknown_chunks = 1;
		unsigned int error = lodepng::decode( decoded_image_data, decoded_image_width, decoded_image_height, decoded_lode_png_state, reinterpret_cast< const unsigned char * >( png_file_data.get_internal_array() ), static_cast< size_t >( png_file_data.get_length() ) );
		if ( error )
		{
			return false;
		}

		// copy extra chunks.
		if ( extra_chunk_list && decoded_lode_png_state.info_png.unknown_chunks_data[ 2 ] )
		{
			unsigned char * extra_chunks_data = decoded_lode_png_state.info_png.unknown_chunks_data[ 2 ];
			uint32_c p = 0;
			while ( p < decoded_lode_png_state.info_png.unknown_chunks_size[ 2 ] )
			{
				if ( p != 0 )
				{
					extra_chunks_data = lodepng::lodepng_chunk_next( extra_chunks_data );
				}
				image_png_chunk_c * chunk = new image_png_chunk_c();
				extra_chunk_list->insert( -1, chunk );
				uint32_c chunk_data_size = lodepng::lodepng_chunk_length( extra_chunks_data );
				ops::memory_copy( &extra_chunks_data[ 4 ], chunk->_type, 4 );
				if ( chunk_data_size > 0 )
				{
					chunk->_data.set_length_absolute( chunk_data_size );
					ops::memory_copy( lodepng::lodepng_chunk_data( extra_chunks_data ), chunk->_data.get_internal_array(), chunk->_data.get_internal_array_size() );
				}
				p += 4 + 4 + chunk_data_size + 4;
			}
		}

		// construct the decoded image, which we may need later if we need to convert to fit the format requested by result.
		image_c decoded_image;
		decoded_image._width = decoded_image_width;
		decoded_image._height = decoded_image_height;
		if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_GREY )
		{
			decoded_image._channel_count = 1;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_GREY_ALPHA )
		{
			decoded_image._channel_count = 2;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_RGB )
		{
			decoded_image._channel_count = 3;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_RGBA )
		{
			decoded_image._channel_count = 4;
		}
		else
		{
			// it shouldn't be possible to reach here.
			return false;
		}
		decoded_image._channel_bit_depth = decoded_lode_png_state.info_png.color.bitdepth;

		// construct the result image.
		image._width = decoded_image_width;
		image._height = decoded_image_height;
		if( image._channel_count == 0 || image._channel_bit_depth == 0 )
		{
			image._channel_count = decoded_image._channel_count;
			image._channel_bit_depth = decoded_image._channel_bit_depth;
			assert( decoded_image_data.size() < constants< sint32_c >::maximum() );
			image._data.construct_mode_dynamic_from_array( &decoded_image_data[ 0 ], static_cast< sint32_c >( decoded_image_data.size() ) );
			return true;
		}
		else
		{
			assert( decoded_image_data.size() < constants< sint32_c >::maximum() );
			decoded_image._data.construct_mode_static_from_array( &decoded_image_data[ 0 ], static_cast< sint32_c >( decoded_image_data.size() ) );
			return image_c::copy_and_convert( decoded_image, image );
		}

		// it shouldn't be possible to reach here.
		return false;
	}

	boolean_c image_c::save_to_png(
		image_c const & image,
		core_list_c< image_png_chunk_c * > const * extra_chunk_list,
		core_list_c< uint8_c > & png_file_data )
	{
		// validate inputs.
		assert( image._width >= 1 && image._width <= 4096 );
		assert( image._height >= 1 && image._height <= 4096 );
		assert( image._channel_count >= 1 && image._channel_count <= 4 );
		assert( image._channel_bit_depth == 8 || image._channel_bit_depth == 16 );

		// initialize a state so we can customize some things.
		lodepng::State lodepng_state;
		lodepng::lodepng_state_init( &lodepng_state );
		lodepng_state.info_raw.bitdepth = lodepng_state.info_png.color.bitdepth = image._channel_bit_depth;

		// define encoding format.
		if ( image._channel_count == 1 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_GREY;
		}
		else if ( image._channel_count == 2 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_GREY_ALPHA;
		}
		else if ( image._channel_count == 3 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_RGB;
		}
		else if ( image._channel_count == 4 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_RGBA;
		}
		
		// add custom chunks to state.
		if ( extra_chunk_list )
		{
			for ( sint32_c i = 0; i < extra_chunk_list->get_length(); i++ )
			{
				image_png_chunk_c const * chunk = (*extra_chunk_list)[ i ];
				lodepng::lodepng_chunk_create( &lodepng_state.info_png.unknown_chunks_data[ 2 ], &lodepng_state.info_png.unknown_chunks_size[ 2 ], chunk->_data.get_internal_array_size(), reinterpret_cast< char const * >( chunk->_type ), chunk->_data.get_internal_array() );
			}
		}

		// convert the image to png format.
		std::vector< unsigned char > png_file_data_std;
		unsigned error = lodepng::encode( png_file_data_std, image._data.get_internal_array(), image._width, image._height, lodepng_state );
		if ( error )
		{
			return false;
		}

		// redundant, copying from std vector to our own container type, this is fine.
		assert( png_file_data_std.size() < constants< sint32_c >::maximum() );
		png_file_data.construct_mode_dynamic_from_array( &png_file_data_std[ 0 ], static_cast< sint32_c >( png_file_data_std.size() ) );

		// we should be good.
		return true;
	}

	boolean_c image_c::load_from_jpg(
		core_list_c< uint8_c > const & jpg_file_data,
		image_c & image
	)
	{
		// validate inputs.
		if ( image._channel_count == 0 || image._channel_bit_depth == 0 )
		{
			assert( image._channel_count == 0 && image._channel_bit_depth == 0 );
		}
		else
		{
			assert( image._channel_count >= 1 && image._channel_count <= 4 );
			assert( image._channel_bit_depth == 8 || image._channel_bit_depth == 16 );
		}

		// decode jpg into pixels.
		int decoded_image_width = 0;
		int decoded_image_height = 0;
		int decoded_image_channel_count = 0;
		unsigned char * decoded_image_data = jpgd::decompress_jpeg_image_from_memory( reinterpret_cast< const unsigned char * >( jpg_file_data.get_internal_array() ), jpg_file_data.get_length(), &decoded_image_width, &decoded_image_height, &decoded_image_channel_count, 3 );
		if ( decoded_image_data == nullptr )
		{
			return false;
		}
		int decoded_image_data_size = decoded_image_width * decoded_image_height * decoded_image_channel_count * 3;

		// construct the decoded image, which we may need later if we need to convert to fit the format requested by result.
		image_c decoded_image;
		decoded_image._width = decoded_image_width;
		decoded_image._height = decoded_image_height;
		if ( decoded_image_channel_count == 1 )
		{
			decoded_image._channel_count = 1;
		}
		else if ( decoded_image_channel_count == 3 )
		{
			decoded_image._channel_count = 3;
		}
		else
		{
			// it shouldn't be possible to reach here.
			free( decoded_image_data );
			return false;
		}
		decoded_image._channel_bit_depth = 8;

		// construct the result image.
		image._width = decoded_image_width;
		image._height = decoded_image_height;
		if( image._channel_count == 0 || image._channel_bit_depth == 0 )
		{
			image._channel_count = decoded_image._channel_count;
			image._channel_bit_depth = decoded_image._channel_bit_depth;
			image._data.construct_mode_dynamic_from_array( decoded_image_data, decoded_image_data_size );
			free( decoded_image_data );
			return true;
		}
		else
		{
			decoded_image._data.construct_mode_static_from_array( decoded_image_data, decoded_image_data_size );
			boolean_c result = image_c::copy_and_convert( decoded_image, image );
			free( decoded_image_data );
			return result;
		}

		// it shouldn't be possible to reach here.
		free( decoded_image_data );
		return false;
	}

	boolean_c image_c::save_to_jpg(
		image_c const & image,
		sint32_c jpg_quality,
		core_list_c< uint8_c > & jpg_file_data
	)
	{
		assert( image._width >= 1 && image._width <= 4096 );
		assert( image._height >= 1 && image._height <= 4096 );
		assert( image._channel_count >= 3 && image._channel_count <= 4 );
		assert( image._channel_bit_depth == 8 || image._channel_bit_depth == 16 );
		assert( jpg_quality >= 1 && jpg_quality <= 100 );

		image_c const * conformant_image = &image;
		image_c stage_image; // will be initialized if needed to hold r8g8b8 image.
		if ( image._channel_bit_depth == 16 || image._channel_count == 4 )
		{
			conformant_image = &stage_image;
			stage_image._width = image._width;
			stage_image._height = image._height;
			stage_image._channel_count = 3;
			stage_image._channel_bit_depth = 8;
			image_c::copy_and_convert( image, stage_image );
		}

		// we can't know the exact size of the buffer that we will need to store the result.
		// but https://en.wikipedia.org/wiki/JPEG#Sample_photographs
		// gives us an idea of how much memory to pre-allocate to store the result.
		jpge::params jpge_params;
		jpge_params.m_quality = jpg_quality;
		int file_data_size = static_cast< sint32_c >( image._width * image._height * 8.25f );
	try_again:
		jpg_file_data.construct_mode_dynamic( file_data_size );
		bool result = jpge::compress_image_to_jpeg_file_in_memory( jpg_file_data.get_internal_array(), file_data_size, image._width, image._height, 3, reinterpret_cast< jpge::uint8_c const * >( conformant_image->_data.get_internal_array() ), jpge_params );
		if ( result )
		{
			jpg_file_data.set_length( file_data_size );
		}
		else
		{
			file_data_size += jpg_file_data.get_length() * 2;
			goto try_again;
		}
		return result;
	}

}
