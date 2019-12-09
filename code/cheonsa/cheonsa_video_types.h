#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	class video_interface_c;

	// note that 3 channel formats are not supported by GPU hardware, except for 32 bits per channel (DXGI_FORMAT_R32G32B32_TYPELESS), but we don't need this.
	enum video_texture_format_e
	{
		video_texture_format_e_none,

		video_texture_format_e_r8_sint,
		video_texture_format_e_r8_uint,
		video_texture_format_e_r8_snorm,
		video_texture_format_e_r8_unorm,

		video_texture_format_e_r16_sint,
		video_texture_format_e_r16_uint,
		video_texture_format_e_r16_snorm,
		video_texture_format_e_r16_unorm,
		video_texture_format_e_r16_float,

		video_texture_format_e_r32_sint,
		video_texture_format_e_r32_uint,
		video_texture_format_e_r32_snorm,
		video_texture_format_e_r32_unorm,
		video_texture_format_e_r32_float,

		video_texture_format_e_r8g8_sint,
		video_texture_format_e_r8g8_uint,
		video_texture_format_e_r8g8_snorm,
		video_texture_format_e_r8g8_unorm,

		video_texture_format_e_r16g16_sint,
		video_texture_format_e_r16g16_uint,
		video_texture_format_e_r16g16_snorm,
		video_texture_format_e_r16g16_unorm,
		video_texture_format_e_r16g16_float,

		video_texture_format_e_r32g32_sint,
		video_texture_format_e_r32g32_uint,
		video_texture_format_e_r32g32_snorm,
		video_texture_format_e_r32g32_unorm,
		video_texture_format_e_r32g32_float,

		video_texture_format_e_r8g8b8a8_sint,
		video_texture_format_e_r8g8b8a8_uint,
		video_texture_format_e_r8g8b8a8_snorm,
		video_texture_format_e_r8g8b8a8_unorm,

		video_texture_format_e_b8g8r8a8_unorm, // for dxgi, d3d11, d3d9, and wpf interoperability. translates to d3dfmt_a8r8g8b8 in d3d9.

		video_texture_format_e_r16g16b16a16_sint,
		video_texture_format_e_r16g16b16a16_uint,
		video_texture_format_e_r16g16b16a16_snorm,
		video_texture_format_e_r16g16b16a16_unorm,
		video_texture_format_e_r16g16b16a16_float,

		video_texture_format_e_r32g32b32a32_sint,
		video_texture_format_e_r32g32b32a32_uint,
		video_texture_format_e_r32g32b32a32_snorm,
		video_texture_format_e_r32g32b32a32_unorm,
		video_texture_format_e_r32g32b32a32_float,

		video_texture_format_e_count_
	};

	enum video_texture_type_e
	{
		video_texture_type_e_none = 0,
		video_texture_type_e_texture2d = 0x01,
		video_texture_type_e_texture2darray = 0x02,
		video_texture_type_e_texture2dms = 0x04,
		video_texture_type_e_texture2dmsarray = 0x08,
		video_texture_type_e_texturecube = 0x10,
		video_texture_type_e_texturecubearray = 0x20,
	};

	inline video_texture_type_e operator | ( video_texture_type_e a, video_texture_type_e b )
	{
		return static_cast< video_texture_type_e >( static_cast< uint8_c >( a ) | static_cast< uint8_c >( b ) );
	}

	sint32_c video_texture_format_size_get( video_texture_format_e texture_format );

	enum video_depth_stencil_format_e
	{
		video_depth_stencil_format_e_none,
		video_depth_stencil_format_e_d24s8, // 24-bit float depth, 8-bit int stencil.
		video_depth_stencil_format_e_d32, // 32-bit float depth.
		video_depth_stencil_format_e_count_
	};

	sint32_c video_depth_stencil_format_size_get( video_depth_stencil_format_e depth_stencil_format );

	// defines elements of a tightly packed vertex format.
	// vertex elements in the structure are packed without padding.
	class video_vertex_format_c
	{
	public:
		class element_c
		{
		public:
			enum type_e
			{
				type_e_none,
				type_e_uint8,
				type_e_sint8,
				type_e_unorm8,
				type_e_snorm8,
				type_e_uint16,
				type_e_sint16,
				type_e_unorm16,
				type_e_snorm16,
				type_e_float16,
				type_e_float32,
				type_e_count_
			};

			enum name_e
			{
				name_e_none,
				name_e_position,
				name_e_normal,
				name_e_normal_u, // tangent.
				name_e_normal_v, // tangent.
				name_e_texture,
				name_e_color,
				name_e_bone_weights,
				name_e_bone_indices,
				name_e_count_
			};

		public:
			type_e format_type; // this vertex element's data format.
			sint8_c format_count; // the number of vector element's in this vertex element, needs to be in the range 1 to 4.
			name_e semantic_name; // the name of this vertex format element that is used to link it with the appropriate vertex shader input.
			sint8_c semantic_index; // the name's semantic index, this number gets appended on to the end of the name string.

			element_c();
			element_c( type_e format_type, sint8_c format_count, name_e semantic_name, sint8_c semantic_index );

			boolean_c operator == ( element_c const & other ) const;
			boolean_c operator != ( element_c const & other ) const;

		};

	public:
		char8_c const * name; // for identification during debug inspection.
		element_c const * elements; // should point to something that will be in scope for the entire life time of the program.
		sint32_c elements_count;
		sint32_c stride; // vertex stride, essentially the size in bytes of the vertex struct, but is allowed to be larger, in the case that the vertex defines some padding after the last element, in order to perform well with hardware, vertex cache, etc (i don't fully understand).

	public:
		video_vertex_format_c();
		video_vertex_format_c( video_vertex_format_c const & other );
		video_vertex_format_c( char8_c const * name, sint32_c elements_count, element_c const * elements, sint32_c stride );
		video_vertex_format_c & operator = ( video_vertex_format_c const & other );
		boolean_c operator == ( video_vertex_format_c const & other ) const;
		boolean_c operator != ( video_vertex_format_c const & other ) const;

	};

	// defines an input vertex layout, which is one or more vertex buffer input streams each with its own vertex format.
	class video_vertex_layout_c
	{
	public:
		char8_c const * name; // for identification during debug inspection.
		video_vertex_format_c const * const * vertex_formats; // should point to something that will be in scope for the entire life time of the program.
		sint32_c vertex_formats_count;

	public:
		video_vertex_layout_c();
		video_vertex_layout_c( video_vertex_layout_c const & other );
		video_vertex_layout_c( char8_c const * name, sint32_c vertex_formats_count, video_vertex_format_c const * const * vertex_formats );
		video_vertex_layout_c & operator = ( video_vertex_layout_c const & other );

	};

	sint32_c video_get_vertex_format_element_type_size( video_vertex_format_c::element_c::type_e type );
	sint32_c video_get_vertex_format_size( video_vertex_format_c::element_c * element_array, sint32_c element_array_count );

	class video_shader_defines_c
	{
	public:
		class element_c
		{
		public:
			char8_c const * name;
			char8_c const * value;

			element_c();
			element_c( char8_c const * name, char8_c const * value );

			boolean_c operator == ( element_c const & other ) const;
			boolean_c operator != ( element_c const & other ) const;

		};

		sint32_c elements_count;
		element_c elements[ 16 ];

	public:
		video_shader_defines_c();
		video_shader_defines_c( sint32_c elements_count, element_c * elements );
		video_shader_defines_c & operator = ( video_shader_defines_c const & other );
		boolean_c operator == ( video_shader_defines_c const & other ) const;
		boolean_c operator != ( video_shader_defines_c const & other ) const;

	};

	enum video_index_format_e
	{
		video_index_format_e_none,
		video_index_format_e_uint16,
		video_index_format_e_uint32,
		video_index_format_e_count_
	};

	sint32_c video_get_index_format_size( video_index_format_e index_format );

	enum video_primitive_type_e
	{
		video_primitive_type_e_point_list, // list of points.
		video_primitive_type_e_line_list, // list of separate lines.
		video_primitive_type_e_line_strip, // list of connected lines.
		video_primitive_type_e_triangle_list, // list of separate triangles.
		video_primitive_type_e_triangle_strip, // list of connected triangles.
		video_primitive_type_e_count_
	};

	enum video_compare_function_e
	{
		video_compare_function_e_disable, // disables depth stencil test, the pixels will always be written.
		video_compare_function_e_never, // always fail the depth test.
		video_compare_function_e_always, // always pass the depth test.
		video_compare_function_e_equal, // depth test passes if the depth of the pixel being written is equal to the depth that is already there.
		video_compare_function_e_not_equal, // depth test passes if the depth of the pixel being written is not equal to the depth that is already there.
		video_compare_function_e_less, // depth test passes if the depth of the pixel being written is less than the depth that is already there.
		video_compare_function_e_less_equal, // depth test passes if the depth of the pixel being written is less than or equal to the depth that is already there.
		video_compare_function_e_greater, // depth test passes if the depth of the pixel being written is greater than the depth that is already there.
		video_compare_function_e_greater_equal, // depth test passes if the depth of the pixel being written is greater than or equal to the depth that is already there.
		video_compare_function_e_count_
	};

	enum video_stencil_operation_e
	{
		video_stencil_operation_e_keep,
		video_stencil_operation_e_zero,
		video_stencil_operation_e_replace,
		video_stencil_operation_e_increment_saturate,
		video_stencil_operation_e_decrement_saturate,
		video_stencil_operation_e_invert,
		video_stencil_operation_e_increment,
		video_stencil_operation_e_decrement,
		video_stencil_operation_e_count_
	};

	enum video_cull_type_e
	{
		video_cull_type_e_none, // no primitives will be culled.
		video_cull_type_e_front, // front facing primitives will be culled.
		video_cull_type_e_back, // back facing primitives will be culled.
		video_cull_type_e_count_
	};

	enum video_fill_type_e
	{
		video_fill_type_e_point,
		video_fill_type_e_edge,
		video_fill_type_e_face,
		video_fill_type_e_count_
	};

	enum video_blend_type_e
	{
		video_blend_type_e_set, // the pixel that is output from the shader is overwrites whatever is already in the target.
		video_blend_type_e_mix, // the pixel that is output from the shader is multiplied (mixed) with the pixel that is already in the target.
		video_blend_type_e_add, // the pixel that is output from the shader is added to the pixel that is already in the target.
		video_blend_type_e_sub, // the pixel that is output from the shader is subtracted from the pixel that is already in the target.
		video_blend_type_e_min, // the pixel that is output from the shader is written to the target if its value is less than the value of the pixel that is already in the target.
		video_blend_type_e_max, // the pixel that is output from the shader is written to the target if its value is greater than the value of the pixel that is already in the target.
		video_blend_type_e_count_
	};

	enum video_sample_filter_type_e
	{
		video_sample_filter_type_e_point,
		video_sample_filter_type_e_linear,
		video_sample_filter_type_e_anisotropic,
		video_sample_filter_type_e_count_
	};

	enum video_sample_address_type_e
	{
		video_sample_address_type_e_clamp,
		video_sample_address_type_e_wrap,
		video_sample_address_type_e_count_
	};

	enum video_multi_sample_count_e
	{
		video_multi_sample_count_e_1x = 1, // disabled.
		video_multi_sample_count_e_2x = 2,
		video_multi_sample_count_e_4x = 4,
		video_multi_sample_count_e_8x = 8,
		video_multi_sample_count_e_count_
	};

	class video_texture_c;

	// an association of a texture render target with an operating system window.
	// when implemented, this is used to render to an operating system window.
	// it also handles resizing the internal render target when the window resizes.
	// it's basically a wrapper for a swap chain? or something.
	class video_output_c
	{
	protected:
		friend class video_interface_c;
		video_interface_c * _video_interface;

	protected:
		video_output_c( video_interface_c * video_interface );

	public:
		virtual ~video_output_c() = 0;
		virtual boolean_c update() = 0;
		virtual video_texture_c * get_texture_resource() const = 0;
		virtual void_c * get_window_handle() const = 0;
		virtual void_c present() const = 0;

	};

	class video_vertex_buffer_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_vertex_buffer_c( video_interface_c * video_interface );

	public:
		virtual ~video_vertex_buffer_c() = 0;
		virtual video_vertex_format_c const * get_vertex_format() const = 0;
		virtual sint32_c get_vertex_count() const = 0;
		virtual void_c set_data( void_c * data, sint32_c data_size ) const = 0;

	};

	class video_index_buffer_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_index_buffer_c( video_interface_c * video_interface );

	public:
		virtual ~video_index_buffer_c() = 0;
		virtual video_index_format_e get_index_format() = 0;
		virtual sint32_c get_index_count() = 0;
		virtual void_c set_data( void_c * data, sint32_c data_size ) = 0;

	};

	class video_constant_buffer_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_constant_buffer_c( video_interface_c * video_interface );

	public:
		virtual ~video_constant_buffer_c() = 0;
		virtual void_c set_data( void_c * data, sint32_c data_size ) const = 0;

	};

	class video_texture_buffer_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_texture_buffer_c( video_interface_c * video_interface );
		
	public:
		virtual ~video_texture_buffer_c() = 0;
		virtual void_c set_data( void_c * data, sint32_c data_size ) const = 0;

	};

	class video_texture_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_texture_c( video_interface_c * video_interface );

	public:
		virtual ~video_texture_c() = 0;
		virtual video_texture_format_e get_texture_format() const = 0;
		virtual sint32_c get_width() const = 0;
		virtual sint32_c get_height() const = 0;
		virtual sint32_c get_multi_sample_count() const = 0;
		virtual sint32_c get_array_slice_count() const = 0;
		virtual sint32_c get_mip_level_count() const = 0;
		//virtual void_c copy_to( video_texture_c * other ) const = 0;
		virtual void_c set_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const = 0;
		virtual void_c get_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const = 0;
		virtual void_c generate_mips() const = 0;
		virtual void_c * get_base_interface() const = 0; // todo: move this getter to video_interface_d3d11_c so that the generic interface remains platform ignorant.

	};

	class video_texture3d_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_texture3d_c( video_interface_c * video_interface );

	public:
		virtual ~video_texture3d_c() = 0;
		virtual video_texture_format_e get_texture_format() const = 0;
		virtual sint32_c get_width() const = 0;
		virtual sint32_c get_height() const = 0;
		virtual sint32_c get_depth() const = 0;
		virtual sint32_c get_array_count() const = 0;
		//virtual void_c copy_to( video_texture3d_c * other ) const = 0;
		virtual void_c set_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const = 0;
		virtual void_c get_data( void_c * top_mip_level_data, sint32_c top_mip_level_data_size ) const = 0;
		virtual void_c * get_base_interface() const = 0;

	};

	class video_depth_stencil_c
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_depth_stencil_c( video_interface_c * video_interface );

	public:
		virtual ~video_depth_stencil_c() = 0;
		virtual video_depth_stencil_format_e get_depth_stencil_format() const = 0;
		virtual sint32_c get_width() const = 0;
		virtual sint32_c get_height() const = 0;
		virtual sint32_c get_array_count() const = 0;
		virtual sint32_c get_multi_sample_count() const = 0;
		//virtual void_c copy_to( video_texture_c * other ) const = 0;

	};

	class video_vertex_shader_c // a program that processes input vertices and outputs them.
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_vertex_shader_c( video_interface_c * video_interface );

	public:
		virtual ~video_vertex_shader_c() = 0;
		virtual void_c enable_stream_out( video_vertex_format_c const * output_vertex_format ) = 0;
		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) = 0;

	};

	class video_geometry_shader_c // a program that processes input primitives and outputs zero or more primitives.
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_geometry_shader_c( video_interface_c * video_interface );

	public:
		virtual ~video_geometry_shader_c() = 0;
		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) = 0;

	};

	class video_pixel_shader_c // a program that processes input primitive info for a given pixel and outputs the color(s) for that pixel.
	{
	protected:
		video_interface_c * _video_interface;

	protected:
		video_pixel_shader_c( video_interface_c * video_interface );

	public:
		virtual ~video_pixel_shader_c() = 0;
		virtual void_c get_compiled_code( void_c const * & compiled_code, sint32_c & compiled_code_size ) = 0;

	};

	class video_shader_includer_c
	{
	public:
		core_list_c< string16_c > included_file_paths; // list of all relative file paths that were attempted to be opened by this includer, regardless of if they were successfully opened or not. this is one method that is used to detect changes to the file, as well as to detect which dependencies were modified.
		core_list_c< char8_c > source_code; // temporary holder of currently opened included file.

	public:
		video_shader_includer_c();
		boolean_c open( char8_c const * const relative_file_path, void_c * * source_code_out, sint32_c * source_code_size_out );
		boolean_c close();

	};

}
