
#line 1 "turtle.rl"
/* vim: set ts=8 sw=4 tw=0 noet : set foldmethod=marker */

// HEADERS {{{ --------------------------------------------------------------------

#include <errno.h>              /* Included for 'stderr' */
#include <iso646.h>             /* verbose || and && */
#include <string.h>             /* C buffer manipulation functions */
#include <assert.h>             /* Asserts in the code */
#include <dispatch/dispatch.h>  /* Clang GCD dispatch_* functions */


#include <ardp/lexer.h>		/* Generic lexer */
#include <ardp/lexer.turtle.h>  /* Turtle specific lexer constants */

#include <ardp/util.h>
#include <ardp/color.h>

//#include "config.h"

//-------------------------------------------------------------------------------}}}

#define BUFSIZE ( 0x10000 )

/**
  * Global variable holding the shared lexer;
  */
static struct lexer *_Nullable shared_lexer;

/*!
 * Shorthand to speedup token input.
 */
#define var( s ) ( shared_lexer->env.s )

/* clang-format off */
/* ragel-machine() {{{*/

#line 40 "lexer2.c"
static const int turtle_start = 162;
static const int turtle_first_final = 162;
static const int turtle_error = -1;

static const int turtle_en_main = 162;


#line 111 "turtle.rl"

/* }}} */
/* clang-format on */


void format( void )
{ /* formating bug off clang-format 3.7 (will be fixed in next release)*/
}

static void log(int level, const char* message)
{
    if (shared_lexer->log.level < level )
        if (shared_lexer->log.eprintf)
            dispatch_async( dispatch_get_main_queue(), ^{
                shared_lexer->log.eprintf(level, message);
            });
}

/* Local function declaration for the internals of lexer object */

/* lexer_emit_token() {{{ */
static void lexer_emit_token( enum turtle_token_type type, uint8_t *_Nullable str, size_t len )
{
        assert( shared_lexer ); /* sanity check */

        if (shared_lexer->cb.token) {
            dispatch_async( shared_lexer->event_queue, ^{
                char* p_str = malloc( ( len + 1 ) * sizeof( *p_str ) );
                assert(p_str); /* Sanity check for the malloc() */

                p_str[0] = '\0';
                strncat( p_str, ( const char * )str, len );

                shared_lexer->cb.token( type, p_str );
                if (shared_lexer->log.level < NOTICE)
                    log( DEBUG, "Token emmitted" );
            });
        }
}
/*}}}*/
/* lexer_emit_token_const() {{{ */
static void lexer_emit_token_const( enum turtle_token_type type )
{
        assert( shared_lexer ); /* sanity check */

        if ( shared_lexer->cb.token ) {
            dispatch_async( shared_lexer->event_queue, ^{
                shared_lexer->cb.token( type, NULL );

                if (shared_lexer->log.level < NOTICE)
                    log( DEBUG, "Token emmitted");
            });
        }
}
/*}}}*/
/* ardp_lexer_turtle_debug() {{{ */
void ardp_lexer_turtle_debug( void )
{
        const char states[][15] = {"created", "initialized", "ready", "unknown"};
        printf( "Shared lexer %s created.\n", shared_lexer ? "is" : "isn't" );
        printf( "It's state is: %s\n", states[shared_lexer->state] );
}
/*}}}*/
/* ardp_lexer_turtle_create() {{{ */
int ardp_lexer_create(void)
{
        if ( unlikely(shared_lexer isnt NULL) ) {
            log( WARNING, "The lexer did already exist. Destroying it." );
            ardp_lexer_destroy();
        }

        shared_lexer = malloc(sizeof(struct lexer));

        if ( shared_lexer is NULL ) {
            log( ERROR, "Didn't allocate the shared lexer. (MALLOC ERROR)");
            //return ARDP_ERROR_LEXER_MALLOC;
            return ARDP_FAILURE;
        }

        shared_lexer->lexer_queue =
            dispatch_queue_create( "eu.cre8iv.ardp.Lexer", DISPATCH_QUEUE_SERIAL );

        shared_lexer->event_queue =
            dispatch_queue_create( "eu.cre8iv.ardp.LexerEvent", 0 );

        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_CREATED;

        log ( INFO, "Created the shared lexer." );

        return ARDP_SUCCESS;
}
/*}}}*/
/* ardp_lexer_turtle_defaults() {{{ */
int ardp_lexer_defaults(void)
{
    switch(ardp_lexer_state()) {
    case ARDP_LEXER_TURTLE_STATUS_CREATED:
    {
        struct lexer *this = shared_lexer;
                      this->line  = 0;
                      this->state = ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED;

        log(INFO, "Defaults for loaded");
        return ARDP_SUCCESS;
    }
    default:
        log(ERROR, "Invalid state of the lexer. Defaults not loaded!");
        return ARDP_FAILURE;
    }
}
/*}}}*/
/* ardp_lexer_turtle_init() {{{*/
int ardp_lexer_init( struct ardp_lexer_config *_Nullable cfg)
{
    switch( ardp_lexer_state() ) {
    case ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED:
    case ARDP_LEXER_TURTLE_STATUS_READY:
        /* clang-format off */
        
#line 168 "lexer2.c"
	{
	 shared_lexer->env.cs = turtle_start;
	 shared_lexer->env.ts = 0;
	 shared_lexer->env.te = 0;
	 shared_lexer->env.act = 0;
	}

#line 230 "turtle.rl"
        /* clang-format on */
        shared_lexer->cb.token    = cfg->cb.token;
        shared_lexer->cb.stoken   = cfg->cb.stoken;
        shared_lexer->log.eprintf = cfg->logging.eprintf;
        shared_lexer->log.level   = cfg->logging.level;

        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_READY;
        log( INFO, "Shared lexer initialized." );
        return ARDP_SUCCESS;

    default:
        log( ERROR, "Wrong state. Initialization aborted." );
        return ARDP_FAILURE;
    }
}
/*}}}*/
/* ardp_lexer_turtle_destroy() {{{ */
void ardp_lexer_destroy()
{
        if ( shared_lexer isnt NULL ) {
                free( shared_lexer );
                log(INFO, "Freed the shared lexer.");
        }
}

int ardp_lexer_state()
{
        if ( shared_lexer is NULL ) {
                return ARDP_LEXER_TURTLE_STATUS_UNKNOWN;
        } else {
                __block int state;
                state = shared_lexer->state;
                return state;
        }
}
/*}}}*/
/* ardp_lexer_turtle_is_ready() {{{ */
bool ardp_lexer_is_ready()
{
        return ( ardp_lexer_state() is ARDP_LEXER_TURTLE_STATUS_READY );
}
/*}}}*/

/* ardp_lexer_turtle_process_block() {{{ */
int ardp_lexer_process_block( uint8_t *_Nullable v,
                                     size_t             len,
                                     uint8_t *_Nullable mark,
                                     bool               is_eof)
{
    if ( not ardp_lexer_is_ready() ) {
        log (ERROR, "Shared lexer is note yet ready to process the data.");
        return ARDP_FAILURE;
    }

    __block uint8_t *_Nullable p  = v; /* Indirection to allow the GCD */
    const   uint8_t *_Nullable pe = p + len; /* Points to fist byte beyond the data */
    const   uint8_t *_Nullable eof= (is_eof) ? pe : NULL; /* Indicated end of all data*/

    /* clang-format off */
    
#line 237 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 1 "NONE"
	{	switch(  shared_lexer->env.act ) {
	case 1:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(PREFIX); }
	break;
	case 2:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(SPARQL_PREFIX); }
	break;
	case 3:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(BASE); }
	break;
	case 4:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(SPARQL_BASE); }
	break;
	case 7:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 8:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 10:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 11:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }
	break;
	case 12:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }
	break;
	case 16:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(LANGTAG,var(ts), var(te) - var(ts));
        }
	break;
	case 17:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(A); }
	break;
	case 27:
	{{p = (( shared_lexer->env.te))-1;}
              dispatch_async(shared_lexer->event_queue, ^{
                shared_lexer->line++;
              });
        }
	break;
	default:
	{{p = (( shared_lexer->env.te))-1;}}
	break;
	}
	}
	goto st162;
tr12:
#line 66 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr20:
#line 72 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st162;
tr37:
#line 107 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st162;
tr39:
#line 104 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st162;
tr51:
#line 69 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr59:
#line 75 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st162;
tr79:
#line 60 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr81:
#line 51 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(QNAME_LITERAL, var(ts), var(te) - var(ts));
        }}
	goto st162;
tr113:
#line 55 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              // '<' URI_LITERAL '>'
              lexer_emit_token(URI_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr173:
#line 107 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st162;
tr180:
#line 93 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st162;
tr181:
#line 94 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st162;
tr183:
#line 88 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st162;
tr186:
#line 89 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st162;
tr192:
#line 91 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st162;
tr193:
#line 92 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st162;
tr198:
#line 95 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st162;
tr199:
#line 96 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st162;
tr214:
#line 98 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                shared_lexer->line++;
              });
        }}
	goto st162;
tr215:
#line 107 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st162;
tr217:
#line 66 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr221:
#line 104 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st162;
tr223:
#line 69 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr227:
#line 61 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr228:
#line 62 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DOUBLE_LITERAL,  var(ts), var(te) - var(ts)); }}
	goto st162;
tr229:
#line 60 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr231:
#line 87 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st162;
tr232:
#line 51 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(QNAME_LITERAL, var(ts), var(te) - var(ts));
        }}
	goto st162;
tr239:
#line 82 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(LANGTAG,var(ts), var(te) - var(ts));
        }}
	goto st162;
tr250:
#line 79 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st162;
st162:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof162;
case 162:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 453 "lexer2.c"
	switch( (*p) ) {
		case 9u: goto tr174;
		case 10u: goto st164;
		case 13u: goto tr176;
		case 32u: goto tr174;
		case 34u: goto tr177;
		case 35u: goto tr178;
		case 39u: goto tr179;
		case 40u: goto tr180;
		case 41u: goto tr181;
		case 44u: goto tr183;
		case 46u: goto st183;
		case 58u: goto tr140;
		case 59u: goto tr186;
		case 60u: goto tr187;
		case 64u: goto st187;
		case 66u: goto tr190;
		case 80u: goto tr191;
		case 91u: goto tr192;
		case 93u: goto tr193;
		case 94u: goto st201;
		case 97u: goto tr195;
		case 98u: goto tr190;
		case 102u: goto tr196;
		case 112u: goto tr191;
		case 116u: goto tr197;
		case 123u: goto tr198;
		case 125u: goto tr199;
		case 195u: goto tr200;
		case 203u: goto tr202;
		case 205u: goto tr203;
		case 223u: goto tr202;
		case 224u: goto tr204;
		case 225u: goto tr205;
		case 226u: goto tr206;
		case 227u: goto tr207;
		case 237u: goto tr209;
		case 239u: goto tr210;
		case 240u: goto tr211;
		case 243u: goto tr213;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr182;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr189;
		} else
			goto tr185;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr201;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr212;
			} else if ( (*p) >= 228u )
				goto tr208;
		} else
			goto tr201;
	} else
		goto tr189;
	goto tr173;
tr174:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 105 "turtle.rl"
	{ shared_lexer->env.act = 29;}
	goto st163;
tr176:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 98 "turtle.rl"
	{ shared_lexer->env.act = 27;}
	goto st163;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
#line 535 "lexer2.c"
	switch( (*p) ) {
		case 13u: goto tr174;
		case 32u: goto tr174;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr174;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 13u: goto tr176;
		case 32u: goto tr174;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr174;
	goto tr214;
tr177:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st165;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
#line 564 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
		case 34u: goto tr216;
		case 39u: goto tr215;
		case 92u: goto st1;
	}
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto st1;
	}
	goto st0;
