#include "ei_application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ei_event.h"
#include "ei_draw_shape.h"
#include "ei_utils.h"

ei_surface_t root_surface;
ei_surface_t pick_surface;
ei_widget_t *root_widget;

ei_bool_t close_requested = EI_FALSE;

ei_linked_rect_t *invalid_rects;

ei_widget_t *active_widget = NULL;
ei_default_handle_func_t default_handler;

void ei_app_create(ei_size_t *main_window_size, ei_bool_t fullscreen)
{
	hw_init();
	root_surface = hw_create_window(main_window_size, fullscreen);
	pick_surface = hw_surface_create(root_surface, main_window_size,
					 EI_FALSE);
	ei_frame_register_class();
	ei_button_register_class();
	ei_toplevel_register_class();
	ei_entry_register_class();

	root_widget = ei_widget_create("frame", NULL);
	root_widget->screen_location = hw_surface_get_rect(root_surface);
	ei_app_invalidate_rect(&(root_widget->screen_location));
}

ei_widget_t *ei_app_root_widget()
{
	return root_widget;
}

ei_surface_t ei_app_root_surface()
{
	return root_surface;
}

void ei_event_set_active_widget(ei_widget_t *widget)
{
	active_widget = widget;
}

ei_widget_t *ei_event_get_active_widget()
{
	return active_widget;
}

void ei_event_set_default_handle_func(ei_default_handle_func_t func)
{
	default_handler = func;
}

ei_default_handle_func_t ei_event_get_default_handle_func()
{
	return default_handler;
}

void dfs(ei_widget_t *widget, ei_rect_t clipper, void (*cb)()) {
	(*cb)(widget, clipper);
	ei_widget_t *son = widget->children_head;
	while(son != NULL) {
		dfs(son, clipper, cb);
		son = son->next_sibling;
	}
}

ei_widget_t *pick_dfs(ei_widget_t *widget, uint32_t pick_id) {
	if(widget->pick_id == pick_id)
		return widget;
	ei_widget_t *curr = widget->children_head;
	while(curr != NULL) {
		ei_widget_t *picked = pick_dfs(curr, pick_id);
		if(picked != NULL)
			return picked;
		curr = curr->next_sibling;
	}
	return NULL;
}

ei_widget_t *ei_widget_pick(ei_point_t *where)
{
	uint8_t *pixels = hw_surface_get_buffer(pick_surface);
	int wid = hw_surface_get_size(pick_surface).width;
	int mouse = (where->x + where->y * wid) * 4;
	ei_color_t pick_color = {*(pixels + mouse + 2),
				 *(pixels + mouse + 1),
				 *(pixels + mouse), 0xFF};
	uint32_t pick_id = ei_map_rgba(pick_surface, &pick_color);
	//printf("pick id : %i \n", pick_id);

	return pick_dfs(root_widget, pick_id);
}

void handle_keyboard_intern(ei_event_t *event) {
	if(event->type == ei_ev_keydown) {
		if(event->param.key.key_sym == '\t') {
			ei_bool_t on_next = EI_FALSE;
			ei_widget_t *first = NULL;
			ei_widget_t *parent = root_widget;
			while (parent != NULL) {
				ei_widget_t *curr = parent->children_head;
				while (curr != NULL) {
					if(strcmp(curr->wclass->name, "entry") == 0 ||
					   strcmp(curr->wclass->name, "button") == 0) {
						if(first == NULL)
							first = curr;
						if(on_next || ei_event_get_active_widget() == NULL) {
							ei_event_set_active_widget(curr);
							ei_app_invalidate_rect(&parent->screen_location);
							return;
						}
						if(ei_event_get_active_widget() == curr)
							on_next = EI_TRUE;
					}
					curr = curr->next_sibling;
				}
				if (parent->children_head != NULL)
					parent = parent->children_head;
				else if (parent->next_sibling != NULL)
					parent = parent->next_sibling;
				else
					parent = parent->parent->next_sibling;
			}
			if(first != NULL) {
				ei_app_invalidate_rect(&first->parent->screen_location);
				ei_event_set_active_widget(first);
			}
		}
	}
}

