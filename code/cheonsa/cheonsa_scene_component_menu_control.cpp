#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c scene_component_menu_control_c::_add_control_to_user_interface()
	{
		if ( _control && _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() )
		{
			_scene_object->get_scene()->get_user_interface()->add_control( _control );
		}
	}

	void_c scene_component_menu_control_c::_remove_control_from_user_interface()
	{
		if ( _control && _scene_object && _scene_object->get_scene() && _scene_object->get_scene()->get_user_interface() )
		{
			_scene_object->get_scene()->get_user_interface()->remove_control( _control );
		}
	}

	void_c scene_component_menu_control_c::_handle_after_added_to_scene()
	{
		_add_control_to_user_interface();
	}

	void_c scene_component_menu_control_c::_handle_before_removed_from_scene()
	{
		_remove_control_from_user_interface();
	}

	scene_component_menu_control_c::scene_component_menu_control_c()
		: scene_component_c()
		, _control_is_ours( false )
		, _control( nullptr )
		, _last_render_frame( 0 )
	{
	}

	scene_component_menu_control_c::~scene_component_menu_control_c()
	{
		_remove_control_from_user_interface();
		if ( _control && _control_is_ours )
		{
			delete _control;
		}
		_control = nullptr;
		_control_is_ours = false;
	}

	void_c scene_component_menu_control_c::set_control( menu_control_c * control, boolean_c and_transfer_ownership )
	{
		if ( _control )
		{
			assert( _control->get_scene_component() == this );
			_remove_control_from_user_interface();
			_control->set_scene_component( nullptr );
			if ( _control_is_ours )
			{
				delete _control;
			}
		}

		_control = control;
		_control_is_ours = and_transfer_ownership;

		if ( _control )
		{
			assert( _control->get_scene_component() == nullptr );
			_control->set_scene_component( this );
			_add_control_to_user_interface();
		}
	}

}
