AM_CFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = ardp

ardp_SOURCES = ardp.c                 \
               string.test.c

ardp_LDADD   = ../lib/libardp.a       \
               ../lib/libardpstring.a \
               ../lib/libhashmap.a    \
               $(AM_LDFLAGS)          \
               -lz -lz -lbz2

ardp_LDFLAGS = -coverage
