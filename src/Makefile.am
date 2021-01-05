AM_CPPFLAGS = -DLOCALEDIR=\"$(datadir)/locale\" \
	-DHIGH_SCORE_TABLE=\"/var/games/xemeraldia.scores\"

bin_PROGRAMS = xemeraldia

xemeraldia_SOURCES = crack.c \
	games.c \
	graphics.c \
	init-graphics.c \
	main.c \
	move.c \
	score.c

xemeraldia_LDADD = $(LIBOBJS) @GTK_LIBS@

xemeraldia_CFLAGS = @GTK_CFLAGS@
