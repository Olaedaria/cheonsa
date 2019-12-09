#pragma once

#include "cheonsa__types.h"
#include "cheonsa_video_renderer_types.h"
#include "cheonsa_video_renderer_canvas.h"
#include "cheonsa_scene_types.h"
#include "cheonsa_menu_types.h"
#include "cheonsa_resource_file_model.h"
#include "cheonsa_data_scribe_ini.h"

namespace cheonsa
{

	// interacts with video_interface_c to render scenes and menus.
	// in the context of 3d objects, if you see the term "real" it means actual real world coordinates, and if you see the term "virtual" then it means a space relative to the primary camera view.
	// "real" will usually always use 64-bit double precision floating point values.
	// "virtual" will usually use 32-bit single precision floating point values, since these will be uploaded to the gpu at some point.
	// scene representation on the cpu uses 64-bit double precision floating point (which is enough for open worlds), but the gpu works with 32-bit single precision floating point.
	// in order to render objects with the gpu that have 64-bit double precision floating point coordinates, we get the delta position of each object relative to the primary camera (that is being rendered from), and we cast those delta coordinates to 32-bit single precision floats and we upload those to the gpu for consumption by the shaders.
	// at a later point, i believe that fixed point precision on the cpu is the way to go, but that's something to try later.
	class video_renderer_interface_c
	{
	public:
		video_renderer_interface_c();
		~video_renderer_interface_c();

		boolean_c start();

		// bakes count number of light probes.
		// call with count < 0 to bake all light probes at once.
		// call with count set to 1 to bake just that many light probes, call over several times to bake incrementally.
		// will return true if it baked the last light probe in the scene (which means it finished baking all of them).
		// i'm not sure how feasable it is to call this every frame, but if it's feasable then great.
		void_c render_scene_light_probes( scene_c * scene, sint32_c count );

		// renders a scene to the specified canvas from the point of view of the specified camera.
		// the existing contents of the canvas will be cleared.
		// menu_render_procedure is optional, but it needs to be provided in order to render 3d menu controls in the scene. its state is not relevant, it will be reset and rebuilt from scratch just for rendering the 3d menu controls.
		void_c render_scene( scene_c * scene, scene_camera_c const * camera, video_renderer_canvas_c * canvas );

		// renders a menu render procedure, which can contain a mix of 2d or 3d draws, but in practice only one set will be drawn at a time (either all 3d or all 2d).
		//void_c render_menu( menu_render_procedure_c * menu_render_procedure, video_renderer_canvas_c * canvas );

		// these debug drawing functions can be called from update.
		// the lines are held in a list until the frame is rendered.
		void_c add_debug_garbage( sint32_c count );
		void_c add_debug_line( boolean_c xray, vector64x3_c const & a_position, vector64x3_c const & b_position, vector32x4_c const & a_and_b_color );
		void_c add_debug_line( boolean_c xray, vector64x3_c const & a_position, vector64x3_c const & b_position, vector32x4_c const & a_color, vector32x4_c const & b_color );
		void_c add_debug_box( boolean_c xray, box64x3_c const & box, vector32x4_c const & color );
		void_c add_debug_box( boolean_c xray, box64x3_c const & box, space_transform_c & box_transform, vector32x4_c const & color );
		void_c add_debug_axes( boolean_c xray, scene_matrix_c & transform, float32_c scale, float32_c alpha );
		void_c add_debug_axes( boolean_c xray, space_transform_c & transform, float32_c scale, float32_c alpha );

		//sint32_c get_shadow_view_count();
		//void_c set_shadow_view_count( sint32_c value );

		//float32_c get_voxel_tree_size();
		//void_c set_voxel_tree_size( float32_c value );

		//sint32_c get_voxel_tree_depth();
		//void_c set_voxel_tree_depth( sint32_c value );

		boolean_c get_menu_draw_debug();
		void_c set_menu_draw_debug( boolean_c value );

