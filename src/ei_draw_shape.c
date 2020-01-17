#include "ei_draw_shape.h"
#include "ei_utils.h"
#include "ei_types.h"
#include "ei_draw.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ei_bool_t do_intersect(ei_rect_t rect1, ei_rect_t rect2)
{
	return rect1.top_left.x <= rect2.top_left.x + rect2.size.width &&
	       rect1.top_left.y <= rect2.top_left.y + rect2.size.height &&
	       rect2.top_left.x <= rect1.top_left.x + rect1.size.width &&
	       rect2.top_left.y <= rect1.top_left.y + rect1.size.height;
}

ei_bool_t inside_rect(ei_point_t point, ei_rect_t rect)
{
	return point.x >= rect.top_left.x &&
	       point.x < rect.top_left.x + rect.size.width &&
	       point.y >= rect.top_left.y &&
	       point.y < rect.top_left.y + rect.size.height;
}

ei_rect_t merge_rect(ei_rect_t rect1, ei_rect_t rect2)
{
	ei_rect_t rect = ei_rect_zero();
	ei_point_t bottom_right1 =
		ei_point_add(rect1.top_left, ei_size_as_point(rect1.size));
	ei_point_t bottom_right2 =
		ei_point_add(rect2.top_left, ei_size_as_point(rect2.size));
	rect.top_left.x = rect1.top_left.x < rect2.top_left.x
				  ? rect1.top_left.x
				  : rect2.top_left.x;
	rect.top_left.y = rect1.top_left.y < rect2.top_left.y
				  ? rect1.top_left.y
				  : rect2.top_left.y;
	bottom_right1.x = bottom_right1.x < bottom_right2.x ? bottom_right2.x
							    : bottom_right1.x;
	bottom_right1.y = bottom_right1.y < bottom_right2.y ? bottom_right2.y
							    : bottom_right1.y;
	rect.size.width = bottom_right1.x - rect.top_left.x;
	rect.size.height = bottom_right1.y - rect.top_left.y;
	return rect;
}

ei_rect_t expand_rect(ei_rect_t rect, int left, int up, int right, int down) {
	return ei_rect(ei_point_sub(rect.top_left, ei_point(left, up)),
		       ei_size_add(rect.size, ei_size(left + right, up + down)));
}


int area_rect(ei_rect_t rect)
{
	return rect.size.width * rect.size.height;
}

/* Returns the intersection of rect1 and rect2 */
ei_rect_t intersect_rect(ei_rect_t rect1, ei_rect_t rect2)
{
	ei_rect_t rect = ei_rect_zero();
	ei_point_t bottom_right1 =
		ei_point_add(rect1.top_left, ei_size_as_point(rect1.size));
	ei_point_t bottom_right2 =
		ei_point_add(rect2.top_left, ei_size_as_point(rect2.size));
	rect.top_left.x = rect1.top_left.x < rect2.top_left.x
				  ? rect2.top_left.x
				  : rect1.top_left.x;
	rect.top_left.y = rect1.top_left.y < rect2.top_left.y
				  ? rect2.top_left.y
				  : rect1.top_left.y;
	bottom_right1.x = bottom_right1.x < bottom_right2.x ? bottom_right1.x
							    : bottom_right2.x;
	bottom_right1.y = bottom_right1.y < bottom_right2.y ? bottom_right1.y
							    : bottom_right2.y;
	rect.size.width = (bottom_right1.x - rect.top_left.x) < 0 ? 0 : (bottom_right1.x - rect.top_left.x);
	rect.size.height = (bottom_right1.y - rect.top_left.y) < 0 ? 0 : (bottom_right1.y - rect.top_left.y) ;
	return rect;
}

void print_rect(ei_rect_t rect)
{
	printf("%i, %i  |  %i, %i\n", rect.top_left.x, rect.top_left.y, rect.size.width, rect.size.height);
}

void print_point(ei_point_t point)
{
	printf("%i, %i\n", point.x, point.y);
}

void arc(ei_point_t centre, int rayon, float debut, float fin,
	 ei_linked_point_t *points, int res)
{
	float pas = (fin - debut) / res;
	for (int i = 0; i < res; i++) {
		float angle = debut + i * pas;
		points[i].point.x = centre.x + round(cos(angle) * rayon);
		points[i].point.y = centre.y - round(sin(angle) * rayon);
		if (i < res - 1)
			points[i].next = &(points[i + 1]);
		else
			points[i].next = NULL;
	}
}

