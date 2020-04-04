#pragma once

namespace cheonsa
{

	// value_type_c should be a pointer type to a class that has add_reference() and remove_reference() methods.
	// the safe pointer automatically deletes the thing that is pointed to when its reference count reaches 0.
	// analogous to std::shared_ptr.
	template< typename value_type_c >
	class core_shared_pointer_c
	{
	private:
		value_type_c _value;

	public:
		core_shared_pointer_c()
			: _value( nullptr )
		{
		}

		core_shared_pointer_c( core_shared_pointer_c const & other )
			: _value( other._value )
		{
			if ( _value )
			{
				_value->add_reference();
			}
		}

		core_shared_pointer_c( value_type_c other )
			: _value( other )
		{
			if ( _value )
			{
				_value->add_reference();
			}
		}

		~core_shared_pointer_c()
		{
			if ( _value )
			{
				if ( _value->remove_reference() == 0 )
				{
					delete _value;
					_value = nullptr;
				}
			}
		}

		core_shared_pointer_c & operator = ( core_shared_pointer_c & other_value_reference )
		{
			if ( other_value_reference._value )
			{
				other_value_reference._value->add_reference();
			}
			if ( _value )
			{
				if ( _value->remove_reference() == 0 )
				{
					delete _value;
					_value = nullptr;
				}
			}
			_value = other_value_reference._value;
		}

		core_shared_pointer_c & operator = ( value_type_c other_value )
		{
			if ( other_value )
			{
				other_value->add_reference();
			}
			if ( _value )
			{
				if ( _value->remove_reference() == 0 )
				{
					delete _value;
					_value = nullptr;
				}
			}
			_value = other_value;
		}

		value_type_c operator -> () const
		{
			assert( _value );
			return _value;
		}

		operator value_type_c () const
		{
			return _value;
		}

	};

}
