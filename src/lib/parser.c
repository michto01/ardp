
#line 1 "parser.c.rl"
#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <ardp/util.h>
#include <ardp/parser.h>
#include <ardp/parser_internal.h>

#define BUFSIZE 0x10000

static unsigned int parse_hex( const uint8_t *src, unsigned int len ) {
    unsigned int i;
    unsigned int j;

    for ( i = j = 0; j < len; j++ ) {
        i *= 16;
        char c = src[j];
        if ( isxdigit(c) ) {
            if ( isdigit(c) ) {
                i += c - '0';
            } else {
                i += toupper(c) - 'A' + 10;
            }
        }
    }
    return i;
}

static void emit( ardp_parser *parser, ardp_token_type type ) {
        parser->handler( type, parser->string, parser->handler_arg );
}

/**
  * Main definition for the N-triples parser.
  */

#line 41 "parser.c"
static const int ntriples_start = 146;
static const int ntriples_first_final = 146;
static const int ntriples_error = 0;

static const int ntriples_en_skip_line = 145;
static const int ntriples_en_main = 146;


#line 141 "parser.c.rl"


ardp_parser* ardp_new_parser( void ) {
    ardp_parser *parser              = malloc(sizeof(ardp_parser));
                  parser->line        = 1;
                  parser->finished    = 0;
                  parser->string      = NULL;
                  parser->reader      = NULL;
                  parser->reader_arg  = NULL;
                  parser->handler     = NULL;
                  parser->handler_arg = NULL;
    return parser;
}

void ardp_free_parser( ardp_parser *parser ) {
    if ( parser->string isnt NULL )
        string_dealloc( parser->string );
    free(parser);
}

void ardp_parser_set_reader( ardp_parser *parser, ardp_reader reader, void *reader_arg ) {
    parser->reader     = reader;
    parser->reader_arg = reader_arg;
}

void ardp_parser_set_handler( ardp_parser *parser, ardp_handler handler, void *handler_arg ) {
    parser->handler     = handler;
    parser->handler_arg = handler_arg;
}

//typedef unsigned char uchar;

int ardp_parser_parse(ardp_parser *parser) {
    if ( parser->reader is NULL or parser->handler is NULL )
        return 0;

    int cs;

    
#line 90 "parser.c"
	{
	cs = ntriples_start;
	}

#line 180 "parser.c.rl"

    uint8_t buf[BUFSIZE];
    uint8_t *mark = 0;

    int have = 0;
    int res  = 1;

    ardp_reader reader = parser->reader;

    while ( not parser->finished ) {
        uint8_t *pe;
        uint8_t *p   = buf + have;
        uint8_t *eof = 0;

        int space = BUFSIZE - have;
        if ( space is 0 ) {
            // out of buffer space
            res = 0;
            break;
        }

        int len = reader( p, space, parser->reader_arg );
        pe = p + len;

        if ( len < space ) {
            eof = pe;
            parser->finished = 1;
        }

        
#line 126 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr2:
#line 80 "parser.c.rl"
	{
        parser->line++;
    }
	goto st146;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
#line 142 "parser.c"
	switch( (*p) ) {
		case 9u: goto st1;
		case 10u: goto tr2;
		case 13u: goto tr2;
		case 32u: goto st1;
		case 35u: goto st2;
		case 60u: goto st3;
		case 95u: goto tr5;
	}
	goto tr0;
tr0:
#line 135 "parser.c.rl"
	{
        printf(_CLR_RED "[ERROR]" _CLR_RESET " on line: %d\n", parser->line );
        p--; {goto st145;}
    }
	goto st0;
#line 160 "parser.c"
st0:
cs = 0;
	goto _out;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 9u: goto st1;
		case 10u: goto tr2;
		case 13u: goto tr2;
		case 32u: goto st1;
		case 35u: goto st2;
		case 60u: goto st3;
		case 95u: goto tr5;
	}
	goto tr0;
tr41:
#line 76 "parser.c.rl"
	{
        // TODO: call handler?
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 188 "parser.c"
	switch( (*p) ) {
		case 10u: goto tr2;
		case 13u: goto tr2;
	}
	goto st2;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr6;
	} else if ( (*p) >= 65u )
		goto tr6;
	goto tr0;
tr6:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st4;
tr7:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 234 "parser.c"
	switch( (*p) ) {
		case 43u: goto tr7;
		case 58u: goto tr8;
	}
	if ( (*p) < 48u ) {
		if ( 45u <= (*p) && (*p) <= 46u )
			goto tr7;
	} else if ( (*p) > 57u ) {
		if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr7;
		} else if ( (*p) >= 65u )
			goto tr7;
	} else
		goto tr7;
	goto tr0;
tr8:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st5;
tr189:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 277 "parser.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto st6;
		case 92u: goto tr10;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr8;
