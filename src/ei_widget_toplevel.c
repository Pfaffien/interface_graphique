#include "ei_widget_toplevel.h"

#include "ei_application.h"
#include "ei_draw_shape.h"
#include "ei_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "ei_placer.h"

void *toplevel_allocfunc()
{
	return malloc(sizeof(ei_widget_toplevel_t));
}

void toplevel_releasefunc(struct ei_widget_t *widget)
{
	ei_app_invalidate_rect(&widget->parent->screen_location);
	ei_event_set_active_widget(NULL);

	free(widget->placer_params);
	free(widget->pick_color);
	free(widget);
}


void toplevel_drawfunc(ei_widget_t *widget, ei_surface_t surface,
		       ei_surface_t pick_surface, ei_rect_t *clipper)
{
	ei_widget_toplevel_t *toplevel = (ei_widget_toplevel_t *)widget;
	ei_color_t dark = {99, 105, 112, 0xFF};
	ei_color_t red = {221, 0, 0, 0xFF};
	ei_color_t dark_red = {143, 24, 26, 0xFF};
	ei_color_t light_red = {254, 63, 63, 0xFF};

	ei_rect_t *clipped = clipper;
	if(clipper != NULL)
		clipped->size.width = widget->screen_location.size.width + (widget->screen_location.top_left.x - clipper->top_left.x) + toplevel->border_width;

	//border and top
	int bord = toplevel->border_width;
	ei_rect_t exterior = expand_rect(widget->screen_location, bord, 30, bord, bord);
	ei_linked_point_t *points = rounded_frame(exterior, 10, 0, 10, TOPLEVEL);
	ei_draw_polygon_opti(surface, pick_surface, points, dark, *widget->pick_color, clipper);
	free(points);

	//inside
	points = rounded_frame(widget->screen_location, 0, 0, 1, FULL);
	ei_draw_polygon_opti(surface, pick_surface, points, toplevel->color, *widget->pick_color, clipped);
	free(points);

	//close button
	points = rounded_frame(ei_rect(ei_point_add(widget->screen_location.top_left,
			       ei_point(7, -23)), ei_size(16, 16)), 8, 8, 10, TOP);
	ei_draw_polygon(surface, points, light_red, clipped);
	free(points);
	points = rounded_frame(ei_rect(ei_point_add(widget->screen_location.top_left,
			       ei_point(7, -23)), ei_size(16, 16)), 8, 8, 10, BOTTOM);
	ei_draw_polygon(surface, points, dark_red, clipped);
	free(points);
	points = rounded_frame(ei_rect(ei_point_add(widget->screen_location.top_left,
			       ei_point(9, -21)), ei_size(12, 12)), 6, 0, 10, FULL);
	ei_draw_polygon(surface, points, red, clipped);
	free(points);

	//resize button
	if(toplevel->resizable != ei_axis_none) {
		points = rounded_frame(ei_rect(ei_point_add(widget->screen_location.top_left,
				       ei_point(widget->screen_location.size.width - 10 + bord,
						widget->screen_location.size.height - 10 + bord)),
				ei_size(10, 10)), 0, 0, 1, FULL);
		ei_draw_polygon(surface, points, dark, clipper);
		free(points);
	}

	ei_point_t where = ei_point_add(widget->screen_location.top_left, ei_point(30, -28));
	ei_draw_text(surface, &where, toplevel->title, ei_default_font,
		     &ei_font_default_color, clipped);
}

void toplevel_setdefaultsfunc(struct ei_widget_t *widget)
{
	ei_widget_toplevel_t *toplevel = (ei_widget_toplevel_t *)widget;
	widget->requested_size = ei_size(320, 240);
	toplevel->color = ei_default_background_color;
	toplevel->border_width = 4;
	toplevel->title = "Toplevel";
	toplevel->closable = EI_TRUE;
	toplevel->resizable = ei_axis_both;
	ei_size_t min = ei_size(160, 120);
	toplevel->min_size = &min;
	toplevel->mouse_handle = ei_point_zero();
	toplevel->is_moving = EI_FALSE;
	toplevel->is_resizing = EI_FALSE;
}

