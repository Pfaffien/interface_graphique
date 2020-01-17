/**
 *  @file   ei_draw_shape.h
 *  @brief  Extension of \ref ei_draw.h functions.
 *
 */

#ifndef EI_DRAW_SHAPE_H
#define EI_DRAW_SHAPE_H

#include "ei_types.h"
#include "hw_interface.h"

/**
 * \brief   Side representation used to draw poligons.
 */
struct side {
	int ymax;
	float xymin;
	float slope;
	struct side *next;
};

/**
 * \brief   The part of a rounded_frame to be drawn.
 */
typedef enum {
	FULL = 0,
	TOP,
	BOTTOM,
	TOPLEVEL
} rounded_t;

/**
 * \brief   Sorts the sides by intersection of the side with the scanline.
 *
 * @param   sides       Linked chain of sides to sort.
 */
void sort(struct side *sides);

/**
 * \brief   Returns whether two rectangles intersect.
 *
 * @param   rect1       Rectangle of type ei_rect_t.
 * @param   rect2       Rectangle of type ei_rect_t.
 *
 * @return  True if there is an intersection.
 */
ei_bool_t do_intersect(ei_rect_t rect1, ei_rect_t rect2);

/**
 * \brief   Returns whether a point is inside a given rectangle.
 *
 * @param   point       Point of type ei_point_t.
 * @param   rect        Rectangle of type ei_rect_t.
 *
 * @return  True if the point is inside the rectangle.
 */
ei_bool_t inside_rect(ei_point_t point, ei_rect_t rect);

/**
 * \brief   Returns the outline of the smallest rectangle containing both parameters.
 *
 * @param   rect1       Rectangle of type ei_rect_t.
 * @param   rect2       Rectangle of type ei_rect_t.
 *
 * @return  A new rectangle.
 */
ei_rect_t merge_rect(ei_rect_t rect1, ei_rect_t rect2);

/**
 * \brief   Returns the area of a given rectangle.
 *
 * @param   rect        Rectangle of type ei_rect_t.
 *
 * @return  The area in pixels squared.
 */
int area_rect(ei_rect_t rect);

/**
 * \brief   Returns a rectangle with a height increased by up + down
 *      and a width increased by left + right.
 *
 * @param   rect        Rectangle of type ei_rect_t.
 * @param   up          How much the top of the rectangle has to be moved.
 * @param   right       How much the right side of the rectangle has to be moved.
 * @param   down        How much the bottom of the rectangle has to be moved.
 * @param   left        How much the left side of the rectangle has to be moved.
 *
 * @return  The expanded rectangle.
 */
ei_rect_t expand_rect(ei_rect_t rect, int up, int right, int down, int left);

/**
 * \brief   Returns the intersection rectangle of rect1 and rect2
 *
 * @param   rect1       Rectangle of type ei_rect_t.
 * @param   rect2       Rectangle of type ei_rect_t.
 *
 * @return  A rectangle.
 */
ei_rect_t intersect_rect(ei_rect_t rect1, ei_rect_t rect2);

/**
 * \brief   Prints a given rectangle.
 *
 * @param   rect        Rectangle of type ei_rect_t.
 */
void print_rect(ei_rect_t rect);

/**
 * \brief   Prints a given point.
 *
 * @param   point       Point of type ei_point_t.
 */
void print_point(ei_point_t point);

/**
 * \brief   Transforms a given linked chain of points by changing some points
 *      to create a curve instead.
 *
 * @param   centre      Center of the curve.
 * @param   rayon       Radius of the circle used to make the curve.
 * @param   debut       Used to create a step and an angle.
 * @param   fin         Used to create a step.
 * @param   points      The linked chain to modify.
 * @param   res         The number of points to change.
 */
void arc(ei_point_t centre, int rayon, float debut, float fin,
	 ei_linked_point_t *points, int res);

/**
 * \brief   Calculates a linked chain of points to create a rounded_frame.
 *
 * @param   rect        Used to create a rounded_frame of the same dimensions, at the same origin.
 * @param   rayon       The radius of the circle used to create the curve.
 * @param   h           Half of the height of the rectangle.
 * @param   res         Number of interpolations to make the curve.
 * @param   type        The part of a rounded_frame to be drawn.
 *
 * @return  A linked chain of points which represent a rounded frame when
 *      they are connected.
 */
