#include "cheonsa_resource_manager.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	uint32_c resource_manager_c::_worker_thread_function( platform_thread_c * platform_thread )
	{
		while ( platform_thread->get_keep_alive() )
		{
			resource_manager_c * resource_manager = reinterpret_cast< resource_manager_c * >( platform_thread->get_parameter() );

			// copy the resources that need to be loaded asynchronously and then delete the original in the main thread.
			resource_manager->_worker_thread_critical_section.enter();
			if ( resource_manager->_load_queue.get_length() )
			{
				resource_manager->_worker_thread_load_queue.insert_at_end( resource_manager->_load_queue.get_internal_array(), resource_manager->_load_queue.get_length() );
				resource_manager->_worker_thread_load_queue.remove_all();
			}
			resource_manager->_worker_thread_critical_section.exit();
			
			// load the next resource in the queue.
			if ( resource_manager->_worker_thread_load_queue.get_length() > 0 )
			{
				resource_file_c * resource = resource_manager->_worker_thread_load_queue[ 0 ];
				resource_manager->_worker_thread_load_queue.remove_at_index( 0 );
					
				string16_c absolute_file_path;
				if ( engine_c::get_instance()->get_content_manager()->resolve_absolute_file_path( resource->get_relative_file_path(), absolute_file_path ) )
				{
					data_stream_file_c stream;
					if ( stream.open( absolute_file_path, data_stream_mode_e_read ) )
					{
						resource_manager->_load_internal( resource, &stream, absolute_file_path, true );
						stream.close();
					}
				}
			}
			else
			{
				// gives some window of opportunity for the other thread to enter the critical section.
				platform_thread_c::sleep( 100 );
			}
		}
		return 0;
	}

	void_c resource_manager_c::_load_internal( resource_file_c * resource, data_stream_c * stream, string16_c const & absolute_file_path, boolean_c for_async_load_thread )
	{
		if ( resource->_load( stream ) )
		{
			resource->_absolute_file_path = absolute_file_path;
			ops::file_system_get_file_or_folder_modified_time( absolute_file_path, resource->_file_modified_time );
			if ( for_async_load_thread )
			{
				ops::file_system_get_file_or_folder_modified_time( absolute_file_path, resource->_file_modified_time );
				assert( resource->_reference_count > 0 ); // there should be at least one fake user count added when the resource was put into the _async_load_queue.
				resource->_reference_count--; // remove the fake user.
			}
		}
	}

	void_c resource_manager_c::_refresh_resource( resource_file_c * resource )
	{
		string16_c absolute_file_path = resource->_absolute_file_path;
		sint64_c file_modified_time = resource->_file_modified_time;

		if ( !engine_c::get_instance()->get_content_manager()->resolve_absolute_file_path( resource->_relative_file_path, absolute_file_path ) )
		{
			return;
		}

		if ( !ops::file_system_get_file_or_folder_modified_time( absolute_file_path, file_modified_time ) )
		{
			return;
		}

		if ( resource->_absolute_file_path != absolute_file_path || resource->_file_modified_time != file_modified_time )
		{
			data_stream_file_c file_stream;
			if ( file_stream.open( absolute_file_path, data_stream_mode_e_read ) )
			{
				if ( resource->get_is_loaded() )
				{
					resource->_unload();
				}
				resource->_file_modified_time = 0;
				if ( resource->_load( &file_stream ) )
				{
					resource->_absolute_file_path = absolute_file_path;
					resource->_file_modified_time = file_modified_time;
				}
			}
		}
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

	void_c resource_manager_c::update()
	{
		if ( _scan_enable )
		{
			if ( _scan_index < 0 || _scan_index > _resource_list.get_length() )
			{
				_scan_index = 0;
			}
			sint32_c scan_per_frame = 4;
			for ( sint32_c i = 0; i < scan_per_frame; i++ )
			{
				resource_file_c * resource = _resource_list[ _scan_index ];
				_refresh_resource( resource );
				_scan_index++;
				if ( _scan_index >= _resource_list.get_length() )
				{
					break;
				}
			}
		}
	}

	void_c resource_manager_c::refresh()
	{
		for ( sint32_c i = 0; i < _resource_list.get_length(); i++ )
		{
			resource_file_c * resource = _resource_list[ i ];
			_refresh_resource( resource );
		}
	}

	template< typename resource_type_c >
	resource_type_c * resource_manager_c::_load( string16_c const & relative_file_path, boolean_c load_now )
	{
		if ( relative_file_path == "" )
		{
			return nullptr;
		}

		// search for existing resource instance.
		for ( sint32_c i = 0; i < _resource_list.get_length(); i++ )
		{
			resource_file_c * resource = _resource_list[ i ];
			if ( resource->get_type() == resource_type_c::get_type_static() && resource->_relative_file_path == relative_file_path )
			{
				return dynamic_cast< resource_type_c * >( resource );
			}
		}

		// create new resource instance.
		resource_type_c * new_resource = new resource_type_c();
		new_resource->_relative_file_path = relative_file_path;
		_resource_list.insert_at_end( new_resource );

		if ( load_now )
		{
			// load the resource data now on this main thread.
			string16_c absolute_file_path;
			if ( engine_c::get_instance()->get_content_manager()->resolve_absolute_file_path( relative_file_path, absolute_file_path ) )
			{
				data_stream_file_c stream;
				if ( stream.open( absolute_file_path, data_stream_mode_e_read ) )
				{
					_load_internal( new_resource, &stream, absolute_file_path, false );
					stream.close();
				}
			}
		}
		else
		{
			// add the resource to the load queue
			new_resource->_reference_count++;
			_worker_thread_critical_section.enter();
			_load_queue.insert_at_end( new_resource );
			_worker_thread_critical_section.exit();
		}

		// return the new resource instance.
		return new_resource;
	}

	resource_file_font_c * resource_manager_c::load_font( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_font_c >( relative_file_path, load_now );
	}

	resource_file_material_map_c * resource_manager_c::load_material_map( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_material_map_c >( relative_file_path, load_now );
	}

	resource_file_menu_layout_c * resource_manager_c::load_menu_layout( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_menu_layout_c >( relative_file_path, load_now );
	}

	resource_file_model_c * resource_manager_c::load_model( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_model_c >( relative_file_path, load_now );
	}
	
	resource_file_sprite_set_c * resource_manager_c::load_sprite_set( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_sprite_set_c >( relative_file_path, load_now );
	}

	resource_file_texture_c * resource_manager_c::load_texture( string16_c const & relative_file_path, boolean_c load_now )
	{
		return _load< resource_file_texture_c >( relative_file_path, load_now );
	}

}