	private:
		boolean_c _menu_draw_debug;
		core_list_c< menu_control_c * > _menu_root_control_group_list;
		core_list_c< menu_draw_list_c * > _menu_draw_list_list;
		core_list_c< video_renderer_vertex_menu_c > _menu_vertex_list; // vertices for rendering menu elements.
		core_list_c< video_renderer_vertex_debug_c > _menu_debug_vertex_list;
		core_list_c< uint16_c > _menu_index_list; // indices into _menu_vertex_list, formatted as a triangle list.
		video_vertex_buffer_c * _menu_vertex_buffer;
		video_index_buffer_c * _menu_index_buffer;
		video_vertex_buffer_c * _menu_debug_vertex_buffer;
		void_c _render_control_for_control_group( menu_control_c * control_group );
		void_c _render_menu_draw_list( menu_draw_list_c const & draw_list );

	public:
		// gathers control groups.
		// compiles draw lists for each element and each control that is the root of a control group.
		// takes each draw list and compiles the big menu vertex and index lists.
		// creates or resizes the big menu vertex and index buffers, uploads the vertex and index data to them, and binds them.
		// binds font glyph map texture.
		// after this step then root level menu controls (2d and 3d) are ready to be rendered to the output.
		void_c pre_render_menus( user_interface_c * user_interface );

		// renders the 2d controls in the user interface to the canvas of the user interface.
		void_c render_2d_menus( user_interface_c * user_interface );

	public:
		// variable renderer settings are stored in here.
		// constant renderer settings are stored as static const variables in video_renderer_interface_c.
		// a novel way to present the user video settings:
		// we can budget the memory footprint of each setting if it were to be applied.
		// so we can show the user how much GPU memory they have, and how much GPU memory each setting will take up.
		class settings_c
		{
		public:
			// index of the display that the window should go full screen on.
			// i believe (but i don't know) that with technologies such as nVidia Mosaic and AMD Eyefinity, multiple displays are made to look like 1 display to the OS and our graphics API, so we don't have to do anything special to handle those situations.
			sint32_c full_screen_display;

			enum window_mode_e
			{
				window_mode_e_window, // reizable window.
				window_mode_e_window_full_screen, // fixed size window that fills the whole screen.
				window_mode_e_full_screen, // exclusive full screen.
				window_mode_e_count_,
				window_mode_e_undefined
			};
			static boolean_c get_window_mode_string( window_mode_e value, string8_c & result );
			static boolean_c get_window_mode_value( string8_c const & string, window_mode_e & result );
			window_mode_e window_mode;

			enum presentation_mode_e
			{
				presentation_mode_e_immediate,
				presentation_mode_e_v_sync,
				presentation_mode_e_count_,
				presentation_mode_e_undefined
			};
			static boolean_c get_presentation_mode_string( presentation_mode_e value, string8_c & result );
			static boolean_c get_presentation_mode_value( string8_c const & string, presentation_mode_e & result );
			presentation_mode_e presentation_mode;

			enum post_process_quality_e
			{
				post_process_quality_e_off,
				post_process_quality_e_high,
				post_process_quality_e_count_,
				post_process_quality_e_undefined
			};
			static boolean_c get_post_process_quality_string( post_process_quality_e value, string8_c & result );
			static boolean_c get_post_process_quality_value( string8_c const & string, post_process_quality_e & result );
			post_process_quality_e post_process_quality; // quality of post process effects.

			enum texture_resolution_e
			{
				texture_resolution_e_quarter,
				texture_resolution_e_half,
				texture_resolution_e_whole,
				texture_resolution_e_count_,
				texture_resolution_e_undefined
			};
			static boolean_c get_texture_resolution_string( texture_resolution_e value, string8_c & result );
			static boolean_c get_texture_resolution_value( string8_c const & string, texture_resolution_e & result );
			texture_resolution_e texture_resolution; // quality of loaded textures. does not affect menu textures which are always whole resolution.

			enum texture_sample_quality_e
			{
				texture_sample_quality_e_point,
				texture_sample_quality_e_bilinear,
				texture_sample_quality_e_trilinear,
				texture_sample_quality_e_anisotropic_2x,
				texture_sample_quality_e_anisotropic_4x,
				texture_sample_quality_e_anisotropic_8x,
				texture_sample_quality_e_anisotropic_16x,
				texture_sample_quality_e_count_,
				texture_sample_quality_e_undefined
			};
			static boolean_c get_texture_sample_quality_string( texture_sample_quality_e value, string8_c & result );
			static boolean_c get_texture_sample_quality_value( string8_c const & string, texture_sample_quality_e & result );
			texture_sample_quality_e texture_sample_quality; // quality of texture sampling.

