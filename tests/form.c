#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"


/*
 * button_press --
 *
 *	Callback called when a user clicks on the button.
 */
void submit(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	(void)widget;
	(void)event;
	(void)user_param;
	printf("Click !\n");
}

/*
 * process_key --
 *
 *	Callback called when any key is pressed by the user.
 *	Simply looks for the "Escape" key to request the application to quit.
 */
ei_bool_t process_key(ei_event_t* event)
{
	if (event->type == ei_ev_keydown)
		if (event->param.key.key_sym == SDLK_ESCAPE) {
			ei_app_quit_request();
			return EI_TRUE;
		}

	return EI_FALSE;
}

/*
 * ei_main --
 *
 *	Main function of the application.
 */
int ei_main(int argc, char** argv)
{
	(void)argc;
	(void)argv;
	ei_size_t	screen_size		= {800, 600};
	ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};

	ei_widget_t*	submit_button;
	ei_anchor_t	button_anchor		= ei_anc_southeast;
	int		button_x		= -20;
	int		button_y		= -20;
	float		button_rel_x		= 1.0;
	float		button_rel_y		= 1.0;
	float		button_rel_width	= 0.5;
	ei_color_t	button_color		= {0x88, 0x88, 0x88, 0xff};
	char*		button_title		= "Submit";
	ei_color_t	button_text_color	= {0x00, 0x00, 0x00, 0xff};
	ei_relief_t	button_relief		= ei_relief_raised;
	int		button_border_width	= 2;
	ei_callback_t	button_callback 	= submit;

	ei_widget_t*	window;
	ei_size_t	window_size		= {320,240};
	char*		window_title		= "Form";
	ei_color_t	window_color		= {0xA0,0xA0,0xA0, 0xff};
	int		window_border_width	= 2;
	ei_bool_t	window_closable		= EI_TRUE;
	ei_axis_set_t	window_resizable	= ei_axis_both;
	ei_point_t	window_position		= {30, -10};

	ei_widget_t *entry;
	ei_anchor_t entry_anchor = ei_anc_northeast;
	int entry_x = -20;
	int entry_y = 50;
	int entry_corner = 5;

	/* Create the application and change the color of the background. */
	ei_app_create(&screen_size, EI_FALSE);
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	ei_event_set_default_handle_func(process_key);

	/* Create, configure and place a toplevel window on screen. */
	window = ei_widget_create("toplevel", ei_app_root_widget());
	ei_toplevel_configure(window, &window_size, &window_color, &window_border_width,
				&window_title, &window_closable, &window_resizable, NULL);
	ei_place(window, NULL, &(window_position.x), &(window_position.y), NULL, NULL, NULL, NULL, NULL, NULL);

	entry = ei_widget_create("entry", window);
	ei_entry_configure(entry, NULL, NULL, NULL, &entry_corner, NULL, NULL, NULL);
	ei_place(entry, &entry_anchor, &entry_x, &entry_y, NULL, NULL, NULL, NULL, NULL, NULL);

    /* Second entry */
    ei_widget_t *entry2;
	ei_anchor_t entry_anchor2 = ei_anc_northeast;
	int entry_x2 = -20;
	int entry_y2 = 100;
	int entry_corner2 = 5;
    entry2 = ei_widget_create("entry", window);
	ei_entry_configure(entry2, NULL, NULL, NULL, &entry_corner2, NULL, NULL, NULL);
	ei_place(entry2, &entry_anchor2, &entry_x2, &entry_y2, NULL, NULL, NULL, NULL, NULL, NULL);

	/* Create, configure and place a button as a descendant of the toplevel window. */
	submit_button = ei_widget_create("button", window);
	ei_button_configure	(submit_button, NULL, &button_color,
				 &button_border_width, NULL, &button_relief, &button_title, NULL, &button_text_color, NULL,
				NULL, NULL, NULL, &button_callback, NULL);
	ei_place(submit_button, &button_anchor, &button_x, &button_y, NULL, NULL, &button_rel_x, &button_rel_y, &button_rel_width, NULL);

	/* Run the application's main loop. */
	ei_app_run();

	ei_app_free();

	return (EXIT_SUCCESS);
}
