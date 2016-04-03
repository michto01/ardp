
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
#include <ctype.h>

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

/*! Holds the column reference point */
static uint8_t* column;

/*!
 * Converst constrained string buffer to unsigned integer.
 *
 * @param[in] src String to probe.
 * @param[in] len Length to probe.
 *
 * @return UTF-8 codepoint.
 */
static uint32_t hex(const unsigned char *src, unsigned int len) {
    uint32_t i = 0;
    for (uint8_t j = 0; j < len; j++) {
        i *= 16;
        uint8_t c = src[j];
        if (isdigit(c)) {
            i += c - '0';
        } else if (isxdigit(c)) {
                i += c - ((isupper(c) ? 'A' : 'a')) + 10;
        } 
        /*
        
        else if ((c >= 'A' && c <= 'F') && (c >= 'A' && c <= 'F')) {
            i += c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            i += c - 'a' +  10;
        }
        */
    }
    return i;
}

/*!
 * Shorthand to speedup token input.
 */
#define var( s ) ( shared_lexer->env.s )

/* clang-format off */
/* ragel-machine() {{{*/

#line 89 "lexer2.c"
static const int turtle_start = 169;
static const int turtle_first_final = 169;
static const int turtle_error = -1;

static const int turtle_en_main = 169;


#line 271 "turtle.rl"

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
/* lexer_emit_u8_token() {{{*/
static void lexer_emit_u8_token(enum turtle_token_type type) {
        assert(shared_lexer); /* sanity check*/
        if (shared_lexer->cb.stoken) {
                __block utf8 s;
                if (shared_lexer->string)
                         s = string_copy(shared_lexer->string);
                else
                         s = NULL;
                dispatch_async( shared_lexer->event_queue, ^{
                        ptrdiff_t col = shared_lexer->env.ts - column;
                        shared_lexer->cb.stoken(type, s, shared_lexer->line+1, col);
                        if (shared_lexer->log.level < NOTICE)
                                log (DEBUG, "Emitted token (UTF8)");
                });
        }
}
/*}}}*/
/* lexer_emit_token() {{{ */
static void lexer_emit_token( enum turtle_token_type type, uint8_t *_Nullable str, size_t len )
{
        assert( shared_lexer ); /* sanity check */

        __block ptrdiff_t col = shared_lexer->env.ts - column;

        if (shared_lexer->cb.token) {
            dispatch_async( shared_lexer->event_queue, ^{
                //char* p_str = malloc( ( len + 1 ) * sizeof( *p_str ) );
                //assert(p_str); /* Sanity check for the malloc() */
                utf8 s = string_create_n(str,len, len+1);

                //strncat( p_str, ( const char * )str, len );

                shared_lexer->cb.stoken( type, s, shared_lexer->line, col );
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
        __block ptrdiff_t col = shared_lexer->env.ts - column;
        if ( shared_lexer->cb.stoken ) {
            dispatch_async( shared_lexer->event_queue, ^{
                shared_lexer->cb.stoken( type, NULL, shared_lexer->line, col );

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
        
#line 260 "lexer2.c"
	{
	 shared_lexer->env.cs = turtle_start;
	 shared_lexer->env.ts = 0;
	 shared_lexer->env.te = 0;
	 shared_lexer->env.act = 0;
	}

#line 433 "turtle.rl"
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
    
#line 329 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 267 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st169;
tr2:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr23:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr26:
#line 230 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr34:
#line 227 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr59:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr61:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr82:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr85:
#line 231 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr93:
#line 228 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr117:
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
	default:
	{{p = (( shared_lexer->env.te))-1;}}
	break;
	}
	}
	goto st169;
tr120:
#line 222 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr122:
#line 215 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st169;
tr154:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr167:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr218:
#line 214 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st169;
tr236:
#line 267 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st169;
tr238:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr242:
#line 248 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st169;
tr243:
#line 249 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st169;
tr245:
#line 243 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st169;
tr248:
#line 244 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st169;
tr254:
#line 246 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st169;
tr255:
#line 247 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st169;
tr261:
#line 250 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st169;
tr262:
#line 251 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st169;
tr277:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr278:
#line 267 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st169;
tr282:
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr284:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr288:
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr291:
#line 223 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr292:
#line 224 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr293:
#line 222 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr295:
#line 242 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st169;
tr296:
#line 215 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st169;
tr300:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr306:
#line 236 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st169;
tr317:
#line 233 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st169;
tr319:
#line 214 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st169;
st169:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 646 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st170;
		case 13u: goto tr238;
		case 34u: goto tr239;
		case 35u: goto tr240;
		case 39u: goto tr241;
		case 40u: goto tr242;
		case 41u: goto tr243;
		case 44u: goto tr245;
		case 46u: goto st181;
		case 58u: goto tr173;
		case 59u: goto tr248;
		case 60u: goto tr249;
		case 64u: goto st185;
		case 66u: goto tr252;
		case 80u: goto tr253;
		case 91u: goto tr254;
		case 93u: goto tr255;
		case 94u: goto st199;
		case 95u: goto tr257;
		case 97u: goto tr258;
		case 98u: goto tr252;
		case 102u: goto tr259;
		case 112u: goto tr253;
		case 116u: goto tr260;
		case 123u: goto tr261;
		case 125u: goto tr262;
		case 195u: goto tr263;
		case 203u: goto tr265;
		case 205u: goto tr266;
		case 223u: goto tr265;
		case 224u: goto tr267;
		case 225u: goto tr268;
		case 226u: goto tr269;
		case 227u: goto tr270;
		case 237u: goto tr272;
		case 239u: goto tr273;
		case 240u: goto tr274;
		case 243u: goto tr276;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr244;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr251;
		} else
			goto tr247;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr264;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr275;
			} else if ( (*p) >= 228u )
				goto tr271;
		} else
			goto tr264;
	} else
		goto tr251;
	goto tr236;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	if ( (*p) == 13u )
		goto tr238;
	goto tr277;
tr239:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 726 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr278;
		case 13u: goto tr278;
		case 34u: goto tr280;
		case 39u: goto tr278;
		case 92u: goto tr281;
	}
	goto tr279;
tr1:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr4:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st0;
tr5:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st0;
tr7:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st0;
tr8:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st0;
tr9:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st0;
tr10:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st0;
tr11:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st0;
tr12:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st0;
tr22:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr279:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 835 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto st1;
	}
	goto tr1;
tr24:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st1;
tr281:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 865 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr4;
		case 39u: goto tr5;
		case 85u: goto st2;
		case 92u: goto tr7;
		case 98u: goto tr8;
		case 102u: goto tr9;
		case 110u: goto tr10;
		case 114u: goto tr11;
		case 116u: goto tr12;
		case 117u: goto st11;
	}
	goto tr0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr14;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr14;
	} else
		goto tr14;
	goto tr0;
tr14:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 902 "lexer2.c"
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
tr25:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 961 "lexer2.c"
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
			goto st10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st10;
	} else
		goto st10;
	goto tr0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr23;
		case 39u: goto tr0;
		case 92u: goto tr24;
	}
	goto tr22;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr25;
	} else
		goto tr25;
	goto tr0;
tr280:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st172;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
#line 1041 "lexer2.c"
	if ( (*p) == 34u )
		goto st12;
	goto tr282;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 34u: goto tr28;
		case 92u: goto tr29;
	}
	goto tr27;
tr30:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr35:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st13;
tr36:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st13;
tr38:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st13;
tr39:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st13;
tr40:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st13;
tr41:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st13;
tr42:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st13;
tr43:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st13;
tr53:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr27:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 1154 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr31;
		case 92u: goto st16;
	}
	goto tr30;
tr28:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr31:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr54:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 1201 "lexer2.c"
	if ( (*p) == 34u )
		goto st15;
	goto tr26;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 34u )
		goto tr34;
	goto tr26;
tr55:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st16;
tr29:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 1233 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr35;
		case 39u: goto tr36;
		case 85u: goto st17;
		case 92u: goto tr38;
		case 98u: goto tr39;
		case 102u: goto tr40;
		case 110u: goto tr41;
		case 114u: goto tr42;
		case 116u: goto tr43;
		case 117u: goto st26;
	}
	goto tr26;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr45;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr45;
	} else
		goto tr45;
	goto tr26;
tr45:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 1270 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st19;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st19;
	} else
		goto st19;
	goto tr26;
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
	goto tr26;
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
	goto tr26;
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
	goto tr26;
