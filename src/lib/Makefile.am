AM_CFLAGS = -I../../include

noinst_LIBRARIES=libardpstring.a libardp.a

libardpstring_a_SOURCES = string.c

libardp_a_SOURCES = parser.c
libardp_a_LIBADD  = libardpstring.a

%.c: %.c.rl
	ragel -C -G2 -o $@ $<
