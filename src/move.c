/*                             */
/* xemeraldia    ------ move.c */
/*                             */

#include "games.h"

static void rotation(int rot);

void  Rotation ()
{
   if ((! gameover_flag) && (!paused))
      rotation (CLOCKWIZE);
}


void  CCRotation ()
{
   if ((! gameover_flag) && (!paused))
      rotation (COUNTER_CLOCKWIZE);
}


void  MoveLeft ()
{
   if ((! gameover_flag) && (!paused))
      moveItem (MOVE_LEFT, 0);
}


void  MoveRight ()
{
   if ((! gameover_flag) && (!paused))
      moveItem (MOVE_RIGHT, 0);
}


void  MoveDown ()
{
   if ((! gameover_flag) && (!paused))
   {
      if (timer) {
         stopTimer ();
      }
      if (star_comes) {
         while (CanStarMove (0, MOVE_DOWN))
         {
            moveItem (0, MOVE_DOWN);
            sc += 1;
         }
      } else {
         while (CanItemMove (0, MOVE_DOWN, drop_i.rot))
         {
            moveItem (0, MOVE_DOWN);
            sc += 1;
         }
      }
      printScore ();
      if (++movedown_counter < BOARD_HEIGHT) {
         startTimer ();
      } else {
         DropItem ();
      }
   }
}


static void rotation (int rot)
{
   int  vrot, i;

   if (! gameover_flag)
   {
      vrot = drop_i.rot + rot;
      if (vrot == 4) {
         vrot = 0;
      } else  if (vrot == -1) {
         vrot = 3;
      }
      if (CanItemMove (0, 0, vrot))
      {
         for (i = 0; i < 3; i++)
         {
            board[drop_i.x + iRot_vx[drop_i.rot][i]]
                  [drop_i.y + iRot_vy[drop_i.rot][i]].blk = EMPTY;
            board[drop_i.x + iRot_vx[drop_i.rot][i]]
                  [drop_i.y + iRot_vy[drop_i.rot][i]].sub = EMPTY_SUB;
         }
         drop_i.rot += rot;
         if (drop_i.rot == 4) {
            drop_i.rot = 0;
         } else  if (drop_i.rot == -1) {
            drop_i.rot = 3;
         }
         printItem ();
      }
   }
}


void  moveItem (int vx, int vy)
{
   int  i;

   if (star_comes)
   {
      if (! CanStarMove (vx, vy)) {
         vx = 0;  vy = 0;
      }
      if ((vx != 0) || (vy != 0)) {
         board[drop_i.x][drop_i.y].sub = EMPTY_SUB;
         board[drop_i.x][drop_i.y].blk = EMPTY;
         drop_i.x += vx;
         drop_i.y += vy;
         printItem ();
      }
   }
   else
   {
      if (! CanItemMove (vx, vy, drop_i.rot)) {
         vx = 0;  vy = 0;
      }
      if ((vx != 0) || (vy != 0)) {
         for (i = 0; i < 3; i++)
         {
            board[drop_i.x + iRot_vx[drop_i.rot][i]]
                  [drop_i.y + iRot_vy[drop_i.rot][i]].blk = EMPTY;
            board[drop_i.x + iRot_vx[drop_i.rot][i]]
                  [drop_i.y + iRot_vy[drop_i.rot][i]].sub = EMPTY_SUB;
         }
         drop_i.x += vx;
         drop_i.y += vy;
         printItem ();
      }
   }
   if (vy != 0) {
      offset_down = 0;
   }
}


gboolean CanItemMove(int vx, int vy, int vrot)
{
   int  flag, i;

   /* If we are moving sideways, we need to check the next row, since the block
    * might have already have been shown next to another block
    * (since adding "soft fall").
    */
   if (vx != 0) {
      vy++;
   }

   for (i = 0; i < 3; i++)
      board[drop_i.x + iRot_vx[drop_i.rot][i]]
            [drop_i.y + iRot_vy[drop_i.rot][i]].blk = EMPTY;

   if ((board[drop_i.x + iRot_vx[vrot][0] + vx]
            [drop_i.y + iRot_vy[vrot][0] + vy].blk == EMPTY) &&
       (board[drop_i.x + iRot_vx[vrot][1] + vx]
            [drop_i.y + iRot_vy[vrot][1] + vy].blk == EMPTY) &&
       (board[drop_i.x + iRot_vx[vrot][2] + vx]
            [drop_i.y + iRot_vy[vrot][2] + vy].blk == EMPTY))
   {
    flag = TRUE;
   } else {
    flag = FALSE;
   }

   /*
    * This shouldn't be here!
    */
   for (i = 0; i < 3; i++) {
      board[drop_i.x + iRot_vx[drop_i.rot][i]]
            [drop_i.y + iRot_vy[drop_i.rot][i]].blk = drop_i.col[i];
   }
   return flag;
}


gboolean CanStarMove(int vx, int vy)
{
   /* If we are moving sideways, we need to check the next row, since the block
    * might have already have been shown next to another block
    * (since adding "soft fall").
    */
   if(vx != 0) {
      vy++;
   }

   if(board[drop_i.x + vx][drop_i.y + vy].blk == EMPTY) {
      return TRUE;
   } else {
      return FALSE;
   }
}
