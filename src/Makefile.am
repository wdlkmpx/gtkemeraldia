AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DLOCALEDIR=\"$(datadir)/locale\"

bin_PROGRAMS = gtkemeraldia

gtkemeraldia_SOURCES = crack.c \
	games.c \
	graphics.c \
	init-graphics.c \
	main.c \
	move.c \
	score.c

gtkemeraldia_LDADD = $(LIBOBJS) @GTK_LIBS@

gtkemeraldia_CFLAGS = @GTK_CFLAGS@
