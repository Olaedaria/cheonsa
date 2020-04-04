#pragma once

#include "cheonsa_string16.h"
#include "cheonsa_video_interface.h"

#define shader_folder_path "_common/shaders/"

namespace cheonsa
{

	class video_renderer_pixel_shader_c;

	// manages compilation and caching of shaders and their various permutations.
	// shaders are cached to disk and will persist between engine run time sessions.
	// cheonsa uses a fixed and finite set of shaders.
	// some shaders are compiled from the same source file but with different preprocessor definitions defined each time in order to produce the proper code path permutations.
	// this shader cache will automatically recompile and recache shaders if their source files are detected to be modified.
	class video_renderer_shader_manager_c
	{
	public:
		// tracks the state of a file that is depended upon by a shader, so that the shader can be recompiled from the source code if if dependencies don't match what's in the cache.
		class file_dependency_c
		{
		public:
			string16_c file_name; // file name, if referencing hlsl then includes file extension, otherwise if referencing cached shader object then excludes file extension (just to make it easier to search for shader object variants, since more than one shader object may be generated from a single compiled hlsl file).
			string16_c absolute_file_path; // resolved file_name to an absolute file path using resolve_shader_file_path_absolute().
			sint64_c file_modified_time; // milliseconds since epoch that tells us when the file was last modified.
			file_dependency_c();
			file_dependency_c & operator = ( file_dependency_c const & other );
			boolean_c operator == ( file_dependency_c const & other ) const;
			boolean_c operator != ( file_dependency_c const & other ) const;
		};

		// each combination of material flags means compiling the shader from the same source file multiple times with different defines active, resulting in different variations or versions of that one shader.
		enum variation_e
		{
			variation_e_vs,
			variation_e_vs_waved,
			variation_e_vs_clipped,
			variation_e_vs_waved_clipped,
			variation_e_ps,
			variation_e_ps_masked,
			variation_e_count_
		};

		// a single shader file is referenced by its file name.
		// it will be compiled multiple times with different defines (code paths) activated depending on which variations it needs to support.
		// this acts as a bucket for that one shader file and its multiple compiled results.
		class shader_variations_c
		{
		public:
			boolean_c is_internal; // if true then this variation won't be deleted even if it has no references.

			file_dependency_c source_file; // uses file_name (with file extension) and absolute_file_path.
			core_list_c< file_dependency_c > source_file_dependency_list; // uses relative path.

			video_vertex_layout_c const * input_vertex_layout; // input vertex layout expected by vertex shader.
			video_vertex_format_c const * output_vertex_format; // optional, vertex format of output, for stream output enabled vertex shader.
			video_vertex_shader_c const * const * vs; // if set, compile "vs_main" with no additional defines.
			video_vertex_shader_c const * const * vs_waved; // if set, compile "vs_main" with "#define waved".
			video_vertex_shader_c const * const * vs_clipped; // if set, compile "vs_main" with "#define clipped".
			video_vertex_shader_c const * const * vs_waved_clipped; // if set, compile "vs_main" with "#define waved" and "#define clipped".
			video_pixel_shader_c const * const * ps; // if set, compile "ps_main" with no additional defines.
			video_pixel_shader_c const * const * ps_masked; // if set, compile "ps_main" with "#define masked".

		public:
			shader_variations_c( string16_c const & file_name, boolean_c is_internal );
			~shader_variations_c();

			// returns true if the given variation is supposed to be compiled and cached.
			boolean_c has_variation( variation_e variation );

		};

	private:
		video_vertex_shader_c * _skin_mesh; // "skin_mesh.hlsl".

		video_pixel_shader_c * _menu_ps_debug; // "menu_ps_debug.hlsl".
		video_pixel_shader_c * _menu_ps_frame; // "menu_ps_frame.hlsl".
		video_pixel_shader_c * _menu_ps_frame_keyed; // "menu_ps_frame_keyed.hlsl".
		video_pixel_shader_c * _menu_ps_solid_color; // "menu_ps_frame_solid_color.hlsl".
		video_pixel_shader_c * _menu_ps_text; // "menu_ps_text.hlsl".
		video_vertex_shader_c * _menu2_vs; // "menu2_vs.hlsl".
		video_vertex_shader_c * _menu2_vs_debug; // "menu2_vs_debug.hlsl".
		video_vertex_shader_c * _menu3_vs; // "menu3_vs.hlsl".
		video_vertex_shader_c * _menu3_vs_debug; // "menu3_vs_debug.hlsl".

