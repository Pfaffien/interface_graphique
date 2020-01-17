/**
 *  @file   ei_widget_frame.h
 *  @brief  Functions to draw frames.
 *
 */

#ifndef EI_WIDGET_FRAME_H
#define EI_WIDGET_FRAME_H

#include "ei_widget.h"

/**
 * \brief  Structure of a frame widget.
 */
typedef struct ei_widget_frame_t {
    ei_widget_t  widget;

	ei_color_t   color;
	int	     border_width;
	ei_relief_t	 relief;
	char*		 text;
	ei_font_t	 text_font;
	ei_color_t	 text_color;
	ei_anchor_t  text_anchor;
	ei_surface_t img;
	ei_rect_t*	 img_rect;
	ei_anchor_t  img_anchor;
} ei_widget_frame_t;

/**
 * \brief   Memory allocation function.
 */
void *frame_allocfunc();

/**
 * \brief   Releases the memory.
 *
 * @param   widget      The widget to release.
 */
void frame_releasefunc(struct ei_widget_t* widget);

/**
 * \brief   Draws a frame on a given surface.
 *
 * @param   widget          The widget to draw.
 * @param   surface         The surface on which to draw the frame.
 * @param   pick_surface    The picking surface associated.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void frame_drawfunc(ei_widget_t* widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

/**
 * \brief   Sets the default values of a frame.
 *
 * @param   widget      The widget to initialize.
 */
void frame_setdefaultsfunc(struct ei_widget_t* widget);

/**
 * \brief   Defines the behavior of a button.
 *
 * @param   widget      The button to handle.
 * @param   event       The event to handle.
 *
 * @return  True if no error has occured.
 */
void frame_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);

#endif