tr2:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 66 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st166;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
#line 595 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr217;
		case 13u: goto tr217;
		case 34u: goto tr2;
		case 39u: goto tr217;
		case 92u: goto st1;
	}
	goto st0;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 34u: goto st0;
		case 39u: goto st0;
		case 85u: goto st2;
		case 92u: goto st0;
		case 98u: goto st0;
		case 102u: goto st0;
		case 110u: goto st0;
		case 114u: goto st0;
		case 116u: goto st0;
		case 117u: goto st6;
	}
	goto tr0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st3;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st3;
	} else
		goto st3;
	goto tr0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st4;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st4;
	} else
		goto st4;
	goto tr0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st5;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st5;
	} else
		goto st5;
	goto tr0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st6;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st6;
	} else
		goto st6;
	goto tr0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st7;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st7;
	} else
		goto st7;
	goto tr0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st8;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st8;
	} else
		goto st8;
	goto tr0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st9;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st9;
	} else
		goto st9;
	goto tr0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st0;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st0;
	} else
		goto st0;
	goto tr0;
tr216:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 66 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st167;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
#line 735 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr217;
		case 13u: goto tr217;
		case 34u: goto tr218;
		case 39u: goto tr217;
		case 92u: goto st1;
	}
	goto st0;
tr218:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st168;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
#line 752 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr15;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr15;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 34u: goto st12;
		case 92u: goto st14;
	}
	goto st11;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 34u: goto st13;
		case 92u: goto st14;
	}
	goto st11;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 34u: goto tr20;
		case 92u: goto st14;
	}
	goto st11;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 34u: goto st11;
		case 39u: goto st11;
		case 85u: goto st15;
		case 92u: goto st11;
		case 98u: goto st11;
		case 102u: goto st11;
		case 110u: goto st11;
		case 114u: goto st11;
		case 116u: goto st11;
		case 117u: goto st19;
	}
	goto tr12;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st16;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st16;
	} else
		goto st16;
	goto tr12;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st17;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st17;
	} else
		goto st17;
	goto tr12;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st18;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st18;
	} else
		goto st18;
	goto tr12;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st19;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st19;
	} else
		goto st19;
	goto tr12;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st20;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st20;
	} else
		goto st20;
	goto tr12;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st21;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st21;
	} else
		goto st21;
	goto tr12;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st22;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st22;
	} else
		goto st22;
	goto tr12;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st11;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st11;
	} else
		goto st11;
	goto tr12;
tr15:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st169;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 929 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr219;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
tr219:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st170;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
#line 946 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr2;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 34u: goto st10;
		case 39u: goto st10;
		case 85u: goto st24;
		case 92u: goto st10;
		case 98u: goto st10;
		case 102u: goto st10;
		case 110u: goto st10;
		case 114u: goto st10;
		case 116u: goto st10;
		case 117u: goto st28;
	}
	goto tr12;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st25;
	} else
		goto st25;
	goto tr12;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st26;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st26;
	} else
		goto st26;
	goto tr12;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st27;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st27;
	} else
		goto st27;
	goto tr12;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st28;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st28;
	} else
		goto st28;
	goto tr12;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st29;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st29;
	} else
		goto st29;
	goto tr12;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st30;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st30;
	} else
		goto st30;
	goto tr12;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st31;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st31;
	} else
		goto st31;
	goto tr12;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st10;
	} else
		goto st10;
	goto tr12;
tr178:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 1084 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
	}
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 10u: goto st172;
		case 13u: goto tr39;
	}
	goto tr37;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	if ( (*p) == 13u )
		goto tr39;
	goto tr221;
tr179:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 1116 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
		case 34u: goto tr215;
		case 39u: goto tr222;
		case 92u: goto st34;
	}
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr41;
		case 92u: goto st34;
	}
	goto st33;
tr41:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 69 "turtle.rl"
	{ shared_lexer->env.act = 12;}
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 1147 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr223;
		case 13u: goto tr223;
		case 34u: goto tr223;
		case 39u: goto tr41;
		case 92u: goto st34;
	}
	goto st33;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 34u: goto st33;
		case 39u: goto st33;
		case 85u: goto st35;
		case 92u: goto st33;
		case 98u: goto st33;
		case 102u: goto st33;
		case 110u: goto st33;
		case 114u: goto st33;
		case 116u: goto st33;
		case 117u: goto st39;
	}
	goto tr0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st36;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st36;
	} else
		goto st36;
	goto tr0;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st37;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st37;
	} else
		goto st37;
	goto tr0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st38;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st38;
	} else
		goto st38;
	goto tr0;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st39;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st39;
	} else
		goto st39;
	goto tr0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st40;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st40;
	} else
		goto st40;
	goto tr0;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st41;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st41;
	} else
		goto st41;
	goto tr0;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st42;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st42;
	} else
		goto st42;
	goto tr0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st33;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st33;
	} else
		goto st33;
	goto tr0;
tr222:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 69 "turtle.rl"
	{ shared_lexer->env.act = 12;}
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 1287 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr223;
		case 13u: goto tr223;
		case 34u: goto tr223;
		case 39u: goto tr224;
		case 92u: goto st34;
	}
	goto st33;
tr224:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 1304 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr54;
		case 92u: goto st56;
	}
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr54;
		case 92u: goto st56;
	}
	goto st43;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	switch( (*p) ) {
		case 39u: goto st45;
		case 92u: goto st47;
	}
	goto st44;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 39u: goto st46;
		case 92u: goto st47;
	}
	goto st44;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 39u: goto tr59;
		case 92u: goto st47;
	}
	goto st44;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 34u: goto st44;
		case 39u: goto st44;
		case 85u: goto st48;
		case 92u: goto st44;
		case 98u: goto st44;
		case 102u: goto st44;
		case 110u: goto st44;
		case 114u: goto st44;
		case 116u: goto st44;
		case 117u: goto st52;
	}
	goto tr51;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st49;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st49;
	} else
		goto st49;
	goto tr51;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st50;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st50;
	} else
		goto st50;
	goto tr51;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st51;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st51;
	} else
		goto st51;
	goto tr51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st52;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st52;
	} else
		goto st52;
	goto tr51;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st53;
	} else
		goto st53;
	goto tr51;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st54;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st54;
	} else
		goto st54;
	goto tr51;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st55;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st55;
	} else
		goto st55;
	goto tr51;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st44;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st44;
	} else
		goto st44;
	goto tr51;
tr54:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 1481 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr225;
		case 92u: goto st56;
	}
	goto st43;
tr225:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 1498 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr41;
		case 92u: goto st56;
	}
	goto st43;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 34u: goto st43;
		case 39u: goto st43;
		case 85u: goto st57;
		case 92u: goto st43;
		case 98u: goto st43;
		case 102u: goto st43;
		case 110u: goto st43;
		case 114u: goto st43;
		case 116u: goto st43;
		case 117u: goto st61;
	}
	goto tr51;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st58;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st58;
	} else
		goto st58;
	goto tr51;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st59;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st59;
	} else
		goto st59;
	goto tr51;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st60;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st60;
	} else
		goto st60;
	goto tr51;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st61;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st61;
	} else
		goto st61;
	goto tr51;
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
	goto tr51;
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
	goto tr51;
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
	goto tr51;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st43;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st43;
	} else
		goto st43;
	goto tr51;
tr182:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st179;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
#line 1636 "lexer2.c"
	if ( (*p) == 46u )
		goto st65;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr185;
	goto tr215;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr37;
tr76:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 61 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 1659 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr227;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 43u: goto st67;
		case 45u: goto st67;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr0;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr228;
tr185:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 60 "turtle.rl"
	{ shared_lexer->env.act = 7;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 1702 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st68;
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr185;
	goto tr229;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr79;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr231;
tr140:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 1737 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st69;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 205u: goto st96;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st97;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr83;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr83;
		} else
			goto tr83;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto st74;
	goto tr232;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st70;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st70;
	} else
		goto st70;
	goto tr81;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr83;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr83;
	} else
		goto tr83;
	goto tr81;
tr83:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 1810 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st69;
		case 45u: goto tr83;
		case 46u: goto st71;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 183u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 204u: goto st76;
		case 205u: goto st77;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st80;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr83;
		} else if ( (*p) >= 48u )
			goto tr83;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto tr83;
	goto tr232;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 37u: goto st69;
		case 45u: goto tr83;
		case 46u: goto st71;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 183u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 204u: goto st76;
		case 205u: goto st77;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st80;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr83;
		} else if ( (*p) >= 48u )
			goto tr83;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto tr83;
	goto tr81;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 33u: goto tr83;
		case 59u: goto tr83;
		case 61u: goto tr83;
		case 95u: goto tr83;
		case 126u: goto tr83;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr83;
	} else if ( (*p) >= 35u )
		goto tr83;
	goto tr81;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr83;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr83;
	} else
		goto tr83;
	goto tr81;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	goto tr83;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( 192u <= (*p) )
		goto tr81;
	goto tr83;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( 128u <= (*p) )
		goto tr83;
	goto tr81;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( (*p) == 190u )
		goto tr81;
	goto tr83;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) == 160u )
		goto st76;
	if ( 161u <= (*p) )
		goto st74;
	goto tr81;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	if ( (*p) == 191u )
		goto st75;
	if ( 192u <= (*p) )
		goto tr81;
	goto st74;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 128u: goto st81;
		case 129u: goto st82;
		case 134u: goto st83;
		case 176u: goto st76;
		case 191u: goto st84;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st74;
	} else if ( (*p) >= 130u )
		goto st74;
	goto tr81;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr83;
	} else if ( (*p) >= 140u )
		goto tr83;
	goto tr81;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr81;
	goto tr83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( 144u <= (*p) )
		goto tr81;
	goto tr83;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	if ( 176u <= (*p) )
		goto tr81;
	goto tr83;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 128u )
		goto st86;
	if ( 129u <= (*p) )
		goto st74;
	goto tr81;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( 129u <= (*p) )
		goto tr83;
	goto tr81;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	goto st74;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 159u )
		goto st75;
	if ( 160u <= (*p) )
		goto tr81;
	goto st74;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	switch( (*p) ) {
		case 164u: goto st76;
		case 183u: goto st90;
		case 191u: goto st91;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st74;
	goto tr81;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr81;
	goto tr83;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( 190u <= (*p) )
		goto tr81;
	goto tr83;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) == 144u )
		goto st93;
	if ( 145u <= (*p) )
		goto st87;
	goto tr81;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( (*p) == 128u )
		goto st76;
	if ( 129u <= (*p) )
		goto st74;
	goto tr81;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	goto st87;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 175u )
		goto st79;
	if ( 176u <= (*p) )
		goto tr81;
	goto st87;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr83;
	} else if ( (*p) >= 176u )
		goto tr83;
	goto tr81;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	switch( (*p) ) {
		case 128u: goto st98;
		case 129u: goto st99;
		case 134u: goto st83;
		case 176u: goto st76;
		case 191u: goto st84;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st74;
	} else if ( (*p) >= 130u )
		goto st74;
	goto tr81;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr83;
	goto tr81;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( 176u <= (*p) )
		goto tr83;
	goto tr81;
