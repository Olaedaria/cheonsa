#pragma once

#include "cheonsa_string8.h"

namespace cheonsa
{

	// this enum is used with binary property scribe, reflection system, and database system.
	// modifying these values will break any files that you have created using these values.
	// the sizes of each type are usually straight forward but not always.
	// for databases, string8 and string16 types are 4 bytes long (interpret as uint32_c), as an offset in bytes into the table's string buffer.
	enum data_type_e
	{
		data_type_e_none = 0,

		data_type_e_list = 1, // only used with binary property scribe.
		data_type_e_string8 = 2, // for database system, type is sint32_c offset into string table.
		data_type_e_string16 = 3, // for database system, type is sint32_c offset into string table.
		data_type_e_uint8 = 4,
		data_type_e_sint8 = 5,
		data_type_e_uint16 = 6,
		data_type_e_sint16 = 7,
		data_type_e_uint32 = 8,
		data_type_e_sint32 = 9,
		data_type_e_uint64 = 10,
		data_type_e_sint64 = 11,
		data_type_e_float32 = 12,
		data_type_e_float64 = 13,

		data_type_e_object = 32, // only used with reflection system.
		data_type_e_object_list = 33, // only used with reflection system.
		data_type_e_class_label = 34, // only used with reflection system.
		data_type_e_category_label = 35, // only used with reflection system.
	};

	// used with databases and reflection to define what kind of editor to use for the data.
	enum data_view_e
	{
		// for all data types, single line of plain text.
		data_view_e_default,

		// provides a scroll bar with a value range of minimum to maximum.
		// only for float32x1 and float64x1 types.
		// asserts when used with other types.
		// asserts when mininum and maximum are not set.
		data_view_e_scroll_bar,

		// provides a scrub bar with a value range of minimum to maximum.
		// only for float32x1 and float64x1 types.
		// asserts when used with other types.
		// asserts when mininum and maximum are not set.
		data_view_e_scrub_bar,

		// provides a button that opens a color picker dialog.
		// only for uint8x3 (rgb), uint8x4 (rgba), uint32x1 (rgba), float32x3 (rgb), float32x4 (rgba), float64x3 (rgb), float64x4 (rgba) types.
		// for int types, plain text value will be "#" prefixed hex string.
		// for float types, plain text value will be standard coma separated list.
		// asserts when used with other types.
		data_view_e_color,

		// for string types, provides a button that opens a text editor for multi-line text editing.
		data_view_e_text,

		// provides euler angles in degrees in text editor.
		// for float types of length 3, value is interpreted as euler angles in degrees.
		// for float types of length 4, value is interpreted as quaternion, but does conversion to let the user interface with it as euler angles in degrees.
		data_view_e_euler_angles,

		// provides a combo list that enables the user to select a single value from an enumerated set of named values.
		// for uint8x1, sint8x1, uint16x1, sint16x1, uint32x1, sint32x1, uint64x1, sint64x1 types only.
		data_view_e_enumeration,

		// provides a combo list that enables the user to check on and off any number of values from an enumerated set of named bit flag values.
		// for uint8x1, sint8x1, uint16x1, sint16x1, uint32x1, sint32x1, uint64x1, sint64x1 types only.
		data_view_e_enumeration_flags,

		// for string8 and string16 types, expects a file path, and provides a button that opens a file picker dialog.
		// asserts when used with other types.
		data_view_e_file_path,

	};

	enum data_stream_mode_e
	{
		data_stream_mode_e_none = 0,
		data_stream_mode_e_read = 0x01,
		data_stream_mode_e_write = 0x02,
	};

}