			enum light_count_e
			{
				light_count_e_16,
				light_count_e_32,
				light_count_e_64,
				light_count_e_128,
				light_count_e_count_,
				light_count_e_undefined
			};
			static boolean_c get_light_count_string( light_count_e value, string8_c & result );
			static boolean_c get_light_count_value( string8_c const & string, light_count_e & result );
			static sint32_c get_light_count( light_count_e value );
			light_count_e light_count; // maximum number of lights per view.

			enum light_probe_resolution_e
			{
				light_probe_resolution_e_16,
				light_probe_resolution_e_32,
				light_probe_resolution_e_64,
				light_probe_resolution_e_count_,
				light_probe_resolution_e_undefined
			};
			static boolean_c get_light_probe_resolution_string( light_probe_resolution_e value, string8_c & result );
			static boolean_c get_light_probe_resolution_value( string8_c const & string, light_probe_resolution_e & result );
			static sint32_c get_light_probe_resolution( light_probe_resolution_e value );
			light_probe_resolution_e light_probe_resolution; // resolution of each light probe cube face.

			enum shadow_view_count_e
			{
				shadow_view_count_e_8,
				shadow_view_count_e_16,
				shadow_view_count_e_32,
				shadow_view_count_e_64,
				shadow_view_count_e_count_,
				shadow_view_count_e_undefined
			};
			static boolean_c get_shadow_view_count_string( shadow_view_count_e value, string8_c & result );
			static boolean_c get_shadow_view_count_value( string8_c const & string, shadow_view_count_e & result );
			static sint32_c get_shadow_view_count( shadow_view_count_e value );
			shadow_view_count_e shadow_view_count; // maximum number of shadow views per view.

			enum shadow_resolution_e
			{
				shadow_resolution_e_512,
				shadow_resolution_e_1024,
				shadow_resolution_e_2048,
				shadow_resolution_e_count_,
				shadow_resolution_e_undefined
			};
			static boolean_c get_shadow_resolution_string( shadow_resolution_e value, string8_c & result );
			static boolean_c get_shadow_resolution_value( string8_c const & string, shadow_resolution_e & result );
			static sint32_c get_shadow_resolution( shadow_resolution_e value );
			shadow_resolution_e shadow_resolution; // resolution of each shadow view.

			enum shadow_sample_quality_e
			{
				shadow_sample_quality_e_1x,
				shadow_sample_quality_e_5x,
				shadow_sample_quality_e_9x,
				shadow_sample_quality_e_16x,
				shadow_sample_quality_e_count_,
				shadow_sample_quality_e_undefined
			};
			static boolean_c get_shadow_sample_quality_string( shadow_sample_quality_e value, string8_c & result );
			static boolean_c get_shadow_sample_quality_value( string8_c const & string, shadow_sample_quality_e & result );
			shadow_sample_quality_e shadow_sample_quality; // quality of shadow map sampling.

		public:
			settings_c();

			settings_c & operator = ( settings_c const & other );

			void_c reset_to_default();

		};

		settings_c _settings; // the rendnerer's current settings.
		void_c set_settings( settings_c const & desired_settings );

	private:
		friend class engine_c;

		void_c _save_settings( data_scribe_ini_c & scribe ); // saves video settings to a "[video]" section in the ini file.
		void_c _load_settings( data_scribe_ini_c & scribe ); // loads video settings from a "[video]" section in the ini file.

	public:
		struct cube_face_view_c {
			vector32x3_c forward; // forward normal that was used to construct view_transform.
			vector32x3_c up; // up normal that was used to construct view_transform.
			matrix32x4x4_c view_transform; };