ei_linked_point_t *rounded_frame(ei_rect_t rect, int rayon, int h, int res,
				 rounded_t type)
{
	float PI = 3.141592653f;
	ei_linked_point_t *points;
	uint32_t len;
	switch(type) {
	case FULL:
		len = 4 * res;
		points = malloc(len * sizeof(ei_linked_point_t));
		arc(ei_point_add(rect.top_left, ei_point(rayon, rayon)), rayon,
		PI / 2, PI, points, res);
		points[res - 1].next = &(points[res]);
		arc(ei_point_add(rect.top_left,
				ei_point(rayon, rect.size.height - rayon)),
		rayon, PI, PI / 2 * 3, points + res, res);
		points[2 * res - 1].next = &(points[2 * res]);
		arc(ei_point_add(rect.top_left,
				ei_point(rect.size.width - rayon,
					rect.size.height - rayon)),
		rayon, PI / 2 * 3, PI * 2, points + 2 * res, res);
		points[3 * res - 1].next = &(points[3 * res]);
		arc(ei_point_add(rect.top_left,
				ei_point(rect.size.width - rayon, rayon)),
		rayon, 0, PI / 2, points + 3 * res, res);
		break;
	case TOP:
		res = res == 1 ? 2
			       : (res / 2) * 2; /* making sure res is even */
		len = 2 * res + 2;
		points = malloc(len * sizeof(ei_linked_point_t));
		arc(ei_point_add(rect.top_left, ei_point(rayon, rayon)), rayon,
		    PI / 2, PI, points, res);
		points[res - 1].next = &(points[res]);
		arc(ei_point_add(rect.top_left,
				 ei_point(rayon, rect.size.height - rayon)),
		    rayon, PI, PI / 4 * 5 + PI / 2 / res, points + res,
		    res / 2);
		points[res + res / 2 - 1].next = &(points[res + res / 2]);

		points[res + res / 2].point =
			ei_point(rect.top_left.x + h,
				 rect.top_left.y + rect.size.height - h);
		points[res + res / 2].next = &(points[res + res / 2 + 1]);
		points[res + res / 2 + 1].point =
			ei_point(rect.top_left.x + rect.size.width - h,
				 rect.top_left.y + h);
		points[res + res / 2 + 1].next = &(points[res + res / 2 + 2]);
		arc(ei_point_add(rect.top_left,
				 ei_point(rect.size.width - rayon, rayon)),
		    rayon, PI / 4, PI / 2, points + res + res / 2 + 2, res / 2);
		break;
	case BOTTOM:
		res = res == 1 ? 2
			       : (res / 2) * 2; /* making sure res is even */
		len = 2 * res + 2;
		points = malloc(len * sizeof(ei_linked_point_t));
		arc(ei_point_add(rect.top_left,
				 ei_point(rect.size.width - rayon,
					  rect.size.height - rayon)),
		    rayon, PI / 2 * 3, PI * 2, points, res);
		points[res - 1].next = &(points[res]);
		arc(ei_point_add(rect.top_left,
				 ei_point(rect.size.width - rayon, rayon)),
		    rayon, 0, PI / 4 + PI / 2 / res, points + res, res / 2);
		points[res + res / 2 - 1].next = &(points[res + res / 2]);

		points[res + res / 2].point =
			ei_point(rect.top_left.x + rect.size.width - h,
				 rect.top_left.y + h);
		points[res + res / 2].next = &(points[res + res / 2 + 1]);
		points[res + res / 2 + 1].point =
			ei_point(rect.top_left.x + h,
				 rect.top_left.y + rect.size.height - h);
		points[res + res / 2 + 1].next = &(points[res + res / 2 + 2]);
		arc(ei_point_add(rect.top_left,
				 ei_point(rayon, rect.size.height - rayon)),
		    rayon, PI / 4 * 5, PI / 2 * 3, points + res + res / 2 + 2,
		    res / 2);
		break;
	case TOPLEVEL:
		len = 2 * res + 2;
		points = malloc(len * sizeof(ei_linked_point_t));
		arc(ei_point_add(rect.top_left, ei_point(rayon, rayon)), rayon,
		    PI / 2, PI, points, res);
		points[res - 1].next = &(points[res]);
		points[res].point = ei_point_add(rect.top_left,
						 ei_point(0, rect.size.height));
		points[res].next = &(points[res + 1]);
		points[res + 1].point = ei_point_add(
			rect.top_left, ei_size_as_point(rect.size));
		points[res + 1].next = &(points[res + 2]);
		arc(ei_point_add(rect.top_left,
				 ei_point(rect.size.width - rayon, rayon)),
		    rayon, 0, PI / 2, points + res + 2, res);
		break;
	}
	return points;
}

void draw_pixel_opti(uint32_t *pixel_ptr, int x, int y, int width, uint32_t col) {
	//if(x < 0 || y < 0 || x >= 800 || y >= 600)
	//	printf("bug : %i %i\n", x,y);
	*(pixel_ptr + x + y * width) = col;
}

