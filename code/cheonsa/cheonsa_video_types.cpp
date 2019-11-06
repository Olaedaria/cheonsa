#include "cheonsa_video_types.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	sint32_c video_texture_format_size_get( video_texture_format_e texture_format )
	{
		static const sint32_c format_sizes[ video_texture_format_e_count_ ] =
		{
			0,

			1,
			1,
			1,
			1,

			2,
			2,
			2,
			2,
			2,

			4,
			4,
			4,
			4,
			4,

			2,
			2,
			2,
			2,

			4,
			4,
			4,
			4,
			4,

			8,
			8,
			8,
			8,
			8,

			4,
			4,
			4,
			4,

			4,

			8,
			8,
			8,
			8,
			8,

			16,
			16,
			16,
			16,
			16
		};
		assert( static_cast< uint32_c >( texture_format ) < video_texture_format_e_count_ );
		return format_sizes[ texture_format ];
	}

	sint32_c video_depth_stencil_format_size_get( video_depth_stencil_format_e depth_stencil_format )
	{
		static const sint32_c format_sizes[ video_depth_stencil_format_e_count_ ] =
		{
			0,
			4,
			4
		};
		assert( static_cast< uint32_c >( depth_stencil_format ) < video_depth_stencil_format_e_count_ );
		return format_sizes[ depth_stencil_format ];
	}

	video_vertex_format_c::element_c::element_c()
		: format_type( type_e_none )
		, format_count( 0 )
		, semantic_name( name_e_none )
		, semantic_index( 0 )
	{
	}

	video_vertex_format_c::element_c::element_c( type_e format_type, sint8_c format_count, name_e semantic_name, sint8_c semantic_index )
		: format_type( format_type )
		, format_count( format_count )
		, semantic_name( semantic_name )
		, semantic_index( semantic_index )
	{
	}

	boolean_c video_vertex_format_c::element_c::operator == ( element_c const & other ) const
	{
		return ( format_type == other.format_type ) && ( format_count == other.format_count ) && ( semantic_name == other.semantic_name ) && ( semantic_index == other.semantic_index );
	}

	boolean_c video_vertex_format_c::element_c::operator != ( element_c const & other ) const
	{
		return ( format_type != other.format_type ) || ( format_count != other.format_count ) || ( semantic_name != other.semantic_name ) || ( semantic_index != other.semantic_index );
	}

	video_vertex_format_c::video_vertex_format_c()
		: name( nullptr )
		, elements( nullptr )
		, elements_count( 0 )
		, stride( 0 )
	{
	}

	video_vertex_format_c::video_vertex_format_c( video_vertex_format_c const & other )
		: name( other.name )
		, elements( other.elements )
		, elements_count( other.elements_count )
		, stride( other.stride )
	{
	}

	video_vertex_format_c::video_vertex_format_c( char8_c const * name, sint32_c elements_count, element_c const * elements, sint32_c stride )
		: name( name )
		, elements( elements )
		, elements_count( elements_count )
		, stride( stride )
	{
		//assert( stride % 32 == 0 ); // check that the struct size is a multiple of 32 bytes.
		assert( elements != nullptr );
		assert( elements_count >= 1 && elements_count <= 8 ); // check that there are a valid number of elements defined.
		sint32_c accounted_size = 0; // check that the size defined by the elements fits within the size of the struct.
		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			accounted_size += video_get_vertex_format_element_type_size( elements[ i ].format_type ) * elements[ i ].format_count;
		}
		assert( stride == accounted_size );
	}

	sint32_c video_get_vertex_format_element_type_size( video_vertex_format_c::element_c::type_e type )
	{
		static const sint32_c type_sizes[ video_vertex_format_c::element_c::type_e_count_ ] =
		{
			0,
			1,
			1,
			1,
			1,
			2,
			2,
			2,
			2,
			2,
			4
		};
		assert( type < video_vertex_format_c::element_c::type_e_count_ );
		return type_sizes[ type ];
	}

	sint32_c video_get_vertex_format_size( video_vertex_format_c::element_c * element_array, sint32_c element_array_count )
	{
		sint32_c result = 0;
		for ( sint32_c i = 0; i < element_array_count; i++ )
		{
			assert( element_array[ i ].format_type < video_vertex_format_c::element_c::type_e_count_ );
			assert( element_array[ i ].format_count >= 1 );
			assert( element_array[ i ].format_count <= 4 );
			result += video_get_vertex_format_element_type_size( element_array[ i ].format_type ) * element_array[ i ].format_count;
		}
		return result;
	}

	sint32_c video_get_index_format_size( video_index_format_e index_format )
	{
		static const sint32_c format_sizes[ video_index_format_e_count_ ] =
		{
			0,
			2,
			4
		};
		assert( index_format < video_index_format_e_count_ );
		return format_sizes[index_format];
	}

	video_vertex_format_c & video_vertex_format_c::operator = ( video_vertex_format_c const & other )
	{
		const_cast< char8_c const * >( name ) = other.name;
		const_cast< element_c const * >( elements ) = other.elements;
		elements_count = other.elements_count;
		stride = other.stride;
		return *this;
	}

	boolean_c video_vertex_format_c::operator == ( video_vertex_format_c const & other ) const
	{
		if ( elements_count != other.elements_count || stride != other.stride )
		{
			return false;
		}

		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			if ( elements[ i ] != other.elements[ i ] )
			{
				return false;
			}
		}

		return true;
	}

	boolean_c video_vertex_format_c::operator != ( video_vertex_format_c const & other ) const
	{
		if ( elements_count != other.elements_count || stride != other.stride )
		{
			return true;
		}

		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			if ( elements[ i ] != other.elements[ i ] )
			{
				return true;
			}
		}

		return false;
	}

	video_vertex_layout_c::video_vertex_layout_c()
		: name( nullptr )
		, vertex_formats( nullptr )
		, vertex_formats_count( 0 )
	{
	}

	video_vertex_layout_c::video_vertex_layout_c( video_vertex_layout_c const & other )
		: name( other.name )
		, vertex_formats( other.vertex_formats )
		, vertex_formats_count( other.vertex_formats_count )
	{
	}

	video_vertex_layout_c::video_vertex_layout_c( char8_c const * name, sint32_c vertex_formats_count, video_vertex_format_c const * const * vertex_formats )
		: name( name )
		, vertex_formats( vertex_formats )
		, vertex_formats_count( vertex_formats_count )
	{
		assert( vertex_formats_count >= 1 && vertex_formats_count <= 8 );
		assert( vertex_formats != nullptr );
		for ( sint32_c i = 0; i < vertex_formats_count; i++ )
		{
			assert( vertex_formats[ i ] != nullptr );
		}
	}

	video_vertex_layout_c & video_vertex_layout_c::operator = ( video_vertex_layout_c const & other )
	{
		name = other.name;
		vertex_formats = other.vertex_formats;
		vertex_formats_count = other.vertex_formats_count;
		return *this;
	}

	video_shader_defines_c::element_c::element_c()
		: name( nullptr )
		, value( nullptr )
	{
	}

	video_shader_defines_c::element_c::element_c( char8_c const * name, char8_c const * value )
		: name( name )
		, value( value )
	{
	}

	boolean_c video_shader_defines_c::element_c::operator == ( element_c const & other ) const
	{
		return ( name == other.name ) && ( value == other.value );
	}

	boolean_c video_shader_defines_c::element_c::operator != ( element_c const & other ) const
	{
		return ( name != other.name ) || ( value != other.value );
	}

	video_shader_defines_c::video_shader_defines_c()
		: elements()
		, elements_count( 0 )
	{
	}

	video_shader_defines_c::video_shader_defines_c( sint32_c elements_count, element_c * elements )
		: elements()
		, elements_count( elements_count )
	{
		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			this->elements[ i ] = elements[ i ];
		}
	}

	video_shader_defines_c & video_shader_defines_c::operator = ( video_shader_defines_c const & other )
	{
		elements_count = other.elements_count;
		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			elements[ i ] = other.elements[ i ];
		}
		return *this;
	}

	boolean_c video_shader_defines_c::operator == ( const video_shader_defines_c& other ) const
	{
		if ( elements_count != other.elements_count )
		{
			return false;
		}

		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			if ( elements[ i ] != other.elements[ i ] )
			{
				return false;
			}
		}

		return true;
	}

	boolean_c video_shader_defines_c::operator != ( const video_shader_defines_c& other ) const
	{
		if ( elements_count != other.elements_count )
		{
			return true;
		}

		for ( sint32_c i = 0; i < elements_count; i++ )
		{
			if ( elements[ i ] != other.elements[ i ] )
			{
				return true;
			}
		}

		return false;
	}

	video_shader_includer_c::video_shader_includer_c()
		: source_code()
		, included_file_paths()
	{
	}

	boolean_c video_shader_includer_c::open( char8_c const * const file_path_relative, void_c * * source_code_out, sint32_c * source_code_size_out )
	{
		assert( file_path_relative );
		assert( source_code_out );
		assert( source_code_size_out );

		*source_code_out = nullptr;
		*source_code_size_out = 0;

		included_file_paths.insert_at_end( string16_c( file_path_relative ) );

		string16_c file_path_absolute;
		video_renderer_shader_manager_c::resolve_file_path( string16_c( file_path_relative ), false, file_path_absolute );

		data_stream_file_c stream;
		if ( stream.open( file_path_absolute, data_stream_mode_e_read ) )
		{
			sint32_c source_code_size = stream.get_size();
			source_code.construct_mode_dynamic( source_code_size + 1 );
			source_code[ source_code_size ] = 0;
			stream.load( source_code.get_internal_array(), source_code_size );
			stream.close();
			*source_code_out = source_code.get_internal_array();
			*source_code_size_out = source_code_size;
			return true;
		}

		return false;
	}

	boolean_c video_shader_includer_c::close()
	{
		source_code.remove_all();
		return true;
	}

	// pure virtual destructors still need to be defined in a cpp file for some reason.

	video_output_c::video_output_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_output_c::~video_output_c()
	{
	}

	video_vertex_buffer_c::video_vertex_buffer_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_vertex_buffer_c::~video_vertex_buffer_c()
	{
	}

	video_index_buffer_c::video_index_buffer_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_index_buffer_c::~video_index_buffer_c()
	{
	}

	video_constant_buffer_c::video_constant_buffer_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_constant_buffer_c::~video_constant_buffer_c()
	{
	}

	video_texture_buffer_c::video_texture_buffer_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_texture_buffer_c::~video_texture_buffer_c()
	{
	}

	video_texture_c::video_texture_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_texture_c::~video_texture_c()
	{
	}

	video_texture3d_c::video_texture3d_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_texture3d_c::~video_texture3d_c()
	{
	}

	video_depth_stencil_c::video_depth_stencil_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_depth_stencil_c::~video_depth_stencil_c()
	{
	}

	video_vertex_shader_c::video_vertex_shader_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_vertex_shader_c::~video_vertex_shader_c()
	{
	}

	video_geometry_shader_c::video_geometry_shader_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_geometry_shader_c::~video_geometry_shader_c()
	{
	}

	video_pixel_shader_c::video_pixel_shader_c( video_interface_c * video_interface )
		: _video_interface( video_interface )
	{
	}

	video_pixel_shader_c::~video_pixel_shader_c()
	{
	}

	video_interface_c::~video_interface_c()
	{
	}

}
