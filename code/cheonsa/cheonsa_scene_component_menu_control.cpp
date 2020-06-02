#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c scene_component_menu_control_c::_handle_after_added_to_user_interface()
	{
		if ( _control )
		{
			assert( _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() );
			_scene_object->get_scene()->get_user_interface()->add_daughter_control( _control );
		}
	}

	void_c scene_component_menu_control_c::_handle_before_removed_from_user_interface()
	{
		if ( _control )
		{
			assert( _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() );
			_scene_object->get_scene()->get_user_interface()->remove_daughter_control( _control );
		}
	}

	void_c scene_component_menu_control_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		if ( _control && _scene_object->get_scene()->get_user_interface() )
		{
			_scene_object->get_scene()->get_user_interface()->add_daughter_control( _control );
		}
	}

	void_c scene_component_menu_control_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		if ( _control && _scene_object->get_scene()->get_user_interface() )
		{
			_scene_object->get_scene()->get_user_interface()->remove_daughter_control( _control );
		}
	}

	scene_component_menu_control_c::scene_component_menu_control_c()
		: scene_component_c()
		, _control( nullptr )
	{
	}

	scene_component_menu_control_c::~scene_component_menu_control_c()
	{
		if ( _control )
		{
			if ( _scene_object && _scene_object->get_scene() && _control->get_mother_user_interface() )
			{
				_scene_object->get_scene()->get_user_interface()->remove_daughter_control( _control );
			}
			_control->remove_reference();
			_control = nullptr;
		}
	}

	menu_control_c * scene_component_menu_control_c::get_control() const
	{
		return _control;
	}

	void_c scene_component_menu_control_c::set_control( menu_control_c * control )
	{
		if ( _control != control )
		{
			if ( _control )
			{
				if ( _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() )
				{
					_scene_object->get_scene()->get_user_interface()->remove_daughter_control( _control );
				}
				_control->set_scene_component( nullptr );
				_control->remove_reference();
			}

			_control = control;

			if ( _control )
			{
				_control->add_reference();
				_control->set_scene_component( this );
				if ( _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() )
				{
					_scene_object->get_scene()->get_user_interface()->add_daughter_control( _control );
				}
			}
		}
	}

}
