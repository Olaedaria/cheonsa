#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	video_renderer_pixel_shader_c::video_renderer_pixel_shader_c( string16_c const & file_name )
		: _shader_variations( nullptr )
		, _ps( nullptr )
		, _ps_masked( nullptr )
		, _reference_count( 0 )
	{
		_shader_variations = new video_renderer_shader_manager_c::shader_variations_c( file_name, false );
		_shader_variations->_ps = &_ps;
		_shader_variations->_ps_masked = &_ps_masked;
	}

	video_renderer_pixel_shader_c::~video_renderer_pixel_shader_c()
	{
		assert( _reference_count == 0 );
		assert( _shader_variations );
		delete _shader_variations;
		_shader_variations = nullptr;
	}

	void_c video_renderer_pixel_shader_c::add_reference()
	{
		_reference_count++;
	}

	void_c video_renderer_pixel_shader_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	video_pixel_shader_c * video_renderer_pixel_shader_c::get_ps() const
	{
		return _ps;
	}

	video_pixel_shader_c * video_renderer_pixel_shader_c::get_ps_masked() const
	{
		return _ps_masked;
	}

	video_renderer_pixel_shader_c::reference_c::reference_c()
		: _value( nullptr )
	{
	}

	video_renderer_pixel_shader_c::reference_c::~reference_c()
	{
		if ( _value )
		{
			assert( _value->_reference_count > 0 );
			_value->_reference_count--;
		}
	}

	video_renderer_pixel_shader_c::reference_c & video_renderer_pixel_shader_c::reference_c::operator = ( video_renderer_pixel_shader_c::reference_c const & other )
	{
		if ( other._value != _value )
		{
			if ( other._value )
			{
				other._value->add_reference();
			}
			if ( _value )
			{
				_value->remove_reference();
			}
			_value = other._value;
		}
		return *this;
	}

	video_renderer_pixel_shader_c::reference_c & video_renderer_pixel_shader_c::reference_c::operator = ( video_renderer_pixel_shader_c * other )
	{
		if ( other != _value )
		{
			if ( other != nullptr )
			{
				other->add_reference();
			}
			if ( _value != nullptr )
			{
				_value->remove_reference();
			}
			_value = other;
		}
		return *this;
	}

	boolean_c video_renderer_pixel_shader_c::reference_c::operator == ( video_renderer_pixel_shader_c::reference_c const & other ) const
	{
		return _value == other._value;
	}

	boolean_c video_renderer_pixel_shader_c::reference_c::operator != ( video_renderer_pixel_shader_c::reference_c const & other ) const
	{
		return _value != other._value;
	}

	video_renderer_pixel_shader_c * video_renderer_pixel_shader_c::reference_c::operator -> () const
	{
		assert( _value );
		return _value;
	}

	video_renderer_pixel_shader_c::reference_c::operator video_renderer_pixel_shader_c * () const
	{
		return _value;
	}

	boolean_c video_renderer_pixel_shader_c::reference_c::get_value_is_set() const
	{
		return _value != nullptr;
	}

	boolean_c video_renderer_pixel_shader_c::reference_c::get_value_is_set_and_loaded() const
	{
		return _value != nullptr && _value->_ps != nullptr;
	}

	video_renderer_shader_manager_c::file_dependency_c::file_dependency_c()
		: _file_name()
		, _absolute_file_path()
		, _last_write_time( 0 )
	{
	}

	video_renderer_shader_manager_c::file_dependency_c & video_renderer_shader_manager_c::file_dependency_c::operator = ( file_dependency_c const & other )
	{
		_file_name = other._file_name;
		_absolute_file_path = other._absolute_file_path;
		_last_write_time = other._last_write_time;
		return *this;
	}

	boolean_c video_renderer_shader_manager_c::file_dependency_c::operator == ( file_dependency_c const & other ) const
	{
		return _file_name == other._file_name && _last_write_time == other._last_write_time;
	}

	boolean_c video_renderer_shader_manager_c::file_dependency_c::operator != ( file_dependency_c const & other ) const
	{
		return _file_name != other._file_name || _last_write_time != other._last_write_time;
	}

	void_c video_renderer_shader_manager_c::_resolve_cache_file_path_absolute( file_dependency_c const & source_file_dependency, variation_e variation, string16_c & result )
	{
		result = ops::path_get_folder_path( source_file_dependency._absolute_file_path );
#if defined( cheonsa_platform_windows )
		result += "cache\\";
#else
#error
#endif
		result += ops::path_get_file_or_folder_name_without_extension( source_file_dependency._file_name );
		result += get_variation_suffix( variation );
		result += ".so";
	}

	// gets the variation suffix for shader object file cache.
	char8_c const * video_renderer_shader_manager_c::get_variation_suffix( video_renderer_shader_manager_c::variation_e variation )
	{
		static char8_c const * const map[ video_renderer_shader_manager_c::variation_e_count_ ] = {
			"",
			"_waved",
			"_clipped",
			"_waved_clipped",
			"",
			"_masked",
		};
		assert( variation >= 0 && variation < video_renderer_shader_manager_c::variation_e_count_ );
		return map[ variation ];
	}

	video_shader_defines_c const * video_renderer_shader_manager_c::get_variation_shader_defines( video_renderer_shader_manager_c::variation_e variation )
	{
		static video_shader_defines_c const * const map[ video_renderer_shader_manager_c::variation_e_count_ ] = {
			nullptr,
			&video_renderer_interface_c::shader_define_waved,
			&video_renderer_interface_c::shader_define_clipped,
			&video_renderer_interface_c::shader_define_waved_clipped,
			nullptr,
			&video_renderer_interface_c::shader_define_masked };
		assert( variation >= 0 && variation < video_renderer_shader_manager_c::variation_e_count_ );
		return map[ variation ];
	}

	video_renderer_shader_manager_c::shader_variations_c::shader_variations_c( string16_c const & file_name, boolean_c is_internal )
		: _is_internal( is_internal )
		, _source_file()
		, _source_file_dependency_list()
		, _input_vertex_layout( nullptr )
		, _output_vertex_format( nullptr )
		, _vs( nullptr )
		, _vs_waved( nullptr )
		, _vs_clipped( nullptr )
		, _vs_waved_clipped( nullptr )
		, _ps( nullptr )
		, _ps_masked( nullptr )
	{
		_source_file._file_name = file_name;
		resolve_file_path( _source_file._file_name, is_internal, _source_file._absolute_file_path );
	}

	video_renderer_shader_manager_c::shader_variations_c::~shader_variations_c()
	{
		if ( _vs && *_vs )
		{
			delete *_vs;
			const_cast< video_vertex_shader_c const * >( *_vs ) = nullptr;
		}
		if ( _vs_waved && *_vs_waved )
		{
			delete *_vs_waved;
			const_cast< video_vertex_shader_c const * >( *_vs_waved ) = nullptr;
		}
		if ( _vs_clipped && *_vs_clipped )
		{
			delete *_vs_clipped;
			const_cast< video_vertex_shader_c const * >( *_vs_clipped ) = nullptr;
		}
		if ( _vs_waved_clipped && *_vs_waved_clipped )
		{
			delete *_vs_waved_clipped;
			const_cast< video_vertex_shader_c const * >( *_vs_waved_clipped ) = nullptr;
		}
		if ( _ps && *_ps )
		{
			delete *_ps;
			const_cast< video_pixel_shader_c const * >( *_ps ) = nullptr;
		}
		if ( _ps_masked && *_ps_masked )
		{
			delete *_ps_masked;
			const_cast< video_pixel_shader_c const * >( *_ps_masked ) = nullptr;
		}
	}

	boolean_c video_renderer_shader_manager_c::shader_variations_c::has_variation( variation_e variation )
	{
		switch ( variation )
		{
		case variation_e_vs:
			return _vs != nullptr;
		case variation_e_vs_waved:
			return _vs_waved != nullptr;
		case variation_e_vs_clipped:
			return _vs_clipped != nullptr;
		case variation_e_vs_waved_clipped:
			return _vs_waved_clipped != nullptr;
		case variation_e_ps:
			return _ps != nullptr;
		case variation_e_ps_masked:
			return _ps_masked != nullptr;
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::_load_source_dependency_information( shader_variations_c * shader_variations )
	{
		shader_variations->_source_file_dependency_list.remove_all();
		if ( !ops::file_system_get_file_or_folder_last_write_time( shader_variations->_source_file._absolute_file_path, shader_variations->_source_file._last_write_time ) )
		{
			return false;
		}
		return __load_source_dependency_list_recursive( shader_variations->_source_file, shader_variations->_is_internal, shader_variations->_source_file_dependency_list );
	}

	// given file_name that references a shader file, recursively scans for additional file names referenced by #include directives in that shader file.
	// populates result for each additional file name that is found to be referenced.
	boolean_c video_renderer_shader_manager_c::__load_source_dependency_list_recursive( file_dependency_c const & source_file_dependency, boolean_c is_internal, core_list_c< file_dependency_c > & result )
	{
		// load the whole file into memory.
		data_stream_file_c stream;
		if ( !stream.open( source_file_dependency._absolute_file_path, data_stream_mode_e_read ) )
		{
			return false;
		}
		sint32_c data_size = stream.get_size();
		string8_c data;
		data.character_list.construct_mode_dynamic( data_size + 1, data_size + 1 );
		data.character_list[ data_size ] = 0;
		stream.load( data.character_list.get_internal_array(), data_size );
		stream.close();

		// find instances of "#include".
		core_list_c< sint32_c > instances;
		ops::string8_find_indices_of( data, string8_c( core_list_mode_e_static, "#include" ), instances );

		// find quoted string.
		for ( sint32_c i = 0; i < instances.get_length(); i++ )
		{
			sint32_c include_file_start = instances[ i ] + 8; // + 8 to skip past "#include".
			char8_c * scanner = &data.character_list[ include_file_start ];
			char8_c * start = nullptr;
			char8_c * end = nullptr;
			// scan for opening quote.
			while ( *scanner != 0 )
			{
				if ( *scanner == '"' )
				{
					scanner++;
					start = scanner;
					break;
				}
				else if ( *scanner != ' ' && *scanner != '\t' )
				{
					// illegal character, we can abort.
					return false;
				}
				scanner++;
			}
			// scan for closing quote.
			while ( *scanner != 0 )
			{
				if ( *scanner == '"' )
				{
					end = scanner;
					break;
				}
				//else if ( !ops::char16_is_valid_for_file_path( *scanner ) )
				//{
				//	// illegal character, we can abort.
				//	return false;
				//}
				scanner++;
			}
			// insert into result list if needed.
			if ( start && end )
			{
				*end = 0; // replace closing quote with null character.
				string16_c file_name = string16_c( start );
				boolean_c already_in_result = false;
				for ( sint32_c j = 0; j < result.get_length(); j++ )
				{
					if ( result[ j ]._file_name == file_name )
					{
						already_in_result = true;
						break;
					}
				}
				if ( already_in_result == false )
				{
					video_renderer_shader_manager_c::file_dependency_c & sub_file_dependency = *result.emplace( -1, 1 );
					sub_file_dependency._file_name = file_name;
					resolve_file_path( sub_file_dependency._file_name, is_internal, sub_file_dependency._absolute_file_path );
					ops::file_system_get_file_or_folder_last_write_time( sub_file_dependency._absolute_file_path, sub_file_dependency._last_write_time );
					__load_source_dependency_list_recursive( sub_file_dependency, is_internal, result ); // recurse.
				}
			}
		}

		return true;
	}

	boolean_c video_renderer_shader_manager_c::_load_cached_dependency_information( shader_variations_c * shader_variations, variation_e variation, string16_c & absolute_file_path, sint64_c & file_last_write_time, core_list_c< file_dependency_c > & file_dependency_list )
	{
		file_last_write_time = 0;
		file_dependency_list.remove_all();
		absolute_file_path = ops::path_get_folder_path( shader_variations->_source_file._absolute_file_path );
#if defined( cheonsa_platform_windows )
		absolute_file_path += "cache\\";
#else
#error
#endif
		absolute_file_path += ops::path_get_file_or_folder_name_without_extension( shader_variations->_source_file._file_name );
		absolute_file_path += get_variation_suffix( variation );
		absolute_file_path += ".so";

		data_stream_file_c stream;
		if ( !stream.open( absolute_file_path, data_stream_mode_e_read ) )
		{
			return false;
		}

		data_scribe_binary_c scribe_binary;
		scribe_binary.set_stream( &stream );

		sint32_c compiled_code_position = 0;
		if ( !scribe_binary.load_sint32( compiled_code_position ) ) // skip the value that defines the position of where the compiled code begins.
		{
			return false;
		}
		if ( !scribe_binary.load_sint64( file_last_write_time ) )
		{
			return false;
		}
		uint8_c dependency_count = 0;
		if ( !scribe_binary.load_uint8( dependency_count ) )
		{
			return false;
		}
		file_dependency_list.set_length_absolute( dependency_count );
		for ( sint32_c i = 0; i < file_dependency_list.get_length(); i++ )
		{
			video_renderer_shader_manager_c::file_dependency_c & file_dependency = file_dependency_list[ i ];
			string8_c file_dependency_file_name_string8;
			if ( !scribe_binary.load_string8( file_dependency_file_name_string8 ) )
			{
				return false;
			}
			file_dependency._file_name = file_dependency_file_name_string8;
			if ( !scribe_binary.load_sint64( file_dependency._last_write_time ) )
			{
				return false;
			}
		}
		stream.close();

		return true;
	}

	void_c video_renderer_shader_manager_c::_refresh_shader_variations( shader_variations_c * shader_variations )
	{
		assert( shader_variations->_source_file._file_name.get_length() > 0 );
		assert( shader_variations->_source_file._absolute_file_path.get_length() > 0 );

		// parse source file for "#include"s and build an up to date source_file_dependency_list.
		_load_source_dependency_information( shader_variations );

		// check cached shader object file variations for their dependencies to compare for differences.
		for ( sint32_c i = 0; i < variation_e_count_; i++ )
		{
			variation_e variation = static_cast< variation_e >( i );
			if ( shader_variations->has_variation( variation ) )
			{
				// load the dependencies that were present at the time of compilation that are saved with the cache file.
				string16_c cache_file_path_absolute;
				sint64_c cache_file_last_write_time = 0;
				core_list_c< file_dependency_c > cache_file_dependency_list; // file names of included files, and their file modified times.
				boolean_c has_cached = _load_cached_dependency_information( shader_variations, variation, cache_file_path_absolute, cache_file_last_write_time, cache_file_dependency_list );

				// compare for differences between source file and cache file.
				if ( has_cached == false || shader_variations->_source_file._last_write_time != cache_file_last_write_time || shader_variations->_source_file_dependency_list != cache_file_dependency_list )
				{
					// cache file is out of sync, need to recompile and recache this shader.
					_compile_and_save_to_cache( shader_variations, variation );
				}
				else
				{
					// source file and cache are in sync, so load the cached shader.
					if ( !_load_from_cache( shader_variations, variation ) )
					{
						// the cache load failed, so try to recompile from the source file.
						_compile_and_save_to_cache( shader_variations, variation );
					}
				}
			}
		}
	}

	boolean_c video_renderer_shader_manager_c::_compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation )
	{
		data_stream_file_c stream;

		string16_c cache_folder_path;
		cache_folder_path = ops::path_get_folder_path( shader_variations->_source_file._absolute_file_path );
#if defined( cheonsa_platform_windows )
		cache_folder_path += "cache\\";
#else
#error
#endif

		if ( !ops::file_system_does_folder_exist( cache_folder_path ) )
		{
			ops::file_system_create_folder( cache_folder_path );
		}

		if ( !stream.open( shader_variations->_source_file._absolute_file_path, data_stream_mode_e_read ) )
		{
			return false;
		}

		core_list_c< char8_c > data;
		sint32_c data_size = stream.get_size();
		data.construct_mode_dynamic( data_size + 1, data_size + 1 ); // + 1 for null character for safety.
		data[ data_size ] = 0;
		stream.load( data.get_internal_array(), data_size );
		stream.close();

		if ( variation == variation_e_vs )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_vs );
		}
		else if ( variation == variation_e_vs_waved )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_waved );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_vs_waved );
		}
		else if ( variation == variation_e_vs_clipped )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_clipped );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_vs_clipped );
		}
		else if ( variation == variation_e_vs_waved_clipped )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_clipped );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_vs_waved_clipped );
		}
		else if ( variation == variation_e_ps )
		{
			assert( shader_variations->_ps );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_ps );
		}
		else if ( variation == variation_e_ps_masked )
		{
			assert( shader_variations->_ps_masked );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size() - 1, shader_variations->_ps_masked );
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation, char8_c const * source_code, sint32_c source_code_size, video_vertex_shader_c const * const * result )
	{
		string16_c cache_file_path_absolute;
		_resolve_cache_file_path_absolute( shader_variations->_source_file, variation, cache_file_path_absolute );
		string16_c cache_file_name = ops::path_get_file_or_folder_name( cache_file_path_absolute );
		core_list_c< char8_c > source_file_path_absolute; // need 8 bit path string for shader compiler.
		ops::convert_string16_to_string8( shader_variations->_source_file._absolute_file_path.character_list, source_file_path_absolute );
		video_shader_includer_c shader_includer;
		video_vertex_shader_c * shader = engine.get_video_interface()->create_vertex_shader( cache_file_name.character_list.get_internal_array(), source_code, source_code_size, source_file_path_absolute.get_internal_array(), &shader_includer, get_variation_shader_defines( variation ), shader_variations->_input_vertex_layout );
		if ( shader )
		{
			if ( shader_variations->_output_vertex_format && variation == variation_e_vs )
			{
				shader->enable_stream_out( shader_variations->_output_vertex_format );
			}
			void_c const * compiled_code;
			sint32_c compiled_code_size;
			shader->get_compiled_code( compiled_code, compiled_code_size );
			__save_to_cache( shader_variations, cache_file_path_absolute, compiled_code, compiled_code_size );
			if ( *result != nullptr )
			{
				delete *result;
			}
			const_cast< video_vertex_shader_c const * >( *result ) = shader;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation, char8_c const * source_code, sint32_c source_code_size, video_pixel_shader_c const * const * result )
	{
		string16_c cache_file_path_absolute;
		_resolve_cache_file_path_absolute( shader_variations->_source_file, variation, cache_file_path_absolute );
		string16_c cache_file_name = ops::path_get_file_or_folder_name( cache_file_path_absolute );
		core_list_c< char8_c > source_file_path_absolute; // need 8 bit path string for shader compiler.
		ops::convert_string16_to_string8( shader_variations->_source_file._absolute_file_path.character_list, source_file_path_absolute );
		video_shader_includer_c shader_includer;
		video_pixel_shader_c * shader = engine.get_video_interface()->create_pixel_shader( cache_file_name.character_list.get_internal_array(), source_code, source_code_size, source_file_path_absolute.get_internal_array(), &shader_includer, get_variation_shader_defines( variation ) );
		if ( shader )
		{
			void_c const * compiled_code;
			sint32_c compiled_code_size;
			shader->get_compiled_code( compiled_code, compiled_code_size );
			
			__save_to_cache( shader_variations, cache_file_path_absolute, compiled_code, compiled_code_size );
			if ( *result != nullptr )
			{
				delete *result;
			}
			const_cast< video_pixel_shader_c const * >( *result ) = shader;
			return true;
		}
		return false;
	}

	void_c video_renderer_shader_manager_c::__save_to_cache( shader_variations_c * shader_variations, string16_c const & cache_file_path_absolute, void_c const * compiled_code, sint32_c compiled_code_size )
	{
		data_stream_file_c stream;
		if ( stream.open( cache_file_path_absolute, data_stream_mode_e_write ) )
		{
			data_scribe_binary_c scribe_binary;
			scribe_binary.set_stream( &stream );

			sint32_c compiled_code_position = 0;
			scribe_binary.save_sint32( 0 ); // place holder for where we will save the location of compiled code.
			scribe_binary.save_sint64( shader_variations->_source_file._last_write_time );
			assert( shader_variations->_source_file_dependency_list.get_length() < 0xFF );
			scribe_binary.save_uint8( static_cast< uint8_c >( shader_variations->_source_file_dependency_list.get_length() ) );
			for ( sint32_c i = 0; i < shader_variations->_source_file_dependency_list.get_length(); i++ )
			{
				file_dependency_c & file_dependency = shader_variations->_source_file_dependency_list[i];
				scribe_binary.save_string8( string8_c( file_dependency._file_name ) );
				scribe_binary.save_sint64( file_dependency._last_write_time );
			}
			compiled_code_position = stream.get_position();
			scribe_binary.save_sint32( compiled_code_size );
			stream.save( compiled_code, compiled_code_size );
			stream.set_position( 0 );
			scribe_binary.save_sint32( compiled_code_position );
			stream.close();
		}
	}

	boolean_c video_renderer_shader_manager_c::_load_from_cache( shader_variations_c * shader_variations, variation_e variation )
	{
		string16_c cache_file_path_absolute;
		_resolve_cache_file_path_absolute( shader_variations->_source_file, variation, cache_file_path_absolute );
		if ( variation == variation_e_vs )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs );
			if ( !__load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->_vs ), const_cast< video_vertex_layout_c * >( shader_variations->_input_vertex_layout ) ) )
			{
				return false;
			}
			if ( shader_variations->_output_vertex_format )
			{
				const_cast< video_vertex_shader_c * >( *shader_variations->_vs )->enable_stream_out( shader_variations->_output_vertex_format );
			}
			return true;
		}
		else if ( variation == variation_e_vs_waved )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_waved );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->_vs_waved ), const_cast< video_vertex_layout_c * >( shader_variations->_input_vertex_layout ) );
		}
		else if ( variation == variation_e_vs_clipped )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_clipped );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->_vs_clipped ), const_cast< video_vertex_layout_c * >( shader_variations->_input_vertex_layout ) );
		}
		else if ( variation == variation_e_vs_waved_clipped )
		{
			assert( shader_variations->_input_vertex_layout && shader_variations->_vs_waved_clipped );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->_vs_waved_clipped ), const_cast< video_vertex_layout_c * >( shader_variations->_input_vertex_layout ) );
		}
		else if ( variation == variation_e_ps )
		{
			assert( shader_variations->_ps );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_pixel_shader_c * * >( shader_variations->_ps ) );
		}
		else if ( variation == variation_e_ps_masked )
		{
			assert( shader_variations->_ps_masked );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_pixel_shader_c * * >( shader_variations->_ps_masked ) );
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__load_from_cache( string16_c const & cache_file_path_absolute, video_vertex_shader_c * * result, video_vertex_layout_c const * vertex_layout )
	{
		string16_c cache_file_name = ops::path_get_file_or_folder_name( cache_file_path_absolute );

		data_stream_file_c stream;
		if ( stream.open( cache_file_path_absolute, data_stream_mode_e_read ) )
		{
			data_scribe_binary_c scribe_binary;
			scribe_binary.set_stream( &stream );

			sint32_c compiled_code_position = 0;
			if ( !scribe_binary.load_sint32( compiled_code_position ) )
			{
				return false;
			}
			if ( !stream.set_position( compiled_code_position ) )
			{
				return false;
			}
			sint32_c compiled_code_size = 0;
			if ( !scribe_binary.load_sint32( compiled_code_size ) )
			{
				return false;
			}
			core_list_c< char8_c > compiled_code;
			compiled_code.construct_mode_dynamic( compiled_code_size, compiled_code_size );
			if ( !stream.load( compiled_code.get_internal_array(), compiled_code_size ) )
			{
				return false;
			}
			stream.close();

			video_vertex_shader_c * shader = engine.get_video_interface()->create_vertex_shader( cache_file_name.character_list.get_internal_array(), compiled_code.get_internal_array(), compiled_code_size, vertex_layout );
			if ( shader != nullptr )
			{
				if ( *result != nullptr)
				{
					delete *result;
				}
				*result = shader;
				return true;
			}
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__load_from_cache( string16_c const & cache_file_path_absolute, video_pixel_shader_c * * result )
	{
		string16_c cache_file_name = ops::path_get_file_or_folder_name( cache_file_path_absolute );

		data_stream_file_c stream;
		if ( stream.open( cache_file_path_absolute, data_stream_mode_e_read ) )
		{
			data_scribe_binary_c scribe_binary;
			scribe_binary.set_stream( &stream );

			sint32_c compiled_code_position = 0;
			if ( !scribe_binary.load_sint32( compiled_code_position ) )
			{
				return false;
			}
			stream.set_position( compiled_code_position );
			sint32_c compiled_code_size = 0;
			if ( !scribe_binary.load_sint32( compiled_code_size ) )
			{
				return false;
			}
			core_list_c< char8_c > compiled_code;
			compiled_code.set_length_absolute( compiled_code_size );
			if ( !stream.load( compiled_code.get_internal_array(), compiled_code_size ) )
			{
				return false;
			}
			stream.close();

			video_pixel_shader_c * shader = engine.get_video_interface()->create_pixel_shader( cache_file_name.character_list.get_internal_array(), compiled_code.get_internal_array(), compiled_code_size );
			if ( shader != nullptr )
			{
				if ( *result != nullptr )
				{
					delete *result;
				}
				*result = shader;
				return true;
			}
		}
		return false;
	}

	video_renderer_shader_manager_c::video_renderer_shader_manager_c()
		: _skin_mesh( nullptr )
		, _menu_ps_debug( nullptr )
		, _menu_ps_frame( nullptr )
		, _menu_ps_frame_keyed( nullptr )
		, _menu_ps_solid_color( nullptr )
		, _menu_ps_text( nullptr )
		, _menu2_vs( nullptr )
		, _menu2_vs_debug( nullptr )
		, _menu3_vs( nullptr )
		, _menu3_vs_debug( nullptr )
		, _scene_camera_normal_and_depth_ps_mesh( nullptr )
		, _scene_camera_normal_and_depth_ps_mesh_masked( nullptr )
		, _scene_camera_normal_and_depth_vs_mesh( nullptr )
		, _scene_camera_normal_and_depth_vs_mesh_waved( nullptr )
		, _scene_camera_normal_and_depth_vs_mesh_clipped( nullptr )
		, _scene_camera_normal_and_depth_vs_mesh_waved_clipped( nullptr )
		, _scene_camera_outline_ps_mesh( nullptr )
		, _scene_camera_outline_ps_mesh_masked( nullptr )
		, _scene_camera_color_ps_debug( nullptr )
		, _scene_camera_color_vs_debug( nullptr )
		, _scene_camera_color_vs_mesh( nullptr )
		, _scene_camera_color_vs_mesh_waved( nullptr )
		, _scene_camera_color_vs_mesh_clipped( nullptr )
		, _scene_camera_color_vs_mesh_waved_clipped( nullptr )
		, _scene_shadow_ps_mesh( nullptr )
		, _scene_shadow_ps_mesh_masked( nullptr )
		, _scene_shadow_vs_mesh( nullptr )
		, _scene_shadow_vs_mesh_waved( nullptr )
		, _scene_post_ps_blur_x( nullptr )
		, _scene_post_ps_blur_y( nullptr )
		, _scene_post_ps_resolve_native( nullptr )
		, _scene_post_ps_resolve_scaled( nullptr )
		//, _scene_post_ps_resolve_quarter( nullptr )
		, _scene_post_ps_process( nullptr )
		, _scene_post_vs( nullptr )
		, _scene_camera_color_ps_mesh( nullptr )
		, _internal_shader_list()
		, _external_shader_list()
	{
	}

	video_renderer_shader_manager_c::~video_renderer_shader_manager_c()
	{
		_internal_shader_list.remove_and_delete_all();

		if ( _scene_camera_color_ps_mesh != nullptr )
		{
			_scene_camera_color_ps_mesh->remove_reference();
			_scene_camera_color_ps_mesh = nullptr;
		}

		//_external_shader_list.remove_and_delete_all();
		for ( sint32_c i = 0; i < _external_shader_list.get_length(); i++ )
		{
			_external_shader_list[ i ]->remove_reference();
		}
		_external_shader_list.remove_all();
	}

	boolean_c video_renderer_shader_manager_c::start()
	{
		shader_variations_c * shader_variations = nullptr;

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"skin_mesh.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base_and_bone_weight;
		shader_variations->_output_vertex_format = &video_renderer_interface_c::vertex_format_mesh_base;
		shader_variations->_vs = &_skin_mesh;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu_ps_debug.hlsl" ), true );
		shader_variations->_ps = &_menu_ps_debug;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu_ps_frame.hlsl" ), true );
		shader_variations->_ps = &_menu_ps_frame;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu_ps_frame_keyed.hlsl" ), true );
		shader_variations->_ps = &_menu_ps_frame_keyed;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu_ps_solid_color.hlsl" ), true );
		shader_variations->_ps = &_menu_ps_solid_color;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu_ps_text.hlsl" ), true );
		shader_variations->_ps = &_menu_ps_text;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu2_vs.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_menu;
		shader_variations->_vs = &_menu2_vs;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu2_vs_debug.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->_vs = &_menu2_vs_debug;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu3_vs.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_menu;
		shader_variations->_vs = &_menu3_vs;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"menu3_vs_debug.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->_vs = &_menu3_vs_debug;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_normal_and_depth_ps_mesh.hlsl" ), true );
		shader_variations->_ps = &_scene_camera_normal_and_depth_ps_mesh;
		shader_variations->_ps_masked = &_scene_camera_normal_and_depth_ps_mesh_masked;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_normal_and_depth_vs_mesh.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->_vs = &_scene_camera_normal_and_depth_vs_mesh;
		shader_variations->_vs_waved = &_scene_camera_normal_and_depth_vs_mesh_waved;
		shader_variations->_vs_clipped = &_scene_camera_normal_and_depth_vs_mesh_clipped;
		shader_variations->_vs_waved_clipped = &_scene_camera_normal_and_depth_vs_mesh_waved_clipped;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_outline_ps_mesh.hlsl" ), true );
		shader_variations->_ps = &_scene_camera_outline_ps_mesh;
		shader_variations->_ps_masked = &_scene_camera_outline_ps_mesh_masked;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_color_ps_debug.hlsl" ), true );
		shader_variations->_ps = &_scene_camera_color_ps_debug;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_color_vs_debug.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->_vs = &_scene_camera_color_vs_debug;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_camera_color_vs_mesh.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->_vs = &_scene_camera_color_vs_mesh;
		shader_variations->_vs_waved = &_scene_camera_color_vs_mesh_waved;
		shader_variations->_vs_clipped = &_scene_camera_color_vs_mesh_clipped;
		shader_variations->_vs_waved_clipped = &_scene_camera_color_vs_mesh_waved_clipped;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_shadow_ps_mesh.hlsl" ), true );
		shader_variations->_ps = &_scene_shadow_ps_mesh;
		shader_variations->_ps_masked = &_scene_shadow_ps_mesh_masked;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_shadow_vs_mesh.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->_vs = &_scene_shadow_vs_mesh;
		shader_variations->_vs_waved = &_scene_shadow_vs_mesh_waved;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_blur_x.hlsl" ), true );
		shader_variations->_ps = &_scene_post_ps_blur_x;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_blur_y.hlsl" ), true );
		shader_variations->_ps = &_scene_post_ps_blur_y;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_resolve_native.hlsl" ), true );
		shader_variations->_ps = &_scene_post_ps_resolve_native;
		_internal_shader_list.insert( -1, shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_resolve_scaled.hlsl" ), true );
		shader_variations->_ps = &_scene_post_ps_resolve_scaled;
		_internal_shader_list.insert( -1, shader_variations );

		//shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_resolve_quarter.hlsl" ), true );
		//shader_variations->ps = &_scene_post_ps_resolve_quarter;
		//_internal_shader_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_ps_process.hlsl" ), true );
		shader_variations->_ps = &_scene_post_ps_process;
		_internal_shader_list.insert( -1, shader_variations );

		//shader_variations = new shader_variations_c( "scene_post_ps_resolve.hlsl", true );
		//shader_variations->ps_msaa_count_1 = &scene_post_ps_resolve_msaa_count_1;
		//shader_variations->ps_msaa_count_2 = &scene_post_ps_resolve_msaa_count_2;
		//shader_variations->ps_msaa_count_4 = &scene_post_ps_resolve_msaa_count_4;
		//shader_variations->ps_msaa_count_8 = &scene_post_ps_resolve_msaa_count_8;
		//_internal_shader_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( core_list_mode_e_static, L"scene_post_vs.hlsl" ), true );
		shader_variations->_input_vertex_layout = &video_renderer_interface_c::vertex_layout_point;
		shader_variations->_vs = &_scene_post_vs;
		_internal_shader_list.insert( -1, shader_variations );

		// load built in shaders.
		if ( !refresh() )
		{
			debug_annoy( L"error", L"could not load a built in shader." );
			return false;
		}

		// load built in mesh material shader.
		_scene_camera_color_ps_mesh = load_ps( string16_c( "scene_camera_color_ps_mesh.hlsl" ) );
		if ( !_scene_camera_color_ps_mesh )
		{
			debug_annoy( L"error", L"could not load a built in material shader." );
			return false;
		}
		_scene_camera_color_ps_mesh->add_reference(); // add one fake user to keep this shader loaded at all times, it's basically built in.

		return true;
	}

	boolean_c video_renderer_shader_manager_c::refresh()
	{
		assert( engine.get_resource_manager() != nullptr );
		assert( engine.get_video_interface() != nullptr );

		for ( sint32_c i = 0; i < _external_shader_list.get_length(); i++ )
		{
			_refresh_shader_variations( _external_shader_list[ i ]->_shader_variations );
		}

		for ( sint32_c i = 0; i < _internal_shader_list.get_length(); i++ )
		{
			_refresh_shader_variations( _internal_shader_list[ i ] );
		}

		// check that all required internal shaders are loaded.
		// otherwise if a shader fails to load during run time, then the shader that is already loaded and working continues to be the one that is used.
		for ( sint32_c i = 0; i < _internal_shader_list.get_length(); i++ )
		{
			shader_variations_c * shader_variations = _internal_shader_list[ i ];
			if ( shader_variations->_input_vertex_layout )
			{
				if ( shader_variations->_vs != nullptr )
				{
					if ( *shader_variations->_vs == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->_vs_waved != nullptr )
				{
					if ( *shader_variations->_vs_waved == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->_vs_clipped != nullptr )
				{
					if ( *shader_variations->_vs_clipped == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->_vs_waved_clipped != nullptr )
				{
					if ( *shader_variations->_vs_waved_clipped == nullptr )
					{
						return false;
					}
				}
			}
			else
			{
				if ( shader_variations->_ps != nullptr )
				{
					if ( *shader_variations->_ps == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->_ps_masked != nullptr )
				{
					if ( *shader_variations->_ps_masked == nullptr )
					{
						return false;
					}
				}
				//if ( shader_variations->ps_msaa_count_1 )
				//{
				//	assert( *shader_variations->ps_msaa_count_1 );
				//}
				//if ( shader_variations->ps_msaa_count_2 )
				//{
				//	assert( *shader_variations->ps_msaa_count_2 );
				//}
				//if ( shader_variations->ps_msaa_count_4 )
				//{
				//	assert( *shader_variations->ps_msaa_count_4 );
				//}
				//if ( shader_variations->ps_msaa_count_8 )
				//{
				//	assert( *shader_variations->ps_msaa_count_8 );
				//}
			}
		}

		return true;
	}

	void_c video_renderer_shader_manager_c::collect_garbage()
	{
		// delete any material pixel shaders that have a & count of 0.
		for ( sint32_c i = 0; i < _external_shader_list.get_length(); i++ )
		{
			video_renderer_pixel_shader_c * material_pixel_shader = _external_shader_list[ i ];
			if ( material_pixel_shader->_reference_count == 0 )
			{
				shader_variations_c * shader_variations = material_pixel_shader->_shader_variations;
				_internal_shader_list.remove_value( shader_variations );
				delete shader_variations;
				_external_shader_list.remove( i, 1 );
				delete material_pixel_shader;
				i--;
			}
		}
	}

	video_renderer_pixel_shader_c * video_renderer_shader_manager_c::load_ps( string16_c const & file_name )
	{
		if ( !file_name.is_set() )
		{
			return nullptr;
		}

		// search for existing.
		for ( sint32_c i = 0; i < _external_shader_list.get_length(); i++ )
		{
			shader_variations_c * shader_variations = _external_shader_list[ i ]->_shader_variations;
			if ( shader_variations->_source_file._file_name == file_name )
			{
				return _external_shader_list[ i ];
			}
		}

		// create new.
		video_renderer_pixel_shader_c * renderer_pixel_shader = new video_renderer_pixel_shader_c( file_name );
		renderer_pixel_shader->add_reference(); // add ourselves as a "fake" user so that the instance doesn't inadvertently delete itself
		_external_shader_list.insert( -1, renderer_pixel_shader );
		_refresh_shader_variations( renderer_pixel_shader->_shader_variations );
		return renderer_pixel_shader;
	}

	boolean_c video_renderer_shader_manager_c::resolve_file_path( string16_c const & relative_file_path, boolean_c is_internal, string16_c & absolute_file_path )
	{
		string16_c result;

		// scan game data folders.
		if ( is_internal == false )
		{
			core_list_c< string16_c > const & game_data_folder_path_list = engine.get_content_manager()->get_game_data_folder_path_list();
			for ( sint32_c i = game_data_folder_path_list.get_length() - 1; i >= 0; i-- )
			{
				result = game_data_folder_path_list[ i ];
#if defined( cheonsa_platform_windows )
				result += "_common\\shaders\\";
#else
#error
#endif
				//result += ops::file_system_convert_path_format_from_cheonsa_to_windows( relative_file_path );
				result += relative_file_path;
				if ( ops::file_system_does_file_exist( result ) )
				{
					absolute_file_path = result;
					return true;
				}
			}
		}

		// scan engine data folder.
		result = engine.get_content_manager()->get_engine_data_folder_path();
#if defined( cheonsa_platform_windows )
		result += "_common\\shaders\\";
#else
#error
#endif
		result += ops::file_system_convert_path_format_from_cheonsa_to_windows( relative_file_path );
		if ( ops::file_system_does_file_exist( result ) )
		{
			absolute_file_path = result;
			return true;
		}

		return false;
	}

}
