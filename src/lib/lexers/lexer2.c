
#line 1 "turtle.rl"
/*! @file turtle.rl
 *
 *  Base file for the `Ragel's` preprocessor. It builds up on the lexer.h abstract
 *  interface.
 *
 *  @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 *  @date   2015
 *
 *  @TODO change the interface to reflect new scheme of token data:
 *              emit_token(type, value, line, column );
 *
 *  @TODO column guard
 */

/* vim: set ts=8 sw=4 tw=0 noet : set foldmethod=marker */

// HEADERS {{{ --------------------------------------------------------------------

#include <unistd.h>             /* sleep() */
#include <errno.h>              /* Included for 'stderr' */
#include <iso646.h>             /* verbose || and && */
#include <string.h>             /* C buffer manipulation functions */
#include <assert.h>             /* Asserts in the code */
#include <dispatch/dispatch.h>  /* Clang GCD dispatch_* functions */

#include <ardp/lexer.h>         /* Generic lexer */
#include <ardp/lexer.turtle.h>  /* Turtle specific lexer constants */

#include <ardp/util.h>
#include <ardp/color.h>
#include <ardp/string.h>

//#include "config.h"

//------------------------------------------------------------------------------}}}

#define BUFSIZE ( 0x10000 )

/*!
 * Global variable holding the shared lexer;
 */
static struct lexer *_Nullable shared_lexer;

/*!
 * Shorthand to speedup token input.
 */
#define var( s ) ( shared_lexer->env.s )

/* clang-format off */
/* ragel-machine() {{{*/

#line 55 "lexer2.c"
static const int turtle_start = 173;
static const int turtle_first_final = 173;
static const int turtle_error = -1;

static const int turtle_en_main = 173;


#line 168 "turtle.rl"

/* }}} */
/* clang-format on */

/*--------------------------------------------------------------------------------*/
// LOCAL FUNCTIONS, HELPERS, SHORTHANDS
/*--------------------------------------------------------------------------------*/
/* log() {{{ */
static void log(int level, const char* message)
{
    if (shared_lexer->log.level < level )
        if (shared_lexer->log.eprintf)
            dispatch_async( dispatch_get_main_queue(), ^{
                shared_lexer->log.eprintf(level, message);
            });
}
/*}}}*/
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

/* dispatch_queue_isempty() {{{ */
bool dispatch_queue_isempty(dispatch_queue_t queue)
{
    dispatch_group_t group = dispatch_group_create();

    dispatch_group_enter(group);
    dispatch_async(queue, ^{
        dispatch_group_leave(group);
    });

    int64_t maxWaitTime = 5 * NSEC_PER_SEC;
    bool isReady = dispatch_group_wait(group, maxWaitTime) == 0;

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
        dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
        dispatch_release(group);
    });

    return isReady;
}
/*}}}*/

/*--------------------------------------------------------------------------------*/
// EXTERN FUNCTIONS
/*--------------------------------------------------------------------------------*/
/* ardp_lexer_create() {{{ */
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
        shared_lexer->finished = 0;
        log ( INFO, "Created the shared lexer." );

        return ARDP_SUCCESS;
}
/*}}}*/
/* ardp_lexer_defaults() {{{ */
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
/* ardp_lexer_init() {{{*/
int ardp_lexer_init( struct ardp_lexer_config *_Nullable cfg)
{
    switch( ardp_lexer_state() ) {
    case ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED:
    case ARDP_LEXER_TURTLE_STATUS_READY:
        /* clang-format off */
        
#line 206 "lexer2.c"
	{
	 shared_lexer->env.cs = turtle_start;
	 shared_lexer->env.ts = 0;
	 shared_lexer->env.te = 0;
	 shared_lexer->env.act = 0;
	}

#line 310 "turtle.rl"
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
/* ardp_lexer_destroy() {{{ */
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
/* ardp_lexer_is_ready() {{{ */
bool ardp_lexer_is_ready()
{
        return ( ardp_lexer_state() is ARDP_LEXER_TURTLE_STATUS_READY );
}
/*}}}*/

/* ardp_lexer_process_block() {{{ */
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
    
#line 275 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 164 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st173;
tr2:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 128 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr12:
#line 128 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr20:
#line 119 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("<<< %s >>>\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st173;
tr31:
#line 163 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st173;
tr33:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 132 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr43:
#line 132 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr51:
#line 123 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("--- %s ---\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st173;
tr61:
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
	case 5:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }
	break;
	case 8:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 9:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 11:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 17:
	{{p = (( shared_lexer->env.te))-1;}
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }
	break;
	case 18:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(A); }
	break;
	case 28:
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
	goto st173;
tr64:
#line 114 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr66:
#line 111 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st173;
tr98:
#line 112 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1)); }}
	goto st173;
tr170:
#line 110 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st173;
tr188:
#line 164 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st173;
tr195:
#line 152 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st173;
tr196:
#line 153 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st173;
tr198:
#line 147 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st173;
tr201:
#line 148 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st173;
tr207:
#line 150 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st173;
tr208:
#line 151 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st173;
tr214:
#line 154 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st173;
tr215:
#line 155 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st173;
tr230:
#line 157 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st173;
tr231:
#line 164 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st173;
tr235:
#line 128 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr237:
#line 163 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st173;
tr241:
#line 132 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr244:
#line 115 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr245:
#line 116 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr246:
#line 114 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr248:
#line 146 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st173;
tr249:
#line 111 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st173;
tr256:
#line 140 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st173;
tr267:
#line 137 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st173;
tr269:
#line 110 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st173;
st173:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 519 "lexer2.c"
	switch( (*p) ) {
		case 9u: goto tr189;
		case 10u: goto st175;
		case 13u: goto tr191;
		case 32u: goto tr189;
		case 34u: goto tr192;
		case 35u: goto tr193;
		case 39u: goto tr194;
		case 40u: goto tr195;
		case 41u: goto tr196;
		case 44u: goto tr198;
		case 46u: goto st186;
		case 58u: goto tr125;
		case 59u: goto tr201;
		case 60u: goto tr202;
		case 64u: goto st190;
		case 66u: goto tr205;
		case 80u: goto tr206;
		case 91u: goto tr207;
		case 93u: goto tr208;
		case 94u: goto st204;
		case 95u: goto tr210;
		case 97u: goto tr211;
		case 98u: goto tr205;
		case 102u: goto tr212;
		case 112u: goto tr206;
		case 116u: goto tr213;
		case 123u: goto tr214;
		case 125u: goto tr215;
		case 195u: goto tr216;
		case 203u: goto tr218;
		case 205u: goto tr219;
		case 223u: goto tr218;
		case 224u: goto tr220;
		case 225u: goto tr221;
		case 226u: goto tr222;
		case 227u: goto tr223;
		case 237u: goto tr225;
		case 239u: goto tr226;
		case 240u: goto tr227;
		case 243u: goto tr229;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr197;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr204;
		} else
			goto tr200;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr217;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr228;
			} else if ( (*p) >= 228u )
				goto tr224;
		} else
			goto tr217;
	} else
		goto tr204;
	goto tr188;
tr189:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 163 "turtle.rl"
	{ shared_lexer->env.act = 29;}
	goto st174;
tr191:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 157 "turtle.rl"
	{ shared_lexer->env.act = 28;}
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 602 "lexer2.c"
	switch( (*p) ) {
		case 13u: goto tr189;
		case 32u: goto tr189;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr189;
	goto tr61;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
	switch( (*p) ) {
		case 13u: goto tr191;
		case 32u: goto tr189;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr189;
	goto tr230;
tr192:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 629 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr231;
		case 13u: goto tr231;
		case 34u: goto tr233;
		case 39u: goto tr231;
		case 92u: goto tr234;
	}
	goto tr232;
tr1:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr232:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
#line 665 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto tr3;
	}
	goto tr1;
tr3:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st1;
tr234:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 701 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr1;
		case 39u: goto tr1;
		case 85u: goto tr4;
		case 92u: goto tr1;
		case 98u: goto tr1;
		case 102u: goto tr1;
		case 110u: goto tr1;
		case 114u: goto tr1;
		case 116u: goto tr1;
		case 117u: goto tr5;
	}
	goto tr0;
tr4:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 728 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr6;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr6;
	} else
		goto tr6;
	goto tr0;
tr6:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 751 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr7;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr7;
	} else
		goto tr7;
	goto tr0;
tr7:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 774 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr8;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr8;
	} else
		goto tr8;
	goto tr0;
tr8:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 797 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr5;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr5;
	} else
		goto tr5;
	goto tr0;
tr5:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 820 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr9;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr9;
	} else
		goto tr9;
	goto tr0;
tr9:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 843 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr10;
	} else
		goto tr10;
	goto tr0;
tr10:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 866 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr11;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr11;
	} else
		goto tr11;
	goto tr0;
tr11:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 889 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr1;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr1;
	} else
		goto tr1;
	goto tr0;
tr233:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 918 "lexer2.c"
	if ( (*p) == 34u )
		goto st10;
	goto tr235;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 34u: goto tr14;
		case 92u: goto tr15;
	}
	goto tr13;
tr16:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st11;
tr13:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 958 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr17;
		case 92u: goto tr18;
	}
	goto tr16;
tr14:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st12;
tr17:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 989 "lexer2.c"
	if ( (*p) == 34u )
		goto st13;
	goto tr12;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 34u )
		goto tr20;
	goto tr12;
tr18:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st14;
tr15:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 1027 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr16;
		case 39u: goto tr16;
		case 85u: goto tr21;
		case 92u: goto tr16;
		case 98u: goto tr16;
		case 102u: goto tr16;
		case 110u: goto tr16;
		case 114u: goto tr16;
		case 116u: goto tr16;
		case 117u: goto tr22;
	}
	goto tr12;
tr21:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 1054 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr23;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr23;
	} else
		goto tr23;
	goto tr12;
tr23:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 1077 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr24;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr24;
	} else
		goto tr24;
	goto tr12;
tr24:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 1100 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr25;
	} else
		goto tr25;
	goto tr12;
tr25:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 1123 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr22;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr22;
	} else
		goto tr22;
	goto tr12;
tr22:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 1146 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr26;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr26;
	} else
		goto tr26;
	goto tr12;
tr26:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
#line 1169 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr27;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr27;
	} else
		goto tr27;
	goto tr12;
