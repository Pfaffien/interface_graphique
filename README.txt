+-------------------------------+
|   Projet Interface Graphique  |
+-------------------------------+


Authors : BAUER Thomas, GODRON Louis, MILLOT Pierre


Applications réalisées :

    Lignes
	Frame
	Button
	Fenêtre hello_world
	Puzzle
	2048



Extensions réalisées :

    Clipping de ligne
	Clipping de polygones
	Champ de saisie
	Utlisiation des applications au clavier



INSTALLATION :

    sudo apt-get update

    sudo apt-get install libsdl1.2-dev
    libsdl-ttf2.0-dev libfreeimage-dev

    sudo apt-get install libsdl-image1.2
    sudo apt-get install libsdl-image1.2-dev



Nouveaux fichiers :

    ei_draw_shape.h
    ei_widget_button.h
    ei_widget_entry.h
    ei_widget_frame.h
    ei_widget_radiobutton.h
    ei_widget_toplevel.h



APPLICATIONS :

    Lines : test du clipper, des fonctions ei_draw_polyline (Algorithme de Bresenham) et draw_polygon (TC et TCA).

    Frame : test de toplevel, de la création d'application, du placer, du widget frame.

    Button : test du button (gestion du relief), de la gestion d'événement clavier et souris.

    hello_world : test du clipping, des coordonnées relatives, de la window (fermeture, redimensionnement).

    puzzle : test du chargement d'images et utilisation de button_geomnotifyfunc pour actualiser les motifs des tuiles.

    2048 : test du chevauchement de fenêtres.

    Champ de saisie : permet de remplir des formulaires.

    Possibilté d'utiliser 'Tab' pour exploiter les différents widgets.



Licence :

    Disponible dans LICENSE.md
