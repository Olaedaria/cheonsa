#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_physics_shape.h"
#include "cheonsa_physics_rigid_body.h"
#include "cheonsa_physics_constraint.h"
#include "cheonsa_resource_file_model.h"
#include "cheonsa_resource_file_materials.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_core_dictionary.h"

namespace cheonsa
{

	// does a little bit of everything.
	// mainly provides 3d meshes.
	// also provides bones, poses, animations, and vertex skinning.
	// can also provide lights, physics, materials, etc.
	class scene_component_model_c
		: public scene_component_c
	{
		friend class video_renderer_interface_c;

	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_model; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }




	//
	// mother/daughter relations.
	//
	public:
		class attachment_point_c;
	private:
		scene_component_model_c * _mother_model; // mother model component, of which this model might be influenced by.
		string8_c _mother_attachment_point_name;
		attachment_point_c * _mother_attachment_point;
		core_list_c< scene_component_model_c * > _daughter_model_list; // daughter model components of this model component.
	public:
		scene_component_model_c * get_mother_model_root(); // recurses up the hierarchy to get the top-most mother model instance.
		scene_component_model_c * get_mother_model(); // gets the immediate mother model instance of this model instance.
		string8_c const & get_mother_attachment_point_name() const;
		void_c add_daughter_model( scene_component_model_c * model ); // adds a daughter model to this model. the daughter model should not be added as a component on any scene object. the daugher model will copy this model's world space transform, and its vertices may be skinned by this model's bones. adds a one-way reference to the daughter model (so that if this model gets deleted, then the daughters will be released and may be deleted as well (no circular references)).
		void_c add_daughter_model_to_attachment_point( scene_component_model_c * model, string8_c const & attachment_point_name ); // adds a daughter model to this model. the daughter model will copy the attachment point's world space transform, and its vertices may be skinned by this model's bones (but probably would not produce desirable results). 
		void_c remove_daughter_model( scene_component_model_c * model );




	//
	// model resource binding.
	//
	private:
		boolean_c _model_resource_is_bound; // will be true if a model resource is loaded and bound (instanced) and this model component is ready to be rendered.
		resource_file_model_c::reference_c _model_resource; // the model resource to load and bind, or that is already bound.
		void_c _handle_model_resource_on_loaded( resource_file_c * resource ); // responds to when _model_resource is loaded.
		void_c _handle_model_resource_on_unloaded( resource_file_c * resource ); // responds to when _model_resource is un loaded.
	public:
		resource_file_model_c * get_model_resource() const; // will return the set model resource. this can be nullptr if none is set. this resource may or may not be loaded.
		void_c set_model_resource( resource_file_model_c * model_resource ); // sets the model resource to bind to this model instance. it will bind as soon as it is able to, since a pointer to a yet-to-be-loaded model resource instance may be suppled.
		boolean_c get_model_resource_is_bound() const; // returns true only if the model resource is set, loaded, and bound.




	//
	// model (animations) resource binding.
	//
	public:
		// used with animation dictionary, the key is the animation name, the value is this.
		// it associates an animation name with animation data in a model resource file.
		class animation_map_c
		{
		public:
			resource_file_model_c const * source_model;
			resource_file_model_c::animation_c const * source_animation;

		public:
			animation_map_c();

		};
	private:
		core_list_c< resource_file_model_c::reference_c > _animations_resource_list; // optional references other model resources that contain additional animations which can then be referenced by name.
		void_c _handle_animations_resource_on_loaded( resource_file_c * resource ); // responds to when _animations_resource is loaded.
		void_c _handle_animations_resource_on_unloaded( resource_file_c * resource ); // responds to when _animations_resource is un loaded.
		core_dictionary_c< string8_c, animation_map_c > _animation_map_dictionary; // maps an animation name to an animation sourced from this model resource or from any of the model resources in _animations_resource_list.
		core_event_c< void_c, scene_component_model_c * > _on_animation_resources_updated; // animation players subscribe to this event to respond to run time reloads of animation resources.
		void_c _refresh_animation_map_dictionary(); // is called whenever _model_resource becomes bound or unbound. but it should also be called after you add or remove animations resources.
	public:
		void_c add_animations_resource( resource_file_model_c * resource ); // adds another (animations) model resource reference which contains animations that we want to make available to this model instance.
		void_c remove_animations_resource( resource_file_model_c * resource ); // removes a (animations) model resource reference that was previously associated with this model instance.
		void_c remove_all_animations_resources();




	//
	// materials resource binding.
	//
	private:
		resource_file_materials_c::reference_c _materials_resource;
		void_c _handle_materials_resource_on_loaded( resource_file_c * resource ); // responds to when _model_materials_resource is loaded.
		void_c _handle_materials_resource_on_unloaded( resource_file_c * resource ); // responds to when _model_materials_resource is un loaded.
	public:
		resource_file_materials_c * get_materials_resource() const;
		void_c set_materials_resource( resource_file_materials_c * value ); // sets the model materials resource which defines base material assignments. these base materials may be overridden by the custom material assignments feature.




