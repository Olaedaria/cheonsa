#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_core_circle_buffer.h"
#include "cheonsa_core_event.h"

namespace cheonsa
{

	enum debug_log_type_e
	{
		debug_log_type_e_information, // white.
		debug_log_type_e_okay, // green-yellow.
		debug_log_type_e_warning, // yellow.
		debug_log_type_e_error, // red-orange.
	};

	class debug_log_message_c
	{
		friend class debug_manager_c;

	private:
		debug_log_type_e _type;
		string16_c _message;

	public:
		debug_log_message_c();

		debug_log_type_e get_type() const;
		string16_c const & get_message() const;

	};

	// defines which debug overlays for the renderer to render.
	// defines which colors for the renderer to use when rendering debug overlays.
	// manages the log file.
	// manages real-time run-time statistic tracking.
	// manages the engine's command line console.
	class debug_manager_c
	{
		friend class engine_c;

	private:
		vector32x4_c _grid_color;
		vector32x4_c _axis_a_color;
		vector32x4_c _axis_b_color;
		vector32x4_c _axis_c_color;
		vector32x4_c _axis_d_color;
		vector32x4_c _bone_color;
		vector32x4_c _menu_bounds_color;

		boolean_c _draw_menu_bounds; // draws boxes around menu controls.
		boolean_c _draw_world_axes; // draws axes at center of scene.
		boolean_c _draw_world_grid; // draws grid around center of scene.
		boolean_c _draw_object_light_probes; // draws spheres for light probes in scene.
		boolean_c _draw_object_icons; // draws icons of objects of objects in scene.
		boolean_c _draw_object_axes; // draws axes of objects in scene.
		boolean_c _draw_object_bounds; // draws bounds of objects in scene.
		boolean_c _draw_object_bones; // draws bones of model objects in scene.
		boolean_c _draw_object_physics; // draws physics debug.

		boolean_c _log_file_is_open;
		data_stream_file_c _log_file; // log file stream located at environment.engine_data_folder + "log.txt".

		boolean_c _statistics_is_showing;
		string16_c _statistics_text;

		debug_manager_c();
		~debug_manager_c();

		boolean_c start( string16_c const & executable_file_path ); // opens the log file.

	public:
		inline vector32x4_c const & get_grid_color() const { return _grid_color; }
		inline void_c set_grid_color( vector32x4_c const & value ) { _grid_color = value; }

		inline vector32x4_c const & get_axis_a_color() const { return _axis_a_color; }
		inline void_c set_axis_a_color( vector32x4_c const & value ) { _axis_a_color = value; }

		inline vector32x4_c const & get_axis_b_color() const { return _axis_b_color; }
		inline void_c set_axis_b_color( vector32x4_c const & value ) { _axis_b_color = value; }

		inline vector32x4_c const & get_axis_c_color() const { return _axis_c_color; }
		inline void_c set_axis_c_color( vector32x4_c const & value ) { _axis_c_color = value; }

		inline vector32x4_c const & get_axis_d_color() const { return _axis_d_color; }
		inline void_c set_axis_d_color( vector32x4_c const & value ) { _axis_d_color = value; }

		inline vector32x4_c const & get_bone_color() const { return _bone_color; }
		inline void_c set_bone_color( vector32x4_c const & value ) { _bone_color = value; }

		inline vector32x4_c const & get_menu_bounds_color() const { return _menu_bounds_color; }
		inline void_c set_menu_bounds_color( vector32x4_c const & value ) { _menu_bounds_color = value; }

		inline boolean_c get_draw_menu_bounds() const { return _draw_menu_bounds; }
		inline void_c set_draw_menu_bounds( boolean_c value ) { _draw_menu_bounds = value; }

		inline boolean_c get_draw_world_axes() const { return _draw_world_axes; }
		inline void_c set_draw_world_axes( boolean_c value ) { _draw_world_axes = value; }

		inline boolean_c get_draw_world_grid() const { return _draw_world_grid; }
		inline void_c set_draw_world_grid( boolean_c value ) { _draw_world_grid = value; }

		inline boolean_c get_draw_object_light_probes() const { return _draw_object_light_probes; }
		inline void_c set_draw_object_light_probes( boolean_c value ) { _draw_object_light_probes = value; }

		inline boolean_c get_draw_object_icons() const { return _draw_object_icons; }
		inline void_c set_draw_object_icons( boolean_c value ) { _draw_object_icons = value; }

		inline boolean_c get_draw_object_axes() const { return _draw_object_axes; }
		inline void_c set_draw_object_axes( boolean_c value ) { _draw_object_axes = value; }

		inline boolean_c get_draw_object_bounds() const { return _draw_object_bounds; }
		inline void_c set_draw_object_bounds( boolean_c value ) { _draw_object_bounds = value; }

		inline boolean_c get_draw_object_bones() const { return _draw_object_bones; }
		inline void_c set_draw_object_bones( boolean_c value ) { _draw_object_bones = value; }

		inline boolean_c get_draw_object_physics() const { return _draw_object_physics; }
		inline void_c set_draw_object_physics( boolean_c value ) { _draw_object_physics = value; }

		void_c _log( debug_log_type_e type, string16_c const & message ); // don't call this directly, prefer to call debug_log() instead.

		core_event_c< void_c, debug_log_message_c const & > on_message_logged; // this event is invoked each time a message is logged with debug_log or debug_annoy.

	};

	void_c debug_log( debug_log_type_e type, char8_c const * message ); // if debugging then writes a message to the debug output in the ide. if the debug manager is started then it also writes a message to the engine's log file and console.
	void_c debug_log( debug_log_type_e type, string8_c const & message );
	void_c debug_log( debug_log_type_e type, char16_c const * message ); // if debugging then writes a message to the debug output in the ide. if the debug manager is started then it also writes a message to the engine's log file and console.
	void_c debug_log( debug_log_type_e type, string16_c const & message );

	void_c debug_annoy( char8_c const * title, char8_c const * message );
	void_c debug_annoy( char16_c const * title, char16_c const * message ); // this will open up an operating system message box pop up window with an OK button, it blocks execution until the user dismisses it.

}
