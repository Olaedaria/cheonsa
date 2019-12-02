#pragma once

#include "cheonsa_resource_file.h"
#include "cheonsa_video_types.h"
#include "cheonsa_physics_types.h"
#include "cheonsa_scene_types.h"
#include "cheonsa_data_scribe_binary.h"

namespace cheonsa
{

	// if the file's byte order is the same as the environment's byte order, then the whole file is loaded into memory, and static mode lists are wrapped around that memory.
	// if the file's byte order is not the same as the environment's byte order, then dynamic mode lists are allocated and each value is loaded one by one so that byte order can be flipped.
	// ideally and in practice, little endian byte order is in and we only want to use this byte order, so IDK why i wrote all the extra code.
	// i think GPU these days are all little endian, in which case the conversion step is currently missing.
	//
	// why are enums defined in the file format when they basically mirror 1 to 1 the enums defined in the engine sub-systems? it seems redundant.
	// this is done so that files can continue to remain compatible with the engine even as enum values on the engine sub-systems side are modified between versions.
	// enums:
	//		resource_file_model_c::mesh_draw_c::primitive_type_e maps to video_primitive_type_e.
	//		resource_file_model_c::physics_body_c::layer_e maps to physics_layer_e.
	//		resource_file_model_c::physics_shape_c::type_e maps to physics_shape_type_e.
	//		resource_file_model_c::physics_constraint_c::type_e maps to physics_constraint_type_e.
	//		resource_file_model_c::light_c::type_e maps to scene_light_type_e.
	class resource_file_model_c : public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_model_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "model2"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		static char8_c const * get_file_signature() { return "cm01"; }

#pragma pack( push, 1 )

		// 2 bytes for uint16_c string reference.
		struct mesh_bone_name_c
		{
			static char8_c const * get_signature() { return "mbn_"; }

		};

		// 8 bytes.
		struct mesh_c
		{
			static char8_c const * get_signature() { return "m___"; }

