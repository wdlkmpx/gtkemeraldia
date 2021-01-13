/*                                   */
/* xemeraldia   ---- init-graphics.c */
/*                                   */

#include "graphics.h"

#include "./bitmaps/star.xpm"

#include "./bitmaps/block1.xpm"
#include "./bitmaps/block2.xpm"
#include "./bitmaps/block3.xpm"
#include "./bitmaps/block4.xpm"
#include "./bitmaps/block5.xpm"
#include "./bitmaps/block6.xpm"
#include "./bitmaps/block1cr.xpm"
#include "./bitmaps/block2cr.xpm"
#include "./bitmaps/block3cr.xpm"
#include "./bitmaps/block4cr.xpm"
#include "./bitmaps/block5cr.xpm"
#include "./bitmaps/block6cr.xpm"

#include "./bitmaps/crush0.xpm"
#include "./bitmaps/crush1.xpm"
#include "./bitmaps/crush2.xpm"
#include "./bitmaps/crush3.xpm"
#include "./bitmaps/crush4.xpm"

static void  createColoredPixmaps ();

GtkWidget  *board_w, *nextItem_w, *quit, *start, *scores, *score_disp, *level_disp, *about;
GtkWidget  *score_text, *high_sc_w, *topLevel;

cairo_surface_t * block[BLOCK_VARIETY * 2 + 1];
cairo_surface_t * crush[CRUSH_ANIME_FRAMES];
cairo_surface_t * star;
cairo_surface_t * board_pix;
cairo_surface_t * saved_screen;

int     colored;

/*
* this creates a gradient: blue - black... interesting
* but there are issues with the Blue block, and potential
* performance issues with cairo
static void createBackground(void)
{
   background = gdk_window_create_similar_surface (gtk_widget_get_window (board_w),
                                                   CAIRO_CONTENT_COLOR_ALPHA,
                                                   WIN_WIDTH, WIN_HEIGHT);
   cairo_t *cr = cairo_create (background);

   cairo_pattern_t *p;
   p = cairo_pattern_create_radial (WIN_WIDTH * .2, WIN_HEIGHT * .1,
                                    0, WIN_WIDTH * .2,
                                    WIN_HEIGHT * .1, WIN_WIDTH * 1.2);
   cairo_pattern_add_color_stop_rgb (p, 0, 0, 0, .7);
   cairo_pattern_add_color_stop_rgb (p, 1, 0, 0, 0);
   cairo_set_source (cr, p);
   cairo_paint (cr);

   cairo_destroy(cr);
}*/

static cairo_surface_t * block_xpm_to_surface (const char **xpm)
{
   GdkPixbuf * pixbuf;
   cairo_surface_t * s;
   s = gdk_window_create_similar_surface (gtk_widget_get_window (board_w),
                                          CAIRO_CONTENT_COLOR_ALPHA,
                                          BLOCK_WIDTH, BLOCK_HEIGHT);
   if (xpm) {
      cairo_t * cr = cairo_create (s);
      pixbuf = gdk_pixbuf_new_from_xpm_data (xpm);
      gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
      cairo_paint (cr);
      cairo_destroy (cr);
      g_object_unref (pixbuf);
   }
   return (s);
}

void  initCairo (void)
{
   colored = 1;

   board_pix = gdk_window_create_similar_surface (gtk_widget_get_window (board_w),
                                                  CAIRO_CONTENT_COLOR_ALPHA,
                                                  WIN_WIDTH, WIN_HEIGHT);

   saved_screen = gdk_window_create_similar_surface (gtk_widget_get_window (board_w),
                                                     CAIRO_CONTENT_COLOR_ALPHA,
                                                     WIN_WIDTH, WIN_HEIGHT);
   star = block_xpm_to_surface (star_xpm);
   crush[0] = block_xpm_to_surface (crush0_xpm);
   crush[1] = block_xpm_to_surface (crush1_xpm);
   crush[2] = block_xpm_to_surface (crush2_xpm);
   crush[3] = block_xpm_to_surface (crush3_xpm);
   crush[4] = block_xpm_to_surface (crush4_xpm);

   if (colored) {
      createColoredPixmaps ();
   } else {
      block[1] = block_xpm_to_surface (block1_xpm);
      block[2] = block_xpm_to_surface (block2_xpm);
      block[3] = block_xpm_to_surface (block3_xpm);
      block[4] = block_xpm_to_surface (block4_xpm);
      block[5] = block_xpm_to_surface (block5_xpm);
      block[6] = block_xpm_to_surface (block6_xpm);
      block[7] = block_xpm_to_surface (block1cr_xpm);
      block[8] = block_xpm_to_surface (block2cr_xpm);
      block[9] = block_xpm_to_surface (block3cr_xpm);
      block[10] = block_xpm_to_surface (block4cr_xpm);
      block[11] = block_xpm_to_surface (block5cr_xpm);
      block[12] = block_xpm_to_surface (block6cr_xpm);
   }
   ///createBackground();
   clearScreen();

   animated_score_font = pango_font_description_from_string ("sans-serif 20");
   pause_font = pango_font_description_from_string ("sans-serif Bold Italic 22");
   game_over_font = pango_font_description_from_string ("sans-serif Bold 10");
}

