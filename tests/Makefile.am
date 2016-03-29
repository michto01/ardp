testsdir = $(datadir)/tests/@PACKAGE@
doc_TESTS = test.ntriples test2.ntriples test3.ntriples

TESTS = check_hashmap_create \
        check_hashmap_push   \
        check_hashmap_get    \
        check_hashmap_resize \
        check_string         \
	check_string_create

check_PROGRAMS = check_hashmap_create \
                 check_hashmap_push   \
                 check_hashmap_get    \
                 check_h\shmap_resize \
                 check_string         \
		 check_string_create

check_hashmap_create_SOURCES = check_hashmap_create.c $(top_srcdir)/src/lib/hash.c $(top_srcdir)/src/lib/hashmap.c
check_hashmap_create_CFLAGS  = -I$(top_srcdir)/include -Wno-pointer-sign

check_hashmap_push_SOURCES   = check_hashmap_push.c $(top_srcdir)/src/lib/hash.c $(top_srcdir)/src/lib/hashmap.c
check_hashmap_push_CFLAGS    = -I$(top_srcdir)/include

check_hashmap_get_SOURCES    = check_hashmap_get.c $(top_srcdir)/src/lib/hash.c $(top_srcdir)/src/lib/hashmap.c
check_hashmap_get_CFLAGS     = -I$(top_srcdir)/include

check_hashmap_resize_SOURCES = check_hashmap_resize.c $(top_srcdir)/src/lib/hash.c $(top_srcdir)/src/lib/hashmap.c
check_hashmap_resize_CFLAGS  = -I$(top_srcdir)/include

check_string_SOURCES = check_string.c $(top_srcdir)/src/lib/string.c
check_string_CFLAGS  = -I$(top_srcdir)/include -Wno-pointer-sign

check_string_create_SOURCES = check_string_create.c $(top_srcdir)/src/lib/string.c
check_string_create_CFLAGS  = -I$(top_srcdir)/include -Wno-pointer-sign