tr27:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 1192 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr28;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr28;
	} else
		goto tr28;
	goto tr12;
tr28:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 1215 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr16;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr16;
	} else
		goto tr16;
	goto tr12;
tr193:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 1233 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st179;
		case 13u: goto tr31;
	}
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 10u: goto st179;
		case 13u: goto tr31;
	}
	goto st23;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	if ( (*p) == 13u )
		goto tr31;
	goto tr237;
tr194:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 1263 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr231;
		case 13u: goto tr231;
		case 34u: goto tr231;
		case 39u: goto tr239;
		case 92u: goto tr240;
	}
	goto tr238;
tr32:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st24;
tr238:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 1299 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr33;
		case 92u: goto tr34;
	}
	goto tr32;
tr34:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st25;
tr240:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 1335 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr32;
		case 39u: goto tr32;
		case 85u: goto tr35;
		case 92u: goto tr32;
		case 98u: goto tr32;
		case 102u: goto tr32;
		case 110u: goto tr32;
		case 114u: goto tr32;
		case 116u: goto tr32;
		case 117u: goto tr36;
	}
	goto tr0;
tr35:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 1362 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr37;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr37;
	} else
		goto tr37;
	goto tr0;
tr37:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 1385 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr38;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr38;
	} else
		goto tr38;
	goto tr0;
tr38:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 1408 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr39;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr39;
	} else
		goto tr39;
	goto tr0;
tr39:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 1431 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr36;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr36;
	} else
		goto tr36;
	goto tr0;
tr36:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 1454 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr40;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr40;
	} else
		goto tr40;
	goto tr0;
tr40:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 1477 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr41;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr41;
	} else
		goto tr41;
	goto tr0;
tr41:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 1500 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr42;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr42;
	} else
		goto tr42;
	goto tr0;
tr42:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 1523 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr32;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr32;
	} else
		goto tr32;
	goto tr0;
tr239:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st181;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
#line 1552 "lexer2.c"
	if ( (*p) == 39u )
		goto st34;
	goto tr241;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 39u: goto tr45;
		case 92u: goto tr46;
	}
	goto tr44;
tr47:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st35;
tr44:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 1592 "lexer2.c"
	switch( (*p) ) {
		case 39u: goto tr48;
		case 92u: goto tr49;
	}
	goto tr47;
tr45:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st36;
tr48:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 1623 "lexer2.c"
	if ( (*p) == 39u )
		goto st37;
	goto tr43;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) == 39u )
		goto tr51;
	goto tr43;
tr49:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st38;
tr46:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 1661 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr47;
		case 39u: goto tr47;
		case 85u: goto tr52;
		case 92u: goto tr47;
		case 98u: goto tr47;
		case 102u: goto tr47;
		case 110u: goto tr47;
		case 114u: goto tr47;
		case 116u: goto tr47;
		case 117u: goto tr53;
	}
	goto tr43;
tr52:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 1688 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr54;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr54;
	} else
		goto tr54;
	goto tr43;
tr54:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 1711 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr55;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr55;
	} else
		goto tr55;
	goto tr43;
tr55:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 1734 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr56;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr56;
	} else
		goto tr56;
	goto tr43;
tr56:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 1757 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr53;
	} else
		goto tr53;
	goto tr43;
tr53:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 1780 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr57;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr57;
	} else
		goto tr57;
	goto tr43;
tr57:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 1803 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr58;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr58;
	} else
		goto tr58;
	goto tr43;
tr58:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 1826 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr59;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr59;
	} else
		goto tr59;
	goto tr43;
tr59:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 1849 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr47;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr47;
	} else
		goto tr47;
	goto tr43;
tr197:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 1867 "lexer2.c"
	if ( (*p) == 46u )
		goto st47;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr200;
	goto tr231;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr0;
tr60:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 115 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 1890 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr244;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	switch( (*p) ) {
		case 43u: goto st49;
		case 45u: goto st49;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr61;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr61;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr245;
tr200:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 114 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 1933 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st50;
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr200;
	goto tr246;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr64;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr248;
tr125:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 1968 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st51;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 205u: goto st78;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st79;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr68;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr68;
		} else
			goto tr68;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto st56;
	goto tr249;
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
	goto tr66;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr68;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr68;
	} else
		goto tr68;
	goto tr66;
tr68:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 2041 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st51;
		case 45u: goto tr68;
		case 46u: goto st53;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 183u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 204u: goto st58;
		case 205u: goto st59;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st62;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr68;
		} else if ( (*p) >= 48u )
			goto tr68;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto tr68;
	goto tr249;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 37u: goto st51;
		case 45u: goto tr68;
		case 46u: goto st53;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 183u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 204u: goto st58;
		case 205u: goto st59;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st62;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr68;
		} else if ( (*p) >= 48u )
			goto tr68;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto tr68;
	goto tr66;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 33u: goto tr68;
		case 59u: goto tr68;
		case 61u: goto tr68;
		case 95u: goto tr68;
		case 126u: goto tr68;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr68;
	} else if ( (*p) >= 35u )
		goto tr68;
	goto tr66;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr68;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr68;
	} else
		goto tr68;
	goto tr66;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	goto tr68;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 192u <= (*p) )
		goto tr66;
	goto tr68;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( 128u <= (*p) )
		goto tr68;
	goto tr66;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) == 190u )
		goto tr66;
	goto tr68;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) == 160u )
		goto st58;
	if ( 161u <= (*p) )
		goto st56;
	goto tr66;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) == 191u )
		goto st57;
	if ( 192u <= (*p) )
		goto tr66;
	goto st56;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 128u: goto st63;
		case 129u: goto st64;
		case 134u: goto st65;
		case 176u: goto st58;
		case 191u: goto st66;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st56;
	} else if ( (*p) >= 130u )
		goto st56;
	goto tr66;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr68;
	} else if ( (*p) >= 140u )
		goto tr68;
	goto tr66;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr66;
	goto tr68;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 144u <= (*p) )
		goto tr66;
	goto tr68;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( 176u <= (*p) )
		goto tr66;
	goto tr68;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) == 128u )
		goto st68;
	if ( 129u <= (*p) )
		goto st56;
	goto tr66;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( 129u <= (*p) )
		goto tr68;
	goto tr66;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	goto st56;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) == 159u )
		goto st57;
	if ( 160u <= (*p) )
		goto tr66;
	goto st56;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 164u: goto st58;
		case 183u: goto st72;
		case 191u: goto st73;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st56;
	goto tr66;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr66;
	goto tr68;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( 190u <= (*p) )
		goto tr66;
	goto tr68;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( (*p) == 144u )
		goto st75;
	if ( 145u <= (*p) )
		goto st69;
	goto tr66;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 128u )
		goto st58;
	if ( 129u <= (*p) )
		goto st56;
	goto tr66;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	goto st69;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( (*p) == 175u )
		goto st61;
	if ( 176u <= (*p) )
		goto tr66;
	goto st69;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr68;
	} else if ( (*p) >= 176u )
		goto tr68;
	goto tr66;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 128u: goto st80;
		case 129u: goto st81;
		case 134u: goto st65;
		case 176u: goto st58;
		case 191u: goto st66;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st56;
	} else if ( (*p) >= 130u )
		goto st56;
	goto tr66;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr68;
	goto tr66;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( 176u <= (*p) )
		goto tr68;
	goto tr66;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 2383 "lexer2.c"
	switch( (*p) ) {
		case 35u: goto st82;
		case 62u: goto tr98;
		case 92u: goto st82;
		case 95u: goto st82;
		case 183u: goto st82;
		case 195u: goto st83;
		case 203u: goto st85;
		case 204u: goto st86;
		case 205u: goto st87;
		case 223u: goto st85;
		case 224u: goto st88;
		case 225u: goto st89;
		case 226u: goto st90;
		case 227u: goto st95;
		case 237u: goto st98;
		case 239u: goto st99;
		case 240u: goto st102;
		case 243u: goto st105;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st82;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st82;
		} else
			goto st82;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st84;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st104;
		} else
			goto st97;
	} else
		goto st82;
	goto tr231;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	switch( (*p) ) {
		case 35u: goto st82;
		case 62u: goto tr98;
		case 92u: goto st82;
		case 95u: goto st82;
		case 183u: goto st82;
		case 195u: goto st83;
		case 203u: goto st85;
		case 204u: goto st86;
		case 205u: goto st87;
		case 223u: goto st85;
		case 224u: goto st88;
		case 225u: goto st89;
		case 226u: goto st90;
		case 227u: goto st95;
		case 237u: goto st98;
		case 239u: goto st99;
		case 240u: goto st102;
		case 243u: goto st105;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st82;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st82;
		} else
			goto st82;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st84;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st104;
		} else
			goto st97;
	} else
		goto st82;
	goto tr0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st82;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st82;
	} else
		goto st82;
	goto tr0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	goto st82;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( 192u <= (*p) )
		goto tr0;
	goto st82;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( 128u <= (*p) )
		goto st82;
	goto tr0;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	if ( (*p) == 190u )
		goto tr0;
	goto st82;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 160u )
		goto st86;
	if ( 161u <= (*p) )
		goto st84;
	goto tr0;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( (*p) == 191u )
		goto st85;
	if ( 192u <= (*p) )
		goto tr0;
	goto st84;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	switch( (*p) ) {
		case 128u: goto st91;
		case 129u: goto st92;
		case 134u: goto st93;
		case 176u: goto st86;
		case 191u: goto st94;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st84;
	} else if ( (*p) >= 130u )
		goto st84;
	goto tr0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st82;
	} else if ( (*p) >= 140u )
		goto st82;
	goto tr0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st82;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( 144u <= (*p) )
		goto tr0;
	goto st82;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	if ( 176u <= (*p) )
		goto tr0;
	goto st82;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 128u )
		goto st96;
	if ( 129u <= (*p) )
		goto st84;
	goto tr0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( 129u <= (*p) )
		goto st82;
	goto tr0;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	goto st84;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) == 159u )
		goto st85;
	if ( 160u <= (*p) )
		goto tr0;
	goto st84;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	switch( (*p) ) {
		case 164u: goto st86;
		case 183u: goto st100;
		case 191u: goto st101;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st84;
	goto tr0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st82;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( 190u <= (*p) )
		goto tr0;
	goto st82;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) == 144u )
		goto st103;
	if ( 145u <= (*p) )
		goto st97;
	goto tr0;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( (*p) == 128u )
		goto st86;
	if ( 129u <= (*p) )
		goto st84;
	goto tr0;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	goto st97;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) == 175u )
		goto st89;
	if ( 176u <= (*p) )
		goto tr0;
	goto st97;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
	switch( (*p) ) {
		case 98u: goto tr253;
		case 112u: goto tr254;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr231;
tr252:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st191;
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st191;
tr264:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 105 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 2699 "lexer2.c"
	if ( (*p) == 45u )
		goto st106;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr61;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr122;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr61;
tr122:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 2731 "lexer2.c"
	if ( (*p) == 45u )
		goto st106;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr122;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr256;
tr253:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 2753 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 97u: goto tr257;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr257:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 2774 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 115u: goto tr258;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 2795 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 101u: goto tr259;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr254:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 2816 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 114u: goto tr260;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 2837 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 101u: goto tr261;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr261:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 2858 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 102u: goto tr262;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr262:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 2879 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 105u: goto tr263;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 2900 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 120u: goto tr264;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr150:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 108 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st201;
tr154:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 106 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st201;
tr187:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 117 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st201;
tr204:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st201;
tr211:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 145 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 2945 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st107;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st107;
	} else
		goto st107;
	goto tr61;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	goto st107;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( 192u <= (*p) )
		goto tr61;
	goto st107;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( 128u <= (*p) )
		goto st107;
	goto tr61;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 190u )
		goto tr61;
	goto st107;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( (*p) == 160u )
		goto st112;
	if ( 161u <= (*p) )
		goto st110;
	goto tr61;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( (*p) == 191u )
		goto st111;
	if ( 192u <= (*p) )
		goto tr61;
	goto st110;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	switch( (*p) ) {
		case 128u: goto st117;
		case 129u: goto st118;
		case 134u: goto st119;
		case 176u: goto st112;
		case 191u: goto st120;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st110;
	} else if ( (*p) >= 130u )
		goto st110;
	goto tr61;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st107;
	} else if ( (*p) >= 140u )
		goto st107;
	goto tr61;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto st107;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	if ( 144u <= (*p) )
		goto tr61;
	goto st107;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( 176u <= (*p) )
		goto tr61;
	goto st107;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	if ( (*p) == 128u )
		goto st122;
	if ( 129u <= (*p) )
		goto st110;
	goto tr61;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	if ( 129u <= (*p) )
		goto st107;
	goto tr61;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	goto st110;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) == 159u )
		goto st111;
	if ( 160u <= (*p) )
		goto tr61;
	goto st110;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	switch( (*p) ) {
		case 164u: goto st112;
		case 183u: goto st126;
		case 191u: goto st127;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st110;
	goto tr61;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto st107;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( 190u <= (*p) )
		goto tr61;
	goto st107;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	if ( (*p) == 144u )
		goto st129;
	if ( 145u <= (*p) )
		goto st123;
	goto tr61;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( (*p) == 128u )
		goto st112;
	if ( 129u <= (*p) )
		goto st110;
	goto tr61;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	goto st123;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) == 175u )
		goto st115;
	if ( 176u <= (*p) )
		goto tr61;
	goto st123;