		// initializes:
		static sint32_c const _texture_bind_index_for_material_textures = 0; // bind index of 4 textures, named in the shader as: material_texture_0, material_texture_1, material_texture_2, material_texture_3.
		static sint32_c const _texture_bind_index_for_model_textures = 4; // bind index of 4 textures, named in the shader as: model_texture_0, model_texture_1, model_texture_2, model_texture_3.
		static sint32_c const _texture_bind_index_for_light_probe_texture = 8; // bind index of cube map of nearest light probe of model that is being rendered.
		static sint32_c const _texture_bind_index_for_shadow_map_texture = 9; // bind index of shadow_map_texture, texture array of shadow maps, each element corresponds to a shadow map view.
		static sint32_c const _texture_bind_index_for_glyph_atlas_texture = 10; // glyph_map_texture, texture array of font glyphs, texture atlas.
		static sint32_c const _texture_bind_index_for_target_textures = 11; // target_outline, target_normal, target_depth, target_color, target_color_quarter, target_color_quarter_blurred, target_color_reflections.
		static sint32_c const _texture_bind_index_for_bones_texture_buffer = 30;
		static sint32_c const _texture_bind_index_for_lights_texture_buffer = 31;
		static sint32_c const _texture_bind_index_for_shadow_views_texture_buffer = 32;
		
		static sint32_c const view_limit = 4; // the renderer will render up to this many different views per frame. if more than this many views want to be created for a given frame, then they will not be created.
		static sint32_c const bones_limit = 1024; // bones are uploaded to a texture buffer, so the shader doesn't need to know this limit.
		static sint32_c const menu_clip_plane_stack_count = 32; // also defined in "base.hlsl", defined value should match exactly.
		static sint32_c const scene_colors_count = 16; // also defined in "base.hlsl", defined value should match exactly.
		static sint32_c const object_lights_count = 8; // also defined in "base.hlsl", defined value should match exactly.
		static sint32_c const object_colors_count = 16; // also defined in "base.hlsl", defined value should match exactly.
		static sint32_c const object_textures_count = 4;
		static sint32_c const material_colors_count = 16; // also defined in "base.hlsl", defined value should match exactly.
		static sint32_c const material_textures_count = 4;

		static cube_face_view_c const cube_face_view_list_base[ 6 ];
		static matrix32x4x4_c get_cube_map_view_transform( sint32_c face_index, vector32x3_c const & position );

		static video_vertex_format_c const vertex_format_mesh_base; // used for static mesh vertex buffer.
		static video_vertex_format_c const vertex_format_mesh_bone_weight; // used by gpu skinned mesh vertex buffer.
		static video_vertex_format_c const vertex_format_menu;
		static video_vertex_format_c const vertex_format_debug;
		static video_vertex_format_c const vertex_format_point;

		static video_vertex_layout_c const vertex_layout_mesh_base; // used by static meshes. references a single vertex buffer.
		static video_vertex_layout_c const vertex_layout_mesh_base_and_bone_weight; // used by gpu skinned meshes. references a base geometry vertex buffer and a bone weights vertex buffer.
		static video_vertex_layout_c const vertex_layout_menu; // used by menu frames and text glyphs, 2d and 3d. references a single vertex buffer.
		static video_vertex_layout_c const vertex_layout_debug; // used by debug line drawer, for 2d and 3d.
		static video_vertex_layout_c const vertex_layout_point; // used by full screen quad to perform post process effects.

		//static video_shader_defines_c const shader_define; // compiler defineds used when compiling basic shaders. "#define lights_per_model_count x", where x is equal to _lights_per_model_count.
		static video_shader_defines_c const shader_define_masked; // compiler defineds used when compiling alpha masked pixel shaders. "#define masked".
		static video_shader_defines_c const shader_define_waved; // compiler defines used when compiling wave displaced vertex shaders. "#define waved".
		static video_shader_defines_c const shader_define_clipped; // compiler defines used when compiling plane clipped vertex shaders. "#define clipped".
		static video_shader_defines_c const shader_define_waved_clipped; // compiler defines used when compiling wave displaced + plane clipped vertex shaders. "#define waved", "#define clipped".

	private:
		// scene, camera, and canvas
		scene_c * _scene; // primary scene that is currently being rendered.

		video_texture_c * texture_white_pixel; // white pixel.
		resource_file_texture_c texture_white_pixel_wrapper;

		video_texture_c * texture_green_pixel; // green pixel for pointing out missing textures.
		resource_file_texture_c texture_green_pixel_wrapper;

		scene_material_c _scene_default_material;

