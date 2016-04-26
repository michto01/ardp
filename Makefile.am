AUTOMAKE_OPTIONS = foreign


# libtool requires some M4 scripts to be added to the source tree.  Make sure that
# Autoconf knows where to find them.
#ACLOCAL_AMFLAGS = -I m4

#force clang
CC=clang
AM_CFLAGS = $(AM_CFLAGS)

if ARDP_ON_UNIX
	AM_CFLAGS += -fblocks -lBlocksRuntime -ldispatch
endif

SUBDIRS = src \
          doc \
          man \
          scripts \
          tests \
          examples

EXTRA_DIST = include \
			 examples
