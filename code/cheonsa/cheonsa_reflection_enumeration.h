#pragma once

#include "cheonsa_base_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_reflection_types.h"

namespace cheonsa
{

	class reflection_enumeration_c
	{
	public:
		class value_c
		{
		private:
			string8_c _name;
			string8_c _description;
			uint32_c _value;

		public:
			value_c( string8_c const & name, string8_c const & description, sint32_c value );

			string8_c const & get_name() const;
			string8_c const & get_description() const;
			sint32_c get_value() const;

		};

	private:
		reflection_manager_c * _reflection_manager;
		string8_c _name;
		string8_c _description;
		boolean_c _is_for_bit_flags;
		core_list_c< value_c * > _value_list;

	public:
		reflection_enumeration_c( reflection_manager_c * reflection_manager, string8_c const & name, string8_c const & description );
		~reflection_enumeration_c();

		string8_c const & get_name() const;
		string8_c const & get_description() const;

		void_c initialize_for_bit_flags(); // if called then the values in this enum represent bit flags and so the property inspector will allow multiple values to be enabled at a time.
		void_c initialize_value( string8_c const & name, string8_c const & description, sint32_c value ); // adds a value to this enumeration.

		sint32_c get_value_count() const;
		value_c const * get_value( sint32_c index ) const;

		boolean_c find_name_with_value( string8_c & name, uint32_c value ) const;
		boolean_c find_name_with_index( string8_c & name, sint32_c index ) const;

		boolean_c find_value_with_name( uint32_c & value, string8_c const & name ) const;
		boolean_c find_value_with_index( uint32_c & value, sint32_c index ) const;

		boolean_c find_index_with_name( sint32_c & index, string8_c const & name ) const;
		boolean_c find_index_with_value( sint32_c & index, uint32_c value ) const;

		boolean_c contains_value( uint32_c value ) const;

	};

}
