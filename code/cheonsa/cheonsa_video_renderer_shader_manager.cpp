#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	video_renderer_pixel_shader_c::video_renderer_pixel_shader_c()
		: _ps( nullptr )
		, _ps_masked( nullptr )
		, _shader_variations( nullptr )
		, _reference_count( 0 )
	{
	}

	video_renderer_pixel_shader_c::~video_renderer_pixel_shader_c()
	{
		assert( _reference_count == 0 );
		assert( _shader_variations );
		delete _shader_variations;
		_shader_variations = nullptr;
	}

	video_renderer_pixel_shader_c::reference_c::reference_c()
		: _value( nullptr )
	{
	}

	video_renderer_pixel_shader_c::reference_c::~reference_c()
	{
		if ( _value )
		{
			_value->_reference_count--;
		}
	}

	video_renderer_pixel_shader_c::reference_c & video_renderer_pixel_shader_c::reference_c::operator = ( video_renderer_pixel_shader_c::reference_c const & other )
	{
		if ( other._value != nullptr )
		{
			other._value->_reference_count++;
		}
		if ( _value != nullptr )
		{
			_value->_reference_count--;
		}
		_value = other._value;
		return *this;
	}

	video_renderer_pixel_shader_c::reference_c & video_renderer_pixel_shader_c::reference_c::operator = ( video_renderer_pixel_shader_c * other )
	{
		if ( other != nullptr )
		{
			other->_reference_count++;
		}
		if ( _value != nullptr )
		{
			_value->_reference_count--;
		}
		_value = other;
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

	boolean_c video_renderer_pixel_shader_c::reference_c::get_loaded() const
	{
		return _value != nullptr && _value->_ps;
	}

	video_pixel_shader_c * video_renderer_pixel_shader_c::reference_c::get_pixel_shader() const
	{
		if ( _value )
		{
			return _value->_ps;
		}
		return nullptr;
	}

	video_pixel_shader_c * video_renderer_pixel_shader_c::reference_c::get_pixel_shader_masked() const
	{
		if ( _value )
		{
			return _value->_ps_masked;
		}
		return nullptr;
	}

	video_renderer_shader_manager_c::file_dependency_c::file_dependency_c()
		: file_name()
		, file_path_absolute()
		, file_modified_time( 0 )
	{
	}

	video_renderer_shader_manager_c::file_dependency_c & video_renderer_shader_manager_c::file_dependency_c::operator = ( file_dependency_c const & other )
	{
		file_name = other.file_name;
		file_path_absolute = other.file_path_absolute;
		file_modified_time = other.file_modified_time;
		return *this;
	}

	boolean_c video_renderer_shader_manager_c::file_dependency_c::operator == ( file_dependency_c const & other ) const
	{
		return file_name == other.file_name && file_modified_time == other.file_modified_time;
	}

	boolean_c video_renderer_shader_manager_c::file_dependency_c::operator != ( file_dependency_c const & other ) const
	{
		return file_name != other.file_name || file_modified_time != other.file_modified_time;
	}

	void_c video_renderer_shader_manager_c::_resolve_cache_file_path_absolute( file_dependency_c const & source_file_dependency, variation_e variation, string16_c & result )
	{
		result = ops::path_get_mother( source_file_dependency.file_path_absolute );
		result += "cache/";
		result += ops::path_get_file_name_without_extension( source_file_dependency.file_name );
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
		: is_internal( is_internal )
		, source_file()
		, source_file_dependency_list()
		, input_vertex_layout( nullptr )
		, output_vertex_format( nullptr )
		, vs( nullptr )
		, vs_waved( nullptr )
		, vs_clipped( nullptr )
		, vs_waved_clipped( nullptr )
		, ps( nullptr )
		, ps_masked( nullptr )
	{
		source_file.file_name = file_name;
		resolve_file_path( source_file.file_name, is_internal, source_file.file_path_absolute );
	}

	video_renderer_shader_manager_c::shader_variations_c::~shader_variations_c()
	{
		if ( vs && *vs )
		{
			delete *vs;
			const_cast< video_vertex_shader_c const * >( *vs ) = nullptr;
		}
		if ( vs_waved && *vs_waved )
		{
			delete *vs_waved;
			const_cast< video_vertex_shader_c const * >( *vs_waved ) = nullptr;
		}
		if ( vs_clipped && *vs_clipped )
		{
			delete *vs_clipped;
			const_cast< video_vertex_shader_c const * >( *vs_clipped ) = nullptr;
		}
		if ( vs_waved_clipped && *vs_waved_clipped )
		{
			delete *vs_waved_clipped;
			const_cast< video_vertex_shader_c const * >( *vs_waved_clipped ) = nullptr;
		}
		if ( ps && *ps )
		{
			delete *ps;
			const_cast< video_pixel_shader_c const * >( *ps ) = nullptr;
		}
		if ( ps_masked && *ps_masked )
		{
			delete *ps_masked;
			const_cast< video_pixel_shader_c const * >( *ps_masked ) = nullptr;
		}
	}

	boolean_c video_renderer_shader_manager_c::shader_variations_c::has_variation( variation_e variation )
	{
		switch ( variation )
		{
		case variation_e_vs:
			return vs != nullptr;
		case variation_e_vs_waved:
			return vs_waved != nullptr;
		case variation_e_vs_clipped:
			return vs_clipped != nullptr;
		case variation_e_vs_waved_clipped:
			return vs_waved_clipped != nullptr;
		case variation_e_ps:
			return ps != nullptr;
		case variation_e_ps_masked:
			return ps_masked != nullptr;
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::_load_source_dependency_information( shader_variations_c * shader_variations )
	{
		shader_variations->source_file_dependency_list.remove_all();
		if ( !ops::data_get_file_or_folder_modified_time( shader_variations->source_file.file_path_absolute, shader_variations->source_file.file_modified_time ) )
		{
			return false;
		}
		return __load_source_dependency_list_recursive( shader_variations->source_file, shader_variations->is_internal, shader_variations->source_file_dependency_list );
	}

	// given file_name that references a shader file, recursively scans for additional file names referenced by #include directives in that shader file.
	// populates result for each additional file name that is found to be referenced.
	boolean_c video_renderer_shader_manager_c::__load_source_dependency_list_recursive( file_dependency_c const & source_file_dependency, boolean_c is_internal, core_list_c< file_dependency_c > & result )
	{
		// load the whole file into memory.
		data_stream_file_c stream;
		if ( !stream.open( source_file_dependency.file_path_absolute, data_stream_mode_e_read ) )
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
		ops::string8_find_indices_of( data, string8_c( mode_e_static, "#include" ), instances );

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
				else if ( !ops::char16_is_valid_for_file_path( *scanner ) )
				{
					// illegal character, we can abort.
					return false;
				}
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
					if ( result[ j ].file_name == file_name )
					{
						already_in_result = true;
						break;
					}
				}
				if ( already_in_result == false )
				{
					video_renderer_shader_manager_c::file_dependency_c & sub_file_dependency = *result.emplace_at_end();
					sub_file_dependency.file_name = file_name;
					resolve_file_path( sub_file_dependency.file_name, is_internal, sub_file_dependency.file_path_absolute );
					ops::data_get_file_or_folder_modified_time( sub_file_dependency.file_path_absolute, sub_file_dependency.file_modified_time );
					__load_source_dependency_list_recursive( sub_file_dependency, is_internal, result ); // recurse.
				}
			}
		}

		return true;
	}

	boolean_c video_renderer_shader_manager_c::_load_cached_dependency_information( shader_variations_c * shader_variations, variation_e variation, string16_c & file_path_absolute, sint64_c & file_modified_time, core_list_c< file_dependency_c > & file_dependency_list )
	{
		file_modified_time = 0;
		file_dependency_list.remove_all();
		file_path_absolute = ops::path_get_mother( shader_variations->source_file.file_path_absolute );
		file_path_absolute += "cache/";
		file_path_absolute += ops::path_get_file_name_without_extension( shader_variations->source_file.file_name );
		file_path_absolute += get_variation_suffix( variation );
		file_path_absolute += ".so";

		data_stream_file_c stream;
		if ( !stream.open( file_path_absolute, data_stream_mode_e_read ) )
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
		if ( !scribe_binary.load_sint64( file_modified_time ) )
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
			file_dependency.file_name = file_dependency_file_name_string8;
			if ( !scribe_binary.load_sint64( file_dependency.file_modified_time ) )
			{
				return false;
			}
		}
		stream.close();

		return true;
	}

	void_c video_renderer_shader_manager_c::_refresh_shader_variations( shader_variations_c * shader_variations )
	{
		assert( shader_variations->source_file.file_name.get_length() > 0 );
		assert( shader_variations->source_file.file_path_absolute.get_length() > 0 );

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
				sint64_c cache_file_modified_time = 0;
				core_list_c< file_dependency_c > cache_file_dependency_list; // file names of included files, and their file modified times.
				boolean_c has_cached = _load_cached_dependency_information( shader_variations, variation, cache_file_path_absolute, cache_file_modified_time, cache_file_dependency_list );

				// compare for differences between source file and cache file.
				if ( has_cached == false || shader_variations->source_file.file_modified_time != cache_file_modified_time || shader_variations->source_file_dependency_list != cache_file_dependency_list )
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
		cache_folder_path = ops::path_get_mother( shader_variations->source_file.file_path_absolute );
		cache_folder_path += "cache/";

		if ( !ops::data_does_folder_exist( cache_folder_path ) )
		{
			ops::data_create_folder( cache_folder_path );
		}

		if ( !stream.open( shader_variations->source_file.file_path_absolute, data_stream_mode_e_read ) )
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
			assert( shader_variations->input_vertex_layout && shader_variations->vs );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->vs );
		}
		else if ( variation == variation_e_vs_waved )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_waved );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->vs_waved );
		}
		else if ( variation == variation_e_vs_clipped )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_clipped );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->vs_clipped );
		}
		else if ( variation == variation_e_vs_waved_clipped )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_clipped );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->vs_waved_clipped );
		}
		else if ( variation == variation_e_ps )
		{
			assert( shader_variations->ps );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->ps );
		}
		else if ( variation == variation_e_ps_masked )
		{
			assert( shader_variations->ps_masked );
			return __compile_and_save_to_cache( shader_variations, variation, data.get_internal_array(), data.get_internal_array_size_used() - 1, shader_variations->ps_masked );
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation, char8_c const * source_code, sint32_c source_code_size, video_vertex_shader_c const * const * result )
	{
		string16_c cache_file_path_absolute;
		_resolve_cache_file_path_absolute( shader_variations->source_file, variation, cache_file_path_absolute );
		string16_c cache_file_name = ops::path_get_file_name( cache_file_path_absolute );
		core_list_c< char8_c > source_file_path_absolute; // need 8 bit path string for shader compiler.
		ops::convert_string16_to_string8( shader_variations->source_file.file_path_absolute.character_list, source_file_path_absolute );
		video_shader_includer_c shader_includer;
		video_vertex_shader_c * shader = engine_c::get_instance()->get_video_interface()->create_vertex_shader( cache_file_name.character_list.get_internal_array(), source_code, source_code_size, source_file_path_absolute.get_internal_array(), &shader_includer, get_variation_shader_defines( variation ), shader_variations->input_vertex_layout );
		if ( shader )
		{
			if ( shader_variations->output_vertex_format && variation == variation_e_vs )
			{
				shader->enable_stream_out( shader_variations->output_vertex_format );
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
		_resolve_cache_file_path_absolute( shader_variations->source_file, variation, cache_file_path_absolute );
		string16_c cache_file_name = ops::path_get_file_name( cache_file_path_absolute );
		core_list_c< char8_c > source_file_path_absolute; // need 8 bit path string for shader compiler.
		ops::convert_string16_to_string8( shader_variations->source_file.file_path_absolute.character_list, source_file_path_absolute );
		video_shader_includer_c shader_includer;
		video_pixel_shader_c * shader = engine_c::get_instance()->get_video_interface()->create_pixel_shader( cache_file_name.character_list.get_internal_array(), source_code, source_code_size, source_file_path_absolute.get_internal_array(), &shader_includer, get_variation_shader_defines( variation ) );
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
			scribe_binary.save_sint64( shader_variations->source_file.file_modified_time );
			assert( shader_variations->source_file_dependency_list.get_length() < 0xFF );
			scribe_binary.save_uint8( static_cast< uint8_c >( shader_variations->source_file_dependency_list.get_length() ) );
			for ( sint32_c i = 0; i < shader_variations->source_file_dependency_list.get_length(); i++ )
			{
				file_dependency_c & file_dependency = shader_variations->source_file_dependency_list[i];
				scribe_binary.save_string8( string8_c( file_dependency.file_name ) );
				scribe_binary.save_sint64( file_dependency.file_modified_time );
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
		_resolve_cache_file_path_absolute( shader_variations->source_file, variation, cache_file_path_absolute );
		if ( variation == variation_e_vs )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs );
			if ( !__load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->vs ), const_cast< video_vertex_layout_c * >( shader_variations->input_vertex_layout ) ) )
			{
				return false;
			}
			if ( shader_variations->output_vertex_format )
			{
				const_cast< video_vertex_shader_c * >( *shader_variations->vs )->enable_stream_out( shader_variations->output_vertex_format );
			}
			return true;
		}
		else if ( variation == variation_e_vs_waved )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_waved );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->vs_waved ), const_cast< video_vertex_layout_c * >( shader_variations->input_vertex_layout ) );
		}
		else if ( variation == variation_e_vs_clipped )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_clipped );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->vs_clipped ), const_cast< video_vertex_layout_c * >( shader_variations->input_vertex_layout ) );
		}
		else if ( variation == variation_e_vs_waved_clipped )
		{
			assert( shader_variations->input_vertex_layout && shader_variations->vs_waved_clipped );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_vertex_shader_c * * >( shader_variations->vs_waved_clipped ), const_cast< video_vertex_layout_c * >( shader_variations->input_vertex_layout ) );
		}
		else if ( variation == variation_e_ps )
		{
			assert( shader_variations->ps );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_pixel_shader_c * * >( shader_variations->ps ) );
		}
		else if ( variation == variation_e_ps_masked )
		{
			assert( shader_variations->ps_masked );
			return __load_from_cache( cache_file_path_absolute, const_cast< video_pixel_shader_c * * >( shader_variations->ps_masked ) );
		}
		return false;
	}

	boolean_c video_renderer_shader_manager_c::__load_from_cache( string16_c const & cache_file_path_absolute, video_vertex_shader_c * * result, video_vertex_layout_c const * vertex_layout )
	{
		string16_c cache_file_name = ops::path_get_file_name( cache_file_path_absolute );

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

			video_vertex_shader_c * shader = engine_c::get_instance()->get_video_interface()->create_vertex_shader( cache_file_name.character_list.get_internal_array(), compiled_code.get_internal_array(), compiled_code_size, vertex_layout );
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
		string16_c cache_file_name = ops::path_get_file_name( cache_file_path_absolute );

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

			video_pixel_shader_c * shader = engine_c::get_instance()->get_video_interface()->create_pixel_shader( cache_file_name.character_list.get_internal_array(), compiled_code.get_internal_array(), compiled_code_size );
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
		, _scene_post_ps_resolve_quarter( nullptr )
		, _scene_post_ps_process( nullptr )
		, _scene_post_vs( nullptr )
		, _scene_camera_color_ps_mesh( nullptr )
		, _shader_variations_list()
		, _material_pixel_shader_list()
	{
	}

	video_renderer_shader_manager_c::~video_renderer_shader_manager_c()
	{
		_shader_variations_list.remove_and_delete_all();

		_scene_camera_color_ps_mesh->_reference_count--; // remove the fake user.
		_material_pixel_shader_list.remove_and_delete_all();
	}

	boolean_c video_renderer_shader_manager_c::start()
	{
		shader_variations_c * shader_variations = nullptr;

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"skin_mesh.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base_and_bone_weight;
		shader_variations->output_vertex_format = &video_renderer_interface_c::vertex_format_mesh_base;
		shader_variations->vs = &_skin_mesh;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu_ps_debug.hlsl" ), true );
		shader_variations->ps = &_menu_ps_debug;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu_ps_frame.hlsl" ), true );
		shader_variations->ps = &_menu_ps_frame;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu_ps_solid_color.hlsl" ), true );
		shader_variations->ps = &_menu_ps_solid_color;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu_ps_text.hlsl" ), true );
		shader_variations->ps = &_menu_ps_text;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu2_vs.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_menu;
		shader_variations->vs = &_menu2_vs;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu2_vs_debug.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->vs = &_menu2_vs_debug;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu3_vs.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_menu;
		shader_variations->vs = &_menu3_vs;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"menu3_vs_debug.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->vs = &_menu3_vs_debug;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_normal_and_depth_ps_mesh.hlsl" ), true );
		shader_variations->ps = &_scene_camera_normal_and_depth_ps_mesh;
		shader_variations->ps_masked = &_scene_camera_normal_and_depth_ps_mesh_masked;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_normal_and_depth_vs_mesh.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->vs = &_scene_camera_normal_and_depth_vs_mesh;
		shader_variations->vs_waved = &_scene_camera_normal_and_depth_vs_mesh_waved;
		shader_variations->vs_clipped = &_scene_camera_normal_and_depth_vs_mesh_clipped;
		shader_variations->vs_waved_clipped = &_scene_camera_normal_and_depth_vs_mesh_waved_clipped;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_outline_ps_mesh.hlsl" ), true );
		shader_variations->ps = &_scene_camera_outline_ps_mesh;
		shader_variations->ps_masked = &_scene_camera_outline_ps_mesh_masked;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_color_ps_debug.hlsl" ), true );
		shader_variations->ps = &_scene_camera_color_ps_debug;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_color_vs_debug.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_debug;
		shader_variations->vs = &_scene_camera_color_vs_debug;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_camera_color_vs_mesh.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->vs = &_scene_camera_color_vs_mesh;
		shader_variations->vs_waved = &_scene_camera_color_vs_mesh_waved;
		shader_variations->vs_clipped = &_scene_camera_color_vs_mesh_clipped;
		shader_variations->vs_waved_clipped = &_scene_camera_color_vs_mesh_waved_clipped;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_shadow_ps_mesh.hlsl" ), true );
		shader_variations->ps = &_scene_shadow_ps_mesh;
		shader_variations->ps_masked = &_scene_shadow_ps_mesh_masked;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_shadow_vs_mesh.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_mesh_base;
		shader_variations->vs = &_scene_shadow_vs_mesh;
		shader_variations->vs_waved = &_scene_shadow_vs_mesh_waved;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_blur_x.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_blur_x;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_blur_y.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_blur_y;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_resolve_native.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_resolve_native;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_resolve_scaled.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_resolve_scaled;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_resolve_quarter.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_resolve_quarter;
		_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_ps_process.hlsl" ), true );
		shader_variations->ps = &_scene_post_ps_process;
		_shader_variations_list.insert_at_end( shader_variations );

		//shader_variations = new shader_variations_c( "scene_post_ps_resolve.hlsl", true );
		//shader_variations->ps_msaa_count_1 = &scene_post_ps_resolve_msaa_count_1;
		//shader_variations->ps_msaa_count_2 = &scene_post_ps_resolve_msaa_count_2;
		//shader_variations->ps_msaa_count_4 = &scene_post_ps_resolve_msaa_count_4;
		//shader_variations->ps_msaa_count_8 = &scene_post_ps_resolve_msaa_count_8;
		//_shader_variations_list.insert_at_end( shader_variations );

		shader_variations = new shader_variations_c( string16_c( mode_e_static, L"scene_post_vs.hlsl" ), true );
		shader_variations->input_vertex_layout = &video_renderer_interface_c::vertex_layout_point;
		shader_variations->vs = &_scene_post_vs;
		_shader_variations_list.insert_at_end( shader_variations );

		// load built in shaders.
		if ( !refresh() )
		{
			debug_annoy( L"error", L"could not load a built in shader." );
			return false;
		}

		// load built in mesh material shader.
		_scene_camera_color_ps_mesh = load_pixel_shader( string16_c( "scene_camera_color_ps_mesh.hlsl" ) );
		if ( !_scene_camera_color_ps_mesh )
		{
			debug_annoy( L"error", L"could not load a built in material shader." );
			return false;
		}

		_scene_camera_color_ps_mesh->_reference_count++; // add one fake user to keep this shader loaded at all times, it's basically built in.

		return true;
	}

	boolean_c video_renderer_shader_manager_c::refresh()
	{
		assert( engine_c::get_instance()->get_resource_manager() != nullptr );
		assert( engine_c::get_instance()->get_video_interface() != nullptr );

		for ( sint32_c i = 0; i < _material_pixel_shader_list.get_length(); i++ )
		{
			_refresh_shader_variations( _material_pixel_shader_list[ i ]->_shader_variations );
		}

		for ( sint32_c i = 0; i < _shader_variations_list.get_length(); i++ )
		{
			_refresh_shader_variations( _shader_variations_list[ i ] );
		}

		// check that all required internal shaders are loaded.
		// otherwise if a shader fails to load during run time, then the shader that is already loaded and working continues to be the one that is used.
		for ( sint32_c i = 0; i < _shader_variations_list.get_length(); i++ )
		{
			shader_variations_c * shader_variations = _shader_variations_list[ i ];
			if ( shader_variations->input_vertex_layout )
			{
				if ( shader_variations->vs != nullptr )
				{
					if ( *shader_variations->vs == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->vs_waved != nullptr )
				{
					if ( *shader_variations->vs_waved == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->vs_clipped != nullptr )
				{
					if ( *shader_variations->vs_clipped == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->vs_waved_clipped != nullptr )
				{
					if ( *shader_variations->vs_waved_clipped == nullptr )
					{
						return false;
					}
				}
			}
			else
			{
				if ( shader_variations->ps != nullptr )
				{
					if ( *shader_variations->ps == nullptr )
					{
						return false;
					}
				}
				if ( shader_variations->ps_masked != nullptr )
				{
					if ( *shader_variations->ps_masked == nullptr )
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
		for ( sint32_c i = 0; i < _material_pixel_shader_list.get_length(); i++ )
		{
			video_renderer_pixel_shader_c * material_pixel_shader = _material_pixel_shader_list[ i ];
			if ( material_pixel_shader->_reference_count == 0 )
			{
				shader_variations_c * shader_variations = material_pixel_shader->_shader_variations;
				_shader_variations_list.remove( shader_variations );
				delete shader_variations;
				_material_pixel_shader_list.remove_at_index( i );
				delete material_pixel_shader;
				i--;
			}
		}
	}

	video_renderer_pixel_shader_c * video_renderer_shader_manager_c::load_pixel_shader( string16_c const & file_name )
	{
		if ( file_name.get_length() == 0 )
		{
			return nullptr;
		}

		// search for existing.
		for ( sint32_c i = 0; i < _material_pixel_shader_list.get_length(); i++ )
		{
			shader_variations_c * shader_variations = _material_pixel_shader_list[ i ]->_shader_variations;
			if ( shader_variations->source_file.file_name == file_name )
			{
				return _material_pixel_shader_list[ i ];
			}
		}

		// create new.
		video_renderer_pixel_shader_c * renderer_pixel_shader = new video_renderer_pixel_shader_c();
		_material_pixel_shader_list.insert_at_end( renderer_pixel_shader );
		renderer_pixel_shader->_shader_variations = new shader_variations_c( file_name, false );
		renderer_pixel_shader->_shader_variations->ps = &renderer_pixel_shader->_ps;
		renderer_pixel_shader->_shader_variations->ps_masked = &renderer_pixel_shader->_ps_masked;
		_refresh_shader_variations( renderer_pixel_shader->_shader_variations );
		return renderer_pixel_shader;
	}

	boolean_c video_renderer_shader_manager_c::resolve_file_path( string16_c const & file_path_relative, boolean_c is_internal, string16_c & file_path_absolute )
	{
		string16_c result;

		// scan game data folders.
		if ( is_internal == false )
		{
			core_list_c< string16_c > const & game_data_folder_path_list = engine_c::get_instance()->get_content_manager()->get_game_data_folder_path_list();
			for ( sint32_c i = game_data_folder_path_list.get_length() - 1; i >= 0; i-- )
			{
				result = game_data_folder_path_list[ i ];
				result += shader_folder_path;
				result += file_path_relative;
				if ( ops::data_does_file_exist( result ) )
				{
					file_path_absolute = result;
					return true;
				}
			}
		}

		// scan engine data folder.
		result = engine_c::get_instance()->get_content_manager()->get_engine_data_folder_path();
		result += shader_folder_path;
		result += file_path_relative;
		if ( ops::data_does_file_exist( result ) )
		{
			file_path_absolute = result;
			return true;
		}

		return false;
	}

}
