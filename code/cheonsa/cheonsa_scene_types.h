#pragma once

#include "cheonsa_types.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_video_renderer_shader_manager.h"

namespace cheonsa
{

	class scene_c;
	class scene_object_c;
	class scene_component_c;
	class scene_component_ridgid_body_c;
	class scene_component_emitter_c;
	class scene_component_light_c;
	class scene_component_light_probe_c;
	class scene_component_menu_control_c;
	class scene_component_mesh_c;
	class scene_component_model_c;
	class scene_component_sound_c;
	class scene_component_sprite_c;

	enum scene_component_type_e : uint8_c
	{
		scene_component_type_e_none,
		scene_component_type_e_camera,
		scene_component_type_e_emitter,
		scene_component_type_e_light,
		scene_component_type_e_light_probe,
		scene_component_type_e_menu_control,
		scene_component_type_e_model,
		scene_component_type_e_sound,
		scene_component_type_e_sprite,
		scene_component_type_e_ridgid_body,
		scene_component_type_e_collision_object,
	};

	//enum scene_object_property_e
	//{
	//	scene_object_property_e_position = 0x01,
	//	scene_object_property_e_rotation = 0x02,
	//	scene_object_property_e_scale = 0x04,
	//	scene_object_property_e_bounding_box = 0x08
	//};

	enum scene_projection_type_e
	{
		scene_projection_type_e_orthographic,
		scene_projection_type_e_perspective,
	};

	enum scene_light_type_e
	{
		scene_light_type_e_direction, // directional light, is defined by a normal (-z axis of object), bounding box is infinite.
		scene_light_type_e_point, // point light, is defined by a point (position of object) and radius (of light), shines omnidirectionally, bounding box is fitted around point and radius.
		scene_light_type_e_cone // cone light, is defined by a point (position of object), radius (of light), normal (-z axis of object), and cone half angle (of light), shines originating from the point, in the direction defined by the normal, and is clipped by the cone angle.
	};

	enum scene_sprite_type_e
	{
		scene_sprite_type_e_fixed, // sprite is oriented by the object, along the object's xy plane.
		scene_sprite_type_e_face_camera_view_up, // sprite is oriented to face face towards the primary camera, and the sprite's up will always be aligned with the primary camera's up.
		scene_sprite_type_e_face_camera_global_up, // sprite is oriented to face towards the primary camera, and the sprite's up will always be aligned with the world's up (usually +z axis).
	};

	//enum scene_inherit_flag_e
	//{
	//	scene_inherit_flag_e_do_not_inherit_position = 0x01,
	//	scene_inherit_flag_e_do_not_inherit_rotation = 0x02,
	//	scene_inherit_flag_e_do_not_inherit_scale = 0x04,
	//};

	enum scene_inherit_e
	{
		scene_inherit_e_none = 0x00,
		scene_inherit_e_position = 0x01,
		scene_inherit_e_rotation = 0x02,
		scene_inherit_e_scale = 0x04,
	};

	inline scene_inherit_e operator | ( scene_inherit_e a, scene_inherit_e b )
	{
		return static_cast< scene_inherit_e >( a | b );
	}

	// pixel perfect hit detection.
	// works with renderer to render geometry in the line of the pick ray.
	// sorts results by their depth, from near to far.
	class scene_pick_c
	{
	public:
		float32_c depth; // the depth of the pick, or -1 if it was not hit.
		scene_component_c * component; // the geometry component that we are testing for pixel perfect hit detection against, it will be either a scene_component_model_c or a scene_component_sprite_c. use run-time type checking to determine what type it actually is.
	public:
		scene_pick_c();
		static float64_c quick_sort_function( scene_pick_c const & pick );
	};