ei_bool_t clip_line(ei_point_t p1, ei_point_t p2, ei_point_t *cp1, ei_point_t *cp2, const ei_rect_t *clipper) {
	if(clipper == NULL) {
		*cp1 = p1;
		*cp2 = p2;
		return EI_TRUE;
	}
	uint8_t edge1 = (clipper->top_left.y + clipper->size.height - p1.y < 0) << 3 |
			     (p1.y - clipper->top_left.y < 0) << 2 |
			     (clipper->top_left.x + clipper->size.width - p1.x < 0) << 1 |
			     (p1.x - clipper->top_left.x < 0);

	uint8_t edge2 = (clipper->top_left.y + clipper->size.height - p2.y < 0) << 3 |
			     (p2.y - clipper->top_left.y < 0) << 2 |
			     (clipper->top_left.x + clipper->size.width - p2.x < 0) << 1 |
			     (p2.x - clipper->top_left.x < 0);


	if(!(edge1 | edge2)) { //fully in
		*cp1 = p1;
		*cp2 = p2;
		return EI_TRUE;
	} else if(edge1 & edge2) { //fully outside
		return EI_FALSE;
	}
	uint8_t to_cut = edge1 == 0 ? edge2 : edge1;
	ei_point_t stay = edge1 == 0 ? p1 : p2;
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	ei_point_t inter;

	if(to_cut & 0x1) { //left edge
		inter.x = clipper->top_left.x;
		inter.y = p1.y + (inter.x - p1.x) * (dx == 0 ? 0 : dy / (float)dx);
	} else if(to_cut & 0x8) { //bottom edge
		inter.y = clipper->top_left.y + clipper->size.height - 1;
		if(p1.x == clipper->top_left.x + clipper->size.width) //infinte recursion bug
			inter.y++;
		inter.x = p1.x + (inter.y - p1.y) * (dy == 0 ? 0 : dx / (float)dy);
	}
	else if(to_cut & 0x2) { //right edge
		inter.x = clipper->top_left.x + clipper->size.width - 1;
		inter.y = p1.y - (p1.x - inter.x) * (dx == 0 ? 0 : dy / (float)dx);
	}
	else { //top edge
		inter.y = clipper->top_left.y;
		inter.x = p1.x + (inter.y - p1.y) * (dy == 0 ? 0 : dx / (float)dy);
	}
	/*
	printf("to_cut %i\n", to_cut);
	print_point(p1);
	print_point(p2);
	print_point(inter);
	print_rect(*clipper);*/

	return clip_line(stay, inter, cp1, cp2, clipper);
}

void draw_line(ei_surface_t surface, ei_point_t p1, ei_point_t p2,
	       const ei_color_t color, const ei_rect_t *clipper)
{
	ei_point_t cp1;
	ei_point_t cp2;
	if(!clip_line(p1, p2, &cp1, &cp2, clipper))
		return;

	/* algorithme de Bresenham */
	int dx = abs(cp2.x - cp1.x);
	int dy = abs(cp2.y - cp1.y);
	int sx = cp1.x < cp2.x ? 1 : -1;
	int sy = cp1.y < cp2.y ? 1 : -1;
	int e = (dx > dy ? dx : -dy) / 2;
	int e2 = 0;

	uint32_t *pixel_ptr = (uint32_t *)hw_surface_get_buffer(surface);
	uint32_t col = ei_map_rgba(surface, &color);
	int width = hw_surface_get_size(surface).width;

	while (1) {
		draw_pixel_opti(pixel_ptr, cp1.x, cp1.y, width, col);
		if (cp1.x == cp2.x && cp1.y == cp2.y)
			break;
		e2 = e;
		if (e2 > -dx) {
			e -= dy;
			cp1.x += sx;
		}
		if (e2 < dy) {
			e += dx;
			cp1.y += sy;
		}
	}
}

void ei_draw_polyline_closed(ei_surface_t surface,
		      const ei_linked_point_t *first_point,
		      const ei_color_t color, const ei_rect_t *clipper)
{
	/* Pointeur sur liste chaînée de points */
	const ei_linked_point_t *link = first_point;
	/* Tant qu'il y a des points dans la liste, on trace la ligne */
	ei_bool_t loop = EI_FALSE;
        while (link->next != NULL || loop) {
		ei_point_t p1 = link->point;
		ei_point_t p2 = loop ? first_point->point : link->next->point;
		draw_line(surface, p1, p2, color, clipper);
		if(loop)
                        break;
                if(link->next->next == NULL)
                        loop = EI_TRUE;
                link = link->next;
	}
}

void free_polygon(ei_linked_point_t *poly) {
	ei_linked_point_t *curr = poly;
	while(curr != NULL) {
		ei_linked_point_t *to_free = curr;
		curr = curr->next;
		free(to_free);
	}
}

