/*                             */
/* xemeraldia   -----  games.c */
/*                             */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "games.h"
#include <stdlib.h>

#if defined(SYSV) || defined(SVR4)
#define srandom srand48
#define random lrand48
#endif

#if defined(WIN32)
#define srandom srand
#define random rand
#endif

static void decideMaxRnd(void), decideNextItem(void), pauseProc(void), endGame(void);
static void initGame(void), restart(void);

struct Board     board[BOARD_WIDTH + 2][BOARD_HEIGHT + 1];
struct DropItem  drop_i, next_i;

static int  max_rnd, star_gauge;

gboolean  gameover_flag = TRUE;
long sc;

/** How many blocks have been cracked in this game. */
long blocks;

long tmp_blocks, chain_step;
int  movedown_counter;
gboolean  paused = FALSE;
gboolean  star_comes;
int offset_down = 0;

const int  iRot_vx[4][3] = {{1,  0,  0}, {0,  0,  1}, {0,  1,  1}, { 1,  1,  0}};
const int  iRot_vy[4][3] = {{0,  0, -1}, {0, -1, -1}, {-1, -1, 0}, {-1,  0,  0}};
const int  iCrChk[4][2] = {{0, 1}, {0, 2}, {0, 2}, {1, 2}};


static void  initGame ()
{
  int  x, y;

  sc = 0;
  blocks = 0;
  star_gauge = 0;
  printScore ();
  printLevel ();
  for (y = 0; y <= BOARD_HEIGHT; y++)
    {
      board[0][y].blk = OBSTACLE;
      board[BOARD_WIDTH + 1][y].blk = OBSTACLE;
      board[0][y].chk = OBSTACLE;
      board[BOARD_WIDTH + 1][y].chk = OBSTACLE;
      for (x = 1; x <= BOARD_WIDTH; x++)
	{
	  board[x][y].blk = EMPTY;
	  board[x][y].sub = EMPTY_SUB;
	  board[x][y].chk = EMPTY;
	}
    }
  for (x = 0; x <= BOARD_WIDTH + 1; x++)
    {
      board[x][BOARD_HEIGHT].blk = OBSTACLE;
      board[x][BOARD_HEIGHT].chk = OBSTACLE;
    }

  decideNextItem ();
}


static void  decideNextItem ()
{
  decideMaxRnd ();
  if ((blocks > 200) && ((random () % max_rnd + 1) == LUCKY_NUMBER))
    {
      next_i.col[0] = STAR;
    }
  else
    {
	  int  reduce_blocks;
      if (blocks < 200)  reduce_blocks = 2;
      else if (blocks < 600)  reduce_blocks = 1;
      else  reduce_blocks = 0;

      next_i.col[0] = (cellstatus_t)(random () % (BLOCK_VARIETY - reduce_blocks) + 1);
      do
	{
	  next_i.col[1] = (cellstatus_t)(random () % (BLOCK_VARIETY - reduce_blocks) + 1);
	} while  (next_i.col[1] == next_i.col[0]);
      do
	{
	  next_i.col[2] = (cellstatus_t)(random () % (BLOCK_VARIETY - reduce_blocks) + 1);
	} while  ((next_i.col[2] == next_i.col[0])
		  || (next_i.col[2] == next_i.col[1]));

      next_i.rot = 0;
    }
}


static void  restart ()
{
	gtk_button_set_label(GTK_BUTTON(start), _("Pause"));
	paused = FALSE;
	gdk_draw_drawable(board_pix, draw_gc, saved_screen, 0, 0, 0, 0, -1, -1);
	g_object_unref(saved_screen);
	gtk_widget_queue_draw(board_w);
	saved_screen = NULL;
	RedrawNextItem();
	startTimer ();
}


void  StartGame ()
{
  if (gameover_flag)
    {
      gtk_button_set_label(GTK_BUTTON(start), _("Pause"));
      gameover_flag = FALSE;
      srandom (time (NULL));
      initGame ();
      clearScreen();
      gtk_widget_queue_draw(board_w);
      makeNext ();
      printItem ();
      startTimer ();
    }
  else
    {
      if (! paused)
	pauseProc ();
      else
	restart ();
    }
}