tr56:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 1329 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st23;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st23;
	} else
		goto st23;
	goto tr26;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st24;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st24;
	} else
		goto st24;
	goto tr26;
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
	goto tr26;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 34u: goto tr54;
		case 92u: goto tr55;
	}
	goto tr53;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr56;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr56;
	} else
		goto tr56;
	goto tr26;
tr240:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 1395 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st174;
		case 13u: goto tr59;
	}
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	switch( (*p) ) {
		case 10u: goto st174;
		case 13u: goto tr59;
	}
	goto st27;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
	if ( (*p) == 13u )
		goto tr59;
	goto tr284;
tr241:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 1425 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr278;
		case 13u: goto tr278;
		case 34u: goto tr278;
		case 39u: goto tr286;
		case 92u: goto tr287;
	}
	goto tr285;
tr60:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr63:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st28;
tr64:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st28;
tr66:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st28;
tr67:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st28;
tr68:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st28;
tr69:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st28;
tr70:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st28;
tr71:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st28;
tr81:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr285:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 1534 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr61;
		case 92u: goto st29;
	}
	goto tr60;
tr83:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st29;
tr287:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 1564 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr63;
		case 39u: goto tr64;
		case 85u: goto st30;
		case 92u: goto tr66;
		case 98u: goto tr67;
		case 102u: goto tr68;
		case 110u: goto tr69;
		case 114u: goto tr70;
		case 116u: goto tr71;
		case 117u: goto st39;
	}
	goto tr0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr73;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr73;
	} else
		goto tr73;
	goto tr0;
tr73:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 1601 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st32;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st32;
	} else
		goto st32;
	goto tr0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st33;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st33;
	} else
		goto st33;
	goto tr0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st34;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st34;
	} else
		goto st34;
	goto tr0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st35;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st35;
	} else
		goto st35;
	goto tr0;
tr84:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 1660 "lexer2.c"
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
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr82;
		case 92u: goto tr83;
	}
	goto tr81;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr84;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr84;
	} else
		goto tr84;
	goto tr0;
tr286:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 1740 "lexer2.c"
	if ( (*p) == 39u )
		goto st40;
	goto tr288;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 39u: goto tr87;
		case 92u: goto tr88;
	}
	goto tr86;
tr89:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr94:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st41;
tr95:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st41;
tr97:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st41;
tr98:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st41;
tr99:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st41;
tr100:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st41;
tr101:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st41;
tr102:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st41;
tr112:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr86:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 1853 "lexer2.c"
	switch( (*p) ) {
		case 39u: goto tr90;
		case 92u: goto st44;
	}
	goto tr89;
tr87:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr90:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr113:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 1900 "lexer2.c"
	if ( (*p) == 39u )
		goto st43;
	goto tr85;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	if ( (*p) == 39u )
		goto tr93;
	goto tr85;
tr114:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st44;
tr88:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 1932 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr94;
		case 39u: goto tr95;
		case 85u: goto st45;
		case 92u: goto tr97;
		case 98u: goto tr98;
		case 102u: goto tr99;
		case 110u: goto tr100;
		case 114u: goto tr101;
		case 116u: goto tr102;
		case 117u: goto st54;
	}
	goto tr85;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr104;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr104;
	} else
		goto tr104;
	goto tr85;
tr104:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 1969 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st47;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st47;
	} else
		goto st47;
	goto tr85;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st48;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st48;
	} else
		goto st48;
	goto tr85;
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
	goto tr85;
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
	goto tr85;
tr115:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 2028 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st51;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st51;
	} else
		goto st51;
	goto tr85;
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
	goto tr85;
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
	goto tr85;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 39u: goto tr113;
		case 92u: goto tr114;
	}
	goto tr112;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr115;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr115;
	} else
		goto tr115;
	goto tr85;
tr244:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 2094 "lexer2.c"
	if ( (*p) == 46u )
		goto st55;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr247;
	goto tr278;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr0;
tr116:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 223 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 2117 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr291;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 43u: goto st57;
		case 45u: goto st57;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr117;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr117;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr292;
tr247:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 222 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 2160 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st58;
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr247;
	goto tr293;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr120;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr295;
tr173:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 2195 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st59;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 205u: goto st86;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st87;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr124;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr124;
		} else
			goto tr124;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto st64;
	goto tr296;
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
	goto tr122;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr124;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
tr124:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 2268 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st59;
		case 45u: goto tr124;
		case 46u: goto st61;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 204u: goto st66;
		case 205u: goto st67;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st70;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr124;
		} else if ( (*p) >= 48u )
			goto tr124;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto tr124;
	goto tr296;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 37u: goto st59;
		case 45u: goto tr124;
		case 46u: goto st61;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 204u: goto st66;
		case 205u: goto st67;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st70;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr124;
		} else if ( (*p) >= 48u )
			goto tr124;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto tr124;
	goto tr122;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 33u: goto tr124;
		case 59u: goto tr124;
		case 61u: goto tr124;
		case 95u: goto tr124;
		case 126u: goto tr124;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr124;
	} else if ( (*p) >= 35u )
		goto tr124;
	goto tr122;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr124;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	goto tr124;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 192u <= (*p) )
		goto tr122;
	goto tr124;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( 128u <= (*p) )
		goto tr124;
	goto tr122;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) == 190u )
		goto tr122;
	goto tr124;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) == 160u )
		goto st66;
	if ( 161u <= (*p) )
		goto st64;
	goto tr122;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) == 191u )
		goto st65;
	if ( 192u <= (*p) )
		goto tr122;
	goto st64;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 128u: goto st71;
		case 129u: goto st72;
		case 134u: goto st73;
		case 176u: goto st66;
		case 191u: goto st74;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st64;
	} else if ( (*p) >= 130u )
		goto st64;
	goto tr122;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 140u )
		goto tr124;
	goto tr122;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( 144u <= (*p) )
		goto tr122;
	goto tr124;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( 176u <= (*p) )
		goto tr122;
	goto tr124;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 128u )
		goto st76;
	if ( 129u <= (*p) )
		goto st64;
	goto tr122;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( 129u <= (*p) )
		goto tr124;
	goto tr122;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	goto st64;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) == 159u )
		goto st65;
	if ( 160u <= (*p) )
		goto tr122;
	goto st64;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 164u: goto st66;
		case 183u: goto st80;
		case 191u: goto st81;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st64;
	goto tr122;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( 190u <= (*p) )
		goto tr122;
	goto tr124;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( (*p) == 144u )
		goto st83;
	if ( 145u <= (*p) )
		goto st77;
	goto tr122;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) == 128u )
		goto st66;
	if ( 129u <= (*p) )
		goto st64;
	goto tr122;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	goto st77;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 175u )
		goto st69;
	if ( 176u <= (*p) )
		goto tr122;
	goto st77;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 176u )
		goto tr124;
	goto tr122;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	switch( (*p) ) {
		case 128u: goto st88;
		case 129u: goto st89;
		case 134u: goto st73;
		case 176u: goto st66;
		case 191u: goto st74;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st64;
	} else if ( (*p) >= 130u )
		goto st64;
	goto tr122;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr124;
	goto tr122;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( 176u <= (*p) )
		goto tr124;
	goto tr122;
tr249:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 2610 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr278;
		case 60u: goto tr278;
		case 62u: goto tr300;
		case 92u: goto tr301;
		case 94u: goto tr278;
		case 96u: goto tr278;
		case 123u: goto tr278;
		case 125u: goto tr278;
	}
	if ( (*p) <= 32u )
		goto tr278;
	goto tr299;
tr153:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr166:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr299:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
#line 2668 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr154;
		case 92u: goto st91;
		case 94u: goto tr0;
		case 96u: goto tr0;
		case 123u: goto tr0;
		case 125u: goto tr0;
	}
	if ( (*p) <= 32u )
		goto tr0;
	goto tr153;
tr168:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st91;
tr301:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st91;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
#line 2703 "lexer2.c"
	switch( (*p) ) {
		case 85u: goto st92;
		case 117u: goto st101;
	}
	goto tr0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr158;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr158;
	} else
		goto tr158;
	goto tr0;
tr158:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st93;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
#line 2732 "lexer2.c"
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
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st95;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st95;
	} else
		goto st95;
	goto tr0;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st96;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st96;
	} else
		goto st96;
	goto tr0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st97;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st97;
	} else
		goto st97;
	goto tr0;
