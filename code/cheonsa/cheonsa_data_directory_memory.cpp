#include "cheonsa_data_directory_memory.h"
#include "cheonsa_data_scribe_binary.h"

namespace cheonsa
{

	data_directory_memory_c::file_c * data_directory_memory_c::_find_existing_file( string16_c const & file_path )
	{
		for ( sint32_c i = 0; i < _file_list.get_length(); i++ )
		{
			if ( file_path == _file_list[ i ]->path )
			{
				return _file_list[ i ];
			}
		}
		return nullptr;
	}

	data_directory_memory_c::file_c * data_directory_memory_c::_find_existing_file_or_create_new_file( string16_c const & file_path )
	{
		file_c * result = _find_existing_file( file_path );
		if ( result )
		{
			return result;
		}
		result = new file_c();
		result->path = file_path;
		_file_list.insert_at_end( result );
		return result;
	}

	data_directory_memory_c::data_directory_memory_c()
		: _file_list()
	{
	}

	data_directory_memory_c::~data_directory_memory_c()
	{
		reset();
	}

	data_stream_memory_c * data_directory_memory_c::open_file( string16_c const & file_path, data_stream_mode_e stream_mode )
	{
		if ( stream_mode == data_stream_mode_e_read )
		{
			file_c * file = _find_existing_file( file_path );
			if ( file != nullptr )
			{
				file->stream.set_position( 0 );
				return & file->stream;
			}
		}
		else
		{
			file_c * file = _find_existing_file_or_create_new_file( file_path );
			file->stream.close();
			file->stream.open();
			return & file->stream;
		}
		return nullptr;
	}

	boolean_c data_directory_memory_c::data_does_file_exist( string16_c const & file_path )
	{
		return _find_existing_file( file_path ) != nullptr;
	}

	boolean_c data_directory_memory_c::delete_file( string16_c const & file_path )
	{
		for ( sint32_c i = 0; i < _file_list.get_length(); i++ )
		{
			file_c * file = _file_list[ i ];
			if ( file->path == file_path )
			{
				_file_list.remove_at_index( i );
				delete file;
				return true;
			}
		}
		return false;
	}

	void_c data_directory_memory_c::reset()
	{
		for ( sint32_c i = 0; i < _file_list.get_length(); i++ )
		{
			delete _file_list[ i ];
			_file_list[ i ] = nullptr;
		}
		_file_list.remove_all();
	}

	boolean_c data_directory_memory_c::save( data_stream_c * stream, endianness_e endianness )
	{
		assert( _file_list.get_length() < constants< sint32_c >::maximum() );
		data_scribe_binary_c scribe_binary;
		scribe_binary.open( stream, endianness );
		scribe_binary.save_uint32( _file_list.get_length() );
		for ( sint32_c i = 0; i < _file_list.get_length(); i++ )
		{
			file_c * file = _file_list[ i ];
			scribe_binary.save_string8( string8_c( file->path ) );
			assert( file->stream.get_size() < constants< sint32_c >::maximum() );
			scribe_binary.save_uint32( static_cast< uint32_c >( file->stream.get_size() ) );
			stream->save( file->stream.get_internal_buffer().get_internal_array(), file->stream.get_size() );
		}
		return true;
	}

	boolean_c data_directory_memory_c::load( data_stream_c * stream, endianness_e endianness )
	{
		reset();
		data_scribe_binary_c scribe_binary;
		scribe_binary.open( stream, endianness );
		uint32_c file_count = scribe_binary.load_uint32();
		for ( uint32_c i = 0; i < file_count; i++ )
		{
			file_c * file = new file_c();
			file->path = scribe_binary.load_string8();
			uint32_c file_size = scribe_binary.load_uint32();
			file->stream.close();
			file->stream.open();
			file->stream.set_size( file_size );
			stream->load( file->stream.get_internal_buffer().get_internal_array(), file_size );
		}
		return true;
	}

	/*
	boolean_c data_directory_memory_c::save( data_scribe_structure_c * structure )
	{
		data_scribe_structure_c::property_c * property = nullptr;

		property = structure->add_property( "FileList", data_scribe_structure_c::property_c::type_e_structure_list );

		for ( sint32_c i = 0; i < _file_list.get_count(); i++ )
		{
			file_c * file = _file_list[ i ];
			data_scribe_structure_c * file_structure = property->add_structure_to_structure_list();
			data_scribe_structure_c::property_c * file_property = nullptr;
			file_property = file_structure->add_property( "path", data_scribe_structure_c::property_c::type_e_string );
			file_property->get_string() = file->path;
			file_property = file_structure->add_property( "data", data_scribe_structure_c::property_c::type_e_buffer );
			file_property->set_buffer( 0, file->stream.get_buffer_pointer(), file->stream.get_size() );
		}

		return true;
	}

	boolean_c data_directory_memory_c::load( data_scribe_structure_c * structure )
	{
		reset();

		data_scribe_structure_c::property_c * property = nullptr;

		if ( structure->find_property( property, "FileList", data_scribe_structure_c::property_c::type_e_structure_list ) )
		{
			for ( sint32_c i = 0; i < property->get_structure_list_count(); i++ )
			{
				file_c * file = new file_c();
				data_scribe_structure_c * file_structure = property->get_structure_in_structure_list_at_index( i );
				data_scribe_structure_c::property_c * file_property = nullptr;
				if ( file_structure->find_property( file_property, "path", data_scribe_structure_c::property_c::type_e_string ) )
				{
					file->path = file_property->get_string();
				}
				if ( file_structure->find_property( file_property, "data", data_scribe_structure_c::property_c::type_e_buffer ) )
				{
					file->stream.open();
					file->stream.save( file_property->get_buffer_data(), file_property->get_buffer_data_size() );
				}
				_file_list.insert_at_end( file );
			}
		}
		return true;
	}
	*/

}