tr187:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 2152 "lexer2.c"
	switch( (*p) ) {
		case 35u: goto st100;
		case 62u: goto tr113;
		case 92u: goto st100;
		case 95u: goto st100;
		case 183u: goto st100;
		case 195u: goto st101;
		case 203u: goto st103;
		case 204u: goto st104;
		case 205u: goto st105;
		case 223u: goto st103;
		case 224u: goto st106;
		case 225u: goto st107;
		case 226u: goto st108;
		case 227u: goto st113;
		case 237u: goto st116;
		case 239u: goto st117;
		case 240u: goto st120;
		case 243u: goto st123;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st100;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st100;
		} else
			goto st100;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st102;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st122;
		} else
			goto st115;
	} else
		goto st100;
	goto tr215;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	switch( (*p) ) {
		case 35u: goto st100;
		case 62u: goto tr113;
		case 92u: goto st100;
		case 95u: goto st100;
		case 183u: goto st100;
		case 195u: goto st101;
		case 203u: goto st103;
		case 204u: goto st104;
		case 205u: goto st105;
		case 223u: goto st103;
		case 224u: goto st106;
		case 225u: goto st107;
		case 226u: goto st108;
		case 227u: goto st113;
		case 237u: goto st116;
		case 239u: goto st117;
		case 240u: goto st120;
		case 243u: goto st123;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st100;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st100;
		} else
			goto st100;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st102;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st122;
		} else
			goto st115;
	} else
		goto st100;
	goto tr37;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st100;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st100;
	} else
		goto st100;
	goto tr37;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	goto st100;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( 192u <= (*p) )
		goto tr37;
	goto st100;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	if ( 128u <= (*p) )
		goto st100;
	goto tr37;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) == 190u )
		goto tr37;
	goto st100;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	if ( (*p) == 160u )
		goto st104;
	if ( 161u <= (*p) )
		goto st102;
	goto tr37;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	if ( (*p) == 191u )
		goto st103;
	if ( 192u <= (*p) )
		goto tr37;
	goto st102;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	switch( (*p) ) {
		case 128u: goto st109;
		case 129u: goto st110;
		case 134u: goto st111;
		case 176u: goto st104;
		case 191u: goto st112;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st102;
	} else if ( (*p) >= 130u )
		goto st102;
	goto tr37;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st100;
	} else if ( (*p) >= 140u )
		goto st100;
	goto tr37;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr37;
	goto st100;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( 144u <= (*p) )
		goto tr37;
	goto st100;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( 176u <= (*p) )
		goto tr37;
	goto st100;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 128u )
		goto st114;
	if ( 129u <= (*p) )
		goto st102;
	goto tr37;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( 129u <= (*p) )
		goto st100;
	goto tr37;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	goto st102;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( (*p) == 159u )
		goto st103;
	if ( 160u <= (*p) )
		goto tr37;
	goto st102;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	switch( (*p) ) {
		case 164u: goto st104;
		case 183u: goto st118;
		case 191u: goto st119;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st102;
	goto tr37;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr37;
	goto st100;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	if ( 190u <= (*p) )
		goto tr37;
	goto st100;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( (*p) == 144u )
		goto st121;
	if ( 145u <= (*p) )
		goto st115;
	goto tr37;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	if ( (*p) == 128u )
		goto st104;
	if ( 129u <= (*p) )
		goto st102;
	goto tr37;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	goto st115;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	if ( (*p) == 175u )
		goto st107;
	if ( 176u <= (*p) )
		goto tr37;
	goto st115;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
	switch( (*p) ) {
		case 98u: goto tr236;
		case 112u: goto tr237;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr215;
tr235:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st188;
tr242:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 47 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st188;
tr247:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 45 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 2468 "lexer2.c"
	if ( (*p) == 45u )
		goto st124;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr0;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr137;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr137;
	} else
		goto tr137;
	goto tr0;
tr137:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 2500 "lexer2.c"
	if ( (*p) == 45u )
		goto st124;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr137;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr137;
	} else
		goto tr137;
	goto tr239;
tr236:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 2522 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 97u: goto tr240;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr240:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 2543 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 115u: goto tr241;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr241:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 2564 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 101u: goto tr242;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr237:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 2585 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 114u: goto tr243;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr243:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 2606 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 101u: goto tr244;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr244:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 2627 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 102u: goto tr245;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr245:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 2648 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 105u: goto tr246;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr246:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 2669 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 120u: goto tr247;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr165:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 48 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st198;
tr169:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 46 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st198;
tr172:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 63 "turtle.rl"
	{ shared_lexer->env.act = 10;}
	goto st198;
tr189:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st198;
tr195:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 86 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 2714 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st125;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st125;
	} else
		goto st125;
	goto tr0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	goto st125;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( 192u <= (*p) )
		goto tr0;
	goto st125;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	if ( 128u <= (*p) )
		goto st125;
	goto tr0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) == 190u )
		goto tr0;
	goto st125;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	if ( (*p) == 160u )
		goto st130;
	if ( 161u <= (*p) )
		goto st128;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	if ( (*p) == 191u )
		goto st129;
	if ( 192u <= (*p) )
		goto tr0;
	goto st128;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 128u: goto st135;
		case 129u: goto st136;
		case 134u: goto st137;
		case 176u: goto st130;
		case 191u: goto st138;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st128;
	} else if ( (*p) >= 130u )
		goto st128;
	goto tr0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st125;
	} else if ( (*p) >= 140u )
		goto st125;
	goto tr0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st125;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	if ( 144u <= (*p) )
		goto tr0;
	goto st125;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( 176u <= (*p) )
		goto tr0;
	goto st125;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( (*p) == 128u )
		goto st140;
	if ( 129u <= (*p) )
		goto st128;
	goto tr0;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( 129u <= (*p) )
		goto st125;
	goto tr0;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	goto st128;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 159u )
		goto st129;
	if ( 160u <= (*p) )
		goto tr0;
	goto st128;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	switch( (*p) ) {
		case 164u: goto st130;
		case 183u: goto st144;
		case 191u: goto st145;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st128;
	goto tr0;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st125;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( 190u <= (*p) )
		goto tr0;
	goto st125;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( (*p) == 144u )
		goto st147;
	if ( 145u <= (*p) )
		goto st141;
	goto tr0;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	if ( (*p) == 128u )
		goto st130;
	if ( 129u <= (*p) )
		goto st128;
	goto tr0;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	goto st141;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( (*p) == 175u )
		goto st133;
	if ( 176u <= (*p) )
		goto tr0;
	goto st141;
tr190:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 3039 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 65u: goto st150;
		case 95u: goto st125;
		case 97u: goto st150;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 83u: goto st151;
		case 95u: goto st125;
		case 115u: goto st151;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 69u: goto tr165;
		case 95u: goto st125;
		case 101u: goto tr165;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr191:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 3178 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 82u: goto st152;
		case 95u: goto st125;
		case 114u: goto st152;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 69u: goto st153;
		case 95u: goto st125;
		case 101u: goto st153;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 70u: goto st154;
		case 95u: goto st125;
		case 102u: goto st154;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 73u: goto st155;
		case 95u: goto st125;
		case 105u: goto st155;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 88u: goto tr169;
		case 95u: goto st125;
		case 120u: goto tr169;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
	if ( (*p) == 94u )
		goto tr250;
	goto tr215;
tr196:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 3412 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 97u: goto st156;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 108u: goto st157;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 115u: goto st158;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 101u: goto tr172;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr197:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 3591 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 114u: goto st159;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 117u: goto st158;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr200:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st204;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
#line 3684 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st125;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st125;
	} else
		goto st125;
	goto tr215;
tr201:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 3704 "lexer2.c"
	goto st125;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 3716 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr215;
	goto st125;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 3730 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st125;
	} else if ( (*p) >= 176u )
		goto st125;
	goto tr215;
tr204:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 3747 "lexer2.c"
	if ( (*p) == 160u )
		goto st130;
	if ( 161u <= (*p) )
		goto st128;
	goto tr215;
tr205:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 3763 "lexer2.c"
	if ( (*p) == 191u )
		goto st129;
	if ( 192u <= (*p) )
		goto tr215;
	goto st128;
tr206:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 3779 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st160;
		case 129u: goto st161;
		case 134u: goto st137;
		case 176u: goto st130;
		case 191u: goto st138;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st128;
	} else if ( (*p) >= 130u )
		goto st128;
	goto tr215;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st125;
	goto tr37;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	if ( 176u <= (*p) )
		goto st125;
	goto tr37;
tr207:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 3817 "lexer2.c"
	if ( (*p) == 128u )
		goto st140;
	if ( 129u <= (*p) )
		goto st128;
	goto tr215;
tr208:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 3833 "lexer2.c"
	goto st128;
tr209:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 3845 "lexer2.c"
	if ( (*p) == 159u )
		goto st129;
	if ( 160u <= (*p) )
		goto tr215;
	goto st128;
tr210:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 3861 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st130;
		case 183u: goto st144;
		case 191u: goto st145;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st128;
	goto tr215;
tr211:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 3880 "lexer2.c"
	if ( (*p) == 144u )
		goto st147;
	if ( 145u <= (*p) )
		goto st141;
	goto tr215;
tr212:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 3896 "lexer2.c"
	goto st141;
