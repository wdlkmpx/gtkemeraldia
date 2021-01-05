/*                             */
/* xemeraldia   --- graphics.c */
/*                             */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

gboolean expose_board(GtkWidget *widget, GdkEventExpose *event, gpointer data G_GNUC_UNUSED)
{
	gdk_draw_drawable(widget->window, draw_gc, board_pix, event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	return TRUE;
}

static void invalidate_area(GtkWidget *widget, gint x, gint y, gint w, gint h)
{
	while(GTK_WIDGET_FLAGS(widget) & GTK_NO_WINDOW)
	{
		x += widget->allocation.x;
		y += widget->allocation.y;
		widget = widget->parent;
	}
	gtk_widget_queue_draw_area(widget, x, y, w, h);
}


void  RedrawNextItem ()
{
  clearNextItem ();
  if ((! gameover_flag) && (! paused))
    printNextItem ();
}


void  showTmpScore (long tmp_sc, int sc_x, int sc_y, long ch_s)
{
  PangoLayout *layout;
  char  tmp_sc_str[8];

  tmpSc[ch_s].x = sc_x;  tmpSc[ch_s].y = sc_y;
  tmpSc[ch_s].sc = tmp_sc;
  tmpSc[ch_s].cnt = 8;
  sprintf (tmp_sc_str, "%7ld", tmpSc[ch_s].sc);

	layout = gtk_widget_create_pango_layout (board_w, tmp_sc_str);
	pango_layout_set_font_description(layout, animated_score_font);
	gdk_draw_layout(board_w->window, draw_gc,
		(tmpSc[ch_s].x - 1) * BLOCK_WIDTH - 6,
		((tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 24 + tmpSc[ch_s].cnt * 3) - 25,
		layout);
	g_object_unref (layout);
  gdk_flush();

  gdk_flush();
  --tmpSc[ch_s].cnt;
  tmp_sc_timer = g_timeout_add(20, animateTmpScore, GINT_TO_POINTER(ch_s));
}


static gboolean animateTmpScore(void *closure)
{
  gint ch_s = GPOINTER_TO_INT(closure);
  PangoLayout *layout;
  char  tmp_sc_str[8];

	invalidate_area(board_w,
		(tmpSc[ch_s].x - 1) * BLOCK_WIDTH,
		(tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 43 + tmpSc[ch_s].cnt * 3,
		90, 25);
	gdk_window_process_updates (board_w->window, TRUE);

  gdk_flush();

  sprintf (tmp_sc_str, "%7ld", tmpSc[ch_s].sc);

	layout = gtk_widget_create_pango_layout (board_w, tmp_sc_str);
	pango_layout_set_font_description(layout, animated_score_font);
	gdk_draw_layout(board_w->window, draw_gc,
		(tmpSc[ch_s].x - 1) * BLOCK_WIDTH - 6,
		((tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 24 + tmpSc[ch_s].cnt * 3) - 25,
		layout);
	g_object_unref (layout);
	gdk_flush();

	if (--tmpSc[ch_s].cnt > 0)
		tmp_sc_timer = g_timeout_add(45, animateTmpScore, GINT_TO_POINTER(ch_s));
	else
		invalidate_area(board_w,
			(tmpSc[ch_s].x - 1) * BLOCK_WIDTH,
			(tmpSc[ch_s].y + 1) * BLOCK_HEIGHT - 43 + tmpSc[ch_s].cnt * 3,
			90, 25);
	return FALSE;
}


void  clearNextItem ()
{
	gdk_draw_rectangle(nextItem_w->window, delete_gc, TRUE, 0, 0,
		BLOCK_WIDTH * 3, BLOCK_HEIGHT * 3);
}


void  printNextItem ()
{
  int  i;

  clearNextItem ();
  if (next_i.col[0] == STAR)
      gdk_draw_drawable(nextItem_w->window, draw_gc, star, 0, 0,
			BLOCK_WIDTH / 2 + DIFF_X, BLOCK_HEIGHT /2 + DIFF_Y,
			BLOCK_WIDTH, BLOCK_HEIGHT);
  else
    {
      for (i = 0; i < 3; i++)
      gdk_draw_drawable(nextItem_w->window, draw_gc, block[next_i.col[i]], 0, 0,
			BLOCK_WIDTH * (iRot_vx[0][i]) + DIFF_X,
			BLOCK_HEIGHT * (1 + iRot_vy[0][i]) + DIFF_Y,
			BLOCK_WIDTH, BLOCK_HEIGHT);
    }
}


void clearScreen()
{
	gdk_draw_drawable(board_pix, draw_gc, background, 0, 0, 0, 0,
		WIN_WIDTH, WIN_HEIGHT);

	gdk_draw_rectangle(board_pix, draw_gc, FALSE, DIFF_X / 2, DIFF_Y / 2,
		  BLOCK_WIDTH * BOARD_WIDTH + DIFF_X,
		  BLOCK_HEIGHT * BOARD_HEIGHT + DIFF_Y);
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

	if(prevblocknum == drop_i.blocknum && cy == prevcy
			&& cx == prevcx && (star_comes || drop_i.rot == prevrot))
		return;

	if(drop_i.blocknum != prevblocknum)
		prevblocknum = -1;
	if(prevblocknum != -1)
		for(i = 0; i < 3 && prevxcoord[i] != -1; i++)
			deleteCell(prevxcoord[i], prevycoord[i]);
	
	if(prevblocknum != -1 && abs(cx - prevcx) > BLOCK_WIDTH / 2)
		cx = prevcx + (BLOCK_WIDTH / 3) * ((cx - prevcx) < 0 ? -1 : 1);
	
	if(star_comes)
	{
		gdk_draw_drawable(board_pix, draw_gc, star,
			0,0,
			cx, cy,
			BLOCK_WIDTH, BLOCK_HEIGHT);
		invalidate_area(board_w, cx, cy, BLOCK_WIDTH, BLOCK_HEIGHT);
		prevxcoord[0] = cx;
		prevycoord[0] = cy;
		prevxcoord[1] = -1;
	} else
	{
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
	int  pixmap_number = color;

	if ((sub == CRACKED) || (sub == DELETE))
		pixmap_number += BLOCK_VARIETY;

	if ((color > 0) && (color <= BLOCK_VARIETY * 2))
	{
		gdk_draw_drawable(board_pix, draw_gc, block[pixmap_number], 0, 0,
				xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
		invalidate_area(board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
	}
}

void printBlock(int x, int y, cellstatus_t color)
{
	drawCell(BLOCK_WIDTH * x - DIFF_X, BLOCK_HEIGHT * y + DIFF_Y
			, color, board[x][y].sub);
}

void deleteCell(int xcoord, int ycoord)
{
	gdk_draw_drawable(board_pix, draw_gc, background,
		xcoord, ycoord,
		xcoord, ycoord,
		BLOCK_WIDTH, BLOCK_HEIGHT);
	invalidate_area(board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);	
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

  if (star_comes)  reduce = 0;

  timer = g_timeout_add((MAX_DELAY - reduce) / BLOCK_HEIGHT, (GSourceFunc)DropItem, NULL);
}


void  stopTimer ()
{
	g_source_remove(timer);
	timer = 0;
}


void  printScore ()
{
  char  buf[30];

#ifdef __GLIBC__
  sprintf (buf, "%'ld", sc);
#else
  sprintf (buf, "%ld", sc);
#endif
  gtk_label_set_label(GTK_LABEL(score_disp), buf);
}


void  printLevel ()
{
  char  buf[30];

  sprintf (buf, "%ld", blocks / 20 + 1);
  gtk_label_set_label(GTK_LABEL(level_disp), buf);
}


void  Done ()
{
}


void crack_1_block(int x, int y)
{
	int xcoord = BLOCK_WIDTH * x - DIFF_X;
	int ycoord = BLOCK_HEIGHT * y + DIFF_Y;
	if ((board[x][y].blk > 0) && (board[x][y].blk <= BLOCK_VARIETY))
	{
		gdk_draw_drawable(board_pix, draw_gc, block[board[x][y].blk + BLOCK_VARIETY], 0, 0,
				xcoord, ycoord,
				BLOCK_WIDTH, BLOCK_HEIGHT);
		invalidate_area(board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
	}
}


void  crushAnimate (int x, int y, int num)
{
	int xcoord = BLOCK_WIDTH * x - DIFF_X;
	int ycoord = BLOCK_HEIGHT * y + DIFF_Y;
	gdk_draw_drawable(board_pix, draw_gc, crush[num], 0, 0,
			xcoord, ycoord,
			BLOCK_WIDTH, BLOCK_HEIGHT);
	invalidate_area(board_w, xcoord, ycoord, BLOCK_WIDTH, BLOCK_HEIGHT);
}