	//
	// meshes.
	//
	public:
		class custom_material_c;

		// a mesh instanced from the bound and loaded model resource object.
		class mesh_c
		{
			friend class scene_component_model_c;
			friend class video_renderer_interface_c;

		public:
			class shape_key_c
			{
				friend class scene_component_model_c;

			private:
				mesh_c * _mesh; // the mesh that this shape key applies to.
				resource_file_model_c::mesh_shape_key_c const * _source_mesh_shape_key;
				float32_c _value;

			public:
				shape_key_c();

				mesh_c * get_mesh() const;

				string8_c get_name() const;

				float32_c get_value() const;
				void_c set_value( float32_c value ); // after changing this value, you will have to call apply_shape_keys() on the model instance.

				float32_c get_minimum_value() const;
				float32_c get_maximum_value() const;

			};

		private:
			scene_component_model_c * _mother_model;

			resource_file_model_c::mesh_c const * _source_mesh; // a pointer to the original mesh in the model resource that this mesh is an instance of.

			boolean_c _render_enabled; // set to true to enable this mesh instance to be rendered. this defaults to true for each mesh when it is newly instanced.

			resource_file_materials_c::thing_c const * _source_material_map_thing; // if set then this mesh has its base material properties defined by this material map thing.

			custom_material_c * _custom_material_assignment; // if set then this mesh has its materials modified by this game defined material assignment.

			//core_list_c< shape_key_c > _shape_key_list; // list of shape keys that are available on this mesh.

		public:
			mesh_c();

			scene_component_model_c * get_mother_model() const;

			string8_c get_name() const;

			boolean_c get_render_enabled() const;
			void_c set_render_enabled( boolean_c value );

		};

	private:
		core_list_c< mesh_c > _mesh_list; // list of mesh instances in this model.
	public:
		// names can be a single name, or a semi-colon separated list of names.
		// each name may end with a wild card ("*", asterisk) to indicate that it should match with all meshes with names that start with that string.
		// a model may contain any number of meshes with the same name, so it is possible for one name to match to more than one mesh instance.
		void_c find_meshes( string8_c const & names, core_list_c< mesh_c * > & result );




	//
	// bones.
	//
	public:
		// the bone works in many different coordinate spaces, sometimes they have to be juggled and converted between.
		// +z axis is forward, along axis of bone from head towards tail.
		// +y axis is down.
		// +x axis is right.
		class bone_c
		{
			friend class video_renderer_interface_c;
			friend class scene_component_model_c;
			friend class bone_logic_copy_transform_c;
			friend class bone_logic_copy_animation_c;
			friend class bone_logic_muscle_c;
			friend class bone_logic_jiggle_c;

		private:
			scene_component_model_c * _mother_model; // this bone's model component, which is the component that owns this bone.

			bone_c * _mother_bone; // this bone's mother bone if any.

			resource_file_model_c::bone_c const * _source_bone; // the original source bone data from the model resource that this bone is instanced from.
			resource_file_model_c::bone_extras_c const * _source_bone_extras; // original source bone extras data from the model resource that this bone is instanced from.

			transform3d_c _local_space_transform_from_base_pose_adjustments; // determines the value of _local_space_transform before animations are applied.
			transform3d_c _local_space_transform; // local space transform of bone, relative to the rest transform. this is driven by animations.
			transform3d_c _world_space_transform; // world space transform of bone, relative to scene origin.

			matrix32x4x4_c _object_space_skin_matrix; // object space skin matrix, transforms vertices from their rest pose (object space) to their final (base pose applied, animations applied) pose.

			core_linked_list_c< bone_c * > _daughter_bone_list; // daughter bones.
			core_linked_list_c< bone_c * >::node_c _daughter_bone_list_node; // used with _daughter_bone_list.

			physics_rigid_body_c * _physics_rigid_body;
			physics_shape_c * _physics_shape;
			transform3d_c _physics_body_frame;
			transform3d_c _physics_body_frame_inverse;
			static void_c _physics_copy_world_space_transform_from_game_to_physics( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis );
			static void_c _physics_copy_world_space_transform_from_physics_to_game( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis );

		public:
			bone_c();
			~bone_c();

			void_c update_transforms_recursive(); // builds world space transform from local space transform, recursive.

			void_c build_object_space_skin_matrix_from_world_space_transform();

			transform3d_c const & get_local_space_transform() const;
			void_c set_local_space_transform( transform3d_c const & value );

			transform3d_c const & get_world_space_transform() const;
			void_c set_world_space_transform( transform3d_c const & value );

		};

