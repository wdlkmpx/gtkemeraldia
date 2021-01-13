/*                             */
/* xemeraldia   --- graphics.c */
/*                             */

#include "games.h"

static gboolean animateTmpScore (void *);

guint timer, tmp_sc_timer;
PangoFontDescription *animated_score_font, *game_over_font, *pause_font;

struct TmpSc
{
   long sc;
   int  x, y;
   char cnt;
};

static struct TmpSc  tmpSc[50];

static void //  https://stackoverflow.com/a/36483677 
_cairo_gdk_draw_pix (cairo_t *cr,
                     int src_x,   int src_y,
                     int dest_x,  int dest_y,
                     int width,   int height)
{
   // This a special case where we don't need
   //   the whole pixbuf, just a region.
   // It's a bit complicated with cairo,
   //   so we need the source's surface to perform operations.
   // See
   //   https://developer.gnome.org/gdk2/stable/gdk2-Drawing-Primitives.html#gdk-draw-drawable
   cairo_pattern_t * pattern;
   cairo_surface_t * source;
   pattern = cairo_get_source (cr);
   cairo_pattern_get_surface (pattern, &source);

   cairo_save (cr);

   /* Move (0, 0) to the destination position */
   cairo_translate (cr, dest_x, dest_y);

   /* Set up the source surface in such a way that
    * (src_x, src_y) maps to (0, 0) in user coordinates. */
   cairo_set_source_surface (cr, source, -src_x, -src_y);

   /* Do the drawing */
   cairo_rectangle (cr, 0, 0, width, height);
   cairo_fill (cr);

   /* Undo all of our modifications to the drawing state */
   cairo_restore (cr);
}


gboolean boardw_draw_cb (GtkWidget *widget, gpointer compat, gpointer data)
{
#if GTK_MAJOR_VERSION >= 3
   cairo_t * cr = (cairo_t *) compat;
   GdkRectangle rect;
   GdkRectangle * area = &rect;
   gdk_cairo_get_clip_rectangle (cr, area);
#else // gtk2, espose_ event
   GdkEventExpose * event = (GdkEventExpose *) compat;
   cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
   GdkRectangle * area = &(event->area);
#endif
   cairo_set_source_surface (cr, board_pix, 0, 0);

   if (area->x == 0 && area->y == 0) {
      cairo_paint (cr);
   } else {
      _cairo_gdk_draw_pix (cr, area->x, area->y,
                           area->x, area->y,
                           area->width, area->height);
   }
#if GTK_MAJOR_VERSION == 2
   cairo_destroy (cr);
#endif
   return TRUE;
}

static void invalidate_area(GtkWidget *widget, gint x, gint y, gint w, gint h)
{
   gtk_widget_queue_draw_area(widget, x, y, w, h);
}


gboolean next_item_draw_cb (GtkWidget *widget, gpointer compat, gpointer data)
{
#if GTK_MAJOR_VERSION >= 3
   cairo_t * cr = (cairo_t *) compat;
#else // gtk2, espose_ event
   cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (widget));
#endif

   // this is a bit simple, so we'll do everything here
   // first, clear the the area
   cairo_set_source_rgba (cr, 0, 0, 0, 1); // black
   cairo_rectangle (cr, 0, 0,
                    BLOCK_WIDTH * 3, BLOCK_HEIGHT * 3);
   cairo_fill (cr);

   // area remains cleared if game is over or paused
   if (gameover_flag || paused) {
      return (TRUE);
   }

   // draw | redraw the next item
   int  i;
   if (next_i.col[0] == STAR) {
      cairo_set_source_surface (cr, star,
                                BLOCK_WIDTH / 2 + DIFF_X, BLOCK_HEIGHT /2 + DIFF_Y);
      cairo_paint (cr);
   } else {
      for (i = 0; i < 3; i++) {
         cairo_set_source_surface (cr, block[next_i.col[i]],
                                   BLOCK_WIDTH * (iRot_vx[0][i]) + DIFF_X,
                                   BLOCK_HEIGHT * (1 + iRot_vy[0][i]) + DIFF_Y);
         cairo_paint (cr);

      }
   }

#if GTK_MAJOR_VERSION == 2
   cairo_destroy (cr);
#endif
   return TRUE;
}


