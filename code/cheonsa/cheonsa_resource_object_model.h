#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_video_types.h"
#include "cheonsa_physics_types.h"
#include "cheonsa_scene_types.h"

namespace cheonsa
{

	// this is a memory mapped file format.
	// we load the whole file into memory.
	// lists forced to start at offsets that are multiples of 8 bytes.
	//
	// why are enums defined in the file format when they basically mirror 1 to 1 the enums defined in the engine's system? it seems redundant.
	// this is done so that files remain compatible with the engine even as enum values evolve between versions.
	// enums:
	//		resource_object_model_c::mesh_draw_c::primitive_type_e maps to video_primitive_type_e.
	//		resource_object_model_c::physics_body_c::layer_e maps to physics_layer_e.
	//		resource_object_model_c::physics_shape_c::type_e maps to physics_shape_type_e.
	//		resource_object_model_c::physics_constraint_c::type_e maps to physics_constraint_type_e.
	//		resource_object_model_c::light_c::type_e maps to scene_light_type_e.
	class resource_object_model_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_model_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "model2"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		static char8_c const * get_file_signature() { return "cm01"; }

#pragma pack( push, 1 )

		// 8 bytes.
		struct mesh_c
		{
			uint16_c name;
			uint16_c padding_0;
			uint16_c draw_start;
			uint16_c draw_end;
		};

		// 20 bytes.
		struct mesh_draw_c
		{
			enum struct primitive_type_e
			{
				point_list,
				line_list,
				line_strip,
				triangle_list,
				triangle_strip
			};

			uint8_c lod_index;
			uint8_c primitive_type_; // access this with get_video_primitive_type() instead so that remap is performed.
			uint16_c padding_0;
			uint32_c vertex_start;
			uint32_c vertex_end;
			uint32_c index_start;
			uint32_c index_end;

			video_primitive_type_e get_video_primitive_type() const;

		};

		// 64 bytes.
		struct mesh_vertex_base_c
		{
			float32_c position[ 3 ];
			float32_c normal[ 3 ];
			float32_c normal_u[ 3 ]; // 3d tangent extrapolated from direction of the 2d texture's u vector, defined by texture coordinates texture_a[0] and texture_a[1].
			float32_c normal_v[ 3 ]; // 3d tangent extrapolated form direction of the 2d texture's v vector, defined by texture coordinates texture_a[0] and texture_a[1].
			float32_c texture[ 4 ]; // 4 values, the first two elements are usually used for texture mapping, the next two elements are usually unused, but they are used to store "wave_frequency" and "wave_amplitude" parameters (which define how the wave displacement vertex shader animates the vertices).
		};

		// 16 bytes.
		// optional per-vertex data.
		// vertex skinning can operate in gpu mode and cpu mode.
		// gpu vertex skinning is most economical, can be applied to most models.
		// cpu vertex skinning will be more expensive but will also support more features, like per-vertex physics, for jiggle or more sophisticated mass spring systems.
		// cpu vertex skinning will also have three levels of quality for vertex normals:
		// do not recalculate, the original vertex normals are used.
		// recalculate one time, the vertex normals are recalculated once from a rest pose with (most likely character) customization animations applied (these animations do not disrupt the t-pose, but just change the surface level shape).
		// recalculate each frame, the vertex normals are recalculated from scratch for each frame, this will give best quality, especially for characters.
		struct mesh_vertex_bone_weight_c
		{
			uint16_c bone_weights[ 4 ]; // up to four bones may influence this vertex. treat as normalized: 0 means 0.0, 65535 means 1.0. if a weight is 0, then all subsequent weights will also be 0.
			uint16_c bone_indices[ 4 ]; // the index of the bone that influences this vertex.
		};

		//struct flesh_vertex_c
		//{
		//	uint32_c vertex_index;
		//	float32_c mass; // mass of 0 means that vertex won't be moved by spring.
		//};

		//struct flesh_spring_c
		//{
		//	uint32_c vertex_index_a; // first vertex that this spring is anchored to.
		//	uint32_c vertex_index_b; // second vertex that this spring is anchored to.
		//	float32_c damping; // rate of energy loss from this system.
		//	float32_c tensile_strength; // this spring's resistance to stretching.
		//	float32_c compression_strength; // this spring's resistance to compressing.
		//	float32_c sheer_strength; // this spring's resistance to sheering.
		//};

