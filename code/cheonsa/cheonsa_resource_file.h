#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_event.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_stream.h"
#include "cheonsa_data_stream_file.h"

namespace cheonsa
{

	class resource_manager_c;

	// base class of resources.
	// a resource file represents a file that is loaded from disk, and which can be reloaded at run time if the source file is modified.
	// through the resource manager, at most only one resource file is instanced per source file per resource type.
	// the game can then instance any number of content kinds of objects using a single resource file instance.
	class resource_file_c
	{
	public:
		static char8_c const * get_type_static() { return "none"; }
		virtual char8_c const * get_type() const { return get_type_static(); }

	protected:
		template< typename resource_file_type_c >
		friend class resource_file_reference_c;
		friend class resource_manager_c;

		boolean_c _is_loaded; // tracks if this resource is loaded and ready to be used or not.
		string16_c _relative_file_path; // file path that the game uses to identify this resource.
		string16_c _absolute_file_path; // relative file path resolved to a full file path that identifies where this resource was actually loaded from.
		sint64_c _file_modified_time; // tracks source file modified time, so we can detect if the source file is modified during run time and re load it.
		sint32_c _reference_count; // number of users using this resource, so we can automatically un load it when it has no more users.

		virtual boolean_c _load( data_stream_c * stream ) = 0;
		virtual void_c _unload() = 0;

	public:
		resource_file_c();
		resource_file_c( resource_file_c const & ) = delete;
		virtual ~resource_file_c() = 0;
		resource_file_c & operator = ( resource_file_c const & ) = delete;

		boolean_c get_is_loaded() const;

		string16_c const & get_relative_file_path() const;
		string16_c const & get_absolute_file_path() const;

		sint64_c get_file_modified_time() const;

		sint32_c get_reference_count() const;

	public:
		core_event_c< void_c, resource_file_c * > on_load; // users may subscribe to this event to be notified when we are done loading.
		core_event_c< void_c, resource_file_c * > on_unload; // users may subscribe to this event to be notified when we are about to unload.

	};

	// handles reference counting for resource_file_c pointer types.
	// does not delete referenced instance when reference count reaches 0.
	// resource manager is in charge of doing that during garbage collection.
	template< typename resource_file_type_c >
	class resource_file_reference_c
	{
	private:
		resource_file_type_c * _reference;

	public:
		resource_file_reference_c()
			: _reference( nullptr )
		{
		}

		resource_file_reference_c( resource_file_reference_c const & other )
			: _reference( other._reference )
		{
			if ( _reference )
			{
				_reference->_reference_count++;
			}
		}

		resource_file_reference_c( resource_file_type_c * other_value )
			: _reference( other_value )
		{
			if ( _reference )
			{
				_reference->_reference_count++;
			}
		}

		~resource_file_reference_c()
		{
			if ( _reference )
			{
				assert( _reference->_reference_count > 0 );
				_reference->_reference_count--;
			}
		}

		resource_file_reference_c & operator = ( resource_file_type_c * other_reference )
		{
			if ( _reference != other_reference )
			{
				if ( _reference != nullptr )
				{
					assert( _reference->_reference_count > 0 );
					_reference->_reference_count--;
				}
				_reference = other_reference;
				if ( _reference != nullptr )
				{
					_reference->_reference_count++;
				}
			}
			return *this;
		}

		resource_file_reference_c & operator = ( resource_file_reference_c & other )
		{
			return operator = ( other._reference );
		}

		resource_file_type_c * operator -> () const
		{
			assert( _reference );
			return _reference;
		}

		operator resource_file_type_c * () const
		{
			return _reference;
		}

		boolean_c is_reference_set() const
		{
			return _reference != nullptr;
		}

		boolean_c is_reference_set_and_loaded() const
		{
			return ( _reference != nullptr ) && ( _reference->_is_loaded );
		}

	};

}
