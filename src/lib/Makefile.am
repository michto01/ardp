AM_CFLAGS= -I$(top_srcdir)/include

# %.c: %.c.rl
#SUFFIXES= .c.rl

.c.rl:
	ragel -C -G2 -o $@ $<

#%.c: %.c.rl
#	echo "Working on :" $< " to produce:" $@
#	ragel -C -G2 -o $@ $<

noinst_LIBRARIES= libardpstring.a libardp.a

libardpstring_a_SOURCES= string.c

libardp_a_SOURCES= parser.rl parser.c
libardp_a_LIBADD= libardpstring.a
