/*                                   */
/* xemeraldia   ---- init-graphics.c */
/*                                   */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>

#include "graphics.h"
#include "bitmaps.h"

static void  createBWPixmaps(GdkDrawable *w, int depth), createColoredPixmaps(GdkDrawable *w, int depth);
static void  createGCs(GdkDrawable *), createCrushAnimePixmaps(GdkDrawable *w, int depth);

GtkWidget  *board_w, *nextItem_w, *quit, *start, *scores, *score_disp, *level_disp, *about;
GtkWidget  *score_frame, *score_text, *high_sc_w, *topLevel;
GdkGC *draw_gc, *delete_gc;
GdkPixmap  *block[BLOCK_VARIETY * 2 + 1], *crush[CRUSH_ANIME_FRAMES];
GdkPixmap  *board_pix, *star, *saved_screen, *background;
GdkColor     black, white;
int     colored;

static void createBackground(void)
{
	cairo_t *cr = gdk_cairo_create(background);
	cairo_pattern_t *p = cairo_pattern_create_radial(
		WIN_WIDTH * .2, WIN_HEIGHT * .1, 0,
		WIN_WIDTH * .2, WIN_HEIGHT * .1, WIN_WIDTH * 1.2
		);
	cairo_pattern_add_color_stop_rgb(p, 0,
		0, 0, .7
		);

	cairo_pattern_add_color_stop_rgb(p, 1,
		0, 0, 0
		);
	cairo_set_source(cr, p);
	cairo_paint(cr);
	cairo_destroy(cr);
}

void  initXlib ()
{
	int     depth;
	GdkVisual *vi;

	gdk_color_black(gdk_colormap_get_system(), &black);
	gdk_color_white(gdk_colormap_get_system(), &white);

	vi = gdk_visual_get_system();
	depth = vi->depth;
	colored = ((depth != 1) && (vi->type != GDK_VISUAL_GRAYSCALE));

	createGCs (board_pix);
	createCrushAnimePixmaps (board_pix, depth);
	if (colored)
	  createColoredPixmaps (board_pix, depth);
	else
	  createBWPixmaps (board_pix, depth);
	createBackground();
	clearNextItem ();
	clearScreen();


	animated_score_font = pango_font_description_from_string("sans-serif 20");
	pause_font = pango_font_description_from_string("sans-serif Bold Italic 22");
	game_over_font = pango_font_description_from_string("sans-serif Bold 10");
}

static gboolean keyPressed(GtkWidget *widget G_GNUC_UNUSED,
	GdkEventKey *event G_GNUC_UNUSED,
	gpointer user_data G_GNUC_UNUSED)
{
	switch(event->keyval)
	{
		case GDK_K:
		case GDK_k:
		case GDK_I:
		case GDK_i:
		case GDK_Down:
		case GDK_Begin:
			Rotation();
			return TRUE;
		case GDK_Up:
		case GDK_J:
		case GDK_j:
			CCRotation();
			return TRUE;
		case GDK_Left:
		case GDK_H:
		case GDK_h:
		case GDK_U:
		case GDK_u:
			MoveLeft();
			return TRUE;
		case GDK_Right:
		case GDK_L:
		case GDK_l:
			MoveRight();
			return TRUE;
		case GDK_space:
			MoveDown();
			return TRUE;
		case GDK_S:
		case GDK_s:
		case GDK_P:
		case GDK_p:
		case GDK_Pause:
			StartGame();
			return TRUE;
		case GDK_Q:
		case GDK_q:
			Quit();
			return TRUE;
	}
	return FALSE;
}

