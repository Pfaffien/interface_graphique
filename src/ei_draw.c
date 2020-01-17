#include "ei_draw.h"

#include <stdlib.h>
#include <string.h>

#include "ei_draw_shape.h"
#include "ei_utils.h"

uint32_t ei_map_rgba(ei_surface_t surface, const ei_color_t *color)
{
	/* Renvoie la valeur de l'attribut de couleur après superposition de
	 * plusieurs fenêtres */
	int r, g, b, a;

	/* Renvoie les indices des positions des couleurs dans r, g, b et a */
	hw_surface_get_channel_indices(surface, &r, &g, &b, &a);

	/* Entier sur 32 bits correspondant à la couleur */
	uint32_t col = (color->red << (8 * r)) | (color->green << (8 * g)) |
		       (color->blue << (8 * b));

	/* Si la surface admet un attribut de transparence, on utilise aussi la
	 * composante alpha */
	if (hw_surface_has_alpha(surface))
		col |= color->alpha << (8 * a);
	return col;
}

/*
 * a : transparence de la source
 * c1 : valeur de l'attribut considéré du destinataire
 * c2 : valeur du même attribut de la source
 * Renvoie la valeur de l'attribut de couleur après superposition de plusieurs
 * fenêtres Pas mieux d'appeler la fonction get_color ?
 * */
uint8_t get_alpha(int c1, int c2, int a)
{
	return (a * c2 + (255 - a) * c1) / 255;
}

void draw_pixel(ei_surface_t surface, ei_point_t point, const ei_color_t color,
		const ei_rect_t *clipper)
{
	/* On crée la fenêtre */
	ei_rect_t wind = hw_surface_get_rect(surface);

	/* Si point est dans la fenêtre */
	if (inside_rect(point, wind)) {
		/* ? */
		if (clipper == NULL || inside_rect(point, *clipper)) {
			uint8_t *buffer = hw_surface_get_buffer(surface);
			/* Le pointeur prend la valeur retournée par ei_map_rgba,
			 On calcule l'adresse du pixel voulu*/

			*((uint32_t *)(buffer +
				       (point.x + point.y * wind.size.width) * 4)) =
				        ei_map_rgba(surface, &color);
		}
	}
}

/* Ligne brisée entre plusieurs points */
void ei_draw_polyline(ei_surface_t surface,
		      const ei_linked_point_t *first_point,
		      const ei_color_t color, const ei_rect_t *clipper)
{
	/* Pointeur sur liste chaînée de points */
	const ei_linked_point_t *link = first_point;
	/* Tant qu'il y a des points dans la liste, on trace la ligne */
	while (link->next != NULL) {
		/* algorithme de Bresenham */
		ei_point_t p1 = link->point;
		ei_point_t p2 = link->next->point;
		draw_line(surface, p1, p2, color, clipper);
		link = link->next;
	}
}

void ei_draw_polygon(ei_surface_t surface, const ei_linked_point_t *first_point,
		     const ei_color_t color, const ei_rect_t *clipper)
{
	/* founding the TC size */
	ei_linked_point_t *clipped = clip_polygon(first_point, clipper);
	if(clipped == NULL)
		return;
	int ymin = 100000;
	int ymax = -100000;
	const ei_linked_point_t *link = clipped;
	while (link != NULL) {
		ymin = link->point.y < ymin ? link->point.y : ymin;
		ymax = link->point.y > ymax ? link->point.y : ymax;
		link = link->next;
	}
	/* building the TC */
	struct side *tc[ymax - ymin + 1];
	for (int y = 0; y < ymax - ymin; y++)
		tc[y] = NULL;
	link = clipped;
	ei_bool_t loop = EI_FALSE;
        while (link->next != NULL || loop) {
		ei_point_t p1 = link->point;
		ei_point_t p2 = loop ? clipped->point : link->next->point;
		if(p1.y != p2.y) { //ignore horizontal lines
			struct side *side = malloc(sizeof(struct side));
			side->ymax = p1.y > p2.y ? p1.y : p2.y;
			side->xymin = p1.y > p2.y ? p2.x : p1.x;
			side->slope = (float)(p2.x - p1.x) / (float)(p2.y - p1.y);

			int yind = (p1.y > p2.y ? p2.y : p1.y) - ymin;
			side->next = tc[yind];
			tc[yind] = side;
		}
		if(loop)
                        break;
                if(link->next->next == NULL)
                        loop = EI_TRUE;
                link = link->next;
	}

	uint32_t *pixel_ptr = (uint32_t *)hw_surface_get_buffer(surface);
	uint32_t col = ei_map_rgba(surface, &color);
	int width = hw_surface_get_size(surface).width;

	struct side *tca = NULL;
	for (int y = ymin; y < ymax; y++) {
		struct side *tca_curr = tca;
		/* add sides of the TC */
		if (tc[y - ymin] != NULL) {
			if (tca == NULL) {
				tca = tc[y - ymin];
			} else { /* adding to queue */
				struct side *last = tca;
				while (tca_curr != NULL) { // find last of tca
					last = tca_curr;
					tca_curr = tca_curr->next;
				}
				last->next = tc[y - ymin];
			}
		}
		/* remove ending sides */
		tca_curr = tca;
		if (tca_curr != NULL) {
			struct side *last = NULL;
			while (tca_curr != NULL) {
				if (tca_curr->ymax == y) { //removing
					if (last == NULL)
						tca = tca_curr->next;
					else
						last->next = tca_curr->next;
					struct side *tmp_next = tca_curr->next;
					free(tca_curr);
					tca_curr = tmp_next;
					continue;
				}
				last = tca_curr;
				tca_curr = tca_curr->next;
			}
		}

		/* sorting */
		sort(tca);

		/* filling pixels */
		tca_curr = tca;
		while (tca_curr != NULL) {
			for (int x = ceil(tca_curr->xymin);
			     x < floor(tca_curr->next->xymin); ++x) {
				//draw_pixel(surface, ei_point(x_i, y), color, clipper);
				draw_pixel_opti(pixel_ptr, x, y, width, col);
			}
			tca_curr = tca_curr->next->next;
		}

		/* update scanline */
		tca_curr = tca;
		while (tca_curr != NULL) {
			tca_curr->xymin += tca_curr->slope;
			tca_curr = tca_curr->next;
		}
	}
	//freeing the last sides
	while (tca != NULL) {
		struct side *tmp_next = tca->next;
		free(tca);
		tca = tmp_next;
	}

	free_polygon(clipped);
}