tr213:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 3908 "lexer2.c"
	if ( (*p) == 175u )
		goto st133;
	if ( 176u <= (*p) )
		goto tr215;
	goto st141;
	}
	_test_eof162:  shared_lexer->env.cs = 162; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof0:  shared_lexer->env.cs = 0; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof1:  shared_lexer->env.cs = 1; goto _test_eof; 
	_test_eof2:  shared_lexer->env.cs = 2; goto _test_eof; 
	_test_eof3:  shared_lexer->env.cs = 3; goto _test_eof; 
	_test_eof4:  shared_lexer->env.cs = 4; goto _test_eof; 
	_test_eof5:  shared_lexer->env.cs = 5; goto _test_eof; 
	_test_eof6:  shared_lexer->env.cs = 6; goto _test_eof; 
	_test_eof7:  shared_lexer->env.cs = 7; goto _test_eof; 
	_test_eof8:  shared_lexer->env.cs = 8; goto _test_eof; 
	_test_eof9:  shared_lexer->env.cs = 9; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof10:  shared_lexer->env.cs = 10; goto _test_eof; 
	_test_eof11:  shared_lexer->env.cs = 11; goto _test_eof; 
	_test_eof12:  shared_lexer->env.cs = 12; goto _test_eof; 
	_test_eof13:  shared_lexer->env.cs = 13; goto _test_eof; 
	_test_eof14:  shared_lexer->env.cs = 14; goto _test_eof; 
	_test_eof15:  shared_lexer->env.cs = 15; goto _test_eof; 
	_test_eof16:  shared_lexer->env.cs = 16; goto _test_eof; 
	_test_eof17:  shared_lexer->env.cs = 17; goto _test_eof; 
	_test_eof18:  shared_lexer->env.cs = 18; goto _test_eof; 
	_test_eof19:  shared_lexer->env.cs = 19; goto _test_eof; 
	_test_eof20:  shared_lexer->env.cs = 20; goto _test_eof; 
	_test_eof21:  shared_lexer->env.cs = 21; goto _test_eof; 
	_test_eof22:  shared_lexer->env.cs = 22; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof43:  shared_lexer->env.cs = 43; goto _test_eof; 
	_test_eof44:  shared_lexer->env.cs = 44; goto _test_eof; 
	_test_eof45:  shared_lexer->env.cs = 45; goto _test_eof; 
	_test_eof46:  shared_lexer->env.cs = 46; goto _test_eof; 
	_test_eof47:  shared_lexer->env.cs = 47; goto _test_eof; 
	_test_eof48:  shared_lexer->env.cs = 48; goto _test_eof; 
	_test_eof49:  shared_lexer->env.cs = 49; goto _test_eof; 
	_test_eof50:  shared_lexer->env.cs = 50; goto _test_eof; 
	_test_eof51:  shared_lexer->env.cs = 51; goto _test_eof; 
	_test_eof52:  shared_lexer->env.cs = 52; goto _test_eof; 
	_test_eof53:  shared_lexer->env.cs = 53; goto _test_eof; 
	_test_eof54:  shared_lexer->env.cs = 54; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof61:  shared_lexer->env.cs = 61; goto _test_eof; 
	_test_eof62:  shared_lexer->env.cs = 62; goto _test_eof; 
	_test_eof63:  shared_lexer->env.cs = 63; goto _test_eof; 
	_test_eof64:  shared_lexer->env.cs = 64; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof65:  shared_lexer->env.cs = 65; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof66:  shared_lexer->env.cs = 66; goto _test_eof; 
	_test_eof67:  shared_lexer->env.cs = 67; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof68:  shared_lexer->env.cs = 68; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof69:  shared_lexer->env.cs = 69; goto _test_eof; 
	_test_eof70:  shared_lexer->env.cs = 70; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof71:  shared_lexer->env.cs = 71; goto _test_eof; 
	_test_eof72:  shared_lexer->env.cs = 72; goto _test_eof; 
	_test_eof73:  shared_lexer->env.cs = 73; goto _test_eof; 
	_test_eof74:  shared_lexer->env.cs = 74; goto _test_eof; 
	_test_eof75:  shared_lexer->env.cs = 75; goto _test_eof; 
	_test_eof76:  shared_lexer->env.cs = 76; goto _test_eof; 
	_test_eof77:  shared_lexer->env.cs = 77; goto _test_eof; 
	_test_eof78:  shared_lexer->env.cs = 78; goto _test_eof; 
	_test_eof79:  shared_lexer->env.cs = 79; goto _test_eof; 
	_test_eof80:  shared_lexer->env.cs = 80; goto _test_eof; 
	_test_eof81:  shared_lexer->env.cs = 81; goto _test_eof; 
	_test_eof82:  shared_lexer->env.cs = 82; goto _test_eof; 
	_test_eof83:  shared_lexer->env.cs = 83; goto _test_eof; 
	_test_eof84:  shared_lexer->env.cs = 84; goto _test_eof; 
	_test_eof85:  shared_lexer->env.cs = 85; goto _test_eof; 
	_test_eof86:  shared_lexer->env.cs = 86; goto _test_eof; 
	_test_eof87:  shared_lexer->env.cs = 87; goto _test_eof; 
	_test_eof88:  shared_lexer->env.cs = 88; goto _test_eof; 
	_test_eof89:  shared_lexer->env.cs = 89; goto _test_eof; 
	_test_eof90:  shared_lexer->env.cs = 90; goto _test_eof; 
	_test_eof91:  shared_lexer->env.cs = 91; goto _test_eof; 
	_test_eof92:  shared_lexer->env.cs = 92; goto _test_eof; 
	_test_eof93:  shared_lexer->env.cs = 93; goto _test_eof; 
	_test_eof94:  shared_lexer->env.cs = 94; goto _test_eof; 
	_test_eof95:  shared_lexer->env.cs = 95; goto _test_eof; 
	_test_eof96:  shared_lexer->env.cs = 96; goto _test_eof; 
	_test_eof97:  shared_lexer->env.cs = 97; goto _test_eof; 
	_test_eof98:  shared_lexer->env.cs = 98; goto _test_eof; 
	_test_eof99:  shared_lexer->env.cs = 99; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof100:  shared_lexer->env.cs = 100; goto _test_eof; 
	_test_eof101:  shared_lexer->env.cs = 101; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof103:  shared_lexer->env.cs = 103; goto _test_eof; 
	_test_eof104:  shared_lexer->env.cs = 104; goto _test_eof; 
	_test_eof105:  shared_lexer->env.cs = 105; goto _test_eof; 
	_test_eof106:  shared_lexer->env.cs = 106; goto _test_eof; 
	_test_eof107:  shared_lexer->env.cs = 107; goto _test_eof; 
	_test_eof108:  shared_lexer->env.cs = 108; goto _test_eof; 
	_test_eof109:  shared_lexer->env.cs = 109; goto _test_eof; 
	_test_eof110:  shared_lexer->env.cs = 110; goto _test_eof; 
	_test_eof111:  shared_lexer->env.cs = 111; goto _test_eof; 
	_test_eof112:  shared_lexer->env.cs = 112; goto _test_eof; 
	_test_eof113:  shared_lexer->env.cs = 113; goto _test_eof; 
	_test_eof114:  shared_lexer->env.cs = 114; goto _test_eof; 
	_test_eof115:  shared_lexer->env.cs = 115; goto _test_eof; 
	_test_eof116:  shared_lexer->env.cs = 116; goto _test_eof; 
	_test_eof117:  shared_lexer->env.cs = 117; goto _test_eof; 
	_test_eof118:  shared_lexer->env.cs = 118; goto _test_eof; 
	_test_eof119:  shared_lexer->env.cs = 119; goto _test_eof; 
	_test_eof120:  shared_lexer->env.cs = 120; goto _test_eof; 
	_test_eof121:  shared_lexer->env.cs = 121; goto _test_eof; 
	_test_eof122:  shared_lexer->env.cs = 122; goto _test_eof; 
	_test_eof123:  shared_lexer->env.cs = 123; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof135:  shared_lexer->env.cs = 135; goto _test_eof; 
	_test_eof136:  shared_lexer->env.cs = 136; goto _test_eof; 
	_test_eof137:  shared_lexer->env.cs = 137; goto _test_eof; 
	_test_eof138:  shared_lexer->env.cs = 138; goto _test_eof; 
	_test_eof139:  shared_lexer->env.cs = 139; goto _test_eof; 
	_test_eof140:  shared_lexer->env.cs = 140; goto _test_eof; 
	_test_eof141:  shared_lexer->env.cs = 141; goto _test_eof; 
	_test_eof142:  shared_lexer->env.cs = 142; goto _test_eof; 
	_test_eof143:  shared_lexer->env.cs = 143; goto _test_eof; 
	_test_eof144:  shared_lexer->env.cs = 144; goto _test_eof; 
	_test_eof145:  shared_lexer->env.cs = 145; goto _test_eof; 
	_test_eof146:  shared_lexer->env.cs = 146; goto _test_eof; 
	_test_eof147:  shared_lexer->env.cs = 147; goto _test_eof; 
	_test_eof148:  shared_lexer->env.cs = 148; goto _test_eof; 
	_test_eof149:  shared_lexer->env.cs = 149; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof150:  shared_lexer->env.cs = 150; goto _test_eof; 
	_test_eof151:  shared_lexer->env.cs = 151; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof152:  shared_lexer->env.cs = 152; goto _test_eof; 
	_test_eof153:  shared_lexer->env.cs = 153; goto _test_eof; 
	_test_eof154:  shared_lexer->env.cs = 154; goto _test_eof; 
	_test_eof155:  shared_lexer->env.cs = 155; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof156:  shared_lexer->env.cs = 156; goto _test_eof; 
	_test_eof157:  shared_lexer->env.cs = 157; goto _test_eof; 
	_test_eof158:  shared_lexer->env.cs = 158; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof159:  shared_lexer->env.cs = 159; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof160:  shared_lexer->env.cs = 160; goto _test_eof; 
	_test_eof161:  shared_lexer->env.cs = 161; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 163: goto tr0;
	case 164: goto tr214;
	case 165: goto tr215;
	case 0: goto tr0;
	case 166: goto tr217;
	case 1: goto tr0;
	case 2: goto tr0;
	case 3: goto tr0;
	case 4: goto tr0;
	case 5: goto tr0;
	case 6: goto tr0;
	case 7: goto tr0;
	case 8: goto tr0;
	case 9: goto tr0;
	case 167: goto tr217;
	case 168: goto tr217;
	case 10: goto tr12;
	case 11: goto tr12;
	case 12: goto tr12;
	case 13: goto tr12;
	case 14: goto tr12;
	case 15: goto tr12;
	case 16: goto tr12;
	case 17: goto tr12;
	case 18: goto tr12;
	case 19: goto tr12;
	case 20: goto tr12;
	case 21: goto tr12;
	case 22: goto tr12;
	case 169: goto tr217;
	case 170: goto tr217;
	case 23: goto tr12;
	case 24: goto tr12;
	case 25: goto tr12;
	case 26: goto tr12;
	case 27: goto tr12;
	case 28: goto tr12;
	case 29: goto tr12;
	case 30: goto tr12;
	case 31: goto tr12;
	case 171: goto tr215;
	case 32: goto tr37;
	case 172: goto tr221;
	case 173: goto tr215;
	case 33: goto tr0;
	case 174: goto tr223;
	case 34: goto tr0;
	case 35: goto tr0;
	case 36: goto tr0;
	case 37: goto tr0;
	case 38: goto tr0;
	case 39: goto tr0;
	case 40: goto tr0;
	case 41: goto tr0;
	case 42: goto tr0;
	case 175: goto tr223;
	case 176: goto tr223;
	case 43: goto tr51;
	case 44: goto tr51;
	case 45: goto tr51;
	case 46: goto tr51;
	case 47: goto tr51;
	case 48: goto tr51;
	case 49: goto tr51;
	case 50: goto tr51;
	case 51: goto tr51;
	case 52: goto tr51;
	case 53: goto tr51;
	case 54: goto tr51;
	case 55: goto tr51;
	case 177: goto tr223;
	case 178: goto tr223;
	case 56: goto tr51;
	case 57: goto tr51;
	case 58: goto tr51;
	case 59: goto tr51;
	case 60: goto tr51;
	case 61: goto tr51;
	case 62: goto tr51;
	case 63: goto tr51;
	case 64: goto tr51;
	case 179: goto tr215;
	case 65: goto tr37;
	case 180: goto tr227;
	case 66: goto tr0;
	case 67: goto tr0;
	case 181: goto tr228;
	case 182: goto tr229;
	case 68: goto tr79;
	case 183: goto tr231;
	case 184: goto tr232;
	case 69: goto tr81;
	case 70: goto tr81;
	case 185: goto tr232;
	case 71: goto tr81;
	case 72: goto tr81;
	case 73: goto tr81;
	case 74: goto tr81;
	case 75: goto tr81;
	case 76: goto tr81;
	case 77: goto tr81;
	case 78: goto tr81;
	case 79: goto tr81;
	case 80: goto tr81;
	case 81: goto tr81;
	case 82: goto tr81;
	case 83: goto tr81;
	case 84: goto tr81;
	case 85: goto tr81;
	case 86: goto tr81;
	case 87: goto tr81;
	case 88: goto tr81;
	case 89: goto tr81;
	case 90: goto tr81;
	case 91: goto tr81;
	case 92: goto tr81;
	case 93: goto tr81;
	case 94: goto tr81;
	case 95: goto tr81;
	case 96: goto tr81;
	case 97: goto tr81;
	case 98: goto tr81;
	case 99: goto tr81;
	case 186: goto tr215;
	case 100: goto tr37;
	case 101: goto tr37;
	case 102: goto tr37;
	case 103: goto tr37;
	case 104: goto tr37;
	case 105: goto tr37;
	case 106: goto tr37;
	case 107: goto tr37;
	case 108: goto tr37;
	case 109: goto tr37;
	case 110: goto tr37;
	case 111: goto tr37;
	case 112: goto tr37;
	case 113: goto tr37;
	case 114: goto tr37;
	case 115: goto tr37;
	case 116: goto tr37;
	case 117: goto tr37;
	case 118: goto tr37;
	case 119: goto tr37;
	case 120: goto tr37;
	case 121: goto tr37;
	case 122: goto tr37;
	case 123: goto tr37;
	case 187: goto tr215;
	case 188: goto tr0;
	case 124: goto tr0;
	case 189: goto tr239;
	case 190: goto tr239;
	case 191: goto tr239;
	case 192: goto tr239;
	case 193: goto tr239;
	case 194: goto tr239;
	case 195: goto tr239;
	case 196: goto tr239;
	case 197: goto tr239;
	case 198: goto tr0;
	case 125: goto tr0;
	case 126: goto tr0;
	case 127: goto tr0;
	case 128: goto tr0;
	case 129: goto tr0;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 134: goto tr0;
	case 135: goto tr0;
	case 136: goto tr0;
	case 137: goto tr0;
	case 138: goto tr0;
	case 139: goto tr0;
	case 140: goto tr0;
	case 141: goto tr0;
	case 142: goto tr0;
	case 143: goto tr0;
	case 144: goto tr0;
	case 145: goto tr0;
	case 146: goto tr0;
	case 147: goto tr0;
	case 148: goto tr0;
	case 149: goto tr0;
	case 199: goto tr215;
	case 150: goto tr37;
	case 151: goto tr37;
	case 200: goto tr215;
	case 152: goto tr37;
	case 153: goto tr37;
	case 154: goto tr37;
	case 155: goto tr37;
	case 201: goto tr215;
	case 202: goto tr215;
	case 156: goto tr37;
	case 157: goto tr37;
	case 158: goto tr37;
	case 203: goto tr215;
	case 159: goto tr37;
	case 204: goto tr215;
	case 205: goto tr215;
	case 206: goto tr215;
	case 207: goto tr215;
	case 208: goto tr215;
	case 209: goto tr215;
	case 210: goto tr215;
	case 160: goto tr37;
	case 161: goto tr37;
	case 211: goto tr215;
	case 212: goto tr215;
	case 213: goto tr215;
	case 214: goto tr215;
	case 215: goto tr215;
	case 216: goto tr215;
	case 217: goto tr215;
	}
	}

	}