tr190:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 306 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr11;
		case 32u: goto tr11;
		case 60u: goto tr12;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr11;
	goto tr0;
tr11:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st7;
tr59:
#line 102 "parser.c.rl"
	{ emit(parser, ARDP_BLANK_NODE); }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 327 "parser.c"
	switch( (*p) ) {
		case 13u: goto st7;
		case 32u: goto st7;
		case 60u: goto st8;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto st7;
	goto tr0;
tr12:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st8;
tr61:
#line 102 "parser.c.rl"
	{ emit(parser, ARDP_BLANK_NODE); }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 348 "parser.c"
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr15;
	} else if ( (*p) >= 65u )
		goto tr15;
	goto tr0;
tr15:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st9;
tr16:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 385 "parser.c"
	switch( (*p) ) {
		case 43u: goto tr16;
		case 58u: goto tr17;
	}
	if ( (*p) < 48u ) {
		if ( 45u <= (*p) && (*p) <= 46u )
			goto tr16;
	} else if ( (*p) > 57u ) {
		if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr16;
		} else if ( (*p) >= 65u )
			goto tr16;
	} else
		goto tr16;
	goto tr0;
tr17:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st10;
tr177:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 428 "parser.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto st11;
		case 92u: goto tr19;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr17;
tr178:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 457 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr20;
		case 32u: goto tr20;
		case 34u: goto tr21;
		case 60u: goto tr22;
		case 95u: goto tr23;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr20;
	goto tr0;
tr20:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 476 "parser.c"
	switch( (*p) ) {
		case 13u: goto st12;
		case 32u: goto st12;
		case 34u: goto st13;
		case 60u: goto st81;
		case 95u: goto tr27;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto st12;
	goto tr0;
tr21:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 495 "parser.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr29;
		case 92u: goto tr30;
	}
	goto tr28;
tr28:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st14;
tr31:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st14;
tr115:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st14;
tr105:
#line 55 "parser.c.rl"
	{
       const unsigned char c = *(mark + 1);
       mark = 0;
       switch ( c ) {
         case 't': string_append_char( &parser->string, '\t'); break;
         case 'b': string_append_char( &parser->string, '\b'); break;
         case 'n': string_append_char( &parser->string, '\n'); break;
         case 'r': string_append_char( &parser->string, '\r'); break;
         case 'f': string_append_char( &parser->string, '\f'); break;
         case '\"': string_append_char( &parser->string, '\"'); break;
         case '\'': string_append_char( &parser->string, '\''); break;
         case '\\': string_append_char( &parser->string, '\\'); break;
         default: printf("%s = %c\n", "MISSED", c);
       }
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 571 "parser.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto st15;
		case 92u: goto tr33;
	}
	goto tr31;
tr29:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
	goto st15;
tr116:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
	goto st15;
tr106:
#line 55 "parser.c.rl"
	{
       const unsigned char c = *(mark + 1);
       mark = 0;
       switch ( c ) {
         case 't': string_append_char( &parser->string, '\t'); break;
         case 'b': string_append_char( &parser->string, '\b'); break;
         case 'n': string_append_char( &parser->string, '\n'); break;
         case 'r': string_append_char( &parser->string, '\r'); break;
         case 'f': string_append_char( &parser->string, '\f'); break;
         case '\"': string_append_char( &parser->string, '\"'); break;
         case '\'': string_append_char( &parser->string, '\''); break;
         case '\\': string_append_char( &parser->string, '\\'); break;
         default: printf("%s = %c\n", "MISSED", c);
       }
    }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 625 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr34;
		case 32u: goto tr34;
		case 46u: goto tr35;
		case 64u: goto tr36;
		case 94u: goto tr37;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr34;
	goto tr0;
tr123:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st16;
tr34:
#line 96 "parser.c.rl"
	{ emit(parser, ARDP_SIMPLE_LITERAL_VALUE); }
	goto st16;
tr153:
#line 102 "parser.c.rl"
	{ emit(parser, ARDP_BLANK_NODE); }
	goto st16;
tr76:
#line 100 "parser.c.rl"
	{ emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }
	goto st16;
