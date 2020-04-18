#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_resource_file_model.h"
#include "cheonsa_resource_file_material_map.h"
#include "cheonsa_video_renderer_interface.h"

namespace cheonsa
{

	// to use, call set_model_resource() with a pointer to a model resource.
	// you may also call set_animations_resource() with a pointer to an additional model resource that contains additional animations to use.
	// this is a way to reuse animations between multiple models.
	// vertex skinning is done in object (model) space.
	class scene_component_model_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_model; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	public:
		enum vertex_skin_mode_e
		{
			vertex_skin_mode_e_gpu, // supports simple vertex skinning, deforms mesh using bones.
			vertex_skin_mode_e_cpu, // supports more complex vertex skinning, deforms mesh using bones, and potentially also using per-vertex physics (soft body).
		};

		// used with animation dictionary, the key is the animation name, the value is this.
		class animation_map_c
		{
		public:
			resource_file_model_c const * source_model;
			resource_file_model_c::animation_c const * source_animation;

		public:
			animation_map_c();

		};

		// drives animation for a list of animatable objects.
		// the game manages how animations are layered and mixed by adding instances of these in the model instance's animation_player_list.
		class animation_player_c
		{
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

			scene_component_model_c * _model; // model that is being animated and that contains animation data.

			string8_c _animation_name; // name of animation within _model_component that this animation player should play.

			resource_file_model_c const * _source_model; // model resource instance that _animation is from.
			resource_file_model_c::animation_c const * _source_animation; // animation instance in _animation_model.

			core_list_c< animation_player_c::object_player_c > _object_player_list; // animation players for each animated object in _animation.

			blend_type_e _blend_type; // how to blend this animation with previous animations in the stack.
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

		public:
			animation_player_c( scene_component_model_c * model );
			~animation_player_c();

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

			float32_c get_blend_weight(); // gets influence of this animation player on previous animation players in the animation player stack.
			void_c set_blend_weight( float32_c value ); // sets influence of this animation player on previous animation players in the animation player stack.
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

		// defines a material to assign to an object in this model.
		// it is persistent, so its state remains the same between model resource object bindings.
		// so materials can be assigned before a model resource is loaded and bound, and then they will be applied when the resource is loaded and bound.
		class material_assignment_c
		{
		private:
			friend class video_renderer_interface_c;
			friend class scene_component_model_c;

			scene_material_c _material; // the material.
			resource_file_material_map_c * _source_model_materials; // if set, then this is the model material resource that created this material assignment. this lets us remove material assignments when the materials resource is unloaded or unbound.
			string8_c _target_mesh_name; // the name of the meshes within this model that this material will be applied to.

		public:
			material_assignment_c( string8_c const & target_mesh_name );

			scene_material_c & get_material(); // you can edit the material during run time.

		};

		class bone_c;

		// a light instanced from the bound and loaded model resource object.
		class light_c
		{
		private:
			friend class scene_component_model_c;

			//boolean_c render_enable;
			resource_file_model_c::light_c const * _source_light;

			bone_c * _mother_bone;

			space_transform_c _local_space_transform; // transform relative to mother bone if any. otherwise in object space.
			space_transform_c _object_space_transform; // transform in object space, after inheriting from mother bone if any.
			//space_transform_c world_space_transform; // transform in world space.

			scene_light_c _scene_light;

		public:
			light_c();

			scene_light_c & get_scene_light() const; // you can modify the scene light reference that is returned.

			bone_c * get_mother_bone() const;

			space_transform_c const & get_local_space_transform() const;

			space_transform_c const & get_object_space_transform() const;

		};

		// a mesh instanced from the bound and loaded model resource object.
		class mesh_c
		{
		private:
			friend class scene_component_model_c;
			friend class video_renderer_interface_c;

			boolean_c _render_enable; // set to true to enable this mesh instance to be rendered.
			bone_c * _mother_bone; // when set, then this unskinned vertices in this mesh will inherit from the world space transform of this bone instance, otherwise the unskinned vertices in this mesh will inherit from the world space transform of the model instance.
			resource_file_model_c::mesh_c const * _source_mesh; // a pointer to the original mesh in the model resource that this mesh is an instance of.
			material_assignment_c * _material_assignment; // if set then this defines the material that is assigned to this mesh.

