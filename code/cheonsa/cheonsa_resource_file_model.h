#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_video_types.h"
#include "cheonsa_physics_types.h"
#include "cheonsa_scene_types.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_video_renderer_types.h" // just to cross check static_assert sizeof stuff.

namespace cheonsa
{

	// loads ".model" files.
	// (file extension is a suggestion, you may deviate from it).
	// this is a binary format.
	// if the file's byte order is the same as the environment's byte order, then the whole file is loaded into memory, and static mode lists are wrapped around sub-ranges of that memory.
	// if the file's byte order is not the same as the environment's byte order, then dynamic mode lists are allocated and each binary value is loaded one by one so that byte order can be flipped.
	// ideally and in practice, little endian byte order is popular these days and is preferable to big endian.
	// i think GPU these days are all little endian, in which case the conversion step is currently missing.
	//
	// why are enums defined in the file format when they basically mirror 1 to 1 the enums defined in the engine sub-systems? it seems redundant.
	// this is done so that files can continue to remain compatible with the engine even as enum values on the engine sub-systems side are modified between versions.
	// enums:
	//		resource_file_model_c::mesh_draw_c::primitive_type_e maps to video_primitive_type_e.
	//		resource_file_model_c::physics_rigid_body_c::layer_e maps to physics_layer_e.
	//		resource_file_model_c::physics_shape_c::type_e maps to physics_shape_type_e.
	//		resource_file_model_c::physics_constraint_c::type_e maps to physics_constraint_type_e.
	//		resource_file_model_c::light_c::type_e maps to scene_light_type_e.
	class resource_file_model_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_model_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "model"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

	public:
#pragma pack( push, 1 )

		struct string_table_c
		{
			static char8_c const * get_signature() { return "st__"; }

		};

		// mesh vertex bone weights reference bones by their indices.
		// the file contains a list of these which is used to map the bone indices to bone names.
		struct mesh_bone_name_c
		{
			static char8_c const * get_signature() { return "mbn_"; }

			uint16_c name; // an offset that references a string in the string table.

		};
		static_assert( sizeof( mesh_bone_name_c ) == 2, "just checking." );

		struct mesh_c
		{
			static char8_c const * get_signature() { return "m___"; }

