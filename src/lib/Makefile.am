AM_CFLAGS= -I$(top_srcdir)/include -Wno-pointer-sign @BLOCKS_CFLAGS@ @BLOCKS_LDFLAGS@

if ARDP_ON_UNIX
	AM_CFLAGS +="-fblocks -lBlocksRuntime -ldispatch"
endif

noinst_LIBRARIES= libardpstring.a \
		  libhashmap.a \
		  libardp_lexer_turtle.a \
		  libardp_parser_turtle.a

libardpstring_a_SOURCES= color.c string.c

libhashmap_a_SOURCES= hash.c hashmap.c

libardp_lexer_turtle_a_SOURCES= lexers/turtle.c #rl
libardp_lexer_turtle_a_LIBADD= libardpstring.a

libardp_parser_turtle_a_SOURCES= parsers/test_lemon.c \
				 parsers/turtle_parser.c \
				 parsers/turtle_parser.h \
				 sequence.c          \
				 rdf/rdf_term.c      \
				 rdf/rdf_statement.c
libardp_parser_turtle_a_LIBADD= libardpstring.a \
				libhashmap.a
#.c.rl:
#	@echo "Working on :" $< " to produce:" $@
#	ragel -C -G2 -o $@ $<

#%.c : %.rl
#	@echo "Working on :" $< " to produce:" $@
#	ragel -C -G2 -o $@ $<


#libardp_a_SOURCES= parser.rl parser.c
#libardp_a_LIBADD= libardpstring.a