ei_linked_point_t* clip_polygon(const ei_linked_point_t *poly, const ei_rect_t *clipper) {
	if(clipper == NULL)
		return (ei_linked_point_t *)poly;
	ei_linked_point_t* last_poly = (ei_linked_point_t *)poly;
	for(int i = 0; i < 4; i++) {
		ei_linked_point_t *new_poly = NULL;
		ei_linked_point_t *curr = last_poly;
		ei_bool_t loop = EI_FALSE;
        	while (curr->next != NULL || loop) {
			ei_point_t p1 = curr->point;
			ei_point_t p2 = loop ? last_poly->point : curr->next->point;
			ei_point_t inter;

			uint8_t case_edge = 0;  //p inside | s inside
			int dx = p2.x - p1.x;
			int dy = p2.y - p1.y;
			switch(i) {
			case 0:  //left edge
				inter.x = clipper->top_left.x;
				inter.y = p1.y + (inter.x - p1.x) * (dx == 0 ? 0 : dy / (float)dx);
				case_edge = ((p1.x >= inter.x) << 1) | (p2.x >= inter.x);
				break;
			case 1: //bottom edge
				inter.y = clipper->top_left.y + clipper->size.height;

				inter.x = p1.x + (inter.y - p1.y) * (dy == 0 ? 0 : dx / (float)dy);
				case_edge = ((p1.y < inter.y) << 1) | (p2.y < inter.y);
				break;
			case 2: //right edge
				inter.x = clipper->top_left.x + clipper->size.width;

				inter.y = p1.y - (p1.x - inter.x) * (dx == 0 ? 0 : dy / (float)dx);
				case_edge = ((p1.x < inter.x) << 1) | (p2.x < inter.x);
				break;
			case 3: //top edge
				inter.y = clipper->top_left.y;
				inter.x = p1.x + (inter.y - p1.y) * (dy == 0 ? 0 : dx / (float)dy);
				case_edge = ((p1.y >= inter.y) << 1) | (p2.y >= inter.y);
				break;
			}
			ei_linked_point_t *new_p = malloc(sizeof(ei_linked_point_t));
			switch(case_edge) {
			case 1: //s inside
				new_p->point = inter;
				new_p->next = new_poly;
				ei_linked_point_t *new_p1 = malloc(sizeof(ei_linked_point_t));
				new_p1->point = p2;
				new_p1->next = new_p;
				new_poly = new_p1;
				break;
			case 2: //p inside
				new_p->point = inter;
				new_p->next = new_poly;
				new_poly = new_p;
				break;
			case 3: //both inside
				new_p->point = p2;
				new_p->next = new_poly;
				new_poly = new_p;
				break;
			default:
				free(new_p);
			}
			ei_linked_point_t *to_free = curr;

			if(loop) {
				if(i == 3) free(to_free);
                        	break;
			}
			if(curr->next->next == NULL)
				loop = EI_TRUE;
			curr = curr->next;
			if(i > 0 && to_free != last_poly)
				free(to_free);
		}
		if(new_poly == NULL) {
			return NULL;
		}

		last_poly = new_poly;
	}
	return last_poly;
}

void sort(struct side *sides)
{
	/* bubble sort */
	struct side *s1 = sides;
	struct side *s2;

	while (s1 != NULL) {
		s2 = s1->next;
		while (s2 != NULL) {
			if (s2->xymin < s1->xymin) {
				/* swap variables */
				int tmpi = s1->ymax;
				s1->ymax = s2->ymax;
				s2->ymax = tmpi;

				float tmp = s1->xymin;
				s1->xymin = s2->xymin;
				s2->xymin = tmp;

				tmp = s1->slope;
				s1->slope = s2->slope;
				s2->slope = tmp;
			}
			s2 = s2->next;
		}
		s1 = s1->next;
	}
}

void ei_draw_polygon_opti(ei_surface_t surface1, ei_surface_t surface2, const ei_linked_point_t *first_point,
		     const ei_color_t color1, const ei_color_t color2, const ei_rect_t *clipper)
{

	ei_linked_point_t *clipped = clip_polygon(first_point, clipper);
	if(clipped == NULL)
		return;
	/* founding the TC size */
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

	uint32_t *pixel_ptr1 = (uint32_t *)hw_surface_get_buffer(surface1);
	uint32_t *pixel_ptr2 = (uint32_t *)hw_surface_get_buffer(surface2);
	uint32_t col1 = ei_map_rgba(surface1, &color1);
	uint32_t col2 = ei_map_rgba(surface2, &color2);
	int width = hw_surface_get_size(surface1).width;

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
				draw_pixel_opti(pixel_ptr1, x, y, width, col1);
				draw_pixel_opti(pixel_ptr2, x, y, width, col2);
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