void handle_event()
{
	ei_event_t event;
	hw_event_wait_next(&event);
	ei_widget_t *picked = NULL;
	if(event.type >= ei_ev_mouse_buttondown)
		picked = ei_widget_pick(&event.param.mouse.where);

	ei_bool_t consumed = EI_FALSE;
	while(!consumed) {
		if (active_widget != NULL)
			consumed = active_widget->wclass->handlefunc(active_widget, &event);
		else if (picked != NULL && picked->wclass->handlefunc != NULL)
			consumed = picked->wclass->handlefunc(picked, &event);
		else {
			consumed = EI_TRUE;
		}
		if (default_handler != NULL) {
			if(default_handler(&event)) {
				//forces redraw
				//ei_rect_t zero = ei_rect_zero();
				//ei_app_invalidate_rect(&zero);

				ei_app_invalidate_rect(&(root_widget->screen_location));
			}
		}
		handle_keyboard_intern(&event);
	}
}

void ei_app_invalidate_rect(ei_rect_t *rect)
{
	ei_linked_rect_t *lr = malloc(sizeof(ei_linked_rect_t));
	lr->rect = *rect;
	lr->next = NULL;
	lr->next = invalid_rects;
	invalid_rects = lr;
}

void free_invalids()
{
	ei_linked_rect_t *clipper = invalid_rects;
	while (clipper != NULL) {
		ei_linked_rect_t *toFree = clipper;
		clipper = clipper->next;
		free(toFree);
	}
	invalid_rects = NULL;
}

void optimize_invalids()
{
	ei_linked_rect_t *r1 = invalid_rects;
	ei_linked_rect_t *r2;

	while(r1 != NULL) {
		r1->rect = intersect_rect(r1->rect, root_widget->screen_location);
		r1 = r1->next;
	}

	r1 = invalid_rects;

	while (r1 != NULL) {
		r2 = r1->next;
		while (r2 != NULL) {
			if (area_rect(r2->rect) > area_rect(r1->rect)) {
				/* swap rects */
				ei_rect_t tmp = r1->rect;
				r1->rect = r2->rect;
				r2->rect = tmp;
			}
			r2 = r2->next;
		}
		r1 = r1->next;
	}

	ei_linked_rect_t *first_loop = invalid_rects;
	while (first_loop != NULL) {
		int area_first = area_rect(first_loop->rect);
		if(area_first <= 0) {
			first_loop = first_loop->next;
			continue;
		}
		ei_linked_rect_t *second_loop = first_loop->next;
		while (second_loop != NULL) {
			if (do_intersect(first_loop->rect, second_loop->rect)) {
				ei_rect_t merge = merge_rect(first_loop->rect, second_loop->rect);
				if (area_rect(merge) < area_first + area_rect(second_loop->rect)) {
					first_loop->rect = merge;
					second_loop->rect = ei_rect_zero();
				}
			}
			second_loop = second_loop->next;
		}
		first_loop = first_loop->next;
	}
}

void draw_callback(ei_widget_t *widget, ei_rect_t clipper) {
	if ((widget == root_widget || widget->placer_params != NULL) && do_intersect(widget->screen_location, clipper)) {
		ei_rect_t clipped = clipper;
		if(widget->parent != NULL)
			clipped = intersect_rect(clipped, widget->parent->screen_location);

		//printf("drawing %s : ", widget->wclass->name);
		//print_rect(clipped);
		widget->wclass->drawfunc(widget, root_surface, pick_surface, &clipped);
	}
}

void draw()
{
	//printf("redraw\n");
	optimize_invalids();

	hw_surface_lock(root_surface);
	hw_surface_lock(pick_surface);
	ei_linked_rect_t *clipper = invalid_rects;
	while (clipper != NULL) {
		if (area_rect(clipper->rect) == 0) {
			clipper = clipper->next;
			continue;
		}
		dfs(root_widget, clipper->rect, draw_callback);
		clipper = clipper->next;
	}

	hw_surface_unlock(root_surface);
	hw_surface_unlock(pick_surface);


	hw_surface_update_rects(root_surface, invalid_rects);

	free_invalids();
}

void ei_app_run()
{
	while (!close_requested) {
		draw();
		while (invalid_rects == NULL)
			handle_event();
	}
}

void ei_app_quit_request()
{
	close_requested = EI_TRUE;
}

void ei_app_free()
{
	ei_widget_destroy(root_widget);
	hw_surface_free(pick_surface);
	hw_quit();
}