void  addScore (int sc_x, int sc_y)
{
  long tmp_sc;

  if (tmp_blocks > 0)
    {
      if (chain_step == 0)
	tmp_sc = 1000;
      else
	tmp_sc = tmp_blocks * 10
	  * chain_step * (chain_step + 3) * (tmp_blocks / 3 + 1);
      showTmpScore (tmp_sc, sc_x, sc_y, chain_step);
      usleep (200000);
//      RedrawBoard (board_w); XXX
      sc += tmp_sc;
      blocks += tmp_blocks;
    }
}


void  makeNext ()
{
  static int counter = 0;

  if (next_i.col[0] == STAR)
    {
      drop_i.col[0] = EMPTY;
      star_comes = TRUE;
      drop_i.x = 4;
      drop_i.y = 0;
    }
  else
    {
		int i;
      star_comes = FALSE;
      for (i = 0; i < 3; i++)
	drop_i.col[i] = next_i.col[i];
      drop_i.rot = next_i.rot;
      drop_i.x = 4;
      drop_i.y = 1;
    }
  drop_i.blocknum = counter++;
  movedown_counter = 0;

  decideNextItem ();
  printNextItem ();

  if (! star_comes)
    {
		int i;
		for(i = 0 ; i < 3 ; i++)
		{
			  int rx = drop_i.x + iRot_vx[drop_i.rot][i];
			  int ry = drop_i.y + iRot_vy[drop_i.rot][i];
			  if(board[rx][ry].blk != EMPTY || board[rx][ry+1].blk != EMPTY)
				  endGame();
		}
    }
  else
    {
      if (! (board[drop_i.x][drop_i.y].blk == EMPTY))
	endGame ();
    }
}


static void  pauseProc ()
{
	PangoLayout *layout;
	PangoRectangle rect;
	gtk_button_set_label(GTK_BUTTON(start), _("Start"));
	paused = TRUE;
	stopTimer ();

	saved_screen = gdk_pixmap_new(board_pix, WIN_WIDTH, WIN_HEIGHT, -1);
	gdk_draw_drawable(saved_screen, draw_gc, board_pix, 0, 0, 0, 0, -1, -1);

	gdk_draw_rectangle(board_pix, delete_gc, TRUE, 0, 0,
		WIN_WIDTH, WIN_HEIGHT);

	gdk_draw_rectangle(board_pix, draw_gc, FALSE, DIFF_X / 2, DIFF_Y / 2,
		  BLOCK_WIDTH * BOARD_WIDTH + DIFF_X,
		  BLOCK_HEIGHT * BOARD_HEIGHT + DIFF_Y);

	layout = gtk_widget_create_pango_layout (board_w, _("PAUSE!"));
	pango_layout_set_font_description(layout, pause_font);
	pango_layout_get_pixel_extents(layout, &rect, NULL);
	gdk_draw_layout(board_pix, draw_gc,
		(WIN_WIDTH/2)-(rect.width/2), 220, layout);
	g_object_unref (layout);

	clearNextItem ();

	gtk_widget_queue_draw(board_w);
}


static void  endGame ()
{
	PangoLayout *layout;
	PangoRectangle rect;
	gtk_button_set_label(GTK_BUTTON(start), _("Start"));
	gameover_flag = TRUE;
	if (timer)
		stopTimer ();
	layout = gtk_widget_create_pango_layout (board_w, _("<<< GAME OVER >>>"));
	pango_layout_set_font_description(layout, game_over_font);
	pango_layout_get_pixel_extents(layout, &rect, NULL);
	gdk_draw_layout(board_pix, draw_gc,
		(WIN_WIDTH/2)-(rect.width/2), 220, layout);
	g_object_unref (layout);
	gtk_widget_queue_draw(board_w);

  update_highscore_table ();
  PrintHighScores ();
}


static void  decideMaxRnd ()
{
  int  x, y, danger_blocks = 0;

  for (y = 0; y <= 4; y++)
    for (x = 1; x <= BOARD_WIDTH; x++)
      if (board[x][y].blk != EMPTY)
	danger_blocks++;
  if (danger_blocks < 6)  max_rnd = 120 - (blocks / 200) / 2;
  else if (danger_blocks < 12)  max_rnd = 60 - (blocks / 200) / 4;
  else if (danger_blocks < 20)  max_rnd = 10 - (blocks / 200) / 16;
  else  max_rnd = 5;
  star_gauge += danger_blocks;
  if (star_gauge > 100)
    {
      max_rnd = 4;
      star_gauge = 0;
    }
}
