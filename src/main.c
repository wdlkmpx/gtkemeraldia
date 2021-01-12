/*                             */
/* xemeraldia    ----- main.c  */
/*                             */

#include "games.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "bitmaps/xemeraldia.xpm"

char score_file[256];
AppData  app_data = {
   .usescorefile = TRUE,
   .scorefile = NULL // set in main()
};
char    *name, *programname;

static GdkPixbuf *xemeraldia_icon;

/* returns a path that must be freed with g_free) */
static char * get_config_dir_file (const char * file)
{
#define EMPTY_STRING ""
   char * config_home = NULL;
   char * res = NULL;
   config_home = getenv ("XDG_CONFIG_HOME");
   if (!config_home) {
      config_home = getenv ("HOME");
      if (!config_home) {
         config_home = EMPTY_STRING;
      }
   }
   if (file) {
      res = g_strconcat (config_home, "/", file, NULL);
   } else {
      res = g_strconcat (config_home, "/", NULL);
   }
   return (res);
}


void Quit()
{
   int i;
   if (board_pix) {
      cairo_surface_destroy (board_pix);
      board_pix = NULL;
   }
   if (saved_screen) {
      cairo_surface_destroy (saved_screen);
      saved_screen = NULL;
   }
   if (star) {
      cairo_surface_destroy (star);
      star = NULL;
   }
   for (i = 0; i < CRUSH_ANIME_FRAMES; i++) {
      if (crush[i]) {
         cairo_surface_destroy (crush[i]);
         crush[i] = NULL;
      }
   }
   for (i = 1; i <= BLOCK_VARIETY * 2; i++) {
      if (block[i]) {
         cairo_surface_destroy (block[i]);
         block[i] = NULL;
      }
   }
   gtk_main_quit();
}

static void About (void)
{
   GtkWidget * w;
   GdkPixbuf * logo;
   const gchar * authors[] =
   {
       "Yuuki Tomikawa <tommy@huie.hokudai.ac.jp>",
       "Nicolás Lichtmaier <nick@reloco.com.ar>",
       "wdlkmpx (github)",
       NULL
   };
   /* TRANSLATORS: Replace this string with your names, one name per line. */
   gchar * translators = _("Translated by");

   char * comments = _("Drop the blocks.  If you drop a square on top of one "
        "of the same color, they (as well as any neighboring "
        "blocks of the same color) will both be shaken by an "
        "'impact'.  The first impact will cause fractures; the "
        "second will cause the block(s) to dissolve.\n\n"

        "You can either use the arrow keys or vi-style (hjkl) "
        "keys to move/rotate the blocks.  “s” or “p” will "
        "pause the game.\n"
   );

   logo = xemeraldia_icon;

   w = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                     "version",      VERSION,
                     "program-name", "XEmeraldia",
                     "copyright",    "Copyright (C) 1995-2021",
                     "comments",     comments,
                     "license",      "Permission to use, copy, modify and distribute the program of Xemeraldia \n for any purpose and without fee is granted.",
                     "website",      "https://github.com/wdlkmpx/xemeraldia",
                     "authors",      authors,
                     "logo",         logo,
                     "translator-credits", translators,
                     NULL);
   gtk_container_set_border_width (GTK_CONTAINER (w), 2);
   gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (topLevel));
   gtk_window_set_modal (GTK_WINDOW (w), TRUE);
   gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_CENTER_ON_PARENT);

   g_signal_connect_swapped (w, "response",
                             G_CALLBACK (gtk_widget_destroy), w);
   gtk_widget_show_all (GTK_WIDGET (w));
}


int  main (int argc, char *argv[])
{
   programname = argv[0];

   // get username
   name = getenv ("USER");
   if (!name) {
      name = getenv ("USERNAME"); // win32
   }

   // set score_file path
   char *hfile = get_config_dir_file ("xemeraldia.scores");
   strncpy (score_file, hfile, sizeof(score_file));
   app_data.scorefile = score_file;
   g_free (hfile);

   bindtextdomain (PACKAGE, LOCALEDIR);
   bind_textdomain_codeset (PACKAGE, "UTF-8");
   textdomain (PACKAGE);

   open_high_scores_file ();

   gtk_init (&argc, &argv);

   if (argc>=2 && !strcmp(argv[1], "-noscore")) {
      app_data.usescorefile = FALSE;
   }

   if (!f_scores)
   {
      if (app_data.usescorefile)
      {
         app_data.usescorefile = FALSE;
         fprintf (stderr, _("%s: Couldn't open high score file %s (%s)\n"),
                            programname, app_data.scorefile, strerror(errno_scores) );
         fprintf (stderr, _("%s: No High score file.  Use '-noscore' to avoid this message.\n"),
                            programname);
      }
   } else {
      if (!app_data.usescorefile)
      {
         fclose(f_scores);
         f_scores = NULL;
      }
   }

   topLevel = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title (GTK_WINDOW(topLevel), "xemeraldia");
   gtk_window_set_resizable (GTK_WINDOW(topLevel), FALSE);

   xemeraldia_icon = gdk_pixbuf_new_from_xpm_data (icon);
   gtk_window_set_icon (GTK_WINDOW(topLevel), xemeraldia_icon);

   // g_log_set_always_fatal(G_LOG_LEVEL_MASK);

   initGTK (topLevel);

   g_signal_connect (G_OBJECT (quit), "clicked", G_CALLBACK (Quit), NULL);
   g_signal_connect (G_OBJECT (topLevel), "delete-event", G_CALLBACK(Quit), NULL);
   g_signal_connect (G_OBJECT (start), "clicked", G_CALLBACK(StartGame), NULL);
   g_signal_connect (G_OBJECT (about), "clicked", G_CALLBACK(About), NULL);
   if(app_data.usescorefile) {
      g_signal_connect (G_OBJECT (scores), "clicked", G_CALLBACK(PrintHighScores), NULL);
   }
   gtk_widget_show_all (topLevel);

   initXlib ();
   read_high_scores ();

   gtk_main ();
   return 0;
}
