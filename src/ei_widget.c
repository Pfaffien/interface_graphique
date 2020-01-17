#include "ei_widget.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ei_utils.h"
#include "ei_widget_button.h"
#include "ei_widget_frame.h"
#include "ei_widget_toplevel.h"
#include "ei_widget_entry.h"

#include "ei_draw_shape.h"

uint32_t widget_count = 0;
ei_widgetclass_t *class_list = NULL;

void ei_widgetclass_register(ei_widgetclass_t *widgetclass)
{
	widgetclass->next = class_list;
	class_list = widgetclass;
}

void ei_frame_register_class()
{
	ei_widgetclass_t *class = malloc(sizeof(ei_widgetclass_t));
	strcpy(class->name, "frame");
	class->allocfunc = &frame_allocfunc;
	class->releasefunc = &frame_releasefunc;
	class->drawfunc = &frame_drawfunc;
	class->setdefaultsfunc = &frame_setdefaultsfunc;
	class->geomnotifyfunc = &frame_geomnotifyfunc;
	class->handlefunc = NULL;
	class->next = NULL;
	ei_widgetclass_register(class);
}

void ei_button_register_class()
{
	ei_widgetclass_t *class = malloc(sizeof(ei_widgetclass_t));
	strcpy(class->name, "button");
	class->allocfunc = &button_allocfunc;
	class->releasefunc = &button_releasefunc;
	class->drawfunc = &button_drawfunc;
	class->setdefaultsfunc = &button_setdefaultsfunc;
	class->geomnotifyfunc = &button_geomnotifyfunc;
	class->handlefunc = &button_handlefunc;
	class->next = NULL;
	ei_widgetclass_register(class);
}

void ei_toplevel_register_class()
{
	ei_widgetclass_t *class = malloc(sizeof(ei_widgetclass_t));
	strcpy(class->name, "toplevel");
	class->allocfunc = &toplevel_allocfunc;
	class->releasefunc = &toplevel_releasefunc;
	class->drawfunc = &toplevel_drawfunc;
	class->setdefaultsfunc = &toplevel_setdefaultsfunc;
	class->geomnotifyfunc = &toplevel_geomnotifyfunc;
	class->handlefunc = &toplevel_handlefunc;
	class->next = NULL;
	ei_widgetclass_register(class);
}

void ei_entry_register_class()
{
	ei_widgetclass_t *class = malloc(sizeof(ei_widgetclass_t));
	strcpy(class->name, "entry");
	class->allocfunc = &entry_allocfunc;
	class->releasefunc = &entry_releasefunc;
	class->drawfunc = &entry_drawfunc;
	class->setdefaultsfunc = &entry_setdefaultsfunc;
	class->geomnotifyfunc = &entry_geomnotifyfunc;
	class->handlefunc = &entry_handlefunc;
	class->next = NULL;
	ei_widgetclass_register(class);
}

ei_widgetclass_t *ei_widgetclass_from_name(ei_widgetclass_name_t name)
{
	ei_widgetclass_t *class = class_list;
	while (class != NULL && strcmp(class->name, name) != 0)
		class = class->next;
	return class;
}

void ei_frame_configure(ei_widget_t *widget, ei_size_t *requested_size,
			const ei_color_t *color, int *border_width,
			ei_relief_t *relief, char **text, ei_font_t *text_font,
			ei_color_t *text_color, ei_anchor_t *text_anchor,
			ei_surface_t *img, ei_rect_t **img_rect,
			ei_anchor_t *img_anchor) {
	if (widget == NULL)
		return;
	ei_widget_frame_t *wid = (ei_widget_frame_t *)widget;

	if (requested_size != NULL) {
		widget->requested_size = *requested_size;
		ei_placer_run(widget);
	}
	if (color != NULL)
		wid->color = *color;
	if (border_width != NULL)
		wid->border_width = *border_width;
	if (relief != NULL)
		wid->relief = *relief;
	if (text != NULL) {
		wid->text = malloc(strlen(*text) + 1);
		strcpy(wid->text, *text);
	}
	if (text_font != NULL)
		wid->text_font = *text_font;
	if (text_color != NULL)
		wid->text_color = *text_color;
	if (text_anchor != NULL)
		wid->text_anchor = *text_anchor;
	if (img != NULL)
		wid->img = *img;
	if (img_rect != NULL)
		wid->img_rect = *img_rect;
	if (img_anchor != NULL)
		wid->img_anchor = *img_anchor;
}

void ei_button_configure(ei_widget_t *widget, ei_size_t *requested_size,
			 const ei_color_t *color, int *border_width,
			 int *corner_radius, ei_relief_t *relief, char **text,
			 ei_font_t *text_font, ei_color_t *text_color,
			 ei_anchor_t *text_anchor, ei_surface_t *img,
			 ei_rect_t **img_rect, ei_anchor_t *img_anchor,
			 ei_callback_t *callback, void **user_param)
{
	if (widget == NULL)
		return;
	ei_widget_button_t *wid = (ei_widget_button_t *)widget;

	if (color != NULL)
		wid->color = *color;
	if (border_width != NULL)
		wid->border_width = *border_width;
	if (corner_radius != NULL)
		wid->corner_radius = *corner_radius;
	if (relief != NULL)
		wid->relief = *relief;
	if (text != NULL)
		wid->text = *text;
	if (text_font != NULL)
		wid->text_font = *text_font;
	if (text_color != NULL)
		wid->text_color = *text_color;
	if (text_anchor != NULL)
		wid->text_anchor = *text_anchor;
	if (img != NULL)
		wid->img = *img;
	if (img_rect != NULL) {
		ei_rect_t *imrec = malloc(sizeof(ei_rect_t));
		imrec->size = (*img_rect)->size;
		imrec->top_left = (*img_rect)->top_left;
		wid->img_rect = imrec;
	}
	if (img_anchor != NULL)
		wid->img_anchor = *img_anchor;
	if (callback != NULL)
		wid->callback = *callback;
	if (user_param != NULL)
		wid->user_param = *user_param;
	if (requested_size != NULL)
		widget->requested_size = *requested_size;
	else {
		int w, h;
		hw_text_compute_size(wid->text, wid->text_font, &w, &h);
		widget->requested_size = ei_size(w, h);
	}
	ei_placer_run(widget);
}

