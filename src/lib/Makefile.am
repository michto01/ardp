AM_CFLAGS= -I$(top_srcdir)/include

noinst_LIBRARIES= libardpstring.a libardp.a libhashmap.a

libardpstring_a_SOURCES= color.c string.c

libhashmap_a_SOURCES= hash.c hashmap.c

#.c.rl:
#	@echo "Working on :" $< " to produce:" $@
#	ragel -C -G2 -o $@ $<

%.c : %.rl
	@echo "Working on :" $< " to produce:" $@
	ragel -C -G2 -o $@ $<


libardp_a_SOURCES= parser.rl parser.c
libardp_a_LIBADD= libardpstring.a
