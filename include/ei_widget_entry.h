/**
 *  @file   ei_widget_entry.h
 *  @brief  Functions to draw and handle entry widgets and forms.
 *
 */

#ifndef EI_WIDGET_ENTRY_H
#define EI_WIDGET_ENTRY_H

#include "ei_widget.h"
#include "ei_event.h"

/**
 * \brief   Structure of an entry widget.
 */
typedef struct ei_widget_entry_t {
    ei_widget_t   widget;

    ei_color_t    color;
    ei_color_t    glow_color;
    int			  corner_radius;
    char	      text[80];
    ei_font_t     text_font;
    ei_color_t    text_color;
    ei_bool_t     cursor;
} ei_widget_entry_t;

/**
 * \brief   Memory allocation function.
 */
void *entry_allocfunc();

/**
 * \brief   Releases the memory.
 *
 * @param   widget      The widget to release.
 */
void entry_releasefunc(struct ei_widget_t* widget);

/**
 * \brief   Draws an entry.
 *
 * @param   widget          The widget to draw.
 * @param   surface         The surface on which to draw the entry.
 * @param   pick_surface    The picking surface.
 * @param   clipper         A pointer on the clipping rectangle.
 */
void entry_drawfunc(ei_widget_t* widget, ei_surface_t surface, ei_surface_t pick_surface, ei_rect_t* clipper);

/**
 * \brief   Initializes the parameters of an entry.
 *
 * @param   widget      The widget to initialize.
 */
void entry_setdefaultsfunc(struct ei_widget_t* widget);

/**
 * \brief   Invalidates the old and new widget
 *
 * @param   widget      The old widget.
 * @param   rect        The new widget.
 */
void entry_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect);

/**
 * \brief   Defines how to handle an entry.
 *
 * @param   widget      The widget to handle.
 * @param   event       The event to handle.
 *
 * @return  False if the entry is not selected, else True.
 */
ei_bool_t entry_handlefunc(struct ei_widget_t* widget, struct ei_event_t* event);

#endif
