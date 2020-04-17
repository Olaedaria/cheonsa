#pragma once

#include "cheonsa_scene.h"
#include "cheonsa_video_renderer_canvas.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_control.h"

namespace cheonsa
{

	// lets us nest a 3d scene inside of a 2d menu.
	// the scene and camera must be managed externally.
	// the canvas is managed internally.
	// we can use this to do things like:
	//		create a 2d panel that gives the player a 3d preview of their character.
	//		create a screen in the 3d scene that renders the scene from a different point of view, or renders a different scene, like a cctv or a live stream or something.
	// we can use this to do things like:
	//		render the same scene from multiple points of view at once.
	//		render multiple scenes from different points of view at once.
	class menu_control_scene_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scene"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	protected:
		menu_element_frame_c _element_frame; // does not have a name.
		menu_frame_style_c _element_frame_style; // style override for frame element.

		video_renderer_canvas_c * _canvas; // canvas, sized to be same as size of _local_box.
		resource_file_texture_c _canvas_wrapper; // canvas wrapper, so that the contents of the canvas can be bound as a texture input and rendered like a menu frame. this has to be resized each time the canvas is resized.

		scene_c * _scene; // the 3d scene that this menu control will render. this needs to be provided by and managed by the game.

		virtual void_c _on_input( input_event_c * input_event ) override;

		virtual void_c _update_transform_and_layout() override;

	public:
		menu_control_scene_c( string8_c const & name );
		~menu_control_scene_c();

		scene_c * get_scene() const; // gets the scene that this scene control will render.
		void_c set_scene( scene_c * value ); // sets the scene that this scene control will render.

		video_renderer_canvas_c * get_canvas() const; // gets the canvas that this scene control renders to.

	public:
		void_c give_control( menu_control_c * control );
		menu_control_c * take_control( menu_control_c * control );

		core_event_c< void_c, float32_c > on_update;
		core_event_c< void_c, menu_event_information_c > on_input;

	};

}