static gboolean keyPressed(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
   switch(event->keyval)
   {
      case GDK_KEY_K:
      case GDK_KEY_k:
      case GDK_KEY_I:
      case GDK_KEY_i:
      case GDK_KEY_Down:
      case GDK_KEY_Begin:
         Rotation();
         return TRUE;
      case GDK_KEY_Up:
      case GDK_KEY_J:
      case GDK_KEY_j:
         CCRotation();
         return TRUE;
      case GDK_KEY_Left:
      case GDK_KEY_H:
      case GDK_KEY_h:
      case GDK_KEY_U:
      case GDK_KEY_u:
         MoveLeft();
         return TRUE;
      case GDK_KEY_Right:
      case GDK_KEY_L:
      case GDK_KEY_l:
         MoveRight();
         return TRUE;
      case GDK_KEY_space:
         MoveDown();
         return TRUE;
      case GDK_KEY_S:
      case GDK_KEY_s:
      case GDK_KEY_P:
      case GDK_KEY_p:
      case GDK_KEY_Pause:
         StartGame();
         return TRUE;
      case GDK_KEY_Q:
      case GDK_KEY_q:
         Quit();
         return TRUE;
   }
   return FALSE;
}

void initGTK(GtkWidget *w)
{
   GtkWidget  *nextBox, *Score, *Level, *Next, *hbox, *vbox, *vbox2, *frame, *framevbox, *x;

   g_signal_connect (G_OBJECT(w), "key-press-event", G_CALLBACK(keyPressed), NULL);

   hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
   gtk_container_add (GTK_CONTAINER(w), hbox);

   board_w = gtk_drawing_area_new ();
   gtk_widget_set_size_request (board_w, WIN_WIDTH, WIN_HEIGHT);
   gtk_widget_set_app_paintable (board_w, TRUE);
   gtk_widget_set_double_buffered (board_w, FALSE);
   g_signal_connect (G_OBJECT (board_w), GTKCOMPAT_DRAW_SIGNAL,
                     G_CALLBACK (boardw_draw_cb), NULL);
   
   gtk_box_pack_start (GTK_BOX(hbox), board_w, TRUE, TRUE, 3);

   vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
   gtk_box_pack_start (GTK_BOX(hbox), vbox, TRUE, TRUE, 3);

   x = gtk_label_new (NULL);
   gtk_box_pack_start (GTK_BOX(vbox), x, FALSE, FALSE, 0);

   nextBox = gtk_frame_new (NULL);
   gtk_box_pack_start (GTK_BOX(vbox), nextBox, FALSE, FALSE, 0);

   vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
   gtk_container_add (GTK_CONTAINER(nextBox), vbox2);

   Next = gtk_label_new (NULL);
   gtk_label_set_markup (GTK_LABEL(Next), _("<b>NEXT</b>"));
   gtk_box_pack_start (GTK_BOX(vbox2), Next, TRUE, TRUE, 0);

   nextItem_w = gtk_drawing_area_new ();
   gtk_widget_set_size_request (nextItem_w, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 3);
   g_signal_connect (G_OBJECT (nextItem_w), GTKCOMPAT_DRAW_SIGNAL,
                     G_CALLBACK (next_item_draw_cb), NULL);
   gtk_box_pack_start (GTK_BOX(vbox2), nextItem_w, TRUE, TRUE, 0);

   frame = gtk_frame_new (NULL);
   gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

   framevbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
   gtk_container_add (GTK_CONTAINER (frame), framevbox);
   
   Score = gtk_label_new(NULL);
   gtk_label_set_markup (GTK_LABEL (Score), _("<b>SCORE</b>"));
   gtk_box_pack_start (GTK_BOX (framevbox), Score, TRUE, TRUE, 0);

   score_disp = gtk_label_new("0");
   gtk_box_pack_start (GTK_BOX (framevbox), score_disp, TRUE, TRUE, 0);

   x = gtk_label_new (NULL);
   gtk_box_pack_start (GTK_BOX (framevbox), x, TRUE, TRUE, 0);

   Level = gtk_label_new (NULL);
   gtk_label_set_markup (GTK_LABEL (Level), _("<b>LEVEL</b>"));
   gtk_box_pack_start (GTK_BOX (framevbox), Level, TRUE, TRUE, 0);

   level_disp = gtk_label_new ("0");
   gtk_box_pack_start (GTK_BOX (framevbox), level_disp, TRUE, TRUE, 0);

   framevbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
   gtk_box_pack_start (GTK_BOX (vbox), framevbox, TRUE, TRUE, 20);

   start = gtk_button_new_with_mnemonic (_("_Start"));
   gtk_box_pack_start (GTK_BOX (framevbox), start, FALSE, FALSE, 2);

   if (app_data.usescorefile)
   {
      scores = gtk_button_new_with_label (_("Scores"));
      gtk_box_pack_start (GTK_BOX (framevbox), scores, FALSE, FALSE, 2);
   }

   about = gtk_button_new_from_stock ("gtk-about");
   gtk_box_pack_start (GTK_BOX (framevbox), about, FALSE, FALSE, 2);

   quit = gtk_button_new_from_stock ("gtk-quit");
   gtk_box_pack_start (GTK_BOX (framevbox), quit, FALSE, FALSE, 2);
}