tr88:
#line 97 "parser.c.rl"
	{ emit(parser, ARDP_DATATYPE_LITERAL_IRI); }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 660 "parser.c"
	switch( (*p) ) {
		case 13u: goto st16;
		case 32u: goto st16;
		case 46u: goto st17;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto st16;
	goto tr0;
tr124:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st17;
tr35:
#line 96 "parser.c.rl"
	{ emit(parser, ARDP_SIMPLE_LITERAL_VALUE); }
	goto st17;
tr154:
#line 102 "parser.c.rl"
	{ emit(parser, ARDP_BLANK_NODE); }
	goto st17;
tr78:
#line 100 "parser.c.rl"
	{ emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }
	goto st17;
tr89:
#line 97 "parser.c.rl"
	{ emit(parser, ARDP_DATATYPE_LITERAL_IRI); }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 693 "parser.c"
	switch( (*p) ) {
		case 9u: goto st17;
		case 10u: goto tr40;
		case 13u: goto tr40;
		case 32u: goto st17;
		case 35u: goto tr41;
	}
	goto tr0;
tr40:
#line 76 "parser.c.rl"
	{
        // TODO: call handler?
    }
#line 80 "parser.c.rl"
	{
        parser->line++;
    }
	goto st147;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
#line 716 "parser.c"
	switch( (*p) ) {
		case 9u: goto st18;
		case 10u: goto tr40;
		case 13u: goto tr40;
		case 32u: goto st18;
		case 35u: goto tr41;
		case 60u: goto st3;
		case 95u: goto tr5;
	}
	goto tr0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	switch( (*p) ) {
		case 9u: goto st18;
		case 10u: goto tr40;
		case 13u: goto tr40;
		case 32u: goto st18;
		case 35u: goto tr41;
		case 60u: goto st3;
		case 95u: goto tr5;
	}
	goto tr0;
tr5:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 759 "parser.c"
	if ( (*p) == 58u )
		goto st20;
	goto tr0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 95u: goto tr44;
		case 195u: goto tr45;
		case 203u: goto tr47;
		case 205u: goto tr48;
		case 223u: goto tr47;
		case 224u: goto tr49;
		case 225u: goto tr50;
		case 226u: goto tr51;
		case 227u: goto tr52;
		case 237u: goto tr54;
		case 239u: goto tr55;
		case 240u: goto tr56;
		case 243u: goto tr58;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr44;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr44;
		} else
			goto tr44;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto tr46;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr57;
		} else
			goto tr53;
	} else
		goto tr46;
	goto tr0;
tr44:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 814 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr59;
		case 32u: goto tr59;
		case 45u: goto tr44;
		case 46u: goto tr60;
		case 60u: goto tr61;
		case 95u: goto tr44;
		case 183u: goto tr44;
		case 195u: goto tr45;
		case 203u: goto tr47;
		case 204u: goto tr62;
		case 205u: goto tr63;
		case 223u: goto tr47;
		case 224u: goto tr49;
		case 225u: goto tr50;
		case 226u: goto tr64;
		case 227u: goto tr52;
		case 237u: goto tr54;
		case 239u: goto tr55;
		case 240u: goto tr56;
		case 243u: goto tr58;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 48u ) {
			if ( 9u <= (*p) && (*p) <= 10u )
				goto tr59;
		} else if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr44;
		} else
			goto tr44;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr46;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr57;
		} else
			goto tr53;
	} else
		goto tr44;
	goto tr0;
tr60:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 869 "parser.c"
	switch( (*p) ) {
		case 45u: goto tr44;
		case 46u: goto tr60;
		case 95u: goto tr44;
		case 183u: goto tr44;
		case 195u: goto tr45;
		case 203u: goto tr47;
		case 204u: goto tr62;
		case 205u: goto tr63;
		case 223u: goto tr47;
		case 224u: goto tr49;
		case 225u: goto tr50;
		case 226u: goto tr64;
		case 227u: goto tr52;
		case 237u: goto tr54;
		case 239u: goto tr55;
		case 240u: goto tr56;
		case 243u: goto tr58;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr44;
		} else if ( (*p) >= 48u )
			goto tr44;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr46;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr57;
		} else
			goto tr53;
	} else
		goto tr44;
	goto tr0;
tr45:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 918 "parser.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr44;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr44;
	} else
		goto tr44;
	goto tr0;
tr46:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 939 "parser.c"
	goto tr44;
tr47:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 952 "parser.c"
	if ( 192u <= (*p) )
		goto tr0;
	goto tr44;
tr62:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 967 "parser.c"
	if ( (*p) <= 127u )
		goto tr0;
	goto tr44;
tr63:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 982 "parser.c"
	if ( (*p) == 190u )
		goto tr0;
	goto tr44;
tr49:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 997 "parser.c"
	if ( (*p) == 160u )
		goto tr62;
	if ( 161u <= (*p) )
		goto tr46;
	goto tr0;
tr50:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 1014 "parser.c"
	if ( (*p) == 191u )
		goto tr47;
	if ( 192u <= (*p) )
		goto tr0;
	goto tr46;
tr64:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 1031 "parser.c"
	switch( (*p) ) {
		case 128u: goto tr65;
		case 129u: goto tr66;
		case 134u: goto tr67;
		case 176u: goto tr62;
		case 191u: goto tr68;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr46;
	} else if ( (*p) >= 130u )
		goto tr46;
	goto tr0;
tr65:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 1056 "parser.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr44;
	} else if ( (*p) >= 140u )
		goto tr44;
	goto tr0;
