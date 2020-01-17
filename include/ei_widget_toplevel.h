/**
 *  @file   ei_widget_toplevel
 *  @brief  Functions to draw and handle toplevel widgets.
 *
 */

#ifndef EI_WIDGET_TOPLEVEL_H
#define EI_WIDGET_TOPLEVEL_H

#include "ei_widget.h"
#include "ei_event.h"

/**
 * \brief   Structure of a toplevel widget.
 */
typedef struct ei_widget_toplevel_t {
    ei_widget_t   widget;

    ei_color_t 	  color;
    int	          border_width;
    char*	  title;
    ei_bool_t	  closable;
    ei_axis_set_t resizable;
    ei_size_t*	  min_size;
    ei_point_t    mouse_handle;
    ei_bool_t     is_moving;
    ei_bool_t     is_resizing;
} ei_widget_toplevel_t;

/**
 * \brief   Memory allocation function.
 */
void *toplevel_allocfunc();

/**
 * \brief   Releases the memory.
 *
 * @param   widget      The widget to release.
 */
void toplevel_releasefunc(struct ei_widget_t* widget);

/**
 * \brief   Draws a toplevel.
 *
 * @param   widget          The widget to draw.
 * @param   surface         The surface on which to draw the toplevel.
 * @param   pick_surface    The picking surface.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void toplevel_drawfunc(ei_widget_t* widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

/**
 * \brief   Sets the default parameters of a toplevel.
 *
 * @param   widget      The toplevel to initialize.
 */
void toplevel_setdefaultsfunc(struct ei_widget_t* widget);

/**
 * \brief   Defines the behavior of a button.
 *
 * @param   widget      The button to handle.
 * @param   event       The event to handle.
 *
 * @return  True if no error has occured.
 */
void toplevel_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);

/**
 * \brief   Defines how to handle a toplevel.
 *
 * @param   widget      The toplevel to handle.
 * @param   event       The event to handle.
 */
ei_bool_t toplevel_handlefunc(struct ei_widget_t* widget, struct ei_event_t* event);

#endif
