"""
during export, the script scans for vertices that are assigned to vertex groups with the following names:
  "_smooth_seam"
    this vertex group is used to average normals and orthogonalize tangents of duplicate vertices (vertices that share the same position) across different meshes.
    for example, if your character model has a head mesh that is separate from the body, you will need to smooth the seam where the head meets the body, so assign the loop of vertices at the edge of the head mesh and the loop of vertices at the edge of the body mesh to this vertex group.
  "_wave_frequency"
    this value gets assigned to exported vertex.texture.c.
    used with waved displaced vertex shaders, of which wind direction and speed variables are fed into.
    generally, if you are modelling foliage, think of it like this:
      lower mass = higher frequency
        smaller leaves will have higher frequency than larger leaves.
  "_wave_amplitude"
    this value gets assigned to exported vertex.texture.d.
    used with waved displaced vertex shaders, of which wind direction and speed variables are fed into.
    generally, if you are modelling foliage, think of it like this:
      higher range of motion = higher amplitude
        the part of the leaf that is in contact with the stem has lower range of motion than the outer edges of the leaf.
"""

bl_info = {
	"name": "Export Cheonsa Model (.chm)",
	"author": "",
	"version": (1, 0, 0),
	"blender": (2, 80, 0),
	"location": "File > Export > Cheonsa Model (.chm)",
	"description": "Export a model in the Cheonsa Model file format (.chm)",
	"warning": "",
	"category": "Import-Export" }

from bpy_extras.io_utils import ExportHelper
import bpy
import mathutils
import os
import time
import math
import struct
import xml.dom.minidom

file_format_version = 1			# private option, fourth byte of file.

"""
same endianness codes used by python's struct pack and unpack, just conveninently listed in an "enum".
cheonsa uses little endian, since we are targeting x86 x64.
"""
class endianness_e:
	native = "@"
	little = "<"
	big = ">"

"""
in-memory stream scribe, used to build the file before writing it to disk.
"""
class memory_stream_c:
	def __init__( this, endianness ):
		this._position = 0
		this._bytes = bytearray()
		this._endianness = endianness
	def get_position( this ):
		return this._position;
	def set_position( this, position ):
		assert position >= 0 and position <= len( this._bytes )
		this._position = position
	def get_size( this ):
		return len( this._bytes )
	def get_bytes( this ):
		return this._bytes
	def load( this, size ): # load bytes straight, does not change byte order.
		result = this._value[this._position:this._position+size]
		this._position += size
		return result
	def load_uint8( this ):
		return struct.unpack( this.endianness + "B", this.load( 1 ) )[0]
	def load_uint8xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "B", this.load( 1 ) )[0]
		return result
	def load_sint8( this ):
		return struct.unpack( this.endianness + "b", this.load( 1 ) )[0]
	def load_sint8xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "b", this.load( 1 ) )[0]
		return result
	def load_uint16( this ):
		return struct.unpack( this.endianness + "H", this.load( 2 ) )[0]
	def load_uint16xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "H", this.load( 2 ) )[0]
		return result
	def load_sint16( this ):
		return struct.unpack( this.endianness + "h", this.load( 2 ) )[0]
	def load_sint16xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "h", this.load( 2 ) )[0]
		return result
	def load_uint32( this ):
		return struct.unpack( this.endianness + "I", this.load( 4 ) )[0]
	def load_uint32xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "I", this.load( 4 ) )[0]
		return result
	def load_sint32( this ):
		return struct.unpack( this.endianness + "i", this.load( 4 ) )[0]
	def load_sint32xN( this, N ):
		result = [0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "i", this.load( 4 ) )[0]
		return result
	def load_float32( this ):
		return struct.unpack( this.endianness + "f", this.load( 4 ) )[0]
	def load_float32xN( this, N ):
		result = [0.0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "f", this.load( 4 ) )[0]
		return result
	def load_float64( this ):
		return struct.unpack( this.endianness + "d", this.load( 8 ) )[0]
	def load_float64xN( this, N ):
		result = [0.0] * N
		for i in range( 0, N ):
			result[i] = struct.unpack( this.endianness + "d", this.load( 8 ) )[0]
		return result
	def load_string8( this ):
		count = this.load_uint16()
		_bytes = this.load( count )
		return _bytes.decode( "utf-8" )
	def load_string16( this ):
		count = this.load_uint16();
		result = ""
		for i in range( 0, count ):
			result += chr( struct.unpack( this.endianness + "H", this.load( 2 ) )[0] )
		return result
	def save( this, _bytes ): # save bytes straight, does not change byte order.
		if this._position == len( this._bytes ):
			this._bytes.extend( _bytes )
			this._position = len( this._bytes )
		else:
			for byte in _bytes:
				if this._position < len( this._bytes ):
					this._bytes[ this._position ] = byte
				else:
					this._bytes.append( byte )
				this._position += 1
	def save_uint8( this, value ):
		this.save( struct.pack( this.endianness + "B", value ) )
	def save_uint8xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "B", i ) )
	def save_sint8( this, value ):
		this.save( struct.pack( this.endianness + "b", value ) )
	def save_sint8xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "b", i ) )
	def save_uint16( this, value ):
		this.save( struct.pack( this.endianness + "H", value ) )
	def save_uint16xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "H", i ) )
	def save_sint16( this, value ):
		this.save( struct.pack( this.endianness + "h", value ) )
	def save_sint16xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "h", i ) )
	def save_uint32( this, value ):
		this.save( struct.pack( this.endianness + "I", value ) )
	def save_uint32xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "I", i ) )
	def save_sint32( this, value ):
		this.save( struct.pack( this.endianness + "i", value ) )
	def save_sint32xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "i", i ) )
	def save_float32( this, value ):
		this.save( struct.pack( this.endianness + "f", value ) )
	def save_float32xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "f", i ) )
	def save_float64( this, value ):
		this.save( struct.pack( this.endianness + "d", value ) )
	def save_float32xN( this, value, N ):
		assert len( value ) == N
		for i in value:
			this.save( struct.pack( this.endianness + "d", i ) )
	def save_string8( this, value ):
		_bytes = value.encode( "utf-8" )
		this.save_uint16( len( _bytes ) )
		this.save( _bytes )
	def save_string16( this, value ):
		this.save_uint16( len( value ) )
		for c in value:
			this.save( struct.pack( this.endianness + "H", ord( c ) ) )
	def save_padding( this, alignment ): # writes padding to stream so that its length is evenly divisble by alignment_multiple. only use this when position is equal to length.
		count = alignment - ( len( this._bytes ) % alignment )
		if count > 0:
			this.save( bytearray( count ) )