void  showTmpScore (long tmp_sc, int sc_x, int sc_y, long ch_s)
{
   PangoLayout *layout;
   char  tmp_sc_str[8];

   tmpSc[ch_s].x = sc_x;  tmpSc[ch_s].y = sc_y;
   tmpSc[ch_s].sc = tmp_sc;
   tmpSc[ch_s].cnt = 8;
   snprintf (tmp_sc_str, sizeof(tmp_sc_str), "%7ld", tmpSc[ch_s].sc);

   layout = gtk_widget_create_pango_layout (board_w, tmp_sc_str);
   pango_layout_set_font_description (layout, animated_score_font);

   cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (board_w));
   cairo_set_source_rgba (cr, 1, 1, 1, 1);
   cairo_move_to (cr,
                  (tmpSc[ch_s].x - 1) * BLOCK_WIDTH - 6,
                  ((tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 24 + tmpSc[ch_s].cnt * 3) - 25);
   pango_cairo_show_layout (cr, layout);
   cairo_destroy (cr);

   g_object_unref (layout);
   gdk_flush();

   --tmpSc[ch_s].cnt;
   tmp_sc_timer = g_timeout_add (20, animateTmpScore, GINT_TO_POINTER(ch_s));
}


static gboolean animateTmpScore(void *closure)
{
   gint ch_s = GPOINTER_TO_INT(closure);
   PangoLayout *layout;
   char  tmp_sc_str[8];

   invalidate_area (board_w,
                    (tmpSc[ch_s].x - 1) * BLOCK_WIDTH,
                    (tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 43 + tmpSc[ch_s].cnt * 3,
                    90, 25);
   gdk_window_process_updates (gtk_widget_get_window (board_w), TRUE);

   gdk_flush();

   snprintf (tmp_sc_str, sizeof(tmp_sc_str), "%7ld", tmpSc[ch_s].sc);

   layout = gtk_widget_create_pango_layout (board_w, tmp_sc_str);
   pango_layout_set_font_description (layout, animated_score_font);

   cairo_t * cr = gdk_cairo_create (gtk_widget_get_window (board_w));
   cairo_set_source_rgba (cr, 1, 1, 1, 1);
   cairo_move_to (cr,
                  (tmpSc[ch_s].x - 1) * BLOCK_WIDTH - 6,
                  ((tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 24 + tmpSc[ch_s].cnt * 3) - 25);
   pango_cairo_show_layout (cr, layout);
   cairo_destroy (cr);

   g_object_unref (layout);
   gdk_flush();

   if (--tmpSc[ch_s].cnt > 0) {
      tmp_sc_timer = g_timeout_add(45, animateTmpScore, GINT_TO_POINTER(ch_s));
   } else {
      invalidate_area (board_w,
                       (tmpSc[ch_s].x - 1) * BLOCK_WIDTH,
                       (tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 43 + tmpSc[ch_s].cnt * 3,
                       90, 25);
   }
   return FALSE;
}


void clearScreen()
{
   cairo_t * cr = cairo_create (board_pix);
   cairo_set_source_rgba (cr, 0, 0, 0, 1); // black
   cairo_rectangle (cr, 0, 0, WIN_WIDTH, WIN_HEIGHT);
   cairo_fill (cr);

   cairo_set_line_width (cr, 1.0);
   cairo_set_source_rgba (cr, 1, 1, 1, 1);
   cairo_rectangle (cr,
                    DIFF_X / 2,
                    DIFF_Y / 2,
                    BLOCK_WIDTH * BOARD_WIDTH + DIFF_X,
                    BLOCK_HEIGHT * BOARD_HEIGHT + DIFF_Y);
   cairo_stroke (cr);
   cairo_destroy (cr);
}

/** Draws the falling block described in drop_i.
 */
void  printItem ()
{
   /* Previous block to delete. */
   static int prevblocknum = -1;
   static int prevxcoord[3], prevycoord[3], prevcx, prevcy;
   static int prevrot;

   int i;

   int cx = (BLOCK_WIDTH * drop_i.x) - DIFF_X;
   int cy = (BLOCK_HEIGHT * drop_i.y) + DIFF_Y + offset_down;

   if (prevblocknum == drop_i.blocknum && cy == prevcy
      && cx == prevcx && (star_comes || drop_i.rot == prevrot))
   {
      return;
   }

   if (drop_i.blocknum != prevblocknum) {
      prevblocknum = -1;
   }
   if (prevblocknum != -1) {
      for(i = 0; i < 3 && prevxcoord[i] != -1; i++)
         deleteCell(prevxcoord[i], prevycoord[i]);
   }

   if (prevblocknum != -1 && abs(cx - prevcx) > BLOCK_WIDTH / 2) {
      cx = prevcx + (BLOCK_WIDTH / 3) * ((cx - prevcx) < 0 ? -1 : 1);
   }

   if (star_comes)
   {
      cairo_t * cr = cairo_create (board_pix);
      cairo_set_source_surface (cr, star, cx, cy);
      cairo_paint (cr);
      cairo_destroy (cr);

      invalidate_area (board_w, cx, cy, BLOCK_WIDTH, BLOCK_HEIGHT);
      prevxcoord[0] = cx;
      prevycoord[0] = cy;
      prevxcoord[1] = -1;
   }
   else {
      for(i = 0; i < 3; i++)
      {
         int dx = iRot_vx[drop_i.rot][i];
         int dy = iRot_vy[drop_i.rot][i];

         int rx = drop_i.x + dx;
         int ry = drop_i.y + dy;

         int xcoord = cx + BLOCK_WIDTH * dx;
         int ycoord = cy + BLOCK_HEIGHT * dy;

         drawCell(xcoord, ycoord, drop_i.col[i], board[rx][ry].sub);
         board[rx][ry].blk  = drop_i.col[i];

         prevxcoord[i] = xcoord;
         prevycoord[i] = ycoord;
      }
   }
   prevblocknum = drop_i.blocknum;
   prevcx = cx;
   prevcy = cy;
   prevrot = drop_i.rot;
}

void drawCell(int xcoord, int ycoord, cellstatus_t color, cellsubstatus_t sub)
{
   cairo_t * cr = cairo_create (board_pix);
   int  pixmap_number = color;

   if ((sub == CRACKED) || (sub == DELETE)) {
      pixmap_number += BLOCK_VARIETY;
   }

   if ((color > 0) && (color <= BLOCK_VARIETY * 2))
   {
      cairo_set_source_surface (cr, block[pixmap_number], xcoord, ycoord);
      cairo_paint (cr);

      invalidate_area (board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
   }
   cairo_destroy (cr);
}

void printBlock(int x, int y, cellstatus_t color)
{
   drawCell (BLOCK_WIDTH * x - DIFF_X, BLOCK_HEIGHT * y + DIFF_Y,
             color, board[x][y].sub);
}

void deleteCell(int xcoord, int ycoord)
{
   cairo_t * cr = cairo_create (board_pix);
   cairo_set_source_rgba (cr, 0, 0, 0, 1);
   cairo_rectangle (cr, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
   cairo_fill (cr);
   cairo_destroy (cr);

   invalidate_area (board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
}

void  delete_1_block (int x, int y)
{
   deleteCell(BLOCK_WIDTH * x - DIFF_X, BLOCK_WIDTH * y + DIFF_Y);
}

void  startTimer ()
{
   int  reduce;

   if (blocks < 200)  reduce = (blocks / 20) * 70;
   else  if (blocks < 400) reduce = ((blocks - 200) / 20) * 50;
   else  if (blocks < 600) reduce = 500 + ((blocks - 400) / 20) * 25;
   else  if (blocks < 800) reduce = 200 + ((blocks - 600) / 20) * 50;
   else  if (blocks < 1000) reduce = 700 + ((blocks - 800) / 20) * 10;
   else  reduce = 860;

   if (star_comes) {
      reduce = 0;
   }
   timer = g_timeout_add ((MAX_DELAY - reduce) / BLOCK_HEIGHT, (GSourceFunc)DropItem, NULL);
}


void  stopTimer ()
{
   g_source_remove(timer);
   timer = 0;
}


void  printScore ()
{
   char  buf[50];
   snprintf (buf, sizeof(buf), "%ld", sc);
   gtk_label_set_label (GTK_LABEL (score_disp), buf);
}


void  printLevel ()
{
   char  buf[50];
   snprintf (buf, sizeof(buf), "%ld", blocks / 20 + 1);
   gtk_label_set_label (GTK_LABEL (level_disp), buf);
}


void crack_1_block(int x, int y)
{
   cairo_t * cr = cairo_create (board_pix);
   int xcoord = BLOCK_WIDTH * x - DIFF_X;
   int ycoord = BLOCK_HEIGHT * y + DIFF_Y;
   if ((board[x][y].blk > 0) && (board[x][y].blk <= BLOCK_VARIETY))
   {
      cairo_set_source_surface (cr, block[board[x][y].blk + BLOCK_VARIETY],
                                xcoord, ycoord);
      cairo_paint (cr);

      invalidate_area (board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
   }
   cairo_destroy (cr);
}


void  crushAnimate (int x, int y, int num)
{
   int xcoord = BLOCK_WIDTH * x - DIFF_X;
   int ycoord = BLOCK_HEIGHT * y + DIFF_Y;

   cairo_t * cr = cairo_create (board_pix);
   cairo_set_source_surface (cr, crush[num], xcoord, ycoord);
   cairo_paint (cr);
   cairo_destroy (cr);

   invalidate_area (board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
}
