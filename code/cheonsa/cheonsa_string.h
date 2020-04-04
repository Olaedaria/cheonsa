#pragma once

#include "cheonsa_core_linked_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_core_event.h"

namespace cheonsa
{

	// these strings are loaded from a "strings.xml" file, they are immutable, they have a unique key which the game uses to find and reference the string, and they have a value which the game can read.
	// don't confuse these strings with the typical kind of programmer string construct, see string8_c and string16_c for that.
	// these are not reference counted, so each time a "strings.xml" file is unloaded or reloaded, all string reference instances that exist are forced to refresh so that they don't hold on to any dead references.
	class string_c
	{
	public:
		// a reference to a string instance.
		// these instances are persistent.
		// but they reference a string_c instance which may be volatile.
		class reference_c
		{
		private:
			friend class content_manager_c;

			static core_linked_list_c< reference_c * > _global_list; // all string_c::reference_c instances in existence.
			core_linked_list_c< reference_c * >::node_c _global_list_node; // this instance's list node in _global_instance_list.

			string8_c _key;
			string_c const * _value;

		public:
			reference_c();
			~reference_c();

			void_c release_value();
			void_c resolve_value();

			void_c clear(); // unsets _key and sets _value to nullptr and invokes on_refreshed.

			void_c set( string_c const * value ); // a little more optimal than set_key() since it avoids doing a look up.

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value ); // sets the key and then tries to resolve value with it.

			string_c const * get_value() const;

			core_event_c< void_c, reference_c const * > on_refreshed;

		};

	private:
		friend class string_file_c;

		string8_c _key;
		string8_c _value; // utf-8 encoded string.

	public:
		string8_c const & get_key() const;
		string8_c const & get_value() const;

		string_c & operator = ( string_c const & other ); // needed so that the _string_heap can be resized.

	};

}