#line 290 "turtle.rl"
    /* clang-format on */

    return ARDP_SUCCESS;
}

void ardp_lexer_turtle_process_block( uint8_t *_Nullable v,
                                      size_t             len,
                                      uint8_t *_Nullable mark,
                                      bool               is_eof,
                                      completation_block handler )
{
        if ( !ardp_lexer_is_ready() ) {
                handler( ARDP_FAILURE );
                return;
        }

        __block uint8_t *_Nullable p = v; // redirection to allow GCD


        const uint8_t *_Nullable pe;
        const uint8_t *_Nullable eof;


        pe  = p + len; /* pe points to 1 byte beyond the end of this block of data */
        eof = ( is_eof ) ? pe : ( ( uint8_t * )0 ); /* Indicates the end of all data, 0 if
                                                       not in this block */

        /* clang-format off */
          
#line 4390 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 1 "NONE"
	{	switch(  shared_lexer->env.act ) {
	case 1:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(PREFIX); }
	break;
	case 2:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(SPARQL_PREFIX); }
	break;
	case 3:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(BASE); }
	break;
	case 4:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(SPARQL_BASE); }
	break;
	case 7:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 8:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 10:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 11:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }
	break;
	case 12:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }
	break;
	case 16:
	{{p = (( shared_lexer->env.te))-1;}
              lexer_emit_token(LANGTAG,var(ts), var(te) - var(ts));
        }
	break;
	case 17:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(A); }
	break;
	case 27:
	{{p = (( shared_lexer->env.te))-1;}
              dispatch_async(shared_lexer->event_queue, ^{
                shared_lexer->line++;
              });
        }
	break;
	default:
	{{p = (( shared_lexer->env.te))-1;}}
	break;
	}
	}
	goto st162;
tr12:
#line 66 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr20:
#line 72 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st162;
tr37:
#line 107 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st162;
tr39:
#line 104 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st162;
tr51:
#line 69 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr59:
#line 75 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st162;
tr79:
#line 60 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr81:
#line 51 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
              lexer_emit_token(QNAME_LITERAL, var(ts), var(te) - var(ts));
        }}
	goto st162;
tr113:
#line 55 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              // '<' URI_LITERAL '>'
              lexer_emit_token(URI_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr173:
#line 107 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st162;
tr180:
#line 93 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st162;
tr181:
#line 94 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st162;
tr183:
#line 88 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st162;
tr186:
#line 89 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st162;
tr192:
#line 91 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st162;
tr193:
#line 92 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st162;
tr198:
#line 95 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st162;
tr199:
#line 96 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st162;
tr214:
#line 98 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                shared_lexer->line++;
              });
        }}
	goto st162;
tr215:
#line 107 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st162;
tr217:
#line 66 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr221:
#line 104 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st162;
tr223:
#line 69 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st162;
tr227:
#line 61 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr228:
#line 62 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DOUBLE_LITERAL,  var(ts), var(te) - var(ts)); }}
	goto st162;
tr229:
#line 60 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st162;
tr231:
#line 87 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st162;
tr232:
#line 51 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(QNAME_LITERAL, var(ts), var(te) - var(ts));
        }}
	goto st162;
tr239:
#line 82 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              lexer_emit_token(LANGTAG,var(ts), var(te) - var(ts));
        }}
	goto st162;
tr250:
#line 79 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st162;
st162:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof162;
case 162:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 4606 "lexer2.c"
	switch( (*p) ) {
		case 9u: goto tr174;
		case 10u: goto st164;
		case 13u: goto tr176;
		case 32u: goto tr174;
		case 34u: goto tr177;
		case 35u: goto tr178;
		case 39u: goto tr179;
		case 40u: goto tr180;
		case 41u: goto tr181;
		case 44u: goto tr183;
		case 46u: goto st183;
		case 58u: goto tr140;
		case 59u: goto tr186;
		case 60u: goto tr187;
		case 64u: goto st187;
		case 66u: goto tr190;
		case 80u: goto tr191;
		case 91u: goto tr192;
		case 93u: goto tr193;
		case 94u: goto st201;
		case 97u: goto tr195;
		case 98u: goto tr190;
		case 102u: goto tr196;
		case 112u: goto tr191;
		case 116u: goto tr197;
		case 123u: goto tr198;
		case 125u: goto tr199;
		case 195u: goto tr200;
		case 203u: goto tr202;
		case 205u: goto tr203;
		case 223u: goto tr202;
		case 224u: goto tr204;
		case 225u: goto tr205;
		case 226u: goto tr206;
		case 227u: goto tr207;
		case 237u: goto tr209;
		case 239u: goto tr210;
		case 240u: goto tr211;
		case 243u: goto tr213;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr182;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr189;
		} else
			goto tr185;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr201;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr212;
			} else if ( (*p) >= 228u )
				goto tr208;
		} else
			goto tr201;
	} else
		goto tr189;
	goto tr173;
tr174:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 105 "turtle.rl"
	{ shared_lexer->env.act = 29;}
	goto st163;
tr176:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 98 "turtle.rl"
	{ shared_lexer->env.act = 27;}
	goto st163;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
#line 4688 "lexer2.c"
	switch( (*p) ) {
		case 13u: goto tr174;
		case 32u: goto tr174;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr174;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 13u: goto tr176;
		case 32u: goto tr174;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr174;
	goto tr214;
tr177:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st165;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
#line 4717 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
		case 34u: goto tr216;
		case 39u: goto tr215;
		case 92u: goto st1;
	}
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto st1;
	}
	goto st0;
tr2:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 66 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st166;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
#line 4748 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr217;
		case 13u: goto tr217;
		case 34u: goto tr2;
		case 39u: goto tr217;
		case 92u: goto st1;
	}
	goto st0;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 34u: goto st0;
		case 39u: goto st0;
		case 85u: goto st2;
		case 92u: goto st0;
		case 98u: goto st0;
		case 102u: goto st0;
		case 110u: goto st0;
		case 114u: goto st0;
		case 116u: goto st0;
		case 117u: goto st6;
	}
	goto tr0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st3;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st3;
	} else
		goto st3;
	goto tr0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st4;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st4;
	} else
		goto st4;
	goto tr0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st5;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st5;
	} else
		goto st5;
	goto tr0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st6;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st6;
	} else
		goto st6;
	goto tr0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st7;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st7;
	} else
		goto st7;
	goto tr0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st8;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st8;
	} else
		goto st8;
	goto tr0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st9;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st9;
	} else
		goto st9;
	goto tr0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st0;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st0;
	} else
		goto st0;
	goto tr0;
tr216:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 66 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st167;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
#line 4888 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr217;
		case 13u: goto tr217;
		case 34u: goto tr218;
		case 39u: goto tr217;
		case 92u: goto st1;
	}
	goto st0;
tr218:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st168;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
#line 4905 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr15;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr15;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 34u: goto st12;
		case 92u: goto st14;
	}
	goto st11;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 34u: goto st13;
		case 92u: goto st14;
	}
	goto st11;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 34u: goto tr20;
		case 92u: goto st14;
	}
	goto st11;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 34u: goto st11;
		case 39u: goto st11;
		case 85u: goto st15;
		case 92u: goto st11;
		case 98u: goto st11;
		case 102u: goto st11;
		case 110u: goto st11;
		case 114u: goto st11;
		case 116u: goto st11;
		case 117u: goto st19;
	}
	goto tr12;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st16;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st16;
	} else
		goto st16;
	goto tr12;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st17;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st17;
	} else
		goto st17;
	goto tr12;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st18;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st18;
	} else
		goto st18;
	goto tr12;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st19;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st19;
	} else
		goto st19;
	goto tr12;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st20;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st20;
	} else
		goto st20;
	goto tr12;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st21;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st21;
	} else
		goto st21;
	goto tr12;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st22;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st22;
	} else
		goto st22;
	goto tr12;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st11;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st11;
	} else
		goto st11;
	goto tr12;
tr15:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st169;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 5082 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr219;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
tr219:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st170;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
#line 5099 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st11;
		case 13u: goto st11;
		case 34u: goto tr2;
		case 39u: goto st11;
		case 92u: goto st23;
	}
	goto st10;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 34u: goto st10;
		case 39u: goto st10;
		case 85u: goto st24;
		case 92u: goto st10;
		case 98u: goto st10;
		case 102u: goto st10;
		case 110u: goto st10;
		case 114u: goto st10;
		case 116u: goto st10;
		case 117u: goto st28;
	}
	goto tr12;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st25;
	} else
		goto st25;
	goto tr12;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st26;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st26;
	} else
		goto st26;
	goto tr12;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st27;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st27;
	} else
		goto st27;
	goto tr12;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st28;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st28;
	} else
		goto st28;
	goto tr12;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st29;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st29;
	} else
		goto st29;
	goto tr12;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st30;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st30;
	} else
		goto st30;
	goto tr12;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st31;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st31;
	} else
		goto st31;
	goto tr12;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st10;
	} else
		goto st10;
	goto tr12;
tr178:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 5237 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
	}
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 10u: goto st172;
		case 13u: goto tr39;
	}
	goto tr37;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	if ( (*p) == 13u )
		goto tr39;
	goto tr221;
tr179:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 5269 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr215;
		case 13u: goto tr215;
		case 34u: goto tr215;
		case 39u: goto tr222;
		case 92u: goto st34;
	}
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr41;
		case 92u: goto st34;
	}
	goto st33;
tr41:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 69 "turtle.rl"
	{ shared_lexer->env.act = 12;}
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 5300 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr223;
		case 13u: goto tr223;
		case 34u: goto tr223;
		case 39u: goto tr41;
		case 92u: goto st34;
	}
	goto st33;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 34u: goto st33;
		case 39u: goto st33;
		case 85u: goto st35;
		case 92u: goto st33;
		case 98u: goto st33;
		case 102u: goto st33;
		case 110u: goto st33;
		case 114u: goto st33;
		case 116u: goto st33;
		case 117u: goto st39;
	}
	goto tr0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st36;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st36;
	} else
		goto st36;
	goto tr0;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st37;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st37;
	} else
		goto st37;
	goto tr0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st38;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st38;
	} else
		goto st38;
	goto tr0;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st39;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st39;
	} else
		goto st39;
	goto tr0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st40;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st40;
	} else
		goto st40;
	goto tr0;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st41;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st41;
	} else
		goto st41;
	goto tr0;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st42;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st42;
	} else
		goto st42;
	goto tr0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st33;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st33;
	} else
		goto st33;
	goto tr0;
