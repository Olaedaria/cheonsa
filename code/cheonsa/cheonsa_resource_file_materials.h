#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_scene_types.h"

namespace cheonsa
{

	// loads ".materials.xml" or ".model.materials.xml" files.
	// (file extension is a suggestion, you may deviate from it).
	// defines what materials to apply to what things in an object.
	class resource_file_materials_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_materials_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "materials"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

	public:
		// a thing can be a mesh of a model.
		struct thing_c
		{
			friend class resource_file_materials_c;

		private:
			// a single name or a semi-colon (';') separated list of names of things to apply the material to.
			// each name may also end with an asterisk ('*') to indicate wild card, that it should match with all things that start with that string.
			string8_c _target_names;

			// the material to apply to the thing.
			scene_material_c _material;

		public:
			thing_c();

			string8_c const & get_target_names() const;

			scene_material_c const & get_material() const;

		};

	private:
		core_list_c< thing_c * > _thing_list; // a collection of objects to apply different materials to.

	public:
		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_materials_c( string16_c const & file_path );

		core_list_c< thing_c * > const & get_thing_list() const;

	};

}
