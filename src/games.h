/*                              */
/* gtkemeraldia     ----- games.h */
/*                              */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphics.h"

#define LUCKY_NUMBER  1

#define MOVE_LEFT    -1
#define MOVE_RIGHT    1
#define MOVE_DOWN     1

#define CLOCKWIZE           1
#define COUNTER_CLOCKWIZE  -1


extern gboolean   gameover_flag;
extern long  sc, blocks, tmp_blocks, chain_step;
extern gboolean   paused;
extern gboolean   star_comes;
extern int   movedown_counter;
extern char *name, *programname;
extern int offset_down;

extern const int  iRot_vx[4][3];
extern const int  iRot_vy[4][3];
extern const int  iCrChk[4][2];

struct Board
{
   cellstatus_t blk;   /* blocks */
   cellsubstatus_t sub;   /* sub    */
   cellstatus_t chk;   /* check  */
};
extern struct Board board[BOARD_WIDTH + 2][BOARD_HEIGHT + 1];

struct DropItem {
   cellstatus_t  col[3];
   int  rot;
   int  x, y;
   int blocknum;
};
extern struct DropItem  drop_i, next_i;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define usleep(x) Sleep((x) * 1000)
#endif