tr66:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 1074 "parser.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr44;
tr67:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 1089 "parser.c"
	if ( 144u <= (*p) )
		goto tr0;
	goto tr44;
tr68:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 1104 "parser.c"
	if ( 176u <= (*p) )
		goto tr0;
	goto tr44;
tr52:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 1119 "parser.c"
	if ( (*p) == 128u )
		goto tr69;
	if ( 129u <= (*p) )
		goto tr46;
	goto tr0;
tr69:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 1136 "parser.c"
	if ( 129u <= (*p) )
		goto tr44;
	goto tr0;
tr53:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 1151 "parser.c"
	goto tr46;
tr54:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 1164 "parser.c"
	if ( (*p) == 159u )
		goto tr47;
	if ( 160u <= (*p) )
		goto tr0;
	goto tr46;
tr55:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 1181 "parser.c"
	switch( (*p) ) {
		case 164u: goto tr62;
		case 183u: goto tr70;
		case 191u: goto tr71;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr46;
	goto tr0;
tr70:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 1201 "parser.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr44;
tr71:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 1216 "parser.c"
	if ( 190u <= (*p) )
		goto tr0;
	goto tr44;
tr56:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 1231 "parser.c"
	if ( (*p) == 144u )
		goto tr72;
	if ( 145u <= (*p) )
		goto tr53;
	goto tr0;
tr72:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 1248 "parser.c"
	if ( (*p) == 128u )
		goto tr62;
	if ( 129u <= (*p) )
		goto tr46;
	goto tr0;
tr57:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 1265 "parser.c"
	goto tr53;
tr58:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 1278 "parser.c"
	if ( (*p) == 175u )
		goto tr50;
	if ( 176u <= (*p) )
		goto tr0;
	goto tr53;
tr48:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 1295 "parser.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr44;
	} else if ( (*p) >= 176u )
		goto tr44;
	goto tr0;
tr51:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st47;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
#line 1313 "parser.c"
	switch( (*p) ) {
		case 128u: goto tr73;
		case 129u: goto tr74;
		case 134u: goto tr67;
		case 176u: goto tr62;
		case 191u: goto tr68;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr46;
	} else if ( (*p) >= 130u )
		goto tr46;
	goto tr0;
tr73:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
#line 1338 "parser.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr44;
	goto tr0;
tr74:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st49;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
#line 1353 "parser.c"
	if ( 176u <= (*p) )
		goto tr44;
	goto tr0;
tr36:
#line 99 "parser.c.rl"
	{ emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_VALUE); }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 1365 "parser.c"
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr75;
	} else if ( (*p) >= 65u )
		goto tr75;
	goto tr0;
tr75:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st51;
tr79:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 1402 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr76;
		case 32u: goto tr76;
		case 45u: goto tr77;
		case 46u: goto tr78;
	}
	if ( (*p) < 65u ) {
		if ( 9u <= (*p) && (*p) <= 10u )
			goto tr76;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr79;
	} else
		goto tr79;
	goto tr0;
tr77:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st52;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
#line 1429 "parser.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr80;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr80;
	} else
		goto tr80;
	goto tr0;
tr80:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 1450 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr76;
		case 32u: goto tr76;
		case 45u: goto tr77;
		case 46u: goto tr78;
	}
	if ( (*p) < 48u ) {
		if ( 9u <= (*p) && (*p) <= 10u )
			goto tr76;
	} else if ( (*p) > 57u ) {
		if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr80;
		} else if ( (*p) >= 65u )
			goto tr80;
	} else
		goto tr80;
	goto tr0;
tr37:
#line 98 "parser.c.rl"
	{ emit(parser, ARDP_DATATYPE_LITERAL_VALUE); }
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
#line 1477 "parser.c"
	if ( (*p) == 94u )
		goto st55;
	goto tr0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) == 60u )
		goto st56;
	goto tr0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr83;
	} else if ( (*p) >= 65u )
		goto tr83;
	goto tr0;
tr83:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st57;
tr84:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 1528 "parser.c"
	switch( (*p) ) {
		case 43u: goto tr84;
		case 58u: goto tr85;
	}
	if ( (*p) < 48u ) {
		if ( 45u <= (*p) && (*p) <= 46u )
			goto tr84;
	} else if ( (*p) > 57u ) {
		if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr84;
		} else if ( (*p) >= 65u )
			goto tr84;
	} else
		goto tr84;
	goto tr0;
tr85:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st58;
tr99:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st58;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
#line 1571 "parser.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto st59;
		case 92u: goto tr87;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr85;
tr100:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 1600 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr88;
		case 32u: goto tr88;
		case 46u: goto tr89;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr88;
	goto tr0;
