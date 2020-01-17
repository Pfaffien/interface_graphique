#include "ei_widget_frame.h"

#include <stdlib.h>

#include "ei_draw_shape.h"
#include "ei_utils.h"
#include "ei_application.h"

void *frame_allocfunc()
{
	return malloc(sizeof(ei_widget_frame_t));
}

void frame_releasefunc(struct ei_widget_t *widget)
{
	if(widget->parent != NULL)
		ei_app_invalidate_rect(&widget->parent->screen_location);

	free(widget->placer_params);
	free(widget->pick_color);
	free(widget);
}


void frame_drawfunc(ei_widget_t *widget, ei_surface_t surface,
		    ei_surface_t pick_surface, ei_rect_t *clipper)
{
	ei_widget_frame_t *wid = (ei_widget_frame_t *)widget;
	if (wid->relief != ei_relief_none) {
		int h = widget->screen_location.size.width <
					widget->screen_location.size.height
				? widget->screen_location.size.width / 2
				: widget->screen_location.size.height / 2;
		ei_color_t dark = {0x75, 0x75, 0x75, 0xFF};
		ei_color_t light = {0xA0, 0xA0, 0xA0, 0xFF};

		ei_linked_point_t *points =
			rounded_frame(widget->screen_location, 0, h, 1, TOP);
		ei_draw_polygon_opti(surface, pick_surface, points,
				wid->relief == ei_relief_raised ? light : dark,
				*widget->pick_color, clipper);
		free(points);

		points = rounded_frame(widget->screen_location, 0, h, 1, BOTTOM);

		ei_draw_polygon_opti(surface, pick_surface, points,
				wid->relief == ei_relief_raised ? dark : light,
				*widget->pick_color, clipper);
		free(points);
	}
	int bord = wid->border_width;
	ei_rect_t inside =
		ei_rect(ei_point_add(widget->screen_location.top_left,
				     ei_point(bord, bord)),
			ei_size_sub(widget->screen_location.size,
				    ei_size(bord * 2, bord * 2)));
	if (wid->img == NULL) {
		ei_linked_point_t *points = rounded_frame(inside, 0, 0, 1, FULL);

		ei_draw_polygon_opti(surface, pick_surface, points, wid->color, *widget->pick_color, clipper);
		free(points);

		int w, h;
		hw_text_compute_size(wid->text, wid->text_font, &w, &h);
		ei_point_t at = ei_point_add(
			widget->screen_location.top_left,
			ei_point(wid->relief == ei_relief_raised ? (widget->screen_location.size.width - w) / 2 : (widget->screen_location.size.width - w) / 2 + 3,
				 wid->relief == ei_relief_raised ? (widget->screen_location.size.height - h) / 2 : (widget->screen_location.size.height - h) / 2 + 1));
		ei_draw_text(surface, &at, wid->text, wid->text_font,
			     &wid->text_color, clipper);
	} else {
		ei_copy_surface(surface, &inside, wid->img, wid->img_rect,
				EI_TRUE);
	}
}

void frame_setdefaultsfunc(struct ei_widget_t *widget)
{
	ei_widget_frame_t *wid = (ei_widget_frame_t *)widget;
	wid->color = (ei_color_t){0, 0, 0, 0};
	wid->border_width = 0;
	wid->relief = ei_relief_none;
	wid->text = "";
	wid->text_font = ei_default_font;
	wid->text_color = ei_font_default_color;
	wid->text_anchor = ei_anc_center;
	wid->img = NULL;
	wid->img_rect = NULL;
	wid->img_anchor = ei_anc_center;
}

void frame_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect) {
	ei_app_invalidate_rect(&widget->screen_location);
	ei_app_invalidate_rect(&rect);
	widget->screen_location = rect;
}
