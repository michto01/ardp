AM_CFLAGS = -I$(top_srcdir)/include

AM_CFLAGS += -fblocks

bin_PROGRAMS = ardp

ardp_SOURCES = ardp.c

ardp_LDADD   = ../lib/libardp_parser_turtle.a  \
               ../lib/libardp_lexer_turtle.a \
               ../lib/libardpstring.a \
               ../lib/libhashmap.a    \
               $(AM_LDFLAGS)          \
               -lz -lz -lbz2

ardp_LDFLAGS = --coverage