tr205:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 3270 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 65u: goto st132;
		case 95u: goto st107;
		case 97u: goto st132;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 83u: goto st133;
		case 95u: goto st107;
		case 115u: goto st133;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 69u: goto tr150;
		case 95u: goto st107;
		case 101u: goto tr150;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr206:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 3409 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 82u: goto st134;
		case 95u: goto st107;
		case 114u: goto st134;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 69u: goto st135;
		case 95u: goto st107;
		case 101u: goto st135;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 70u: goto st136;
		case 95u: goto st107;
		case 102u: goto st136;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 73u: goto st137;
		case 95u: goto st107;
		case 105u: goto st137;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 88u: goto tr154;
		case 95u: goto st107;
		case 120u: goto tr154;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	if ( (*p) == 94u )
		goto tr267;
	goto tr231;
tr210:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 3643 "lexer2.c"
	if ( (*p) == 58u )
		goto st138;
	goto tr231;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	switch( (*p) ) {
		case 95u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 205u: goto st163;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st164;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 57u )
				goto tr155;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr155;
		} else
			goto tr155;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto st141;
	goto tr0;
tr155:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 110 "turtle.rl"
	{ shared_lexer->env.act = 5;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 3697 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto tr155;
		case 46u: goto st139;
		case 95u: goto tr155;
		case 183u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 204u: goto st143;
		case 205u: goto st144;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st147;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr155;
		} else if ( (*p) >= 48u )
			goto tr155;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto tr155;
	goto tr269;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	switch( (*p) ) {
		case 45u: goto tr155;
		case 46u: goto st139;
		case 95u: goto tr155;
		case 183u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 204u: goto st143;
		case 205u: goto st144;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st147;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr155;
		} else if ( (*p) >= 48u )
			goto tr155;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto tr155;
	goto tr170;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr155;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr155;
	} else
		goto tr155;
	goto tr61;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	goto tr155;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( 192u <= (*p) )
		goto tr61;
	goto tr155;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	if ( 128u <= (*p) )
		goto tr155;
	goto tr61;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( (*p) == 190u )
		goto tr170;
	goto tr155;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( (*p) == 160u )
		goto st143;
	if ( 161u <= (*p) )
		goto st141;
	goto tr61;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( (*p) == 191u )
		goto st142;
	if ( 192u <= (*p) )
		goto tr61;
	goto st141;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	switch( (*p) ) {
		case 128u: goto st148;
		case 129u: goto st149;
		case 134u: goto st150;
		case 176u: goto st143;
		case 191u: goto st151;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st141;
	} else if ( (*p) >= 130u )
		goto st141;
	goto tr170;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr155;
	} else if ( (*p) >= 140u )
		goto tr155;
	goto tr170;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr170;
	goto tr155;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	if ( 144u <= (*p) )
		goto tr61;
	goto tr155;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	if ( 176u <= (*p) )
		goto tr61;
	goto tr155;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	if ( (*p) == 128u )
		goto st153;
	if ( 129u <= (*p) )
		goto st141;
	goto tr61;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	if ( 129u <= (*p) )
		goto tr155;
	goto tr61;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	goto st141;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	if ( (*p) == 159u )
		goto st142;
	if ( 160u <= (*p) )
		goto tr61;
	goto st141;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 164u: goto st143;
		case 183u: goto st157;
		case 191u: goto st158;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st141;
	goto tr61;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto tr155;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	if ( 190u <= (*p) )
		goto tr61;
	goto tr155;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	if ( (*p) == 144u )
		goto st160;
	if ( 145u <= (*p) )
		goto st154;
	goto tr61;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	if ( (*p) == 128u )
		goto st143;
	if ( 129u <= (*p) )
		goto st141;
	goto tr61;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	goto st154;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	if ( (*p) == 175u )
		goto st146;
	if ( 176u <= (*p) )
		goto tr61;
	goto st154;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr155;
	} else if ( (*p) >= 176u )
		goto tr155;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 128u: goto st165;
		case 129u: goto st166;
		case 134u: goto st150;
		case 176u: goto st143;
		case 191u: goto st151;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st141;
	} else if ( (*p) >= 130u )
		goto st141;
	goto tr0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr155;
	goto tr0;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	if ( 176u <= (*p) )
		goto tr155;
	goto tr0;
tr212:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 4020 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 97u: goto st167;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 108u: goto st168;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 115u: goto st169;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 101u: goto tr187;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr213:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 4199 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 114u: goto st170;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 117u: goto st169;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr216:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 4292 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st107;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st107;
	} else
		goto st107;
	goto tr231;
tr217:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 4312 "lexer2.c"
	goto st107;
tr218:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 4324 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr231;
	goto st107;
tr219:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 4338 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st107;
	} else if ( (*p) >= 176u )
		goto st107;
	goto tr231;
tr220:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 4355 "lexer2.c"
	if ( (*p) == 160u )
		goto st112;
	if ( 161u <= (*p) )
		goto st110;
	goto tr231;
tr221:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 4371 "lexer2.c"
	if ( (*p) == 191u )
		goto st111;
	if ( 192u <= (*p) )
		goto tr231;
	goto st110;
tr222:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 4387 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st171;
		case 129u: goto st172;
		case 134u: goto st119;
		case 176u: goto st112;
		case 191u: goto st120;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st110;
	} else if ( (*p) >= 130u )
		goto st110;
	goto tr231;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st107;
	goto tr0;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	if ( 176u <= (*p) )
		goto st107;
	goto tr0;
tr223:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 4425 "lexer2.c"
	if ( (*p) == 128u )
		goto st122;
	if ( 129u <= (*p) )
		goto st110;
	goto tr231;
tr224:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 4441 "lexer2.c"
	goto st110;
tr225:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st218;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
#line 4453 "lexer2.c"
	if ( (*p) == 159u )
		goto st111;
	if ( 160u <= (*p) )
		goto tr231;
	goto st110;
tr226:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st219;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
#line 4469 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st112;
		case 183u: goto st126;
		case 191u: goto st127;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st110;
	goto tr231;
tr227:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st220;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
#line 4488 "lexer2.c"
	if ( (*p) == 144u )
		goto st129;
	if ( 145u <= (*p) )
		goto st123;
	goto tr231;
tr228:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st221;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
#line 4504 "lexer2.c"
	goto st123;
