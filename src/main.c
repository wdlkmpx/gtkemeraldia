/*                             */
/* xemeraldia    ----- main.c  */
/*                             */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE

#include "games.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_GETOPT_LONG_ONLY
#include <getopt.h>
#endif

#define char const char
#include "bitmaps/xemeraldia.xpm"
#undef char

AppData  app_data = { .usescorefile = TRUE, .scorefile = HIGH_SCORE_TABLE };
char    *name, *programname;

static GdkPixbuf *xemeraldia_icon;

void Quit()
{
	gtk_main_quit();
}

static void About(void)
{
	GtkWidget *dialog, *label, *img, *hbox;
	dialog = gtk_dialog_new_with_buttons(_("About XEmeraldia"),
		GTK_WINDOW(topLevel),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_ACCEPT,
		NULL);
		
	hbox = gtk_hbox_new(FALSE, 2);

	img = gtk_image_new_from_pixbuf(xemeraldia_icon);
	gtk_misc_set_padding(GTK_MISC(img), 20, 10);
	gtk_box_pack_start(GTK_BOX(hbox), img, FALSE,
		FALSE, 0);

	label = gtk_label_new(NULL);

	gtk_label_set_markup(GTK_LABEL(label),
		_("<span size='xx-large' weight='bold'>xemeraldia</span>"));

	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE,
		FALSE, 0);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE,
		FALSE, 0);

	label = gtk_label_new(NULL);

	gtk_label_set_markup(GTK_LABEL(label),
		_("Drop the blocks.  If you drop a square on top of one "
		"of the same color, they (as well as any neighboring "
		"blocks of the same color) will both be shaken by an "
		"“impact”.  The first impact will cause fractures; the "
		"second will cause the block(s) to dissolve.\n\n"

		"You can either use the arrow keys or vi-style (hjkl) "
		"keys to move/rotate the blocks.  “s” or “p” will "
		"pause the game, and if your boss comes along, “q” can "
		"be used to avoid an unpleasant confrontation.\n"
		"\n"
		"Author: Yuuki Tomikawa &lt;tommy@huie.hokudai.ac.jp>\n"
		"\n"
		"Ported to GTK+ 2.x in 2003\n"
		"by Nicolás Lichtmaier &lt;nick@reloco.com.ar>\n"
		)
	);

	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_misc_set_padding(GTK_MISC(label), 20, 10);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE,
		TRUE, 0);

	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void get_x_color(const char *cn, GdkColor *color)
{
	gdk_color_parse(cn, color);
	gdk_colormap_alloc_color(gdk_colormap_get_system(), color, FALSE, TRUE);
}

#ifdef HAVE_GETOPT_LONG_ONLY
struct option options[] =
{
	{"score", FALSE, &(app_data.usescorefile), TRUE},
	{"noscore", FALSE, &(app_data.usescorefile), FALSE},
	{0, 0, 0, 0}
};
#endif

int  main (int argc, char *argv[])
{
	programname = argv[0];
	name = getenv ("USER");
	if (!name) {
		name = getenv ("USERNAME"); // win32
	}
	
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);

	open_high_scores_file();

	gtk_init(&argc, &argv);

#ifdef HAVE_GETOPT_LONG_ONLY
	while(getopt_long_only(argc, argv, "", options, NULL) != EOF);
#else
	if(argc>=2 && !strcmp(argv[1], "-noscore"))
		app_data.usescorefile = FALSE;
#endif

	if(!f_scores)
	{
		if(app_data.usescorefile)
		{
			app_data.usescorefile = FALSE;
			fprintf(stderr, _("%s: Couldn't open high score file %s (%s)\n"),
				programname, app_data.scorefile, strerror(errno_scores) );
			fprintf(stderr, _("%s: No High score file.  Use '-noscore' to avoid this message.\n"),
				programname );
		}
	} else
	{
		if(!app_data.usescorefile)
		{
			fclose(f_scores);
			f_scores = NULL;
		}
	}
	
	topLevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(topLevel), "xemeraldia");
	gtk_window_set_resizable(GTK_WINDOW(topLevel), FALSE);

	xemeraldia_icon = gdk_pixbuf_new_from_xpm_data(icon);
	gtk_window_set_icon(GTK_WINDOW(topLevel), xemeraldia_icon);

/*	g_log_set_always_fatal(G_LOG_LEVEL_MASK); */

	get_x_color("red", &app_data.block1pixel);
	get_x_color("blue", &app_data.block2pixel);
	get_x_color("green", &app_data.block3pixel);
	get_x_color("yellow", &app_data.block4pixel);
	get_x_color("violet", &app_data.block5pixel);
	get_x_color("sky blue", &app_data.block6pixel);
	get_x_color("orange", &app_data.starpixel);

	board_pix = gdk_pixmap_new(NULL, WIN_WIDTH, WIN_HEIGHT, gdk_visual_get_system()->depth);
	background = gdk_pixmap_new(NULL, WIN_WIDTH, WIN_HEIGHT, gdk_visual_get_system()->depth);

	  initGTK(topLevel);

	g_signal_connect(G_OBJECT(quit), "clicked", G_CALLBACK(Quit), NULL);
	g_signal_connect(G_OBJECT(topLevel), "delete-event", G_CALLBACK(Quit), NULL);
	g_signal_connect(G_OBJECT(start), "clicked", G_CALLBACK(StartGame), NULL);
	g_signal_connect(G_OBJECT(about), "clicked", G_CALLBACK(About), NULL);
	if(app_data.usescorefile)
		g_signal_connect(G_OBJECT(scores), "clicked", G_CALLBACK(PrintHighScores), NULL);

	gtk_widget_show_all(topLevel);

	initXlib ();
	read_high_scores ();

	gtk_main();
	return 0;
}