tr222:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 69 "turtle.rl"
	{ shared_lexer->env.act = 12;}
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 5440 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr223;
		case 13u: goto tr223;
		case 34u: goto tr223;
		case 39u: goto tr224;
		case 92u: goto st34;
	}
	goto st33;
tr224:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 5457 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr54;
		case 92u: goto st56;
	}
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr54;
		case 92u: goto st56;
	}
	goto st43;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	switch( (*p) ) {
		case 39u: goto st45;
		case 92u: goto st47;
	}
	goto st44;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 39u: goto st46;
		case 92u: goto st47;
	}
	goto st44;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 39u: goto tr59;
		case 92u: goto st47;
	}
	goto st44;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 34u: goto st44;
		case 39u: goto st44;
		case 85u: goto st48;
		case 92u: goto st44;
		case 98u: goto st44;
		case 102u: goto st44;
		case 110u: goto st44;
		case 114u: goto st44;
		case 116u: goto st44;
		case 117u: goto st52;
	}
	goto tr51;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st49;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st49;
	} else
		goto st49;
	goto tr51;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st50;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st50;
	} else
		goto st50;
	goto tr51;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st51;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st51;
	} else
		goto st51;
	goto tr51;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st52;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st52;
	} else
		goto st52;
	goto tr51;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st53;
	} else
		goto st53;
	goto tr51;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st54;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st54;
	} else
		goto st54;
	goto tr51;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st55;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st55;
	} else
		goto st55;
	goto tr51;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st44;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st44;
	} else
		goto st44;
	goto tr51;
tr54:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 5634 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr225;
		case 92u: goto st56;
	}
	goto st43;
tr225:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 5651 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st44;
		case 13u: goto st44;
		case 34u: goto st44;
		case 39u: goto tr41;
		case 92u: goto st56;
	}
	goto st43;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 34u: goto st43;
		case 39u: goto st43;
		case 85u: goto st57;
		case 92u: goto st43;
		case 98u: goto st43;
		case 102u: goto st43;
		case 110u: goto st43;
		case 114u: goto st43;
		case 116u: goto st43;
		case 117u: goto st61;
	}
	goto tr51;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st58;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st58;
	} else
		goto st58;
	goto tr51;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st59;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st59;
	} else
		goto st59;
	goto tr51;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st60;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st60;
	} else
		goto st60;
	goto tr51;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st61;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st61;
	} else
		goto st61;
	goto tr51;
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
	goto tr51;
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
	goto tr51;
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
	goto tr51;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st43;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st43;
	} else
		goto st43;
	goto tr51;
tr182:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st179;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
#line 5789 "lexer2.c"
	if ( (*p) == 46u )
		goto st65;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr185;
	goto tr215;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr37;
tr76:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 61 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 5812 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr227;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 43u: goto st67;
		case 45u: goto st67;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr0;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st181;
	goto tr228;
tr185:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 60 "turtle.rl"
	{ shared_lexer->env.act = 7;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 5855 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st68;
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr185;
	goto tr229;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	switch( (*p) ) {
		case 69u: goto st66;
		case 101u: goto st66;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr79;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr76;
	goto tr231;
tr140:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 5890 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st69;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 205u: goto st96;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st97;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr83;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr83;
		} else
			goto tr83;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto st74;
	goto tr232;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st70;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st70;
	} else
		goto st70;
	goto tr81;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr83;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr83;
	} else
		goto tr83;
	goto tr81;
tr83:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 5963 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st69;
		case 45u: goto tr83;
		case 46u: goto st71;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 183u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 204u: goto st76;
		case 205u: goto st77;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st80;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr83;
		} else if ( (*p) >= 48u )
			goto tr83;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto tr83;
	goto tr232;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 37u: goto st69;
		case 45u: goto tr83;
		case 46u: goto st71;
		case 92u: goto st72;
		case 95u: goto tr83;
		case 183u: goto tr83;
		case 195u: goto st73;
		case 203u: goto st75;
		case 204u: goto st76;
		case 205u: goto st77;
		case 223u: goto st75;
		case 224u: goto st78;
		case 225u: goto st79;
		case 226u: goto st80;
		case 227u: goto st85;
		case 237u: goto st88;
		case 239u: goto st89;
		case 240u: goto st92;
		case 243u: goto st95;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr83;
		} else if ( (*p) >= 48u )
			goto tr83;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st74;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st94;
		} else
			goto st87;
	} else
		goto tr83;
	goto tr81;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 33u: goto tr83;
		case 59u: goto tr83;
		case 61u: goto tr83;
		case 95u: goto tr83;
		case 126u: goto tr83;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr83;
	} else if ( (*p) >= 35u )
		goto tr83;
	goto tr81;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr83;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr83;
	} else
		goto tr83;
	goto tr81;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	goto tr83;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( 192u <= (*p) )
		goto tr81;
	goto tr83;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( 128u <= (*p) )
		goto tr83;
	goto tr81;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( (*p) == 190u )
		goto tr81;
	goto tr83;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) == 160u )
		goto st76;
	if ( 161u <= (*p) )
		goto st74;
	goto tr81;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	if ( (*p) == 191u )
		goto st75;
	if ( 192u <= (*p) )
		goto tr81;
	goto st74;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	switch( (*p) ) {
		case 128u: goto st81;
		case 129u: goto st82;
		case 134u: goto st83;
		case 176u: goto st76;
		case 191u: goto st84;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st74;
	} else if ( (*p) >= 130u )
		goto st74;
	goto tr81;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr83;
	} else if ( (*p) >= 140u )
		goto tr83;
	goto tr81;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr81;
	goto tr83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( 144u <= (*p) )
		goto tr81;
	goto tr83;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	if ( 176u <= (*p) )
		goto tr81;
	goto tr83;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 128u )
		goto st86;
	if ( 129u <= (*p) )
		goto st74;
	goto tr81;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( 129u <= (*p) )
		goto tr83;
	goto tr81;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	goto st74;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 159u )
		goto st75;
	if ( 160u <= (*p) )
		goto tr81;
	goto st74;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	switch( (*p) ) {
		case 164u: goto st76;
		case 183u: goto st90;
		case 191u: goto st91;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st74;
	goto tr81;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr81;
	goto tr83;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( 190u <= (*p) )
		goto tr81;
	goto tr83;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) == 144u )
		goto st93;
	if ( 145u <= (*p) )
		goto st87;
	goto tr81;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( (*p) == 128u )
		goto st76;
	if ( 129u <= (*p) )
		goto st74;
	goto tr81;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	goto st87;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 175u )
		goto st79;
	if ( 176u <= (*p) )
		goto tr81;
	goto st87;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr83;
	} else if ( (*p) >= 176u )
		goto tr83;
	goto tr81;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	switch( (*p) ) {
		case 128u: goto st98;
		case 129u: goto st99;
		case 134u: goto st83;
		case 176u: goto st76;
		case 191u: goto st84;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st74;
	} else if ( (*p) >= 130u )
		goto st74;
	goto tr81;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr83;
	goto tr81;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( 176u <= (*p) )
		goto tr83;
	goto tr81;
tr187:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 6305 "lexer2.c"
	switch( (*p) ) {
		case 35u: goto st100;
		case 62u: goto tr113;
		case 92u: goto st100;
		case 95u: goto st100;
		case 183u: goto st100;
		case 195u: goto st101;
		case 203u: goto st103;
		case 204u: goto st104;
		case 205u: goto st105;
		case 223u: goto st103;
		case 224u: goto st106;
		case 225u: goto st107;
		case 226u: goto st108;
		case 227u: goto st113;
		case 237u: goto st116;
		case 239u: goto st117;
		case 240u: goto st120;
		case 243u: goto st123;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st100;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st100;
		} else
			goto st100;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st102;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st122;
		} else
			goto st115;
	} else
		goto st100;
	goto tr215;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	switch( (*p) ) {
		case 35u: goto st100;
		case 62u: goto tr113;
		case 92u: goto st100;
		case 95u: goto st100;
		case 183u: goto st100;
		case 195u: goto st101;
		case 203u: goto st103;
		case 204u: goto st104;
		case 205u: goto st105;
		case 223u: goto st103;
		case 224u: goto st106;
		case 225u: goto st107;
		case 226u: goto st108;
		case 227u: goto st113;
		case 237u: goto st116;
		case 239u: goto st117;
		case 240u: goto st120;
		case 243u: goto st123;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st100;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st100;
		} else
			goto st100;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st102;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st122;
		} else
			goto st115;
	} else
		goto st100;
	goto tr37;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st100;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st100;
	} else
		goto st100;
	goto tr37;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	goto st100;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( 192u <= (*p) )
		goto tr37;
	goto st100;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	if ( 128u <= (*p) )
		goto st100;
	goto tr37;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) == 190u )
		goto tr37;
	goto st100;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	if ( (*p) == 160u )
		goto st104;
	if ( 161u <= (*p) )
		goto st102;
	goto tr37;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	if ( (*p) == 191u )
		goto st103;
	if ( 192u <= (*p) )
		goto tr37;
	goto st102;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	switch( (*p) ) {
		case 128u: goto st109;
		case 129u: goto st110;
		case 134u: goto st111;
		case 176u: goto st104;
		case 191u: goto st112;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st102;
	} else if ( (*p) >= 130u )
		goto st102;
	goto tr37;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st100;
	} else if ( (*p) >= 140u )
		goto st100;
	goto tr37;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr37;
	goto st100;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( 144u <= (*p) )
		goto tr37;
	goto st100;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( 176u <= (*p) )
		goto tr37;
	goto st100;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 128u )
		goto st114;
	if ( 129u <= (*p) )
		goto st102;
	goto tr37;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( 129u <= (*p) )
		goto st100;
	goto tr37;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	goto st102;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( (*p) == 159u )
		goto st103;
	if ( 160u <= (*p) )
		goto tr37;
	goto st102;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	switch( (*p) ) {
		case 164u: goto st104;
		case 183u: goto st118;
		case 191u: goto st119;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st102;
	goto tr37;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr37;
	goto st100;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	if ( 190u <= (*p) )
		goto tr37;
	goto st100;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( (*p) == 144u )
		goto st121;
	if ( 145u <= (*p) )
		goto st115;
	goto tr37;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	if ( (*p) == 128u )
		goto st104;
	if ( 129u <= (*p) )
		goto st102;
	goto tr37;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	goto st115;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	if ( (*p) == 175u )
		goto st107;
	if ( 176u <= (*p) )
		goto tr37;
	goto st115;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
	switch( (*p) ) {
		case 98u: goto tr236;
		case 112u: goto tr237;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr215;
tr235:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st188;
tr242:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 47 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st188;
tr247:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 45 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 6621 "lexer2.c"
	if ( (*p) == 45u )
		goto st124;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr0;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr137;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr137;
	} else
		goto tr137;
	goto tr0;
tr137:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 6653 "lexer2.c"
	if ( (*p) == 45u )
		goto st124;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr137;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr137;
	} else
		goto tr137;
	goto tr239;
tr236:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 6675 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 97u: goto tr240;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr240:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 6696 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 115u: goto tr241;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr241:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 6717 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 101u: goto tr242;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr237:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 6738 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 114u: goto tr243;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr243:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 6759 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 101u: goto tr244;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr244:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 6780 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 102u: goto tr245;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr245:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 6801 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 105u: goto tr246;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr246:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{ shared_lexer->env.act = 16;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 6822 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st124;
		case 120u: goto tr247;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr235;
	} else if ( (*p) >= 65u )
		goto tr235;
	goto tr239;
tr165:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 48 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st198;
tr169:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 46 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st198;
tr172:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 63 "turtle.rl"
	{ shared_lexer->env.act = 10;}
	goto st198;