tr229:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st222;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
#line 4516 "lexer2.c"
	if ( (*p) == 175u )
		goto st115;
	if ( 176u <= (*p) )
		goto tr231;
	goto st123;
	}
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof0:  shared_lexer->env.cs = 0; goto _test_eof; 
	_test_eof1:  shared_lexer->env.cs = 1; goto _test_eof; 
	_test_eof2:  shared_lexer->env.cs = 2; goto _test_eof; 
	_test_eof3:  shared_lexer->env.cs = 3; goto _test_eof; 
	_test_eof4:  shared_lexer->env.cs = 4; goto _test_eof; 
	_test_eof5:  shared_lexer->env.cs = 5; goto _test_eof; 
	_test_eof6:  shared_lexer->env.cs = 6; goto _test_eof; 
	_test_eof7:  shared_lexer->env.cs = 7; goto _test_eof; 
	_test_eof8:  shared_lexer->env.cs = 8; goto _test_eof; 
	_test_eof9:  shared_lexer->env.cs = 9; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
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
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
	_test_eof43:  shared_lexer->env.cs = 43; goto _test_eof; 
	_test_eof44:  shared_lexer->env.cs = 44; goto _test_eof; 
	_test_eof45:  shared_lexer->env.cs = 45; goto _test_eof; 
	_test_eof46:  shared_lexer->env.cs = 46; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof47:  shared_lexer->env.cs = 47; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof48:  shared_lexer->env.cs = 48; goto _test_eof; 
	_test_eof49:  shared_lexer->env.cs = 49; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof50:  shared_lexer->env.cs = 50; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof51:  shared_lexer->env.cs = 51; goto _test_eof; 
	_test_eof52:  shared_lexer->env.cs = 52; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof53:  shared_lexer->env.cs = 53; goto _test_eof; 
	_test_eof54:  shared_lexer->env.cs = 54; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof61:  shared_lexer->env.cs = 61; goto _test_eof; 
	_test_eof62:  shared_lexer->env.cs = 62; goto _test_eof; 
	_test_eof63:  shared_lexer->env.cs = 63; goto _test_eof; 
	_test_eof64:  shared_lexer->env.cs = 64; goto _test_eof; 
	_test_eof65:  shared_lexer->env.cs = 65; goto _test_eof; 
	_test_eof66:  shared_lexer->env.cs = 66; goto _test_eof; 
	_test_eof67:  shared_lexer->env.cs = 67; goto _test_eof; 
	_test_eof68:  shared_lexer->env.cs = 68; goto _test_eof; 
	_test_eof69:  shared_lexer->env.cs = 69; goto _test_eof; 
	_test_eof70:  shared_lexer->env.cs = 70; goto _test_eof; 
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
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
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
	_test_eof100:  shared_lexer->env.cs = 100; goto _test_eof; 
	_test_eof101:  shared_lexer->env.cs = 101; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof103:  shared_lexer->env.cs = 103; goto _test_eof; 
	_test_eof104:  shared_lexer->env.cs = 104; goto _test_eof; 
	_test_eof105:  shared_lexer->env.cs = 105; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof106:  shared_lexer->env.cs = 106; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
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
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof135:  shared_lexer->env.cs = 135; goto _test_eof; 
	_test_eof136:  shared_lexer->env.cs = 136; goto _test_eof; 
	_test_eof137:  shared_lexer->env.cs = 137; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof138:  shared_lexer->env.cs = 138; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
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
	_test_eof150:  shared_lexer->env.cs = 150; goto _test_eof; 
	_test_eof151:  shared_lexer->env.cs = 151; goto _test_eof; 
	_test_eof152:  shared_lexer->env.cs = 152; goto _test_eof; 
	_test_eof153:  shared_lexer->env.cs = 153; goto _test_eof; 
	_test_eof154:  shared_lexer->env.cs = 154; goto _test_eof; 
	_test_eof155:  shared_lexer->env.cs = 155; goto _test_eof; 
	_test_eof156:  shared_lexer->env.cs = 156; goto _test_eof; 
	_test_eof157:  shared_lexer->env.cs = 157; goto _test_eof; 
	_test_eof158:  shared_lexer->env.cs = 158; goto _test_eof; 
	_test_eof159:  shared_lexer->env.cs = 159; goto _test_eof; 
	_test_eof160:  shared_lexer->env.cs = 160; goto _test_eof; 
	_test_eof161:  shared_lexer->env.cs = 161; goto _test_eof; 
	_test_eof162:  shared_lexer->env.cs = 162; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 
	_test_eof218:  shared_lexer->env.cs = 218; goto _test_eof; 
	_test_eof219:  shared_lexer->env.cs = 219; goto _test_eof; 
	_test_eof220:  shared_lexer->env.cs = 220; goto _test_eof; 
	_test_eof221:  shared_lexer->env.cs = 221; goto _test_eof; 
	_test_eof222:  shared_lexer->env.cs = 222; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 174: goto tr61;
	case 175: goto tr230;
	case 176: goto tr231;
	case 0: goto tr0;
	case 1: goto tr0;
	case 2: goto tr0;
	case 3: goto tr0;
	case 4: goto tr0;
	case 5: goto tr0;
	case 6: goto tr0;
	case 7: goto tr0;
	case 8: goto tr0;
	case 9: goto tr0;
	case 177: goto tr235;
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
	case 178: goto tr231;
	case 23: goto tr0;
	case 179: goto tr237;
	case 180: goto tr231;
	case 24: goto tr0;
	case 25: goto tr0;
	case 26: goto tr0;
	case 27: goto tr0;
	case 28: goto tr0;
	case 29: goto tr0;
	case 30: goto tr0;
	case 31: goto tr0;
	case 32: goto tr0;
	case 33: goto tr0;
	case 181: goto tr241;
	case 34: goto tr43;
	case 35: goto tr43;
	case 36: goto tr43;
	case 37: goto tr43;
	case 38: goto tr43;
	case 39: goto tr43;
	case 40: goto tr43;
	case 41: goto tr43;
	case 42: goto tr43;
	case 43: goto tr43;
	case 44: goto tr43;
	case 45: goto tr43;
	case 46: goto tr43;
	case 182: goto tr231;
	case 47: goto tr0;
	case 183: goto tr244;
	case 48: goto tr61;
	case 49: goto tr61;
	case 184: goto tr245;
	case 185: goto tr246;
	case 50: goto tr64;
	case 186: goto tr248;
	case 187: goto tr249;
	case 51: goto tr66;
	case 52: goto tr66;
	case 188: goto tr249;
	case 53: goto tr66;
	case 54: goto tr66;
	case 55: goto tr66;
	case 56: goto tr66;
	case 57: goto tr66;
	case 58: goto tr66;
	case 59: goto tr66;
	case 60: goto tr66;
	case 61: goto tr66;
	case 62: goto tr66;
	case 63: goto tr66;
	case 64: goto tr66;
	case 65: goto tr66;
	case 66: goto tr66;
	case 67: goto tr66;
	case 68: goto tr66;
	case 69: goto tr66;
	case 70: goto tr66;
	case 71: goto tr66;
	case 72: goto tr66;
	case 73: goto tr66;
	case 74: goto tr66;
	case 75: goto tr66;
	case 76: goto tr66;
	case 77: goto tr66;
	case 78: goto tr66;
	case 79: goto tr66;
	case 80: goto tr66;
	case 81: goto tr66;
	case 189: goto tr231;
	case 82: goto tr0;
	case 83: goto tr0;
	case 84: goto tr0;
	case 85: goto tr0;
	case 86: goto tr0;
	case 87: goto tr0;
	case 88: goto tr0;
	case 89: goto tr0;
	case 90: goto tr0;
	case 91: goto tr0;
	case 92: goto tr0;
	case 93: goto tr0;
	case 94: goto tr0;
	case 95: goto tr0;
	case 96: goto tr0;
	case 97: goto tr0;
	case 98: goto tr0;
	case 99: goto tr0;
	case 100: goto tr0;
	case 101: goto tr0;
	case 102: goto tr0;
	case 103: goto tr0;
	case 104: goto tr0;
	case 105: goto tr0;
	case 190: goto tr231;
	case 191: goto tr61;
	case 106: goto tr61;
	case 192: goto tr256;
	case 193: goto tr256;
	case 194: goto tr256;
	case 195: goto tr256;
	case 196: goto tr256;
	case 197: goto tr256;
	case 198: goto tr256;
	case 199: goto tr256;
	case 200: goto tr256;
	case 201: goto tr61;
	case 107: goto tr61;
	case 108: goto tr61;
	case 109: goto tr61;
	case 110: goto tr61;
	case 111: goto tr61;
	case 112: goto tr61;
	case 113: goto tr61;
	case 114: goto tr61;
	case 115: goto tr61;
	case 116: goto tr61;
	case 117: goto tr61;
	case 118: goto tr61;
	case 119: goto tr61;
	case 120: goto tr61;
	case 121: goto tr61;
	case 122: goto tr61;
	case 123: goto tr61;
	case 124: goto tr61;
	case 125: goto tr61;
	case 126: goto tr61;
	case 127: goto tr61;
	case 128: goto tr61;
	case 129: goto tr61;
	case 130: goto tr61;
	case 131: goto tr61;
	case 202: goto tr231;
	case 132: goto tr0;
	case 133: goto tr0;
	case 203: goto tr231;
	case 134: goto tr0;
	case 135: goto tr0;
	case 136: goto tr0;
	case 137: goto tr0;
	case 204: goto tr231;
	case 205: goto tr231;
	case 138: goto tr0;
	case 206: goto tr269;
	case 139: goto tr170;
	case 140: goto tr61;
	case 141: goto tr61;
	case 142: goto tr61;
	case 143: goto tr61;
	case 144: goto tr170;
	case 145: goto tr61;
	case 146: goto tr61;
	case 147: goto tr170;
	case 148: goto tr170;
	case 149: goto tr170;
	case 150: goto tr61;
	case 151: goto tr61;
	case 152: goto tr61;
	case 153: goto tr61;
	case 154: goto tr61;
	case 155: goto tr61;
	case 156: goto tr61;
	case 157: goto tr61;
	case 158: goto tr61;
	case 159: goto tr61;
	case 160: goto tr61;
	case 161: goto tr61;
	case 162: goto tr61;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 166: goto tr0;
	case 207: goto tr231;
	case 167: goto tr0;
	case 168: goto tr0;
	case 169: goto tr0;
	case 208: goto tr231;
	case 170: goto tr0;
	case 209: goto tr231;
	case 210: goto tr231;
	case 211: goto tr231;
	case 212: goto tr231;
	case 213: goto tr231;
	case 214: goto tr231;
	case 215: goto tr231;
	case 171: goto tr0;
	case 172: goto tr0;
	case 216: goto tr231;
	case 217: goto tr231;
	case 218: goto tr231;
	case 219: goto tr231;
	case 220: goto tr231;
	case 221: goto tr231;
	case 222: goto tr231;
	}
	}

	}

#line 370 "turtle.rl"
    /* clang-format on */

    return ARDP_SUCCESS;
}
/*}}}*/
/* ardp_lexer_process_block() OLD using block {{{*/
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
          
