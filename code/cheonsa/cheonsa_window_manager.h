#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"
#include "cheonsa_platform_icon.h"

namespace cheonsa
{

	enum window_state_e
	{
		window_state_e_undefined, // window is not created yet.
		window_state_e_minimized,
		window_state_e_normaled,
		window_state_e_maximized,
	};

	// manages creating a borderless style window.
	// window can still be resized by the edges and moved by the title bar, but they are invisible.
	// because the window is borderless, the area that can be rendered to is the entire window rectangle.
	// the engine creates the main client window at start up.
	// then the user interface creates a canvas with the window as the output.
	class window_manager_c
	{
	private:
		friend class engine_c;
		friend class window_manager_members_c;

		void_c * _process_handle;
		void_c * _window_handle;
		window_state_e _window_state; // treat as read only, but use set_window_state() to change.
		boolean_c _window_is_focused; // will be set to true as long as the client's window has focus.
		sint32_c _window_edge_thickness; // when the client window is borderless mode, this tells the non client hit detection logic the thickness of each resizable edge handle in pxiels.
		sint32_c _window_title_bar_thickness; // when the client window is in borderless mode, this tells the non client hit detection logic the size of the title bar in pixels.

		window_manager_c();

	public:
		~window_manager_c();

		boolean_c open_client_window(); // opens the client window.
		void_c center_client_window(); // centers the window again after exiting full screen.
		void_c close_client_window(); // closes the client window.

		void_c * get_window_handle(); // gets the client window handle, but casted to a void_c const * so that it is platform neutral.
		void_c set_window_title( string16_c const & value ); // called by client to set title of the window.
		void_c set_window_icon_big( platform_icon_c * value ); // called by client to set icon of the window. the big icon is shown when alt+tabbing.
		void_c set_window_icon_small( platform_icon_c * value ); // called by client to set icon of the window. the small icon is shown in the title bar of the window.

		window_state_e get_window_state() const;
		void_c set_window_state( window_state_e value );

		sint32_c get_window_edge_thickness() const;
		sint32_c get_window_title_bar_thickness() const;

	};

}