void initGTK(GtkWidget *w)
{
	GtkWidget  *nextBox, *Score, *Level, *Next, *hbox, *vbox, *vbox2, *frame, *framevbox, *x;

	g_signal_connect(G_OBJECT(w), "key-press-event", G_CALLBACK(keyPressed), NULL);

	hbox = gtk_hbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(w), hbox);

	board_w = gtk_drawing_area_new();
	gtk_widget_set_size_request(board_w, WIN_WIDTH, WIN_HEIGHT);
	gtk_widget_set_app_paintable(board_w, TRUE);
	gtk_widget_set_double_buffered(board_w, FALSE);
	g_signal_connect(G_OBJECT (board_w), "expose_event",
		G_CALLBACK (expose_board), NULL);
	
	gtk_box_pack_start(GTK_BOX(hbox), board_w, TRUE, TRUE, 3);

	vbox = gtk_vbox_new(FALSE, 6);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 3);

	x = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), x, FALSE, FALSE, 0);

	nextBox = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), nextBox, FALSE, FALSE, 0);

	vbox2 = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(nextBox), vbox2);

	Next = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(Next), _("<b>NEXT</b>"));
	gtk_box_pack_start(GTK_BOX(vbox2), Next, TRUE, TRUE, 0);

	nextItem_w = gtk_drawing_area_new();
	gtk_widget_set_size_request(nextItem_w, BLOCK_WIDTH * 3, BLOCK_HEIGHT * 3);
	g_signal_connect (G_OBJECT (nextItem_w), "expose_event",
		G_CALLBACK (RedrawNextItem), NULL);
	gtk_box_pack_start(GTK_BOX(vbox2), nextItem_w, TRUE, TRUE, 0);

	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

	framevbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(frame), framevbox);
	
	Score = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(Score), _("<b>SCORE</b>"));
	gtk_box_pack_start(GTK_BOX(framevbox), Score, TRUE, TRUE, 0);

	score_disp = gtk_label_new("0");
	gtk_box_pack_start(GTK_BOX(framevbox), score_disp, TRUE, TRUE, 0);

	x = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(framevbox), x, TRUE, TRUE, 0);

	Level = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(Level), _("<b>LEVEL</b>"));
	gtk_box_pack_start(GTK_BOX(framevbox), Level, TRUE, TRUE, 0);

	level_disp = gtk_label_new("0");
	gtk_box_pack_start(GTK_BOX(framevbox), level_disp, TRUE, TRUE, 0);


	frame = gtk_alignment_new(.5,0.2,.9,.15);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

	framevbox = gtk_vbutton_box_new();
	gtk_container_add(GTK_CONTAINER(frame), framevbox);

	start = gtk_button_new_with_label(_("Start"));
/*  GTK_WIDGET_SET_FLAGS(start, GTK_CAN_DEFAULT); */
	gtk_box_pack_start(GTK_BOX(framevbox), start, TRUE, FALSE, 0);

	if(app_data.usescorefile)
	{
		scores = gtk_button_new_with_label(_("Scores"));
		gtk_box_pack_start(GTK_BOX(framevbox), scores, TRUE, FALSE, 0);
	}

#ifdef GTK_STOCK_ABOUT
	about = gtk_button_new_from_stock(GTK_STOCK_ABOUT);
#else
	about = gtk_button_new_with_label(_("About..."));
#endif
	gtk_box_pack_start(GTK_BOX(framevbox), about, TRUE, FALSE, 0);

	quit = gtk_button_new_from_stock(GTK_STOCK_QUIT);
	gtk_box_pack_start(GTK_BOX(framevbox), quit, TRUE, FALSE, 0);

/*
	frame = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, FALSE, 0);
*/
}


static void  createGCs (GdkDrawable *w)
{
  GdkGCValues values;

  values.foreground  = white;
  values.background  = black;
  draw_gc = gdk_gc_new_with_values(w, &values, GDK_GC_FOREGROUND | GDK_GC_BACKGROUND);
  values.foreground  = black;
  values.background  = black;
  delete_gc = gdk_gc_new_with_values(w, &values, GDK_GC_FOREGROUND | GDK_GC_BACKGROUND);
}


static void  createBWPixmaps (GdkDrawable *w, int depth)
{
  int   i;
  const char **block_bits[BLOCK_VARIETY * 2 + 1];

  star = gdk_pixmap_create_from_xpm_d (w, NULL, NULL, (char **) star_xpm);
  block_bits[1] = block1_xpm;
  block_bits[2] = block2_xpm;
  block_bits[3] = block3_xpm;
  block_bits[4] = block4_xpm;
  block_bits[5] = block5_xpm;
  block_bits[6] = block6_xpm;
  block_bits[7] = block1cr_xpm;
  block_bits[8] = block2cr_xpm;
  block_bits[9] = block3cr_xpm;
  block_bits[10] = block4cr_xpm;
  block_bits[11] = block5cr_xpm;
  block_bits[12] = block6cr_xpm;
  for (i = 1; i <= BLOCK_VARIETY * 2; i++)
    {
      block[i] = gdk_pixmap_create_from_xpm_d (w, NULL, NULL, (char **) block_bits[i]);
    }
}


