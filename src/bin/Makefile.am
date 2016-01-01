AM_CFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = ardp

ardp_SOURCES = ardp.c string.test.c
ardp_LDADD   = ../lib/libardp.a \
               ../lib/libardpstring.a \
               $(AM_LDFLAGS) -lz -lz -lbz2
