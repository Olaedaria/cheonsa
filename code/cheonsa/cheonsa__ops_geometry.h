#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{
	namespace ops
	{

		line32_c make_line32_from_normal_and_distance( vector32x2_c const & normal, float32_c distance );
		line32_c make_line32_from_normal_and_point( vector32x2_c const & normal, vector32x2_c const & point );
		line32_c make_line32_from_segment( vector32x2_c const & point_a, vector32x2_c const & point_b ); // normal/tangent of result points to right of vector point_b - point_a.
		line32_c make_line32_normalized( line32_c const & line );

		//vector32x2_c get_box_point( box32x2_c const & box, matrix32x2x2_c const & box_basis, vector32x2_c const & box_origin, sint32_c box_point_index ); // box_point_index, 0 is top left, 1 is top right, 2 is bottom left, 3 is bottom right.
		//line32_c get_box_line( box32x2_c const & box, matrix32x2x2_c const & box_basis, vector32x2_c const & box_origin, sint32_c box_edge_index ); // box_edge_index, 0 is left, 1 is top, 2 is right, 3 is bottom. normal of lines point towards the outside of the box.

		plane32_c make_plane32_from_normal_and_point( vector32x3_c const & normal, vector32x3_c const & point ); // normal needs to be normalized already.
		plane32_c make_plane32_from_triangle( vector32x3_c const & point_a, vector32x3_c const & point_b, vector32x3_c const & point_c ); // if the triangle points are laid out counter clock wise on the face of a clock, then the normal will point out of the face of the clock.

		plane64_c make_plane64_from_normal_and_point( vector64x3_c const & normal, vector64x3_c const & point ); // normal needs to be normalized already. point is any point on the plane.
		plane64_c make_plane64_from_triangle( vector64x3_c const & point_a, vector64x3_c const & point_b, vector64x3_c const & point_c ); // if the triangle points are laid out counter clock wise on the face of a clock, then the normal will point out of the face of the clock.

		// normalizes the given plane, if it is not normalized.
		// in cheonsa, the abc components of the plane are the plane's normal (a normalized unit length vector), and then the d component is the distance of the plane along the plane's normal to the coordinate space's origin.
		// in other math libraries and game engines, sometimes the plane is treated like a 4 component vector, and so to normalize it means to normalize it like you would a 4 component vector. which does also work and it means the same thing, but to me it is very ugly and unintuitive to think about.
		plane64_c make_plane64_normalized( plane64_c const & plane );

		frustum64_c make_frustum64_perspective3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_x, float64_c const fov_y, float64_c const clip_near, float64_c const clip_far );
		frustum64_c make_frustum64_orthographic3( vector64x3_c const & position, vector64x3_c const & forward, vector64x3_c const & up, float64_c const fov_x, float64_c const fov_y, float64_c const clip_near, float64_c const clip_far );
		frustum64_c make_frustum64_from_view_projection( matrix32x4x4_c const & view_projection );

		polygon32x2_c make_polygon32x2_from_box32x2( box32x2_c const & box_in ); // result is a counter clock wise wind order polygon, starting with the top left vertex of the box, so the first edge will be the left edge.
		polygon32x2_c make_polygon32x2_transformed( polygon32x2_c const & polygon_in, matrix32x2x2_c transform_basis, vector32x2_c transform_origin ); // transforms the points in the given polygon by the given transform.

		//ray64_c make_transformed_ray( ray64_c const & ray, matrix64x3x4_c const & transform );
		vector64x3_c make_vector64x3_normal_from_triangle( vector64x3_c a, vector64x3_c b, vector64x3_c c );
		box64x3_c make_aabb_from_obb( box64x3_c const & obb, transform3d_c const & obb_transform );
		box32x2_c make_aabb_from_obb( box32x2_c const & obb, vector32x2_c const & obb_position, matrix32x2x2_c obb_basis );
		//plane64_c make_transformed_plane( plane64_c const & plane, transform3d_c const & transform );

		float32_c distance_between_point_and_line( vector32x2_c const & point, line32_c const & line ); // positive distance means that point lies above line, negative distance means that point lies beneath line.
		float64_c distance_between_point_and_plane( vector64x3_c const & point, plane64_c const & plane ); // positive distance means that point lies above plane, negative distance means that point lies beneath plane.
		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box );
		vector64x3_c nearest_point_on_box( vector64x3_c const & point, box64x3_c const & box, transform3d_c const & box_transform );
		vector64x3_c nearest_point_on_plane( vector64x3_c const & point, plane64_c const & plane );
		vector64x3_c nearest_point_on_ray( vector64x3_c const & point, ray64_c const & ray );
		vector64x3_c nearest_point_on_segment( vector64x3_c const & point, segment64_c const & segment );
		vector64x2_c nearest_point_on_box( vector64x2_c const & point, box64x2_c const & box );

		boolean_c intersect_frustum_vs_point( frustum64_c const & frustum, vector64x3_c const & point );
		boolean_c intersect_frustum_vs_sphere( frustum64_c const & frustum, sphere64_c const & sphere );
		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box );
		boolean_c intersect_frustum_vs_box( frustum64_c const & frustum, box64x3_c const & box, transform3d_c const & box_transform );
		boolean_c intersect_sphere_vs_point( sphere64_c const & sphere, vector64x3_c const & point );
		boolean_c intersect_sphere_vs_sphere( sphere64_c const & sphere_a, sphere64_c const & sphere_b );
		boolean_c intersect_box_vs_point( box64x3_c const & box, vector64x3_c const & point );
		boolean_c intersect_box_vs_box( box64x3_c const & box_a, box64x3_c const & box_b );
		boolean_c intersect_box_vs_box( box64x3_c const & box_a, transform3d_c const & box_a_transform, box64x3_c const & box_b, transform3d_c const & box_b_transform );
		boolean_c intersect_box_vs_sphere( box64x3_c const & box, sphere64_c const & sphere );
		boolean_c intersect_box_vs_sphere( box64x3_c const & box, transform3d_c const & box_transform, sphere64_c const & b );
		boolean_c intersect_box_vs_point( box32x2_c const & box, vector32x2_c const & point );
		boolean_c intersect_box_vs_point( box64x2_c const & box, vector64x2_c const & point );
		boolean_c intersect_box_vs_box( box32x2_c const & box_a, box32x2_c const & box_b );
		boolean_c intersect_box_vs_box( box64x2_c const & box_a, box64x2_c const & box_b );
		boolean_c intersect_box_vs_circle( box64x2_c const & box, circle64_c const & circle );
		boolean_c intersect_plane_vs_plane_vs_plane( plane64_c const & a, plane64_c const & b, plane64_c const & c, vector64x3_c * intersection_point );
		//boolean_c intersect_ray_vs_plane( ray64_c const & ray, plane64_c const & plane, float64_c & t );
		//boolean_c intersect_ray_vs_box( ray64_c const & ray, box64x3_c const & box );
		boolean_c intersect_segment_vs_box( segment64_c const & segment, box64x3_c const & box );
		//boolean_c intersect_ray_vs_sphere( const ray32_c& a, sphere32_c const & b, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_sphere( vector32x3_c const & ray_start, vector32x3_c const & ray_end, sphere32_c const & sphere32_c, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_box( const ray32_c& a, box32x3_c const & b, float32_c * intersection_distance );
		//boolean_c intersect_ray_vs_box( vector32x3_c const & ray_start, vector32x3_c const & ray_end, box32x3_c const & box32x3_c, float32_c * intersection_distance );
		boolean_c intersect_point_vs_polygon( vector32x2_c const & point, polygon32x2_c const & polygon ); // returns true if point lies in or on polygon, returns false if not. wind order of polygon should be counter clock wise.
		sint32_c intersect_polygon_vs_polygon( polygon32x2_c const & polygon_a, polygon32x2_c const & polygon_b ); // returns 0 if both polygons do not intersect, returns 1 if polygons partially intersect, returns 2 if polygon_b is fully contained in polygon_a.

		boolean_c sweep_ray_vs_plane( ray64_c const & ray, plane64_c const & plane, float64_c & t );
		boolean_c sweep_ray_vs_box( ray64_c const & ray, box64x3_c const & box, float64_c & t_minimum, float64_c & t_maximum );
		boolean_c sweep_point_vs_plane( segment64_c const & point_path, plane64_c const & plane, float64_c & t );
		boolean_c sweep_point_vs_sphere( segment64_c const & point_path, sphere64_c const & sphere, float64_c & t );
		boolean_c sweep_point_vs_capsule( segment64_c const & point_path, capsule64_c const & capsule, float64_c & t );
		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, float64_c & t );
		boolean_c sweep_point_vs_box( segment64_c const & point_path, box64x3_c const & box, transform3d_c const & box_transform, float64_c & t );
		boolean_c sweep_sphere_vs_box( segment64_c const & sphere_path, float64_c const sphere_radius, box64x3_c const & box, transform3d_c const & box_transform, float64_c & t );
		boolean_c sweep_point_vs_triangle( segment64_c const & point_path, triangle64_c const & triangle, float64_c & t );

	}
}
