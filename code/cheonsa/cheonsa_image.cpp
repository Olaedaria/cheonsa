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
		: type{}
		, data_size( 0 )
		, data( nullptr )
		, data_is_ours( false )
		, crc( 0 )
	{
	}

	image_png_chunk_c::~image_png_chunk_c()
	{
		if ( data_is_ours && data != nullptr )
		{
			delete[] data;
			data = nullptr;
			data_is_ours = false;
		}
	}

	image_png_chunk_c & image_png_chunk_c::operator = ( image_png_chunk_c & other )
	{
		type[ 0 ] = other.type[ 0 ];
		type[ 1 ] = other.type[ 1 ];
		type[ 2 ] = other.type[ 2 ];
		type[ 3 ] = other.type[ 3 ];
		data_size = other.data_size;
		data = other.data;
		data_is_ours = other.data_is_ours;

		other.type[ 0 ] = 0;
		other.type[ 1 ] = 0;
		other.type[ 2 ] = 0;
		other.type[ 3 ] = 0;
		other.data_size = 0;
		other.data = nullptr;
		other.data_is_ours = false;

		return *this;
	}

	image_c::image_c()
		: width( 0 )
		, height( 0 )
		, channel_count( 0 )
		, channel_bit_depth( 0 )
		, data()
	{
	}

	boolean_c image_copy_and_convert( image_c const & in, image_c & out )
	{
		// validate inputs.
		assert( in.width >= 1 && in.width <= 4096 );
		assert( in.height >= 1 && in.height <= 4096 );
		assert( in.channel_count >= 1 && in.channel_count <= 4 );
		assert( in.channel_bit_depth == 8 || in.channel_bit_depth == 16 );
		assert( in.data.get_length() == in.width * in.height * in.channel_count * ( in.channel_bit_depth / 8 ) );
		assert( out.width == in.width );
		assert( out.height == in.height );
		assert( out.channel_count >= 1 && out.channel_count <= 4 );
		assert( out.channel_bit_depth == 8 || out.channel_bit_depth == 16 );

		// copy and/or convert.
		sint32_c pixel_count = in.width * in.height;
		sint32_c out_data_size = out.width * out.height * out.channel_count * ( out.channel_bit_depth / 8 );
		out.data.construct_mode_dynamic( out_data_size, out_data_size );
		if ( in.channel_count == out.channel_count && in.channel_bit_depth == out.channel_bit_depth )
		{
			assert( in.data.get_length() == out.data.get_length() );
			ops::memory_copy( in.data.get_internal_array(), out.data.get_internal_array(), in.data.get_length() );
			return true;
		}
		else if ( in.channel_bit_depth == 8 && out.channel_bit_depth == 8 )
		{
			uint8_c out_pixel_default[ 4 ] = { 0x00, 0x00, 0x00, 0xFF };
			uint8_c const * in_data_pointer = in.data.get_internal_array();
			uint8_c * out_data_pointer = out.data.get_internal_array();
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out.channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in.channel_count ? in_data_pointer[ j ] : out_pixel_default[ j ];
				}
				in_data_pointer += in.channel_count;
				out_data_pointer += out.channel_count;
			}
			return true;
		}
		else if ( in.channel_bit_depth == 8 && out.channel_bit_depth == 16 )
		{
			uint16_c out_pixel_default[ 4 ] = { 0x0000, 0x0000, 0x0000, 0xFFFF };
			uint8_c const * in_data_pointer = in.data.get_internal_array();
			uint16_c * out_data_pointer = reinterpret_cast< uint16_c * >( out.data.get_internal_array() );
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out.channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in.channel_count ? static_cast< uint16_c >( in_data_pointer[ j ] ) << 8 : out_pixel_default[ j ];
				}
				in_data_pointer += in.channel_count;
				out_data_pointer += out.channel_count;
			}
			return true;
		}
		else if ( in.channel_bit_depth == 16 && out.channel_bit_depth == 8 )
		{
			uint8_c out_pixel_default[ 4 ] = { 0x00, 0x00, 0x00, 0xFF };
			uint16_c const * in_data_pointer = reinterpret_cast< uint16_c const * >( in.data.get_internal_array() );
			uint8_c * out_data_pointer = out.data.get_internal_array();
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out.channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in.channel_count ? static_cast< uint8_c >( in_data_pointer[ j ] >> 8 ) : out_pixel_default[ j ];
				}
				in_data_pointer += in.channel_count;
				out_data_pointer += out.channel_count;
			}
			return true;
		}
		else if ( in.channel_bit_depth == 16 && out.channel_bit_depth == 16 )
		{
			uint16_c out_pixel_default[ 4 ] = { 0x0000, 0x0000, 0x0000, 0xFFFF };
			uint16_c const * in_data_pointer = reinterpret_cast< uint16_c const * >( in.data.get_internal_array() );
			uint16_c * out_data_pointer = reinterpret_cast< uint16_c * >( out.data.get_internal_array() );
			for ( sint32_c i = 0; i < pixel_count; i++ )
			{
				for ( sint32_c j = 0; j < out.channel_count; j++ )
				{
					out_data_pointer[ j ] = j < in.channel_count ? in_data_pointer[ j ] : out_pixel_default[ j ];
				}
				in_data_pointer += in.channel_count;
				out_data_pointer += out.channel_count;
			}
			return true;
		}

		return false;
	}


	boolean_c image_load_from_png_just_the_extra_chunks(
		data_stream_c * png_file_stream,
		core_list_c< image_png_chunk_c > & extra_chunk_list )
	{
		assert( png_file_stream != nullptr );

		data_stream_c * & stream = png_file_stream;

		extra_chunk_list.remove_all();

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
		scribe.set_stream( stream );
		scribe.set_byte_order( byte_order_e_big );
		
		sint32_c stream_size = stream->get_size();
		while ( stream->get_position() + 8 <= stream_size );
		{
			image_png_chunk_c * chunk = extra_chunk_list.emplace_at_end();
			if ( !scribe.load_uint32( chunk->data_size ) )
			{
				return false;
			}
			if ( !scribe.get_stream()->load( chunk->type, 4 ) )
			{
				return false;
			}
			if ( chunk->data_size > 0 )
			{
				chunk->data = new uint8_c[ chunk->data_size ];
				chunk->data_is_ours = true;
				if ( !stream->load( chunk->data, chunk->data_size ) )
				{
					return false;
				}
			}
			if ( !scribe.load_uint32( chunk->crc ) )
			{
				return false;
			}
			// we won't check the crc.
		}

		return true;
	}

	boolean_c image_load_from_png(
		core_list_c< uint8_c > const & png_file_data,
		image_c & image,
		core_list_c< image_png_chunk_c > * extra_chunk_list )
	{
		// validate inputs.
		if ( image.channel_count == 0 || image.channel_bit_depth == 0 )
		{
			assert( image.channel_count == 0 && image.channel_bit_depth == 0 );
		}
		else
		{
			assert( image.channel_count >= 1 && image.channel_count <= 4 );
			assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );
		}

		if ( extra_chunk_list != nullptr )
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
				image_png_chunk_c * chunk = extra_chunk_list->emplace_at_end();
				chunk->data_size = lodepng::lodepng_chunk_length( extra_chunks_data );
				ops::memory_copy( &extra_chunks_data[ 4 ], chunk->type, 4 );
				chunk->data_is_ours = false;
				chunk->data = nullptr;
				if ( chunk->data_size > 0 )
				{
					chunk->data_is_ours = true;
					chunk->data = new uint8_c[ chunk->data_size ];
					ops::memory_copy( lodepng::lodepng_chunk_data( extra_chunks_data ), chunk->data, chunk->data_size );
				}
				p += 4 + 4 + chunk->data_size + 4;
			}
		}

		// construct the decoded image, which we may need later if we need to convert to fit the format requested by result.
		image_c decoded_image;
		decoded_image.width = decoded_image_width;
		decoded_image.height = decoded_image_height;
		if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_GREY )
		{
			decoded_image.channel_count = 1;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_GREY_ALPHA )
		{
			decoded_image.channel_count = 2;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_RGB )
		{
			decoded_image.channel_count = 3;
		}
		else if ( decoded_lode_png_state.info_png.color.colortype == lodepng::LCT_RGBA )
		{
			decoded_image.channel_count = 4;
		}
		else
		{
			// it shouldn't be possible to reach here.
			return false;
		}
		decoded_image.channel_bit_depth = decoded_lode_png_state.info_png.color.bitdepth;

		// construct the result image.
		image.width = decoded_image_width;
		image.height = decoded_image_height;
		if( image.channel_count == 0 || image.channel_bit_depth == 0 )
		{
			image.channel_count = decoded_image.channel_count;
			image.channel_bit_depth = decoded_image.channel_bit_depth;
			assert( decoded_image_data.size() < constants< sint32_c >::maximum() );
			image.data.construct_mode_dynamic_from_array( &decoded_image_data[ 0 ], static_cast< sint32_c >( decoded_image_data.size() ) );
			return true;
		}
		else
		{
			assert( decoded_image_data.size() < constants< sint32_c >::maximum() );
			decoded_image.data.construct_mode_static_from_array( &decoded_image_data[ 0 ], static_cast< sint32_c >( decoded_image_data.size() ) );
			return image_copy_and_convert( decoded_image, image );
		}

		// it shouldn't be possible to reach here.
		return false;
	}

	boolean_c image_save_to_png(
		image_c const & image,
		core_list_c< image_png_chunk_c > const * extra_chunk_list,
		core_list_c< uint8_c > & png_file_data )
	{
		// validate inputs.
		assert( image.width >= 1 && image.width <= 4096 );
		assert( image.height >= 1 && image.height <= 4096 );
		assert( image.channel_count >= 1 && image.channel_count <= 4 );
		assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );

		// initialize a state so we can customize some things.
		lodepng::State lodepng_state;
		lodepng::lodepng_state_init( &lodepng_state );
		lodepng_state.info_raw.bitdepth = lodepng_state.info_png.color.bitdepth = image.channel_bit_depth;

		// define encoding format.
		if ( image.channel_count == 1 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_GREY;
		}
		else if ( image.channel_count == 2 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_GREY_ALPHA;
		}
		else if ( image.channel_count == 3 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_RGB;
		}
		else if ( image.channel_count == 4 )
		{
			lodepng_state.info_raw.colortype = lodepng_state.info_png.color.colortype = lodepng::LCT_RGBA;
		}
		
		// add custom chunks to state.
		if ( extra_chunk_list != nullptr )
		{
			for ( sint32_c i = 0; i < extra_chunk_list->get_length(); i++ )
			{
				image_png_chunk_c const & chunk = (*extra_chunk_list)[ i ];
				lodepng::lodepng_chunk_create( &lodepng_state.info_png.unknown_chunks_data[ 2 ], &lodepng_state.info_png.unknown_chunks_size[ 2 ], chunk.data_size, reinterpret_cast< char const * >( chunk.type ), chunk.data );
			}
		}

		// convert the image to png format.
		std::vector< unsigned char > png_file_data_std;
		unsigned error = lodepng::encode( png_file_data_std, image.data.get_internal_array(), image.width, image.height, lodepng_state );
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

	boolean_c image_load_from_jpg(
		core_list_c< uint8_c > const & jpg_file_data,
		image_c & image
	)
	{
		// validate inputs.
		if ( image.channel_count == 0 || image.channel_bit_depth == 0 )
		{
			assert( image.channel_count == 0 && image.channel_bit_depth == 0 );
		}
		else
		{
			assert( image.channel_count >= 1 && image.channel_count <= 4 );
			assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );
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
		decoded_image.width = decoded_image_width;
		decoded_image.height = decoded_image_height;
		if ( decoded_image_channel_count == 1 )
		{
			decoded_image.channel_count = 1;
		}
		else if ( decoded_image_channel_count == 3 )
		{
			decoded_image.channel_count = 3;
		}
		else
		{
			// it shouldn't be possible to reach here.
			free( decoded_image_data );
			return false;
		}
		decoded_image.channel_bit_depth = 8;

		// construct the result image.
		image.width = decoded_image_width;
		image.height = decoded_image_height;
		if( image.channel_count == 0 || image.channel_bit_depth == 0 )
		{
			image.channel_count = decoded_image.channel_count;
			image.channel_bit_depth = decoded_image.channel_bit_depth;
			image.data.construct_mode_dynamic_from_array( decoded_image_data, decoded_image_data_size );
			free( decoded_image_data );
			return true;
		}
		else
		{
			decoded_image.data.construct_mode_static_from_array( decoded_image_data, decoded_image_data_size );
			boolean_c result = image_copy_and_convert( decoded_image, image );
			free( decoded_image_data );
			return result;
		}

		// it shouldn't be possible to reach here.
		free( decoded_image_data );
		return false;
	}

	boolean_c image_save_to_jpg(
		image_c const & image,
		sint32_c jpg_quality,
		core_list_c< uint8_c > & jpg_file_data
	)
	{
		assert( image.width >= 1 && image.width <= 4096 );
		assert( image.height >= 1 && image.height <= 4096 );
		assert( image.channel_count >= 3 && image.channel_count <= 4 );
		assert( image.channel_bit_depth == 8 || image.channel_bit_depth == 16 );
		assert( jpg_quality >= 1 && jpg_quality <= 100 );

		image_c const * conformant_image = &image;
		image_c stage_image; // will be initialized if needed to hold r8g8b8 image.
		if ( image.channel_bit_depth == 16 || image.channel_count == 4 )
		{
			conformant_image = &stage_image;
			stage_image.width = image.width;
			stage_image.height = image.height;
			stage_image.channel_count = 3;
			stage_image.channel_bit_depth = 8;
			image_copy_and_convert( image, stage_image );
		}

		// we can't know the exact size of the buffer that we will need to store the result.
		// but https://en.wikipedia.org/wiki/JPEG#Sample_photographs
		// gives us an idea of how much memory to pre-allocate to store the result.
		jpge::params jpge_params;
		jpge_params.m_quality = jpg_quality;
		int file_data_size = static_cast< sint32_c >( image.width * image.height * 8.25f );
	try_again:
		jpg_file_data.construct_mode_dynamic( file_data_size );
		bool result = jpge::compress_image_to_jpeg_file_in_memory( jpg_file_data.get_internal_array(), file_data_size, image.width, image.height, 3, reinterpret_cast< jpge::uint8_c const * >( conformant_image->data.get_internal_array() ), jpge_params );
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
