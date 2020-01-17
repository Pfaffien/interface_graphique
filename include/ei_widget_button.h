/**
 *  @file   ei_widget_button.h
 *  @brief  Functions to draw and handle buttons.
 *
 */


#ifndef EI_WIDGET_BUTTON_H
#define EI_WIDGET_BUTTON_H

#include "ei_widget.h"
#include "ei_event.h"

/**
 * \brief   Structure of a button.
 */
typedef struct ei_widget_button_t {
    ei_widget_t   widget;

    ei_color_t    color;
    int			  border_width;
    int			  corner_radius;
    ei_relief_t	  relief;
    char*	      text;
    ei_font_t     text_font;
    ei_color_t    text_color;
    ei_anchor_t   text_anchor;
    ei_surface_t  img;
    ei_rect_t*	  img_rect;
    ei_anchor_t	  img_anchor;
    ei_callback_t callback;
    void*		  user_param;
} ei_widget_button_t;

/**
 * \brief   Memory allocation function.
 */
void *button_allocfunc();

/**
 * \brief   Releases the memory.
 *
 * @param   widget      The widget to release.
 */
void button_releasefunc(struct ei_widget_t* widget);

/**
 * \brief   Draws a button on a given surface.
 *
 * @param   widget          Pointer on the button to draw.
 * @param   surface         The surface on which to draw the button.
 * @param   pick_surface    The picking surface.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void button_drawfunc(ei_widget_t* widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

/**
 * \brief   Initializes a button with default parameters.
 *
 * @param   widget      The widget to initialize.
 */
void button_setdefaultsfunc(struct ei_widget_t* widget);

/**
 * \brief   Invalidates the old and new widget
 *
 * @param   widget      The old widget.
 * @param   rect        The new widget.
 */
void button_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);

/**
 * \brief   Defines the behavior of a button.
 *
 * @param   widget      The button to handle.
 * @param   event       The event to handle.
 *
 * @return  True if no error has occured.
 */
ei_bool_t button_handlefunc(struct ei_widget_t* widget, struct ei_event_t* event);

#endif