tr87:
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st60;
tr101:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 1633 "parser.c"
	switch( (*p) ) {
		case 85u: goto st61;
		case 117u: goto st65;
	}
	goto tr0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st62;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st62;
	} else
		goto st62;
	goto tr0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st63;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st63;
	} else
		goto st63;
	goto tr0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st64;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st64;
	} else
		goto st64;
	goto tr0;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st65;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st65;
	} else
		goto st65;
	goto tr0;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st66;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st66;
	} else
		goto st66;
	goto tr0;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st67;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st67;
	} else
		goto st67;
	goto tr0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st68;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st68;
	} else
		goto st68;
	goto tr0;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st69;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st69;
	} else
		goto st69;
	goto tr0;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr100;
		case 92u: goto tr101;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr99;
tr33:
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st70;
tr30:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st70;
tr117:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st70;
tr107:
#line 55 "parser.c.rl"
	{
       const unsigned char c = *(mark + 1);
       mark = 0;
       switch ( c ) {
         case 't': string_append_char( &parser->string, '\t'); break;
         case 'b': string_append_char( &parser->string, '\b'); break;
         case 'n': string_append_char( &parser->string, '\n'); break;
         case 'r': string_append_char( &parser->string, '\r'); break;
         case 'f': string_append_char( &parser->string, '\f'); break;
         case '\"': string_append_char( &parser->string, '\"'); break;
         case '\'': string_append_char( &parser->string, '\''); break;
         case '\\': string_append_char( &parser->string, '\\'); break;
         default: printf("%s = %c\n", "MISSED", c);
       }
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st70;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
#line 1825 "parser.c"
	switch( (*p) ) {
		case 34u: goto st71;
		case 39u: goto st71;
		case 85u: goto st72;
		case 92u: goto st71;
		case 98u: goto st71;
		case 102u: goto st71;
		case 110u: goto st71;
		case 114u: goto st71;
		case 116u: goto st71;
		case 117u: goto st76;
	}
	goto tr0;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr106;
		case 92u: goto tr107;
	}
	goto tr105;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st73;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st73;
	} else
		goto st73;
	goto tr0;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st74;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st74;
	} else
		goto st74;
	goto tr0;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st75;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st75;
	} else
		goto st75;
	goto tr0;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st76;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st76;
	} else
		goto st76;
	goto tr0;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st77;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st77;
	} else
		goto st77;
	goto tr0;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st78;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st78;
	} else
		goto st78;
	goto tr0;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st79;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st79;
	} else
		goto st79;
	goto tr0;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st80;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st80;
	} else
		goto st80;
	goto tr0;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr116;
		case 92u: goto tr117;
	}
	goto tr115;
tr22:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
	goto st81;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
#line 1973 "parser.c"
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr118;
	} else if ( (*p) >= 65u )
		goto tr118;
	goto tr0;
tr118:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st82;
tr119:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st82;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
#line 2010 "parser.c"
	switch( (*p) ) {
		case 43u: goto tr119;
		case 58u: goto tr120;
	}
	if ( (*p) < 48u ) {
		if ( 45u <= (*p) && (*p) <= 46u )
			goto tr119;
	} else if ( (*p) > 57u ) {
		if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr119;
		} else if ( (*p) >= 65u )
			goto tr119;
	} else
		goto tr119;
	goto tr0;
tr120:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st83;
tr134:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
#line 2053 "parser.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto st84;
		case 92u: goto tr122;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr120;
tr135:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
	goto st84;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
#line 2082 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr123;
		case 32u: goto tr123;
		case 46u: goto tr124;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr123;
	goto tr0;
tr122:
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st85;
tr136:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st85;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
#line 2115 "parser.c"
	switch( (*p) ) {
		case 85u: goto st86;
		case 117u: goto st90;
	}
	goto tr0;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st87;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st87;
	} else
		goto st87;
	goto tr0;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st88;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st88;
	} else
		goto st88;
	goto tr0;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st89;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st89;
	} else
		goto st89;
	goto tr0;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st90;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st90;
	} else
		goto st90;
	goto tr0;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st91;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st91;
	} else
		goto st91;
	goto tr0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st92;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st92;
	} else
		goto st92;
	goto tr0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st93;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st93;
	} else
		goto st93;
	goto tr0;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st94;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st94;
	} else
		goto st94;
	goto tr0;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr135;
		case 92u: goto tr136;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr134;
tr27:
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
	goto st95;
tr23:
#line 101 "parser.c.rl"
	{ emit(parser, ARDP_IRI); }
