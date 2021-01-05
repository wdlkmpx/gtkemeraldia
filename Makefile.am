
gamesdir = ${prefix}/games
games_PROGRAMS = xemeraldia

xemeraldia_SOURCES = crack.c games.c graphics.c init-graphics.c main.c \
	move.c score.c bitmaps.h config.h games.h gettext.h graphics.h \
	bitmaps/block1cr.xbm bitmaps/block1.xbm bitmaps/block2cr.xbm \
	bitmaps/block2.xbm bitmaps/block3cr.xbm bitmaps/block3.xbm \
	bitmaps/block4cr.xbm bitmaps/block4.xbm bitmaps/block5cr.xbm \
	bitmaps/block5.xbm bitmaps/block6cr.xbm bitmaps/block6.xbm \
	bitmaps/crush0.xbm \
	bitmaps/crush1.xbm bitmaps/crush2.xbm bitmaps/crush3.xbm \
	bitmaps/crush4.xbm bitmaps/xemeraldia.xpm bitmaps/star.xbm
xemeraldia_LDADD = $(LIBOBJS) @XEMERALDIA_LIBS@
xemeraldia_CFLAGS=@XEMERALDIA_CFLAGS@

appsdir = $(datadir)/applications
apps_DATA = xemeraldia.desktop

pixmapsdir = $(datadir)/pixmaps
dist_pixmaps_DATA = bitmaps/xemeraldia.xpm

SUBDIRS = m4 po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath NEWS README.jp xemeraldia.desktop.in TODO

AM_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" \
	-DHIGH_SCORE_TABLE=\"/var/games/xemeraldia.scores\"

# automake thinks this is data!
install-data-hook:
	-chgrp games $(DESTDIR)$(gamesdir)/xemeraldia
	-chmod g+s $(DESTDIR)$(gamesdir)/xemeraldia
