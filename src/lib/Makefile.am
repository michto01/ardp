AM_CFLAGS= -I../../include -I$(top_srcdir)/include @AM_CXXFLAGS@

# %.c: %.c.rl
#SUFFIXES= .c.rl

#.c.c.rl:
#	ragel -C -G2 -o $@ $<

noinst_LIBRARIES=libardpstring.a libardp.a

libardpstring_a_SOURCES= string.c

libardp_a_SOURCES= parser.c
libardp_a_LIBADD= libardpstring.a