		//struct flesh_header_c
		//{
		//	uint32_c flesh_count;
		//	uint32_c flesh_offset;
		//	uint32_c flesh_bone_name_count;
		//	uint32_c flesh_bone_name_offset;
		//	uint32_c flesh_vertex_count;
		//	uint32_c flesh_vertex_offset;
		//	uint32_c flesh_edge_count;
		//	uint32_c flesh_edge_offset;
		//};

		// 44 bytes.
		struct bone_c
		{
			enum struct flags_e : uint8_c
			{
				none = 0x00,
				do_not_inherit_position = 0x01, // if set, then this bone will not inherit position from its mother.
				do_not_inherit_rotation = 0x02, // if set, then this bone will not inherit rotation from its mother.
				do_not_inherit_scale = 0x04, // if set, then this bone will not inherit scale from its mother.
				do_not_rebound = 0x08, // if set, then this bone will not be used to recalculate the model's local space bounding box. useful for saving computation time on small bones like fingers and eyes.
			};

			uint16_c name; // offset into string table, unique name of this bone.
			uint16_c mother_bone_name; // name of bone in this model that is mother of this bone.
			uint8_c flags; // interpret as bone_flags_e.
			uint8_c padding_0[ 3 ];
			float32_c roll;
			float32_c head_position[ 3 ];
			float32_c head_radius; // used to recalculate local space oriented bounding box (rebound) of model instance .
			float32_c tail_position[ 3 ];
			float32_c tail_radius; // used to recalculate local space oriented bounding box (rebound) of model instance.
		};

		// calculated post-load, not stored in or loaded from the file.
		struct bone_extras_c
		{
			float32_c length; // the bone's original distance between head and tail. calculated post-load.
			space_transform_c local_space_rest_transform; // the bone's rest transform relative to it's mother bone. calculated post-load.
			space_transform_c object_space_rest_transform; // the bone's rest transform relative to it's model. calculated post-load.
			space_transform_c object_space_rest_transform_inverted; // the inverse of model_rest_transform. calculated post-load. used to build object space skin transform matrix.
		};

		// 8 bytes.
		struct bone_logic_c
		{
			uint16_c type; // offset into string table.
			uint16_c padding_0;
			uint16_c property_start; // range of bone logic properties for this bone logic.
			uint16_c property_end; // range of bone logic properties for this bone logic.
		};

		// 4 bytes.
		struct bone_logic_property_c
		{
			uint16_c name; // offset into string table.
			uint16_c value; // offset into string table. but it's up to bone logic implementation how to implement the value in the string table. maybe it's a binary value, maybe it's a human readable ascii value.
		};

		// 36 bytes.
		struct bone_attachment_c
		{
			uint16_c name; // offset into string table.
			uint16_c mother_bone_name; // name of bone in this model that is mother of this attachment point.
			float32_c position[ 3 ]; // object space position of this attachment point.
			float32_c rotation[ 4 ]; // quaternion rotation of this attachment point.
			float32_c scale; // uniform scale of this attachment point.
		};

		// 32 bytes.
		struct animation_c
		{
			uint16_c name; // offset into string table.
			uint16_c padding_0;
			float32_c move_speed; // for locomotion animations, this is the speed that the character should move when the play back speed is 1x. can be in meters, radians, or degrees, depends on how the game is implemented to interpret it.
			float32_c time_in; // time that animation starts at. doesn't have to be 0, since keys may exist before this point in time for interpolation.
			float32_c time_out; // time that animation ends at. keys may exist after this point in time for interpolation.
			uint32_c object_start; // the first object being animated by this animation.
			uint32_c object_end; // the last object + 1.
			uint32_c event_start; // the first event in this animation.
			uint32_c event_end; // the last event + 1.
		};

		// 12 bytes.
		struct animation_object_c
		{
			uint16_c name; // the name of the object that is being animated.
			uint16_c padding_0;
			uint32_c property_start; // the first property that is being animated.
			uint32_c property_end; // the number of properties that are being animated.
		};

		// 12 bytes.
		struct animation_property_c
		{
			enum struct type_e
			{
				position,
				rotation,
				scale
			};