		scene_object_c * _scene_light_probe_object; // we need an object to reuse multiple times per frame to visualize light probes.
		resource_file_model_c::reference_c _scene_light_probe_model; // one model resource (ideally a sphere) is used to visualize all of the light probes.
		//scene_component_model_c * _scene_light_probe_model_component; // one model component instance (ideally a sphere model) is used to visualize all of the light probes, it isn't associated with a .

		video_vertex_buffer_c * _quad_vertex_buffer; // full screen rectangle, used to draw rectangle quad that covers the whole screen, for post process and resolve passes.

		struct shadow_camera_block_c
		{
			matrix32x4x4_c shadow_camera_view_projection_transform;
		};

		struct shadow_object_block_c
		{
			matrix32x4x4_c shadow_object_world_transform;
		};

		struct scene_block_c
		{
			float32_c scene_time_counter; // time in seconds since the scene started, wraps around once every 24 hours (86400 seconds).
			vector32x3_c scene_wind_direction; // direction normal that wind is blowing in used to generate rolling waves.
			float32_c scene_wind_speed; // current speed of wind which can be used to factor amplitude (height, size) of waves.
			float32_c scene_wind_counter; // current offset of wind used to generate rolling waves, this value is incremented each frame by time_step * wind_speed.
			float32_c scene_wind_period_inverse; // inverse of time in seconds between rolling waves which is used to factor frequency.
			float32_c scene_wind_period_weight_minimum; // minimum influence of rolling waves.
			float32_c scene_wind_period_weight_range; // 1.0 - wind_period_weight_minimum.
			sint32_c scene_directional_light_count; // the first of this many lights in the light list are directional lights, used by the sky dome shader to color the sky and light the atmosphere.
			float32_c scene_light_probe_mip_count; // float for some reason, probably to work with HLSL's Sample function, to sample between mip levels.
			float32_c scene_unused_0;
			vector32x4_c scene_colors[ scene_colors_count ]; // these are some colors that for example can be used by the sky dome shader to color the atmosphere.
		};

		struct camera_block_c
		{
			matrix32x4x4_c camera_view_projection_transform; // the camera's view and projection matricies multiplied together.
			vector32x4_c camera_clip_plane; // used to clip pixels that lie behind the planar reflector so that they don't show up in the reflection itself.
			vector32x2_c canvas_actual_size; // width and height of target in pixels.
			vector32x2_c canvas_actual_size_inverse; // inverse of canvas_actual_size, which also represents the size of each pixel in texture space.
			vector32x2_c canvas_apparent_to_actual_ratio; // used to scale texture coordinates from apparent canvas size (constrained by view port) to actual canvas size.
			float32_c camera_clip_near; // distance in meters to camera's near clip plane.
			float32_c camera_clip_far; // distance in meters to camera's far clip plane.
			sint32_c scene_camera_light_indices[ object_lights_count ]; // lights that affect the camera, used to do certain post process effects like water on lens, soft bloom, etc.
		};

		struct object_block_c
		{
			matrix32x4x4_c object_world_transform; // this object's scene transform matrix.
			sint32_c object_light_indices[ object_lights_count ];
			vector32x4_c object_colors[ object_colors_count ];
		};

		struct material_block_c
		{
			vector32x4_c material_counters; // 4 different time counters. the way they behave can be defined by the material resource.
			vector32x4_c material_colors[ material_colors_count ]; // material colors.
		};

		struct menu_block_c
		{
			vector32x2_c menu_target_size_inverse;
			vector32x2_c menu_target_offset;
		};

		struct menu_batch_block_c
		{
			matrix32x4x4_c menu_world_transform; // for placing 2d menus in 3d space.
			vector32x4_c menu_basis; // 2d basis rotation and scale matrix, to rotate and scale the vertices around the origin.
			vector32x2_c menu_origin; // 2d position offset, applied after rotation and scale to position the vertex at its final position on the screen.
			float32_c menu_saturation;
			float32_c menu_unused_0;
			vector32x4_c menu_color;
			sint32_c menu_clip_plane_stack_length; // number of clip planes to use, generally come in multiples of 4 because menu system uses clip rectangles.
			sint32_c menu_unused_1[ 3 ];
			vector32x4_c menu_clip_plane_stack[ menu_clip_plane_stack_count ]; // these work as long as each set of planes defines an enclosed convex shape, then we can have shapes within shapes.
		};