		public:
			mesh_c();

			boolean_c get_render_enable() const;
			void_c set_render_enable( boolean_c value );

			bone_c * get_mother_bone() const;

			resource_file_model_c::mesh_c const * get_source_mesh() const;

			material_assignment_c * get_material_assignment() const;

		};

		// bone logics can be used to dynamically modify the transform of bones in the model.
		// they can be used to drive simulation of muscles, clothes, breasts, hair, etc.
		// they are updated after all the animations are sampled, mixed, and applied, so they are able to read the pose as it was set by the animation and work from that.
		class bone_logic_c
		{
		public:
			static char8_c const * get_type_static() { return "[none]"; }
			virtual char8_c const * get_type() const { return get_type_static(); }

		public:
			scene_component_model_c * _model;

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
			bone_logic_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic );

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
			bone_logic_copy_transform_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic );

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
			bone_logic_copy_animation_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic );

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
			bone_logic_muscle_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic );

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
				void_c update( float32_c time_step, vector64x3_c const & position_target, vector64x3_c const & model_velocity, vector64x3_c const & scene_gravity );

			};

			particle_c _head_particle;
			particle_c _tail_particle;

		public:
			bone_logic_jiggle_c( scene_component_model_c * model, resource_file_model_c::bone_logic_c const * source_bone_logic );

			virtual void_c start() override;
			virtual void_c update( float32_c time_step ) override;

		};

		// the bone works in many different coordinate spaces, sometimes they have to be juggled and converted between.
		class bone_c
		{
		private:
			friend class video_renderer_interface_c;
			friend class scene_component_model_c;
			friend class bone_logic_copy_transform_c;
			friend class bone_logic_copy_animation_c;
			friend class bone_logic_muscle_c;
			friend class bone_logic_jiggle_c;

			scene_component_model_c * _model; // this bone's model component, which is the component that owns this bone.

			resource_file_model_c::bone_c const * _source_bone; // the original source bone data from the model resource that this bone is instanced from.
			resource_file_model_c::bone_extras_c const * _source_bone_extras; // original source bone extras data from the model resource that this bone is instanced from.

			bone_c * _mother_bone; // this bone's mother bone if any.

			space_transform_c _local_space_transform; // local space transform of bone, relative to the rest transform. this is driven by animations.
			space_transform_c _object_space_transform; // object space transform of bone, relative to scene object. this is constructed from local_space_transform and mother bone, and/or it is driven by bone logics. this is used to build object_space_skin_matrix.
			space_transform_c _world_space_transform; // world space transform of bone, relative to scene origin. this isn't used in most cases, except by bone logics that prefer to do physics simulations in world space, in which case it does the simulation here then converts it to object_space_transform.

			matrix32x4x4_c _object_space_skin_matrix; // object space skin matrix, transform vertices from their rest pose (object space) to their post pose (object space).

			core_linked_list_c< bone_c * > _daughter_bone_list; // daughter bones.
			core_linked_list_c< bone_c * >::node_c _daughter_bone_list_node; // used with _daughter_bone_list.

		public:
			bone_c();

			void_c build_object_space_transform_from_local_space_transform();
			void_c build_world_space_transform_from_object_space_transform( space_transform_c const & scene_object_world_space_transform );
			void_c build_object_space_transform_from_world_space_transform( space_transform_c const & scene_object_world_space_transform_inverted );
			void_c build_object_space_skin_matrix_from_object_space_transform();

			void_c update_hierarchy(); // builds object_space_transform from local_space_transform, builds object_space_skin_matrix from object_space_transform.
			//void_c update_hierarchy_build_object_space_transform_from_local_space_transform();
			//void_c update_hierarchy_build_object_space_skin_matrix_from_object_space_transform();

			space_transform_c const & get_local_space_transform() const;
			space_transform_c const & get_object_space_transform() const;
			space_transform_c const & get_world_space_transform() const;
			space_transform_c & get_world_space_transform();

		};

		class bone_attachment_c
		{
		private:
			friend class scene_component_model_c;

			//scene_component_model_c * _model; // this bone's model component, which is the component that owns this attachment.

			resource_file_model_c::bone_attachment_c const * _source_bone_attachment;

			bone_c * _mother_bone; // this attachment's mother bone if any.

			space_transform_c _local_space_transform;
			space_transform_c _object_space_transform;
			space_transform_c _world_space_transform;

		public:
			bone_attachment_c();

			space_transform_c const & get_local_space_transform() const;
			space_transform_c const & get_object_space_transform() const;
			space_transform_c const & get_world_space_transform() const;

		};

	protected:
		friend class video_renderer_interface_c;

		boolean_c _model_resource_is_bound; // will be true if a model resource is loaded and bound (instanced) and this model component is ready to be rendered.
		resource_file_model_c::reference_c _model_resource; // the model resource to load and bind, or that is already bound.
		void_c _handle_model_resource_on_load( resource_file_c * resource ); // responds to when _model_resource is loaded.
		void_c _handle_model_resource_on_un_load( resource_file_c * resource ); // responds to when _model_resource is un loaded.

		core_list_c< resource_file_model_c::reference_c > _animations_resource_list; // optional references other model resources that contain additional animations which can then be referenced by name.
		void_c _handle_animations_resource_on_load( resource_file_c * resource ); // responds to when _animations_resource is loaded.
		void_c _handle_animations_resource_on_un_load( resource_file_c * resource ); // responds to when _animations_resource is un loaded.

		resource_file_material_map_c::reference_c _material_map_resource;
		void_c _handle_material_map_resource_on_load( resource_file_c * resource ); // responds to when _model_materials_resource is loaded.
		void_c _handle_material_map_resource_on_unload( resource_file_c * resource ); // responds to when _model_materials_resource is un loaded.

		vector64x3_c _object_velocity; // the difference of the current position with the position from the last frame, divided by time between frames.
		vector64x3_c _object_position_last;
		float32_c _opacity;
		boolean_c _render_enable;
		boolean_c _shadow_cast_enable;
		boolean_c _light_cast_enable;
		boolean_c _automatic_light_probe_invalidation_enable; // if true, then when the model's relevant properties change, then all affected light probes will be invalidated. should be set to true on static geometry. should be set to false on characters.
		boolean_c _local_light_probe_enable;// optional, if set then this model will create and use its own light probe instead of doing a light probe lookup. the light probe will then be available to other models that do do a light probe lookup.
		scene_component_light_probe_c * _local_light_probe_component; // will be instantiated if set_local_light_probe_enable( true ) is called, then the renderer will use this light probe to light the model, otherwise the renderer will do a light probe look up.
		scene_component_light_probe_c * _fixed_light_probe_component; // optional, if set then the renderer will use this light probe to light the model instead of using _local_light_probe_component or doing a light probe lookup. has higher priority than _local_light_probe.
		sint32_c _render_light_indices_count; // used by renderer to define how many lights in _render_proxy_light_indices are set, the shader will use this to calculate lighting for just this many lights.
		sint32_c _render_light_indices[ video_renderer_interface_c::object_lights_count ]; // used by renderer to associate light instances with this model, these are the lights that the shader will use to contribute to lighting for this model.
		uint32_c _render_frame_last; // used by the renderer to track the last frame number that this model was last skinned, so that it is not skinned more than once per render frame.
		uint8_c _lod_slice; // used by the renderer to track which lod slice and thus lod level to use when rendering the model for a given frame.

		core_list_c< light_c > _light_list; // list of light instances in this model. don't resize this list while the scene_light_c instances encapsulated in this list are being referenced by the scene.

		core_list_c< mesh_c > _mesh_list; // list of mesh instances in this model.

		vertex_skin_mode_e _vertex_skin_mode; // how to skin this model. models are only able to be skinned if _bone_skin_matrix_list.get_length() > 0 which indicates that the model is designed to be skinned.
		core_list_c< video_renderer_vertex_mesh_base_c > _mesh_vertex_list_cpu_skinned; // stores result of cpu skinned vertices, before they are [copied|uploaded] to _mesh_vertex_buffer_skinned.
		video_vertex_buffer_c * _mesh_vertex_buffer_skinned; // holds gpu or cpu skinned vertices. this is initially nullptr, then it is initialized by the renderer when the model is about to be rendered. this is deleted and set to nullptr if the _vertex_skin_mode is changed during run time.
		video_vertex_buffer_c * _get_vertex_buffer_for_rendering() const; // returns _mesh_vertex_buffer_skinned if _bone_skin_matrix_list.get_length() > 0, otherwise returns the vertex buffer from the model resource.

		void_c _gather_meshes_with_name( string8_c const & name, core_list_c< mesh_c * > & result ); // finds all meshes in this model that match name, which may optionally end in a wild card character '*' to match against meshes that start with name.

		bone_logic_c * _instantiate_new_bone_logic( resource_file_model_c::bone_logic_c const * source_bone_logic );

		core_list_c< bone_c > _bone_list; // all of the bones in this model.
		core_linked_list_c< bone_c * > _root_bone_list; // just the root bones in this model (pointers to bone instances in _bone_list).

		core_list_c< matrix32x4x4_c * > _bone_skin_matrix_list; // all of the bone skin matrices, to transform vertices from rest-pose (object space) to post-pose (object space). this has as many entries as _model_resource->_header->mesh_bone_name_count. for bones that can't be resolved, they will be set to nullptr.
		void_c _update_bone_skin_matrix_list_hierarchy();
		void_c _update_bone_skin_matrix_list(); // is called whenever _model_resource becomes bound/unbound, or when this model instance is made to be a daughter of another model instance (which enables this instance's meshes to be skinned by bones from the mother model).
		core_list_c< bone_logic_c * > _bone_logic_list;
		void_c _restart_bone_logics(); // restarts all bone logics in this model and all daughter models.

		core_dictionary_c< string8_c, animation_map_c > _animation_map_dictionary; // maps an animation name to an animation sourced from this model resource or from any of the model resources in _animations_resource_list.
		core_event_c< void_c, scene_component_model_c * > _on_animation_resources_updated;

		boolean_c _physics_enable; // keeps track of if this model instance is set to contribute to the physics simulation scene.
		//scene_component_body_c * _body_component; // if set, then this is the body component for the object. this would be different from body components that might be on bone objects.
		//core_list_c< physics_constraint_c * > _physics_constraint_list; // all of the physics constraints that are a part of this model that are used to constrain various bodies that are also within this model.
		//boolean_c _physics_added_to_scene;
		//void_c _add_physics_to_scene();
		//void_c _remove_physics_from_scene();

		scene_component_model_c * _mother_model; // mother model component, of which this model might be influenced by.
		core_linked_list_c< scene_component_model_c * > _daughter_model_list; // daughter model components of this model component.
		core_linked_list_c< scene_component_model_c * >::node_c _daughter_model_list_node;

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		virtual void_c _handle_before_property_modified( scene_object_property_e property );
		virtual void_c _handle_after_property_modified( scene_object_property_e property );

		// todo: implement cpu skinning to enable advanced features for flesh like skin, bones, muscles, fat, and normals recalculation.
		// perhaps later on this can be translated to run on gpu, but i don't see much reason for doing that right now.
		// void_c _process_vertices_for_flesh();

	public:
		scene_component_model_c();
		virtual ~scene_component_model_c() override;

		void_c update_animations( float32_c time_step ); // updates and applies animations, then updates and applies non-simulated bone logics.
		void_c update_simulated_bone_logics( float32_c time_step ); // updates simulated bone logics.

		void_c invalidate_light_probes(); // invalidates all light probes that are in range of this model instance.

		void_c reseat(); // call after large transformational changes (like teleportation) to reset simulated bone logics so that they don't spring around and look dumb or break.
		void_c reseat_bones(); // mainly for internal (re)use, you shouldn't need to call this.
		void_c reseat_bone_attachments_and_lights(); // mainly for internal (re)use, you shouldn't need to call this. updates world space transform of bone attachments and lights.

		scene_component_model_c * get_root_model();

		resource_file_model_c * get_model_resource(); // will return the set model resource.
		void_c set_model_resource( resource_file_model_c * resource ); // sets the model resource to bind to this model instance. it will bind as soon as it is able to, since a pointer to a yet-to-be-loaded model resource instance may be suppled.
		boolean_c get_model_resource_is_bound(); // returns true only if the model resource is set, loaded, and bound.

		void_c add_animations_resource( resource_file_model_c * resource ); // additional model resources can be referenced which contain animations that we want to play on this model.
		void_c remove_animations_resource( resource_file_model_c * resource );
		sint32_c get_animations_resource_count();
		resource_file_model_c * get_animations_resource_at_index( sint32_c index );
		void_c remove_all_animations_resources();
		void_c refresh_animation_map_dictionary(); // is called whenever _model_resource becomes bound or unbound. but it should also be called when one or more animations resources are added at once.

		resource_file_material_map_c * get_material_map_resource();
		void_c set_material_map_resource( resource_file_material_map_c * value ); // sets the model materials assignments. this works in addition to the material_assignment_list, which has higher priority.

		vertex_skin_mode_e get_vertex_skin_mode() const;
		void_c set_vertex_skin_mode( vertex_skin_mode_e value );

		float32_c get_opacity() const;
		void_c set_opacity( float32_c value );

		boolean_c get_render_enable() const;
		void_c set_render_enable( boolean_c value );

		boolean_c get_shadow_cast_enable() const;
		void_c set_shadow_cast_enable( boolean_c value );

		boolean_c get_light_cast_enable() const;
		void_c set_light_cast_enable( boolean_c value );

		boolean_c get_automatic_light_probe_invalidation_enable() const;
		void_c set_automatic_light_probe_invalidation_enable( boolean_c value );

		boolean_c get_local_light_probe_enable() const;
		void_c set_local_light_probe_enable( boolean_c value );

		boolean_c get_physics_enable() const;
		void_c set_physics_enable( boolean_c value );

		scene_component_model_c * get_mother_model();
		void_c set_mother_model( scene_component_model_c * model );

		core_list_c< animation_player_c * > animation_player_list;

		boolean_c object_colors_enable;
		vector32x4_c object_colors[ video_renderer_interface_c::object_colors_count ];
		boolean_c object_textures_enable;
		resource_file_texture_c::reference_c object_textures[ video_renderer_interface_c::object_textures_count ];

		core_list_c< material_assignment_c * > material_assignment_list; // list of game defined material assignments to apply to mesh instances. this is not tied to instantiation, therefore it persists even as the model resource is bound/unbound or loaded/unloaded. these assignments are applied when the model resource is bound and parts are instanced.
		void_c unapply_material_assignments(); // call this before modifying material_assignment_list.
		void_c apply_material_assignments(); // call this after modifying material_assignment_list.

		bone_c * find_bone_at_path( string8_c const & path ); // searches hierarchy at path and returns the bone that matches or nullptr.
		bone_c * find_bone_with_name( string8_c const & name, boolean_c search_mother_first ); // searches bones in this model (_bone_list) and returns the first match.
		void_c find_bones_with_name( string8_c const & name, core_list_c< bone_c * > & result ); // name can end with a wild card character '*'. this will match all bones with names that start with name. appends bones to result. note that all bones in the model still need to be uniquely named.
		sint32_c find_bone_index_with_name( string8_c const & name );
		core_list_c< bone_attachment_c > _bone_attachment_list; // all of the bone attachments are allocated at once in this fixed length list.
		bone_attachment_c * find_bone_attachment_with_name( string8_c const & name );

	public:
		core_event_c< void_c, scene_component_model_c * > on_model_resource_bound; // users may subscribe to this to be notified when the model resource or animation resource becomes bound.
		core_event_c< void_c, scene_component_model_c * > on_model_resource_un_bound; // users may subscribe to this to be notified when the model resource or animation resource is about to become unbound.

	};

}