"""
used to compile model data to save.
"""
class model_c:
	class chunk_c:
		def __init__( this ):
			this.signature = b"\0\0\0\0"
			this.count = 0
			this.data_offset = 0
			this.data_size = 0
			this.memory_stream = None # should be a memory_stream_c instance. may be longer than data_size if padding is present at end.
	class mesh_c:
		def __init__( this ):
			this.name = ""
			this.draw_list = []
			this.draw_start = 0
			this.draw_end = 0
	class mesh_draw_c:
		class primitive_type_e:
			point_list = 0
			line_list = 1
			line_strip = 2
			triangle_list = 3
			triangle_strip = 4
		def __init__( this ):
			this.lod_index = 0
			this.primitive_type = 0
			this.vertex_list = []
			this.vertex_start = 0
			this.vertex_end = 0
			this.index_list = []
			this.index_start = 0
			this.index_end = 0
			this.flat_vertex_dict = {}
			this.flat_vertex_list = []		# list of all of the flat triangles.
			this.flat_index_list = []
			this.smooth_vertex_dict = {}
			this.smooth_vertex_list = []	# list of all of the smooth triangles. stored separately from flat triangles so that smoothing can be applied without affecting the other vertices.
			this.smooth_index_list = []
		"""
		adds vertices and vertex indices to this mesh draw.
		reuses existing equivalent vertices when possible.
		"""
		def add_vertices( this, vertex_list, smooth ):
			if smooth:
				vertex_dict = this.smooth_vertex_dict
				vertex_list = this.smooth_vertex_list
				index_list = this.smooth_index_list
			else:
				vertex_dict = this.flat_vertex_dict
				vertex_list = this.flat_vertex_list
				index_list = this.flat_index_list
			for vertex in vertex_list:
				if not vertex.position[ 0 ] in vertex_dict:
					vertex_dict[ vertex.position[ 0 ] ] = []
				similar_veretx_list = vertex_dict[ vertex.position[ 0 ] ]
				for similar_vertex in similar_vertex_list:
					if vertex.position == similar_vertex.position and vertex.normal == similar_vertex.normal and vertex.texture == similar_vertex.texture and vertex.bone_weights == similar_vertex.bone_weights and vertex.bone_indices == similar_vertex.bone_indices:
						index_list.append( similar_vertex.index ) # reuse an existing vertex.
						return
				vertex.index = len( vertex_list )
				vertex_dict[ vertex.position[ 0 ] ].append( vertex )
				index_list.append( vertex.index )
				vertex_list.append( vertex )
	"""
	analogous to both mesh_vertex_base_c and mesh_vertex_bone_weight_c in the engine.
	will be split into both of those when saved.
	"""
	class mesh_vertex_c:
		def __init__( this ):
			this.index = 0
			this.position = [ 0.0, 0.0, 0.0 ]
			this.normal = [ 0.0, 0.0, 0.0 ]
			this.normal_u = [ 0.0, 0.0, 0.0 ]
			this.normal_v = [ 0.0, 0.0, 0.0 ]
			this.texture = [ 0.0, 0.0, 0.0, 0.0 ]
			this.bone_weights = [ 0.0, 0.0, 0.0, 0.0 ]
			this.bone_indices = [ 0, 0, 0, 0 ]
	class mesh_shape_key_vertex_c:
		def __init__( this ):
			this.vertex_index = 0
			this.position = [ 0.0, 0.0, 0.0 ]
	class mesh_shape_key_c:
		def __init__( this ):
			this.name = ""
			this.mesh_index = 0
			this.shape_key_vertex_start = 0
			this.shape_key_vertex_end = 0
			this.minimum_value = 0.0
			this.maximum_value = 0.0
			this.shape_key_vertex_list = []
	class bone_c:
		def __init__( this ):
			this.name = ""
			this.mother_bone_name = ""
			this.flags = 0
			this.roll = 0.0
			this.head_position = [ 0.0, 0.0, 0.0 ]
			this.head_radius = 0.0
			this.tail_position = [ 0.0, 0.0, 0.0 ]
			this.tail_radius = 0.0
	class bone_logic_c:
		def __init__( this ):
			this.type = 0
			this.property_start = 0
			this.property_end = 0
			this.property_list = []
	class bone_logic_property_c:
		def __init__( this ):
			this.name = ""
			this.value = ""
	class attachment_point_c:
		def __init__( this ):
			this.name = ""
			this.mother_bone_name = ""
			this.position = [ 0.0, 0.0, 0.0 ]
			this.rotation = [ 0.0, 0.0, 0.0, 1.0 ]
			this.scale = 0.0
	class animation_c:
		def __init__( this ):
			this.name = ""
			this.move_speed = 0.0
			this.time_in = 0.0
			this.time_out = 0.0
			this.object_list = []
			this.object_start = 0
			this.object_end = 0
			this.event_list = []
			this.event_start = 0
			this.event_end = 0
	class animation_object_c:
		def __init__( this ):
			this.name = ""
			this.property_list = []
			this.property_start = 0
			this.property_end = 0
		def add_property( this, type ):	# adds a property or returns an existing one.
			for property in this.property_list:
				if property.type == type:
					return property
			property = model_c.animation_property_c()
			property.type = type
			this.property_list.append( property )
			return property
		def add_key( this, type, time ):	# adds a key or returns an existing one.
			property = this.add_property( type )
			for key in property.key_list:
				if key.time == time:
					return key
			key = property.add_key( time )
			return key
	class animation_property_c:
		class type_e:
			position = 0
			rotation = 1
			scale = 2
		def __init__( this ):
			this.type = 0
			this.key_list = []
			this.key_start = 0
			this.key_end = 0
		def add_key( this, time ): # adds a key to this track at time.
			for key in this.key_list:
				if key.time == time:
					return key
			key = model_c.animation_key_c()
			key.time = time
			this.key_list.append( key )
			return key
	class animation_key_c:
		def __init__( this ):
			this.time = 0.0
			this.value[ 0.0, 0.0, 0.0, 0.0 ]
	class animation_event_c:
		def __init__( this ):
			this.time = 0.0
			this.type = 0
			this.value = 0
	class physics_ridgid_body_c:
		class flags_e:
			is_active = 0x01
			is_dynamic = 0x02
			is_kinematic = 0x04
		class layer_e:
			static = 0x01
			static_no_clip = 0x02
			dynamic = 0x04
			dynamic_no_clip = 0x08
			character = 0x10
			character_no_clip = 0x20
			camera = 0x40
		def __init__( this ):
			this.name = ""
			this.mother_bone_name = ""
			this.position = [0.0, 0.0, 0.0]
			this.rotation = [0.0, 0.0, 0.0, 1.0]
			this.scale = 0.0
			this.mass = 0.0
			this.friction = 1.0
			this.restitution = 0.0
			this.linear_damping = 0.0
			this.angular_damping = 0.0
			this.flags = 0
			this.layer = 0
			this.shape_list = []
			this.shape_start = 0
			this.shape_end = 0
			this.effects_material_name = ""
	class physics_shape_c:
		class type_e:
			none = 0
			sphere = 1
			box = 2
			capsule = 3
			cylinder = 4
			cone = 5
			convex_hull = 6
			triangle_mesh = 7
		def __init__( this ):
			this.type = 0
			this.sphere_radius = 0.0
			this.box_width = 0.0
			this.box_depth = 0.0
			this.box_height = 0.0
			this.capsule_radius = 0.0
			this.capsule_height = 0.0
			this.cylinder_radius = 0.0
			this.cylinder_height = 0.0
			this.cone_radius = 0.0
			this.cone_height = 0.0
			this.convex_hull_vertex_start = 0
			this.convex_hull_vertex_end = 0
			this.convex_hull_vertex_list = [] # each vertex is a 3 element long tuple of floats.
			this.triangle_mesh_vertex_start = 0
			this.triangle_mesh_vertex_end = 0
			this.triangle_mesh_index_start = 0
			this.triangle_mesh_index_end = 0
			this.triangle_mesh_vertex_list = [] # each vertex is a 3 element long tuple of floats.
			this.triangle_mesh_index_list = [] # indices into triangle_mesh_vertex_list.
			this.position = [ 0.0, 0.0, 0.0 ]
			this.rotation = [ 0.0, 0.0, 0.0, 1.0 ]
			this.margin = 0.0
	class physics_constraint_c:
		class type_e:
			none = 0
			fixed = 1
			point = 2
			hinge = 3
			cone = 4
			generic = 5
		class flags_e:
			disable_collision = 0x01
		class parameters_hinge_c:
			def __init__( this ):
				this.angular_lower_a = 0.0
				this.angular_upper_a = 0.0
		class parameters_cone_c:
			def __init__( this ):
				this.angular_upper_a = 0.0
				this.angular_upper_b = 0.0
				this.angular_upper_c = 0.0
		class parameters_generic_c:
			def __init__( this ):
				this.linear_lower_a = 0.0
				this.linear_upper_a = 0.0
				this.linear_lower_b = 0.0
				this.linear_upper_b = 0.0
				this.linear_lower_c = 0.0
				this.linear_upper_c = 0.0
				this.angular_lower_a = 0.0
				this.angular_upper_a = 0.0
				this.angular_lower_b = 0.0
				this.angular_upper_b = 0.0
				this.angular_lower_c = 0.0
				this.angular_upper_c = 0.0
		def __init__( this ):
			this.type = 0						# uint16_c
			this.flags = 0						# uint16_c
			this.parameters = None				# None, parameters_hinge_c, parameters_cone_c, or paramaters_generic_c, depending on constraint type.
			this.parameters_list = []			# list of floats.
			this.parameters_start = 0			# uint16_c
			this.parameters_end = 0				# uint16_c
			this.body_a_index = 0				# sint16_c
			this.body_b_index = 0				# sint16_c
			this.frame_position = [ 0.0, 0.0, 0.0 ]
			this.frame_rotation = [ 0.0, 0.0, 0.0, 1.0 ]
	class light_c:
		class type_e:
			direction = 0
			point = 1
			spot = 2
		def __init__( this ):
			this.name = ""
			this.mother_bone_name = 0
			this.type = 0
			this.position = [ 0.0, 0.0, 0.0 ]
			this.rotation = [ 0.0, 0.0, 0.0, 1.0 ]
			this.color = [ 0.0, 0.0, 0.0 ]
			this.brightness = 0.0
			this.range = 0.0
			this.cone_angle = 0.0
	class property_c:
		def __init__( this ):
			this.name = ""
			this.value = ""
	class string_table_c:
		def __init__( this ):
			this._bytes = b""
			this._string_dict = {} # maps the string values to byte offsets.
			this.add_string( "" ) # add empty string at offset 0.
		def add_string( this, string ): # adds a new string and returns its offset, or returns the offset of an existing equivalent string.
			if string in this._string_dict:
				return this._string_dict[ string ]
			this._string_dict[ string ] = size
			this._bytes += string.encode( "utf-8" )
	def __init__( this ):
		this.mesh_bone_name_list = []		# list of String.
		this.mesh_list = []
		this.mesh_draw_list = []			# compiled from mesh_list.
		this.mesh_vertex_list = []			# compiled from mesh_draw_list. mesh_vertex_c, but gets saved as two lists of two other types (not defined in this file), one for base data and one for bone weight data.
		this.mesh_index_list = []			# compiled from mesh_draw_list. saved format depends on length of vertex list, if less than 65535 then saved as list of uint16_c, otherwise saved as list of uint32_c.
		this.mesh_bone_name_dictionary = {}	# dictionary of str keys and int values.
		this.mesh_shape_key_vertex_list = []# list of mesh_shape_key_vertex_c.
		this.mesh_shape_key_list = []		# list of mesh_shape_key_c.
		this.bone_list = []					# list of bone_c.
		this.bone_logic_list = []			# list of bone_logic_c.
		this.bone_logic_property_list = []	# list of bone_logic_property_c, compiled from bone_logic_list
		this.attachment_point_list = []		# list of attachment_point_c.
		this.animation_list = []			# list of animation_c.
		this.animation_object_list = []		# list of animation_object_c, compiled from animation_list
		this.animation_property_list = []	# list of animation_property_c, compiled from animation_object_list
		this.animation_key_list = []		# list of animation_key_c, compiled from animation_property_list
		this.animation_event_list = []		# list of animation_event_c, compiled from animation_list
		this.physics_body_list = []			# list of physics_ridgid_body_c.
		this.physics_shape_list = []		# list of physics_shape_c, compiled from physics_body_list
		this.physics_vertex_list = []		# list of physics_vertex_c, compiled from physics_shape_list
		this.physics_index_list = []		# list of uint16_c, compiled from physics_shape_list
		this.physics_constraint_list = []	# list of physics_constraint_c.
		this.physics_parameters_list = []	# list of float32_c.
		this.light_list = []				# list of light_c.
		this.property_list = []				# list of property_c.
		this.string_table = string_table_c()# compiled during save from all of the strings defined in all of the other objects.
	def add_mesh_bone_name( this, bone_name ):
		if bone_name in this.mesh_bone_name_dictionary:
			return this.mesh_bone_name_dictionary[ bone_name ]
		else:
			result = len( this.mesh_bone_name_list )
			this.mesh_bone_name_dictionary[ bone_name ] = result
			this.mesh_bone_name_list.append( bone_name )
			return result
	def find_bone_index_with_bone_name( this, bone_name ):
		for i, bone in this.bone_list:
			if bone.name == bone_name:
				return i
		return -1
	"""
	saves data to the provided stream.
	compiles nested lists into flat lists.
	compiles string table.
	"""
	def save( this, file_stream ):
		this.mesh_draw_list = []
		this.mesh_vertex_list = []
		this.mesh_index_list = []
		index_base = 0
		for mesh in this.mesh_list:
			mesh.draw_start = len( this.mesh_draw_list )
			this.mesh_draw_list.extend( mesh.draw_list )
			mesh.draw_end = len( this.mesh_draw_list )
			for mesh_draw in mesh.draw_list:
				mesh_draw.vertex_start = len( this.mesh_vertex_list )
				this.mesh_vertex_list.extend( mesh_draw.vertex_list )
				mesh_draw.vertex_end = len( this.mesh_vertex_list )
				mesh_draw.index_start = len( this.mesh_index_list )
				for index in mesh_draw.index_list:
					this.mesh_index_list.append( index + index_base )
				mesh_draw.index_end = len( this.mesh_index_list )
				index_base += len( mesh_draw.vertex_list )
		for mesh_shape_key in this.mesh_shape_key_list:
			mesh_shape_key.shape_key_vertex_start = len( this.mesh_shape_key_vertex_list )
			this.mesh_shape_key_vertex_list.extend( mesh_shape_key.shape_key_vertex_list )
			mesh_shape_key.shape_key_vertex_end = len( this.mesh_shape_key_vertex_list )
		this.bone_logic_property_list = []
		for bone_logic in this.bone_logic_list:
			bone_logic.property_start = len( this.bone_logic_property_list )
			this.bone_logic_property_list.extend( bone_logic.property_list )
			bone_logic.property_end = len( this.bone_logic_property_list )
		this.animation_object_list = []
		this.animation_property_list = []
		this.animation_key_list = []
		this.animation_event_list = []
		for animation in this.animation_list:
			animation.object_start = len( this.animation_object_list )
			this.animation_object_list.extend( animation.object_list )
			animation.object_end = len( this.animation_object_list )
			animation.event_start = len( this.animation_event_list )
			this.animation_event_list.extend( animation.event_list )
			animation.event_end = len( this.animation_event_list )
			for animation_object in animation.object_list:
				animation_object.property_start = len( this.animation_property_list )
				this.animation_property_list.extend( animation_object.property_list )
				animation_object.property_end = len( this.animation_property_list )
				for animation_property in animation_object.property_list:
					animation_property.key_start = len( this.animation_key_list )
					this.animation_key_list.extend( animation_property.key_list )
					animation_property.key_end = len( this.animation_key_list )
		this.physics_shape_list = []
		this.physics_vertex_list = []
		this.physics_index_list = []
		this.physics_constraint_list = []
		this.physics_parameters_list = []
		index_base = 0
		for physics_body in this.physics_body_list:
			physics_body.shape_start = len( this.physics_shape_list )
			this.physics_shape_list.extend( physics_body.physics_shape_list )
			physics_body.shape_end = len( this.physics_shape_list )
			for physics_shape in physics_body.physics_shape_list:
				if physics_shape.type == physics_shape_type_e.convex_hull:
					physics_shape.convex_hull_vertex_start = len( this.physics_vertex_list )
					this.physics_vertex_list.extend( physics_shape.vertex_list )
					physics_shape.convex_hull_vertex_end = len( this.physics_vertex_list )
				elif physics_shape.type == physics_shape_type_e.triangle_mesh:
					physics_shape.triangle_mesh_vertex_start = len( this.physics_vertex_list )
					this.physics_vertex_list.extend( physics_shape.vertex_list )
					physics_shape.triangle_mesh_vertex_end = len( this.physics_vertex_list )
					physics_shape.triangle_mesh_index_start = len( this.physics_index_list )
					for index in physics_shape.index_list:
						this.physics_index_list.append( index + index_base )
					physics_shape.triangle_mesh_index_end = len( this.physics_index_list )
					index_base += len( physics_shape.index_list )
		for physics_constraint in this.physics_constraint_list:
			if physics_constraint.type == physics_constraint_type_e.hinge:
				assert len( physics_constraint.parameters_list ) == 2
				physics_constraint.parameters_start = len( this.physics_parameters_list )
				this.physics_parameters_list.extend( physics_constraint.parameters_list )
				#this.physics_parameters_list.append( physics_constraint.parameters.angular_lower_a )
				#this.physics_parameters_list.append( physics_constraint.parameters.angular_upper_a )
				#physics_constraint.parameters_end = len( this.physics_parameters_list )
			elif physics_constraint.type == physics_constraint_type_e.cone:
				assert len( physics_constraint.parameters_list ) == 3
				physics_constraint.parameters_start = len( this.physics_parameters_list )
				this.physics_parameters_list.extend( physics_constraint.parameters_list )
				#this.physics_parameters_list.append( physics_constraint.parameters.angular_upper_a )
				#this.physics_parameters_list.append( physics_constraint.parameters.angular_upper_b )
				#this.physics_parameters_list.append( physics_constraint.parameters.angular_upper_c )
				#physics_constraint.parameters_end = len( this.physics_parameters_list )
			elif physics_constraint.type == physics_constraint_type_e.generic:
				assert len( physics_constraint.parameters_list ) == 12
				physics_constraint.parameters_start = len( this.physics_parameters_list )
				this.physics_parameters_list.extend( physics_constraint.parameters_list )
				#this.physics_parameters_list.append( physics_constraint.linear_lower_a )
				#this.physics_parameters_list.append( physics_constraint.linear_upper_a )
				#this.physics_parameters_list.append( physics_constraint.linear_lower_b )
				#this.physics_parameters_list.append( physics_constraint.linear_upper_b )
				#this.physics_parameters_list.append( physics_constraint.linear_lower_c )
				#this.physics_parameters_list.append( physics_constraint.linear_upper_c )
				#this.physics_parameters_list.append( physics_constraint.angular_lower_a )
				#this.physics_parameters_list.append( physics_constraint.angular_upper_a )
				#this.physics_parameters_list.append( physics_constraint.angular_lower_b )
				#this.physics_parameters_list.append( physics_constraint.angular_upper_b )
				#this.physics_parameters_list.append( physics_constraint.angular_lower_c )
				#this.physics_parameters_list.append( physics_constraint.angular_upper_c )
				#physics_constraint.parameters_end = len( this.physics_parameters_list )
		# build chunks.
		# this will also build the string table which is saved last.
		data_endianness = endianness_e.little
		data_list_byte_alignment = 16
		chunk_list = []
		# mesh_bone_name_list
		if len( this.mesh_bone_name_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"mbn_"
			chunk.count = len( this.mesh_bone_name_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_bone_name in this.mesh_bone_name_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( mesh_bone_name ) )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_list
		if len( this.mesh_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"m___"
			chunk.count = len( this.mesh_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh in this.mesh_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( mesh.name ) )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_uint16( mesh.draw_start )
				chunk.memory_stream.save_uint16( mesh.draw_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_draw_list
		if len( this.mesh_draw_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"md__"
			chunk.count = len( this.mesh_draw_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_draw in this.mesh_draw_list:
				chunk.memory_stream.save_uint8( mesh_draw.lod_index )
				chunk.memory_stream.save_uint8( mesh_draw.primitive_type )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_uint32( mesh_draw.vertex_start )
				chunk.memory_stream.save_uint32( mesh_draw.vertex_end )
				chunk.memory_stream.save_uint32( mesh_draw.index_start )
				chunk.memory_stream.save_uint32( mesh_draw.index_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_vertex_list_base
		if len( this.mesh_vertex_list_base ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"mvb_"
			chunk.count = len( this.mesh_vertex_list_base )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_vertex in this.mesh_vertex_list:
				chunk.memory_stream.save_float32( mesh_vertex.position[ 0 ] )
				chunk.memory_stream.save_float32( mesh_vertex.position[ 1 ] )
				chunk.memory_stream.save_float32( mesh_vertex.position[ 2 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal[ 0 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal[ 1 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal[ 2 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_u[ 0 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_u[ 1 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_u[ 2 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_v[ 0 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_v[ 1 ] )
				chunk.memory_stream.save_float32( mesh_vertex.normal_v[ 2 ] )
				chunk.memory_stream.save_float32( mesh_vertex.texture[ 0 ] )
				chunk.memory_stream.save_float32( mesh_vertex.texture[ 1 ] )
				chunk.memory_stream.save_float32( mesh_vertex.texture[ 2 ] )
				chunk.memory_stream.save_float32( mesh_vertex.texture[ 3 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_vertex_list_bone_weight
		if len( this.mesh_bone_name_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"mvbw"
			chunk.count = len( this.mesh_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_vertex in this.mesh_vertex_list:
				chunk.memory_stream.save_uint16( int( mesh_vertex.bone_weights[ 0 ] * 65535.0 ) )
				chunk.memory_stream.save_uint16( int( mesh_vertex.bone_weights[ 1 ] * 65535.0 ) )
				chunk.memory_stream.save_uint16( int( mesh_vertex.bone_weights[ 2 ] * 65535.0 ) )
				chunk.memory_stream.save_uint16( int( mesh_vertex.bone_weights[ 3 ] * 65535.0 ) )
				chunk.memory_stream.save_uint16( mesh_vertex.bone_indices[ 0 ] )
				chunk.memory_stream.save_uint16( mesh_vertex.bone_indices[ 1 ] )
				chunk.memory_stream.save_uint16( mesh_vertex.bone_indices[ 2 ] )
				chunk.memory_stream.save_uint16( mesh_vertex.bone_indices[ 3 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_index_list
		if len( this.mesh_index_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"mi__"
			chunk.count = len( this.mesh_index_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			if len( this.mesh_vertex_list ) <= 65535:
				for mesh_index in this.mesh_index_list:
					chunk.memory_stream.save_uint16( mesh_index )
			else:
				for mesh_index in this.mesh_index_list:
					chunk.memory_stream.save_uint32( mesh_index )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_shape_key_vertex_list
		if len( this.mesh_shape_key_vertex_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"mskv"
			chunk.count = len( this.mesh_shape_key_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_shape_key_vertex in this.mesh_shape_key_vertex_list:
				chunk.memory_stream.save_uint32( mesh_shape_key_vertex.vertex_index )
				chunk.memory_stream.save_float32( mesh_shape_key_vertex.position[ 0 ] )
				chunk.memory_stream.save_float32( mesh_shape_key_vertex.position[ 1 ] )
				chunk.memory_stream.save_float32( mesh_shape_key_vertex.position[ 2 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# mesh_shape_key_list
		if len( this.mesh_shape_key_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"msk_"
			chunk.count = len( this.mesh_shape_key_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for mesh_shape_key in this.mesh_shape_key_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( mesh_shape_key.name ) )
				chunk.memory_stream.save_uint16( mesh_shape_key.mesh_index )
				chunk.memory_stream.save_uint32( mesh_shape_key.shape_key_vertex_start )
				chunk.memory_stream.save_uint32( mesh_shape_key.shape_key_vertex_end )
				chunk.memory_stream.save_float32( mesh_shape_key.minimum_value )
				chunk.memory_stream.save_float32( mesh_shape_key.maximum_value )
		# bone_list
		if len( this.bone_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"b___"
			chunk.count = len( this.bone_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for bone in this.bone_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( bone.name ) )
				chunk.memory_stream.save_sint16( this.string_table.add_string( bone.mother_bone_name ) )
				chunk.memory_stream.save_uint8( bone.flags )
				chunk.memory_stream.save_uint8( 0 )
				chunk.memory_stream.save_uint8( 0 )
				chunk.memory_stream.save_uint8( 0 )
				chunk.memory_stream.save_float32( bone.roll )
				chunk.memory_stream.save_float32( bone.head_position[ 0 ] )
				chunk.memory_stream.save_float32( bone.head_position[ 1 ] )
				chunk.memory_stream.save_float32( bone.head_position[ 2 ] )
				chunk.memory_stream.save_float32( bone.head_radius )
				chunk.memory_stream.save_float32( bone.tail_position[ 0 ] )
				chunk.memory_stream.save_float32( bone.tail_position[ 1 ] )
				chunk.memory_stream.save_float32( bone.tail_position[ 2 ] )
				chunk.memory_stream.save_float32( bone.tail_radius )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# bone_logic_list
		if len( this.bone_logic_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"bl__"
			chunk.count = len( this.bone_logic_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for bone_logic in this.bone_logic_list:
				chunk.memory_stream.save_uint16( bone_logic.type )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_uint16( bone_logic.property_start )
				chunk.memory_stream.save_uint16( bone_logic.property_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# bone_logic_property_list
		if len( this.bone_logic_property_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"blp_"
			chunk.count = len( this.bone_logic_property_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for bone_logic_property in this.bone_logic_property_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( bone_logic_property.key ) )
				chunk.memory_stream.save_uint16( this.string_table.add_string( bone_logic_property.value ) )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# attachment_point_list
		if len( this.attachment_point_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ap__"
			chunk.count = len( this.attachment_point_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for attachment_point in this.attachment_point_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( attachment_point.name ) )
				chunk.memory_stream.save_uint16( this.string_table.add_string( attachment_point.mother_bone_name ) )
				chunk.memory_stream.save_float32( attachment_point.position[ 0 ] )
				chunk.memory_stream.save_float32( attachment_point.position[ 1 ] )
				chunk.memory_stream.save_float32( attachment_point.position[ 2 ] )
				chunk.memory_stream.save_float32( attachment_point.rotation[ 0 ] )
				chunk.memory_stream.save_float32( attachment_point.rotation[ 1 ] )
				chunk.memory_stream.save_float32( attachment_point.rotation[ 2 ] )
				chunk.memory_stream.save_float32( attachment_point.rotation[ 3 ] )
				chunk.memory_stream.save_float32( attachment_point.rotation )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# animation_list
		if len( this.animation_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"a___"
			chunk.count = len( this.animation_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for animation in this.animation_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( animation.name ) )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_float32( animation.move_speed )
				chunk.memory_stream.save_float32( animation.time_in )
				chunk.memory_stream.save_float32( animation.time_out )
				chunk.memory_stream.save_uint32( animation.object_start )
				chunk.memory_stream.save_uint32( animation.object_end )
				chunk.memory_stream.save_uint32( animation.event_start )
				chunk.memory_stream.save_uint32( animation.event_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# animation_object_list
		if len( this.animation_object_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ao__"
			chunk.count = len( this.animation_object_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for animation_object in this.animation_object_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( animation_object.name ) )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_uint32( animation_object.property_start )
				chunk.memory_stream.save_uint32( animation_object.property_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# animation_property_list
		if len( this.animation_property_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ap__"
			chunk.count = len( this.animation_property_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for animation_property in this.animation_property_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( animation_property.name ) )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_uint32( animation_property.key_start )
				chunk.memory_stream.save_uint32( animation_property.key_end )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.appen( chunk )
		# animation_key_list
		if len( this.animation_key_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ak__"
			chunk.count = len( this.animation_key_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for animation_key in this.animation_key_list:
				chunk.memory_stream.save_float32( animation_key.time )
				chunk.memory_stream.save_float32( animation_key.value[ 0 ] )
				chunk.memory_stream.save_float32( animation_key.value[ 1 ] )
				chunk.memory_stream.save_float32( animation_key.value[ 2 ] )
				chunk.memory_stream.save_float32( animation_key.value[ 3 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# animation_event_list
		if len( this.animation_event_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ae__"
			chunk.count = len( this.animation_event_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for animation_event in this.animation_event_list:
				chunk.memory_stream.save_float32( animation_event.time )
				chunk.memory_stream.save_uint16( animation_event.type )
				chunk.memory_stream.save_uint16( animation_event.value )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_body_list
		if len( this.physics_body_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"pb__"
			chunk.count = len( this.physics_body_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_body in this.physics_body_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( physics_body.name ) )
				chunk.memory_stream.save_sint16( this.string_table.add_string( physics_body.mother_bone_name ) )
				chunk.memory_stream.save_float32( physics_body.position[ 0 ] )
				chunk.memory_stream.save_float32( physics_body.position[ 1 ] )
				chunk.memory_stream.save_float32( physics_body.position[ 2 ] )
				chunk.memory_stream.save_float32( physics_body.rotation[ 0 ] )
				chunk.memory_stream.save_float32( physics_body.rotation[ 1 ] )
				chunk.memory_stream.save_float32( physics_body.rotation[ 2 ] )
				chunk.memory_stream.save_float32( physics_body.rotation[ 3 ] )
				chunk.memory_stream.save_float32( physics_body.scale )
				chunk.memory_stream.save_float32( physics_body.mass )
				chunk.memory_stream.save_float32( physics_body.friction )
				chunk.memory_stream.save_float32( physics_body.restitution )
				chunk.memory_stream.save_float32( physics_body.linear_damping )
				chunk.memory_stream.save_float32( physics_body.angular_damping )
				chunk.memory_stream.save_uint16( physics_body.flags )
				chunk.memory_stream.save_uint16( physics_body.layer )
				chunk.memory_stream.save_uint16( physics_body.shape_start )
				chunk.memory_stream.save_uint16( physics_body.shape_end )
				chunk.memory_stream.save_uint16( physics_body.effects_material_name )
				chunk.memory_stream.save_uint16( 0 )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_shape_list
		if len( this.physics_shape_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"ps__"
			chunk.count = len( this.physics_shape_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_shape in this.physics_shape_list:
				chunk.memory_stream.save_uint16( physics_shape.type )
				chunk.memory_stream.save_uint16( 0 )
				if physics_shape.type == physics_shape_type_e.sphere:
					chunk.memory_stream.save_float32( physics_shape.sphere_radius )
					chunk.memory_stream.save_uint32( 0 )
					chunk.memory_stream.save_uint32( 0 )
				elif physics_shape.type == physics_shape_type_e.box:
					chunk.memory_stream.save_float32( physics_shape.box_width )
					chunk.memory_stream.save_float32( physics_shape.box_depth )
					chunk.memory_stream.save_float32( physics_shape.box_height )
				elif physics_shape.type == physics_shape_type_e.capsule:
					chunk.memory_stream.save_float32( physics_shape.capsule_radius )
					chunk.memory_stream.save_float32( physics_shape.capsule_height )
					chunk.memory_stream.save_uint32( 0 )
				elif physics_shape.type == physics_shape_type_e.cylinder:
					chunk.memory_stream.save_float32( physics_shape.cylinder_radius )
					chunk.memory_stream.save_float32( physics_shape.cylinder_height )
					chunk.memory_stream.save_uint32( 0 )
				elif physics_shape.type == physics_shape_type_e.cone:
					chunk.memory_stream.save_float32( physics_shape.cone_radius )
					chunk.memory_stream.save_float32( physics_shape.cone_height )
					chunk.memory_stream.save_uint32( 0 )
				elif physics_shape.type == physics_shape_type_e.convex_hull:
					chunk.memory_stream.save_uint16( physics_shape.vertex_start )
					chunk.memory_stream.save_uint16( physics_shape.vertex_end )
					chunk.memory_stream.save_uint32( 0 )
					chunk.memory_stream.save_uint32( 0 )
				elif physics_shape.type == physics_shape_type_e.triangle_mesh:
					chunk.memory_stream.save_uint16( physics_shape.vertex_start )
					chunk.memory_stream.save_uint16( physics_shape.vertex_end )
					chunk.memory_stream.save_uint16( physics_shape.index_start )
					chunk.memory_stream.save_uint16( physics_shape.index_end )
					chunk.memory_stream.save_uint32( 0 )
				chunk.memory_stream.save_float32( physics_shape.position[ 0 ] )
				chunk.memory_stream.save_float32( physics_shape.position[ 1 ] )
				chunk.memory_stream.save_float32( physics_shape.position[ 2 ] )
				chunk.memory_stream.save_float32( physics_shape.rotation[ 0 ] )
				chunk.memory_stream.save_float32( physics_shape.rotation[ 1 ] )
				chunk.memory_stream.save_float32( physics_shape.rotation[ 2 ] )
				chunk.memory_stream.save_float32( physics_shape.rotation[ 3 ] )
				chunk.memory_stream.save_float32( physics_shape.margin )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_vertex_list
		if len( this.physics_vertex_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"pv__"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_vertex in this.physics_vertex_list:
				chunk.memory_stream.save_float32( physics_vertex.position[ 0 ] )
				chunk.memory_stream.save_float32( physics_vertex.position[ 1 ] )
				chunk.memory_stream.save_float32( physics_vertex.position[ 2 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_index_list
		if len( this.physics_index_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"pi__"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_index in this.physics_index_list:
				chunk.memory_stream.save_uint16( physics_index )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_constraint_list
		if len( this.physics_constraint_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"pc__"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_constraint in this.physics_constraint_list:
				chunk.memory_stream.save_uint16( physics_constraint.type )
				chunk.memory_stream.save_uint16( physics_constraint.flags )
				chunk.memory_stream.save_uint16( physics_constraint.parameters_start )
				chunk.memory_stream.save_uint16( physics_constraint.parameters_end )
				chunk.memory_stream.save_uint16( physics_constraint.body_a_index )
				chunk.memory_stream.save_uint16( physics_constraint.body_b_index )
				chunk.memory_stream.save_float32( physics_constraint.frame_position[ 0 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_position[ 1 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_position[ 2 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_rotation[ 0 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_rotation[ 1 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_rotation[ 2 ] )
				chunk.memory_stream.save_float32( physics_constraint.frame_rotation[ 3 ] )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# physics_parameter_list
		if len( this.physics_parameters_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"pp__"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for physics_parameters in this.physics_parameters_list:
				chunk.memory_stream.save_float32( physics_parameters )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# light_list
		if len( this.light_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"l___"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for light in this.light_list:
				chunk.memory_stream.save_uint16( light.string_table.add_string( light.name ) )
				chunk.memory_stream.save_uint16( light.string_table.add_string( light.mother_bone_name ) )
				chunk.memory_stream.save_uint16( light.type )
				chunk.memory_stream.save_uint16( 0 )
				chunk.memory_stream.save_float32( light.position[ 0 ] )
				chunk.memory_stream.save_float32( light.position[ 1 ] )
				chunk.memory_stream.save_float32( light.position[ 2 ] )
				chunk.memory_stream.save_float32( light.rotation[ 0 ] )
				chunk.memory_stream.save_float32( light.rotation[ 1 ] )
				chunk.memory_stream.save_float32( light.rotation[ 2 ] )
				chunk.memory_stream.save_float32( light.rotation[ 3 ] )
				chunk.memory_stream.save_float32( light.color[ 0 ] )
				chunk.memory_stream.save_float32( light.color[ 1 ] )
				chunk.memory_stream.save_float32( light.color[ 2 ] )
				chunk.memory_stream.save_float32( light.brightness )
				chunk.memory_stream.save_float32( light.range )
				chunk.memory_stream.save_float32( light.cone_angle )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# property_list
		if len( this.property_list ) > 0:
			chunk = model_c.chunk_c()
			chunk.signature = b"prop"
			chunk.count = len( this.physics_vertex_list )
			chunk.memory_stream = memory_stream_c( data_endianness )
			for thing in this.property_list:
				chunk.memory_stream.save_uint16( this.string_table.add_string( thing.name ) )
				chunk.memory_stream.save_uint16( this.string_table.add_string( thing.value ) )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.append( chunk )
		# string_table
		if True:
			chunk = model_c.chunk_c()
			chunk.signature = b"st__"
			chunk.count = 0
			chunk.memory_stream = memory_stream_c( data_endianness )
			chunk.memory_stream.save( this.string_table._bytes )
			chunk.data_size = chunk.memory_stream.get_size()
			chunk.memory_stream.save_padding( data_list_byte_alignment )
			chunk_list.insert( 0, chunk )

		# save header.
		chunk_data_offset = ( 8 ) + ( len( chunk_list ) * 16 ) + ( data_list_byte_alignment - ( chunk_data_offset % data_list_byte_alignment ) )
		header_memory_stream = memory_stream_c( data_endianness )
		header_memory_stream.save( b"cm" if data_endianness == endianness_e.little else b"CM" )
		header_memory_stream.save( b"01" )
		header_memory_stream.save_sint32( len( chunk_list ) )
		for chunk in chunk_list:
			assert( len( chunk.signature ) == 4 )
			header_memory_stream.save( chunk.signature )
			header_memory_stream.save_sint32( chunk.count )
			header_memory_stream.save_sint32( chunk_data_offset )
			header_memory_stream.save_sint32( chunk.data_size )
			chunk_data_offset += chunk.memory_stream.get_size()
		header_memory_stream.save_padding( data_list_byte_alignment )

		# save to disk file.
		file_stream.save( header_memory_stream._bytes )
		for chunk in chunk_list:
			file_stream.save( chunk.memory_stream._bytes )


"""
performs the export with the blender context's currently selected object(s).
finds the root (most parent) object of the primary selected object.
if the root object has children, then the root object and all related descendants will be considered for export.
if the root object has no children, then the context's currently selected object(s) will be considered for export.
of the objects that are being considered for export, the first one that is an 'ARMATURE' type will have its bones exported. at most only one armature will be exported.
scans for a text block of same name as root object plus ".exports.xml" (case sensitive), which will define additional export data and settings.
"""
def do_export( context, file_path ):
	def vector2_minus_vector2( a, b ):
		return [ a[ 0 ] - b[ 0 ], a[ 1 ] - b[ 1 ] ]
	def vector3_plus_vector3( a, b ):
		return [ a[ 0 ] + b[ 0 ], a[ 1 ] + b[ 1 ], a[ 2 ] + b[ 2 ] ]
	def vector3_minus_vector3( a, b ):
		return [ a[ 0 ] - b[ 0 ], a[ 1 ] - b[ 1 ], a[ 2 ] - b[ 2 ] ]
	def vector3_times_vector3( a, b ):
		return [ a[ 0 ] * b[ 0 ], a[ 1 ] * b[ 1 ], a[ 2 ] * b[ 2 ] ]
	def vector3_times_scalar( a, b ):
		return [ a[ 0 ] * b, a[ 1 ] * b, a[ 2 ] * b ]
	def vector3_normalize( a ):
		length = math.sqrt( ( a[ 0 ] * a[ 0 ] ) + ( a[ 1 ] * a[ 1 ] ) + ( a[ 2 ] * a[ 2 ] ) )
		if length == 0.0:
			return [ 0.0, 0.0, 0.0 ]
		return [ a[ 0 ] / length, a[ 1 ] / length, a[ 2 ] / length ]
	def vector3_cross_product( a, b ):
		return [ ( a[ 1 ] * b[ 2 ] ) - ( a[ 2 ] * b[ 1 ] ), ( a[ 2 ] * b[ 0 ] ) - ( a[ 0 ] * b[ 2 ] ), ( a[ 0 ] * b[ 1 ] ) - ( a[ 1 ] * b[ 0 ] ) ]
	def vector3_dot_product( a, b ):
		return ( a[ 0 ] * b[ 0 ] ) + ( a[ 1 ] * b[ 1 ] ) + ( a[ 2 ] * b[ 2 ] )
	"""
	finds and returns a vector that is most similar to b that is also tangent to a.
	the result may not be normalized so you may need to renormalize it if needed.
	"""
	def vector3_orthogonalize( a, b ):
		return vector3_minus_vector3( b, vector3_times_scalar( a, vector3_dot_product( a, b ) ) )
	"""
	averages normals (and tangents) of vertices that share the same position.
	for duplicate vertices that share the exact same normal, we want this normal to contribute only once, because it belongs conceptually to the same vertex.
	this is so that if we are smoothing seams across meshes A and B, it's possible that the conceptually single vertex in mesh A is made of up 3 different vertices (due to texture UV seams or whatever), while the conceptually single vertex in mesh B is just 1 vertex, and we don't want the vertex in mesh A to contribute 3 to 1.
	"""
	def smooth_duplicate_vertices( vertex_list_in ):
		similar_vertex_lookup = {}
		for vertex in vertex_list_in:
			vertex.is_smoothed_normal = False
			if not vertex.position[ 0 ] in similar_vertex_lookup:
				similar_vertex_lookup[ vertex.position[ 0 ] ] = [ vertex ]
			else:
				similar_vertex_lookup[ vertex.position[ 0 ] ].append( vertex )
		for vertex in vertex_list:
			if vertex.is_smoothed_normal == False:
				duplicate_vertex_list = [] # holds vertices that share the same position coordinates.
				for similar_vertex in similar_vertex_lookup[ vertex.position[ 0 ] ]:
					if similar_vertex.position == vertex.position:
						duplicate_vertex_list.append( similar_vertex )
				contributed_normal_list = [] # to prevent summing the same normal more than once.
				smooth_normal = [ 0.0, 0.0, 0.0 ]
				for duplicate_vertex in duplicate_vertex_list:
					if not duplicate_vertex.normal in contributed_normal_list:
						contributed_normal_list.append( duplicate_vertex.normal )
						smooth_normal = vector3_plus_vector3( smooth_normal, duplicate_vertex.normal )
				smooth_normal = vector3_normalize( smooth_normal )
				for duplicate_vertex in duplicate_vertex_list:
					duplicate_vertex.is_smoothed_normal = True
					duplicate_vertex.normal = smooth_normal
					duplicate_vertex.normal_u = vector3_normalize( vector3_orthogonalize( duplicate_vertex.normal, normal_u ) ) # make tangent orthogonal to normal.
					duplicate_vertex.normal_v = vector3_normalize( vector3_orthogonalize( duplicate_vertex.normal, normal_v ) ) # make tangent orthogonal to normal.
	"""
	calculates tangent vectors (normal_u and normal_v) for an input index list that is formatted as a triangle list.
	"""
	def make_tangents_for_triangle_list( vertex_list, index_list ):
		triangle_count = int( len( index_list ) / 3 )
		for vertex in vertex_list:
			vertex.normal_u = [ 0.0, 0.0, 0.0 ]
			vertex.normal_v = [ 0.0, 0.0, 0.0 ]
		for triangle_index in range( 0, triangle_count ):
			# get the vertices that make up this triangle
			a = vertex_list[ index_list[ triangle_index * 3 ] ]
			b = vertex_list[ index_list[ triangle_index * 3 + 1 ] ]
			c = vertex_list[ index_list[ triangle_index * 3 + 2 ] ]
			# the tangent is in the +U direction of the texture coordinates
			delta_position_ab = vector3_minus_vector3( b.position, a.position )
			delta_position_ac = vector3_minus_vector3( c.position, a.position )
			delta_texture_ab = vector2_minus_vector2( b.texture, a.texture )
			delta_texture_ac = vector2_minus_vector2( c.texture, a.texture )
			normal_u = vector3_normalize( vector3_minus_vector3( vector3_times_scalar( delta_position_ab, delta_texture_ac[ 1 ] ), vector3_times_scalar( delta_position_ac, delta_texture_ab[ 1 ] ) ) );
			normal_v = vector3_normalize( vector3_minus_vector3( vector3_times_scalar( delta_position_ac, delta_texture_ab[ 0 ] ), vector3_times_scalar( delta_position_ab, delta_texture_ac[ 0 ] ) ) );
			if vector3_dot_product( vector3_cross_product( normal, normal_u ), normal_v ) < 0.0:
				normal_u[ 0 ] = -normal_u[ 0 ]
				normal_u[ 1 ] = -normal_u[ 1 ]
				normal_u[ 2 ] = -normal_u[ 2 ]
			# accumulate the face's tangent space with each vertex.
			vertex.normal_u = vector3_plus_vector3( vertex.normal_u, normal_u )
			vertex.normal_v = vector3_plus_vector3( vertex.normal_v, normal_v )
		# orthogonalize (with vertex normal) and normalize the tangents in each vertex.
		for vertex in vertex_list:
			vertex.normal_u = vector3_normalize( vector3_orthogonalize( vertex.normal, vertex.normal_u ) )
			vertex.normal_v = vector3_normalize( vector3_orthogonalize( vertex.normal, vertex.normal_v ) )
	"""
	creates a copy of input mesh, triangulates it, and returns it, you'll have to delete the copy with bpy.ops.object.delete() when you're done otherwise it will stay in the scene.
	"""
	def triangulate_blender_mesh( context, blender_object ):
		# duplicate the mesh.
		bpy.ops.object.select_all( action = 'DESELECT' )
		blender_object.select = True
		context.scene.objects.active = blender_object
		bpy.ops.object.duplicate( linked = False, mode = 'INIT' )
		blender_object_copy = context.scene.objects.active
		# apply the relevant modifiers that may be present, since it's not like cheonsa implements analogs to these.
		modifier_index = 0
		while modifier_index < len( blender_object_copy.modifiers ):
			blender_modifier = blender_object_copy.modifiers[ modifier_index ]
			if blender_modifier.type == 'MIRROR':
				bpy.ops.object.modifier_apply( apply_as = 'DATA', modifier = blender_modifier.name )
				modifier_index -= 1
			elif blender_modifier.type == 'SUBSURF':
				if blender_modifier.levels > 0:
					bpy.ops.object.modifier_apply( apply_as = 'DATA', modifier = blender_modifier.name )
					modifier_index -= 1
			elif blender_modifier.type == 'SOLIDIFY':
				bpy.ops.object.modifier_apply( apply_as = 'DATA', modifier = blender_modifier.name )
				modifier_index -= 1
			modifier_index += 1
		# triangulate the mesh copy.
		bpy.ops.object.mode_set( mode = 'EDIT' )
		bpy.ops.mesh.select_all( action = 'SELECT' )
		bpy.ops.mesh.quads_convert_to_tris()
		context.scene.update()
		bpy.ops.object.mode_set( mode = 'OBJECT' )
		# i'm not sure.
		#blender_mesh_copy.data = blender_mesh_copy.to_mesh( bpy.context.scene, False, 'PREVIEW' ) #write data object.
		#bpy.context.scene.update()
		return blender_object_copy
	"""
	used with list sort to sort a list of vertex influences from heaviest to lightest.
	"""
	def sort_vertex_influences( a ):
		return -a[ 1 ]
	"""
	returns all of the text before the last period (if any) in a blender name.
	there cases where you may want to give different objects in your model the same name.
	blender does not allow for this but cheonsa does, so this function gets around this.
	"""
	def trim_blender_name( blender_name ):
		if ( blender_name.find( "." ) != -1 ):
			return blender_name.rpartition( "." )[ 0 ]
		return blender_name
	"""
	recursively appends to a flat list the input blender_object and all children.
	"""
	def compile_blender_object_list( blender_object, result ):
		result.append( blender_object )
		for blender_object_child in blender_object.children:
			compile_blender_object_list( blender_object_child, result )
	"""
	gets the local object space min and max bounding box tuples for a given blender object.
	"""
	def extract_blender_bound_box( blender_object, min, max ):
		# looks like blender bound box is 8 points, one for each corner of a box.
		# looks like point 0 is the minimum bounds, and point 6 is the maximum bounds.
		min = ( blender_object.bound_box[ 0 ][ 0 ], blender_object.bound_box[ 0 ][ 1 ], blender_object.bound_box[ 0 ][ 2 ] )
		max = ( blender_object.bound_box[ 6 ][ 0 ], blender_object.bound_box[ 6 ][ 1 ], blender_object.bound_box[ 6 ][ 2 ] )
	"""
	holds an animation to export, extracted from the xml.
	"""
	class export_animation_c:
		def __init__( this ):
			this.name = ""
			this.move_speed = 0.0 # used by locomotive animations, tells the game how fast the character should move if the animation plays at 1x speed, so that feet don't slide too much. units can be in meters or radians, but it depends on how the game is implemented to interpret this value.
			this.frame_start = 0.0
			this.frame_end = 0.0
			this.export_time_scale = 1.0 # this property is not saved to the file, but is used during export to modify the times of the keyframes.
			this.export_event_list = [] # collection of export_event_c instances.
	"""
	holds an animation event to export, extracted from the xml.
	"""
	class export_event_c:
		def __init__( this ):
			this.frame = 0.0
			this.type = ""
			this.value = ""

	# status report.
	print( "cmdl export: \"" + file_path + "\"" )

	# export from current selection.
	if len( context.selected_objects ) == 0:
		print( "error: no selection" )
		return

	# find selection of objects to export.
	blender_object_list = [] # all of the blender objects that we will be considering for export.
	blender_object_root = context.active_object # find root of selection.
	while blender_object_root.parent != None:
		blender_object_root = blender_object_root.parent
	if len( blender_object_root.children ) == 0 and len( context.selected_objects ) > 1: # if the root has no children and there is more than one object selected, then we will export the selected objects.
		blender_object_root = None
		blender_object_list = context.selected_objects
	else: # if the root has children, then we will export the root and it's children.
		compile_blender_object_list( blender_object_root, blender_object_list )

	# find armature of model if any.
	blender_armature = None
	if blender_object_root != None:
		if blender_object_root.type == 'ARMATURE':
			blender_armature = blender_object_root
		else:
			for blender_object_child in blender_object_root.children:
				if blender_object_child.type == 'ARMATURE':
					blender_armature = blender_object_child
					break

	# preliminary result.
	# we will compile data for export in this object.
	cheonsa_model = model_c()

	# find the XML file that contains more information.
	# use this to build list of animations to export, as well as bone logics and properties.
	export_animation_list = [] # list of export_animation_c, which we will use later to export animations (actions) from the blend.
	if blender_object_root != None:
		text_block_name = blender_object_root.name + "exports.xml"
		if text_block_name in bpy.data.texts:
			xml_document = xml.dom.minidom.parseString( bpy.data.texts[ text_block_name ].as_string() )
			if xml_document != None:
				if xml_document.documentElement != None and xml_document.documentElement.tagName == "export":
					# rebound_bones.
					rebound_bones = xml_document.documentElement.getAttribute( "rebound_bones" )
					# property.
					xml_property_elements = xml_document.documentElement.getElementsByTagName( "property" )
					for xml_property_element in xml_property_elements:
						cheonsa_model_property = model_c.property_c()
						cheonsa_model_property.name = xml_property_element.getAttribute( "name" )
						cheonsa_model_property.value = xml_property_element.getAttribute( "value" )
						cheonsa_model.property_list.append( cheonsa_model_property )
					# animation.
					xml_animation_elements = xml_document.documentElement.getElementsByTagName( "animation" )
					for xml_animation_element in xml_animation_elements:
						export_animation = export_animation_c()
						export_animation.name = xml_animation_element.getAttribute( "name" )
						export_animation.frame_start = float( xml_animation_element.getAttribute( "frame_start" ) )
						export_animation.frame_end = float( xml_animation_element.getAttribute( "frame_end" ) )
						attribute = xml_animation_element.getAttribute( "move_speed" )
						if attribute != "":
							export_animation.move_speed = float( attribute )
						attribute = xml_animation_element.getAttribute( "export_time_scale" )
						if attribute != "":
							export_animation.export_time_scale = float( attribute )
						xml_event_elements = xml_animation_element.getElementsByTagName( "event" )
						for xml_event_element in xml_event_elements:
							export_event = export_event_c()
							export_event.frame = float( xml_event_element.getAttribute( "frame" ) )
							export_event.type = xml_event_element.getAttribute( "type" )
							export_event.value = xml_event_element.getAttribute( "value" )
							export_animation.export_event_list.append( export_event )
						export_animation_list.append( export_animation )
					# bone_logic.
					xml_bone_logic_elements = xml_document.documentElement.getElementsByTagName( "bone_logic" )
					for xml_bone_logic_element in xml_bone_logic_elements:
						cheonsa_model_bone_logic_root = model_c.bone_logic_c()
						for xml_attribute_index, xml_attribute in enumerate( xml_bone_logic_element.attributes ):
							if xml_attribute.name == "type":
								cheonsa_model_bone_logic_root.type = xml_attribute.value
							elif xml_attribute.name == "bone_names":
								cheonsa_model_bone_logic_root.bone_names = xml_attribute.value
							else:
								cheonsa_model_bone_logic_property = model_c.bone_logic_property_c()
								cheonsa_model_bone_logic_property.name = xml_attribute.name
								cheonsa_model_bone_logic_property.value = xml_attribute.value
								cheonsa_model_bone_logic_root.property_list.append( cheonsa_model_bone_logic_property )
						for bone_name in bone_logic_root.bone_names.split( ',' ):
							cheonsa_model_bone_logic = model_c.bone_logic_c()
							cheonsa_model_bone_logic.type = bone_logic_root.type
							cheonsa_model_bone_logic.bone_name = bone_name.strip()
							cheonsa_model_bone_logic.property_list = bone_logic_root.property_list
							cheonsa_model.bone_logic_list.append( cheonsa_model_bone_logic )

	# meshes.
	printed = False
	smooth_seam_vertex_list = [] # list of model_c.mesh_vertex_c, with vertex influence from vertex group "_smooth_seam" with weight >= 1. these are vertices that lie on the edges of meshes that are supposed to connect together and have their normals and tangents averaged. for example, the ring of vertices around the base of the head mesh and the top of the neck (body) mesh.
	for blender_object in blender_object_list:
		if blender_object.type != 'MESH' or blender_object.hide_render == True:
			continue;
		blender_bone_dictionary = {}
		clean_vertex_groups = ( blender_armature != None ) and ( "_clean" in blender_object.vertex_groups )
		if clean_vertex_groups:
			bone_index = 0
			for blender_bone in blender_armature.data.bones:
				blender_bone_dictionary[ blender_bone.name ] = bone_index
				bone_index += 1
		if printed == False:
			print( "  building meshes" )
			printed = True
		cheonsa_mesh = model_c.mesh_c()
		cheonsa_mesh.name = trim_blender_name( blender_object.name )
		#cheonsa_mesh.mother_bone_name = blender_object.parent_bone
		#cheonsa_mesh.position[ 0 ] = blender_object.location.x
		#cheonsa_mesh.position[ 1 ] = blender_object.location.y
		#cheonsa_mesh.position[ 2 ] = blender_object.location.z
		#cheonsa_mesh.rotation[ 0 ] = blender_object.rotation_quaternion.x
		#cheonsa_mesh.rotation[ 1 ] = blender_object.rotation_quaternion.y
		#cheonsa_mesh.rotation[ 2 ] = blender_object.rotation_quaternion.z
		#cheonsa_mesh.rotation[ 3 ] = blender_object.rotation_quaternion.w
		#if blender_object.scale.x != blender_object.scale.y or blender_object.scale.x != blender_object.scale.z:
		#	print( "    warning: a mesh object has non-uniform scale, but cheonsa meshes only support uniform scale. the mesh will still be exported but using the x scale to define the uniform scale." )
		#cheonsa_mesh.scale = blender_object.scale.x
		blender_mesh = triangulate_blender_mesh( context, blender_object ) # create a triangulated copy of the mesh
		blender_active_uv = blender_mesh.data.uv_layers.active
		#blender_active_uv_b = None
		#blender_active_rgb = blender_mesh.data.vertex_colors.active
		cheonsa_model_mesh_draw = model_c.mesh_draw_c()
		cheonsa_model_mesh_draw.lod_index = 0
		cheonsa_model_mesh_draw.primitive_type = primitive_type_e.triangle_list
		for blender_polygon in blender_mesh.data.polygons:
			cheonsa_mesh_vertex_list_for_polygon = []
			if len( blender_polygon.loop_indices ) == 3:
				for blender_loop_index in blender_polygon.loop_indices:
					blender_loop = blender_mesh.data.loops[ blender_loop_index ]
					blender_mesh_vertex = blender_mesh.data.vertices[ blender_loop.vertex_index ] # look up the vertex with the vertex index from the loop
					cheonsa_mesh_vertex = model_c.mesh_vertex_c()
					cheonsa_mesh_vertex.position[ 0 ] = blender_mesh_vertex.co.x # extract vertex position XYZs
					cheonsa_mesh_vertex.position[ 1 ] = blender_mesh_vertex.co.y
					cheonsa_mesh_vertex.position[ 2 ] = blender_mesh_vertex.co.z
					if blender_polygon.use_smooth: # face is smooth, take normal from vertex.
						cheonsa_mesh_vertex.normal[ 0 ] = blender_mesh_vertex.normal.x
						cheonsa_mesh_vertex.normal[ 1 ] = blender_mesh_vertex.normal.y
						cheonsa_mesh_vertex.normal[ 2 ] = blender_mesh_vertex.normal.z
					else: # face is flat, take normal from face.
						cheonsa_mesh_vertex.normal[ 0 ] = blender_polygon.normal.x
						cheonsa_mesh_vertex.normal[ 1 ] = blender_polygon.normal.y
						cheonsa_mesh_vertex.normal[ 2 ] = blender_polygon.normal.z
					if blender_active_uv != None:
						cheonsa_mesh_vertex.texture[ 0 ] = blender_active_uv.data[ blender_loop_index ].uv.x
						cheonsa_mesh_vertex.texture[ 1 ] = 1.0 - blender_active_uv.data[ blender_loop_index ].uv.y # ivert Y|V axis to work with Direct3D texture addressing.
					#if blender_active_uv_b != None:
					#	cheonsa_mesh_vertex.texture[ 2 ] = blender_active_uv_b.data[ blender_loop_index ].uv[ 0 ]
					#	cheonsa_mesh_vertex.texture[ 3 ] = 1.0 - blender_active_uv_b.data[ blender_loop_index ].uv[ 1 ] # ivert V axis to work with Direct3D texture addressing.
					#if blender_active_rgb != None:	# vertex colors
					#	blender_rgb = blender_active_rgb.data[ blender_loop_index ].color
					#	cheonsa_mesh_vertex.color = [ int( blender_rgb[ 0 ] * 0xFF ), int( blender_rgb[ 1 ] * 0xFF ), int( blender_rgb[ 2 ] * 0xFF ), 0xFF ]
					#else:
					#	cheonsa_mesh_vertex.color = [ 0xFF, 0xFF, 0xFF, 0xFF ]
					vertex_influence_list = [] # compile vertex groups
					for blender_vertex_group_element in blender_mesh_vertex.groups:	# gather all of the influences on this vertex.
						group_name = blender_mesh.vertex_groups[ blender_vertex_group_element.group ].name
						group_weight = blender_vertex_group_element.weight
						if group_name == "_smooth_seam" and group_weight >= 1:
							smooth_seam_vertex_list.append( cheonsa_mesh_vertex )
						elif group_name == "_wave_frequency":
							cheonsa_mesh_vertex.texture[ 2 ] = group_weight * 1000.0
						elif group_name == "_wave_amplitude":
							cheonsa_mesh_vertex.texture[ 3 ] = group_weight
						elif group_weight > 0.0:
							vertex_influence_list.append( [ group_name, group_weight ] )
					vertex_influence_list.sort( key = sort_vertex_influences ) # sort influences by weight from heaviest to lightest.
					if clean_vertex_groups: # clean vertex groups, which means to remove vertex influences that reference bones that don't exist in the armature.
						cheonsa_vertex_influence_index = 0
						while cheonsa_vertex_influence_index < len( vertex_influence_list ):
							cheonsa_vertex_influence = vertex_influence_list[ cheonsa_vertex_influence_index ]
							if not cheonsa_vertex_influence[ 0 ] in blender_bone_dictionary:
								del vertex_influence_list[ cheonsa_vertex_influence_index ]
							else:
								cheonsa_vertex_influence_index += 1
					vertex_influence_list = vertex_influence_list[ :4 ] # trim to be no more than 4 weights, since cheonsa only supports up to 4 influences per vertex.
					for cheonsa_vertex_influence_index, cheonsa_vertex_influence in enumerate( vertex_influence_list ):	# add bone name lookups to model.
							cheonsa_mesh_vertex.bone_weights[ cheonsa_vertex_influence_index ] = cheonsa_vertex_influence[ 1 ]
							cheonsa_mesh_vertex.bone_indices[ cheonsa_vertex_influence_index ] = model.add_mesh_bone_name( cheonsa_vertex_influence[ 0 ] )
					weight_sum = cheonsa_mesh_vertex.bone_weights[ 0 ] + cheonsa_mesh_vertex.bone_weights[ 1 ] + cheonsa_mesh_vertex.bone_weights[ 2 ] + cheonsa_mesh_vertex.bone_weights[ 3 ]	# normalize weights.
					if weight_sum > 0.0:
						cheonsa_mesh_vertex.bone_weights[ 0 ] = cheonsa_mesh_vertex.bone_weights[ 0 ] / weight_sum
						cheonsa_mesh_vertex.bone_weights[ 1 ] = cheonsa_mesh_vertex.bone_weights[ 1 ] / weight_sum
						cheonsa_mesh_vertex.bone_weights[ 2 ] = cheonsa_mesh_vertex.bone_weights[ 2 ] / weight_sum
						cheonsa_mesh_vertex.bone_weights[ 3 ] = cheonsa_mesh_vertex.bone_weights[ 3 ] / weight_sum
					else:
						cheonsa_mesh_vertex.bone_weights[ 0 ] = 0
						cheonsa_mesh_vertex.bone_weights[ 1 ] = 0
						cheonsa_mesh_vertex.bone_weights[ 2 ] = 0
						cheonsa_mesh_vertex.bone_weights[ 3 ] = 0
					cheonsa_mesh_vertex_list_for_polygon.append( cheonsa_mesh_vertex ) # add the vertex to the polygon vertex list.
				cheonsa_model_mesh_draw.add_vertices( cheonsa_mesh_vertex_list_for_polygon, blender_polygon.use_smooth ) # add the polygon (triangle) vertex list to the draw.
			cheonsa_mesh.draw_list.append( cheonsa_model_mesh_draw ) # add the draw to the mesh.
		for cheonsa_model_mesh_draw in cheonsa_mesh.draw_list: # build tangents.
			if cheonsa_model_mesh_draw.primitive_type == model_c.primitive_type_e.triangle_list:
				make_tangents_for_triangle_list( cheonsa_model_mesh_draw.smooth_vertex_list )
				make_tangents_for_triangle_list( cheonsa_model_mesh_draw.flat_vertex_list )
		mesh_index = len( cheonsa_model.mesh_list )
		cheonsa_model.mesh_list.append( cheonsa_mesh ) # add the new mesh to the model.
		# mesh shape keys.
		if blender_mesh.data.shape_keys != None:
			if len( blender_mesh.data.shape_keys.key_blocks ) > 1: # the first shape key is always the Basis and we aren't interested in it.
				for i in range( 1, len( blender_mesh.data.shape_keys.key_blocks ) ):
					blender_mesh_shape_key = blender_mesh.data.shape_keys.key_blocks[ i ]
					cheonsa_mesh_shape_key = model_c.mesh_shape_key_c()
					cheonsa_mesh_shape_key.name = blender_mesh_shape_key.name
					cheonsa_mesh_shape_key.mesh_index = mesh_index
					cheonsa_mesh_shape_key.minimum_value = blender_mesh_shape_key.slider_min
					cheonsa_mesh_shape_key.maximum_value = blender_mesh_shape_key.slider_max
					for j in range( 0, len( blender_mesh_shape_key.data ) ):
						blender_mesh_vertex = blender_mesh.data.vertices[ j ]
						blender_mesh_shape_key_vertex = blender_mesh_shape_key.data[ j ]
						if blender_mesh_vertex.co != blender_mesh_shape_key_vertex.co:
							cheonsa_mesh_shape_key_vertex = model_c.mesh_shape_key_vertex_c()
							cheonsa_mesh_shape_key_vertex.vertex_index = j
							cheonsa_mesh_shape_key_vertex.position[ 0 ] = blender_mesh_shape_key_vertex.co.x - blender_mesh_vertex.co.x
							cheonsa_mesh_shape_key_vertex.position[ 1 ] = blender_mesh_shape_key_vertex.co.y - blender_mesh_vertex.co.y
							cheonsa_mesh_shape_key_vertex.position[ 2 ] = blender_mesh_shape_key_vertex.co.z - blender_mesh_vertex.co.z
							cheonsa_mesh_shape_key.shape_key_vertex_list.append( cheonsa_mesh_shape_key_vertex )
		bpy.ops.object.delete()	# delete the duplicated mesh that is currently the active selected object.
	smooth_duplicate_vertices( smooth_seam_vertex_list ) # smooth seams between meshes that are supposed to connect together seamlessly.

	# bones.
	if blender_armature != None:
		print( "  building bones" )
		bpy.ops.object.select_all( action = 'DESELECT' )				# deselect all.
		blender_armature.select = True									# select armature.
		context.scene.objects.active = blender_armature					# set armature as active selection.
		original_pose_position = blender_armature.data.pose_position	# save pose position so we can restore it later.
		blender_armature.data.pose_position = 'REST'					# enter rest pose so we don't get pose coordinates.
		context.scene.update()											# apply armature change.
		bpy.ops.object.mode_set( mode = 'EDIT', toggle = False )		# enter edit mode on the armature.
		for blender_edit_bone in blender_armature.data.edit_bones:
			if blender_edit_bone.name == "":
				print( "    error: a bone in the armature has empty an name. all bones in the armature need to have names." )
				return
			cheonsa_model_bone = model_c.bone_c()
			cheonsa_model_bone.name = blender_edit_bone.name
			if blender_edit_bone.parent != None:
				cheonsa_model_bone.mother_bone_name = blender_edit_bone.parent.name
			cheonsa_model_bone.flags = 0x01 # enable inherit position.
			if not blender_edit_bone.use_inherit_rotation:
				cheonsa_model_bone.flags |= 0x02 # enable inherit rotation.
			if not blender_edit_bone.use_inherit_scale:
				cheonsa_model_bone.flags |= 0x04 # enable inherit scale.
			if blender_edit_bone.name in rebound_bones:
				cheonsa_model_bone.flags |= 0x08 # enable rebound.
			cheonsa_model_bone.roll = blender_edit_bone.roll
			cheonsa_model_bone.head_position[ 0 ] = blender_edit_bone.head.x
			cheonsa_model_bone.head_position[ 1 ] = blender_edit_bone.head.y
			cheonsa_model_bone.head_position[ 2 ] = blender_edit_bone.head.z
			cheonsa_model_bone.head_radius = blender_edit_bone.head_radius
			cheonsa_model_bone.tail_position[ 0 ] = blender_edit_bone.tail.x
			cheonsa_model_bone.tail_position[ 1 ] = blender_edit_bone.tail.y
			cheonsa_model_bone.tail_position[ 2 ] = blender_edit_bone.tail.z
			cheonsa_model_bone.tail_radius = blender_edit_bone.tail_radius
			cheonsa_model.bone_list.append( cheonsa_model_bone )
		bpy.ops.object.mode_set( mode = 'OBJECT', toggle = False )		# exit edit mode on the armature.
		blender_armature.data.pose_position = original_pose_position	# restore last pose position.
		context.scene.update()											# update armature change.
		bpy.ops.object.select_all( action = 'DESELECT' )				# deselect all.
		blender_object_root.select = True								# select root.
		context.scene.objects.active = blender_object_root				# make root active.

	# attachment points.
	for blender_object in blender_object_list:
		if blender_object.type == 'EMPTY' and blender_object.parent_type == 'BONE':
			trimmed_name = trim_blender_name( blender_object.name )
			if blender_object.name.startswith( "attachment" ):
				cheonsa_model_attachment_point = model_c.attachment_point_c()
				cheonsa_model_attachment_point.name = trim_blender_name( blender_object.name )
				cheonsa_model_attachment_point.mother_bone_name = blender_object.parent_bone
				cheonsa_model_attachment_point.position[ 0 ] = blender_object.location.x
				cheonsa_model_attachment_point.position[ 1 ] = blender_object.location.y
				cheonsa_model_attachment_point.position[ 2 ] = blender_object.location.z
				cheonsa_model_attachment_point.rotation[ 0 ] = blender_object.rotation_quaternion.x
				cheonsa_model_attachment_point.rotation[ 1 ] = blender_object.rotation_quaternion.y
				cheonsa_model_attachment_point.rotation[ 2 ] = blender_object.rotation_quaternion.z
				cheonsa_model_attachment_point.rotation[ 3 ] = blender_object.rotation_quaternion.w
				if blender_object.scale[ 0 ] != blender_object.scale[ 1 ] or blender_object.scale[ 0 ] != blender_object.scale[ 2 ]:
					print( "    warning: an attachment object has non-uniform scale, but cheonsa attachments only support uniform scale. the attachment will still be exported but using the x scale to define the uniform scale." )
				cheonsa_model_attachment_point.scale = blender_object.scale[ 0 ]
				cheonsa_model.attachment_point_list.append( cheonsa_model_attachment_point )

	# animations.
	if ( len( export_animation_list ) > 0 ):
		print( "  building animations" )
		blender_action_name_list = bpy.data.actions.keys()
		for export_animation in export_animation_list:
			frame_to_time = 1.0 / context.scene.render.fps * export_animation.export_time_scale
			cheonsa_model_animation = model_c.animation_c()
			cheonsa_model_animation.name = export_animation.name
			cheonsa_model_animation.move_speed = export_animation.move_speed
			cheonsa_model_animation.time_in = export_animation.frame_start * frame_to_time
			cheonsa_model_animation.time_out = export_animation.frame_end * frame_to_time
			for export_event in export_animation.export_event_list:
				cheonsa_model_animation_event = model_c.animation_event_c()
				cheonsa_model_animation_event.time = export_event.frame * frame_to_time
				cheonsa_model_animation_event.type = export_event.type
				cheonsa_model_animation_event.value = export_event.value
				cheonsa_model_animation.event_list.append( cheonsa_model_animation_event )
			for blender_action_name in blender_action_name_list:
				if trim_blender_name( blender_action_name ) == export_animation.name:
					blender_action = bpy.data.actions[blender_action_name]
					for blender_group in blender_action.groups:
						cheonsa_model_animation_object = model_c.animation_object_c()
						cheonsa_model_animation_object.name = blender_group.name
						for blender_f_curve in blender_group.channels:
							for blender_key_frame in blender_f_curve.keyframe_points:
								if blender_f_curve.data_path.endswith( "location" ):
									if blender_f_curve.array_index == 0:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.position, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 0 ] = blender_key_frame.co[ 1 ]
									elif blender_f_curve.array_index == 1:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.position, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 1 ] = blender_key_frame.co[ 1 ]
									elif blender_f_curve.array_index == 2:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.position, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 2 ] = blender_key_frame.co[ 1 ]
								elif blender_f_curve.data_path.endswith( "rotation_quaternion" ):
									if blender_f_curve.array_index == 0:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.rotation, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 3 ] = blender_key_frame.co[ 1 ] # value[3] = blender_key_frame.co[1]
									elif blender_f_curve.array_index == 1:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.rotation, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 0 ] = blender_key_frame.co[ 1 ] # value[0] = -blender_key_frame.co[1]
									elif blender_f_curve.array_index == 2:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.rotation, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 1 ] = blender_key_frame.co[ 1 ] # value[2] = blender_key_frame.co[1]
									elif blender_f_curve.array_index == 3:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.rotation, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 2 ] = blender_key_frame.co[ 1 ] # value[1] = blender_key_frame.co[1]
								elif blender_f_curve.data_path.endswith( "scale" ):
									if blender_f_curve.array_index == 0:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.scale, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 0 ] = blender_key_frame.co[ 1 ]
									elif blender_f_curve.array_index == 1:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.scale, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 1 ] = blender_key_frame.co[ 1 ]
									elif blender_f_curve.array_index == 2:
										cheonsa_model_animation_object.add_key( model_c.animation_property_e.scale, blender_key_frame.co[ 0 ] * frame_to_time ).value[ 2 ] = blender_key_frame.co[ 1 ]
						cheonsa_model.animation_object_list.append( cheonsa_model_animation_object )
					break
			cheonsa_model.animation_list.append( cheonsa_model_animation )

	# physics.

	"""
	it looks like blender 2.80 no longer supports compound shapes, so this function will be unused at the moment.
	info: https://docs.blender.org/manual/en/latest/game_engine/physics/types/rigid_body.html
	gathers all collision bounds contributing blender objects related to the input blender_object.
	returns an empty list if the blender_object is not use_collision_bounds and ( blender_object is not physics_type 'STATIC' or 'RIGID_BODY' ).
	if the collision type is not compound then it returns a list of length 1 containing only the input blender_object.
	if the collision type is compound then it returns a list of length >= 1 containing all of the child blender objects that are correctly configured to contribute to the compound collision shape.
	"""
	def gather_collision_objects( blender_object ):
		result = []
		if blender_object.game.use_collision_bounds and ( blender_object.game.physics_type == 'STATIC' or blender_object.game.physics_type == 'RIGID_BODY' ):
			if blender_object.game.use_collision_compound:
				for blender_object_child in blender_object.children:
					if blender_object_child.game.use_collision_bounds and blender_object_child.game.use_collision_compound:
						result.append( blender_object_child )
			else:
				result.append( blender_object )
		return result

	#blender_rigid_body_joint_list = []
	printed = False
	for blender_object in blender_object_list:
		if blender_object.rigid_body != None:
			if printed == False:
				print( "  building physics" )
				printed = True
			cheonsa_model_physics_body = model_c.physics_ridgid_body_c()
			cheonsa_model_physics_body.name = trim_blender_name( blender_object.name )
			cheonsa_model_physics_body.mother_bone_name = blender_object.parent_bone
			cheonsa_model_physics_body.position[ 0 ] = blender_object.location.x
			cheonsa_model_physics_body.position[ 1 ] = blender_object.location.y
			cheonsa_model_physics_body.position[ 2 ] = blender_object.location.z
			cheonsa_model_physics_body.rotation[ 0 ] = blender_object.rotation_quaternion.x
			cheonsa_model_physics_body.rotation[ 1 ] = blender_object.rotation_quaternion.y
			cheonsa_model_physics_body.rotation[ 2 ] = blender_object.rotation_quaternion.z
			cheonsa_model_physics_body.rotation[ 3 ] = blender_object.rotation_quaternion.w
			if blender_object.scale.x != blender_object.scale.y or blender_object.scale.x != blender_object.scale.z:
				print( "    warning: a physics body object has non-uniform scale, but cheonsa physics bodies only support uniform scale. the physics body will still be exported but using the x scale to define the uniform scale." )
			cheonsa_model_physics_body.scale = blender_object.scale.x
			cheonsa_model_physics_body.mass = blender_object.rigid_body.mass
			cheonsa_model_physics_body.friction = blender_object.rigid_body.friction
			cheonsa_model_physics_body.restitution = blender_object.rigid_body.restitution
			cheonsa_model_physics_body.linear_damping = blender_object.rigid_body.linear_damping
			cheonsa_model_physics_body.angular_damping = blender_object.rigid_body.angular_damping
			cheonsa_model_physics_body.flags = 0x00
			if blender_object.rigid_body.type == 'ACTIVE':
				cheonsa_model_physics_body.flags |= model_c.physics_ridgid_body_c.flags_e.is_active
			if blender_object.rigid_body.enabled:
				cheonsa_model_physics_body.flags |= model_c.physics_ridgid_body_c.flags_e.is_dynamic
			if blender_object.rigid_body.kinematic:
				cheonsa_model_physics_body.flags |= model_c.physics_ridgid_body_c.flags_e.is_kinematic
			for i in range( 0, 6 ):
				if blender_object.rigid_body.collision_collections[ i ]:
					cheonsa_model_physics_body.layer = math.pow( 2, i );
					break
				aabb_min = None
				aabb_max = None
				extract_blender_bound_box( blender_object, aabb_min, aabb_max )
				aabb_extents = ( aabb_max[ 0 ] - aabb_min[ 0 ], aabb_max[ 1 ] - aabb_min[ 1 ], aabb_max[ 2 ] - aabb_min[ 2 ] )
				aabb_center = ( aabb_extents[ 0 ] * 0.5, aabb_extents[ 1 ] * 0.5, aabb_extents[ 2 ] * 0.5 )
				cheonsa_model_physics_shape = model_c.physics_shape_c()
				if blender_object.rigid_body.collision_shape == 'BOX':
					cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.box
					cheonsa_model_physics_shape.box_width = aabb_extents[ 0 ]
					cheonsa_model_physics_shape.box_depth = aabb_extents[ 1 ]
					cheonsa_model_physics_shape.box_height = aabb_extents[ 2 ]
				elif blender_object.rigid_body.collision_shape == 'SPHERE':
					cheonsa_model_physics_shape = model_c.physics_shape_c()
					cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.sphere
					cheonsa_model_physics_shape.sphere_radius = aabb_extents[ 0 ]
					if aabb_extents[ 1 ] > aabb_extents[ 0 ]:
						cheonsa_model_physics_shape.sphere_radius = aabb_extents[ 1 ]
					if aabb_extents[ 2 ] > aabb_extents[ 1 ]:
						cheonsa_model_physics_shape.sphere_radius = aabb_extents[ 2 ]
					cheonsa_model_physics_shape.sphere_radius *= 0.5
				elif blender_object.rigid_body.collision_shape == 'CONE' or blender_object.rigid_body.collision_shape == 'CAPSULE' or blender_object.rigid_body.collision_shape == 'CYLINDER':
					cheonsa_model_physics_shape = model_c.physics_shape_c()
					if aabb_extents[ 0 ] > aabb_extents[ 1 ]:
						generic_radius = aabb_extents[ 0 ] * 0.5
					else:
						generic_radius = aabb_extents[ 1 ] * 0.5
					generic_height = aabb_extents[ 2 ]
					if blender_object.rigid_body.collision_shape == 'CONE':
						cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.cone
						cheonsa_model_physics_shape.cone_radius = generic_radius
						cheonsa_model_physics_shape.cone_height = generic_height
					elif blender_object.rigid_body.collision_shape == 'CAPSULE':
						cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.capsule
						cheonsa_model_physics_shape.capsule_radius = generic_radius
						cheonsa_model_physics_shape.capsule_height = generic_height
					elif blender_object.rigid_body.collision_shape == 'CYLINDER':
						cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.cylinder
						cheonsa_model_physics_shape.cylinder_radius = generic_radius
						cheonsa_model_physics_shape.cylinder_height = generic_height
				elif blender_object.rigid_body.collision_shape == 'CONVEX_HULL':
					if blender_object.type == 'MESH':
						cheonsa_model_physics_shape = model_c.physics_shape_c()
						cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.convex_hull
						blender_mesh = blender_object
						for blender_mesh_vertex in blender_mesh.data.vertices:
							cheonsa_model_physics_shape.convex_hull_vertex_list.append( ( blender_mesh_vertex.co.x, blender_mesh_vertex.co.y, blender_mesh_vertex.co.z ) )
				elif blender_object.rigid_body.collision_shape == 'MESH':
					if blender_object.type == 'MESH':
						cheonsa_model_physics_shape = cheonsa_model_physics_body.add_shape()
						cheonsa_model_physics_shape.type = model_c.physics_shape_type_e.triangle_mesh
						blender_mesh = triangulate_blender_mesh( context, mesh_and_options.blender_mesh ) # duplicate and triangulate blender mesh.
						index_remap = {} # tracks which vertices have already been added.
						for blender_polygon in blender_mesh.data.polygons:
							polygon_vertex_list = []
							for blender_loop_index in blender_polygon.loop_indices:
								blender_loop = blender_mesh.data.loops[ blender_loop_index ]
								if blender_loop.vertex_index in index_remap:
									cheonsa_model_physics_shape.triangle_mesh_index_list.append( index_remap[ blender_loop.vertex_index ] )
								else:
									index = len( cheonsa_model_physics_shape.triangle_mesh_vertex_list )
									index_remap[ blender_loop.vertex_index ] = index
									blender_mesh_vertex = blender_mesh.data.vertices[ blender_loop.vertex_index ]
									cheonsa_model_physics_shape.triangle_mesh_vertex_list.append( ( blender_mesh_vertex.co[ 0 ], blender_mesh_vertex.co[ 1 ], blender_mesh_vertex.co[ 2 ] ) )
									cheonsa_model_physics_shape.triangle_mesh_index_list.append( index )
						bpy.ops.object.delete() # delete previously duplicated blender mesh.
				cheonsa_model_physics_shape.position[ 0 ] = aabb_center[ 0 ]
				cheonsa_model_physics_shape.position[ 1 ] = aabb_center[ 1 ]
				cheonsa_model_physics_shape.position[ 2 ] = aabb_center[ 2 ]
				cheonsa_model_physics_shape.rotation[ 0 ] = blender_object.rotation_quaternion.x
				cheonsa_model_physics_shape.rotation[ 1 ] = blender_object.rotation_quaternion.y
				cheonsa_model_physics_shape.rotation[ 2 ] = blender_object.rotation_quaternion.z
				cheonsa_model_physics_shape.rotation[ 3 ] = blender_object.rotation_quaternion.w
				if blender_object.rigid_body.use_margin:
					cheonsa_model_physics_shape.margin = blender_object.rigid_body.collision_margin
				cheonsa_model_physics_body.shape_list.append( cheonsa_model_physics_shape )
			cheonsa_model.phsyics_body_list.append( cheonsa_model_physics_body )
	if len( cheonsa_model.phsyics_body_list ) > 0:
		for blender_object in blender_object_list:
			if blender_object.rigid_body_constraint == None:
				continue
			if blender_object.rigid_body_constraint.type == 'FIXED':
				pass
			elif blender_object.rigid_body_constraint.type == 'POINT':
				pass
			elif blender_object.rigid_body_constraint.type == 'HINGE':
				pass
			elif blender_object.rigid_body_constraint.type == 'SLIDER':
				pass
			elif blender_object.rigid_body_constraint.type == 'PISTON':
				pass
			elif blender_object.rigid_body_constraint.type == 'GENERIC':
				pass
			elif blender_object.rigid_body_constraint.type == 'GENERIC_SPRING':
				pass
			elif blender_object_rigid_body_constraint.type == 'MOTOR':
				pass

	#for blender_constraint in blender_object.constraints:
	#	if blender_constraint.type == 'RIGID_BODY_JOINT':
	#		blender_rigid_body_joint_list.append( (blender_object, blender_constraint) )
	#for value in blender_rigid_body_joint_list:
	#	blender_object = value[0]
	#	blender_rigid_body_joint = value[1]
	#	model_constraint = model_file.physics_chunk.add_constraint()
	#	model_constraint.frame_position[0] = blender_rigid_body_joint.pivot_x
	#	model_constraint.frame_position[1] = blender_rigid_body_joint.pivot_y
	#	model_constraint.frame_position[2] = blender_rigid_body_joint.pivot_z
	#	blender_euler = mathutils.Euler(blender_rigid_body_joint.axis_x, blender_rigid_body_joint.axis_y, blender_rigid_body_joint.axis_z, 'XYZ')
	#	blender_quaternion = blender_euler.to_quaternion()
	#	model_constraint.frame_rotation[0] = blender_quaternion.x
	#	model_constraint.frame_rotation[1] = blender_quaternion.y
	#	model_constraint.frame_rotation[2] = blender_quaternion.z
	#	if blender_rigid_body_joint.pivot_type == 'HINGE':
	#		model_constraint.parameters_list = [ 0, 0 ]
	#		if blender_rigid_body_joint.use_angular_limit_x:
	#			model_constraint.parameters_list[ 0 ] = blender_rigid_body_joint.limit_ang_min_x
	#			model_constraint.parameters_list[ 1 ] = blender_rigid_body_joint.limit_ang_max_x
	#	elif blender_rigid_body_joint.pivot_type == 'CONE_TWIST':
	#		model_constraint.parameters_list = [ 0, 0, 0 ]
	#		if blender_rigid_body_joint.use_angular_limit_x:
	#			model_constraint.parameters_list[ 0 ] = blender_rigid_body_joint.limit_ang_max_x
	#		if blender_rigid_body_joint.use_angular_limit_y:
	#			model_constraint.parameters_list[ 1 ] = blender_rigid_body_joint.limit_ang_max_y
	#		if blender_rigid_body_joint.use_angular_limit_z:
	#			model_constraint.parameters_list[ 2 ] = blender_rigid_body_joint.limit_ang_max_z
	#	elif blender_rigid_body_joint.pivot_type == 'GENERIC_6_DOF':
	#		model_constraint.parameters_list = [ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]
	#		if blender_rigid_body_joint.use_linear_limit_x:
	#			model_constraint.linear_lower_a = blender_rigid_body_joint.limit_min_x
	#			model_constraint.linear_upper_a = blender_rigid_body_joint.limit_max_x
	#		if blender_rigid_body_joint.use_linear_limit_y:
	#			model_constraint.linear_lower_b = blender_rigid_body_joint.limit_min_y
	#			model_constraint.linear_upper_b = blender_rigid_body_joint.limit_max_y
	#		if blender_rigid_body_joint.use_linear_limit_z:
	#			model_constraint.linear_lower_c = blender_rigid_body_joint.limit_min_z
	#			model_constraint.linear_upper_c = blender_rigid_body_joint.limit_max_z
	#		if blender_rigid_body_joint.use_angular_limit_x:
	#			model_constraint.angular_lower_a = blender_rigid_body_joint.limit_ang_min_x
	#			model_constraint.angular_upper_a = blender_rigid_body_joint.limit_ang_max_x
	#		if blender_rigid_body_joint.use_angular_limit_y:
	#			model_constraint.angular_lower_b = blender_rigid_body_joint.limit_ang_min_y
	#			model_constraint.angular_upper_b = blender_rigid_body_joint.limit_ang_max_y
	#		if blender_rigid_body_joint.use_angular_limit_z:
	#			model_constraint.angular_lower_c = blender_rigid_body_joint.limit_ang_min_z
	#			model_constraint.angular_upper_c = blender_rigid_body_joint.limit_ang_max_z

	# lights
	printed = False
	for blender_object in blender_object_list:
		if blender_object.type != 'LIGHT' or blender_object.hide_render:
			continue
		if printed == False:
			print( "  building lights" )
			printed = True
		cheonsa_model_light = model_c.light_c()
		cheonsa_model_light.name = trim_blender_name( blender_object.name )
		cheonsa_model_light.mother_bone_name = blender_object.parent_bone
		if blender_object.data.type == 'POINT':
			cheonsa_model_light.type = model_c.light_c.type_e.point
		elif blender_object.data.type == 'SUN':
			cheonsa_model_light.type = model_c.light_c.type_e.direction
		elif blender_object.data.type == 'SPOT':
			cheonsa_model_light.type = model_c.light_c.type_e.spot
			cheonsa_model_light.cone_angle = blender_object.data.spot_size
		cheonsa_model_light.position[ 0 ] = blender_object.location[ 0 ]
		cheonsa_model_light.position[ 1 ] = blender_object.location[ 1 ]
		cheonsa_model_light.position[ 2 ] = blender_object.location[ 2 ]
		cheonsa_model_light.rotation[ 0 ] = blender_object.rotation_quaternion[ 1 ]
		cheonsa_model_light.rotation[ 1 ] = blender_object.rotation_quaternion[ 2 ]
		cheonsa_model_light.rotation[ 2 ] = blender_object.rotation_quaternion[ 3 ]
		cheonsa_model_light.rotation[ 3 ] = blender_object.rotation_quaternion[ 0 ]
		cheonsa_model_light.color[ 0 ] = blender_object.data.color[ 0 ]
		cheonsa_model_light.color[ 1 ] = blender_object.data.color[ 1 ]
		cheonsa_model_light.color[ 2 ] = blender_object.data.color[ 2 ]
		cheonsa_model_light.brightness = blender_object.data.energy
		cheonsa_model.light_list.append( cheonsa_model_light )

	# save
	print( "  saving..." )
	cheonsa_model.save( file_path )
	print( "  done!" )
	return True

# EXPORT OPERATOR
class Export_chm( bpy.types.Operator, ExportHelper ):
	"""Export the active selection as a Cheonsa Model (.chm) file."""
	bl_idname = "export_shape.chm"
	bl_label = "Export Cheonsa Model (.chm)"
	file_extension = ".chm"

	@classmethod
	def poll( cls, context ):
		obj = context.active_object
		return obj is not None and obj.type in {'MESH', 'ARMATURE', 'EMPTY', 'LIGHT'}

	def execute( self, context ):
		start_time = time.time()
		#props = self.properties
		file_path = self.filepath
		file_path = bpy.path.ensure_ext( file_path, self.file_extension )
		do_export( context, file_path )
		return { 'FINISHED' }

	def invoke( self, context, event ):
		context.window_manager.fileselect_add( self )
		return { 'RUNNING_MODAL' }

def menu_func_export_button( self, context ):
	self.layout.operator( Export_chm.bl_idname, text="Cheonsa (.chm)" )

classes = [ Export_chm ]

def register():
	for cls in classes:
		bpy.utils.register_class( cls )
	bpy.types.TOPBAR_MT_file_export.append( menu_func_export_button )

def unregister():
	bpy.types.TOPBAR_MT_file_export.remove( menu_func_export_button )
	for cls in classes:
		bpy.utils.unregister_class( cls )

if __name__ == "__main__":
	register()