tr169:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st97;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
#line 2791 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st98;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st98;
	} else
		goto st98;
	goto tr0;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st99;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st99;
	} else
		goto st99;
	goto tr0;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st100;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st100;
	} else
		goto st100;
	goto tr0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr167;
		case 92u: goto tr168;
		case 94u: goto tr0;
		case 96u: goto tr0;
		case 123u: goto tr0;
		case 125u: goto tr0;
	}
	if ( (*p) <= 32u )
		goto tr0;
	goto tr166;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr169;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr169;
	} else
		goto tr169;
	goto tr0;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
	switch( (*p) ) {
		case 98u: goto tr303;
		case 112u: goto tr304;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr278;
tr302:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st186;
tr309:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 211 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st186;
tr314:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 209 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 2893 "lexer2.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr117;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr170;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr170;
	} else
		goto tr170;
	goto tr117;
tr170:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 2925 "lexer2.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr170;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr170;
	} else
		goto tr170;
	goto tr306;
tr303:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 2947 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 97u: goto tr307;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr307:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 2968 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 115u: goto tr308;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr308:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 2989 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr309;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr304:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 3010 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 114u: goto tr310;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr310:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 3031 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr311;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr311:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 3052 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 102u: goto tr312;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr312:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 3073 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 105u: goto tr313;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr313:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 3094 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 120u: goto tr314;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr198:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 212 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st196;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 210 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st196;
tr235:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 225 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st196;
tr251:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st196;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 241 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 3139 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st103;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st103;
	} else
		goto st103;
	goto tr117;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	goto st103;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	if ( 192u <= (*p) )
		goto tr117;
	goto st103;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	if ( 128u <= (*p) )
		goto st103;
	goto tr117;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) == 190u )
		goto tr117;
	goto st103;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( (*p) == 160u )
		goto st108;
	if ( 161u <= (*p) )
		goto st106;
	goto tr117;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( (*p) == 191u )
		goto st107;
	if ( 192u <= (*p) )
		goto tr117;
	goto st106;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	switch( (*p) ) {
		case 128u: goto st113;
		case 129u: goto st114;
		case 134u: goto st115;
		case 176u: goto st108;
		case 191u: goto st116;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st106;
	} else if ( (*p) >= 130u )
		goto st106;
	goto tr117;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st103;
	} else if ( (*p) >= 140u )
		goto st103;
	goto tr117;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto st103;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( 144u <= (*p) )
		goto tr117;
	goto st103;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( 176u <= (*p) )
		goto tr117;
	goto st103;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) == 128u )
		goto st118;
	if ( 129u <= (*p) )
		goto st106;
	goto tr117;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 129u <= (*p) )
		goto st103;
	goto tr117;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	goto st106;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( (*p) == 159u )
		goto st107;
	if ( 160u <= (*p) )
		goto tr117;
	goto st106;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	switch( (*p) ) {
		case 164u: goto st108;
		case 183u: goto st122;
		case 191u: goto st123;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st106;
	goto tr117;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto st103;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	if ( 190u <= (*p) )
		goto tr117;
	goto st103;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) == 144u )
		goto st125;
	if ( 145u <= (*p) )
		goto st119;
	goto tr117;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	if ( (*p) == 128u )
		goto st108;
	if ( 129u <= (*p) )
		goto st106;
	goto tr117;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	goto st119;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) == 175u )
		goto st111;
	if ( 176u <= (*p) )
		goto tr117;
	goto st119;
tr252:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 3464 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 65u: goto st128;
		case 95u: goto st103;
		case 97u: goto st128;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 83u: goto st129;
		case 95u: goto st103;
		case 115u: goto st129;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 69u: goto tr198;
		case 95u: goto st103;
		case 101u: goto tr198;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr253:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 3603 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 82u: goto st130;
		case 95u: goto st103;
		case 114u: goto st130;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 69u: goto st131;
		case 95u: goto st103;
		case 101u: goto st131;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 70u: goto st132;
		case 95u: goto st103;
		case 102u: goto st132;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 73u: goto st133;
		case 95u: goto st103;
		case 105u: goto st133;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 88u: goto tr202;
		case 95u: goto st103;
		case 120u: goto tr202;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
	if ( (*p) == 94u )
		goto tr317;
	goto tr278;
tr257:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 3837 "lexer2.c"
	if ( (*p) == 58u )
		goto st134;
	goto tr278;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 95u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 205u: goto st159;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st160;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 57u )
				goto tr203;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr203;
		} else
			goto tr203;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto st137;
	goto tr0;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 214 "turtle.rl"
	{ shared_lexer->env.act = 5;}
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 3891 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto st135;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 204u: goto st139;
		case 205u: goto st140;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st143;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr203;
		} else if ( (*p) >= 48u )
			goto tr203;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto tr203;
	goto tr319;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto st135;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 204u: goto st139;
		case 205u: goto st140;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st143;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr203;
		} else if ( (*p) >= 48u )
			goto tr203;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto tr203;
	goto tr218;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr117;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	goto tr203;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( 192u <= (*p) )
		goto tr117;
	goto tr203;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( 128u <= (*p) )
		goto tr203;
	goto tr117;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) == 190u )
		goto tr218;
	goto tr203;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	if ( (*p) == 160u )
		goto st139;
	if ( 161u <= (*p) )
		goto st137;
	goto tr117;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 191u )
		goto st138;
	if ( 192u <= (*p) )
		goto tr117;
	goto st137;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	switch( (*p) ) {
		case 128u: goto st144;
		case 129u: goto st145;
		case 134u: goto st146;
		case 176u: goto st139;
		case 191u: goto st147;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st137;
	} else if ( (*p) >= 130u )
		goto st137;
	goto tr218;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 140u )
		goto tr203;
	goto tr218;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( 144u <= (*p) )
		goto tr117;
	goto tr203;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	if ( 176u <= (*p) )
		goto tr117;
	goto tr203;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	if ( (*p) == 128u )
		goto st149;
	if ( 129u <= (*p) )
		goto st137;
	goto tr117;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( 129u <= (*p) )
		goto tr203;
	goto tr117;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	goto st137;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	if ( (*p) == 159u )
		goto st138;
	if ( 160u <= (*p) )
		goto tr117;
	goto st137;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 164u: goto st139;
		case 183u: goto st153;
		case 191u: goto st154;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st137;
	goto tr117;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr203;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	if ( 190u <= (*p) )
		goto tr117;
	goto tr203;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	if ( (*p) == 144u )
		goto st156;
	if ( 145u <= (*p) )
		goto st150;
	goto tr117;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	if ( (*p) == 128u )
		goto st139;
	if ( 129u <= (*p) )
		goto st137;
	goto tr117;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	goto st150;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	if ( (*p) == 175u )
		goto st142;
	if ( 176u <= (*p) )
		goto tr117;
	goto st150;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 176u )
		goto tr203;
	goto tr0;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	switch( (*p) ) {
		case 128u: goto st161;
		case 129u: goto st162;
		case 134u: goto st146;
		case 176u: goto st139;
		case 191u: goto st147;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st137;
	} else if ( (*p) >= 130u )
		goto st137;
	goto tr0;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr203;
	goto tr0;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	if ( 176u <= (*p) )
		goto tr203;
	goto tr0;
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 4214 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 97u: goto st163;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 108u: goto st164;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 115u: goto st165;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 101u: goto tr235;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 4393 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 114u: goto st166;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 117u: goto st165;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st204;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
#line 4486 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st103;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st103;
	} else
		goto st103;
	goto tr278;
tr264:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 4506 "lexer2.c"
	goto st103;
tr265:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 4518 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr278;
	goto st103;
tr266:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 4532 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st103;
	} else if ( (*p) >= 176u )
		goto st103;
	goto tr278;
tr267:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 4549 "lexer2.c"
	if ( (*p) == 160u )
		goto st108;
	if ( 161u <= (*p) )
		goto st106;
	goto tr278;
tr268:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 4565 "lexer2.c"
	if ( (*p) == 191u )
		goto st107;
	if ( 192u <= (*p) )
		goto tr278;
	goto st106;
tr269:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 4581 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st167;
		case 129u: goto st168;
		case 134u: goto st115;
		case 176u: goto st108;
		case 191u: goto st116;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st106;
	} else if ( (*p) >= 130u )
		goto st106;
	goto tr278;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st103;
	goto tr0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	if ( 176u <= (*p) )
		goto st103;
	goto tr0;
tr270:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 4619 "lexer2.c"
	if ( (*p) == 128u )
		goto st118;
	if ( 129u <= (*p) )
		goto st106;
	goto tr278;
tr271:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 4635 "lexer2.c"
	goto st106;
tr272:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 4647 "lexer2.c"
	if ( (*p) == 159u )
		goto st107;
	if ( 160u <= (*p) )
		goto tr278;
	goto st106;
tr273:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 4663 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st108;
		case 183u: goto st122;
		case 191u: goto st123;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st106;
	goto tr278;