void ei_toplevel_configure(ei_widget_t *widget, ei_size_t *requested_size,
			   ei_color_t *color, int *border_width, char **title,
			   ei_bool_t *closable, ei_axis_set_t *resizable,
			   ei_size_t **min_size)
{
	if (widget == NULL)
		return;
	ei_widget_toplevel_t *toplevel = (ei_widget_toplevel_t *)widget;

	if (requested_size != NULL) {
		widget->requested_size = *requested_size;
		ei_placer_run(widget);
	}
	if (color != NULL)
		toplevel->color = *color;
	if (border_width != NULL)
		toplevel->border_width = *border_width;
	if (title != NULL)
		toplevel->title = *title;
	if (closable != NULL)
		toplevel->closable = *closable;
	if (resizable != NULL)
		toplevel->resizable = *resizable;
	if (min_size != NULL)
		toplevel->min_size = *min_size;
}

void ei_entry_configure(ei_widget_t *widget, ei_size_t *requested_size,
			   ei_color_t *color, ei_color_t *glow_color, int *corner_radius,
			   char **text, ei_font_t *text_font, ei_color_t *text_color)
{
	if (widget == NULL)
		return;
	ei_widget_entry_t *entry = (ei_widget_entry_t *)widget;

	if (color != NULL)
		entry->color = *color;
	if(glow_color != NULL)
		entry->glow_color = *glow_color;
	if (corner_radius != NULL)
		entry->corner_radius = *corner_radius;
	if (text != NULL)
		strcpy(entry->text, *text);
	if (text_font != NULL)
		entry->text_font = *text_font;
	if (text_color != NULL)
		entry->text_color = *text_color;
	if (requested_size != NULL) {
		widget->requested_size = *requested_size;
		ei_placer_run(widget);
	}
}

ei_color_t *get_color_of_id()
{
	ei_color_t *color = malloc(sizeof(ei_color_t));
	color->red = (widget_count >> 16) & 0xFF;
	color->green = (widget_count >> 8) & 0xFF;
	color->blue = widget_count & 0xFF;
	color->alpha = 0xFF;
	widget_count++;
	return color;
}

ei_widget_t *ei_widget_create(ei_widgetclass_name_t class_name,
			      ei_widget_t *parent)
{
	ei_widgetclass_t *class = ei_widgetclass_from_name(class_name);
	ei_widget_t *widget = class->allocfunc();
	widget->wclass = class;
	widget->parent = parent;

	widget->pick_id = widget_count;
	widget->pick_color = get_color_of_id();
	widget->placer_params = NULL;
	widget->requested_size = ei_size_zero();
	widget->screen_location = ei_rect_zero();

	widget->wclass->setdefaultsfunc(widget);
	widget->content_rect = &widget->screen_location;


	widget->children_head = NULL;
	widget->children_tail = NULL;
	widget->next_sibling = NULL;

	if (parent != NULL) {
		if (parent->children_tail == NULL) {
			parent->children_tail = widget;
			parent->children_head = widget;
		} else {
			parent->children_tail->next_sibling = widget;
			parent->children_tail = widget;
		}
	}
	return widget;
}

void dfs_post(ei_widget_t *widget, void (*cb)()) {
	ei_widget_t *son = widget->children_head;
	while(son != NULL) {
		ei_widget_t *next_son = son->next_sibling;
		dfs_post(son, cb);
		son = next_son;
	}
	(*cb)(widget);
}

void widget_remove(ei_widget_t *widget) {
	ei_widget_t *parent = widget->parent;
	if(parent != NULL) {
		if (widget == parent->children_head) {
			parent->children_head = widget->next_sibling;
			if (widget == parent->children_tail)
				parent->children_tail = NULL;
		} else {
			ei_widget_t *curr = parent->children_head;
			while (curr != NULL) {
				if (curr->next_sibling == widget)
					break;
				curr = curr->next_sibling;
			}
			if (curr != NULL) {
				if (widget == parent->children_tail)
					parent->children_tail = curr;
				curr->next_sibling = widget->next_sibling;
			} else
				printf("widget not found in hierachy : %s\n", widget->wclass->name);
		}
	}
	widget->children_head = NULL;
	widget->children_tail = NULL;

	widget->wclass->releasefunc(widget);
}


void ei_widget_destroy(ei_widget_t *widget)
{
	dfs_post(widget, widget_remove);
}


void ei_widget_set_destroy_cb(ei_widget_t *widget, ei_callback_t callback,
			      void *user_param)
{
	(void)widget;
	(void)callback;
	(void)user_param;
}