		video_pixel_shader_c * _scene_camera_normal_and_depth_ps_mesh; // "scene_camera_normal_and_depth_ps.hlsl".
		video_pixel_shader_c * _scene_camera_normal_and_depth_ps_mesh_masked; // "scene_camera_normal_and_depth_ps.hlsl" with "#define masked".
		video_vertex_shader_c * _scene_camera_normal_and_depth_vs_mesh; // "scene_camera_normal_and_depth_vs_mesh.hlsl".
		video_vertex_shader_c * _scene_camera_normal_and_depth_vs_mesh_waved; // "scene_camera_normal_and_depth_vs_mesh.hlsl" with "#define waved".
		video_vertex_shader_c * _scene_camera_normal_and_depth_vs_mesh_clipped; // "scene_camera_normal_and_depth_vs_mesh.hlsl" with "#define clipped".
		video_vertex_shader_c * _scene_camera_normal_and_depth_vs_mesh_waved_clipped; // "scene_camera_normal_and_depth_vs_mesh.hlsl" with "#define waved" and "#define clipped".
		
		video_pixel_shader_c * _scene_camera_outline_ps_mesh;
		video_pixel_shader_c * _scene_camera_outline_ps_mesh_masked;

		video_pixel_shader_c * _scene_camera_color_ps_debug; // "scene_camera_color_ps_debug.hlsl".
		video_vertex_shader_c * _scene_camera_color_vs_debug; // "scene_camera_color_vs_debug.hlsl".
		video_vertex_shader_c * _scene_camera_color_vs_mesh; // "scene_camera_color_vs_mesh.hlsl".
		video_vertex_shader_c * _scene_camera_color_vs_mesh_waved; // "scene_camera_color_vs_mesh.hlsl" with "#define waved".
		video_vertex_shader_c * _scene_camera_color_vs_mesh_clipped; // "scene_camera_color_vs_mesh.hlsl" with "#define clipped".
		video_vertex_shader_c * _scene_camera_color_vs_mesh_waved_clipped; // "scene_camera_color_vs_mesh.hlsl" with "#define waved" and "#define clipped".

		video_pixel_shader_c * _scene_shadow_ps_mesh; // "scene_shadow_ps_mesh.hlsl".
		video_pixel_shader_c * _scene_shadow_ps_mesh_masked; // "scene_shadow_ps_mesh.hlsl" with "#define masked".
		video_vertex_shader_c * _scene_shadow_vs_mesh; // "scene_shadow_vs_mesh.hlsl".
		video_vertex_shader_c * _scene_shadow_vs_mesh_waved; // "scene_shadow_vs_mesh.hlsl" with "#define waved".

		video_pixel_shader_c * _scene_post_ps_blur_x; // "scene_post_ps_blur_x.hlsl".
		video_pixel_shader_c * _scene_post_ps_blur_y; // "scene_post_ps_blur_y.hlsl".
		video_pixel_shader_c * _scene_post_ps_resolve_native; // "scene_post_ps_resolve_native.hlsl". can be used to convert between texture formats i guess?
		video_pixel_shader_c * _scene_post_ps_resolve_scaled; // "scene_post_ps_resolve_scaled.hlsl".
		//video_pixel_shader_c * _scene_post_ps_resolve_quarter; // "scene_post_ps_resolve_quarter.hlsl".
		video_pixel_shader_c * _scene_post_ps_process; // "scene_post_ps_process.hlsl".
		video_vertex_shader_c * _scene_post_vs; // "scene_post_vs.hlsl".

		video_renderer_pixel_shader_c * _scene_camera_color_ps_mesh;

		core_list_c< shader_variations_c * > _shader_variations_list; // built-in shaders.
		core_list_c< video_renderer_pixel_shader_c * > _material_pixel_shader_list; // material (data driven, run time) defined pixel shaders.

