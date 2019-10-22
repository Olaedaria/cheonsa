#pragma once

#include "cheonsa_script_types.h"
#include "cheonsa_script_scope.h"

namespace cheonsa
{

	// the script context manages executing script instances.
	// this allows a way for us to suspend, save, load, and resume executing script states between play time sessions.
	// this will hopefully allow us to save and load scenes while scripts are still running within them.
	class script_context_c
	{
	private:
		friend class script_instance_c;

		core_list_c< script_instance_c * > _scripts; // all of the scripts that are executing.
		sint32_c _next_script_serial_index;
		void_c _compact_script_serial_indices();

	public:
		script_context_c();
		~script_context_c();

		script_instance_c * get_script( sint32_c serial_index ); // used by scene objects to re-aquire serialized script instance during _ResolvePostLoadLinks.
		script_instance_c * new_script( string16_c const & file_path ); // creates a new script within this context and returns it.
		void_c delete_script( script_instance_c * script ); // deletes a script within this context.

		void_c reset();
		boolean_c save( data_scribe_binary_property_c & scribe );
		boolean_c load( data_scribe_binary_property_c & scribe );

	};

}
