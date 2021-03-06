﻿#include "cheonsa_data_stream_file.h"
#include "cheonsa__ops.h"
#include "cheonsa___build_configuration.h"

#include <cassert>

#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// implementing with windows specific rather than standard library, this gives us control read access sharing.
#endif

namespace cheonsa
{

	data_stream_file_c::data_stream_file_c()
		: _file( INVALID_HANDLE_VALUE )
	{
	}

	data_stream_file_c::~data_stream_file_c()
	{
#if defined( cheonsa_platform_windows )
		if ( _file != INVALID_HANDLE_VALUE )
		{
			CloseHandle( _file );
			_file = INVALID_HANDLE_VALUE;
		}
#else
#error data_stream_file_c::~data_stream_file_c() is not implemented.
#endif
	}

	boolean_c data_stream_file_c::open( string16_c const & file_path, data_stream_mode_e const stream_mode )
	{
#if defined( cheonsa_platform_windows )
		assert( ops::file_system_is_path_formatted_for_windows( file_path, ops::file_system_path_type_e_file ) );

		DWORD dwDesiredAccess = 0;
		if ( stream_mode & data_stream_mode_e_read )
		{
			dwDesiredAccess |= GENERIC_READ;
		}
		if ( stream_mode & data_stream_mode_e_write )
		{
			dwDesiredAccess |= GENERIC_WRITE;
		}

		DWORD dwShareMode = FILE_SHARE_READ;

		DWORD dwCreationDisposition = 0;
		if ( ops::file_system_does_file_exist( file_path ) )
		{
			if ( stream_mode & data_stream_mode_e_write )
			{
				dwCreationDisposition = TRUNCATE_EXISTING;
			}
			else if ( stream_mode & data_stream_mode_e_read )
			{
				dwCreationDisposition = OPEN_EXISTING;
			}
		}
		else
		{
			if ( stream_mode & data_stream_mode_e_write )
			{
				dwCreationDisposition = CREATE_NEW;
			}
			else if ( stream_mode & data_stream_mode_e_read )
			{
				return false;
			}
		}

		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

		_file = CreateFileW( file_path.character_list.get_internal_array(), dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL );

		return _file != INVALID_HANDLE_VALUE;
#else
#error data_stream_file_c::open() is not implemented.
#endif
	}

	void_c data_stream_file_c::close()
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		CloseHandle( _file );
		_file = INVALID_HANDLE_VALUE;
#else
#error data_stream_file_c::close() is not implemented.
#endif
	}

	sint32_c data_stream_file_c::get_size() const
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		LARGE_INTEGER result;
		assert( GetFileSizeEx( _file, &result ) );
		assert( result.QuadPart < constants< sint32_c >::maximum() );
		return static_cast< sint32_c >( result.QuadPart );
#else
#error data_stream_file_c::get_size() is not implemented.
#endif
	}

	sint32_c data_stream_file_c::get_position() const
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		LARGE_INTEGER liDistanceToMove = {};
		LARGE_INTEGER liNewFilePointer = {};
		assert( SetFilePointerEx( _file, liDistanceToMove, &liNewFilePointer, FILE_CURRENT ) );
		assert( liNewFilePointer.QuadPart < constants< sint32_c >::maximum() );
		return static_cast< sint32_c >( liNewFilePointer.QuadPart );
#else
#error data_stream_file_c::get_position() is not implemented.
#endif
	}

	boolean_c data_stream_file_c::set_position( sint32_c position )
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		LARGE_INTEGER liDistanceToMove = {};
		liDistanceToMove.QuadPart = position;
		return SetFilePointerEx( _file, liDistanceToMove, NULL, FILE_BEGIN );
#else
#error data_stream_file_c::set_position() is not implemented.
#endif
	}

	boolean_c data_stream_file_c::load( void_c * const data, sint32_c const size )
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		if ( size )
		{
			DWORD NumberOfBytesRead;
			assert( ReadFile( _file, data, size, &NumberOfBytesRead, NULL ) );
			return NumberOfBytesRead == size;
		}
		return true;
#else
#error data_stream_file_c::load() is not implemented.
#endif
	}

	boolean_c data_stream_file_c::save( void_c const * const data, sint32_c const size )
	{
#if defined( cheonsa_platform_windows )
		assert( _file != INVALID_HANDLE_VALUE );
		if ( size )
		{
			DWORD NumberOfBytesWritten;
			assert( WriteFile( _file, data, size, &NumberOfBytesWritten, NULL ) );
			return NumberOfBytesWritten == size;
		}
		return true;
#else
#error data_stream_file_c::save() is not implemented.
#endif
	}

}
