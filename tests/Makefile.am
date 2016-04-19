testsdir = $(datadir)/tests/@PACKAGE@
doc_TESTS = test.ntriples test2.ntriples test3.ntriples

TESTS = check_hashmap_create \
        check_hashmap_push   \
        check_hashmap_get    \
        check_hashmap_resize \
        check_string         \
        check_string_create  \
        check_string_copy    \
        check_string_append  \
        check_string_append_str \
        check_string_generic_cmp \
        check_string_strlen \
        check_sequence_free \
				check_rdf_term_create

check_PROGRAMS = check_hashmap_create \
                 check_hashmap_push   \
                 check_hashmap_get    \
                 check_hashmap_resize \
                 check_string         \
                 check_string_create  \
                 check_string_copy    \
                 check_string_append  \
                 check_string_append_str \
                 check_string_generic_cmp \
                 check_string_strlen \
                 check_sequence_free \
								 check_rdf_term_create


check_hashmap_create_SOURCES = check_hashmap_create.c
check_hashmap_create_CFLAGS  = -I$(top_srcdir)/include -Wno-pointer-sign
check_hashmap_create_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                               $(top_builddir)/src/lib/libhashmap.a    \
                               $(AM_LDFLAGS)

check_hashmap_push_SOURCES   = check_hashmap_push.c
check_hashmap_push_CFLAGS    = -I$(top_srcdir)/include
check_hashmap_push_LDADD     = $(top_builddir)/src/lib/libardpstring.a \
                               $(top_builddir)/src/lib/libhashmap.a    \
                               $(AM_LDFLAGS)

check_hashmap_get_SOURCES    = check_hashmap_get.c
check_hashmap_get_CFLAGS     = -I$(top_srcdir)/include
check_hashmap_get_LDADD      = $(top_builddir)/src/lib/libardpstring.a \
                               $(top_builddir)/src/lib/libhashmap.a    \
                               $(AM_LDFLAGS)

check_hashmap_resize_SOURCES = check_hashmap_resize.c
check_hashmap_resize_CFLAGS  = -I$(top_srcdir)/include
check_hashmap_resize_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                               $(top_builddir)/src/lib/libhashmap.a    \
                               $(AM_LDFLAGS)


check_string_SOURCES = check_string.c
check_string_CFLAGS  = -I$(top_srcdir)/include
check_string_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                       $(AM_LDFLAGS)

check_string_create_SOURCES = check_string_create.c
check_string_create_CFLAGS  = -I$(top_srcdir)/include
check_string_create_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                              $(AM_LDFLAGS)

check_string_copy_SOURCES   = check_string_copy.c
check_string_copy_CFLAGS    = -I$(top_srcdir)/include
check_string_copy_LDADD     = $(top_builddir)/src/lib/libardpstring.a \
                              $(AM_LDFLAGS)

check_string_append_SOURCES = check_string_append.c
check_string_append_CFLAGS  = -I$(top_srcdir)/include
check_string_append_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                              $(AM_LDFLAGS)

check_string_append_str_SOURCES = check_string_append_str.c
check_string_append_str_CFLAGS  = -I$(top_srcdir)/include
check_string_append_str_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                                  $(AM_LDFLAGS)

check_string_generic_cmp_SOURCES = check_string_generic_cmp.c
check_string_generic_cmp_CFLAGS  = -I$(top_srcdir)/include
check_string_generic_cmp_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                                   $(AM_LDFLAGS)

check_string_strlen_SOURCES = check_string_strlen.c
check_string_strlen_CFLAGS  = -I$(top_srcdir)/include
check_string_strlen_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
                              $(AM_LDFLAGS)

check_sequence_free_SOURCES = check_sequence_free.c
check_sequence_free_CFLAGS  = -I$(top_srcdir)/include
check_sequence_free_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
															$(top_builddir)/src/lib/libardp_parser_turtle.a \
                              $(AM_LDFLAGS)

check_rdf_term_create_SOURCES = check_rdf_term_create.c
check_rdf_term_create_CFLAGS  = -I$(top_srcdir)/include
check_rdf_term_create_LDADD   = $(top_builddir)/src/lib/libardpstring.a \
															  $(top_builddir)/src/lib/libardp_parser_turtle.a \
                                $(AM_LDFLAGS)