			uint16_c name; // an offset that references a string in the string table.
			uint16_c padding_0;
			uint16_c draw_start;
			uint16_c draw_end;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( name );
				scribe.load_uint16( padding_0 );
				scribe.load_uint16( draw_start );
				scribe.load_uint16( draw_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( mesh_c ) == 8, "just checking." );

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
			uint32_c vertex_start; // index of first vertex in this mesh draw, inclusive. this was for direct3d 9 DrawIndexedPrimitive, it's no longer needed in direct3d 10 and above.
			uint32_c vertex_end; // index of last vertex in this mesh draw, exclusive. vertex_end - vertex_start is number of vertices in this mesh draw. this was for direct3d 9 DrawIndexedPrimitive, it's no longer needed in direct3d 10 and above.
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
				scribe.load_uint8( lod_index );
				scribe.load_uint8( primitive_type_ );
				scribe.load_uint16( padding_0 );
				scribe.load_uint32( vertex_start );
				scribe.load_uint32( vertex_end );
				scribe.load_uint32( index_start );
				scribe.load_uint32( index_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( mesh_draw_c ) == 20, "just checking." );

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
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				scribe.load_float32( normal[ 0 ] );
				scribe.load_float32( normal[ 1 ] );
				scribe.load_float32( normal[ 2 ] );
				scribe.load_float32( normal_u[ 0 ] );
				scribe.load_float32( normal_u[ 1 ] );
				scribe.load_float32( normal_u[ 2 ] );
				scribe.load_float32( normal_v[ 0 ] );
				scribe.load_float32( normal_v[ 1 ] );
				scribe.load_float32( normal_v[ 2 ] );
				scribe.load_float32( texture[ 0 ] );
				scribe.load_float32( texture[ 1 ] );
				scribe.load_float32( texture[ 2 ] );
				scribe.load_float32( texture[ 3 ] );
				return !scribe.encountered_error();;
			}

		};
		static_assert( sizeof( mesh_vertex_base_c ) == 64, "just checking." );
		static_assert( sizeof( mesh_vertex_base_c ) == sizeof( video_renderer_vertex_mesh_base_c ), "just checking." );

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
				scribe.load_uint16( bone_weights[ 0 ] );
				scribe.load_uint16( bone_weights[ 1 ] );
				scribe.load_uint16( bone_weights[ 2 ] );
				scribe.load_uint16( bone_weights[ 3 ] );
				scribe.load_uint16( bone_indices[ 0 ] );
				scribe.load_uint16( bone_indices[ 1 ] );
				scribe.load_uint16( bone_indices[ 2 ] );
				scribe.load_uint16( bone_indices[ 3 ] );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( mesh_vertex_bone_weight_c ) == 16, "just checking." );
		static_assert( sizeof( mesh_vertex_bone_weight_c ) == sizeof( video_renderer_vertex_mesh_bone_weight_c ), "just checking." );

		// empty struct, just to define get_signature().
		// if vertex list length is less than 65535, then actual type is uint16_c, else actual type is uint32_c.
		struct mesh_index_c
		{
			static char8_c const * get_signature() { return "mi__"; }

		};

		// in blender, each shape key provides vertex coordinates for all vertices in the mesh, even if there is no relative difference.
		// in cheonsa, we ommit the ones with no relative difference, so we have a vertex index to say which vertex is being modified.
		struct mesh_shape_key_vertex_c
		{
			static char8_c const * get_signature() { return "mskv"; }

			uint32_c vertex_index; // index of the vertex that this shape key vertex modifies. unlike the mesh index list which can be uint16_c or uint32_c (depending on how many vertices there are), this is always uint32_c.
			float32_c position[ 3 ]; // vertex position relative to base mesh vertex position.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint32( vertex_index );
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( mesh_shape_key_vertex_c ) == 16, "just checking." );

		struct mesh_shape_key_c
		{
			static char8_c const * get_signature() { return "msk_"; }

			uint16_c name; // used by the game to identify this shape key by.
			uint16_c mesh_index; // the index of the mesh that this shape key belongs to and affects.

			uint32_c shape_key_vertex_start; // index of first shape key vertex in this shape key, inclusive.
			uint32_c shape_key_vertex_end; // index of last shape key vertex in this shape key, exclusive. shape_key_vertex_end - shape_key_vertex_start.

			float32_c minimum_value; // minimum allowable weight value.
			float32_c maximum_value; // maximum allowable weight value.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( name );
				scribe.load_uint16( mesh_index );
				scribe.load_uint32( shape_key_vertex_start );
				scribe.load_uint32( shape_key_vertex_end );
				scribe.load_float32( minimum_value );
				scribe.load_float32( maximum_value );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( mesh_shape_key_c ) == 20, "just checking." );

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

		struct bone_c
		{
			static char8_c const * get_signature() { return "b___"; }

			// these flags are kind of backwards aren't they...
			// usually, when something is set to 1, it means it is turned on, not turned off.
			enum flags_e
			{
				flags_e_none = 0x00,
				flags_e_inherit_position = 0x01, // if set, then this bone will inherit position from its mother.
				flags_e_inherit_rotation = 0x02, // if set, then this bone will inherit rotation from its mother.
				flags_e_inherit_scale = 0x04, // if set, then this bone will inherit scale from its mother.
				flags_e_rebound = 0x08, // if set, then this bone will be used to recalculate the model's local space bounding box. used to optimize rebounding by avoiding small bones like fingers and eyes.
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
				scribe.load_uint16( name );
				scribe.load_uint16( mother_bone_name );
				scribe.load_uint8( flags );
				scribe.load_uint8( padding_0[ 0 ] );
				scribe.load_uint8( padding_0[ 1 ] );
				scribe.load_uint8( padding_0[ 2 ] );
				scribe.load_float32( roll );
				scribe.load_float32( head_position[ 0 ] );
				scribe.load_float32( head_position[ 1 ] );
				scribe.load_float32( head_position[ 2 ] );
				scribe.load_float32( head_radius );
				scribe.load_float32( tail_position[ 0 ] );
				scribe.load_float32( tail_position[ 1 ] );
				scribe.load_float32( tail_position[ 2 ] );
				scribe.load_float32( tail_radius );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( bone_c ) == 44, "just checking." );

		// calculated post-load, not stored in or loaded from the file.
		struct bone_extras_c
		{
			float32_c length; // the bone's original distance between head and tail. calculated post-load.
			transform3d_c local_space_rest_transform; // the bone's rest transform relative to it's mother bone. calculated post-load.
			transform3d_c object_space_rest_transform; // the bone's rest transform relative to it's model. calculated post-load.
			transform3d_c object_space_rest_transform_inverted; // the inverse of model_rest_transform. calculated post-load. used to build object space skin transform matrix.
		};

		struct bone_logic_c
		{
			static char8_c const * get_signature() { return "bl__"; }

			uint16_c type; // offset into string table.
			uint16_c padding_0;
			uint16_c property_start; // range of bone logic properties for this bone logic.
			uint16_c property_end; // range of bone logic properties for this bone logic.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( type );
				scribe.load_uint16( padding_0 );
				scribe.load_uint16( property_start );
				scribe.load_uint16( property_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( bone_logic_c ) == 8, "just checking." );

		struct bone_logic_property_c
		{
			static char8_c const * get_signature() { return "blp_"; }

			uint16_c name; // offset into string table.
			uint16_c value; // offset into string table. but it's up to bone logic implementation how to implement the value in the string table. maybe it's a binary value, maybe it's a human readable ascii value.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( name );
				scribe.load_uint16( value );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( bone_logic_property_c ) == 4, "just checking." );

		struct attachment_point_c
		{
			static char8_c const * get_signature() { return "ap__"; }

			uint16_c name; // offset into string table.
			uint16_c mother_bone_name; // name of bone in this model that is mother of this attachment point.
			float32_c position[ 3 ]; // object space position of this attachment point.
			float32_c rotation[ 4 ]; // quaternion rotation of this attachment point.
			float32_c scale; // uniform scale of this attachment point.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( name );
				scribe.load_uint16( mother_bone_name );
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				scribe.load_float32( rotation[ 0 ] );
				scribe.load_float32( rotation[ 1 ] );
				scribe.load_float32( rotation[ 2 ] );
				scribe.load_float32( rotation[ 3 ] );
				scribe.load_float32( scale );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( attachment_point_c ) == 36, "just checking." );

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
				scribe.load_uint16( name );
				scribe.load_uint16( padding_0 );
				scribe.load_float32( move_speed );
				scribe.load_float32( time_in );
				scribe.load_float32( time_out );
				scribe.load_uint32( object_start );
				scribe.load_uint32( object_end );
				scribe.load_uint32( event_start );
				scribe.load_uint32( event_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( animation_c ) == 32, "just checking." );

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
				scribe.load_uint16( name );
				scribe.load_uint16( padding_0 );
				scribe.load_uint32( property_start );
				scribe.load_uint32( property_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( animation_object_c ) == 12, "just checking." );

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
				scribe.load_uint16( type );
				scribe.load_uint16( padding_0 );
				scribe.load_uint32( key_start );
				scribe.load_uint32( key_end );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( animation_property_c ) == 12, "just checking." );

		struct animation_key_c
		{
			static char8_c const * get_signature() { return "ak__"; }

			float32_c time; // when this key occurs in the animation.
			float32_c value[ 4 ]; // the value of this key.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_float32( time );
				scribe.load_float32( value[ 0 ] );
				scribe.load_float32( value[ 1 ] );
				scribe.load_float32( value[ 2 ] );
				scribe.load_float32( value[ 3 ] );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( animation_key_c ) == 20, "just checking." );

		struct animation_event_c
		{
			static char8_c const * get_signature() { return "ae__"; }

			float32_c time; // when this core_event_c occurs in the animation.
			uint16_c type; // offset into string table, this event's type.
			uint16_c value; // offset into string table, this event's value.

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_float32( time );
				scribe.load_uint16( type );
				scribe.load_uint16( value );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( animation_event_c ) == 8, "just checking." );

		struct physics_rigid_body_c
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
			uint16_c mother_bone_name; // name of bone in this model that this physics body is linked with, or 0 if this physics body is linked with the model itself.
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
			uint16_c effects_material_name; // extra parameter that can reference a game defined effects material that defines sound effects and visual effects to play during collisions.
			uint16_c padding_0;

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
				scribe.load_uint16( name );
				scribe.load_uint16( mother_bone_name );
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				scribe.load_float32( rotation[ 0 ] );
				scribe.load_float32( rotation[ 1 ] );
				scribe.load_float32( rotation[ 2 ] );
				scribe.load_float32( rotation[ 3 ] );
				scribe.load_float32( scale );
				scribe.load_float32( mass );
				scribe.load_float32( friction );
				scribe.load_float32( restitution );
				scribe.load_float32( linear_damping );
				scribe.load_float32( angular_damping );
				scribe.load_uint16( flags );
				scribe.load_uint16( layer_ );
				scribe.load_uint16( shape_start );
				scribe.load_uint16( shape_end );
				scribe.load_uint16( effects_material_name );
				scribe.load_uint16( padding_0 );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( physics_rigid_body_c ) == 68, "just checking." );

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
					float32_c height; // z used by capsule, cylinder, and box. this is height, so to convert it to segment length, subtract by radius * 2.
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

			float32_c position[ 3 ]; // local position (relative to body), is only set if this shape is a part of a compound shape.
			float32_c rotation[ 4 ]; // local rotation (relative to body), is only set if this shape is a part of a compound shape.
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
				scribe.load_uint16( type_ );
				scribe.load_uint16( padding_0 );

				sint32_c return_stream_position = scribe.get_stream()->get_position() + 12;
				if ( type_ == type_e_sphere )
				{
					scribe.load_float32( sphere.radius );
				}
				else if ( type_ == type_e_box )
				{
					scribe.load_float32( box.width );
					scribe.load_float32( box.depth );
					scribe.load_float32( box.height );
				}
				else if ( type_ == type_e_capsule || type_ == type_e_cylinder || type_ == type_e_cone )
				{
					scribe.load_float32( capsule.radius );
					scribe.load_float32( capsule.height );
				}
				else if ( type_ == type_e_convex_hull )
				{
					scribe.load_uint16( convex_hull.vertex_start );
					scribe.load_uint16( convex_hull.vertex_end );
				}
				else if ( type_ == type_e_triangle_mesh )
				{
					scribe.load_uint16( triangle_mesh.vertex_start );
					scribe.load_uint16( triangle_mesh.vertex_end );
					scribe.load_uint16( triangle_mesh.index_start );
					scribe.load_uint16( triangle_mesh.index_end );
				}
				else
				{
					return false;
				}

				if ( !scribe.get_stream()->set_position( return_stream_position ) )
				{
					return false;
				}

				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				scribe.load_float32( rotation[ 0 ] );
				scribe.load_float32( rotation[ 1 ] );
				scribe.load_float32( rotation[ 2 ] );
				scribe.load_float32( rotation[ 3 ] );
				scribe.load_float32( margin );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( physics_shape_c ) == 48, "just checking" );

		struct physics_vertex_c
		{
			static char8_c const * get_signature() { return "pv__"; }

			float32_c position[ 3 ];

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( physics_vertex_c ) == 12, "just checking" );

		struct physics_index_c
		{
			static char8_c const * get_signature() { return "pi__"; }

			uint16_c value;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				if ( !scribe.load_uint16( value ) )
				{
					return false;
				}
				return true;
			}

		};
		static_assert( sizeof( physics_index_c ) == 2, "just checking" );

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
				flags_e_none = 0,
				flags_e_disable_collisions = 0x01, // if set then collisions are disabled between the two rigid bodies that are linked by the constraint.
			};

			uint16_c type_; // use get_physics_constraint_type() to access.
			uint16_c flags;

			sint16_c parameters_start;
			sint16_c padding_0;

			sint16_c body_a_index;
			sint16_c body_b_index;

			float32_c frame_position[ 3 ];
			float32_c frame_rotation[ 4 ];

			//float32_c linear_limits[ 3 ][ 2 ]; // min and max limits for x, y, and z axes.
			//float32_c angular_limits[ 3 ][ 2 ]; // min and max limits for x, y, and z axes.

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
				scribe.load_uint16( type_ );
				scribe.load_uint16( flags );
				scribe.load_sint16( parameters_start );
				scribe.load_sint16( padding_0 );
				scribe.load_sint16( body_a_index );
				scribe.load_sint16( body_b_index );
				scribe.load_float32( frame_position[ 0 ] );
				scribe.load_float32( frame_position[ 1 ] );
				scribe.load_float32( frame_position[ 2 ] );
				scribe.load_float32( frame_rotation[ 0 ] );
				scribe.load_float32( frame_rotation[ 1 ] );
				scribe.load_float32( frame_rotation[ 2 ] );
				scribe.load_float32( frame_rotation[ 3 ] );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( physics_constraint_c ) == 40, "just checking." );

		struct physics_parameter_c
		{
			static char8_c const * get_signature() { return "pp__"; }

			float32_c value;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_float32( value );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( physics_parameter_c ) == 4, "just checking" );

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
				flags_e_render_enabled = 0x01,
				flags_e_shadow_cast_enabled = 0x02,
			};

			uint16_c name;
			uint16_c mother_bone_name; // name of bone in this model that this light will inherit world space transform from, or 0 if this light will inherit world space transform from the model instance.
			uint16_c type_; // use get_scene_light_type() to access.
			uint16_c flags;
			float32_c position[ 3 ];
			float32_c rotation[ 4 ];
			float32_c color[ 3 ];
			float32_c strength;
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
				scribe.load_uint16( name );
				scribe.load_uint16( mother_bone_name );
				scribe.load_uint16( type_ );
				scribe.load_uint16( flags );
				scribe.load_float32( position[ 0 ] );
				scribe.load_float32( position[ 1 ] );
				scribe.load_float32( position[ 2 ] );
				scribe.load_float32( rotation[ 0 ] );
				scribe.load_float32( rotation[ 1 ] );
				scribe.load_float32( rotation[ 2 ] );
				scribe.load_float32( rotation[ 3 ] );
				scribe.load_float32( color[ 0 ] );
				scribe.load_float32( color[ 1 ] );
				scribe.load_float32( color[ 2 ] );
				scribe.load_float32( strength );
				scribe.load_float32( range_ );
				scribe.load_float32( cone_angle );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( light_c ) == 60, "just checking" );

		struct property_c
		{
			static char8_c const * get_signature() { return "prop"; }

			uint16_c name;
			uint16_c value;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.load_uint16( name );
				scribe.load_uint16( value );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( property_c ) == 4, "just checking." );

		struct chunk_header_c
		{
			char8_c signature[ 4 ]; // type signature.
			sint32_c count;
			sint32_c data_offset;
			sint32_c data_size;

			inline boolean_c load( data_scribe_binary_c & scribe )
			{
				scribe.get_stream()->load( signature, 4 );
				scribe.load_sint32( count );
				scribe.load_sint32( data_offset );
				scribe.load_sint32( data_size );
				return !scribe.encountered_error();
			}

		};
		static_assert( sizeof( chunk_header_c ) == 16, "just checking." );

