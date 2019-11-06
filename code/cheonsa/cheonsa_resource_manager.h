#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa_resource_file_material_map.h"
#include "cheonsa_resource_file_menu_layout.h"
#include "cheonsa_resource_file_model.h"
#include "cheonsa_resource_file_sprite_set.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_platform_thread.h"
#include "cheonsa_platform_critical_section.h"

namespace cheonsa
{

	// tracks, loads, and unloads resources that are used by the game.
	// automatically scans source files of loaded resources and reloads them when modifications are detected.
	class resource_manager_c
	{
	private:
		core_list_c< resource_file_c * > _resource_list; // all of the resources currently instantiated.

		core_list_c< resource_file_c * > _load_queue; // main thread buffer of resources to add to the worker thread copy.

		platform_thread_c _worker_thread;
		platform_critical_section_c _worker_thread_critical_section;
		core_list_c< resource_file_c * > _worker_thread_load_queue;
		static uint32_c _worker_thread_function( platform_thread_c * platform_thread );

		template< typename resource_type >
		resource_type * _load( string16_c const & file_path, boolean_c load_now );

		sint32_c _cpu_memory_usage; // tracks roughly how much cpu RAM is used by currently loaded resources.
		sint32_c _gpu_memory_usage; // tracks roughly how much gpu RAM is used by currently loaded resources.

		boolean_c _scan_enable; // if true then the resource manager will continuously scan for file modifications in the background.
		sint32_c _scan_index; // tracks which resource the scanner is currently on.
		
		void_c _refresh_resource( resource_file_c * resource ); // used by the scanner to check if a source file of a given resource was modified, and if so then it reloads the resource, otherwise it doesn't do anything.

		// called by async load thread, is here for friend access benefit (since the async load thread does not have access).
		// also called by templated _load method, since friend benefits don't apply in there.
		static void_c _load_internal( resource_file_c * resource, data_stream_c * stream, string16_c const & file_path_absolute, boolean_c for_async_load_thread );

	public:
		resource_manager_c(); // spins up the background resource loading thread.
		~resource_manager_c(); // stops the background resource loading thread.

		boolean_c start();

		// scans a few resource source files for modifications, and reloads those resources from their source files if needed, and deletes resources that are no longer referenced.
		// we may move this to another thread later on.
		void_c update();

		// scans all resource source files for modifications, and reloads those resources from their source files if needed.
		void_c refresh();

		resource_file_font_c * load_font( string16_c const & file_path_relative, boolean_c load_now = true );
		resource_file_material_map_c * load_material_map( string16_c const & file_path_relative, boolean_c load_now = true );
		resource_file_menu_layout_c * load_menu_layout( string16_c const & file_path_relative, boolean_c load_now = true );
		resource_file_model_c * load_model( string16_c const & file_path_relative, boolean_c load_now = true );
		resource_file_sprite_set_c * load_sprite_set( string16_c const & file_path_relative, boolean_c load_now = true );
		resource_file_texture_c * load_texture( string16_c const & file_path_relative, boolean_c load_now = true );

	};

}