static void  createColoredPixmaps (GdkDrawable *w, int depth)
{
  int   i;
  GdkColor block_pixel[BLOCK_VARIETY + 1];

  star = gdk_pixmap_create_from_xpm_d (w, NULL, NULL, (char **) star_xpm);
  block_pixel[1] = app_data.block1pixel;
  block_pixel[2] = app_data.block2pixel;
  block_pixel[3] = app_data.block3pixel;
  block_pixel[4] = app_data.block4pixel;
  block_pixel[5] = app_data.block5pixel;
  block_pixel[6] = app_data.block6pixel;
  for (i = 1; i <= BLOCK_VARIETY; i++)
  {
		cairo_surface_t *s = cairo_image_surface_create(CAIRO_FORMAT_RGB24, BLOCK_WIDTH, BLOCK_HEIGHT);
		cairo_t *c = cairo_create(s);
		gdk_cairo_set_source_color(c, &block_pixel[i]);
		cairo_move_to(c, 0, 0);
		cairo_set_line_width(c, BLOCK_HEIGHT * .0625);
		cairo_line_to(c, BLOCK_WIDTH, BLOCK_HEIGHT);
		cairo_stroke(c);

		cairo_rectangle(c, BLOCK_WIDTH * .1, BLOCK_HEIGHT * .1, BLOCK_WIDTH * .9, BLOCK_HEIGHT * .9);
		
		cairo_pattern_t *p = cairo_pattern_create_radial(BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2, 0,
			BLOCK_WIDTH / 2, BLOCK_HEIGHT / 2, BLOCK_WIDTH / 2);
		cairo_pattern_add_color_stop_rgb(p, 0,
			block_pixel[i].red / 65535.0,
			block_pixel[i].green / 65535.0,
			block_pixel[i].blue / 65535.0
			);

		cairo_pattern_add_color_stop_rgb(p, 1,
			(block_pixel[i].red / 65535.0) * .7,
			(block_pixel[i].green / 65535.0) * .7,
			(block_pixel[i].blue / 65535.0) * .7
			);
		cairo_set_source(c, p);
		cairo_fill(c);
		
		cairo_move_to(c, 0, 0);
		cairo_line_to(c, BLOCK_WIDTH, 0);
		cairo_line_to(c, BLOCK_WIDTH, BLOCK_HEIGHT * .55);
		cairo_curve_to(c, BLOCK_WIDTH / 2, BLOCK_HEIGHT * .16,
			BLOCK_WIDTH / 2, BLOCK_HEIGHT * .78,
			0, BLOCK_HEIGHT * .44);
		cairo_close_path(c);
		//p = cairo_pattern_create_linear(16, 16, 16, 0);
		p = cairo_pattern_create_radial(BLOCK_WIDTH * .2, BLOCK_HEIGHT * .2, 1,
			BLOCK_WIDTH * .2, BLOCK_HEIGHT * .2, 28);
		cairo_pattern_add_color_stop_rgba(p, 0, 1, 1, 1, .9);
		cairo_pattern_add_color_stop_rgba(p, 1, 1, 1, 1, .2);
		cairo_set_source(c, p);
		cairo_fill(c);
		
		block[i] = gdk_pixmap_new(w,BLOCK_WIDTH,BLOCK_HEIGHT,-1);
		cairo_t *cr_pixmap = gdk_cairo_create(block[i]);
		cairo_set_source_surface (cr_pixmap, s, 0, 0);
		cairo_paint(cr_pixmap);
		cairo_destroy(cr_pixmap);
		
		/* the "crack" */
		cairo_set_line_width(c, BLOCK_HEIGHT * .09);
		cairo_set_source_rgba(c, 0, 0, 0, .8);
		cairo_move_to(c, 0, BLOCK_HEIGHT * .1);
		cairo_line_to(c, BLOCK_WIDTH * .6, BLOCK_HEIGHT * .6);
		cairo_line_to(c, BLOCK_WIDTH, BLOCK_HEIGHT * .2);
		cairo_stroke(c);
		cairo_move_to(c, BLOCK_WIDTH * .6, BLOCK_HEIGHT * .6);
		cairo_line_to(c, BLOCK_WIDTH * .4, BLOCK_HEIGHT);
		cairo_stroke(c);

		block[i +  BLOCK_VARIETY] = gdk_pixmap_new(w,BLOCK_WIDTH,BLOCK_HEIGHT,-1);
		cr_pixmap = gdk_cairo_create(block[i + BLOCK_VARIETY]);
		cairo_set_source_surface (cr_pixmap, s, 0, 0);
		cairo_paint(cr_pixmap);
		cairo_destroy(cr_pixmap);
		
		cairo_destroy(c);
		cairo_surface_destroy(s);
  }
}


static void  createCrushAnimePixmaps (GdkDrawable *w, int depth)
{
  int   i;
  const char **crush_bits[CRUSH_ANIME_FRAMES];

  crush_bits[0] = crush0_xpm;
  crush_bits[1] = crush1_xpm;
  crush_bits[2] = crush2_xpm;
  crush_bits[3] = crush3_xpm;
  crush_bits[4] = crush4_xpm;
  for (i = 0; i < CRUSH_ANIME_FRAMES; i++) {
    crush[i] = gdk_pixmap_create_from_xpm_d (w, NULL, &black, (char **) crush_bits[i]);
  }
}