	private:
		core_list_c< bone_c > _bone_list; // all of the bones in this model.
		core_linked_list_c< bone_c * > _root_bone_list; // just the root bones in this model (pointers to bone instances in _bone_list).
		core_list_c< matrix32x4x4_c * > _bone_skin_matrix_list; // pointers to _object_space_skin_matrix in bone_c instances. these may be bones in this model instance or in any mother model instance (because it is possible for vertices to be weighted to bones that don't exist in their own model). for bones that can't be resolved, they will be set to nullptr.
		void_c _update_bone_skin_matrix_list_recursive(); // is called whenever _model_resource becomes bound/unbound, or when this model instance is made to be a daughter of another model instance (which enables this instance's meshes to be skinned by bones from the mother model).
	public:
		bone_c * find_bone_at_path( string8_c const & path ); // searches hierarchy at path and returns the bone that matches or nullptr.
		bone_c * find_bone_with_name( string8_c const & name, boolean_c search_mother_first ); // searches bones in this model (_bone_list) and returns the first match.

		// names can be a single name, or a semi-colon separated list of names.
		// each name may end with a wild card ("*", asterisk) to indicate that it should match with all bones with names that start with that string.
		// a model can't (or shouldn't) contain more than one bone of a given name because the animation system can only drive one bone per name.
		void_c find_bones( string8_c const & names, core_list_c< bone_c * > & result );

		// used by the animation system to look up the bone instance that an animation should apply to.
		sint32_c find_bone_index_with_name( string8_c const & name );




	//
	// bone logics.
	//
	public:
		// bone logics can be used to dynamically modify the transform of bones in the model.
		// they can be used to drive simulation of muscles, clothes, breasts, hair, etc.
		// they are updated after all the animations are sampled, mixed, and applied, so they are able to read the pose as it was set by the animation and work from that.
		class bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "[none]"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			scene_component_model_c * _mother_model;

			resource_file_model_c::bone_logic_c const * _source_bone_logic; // original bone logic data that this bone logic is instanced from.

			boolean_c _is_ready;

			// is set to true for bone logic types that do simulations over space and time.
			// also note that simulated bone logics are decoupled from the physics engine, meaning that the physics engine does not handle the simulation of simulated bone logics.
			// if false, this causes the bone logic to update when update() is called, after the animations are sampled, mixed, and applied, and before the physics engine simulation updates. this way the bone logic can read the pose that was written by the animations.
			// if true, this causes the bone logic to update when update_simulated_bone_logics() is called, after the physics engine simulation updates, in the case that the physics simulation is driving a rag doll pose or something. this way the bone logic can read the pose that was written by the physics simulation.
			// this is needed so that these bone logics can be updated after the physics engine updates, so that they can respnond to changes that the physics engine makes.
			boolean_c _is_simulated;

			string8_c _this_bone_name;
			bone_c * _this_bone;
			string8_c _that_bone_name;
			bone_c * _that_bone;

