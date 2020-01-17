#include "ei_widget_entry.h"

#include "ei_draw_shape.h"

#include <stdlib.h>
#include <string.h>

#include "ei_application.h"
#include "ei_utils.h"

void *entry_allocfunc()
{
	return malloc(sizeof(ei_widget_entry_t));
}

void entry_releasefunc(struct ei_widget_t *widget)
{
	ei_app_invalidate_rect(&widget->parent->screen_location);

	free(widget->placer_params);
	free(widget->pick_color);
	free(widget);
}


void entry_drawfunc(ei_widget_t *widget, ei_surface_t surface,
		    ei_surface_t pick_surface, ei_rect_t *clipper)
{
	ei_widget_entry_t *entry = (ei_widget_entry_t *)widget;

	ei_color_t black = {0, 0, 0, 255};

	ei_linked_point_t *points = rounded_frame(
		widget->screen_location, entry->corner_radius, 0, 5, FULL);
	ei_draw_polygon_opti(surface, pick_surface, points, entry->color, *widget->pick_color, clipper);
	ei_draw_polyline_closed(surface, points, black, clipper);
	free(points);

	if (ei_event_get_active_widget() == widget) {
		ei_rect_t large = expand_rect(widget->screen_location, -1, -1, -1, -1);
		points = rounded_frame(large, entry->corner_radius, 0, 5, FULL);
		ei_draw_polyline_closed(surface, points, entry->glow_color, clipper);
		free(points);
		points = rounded_frame(widget->screen_location, entry->corner_radius, 0, 5, FULL);
		ei_draw_polyline_closed(surface, points, entry->glow_color, clipper);
		free(points);

		if(entry->cursor) {
			ei_linked_point_t cursor[2];
			int wid, hei;
			hw_text_compute_size(entry->text, entry->text_font, &wid, &hei);
			cursor[0].point = ei_point_add(widget->screen_location.top_left,
						ei_point(wid + 7, 6));
			cursor[1].point =
				ei_point_add(cursor[0].point, ei_point(0, widget->screen_location.size.height - 10));
			cursor[0].next = &cursor[1];
			cursor[1].next = NULL;
			ei_draw_polyline(surface, cursor, entry->text_color, clipper);
		}
	}

	if(strlen(entry->text) > 0) {
		ei_point_t text_pos =
			ei_point_add(widget->screen_location.top_left, ei_point(5, 0));
		ei_draw_text(surface, &text_pos, entry->text, entry->text_font,
			&entry->text_color, clipper);
	}
}

void entry_setdefaultsfunc(struct ei_widget_t *widget)
{
	ei_widget_entry_t *entry = (ei_widget_entry_t *)widget;
	entry->color = (ei_color_t){255, 255, 255, 255};
	entry->glow_color = (ei_color_t){0, 255, 255, 200};
	entry->corner_radius = 0;
	strcpy(entry->text, "");
	widget->requested_size = ei_size(160, 30);
	entry->text_font = ei_default_font;
	entry->text_color = ei_font_default_color;
	entry->cursor = EI_FALSE;
}

void entry_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect) {
	ei_app_invalidate_rect(&widget->screen_location);
	ei_app_invalidate_rect(&rect);

	widget->screen_location = rect;
}


ei_bool_t entry_handlefunc(struct ei_widget_t *widget,
			   struct ei_event_t *event)
{
	ei_widget_entry_t *entry = (ei_widget_entry_t *)widget;
	if(event->type == ei_ev_mouse_buttondown) {
		ei_bool_t hover = ei_widget_pick(&event->param.mouse.where) == widget;
		ei_rect_t large = expand_rect(widget->screen_location, 0, 0, 1, 1);
		ei_app_invalidate_rect(&large);
		if(hover) {
			ei_event_set_active_widget(widget);
			hw_event_schedule_app(500, NULL);
			return EI_TRUE;
		} else {
			entry->cursor = EI_FALSE;
			ei_event_set_active_widget(NULL);
			return EI_FALSE;
		}
	}
	if(event->type == ei_ev_app) {
		entry->cursor = !entry->cursor;
		hw_event_schedule_app(500, NULL);

		int wid, hei;
		hw_text_compute_size(entry->text, entry->text_font, &wid, &hei);
		ei_rect_t cursor = ei_rect(ei_point_add(widget->screen_location.top_left,
						ei_point(wid + 7, 6)), ei_size(1, widget->screen_location.size.height - 9));
		ei_app_invalidate_rect(&cursor);
		return EI_TRUE;
	}
	if(event->type == ei_ev_keydown) {
		SDLKey key = event->param.key.key_sym;

		if(key == SDLK_BACKSPACE && strlen(entry->text) > 0) {
			entry->text[strlen(entry->text) - 1] = '\0';
		}
		else {
			if(ei_has_modifier(event->param.key.modifier_mask, ei_mod_shift_left) && key >= 'a' && key <= 'z') //maj
				key -= 32;
			if(key >= 256 && key <= 265) { //keypad
				key -= 208;
			}
			if((key >= ' ' && key <= 'z')) {
				char cat[2] = {key, '\0'};
				strcat(entry->text, cat);
			}
		}

		ei_app_invalidate_rect(&widget->screen_location);
		return EI_TRUE;
	}
	return EI_TRUE;
}