		struct
		{
			shadow_camera_block_c * shadow_camera_block;
			video_constant_buffer_c * shadow_camera_block_constant_buffer;

			shadow_object_block_c * shadow_object_block;
			video_constant_buffer_c * shadow_object_block_constant_buffer;

			scene_block_c * scene_block;
			video_constant_buffer_c * scene_block_constant_buffer;

			camera_block_c * camera_block;
			video_constant_buffer_c * camera_block_constant_buffer;

			object_block_c * object_block;
			video_constant_buffer_c * object_block_constant_buffer;

			material_block_c * material_block;
			video_constant_buffer_c * material_block_constant_buffer;

			menu_block_c * menu_block;
			video_constant_buffer_c * menu_block_constant_buffer;

			menu_batch_block_c * menu_batch_block;
			video_constant_buffer_c * menu_batch_block_constant_buffer;

		} _constant_buffers;

		struct
		{
			video_texture_buffer_c * bones_texture_buffer; // used by gpu mesh skinner vertex shader.
			
			core_list_c< video_renderer_light_c > lights; // same as _lights but gpu compatible, its data is uploaded to lights_texture_buffer so shaders can load data from it.
			video_texture_buffer_c * lights_texture_buffer; // maps contents of lights.

			core_list_c< video_renderer_shadow_view_c > shadow_views; // same as _shadow_views but gpu compatible, its data is uploaded to shadow_views_texture_buffer so shaders can load data from it.
			video_texture_buffer_c * shadow_views_texture_buffer; // maps contents of shadow_views.

		} _texture_buffers;

		video_texture_c * _light_probe_cube_energy_default; // texture array of 6 1x1 elements, all black, bound to pixel shader in place of light probe cube map when ambient lighting is not wanted. we do this isntead of compiling another permutation of shaders.

		// each shadow view represents a shadow casting point of view (from a light source).
		// each view has the shape of a frustum.
		// some types of lights will use a different number of these.
		// spot lights will use at most 1 view, 1 for the direction that the spot light is pointing if it is also visible to the primary view.
		// point lights will use at most 6 views, 1 for each cardinal direction that is visible to the primary view.
		// directional lights will use at most 4 views, 1 for each cascade.
		class shadow_view_c
		{
		public:
			vector64x3_c world_space_position; // world space position of the shadow caster view, relative to the real world origin.
			frustum64_c world_space_frustum; // world space frustum of the shadow caster view. used to gather the geometry to render into the shadow map. generally, if this frustum does not intersect with the primary view's frustum then there's no need to render geometry into this shadow map, except if we want light probes and reflections to use the primary view's shadow maps.
			vector32x3_c virtual_space_position; // world space position of the shadow caster view, relative to the view that the shadows are for (only used by point light shadow casters).
			matrix32x4x4_c virtual_space_view_projection_transform; // view * projection transform, relative to the view that the shadows are for, and with a minor offset to facilitate world space texel snapping (for directional light types only).
			core_list_c< scene_component_model_c * > model_list; // shadow casting models visible to this shadow view.

		public:
			shadow_view_c();

		};

		// associates a canvas with a camera point of view.
		// keeps track of scene object list ranges that are directly visible to that view.
		// the canvas may be the primary canvas, or it may be a non-primary off-screen canvas that feeds as input into a later render pass.
		// the camera may be the primary camera, or it may be a non-primary camera such as a virtual camera spawned by a planar reflector (mirror) that is visible to the primary view.
		// all views for a given frame are compiled into a list, with the primary view being at the front of the list.
		// all views in the list are rendered, starting with the last and ending with the first.
		class view_c
		{
		public:
			video_renderer_canvas_c * canvas; // canvas that this view renders to. will always be set for the primary view.
			scene_camera_c const * camera; // primary camera that is currently being rendered from. will be set for the primary view. will not be set for reflection views.