tr274:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 4682 "lexer2.c"
	if ( (*p) == 144u )
		goto st125;
	if ( 145u <= (*p) )
		goto st119;
	goto tr278;
tr275:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 4698 "lexer2.c"
	goto st119;
tr276:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 4710 "lexer2.c"
	if ( (*p) == 175u )
		goto st111;
	if ( 176u <= (*p) )
		goto tr278;
	goto st119;
	}
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
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
	_test_eof10:  shared_lexer->env.cs = 10; goto _test_eof; 
	_test_eof11:  shared_lexer->env.cs = 11; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
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
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
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
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
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
	_test_eof82:  shared_lexer->env.cs = 82; goto _test_eof; 
	_test_eof83:  shared_lexer->env.cs = 83; goto _test_eof; 
	_test_eof84:  shared_lexer->env.cs = 84; goto _test_eof; 
	_test_eof85:  shared_lexer->env.cs = 85; goto _test_eof; 
	_test_eof86:  shared_lexer->env.cs = 86; goto _test_eof; 
	_test_eof87:  shared_lexer->env.cs = 87; goto _test_eof; 
	_test_eof88:  shared_lexer->env.cs = 88; goto _test_eof; 
	_test_eof89:  shared_lexer->env.cs = 89; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
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
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
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
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
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
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
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
	case 170: goto tr277;
	case 171: goto tr278;
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
	case 10: goto tr0;
	case 11: goto tr0;
	case 172: goto tr282;
	case 12: goto tr26;
	case 13: goto tr26;
	case 14: goto tr26;
	case 15: goto tr26;
	case 16: goto tr26;
	case 17: goto tr26;
	case 18: goto tr26;
	case 19: goto tr26;
	case 20: goto tr26;
	case 21: goto tr26;
	case 22: goto tr26;
	case 23: goto tr26;
	case 24: goto tr26;
	case 25: goto tr26;
	case 26: goto tr26;
	case 173: goto tr278;
	case 27: goto tr0;
	case 174: goto tr284;
	case 175: goto tr278;
	case 28: goto tr0;
	case 29: goto tr0;
	case 30: goto tr0;
	case 31: goto tr0;
	case 32: goto tr0;
	case 33: goto tr0;
	case 34: goto tr0;
	case 35: goto tr0;
	case 36: goto tr0;
	case 37: goto tr0;
	case 38: goto tr0;
	case 39: goto tr0;
	case 176: goto tr288;
	case 40: goto tr85;
	case 41: goto tr85;
	case 42: goto tr85;
	case 43: goto tr85;
	case 44: goto tr85;
	case 45: goto tr85;
	case 46: goto tr85;
	case 47: goto tr85;
	case 48: goto tr85;
	case 49: goto tr85;
	case 50: goto tr85;
	case 51: goto tr85;
	case 52: goto tr85;
	case 53: goto tr85;
	case 54: goto tr85;
	case 177: goto tr278;
	case 55: goto tr0;
	case 178: goto tr291;
	case 56: goto tr117;
	case 57: goto tr117;
	case 179: goto tr292;
	case 180: goto tr293;
	case 58: goto tr120;
	case 181: goto tr295;
	case 182: goto tr296;
	case 59: goto tr122;
	case 60: goto tr122;
	case 183: goto tr296;
	case 61: goto tr122;
	case 62: goto tr122;
	case 63: goto tr122;
	case 64: goto tr122;
	case 65: goto tr122;
	case 66: goto tr122;
	case 67: goto tr122;
	case 68: goto tr122;
	case 69: goto tr122;
	case 70: goto tr122;
	case 71: goto tr122;
	case 72: goto tr122;
	case 73: goto tr122;
	case 74: goto tr122;
	case 75: goto tr122;
	case 76: goto tr122;
	case 77: goto tr122;
	case 78: goto tr122;
	case 79: goto tr122;
	case 80: goto tr122;
	case 81: goto tr122;
	case 82: goto tr122;
	case 83: goto tr122;
	case 84: goto tr122;
	case 85: goto tr122;
	case 86: goto tr122;
	case 87: goto tr122;
	case 88: goto tr122;
	case 89: goto tr122;
	case 184: goto tr278;
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
	case 185: goto tr278;
	case 186: goto tr117;
	case 102: goto tr117;
	case 187: goto tr306;
	case 188: goto tr306;
	case 189: goto tr306;
	case 190: goto tr306;
	case 191: goto tr306;
	case 192: goto tr306;
	case 193: goto tr306;
	case 194: goto tr306;
	case 195: goto tr306;
	case 196: goto tr117;
	case 103: goto tr117;
	case 104: goto tr117;
	case 105: goto tr117;
	case 106: goto tr117;
	case 107: goto tr117;
	case 108: goto tr117;
	case 109: goto tr117;
	case 110: goto tr117;
	case 111: goto tr117;
	case 112: goto tr117;
	case 113: goto tr117;
	case 114: goto tr117;
	case 115: goto tr117;
	case 116: goto tr117;
	case 117: goto tr117;
	case 118: goto tr117;
	case 119: goto tr117;
	case 120: goto tr117;
	case 121: goto tr117;
	case 122: goto tr117;
	case 123: goto tr117;
	case 124: goto tr117;
	case 125: goto tr117;
	case 126: goto tr117;
	case 127: goto tr117;
	case 197: goto tr278;
	case 128: goto tr0;
	case 129: goto tr0;
	case 198: goto tr278;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 199: goto tr278;
	case 200: goto tr278;
	case 134: goto tr0;
	case 201: goto tr319;
	case 135: goto tr218;
	case 136: goto tr117;
	case 137: goto tr117;
	case 138: goto tr117;
	case 139: goto tr117;
	case 140: goto tr218;
	case 141: goto tr117;
	case 142: goto tr117;
	case 143: goto tr218;
	case 144: goto tr218;
	case 145: goto tr218;
	case 146: goto tr117;
	case 147: goto tr117;
	case 148: goto tr117;
	case 149: goto tr117;
	case 150: goto tr117;
	case 151: goto tr117;
	case 152: goto tr117;
	case 153: goto tr117;
	case 154: goto tr117;
	case 155: goto tr117;
	case 156: goto tr117;
	case 157: goto tr117;
	case 158: goto tr117;
	case 159: goto tr0;
	case 160: goto tr0;
	case 161: goto tr0;
	case 162: goto tr0;
	case 202: goto tr278;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 203: goto tr278;
	case 166: goto tr0;
	case 204: goto tr278;
	case 205: goto tr278;
	case 206: goto tr278;
	case 207: goto tr278;
	case 208: goto tr278;
	case 209: goto tr278;
	case 210: goto tr278;
	case 167: goto tr0;
	case 168: goto tr0;
	case 211: goto tr278;
	case 212: goto tr278;
	case 213: goto tr278;
	case 214: goto tr278;
	case 215: goto tr278;
	case 216: goto tr278;
	case 217: goto tr278;
	}
	}

	}

#line 493 "turtle.rl"
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
          
#line 5193 "lexer2.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 267 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st169;
tr2:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr23:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr26:
#line 230 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr34:
#line 227 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr59:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr61:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr82:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr85:
#line 231 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr93:
#line 228 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr117:
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
	default:
	{{p = (( shared_lexer->env.te))-1;}}
	break;
	}
	}
	goto st169;
tr120:
#line 222 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr122:
#line 215 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st169;
tr154:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr167:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr218:
#line 214 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st169;
tr236:
#line 267 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st169;
tr238:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr242:
#line 248 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st169;
tr243:
#line 249 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st169;
tr245:
#line 243 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st169;
tr248:
#line 244 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st169;
tr254:
#line 246 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st169;
tr255:
#line 247 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st169;
tr261:
#line 250 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st169;
tr262:
#line 251 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st169;
tr277:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr278:
#line 267 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st169;
tr282:
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr284:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_async(shared_lexer->event_queue, ^{
                  column = p;
                  shared_lexer->line++;
              });
        }}
	goto st169;
tr288:
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st169;
tr291:
#line 223 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr292:
#line 224 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr293:
#line 222 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st169;
tr295:
#line 242 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st169;
tr296:
#line 215 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(QNAME,         var(ts),     var(te) - var(ts)); }}
	goto st169;
tr300:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 216 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
                lexer_emit_u8_token(IRIREF);
                //lexer_emit_token(IRIREF,        var(ts) +1, (var(te) - 1) - (var(ts) + 1));
        }}
	goto st169;
tr306:
#line 236 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st169;
tr317:
#line 233 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st169;
tr319:
#line 214 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(BLANK_LITERAL, var(ts) +2,  var(te) - (var(ts) +2)); }}
	goto st169;
