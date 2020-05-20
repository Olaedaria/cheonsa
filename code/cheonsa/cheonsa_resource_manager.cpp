#include "cheonsa_resource_manager.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	uint32_c resource_manager_c::_worker_thread_function( thread_c * platform_thread )
	{
		while ( platform_thread->get_keep_alive() )
		{
			resource_manager_c * resource_manager = reinterpret_cast< resource_manager_c * >( platform_thread->get_parameter() );

			// copy the resources that need to be loaded asynchronously and then delete the original in the main thread.
			resource_manager->_worker_thread_critical_section.enter();
			if ( resource_manager->_load_queue.get_length() )
			{
				resource_manager->_worker_thread_load_queue.insert( -1, resource_manager->_load_queue.get_internal_array(), resource_manager->_load_queue.get_length() );
				resource_manager->_worker_thread_load_queue.remove_all();
			}
			resource_manager->_worker_thread_critical_section.exit();
			
			// load the next resource in the queue.
			if ( resource_manager->_worker_thread_load_queue.get_length() > 0 )
			{
				resource_file_c * resource_file = resource_manager->_worker_thread_load_queue[ 0 ];
				resource_manager->_worker_thread_load_queue.remove( 0, 1 );
				resource_file->remove_reference();
				resource_file->refresh();
			}
			else
			{
				// gives some window of opportunity for the other thread to enter the critical section.
				thread_c::sleep( 100 );
			}
		}
		return 0;
	}

	resource_manager_c::resource_manager_c()
		: _resource_list()
		, _load_queue()
		, _worker_thread()
		, _worker_thread_critical_section()
		, _worker_thread_load_queue()
		, _cpu_memory_usage( 0 )
		, _gpu_memory_usage( 0 )
		, _scan_enable( false )
		, _scan_index( 0 )
	{
	}

	resource_manager_c::~resource_manager_c()
	{
		_worker_thread.stop();
	}

	boolean_c resource_manager_c::start()
	{
		_worker_thread.start( &resource_manager_c::_worker_thread_function, this );
		return true;
	}

	void_c resource_manager_c::refresh()
	{
		for ( sint32_c i = 0; i < _resource_list.get_length(); i++ )
		{
			resource_file_c * resource_file = _resource_list[ i ];
			resource_file->refresh();
		}
	}

	template< typename resource_file_type_c >
	resource_file_type_c * resource_manager_c::_load( string16_c const & file_path, boolean_c load_now )
	{
		if ( file_path == "" )
		{
			return nullptr;
		}

		// search for existing resource instance.
		for ( sint32_c i = 0; i < _resource_list.get_length(); i++ )
		{
			resource_file_c * resource = _resource_list[ i ];
			if ( resource->get_resource_file_type() == resource_file_type_c::get_resource_file_type_static() && resource->get_file_path() == file_path )
			{
				return dynamic_cast< resource_file_type_c * >( resource );
			}
		}

		// create new resource instance.
		resource_file_type_c * resource_file = new resource_file_type_c( file_path );
		_resource_list.insert( -1, resource_file );

		// load.
		if ( load_now )
		{
			// load the resource data now on this main thread.
			resource_file->refresh();
		}
		else
		{
			// add the resource to the load queue
			resource_file->add_reference();
			_worker_thread_critical_section.enter();
			_load_queue.insert( -1, resource_file );
			_worker_thread_critical_section.exit();
		}

		// return the new resource instance.
		return resource_file;
	}

	resource_file_font_c * resource_manager_c::load_font( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_font_c >( relative_file_path, load_now );
	}

	resource_file_materials_c * resource_manager_c::load_materials( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_materials_c >( relative_file_path, load_now );
	}

	resource_file_menu_layout_c * resource_manager_c::load_menu_layout( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_menu_layout_c >( relative_file_path, load_now );
	}

	resource_file_model_c * resource_manager_c::load_model( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_model_c >( relative_file_path, load_now );
	}
	
	resource_file_sprites_c * resource_manager_c::load_sprites( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_sprites_c >( relative_file_path, load_now );
	}

	resource_file_strings_c * resource_manager_c::load_strings( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_strings_c >( relative_file_path, load_now );
	}

	resource_file_texture_c * resource_manager_c::load_texture( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_texture_c >( relative_file_path, load_now );
	}

}
