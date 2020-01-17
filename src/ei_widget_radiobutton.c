#include <stdlib.h>

#include "ei_widget_radiobutton.h"
#include "ei_draw_shape.h"
#include "ei_application.h"
#include "ei_utils.h"

void *radiobutton_alloc_func()
{
        return malloc(sizeof(ei_widget_radiobutton_group_t));
}

void radiobutton_releasefunc(struct ei_widget_t *widget)
{
        free(widget->pick_color);
        free(widget);
}

int children(ei_widget_radiobutton_group_t group)
{
        int res = 0;
        ei_widget_radiobutton_t *current = group->first_child;
        while (current != NULL) {
                res++;
                current = current->next;
        }
        return res;
}

void unselect(struct ei_widget_radiobutton_group_t group)
{
        ei_widget_radiobutton_t *current = group->first_child;
        while (current != NULL) {
                if (current->selected) {
                        current->selected = EI_FALSE;
                        current->relief = ei_relief_sunken;
                        /*
                         * current->color = couleur du relief raised
                         * à faire
                         */
                }
                current = current->next;
        }
}

void radiobutton_drawfunc(ei_widget_t *widget,
                          ei_surface_t surface,
                          ei_surface_t pick_surface,
                          ei_rect_t *clipper)
{
        /* 
         * Dessiner un rectangle, en mettant le titre en haut, et faire en sorte de placer les boutons radio dans le rectangle (adapter la taille au nombre de boutons)
         * Dessiner les boutons comme dans le sujet (reprendre les fonctions de dessin des boutons classiques)
         * Placer le texte à côté des boutons
         */
}

void radiobutton_group_setdefaultsfunc(struct ei_widget_t *widget)
{
        ei_widget_radiobutton_group_t *wid = (ei_widget_radiobutton_group_t *)widget;
        wid->title = NULL;
        wid->width = k_default_radiobutton_width;
        wid->height = k_default_radiobutton_height;
        wid->title_font = ei_default_font;
        wid->title_color = ei_font_default_color;
        wid->group_anchor = ei_anc_center;
        wid->first_child = NULL;
        wid->number_of_buttons = 0;
}

void radiobutton_setdefaultsfunc(struct ei_widget_t *widget)
{
        ei_widget_radiobutton_group_t *wid = (ei_widget_radiobutton_group_t *)widget;
        wid->number_of_buttons = 1;

        ei_widget_radiobutton_t *radiobutton = (ei_widget_radiobutton_t *)group->first_child;
        radiobutton->color = (ei_color_t){0, 0, 0, 0};
        radiobutton->border_width = k_default_button_border_width;      // à voir ...
        radiobutton->relief = ei_relief_none;
        radiobutton->text = NULL;
        radiobutton->text_font = ei_default_font;
        radiobutton->text_color = ei_font_default_color;
        radiobutton->text_anchor = ei_anc_center;
        radiobutton->radiobutton_anchor = ei_anc_center;
        radiobutton->others = NULL;
        radiobutton->selected = EI_FALSE;
}

ei_bool_t radiobutton_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event)
{
        /* 
         * Lors d'un ei_ev_mouse_buttondown sur un bouton :
         *
         * Au départ, tous les boutons sont désélectionnés (ei_relief_raised, selected = EI_FALSE, color = couleur par défaut)
         * Dès qu'on clique sur un bouton, tous les autres sont désélectionnés (fonction unselect)
         * Ensuite, le bouton choisi est sélectionné (ei_relief_sunken, selected = EI_TRUE, color = couleur de sélection)
         * Si on clique sur un bouton sélectionné, cela le désélectionne (pas besoin d'utiliser unselect) */

    /* OSEF des autres événements de boutons je suppose (quand le bouton de souris reste enfoncé mais qu'on la déplace ...) */
}