#line 84 "parser.c.rl"
	{
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
	goto st95;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
#line 2277 "parser.c"
	if ( (*p) == 58u )
		goto st96;
	goto tr0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	switch( (*p) ) {
		case 95u: goto tr138;
		case 195u: goto tr139;
		case 203u: goto tr141;
		case 205u: goto tr142;
		case 223u: goto tr141;
		case 224u: goto tr143;
		case 225u: goto tr144;
		case 226u: goto tr145;
		case 227u: goto tr146;
		case 237u: goto tr148;
		case 239u: goto tr149;
		case 240u: goto tr150;
		case 243u: goto tr152;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr138;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr138;
		} else
			goto tr138;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto tr140;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr151;
		} else
			goto tr147;
	} else
		goto tr140;
	goto tr0;
tr138:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st97;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
#line 2332 "parser.c"
	switch( (*p) ) {
		case 13u: goto tr153;
		case 32u: goto tr153;
		case 45u: goto tr138;
		case 46u: goto tr154;
		case 95u: goto tr138;
		case 183u: goto tr138;
		case 195u: goto tr139;
		case 203u: goto tr141;
		case 204u: goto tr155;
		case 205u: goto tr156;
		case 223u: goto tr141;
		case 224u: goto tr143;
		case 225u: goto tr144;
		case 226u: goto tr157;
		case 227u: goto tr146;
		case 237u: goto tr148;
		case 239u: goto tr149;
		case 240u: goto tr150;
		case 243u: goto tr152;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 48u ) {
			if ( 9u <= (*p) && (*p) <= 10u )
				goto tr153;
		} else if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr138;
		} else
			goto tr138;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr140;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr151;
		} else
			goto tr147;
	} else
		goto tr138;
	goto tr0;
tr139:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st98;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
#line 2386 "parser.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr138;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr138;
	} else
		goto tr138;
	goto tr0;
tr140:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st99;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
#line 2407 "parser.c"
	goto tr138;
tr141:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st100;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
#line 2420 "parser.c"
	if ( 192u <= (*p) )
		goto tr0;
	goto tr138;
tr155:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st101;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
#line 2435 "parser.c"
	if ( 128u <= (*p) )
		goto tr138;
	goto tr0;
tr156:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st102;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
#line 2450 "parser.c"
	if ( (*p) == 190u )
		goto tr0;
	goto tr138;
tr143:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st103;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
#line 2465 "parser.c"
	if ( (*p) == 160u )
		goto tr155;
	if ( 161u <= (*p) )
		goto tr140;
	goto tr0;
tr144:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st104;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
#line 2482 "parser.c"
	if ( (*p) == 191u )
		goto tr141;
	if ( 192u <= (*p) )
		goto tr0;
	goto tr140;
tr157:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st105;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
#line 2499 "parser.c"
	switch( (*p) ) {
		case 128u: goto tr158;
		case 129u: goto tr159;
		case 134u: goto tr160;
		case 176u: goto tr155;
		case 191u: goto tr161;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr140;
	} else if ( (*p) >= 130u )
		goto tr140;
	goto tr0;
tr158:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st106;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
#line 2524 "parser.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr138;
	} else if ( (*p) >= 140u )
		goto tr138;
	goto tr0;
tr159:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st107;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
#line 2542 "parser.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr138;
tr160:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st108;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
#line 2557 "parser.c"
	if ( 144u <= (*p) )
		goto tr0;
	goto tr138;
tr161:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st109;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
#line 2572 "parser.c"
	if ( 176u <= (*p) )
		goto tr0;
	goto tr138;
tr146:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st110;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
#line 2587 "parser.c"
	if ( (*p) == 128u )
		goto tr162;
	if ( 129u <= (*p) )
		goto tr140;
	goto tr0;
tr162:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st111;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
#line 2604 "parser.c"
	if ( 129u <= (*p) )
		goto tr138;
	goto tr0;
tr147:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st112;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
#line 2619 "parser.c"
	goto tr140;
tr148:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st113;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
#line 2632 "parser.c"
	if ( (*p) == 159u )
		goto tr141;
	if ( 160u <= (*p) )
		goto tr0;
	goto tr140;
tr149:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st114;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
#line 2649 "parser.c"
	switch( (*p) ) {
		case 164u: goto tr155;
		case 183u: goto tr163;
		case 191u: goto tr164;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr140;
	goto tr0;
tr163:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st115;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
#line 2669 "parser.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr138;
tr164:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st116;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
#line 2684 "parser.c"
	if ( 190u <= (*p) )
		goto tr0;
	goto tr138;
tr150:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st117;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
#line 2699 "parser.c"
	if ( (*p) == 144u )
		goto tr165;
	if ( 145u <= (*p) )
		goto tr147;
	goto tr0;
tr165:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st118;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
#line 2716 "parser.c"
	if ( (*p) == 128u )
		goto tr155;
	if ( 129u <= (*p) )
		goto tr140;
	goto tr0;
tr151:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st119;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
#line 2733 "parser.c"
	goto tr147;
tr152:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st120;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
#line 2746 "parser.c"
	if ( (*p) == 175u )
		goto tr144;
	if ( 176u <= (*p) )
		goto tr0;
	goto tr147;
tr142:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st121;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
#line 2763 "parser.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr138;
	} else if ( (*p) >= 176u )
		goto tr138;
	goto tr0;