	// defines various render states and properties to use when [drawing|rendering] geometry.
	// this has more in common with video_renderer name than scene name (because it does not have any spatial properties) but whatever.
	class scene_material_c
	{
	public:
		video_cull_type_e cull_mode;
		video_fill_type_e fill_mode;
		video_blend_type_e blend_type;
		//boolean_c is_lit; // if true then the renderer will find near by lights to affect the thing being rendered, otherwise the renderer can skip doing this.
		boolean_c is_mirror; // tells the renderer that this material is a planar reflector, so the renderer will stencil out this object's pixels and then render the planar reflection to those pixels in a pre pass. the view_color_reflections texture is then made available as input to the pixel shader in the main pass.
		boolean_c is_overlay; // tells the renderer that this material wants to use the screen texture as input for post process style effects like refractions.
		boolean_c is_masked; // masked means alpha masked, which means that the pxiel shader will discard pixels with alpha < 0.5f. tells the renderer what kind of pixel shader to use for this material when drawing early-z and shadow maps.
		boolean_c is_waved; // waved means wave displaced, which means that vertices will be animated over time based on parameters stored in the vertex's texture_b (useful for faking wind in cloth and foliage). tells the renderer what kind of vertex shader to use for this material.
		vector32x4_c colors[ 4 ]; // colors to supply as input to the pixel shader. alpha defines transparency.
		resource_file_texture_c::reference_c textures[ 4 ]; // textures to supply as input to the pixel shader.
		video_renderer_pixel_shader_c::reference_c pixel_shader; // pixel shader to put it all together.

	public:
		scene_material_c();

		void_c reset();

		scene_material_c & operator = ( scene_material_c const & other );

	};

	// +z axis is forward.
	// +y axis is down.
	// +x axis is right.
	class scene_camera_c
	{
	private:
		friend class video_renderer_interface_c;
		friend class scene_c;

		transform3d_c _world_space_transform;
		scene_projection_type_e _projection_type; // how this camera's projection works.
		vector32x2_c _perspective_center; // unit percents that define the point on the canvas that will be the center of the projection. used to create off-center projections. default is vector32x2_c( 0.0f, 0.0f ) which is center. vector32x2_c( -1.0f, -1.0f ) is top left corner. vector32x2_c( 1.0f, 1.0f ) is bottom right corner. due to planar projection, off-centered projections will appear more distorted and will have a slightly narrower field of view than specified by _perspective_field_of_view.
		float32_c _perspective_clip_near; // distance to the near clip plane.
		float32_c _perspective_clip_far; // distance to the far clip plane.
		float32_c _perspective_field_of_view; // vertical field of view in radians. aspect ratio of canvas is used to determine horizontal field of view.
		vector32x2_c _orthographic_center;
		float32_c _orthographic_clip_near;
		float32_c _orthographic_clip_far;
		float32_c _orthographic_field_of_view; // vertical hieght of orthographic view volume in meters. aspect ratio of canvas is used to determine horizontal width.

	public:
		scene_camera_c();

		transform3d_c const & get_world_space_transform() const;
		void_c set_world_space_transform( transform3d_c const & value );

		scene_projection_type_e get_projection_type() const;
		void_c set_projection_type( scene_projection_type_e value );

		vector32x2_c const & get_perspective_center() const;
		void_c set_perspective_center( vector32x2_c const & value );

		float32_c get_perspective_clip_near() const;
		void_c set_perspective_clip_near( float32_c value );

		float32_c get_perspective_clip_far() const;
		void_c set_perspective_clip_far( float32_c value );

		float32_c get_perspective_field_of_view() const;
		void_c set_perspective_field_of_view( float32_c value );

		vector32x2_c const & get_orthographic_center() const;
		void_c set_orthographic_ccenter( vector32x2_c const value );

		float32_c get_orthographic_clip_near() const;
		void_c set_orthographic_clip_near( float32_c value );

		float32_c get_orthographic_clip_far() const;
		void_c set_orthographic_clip_far( float32_c value );

		float32_c get_orthographic_field_of_view() const;
		void_c set_orthographic_field_of_view( float32_c value );

		// this function lets us create a view matrix that is relative to another arbitrary point in world space.
		// world space positions are 64-bit floats.
		// but the gpu works with 32-bit floats, which don't have enough precision for naive rendering of open worlds.
		// rather than simply casting 64-bit floats to 32-bit floats, we render all objects with world transforms that are relative to the camera's origin, this keeps 32-bit float precision high near the camera and prevents it from visibly degrading as the camera gets farther away from the world origin.
		matrix32x4x4_c make_view_transform_relative_to_origin( vector64x3_c origin ) const;