#line 5009 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 164 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st173;
tr2:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 128 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr12:
#line 128 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr20:
#line 119 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("<<< %s >>>\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st173;
tr31:
#line 163 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st173;
tr33:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 132 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr43:
#line 132 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr51:
#line 123 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                printf("--- %s ---\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        }}
	goto st173;
tr61:
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
	case 5:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }
	break;
	case 8:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 9:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 11:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); }
	break;
	case 17:
	{{p = (( shared_lexer->env.te))-1;}
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }
	break;
	case 18:
	{{p = (( shared_lexer->env.te))-1;} lexer_emit_token_const(A); }
	break;
	case 28:
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
	goto st173;
tr64:
#line 114 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr66:
#line 111 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st173;
tr98:
#line 112 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1)); }}
	goto st173;
tr170:
#line 110 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st173;
tr188:
#line 164 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st173;
tr195:
#line 152 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st173;
tr196:
#line 153 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st173;
tr198:
#line 147 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st173;
tr201:
#line 148 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st173;
tr207:
#line 150 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st173;
tr208:
#line 151 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st173;
tr214:
#line 154 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st173;
tr215:
#line 155 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st173;
tr230:
#line 157 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st173;
tr231:
#line 164 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st173;
tr235:
#line 128 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
                printf("=== %s ===\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr237:
#line 163 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st173;
tr241:
#line 132 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
                printf("??? %s ???\n", shared_lexer->string);
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st173;
tr244:
#line 115 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr245:
#line 116 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr246:
#line 114 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st173;
tr248:
#line 146 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st173;
tr249:
#line 111 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st173;
tr256:
#line 140 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st173;
tr267:
#line 137 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st173;
tr269:
#line 110 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st173;
st173:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 5253 "lexer2.c"
	switch( (*p) ) {
		case 9u: goto tr189;
		case 10u: goto st175;
		case 13u: goto tr191;
		case 32u: goto tr189;
		case 34u: goto tr192;
		case 35u: goto tr193;
		case 39u: goto tr194;
		case 40u: goto tr195;
		case 41u: goto tr196;
		case 44u: goto tr198;
		case 46u: goto st186;
		case 58u: goto tr125;
		case 59u: goto tr201;
		case 60u: goto tr202;
		case 64u: goto st190;
		case 66u: goto tr205;
		case 80u: goto tr206;
		case 91u: goto tr207;
		case 93u: goto tr208;
		case 94u: goto st204;
		case 95u: goto tr210;
		case 97u: goto tr211;
		case 98u: goto tr205;
		case 102u: goto tr212;
		case 112u: goto tr206;
		case 116u: goto tr213;
		case 123u: goto tr214;
		case 125u: goto tr215;
		case 195u: goto tr216;
		case 203u: goto tr218;
		case 205u: goto tr219;
		case 223u: goto tr218;
		case 224u: goto tr220;
		case 225u: goto tr221;
		case 226u: goto tr222;
		case 227u: goto tr223;
		case 237u: goto tr225;
		case 239u: goto tr226;
		case 240u: goto tr227;
		case 243u: goto tr229;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr197;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr204;
		} else
			goto tr200;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr217;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr228;
			} else if ( (*p) >= 228u )
				goto tr224;
		} else
			goto tr217;
	} else
		goto tr204;
	goto tr188;
tr189:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 163 "turtle.rl"
	{ shared_lexer->env.act = 29;}
	goto st174;
tr191:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 157 "turtle.rl"
	{ shared_lexer->env.act = 28;}
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 5336 "lexer2.c"
	switch( (*p) ) {
		case 13u: goto tr189;
		case 32u: goto tr189;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr189;
	goto tr61;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
	switch( (*p) ) {
		case 13u: goto tr191;
		case 32u: goto tr189;
	}
	if ( 9u <= (*p) && (*p) <= 10u )
		goto tr189;
	goto tr230;
tr192:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 5363 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr231;
		case 13u: goto tr231;
		case 34u: goto tr233;
		case 39u: goto tr231;
		case 92u: goto tr234;
	}
	goto tr232;
tr1:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr232:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
#line 5399 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto tr3;
	}
	goto tr1;
tr3:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st1;
tr234:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 5435 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr1;
		case 39u: goto tr1;
		case 85u: goto tr4;
		case 92u: goto tr1;
		case 98u: goto tr1;
		case 102u: goto tr1;
		case 110u: goto tr1;
		case 114u: goto tr1;
		case 116u: goto tr1;
		case 117u: goto tr5;
	}
	goto tr0;
tr4:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 5462 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr6;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr6;
	} else
		goto tr6;
	goto tr0;
tr6:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 5485 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr7;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr7;
	} else
		goto tr7;
	goto tr0;
tr7:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 5508 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr8;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr8;
	} else
		goto tr8;
	goto tr0;
tr8:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 5531 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr5;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr5;
	} else
		goto tr5;
	goto tr0;
tr5:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 5554 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr9;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr9;
	} else
		goto tr9;
	goto tr0;
tr9:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 5577 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr10;
	} else
		goto tr10;
	goto tr0;
tr10:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 5600 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr11;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr11;
	} else
		goto tr11;
	goto tr0;
tr11:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 5623 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr1;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr1;
	} else
		goto tr1;
	goto tr0;
tr233:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 5652 "lexer2.c"
	if ( (*p) == 34u )
		goto st10;
	goto tr235;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 34u: goto tr14;
		case 92u: goto tr15;
	}
	goto tr13;
tr16:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st11;
tr13:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 5692 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr17;
		case 92u: goto tr18;
	}
	goto tr16;
tr14:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st12;
tr17:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 5723 "lexer2.c"
	if ( (*p) == 34u )
		goto st13;
	goto tr12;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 34u )
		goto tr20;
	goto tr12;
tr18:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st14;
tr15:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 5761 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr16;
		case 39u: goto tr16;
		case 85u: goto tr21;
		case 92u: goto tr16;
		case 98u: goto tr16;
		case 102u: goto tr16;
		case 110u: goto tr16;
		case 114u: goto tr16;
		case 116u: goto tr16;
		case 117u: goto tr22;
	}
	goto tr12;
tr21:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 5788 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr23;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr23;
	} else
		goto tr23;
	goto tr12;
tr23:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 5811 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr24;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr24;
	} else
		goto tr24;
	goto tr12;
tr24:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 5834 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr25;
	} else
		goto tr25;
	goto tr12;
tr25:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 5857 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr22;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr22;
	} else
		goto tr22;
	goto tr12;
tr22:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 5880 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr26;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr26;
	} else
		goto tr26;
	goto tr12;
tr26:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
#line 5903 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr27;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr27;
	} else
		goto tr27;
	goto tr12;
tr27:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 5926 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr28;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr28;
	} else
		goto tr28;
	goto tr12;
tr28:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 5949 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr16;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr16;
	} else
		goto tr16;
	goto tr12;
tr193:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 5967 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st179;
		case 13u: goto tr31;
	}
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 10u: goto st179;
		case 13u: goto tr31;
	}
	goto st23;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	if ( (*p) == 13u )
		goto tr31;
	goto tr237;
tr194:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 5997 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr231;
		case 13u: goto tr231;
		case 34u: goto tr231;
		case 39u: goto tr239;
		case 92u: goto tr240;
	}
	goto tr238;
tr32:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st24;
tr238:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 6033 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr33;
		case 92u: goto tr34;
	}
	goto tr32;
tr34:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st25;
tr240:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 6069 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr32;
		case 39u: goto tr32;
		case 85u: goto tr35;
		case 92u: goto tr32;
		case 98u: goto tr32;
		case 102u: goto tr32;
		case 110u: goto tr32;
		case 114u: goto tr32;
		case 116u: goto tr32;
		case 117u: goto tr36;
	}
	goto tr0;
tr35:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 6096 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr37;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr37;
	} else
		goto tr37;
	goto tr0;
tr37:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 6119 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr38;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr38;
	} else
		goto tr38;
	goto tr0;
tr38:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 6142 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr39;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr39;
	} else
		goto tr39;
	goto tr0;
tr39:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 6165 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr36;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr36;
	} else
		goto tr36;
	goto tr0;
tr36:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 6188 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr40;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr40;
	} else
		goto tr40;
	goto tr0;
tr40:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 6211 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr41;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr41;
	} else
		goto tr41;
	goto tr0;
tr41:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 6234 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr42;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr42;
	} else
		goto tr42;
	goto tr0;
tr42:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 6257 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr32;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr32;
	} else
		goto tr32;
	goto tr0;
tr239:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st181;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
#line 6286 "lexer2.c"
	if ( (*p) == 39u )
		goto st34;
	goto tr241;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 39u: goto tr45;
		case 92u: goto tr46;
	}
	goto tr44;
tr47:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st35;
tr44:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 6326 "lexer2.c"
	switch( (*p) ) {
		case 39u: goto tr48;
		case 92u: goto tr49;
	}
	goto tr47;
tr45:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st36;
tr48:
#line 63 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 6357 "lexer2.c"
	if ( (*p) == 39u )
		goto st37;
	goto tr43;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	if ( (*p) == 39u )
		goto tr51;
	goto tr43;
tr49:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st38;
tr46:
#line 58 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 6395 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr47;
		case 39u: goto tr47;
		case 85u: goto tr52;
		case 92u: goto tr47;
		case 98u: goto tr47;
		case 102u: goto tr47;
		case 110u: goto tr47;
		case 114u: goto tr47;
		case 116u: goto tr47;
		case 117u: goto tr53;
	}
	goto tr43;
tr52:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 6422 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr54;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr54;
	} else
		goto tr54;
	goto tr43;
tr54:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 6445 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr55;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr55;
	} else
		goto tr55;
	goto tr43;
tr55:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 6468 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr56;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr56;
	} else
		goto tr56;
	goto tr43;
tr56:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 6491 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr53;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr53;
	} else
		goto tr53;
	goto tr43;
tr53:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 6514 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr57;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr57;
	} else
		goto tr57;
	goto tr43;
tr57:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 6537 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr58;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr58;
	} else
		goto tr58;
	goto tr43;
tr58:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st45;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
#line 6560 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr59;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr59;
	} else
		goto tr59;
	goto tr43;
tr59:
#line 69 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 6583 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr47;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr47;
	} else
		goto tr47;
	goto tr43;
tr197:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 6601 "lexer2.c"
	if ( (*p) == 46u )
		goto st47;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr200;
	goto tr231;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr0;
