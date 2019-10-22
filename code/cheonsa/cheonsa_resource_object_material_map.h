#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_scene_types.h"

namespace cheonsa
{

	// maps renderer materials to things in an object.
	class resource_object_material_map_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_material_map_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "model_materials"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		// a thing can be a mesh of a model.
		struct thing_c
		{
		public:
			string8_c target_name; // the name of the object to apply the material to. the object may contain more than one instance of a thing with this name, the material will be applied to all of them.
			scene_material_c material; // the material to apply to the object.
		};

		core_list_c< thing_c * > things; // a collection of objects to apply different materials to.

	public:
		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_object_material_map_c();
		resource_object_material_map_c( resource_object_material_map_c const & ) = delete;
		virtual ~resource_object_material_map_c() override;
		resource_object_material_map_c & operator = ( resource_object_material_map_c const & ) = delete;

	};

}
