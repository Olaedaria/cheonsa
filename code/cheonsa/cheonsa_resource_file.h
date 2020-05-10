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
	// through the resource manager, at most only one resource is instanced per source file per resource type.
	// the game can then instance any number of content kinds of objects using a single resource file instance.
	class resource_file_c
	{
	public:
		static char8_c const * get_resource_file_type_static() { return "none"; }
		virtual char8_c const * get_resource_file_type() const { return get_resource_file_type_static(); }

	protected:
		string16_c _file_path; // the relative file path of the file in the game's content system.
		string16_c _file_path_absolute; // the resolved absolute file path of the file in the operating system's file system.
		sint64_c _file_last_write_time; // the last write time of the loaded file.

		boolean_c _search_game_data; // default is true. if true then the content manager will search the game data folders for a match when trying to resolve _file_path to _file_path_absolute.
		boolean_c _search_engine_data; // default is true. if true then the content manager will search the engine data folders for a match when trying to resolve _file_path to _file_path_absolute.
		boolean_c _is_loaded; // tracks if this resource is loaded and ready to be used or not.

		sint32_c _reference_count; // tracks the number of users using this resource. this resource does not delete itself when this count reaches 0.

		virtual void_c _load( data_stream_c * stream ) = 0;
		virtual void_c _unload() = 0;

	public:
		resource_file_c( string16_c const & file_path );
		virtual ~resource_file_c();

		boolean_c get_search_game_data() const;
		void_c set_search_game_data( boolean_c value );

		boolean_c get_search_engine_data() const;
		void_c set_search_engine_data( boolean_c value );

		boolean_c get_is_loaded() const;

		string16_c const & get_file_path() const; // gets the relative file path of the file in the game's content system.
		string16_c const & get_file_path_absolute() const; // gets the resolved absolute file path of the file in the operating system's file system, which is the specific file that this resource is loaded from.
		sint64_c get_file_last_write_time() const; // gets the last write time of the loaded file.

		void_c add_reference(); // adds a reference to this resource file.
		void_c remove_reference(); // removes a reference from this resource file. does not delete this resource file if it reaches 0, that is the resource manager's responsibility.

		void_c refresh(); // scans for changes to source file and reloads the resource if needed.

	public:
		core_event_c< void_c, resource_file_c * > on_loaded; // this event is invoked from _load as soon the resource transitions to a usable state.
		core_event_c< void_c, resource_file_c * > on_unloaded; // this event is invoked from _unload right before the resource transitions to an unusable state.

	};

	// handles reference counting for resource_file_c pointer types.
	// does not delete referenced instance when reference count reaches 0.
	// resource manager is in charge of doing that during garbage collection.
	template< typename resource_file_type_c >
	class resource_file_reference_c
	{
	private:
		resource_file_type_c * _value;

	public:
		resource_file_reference_c()
			: _value( nullptr )
		{
		}

		resource_file_reference_c( resource_file_reference_c const & other )
			: _value( other._value )
		{
			if ( _value )
			{
				_value->add_reference();
			}
		}

		resource_file_reference_c( resource_file_type_c * other )
			: _value( other )
		{
			if ( _value )
			{
				_value->add_reference();
			}
		}

		~resource_file_reference_c()
		{
			if ( _value )
			{
				_value->remove_reference();
			}
		}

		resource_file_reference_c & operator = ( resource_file_type_c * other )
		{
			if ( _value != other )
			{
				if ( _value )
				{
					_value->remove_reference();
				}
				_value = other;
				if ( _value )
				{
					_value->add_reference();
				}
			}
			return *this;
		}

		resource_file_reference_c & operator = ( resource_file_reference_c & other )
		{
			*this = other._value;
			return *this;
		}

		resource_file_type_c * operator -> () const
		{
			assert( _value );
			return _value;
		}

		operator resource_file_type_c * () const
		{
			return _value;
		}

		boolean_c get_is_value_set() const
		{
			return _value != nullptr;
		}

		boolean_c get_is_value_set_and_loaded() const
		{
			return ( _value != nullptr ) && ( _value->get_is_loaded() );
		}

	};

}
