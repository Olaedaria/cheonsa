#pragma once

#include "cheonsa_string8.h"

namespace cheonsa
{

	enum data_endianness_e
	{
		data_endianness_e_little,
		data_endianness_e_big,
	};

	// this enum is used with binary property scribe, reflection system, and database system.
	// be warned that modifying these values will break any files that you have created using these values, so if you need to modify it maybe you can do so without changing mappings.
	// the sizes of each type are usually straight forward but not always.
	// for databases, string8 and string16 are 4 bytes long (interpret as uint32_c), as an offset in bytes into the table's string buffer. the length of the string that is referenced can be anything.
	enum data_type_e
	{
		data_type_e_none = 0,

		data_type_e_list = 1, // only used with binary property scribe.
		data_type_e_string8 = 2,
		data_type_e_string16 = 3,
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

	sint32_c data_get_type_size( data_type_e type );

	// used with databases and reflection to define what kind of editor to use for the data.
	enum data_view_e
	{
		// for all data types, single line of plain text.
		data_view_e_default,

		// only for uint8 type.
		data_view_e_boolean,

		// provides a slider in the range of minimum and maximum.
		// only for float32x1 and float64x1 types.
		// asserts when used with other types.
		data_view_e_slider,

		// provides a wheel that the user can use to scrub through values.
		// only for float32x1 and float64x1 types.
		// asserts when used with other types.
		data_view_e_scrubber,

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

		// provides a combo list is used to select from a set of named values.
		// for uint8, sint8, uint16, sint16, uint32, sint32 types only.
		data_view_e_enumeration,

		// for string8 and string16 types, expects a file path, and provides a button that opens a file picker dialog.
		// asserts when used with other types.
		data_view_e_file_path,

		// for string8 and string16 types, expects a folder path, and provides a button that opens a folder picker dialog.
		// asserts when used with other types.
		data_view_e_folder_path,
	};

	enum data_stream_mode_e
	{
		data_stream_mode_e_none = 0,
		data_stream_mode_e_read = 0x01,
		data_stream_mode_e_write = 0x02,
	};

	// gets the native endianess of the cpu.
	data_endianness_e data_get_native_endianness();

}