		public:
			bone_logic_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start();
			virtual void_c update( float32_c time_step );

		};

		// good for "equipping" things like hair styles onto a character's head, where the hair model is separate from the character model.
		// bones are found across models, such that during bone logic start, if a bone is found in the mother model then it is used.
		class bone_logic_copy_transform_c : public bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "copy_transform"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			bone_logic_copy_transform_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start() override;
			virtual void_c update( float32_c time_step ) override;

		};

		// good for "tween" bones that can help to smooth out mesh deformations (since we don't use dual quaternion skinning).
		// for example with a character's fore arm: the fore arm can be made up of an upper bone, a lower bone, and any number of tween bones.
		// the mesh is skinned to all of the bones; upper, lower, and tweens.
		// the fore arm animations however are authored to rotate only the lower bone.
		// bone_logic_copy_animation_c's on the tween bones "inherit" the twist from the lower fore arm and apply it to the tween bones.
		class bone_logic_copy_animation_c : public bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "copy_animation"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			float32_c position_weight;
			float32_c rotation_weight;
			float32_c scale_weight;

		public:
			bone_logic_copy_animation_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start() override;
			virtual void_c update( float32_c time_step ) override;

		};

		// connects the tail of the bone to another bone.
		// good for simulating muscles or pistons.
		class bone_logic_muscle_c : public bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "muscle"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			float32_c compression_factor; // if 1 then the bone stays the same thickness as it compresses or stretches. if greater than 0 then the bone gets thicker or thinner by this factor when it compresses or stretches. a value of 2 for example means that the bone gets twice as thick when it is compressed to half its natural length, and gets half as thick when the bone is stretched to twice its natural length.

		public:
			bone_logic_muscle_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start() override;
			virtual void_c update( float32_c time_step ) override;

		};

		class bone_logic_jiggle_c : public bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "jiggle"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			class particle_c
			{
			public:
				float64_c tension;
				float64_c damping;
				float64_c limit; // updates runs at a fixed time step (usually 60hz), which isn't fast enough to keep up with big changes. so if the particle strays farther than this distance away from its target, it will be clamped (leashed) to this distance away form its target.
				float64_c gravity;
				vector64x3_c position;
				vector64x3_c velocity;

			public:
				particle_c();
				void_c update( float32_c time_step, vector64x3_c const & position_target, vector64x3_c const & scene_gravity );

			};

			particle_c _head_particle;
			particle_c _tail_particle;

		public:
			bone_logic_jiggle_c( scene_component_model_c * mother_model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start() override;
			virtual void_c update( float32_c time_step ) override;

		};

	private:
		core_list_c< bone_logic_c * > _bone_logic_list;
		bone_logic_c * _instantiate_new_bone_logic( resource_file_model_c::bone_logic_c const * source_bone_logic );
		void_c _restart_bone_logics_recursive(); // restarts all bone logics in this model and all daughter models.




	//
	// attachment points.
	//
	public:
		class attachment_point_c
		{
			friend class scene_component_model_c;

		private:
			resource_file_model_c::attachment_point_c const * _source_attachment_point;

			bone_c * _mother_bone; // this attachment's mother bone if any.

			transform3d_c _local_space_transform;
			transform3d_c _world_space_transform;

		public:
			attachment_point_c();

			transform3d_c const & get_local_space_transform() const;
			transform3d_c const & get_world_space_transform() const;

		};

	private:
		core_list_c< attachment_point_c > _attachment_point_list; // all of the bone attachments are allocated at once in this fixed length list.
	public:
		attachment_point_c * find_attachment_point( string8_c const & name );

		// names can be a single name, or a semi-colon separated list of names.
		// each name may end with a wild card ("*", asterisk) to indicate that it should match with all attachment points with names that start with that string.
		// a model may contain any number of attachment points with the same name, so it is possible for one name to match to more than one attachment point instance.
		void_c find_attachment_points( string8_c const & names, core_list_c< attachment_point_c * > & result );




	//
	// lights.
	//
	public:
		// a light instanced from the bound and loaded model resource object.
		class light_c
		{
			friend class scene_component_model_c;

		private:
			resource_file_model_c::light_c const * _source_light;

			bone_c * _mother_bone;

			transform3d_c _local_space_transform; // transform relative to mother bone if any. otherwise in object space.
			transform3d_c _world_space_transform;

			scene_light_c _scene_light;

		public:
			light_c();

			scene_light_c & get_scene_light() const; // you can modify the scene light reference that is returned.

			bone_c * get_mother_bone() const;

			transform3d_c const & get_local_space_transform() const;

			transform3d_c const & get_world_space_transform() const;

		};

	private:
		core_list_c< light_c > _light_list; // list of light instances in this model. don't resize this list while the scene_light_c instances encapsulated in this list are being referenced by the scene.




	//
	// vertex buffers and skinning.
	//
	public:
		enum vertex_skin_mode_e
		{
			vertex_skin_mode_e_gpu, // supports simple vertex skinning, deforms mesh using bones.
			vertex_skin_mode_e_cpu, // in the future, can support more advanced kinds of skinning.
		};

	private:
		vertex_skin_mode_e _vertex_skin_mode; // preference for how to skin this model. this setting is only observed if the bound model resource has bones.
		core_list_c< video_renderer_vertex_mesh_base_c > _cpu_vertex_buffer_mesh_base_shape_transformed; // if any shape keys are applied, then this buffer is initialized and with the result. shape keys are always applied with the cpu regardless of the _vertex_skin_mode.
		video_vertex_buffer_c * _gpu_vertex_buffer_mesh_base_shape_transformed; // gpu mirror copy of _cpu_vertex_buffer_shape_transformed. is only initialized if _vertex_skin_mode is vertex_skin_mode_e_gpu, since the vertex shader needs a vertex buffer to use as input.
		core_list_c< video_renderer_vertex_mesh_base_c > _cpu_vertex_buffer_mesh_base_shape_and_bone_transformed; // is only used if _vertex_skin_mode is vertex_skin_mode_e_cpu.
		video_vertex_buffer_c * _gpu_vertex_buffer_mesh_base_shape_and_bone_transformed; // if cpu skinning is used, then this is a gpu mirror copy of _cpu_vertex_buffer_shape_and_bone_transformed. if gpu skinning is used, then this is used to hold the stream out result of the vertex skinning vertex shader. if the model has any bones at all then this will always be initialized, otherwise it will be nullptr.
		video_vertex_buffer_c * _get_vertex_buffer_mesh_base_for_rendering() const; // returns _gpu_vertex_buffer_shape_and_bone_transformed if there are any bones, otherwise returns the vertex buffer from the model resource.
	public:
		vertex_skin_mode_e get_vertex_skin_mode() const;
		void_c set_vertex_skin_mode( vertex_skin_mode_e value );




	//
	// physics.
	//
	private:
		physics_rigid_body_c * _physics_rigid_body; // physics body to link with the game object (if any).
		physics_shape_c * _physics_shape; // physics shape for _physics_rigid_body.
		core_list_c< physics_constraint_c * > _physics_constraint_list; // all the physics constraints in this model.
		boolean_c _physics_enabled; // keeps track of if this model instance is set to contribute to the physics simulation scene.
		boolean_c _physics_added_to_simulation; // keeps track of if physics rigid bodies have been added to the scene or not.
		void_c _physics_add_to_simulation(); // adds physics rigid bodies of this model to the physics simulation scene.
		void_c _physics_remove_from_simulation(); // removes physics rigid bodies of this model from the physics simulation scene.
		static void_c _physics_copy_world_space_transform_from_game_to_physics( void_c * object, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis );
		static void_c _physics_copy_world_space_transform_from_physics_to_game( void_c * object, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_basis );
	public:
		boolean_c get_physics_enabled() const;
		void_c set_physics_enabled( boolean_c value );




	//
	// custom object colors.
	//
	private:
		boolean_c _custom_object_colors_enabled; // set to true to tell the renderer to bind object_colors when rendering this model.
		vector32x4_c _custom_object_colors[ video_renderer_interface_c::object_colors_count ]; // list of game defined colors to make available to the pixel shader when rendering this model. these will bind to the object_colors[] shader constants in the pixel shader. the game will also need to provide special pixel shaders to make use of these values.
	public:
		boolean_c get_custom_object_colors_enabled() const;
		void_c set_custom_object_colors_enabled( boolean_c value );
		vector32x4_c const & get_custom_object_color( sint32_c index ) const;
		void_c set_custom_object_color( sint32_c index, vector32x4_c const & value );




	//
	// custom object textures.
	//
	private:
		boolean_c _custom_object_textures_enabled; // set to true to tell the renderer to bind object_textures when rendering this model.
		resource_file_texture_c::reference_c _custom_object_textures[ video_renderer_interface_c::object_textures_count ]; // list of game defined textures to make available to the pixel shader when rendering this model. these will bind to the object_texture_0-3 texture registers in the pixel shader. the game will also need to provide special pixel shaders to make use of these values.
	public:
		boolean_c get_custom_object_textures_enabled() const;
		void_c set_custom_object_textures_enabled( boolean_c value );
		resource_file_texture_c * get_custom_object_texture( sint32_c index ) const;
		void_c set_custom_object_texture( sint32_c index, resource_file_texture_c * value );




	//
	// custom material assignments.
	//
	public:
		// enables the game to define custom override material properties to apply to mesh instances in this model instance.
		// designed for character model customization.
		// it is persistent between model resource file binding/unbinding, so the game can just set them once and forget about them.
		// the intention behind this design was to enable character customization features in the game.
		// for mesh _render_enabled states though, the game will still need to respond to resource file binding/unbinding through on_model_resource_bound/on_model_resource_un_bound to reset _render_enabled states as desired.
		// so materials can be assigned before a model resource is loaded and bound, and then they will be applied when the resource is loaded and bound.
		// material assignments that are placed later in the list may modify material assignments that came before.
		class custom_material_c
		{
			friend class video_renderer_interface_c;

		public:
			struct color_entry_c
			{
				boolean_c enabled;
				vector32x4_c value;
				color_entry_c();
			};

			struct texture_entry_c
			{
				boolean_c enabled;
				resource_file_texture_c::reference_c value;
				texture_entry_c();
			};

		private:
			string8_c _key; // a unique name that the game can use to identify and find this custom material by during run time.

			// a single mesh name or a comma separated list of mesh names that this assignment will apply to.
			// each mesh name may end with a wild card ("*", asterisk) to indicate that it should match with all meshes with names that start with that string.
			string8_c _target_mesh_names;

			color_entry_c _color_entries[ video_renderer_interface_c::material_colors_count ]; // colors to supply as input to the pixel shader. alpha defines transparency.
			texture_entry_c _texture_entries[ video_renderer_interface_c::material_textures_count ]; // textures to supply as input to the pixel shader.

			boolean_c _pixel_shader_enabled;
			video_renderer_pixel_shader_c::reference_c _pixel_shader_value; // pixel shader to put it all together.

			sint32_c _reference_count;

			custom_material_c();

		public:
			static custom_material_c * make_new_instance(); // creates a new instance with a reference count of 0. this is just to ensure that all instances of these are on the heap.

			sint32_c add_reference();
			sint32_c remove_reference();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			string8_c const & get_target_mesh_names() const;
			void_c set_target_mesh_names( string8_c const & value );

			vector32x4_c const & get_color( sint32_c index ) const;
			void_c set_color( sint32_c index, vector32x4_c const & value );
			void_c clear_color( sint32_c index );

			resource_file_texture_c * get_texture( sint32_c index ) const;
			void_c set_texture( sint32_c index, resource_file_texture_c * value );
			void_c clear_texture( sint32_c index );

			void_c set_pixel_shader( video_renderer_pixel_shader_c * value );
			void_c clear_pixel_shader();

		};

	private:
		core_list_c< custom_material_c * > _custom_material_list; // list of game defined material assignments to apply to mesh instances. this state persists even as the model resource is bound/unbound or loaded/unloaded. these assignments are applied when the model resource is bound and its mesh parts are instanced.
	public:
		void_c remove_all_custom_materials();
		custom_material_c * find_custom_material( string8_c const & key );
		custom_material_c * get_custom_material( sint32_c index );
		void_c add_custom_material( custom_material_c * value );
		void_c remove_custom_material( custom_material_c * value );

		// call this after you make any changes to the custom materials list.
		// this also applies custom materials to daughter models.
		// in this way, daughter models (like hair) can have have their custom materials defined by the root model's custom materials.
		// the game only needs to set the custom material on the root model.
		void_c apply_custom_materials();



	//
	// custom rest pose adjustments.
	//
	public:
		class rest_pose_shape_key_c
		{
		private:
			string8_c _shape_key_name;
			float32_c _value; // value between 0 and 1, where 0 is no influence and 1 is full influence. can also be outside of 0 and 1, if you want.

			sint32_c _reference_count;

			rest_pose_shape_key_c();

		public:
			static rest_pose_shape_key_c * make_new_instance();

			sint32_c add_reference();
			sint32_c remove_reference();

			string8_c const & get_shape_key_name();
			void_c set_shape_key_name( string8_c const & value );

			float32_c get_value();
			void_c set_value( float32_c value );

		};

		// enables the game to use an animation in the model file to define tweaks to the rest pose of the model.
		// this lets the game select a specific animation and a specific time in that animation to apply. it is static. it does not play back the animation.
		class rest_pose_animation_c
		{
		private:
			string8_c _animation_name; // the name of the animation to apply to the base pose.
			float32_c _time_percent; // value between 0 and 1, where 0 is the start of the animation and 1 is the end of the animation.

			sint32_c _reference_count;

			rest_pose_animation_c();

		public:
			static rest_pose_animation_c * make_new_instance(); // creates a new instance with a reference count of 0.

			sint32_c add_reference();
			sint32_c remove_reference();

			string8_c const & get_animation_name() const;
			void_c set_animation_name( string8_c const & value );

			float32_c get_time_percent() const;
			void_c set_time_percent( float32_c value );

		};
		// enables the game to apply rest pose adjustments to specific bones in the model.
		class rest_pose_adjustment_c
		{
		private:
			string8_c _bone_names; // name(s) of bones to apply this adjustment to, should be same format as what find_bones() expects.
			vector32x3_c _position; // ( 0, 0, 0 ) means no change.
			quaternion32_c _rotation; // ( 0, 0, 0, 1 ) (identity) means no change.
			vector32x3_c _scale; // ( 1, 1, 1 ) means no change.

			sint32_c _reference_count;

			rest_pose_adjustment_c();

		public:
			static rest_pose_adjustment_c * make_new_instance(); // creates a new instance with a reference count of 0.

			sint32_c add_reference();
			sint32_c remove_reference();

			string8_c const & get_bone_names() const;
			void_c set_bone_names( string8_c const & value );

			vector32x3_c const & get_position() const;
			void_c set_position( vector32x3_c const & value );

			quaternion32_c const & get_rotation() const;
			void_c set_rotation( quaternion32_c const & value );

			vector32x3_c const & get_scale() const;
			void_c set_scale( vector32x3_c const & value );

		};

		core_list_c< rest_pose_shape_key_c * > _rest_pose_shape_key_list;
		core_list_c< rest_pose_animation_c * > _rest_pose_animation_list;
		core_list_c< rest_pose_adjustment_c * > _rest_pose_adjustment_list;

	public:
		void_c remove_all_rest_pose_animations();
		rest_pose_animation_c * get_rest_pose_animation( sint32_c index );
		void_c add_rest_pose_animation( rest_pose_animation_c * value );
		void_c remove_rest_pose_animation( rest_pose_animation_c * value );

		void_c remove_all_rest_pose_adjustments();
		rest_pose_adjustment_c * get_rest_pose_adjustment( sint32_c index );
		void_c add_rest_pose_adjustment( rest_pose_adjustment_c * value );
		void_c remove_rest_pose_adjustment( rest_pose_adjustment_c * value );

		// call this after you make any changes to any rest pose options.
		void_c apply_rest_pose_adjustments();




	//
	// animation player list.
	//
	public:
		// drives animation for a list of animatable objects.
		// the game manages how animations are layered and mixed by adding instances of these in the model instance's animation_player_list.
		class animation_player_c
		{
			friend class scene_component_model_c;

		public:
			// how to blend this animation with the previous state.
			enum blend_type_e
			{
				blend_type_e_mix, // current animation will mix with previous transform state.
				blend_type_e_add, // current animation will add to previous transform state.
			};

			enum time_to_bind_type_e
			{
				time_to_bind_type_e_seconds, // the value of _time_to_bind is in seconds.
				time_to_bind_type_e_percent, // the value of _time_to_bind is a percent and should map to the range of the animation once it is bound.
			};

			// drives animation for an object.
			class object_player_c
			{
			public:
				// drives animation for a property in an object.
				class property_player_c
				{
				public:
					uint32_c property_type;
					//sint32_c property_index; // how this is interpreted depends on object type, but it maps a number to a property, extrapolated by property_type.
					sint32_c key_current; // current key index. will be greater than or equal to zero if this property player is valid.
					sint32_c key_minimum; // lowest possible key index, so we don't sample out of bounds.
					sint32_c key_maximum; // highest possible key index, so we don't sample out of bounds.

				public:
					property_player_c();

				};

				string8_c object_name; // name of object that this player drives.
				sint32_c object_index_for_bone; // if the aniamted object is a bone, then this is the index of the bone that this player drives.
				// etc...

				core_list_c< animation_player_c::object_player_c::property_player_c > property_player_list; // core_list_c of properties driven by this player.
				animation_player_c::object_player_c::property_player_c * property_player_for_position; // position driver.
				animation_player_c::object_player_c::property_player_c * property_player_for_rotation; // rotation driver.
				animation_player_c::object_player_c::property_player_c * property_player_for_scale; // scale driver.
				// etc...

			public:
				object_player_c();

			};

		private:
			scene_component_model_c * _mother_model; // model that is being animated and that contains animation data.

			string8_c _animation_name; // name of animation within _model_component that this animation player should play.

			resource_file_model_c const * _source_model; // model resource instance that _animation is from.
			resource_file_model_c::animation_c const * _source_animation; // animation instance in _animation_model.

			core_list_c< animation_player_c::object_player_c > _object_player_list; // animation players for each animated object in _animation.

			blend_type_e _blend_type; // how to blend this animation with previous animations in the list.
			float32_c _blend_weight; // weight to blend this animation with previous animations.

			sint32_c _loop_count_current; // keeps track of how many times animation has looped.
			sint32_c _loop_count_target; // keeps track of how many times animation wants to loop before it stops playing, or 0 to loop forever.

			float32_c _speed; // speed at which to play back this animation.

			float32_c _time; // current play time of current animation.
			time_to_bind_type_e _time_to_bind_type;
			float32_c _time_to_bind; // if the user tries to get or set the play time of this player before an animation is bound to it then we have to remember that value so we can put it in effect once the animation does get bound.
			//animation_player_c * _time_from_other; // if this is set then the time of this animation player will be taken from the other animation player.

			// evaluates _model and _animation_name and resolves _source_model and _source_animation.
			// this method is subscribed to the on_load and on_unload events of the bound _model_resource and _animations_resource.
			void_c _handle_on_animation_resources_updated( scene_component_model_c * model );

			sint32_c _reference_count;

			animation_player_c( scene_component_model_c * mother_model ); // provide the model instance that this animation player will belong to. you may add and remove this animation player from this model instance's animation player list only. this animation player needs to subscribe to events on that model instance so that it continues to work between model/animation resource file unload/load.

		public:			
			~animation_player_c();

			void_c add_reference();
			void_c remove_reference();

			// updates this animation player.
			void_c update( float32_c time_step );

			// plays the animation loop_count number of times.
			// call with loop_count set to 0 to loop forever.
			// if you want to pause play back, call set_speed( 0.0f ).
			void_c play( sint32_c loop_count );

			void_c set_animation( string8_c const & value );
			void_c clear_animation();

			blend_type_e get_blend_type(); // gets blend mode that determines how this animation player blends with existing pose.
			void_c set_blend_type( blend_type_e value ); // sets blend mode that determines how this animation player blends with existing pose.

			float32_c get_blend_weight(); // gets influence of this animation player on previous animation players in the animation player list.
			void_c set_blend_weight( float32_c value ); // sets influence of this animation player on previous animation players in the animation player list.
			void_c traverse_blend_weight( float32_c value, float32_c amount );

			float32_c get_animation_move_speed(); // returns the move_speed from the bound animation, if any.

			float32_c get_speed(); // returns speed that animation is playing back at.
			void_c set_speed( float32_c value ); // sets speed that animation will play back at.

			float32_c get_time(); // gets the current animation time.
			void_c set_time( float32_c value ); // sets the current animation time.

			float32_c get_time_percent(); // gets the current animation time in range 0 to 1.
			void_c set_time_percent( float32_c value ); // sets the current animation time in range 0 to 1.

			//void_c set_to_use_time_from_other( animation_player_c * other ); // this sets this animation player to call set_time( other->get_time() ) during each update(). this is useful for creating gait (like walk and run) animations that stay in phase so that they blend between each other seamlessly.

			void_c sample_property_player( animation_player_c::object_player_c::property_player_c * property_player, vector32x4_c & result ); // samples current value from an arbitrary property player.
			boolean_c sample_position( sint32_c object_player_index, vector32x3_c & result ); // samples current position from this animation. returns false if the animation does not animate the position property.
			boolean_c sample_rotation( sint32_c object_player_index, quaternion32_c & result ); // samples current rotation from this animation. returns false if the animation does not animate the rotation property.
			boolean_c sample_scale( sint32_c object_player_index, vector32x3_c & result ); // samples current scale from this animation. returns false if the animation does not animate the scale property.

		};

	private:
		core_list_c< animation_player_c * > _animation_player_list;
	public:
		// creates a new animation player instance that is linked with this model instance, and adds it to the end of the animation player list.
		// it will have a reference count of 1, held by this model instance.
		// typically, the game will also hold a reference to the animation player so that it can control the animations on this model instance.
		// if the model instance is deleted and the game still has references to the animation player instances, then they will stay alive but they will be useless.
		animation_player_c * add_animation_player();
		void_c remove_animation_player( animation_player_c * animation_player ); // deletes a previously created model player instance that is linked with this model instance.
		void_c remove_all_animation_players();




	//
	// miscellaneous options.
	//
	private:
		transform3d_c _world_space_transform_inverse;
		float32_c _opacity;
		boolean_c _render_enabled;
		boolean_c _shadow_cast_enabled;
		boolean_c _light_cast_enabled;
		boolean_c _automatic_light_probe_invalidation_enabled; // if true, then when the model's relevant properties change, then all affected light probes will be invalidated. should be set to true on static geometry. should be set to false on characters.
		boolean_c _local_light_probe_enabled;// optional, if set then this model will create and use its own light probe instead of doing a light probe lookup. the light probe will then be available to other models that do do a light probe lookup.
		scene_component_light_probe_c * _local_light_probe_component; // will be instantiated if set_local_light_probe_enable( true ) is called, then the renderer will use this light probe to light the model, otherwise the renderer will do a light probe look up.
		scene_component_light_probe_c * _fixed_light_probe_component; // optional, if set then the renderer will use this light probe to light the model instead of using _local_light_probe_component or doing a light probe lookup. has higher priority than _local_light_probe.
	public:
		float32_c get_opacity() const;
		void_c set_opacity( float32_c value );
		boolean_c get_render_enabled() const;
		void_c set_render_enabled( boolean_c value );
		boolean_c get_shadow_cast_enabled() const;
		void_c set_shadow_cast_enabled( boolean_c value );
		boolean_c get_light_cast_enabled() const;
		void_c set_light_cast_enabled( boolean_c value );
		boolean_c get_automatic_light_probe_invalidation_enabled() const;
		void_c set_automatic_light_probe_invalidation_enabled( boolean_c value );
		boolean_c get_local_light_probe_enabled() const;
		void_c set_local_light_probe_enabled( boolean_c value );




	//
	// for internal use by renderer.
	//
	private:
		sint32_c _render_light_indices_count; // used by renderer to remember how many lights in _render_proxy_light_indices are set, the shader will use this in a for loop to accumulate lighting for just this many lights.
		sint32_c _render_light_indices[ video_renderer_interface_c::object_lights_count ]; // used by renderer to associate light instances with this model, these are the lights that the shader will use to contribute to lighting for this model.
		uint32_c _render_frame_last; // used by the renderer to remember the last frame number that this model was skinned, so that it is not skinned more than once per render frame.
		float32_c _render_lod_scale; // used by the renderer to determine which lod level to use when rendering the model for a frame. this is calculated as a function of the primary view's field of view and the model's distance to the primary view.




	//
	// normal scene component stuff.
	//
	private:
		void_c _update_world_transforms(); // builds world space transforms of all bones, and attachments, and lights, based on their local space transforms and inheritance.
		virtual void_c _handle_before_removed_from_scene(); // is called when the scene object is added to a scene. the scene may or may not already be associated with the engine's user interface at this point.
		virtual void_c _handle_after_added_to_scene(); // is called when the scene object is removed from a scene. the scene may or may not already be associated with the engine's user interface at this point.
		virtual void_c _handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator ) override;

	public:
		scene_component_model_c();
		virtual ~scene_component_model_c() override;

		// samples motion state of non-kinematic dynamic rigid bodies.
		// updates animations.
		// updates bone logics.
		// updates local space bounding box.
		// updates (sets) transform of kinematic rigid bodies.
		// recurses to all daughter models.
		void_c update_animations( float32_c time_step );

		void_c invalidate_light_probes(); // invalidates all light probes that are in range of this model instance.

		void_c reseat(); // call after large transformational changes (like teleportation) to reset simulated bone logics so that they don't spring around and look dumb or break.

		core_event_c< void_c, scene_component_model_c * > on_model_resource_bound; // users may subscribe to this to be notified when the model resource or animation resource becomes bound.
		core_event_c< void_c, scene_component_model_c * > on_model_resource_unbound; // users may subscribe to this to be notified when the model resource or animation resource is about to become unbound.

	};

}
