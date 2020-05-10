#pragma once

#include "cheonsa_core_list.h"
#include <cassert>

namespace cheonsa
{

	template< typename return_type_c, typename parameter_type_c >
	class core_event_c
	{
	public:
		class delegate_c
		{
		public:
			enum type_e
			{
				type_e_none,
				type_e_function,
				type_e_method
			};
			virtual type_e get_type() = 0;
			virtual boolean_c equals( delegate_c * other ) = 0;
			virtual return_type_c invoke( parameter_type_c parameter ) = 0;
		};

		class delegate_function_c
			: public delegate_c
		{
		public:
			typedef return_type_c (*function_type_c)( parameter_type_c );
			function_type_c _function;

		public:
			delegate_function_c( function_type_c function )
				: _function( function )
			{
				assert( function );
			}

			virtual type_e get_type()
			{
				return type_e_function;
			}

			virtual boolean_c equals( delegate_c * other )
			{
				if ( other->get_type() == type_function )
				{
					delegate_function * other_delegate_function = static_cast< delegate_function * >( other );
					return ( other_delegate_function->_function == _function );
				}
				return false;
			}

			virtual return_type_c invoke( parameter_type_c parameter )
			{
				return _function( parameter );
			}

		};

		template< class class_type_c >
		class delegate_method_c
			: public delegate_c
		{
		public:
			typedef return_type_c (class_type_c::*method_type_c)( parameter_type_c );
			class_type_c * _instance;
			method_type_c _method;

		public:
			delegate_method_c( class_type_c * instance, method_type_c method )
				: _instance( instance )
				, _method( method )
			{
				assert( instance );
				assert( method );
			}

			virtual type_e get_type()
			{
				return type_e_method;
			}

			virtual boolean_c equals( delegate_c * other )
			{
				if ( other->get_type() == type_e_method )
				{
					delegate_method_c * other_delegate_method = static_cast< delegate_method_c * >( other );
					return ( other_delegate_method->_instance == _instance ) && ( other_delegate_method->_method == _method );
				}
				return false;
			}

			virtual return_type_c invoke( parameter_type_c paramter )
			{
				return ( _instance->*_method )( paramter );
			}

		};

	private:
		core_list_c< delegate_c * > _delegate_list;

	public:
		core_event_c()
		{
		}

		~core_event_c()
		{
			for ( sint32_c i = 0; i < _delegate_list.get_length(); i++ )
			{
				delete _delegate_list[ i ];
			}
		}

		template< typename function_type_c >
		void_c subscribe( function_type_c function )
		{
			delegate_function_c * new_delegate = new delegate_function_c( function );
			for ( sint32_c i = 0; i < _delegate_list.get_length(); i++ )
			{
				if ( _delegate_list[ i ]->equals( new_delegate ) )
				{
					delete new_delegate;
					return;
				}
			}
			_delegate_list.insert_at_end( new_delegate );
		}

		template< class class_type_c, typename method_type_c >
		void_c subscribe( class_type_c * instance, method_type_c method )
		{
			delegate_method_c< class_type_c > * new_delegate = new delegate_method_c< class_type_c >( instance, method );
			for ( sint32_c i = 0; i < _delegate_list.get_length(); i++ )
			{
				if ( _delegate_list[ i ]->equals( new_delegate ) )
				{
					delete new_delegate;
					return;
				}
			}
			_delegate_list.insert( -1, new_delegate );
		}

		template< typename function_type_c >
		void_c unsubscribe( function_type_c function )
		{
			delegate_method_c * new_delegate = new delegate_function_c( function );
			for ( sint32_c i = 0; i < _delegate_list.get_length(); i++ )
			{
				if ( _delegate_list[ i ]->equals( new_delegate ) )
				{
					delete new_delegate;
					_delegate_list.remove( i, 1 );
					return;
				}
			}
			delete new_delegate;
		}

		template< class class_type_c, typename method_type_c >
		void_c unsubscribe( class_type_c * instance, method_type_c method )
		{
			delegate_method_c< class_type_c > * new_delegate = new delegate_method_c< class_type_c >( instance, method );
			for ( sint32_c i = 0; i < _delegate_list.get_length(); i++ )
			{
				if ( _delegate_list[ i ]->equals( new_delegate ) )
				{
					delete new_delegate;
					_delegate_list.remove( i, 1 );
					return;
				}
			}
			delete new_delegate;
		}


		void_c invoke( parameter_type_c parameter ) const
		{
			core_list_c< delegate_c * > copy = _delegate_list; // iterate over a copy so that we are not affected if subscription list is modified during invocation.
			for ( sint32_c i = 0; i < copy.get_length(); i++ )
			{
				copy[ i ]->invoke( parameter );
			}
		}

		return_type_c invoke_with_return_value( parameter_type_c parameter ) const
		{
			return_type_c result = 0;
			core_list_c< delegate_c * > copy = _delegate_list; // iterate over a copy so that we are not affected if subscription list is modified during invocation.
			for ( sint32_c i = 0; i < copy.get_length(); i++ )
			{
				result = copy[ i ]->invoke( parameter );
			}
			return result;
		}

		sint32_c get_subscriber_count() const
		{
			return _delegate_list.get_length();
		}

	};

}
