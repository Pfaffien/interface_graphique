#include "ei_widget_button.h"

#include "ei_draw_shape.h"

#include <stdlib.h>

#include "ei_application.h"
#include "ei_utils.h"

void *button_allocfunc()
{
	return malloc(sizeof(ei_widget_button_t));
}

void button_releasefunc(struct ei_widget_t *widget)
{
	ei_app_invalidate_rect(&widget->parent->screen_location);

	free(widget->placer_params);
	free(widget->pick_color);
	free(widget);
}


void button_drawfunc(ei_widget_t *widget, ei_surface_t surface,
		     ei_surface_t pick_surface, ei_rect_t *clipper)
{
	ei_widget_button_t *wid = (ei_widget_button_t *)widget;
	if (wid->relief != ei_relief_none) {
		int h = widget->screen_location.size.width <
					widget->screen_location.size.height
				? widget->screen_location.size.width / 2
				: widget->screen_location.size.height / 2;

		/* Color definitions to create the relief */
		ei_color_t dark = {72, 72, 72, 0xFF};
		ei_color_t light = {200, 200, 200, 0xFF};

		if(ei_event_get_active_widget() == widget && wid->relief != ei_relief_sunken) {
			dark.red = 0;
			dark.green = 200;
			dark.blue = 200;
			light.red = 100;
			light.green = 255;
			light.blue = 255;
		}
		/* Draws the top part of the button */
		ei_linked_point_t *points = rounded_frame(
			widget->screen_location, wid->corner_radius, h, 10, TOP);
		ei_draw_polygon_opti(surface, pick_surface, points,
				wid->relief == ei_relief_raised ? light : dark,
				*widget->pick_color,
				clipper);
		free(points);

		/* Draws the bottom part of the button */
		points = rounded_frame(widget->screen_location,
				       wid->corner_radius, h, 10, BOTTOM);
		ei_draw_polygon_opti(surface, pick_surface, points,
				wid->relief == ei_relief_raised ? dark : light,
				*widget->pick_color,
				clipper);
		free(points);
	}
	int bord = wid->border_width;
	ei_rect_t inside =
		ei_rect(ei_point_add(widget->screen_location.top_left,
				     ei_point(bord, bord)),
			ei_size_sub(widget->screen_location.size,
				    ei_size(bord * 2, bord * 2)));

	ei_rect_t clipped = inside;
	if(clipper != NULL) {
		clipped = intersect_rect(clipped, *clipper);
	}
	if (wid->img == NULL) {
		ei_linked_point_t *points = rounded_frame(inside, wid->corner_radius, 0, 10, FULL);
		ei_draw_polygon_opti(surface, pick_surface, points, wid->color, *widget->pick_color, &clipped);
		free(points);

		int w, h;
		hw_text_compute_size(wid->text, wid->text_font, &w, &h);
		ei_point_t at = ei_point_add(
			widget->screen_location.top_left,
			ei_point(wid->relief == ei_relief_raised ? (widget->screen_location.size.width - w) / 2 : (widget->screen_location.size.width - w) / 2 + 3,
				 wid->relief == ei_relief_raised ? (widget->screen_location.size.height - h) / 2 : (widget->screen_location.size.height - h) / 2 + 1));
		ei_draw_text(surface, &at, wid->text, wid->text_font,
			     &wid->text_color, &clipped);
	} else {
		ei_copy_surface(surface, &inside, wid->img, wid->img_rect, EI_TRUE);
	}
}

void button_setdefaultsfunc(struct ei_widget_t *widget)
{
	ei_widget_button_t *wid = (ei_widget_button_t *)widget;
	wid->color = (ei_color_t){0, 0, 0, 0};
	wid->border_width = k_default_button_border_width;
	wid->corner_radius = k_default_button_corner_radius;
	wid->relief = ei_relief_none;
	wid->text = NULL;
	wid->text_font = ei_default_font;
	wid->text_color = ei_font_default_color;
	wid->text_anchor = ei_anc_center;
	wid->img = NULL;
	wid->img_rect = NULL;
	wid->img_anchor = ei_anc_center;
}

void button_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect) {
	ei_app_invalidate_rect(&widget->screen_location);
	ei_app_invalidate_rect(&rect);

	widget->screen_location = rect;
}


ei_bool_t button_handlefunc(struct ei_widget_t *widget,
			    struct ei_event_t *event)
{
	ei_widget_button_t *button = (ei_widget_button_t *)widget;
	if (event->type == ei_ev_mouse_buttondown && ei_widget_pick(&event->param.mouse.where) == widget) {
		ei_event_set_active_widget(widget);
		button->relief = ei_relief_sunken;
        	ei_app_invalidate_rect(&widget->screen_location);
	} else if (event->type == ei_ev_mouse_buttonup) {
		if (button->relief == ei_relief_sunken &&
		    button->callback != NULL &&
		    ei_event_get_active_widget() == widget) {
			button->callback(widget, event, button->user_param);
		}
		ei_event_set_active_widget(NULL);
		button->relief = ei_relief_raised;
		ei_app_invalidate_rect(&widget->screen_location);
	} else if (event->type == ei_ev_mouse_move &&
		   ei_event_get_active_widget() == widget) {
		ei_bool_t inside = ei_widget_pick(&event->param .mouse.where) == widget;
		if (!inside && button->relief == ei_relief_sunken) {
			button->relief = ei_relief_raised;
			ei_app_invalidate_rect(&widget->screen_location);
		}
		if (inside && button->relief == ei_relief_raised) {
			button->relief = ei_relief_sunken;
			ei_app_invalidate_rect(&widget->screen_location);
		}
	} else if(event->type == ei_ev_keydown && ei_event_get_active_widget() == widget) {
		if(event->param.key.key_sym == SDLK_RETURN) {
			button->relief = ei_relief_sunken;
        		ei_app_invalidate_rect(&widget->screen_location);
			button->callback(widget, event, button->user_param);
		}
	} else if(event->type == ei_ev_keyup && ei_event_get_active_widget() == widget) {
		if(event->param.key.key_sym == SDLK_RETURN) {
        		button->relief = ei_relief_raised;
			ei_event_set_active_widget(NULL);
			ei_app_invalidate_rect(&widget->screen_location);
		}
	}
	return EI_TRUE;
}