static void  createColoredPixmaps ()
{
   int   i;
   const int block_color[BLOCK_VARIETY + 1][3] =
   {
      { 0,   0,   0   }, /* ignored */
      { 255, 0,   0   }, /* red */
      { 0,   0,   255 }, /* red */
      { 26,  210, 26  }, /* blue */
      { 255, 225, 0   }, /* green */
      { 238, 130, 238 }, /* yellow */
      { 135, 206, 235 }, /* violet */
   };

   for (i = 1; i <= BLOCK_VARIETY; i++)
   {
      cairo_surface_t *s = cairo_image_surface_create (CAIRO_FORMAT_RGB24, BLOCK_WIDTH, BLOCK_HEIGHT);
      cairo_t *c = cairo_create (s);
      cairo_set_source_rgb (c,
                            block_color[i][0] / 255.0,
                            block_color[i][1] / 255.0,
                            block_color[i][2] / 255.0);
      cairo_move_to (c, 0, 0);
      cairo_set_line_width (c, BLOCK_HEIGHT * .0625);
      cairo_line_to (c, BLOCK_WIDTH, BLOCK_HEIGHT);
      cairo_stroke (c);

      cairo_rectangle (c, BLOCK_WIDTH * .1, BLOCK_HEIGHT * .1, BLOCK_WIDTH * .9, BLOCK_HEIGHT * .9);
      
      cairo_pattern_t *p;
      p = cairo_pattern_create_radial (BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2, 0,
                                       BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2, BLOCK_WIDTH / 2);
      cairo_pattern_add_color_stop_rgb (p, 0,
                                        block_color[i][0] / 255.0,
                                        block_color[i][1] / 255.0,
                                        block_color[i][2] / 255.0);
      cairo_pattern_add_color_stop_rgb (p, 1,
                                        block_color[i][0] / 255.0,
                                        block_color[i][1] / 255.0,
                                        block_color[i][2] / 255.0);
      cairo_set_source (c, p);
      cairo_fill (c);
      
      cairo_move_to (c, 0, 0);
      cairo_line_to (c, BLOCK_WIDTH, 0);
      cairo_line_to (c, BLOCK_WIDTH, BLOCK_HEIGHT * .55);
      cairo_curve_to (c, BLOCK_WIDTH / 2, BLOCK_HEIGHT * .16,
                      BLOCK_WIDTH / 2, BLOCK_HEIGHT * .78,
                      0, BLOCK_HEIGHT * .44);
      cairo_close_path (c);

      p = cairo_pattern_create_radial (BLOCK_WIDTH * .2, BLOCK_HEIGHT * .2, 1,
                                       BLOCK_WIDTH * .2, BLOCK_HEIGHT * .2, 28);
      cairo_pattern_add_color_stop_rgba (p, 0, 1, 1, 1, .9);
      cairo_pattern_add_color_stop_rgba (p, 1, 1, 1, 1, .2);
      cairo_set_source (c, p);
      cairo_fill (c);
      
      block[i] = block_xpm_to_surface (NULL);
      cairo_t *cr_pixmap = cairo_create (block[i]);
      cairo_set_source_surface (cr_pixmap, s, 0, 0);
      cairo_paint (cr_pixmap);
      cairo_destroy (cr_pixmap);
      
      /* the "crack" */
      cairo_set_line_width (c, BLOCK_HEIGHT * .09);
      cairo_set_source_rgba (c, 0, 0, 0, .8);
      cairo_move_to (c, 0, BLOCK_HEIGHT * .1);
      cairo_line_to (c, BLOCK_WIDTH * .6, BLOCK_HEIGHT * .6);
      cairo_line_to (c, BLOCK_WIDTH, BLOCK_HEIGHT * .2);
      cairo_stroke (c);
      cairo_move_to (c, BLOCK_WIDTH * .6, BLOCK_HEIGHT * .6);
      cairo_line_to (c, BLOCK_WIDTH * .4, BLOCK_HEIGHT);
      cairo_stroke (c);

      block[i +  BLOCK_VARIETY] = block_xpm_to_surface (NULL);
      cr_pixmap = cairo_create (block[i + BLOCK_VARIETY]);
      cairo_set_source_surface (cr_pixmap, s, 0, 0);
      cairo_paint (cr_pixmap);
      cairo_destroy (cr_pixmap);
      
      cairo_destroy (c);
      cairo_surface_destroy (s);
   }
}
