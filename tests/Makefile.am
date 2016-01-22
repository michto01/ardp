testsdir = $(datadir)/tests/@PACKAGE@
doc_TESTS = test.ntriples test2.ntriples test3.ntriples

TESTS = check_hashmap_create \
        check_hashmap_push   \
        check_hashmap_get    \
        check_hashmap_resize

check_PROGRAMS = check_hashmap_create \
                 check_hashmap_push   \
                 check_hashmap_get    \
                 check_hashmap_resize


check_hashmap_create_SOURCES = check_hashmap_create.c
check_hashmap_create_CFLAGS  = -I$(top_srcdir)/include
check_hashmap_create_LDADD   = ../src/lib/libhashmap.a

check_hashmap_push_SOURCES   = check_hashmap_push.c
check_hashmap_push_CFLAGS    = -I$(top_srcdir)/include
check_hashmap_push_LDADD     = ../src/lib/libhashmap.a

check_hashmap_get_SOURCES    = check_hashmap_get.c
check_hashmap_get_CFLAGS     = -I$(top_srcdir)/include
check_hashmap_get_LDADD      = ../src/lib/libhashmap.a

check_hashmap_resize_SOURCES = check_hashmap_resize.c
check_hashmap_resize_CFLAGS  = -I$(top_srcdir)/include
check_hashmap_resize_LDADD   = ../src/lib/libhashmap.a