#pragma pack( pop )

		// usable data extracted from version specific header.
		// in the case that the file's byte order is the same as the environment's native byte order, then the whole file is loaded into memory as _raw_data and then each list instance wraps around parts of that memory.
		// in the case that the file's byte order is not the same as the environment's native byte order, then each list is allocated dynamically.
		struct data_c
		{
			core_list_c< char8_c > string_table;

			box32x3_c mesh_box; // calculated post-load, minimum axis aligned bounding box that contains all the vertices in the rest pose.

			core_list_c< mesh_bone_name_c > mesh_bone_name_list; // names of bones that will be used to skin in the model.
			core_list_c< mesh_c > mesh_list;
			core_list_c< mesh_draw_c > mesh_draw_list;
			core_list_c< mesh_vertex_base_c > mesh_vertex_list_base;
			core_list_c< mesh_vertex_bone_weight_c > mesh_vertex_list_bone_weight; // bone indices reference bones in mesh_bone_name_list. should only be present if bone names are defined. length should be same as mesh_vertex_base_list, even if the model contains a mix of skinned and unskinned meshes (will be a little waste of memory for the unskinned meshes).
			core_list_c< uint16_c > mesh_index_list; // interpret internal array as uint16_c if mesh_vertex_list_base length is <= 65535. reinterpret internal array as uint32_c if otherwise.
			core_list_c< mesh_shape_key_vertex_c > mesh_shape_key_vertex_list;
			core_list_c< mesh_shape_key_c > mesh_shape_key_list;

			core_list_c< bone_c > bone_list;
			core_list_c< bone_logic_c > bone_logic_list;
			core_list_c< bone_logic_property_c > bone_logic_property_list;

			core_list_c< attachment_point_c > attachment_point_list;

			core_list_c< animation_c > animation_list;
			core_list_c< animation_object_c > animation_object_list;
			core_list_c< animation_property_c > animation_property_list;
			core_list_c< animation_key_c > animation_key_list;
			core_list_c< animation_event_c > animation_event_list;

			core_list_c< physics_rigid_body_c > physics_body_list;
			core_list_c< physics_shape_c > physics_shape_list;
			core_list_c< physics_vertex_c > physics_vertex_list;
			core_list_c< physics_index_c > physics_index_list;
			core_list_c< physics_constraint_c > physics_constraint_list;
			core_list_c< physics_parameter_c > physics_parameter_list;

			core_list_c< light_c > light_list;

			core_list_c< property_c > property_list;

			core_list_c< bone_extras_c > bone_extras_list;

			core_list_c< video_renderer_vertex_mesh_base_c > _cpu_vertex_buffer_mesh_base; // wrapper for mesh_vertex_list_base with type reinterpreted as video_renderer_vertex_mesh_base_c, so that the renderer can use it. can be used as input for cpu skinning.
			video_vertex_buffer_c * _gpu_vertex_buffer_mesh_base; // gpu mirror copy of _cpu_vertex_buffer_mesh_base. can be used as input for gpu skinning.
			core_list_c< video_renderer_vertex_mesh_bone_weight_c > _cpu_vertex_buffer_mesh_bone_weight; // wrapper for mesh_vertex_list_bone_weight with type reinterpreted as video_renderer_vertex_mesh_bone_weight_c, so that the renderer can use it. can be used as input for cpu skinning.
			video_vertex_buffer_c * _gpu_vertex_buffer_mesh_bone_weight; // gpu mirror copy of _cpu_vertex_buffer_mesh_bone_weight. can be used as input for gpu skinning.
			video_index_buffer_c * _gpu_mesh_index_buffer; // gpu mirror copy of mesh_index_list. 

			void_c reset();

		};

	private:
		friend class video_renderer_interface_c;

		sint32_c _raw_data_size; // size of the source file in bytes.
		uint8_c * _raw_data; // all of the source file loaded into memory. this buffer is only allocated in the case that the file is saved with a byte order that matches this environment's native byte order, then the lists in _data reference this memory directly. otherwise, the lists in _data are allocated to hold byte swapped data and there's extra memory overhead.

		data_c _data;

	public:
		sint32_c find_bone_index_with_bone_name( uint16_c bone_name ); // bone_name as offset into string table.
		sint32_c find_bone_index_with_bone_name( string8_c const & bone_name );
		boolean_c find_bone_logic_property_as_sint32( bone_logic_c const * logic, string8_c const & name, sint32_c & value );
		boolean_c find_bone_logic_property_as_float32( bone_logic_c const * logic, string8_c const & name, float32_c & value );
		boolean_c find_bone_logic_property_as_string8( bone_logic_c const * logic, string8_c const & name, string8_c & value );

	private:
		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_model_c( string16_c const & file_path );

		data_c const & get_data() const;

		// helper function for looking up strings.
		inline string8_c get_string( uint16_c offset ) const
		{
			return string8_c( core_list_mode_e_static, &_data.string_table[ offset ] );
		}

		// helper function for looking up strings.
		inline char8_c const * get_string_pointer( uint16_c offset ) const
		{
			return &_data.string_table[ offset ];
		}

		// helper function for one-off sampling of animation object spatial properties in this model resource.
		// the animation player code in the model component is better optimized for sampling of animation data each frame, since there is temporal coherence between contiguous frames.
		void_c sample_animation_object( sint32_c animation_object_index, float32_c time, vector32x3_c & result_position, quaternion32_c & result_rotation, vector32x3_c & result_scale ) const;

		// builds rest pose bone transform in object space, without scale.
		static transform3d_c make_bone_transform( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll, vector32x3_c const & global_up );

		static transform3d_c make_bone_transform2( vector64x3_c const & bone_head, vector64x3_c const & bone_tail, float32_c bone_roll );

	};

}