tr145:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st122;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
#line 2781 "parser.c"
	switch( (*p) ) {
		case 128u: goto tr166;
		case 129u: goto tr167;
		case 134u: goto tr160;
		case 176u: goto tr155;
		case 191u: goto tr161;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr140;
	} else if ( (*p) >= 130u )
		goto tr140;
	goto tr0;
tr166:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st123;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
#line 2806 "parser.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr138;
	goto tr0;
tr167:
#line 71 "parser.c.rl"
	{
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }
	goto st124;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
#line 2821 "parser.c"
	if ( 176u <= (*p) )
		goto tr138;
	goto tr0;
tr19:
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st125;
tr179:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st125;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
#line 2849 "parser.c"
	switch( (*p) ) {
		case 85u: goto st126;
		case 117u: goto st130;
	}
	goto tr0;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st127;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st127;
	} else
		goto st127;
	goto tr0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st128;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st128;
	} else
		goto st128;
	goto tr0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st129;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st129;
	} else
		goto st129;
	goto tr0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st130;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st130;
	} else
		goto st130;
	goto tr0;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st131;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st131;
	} else
		goto st131;
	goto tr0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st132;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st132;
	} else
		goto st132;
	goto tr0;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st133;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st133;
	} else
		goto st133;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st134;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st134;
	} else
		goto st134;
	goto tr0;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr178;
		case 92u: goto tr179;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr177;
tr10:
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st135;
tr191:
#line 47 "parser.c.rl"
	{
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }
#line 43 "parser.c.rl"
	{
        mark = p;
    }
	goto st135;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
#line 3001 "parser.c"
	switch( (*p) ) {
		case 85u: goto st136;
		case 117u: goto st140;
	}
	goto tr0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st137;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st137;
	} else
		goto st137;
	goto tr0;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st138;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st138;
	} else
		goto st138;
	goto tr0;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st139;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st139;
	} else
		goto st139;
	goto tr0;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st140;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st140;
	} else
		goto st140;
	goto tr0;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st141;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st141;
	} else
		goto st141;
	goto tr0;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st142;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st142;
	} else
		goto st142;
	goto tr0;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st143;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st143;
	} else
		goto st143;
	goto tr0;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st144;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st144;
	} else
		goto st144;
	goto tr0;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr190;
		case 92u: goto tr191;
		case 94u: goto tr0;
		case 96u: goto tr0;
	}
	if ( (*p) > 32u ) {
		if ( 123u <= (*p) && (*p) <= 125u )
			goto tr0;
	} else
		goto tr0;
	goto tr189;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	switch( (*p) ) {
		case 10u: goto tr193;
		case 13u: goto tr193;
	}
	goto st145;
tr193:
#line 80 "parser.c.rl"
	{
        parser->line++;
    }
#line 133 "parser.c.rl"
	{ {goto st146;} }
	goto st148;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