			uint16_c type;
			uint16_c padding_0;
			uint32_c key_start; // the first key that animates the property.
			uint32_c key_end; // the number of keys that animate the property.
		};

		// 20 bytes.
		struct animation_key_c
		{
			float32_c time; // when this key occurs in the animation.
			float32_c value[ 4 ]; // the value of this key.
		};

		// 16 bytes.
		struct animation_event_c
		{
			float32_c time; // when this core_event_c occurs in the animation.
			uint16_c type; // offset into string table, this event's type.
			uint16_c value; // offset into string table, this event's value.
		};

		// 64 bytes.
		struct physics_body_c
		{
			enum struct flags_e
			{
				is_active = 0x01, // if true then the body is active, if false then the body is passive.
				is_dynamic = 0x02, // if true then the body is dynamic (it can be moved by interactions with other objects and forces), if false then the body is static (it still participates in collisions but it is immovable).
				is_kinematic = 0x04, // if true then it means that the body is driven by the bone, if false then it means that the bone is driven by the body.
			};

			enum struct layer_e
			{
				static_ = 0x01, // for static environments objects, collides with static, dynamic, character, and camera.
				static_no_clip = 0x02, // for regular objects, collides with static, dynamic, and character. out of phase with camera.
				dynamic = 0x04, // for regular objects, collides with static, dynamic, and camera. out of phase with character.
				dynamic_no_clip = 0x08, // for regular objects, collides with static and dynamic. out of phase with character and camera.
				character = 0x10, // for character bounds, collides with static, dynamic, and camera. out of phase with character.
				character_no_clip = 0x20, // for character bounds, collides with static and dynamic. out of phase with character and camera.
				camera = 0x40, // for player camera, collides with all layers except layers marked as no_clip. out of phase with no_clip and camera.
			};

			uint16_c name; // unique name of this physics body.
			uint16_c mother_bone_name; // name of bone in this model that this physics body is linked with (the body will drive the bone and/or the bone will drive the body (depends if animations vs physics are in effect)), or 0 if this body is supposed to drive the model itself.
			float32_c position[ 3 ];
			float32_c rotation[ 4 ];
			float32_c scale;
			float32_c mass;
			float32_c friction;
			float32_c restitution;
			float32_c linear_damping;
			float32_c angular_damping;
			uint16_c flags;
			uint16_c layer_; // use get_physics_layer() and get_physics_layer_mask() to access.
			uint16_c shape_start;
			uint16_c shape_end;

			uint16_c get_physics_layer() const; // can be interpreted as physics_layer_e.
			uint16_c get_physics_layer_mask() const;

		};

		// 52 bytes.
		struct physics_shape_c
		{
			enum struct type_e
			{
				none,
				sphere,
				box,
				capsule,
				cylinder,
				cone,
				convex_hull,
				triangle_mesh
			};

			uint16_c type_; // use get_physics_shape_type() to access.
			uint16_c padding_0;

			// 12 bytes.
			union
			{
				struct
				{
					float32_c radius; // xyz used by sphere. xy used by cone, capsule, and cylinder.
				} sphere;
				struct
				{
					float32_c width; // x used by box.
					float32_c depth; // y used by box.
					float32_c height; // z used by capsule, cylinder, and box.
				} box;
				struct
				{
					float32_c radius; // xyz used by sphere. xy used by cone, capsule, and cylinder.
					float32_c height; // z used by capsule, cylinder, and box.
				} capsule;
				struct
				{
					float32_c radius; // xyz used by sphere. xy used by cone, capsule, and cylinder.
					float32_c height; // z used by capsule, cylinder, and box.
				} cylinder;
				struct
				{
					float32_c radius; // xyz used by sphere. xy used by cone, capsule, and cylinder.
					float32_c height; // z used by capsule, cylinder, and box.
				} cone;
				struct
				{
					uint16_c vertex_start; // used by convex_hull and triangle_mesh.
					uint16_c vertex_end; // used by convex_hull and triangle_mesh. 
				} convex_hull;
				struct
				{
					uint16_c vertex_start; // used by convex_hull and triangle_mesh.
					uint16_c vertex_end; // used by convex_hull and triangle_mesh. 
					uint16_c index_start; // used by convex_hull and triangle_mesh.
					uint16_c index_end; // used by convex_hull and triangle_mesh.
				} triangle_mesh;
			};