		static void_c _resolve_cache_file_path_absolute( file_dependency_c const & source_file_dependency, variation_e variation, string16_c & result );
		static char8_c const * get_variation_suffix( video_renderer_shader_manager_c::variation_e variation );
		static video_shader_defines_c const * get_variation_shader_defines( video_renderer_shader_manager_c::variation_e variation );

		static boolean_c _load_source_dependency_information( shader_variations_c * shader_variations ); // scans source code files for all dependencies and times modified.
		static boolean_c __load_source_dependency_list_recursive( file_dependency_c const & source_file_dependency, boolean_c is_internal, core_list_c< file_dependency_c > & result );
		static boolean_c _load_cached_dependency_information( shader_variations_c * shader_variations, variation_e variation, string16_c & absolute_file_path, sint64_c & file_modified_time, core_list_c< file_dependency_c > & file_dependency_list ); // scans cache file for all file dependencies (included file names and times modified at the time of compilation).

		void_c _refresh_shader_variations( shader_variations_c * shader_variations );
		boolean_c _compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation );
		boolean_c __compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation, char8_c const * source_code, sint32_c source_code_size, video_vertex_shader_c const * const * result );
		boolean_c __compile_and_save_to_cache( shader_variations_c * shader_variations, variation_e variation, char8_c const * source_code, sint32_c source_code_size, video_pixel_shader_c const * const * result );
		void_c __save_to_cache( shader_variations_c * shader_variations, string16_c const & cache_file_path_absolute, void_c const * compiled_code, sint32_c compiled_code_size );
		boolean_c _load_from_cache( shader_variations_c * shader_variations, variation_e variation );
		boolean_c __load_from_cache( string16_c const & cache_file_path_absolute, video_vertex_shader_c * * result, video_vertex_layout_c const * vertex_layout );
		boolean_c __load_from_cache( string16_c const & cache_file_path_absolute, video_pixel_shader_c * * result );

	public:
		video_renderer_shader_manager_c();
		~video_renderer_shader_manager_c();

		boolean_c start();

		boolean_c refresh(); // detects source file modifications and data directory changes, and re-compiles and re-caches shaders from source code needed, or loads shaders from cached files if they are already in sync with the source files.
		void_c collect_garbage(); // deletes any material pixel shaders that have no references.

		video_renderer_pixel_shader_c * load_pixel_shader( string16_c const & file_name ); // loads a game defined pixel shader for use with custom materials.

		static boolean_c resolve_file_path( string16_c const & relative_file_path, boolean_c is_internal, string16_c & absolute_file_path );

		inline video_vertex_shader_c * get_skin_mesh() const { return _skin_mesh; }
		inline video_pixel_shader_c * get_menu_ps_debug() const { return _menu_ps_debug; }
		inline video_pixel_shader_c * get_menu_ps_frame() const { return _menu_ps_frame; }
		inline video_pixel_shader_c * get_menu_ps_solid_color() const { return _menu_ps_solid_color; }
		inline video_pixel_shader_c * get_menu_ps_text() const { return _menu_ps_text; }
		inline video_vertex_shader_c * get_menu2_vs() const { return _menu2_vs; }
		inline video_vertex_shader_c * get_menu2_vs_debug() const { return _menu2_vs_debug; }
		inline video_vertex_shader_c * get_menu3_vs() const { return _menu3_vs; }
		inline video_vertex_shader_c * get_menu3_vs_debug() const { return _menu3_vs_debug; }
		inline video_pixel_shader_c * get_scene_camera_normal_and_depth_ps_mesh() const { return _scene_camera_normal_and_depth_ps_mesh; }
		inline video_pixel_shader_c * get_scene_camera_normal_and_depth_ps_mesh_masked() const { return _scene_camera_normal_and_depth_ps_mesh_masked; }
		inline video_vertex_shader_c * get_scene_camera_normal_and_depth_vs_mesh() const { return _scene_camera_normal_and_depth_vs_mesh; }
		inline video_vertex_shader_c * get_scene_camera_normal_and_depth_vs_mesh_waved() const { return _scene_camera_normal_and_depth_vs_mesh_waved; }
		inline video_vertex_shader_c * get_scene_camera_normal_and_depth_vs_mesh_clipped() const { return _scene_camera_normal_and_depth_vs_mesh_clipped; }
		inline video_vertex_shader_c * get_scene_camera_normal_and_depth_vs_mesh_waved_clipped() const { return _scene_camera_normal_and_depth_vs_mesh_waved_clipped; }
		inline video_pixel_shader_c * get_scene_camera_outline_ps_mesh() const { return _scene_camera_outline_ps_mesh; }
		inline video_pixel_shader_c * get_scene_camera_outline_ps_mesh_masked() const { return _scene_camera_outline_ps_mesh_masked; }
		inline video_pixel_shader_c * get_scene_camera_color_ps_debug() const { return _scene_camera_color_ps_debug; }
		inline video_vertex_shader_c * get_scene_camera_color_vs_debug() const { return _scene_camera_color_vs_debug; }
		inline video_vertex_shader_c * get_scene_camera_color_vs_mesh() const { return _scene_camera_color_vs_mesh; }
		inline video_vertex_shader_c * get_scene_camera_color_vs_mesh_waved() const { return _scene_camera_color_vs_mesh_waved; }
		inline video_vertex_shader_c * get_scene_camera_color_vs_mesh_clipped() const { return _scene_camera_color_vs_mesh_clipped; }
		inline video_vertex_shader_c * get_scene_camera_color_vs_mesh_waved_clipped() const { return _scene_camera_color_vs_mesh_waved_clipped; }
		inline video_pixel_shader_c * get_scene_shadow_ps_mesh() const { return _scene_shadow_ps_mesh; }
		inline video_pixel_shader_c * get_scene_shadow_ps_mesh_masked() const { return _scene_shadow_ps_mesh_masked; }
		inline video_vertex_shader_c * get_scene_shadow_vs_mesh() const { return _scene_shadow_vs_mesh; }
		inline video_vertex_shader_c * get_scene_shadow_vs_mesh_waved() const { return _scene_shadow_vs_mesh_waved; }
		inline video_pixel_shader_c * get_scene_post_ps_blur_x() const { return _scene_post_ps_blur_x; }
		inline video_pixel_shader_c * get_scene_post_ps_blur_y() const { return _scene_post_ps_blur_y; }
		inline video_pixel_shader_c * get_scene_post_ps_resolve_native() const { return _scene_post_ps_resolve_native; }
		inline video_pixel_shader_c * get_scene_post_ps_resolve_scaled() const { return _scene_post_ps_resolve_scaled; }
		//inline video_pixel_shader_c * get_scene_post_ps_resolve_quarter() const { return _scene_post_ps_resolve_quarter; }
		inline video_pixel_shader_c * get_scene_post_ps_process() const { return _scene_post_ps_process; }
		inline video_vertex_shader_c * get_scene_post_vs() const { return _scene_post_vs; }
		inline video_renderer_pixel_shader_c * get_scene_camera_color_ps_mesh() const { return _scene_camera_color_ps_mesh; }

	};

	// holds a couple of pixel shader instances of different variations and a reference count, and a pointer to the shader_variations_c instance that manages _ps and _ps_masked.
	class video_renderer_pixel_shader_c
	{
	private:
		friend class video_renderer_shader_manager_c;

		video_pixel_shader_c * _ps;
		video_pixel_shader_c * _ps_masked; // this can be somewhat wasteful, because in the case that _ps and _ps_masked compile exactly the same code paths, two different but equivalent pixel shader instances are still compiled and saved.
		video_renderer_shader_manager_c::shader_variations_c * _shader_variations;
		sint32_c _reference_count;

	public:
		video_renderer_pixel_shader_c();
		~video_renderer_pixel_shader_c();

	public:
		class reference_c
		{
		private:
			video_renderer_pixel_shader_c * _value;

		public:
			reference_c();
			~reference_c();

			reference_c & operator = ( reference_c const & other );
			reference_c & operator = ( video_renderer_pixel_shader_c * other );

			boolean_c operator == ( reference_c const & other ) const;
			boolean_c operator != ( reference_c const & other ) const;

			boolean_c get_loaded() const;

			video_pixel_shader_c * get_pixel_shader() const;
			video_pixel_shader_c * get_pixel_shader_masked() const;
		};

	};

}