			uint16_c name;
			uint16_c padding_0;
			uint16_c draw_start;
			uint16_c draw_end;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( draw_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( draw_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 20 bytes.
		struct mesh_draw_c
		{
			static char8_c const * get_signature() { return "md__"; }

			enum primitive_type_e
			{
				primitive_type_e_point_list,
				primitive_type_e_line_list,
				primitive_type_e_line_strip,
				primitive_type_e_triangle_list,
				primitive_type_e_triangle_strip
			};

			uint8_c lod_index;
			uint8_c primitive_type_; // access this with get_video_primitive_type() instead so that remap is performed.
			uint16_c padding_0;
			uint32_c vertex_start; // index of first vertex in this mesh draw, inclusive.
			uint32_c vertex_end; // index of last vertex in this mesh draw, exclusive. vertex_end - vertex_start is number of vertices in this mesh draw.
			uint32_c index_start; // index of first index in this mesh draw, inclusive.
			uint32_c index_end; // index of last index in this mesh draw, exclusive. index_end - index_start is the number of indices in this mesh draw.

			inline video_primitive_type_e get_video_primitive_type() const
			{
				switch ( static_cast< primitive_type_e >( primitive_type_ ) )
				{
					case primitive_type_e_triangle_list:
						return video_primitive_type_e_triangle_list;
					case primitive_type_e_triangle_strip:
						return video_primitive_type_e_triangle_strip;
					case primitive_type_e_line_list:
						return video_primitive_type_e_line_list;
					case primitive_type_e_line_strip:
						return video_primitive_type_e_line_strip;
					case primitive_type_e_point_list:
						return video_primitive_type_e_point_list;
				}
				return video_primitive_type_e_point_list;
			}

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint8( lod_index ) )
				{
					return false;
				}
				if ( !scribe.load_uint8( primitive_type_ ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( vertex_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( vertex_end ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( index_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( index_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 64 bytes.
		struct mesh_vertex_base_c
		{
			static char8_c const * get_signature() { return "mvb_"; }

			float32_c position[ 3 ];
			float32_c normal[ 3 ];
			float32_c normal_u[ 3 ]; // 3d tangent extrapolated from direction of the 2d texture's u vector, defined by texture coordinates texture_a[0] and texture_a[1].
			float32_c normal_v[ 3 ]; // 3d tangent extrapolated form direction of the 2d texture's v vector, defined by texture coordinates texture_a[0] and texture_a[1].
			float32_c texture[ 4 ]; // 4 values, the first two elements are usually used for texture mapping, the next two elements are usually unused, but they are used to store "wave_frequency" and "wave_amplitude" parameters (which define how the wave displacement vertex shader animates the vertices).

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_u[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_u[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_u[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_v[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_v[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( normal_v[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( texture[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( texture[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( texture[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( texture[ 3 ] ) )
				{
					return false;
				}
				return true;
			}

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
			static char8_c const * get_signature() { return "mvbw"; }

			uint16_c bone_weights[ 4 ]; // up to four bones may influence this vertex. treat as normalized: 0 means 0.0, 65535 means 1.0. if a weight is 0, then all subsequent weights will also be 0.
			uint16_c bone_indices[ 4 ]; // the index of the bone that influences this vertex.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( bone_weights[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_weights[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_weights[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_weights[ 3 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_indices[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_indices[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_indices[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( bone_indices[ 3 ] ) )
				{
					return false;
				}
				return true;
			}

		};

		// 2 bytes for uint16_c, or 4 bytes for uint32_c.
		struct mesh_index_c
		{
			static char8_c const * get_signature() { return "mi__"; }

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
			static char8_c const * get_signature() { return "b___"; }

			// these flags are kind of backwards aren't they...
			// usually, when something is set to 1, it means it is turned on, not turned off.
			enum flags_e
			{
				flags_e_none = 0x00,
				flags_e_do_not_inherit_position = 0x01, // if set, then this bone will not inherit position from its mother.
				flags_e_do_not_inherit_rotation = 0x02, // if set, then this bone will not inherit rotation from its mother.
				flags_e_do_not_inherit_scale = 0x04, // if set, then this bone will not inherit scale from its mother.
				flags_e_do_not_rebound = 0x08, // if set, then this bone will not be used to recalculate the model's local space bounding box. useful for saving computation time on small bones like fingers and eyes.
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

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( mother_bone_name ) )
				{
					return false;
				}
				if ( !scribe.load_uint8( flags ) )
				{
					return false;
				}
				if ( !scribe.load_uint8( padding_0[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint8( padding_0[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_uint8( padding_0[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( roll ) )
				{
					return false;
				}
				if ( !scribe.load_float32( head_position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( head_position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( head_position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( head_radius ) )
				{
					return false;
				}
				if ( !scribe.load_float32( tail_position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( tail_position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( tail_position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( tail_radius ) )
				{
					return false;
				}
				return true;
			}

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
			static char8_c const * get_signature() { return "bl__"; }

			uint16_c type; // offset into string table.
			uint16_c padding_0;
			uint16_c property_start; // range of bone logic properties for this bone logic.
			uint16_c property_end; // range of bone logic properties for this bone logic.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( type ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( property_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( property_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 4 bytes.
		struct bone_logic_property_c
		{
			static char8_c const * get_signature() { return "blp_"; }

			uint16_c name; // offset into string table.
			uint16_c value; // offset into string table. but it's up to bone logic implementation how to implement the value in the string table. maybe it's a binary value, maybe it's a human readable ascii value.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( value ) )
				{
					return false;
				}
				return true;
			}

		};

		// 36 bytes.
		struct bone_attachment_c
		{
			static char8_c const * get_signature() { return "ba__"; }

			uint16_c name; // offset into string table.
			uint16_c mother_bone_name; // name of bone in this model that is mother of this attachment point.
			float32_c position[ 3 ]; // object space position of this attachment point.
			float32_c rotation[ 4 ]; // quaternion rotation of this attachment point.
			float32_c scale; // uniform scale of this attachment point.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( mother_bone_name ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 3 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( scale ) )
				{
					return false;
				}
				return true;
			}

		};

		// 32 bytes.
		struct animation_c
		{
			static char8_c const * get_signature() { return "a___"; }

			uint16_c name; // offset into string table.
			uint16_c padding_0;
			float32_c move_speed; // for locomotion animations, this is the speed that the character should move when the play back speed is 1x. can be in meters, radians, or degrees, depends on how the game is implemented to interpret it.
			float32_c time_in; // time that animation starts at. doesn't have to be 0, since keys may exist before this point in time for interpolation.
			float32_c time_out; // time that animation ends at. keys may exist after this point in time for interpolation.
			uint32_c object_start; // the first object being animated by this animation.
			uint32_c object_end; // the last object + 1.
			uint32_c event_start; // the first event in this animation.
			uint32_c event_end; // the last event + 1.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_float32( move_speed ) )
				{
					return false;
				}
				if ( !scribe.load_float32( time_in ) )
				{
					return false;
				}
				if ( !scribe.load_float32( time_out ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( object_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( object_end ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( event_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( event_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 12 bytes.
		struct animation_object_c
		{
			static char8_c const * get_signature() { return "ao__"; }

			uint16_c name; // the name of the object that is being animated.
			uint16_c padding_0;
			uint32_c property_start; // the first property that is being animated.
			uint32_c property_end; // the number of properties that are being animated.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( property_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( property_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 12 bytes.
		struct animation_property_c
		{
			static char8_c const * get_signature() { return "ap__"; }

			enum type_e
			{
				type_e_position,
				type_e_rotation,
				type_e_scale
			};

			uint16_c type;
			uint16_c padding_0;
			uint32_c key_start; // the first key that animates the property.
			uint32_c key_end; // the number of keys that animate the property.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( type ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( key_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint32( key_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 20 bytes.
		struct animation_key_c
		{
			static char8_c const * get_signature() { return "ak__"; }

			float32_c time; // when this key occurs in the animation.
			float32_c value[ 4 ]; // the value of this key.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_float32( time ) )
				{
					return false;
				}
				if ( !scribe.load_float32( value[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( value[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( value[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( value[ 3 ] ) )
				{
					return false;
				}
				return true;
			}

		};

		// 16 bytes.
		struct animation_event_c
		{
			static char8_c const * get_signature() { return "ae__"; }

			float32_c time; // when this core_event_c occurs in the animation.
			uint16_c type; // offset into string table, this event's type.
			uint16_c value; // offset into string table, this event's value.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_float32( time ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( type ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( value ) )
				{
					return false;
				}
				return true;
			}

		};

		// 64 bytes.
		struct physics_body_c
		{
			static char8_c const * get_signature() { return "pb__"; }

			enum flags_e
			{
				flags_e_is_active = 0x01, // if true then the body is active, if false then the body is passive.
				flags_e_is_dynamic = 0x02, // if true then the body is dynamic (it can be moved by interactions with other objects and forces), if false then the body is static (it still participates in collisions but it is immovable).
				flags_e_is_kinematic = 0x04, // if true then it means that the body is driven by the bone, if false then it means that the bone is driven by the body.
			};

			enum layer_e
			{
				layer_e_static_ = 0x01, // for static environments objects, collides with static, dynamic, character, and camera.
				layer_e_static_no_clip = 0x02, // for regular objects, collides with static, dynamic, and character. out of phase with camera.
				layer_e_dynamic = 0x04, // for regular objects, collides with static, dynamic, and camera. out of phase with character.
				layer_e_dynamic_no_clip = 0x08, // for regular objects, collides with static and dynamic. out of phase with character and camera.
				layer_e_character = 0x10, // for character bounds, collides with static, dynamic, and camera. out of phase with character.
				layer_e_character_no_clip = 0x20, // for character bounds, collides with static and dynamic. out of phase with character and camera.
				layer_e_camera = 0x40, // for player camera, collides with all layers except layers marked as no_clip. out of phase with no_clip and camera.
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

			inline physics_layer_e get_physics_layer() const
			{
				switch ( static_cast< layer_e >( layer_ ) )
				{
				case layer_e_static_:
					return physics_layer_e_static;
				case layer_e_static_no_clip:
					return physics_layer_e_static_no_clip;
				case layer_e_dynamic:
					return physics_layer_e_dynamic;
				case layer_e_dynamic_no_clip:
					return physics_layer_e_dynamic_no_clip;
				case layer_e_character:
					return physics_layer_e_character;
				case layer_e_character_no_clip:
					return physics_layer_e_character_no_clip;
				case layer_e_camera:
					return physics_layer_e_camera;
				}
				return physics_layer_e_static;
			}

			inline uint16_c get_physics_layer_mask() const
			{
				return physics_get_layer_mask_for_layer( get_physics_layer() );
			}

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( mother_bone_name ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 3 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( scale ) )
				{
					return false;
				}
				if ( !scribe.load_float32( mass ) )
				{
					return false;
				}
				if ( !scribe.load_float32( friction ) )
				{
					return false;
				}
				if ( !scribe.load_float32( restitution ) )
				{
					return false;
				}
				if ( !scribe.load_float32( linear_damping ) )
				{
					return false;
				}
				if ( !scribe.load_float32( angular_damping ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( flags ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( layer_ ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( shape_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( shape_end ) )
				{
					return false;
				}
				return true;
			}

		};

		// 52 bytes.
		struct physics_shape_c
		{
			static char8_c const * get_signature() { return "ps__"; }

			enum type_e
			{
				type_e_none,
				type_e_sphere,
				type_e_box,
				type_e_capsule,
				type_e_cylinder,
				type_e_cone,
				type_e_convex_hull,
				type_e_triangle_mesh
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

			inline physics_shape_type_e get_physics_shape_type() const
			{
				switch ( static_cast< type_e >( type_ ) )
				{
				case type_e_sphere:
					return physics_shape_type_e_sphere;
				case type_e_box:
					return physics_shape_type_e_box;
				case type_e_capsule:
					return physics_shape_type_e_capsule;
				case type_e_cylinder:
					return physics_shape_type_e_cylinder;
				case type_e_cone:
					return physics_shape_type_e_cone;
				case type_e_convex_hull:
					return physics_shape_type_e_convex_hull;
				case type_e_triangle_mesh:
					return physics_shape_type_e_triangle_mesh;
				}
				return physics_shape_type_e_none;
			}

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( type_ ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( padding_0 ) )
				{
					return false;
				}

				sint32_c return_stream_position = scribe.get_stream()->get_position() + 12;
				if ( type_ == type_e_sphere )
				{
					if ( !scribe.load_float32( sphere.radius ) )
					{
						return false;
					}
				}
				else if ( type_ == type_e_box )
				{
					if ( !scribe.load_float32( box.width ) )
					{
						return false;
					}
					if ( !scribe.load_float32( box.depth ) )
					{
						return false;
					}
					if ( !scribe.load_float32( box.height ) )
					{
						return false;
					}
				}
				else if ( type_ == type_e_capsule || type_ == type_e_cylinder || type_ == type_e_cone )
				{
					if ( !scribe.load_float32( capsule.radius ) )
					{
						return false;
					}
					if ( !scribe.load_float32( capsule.height ) )
					{
						return false;
					}
				}
				else if ( type_ == type_e_convex_hull )
				{
					if ( !scribe.load_uint16( convex_hull.vertex_start ) )
					{
						return false;
					}
					if ( !scribe.load_uint16( convex_hull.vertex_end ) )
					{
						return false;
					}
				}
				else if ( type_ == type_e_triangle_mesh )
				{
					if ( !scribe.load_uint16( triangle_mesh.vertex_start ) )
					{
						return false;
					}
					if ( !scribe.load_uint16( triangle_mesh.vertex_end ) )
					{
						return false;
					}
					if ( !scribe.load_uint16( triangle_mesh.index_start ) )
					{
						return false;
					}
					if ( !scribe.load_uint16( triangle_mesh.index_end ) )
					{
						return false;
					}
				}
				else
				{
					return false;
				}

				if ( !scribe.get_stream()->set_position( return_stream_position ) )
				{
					return false;
				}

				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 3 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( margin ) )
				{
					return false;
				}

				return true;
			}

		};

		// 12 bytes.
		struct physics_vertex_c
		{
			static char8_c const * get_signature() { return "pv__"; }

			float32_c position[ 3 ];

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				return true;
			}

		};

		// 2 bytes for uint16_c.
		struct physics_index_c
		{
			static char8_c const * get_signature() { return "pi__"; }

		};

		// 40 bytes.
		struct physics_constraint_c
		{
			static char8_c const * get_signature() { return "pc__"; }

			enum type_e
			{
				type_e_none,
				type_e_fixed, // does not use any limit paramters.
				type_e_point, // does not use any limit paramters.
				type_e_hinge, // uses ang_x minimum and maximum for range.
				type_e_cone, // uses ang_x for twist, and ang_y and ang_z for swing. uses minimum only to define twist/swing range, not minimum and maximum.
				type_e_generic // uses all of the limit paramaters.
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

			inline physics_constraint_type_e get_physics_constraint_type() const
			{
				switch ( static_cast< type_e >( type_ ) )
				{
				case type_e_fixed:
					return physics_constraint_type_e_fixed;
				case type_e_point:
					return physics_constraint_type_e_point;
				case type_e_hinge:
					return physics_constraint_type_e_hinge;
				case type_e_cone:
					return physics_constraint_type_e_cone;
				case type_e_generic:
					return physics_constraint_type_e_generic;
				}
				return physics_constraint_type_e_none;
			}

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( type_ ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( flags ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( parameters_start ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( parameters_end ) )
				{
					return false;
				}
				if ( !scribe.load_sint16( body_a_index ) )
				{
					return false;
				}
				if ( !scribe.load_sint16( body_b_index ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_rotation[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_rotation[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_rotation[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( frame_rotation[ 3 ] ) )
				{
					return false;
				}
				return true;
			}

			inline boolean_c safety_check()
			{
				if ( type_ == type_e_fixed || type_ == type_e_point )
				{
					if ( parameters_end - parameters_start != 0 )
					{
						return false;
					}
				}
				else if ( type_ == type_e_hinge )
				{
					if ( parameters_end - parameters_start != 2 )
					{
						return false;
					}
				}
				else if ( type_ == type_e_cone )
				{
					if ( parameters_end - parameters_start != 3 )
					{
						return false;
					}
				}
				else if ( type_ == type_e_generic )
				{
					if ( parameters_end - parameters_start != 12 )
					{
						return false;
					}
				}
				return true;
			}

		};

		// 4 bytes for float32_c.
		struct physics_parameter_c
		{
			static char8_c const * get_signature() { return "pp__"; }

		};

		// 60 bytes.
		struct light_c
		{
			static char8_c const * get_signature() { return "l___"; }

			enum type_e
			{
				type_e_direction,
				type_e_point,
				type_e_cone
			};

			enum flags_e
			{
				flags_e_none = 0,
				flags_e_render_enable = 0x01,
				flags_e_shadow_cast_enable = 0x02,
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

			inline scene_light_type_e get_scene_light_type() const
			{
				switch ( static_cast< type_e >( type_ ) )
				{
				case type_e_direction:
					return scene_light_type_e_direction;
				case type_e_point:
					return scene_light_type_e_point;
				case type_e_cone:
					return scene_light_type_e_cone;
				}
				return scene_light_type_e_point;
			}

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( mother_bone_name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( type_ ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( flags ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( position[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( rotation[ 3 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( color[ 0 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( color[ 1 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( color[ 2 ] ) )
				{
					return false;
				}
				if ( !scribe.load_float32( brightness ) )
				{
					return false;
				}
				if ( !scribe.load_float32( range_ ) )
				{
					return false;
				}
				if ( !scribe.load_float32( cone_angle ) )
				{
					return false;
				}
				return true;
			}

		};

		// 4 bytes.
		struct property_c
		{
			static char8_c const * get_signature() { return "prop"; }

			uint16_c name;
			uint16_c value;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( name ) )
				{
					return false;
				}
				if ( !scribe.load_uint16( value ) )
				{
					return false;
				}
				return true;
			}

		};

		struct string_table_c
		{
			static char8_c const * get_signature() { return "st__"; }

		};

		// 16 bytes.
		struct chunk_header_c
		{
			char8_c signature[ 4 ]; // type signature.
			sint32_c count;
			sint32_c data_offset;
			sint32_c data_size;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.get_stream()->load( signature, 4 ) )
				{
					return false;
				}
				if ( !scribe.load_sint32( count ) )
				{
					return false;
				}
				if ( !scribe.load_sint32( data_offset ) )
				{
					return false;
				}
				if ( !scribe.load_sint32( data_size ) )
				{
					return false;
				}
				return true;
			}

		};

#pragma pack( pop )

		// usable data extracted from version specific header.
		// in the case that the file's byte order is the same as the environment's native byte order, then the whole file is loaded into memory as _raw_data and then each list instance wraps around parts of that memory.
		// in the case that the file's byte order is not the same as the environment's native byte order, then each list is allocated dynamically.
		struct data_c
		{
			box32x3_c mesh_box; // calculated post-load, minimum axis aligned bounding box that contains all the vertices in the rest pose.

			core_list_c< uint16_c > mesh_bone_name_list; // names of bones that will be used to skin in the model.
			core_list_c< mesh_c > mesh_list;
			core_list_c< mesh_draw_c > mesh_draw_list;
			core_list_c< mesh_vertex_base_c > mesh_vertex_list_base;
			core_list_c< mesh_vertex_bone_weight_c > mesh_vertex_list_bone_weight; // bone indices reference bones in mesh_bone_name_list. should only be present if bone names are defined. length should be same as mesh_vertex_base_list, even if the model contains a mix of skinned and unskinned meshes (will be a little waste of memory for the unskinned meshes).
			core_list_c< uint16_c > mesh_index_list; // interpret internal array as uint16_c if length is <= 65535. reinterpret internal array as uint32_c if otherwise.

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
			core_list_c< float32_c > physics_parameter_list;

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

		data_c _data;

	public:
		sint32_c find_bone_index_with_bone_name( uint16_c bone_name ); // bone_name as offset into string table.
		sint32_c find_bone_index_with_bone_name( string8_c const & bone_name );
		boolean_c find_bone_logic_property_as_sint32( bone_logic_c const * logic, string8_c const & name, sint32_c & value );
		boolean_c find_bone_logic_property_as_float32( bone_logic_c const * logic, string8_c const & name, float32_c & value );
		boolean_c find_bone_logic_property_as_string8( bone_logic_c const * logic, string8_c const & name, string8_c & value );

	private:
		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_model_c();
		resource_file_model_c( resource_file_model_c const & ) = delete;
		virtual ~resource_file_model_c() override;
		resource_file_model_c & operator = ( resource_file_model_c const & ) = delete;

		data_c const & get_data() const;

		// builds rest pose bone transform in object space, without scale.
		static space_transform_c make_bone_transform( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll, vector32x3_c const & global_up );

	};

}