tr60:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 115 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 6624 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr244;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	switch( (*p) ) {
		case 43u: goto st49;
		case 45u: goto st49;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr61;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr61;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st184;
	goto tr245;
tr200:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 114 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 6667 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st50;
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr200;
	goto tr246;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 69u: goto st48;
		case 101u: goto st48;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr64;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr60;
	goto tr248;
tr125:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 6702 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st51;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 205u: goto st78;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st79;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr68;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr68;
		} else
			goto tr68;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto st56;
	goto tr249;
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
	goto tr66;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr68;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr68;
	} else
		goto tr68;
	goto tr66;
tr68:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 6775 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st51;
		case 45u: goto tr68;
		case 46u: goto st53;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 183u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 204u: goto st58;
		case 205u: goto st59;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st62;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr68;
		} else if ( (*p) >= 48u )
			goto tr68;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto tr68;
	goto tr249;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 37u: goto st51;
		case 45u: goto tr68;
		case 46u: goto st53;
		case 92u: goto st54;
		case 95u: goto tr68;
		case 183u: goto tr68;
		case 195u: goto st55;
		case 203u: goto st57;
		case 204u: goto st58;
		case 205u: goto st59;
		case 223u: goto st57;
		case 224u: goto st60;
		case 225u: goto st61;
		case 226u: goto st62;
		case 227u: goto st67;
		case 237u: goto st70;
		case 239u: goto st71;
		case 240u: goto st74;
		case 243u: goto st77;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr68;
		} else if ( (*p) >= 48u )
			goto tr68;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st56;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st76;
		} else
			goto st69;
	} else
		goto tr68;
	goto tr66;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 33u: goto tr68;
		case 59u: goto tr68;
		case 61u: goto tr68;
		case 95u: goto tr68;
		case 126u: goto tr68;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr68;
	} else if ( (*p) >= 35u )
		goto tr68;
	goto tr66;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr68;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr68;
	} else
		goto tr68;
	goto tr66;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	goto tr68;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 192u <= (*p) )
		goto tr66;
	goto tr68;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( 128u <= (*p) )
		goto tr68;
	goto tr66;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) == 190u )
		goto tr66;
	goto tr68;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) == 160u )
		goto st58;
	if ( 161u <= (*p) )
		goto st56;
	goto tr66;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) == 191u )
		goto st57;
	if ( 192u <= (*p) )
		goto tr66;
	goto st56;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 128u: goto st63;
		case 129u: goto st64;
		case 134u: goto st65;
		case 176u: goto st58;
		case 191u: goto st66;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st56;
	} else if ( (*p) >= 130u )
		goto st56;
	goto tr66;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr68;
	} else if ( (*p) >= 140u )
		goto tr68;
	goto tr66;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr66;
	goto tr68;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 144u <= (*p) )
		goto tr66;
	goto tr68;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( 176u <= (*p) )
		goto tr66;
	goto tr68;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) == 128u )
		goto st68;
	if ( 129u <= (*p) )
		goto st56;
	goto tr66;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( 129u <= (*p) )
		goto tr68;
	goto tr66;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	goto st56;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) == 159u )
		goto st57;
	if ( 160u <= (*p) )
		goto tr66;
	goto st56;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	switch( (*p) ) {
		case 164u: goto st58;
		case 183u: goto st72;
		case 191u: goto st73;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st56;
	goto tr66;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr66;
	goto tr68;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( 190u <= (*p) )
		goto tr66;
	goto tr68;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( (*p) == 144u )
		goto st75;
	if ( 145u <= (*p) )
		goto st69;
	goto tr66;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 128u )
		goto st58;
	if ( 129u <= (*p) )
		goto st56;
	goto tr66;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	goto st69;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	if ( (*p) == 175u )
		goto st61;
	if ( 176u <= (*p) )
		goto tr66;
	goto st69;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr68;
	} else if ( (*p) >= 176u )
		goto tr68;
	goto tr66;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 128u: goto st80;
		case 129u: goto st81;
		case 134u: goto st65;
		case 176u: goto st58;
		case 191u: goto st66;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st56;
	} else if ( (*p) >= 130u )
		goto st56;
	goto tr66;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr68;
	goto tr66;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( 176u <= (*p) )
		goto tr68;
	goto tr66;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 7117 "lexer2.c"
	switch( (*p) ) {
		case 35u: goto st82;
		case 62u: goto tr98;
		case 92u: goto st82;
		case 95u: goto st82;
		case 183u: goto st82;
		case 195u: goto st83;
		case 203u: goto st85;
		case 204u: goto st86;
		case 205u: goto st87;
		case 223u: goto st85;
		case 224u: goto st88;
		case 225u: goto st89;
		case 226u: goto st90;
		case 227u: goto st95;
		case 237u: goto st98;
		case 239u: goto st99;
		case 240u: goto st102;
		case 243u: goto st105;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st82;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st82;
		} else
			goto st82;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st84;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st104;
		} else
			goto st97;
	} else
		goto st82;
	goto tr231;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	switch( (*p) ) {
		case 35u: goto st82;
		case 62u: goto tr98;
		case 92u: goto st82;
		case 95u: goto st82;
		case 183u: goto st82;
		case 195u: goto st83;
		case 203u: goto st85;
		case 204u: goto st86;
		case 205u: goto st87;
		case 223u: goto st85;
		case 224u: goto st88;
		case 225u: goto st89;
		case 226u: goto st90;
		case 227u: goto st95;
		case 237u: goto st98;
		case 239u: goto st99;
		case 240u: goto st102;
		case 243u: goto st105;
	}
	if ( (*p) < 97u ) {
		if ( (*p) < 45u ) {
			if ( 37u <= (*p) && (*p) <= 38u )
				goto st82;
		} else if ( (*p) > 58u ) {
			if ( 64u <= (*p) && (*p) <= 90u )
				goto st82;
		} else
			goto st82;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st84;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st104;
		} else
			goto st97;
	} else
		goto st82;
	goto tr0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st82;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st82;
	} else
		goto st82;
	goto tr0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	goto st82;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( 192u <= (*p) )
		goto tr0;
	goto st82;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( 128u <= (*p) )
		goto st82;
	goto tr0;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	if ( (*p) == 190u )
		goto tr0;
	goto st82;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( (*p) == 160u )
		goto st86;
	if ( 161u <= (*p) )
		goto st84;
	goto tr0;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( (*p) == 191u )
		goto st85;
	if ( 192u <= (*p) )
		goto tr0;
	goto st84;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
	switch( (*p) ) {
		case 128u: goto st91;
		case 129u: goto st92;
		case 134u: goto st93;
		case 176u: goto st86;
		case 191u: goto st94;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st84;
	} else if ( (*p) >= 130u )
		goto st84;
	goto tr0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st82;
	} else if ( (*p) >= 140u )
		goto st82;
	goto tr0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st82;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	if ( 144u <= (*p) )
		goto tr0;
	goto st82;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
	if ( 176u <= (*p) )
		goto tr0;
	goto st82;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) == 128u )
		goto st96;
	if ( 129u <= (*p) )
		goto st84;
	goto tr0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( 129u <= (*p) )
		goto st82;
	goto tr0;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	goto st84;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) == 159u )
		goto st85;
	if ( 160u <= (*p) )
		goto tr0;
	goto st84;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	switch( (*p) ) {
		case 164u: goto st86;
		case 183u: goto st100;
		case 191u: goto st101;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st84;
	goto tr0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto st82;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( 190u <= (*p) )
		goto tr0;
	goto st82;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) == 144u )
		goto st103;
	if ( 145u <= (*p) )
		goto st97;
	goto tr0;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( (*p) == 128u )
		goto st86;
	if ( 129u <= (*p) )
		goto st84;
	goto tr0;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	goto st97;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) == 175u )
		goto st89;
	if ( 176u <= (*p) )
		goto tr0;
	goto st97;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
	switch( (*p) ) {
		case 98u: goto tr253;
		case 112u: goto tr254;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr231;
tr252:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st191;
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 107 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st191;
tr264:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 105 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 7433 "lexer2.c"
	if ( (*p) == 45u )
		goto st106;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr61;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr122;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr61;
tr122:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 7465 "lexer2.c"
	if ( (*p) == 45u )
		goto st106;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr122;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr122;
	} else
		goto tr122;
	goto tr256;
tr253:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 7487 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 97u: goto tr257;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr257:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 7508 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 115u: goto tr258;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 7529 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 101u: goto tr259;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr254:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 7550 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 114u: goto tr260;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 7571 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 101u: goto tr261;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr261:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 7592 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 102u: goto tr262;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr262:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 7613 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 105u: goto tr263;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 140 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 7634 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st106;
		case 120u: goto tr264;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr252;
	} else if ( (*p) >= 65u )
		goto tr252;
	goto tr256;
