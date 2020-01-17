#ifndef EI_WIDGET_RADIOBUTTON_H
#define EI_WIDGET_RADIOBUTTON_H

#include "ei_widget.h"
#include "ei_event.h"
#include "ei_types.h"

/*
 * On affiche un bouton classique en relief, blanc, mais sous forme de diamant
 * Quand il est enfoncé, on l'affiche comme étant enfoncé, avec une autre couleur et
 * on parcourt la liste chaînée des autres boutons pour déselectionner celui qui
 * l'était
 */

static const int k_default_radiobutton_width = 20;
static const int k_default_radiobutton_height = 30;

struct ei_widget_radiobutton_t;

/*
 * A radiobutton
 */
typedef struct ei_single_radio_t {

        int                 border_width;
        ei_relief_t         relief;
        char*               text;
        ei_font_t           text_font;
        ei_color_t          text_color;
        ei_anchor_t         text_anchor;
        ei_anchor_t         radiobutton_anchor;
        ei_bool_t           selected;   // tells whether the button is selected
        ei_callback_t       callback;
        void*               user_param;
} ei_single_radio_t;

/*
 * A group of radiobuttons
 */
typedef struct ei_widget_radiobutton_t {
        ei_widget_t                 widget;

        char*                       title;
        ei_font_t                   title_font;
        ei_color_t                  title_color;
        ei_anchor_t                 group_anchor;
        ei_single_radio_t*          first_child;
        int                         number_of_buttons;
} ei_widget_radiobutton_t;

void *radiobutton_allocfunc();

/* Returns the number of buttons */
int children(ei_widget_radiobutton_group_t radiobutton);

/* Unselects the selected button */
void unselect(struct ei_widget_radiobutton_group_t group);

void radiobutton_releasefunc(struct ei_widget_t *widget);

void radiobutton_drawfunc(ei_widget_t *widget,
                          ei_surface_t surface,
                          ei_surface_t pick_surface,
                          ei_rect_t *clipper);

/* Creates an empty group of radiobuttons */
void radiobutton_group_setdefaultsfunc(struct ei_widget_t *widget);

/* Creates an empty radiobutton, first child of a group */
void radiobutton_setdefaultsfunc(struct ei_widget_t *widget);

ei_bool_t radiobutton_handlefunc(struct ei_widget_t *widget, struct ei_event_t *event);

#endif /* EI_WIDGET_RADIOBUTTON_H */
