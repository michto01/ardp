AM_CFLAGS = -I../../include

noinst_LIBRARIES=libardpstring.a libardp.a

libardstring_a_SOURCES = string.c

libardp_a_SOURCES = parser.c
libadrp_a_LDADD = libadrpstring.a

%.c: %.c.rl
	ragel -C -G2 -o $@ $<
