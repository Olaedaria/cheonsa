#include "cheonsa_data_scribe_png.h"
#include "cheonsa_data_stream_memory.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	uint8_c const data_scribe_png_c::png_file_signature[ 8 ] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	uint32_c png_crc_table[ 256 ];

	boolean_c png_crc_table_computed = false;

	void png_crc_table_make()
	{
		uint32_c c;
		for ( uint32_c n = 0; n < 256; n++ )
		{
			c = n;
			for ( uint32_c k = 0; k < 8; k++ )
			{
				if ( c & 1 )
				{
					c = 0xedb88320L ^ ( c >> 1 );
				}
				else
				{
					c = c >> 1;
				}
			}
			png_crc_table[ n ] = c;
		}
		png_crc_table_computed = true;
	}

	uint32_c png_crc_update( uint32_c crc, uint8_c const * data, sint32_c data_size )
	{
		uint32_c c = crc;
		if ( !png_crc_table_computed )
		{
		png_crc_table_make();
		}
		for ( sint32_c n = 0; n < data_size; n++ )
		{
			c = png_crc_table[ ( c ^ data[ n ] ) & 0xff ] ^ ( c >> 8 );
		}
		return c;
	}

	uint32_c crc( uint8_c const * data, sint32_c data_size )
	{
		return png_crc_update( 0xffffffffL, data, data_size ) ^ 0xffffffffL;
	}

	boolean_c data_scribe_png_c::open( data_stream_c * stream )
	{
		assert( stream->get_position() == 0 );

		// check for png file signature.
		char8_c file_signature[ 8 ];
		if ( !stream->load( file_signature, 8 ) )
		{
			return false;
		}

		if (
			file_signature[ 0 ] != png_file_signature[ 0 ] ||
			file_signature[ 1 ] != png_file_signature[ 1 ] ||
			file_signature[ 2 ] != png_file_signature[ 2 ] ||
			file_signature[ 3 ] != png_file_signature[ 3 ] ||
			file_signature[ 4 ] != png_file_signature[ 4 ] ||
			file_signature[ 5 ] != png_file_signature[ 5 ] ||
			file_signature[ 6 ] != png_file_signature[ 6 ] ||
			file_signature[ 7 ] != png_file_signature[ 7 ] )
		{
			return false;
		}

		stream->set_position( 0 );
		sint32_c file_data_size = stream->get_size();
		_file_data.construct_mode_dynamic( file_data_size );
		stream->load( _file_data.get_internal_array(), file_data_size );

		data_stream_memory_c local_stream;
		local_stream.open_static( _file_data.get_internal_array(), _file_data.get_internal_array_size_used() );

		data_scribe_binary_c local_scribe;
		local_scribe.set_stream( &local_stream );
		local_scribe.set_byte_order( byte_order_e_big ); // pngs are big endian.
		
		local_stream.set_position( 8 ); // skip past png file signature.
		while ( local_stream.get_position() < local_stream.get_size() )
		{
			chunk_c * chunk = _chunk_list.emplace_at_end();
			chunk->data_size = local_scribe.load_uint32();
			chunk->type[ 0 ] = local_scribe.load_uint8();
			chunk->type[ 1 ] = local_scribe.load_uint8();
			chunk->type[ 2 ] = local_scribe.load_uint8();
			chunk->type[ 3 ] = local_scribe.load_uint8();
			chunk->data = nullptr;
			if ( chunk->data_size > 0 )
			{
				chunk->data = &local_stream.get_internal_buffer().get_internal_array()[ local_stream.get_position() ];
			}
			local_stream.set_position( local_stream.get_position() + chunk->data_size );
			chunk->crc = local_scribe.load_uint32();
		}

		return true;
	}

}
