#pragma once

#include "cheonsa_base_types.h"
#include "cheonsa_string8.h"
#include "cheonsa_math_types.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_video_types.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_video_renderer_shader_manager.h"

namespace cheonsa
{

	class scene_c;
	class scene_object_c;
	class scene_component_c;
	class scene_component_body_c;
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
		scene_component_type_e_body,
		scene_component_type_e_camera,
		scene_component_type_e_emitter,
		scene_component_type_e_light,
		scene_component_type_e_light_probe,
		scene_component_type_e_menu,
		scene_component_type_e_model,
		scene_component_type_e_sound,
		scene_component_type_e_sprite,
	};

	enum scene_object_property_e
	{
		scene_object_property_e_position = 0x01,
		scene_object_property_e_rotation = 0x02,
		scene_object_property_e_scale = 0x04,
		scene_object_property_e_bounding_box = 0x08
	};

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
		scene_sprite_type_e_real, // sprite is oriented by the object, along the object's xy plane.
		scene_sprite_type_e_face_camera_view_up, // sprite is oriented to face face towards the primary camera, and the sprite's up will always be aligned with the primary camera's up.
		scene_sprite_type_e_face_camera_global_up, // sprite is oriented to face towards the primary camera, and the sprite's up will always be aligned with the world's up (usually +z axis).
	};

	enum scene_inherit_flag_e
	{
		scene_inherit_flag_e_do_not_inherit_position = 0x01,
		scene_inherit_flag_e_do_not_inherit_rotation = 0x02,
		scene_inherit_flag_e_do_not_inherit_scale = 0x04,
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

	class scene_camera_c
	{
	private:
		friend class video_renderer_interface_c;
		friend class scene_c;

		space_transform_c _world_space_transform;
		scene_projection_type_e _projection_type; // how this camera's projection works.
		vector32x2_c _perspective_center; // coordinates in range -1 to 1 that defines the point on the canvas that will be the center of the projection. used to create off-center projection. default is ( 0.0, 0.0 ) which is center.
		float32_c _perspective_clip_near; // distance to the near clip plane.
		float32_c _perspective_clip_far; // distance to the far clip plane.
		float32_c _perspective_field_of_view; // vertical field of view in radians. aspect ratio of canvas is used to determine horizontal field of view.
		vector32x2_c _orthographic_center;
		float32_c _orthographic_clip_near;
		float32_c _orthographic_clip_far;
		float32_c _orthographic_field_of_view; // vertical hieght of orthographic view volume in meters. aspect ratio of canvas is used to determine horizontal width.

	public:
		scene_camera_c();

		space_transform_c const & get_world_space_transform() const;
		void_c set_world_space_transform( space_transform_c const & value );

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

		//matrix32x4x4_c make_view_transform_at_origin() const; // because cameras are placed virtually at the world origin when rendering, we have this function to make the view matrix with position at (0, 0, 0).
		matrix32x4x4_c make_view_transform_relative_to_origin( vector64x3_c origin ) const;
		matrix32x4x4_c make_projection_transform( sint32_c canvas_width, sint32_c canvas_height ) const;

		ray64_c build_ray_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const;
		segment64_c build_segment_through_canvas( vector32x2_c const & canvas_coordinates, sint32_c canvas_width, sint32_c canvas_height ) const;
		vector32x2_c transform_point_from_world_space_to_canvas_space( vector64x3_c const & world_space_point, sint32_c canvas_width, sint32_c canvas_height ) const;

	};

	// because lights can be instanced by components and by models.
	// this decouples the light from its user.
	// we need a unified way to spatially partition them, gather them, and feed them to the renderer.
	// when the light is added to the scene, it will be placed in either the scene's _local_lights_tree if it's local effect (point, cone), or it will be placed in the scene's _global_lights_list if it's global effect (direction).
	// data is private because the interdependencies are particular, so setter functions are provided that will automatically manage shared state with scene if needed.
	class scene_light_c
	{
	private:
		friend class video_renderer_interface_c;
		friend class scene_c;
		friend class scene_tree_octo_c< scene_light_c * >;
		friend void_c insert_scored_light( core_list_c< scene_light_c * > & light_list, scene_light_c * light );

		scene_c * _scene; // the scene that this light lives in.
		space_transform_c _world_space_transform; // transform in world space, after inheriting from scene object of mother model if any.
		boolean_c _render_enable;
		boolean_c _shadow_cast_enable;
		scene_light_type_e _type;
		vector32x3_c _color;
		float32_c _brightness;
		float32_c _range; // range is a function of brightness, so it can't be set.
		float32_c _cone_angle;
		box32x3_c _local_space_obb;
		box64x3_c _world_space_aabb;
		sint32_c _shadow_view_start; // used by scene renderer to keep track of this light's shadow atlas allocation.
		sint32_c _shadow_view_count; // used by scene renderer to keep track of this light's shadow atlas allocation.
		uint32_c _render_frame_last; // used by scene renderer to track the last time this light was rendered.
		sint32_c _render_index; // used by renderer to keep track of this light after it is gathered, and to assign it to things that need to be lit.
		float64_c _render_score; // used by renderer to prioritize sort lights.
		scene_tree_octo_c< scene_light_c * >::node_c * _scene_tree_leaf_node; // used by the scene's _local_lights_tree.
		core_linked_list_c< scene_light_c * >::node_c _scene_tree_list_node; // used by the scene's _local_lights_tree.

	public:
		scene_light_c();

		scene_c * get_scene() const;

		space_transform_c const & get_world_space_transform() const;
		void_c set_world_space_transform( space_transform_c const & value );

		boolean_c get_render_enable() const;
		void_c set_render_enable( boolean_c value );

		boolean_c get_shadow_cast_enable() const;
		void_c set_shadow_cast_enable( boolean_c value );

		scene_light_type_e get_type() const;
		void_c set_type( scene_light_type_e value );

		vector32x3_c const & get_color() const;
		void_c set_color( vector32x3_c const & value );

		float32_c get_brightness() const;
		void_c set_brightness( float32_c value );

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

	class scene_node_c
	{
	protected:
		string8_c _name; // optional name of this node.

		scene_node_c * _mother; // optional mother node.
		core_linked_list_c< scene_node_c * > _daughters; // daughter nodes.
		core_linked_list_c< scene_node_c * >::node_c _daughters_node;

		space_transform_c _local_space_transform; // the transform of this scene node relative to its mother scene node's world space.
		space_transform_c _world_space_transform; // the transform of this scene node in world space (after inheriting transform from its mother).

		box32x3_c _local_space_bounding_box; // oriented bounding box in local space.
		box64x3_c _world_space_bounding_box; // axis aligned bounding box in world space.

		sint32_c _auto_update_suspend_count; // if non-zero then any set_*() functions will not update the world space transform and hierarchy.

	public:
		scene_node_c();
		~scene_node_c(); // deletes all descendants.

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value );

		scene_node_c const * get_root_scene_node() const;
		scene_node_c * get_root_scene_node();

		scene_node_c const * get_mother() const;
		scene_node_c * get_mother();
		void_c set_mother( scene_node_c * value, boolean_c and_preserve_world_space_transform );

		core_linked_list_c< scene_node_c * > const & get_daughters() const;
		void_c add_daughter( scene_node_c * value, boolean_c and_preserve_world_space_transform );
		void_c remove_daughter( scene_node_c * value, boolean_c and_preserve_world_space_transform );

		scene_node_c const * find_daughter_with_name( string8_c const & name ) const; // searches heirarchy for the first node that matches name.
		scene_node_c * find_daughter_with_name( string8_c const & name );
		scene_node_c const * find_daughter_with_path( string8_c const & path ) const; // searches heirarchy for the node that lives at path.
		scene_node_c * find_daughter_with_path( string8_c const & path );

		space_transform_c const & get_local_space_transform() const;

		vector64x3_c const & get_local_space_position() const;
		void_c set_local_space_position( vector64x3_c const & value );

		quaternion32_c const & get_local_space_rotation() const;
		void_c set_local_space_rotation( quaternion32_c const & value );

		vector32x3_c const & get_local_space_scale() const;
		void_c set_local_space_scale( vector32x3_c const & value );
		void_c set_local_space_scale( float32_c value ); // sets uniform scale.

		space_transform_c const & get_world_space_transform() const;
		void_c set_world_space_transform( space_transform_c const & value );

		vector64x3_c const & get_world_space_position() const;
		void_c set_world_space_position( vector64x3_c const & value );

		quaternion32_c const & get_world_space_rotation() const;
		void_c set_world_space_rotation( quaternion32_c const & value );

		vector32x3_c const & get_world_space_scale() const;
		void_c set_world_space_scale( vector32x3_c const & value );
		void_c set_world_space_scale( float32_c value );

		box32x3_c const & get_local_space_bounding_box() const;
		void_c set_local_space_bounding_box( box32x3_c const & value );

		box64x3_c const & get_world_space_bounding_box() const;

		void_c suspend_auto_update(); // increments the auto update suspension count.
		void_c resume_auto_update(); // decrements the auto update suspension count.
		
		void_c update_transform(); // updates world space transform of this node only.
		void_c update_transform_hierarchy(); // updates the world space transform of this node and all descendants.

	};

}