		// gets the projection transform.
		matrix32x4x4_c make_projection_transform( sint32_c canvas_width, sint32_c canvas_height ) const;

		ray64_c build_ray_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const;
		segment64_c build_segment_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const;
		vector32x2_c transform_point_from_world_space_to_canvas_space( vector64x3_c const & world_space_point, sint32_c canvas_width, sint32_c canvas_height ) const; // returns values between 0 and 1, where (0, 0) is top left and (1, 1) is bottom right. canvas_width and canvas_height are provided to create projection matrix (returned result is not in pixels so it will not be in this range).

	};

	// because lights can be instanced by components and by models, it's nice to have a unified way for any code to light the scene without being tied to component.
	// we need a unified way to spatially partition them, gather them, and feed them to the renderer.
	// when the light is added to the scene, it will be placed in either the scene's _local_lights_tree if it's local effect (point, cone), or it will be placed in the scene's _global_lights_list if it's global effect (direction).
	// data is private because the interdependencies are particular, so setter functions are provided that will automatically manage shared state with scene if needed.
	// +z axis is forward.
	// +y axis is down.
	// +x axis is right.
	class scene_light_c
	{
		friend class video_renderer_interface_c;
		friend class scene_c;
		friend class scene_tree_octo_c< scene_light_c * >;
		friend void_c insert_scored_light( core_list_c< scene_light_c * > & light_list, scene_light_c * light );

	private:
		scene_c * _scene; // the scene that this light lives in.
		transform3d_c _world_space_transform; // transform in world space, after inheriting from scene object of mother model if any.
		boolean_c _render_enabled; // if true then this light will be rendered.
		boolean_c _shadow_cast_enabled; // if true then this light will will cast shadows.
		scene_light_type_e _type;
		vector32x3_c _color;
		float32_c _strength;
		float32_c _range; // range is a function of brightness, so it can't be set.
		float32_c _cone_angle;
		box32x3_c _local_space_obb;
		box64x3_c _world_space_aabb;
		sint32_c _shadow_view_start; // used by scene renderer to keep track of this light's shadow atlas allocation.
		sint32_c _shadow_view_count; // used by scene renderer to keep track of this light's shadow atlas allocation.
		uint32_c _render_frame_last; // used by scene renderer to track the last time this light was rendered.
		sint32_c _render_index; // used by renderer to keep track of this light after it is gathered, and to assign it to things that need to be lit.
		float64_c _render_score; // used by renderer to sort the visible set of lights based on a priority score.
		scene_tree_octo_c< scene_light_c * >::node_c * _scene_tree_leaf_node; // used by the scene's _local_lights_tree.
		core_linked_list_c< scene_light_c * >::node_c _scene_tree_list_node; // used by the scene's _local_lights_tree.

	public:
		scene_light_c();

		scene_c * get_scene() const;

		transform3d_c const & get_world_space_transform() const;
		void_c set_world_space_transform( transform3d_c const & value );

		boolean_c get_render_enabled() const;
		void_c set_render_enabled( boolean_c value );

		boolean_c get_shadow_cast_enabled() const;
		void_c set_shadow_cast_enabled( boolean_c value );

		scene_light_type_e get_type() const;
		void_c set_type( scene_light_type_e value );

		vector32x3_c const & get_color() const;
		void_c set_color( vector32x3_c const & value );

		float32_c get_strength() const;
		void_c set_strength( float32_c value );

		float32_c get_range() const;
		//void_c set_range( float32_c value );

		float32_c get_cone_angle() const;
		void_c set_cone_angle( float32_c value );

		box32x3_c const & get_local_space_obb() const;

		box64x3_c const & get_world_space_aabb() const;

		// should be called after any properties are changed that affect size or shape (set_brightness(), set_cone_angle(), set_type()).
		// updates the bounding boxes and also the light's placement in the scene's light tree if needed.
		void_c update_bounding_boxes();

	};

}
