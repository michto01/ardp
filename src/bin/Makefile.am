AM_CFLAGS = -I../../include -I$(top_srcdir)/include @AM_CXXFLAGS@

bin_PROGRAMS = ardp

ardp_SOURCES = ardp.c
ardp_LDADD   = $(top_builddir)/src/lib/libardp.a \
               $(top_builddir)/src/lib/libardpstring.a \
               $(AM_LDFLAGS)