tr189:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st198;
tr195:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 86 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 6867 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 95u: goto st125;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st125;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st125;
	} else
		goto st125;
	goto tr0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	goto st125;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( 192u <= (*p) )
		goto tr0;
	goto st125;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	if ( 128u <= (*p) )
		goto st125;
	goto tr0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) == 190u )
		goto tr0;
	goto st125;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	if ( (*p) == 160u )
		goto st130;
	if ( 161u <= (*p) )
		goto st128;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	if ( (*p) == 191u )
		goto st129;
	if ( 192u <= (*p) )
		goto tr0;
	goto st128;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 128u: goto st135;
		case 129u: goto st136;
		case 134u: goto st137;
		case 176u: goto st130;
		case 191u: goto st138;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st128;
	} else if ( (*p) >= 130u )
		goto st128;
	goto tr0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st125;
	} else if ( (*p) >= 140u )
		goto st125;
	goto tr0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st125;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	if ( 144u <= (*p) )
		goto tr0;
	goto st125;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( 176u <= (*p) )
		goto tr0;
	goto st125;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( (*p) == 128u )
		goto st140;
	if ( 129u <= (*p) )
		goto st128;
	goto tr0;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( 129u <= (*p) )
		goto st125;
	goto tr0;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	goto st128;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 159u )
		goto st129;
	if ( 160u <= (*p) )
		goto tr0;
	goto st128;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	switch( (*p) ) {
		case 164u: goto st130;
		case 183u: goto st144;
		case 191u: goto st145;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st128;
	goto tr0;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st125;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( 190u <= (*p) )
		goto tr0;
	goto st125;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( (*p) == 144u )
		goto st147;
	if ( 145u <= (*p) )
		goto st141;
	goto tr0;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	if ( (*p) == 128u )
		goto st130;
	if ( 129u <= (*p) )
		goto st128;
	goto tr0;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	goto st141;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( (*p) == 175u )
		goto st133;
	if ( 176u <= (*p) )
		goto tr0;
	goto st141;
tr190:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 7192 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 65u: goto st150;
		case 95u: goto st125;
		case 97u: goto st150;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 83u: goto st151;
		case 95u: goto st125;
		case 115u: goto st151;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 69u: goto tr165;
		case 95u: goto st125;
		case 101u: goto tr165;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr191:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 7331 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 82u: goto st152;
		case 95u: goto st125;
		case 114u: goto st152;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 69u: goto st153;
		case 95u: goto st125;
		case 101u: goto st153;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 70u: goto st154;
		case 95u: goto st125;
		case 102u: goto st154;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 73u: goto st155;
		case 95u: goto st125;
		case 105u: goto st155;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 88u: goto tr169;
		case 95u: goto st125;
		case 120u: goto tr169;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
	if ( (*p) == 94u )
		goto tr250;
	goto tr215;
tr196:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 7565 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 97u: goto st156;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 108u: goto st157;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 115u: goto st158;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 101u: goto tr172;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr197:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 7744 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 114u: goto st159;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr215;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	switch( (*p) ) {
		case 45u: goto st125;
		case 46u: goto st126;
		case 58u: goto tr140;
		case 95u: goto st125;
		case 117u: goto st158;
		case 183u: goto st125;
		case 195u: goto st127;
		case 203u: goto st129;
		case 204u: goto st130;
		case 205u: goto st131;
		case 223u: goto st129;
		case 224u: goto st132;
		case 225u: goto st133;
		case 226u: goto st134;
		case 227u: goto st139;
		case 237u: goto st142;
		case 239u: goto st143;
		case 240u: goto st146;
		case 243u: goto st149;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st125;
		} else if ( (*p) >= 48u )
			goto st125;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st128;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st148;
		} else
			goto st141;
	} else
		goto st125;
	goto tr37;
tr200:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st204;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
#line 7837 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st125;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st125;
	} else
		goto st125;
	goto tr215;
tr201:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 7857 "lexer2.c"
	goto st125;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 7869 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr215;
	goto st125;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 7883 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st125;
	} else if ( (*p) >= 176u )
		goto st125;
	goto tr215;
tr204:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 7900 "lexer2.c"
	if ( (*p) == 160u )
		goto st130;
	if ( 161u <= (*p) )
		goto st128;
	goto tr215;
tr205:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 7916 "lexer2.c"
	if ( (*p) == 191u )
		goto st129;
	if ( 192u <= (*p) )
		goto tr215;
	goto st128;
tr206:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 7932 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st160;
		case 129u: goto st161;
		case 134u: goto st137;
		case 176u: goto st130;
		case 191u: goto st138;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st128;
	} else if ( (*p) >= 130u )
		goto st128;
	goto tr215;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st125;
	goto tr37;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	if ( 176u <= (*p) )
		goto st125;
	goto tr37;
tr207:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 7970 "lexer2.c"
	if ( (*p) == 128u )
		goto st140;
	if ( 129u <= (*p) )
		goto st128;
	goto tr215;
tr208:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 7986 "lexer2.c"
	goto st128;
tr209:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 7998 "lexer2.c"
	if ( (*p) == 159u )
		goto st129;
	if ( 160u <= (*p) )
		goto tr215;
	goto st128;
tr210:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 8014 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st130;
		case 183u: goto st144;
		case 191u: goto st145;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st128;
	goto tr215;
tr211:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 8033 "lexer2.c"
	if ( (*p) == 144u )
		goto st147;
	if ( 145u <= (*p) )
		goto st141;
	goto tr215;
tr212:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 8049 "lexer2.c"
	goto st141;