st169:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 5510 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st170;
		case 13u: goto tr238;
		case 34u: goto tr239;
		case 35u: goto tr240;
		case 39u: goto tr241;
		case 40u: goto tr242;
		case 41u: goto tr243;
		case 44u: goto tr245;
		case 46u: goto st181;
		case 58u: goto tr173;
		case 59u: goto tr248;
		case 60u: goto tr249;
		case 64u: goto st185;
		case 66u: goto tr252;
		case 80u: goto tr253;
		case 91u: goto tr254;
		case 93u: goto tr255;
		case 94u: goto st199;
		case 95u: goto tr257;
		case 97u: goto tr258;
		case 98u: goto tr252;
		case 102u: goto tr259;
		case 112u: goto tr253;
		case 116u: goto tr260;
		case 123u: goto tr261;
		case 125u: goto tr262;
		case 195u: goto tr263;
		case 203u: goto tr265;
		case 205u: goto tr266;
		case 223u: goto tr265;
		case 224u: goto tr267;
		case 225u: goto tr268;
		case 226u: goto tr269;
		case 227u: goto tr270;
		case 237u: goto tr272;
		case 239u: goto tr273;
		case 240u: goto tr274;
		case 243u: goto tr276;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr244;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr251;
		} else
			goto tr247;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr264;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr275;
			} else if ( (*p) >= 228u )
				goto tr271;
		} else
			goto tr264;
	} else
		goto tr251;
	goto tr236;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
	if ( (*p) == 13u )
		goto tr238;
	goto tr277;
tr239:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 5590 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr278;
		case 13u: goto tr278;
		case 34u: goto tr280;
		case 39u: goto tr278;
		case 92u: goto tr281;
	}
	goto tr279;
tr1:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr4:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st0;
tr5:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st0;
tr7:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st0;
tr8:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st0;
tr9:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st0;
tr10:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st0;
tr11:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st0;
tr12:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st0;
tr22:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr279:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 5699 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 39u: goto tr0;
		case 92u: goto st1;
	}
	goto tr1;
tr24:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st1;
tr281:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 5729 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr4;
		case 39u: goto tr5;
		case 85u: goto st2;
		case 92u: goto tr7;
		case 98u: goto tr8;
		case 102u: goto tr9;
		case 110u: goto tr10;
		case 114u: goto tr11;
		case 116u: goto tr12;
		case 117u: goto st11;
	}
	goto tr0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr14;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr14;
	} else
		goto tr14;
	goto tr0;
tr14:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 5766 "lexer2.c"
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
tr25:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 5825 "lexer2.c"
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
			goto st10;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st10;
	} else
		goto st10;
	goto tr0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr23;
		case 39u: goto tr0;
		case 92u: goto tr24;
	}
	goto tr22;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr25;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr25;
	} else
		goto tr25;
	goto tr0;
tr280:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st172;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
#line 5905 "lexer2.c"
	if ( (*p) == 34u )
		goto st12;
	goto tr282;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 34u: goto tr28;
		case 92u: goto tr29;
	}
	goto tr27;
tr30:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr35:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st13;
tr36:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st13;
tr38:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st13;
tr39:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st13;
tr40:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st13;
tr41:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st13;
tr42:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st13;
tr43:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st13;
tr53:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr27:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 6018 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr31;
		case 92u: goto st16;
	}
	goto tr30;
tr28:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr31:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr54:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 6065 "lexer2.c"
	if ( (*p) == 34u )
		goto st15;
	goto tr26;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 34u )
		goto tr34;
	goto tr26;
tr55:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st16;
tr29:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 6097 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr35;
		case 39u: goto tr36;
		case 85u: goto st17;
		case 92u: goto tr38;
		case 98u: goto tr39;
		case 102u: goto tr40;
		case 110u: goto tr41;
		case 114u: goto tr42;
		case 116u: goto tr43;
		case 117u: goto st26;
	}
	goto tr26;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr45;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr45;
	} else
		goto tr45;
	goto tr26;
tr45:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 6134 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st19;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st19;
	} else
		goto st19;
	goto tr26;
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
	goto tr26;
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
	goto tr26;
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
	goto tr26;
tr56:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 6193 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st23;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st23;
	} else
		goto st23;
	goto tr26;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st24;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st24;
	} else
		goto st24;
	goto tr26;
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
	goto tr26;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 34u: goto tr54;
		case 92u: goto tr55;
	}
	goto tr53;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr56;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr56;
	} else
		goto tr56;
	goto tr26;
tr240:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 6259 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto st174;
		case 13u: goto tr59;
	}
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	switch( (*p) ) {
		case 10u: goto st174;
		case 13u: goto tr59;
	}
	goto st27;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
	if ( (*p) == 13u )
		goto tr59;
	goto tr284;
tr241:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 6289 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr278;
		case 13u: goto tr278;
		case 34u: goto tr278;
		case 39u: goto tr286;
		case 92u: goto tr287;
	}
	goto tr285;
tr60:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr63:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st28;
tr64:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st28;
tr66:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st28;
tr67:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st28;
tr68:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st28;
tr69:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st28;
tr70:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st28;
tr71:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st28;
tr81:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr285:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 6398 "lexer2.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr61;
		case 92u: goto st29;
	}
	goto tr60;
tr83:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st29;
tr287:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 6428 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr63;
		case 39u: goto tr64;
		case 85u: goto st30;
		case 92u: goto tr66;
		case 98u: goto tr67;
		case 102u: goto tr68;
		case 110u: goto tr69;
		case 114u: goto tr70;
		case 116u: goto tr71;
		case 117u: goto st39;
	}
	goto tr0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr73;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr73;
	} else
		goto tr73;
	goto tr0;
tr73:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 6465 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st32;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st32;
	} else
		goto st32;
	goto tr0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st33;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st33;
	} else
		goto st33;
	goto tr0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st34;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st34;
	} else
		goto st34;
	goto tr0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st35;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st35;
	} else
		goto st35;
	goto tr0;
tr84:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 6524 "lexer2.c"
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
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr0;
		case 39u: goto tr82;
		case 92u: goto tr83;
	}
	goto tr81;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr84;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr84;
	} else
		goto tr84;
	goto tr0;
tr286:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 6604 "lexer2.c"
	if ( (*p) == 39u )
		goto st40;
	goto tr288;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 39u: goto tr87;
		case 92u: goto tr88;
	}
	goto tr86;
tr89:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr94:
#line 159 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st41;
tr95:
#line 165 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st41;
tr97:
#line 171 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st41;
tr98:
#line 135 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st41;
tr99:
#line 153 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st41;
tr100:
#line 141 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st41;
tr101:
#line 147 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st41;
tr102:
#line 129 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st41;
tr112:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr86:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
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
#line 6717 "lexer2.c"
	switch( (*p) ) {
		case 39u: goto tr90;
		case 92u: goto st44;
	}
	goto tr89;
tr87:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr90:
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr113:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 97 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 6764 "lexer2.c"
	if ( (*p) == 39u )
		goto st43;
	goto tr85;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	if ( (*p) == 39u )
		goto tr93;
	goto tr85;
tr114:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st44;
tr88:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 6796 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr94;
		case 39u: goto tr95;
		case 85u: goto st45;
		case 92u: goto tr97;
		case 98u: goto tr98;
		case 102u: goto tr99;
		case 110u: goto tr100;
		case 114u: goto tr101;
		case 116u: goto tr102;
		case 117u: goto st54;
	}
	goto tr85;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr104;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr104;
	} else
		goto tr104;
	goto tr85;
tr104:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 6833 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st47;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st47;
	} else
		goto st47;
	goto tr85;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st48;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st48;
	} else
		goto st48;
	goto tr85;
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
	goto tr85;
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
	goto tr85;
tr115:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 6892 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st51;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st51;
	} else
		goto st51;
	goto tr85;
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
	goto tr85;
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
	goto tr85;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 39u: goto tr113;
		case 92u: goto tr114;
	}
	goto tr112;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr115;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr115;
	} else
		goto tr115;
	goto tr85;
tr244:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 6958 "lexer2.c"
	if ( (*p) == 46u )
		goto st55;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr247;
	goto tr278;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr0;
tr116:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 223 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 6981 "lexer2.c"
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr291;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 43u: goto st57;
		case 45u: goto st57;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr117;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr117;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st179;
	goto tr292;