tr150:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 108 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st201;
tr154:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 106 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st201;
tr187:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 117 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st201;
tr204:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st201;
tr211:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 145 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 7679 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 95u: goto st107;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr61;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st107;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st107;
	} else
		goto st107;
	goto tr61;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	goto st107;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( 192u <= (*p) )
		goto tr61;
	goto st107;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( 128u <= (*p) )
		goto st107;
	goto tr61;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 190u )
		goto tr61;
	goto st107;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( (*p) == 160u )
		goto st112;
	if ( 161u <= (*p) )
		goto st110;
	goto tr61;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( (*p) == 191u )
		goto st111;
	if ( 192u <= (*p) )
		goto tr61;
	goto st110;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	switch( (*p) ) {
		case 128u: goto st117;
		case 129u: goto st118;
		case 134u: goto st119;
		case 176u: goto st112;
		case 191u: goto st120;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st110;
	} else if ( (*p) >= 130u )
		goto st110;
	goto tr61;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st107;
	} else if ( (*p) >= 140u )
		goto st107;
	goto tr61;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto st107;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	if ( 144u <= (*p) )
		goto tr61;
	goto st107;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( 176u <= (*p) )
		goto tr61;
	goto st107;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	if ( (*p) == 128u )
		goto st122;
	if ( 129u <= (*p) )
		goto st110;
	goto tr61;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	if ( 129u <= (*p) )
		goto st107;
	goto tr61;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	goto st110;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) == 159u )
		goto st111;
	if ( 160u <= (*p) )
		goto tr61;
	goto st110;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	switch( (*p) ) {
		case 164u: goto st112;
		case 183u: goto st126;
		case 191u: goto st127;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st110;
	goto tr61;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto st107;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( 190u <= (*p) )
		goto tr61;
	goto st107;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	if ( (*p) == 144u )
		goto st129;
	if ( 145u <= (*p) )
		goto st123;
	goto tr61;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	if ( (*p) == 128u )
		goto st112;
	if ( 129u <= (*p) )
		goto st110;
	goto tr61;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	goto st123;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	if ( (*p) == 175u )
		goto st115;
	if ( 176u <= (*p) )
		goto tr61;
	goto st123;
tr205:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 8004 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 65u: goto st132;
		case 95u: goto st107;
		case 97u: goto st132;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 83u: goto st133;
		case 95u: goto st107;
		case 115u: goto st133;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 69u: goto tr150;
		case 95u: goto st107;
		case 101u: goto tr150;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr206:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 8143 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 82u: goto st134;
		case 95u: goto st107;
		case 114u: goto st134;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 69u: goto st135;
		case 95u: goto st107;
		case 101u: goto st135;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 70u: goto st136;
		case 95u: goto st107;
		case 102u: goto st136;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 73u: goto st137;
		case 95u: goto st107;
		case 105u: goto st137;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 88u: goto tr154;
		case 95u: goto st107;
		case 120u: goto tr154;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	if ( (*p) == 94u )
		goto tr267;
	goto tr231;
tr210:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 8377 "lexer2.c"
	if ( (*p) == 58u )
		goto st138;
	goto tr231;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	switch( (*p) ) {
		case 95u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 205u: goto st163;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st164;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 57u )
				goto tr155;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr155;
		} else
			goto tr155;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto st141;
	goto tr0;
tr155:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 110 "turtle.rl"
	{ shared_lexer->env.act = 5;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 8431 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto tr155;
		case 46u: goto st139;
		case 95u: goto tr155;
		case 183u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 204u: goto st143;
		case 205u: goto st144;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st147;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr155;
		} else if ( (*p) >= 48u )
			goto tr155;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto tr155;
	goto tr269;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	switch( (*p) ) {
		case 45u: goto tr155;
		case 46u: goto st139;
		case 95u: goto tr155;
		case 183u: goto tr155;
		case 195u: goto st140;
		case 203u: goto st142;
		case 204u: goto st143;
		case 205u: goto st144;
		case 223u: goto st142;
		case 224u: goto st145;
		case 225u: goto st146;
		case 226u: goto st147;
		case 227u: goto st152;
		case 237u: goto st155;
		case 239u: goto st156;
		case 240u: goto st159;
		case 243u: goto st162;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr155;
		} else if ( (*p) >= 48u )
			goto tr155;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st141;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st161;
		} else
			goto st154;
	} else
		goto tr155;
	goto tr170;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr155;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr155;
	} else
		goto tr155;
	goto tr61;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	goto tr155;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( 192u <= (*p) )
		goto tr61;
	goto tr155;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	if ( 128u <= (*p) )
		goto tr155;
	goto tr61;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( (*p) == 190u )
		goto tr170;
	goto tr155;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( (*p) == 160u )
		goto st143;
	if ( 161u <= (*p) )
		goto st141;
	goto tr61;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( (*p) == 191u )
		goto st142;
	if ( 192u <= (*p) )
		goto tr61;
	goto st141;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	switch( (*p) ) {
		case 128u: goto st148;
		case 129u: goto st149;
		case 134u: goto st150;
		case 176u: goto st143;
		case 191u: goto st151;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st141;
	} else if ( (*p) >= 130u )
		goto st141;
	goto tr170;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr155;
	} else if ( (*p) >= 140u )
		goto tr155;
	goto tr170;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr170;
	goto tr155;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	if ( 144u <= (*p) )
		goto tr61;
	goto tr155;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	if ( 176u <= (*p) )
		goto tr61;
	goto tr155;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	if ( (*p) == 128u )
		goto st153;
	if ( 129u <= (*p) )
		goto st141;
	goto tr61;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	if ( 129u <= (*p) )
		goto tr155;
	goto tr61;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	goto st141;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	if ( (*p) == 159u )
		goto st142;
	if ( 160u <= (*p) )
		goto tr61;
	goto st141;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	switch( (*p) ) {
		case 164u: goto st143;
		case 183u: goto st157;
		case 191u: goto st158;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st141;
	goto tr61;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr61;
	goto tr155;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	if ( 190u <= (*p) )
		goto tr61;
	goto tr155;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	if ( (*p) == 144u )
		goto st160;
	if ( 145u <= (*p) )
		goto st154;
	goto tr61;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	if ( (*p) == 128u )
		goto st143;
	if ( 129u <= (*p) )
		goto st141;
	goto tr61;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	goto st154;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	if ( (*p) == 175u )
		goto st146;
	if ( 176u <= (*p) )
		goto tr61;
	goto st154;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr155;
	} else if ( (*p) >= 176u )
		goto tr155;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 128u: goto st165;
		case 129u: goto st166;
		case 134u: goto st150;
		case 176u: goto st143;
		case 191u: goto st151;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st141;
	} else if ( (*p) >= 130u )
		goto st141;
	goto tr0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr155;
	goto tr0;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	if ( 176u <= (*p) )
		goto tr155;
	goto tr0;
tr212:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 8754 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 97u: goto st167;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 108u: goto st168;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 115u: goto st169;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 101u: goto tr187;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr213:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 8933 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 114u: goto st170;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr231;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	switch( (*p) ) {
		case 45u: goto st107;
		case 46u: goto st108;
		case 58u: goto tr125;
		case 95u: goto st107;
		case 117u: goto st169;
		case 183u: goto st107;
		case 195u: goto st109;
		case 203u: goto st111;
		case 204u: goto st112;
		case 205u: goto st113;
		case 223u: goto st111;
		case 224u: goto st114;
		case 225u: goto st115;
		case 226u: goto st116;
		case 227u: goto st121;
		case 237u: goto st124;
		case 239u: goto st125;
		case 240u: goto st128;
		case 243u: goto st131;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st107;
		} else if ( (*p) >= 48u )
			goto st107;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st110;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st130;
		} else
			goto st123;
	} else
		goto st107;
	goto tr0;
tr216:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 9026 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st107;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st107;
	} else
		goto st107;
	goto tr231;
tr217:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 9046 "lexer2.c"
	goto st107;
tr218:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 9058 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr231;
	goto st107;
tr219:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 9072 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st107;
	} else if ( (*p) >= 176u )
		goto st107;
	goto tr231;
tr220:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 9089 "lexer2.c"
	if ( (*p) == 160u )
		goto st112;
	if ( 161u <= (*p) )
		goto st110;
	goto tr231;
tr221:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 9105 "lexer2.c"
	if ( (*p) == 191u )
		goto st111;
	if ( 192u <= (*p) )
		goto tr231;
	goto st110;
tr222:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 9121 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st171;
		case 129u: goto st172;
		case 134u: goto st119;
		case 176u: goto st112;
		case 191u: goto st120;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st110;
	} else if ( (*p) >= 130u )
		goto st110;
	goto tr231;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st107;
	goto tr0;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
	if ( 176u <= (*p) )
		goto st107;
	goto tr0;
tr223:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 9159 "lexer2.c"
	if ( (*p) == 128u )
		goto st122;
	if ( 129u <= (*p) )
		goto st110;
	goto tr231;
tr224:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 9175 "lexer2.c"
	goto st110;
tr225:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st218;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
#line 9187 "lexer2.c"
	if ( (*p) == 159u )
		goto st111;
	if ( 160u <= (*p) )
		goto tr231;
	goto st110;
tr226:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st219;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
#line 9203 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st112;
		case 183u: goto st126;
		case 191u: goto st127;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st110;
	goto tr231;
tr227:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st220;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
#line 9222 "lexer2.c"
	if ( (*p) == 144u )
		goto st129;
	if ( 145u <= (*p) )
		goto st123;
	goto tr231;
tr228:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st221;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
#line 9238 "lexer2.c"
	goto st123;
