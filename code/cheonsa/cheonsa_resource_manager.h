#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa_resource_file_materials.h"
#include "cheonsa_resource_file_menu_layout.h"
#include "cheonsa_resource_file_model.h"
#include "cheonsa_resource_file_sprites.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_platform_thread.h"
#include "cheonsa_platform_critical_section.h"

namespace cheonsa
{

	// tracks, loads, and unloads resources that are used by the game.
	// automatically scans source files of loaded resources and reloads them when modifications are detected.
	// not yet implemented: automatic unloading and deletion of resources with 0 reference count.
	class resource_manager_c
	{
	private:
		core_list_c< resource_file_c * > _resource_list; // all of the resources currently instantiated.

		core_list_c< resource_file_c * > _load_queue; // load queue on the main thread side. this main thread buffer will be copied to the worker thread buffer continuously, then cleared.

		platform_thread_c _worker_thread;
		platform_critical_section_c _worker_thread_critical_section;
		core_list_c< resource_file_c * > _worker_thread_load_queue;
		static uint32_c _worker_thread_function( platform_thread_c * platform_thread );

		template< typename resource_file_type_c >
		resource_file_type_c * _load( string16_c const & file_path, boolean_c load_now );

		sint32_c _cpu_memory_usage; // tracks roughly how much cpu RAM is used by currently loaded resources.
		sint32_c _gpu_memory_usage; // tracks roughly how much gpu RAM is used by currently loaded resources.

		boolean_c _scan_enable; // if true then the resource manager will continuously scan for source file modifications in the background.
		sint32_c _scan_index; // tracks which resource file the scanner is currently on.

	public:
		resource_manager_c(); // spins up the background resource loading thread.
		~resource_manager_c(); // stops the background resource loading thread.

		boolean_c start();

		// scans all resource source files for modifications, and reloads them if any changes are detected.
		void_c refresh();

		resource_file_font_c * load_font( string16_c const & relative_file_path, boolean_c load_now = true );
		resource_file_materials_c * load_materials( string16_c const & relative_file_path, boolean_c load_now = true );
		resource_file_menu_layout_c * load_menu_layout( string16_c const & relative_file_path, boolean_c load_now = true );
		resource_file_model_c * load_model( string16_c const & relative_file_path, boolean_c load_now = true );
		resource_file_sprites_c * load_sprites( string16_c const & relative_file_path, boolean_c load_now = true );
		resource_file_texture_c * load_texture( string16_c const & relative_file_path, boolean_c load_now = true );

	};

}