ei_linked_point_t *rounded_frame(ei_rect_t rect, int rayon, int h, int res,
				 rounded_t type);

/**
 * \brief   Returns the color of a pixel after considering the transparency of
 *      the upper layer.
 *
 *  @param  c1      Color component considered of the surface already at screen.
 *  @param  c2      Color component considered of the upper layer.
 *  @param  a       Transparency of the upper layer.
 *
 *  @param  A 8-bit integer representing a color component after overlap.
 */
uint8_t get_alpha(int c1, int c2, int a);

/**
 * \brief   Changes the color of a given pixel to a given color.
 *
 * @param   pixel_ptr       Pointer on the origin considered.
 * @param   x               Abscissa relative to the origin considered.
 * @param   y               Ordinate relative to the origin considered.
 * @param   width           Width of the area considered.
 * @param   col             New color.
 */
void draw_pixel_opti(uint32_t *pixel_ptr, int x, int y, int width, uint32_t col);

/**
 * \brief   Changes the color of a given pixel to a given color considering
 *      transparency.
 *
 * @param   dst_ptr     Table containing the four components RGBA.
 * @param   x           Abscissa of the pixel.
 * @param   y           Ordinate the pixel.
 * @param   width       Width of the area considered
 * @param   col         New color.
 */
void draw_pixel_opti_alpha(uint8_t *dst_ptr, int x, int y, int width, ei_color_t col);

/**
 * \brief               Returns a clipped line.
 *
 * @param   p1          First end of the line to clip.
 * @param   p2          Second end of the line to clip.
 * @param   cp1         A point which is modified by the function (new point).
 * @param   cp2         A point which is modified by the function (new point).
 * @param   clipper     Pointer on the rectangle used for the clipping.
 *
 * @return  True if the line and the clipper intersect and if no error has
 *      occured.
 */
ei_bool_t clip_line(ei_point_t p1, ei_point_t p2, ei_point_t *cp1, ei_point_t *cp2, const ei_rect_t *clipper);

/**
 * \brief   Draws a clipped line going from p1 to p2 on a given surface in
 *      a given color.
 *
 * @param   surface     Surface on which to draw the line.
 * @param   p1          First end of the line to draw.
 * @param   p2          Second end of the line to draw.
 * @param   color       Color of the line to draw.
 * @param   clipper     A pointer on the clipping rectangle.
 */
void draw_line(ei_surface_t surface, ei_point_t p1, ei_point_t p2,
	       const ei_color_t color, const ei_rect_t *clipper);

/**
 * \brief   Draws a broken line going through the given points.
 *
 * @param   surface         Surface on which to draw the line.
 * @param   first_point     First point of the linked chain of the points to draw.
 * @param   color           Color of the line to draw.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void ei_draw_polyline_closed	(ei_surface_t			surface,
						 const ei_linked_point_t*	first_point,
						 const ei_color_t		color,
						 const ei_rect_t*		clipper);

/**
 * \brief   Frees the memory.
 *
 * @param   poly        The polygon to free.
 */
void free_polygon(ei_linked_point_t *poly);

/**
 * \brief   Returns a clipped polygon.
 *
 * @param   poly        The polygon to clip.
 * @param   clipper     A pointer on the clipping rectangle.
 *
 * @return  A linked chain of points representing the intersection of poly
 *      and clipper when they are connected.
 */
ei_linked_point_t* clip_polygon(const ei_linked_point_t *poly, const ei_rect_t *clipper);

/**
 * \brief   Draws a polygon.
 *
 * @param   surface1        Drawing surface.
 * @param   surface2        Picking surface.
 * @param   first_point     First point of the linked chain representing the polygon to draw.
 * @param   color1          Color of the drawing surface.
 * @param   color2          Color of the picking surface.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void ei_draw_polygon_opti(ei_surface_t surface1, ei_surface_t surface2,
			  const ei_linked_point_t *first_point,
			  const ei_color_t color1, const ei_color_t color2,
			  const ei_rect_t *clipper);

#endif