			float32_c position[ 3 ]; // abc local position (relative to body). is only set if this shape is a part of a compound shape.
			float32_c rotation[ 4 ];
			float32_c margin;

			physics_shape_type_e get_physics_shape_type() const;

		};

		// 40 bytes.
		struct physics_constraint_c
		{
			enum struct type_e
			{
				none,
				fixed, // does not use any limit paramters.
				point, // does not use any limit paramters.
				hinge, // uses ang_x minimum and maximum for range.
				cone, // uses ang_x for twist, and ang_y and ang_z for swing. uses minimum only to define twist/swing range, not minimum and maximum.
				generic // uses all of the limit paramaters.
			};

			enum struct flags_e
			{
				none = 0,
				disable_collision = 0x01
			};

			struct parameters_hinge_c
			{
				float32_c angular_lower_a;
				float32_c angular_upper_a;
			};

			struct parameters_cone_c
			{
				float32_c angular_upper_a;
				float32_c angular_upper_b;
				float32_c angular_upper_c;
			};

			struct parameters_generic_c
			{
				float32_c linear_lower_a;
				float32_c linear_upper_a;
				float32_c linear_lower_b;
				float32_c linear_upper_b;
				float32_c linear_lower_c;
				float32_c linear_upper_c;
				float32_c angular_lower_a;
				float32_c angular_upper_a;
				float32_c angular_lower_b;
				float32_c angular_upper_b;
				float32_c angular_lower_c;
				float32_c angular_upper_c;
			};

			uint16_c type_; // use get_physics_constraint_type() to access.
			uint16_c flags;

			uint16_c parameters_start; // index into physics_parameters_list.
			uint16_c parameters_end; // not needed, but it would be used for padding otherwise.

			sint16_c body_a_index;
			sint16_c body_b_index;

			float32_c frame_position[ 3 ];
			float32_c frame_rotation[ 4 ];

			physics_constraint_type_e get_physics_constraint_type() const;

		};

		// 12 bytes.
		struct physics_vertex_c
		{
			float32_c position[ 3 ];
		};

		// 60 bytes.
		struct light_c
		{
			enum struct type_e : uint16_c
			{
				direction,
				point,
				cone
			};

			enum struct flags_e : uint16_c
			{
				none = 0,
				render_enable = 0x01,
				shadow_cast_enable = 0x02,
			};

			uint16_c name;
			uint16_c mother_bone_name; // name of bone in this model that this light will inherit world space transform from, or 0 if this light will inherit world space transform from the model instance.
			uint16_c type_; // use get_scene_light_type() to access.
			uint16_c flags;
			float32_c position[ 3 ];
			float32_c rotation[ 4 ];
			float32_c color[ 3 ];
			float32_c brightness;
			float32_c range_; // don't use any more, range should be found as a function of brightness.
			float32_c cone_angle;

			scene_light_type_e get_scene_light_type() const;

		};

		// 4 bytes.
		struct property_c
		{
			uint16_c name;
			uint16_c value;
		};

		// version 1.
		// 124 bytes.
		struct header_v1_c
		{
			char8_c signature[ 3 ]; // should be "chm".
			char8_c version;		// should be 1.

			sint32_c mesh_bone_name_count;
			sint32_c mesh_count;
			sint32_c mesh_draw_count;
			sint32_c mesh_vertex_count;
			sint32_c mesh_index_count;

			sint32_c bone_count;
			sint32_c bone_logic_count;
			sint32_c bone_logic_property_count;
			sint32_c bone_attachment_count;

			sint32_c animation_count;
			sint32_c animation_object_count;
			sint32_c animation_property_count;
			sint32_c animation_key_count;
			sint32_c animation_event_count;

			sint32_c physics_body_count;
			sint32_c physics_shape_count;
			sint32_c physics_vertex_count;
			sint32_c physics_index_count;
			sint32_c physics_constraint_count;
			sint32_c physics_parameter_count;

			sint32_c light_count;

			sint32_c property_count;

			sint32_c string_table_length;

		};

#pragma pack( pop )

		// usable data extracted from version specific header.
		// each list is just a wrapper for chunks of memory in _raw_data.
		struct data_c
		{
			box32x3_c mesh_box; // calculated post-load, minimum axis aligned bounding box that contains all the vertices in the rest pose.