tr247:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 222 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 7024 "lexer2.c"
	switch( (*p) ) {
		case 46u: goto st58;
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr247;
	goto tr293;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr120;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr295;
tr173:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 7059 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st59;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 205u: goto st86;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st87;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 58u )
				goto tr124;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr124;
		} else
			goto tr124;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto st64;
	goto tr296;
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
	goto tr122;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr124;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
tr124:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 7132 "lexer2.c"
	switch( (*p) ) {
		case 37u: goto st59;
		case 45u: goto tr124;
		case 46u: goto st61;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 204u: goto st66;
		case 205u: goto st67;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st70;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr124;
		} else if ( (*p) >= 48u )
			goto tr124;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto tr124;
	goto tr296;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 37u: goto st59;
		case 45u: goto tr124;
		case 46u: goto st61;
		case 92u: goto st62;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto st63;
		case 203u: goto st65;
		case 204u: goto st66;
		case 205u: goto st67;
		case 223u: goto st65;
		case 224u: goto st68;
		case 225u: goto st69;
		case 226u: goto st70;
		case 227u: goto st75;
		case 237u: goto st78;
		case 239u: goto st79;
		case 240u: goto st82;
		case 243u: goto st85;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 58u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr124;
		} else if ( (*p) >= 48u )
			goto tr124;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st64;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st84;
		} else
			goto st77;
	} else
		goto tr124;
	goto tr122;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 33u: goto tr124;
		case 59u: goto tr124;
		case 61u: goto tr124;
		case 95u: goto tr124;
		case 126u: goto tr124;
	}
	if ( (*p) > 47u ) {
		if ( 63u <= (*p) && (*p) <= 64u )
			goto tr124;
	} else if ( (*p) >= 35u )
		goto tr124;
	goto tr122;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr124;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	goto tr124;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	if ( 192u <= (*p) )
		goto tr122;
	goto tr124;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	if ( 128u <= (*p) )
		goto tr124;
	goto tr122;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	if ( (*p) == 190u )
		goto tr122;
	goto tr124;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) == 160u )
		goto st66;
	if ( 161u <= (*p) )
		goto st64;
	goto tr122;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) == 191u )
		goto st65;
	if ( 192u <= (*p) )
		goto tr122;
	goto st64;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	switch( (*p) ) {
		case 128u: goto st71;
		case 129u: goto st72;
		case 134u: goto st73;
		case 176u: goto st66;
		case 191u: goto st74;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st64;
	} else if ( (*p) >= 130u )
		goto st64;
	goto tr122;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 140u )
		goto tr124;
	goto tr122;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	if ( 144u <= (*p) )
		goto tr122;
	goto tr124;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	if ( 176u <= (*p) )
		goto tr122;
	goto tr124;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	if ( (*p) == 128u )
		goto st76;
	if ( 129u <= (*p) )
		goto st64;
	goto tr122;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	if ( 129u <= (*p) )
		goto tr124;
	goto tr122;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	goto st64;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) == 159u )
		goto st65;
	if ( 160u <= (*p) )
		goto tr122;
	goto st64;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	switch( (*p) ) {
		case 164u: goto st66;
		case 183u: goto st80;
		case 191u: goto st81;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st64;
	goto tr122;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( 190u <= (*p) )
		goto tr122;
	goto tr124;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( (*p) == 144u )
		goto st83;
	if ( 145u <= (*p) )
		goto st77;
	goto tr122;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) == 128u )
		goto st66;
	if ( 129u <= (*p) )
		goto st64;
	goto tr122;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	goto st77;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	if ( (*p) == 175u )
		goto st69;
	if ( 176u <= (*p) )
		goto tr122;
	goto st77;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 176u )
		goto tr124;
	goto tr122;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
	switch( (*p) ) {
		case 128u: goto st88;
		case 129u: goto st89;
		case 134u: goto st73;
		case 176u: goto st66;
		case 191u: goto st74;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st64;
	} else if ( (*p) >= 130u )
		goto st64;
	goto tr122;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr124;
	goto tr122;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
	if ( 176u <= (*p) )
		goto tr124;
	goto tr122;
tr249:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 7474 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr278;
		case 60u: goto tr278;
		case 62u: goto tr300;
		case 92u: goto tr301;
		case 94u: goto tr278;
		case 96u: goto tr278;
		case 123u: goto tr278;
		case 125u: goto tr278;
	}
	if ( (*p) <= 32u )
		goto tr278;
	goto tr299;
tr153:
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr166:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr299:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
#line 103 "turtle.rl"
	{
       // printf("%c", fc);
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
#line 7532 "lexer2.c"
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr154;
		case 92u: goto st91;
		case 94u: goto tr0;
		case 96u: goto tr0;
		case 123u: goto tr0;
		case 125u: goto tr0;
	}
	if ( (*p) <= 32u )
		goto tr0;
	goto tr153;
tr168:
#line 114 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (shared_lexer->string)
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st91;
tr301:
#line 92 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                shared_lexer->string = string_new();
    }
	goto st91;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
#line 7567 "lexer2.c"
	switch( (*p) ) {
		case 85u: goto st92;
		case 117u: goto st101;
	}
	goto tr0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr158;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr158;
	} else
		goto tr158;
	goto tr0;
tr158:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st93;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
#line 7596 "lexer2.c"
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
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st95;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st95;
	} else
		goto st95;
	goto tr0;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st96;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st96;
	} else
		goto st96;
	goto tr0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st97;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st97;
	} else
		goto st97;
	goto tr0;
tr169:
#line 110 "turtle.rl"
	{
        mark = p;
    }
	goto st97;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
#line 7655 "lexer2.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st98;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st98;
	} else
		goto st98;
	goto tr0;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st99;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st99;
	} else
		goto st99;
	goto tr0;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto st100;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto st100;
	} else
		goto st100;
	goto tr0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	switch( (*p) ) {
		case 34u: goto tr0;
		case 60u: goto tr0;
		case 62u: goto tr167;
		case 92u: goto tr168;
		case 94u: goto tr0;
		case 96u: goto tr0;
		case 123u: goto tr0;
		case 125u: goto tr0;
	}
	if ( (*p) <= 32u )
		goto tr0;
	goto tr166;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr169;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr169;
	} else
		goto tr169;
	goto tr0;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
	switch( (*p) ) {
		case 98u: goto tr303;
		case 112u: goto tr304;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr278;
tr302:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st186;
tr309:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 211 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st186;
tr314:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 209 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 7757 "lexer2.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr117;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr170;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr170;
	} else
		goto tr170;
	goto tr117;
tr170:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 7789 "lexer2.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr170;
	} else if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr170;
	} else
		goto tr170;
	goto tr306;
tr303:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st188;
st188:
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 7811 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 97u: goto tr307;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr307:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st189;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
#line 7832 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 115u: goto tr308;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr308:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 7853 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr309;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr304:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 7874 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 114u: goto tr310;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr310:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 7895 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr311;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr311:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st193;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
#line 7916 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 102u: goto tr312;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr312:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 7937 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 105u: goto tr313;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr313:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 236 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 7958 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 120u: goto tr314;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr302;
	} else if ( (*p) >= 65u )
		goto tr302;
	goto tr306;
tr198:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 212 "turtle.rl"
	{ shared_lexer->env.act = 4;}
	goto st196;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 210 "turtle.rl"
	{ shared_lexer->env.act = 2;}
	goto st196;