			vector64x3_c world_space_position; // world space origin of this view. if this is a camera view then it is the same as camera->_scene_object->_world_space_transform.position.
			matrix32x3x3_c world_space_basis; // used to build camera facing sprite geometry. this method is a little lazy though, it's constructs sprite orientations that are relative to the camera's orientation, and not relative to the camera's position.
			frustum64_c world_space_frustum; // world space frustum used to gather renderables in this view.

			matrix32x4x4_c camera_view_projection_transform; // single precision view projection * transform, relative to primary view.
			float32_c camera_clip_near; // camera near clip plane distance.
			float32_c camera_clip_far; // camera far clip plane distance.

			boolean_c clip_plane_enable; // if true then clip plane will be used. this defines a plane (different from clip_near and clip_far) that can be used to discard geometry that lies behind the plane.
			plane32_c clip_plane; // clip plane relative to primary camera view. pixels that lie behind|under this plane will be discarded. mainly used to render planar reflections. relative to primary view (virtual world origin).

			sint32_c view_port_rectangle[ 4 ]; // [ left, top, right, bottom ], view-port rectangle, sub-region of canvas.
			sint32_c scissor_rectangle[ 4 ]; // [ left, top, right, bottom ], scissor rectangle, sub-region of view-port.

			scene_component_model_c * mirror_model; // if set, then this view is for a model that has meshes that have planar mirror materials applied.

		public:
			view_c();

			// initializes world_space_position, world_space_basis, world_space_frustum, and virtual_space_view_projection_transform.
			void_c initialize_for_camera( scene_camera_c const * camera, vector64x3_c primary_view_world_space_position );

		};

		core_list_c< view_c > _views; // the first view is always the primary view. additional views are created for planar reflectors (mirrors) that are directly visible to the primary camera. this is allocated to a fixed length at start up.

		core_list_c< shadow_view_c > _shadow_views; // short-lived staging area, shadow views are allocated and built as needed for each view or light probe that is rendered, and then released as soon as that render is done.
		video_texture_c * _shadow_map_texture2darray; // each texture slice corresponds to a shadow casting point of view.
		void_c _add_shadow_views( scene_light_c * const light, view_c const & camera_view ); // appends shadow views to _shadow_view_list.

		core_list_c< scene_light_c * > _gathered_lights; // all lights that intersect with the given view.
		core_list_c< scene_light_c * > _lights; // list of highest scoring lights selected from _lights_in_view.

		core_list_c< scene_component_c * > _gathered_components;
		core_list_c< scene_component_model_c * > _models_to_render;
		core_list_c< scene_component_model_c * > _models_to_render_for_outline;
		core_list_c< scene_component_model_c * > _models_to_skin;
		core_list_c< scene_component_sprite_c * > _sprites_to_render;
		core_list_c< scene_component_sprite_c * > _sprites_to_render_for_outline;
		core_list_c< video_renderer_vertex_mesh_base_c > _sprite_vertex_list; // at some point it would be good to batch sprites based on shared texture and material to reduce gpu state changes, but for now we just render them in whatever order we gather them in.
		video_vertex_buffer_c * _sprite_vertex_buffer; // point sprite vertices, these will be expanded by the sprite geometry shader into billboards.
		core_list_c< scene_component_menu_control_c * > _menu_controls_to_render;
		void_c _skin_model_vertices_with_cpu( scene_component_model_c * model );
		void_c _skin_model_vertices_with_gpu( scene_component_model_c * model );

		// clears all frame lists (_shadow_views, _gpu_shadow_views, _gathered_lights, _lights, _gpu_lights, _gathered_components, _models_to_render, _models_to_render_for_outline, _models_to_skin, _sprites_to_render, _sprites_to_render_for_outline, _sprite_vertex_list).
		// gathers renderables into _gathered_components.
		// sorts those components into _models_to_render, _models_to_render_for_outline, _sprites_to_render, _sprites_to_render_for_outline, _menu_controls_to_render.
		// also builds _sprite_vertex_list and _models_to_skin.
		// gathers into _gathered_lights all lights that intersect with view.
		// prioritizes|scores lights then adds the best scoring ones to both _lights and _gpu_lights.
		// also builds shadow views for the best scoring lights that are also shadow casters.
		// associates renderables with _lights that were gathered.
		// renders shadow views to shadow maps.
		// uploads _gpu_lights data to _texture_buffers.lights_texture_buffer.
		// uploads _gpu_shadow_views data to to _texture_buffers.shadow_views_texture_buffer.
		// uploads _sprite_vertex_list data to _sprite_vertex_buffer.
		void_c _just_do_everything_for_me( view_c const & view );