void toplevel_geomnotifyfunc(struct ei_widget_t* widget, ei_rect_t rect) {
	//add title and border to position
	ei_widget_toplevel_t *toplevel = (ei_widget_toplevel_t *)widget;
	ei_app_invalidate_rect(&widget->screen_location);
	widget->screen_location = rect;
	widget->screen_location.top_left.x += toplevel->border_width;
	widget->screen_location.top_left.y += 30;
	ei_app_invalidate_rect(&widget->screen_location);
}


ei_bool_t toplevel_handlefunc(struct ei_widget_t *widget,
			      struct ei_event_t *event)
{
	ei_widget_toplevel_t *toplevel = (ei_widget_toplevel_t *)widget;
	ei_point_t mouse = ei_point_sub(event->param.mouse.where,
					widget->screen_location.top_left);
	if (event->type == ei_ev_mouse_buttondown) {
		if(mouse.y < 30) {
			//bring to top
			ei_widget_t *parent = widget->parent;
			if (widget != parent->children_tail) {
				if (widget == parent->children_head) {
					parent->children_head = widget->next_sibling;
				}

				if(parent->children_head->next_sibling != NULL) {
					ei_widget_t *curr = parent->children_head;
					while(curr->next_sibling != NULL && curr->next_sibling != widget) {
						curr = curr->next_sibling;
					}
					curr->next_sibling = widget->next_sibling;
				}
				parent->children_tail->next_sibling = widget;
				parent->children_tail = widget;
				widget->next_sibling = NULL;
				ei_app_invalidate_rect(parent->content_rect);
			}
		}

		//ei_point_t close = ei_point()
        	ei_point_t center_close = ei_point(15, -15);
		if ((mouse.x - center_close.x) * (mouse.x - center_close.x) +
            	    (mouse.y - center_close.y) * (mouse.y - center_close.y) <= 64) {
            		ei_widget_destroy(widget);
			return EI_TRUE;
		}


		if (mouse.x >= 25 && mouse.y <= 30) {
			toplevel->mouse_handle = event->param.mouse.where;
			toplevel->is_moving = EI_TRUE;
			ei_event_set_active_widget(widget);
		}

		if (toplevel->resizable != ei_axis_none && widget->screen_location.size.width - mouse.x <= 10 &&
		    widget->screen_location.size.height - mouse.y <= 10) {
			toplevel->mouse_handle = event->param.mouse.where;
			toplevel->is_resizing = EI_TRUE;
			ei_event_set_active_widget(widget);
		}

	} else if (event->type == ei_ev_mouse_move) {
		if (toplevel->is_resizing || toplevel->is_moving) {
			ei_point_t delta =
				ei_point_sub(event->param.mouse.where,
					     toplevel->mouse_handle);
			if(toplevel->resizable == ei_axis_x)
				delta.y = 0;
			if(toplevel->resizable == ei_axis_y)
				delta.x = 0;
			int overlap = abs(delta.x) > abs(delta.y) ? abs(delta.x) : abs(delta.y);
			int bord = toplevel->border_width;
			ei_rect_t exterior = expand_rect(widget->screen_location, bord + 1, 31, bord + overlap, bord + overlap);
			ei_app_invalidate_rect(&exterior);
			if (toplevel->is_moving) {
				toplevel->widget.screen_location.top_left.x +=
					delta.x;
				toplevel->widget.screen_location.top_left.y +=
					delta.y;
			} else if (toplevel->is_resizing) {
				toplevel->widget.screen_location.size.width +=
					delta.x;
				toplevel->widget.screen_location.size.height +=
					delta.y;
			}
			toplevel->mouse_handle = event->param.mouse.where;

			ei_widget_t *son = toplevel->widget.children_head;
			while (son != NULL) {
				ei_placer_run(son);
				son = son->next_sibling;
			}
		}
	} else if (event->type == ei_ev_mouse_buttonup) {
		ei_event_set_active_widget(NULL);
		toplevel->is_moving = EI_FALSE;
		toplevel->is_resizing = EI_FALSE;
	}
	return EI_TRUE;
}