tr235:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 225 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st196;
tr251:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st196;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 241 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 8003 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 95u: goto st103;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr117;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st103;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st103;
	} else
		goto st103;
	goto tr117;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
	goto st103;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
	if ( 192u <= (*p) )
		goto tr117;
	goto st103;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
	if ( 128u <= (*p) )
		goto st103;
	goto tr117;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
	if ( (*p) == 190u )
		goto tr117;
	goto st103;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( (*p) == 160u )
		goto st108;
	if ( 161u <= (*p) )
		goto st106;
	goto tr117;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( (*p) == 191u )
		goto st107;
	if ( 192u <= (*p) )
		goto tr117;
	goto st106;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	switch( (*p) ) {
		case 128u: goto st113;
		case 129u: goto st114;
		case 134u: goto st115;
		case 176u: goto st108;
		case 191u: goto st116;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st106;
	} else if ( (*p) >= 130u )
		goto st106;
	goto tr117;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto st103;
	} else if ( (*p) >= 140u )
		goto st103;
	goto tr117;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto st103;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( 144u <= (*p) )
		goto tr117;
	goto st103;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	if ( 176u <= (*p) )
		goto tr117;
	goto st103;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	if ( (*p) == 128u )
		goto st118;
	if ( 129u <= (*p) )
		goto st106;
	goto tr117;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
	if ( 129u <= (*p) )
		goto st103;
	goto tr117;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
	goto st106;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
	if ( (*p) == 159u )
		goto st107;
	if ( 160u <= (*p) )
		goto tr117;
	goto st106;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
	switch( (*p) ) {
		case 164u: goto st108;
		case 183u: goto st122;
		case 191u: goto st123;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st106;
	goto tr117;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto st103;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	if ( 190u <= (*p) )
		goto tr117;
	goto st103;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	if ( (*p) == 144u )
		goto st125;
	if ( 145u <= (*p) )
		goto st119;
	goto tr117;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	if ( (*p) == 128u )
		goto st108;
	if ( 129u <= (*p) )
		goto st106;
	goto tr117;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
	goto st119;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	if ( (*p) == 175u )
		goto st111;
	if ( 176u <= (*p) )
		goto tr117;
	goto st119;
tr252:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 8328 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 65u: goto st128;
		case 95u: goto st103;
		case 97u: goto st128;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 83u: goto st129;
		case 95u: goto st103;
		case 115u: goto st129;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 69u: goto tr198;
		case 95u: goto st103;
		case 101u: goto tr198;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr253:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st198;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
#line 8467 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 82u: goto st130;
		case 95u: goto st103;
		case 114u: goto st130;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 69u: goto st131;
		case 95u: goto st103;
		case 101u: goto st131;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 70u: goto st132;
		case 95u: goto st103;
		case 102u: goto st132;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 73u: goto st133;
		case 95u: goto st103;
		case 105u: goto st133;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 88u: goto tr202;
		case 95u: goto st103;
		case 120u: goto tr202;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
	if ( (*p) == 94u )
		goto tr317;
	goto tr278;
tr257:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st200;
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
#line 8701 "lexer2.c"
	if ( (*p) == 58u )
		goto st134;
	goto tr278;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 95u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 205u: goto st159;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st160;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 196u ) {
		if ( (*p) < 65u ) {
			if ( 48u <= (*p) && (*p) <= 57u )
				goto tr203;
		} else if ( (*p) > 90u ) {
			if ( 97u <= (*p) && (*p) <= 122u )
				goto tr203;
		} else
			goto tr203;
	} else if ( (*p) > 202u ) {
		if ( (*p) < 228u ) {
			if ( 206u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto st137;
	goto tr0;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 214 "turtle.rl"
	{ shared_lexer->env.act = 5;}
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 8755 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto st135;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 204u: goto st139;
		case 205u: goto st140;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st143;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr203;
		} else if ( (*p) >= 48u )
			goto tr203;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto tr203;
	goto tr319;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto st135;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto st136;
		case 203u: goto st138;
		case 204u: goto st139;
		case 205u: goto st140;
		case 223u: goto st138;
		case 224u: goto st141;
		case 225u: goto st142;
		case 226u: goto st143;
		case 227u: goto st148;
		case 237u: goto st151;
		case 239u: goto st152;
		case 240u: goto st155;
		case 243u: goto st158;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr203;
		} else if ( (*p) >= 48u )
			goto tr203;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st137;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st157;
		} else
			goto st150;
	} else
		goto tr203;
	goto tr218;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr117;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
	goto tr203;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
	if ( 192u <= (*p) )
		goto tr117;
	goto tr203;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
	if ( 128u <= (*p) )
		goto tr203;
	goto tr117;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
	if ( (*p) == 190u )
		goto tr218;
	goto tr203;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
	if ( (*p) == 160u )
		goto st139;
	if ( 161u <= (*p) )
		goto st137;
	goto tr117;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
	if ( (*p) == 191u )
		goto st138;
	if ( 192u <= (*p) )
		goto tr117;
	goto st137;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
	switch( (*p) ) {
		case 128u: goto st144;
		case 129u: goto st145;
		case 134u: goto st146;
		case 176u: goto st139;
		case 191u: goto st147;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st137;
	} else if ( (*p) >= 130u )
		goto st137;
	goto tr218;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 140u )
		goto tr203;
	goto tr218;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
	if ( 144u <= (*p) )
		goto tr117;
	goto tr203;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
	if ( 176u <= (*p) )
		goto tr117;
	goto tr203;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
	if ( (*p) == 128u )
		goto st149;
	if ( 129u <= (*p) )
		goto st137;
	goto tr117;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
	if ( 129u <= (*p) )
		goto tr203;
	goto tr117;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
	goto st137;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
	if ( (*p) == 159u )
		goto st138;
	if ( 160u <= (*p) )
		goto tr117;
	goto st137;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
	switch( (*p) ) {
		case 164u: goto st139;
		case 183u: goto st153;
		case 191u: goto st154;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st137;
	goto tr117;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr203;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
	if ( 190u <= (*p) )
		goto tr117;
	goto tr203;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
	if ( (*p) == 144u )
		goto st156;
	if ( 145u <= (*p) )
		goto st150;
	goto tr117;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
	if ( (*p) == 128u )
		goto st139;
	if ( 129u <= (*p) )
		goto st137;
	goto tr117;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
	goto st150;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
	if ( (*p) == 175u )
		goto st142;
	if ( 176u <= (*p) )
		goto tr117;
	goto st150;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 176u )
		goto tr203;
	goto tr0;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
	switch( (*p) ) {
		case 128u: goto st161;
		case 129u: goto st162;
		case 134u: goto st146;
		case 176u: goto st139;
		case 191u: goto st147;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st137;
	} else if ( (*p) >= 130u )
		goto st137;
	goto tr0;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr203;
	goto tr0;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
	if ( 176u <= (*p) )
		goto tr203;
	goto tr0;
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 9078 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 97u: goto st163;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 108u: goto st164;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 115u: goto st165;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 101u: goto tr235;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 9257 "lexer2.c"
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 114u: goto st166;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr278;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
	switch( (*p) ) {
		case 45u: goto st103;
		case 46u: goto st104;
		case 58u: goto tr173;
		case 95u: goto st103;
		case 117u: goto st165;
		case 183u: goto st103;
		case 195u: goto st105;
		case 203u: goto st107;
		case 204u: goto st108;
		case 205u: goto st109;
		case 223u: goto st107;
		case 224u: goto st110;
		case 225u: goto st111;
		case 226u: goto st112;
		case 227u: goto st117;
		case 237u: goto st120;
		case 239u: goto st121;
		case 240u: goto st124;
		case 243u: goto st127;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto st103;
		} else if ( (*p) >= 48u )
			goto st103;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto st106;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto st126;
		} else
			goto st119;
	} else
		goto st103;
	goto tr0;
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st204;
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
#line 9350 "lexer2.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto st103;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto st103;
	} else
		goto st103;
	goto tr278;
tr264:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 9370 "lexer2.c"
	goto st103;
tr265:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 9382 "lexer2.c"
	if ( 192u <= (*p) )
		goto tr278;
	goto st103;
tr266:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 9396 "lexer2.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto st103;
	} else if ( (*p) >= 176u )
		goto st103;
	goto tr278;
tr267:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 9413 "lexer2.c"
	if ( (*p) == 160u )
		goto st108;
	if ( 161u <= (*p) )
		goto st106;
	goto tr278;
tr268:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 9429 "lexer2.c"
	if ( (*p) == 191u )
		goto st107;
	if ( 192u <= (*p) )
		goto tr278;
	goto st106;
tr269:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 9445 "lexer2.c"
	switch( (*p) ) {
		case 128u: goto st167;
		case 129u: goto st168;
		case 134u: goto st115;
		case 176u: goto st108;
		case 191u: goto st116;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto st106;
	} else if ( (*p) >= 130u )
		goto st106;
	goto tr278;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
	if ( 140u <= (*p) && (*p) <= 141u )
		goto st103;
	goto tr0;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
	if ( 176u <= (*p) )
		goto st103;
	goto tr0;
tr270:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 9483 "lexer2.c"
	if ( (*p) == 128u )
		goto st118;
	if ( 129u <= (*p) )
		goto st106;
	goto tr278;
tr271:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 9499 "lexer2.c"
	goto st106;
tr272:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 9511 "lexer2.c"
	if ( (*p) == 159u )
		goto st107;
	if ( 160u <= (*p) )
		goto tr278;
	goto st106;