void ei_draw_text(ei_surface_t surface, const ei_point_t *where,
		  const char *text, const ei_font_t font,
		  const ei_color_t *color, const ei_rect_t *clipper)
{
	if(text == NULL || strcmp(text, "") == 0)
		return;
	ei_surface_t sur_text = hw_text_create_surface(text, font, color);
	ei_rect_t rect = ei_rect(*where, hw_surface_get_size(sur_text));
	if (!do_intersect(rect, *clipper)) {
		hw_surface_free(sur_text);
		return;
	}
	rect = intersect_rect(rect, *clipper);
	ei_rect_t clipper_text = ei_rect(ei_point_zero(), rect.size);
	clipper_text.top_left.x = where->x >= 0 ? 0 : -where->x;
	clipper_text.top_left.y = where->y >= 0 ? 0 : -where->y;
	ei_copy_surface(surface, &rect, sur_text, &clipper_text, EI_TRUE);
	hw_surface_free(sur_text);
}

void ei_fill(ei_surface_t surface, const ei_color_t *color,
	     const ei_rect_t *clipper) {
	ei_size_t size = hw_surface_get_size(surface);
	for (int x = 0; x < size.width; x++)
		for (int y = 0; y < size.height; y++)
			draw_pixel(surface, ei_point(x, y), *color, clipper);
}

int ei_copy_surface(ei_surface_t destination, const ei_rect_t *dst_rect,
		    const ei_surface_t source, const ei_rect_t *src_rect,
		    const ei_bool_t alpha) {
	ei_size_t dst_size = hw_surface_get_size(destination);
	ei_size_t src_size = hw_surface_get_size(source);
	if (dst_rect == NULL) {
		ei_rect_t r = ei_rect(ei_point_zero(), dst_size);
		dst_rect = &r;
	}
	if (src_rect == NULL) {
		ei_rect_t r = ei_rect(ei_point_zero(), src_size);
		src_rect = &r;
	}
	if (dst_rect->size.width != src_rect->size.width ||
	    dst_rect->size.height != src_rect->size.height) {
		return 1;
	}
	uint8_t *dst_ptr = hw_surface_get_buffer(destination);
	uint8_t *src_ptr = hw_surface_get_buffer(source);
	for (int x = 0; x < dst_rect->size.width; x++) {
		for (int y = 0; y < dst_rect->size.height; y++) {
			int dst_pix =
				(x + dst_rect->top_left.x) +
				(y + dst_rect->top_left.y) * dst_size.width;
			int src_pix =
				(x + src_rect->top_left.x) +
				(y + src_rect->top_left.y) * src_size.width;
			if (alpha) {
				int al = src_ptr[src_pix * 4 + 3];
				dst_ptr[dst_pix * 4] =
					get_alpha(dst_ptr[dst_pix * 4],
						  src_ptr[src_pix * 4], al);
				dst_ptr[dst_pix * 4 + 1] =
					get_alpha(dst_ptr[dst_pix * 4 + 1],
						  src_ptr[src_pix * 4 + 1], al);
				dst_ptr[dst_pix * 4 + 2] =
					get_alpha(dst_ptr[dst_pix * 4 + 2],
						  src_ptr[src_pix * 4 + 2], al);
				dst_ptr[dst_pix * 4 + 3] = 0xFF;
			} else {
				*((uint32_t *)(dst_ptr + dst_pix * 4)) =
					*((uint32_t *)(src_ptr + src_pix * 4));
			}
		}
	}
	return 0;
}
