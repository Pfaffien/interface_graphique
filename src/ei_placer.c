#include "ei_placer.h"

#include <stdlib.h>

#include "ei_utils.h"
#include "ei_widget.h"
#include "ei_draw_shape.h"

void ei_place(struct ei_widget_t *widget, ei_anchor_t *anchor, int *x, int *y,
	      int *width, int *height, float *rel_x, float *rel_y,
	      float *rel_width, float *rel_height)
{
	if (widget == NULL)
		return;

	if(widget->placer_params != NULL) {
		if(anchor != NULL)
			widget->placer_params->anchor = anchor;
		if(x != NULL) {
			widget->placer_params->x = x;
			widget->placer_params->x_data = *x;
		}
		if(y != NULL) {
			widget->placer_params->y = y;
			widget->placer_params->y_data = *y;
		}
		if(width != NULL)
			widget->placer_params->w = width;
		if(height != NULL)
			widget->placer_params->h = height;
		if(rel_x != NULL)
			widget->placer_params->rx = rel_x;
		if(rel_y != NULL)
			widget->placer_params->ry = rel_y;
		if(rel_width != NULL)
			widget->placer_params->rw = rel_width;
		if(rel_height != NULL)
			widget->placer_params->rh = rel_height;
	} else {
		ei_placer_params_t *placer = malloc(sizeof(ei_placer_params_t));
		placer->anchor = anchor;
		placer->x = x;
		placer->y = y;
		placer->w = width;
		placer->h = height;
		placer->rx = rel_x;
		placer->ry = rel_y;
		placer->rw = rel_width;
		placer->rh = rel_height;

		placer->anchor_data = anchor == NULL ? ei_anc_northwest : *anchor;
		placer->x_data = x == NULL ? 0 : *x;
		placer->y_data = y == NULL ? 0 : *y;
		placer->rx_data = rel_x == NULL ? 0 : *rel_x;
		placer->ry_data = rel_x == NULL ? 0 : *rel_y;

		widget->placer_params = placer;
	}

	ei_placer_run(widget);
}

void ei_placer_run(struct ei_widget_t *widget)
{
	ei_placer_params_t *params = widget->placer_params;
	if(params == NULL)
		return;
	ei_rect_t *ptr_rect = &widget->parent->screen_location;

	ei_point_t top_left = ei_point_zero();
	ei_widget_t *curr = widget->parent;
	while(curr != NULL) {
		top_left = ei_point_add(top_left, curr->screen_location.top_left);
		curr = curr->parent;
	}

	int w; /* = params->width != 0 ? params->width : ptr_rect->size.width *
		  params->rel_width; */
	int h; /* = params->height != 0 ? params->height : ptr_rect->size.height
		  * params->rel_height; */
	if (params->w != NULL)
		w = *params->w;
	else if (params->rw != NULL)
		w = ptr_rect->size.width * *params->rw;
	else
		w = widget->requested_size.width;

	if (params->h != NULL)
		h = *params->h;
	else if (params->rh != NULL)
		h = ptr_rect->size.height * *params->rh;
	else
		h = widget->requested_size.height;

	int aw = ptr_rect->size.width - w;
	int ah = ptr_rect->size.height - h;

	ei_point_t anchor;
	switch (params->anchor_data) {
	case ei_anc_northwest:
		anchor = ei_point_zero();
		break;
	case ei_anc_north:
		anchor = ei_point(aw / 2, 0);
		break;
	case ei_anc_northeast:
		anchor = ei_point(aw, 0);
		break;
	case ei_anc_east:
		anchor = ei_point(aw, ah / 2);
		break;
	case ei_anc_southeast:
		anchor = ei_point(aw, ah);
		break;
	case ei_anc_south:
		anchor = ei_point(aw / 2, ah);
		break;
	case ei_anc_southwest:
		anchor = ei_point(0, ah);
		break;
	case ei_anc_west:
		anchor = ei_point(0, ah / 2);
		break;
	case ei_anc_center:
		anchor = ei_point(aw / 2, ah / 2);
		break;
	default:
		anchor = ei_point_zero();
		break;
	}
	ei_point_t real = ei_point_add(top_left, anchor);

	/* Prise en compte des coordonnées relatives */
	//int gap_x = params->rx_data * ptr_rect->size.width;
	//int gap_y = params->ry_data * ptr_rect->size.height;
	//ei_point_t gap = ei_point(gap_x, gap_y);
	//real = ei_point_add(real, gap);

	real = ei_point_add(real, ei_point(params->x_data, params->y_data));

	if(widget->wclass->geomnotifyfunc != NULL)
		widget->wclass->geomnotifyfunc(widget, ei_rect(real, ei_size(w, h)));
}

void ei_placer_forget(struct ei_widget_t *widget)
{
	free(widget->placer_params);
	widget->placer_params = NULL;
}