tr273:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 9527 "lexer2.c"
	switch( (*p) ) {
		case 164u: goto st108;
		case 183u: goto st122;
		case 191u: goto st123;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto st106;
	goto tr278;
tr274:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 9546 "lexer2.c"
	if ( (*p) == 144u )
		goto st125;
	if ( 145u <= (*p) )
		goto st119;
	goto tr278;
tr275:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 9562 "lexer2.c"
	goto st119;
tr276:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 267 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 9574 "lexer2.c"
	if ( (*p) == 175u )
		goto st111;
	if ( 176u <= (*p) )
		goto tr278;
	goto st119;
	}
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
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
	_test_eof10:  shared_lexer->env.cs = 10; goto _test_eof; 
	_test_eof11:  shared_lexer->env.cs = 11; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
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
	_test_eof23:  shared_lexer->env.cs = 23; goto _test_eof; 
	_test_eof24:  shared_lexer->env.cs = 24; goto _test_eof; 
	_test_eof25:  shared_lexer->env.cs = 25; goto _test_eof; 
	_test_eof26:  shared_lexer->env.cs = 26; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof28:  shared_lexer->env.cs = 28; goto _test_eof; 
	_test_eof29:  shared_lexer->env.cs = 29; goto _test_eof; 
	_test_eof30:  shared_lexer->env.cs = 30; goto _test_eof; 
	_test_eof31:  shared_lexer->env.cs = 31; goto _test_eof; 
	_test_eof32:  shared_lexer->env.cs = 32; goto _test_eof; 
	_test_eof33:  shared_lexer->env.cs = 33; goto _test_eof; 
	_test_eof34:  shared_lexer->env.cs = 34; goto _test_eof; 
	_test_eof35:  shared_lexer->env.cs = 35; goto _test_eof; 
	_test_eof36:  shared_lexer->env.cs = 36; goto _test_eof; 
	_test_eof37:  shared_lexer->env.cs = 37; goto _test_eof; 
	_test_eof38:  shared_lexer->env.cs = 38; goto _test_eof; 
	_test_eof39:  shared_lexer->env.cs = 39; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof40:  shared_lexer->env.cs = 40; goto _test_eof; 
	_test_eof41:  shared_lexer->env.cs = 41; goto _test_eof; 
	_test_eof42:  shared_lexer->env.cs = 42; goto _test_eof; 
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
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
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
	_test_eof82:  shared_lexer->env.cs = 82; goto _test_eof; 
	_test_eof83:  shared_lexer->env.cs = 83; goto _test_eof; 
	_test_eof84:  shared_lexer->env.cs = 84; goto _test_eof; 
	_test_eof85:  shared_lexer->env.cs = 85; goto _test_eof; 
	_test_eof86:  shared_lexer->env.cs = 86; goto _test_eof; 
	_test_eof87:  shared_lexer->env.cs = 87; goto _test_eof; 
	_test_eof88:  shared_lexer->env.cs = 88; goto _test_eof; 
	_test_eof89:  shared_lexer->env.cs = 89; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
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
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
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
	_test_eof124:  shared_lexer->env.cs = 124; goto _test_eof; 
	_test_eof125:  shared_lexer->env.cs = 125; goto _test_eof; 
	_test_eof126:  shared_lexer->env.cs = 126; goto _test_eof; 
	_test_eof127:  shared_lexer->env.cs = 127; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
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
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
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
	case 170: goto tr277;
	case 171: goto tr278;
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
	case 10: goto tr0;
	case 11: goto tr0;
	case 172: goto tr282;
	case 12: goto tr26;
	case 13: goto tr26;
	case 14: goto tr26;
	case 15: goto tr26;
	case 16: goto tr26;
	case 17: goto tr26;
	case 18: goto tr26;
	case 19: goto tr26;
	case 20: goto tr26;
	case 21: goto tr26;
	case 22: goto tr26;
	case 23: goto tr26;
	case 24: goto tr26;
	case 25: goto tr26;
	case 26: goto tr26;
	case 173: goto tr278;
	case 27: goto tr0;
	case 174: goto tr284;
	case 175: goto tr278;
	case 28: goto tr0;
	case 29: goto tr0;
	case 30: goto tr0;
	case 31: goto tr0;
	case 32: goto tr0;
	case 33: goto tr0;
	case 34: goto tr0;
	case 35: goto tr0;
	case 36: goto tr0;
	case 37: goto tr0;
	case 38: goto tr0;
	case 39: goto tr0;
	case 176: goto tr288;
	case 40: goto tr85;
	case 41: goto tr85;
	case 42: goto tr85;
	case 43: goto tr85;
	case 44: goto tr85;
	case 45: goto tr85;
	case 46: goto tr85;
	case 47: goto tr85;
	case 48: goto tr85;
	case 49: goto tr85;
	case 50: goto tr85;
	case 51: goto tr85;
	case 52: goto tr85;
	case 53: goto tr85;
	case 54: goto tr85;
	case 177: goto tr278;
	case 55: goto tr0;
	case 178: goto tr291;
	case 56: goto tr117;
	case 57: goto tr117;
	case 179: goto tr292;
	case 180: goto tr293;
	case 58: goto tr120;
	case 181: goto tr295;
	case 182: goto tr296;
	case 59: goto tr122;
	case 60: goto tr122;
	case 183: goto tr296;
	case 61: goto tr122;
	case 62: goto tr122;
	case 63: goto tr122;
	case 64: goto tr122;
	case 65: goto tr122;
	case 66: goto tr122;
	case 67: goto tr122;
	case 68: goto tr122;
	case 69: goto tr122;
	case 70: goto tr122;
	case 71: goto tr122;
	case 72: goto tr122;
	case 73: goto tr122;
	case 74: goto tr122;
	case 75: goto tr122;
	case 76: goto tr122;
	case 77: goto tr122;
	case 78: goto tr122;
	case 79: goto tr122;
	case 80: goto tr122;
	case 81: goto tr122;
	case 82: goto tr122;
	case 83: goto tr122;
	case 84: goto tr122;
	case 85: goto tr122;
	case 86: goto tr122;
	case 87: goto tr122;
	case 88: goto tr122;
	case 89: goto tr122;
	case 184: goto tr278;
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
	case 185: goto tr278;
	case 186: goto tr117;
	case 102: goto tr117;
	case 187: goto tr306;
	case 188: goto tr306;
	case 189: goto tr306;
	case 190: goto tr306;
	case 191: goto tr306;
	case 192: goto tr306;
	case 193: goto tr306;
	case 194: goto tr306;
	case 195: goto tr306;
	case 196: goto tr117;
	case 103: goto tr117;
	case 104: goto tr117;
	case 105: goto tr117;
	case 106: goto tr117;
	case 107: goto tr117;
	case 108: goto tr117;
	case 109: goto tr117;
	case 110: goto tr117;
	case 111: goto tr117;
	case 112: goto tr117;
	case 113: goto tr117;
	case 114: goto tr117;
	case 115: goto tr117;
	case 116: goto tr117;
	case 117: goto tr117;
	case 118: goto tr117;
	case 119: goto tr117;
	case 120: goto tr117;
	case 121: goto tr117;
	case 122: goto tr117;
	case 123: goto tr117;
	case 124: goto tr117;
	case 125: goto tr117;
	case 126: goto tr117;
	case 127: goto tr117;
	case 197: goto tr278;
	case 128: goto tr0;
	case 129: goto tr0;
	case 198: goto tr278;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 199: goto tr278;
	case 200: goto tr278;
	case 134: goto tr0;
	case 201: goto tr319;
	case 135: goto tr218;
	case 136: goto tr117;
	case 137: goto tr117;
	case 138: goto tr117;
	case 139: goto tr117;
	case 140: goto tr218;
	case 141: goto tr117;
	case 142: goto tr117;
	case 143: goto tr218;
	case 144: goto tr218;
	case 145: goto tr218;
	case 146: goto tr117;
	case 147: goto tr117;
	case 148: goto tr117;
	case 149: goto tr117;
	case 150: goto tr117;
	case 151: goto tr117;
	case 152: goto tr117;
	case 153: goto tr117;
	case 154: goto tr117;
	case 155: goto tr117;
	case 156: goto tr117;
	case 157: goto tr117;
	case 158: goto tr117;
	case 159: goto tr0;
	case 160: goto tr0;
	case 161: goto tr0;
	case 162: goto tr0;
	case 202: goto tr278;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 203: goto tr278;
	case 166: goto tr0;
	case 204: goto tr278;
	case 205: goto tr278;
	case 206: goto tr278;
	case 207: goto tr278;
	case 208: goto tr278;
	case 209: goto tr278;
	case 210: goto tr278;
	case 167: goto tr0;
	case 168: goto tr0;
	case 211: goto tr278;
	case 212: goto tr278;
	case 213: goto tr278;
	case 214: goto tr278;
	case 215: goto tr278;
	case 216: goto tr278;
	case 217: goto tr278;
	}
	}

	}

#line 523 "turtle.rl"
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