tr213:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 8061 "lexer2.c"
	if ( (*p) == 175u )
		goto st133;
	if ( 176u <= (*p) )
		goto tr215;
	goto st141;
	}
	_test_eof162:  shared_lexer->env.cs = 162; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof0:  shared_lexer->env.cs = 0; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof1:  shared_lexer->env.cs = 1; goto _test_eof; 
	_test_eof2:  shared_lexer->env.cs = 2; goto _test_eof; 
	_test_eof3:  shared_lexer->env.cs = 3; goto _test_eof; 
	_test_eof4:  shared_lexer->env.cs = 4; goto _test_eof; 
	_test_eof5:  shared_lexer->env.cs = 5; goto _test_eof; 
	_test_eof6:  shared_lexer->env.cs = 6; goto _test_eof; 
	_test_eof7:  shared_lexer->env.cs = 7; goto _test_eof; 
	_test_eof8:  shared_lexer->env.cs = 8; goto _test_eof; 
	_test_eof9:  shared_lexer->env.cs = 9; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof10:  shared_lexer->env.cs = 10; goto _test_eof; 
	_test_eof11:  shared_lexer->env.cs = 11; goto _test_eof; 
	_test_eof12:  shared_lexer->env.cs = 12; goto _test_eof; 
	_test_eof13:  shared_lexer->env.cs = 13; goto _test_eof; 
	_test_eof14:  shared_lexer->env.cs = 14; goto _test_eof; 
	_test_eof15:  shared_lexer->env.cs = 15; goto _test_eof; 
	_test_eof16:  shared_lexer->env.cs = 16; goto _test_eof; 
	_test_eof17:  shared_lexer->env.cs = 17; goto _test_eof; 
	_test_eof18:  shared_lexer->env.cs = 18; goto _test_eof; 
	_test_eof19:  shared_lexer->env.cs = 19; goto _test_eof; 
	_test_eof20:  shared_lexer->env.cs = 20; goto _test_eof; 
	_test_eof21:  shared_lexer->env.cs = 21; goto _test_eof; 
	_test_eof22:  shared_lexer->env.cs = 22; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof43:  shared_lexer->env.cs = 43; goto _test_eof; 
	_test_eof44:  shared_lexer->env.cs = 44; goto _test_eof; 
	_test_eof45:  shared_lexer->env.cs = 45; goto _test_eof; 
	_test_eof46:  shared_lexer->env.cs = 46; goto _test_eof; 
	_test_eof47:  shared_lexer->env.cs = 47; goto _test_eof; 
	_test_eof48:  shared_lexer->env.cs = 48; goto _test_eof; 
	_test_eof49:  shared_lexer->env.cs = 49; goto _test_eof; 
	_test_eof50:  shared_lexer->env.cs = 50; goto _test_eof; 
	_test_eof51:  shared_lexer->env.cs = 51; goto _test_eof; 
	_test_eof52:  shared_lexer->env.cs = 52; goto _test_eof; 
	_test_eof53:  shared_lexer->env.cs = 53; goto _test_eof; 
	_test_eof54:  shared_lexer->env.cs = 54; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof61:  shared_lexer->env.cs = 61; goto _test_eof; 
	_test_eof62:  shared_lexer->env.cs = 62; goto _test_eof; 
	_test_eof63:  shared_lexer->env.cs = 63; goto _test_eof; 
	_test_eof64:  shared_lexer->env.cs = 64; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof65:  shared_lexer->env.cs = 65; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof66:  shared_lexer->env.cs = 66; goto _test_eof; 
	_test_eof67:  shared_lexer->env.cs = 67; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof68:  shared_lexer->env.cs = 68; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof69:  shared_lexer->env.cs = 69; goto _test_eof; 
	_test_eof70:  shared_lexer->env.cs = 70; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof71:  shared_lexer->env.cs = 71; goto _test_eof; 
	_test_eof72:  shared_lexer->env.cs = 72; goto _test_eof; 
	_test_eof73:  shared_lexer->env.cs = 73; goto _test_eof; 
	_test_eof74:  shared_lexer->env.cs = 74; goto _test_eof; 
	_test_eof75:  shared_lexer->env.cs = 75; goto _test_eof; 
	_test_eof76:  shared_lexer->env.cs = 76; goto _test_eof; 
	_test_eof77:  shared_lexer->env.cs = 77; goto _test_eof; 
	_test_eof78:  shared_lexer->env.cs = 78; goto _test_eof; 
	_test_eof79:  shared_lexer->env.cs = 79; goto _test_eof; 
	_test_eof80:  shared_lexer->env.cs = 80; goto _test_eof; 
	_test_eof81:  shared_lexer->env.cs = 81; goto _test_eof; 
	_test_eof82:  shared_lexer->env.cs = 82; goto _test_eof; 
	_test_eof83:  shared_lexer->env.cs = 83; goto _test_eof; 
	_test_eof84:  shared_lexer->env.cs = 84; goto _test_eof; 
	_test_eof85:  shared_lexer->env.cs = 85; goto _test_eof; 
	_test_eof86:  shared_lexer->env.cs = 86; goto _test_eof; 
	_test_eof87:  shared_lexer->env.cs = 87; goto _test_eof; 
	_test_eof88:  shared_lexer->env.cs = 88; goto _test_eof; 
	_test_eof89:  shared_lexer->env.cs = 89; goto _test_eof; 
	_test_eof90:  shared_lexer->env.cs = 90; goto _test_eof; 
	_test_eof91:  shared_lexer->env.cs = 91; goto _test_eof; 
	_test_eof92:  shared_lexer->env.cs = 92; goto _test_eof; 
	_test_eof93:  shared_lexer->env.cs = 93; goto _test_eof; 
	_test_eof94:  shared_lexer->env.cs = 94; goto _test_eof; 
	_test_eof95:  shared_lexer->env.cs = 95; goto _test_eof; 
	_test_eof96:  shared_lexer->env.cs = 96; goto _test_eof; 
	_test_eof97:  shared_lexer->env.cs = 97; goto _test_eof; 
	_test_eof98:  shared_lexer->env.cs = 98; goto _test_eof; 
	_test_eof99:  shared_lexer->env.cs = 99; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof100:  shared_lexer->env.cs = 100; goto _test_eof; 
	_test_eof101:  shared_lexer->env.cs = 101; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof103:  shared_lexer->env.cs = 103; goto _test_eof; 
	_test_eof104:  shared_lexer->env.cs = 104; goto _test_eof; 
	_test_eof105:  shared_lexer->env.cs = 105; goto _test_eof; 
	_test_eof106:  shared_lexer->env.cs = 106; goto _test_eof; 
	_test_eof107:  shared_lexer->env.cs = 107; goto _test_eof; 
	_test_eof108:  shared_lexer->env.cs = 108; goto _test_eof; 
	_test_eof109:  shared_lexer->env.cs = 109; goto _test_eof; 
	_test_eof110:  shared_lexer->env.cs = 110; goto _test_eof; 
	_test_eof111:  shared_lexer->env.cs = 111; goto _test_eof; 
	_test_eof112:  shared_lexer->env.cs = 112; goto _test_eof; 
	_test_eof113:  shared_lexer->env.cs = 113; goto _test_eof; 
	_test_eof114:  shared_lexer->env.cs = 114; goto _test_eof; 
	_test_eof115:  shared_lexer->env.cs = 115; goto _test_eof; 
	_test_eof116:  shared_lexer->env.cs = 116; goto _test_eof; 
	_test_eof117:  shared_lexer->env.cs = 117; goto _test_eof; 
	_test_eof118:  shared_lexer->env.cs = 118; goto _test_eof; 
	_test_eof119:  shared_lexer->env.cs = 119; goto _test_eof; 
	_test_eof120:  shared_lexer->env.cs = 120; goto _test_eof; 
	_test_eof121:  shared_lexer->env.cs = 121; goto _test_eof; 
	_test_eof122:  shared_lexer->env.cs = 122; goto _test_eof; 
	_test_eof123:  shared_lexer->env.cs = 123; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof135:  shared_lexer->env.cs = 135; goto _test_eof; 
	_test_eof136:  shared_lexer->env.cs = 136; goto _test_eof; 
	_test_eof137:  shared_lexer->env.cs = 137; goto _test_eof; 
	_test_eof138:  shared_lexer->env.cs = 138; goto _test_eof; 
	_test_eof139:  shared_lexer->env.cs = 139; goto _test_eof; 
	_test_eof140:  shared_lexer->env.cs = 140; goto _test_eof; 
	_test_eof141:  shared_lexer->env.cs = 141; goto _test_eof; 
	_test_eof142:  shared_lexer->env.cs = 142; goto _test_eof; 
	_test_eof143:  shared_lexer->env.cs = 143; goto _test_eof; 
	_test_eof144:  shared_lexer->env.cs = 144; goto _test_eof; 
	_test_eof145:  shared_lexer->env.cs = 145; goto _test_eof; 
	_test_eof146:  shared_lexer->env.cs = 146; goto _test_eof; 
	_test_eof147:  shared_lexer->env.cs = 147; goto _test_eof; 
	_test_eof148:  shared_lexer->env.cs = 148; goto _test_eof; 
	_test_eof149:  shared_lexer->env.cs = 149; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof150:  shared_lexer->env.cs = 150; goto _test_eof; 
	_test_eof151:  shared_lexer->env.cs = 151; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof152:  shared_lexer->env.cs = 152; goto _test_eof; 
	_test_eof153:  shared_lexer->env.cs = 153; goto _test_eof; 
	_test_eof154:  shared_lexer->env.cs = 154; goto _test_eof; 
	_test_eof155:  shared_lexer->env.cs = 155; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof156:  shared_lexer->env.cs = 156; goto _test_eof; 
	_test_eof157:  shared_lexer->env.cs = 157; goto _test_eof; 
	_test_eof158:  shared_lexer->env.cs = 158; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof159:  shared_lexer->env.cs = 159; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof160:  shared_lexer->env.cs = 160; goto _test_eof; 
	_test_eof161:  shared_lexer->env.cs = 161; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 163: goto tr0;
	case 164: goto tr214;
	case 165: goto tr215;
	case 0: goto tr0;
	case 166: goto tr217;
	case 1: goto tr0;
	case 2: goto tr0;
	case 3: goto tr0;
	case 4: goto tr0;
	case 5: goto tr0;
	case 6: goto tr0;
	case 7: goto tr0;
	case 8: goto tr0;
	case 9: goto tr0;
	case 167: goto tr217;
	case 168: goto tr217;
	case 10: goto tr12;
	case 11: goto tr12;
	case 12: goto tr12;
	case 13: goto tr12;
	case 14: goto tr12;
	case 15: goto tr12;
	case 16: goto tr12;
	case 17: goto tr12;
	case 18: goto tr12;
	case 19: goto tr12;
	case 20: goto tr12;
	case 21: goto tr12;
	case 22: goto tr12;
	case 169: goto tr217;
	case 170: goto tr217;
	case 23: goto tr12;
	case 24: goto tr12;
	case 25: goto tr12;
	case 26: goto tr12;
	case 27: goto tr12;
	case 28: goto tr12;
	case 29: goto tr12;
	case 30: goto tr12;
	case 31: goto tr12;
	case 171: goto tr215;
	case 32: goto tr37;
	case 172: goto tr221;
	case 173: goto tr215;
	case 33: goto tr0;
	case 174: goto tr223;
	case 34: goto tr0;
	case 35: goto tr0;
	case 36: goto tr0;
	case 37: goto tr0;
	case 38: goto tr0;
	case 39: goto tr0;
	case 40: goto tr0;
	case 41: goto tr0;
	case 42: goto tr0;
	case 175: goto tr223;
	case 176: goto tr223;
	case 43: goto tr51;
	case 44: goto tr51;
	case 45: goto tr51;
	case 46: goto tr51;
	case 47: goto tr51;
	case 48: goto tr51;
	case 49: goto tr51;
	case 50: goto tr51;
	case 51: goto tr51;
	case 52: goto tr51;
	case 53: goto tr51;
	case 54: goto tr51;
	case 55: goto tr51;
	case 177: goto tr223;
	case 178: goto tr223;
	case 56: goto tr51;
	case 57: goto tr51;
	case 58: goto tr51;
	case 59: goto tr51;
	case 60: goto tr51;
	case 61: goto tr51;
	case 62: goto tr51;
	case 63: goto tr51;
	case 64: goto tr51;
	case 179: goto tr215;
	case 65: goto tr37;
	case 180: goto tr227;
	case 66: goto tr0;
	case 67: goto tr0;
	case 181: goto tr228;
	case 182: goto tr229;
	case 68: goto tr79;
	case 183: goto tr231;
	case 184: goto tr232;
	case 69: goto tr81;
	case 70: goto tr81;
	case 185: goto tr232;
	case 71: goto tr81;
	case 72: goto tr81;
	case 73: goto tr81;
	case 74: goto tr81;
	case 75: goto tr81;
	case 76: goto tr81;
	case 77: goto tr81;
	case 78: goto tr81;
	case 79: goto tr81;
	case 80: goto tr81;
	case 81: goto tr81;
	case 82: goto tr81;
	case 83: goto tr81;
	case 84: goto tr81;
	case 85: goto tr81;
	case 86: goto tr81;
	case 87: goto tr81;
	case 88: goto tr81;
	case 89: goto tr81;
	case 90: goto tr81;
	case 91: goto tr81;
	case 92: goto tr81;
	case 93: goto tr81;
	case 94: goto tr81;
	case 95: goto tr81;
	case 96: goto tr81;
	case 97: goto tr81;
	case 98: goto tr81;
	case 99: goto tr81;
	case 186: goto tr215;
	case 100: goto tr37;
	case 101: goto tr37;
	case 102: goto tr37;
	case 103: goto tr37;
	case 104: goto tr37;
	case 105: goto tr37;
	case 106: goto tr37;
	case 107: goto tr37;
	case 108: goto tr37;
	case 109: goto tr37;
	case 110: goto tr37;
	case 111: goto tr37;
	case 112: goto tr37;
	case 113: goto tr37;
	case 114: goto tr37;
	case 115: goto tr37;
	case 116: goto tr37;
	case 117: goto tr37;
	case 118: goto tr37;
	case 119: goto tr37;
	case 120: goto tr37;
	case 121: goto tr37;
	case 122: goto tr37;
	case 123: goto tr37;
	case 187: goto tr215;
	case 188: goto tr0;
	case 124: goto tr0;
	case 189: goto tr239;
	case 190: goto tr239;
	case 191: goto tr239;
	case 192: goto tr239;
	case 193: goto tr239;
	case 194: goto tr239;
	case 195: goto tr239;
	case 196: goto tr239;
	case 197: goto tr239;
	case 198: goto tr0;
	case 125: goto tr0;
	case 126: goto tr0;
	case 127: goto tr0;
	case 128: goto tr0;
	case 129: goto tr0;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 134: goto tr0;
	case 135: goto tr0;
	case 136: goto tr0;
	case 137: goto tr0;
	case 138: goto tr0;
	case 139: goto tr0;
	case 140: goto tr0;
	case 141: goto tr0;
	case 142: goto tr0;
	case 143: goto tr0;
	case 144: goto tr0;
	case 145: goto tr0;
	case 146: goto tr0;
	case 147: goto tr0;
	case 148: goto tr0;
	case 149: goto tr0;
	case 199: goto tr215;
	case 150: goto tr37;
	case 151: goto tr37;
	case 200: goto tr215;
	case 152: goto tr37;
	case 153: goto tr37;
	case 154: goto tr37;
	case 155: goto tr37;
	case 201: goto tr215;
	case 202: goto tr215;
	case 156: goto tr37;
	case 157: goto tr37;
	case 158: goto tr37;
	case 203: goto tr215;
	case 159: goto tr37;
	case 204: goto tr215;
	case 205: goto tr215;
	case 206: goto tr215;
	case 207: goto tr215;
	case 208: goto tr215;
	case 209: goto tr215;
	case 210: goto tr215;
	case 160: goto tr37;
	case 161: goto tr37;
	case 211: goto tr215;
	case 212: goto tr215;
	case 213: goto tr215;
	case 214: goto tr215;
	case 215: goto tr215;
	case 216: goto tr215;
	case 217: goto tr215;
	}
	}

	}

#line 319 "turtle.rl"
        /* clang-format on */
        handler( ARDP_SUCCESS );
}
/*}}}*/
/* ardp_lexer_turtle_reader() {{{ */
int ardp_lexer_process_reader( lexer_reader reader, void *_Nullable reader_args)
{
    assert(0); // Not implemented yet;
    return -1;
}

void ardp_lexer_process_reader_old( lexer_reader reader,
                                       void *_Nullable reader_args,
                                       completation_block handler )
{
        if ( ardp_lexer_is_ready() ) {
                handler( ARDP_LEXER_ERROR );
                return;
        }

        if ( reader is NULL ) {
                handler( ARDP_LEXER_READER_NOT_READY );
                return;
        }

        __block uint8_t *p;

        uint8_t buf[BUFSIZE];
        size_t have   = 0;
        uint8_t *mark = NULL; //nullptr;
        bool eof      = false;

        int status = 0;

        while ( not shared_lexer->finished ) {
                p = buf + have;

                size_t space = BUFSIZE - have;
                if ( space <= 0 ) {
                        /* Out of buffer space */
                        status = ARDP_LEXER_OMEM;
                        break; /* goto exit */
                }

                int len = reader( p, space, reader_args );

                if ( len < space ) {
                        eof                    = true;
                        shared_lexer->finished = 1;
                }

                ardp_lexer_turtle_process_block( p, len, mark, eof, ^( int success ) {
                  if ( success isnt ARDP_SUCCESS )
                          printf( "Internal lexer error - recoverable\n" );
                } );

                if ( shared_lexer->env.cs is turtle_error ) {
                        status = ARDP_LEXER_GENERIC_ERROR;
                        break; /* goto exit */
                } else if ( mark ) {
                        have = ( p + len ) - mark;
                        memmove( buf, mark, have );
                        mark = buf;
                } else {
                        have = 0;
                }
        }
        handler( status );
}
/*}}}*/