#line 3150 "parser.c"
	goto st0;
	}
	_test_eof146: cs = 146; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof147: cs = 147; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof88: cs = 88; goto _test_eof; 
	_test_eof89: cs = 89; goto _test_eof; 
	_test_eof90: cs = 90; goto _test_eof; 
	_test_eof91: cs = 91; goto _test_eof; 
	_test_eof92: cs = 92; goto _test_eof; 
	_test_eof93: cs = 93; goto _test_eof; 
	_test_eof94: cs = 94; goto _test_eof; 
	_test_eof95: cs = 95; goto _test_eof; 
	_test_eof96: cs = 96; goto _test_eof; 
	_test_eof97: cs = 97; goto _test_eof; 
	_test_eof98: cs = 98; goto _test_eof; 
	_test_eof99: cs = 99; goto _test_eof; 
	_test_eof100: cs = 100; goto _test_eof; 
	_test_eof101: cs = 101; goto _test_eof; 
	_test_eof102: cs = 102; goto _test_eof; 
	_test_eof103: cs = 103; goto _test_eof; 
	_test_eof104: cs = 104; goto _test_eof; 
	_test_eof105: cs = 105; goto _test_eof; 
	_test_eof106: cs = 106; goto _test_eof; 
	_test_eof107: cs = 107; goto _test_eof; 
	_test_eof108: cs = 108; goto _test_eof; 
	_test_eof109: cs = 109; goto _test_eof; 
	_test_eof110: cs = 110; goto _test_eof; 
	_test_eof111: cs = 111; goto _test_eof; 
	_test_eof112: cs = 112; goto _test_eof; 
	_test_eof113: cs = 113; goto _test_eof; 
	_test_eof114: cs = 114; goto _test_eof; 
	_test_eof115: cs = 115; goto _test_eof; 
	_test_eof116: cs = 116; goto _test_eof; 
	_test_eof117: cs = 117; goto _test_eof; 
	_test_eof118: cs = 118; goto _test_eof; 
	_test_eof119: cs = 119; goto _test_eof; 
	_test_eof120: cs = 120; goto _test_eof; 
	_test_eof121: cs = 121; goto _test_eof; 
	_test_eof122: cs = 122; goto _test_eof; 
	_test_eof123: cs = 123; goto _test_eof; 
	_test_eof124: cs = 124; goto _test_eof; 
	_test_eof125: cs = 125; goto _test_eof; 
	_test_eof126: cs = 126; goto _test_eof; 
	_test_eof127: cs = 127; goto _test_eof; 
	_test_eof128: cs = 128; goto _test_eof; 
	_test_eof129: cs = 129; goto _test_eof; 
	_test_eof130: cs = 130; goto _test_eof; 
	_test_eof131: cs = 131; goto _test_eof; 
	_test_eof132: cs = 132; goto _test_eof; 
	_test_eof133: cs = 133; goto _test_eof; 
	_test_eof134: cs = 134; goto _test_eof; 
	_test_eof135: cs = 135; goto _test_eof; 
	_test_eof136: cs = 136; goto _test_eof; 
	_test_eof137: cs = 137; goto _test_eof; 
	_test_eof138: cs = 138; goto _test_eof; 
	_test_eof139: cs = 139; goto _test_eof; 
	_test_eof140: cs = 140; goto _test_eof; 
	_test_eof141: cs = 141; goto _test_eof; 
	_test_eof142: cs = 142; goto _test_eof; 
	_test_eof143: cs = 143; goto _test_eof; 
	_test_eof144: cs = 144; goto _test_eof; 
	_test_eof145: cs = 145; goto _test_eof; 
	_test_eof148: cs = 148; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
	case 5: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
	case 12: 
	case 13: 
	case 14: 
	case 15: 
	case 16: 
	case 17: 
	case 18: 
	case 19: 
	case 20: 
	case 21: 
	case 22: 
	case 23: 
	case 24: 
	case 25: 
	case 26: 
	case 27: 
	case 28: 
	case 29: 
	case 30: 
	case 31: 
	case 32: 
	case 33: 
	case 34: 
	case 35: 
	case 36: 
	case 37: 
	case 38: 
	case 39: 
	case 40: 
	case 41: 
	case 42: 
	case 43: 
	case 44: 
	case 45: 
	case 46: 
	case 47: 
	case 48: 
	case 49: 
	case 50: 
	case 51: 
	case 52: 
	case 53: 
	case 54: 
	case 55: 
	case 56: 
	case 57: 
	case 58: 
	case 59: 
	case 60: 
	case 61: 
	case 62: 
	case 63: 
	case 64: 
	case 65: 
	case 66: 
	case 67: 
	case 68: 
	case 69: 
	case 70: 
	case 71: 
	case 72: 
	case 73: 
	case 74: 
	case 75: 
	case 76: 
	case 77: 
	case 78: 
	case 79: 
	case 80: 
	case 81: 
	case 82: 
	case 83: 
	case 84: 
	case 85: 
	case 86: 
	case 87: 
	case 88: 
	case 89: 
	case 90: 
	case 91: 
	case 92: 
	case 93: 
	case 94: 
	case 95: 
	case 96: 
	case 97: 
	case 98: 
	case 99: 
	case 100: 
	case 101: 
	case 102: 
	case 103: 
	case 104: 
	case 105: 
	case 106: 
	case 107: 
	case 108: 
	case 109: 
	case 110: 
	case 111: 
	case 112: 
	case 113: 
	case 114: 
	case 115: 
	case 116: 
	case 117: 
	case 118: 
	case 119: 
	case 120: 
	case 121: 
	case 122: 
	case 123: 
	case 124: 
	case 125: 
	case 126: 
	case 127: 
	case 128: 
	case 129: 
	case 130: 
	case 131: 
	case 132: 
	case 133: 
	case 134: 
	case 135: 
	case 136: 
	case 137: 
	case 138: 
	case 139: 
	case 140: 
	case 141: 
	case 142: 
	case 143: 
	case 144: 
#line 135 "parser.c.rl"
	{
        printf(_CLR_RED "[ERROR]" _CLR_RESET " on line: %d\n", parser->line );
        p--; {goto st145;}
    }
	break;
#line 3456 "parser.c"
	}
	}

	_out: {}
	}

#line 210 "parser.c.rl"

        if ( cs is ntriples_error ) {
            res = 0;
            break;
        } else if (mark) {
            have = pe - mark;
            memmove( buf, mark, have );
            mark = buf;
        } else {
            have = 0;
        }
    }

    return res;
}
