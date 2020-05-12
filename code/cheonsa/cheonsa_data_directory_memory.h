#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_stream_memory.h"
#include "cheonsa_data_scribe_binary.h"

namespace cheonsa
{

	// an in-memory file system, that can serve as a place to store save-game-game-state. then it can be saved to or loaded from disk.
	class data_directory_memory_c
	{
	private:
		class file_c
		{
		public:
			string16_c path;
			data_stream_memory_c stream;
		};

		core_list_c< file_c * > _file_list;
		file_c * _find_existing_file( string16_c const & file_path );
		file_c * _find_existing_file_or_create_new_file( string16_c const & file_path );

	public:
		data_directory_memory_c();
		~data_directory_memory_c();

		data_stream_memory_c * open_file( string16_c const & file_path, data_stream_mode_e stream_mode ); // opens and returns a * to a stream located at file_path in this file system. do not close the stream when you are done with it because that will erase whatever is in it. do not delete the stream when you are done with it because it is managed by this file system.
		boolean_c file_system_does_file_exist( string16_c const & file_path ); // returns true if a file exists at file_path within this file system.
		boolean_c delete_file( string16_c const & file_path ); // returns true if successfully deleted file at file_path within this file system.
		void_c reset(); // deletes all files in this file system.

		boolean_c save( data_scribe_binary_c & scribe ); // saves the state of this memory file system to a stream.
		boolean_c load( data_scribe_binary_c & scribe ); // loads the state of this memory file system from a stream.

	};

}