			core_list_c< uint16_c > mesh_bone_name_list; // names of bones that will be used to skin in the model.
			core_list_c< mesh_c > mesh_list;
			core_list_c< mesh_draw_c > mesh_draw_list;
			core_list_c< mesh_vertex_base_c > mesh_vertex_list_base;
			core_list_c< mesh_vertex_bone_weight_c > mesh_vertex_list_bone_weight; // bone indices reference bones in mesh_bone_name_list. should only be present if bone names are defined. length should be same as mesh_vertex_base_list, even if the model contains a mix of skinned and unskinned meshes (will be a little waste of memory for the unskinned meshes).
			core_list_c< uint16_c > mesh_index_list; // interpret internal array as uint16_c if _header->mesh_index_count <= 65535. reinterpret internal array as uint32_c if otherwise.

			core_list_c< bone_c > bone_list;
			core_list_c< bone_logic_c > bone_logic_list;
			core_list_c< bone_logic_property_c > bone_logic_property_list;
			core_list_c< bone_attachment_c > bone_attachment_list;

			core_list_c< animation_c > animation_list;
			core_list_c< animation_object_c > animation_object_list;
			core_list_c< animation_property_c > animation_property_list;
			core_list_c< animation_key_c > animation_key_list;
			core_list_c< animation_event_c > animation_event_list;

			core_list_c< physics_body_c > physics_body_list;
			core_list_c< physics_shape_c > physics_shape_list;
			core_list_c< physics_vertex_c > physics_vertex_list;
			core_list_c< uint16_c > physics_index_list;
			core_list_c< physics_constraint_c > physics_constraint_list;
			core_list_c< float32_c > physics_parameters_list;

			core_list_c< light_c > light_list;

			core_list_c< property_c > property_list;

			core_list_c< char8_c > string_table;

			core_list_c< bone_extras_c > bone_extras_list;

			// this vertex buffer holds the base geometry.
			// the vertex format is video_renderer_vertex_format_mesh, struct video_renderer_vertex_mesh_base_c.
			// when drawing the model without gpu skinning, this vertex buffer is used by itself.
			// when drawing the model with gpu skinning, this vertex buffer is used in tandem with _mesh_vertex_buffer_skin.
			video_vertex_buffer_c * mesh_vertex_buffer_base;
			// this vertex buffer holds bone weights and bone indices.
			// the vertex format is video_renderer_vertex_format_skin, struct video_renderer_vertex_mesh_bone_weight_c.
			// when drawing the model with gpu skinning, this vertex buffer is used in tandem with _mesh_vertex_buffer_mesh.
			video_vertex_buffer_c * mesh_vertex_buffer_bone_weight;
			// index buffer used to draw the mesh primitives.
			// meshes reference draws, which reference a range of indices in this list.
			// each draw is typically always formatted as a triangle list.
			video_index_buffer_c * mesh_index_buffer;

			void_c reset();

		};

	private:
		friend class video_renderer_interface_c;

		sint32_c _raw_data_size; // size of the file in bytes.
		uint8_c * _raw_data; // all of the file loaded into memory.
		data_c _data; // data extracted from _raw_data.

	public:
		sint32_c find_bone_index_with_bone_name( uint16_c bone_name ); // bone_name as offset into string table.
		sint32_c find_bone_index_with_bone_name( string8_c const & bone_name );
		boolean_c find_bone_logic_property_as_sint32( bone_logic_c const * logic, string8_c const & name, sint32_c & value );
		boolean_c find_bone_logic_property_as_float32( bone_logic_c const * logic, string8_c const & name, float32_c & value );
		boolean_c find_bone_logic_property_as_string8( bone_logic_c const * logic, string8_c const & name, string8_c & value );

	private:
		virtual boolean_c _load( data_stream_c * stream );
		virtual void_c _unload();

	public:
		resource_object_model_c();
		resource_object_model_c( resource_object_model_c const & ) = delete;
		virtual ~resource_object_model_c() override;
		resource_object_model_c & operator = ( resource_object_model_c const & ) = delete;

		data_c const & get_data() const;

		// builds rest pose bone transform in object space, without scale.
		static space_transform_c make_bone_transform( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll, vector32x3_c const & global_up );

	};

}