		// scene debug drawing
		core_list_c< video_renderer_vertex_debug_64_c > _debug_line_list_64; // debug vertices in world space.
		core_list_c< video_renderer_vertex_debug_64_c > _debug_line_list_xray_64; // debug vertices in world space.
		core_list_c< video_renderer_vertex_debug_c > _debug_line_list; // debug vertices translated to be relative to primary carmera, holds both normal and x-ray lines.
		video_vertex_buffer_c * _debug_line_list_vertex_buffer; // debug line list vertex buffer.

		// sets up reflected point of view, that can be used to render a planar reflected geometry to the primary canvas.
		// in this way, we don't need to allocate additional canvases for each reflector, and theoretically this can be extended to recurse|nest mirror views (not implemented yet, may not be practical for performance but we'll see).
		// however, because it renders to the primary canvas, there are some limitations:
		// we can't have semi-transparent mirrors, all mirror meshes must be fully opaque.
		// we can't have distorted|warped reflections, the reflection information that is available to be sampled is a 1:1 pixel perfect fit to the mirror geometry, you /can/ distort the sample point but there will be artifacts.
		//void_c initialize_mirror_perspective(
		//	plane64_c const & mirror_plane, // the reflection plane.
		//	box64x3_c const & mirror_plane_world_space_aabb, // the bounding box of the object that is the mirror. the plane by itself is infinite, so this makes it finite, it doesn't have to be a perfect fit, a little loose is fine.
		//	space_transform_c & at_transform, // the scene transform of the bounding box.
		//	view_c * result
		//);

		// scene drawing
		void_c _bind_model_properties( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c with_lights, boolean_c with_ambience, boolean_c with_outline );
		void_c _bind_mesh_vs_for_normal_and_depth( boolean_c is_waved, boolean_c is_clipped ); // binds vertex shaders for normal and depth rendering.
		void_c _bind_mesh_vs_for_color( boolean_c is_waved, boolean_c is_clipped ); // binds vertex shaders for color rendering.
		void_c _bind_material_for_color( scene_material_c * material ); // binds pixel shader, pixel shader textures, material colors, and rasterizer state.
		void_c _draw_model_with_no_material( scene_component_model_c * model, vector64x3_c const & view_position ); // draws model geometry only, leaves pixel shader state alone.
		void_c _draw_model_for_sky( scene_component_model_c * model ); // does not take view_position like the other functions, it assumes that view_position is zero.
		void_c _draw_model_for_outline( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped ); // draws model (silhouette, mask) to outline buffer with value of outline_color_index.
		void_c _draw_model_for_color( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped, boolean_c do_opaque, boolean_c do_transparent, boolean_c do_overlay, boolean_c with_lights, boolean_c with_ambience );
		//void_c _draw_model_for_energy( scene_component_model_c * model, vector64x3_c const & view_position ); // uses cheaper pixel shaders than _draw_model_for_color to generate approximate environment maps.
		void_c _draw_model_for_shadow( scene_component_model_c * model, vector64x3_c const & view_position ); // draws model for shadow map.
		void_c _draw_model_for_normal_and_depth( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped ); // draws model for normal and depth.

		// uses shaders to copy input texture to output texture.
		// input will be scaled to stretch over output.
		// if input and output are of the same dimensions, then a nearest neighbor texture sampler will be used.
		// if input and output are not of the same dimensions, then a linear texture sampler will be used.
		void_c _resolve_texture_full( video_texture_c * input, video_texture_c * output );

		//// uses shaders to down-sample an input texture to an output texture that is 1/4th width and height of the input texture (which is 1/16th the number of pixels).
		//// for each output pixel, uses a linear texture sampler to sample and blend neighborhoods of 4 pixels at a time from the input texture.
		//// input texture should have x and y dimensions that are multiples of 4 in order to avoid artifacts.
		//void_c _resolve_texture_quarter( video_texture_c * input, video_texture_c * output );

	};

}