tr229:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 164 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st222;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
#line 9250 "lexer2.c"
	if ( (*p) == 175u )
		goto st115;
	if ( 176u <= (*p) )
		goto tr231;
	goto st123;
	}
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof0:  shared_lexer->env.cs = 0; goto _test_eof; 
	_test_eof1:  shared_lexer->env.cs = 1; goto _test_eof; 
	_test_eof2:  shared_lexer->env.cs = 2; goto _test_eof; 
	_test_eof3:  shared_lexer->env.cs = 3; goto _test_eof; 
	_test_eof4:  shared_lexer->env.cs = 4; goto _test_eof; 
	_test_eof5:  shared_lexer->env.cs = 5; goto _test_eof; 
	_test_eof6:  shared_lexer->env.cs = 6; goto _test_eof; 
	_test_eof7:  shared_lexer->env.cs = 7; goto _test_eof; 
	_test_eof8:  shared_lexer->env.cs = 8; goto _test_eof; 
	_test_eof9:  shared_lexer->env.cs = 9; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
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
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
	_test_eof43:  shared_lexer->env.cs = 43; goto _test_eof; 
	_test_eof44:  shared_lexer->env.cs = 44; goto _test_eof; 
	_test_eof45:  shared_lexer->env.cs = 45; goto _test_eof; 
	_test_eof46:  shared_lexer->env.cs = 46; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof47:  shared_lexer->env.cs = 47; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof48:  shared_lexer->env.cs = 48; goto _test_eof; 
	_test_eof49:  shared_lexer->env.cs = 49; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof50:  shared_lexer->env.cs = 50; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof51:  shared_lexer->env.cs = 51; goto _test_eof; 
	_test_eof52:  shared_lexer->env.cs = 52; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof53:  shared_lexer->env.cs = 53; goto _test_eof; 
	_test_eof54:  shared_lexer->env.cs = 54; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof61:  shared_lexer->env.cs = 61; goto _test_eof; 
	_test_eof62:  shared_lexer->env.cs = 62; goto _test_eof; 
	_test_eof63:  shared_lexer->env.cs = 63; goto _test_eof; 
	_test_eof64:  shared_lexer->env.cs = 64; goto _test_eof; 
	_test_eof65:  shared_lexer->env.cs = 65; goto _test_eof; 
	_test_eof66:  shared_lexer->env.cs = 66; goto _test_eof; 
	_test_eof67:  shared_lexer->env.cs = 67; goto _test_eof; 
	_test_eof68:  shared_lexer->env.cs = 68; goto _test_eof; 
	_test_eof69:  shared_lexer->env.cs = 69; goto _test_eof; 
	_test_eof70:  shared_lexer->env.cs = 70; goto _test_eof; 
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
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
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
	_test_eof100:  shared_lexer->env.cs = 100; goto _test_eof; 
	_test_eof101:  shared_lexer->env.cs = 101; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof103:  shared_lexer->env.cs = 103; goto _test_eof; 
	_test_eof104:  shared_lexer->env.cs = 104; goto _test_eof; 
	_test_eof105:  shared_lexer->env.cs = 105; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof106:  shared_lexer->env.cs = 106; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
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
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof135:  shared_lexer->env.cs = 135; goto _test_eof; 
	_test_eof136:  shared_lexer->env.cs = 136; goto _test_eof; 
	_test_eof137:  shared_lexer->env.cs = 137; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof138:  shared_lexer->env.cs = 138; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
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
	_test_eof150:  shared_lexer->env.cs = 150; goto _test_eof; 
	_test_eof151:  shared_lexer->env.cs = 151; goto _test_eof; 
	_test_eof152:  shared_lexer->env.cs = 152; goto _test_eof; 
	_test_eof153:  shared_lexer->env.cs = 153; goto _test_eof; 
	_test_eof154:  shared_lexer->env.cs = 154; goto _test_eof; 
	_test_eof155:  shared_lexer->env.cs = 155; goto _test_eof; 
	_test_eof156:  shared_lexer->env.cs = 156; goto _test_eof; 
	_test_eof157:  shared_lexer->env.cs = 157; goto _test_eof; 
	_test_eof158:  shared_lexer->env.cs = 158; goto _test_eof; 
	_test_eof159:  shared_lexer->env.cs = 159; goto _test_eof; 
	_test_eof160:  shared_lexer->env.cs = 160; goto _test_eof; 
	_test_eof161:  shared_lexer->env.cs = 161; goto _test_eof; 
	_test_eof162:  shared_lexer->env.cs = 162; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 
	_test_eof218:  shared_lexer->env.cs = 218; goto _test_eof; 
	_test_eof219:  shared_lexer->env.cs = 219; goto _test_eof; 
	_test_eof220:  shared_lexer->env.cs = 220; goto _test_eof; 
	_test_eof221:  shared_lexer->env.cs = 221; goto _test_eof; 
	_test_eof222:  shared_lexer->env.cs = 222; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 174: goto tr61;
	case 175: goto tr230;
	case 176: goto tr231;
	case 0: goto tr0;
	case 1: goto tr0;
	case 2: goto tr0;
	case 3: goto tr0;
	case 4: goto tr0;
	case 5: goto tr0;
	case 6: goto tr0;
	case 7: goto tr0;
	case 8: goto tr0;
	case 9: goto tr0;
	case 177: goto tr235;
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
	case 178: goto tr231;
	case 23: goto tr0;
	case 179: goto tr237;
	case 180: goto tr231;
	case 24: goto tr0;
	case 25: goto tr0;
	case 26: goto tr0;
	case 27: goto tr0;
	case 28: goto tr0;
	case 29: goto tr0;
	case 30: goto tr0;
	case 31: goto tr0;
	case 32: goto tr0;
	case 33: goto tr0;
	case 181: goto tr241;
	case 34: goto tr43;
	case 35: goto tr43;
	case 36: goto tr43;
	case 37: goto tr43;
	case 38: goto tr43;
	case 39: goto tr43;
	case 40: goto tr43;
	case 41: goto tr43;
	case 42: goto tr43;
	case 43: goto tr43;
	case 44: goto tr43;
	case 45: goto tr43;
	case 46: goto tr43;
	case 182: goto tr231;
	case 47: goto tr0;
	case 183: goto tr244;
	case 48: goto tr61;
	case 49: goto tr61;
	case 184: goto tr245;
	case 185: goto tr246;
	case 50: goto tr64;
	case 186: goto tr248;
	case 187: goto tr249;
	case 51: goto tr66;
	case 52: goto tr66;
	case 188: goto tr249;
	case 53: goto tr66;
	case 54: goto tr66;
	case 55: goto tr66;
	case 56: goto tr66;
	case 57: goto tr66;
	case 58: goto tr66;
	case 59: goto tr66;
	case 60: goto tr66;
	case 61: goto tr66;
	case 62: goto tr66;
	case 63: goto tr66;
	case 64: goto tr66;
	case 65: goto tr66;
	case 66: goto tr66;
	case 67: goto tr66;
	case 68: goto tr66;
	case 69: goto tr66;
	case 70: goto tr66;
	case 71: goto tr66;
	case 72: goto tr66;
	case 73: goto tr66;
	case 74: goto tr66;
	case 75: goto tr66;
	case 76: goto tr66;
	case 77: goto tr66;
	case 78: goto tr66;
	case 79: goto tr66;
	case 80: goto tr66;
	case 81: goto tr66;
	case 189: goto tr231;
	case 82: goto tr0;
	case 83: goto tr0;
	case 84: goto tr0;
	case 85: goto tr0;
	case 86: goto tr0;
	case 87: goto tr0;
	case 88: goto tr0;
	case 89: goto tr0;
	case 90: goto tr0;
	case 91: goto tr0;
	case 92: goto tr0;
	case 93: goto tr0;
	case 94: goto tr0;
	case 95: goto tr0;
	case 96: goto tr0;
	case 97: goto tr0;
	case 98: goto tr0;
	case 99: goto tr0;
	case 100: goto tr0;
	case 101: goto tr0;
	case 102: goto tr0;
	case 103: goto tr0;
	case 104: goto tr0;
	case 105: goto tr0;
	case 190: goto tr231;
	case 191: goto tr61;
	case 106: goto tr61;
	case 192: goto tr256;
	case 193: goto tr256;
	case 194: goto tr256;
	case 195: goto tr256;
	case 196: goto tr256;
	case 197: goto tr256;
	case 198: goto tr256;
	case 199: goto tr256;
	case 200: goto tr256;
	case 201: goto tr61;
	case 107: goto tr61;
	case 108: goto tr61;
	case 109: goto tr61;
	case 110: goto tr61;
	case 111: goto tr61;
	case 112: goto tr61;
	case 113: goto tr61;
	case 114: goto tr61;
	case 115: goto tr61;
	case 116: goto tr61;
	case 117: goto tr61;
	case 118: goto tr61;
	case 119: goto tr61;
	case 120: goto tr61;
	case 121: goto tr61;
	case 122: goto tr61;
	case 123: goto tr61;
	case 124: goto tr61;
	case 125: goto tr61;
	case 126: goto tr61;
	case 127: goto tr61;
	case 128: goto tr61;
	case 129: goto tr61;
	case 130: goto tr61;
	case 131: goto tr61;
	case 202: goto tr231;
	case 132: goto tr0;
	case 133: goto tr0;
	case 203: goto tr231;
	case 134: goto tr0;
	case 135: goto tr0;
	case 136: goto tr0;
	case 137: goto tr0;
	case 204: goto tr231;
	case 205: goto tr231;
	case 138: goto tr0;
	case 206: goto tr269;
	case 139: goto tr170;
	case 140: goto tr61;
	case 141: goto tr61;
	case 142: goto tr61;
	case 143: goto tr61;
	case 144: goto tr170;
	case 145: goto tr61;
	case 146: goto tr61;
	case 147: goto tr170;
	case 148: goto tr170;
	case 149: goto tr170;
	case 150: goto tr61;
	case 151: goto tr61;
	case 152: goto tr61;
	case 153: goto tr61;
	case 154: goto tr61;
	case 155: goto tr61;
	case 156: goto tr61;
	case 157: goto tr61;
	case 158: goto tr61;
	case 159: goto tr61;
	case 160: goto tr61;
	case 161: goto tr61;
	case 162: goto tr61;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 166: goto tr0;
	case 207: goto tr231;
	case 167: goto tr0;
	case 168: goto tr0;
	case 169: goto tr0;
	case 208: goto tr231;
	case 170: goto tr0;
	case 209: goto tr231;
	case 210: goto tr231;
	case 211: goto tr231;
	case 212: goto tr231;
	case 213: goto tr231;
	case 214: goto tr231;
	case 215: goto tr231;
	case 171: goto tr0;
	case 172: goto tr0;
	case 216: goto tr231;
	case 217: goto tr231;
	case 218: goto tr231;
	case 219: goto tr231;
	case 220: goto tr231;
	case 221: goto tr231;
	case 222: goto tr231;
	}
	}

	}

#line 400 "turtle.rl"
        /* clang-format on */
        handler( ARDP_SUCCESS );
}
/*}}}*/
/* ardp_lexer_process_reader() {{{ */
int ardp_lexer_process_reader( lexer_reader reader, void *_Nullable reader_args)
{
        if ( reader == NULL )
                return 1;

        int status = 0;

        if ( !ardp_lexer_is_ready() )
                return 2;

        shared_lexer->finished = 0;

        uint8_t buf[BUFSIZE];

        size_t   have = 0;
        uint8_t *mark = NULL;
        bool     eof  = false;

        while (!shared_lexer->finished) {
               uint8_t *p = buf + have;

               size_t space = BUFSIZE - have;
               if (space <= 0) {
                       status = ARDP_LEXER_OMEM; /* Out of the buffer space */
                       break;                    /* goto exit; */
               }

               int len = reader(p, space, reader_args);
               if (len < space) {
                       eof = true;
                       shared_lexer->finished = 1;
               }

               status = ardp_lexer_process_block( p, len, mark, eof );

               if (shared_lexer->env.cs == turtle_error ) {
                        status = ARDP_LEXER_GENERIC_ERROR;
                        break;
               } else if (mark) {
                        have = ( p + len ) - mark;
                        memmove( buf, mark, have );
                        mark = buf;
               } else {
                        have = 0;
               }
        }

        dispatch_queue_t *q = &shared_lexer->lexer_queue;

        // Optimalization to free CPU operations
        while(   !dispatch_queue_isempty(*q)
              || !dispatch_queue_isempty(shared_lexer->event_queue))
                usleep(5);
        return status;
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

