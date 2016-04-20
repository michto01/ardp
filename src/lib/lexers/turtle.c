
#line 1 "turtle.rl"
/*! @file turtle.rl
 *
 *  Base file for the `Ragel's` preprocessor. It builds up on the lexer.h abstract
 *  interface.
 *
 *  @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 *  @date   2015
 *
 *  @TODO column guard
 *  @FIXME Not all the linebreaks are being counted, causing misaligned lines
 */

/* vim: set ts=8 sw=4 tw=0 noet : set foldmethod=marker */

// HEADERS {{{ --------------------------------------------------------------------
#include "config.h"
#include <unistd.h>             /* sleep() */
#include <errno.h>              /* Included for 'stderr' */
#include <ctype.h>              /* isalnum() ... */
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
    }
    return i;
}

/*!
 * Shorthand to speedup token input.
 */
#define var( s ) ( shared_lexer->env.s )

/* clang-format off */
/* ragel-machine() {{{*/

#line 79 "turtle.c"
static const int turtle_start = 188;
static const int turtle_first_final = 188;
static const int turtle_error = -1;

static const int turtle_en_main = 188;


#line 277 "turtle.rl"

/* }}} */
/* clang-format on */

/*--------------------------------------------------------------------------------*/
// LOCAL FUNCTIONS, HELPERS, SHORTHANDS
/*--------------------------------------------------------------------------------*/
/* log() {{{ */
static void log(int level, const char* message)
{
    if (shared_lexer->log.level >= level )
        if (shared_lexer->log.eprintf) {
                shared_lexer->log.eprintf(level, message);
        }
}
/*}}}*/
/* lexer_emit_u8_token() {{{*/
static void lexer_emit_u8_token(enum turtle_token_type type)
{
        if (likely(shared_lexer->cb.stoken)) {
                /*__block*/ utf8 s;
                if (shared_lexer->string)
                         s = string_copy(shared_lexer->string);
                else
                         s = NULL;
                dispatch_async( shared_lexer->event_queue, ^{
                        shared_lexer->cb.stoken(type, s, shared_lexer->line, 0);
                        log (DEBUG, "Emitted token (UTF8)");
                });
        }
}
/*}}}*/
/* lexer_emit_token() {{{ */
static void lexer_emit_token( enum turtle_token_type type, uint8_t *_Nullable str, size_t len )
{
        if (likely(shared_lexer->cb.stoken)) {
                // dispatch_barrier_async(shared_lexer->lexer_queue, ^{
                        utf8 s = string_create_n(str,len,len);

                        dispatch_async( shared_lexer->event_queue, ^{
                                shared_lexer->cb.stoken( type, s, shared_lexer->line, 0);
                                log( DEBUG, "Token emmitted" );
                        });
                // });
        }
}
/*}}}*/
/* lexer_emit_token_const() {{{ */
static void lexer_emit_token_const( enum turtle_token_type type )
{
        if ( likely(shared_lexer->cb.stoken) ) {
            dispatch_async( shared_lexer->event_queue, ^{
                shared_lexer->cb.stoken( type, NULL, shared_lexer->line, 0 );
                log( DEBUG, "Token emmitted");
            });
        }
}
/*}}}*/
/* ardp_lexer_turtle_debug() {{{ */
void ardp_lexer_turtle_debug( void )
{
        const char states[][15] = {"created", "initialized", "ready", "unknown"};
        fprintf(stderr, "Shared lexer %s created.\n", shared_lexer ? "is" : "isn't" );
        fprintf(stderr, "It's state is: %s\n", states[shared_lexer->state] );
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

        shared_lexer = calloc(1, sizeof(struct lexer)); // malloc(sizeof(struct lexer));

        if ( unlikely(shared_lexer is NULL) ) {
            log( ERROR, "Didn't allocate the shared lexer. (MALLOC ERROR)");
            //return ARDP_ERROR_LEXER_MALLOC;
            return ARDP_FAILURE;
        }

        shared_lexer->lexer_queue =
            dispatch_queue_create( "eu.cre8iv.ardp.Lexer", 0  /*DISPATCH_QUEUE_SERIAL*/ );

#if ARDP_ON_OSX
        shared_lexer->event_queue =
            dispatch_queue_create( "eu.cre8iv.ardp.Events", DISPATCH_QUEUE_SERIAL );
#else
#define DISPATCH_QUEUE_SERIAL NULL
        shared_lexer->event_queue =
                dispatch_queue_create("eu.cre8iv.ardp.Events", NULL);
#endif
        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_CREATED;
        shared_lexer->finished = 1;

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
                      this->line  = 1;
                      this->state = ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED;

        column = NULL;

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
        
#line 243 "turtle.c"
	{
	 shared_lexer->env.cs = turtle_start;
	 shared_lexer->env.ts = 0;
	 shared_lexer->env.te = 0;
	 shared_lexer->env.act = 0;
	}

#line 432 "turtle.rl"
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
                if (shared_lexer->string)
                        string_dealloc(shared_lexer->string);
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

    /*__block*/ uint8_t *_Nullable p  = v; /* Indirection to allow the GCD */
    const   uint8_t *_Nullable pe = p + len; /* Points to fist byte beyond the data */
    const   uint8_t *_Nullable eof= (is_eof) ? pe : NULL; /* Indicated end of all data*/

    /* clang-format off */
        
#line 314 "turtle.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 273 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st188;
tr2:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr23:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr26:
#line 237 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr34:
#line 234 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr58:
#line 266 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr61:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr82:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr85:
#line 238 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr93:
#line 235 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
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
	goto st188;
tr120:
#line 229 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr122:
#line 225 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(QNAME); }}
	goto st188;
tr154:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr167:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr218:
#line 224 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(BLANK_LITERAL); }}
	goto st188;
tr255:
#line 273 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st188;
tr256:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr261:
#line 255 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st188;
tr262:
#line 256 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st188;
tr264:
#line 250 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st188;
tr268:
#line 251 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st188;
tr274:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st188;
tr275:
#line 254 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st188;
tr281:
#line 257 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st188;
tr282:
#line 258 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st188;
tr297:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr298:
#line 273 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st188;
tr302:
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr304:
#line 266 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr308:
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr311:
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr312:
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr313:
#line 229 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr315:
#line 249 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st188;
tr316:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 225 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(QNAME); }}
	goto st188;
tr320:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr326:
#line 243 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st188;
tr337:
#line 240 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st188;
tr339:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 224 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(BLANK_LITERAL); }}
	goto st188;
st188:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 628 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr256;
		case 13u: goto st189;
		case 34u: goto tr258;
		case 35u: goto tr259;
		case 39u: goto tr260;
		case 40u: goto tr261;
		case 41u: goto tr262;
		case 44u: goto tr264;
		case 46u: goto st200;
		case 58u: goto tr267;
		case 59u: goto tr268;
		case 60u: goto tr269;
		case 64u: goto st204;
		case 66u: goto tr272;
		case 80u: goto tr273;
		case 91u: goto tr274;
		case 93u: goto tr275;
		case 94u: goto st218;
		case 95u: goto tr277;
		case 97u: goto tr278;
		case 98u: goto tr272;
		case 102u: goto tr279;
		case 112u: goto tr273;
		case 116u: goto tr280;
		case 123u: goto tr281;
		case 125u: goto tr282;
		case 195u: goto tr283;
		case 203u: goto tr285;
		case 205u: goto tr286;
		case 223u: goto tr285;
		case 224u: goto tr287;
		case 225u: goto tr288;
		case 226u: goto tr289;
		case 227u: goto tr290;
		case 237u: goto tr292;
		case 239u: goto tr293;
		case 240u: goto tr294;
		case 243u: goto tr296;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr263;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr271;
		} else
			goto tr266;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr284;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr295;
			} else if ( (*p) >= 228u )
				goto tr291;
		} else
			goto tr284;
	} else
		goto tr271;
	goto tr255;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
	if ( (*p) == 10u )
		goto tr256;
	goto tr297;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 708 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr298;
		case 13u: goto tr298;
		case 34u: goto tr300;
		case 92u: goto tr301;
	}
	goto tr299;
tr1:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr4:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st0;
tr5:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st0;
tr7:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st0;
tr8:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st0;
tr9:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st0;
tr10:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st0;
tr11:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st0;
tr12:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st0;
tr22:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr299:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
#line 814 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 92u: goto st1;
	}
	goto tr1;
tr24:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st1;
tr301:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 844 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 881 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 940 "turtle.c"
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
tr300:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 1020 "turtle.c"
	if ( (*p) == 34u )
		goto st12;
	goto tr302;
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr35:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st13;
tr36:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st13;
tr38:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st13;
tr39:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st13;
tr40:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st13;
tr41:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st13;
tr42:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st13;
tr43:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st13;
tr53:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr27:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 1131 "turtle.c"
	switch( (*p) ) {
		case 34u: goto tr31;
		case 92u: goto st16;
	}
	goto tr30;
tr28:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr31:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr54:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
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
#line 1179 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st16;
tr29:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 1212 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 1249 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 1308 "turtle.c"
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
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 1374 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr58;
		case 13u: goto st193;
	}
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	switch( (*p) ) {
		case 10u: goto tr58;
		case 13u: goto st193;
	}
	goto st27;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
	if ( (*p) == 10u )
		goto tr58;
	goto tr304;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 1404 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr298;
		case 13u: goto tr298;
		case 39u: goto tr306;
		case 92u: goto tr307;
	}
	goto tr305;
tr60:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr63:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st28;
tr64:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st28;
tr66:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st28;
tr67:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st28;
tr68:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st28;
tr69:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st28;
tr70:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st28;
tr71:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st28;
tr81:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr305:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 1510 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 39u: goto tr61;
		case 92u: goto st29;
	}
	goto tr60;
tr83:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st29;
tr307:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 1540 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 1577 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 1636 "turtle.c"
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
tr306:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 1716 "turtle.c"
	if ( (*p) == 39u )
		goto st40;
	goto tr308;
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr94:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st41;
tr95:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st41;
tr97:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st41;
tr98:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st41;
tr99:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st41;
tr100:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st41;
tr101:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st41;
tr102:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st41;
tr112:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr86:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 1827 "turtle.c"
	switch( (*p) ) {
		case 39u: goto tr90;
		case 92u: goto st44;
	}
	goto tr89;
tr87:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr90:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr113:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
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
#line 1875 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st44;
tr88:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 1908 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 1945 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 2004 "turtle.c"
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
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 2070 "turtle.c"
	if ( (*p) == 46u )
		goto st55;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr266;
	goto tr298;
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
#line 230 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 2093 "turtle.c"
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr311;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 43u: goto st57;
		case 45u: goto st57;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr117;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr117;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr312;
tr266:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 229 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 2136 "turtle.c"
	switch( (*p) ) {
		case 46u: goto st58;
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr266;
	goto tr313;
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
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr315;
tr173:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st201;
tr267:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 2193 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 205u: goto tr317;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr318;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr129;
	goto tr316;
tr125:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 2244 "turtle.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr123;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr123;
	} else
		goto tr123;
	goto tr122;
tr123:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 2266 "turtle.c"
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 2290 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 45u: goto tr124;
		case 46u: goto tr126;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 204u: goto tr131;
		case 205u: goto tr132;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr135;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr124;
	goto tr316;
tr126:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st61;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
#line 2342 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 45u: goto tr124;
		case 46u: goto tr126;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 204u: goto tr131;
		case 205u: goto tr132;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr135;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr124;
	goto tr122;
tr127:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 2394 "turtle.c"
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
tr128:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st63;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
#line 2420 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr124;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
tr129:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st64;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
#line 2442 "turtle.c"
	goto tr124;
tr130:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 2456 "turtle.c"
	if ( 192u <= (*p) )
		goto tr122;
	goto tr124;
tr131:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 2472 "turtle.c"
	if ( (*p) <= 127u )
		goto tr122;
	goto tr124;
tr132:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st67;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
#line 2488 "turtle.c"
	if ( (*p) == 190u )
		goto tr122;
	goto tr124;
tr133:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st68;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
#line 2504 "turtle.c"
	if ( (*p) == 160u )
		goto tr131;
	if ( 161u <= (*p) )
		goto tr129;
	goto tr122;
tr134:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st69;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
#line 2522 "turtle.c"
	if ( (*p) == 191u )
		goto tr130;
	if ( 192u <= (*p) )
		goto tr122;
	goto tr129;
tr135:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st70;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
#line 2540 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr143;
		case 129u: goto tr144;
		case 134u: goto tr145;
		case 176u: goto tr131;
		case 191u: goto tr146;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr129;
	} else if ( (*p) >= 130u )
		goto tr129;
	goto tr122;
tr143:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st71;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
#line 2566 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 140u )
		goto tr124;
	goto tr122;
tr144:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st72;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
#line 2585 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
tr145:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st73;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
#line 2601 "turtle.c"
	if ( 144u <= (*p) )
		goto tr122;
	goto tr124;
tr146:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 2617 "turtle.c"
	if ( 176u <= (*p) )
		goto tr122;
	goto tr124;
tr136:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st75;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
#line 2633 "turtle.c"
	if ( (*p) == 128u )
		goto tr147;
	if ( 129u <= (*p) )
		goto tr129;
	goto tr122;
tr147:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st76;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
#line 2651 "turtle.c"
	if ( 129u <= (*p) )
		goto tr124;
	goto tr122;
tr137:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st77;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
#line 2667 "turtle.c"
	goto tr129;
tr138:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st78;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
#line 2681 "turtle.c"
	if ( (*p) == 159u )
		goto tr130;
	if ( 160u <= (*p) )
		goto tr122;
	goto tr129;
tr139:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st79;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
#line 2699 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr131;
		case 183u: goto tr148;
		case 191u: goto tr149;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr129;
	goto tr122;
tr148:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st80;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
#line 2720 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
tr149:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st81;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
#line 2736 "turtle.c"
	if ( 190u <= (*p) )
		goto tr122;
	goto tr124;
tr140:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st82;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
#line 2752 "turtle.c"
	if ( (*p) == 144u )
		goto tr150;
	if ( 145u <= (*p) )
		goto tr137;
	goto tr122;
tr150:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
#line 2770 "turtle.c"
	if ( (*p) == 128u )
		goto tr131;
	if ( 129u <= (*p) )
		goto tr129;
	goto tr122;
tr141:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st84;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
#line 2788 "turtle.c"
	goto tr137;
tr142:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st85;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
#line 2802 "turtle.c"
	if ( (*p) == 175u )
		goto tr134;
	if ( 176u <= (*p) )
		goto tr122;
	goto tr137;
tr317:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st86;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 2820 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 176u )
		goto tr124;
	goto tr122;
tr318:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st87;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
#line 2839 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr151;
		case 129u: goto tr152;
		case 134u: goto tr145;
		case 176u: goto tr131;
		case 191u: goto tr146;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr129;
	} else if ( (*p) >= 130u )
		goto tr129;
	goto tr122;
tr151:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st88;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
#line 2865 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr124;
	goto tr122;
tr152:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st89;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
#line 2881 "turtle.c"
	if ( 176u <= (*p) )
		goto tr124;
	goto tr122;
tr269:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 2893 "turtle.c"
	switch( (*p) ) {
		case 34u: goto tr298;
		case 60u: goto tr298;
		case 62u: goto tr320;
		case 92u: goto tr321;
		case 94u: goto tr298;
		case 96u: goto tr298;
		case 123u: goto tr298;
		case 125u: goto tr298;
	}
	if ( (*p) <= 32u )
		goto tr298;
	goto tr319;
tr153:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr166:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr319:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
#line 2949 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st91;
tr321:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st91;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
#line 2985 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st93;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
#line 3014 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st97;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
#line 3073 "turtle.c"
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
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	switch( (*p) ) {
		case 98u: goto tr323;
		case 112u: goto tr324;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr298;
tr322:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st205;
tr329:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 221 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st205;
tr334:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 219 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 3175 "turtle.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
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
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 3207 "turtle.c"
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
	goto tr326;
tr323:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 3229 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 97u: goto tr327;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr327:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 3250 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 115u: goto tr328;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr328:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 3271 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr329;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr324:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 3292 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 114u: goto tr330;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr330:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 3313 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr331;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr331:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 3334 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 102u: goto tr332;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr332:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 3355 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 105u: goto tr333;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr333:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 3376 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 120u: goto tr334;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr198:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 222 "turtle.rl"
	{ shared_lexer->env.act = 4;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st215;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 220 "turtle.rl"
	{ shared_lexer->env.act = 2;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st215;
tr254:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 232 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st215;
tr271:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
tr278:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 248 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 3463 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr171:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st103;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
#line 3514 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr172:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st104;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
#line 3565 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr174:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st105;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
#line 3615 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr171;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr171;
	} else
		goto tr171;
	goto tr117;
tr175:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st106;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
#line 3637 "turtle.c"
	goto tr171;
tr176:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st107;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
#line 3651 "turtle.c"
	if ( 192u <= (*p) )
		goto tr117;
	goto tr171;
tr177:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st108;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
#line 3667 "turtle.c"
	if ( (*p) <= 127u )
		goto tr117;
	goto tr171;
tr178:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st109;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
#line 3683 "turtle.c"
	if ( (*p) == 190u )
		goto tr117;
	goto tr171;
tr179:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st110;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
#line 3699 "turtle.c"
	if ( (*p) == 160u )
		goto tr177;
	if ( 161u <= (*p) )
		goto tr175;
	goto tr117;
tr180:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st111;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
#line 3717 "turtle.c"
	if ( (*p) == 191u )
		goto tr176;
	if ( 192u <= (*p) )
		goto tr117;
	goto tr175;
tr181:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st112;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
#line 3735 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr189;
		case 129u: goto tr190;
		case 134u: goto tr191;
		case 176u: goto tr177;
		case 191u: goto tr192;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr175;
	} else if ( (*p) >= 130u )
		goto tr175;
	goto tr117;
tr189:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st113;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
#line 3761 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr171;
	} else if ( (*p) >= 140u )
		goto tr171;
	goto tr117;
tr190:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st114;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
#line 3780 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr171;
tr191:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st115;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
#line 3796 "turtle.c"
	if ( 144u <= (*p) )
		goto tr117;
	goto tr171;
tr192:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st116;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
#line 3812 "turtle.c"
	if ( 176u <= (*p) )
		goto tr117;
	goto tr171;
tr182:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st117;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
#line 3828 "turtle.c"
	if ( (*p) == 128u )
		goto tr193;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr117;
tr193:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st118;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
#line 3846 "turtle.c"
	if ( 129u <= (*p) )
		goto tr171;
	goto tr117;
tr183:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st119;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
#line 3862 "turtle.c"
	goto tr175;
tr184:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st120;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
#line 3876 "turtle.c"
	if ( (*p) == 159u )
		goto tr176;
	if ( 160u <= (*p) )
		goto tr117;
	goto tr175;
tr185:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st121;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
#line 3894 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr177;
		case 183u: goto tr194;
		case 191u: goto tr195;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr175;
	goto tr117;
tr194:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st122;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
#line 3915 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr171;
tr195:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st123;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
#line 3931 "turtle.c"
	if ( 190u <= (*p) )
		goto tr117;
	goto tr171;
tr186:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st124;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
#line 3947 "turtle.c"
	if ( (*p) == 144u )
		goto tr196;
	if ( 145u <= (*p) )
		goto tr183;
	goto tr117;
tr196:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st125;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
#line 3965 "turtle.c"
	if ( (*p) == 128u )
		goto tr177;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr117;
tr187:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st126;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
#line 3983 "turtle.c"
	goto tr183;
tr188:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st127;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
#line 3997 "turtle.c"
	if ( (*p) == 175u )
		goto tr180;
	if ( 176u <= (*p) )
		goto tr117;
	goto tr183;
tr272:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 4025 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 65u: goto tr335;
		case 95u: goto tr171;
		case 97u: goto tr335;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr335:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st128;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
#line 4078 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 83u: goto tr197;
		case 95u: goto tr171;
		case 115u: goto tr197;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr197:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st129;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
#line 4131 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 69u: goto tr198;
		case 95u: goto tr171;
		case 101u: goto tr198;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr273:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 4194 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 82u: goto tr336;
		case 95u: goto tr171;
		case 114u: goto tr336;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr336:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st130;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
#line 4247 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 69u: goto tr199;
		case 95u: goto tr171;
		case 101u: goto tr199;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr199:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st131;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
#line 4300 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 70u: goto tr200;
		case 95u: goto tr171;
		case 102u: goto tr200;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr200:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st132;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
#line 4353 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 73u: goto tr201;
		case 95u: goto tr171;
		case 105u: goto tr201;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr201:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st133;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
#line 4406 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 88u: goto tr202;
		case 95u: goto tr171;
		case 120u: goto tr202;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
	if ( (*p) == 94u )
		goto tr337;
	goto tr298;
tr277:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st219;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
#line 4468 "turtle.c"
	if ( (*p) == 58u )
		goto st134;
	goto tr298;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 95u: goto tr203;
		case 195u: goto tr204;
		case 203u: goto tr206;
		case 205u: goto tr207;
		case 223u: goto tr206;
		case 224u: goto tr208;
		case 225u: goto tr209;
		case 226u: goto tr210;
		case 227u: goto tr211;
		case 237u: goto tr213;
		case 239u: goto tr214;
		case 240u: goto tr215;
		case 243u: goto tr217;
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
				goto tr205;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr216;
		} else
			goto tr212;
	} else
		goto tr205;
	goto tr0;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st220;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
#line 4526 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto tr219;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto tr220;
		case 203u: goto tr222;
		case 204u: goto tr223;
		case 205u: goto tr224;
		case 223u: goto tr222;
		case 224u: goto tr225;
		case 225u: goto tr226;
		case 226u: goto tr227;
		case 227u: goto tr228;
		case 237u: goto tr230;
		case 239u: goto tr231;
		case 240u: goto tr232;
		case 243u: goto tr234;
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
				goto tr221;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr233;
		} else
			goto tr229;
	} else
		goto tr203;
	goto tr339;
tr219:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st135;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
#line 4576 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto tr219;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto tr220;
		case 203u: goto tr222;
		case 204u: goto tr223;
		case 205u: goto tr224;
		case 223u: goto tr222;
		case 224u: goto tr225;
		case 225u: goto tr226;
		case 226u: goto tr227;
		case 227u: goto tr228;
		case 237u: goto tr230;
		case 239u: goto tr231;
		case 240u: goto tr232;
		case 243u: goto tr234;
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
				goto tr221;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr233;
		} else
			goto tr229;
	} else
		goto tr203;
	goto tr218;
tr220:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st136;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
#line 4626 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr218;
tr221:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st137;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
#line 4648 "turtle.c"
	goto tr203;
tr222:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st138;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
#line 4662 "turtle.c"
	if ( 192u <= (*p) )
		goto tr218;
	goto tr203;
tr223:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st139;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
#line 4678 "turtle.c"
	if ( 128u <= (*p) )
		goto tr203;
	goto tr218;
tr224:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st140;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
#line 4694 "turtle.c"
	if ( (*p) == 190u )
		goto tr218;
	goto tr203;
tr225:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st141;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
#line 4710 "turtle.c"
	if ( (*p) == 160u )
		goto tr223;
	if ( 161u <= (*p) )
		goto tr221;
	goto tr218;
tr226:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st142;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
#line 4728 "turtle.c"
	if ( (*p) == 191u )
		goto tr222;
	if ( 192u <= (*p) )
		goto tr218;
	goto tr221;
tr227:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st143;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
#line 4746 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr235;
		case 129u: goto tr236;
		case 134u: goto tr237;
		case 176u: goto tr223;
		case 191u: goto tr238;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr221;
	} else if ( (*p) >= 130u )
		goto tr221;
	goto tr218;
tr235:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st144;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
#line 4772 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 140u )
		goto tr203;
	goto tr218;
tr236:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st145;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
#line 4791 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
tr237:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st146;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
#line 4807 "turtle.c"
	if ( 144u <= (*p) )
		goto tr218;
	goto tr203;
tr238:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st147;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
#line 4823 "turtle.c"
	if ( 176u <= (*p) )
		goto tr218;
	goto tr203;
tr228:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st148;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
#line 4839 "turtle.c"
	if ( (*p) == 128u )
		goto tr239;
	if ( 129u <= (*p) )
		goto tr221;
	goto tr218;
tr239:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st149;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
#line 4857 "turtle.c"
	if ( 129u <= (*p) )
		goto tr203;
	goto tr218;
tr229:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st150;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
#line 4873 "turtle.c"
	goto tr221;
tr230:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st151;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
#line 4887 "turtle.c"
	if ( (*p) == 159u )
		goto tr222;
	if ( 160u <= (*p) )
		goto tr218;
	goto tr221;
tr231:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st152;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
#line 4905 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr223;
		case 183u: goto tr240;
		case 191u: goto tr241;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr221;
	goto tr218;
tr240:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st153;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
#line 4926 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
tr241:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st154;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
#line 4942 "turtle.c"
	if ( 190u <= (*p) )
		goto tr218;
	goto tr203;
tr232:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st155;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
#line 4958 "turtle.c"
	if ( (*p) == 144u )
		goto tr242;
	if ( 145u <= (*p) )
		goto tr229;
	goto tr218;
tr242:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st156;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
#line 4976 "turtle.c"
	if ( (*p) == 128u )
		goto tr223;
	if ( 129u <= (*p) )
		goto tr221;
	goto tr218;
tr233:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st157;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
#line 4994 "turtle.c"
	goto tr229;
tr234:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st158;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
#line 5008 "turtle.c"
	if ( (*p) == 175u )
		goto tr226;
	if ( 176u <= (*p) )
		goto tr218;
	goto tr229;
tr204:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st159;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
#line 5026 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr0;
tr205:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st160;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
#line 5048 "turtle.c"
	goto tr203;
tr206:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st161;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
#line 5062 "turtle.c"
	if ( 192u <= (*p) )
		goto tr0;
	goto tr203;
tr207:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st162;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
#line 5078 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 176u )
		goto tr203;
	goto tr0;
tr208:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st163;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
#line 5097 "turtle.c"
	if ( (*p) == 160u )
		goto tr243;
	if ( 161u <= (*p) )
		goto tr205;
	goto tr0;
tr243:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st164;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
#line 5115 "turtle.c"
	if ( 128u <= (*p) )
		goto tr203;
	goto tr0;
tr209:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st165;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
#line 5131 "turtle.c"
	if ( (*p) == 191u )
		goto tr206;
	if ( 192u <= (*p) )
		goto tr0;
	goto tr205;
tr210:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st166;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
#line 5149 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr244;
		case 129u: goto tr245;
		case 134u: goto tr246;
		case 176u: goto tr243;
		case 191u: goto tr247;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr205;
	} else if ( (*p) >= 130u )
		goto tr205;
	goto tr0;
tr244:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st167;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
#line 5175 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr203;
	goto tr0;
tr245:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st168;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
#line 5191 "turtle.c"
	if ( 176u <= (*p) )
		goto tr203;
	goto tr0;
tr246:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st169;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 5207 "turtle.c"
	if ( 144u <= (*p) )
		goto tr0;
	goto tr203;
tr247:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st170;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
#line 5223 "turtle.c"
	if ( 176u <= (*p) )
		goto tr0;
	goto tr203;
tr211:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 5239 "turtle.c"
	if ( (*p) == 128u )
		goto tr248;
	if ( 129u <= (*p) )
		goto tr205;
	goto tr0;
tr248:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st172;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
#line 5257 "turtle.c"
	if ( 129u <= (*p) )
		goto tr203;
	goto tr0;
tr212:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 5273 "turtle.c"
	goto tr205;
tr213:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 5287 "turtle.c"
	if ( (*p) == 159u )
		goto tr206;
	if ( 160u <= (*p) )
		goto tr0;
	goto tr205;
tr214:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 5305 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr243;
		case 183u: goto tr249;
		case 191u: goto tr250;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr205;
	goto tr0;
tr249:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 5326 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr203;
tr250:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 5342 "turtle.c"
	if ( 190u <= (*p) )
		goto tr0;
	goto tr203;
tr215:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 5358 "turtle.c"
	if ( (*p) == 144u )
		goto tr251;
	if ( 145u <= (*p) )
		goto tr212;
	goto tr0;
tr251:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st179;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
#line 5376 "turtle.c"
	if ( (*p) == 128u )
		goto tr243;
	if ( 129u <= (*p) )
		goto tr205;
	goto tr0;
tr216:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 5394 "turtle.c"
	goto tr212;
tr217:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st181;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
#line 5408 "turtle.c"
	if ( (*p) == 175u )
		goto tr209;
	if ( 176u <= (*p) )
		goto tr0;
	goto tr212;
tr279:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st221;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
#line 5436 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 97u: goto tr340;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr340:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 5488 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 108u: goto tr252;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr252:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 5540 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 115u: goto tr253;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr253:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 5592 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 101u: goto tr254;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr280:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st222;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
#line 5654 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 114u: goto tr341;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr341:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 5706 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 117u: goto tr253;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr283:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st223;
st223:
	if ( ++p == pe )
		goto _test_eof223;
case 223:
#line 5768 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr171;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr171;
	} else
		goto tr171;
	goto tr298;
tr284:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st224;
st224:
	if ( ++p == pe )
		goto _test_eof224;
case 224:
#line 5800 "turtle.c"
	goto tr171;
tr285:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st225;
st225:
	if ( ++p == pe )
		goto _test_eof225;
case 225:
#line 5824 "turtle.c"
	if ( 192u <= (*p) )
		goto tr298;
	goto tr171;
tr286:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st226;
st226:
	if ( ++p == pe )
		goto _test_eof226;
case 226:
#line 5850 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr171;
	} else if ( (*p) >= 176u )
		goto tr171;
	goto tr298;
tr287:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st227;
st227:
	if ( ++p == pe )
		goto _test_eof227;
case 227:
#line 5879 "turtle.c"
	if ( (*p) == 160u )
		goto tr177;
	if ( 161u <= (*p) )
		goto tr175;
	goto tr298;
tr288:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st228;
st228:
	if ( ++p == pe )
		goto _test_eof228;
case 228:
#line 5907 "turtle.c"
	if ( (*p) == 191u )
		goto tr176;
	if ( 192u <= (*p) )
		goto tr298;
	goto tr175;
tr289:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st229;
st229:
	if ( ++p == pe )
		goto _test_eof229;
case 229:
#line 5935 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr342;
		case 129u: goto tr343;
		case 134u: goto tr191;
		case 176u: goto tr177;
		case 191u: goto tr192;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr175;
	} else if ( (*p) >= 130u )
		goto tr175;
	goto tr298;
tr342:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 5961 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr171;
	goto tr0;
tr343:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 5977 "turtle.c"
	if ( 176u <= (*p) )
		goto tr171;
	goto tr0;
tr290:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st230;
st230:
	if ( ++p == pe )
		goto _test_eof230;
case 230:
#line 6003 "turtle.c"
	if ( (*p) == 128u )
		goto tr193;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr298;
tr291:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st231;
st231:
	if ( ++p == pe )
		goto _test_eof231;
case 231:
#line 6031 "turtle.c"
	goto tr175;
tr292:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st232;
st232:
	if ( ++p == pe )
		goto _test_eof232;
case 232:
#line 6055 "turtle.c"
	if ( (*p) == 159u )
		goto tr176;
	if ( 160u <= (*p) )
		goto tr298;
	goto tr175;
tr293:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st233;
st233:
	if ( ++p == pe )
		goto _test_eof233;
case 233:
#line 6083 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr177;
		case 183u: goto tr194;
		case 191u: goto tr195;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr175;
	goto tr298;
tr294:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st234;
st234:
	if ( ++p == pe )
		goto _test_eof234;
case 234:
#line 6114 "turtle.c"
	if ( (*p) == 144u )
		goto tr196;
	if ( 145u <= (*p) )
		goto tr183;
	goto tr298;
tr295:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st235;
st235:
	if ( ++p == pe )
		goto _test_eof235;
case 235:
#line 6142 "turtle.c"
	goto tr183;
tr296:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st236;
st236:
	if ( ++p == pe )
		goto _test_eof236;
case 236:
#line 6166 "turtle.c"
	if ( (*p) == 175u )
		goto tr180;
	if ( 176u <= (*p) )
		goto tr298;
	goto tr183;
	}
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
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
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
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
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
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
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
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
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
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
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
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
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
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
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof218:  shared_lexer->env.cs = 218; goto _test_eof; 
	_test_eof219:  shared_lexer->env.cs = 219; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof220:  shared_lexer->env.cs = 220; goto _test_eof; 
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
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof221:  shared_lexer->env.cs = 221; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof222:  shared_lexer->env.cs = 222; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof223:  shared_lexer->env.cs = 223; goto _test_eof; 
	_test_eof224:  shared_lexer->env.cs = 224; goto _test_eof; 
	_test_eof225:  shared_lexer->env.cs = 225; goto _test_eof; 
	_test_eof226:  shared_lexer->env.cs = 226; goto _test_eof; 
	_test_eof227:  shared_lexer->env.cs = 227; goto _test_eof; 
	_test_eof228:  shared_lexer->env.cs = 228; goto _test_eof; 
	_test_eof229:  shared_lexer->env.cs = 229; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof230:  shared_lexer->env.cs = 230; goto _test_eof; 
	_test_eof231:  shared_lexer->env.cs = 231; goto _test_eof; 
	_test_eof232:  shared_lexer->env.cs = 232; goto _test_eof; 
	_test_eof233:  shared_lexer->env.cs = 233; goto _test_eof; 
	_test_eof234:  shared_lexer->env.cs = 234; goto _test_eof; 
	_test_eof235:  shared_lexer->env.cs = 235; goto _test_eof; 
	_test_eof236:  shared_lexer->env.cs = 236; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 189: goto tr297;
	case 190: goto tr298;
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
	case 191: goto tr302;
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
	case 192: goto tr298;
	case 27: goto tr0;
	case 193: goto tr304;
	case 194: goto tr298;
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
	case 195: goto tr308;
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
	case 196: goto tr298;
	case 55: goto tr0;
	case 197: goto tr311;
	case 56: goto tr117;
	case 57: goto tr117;
	case 198: goto tr312;
	case 199: goto tr313;
	case 58: goto tr120;
	case 200: goto tr315;
	case 201: goto tr316;
	case 59: goto tr122;
	case 60: goto tr122;
	case 202: goto tr316;
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
	case 203: goto tr298;
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
	case 204: goto tr298;
	case 205: goto tr117;
	case 102: goto tr117;
	case 206: goto tr326;
	case 207: goto tr326;
	case 208: goto tr326;
	case 209: goto tr326;
	case 210: goto tr326;
	case 211: goto tr326;
	case 212: goto tr326;
	case 213: goto tr326;
	case 214: goto tr326;
	case 215: goto tr117;
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
	case 216: goto tr298;
	case 128: goto tr0;
	case 129: goto tr0;
	case 217: goto tr298;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 218: goto tr298;
	case 219: goto tr298;
	case 134: goto tr0;
	case 220: goto tr339;
	case 135: goto tr218;
	case 136: goto tr218;
	case 137: goto tr218;
	case 138: goto tr218;
	case 139: goto tr218;
	case 140: goto tr218;
	case 141: goto tr218;
	case 142: goto tr218;
	case 143: goto tr218;
	case 144: goto tr218;
	case 145: goto tr218;
	case 146: goto tr218;
	case 147: goto tr218;
	case 148: goto tr218;
	case 149: goto tr218;
	case 150: goto tr218;
	case 151: goto tr218;
	case 152: goto tr218;
	case 153: goto tr218;
	case 154: goto tr218;
	case 155: goto tr218;
	case 156: goto tr218;
	case 157: goto tr218;
	case 158: goto tr218;
	case 159: goto tr0;
	case 160: goto tr0;
	case 161: goto tr0;
	case 162: goto tr0;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 166: goto tr0;
	case 167: goto tr0;
	case 168: goto tr0;
	case 169: goto tr0;
	case 170: goto tr0;
	case 171: goto tr0;
	case 172: goto tr0;
	case 173: goto tr0;
	case 174: goto tr0;
	case 175: goto tr0;
	case 176: goto tr0;
	case 177: goto tr0;
	case 178: goto tr0;
	case 179: goto tr0;
	case 180: goto tr0;
	case 181: goto tr0;
	case 221: goto tr298;
	case 182: goto tr0;
	case 183: goto tr0;
	case 184: goto tr0;
	case 222: goto tr298;
	case 185: goto tr0;
	case 223: goto tr298;
	case 224: goto tr298;
	case 225: goto tr298;
	case 226: goto tr298;
	case 227: goto tr298;
	case 228: goto tr298;
	case 229: goto tr298;
	case 186: goto tr0;
	case 187: goto tr0;
	case 230: goto tr298;
	case 231: goto tr298;
	case 232: goto tr298;
	case 233: goto tr298;
	case 234: goto tr298;
	case 235: goto tr298;
	case 236: goto tr298;
	}
	}

	}

#line 494 "turtle.rl"
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
          
#line 6686 "turtle.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch (  shared_lexer->env.cs )
	{
tr0:
#line 273 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}}
	goto st188;
tr2:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr23:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr26:
#line 237 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr34:
#line 234 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr58:
#line 266 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr61:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr82:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr85:
#line 238 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr93:
#line 235 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
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
	goto st188;
tr120:
#line 229 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr122:
#line 225 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(QNAME); }}
	goto st188;
tr154:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr167:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr218:
#line 224 "turtle.rl"
	{{p = (( shared_lexer->env.te))-1;}{ lexer_emit_u8_token(BLANK_LITERAL); }}
	goto st188;
tr255:
#line 273 "turtle.rl"
	{ shared_lexer->env.te = p+1;}
	goto st188;
tr256:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr261:
#line 255 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_ROUND);  }}
	goto st188;
tr262:
#line 256 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_ROUND);  }}
	goto st188;
tr264:
#line 250 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(COMMA); }}
	goto st188;
tr268:
#line 251 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(SEMICOLON); }}
	goto st188;
tr274:
#line 253 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_SQUARE); }}
	goto st188;
tr275:
#line 254 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_SQUARE); }}
	goto st188;
tr281:
#line 257 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(L_CURLY);  }}
	goto st188;
tr282:
#line 258 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_token_const(R_CURLY);  }}
	goto st188;
tr297:
#line 260 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr298:
#line 273 "turtle.rl"
	{ shared_lexer->env.te = p;p--;}
	goto st188;
tr302:
#line 237 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr304:
#line 266 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        }}
	goto st188;
tr308:
#line 238 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(STRING_LITERAL); }}
	goto st188;
tr311:
#line 230 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr312:
#line 231 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr313:
#line 229 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); }}
	goto st188;
tr315:
#line 249 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_token_const(DOT); }}
	goto st188;
tr316:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 225 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(QNAME); }}
	goto st188;
tr320:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 226 "turtle.rl"
	{ shared_lexer->env.te = p+1;{ lexer_emit_u8_token(IRIREF); }}
	goto st188;
tr326:
#line 243 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        }}
	goto st188;
tr337:
#line 240 "turtle.rl"
	{ shared_lexer->env.te = p+1;{
              lexer_emit_token_const(HAT);
        }}
	goto st188;
tr339:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
#line 224 "turtle.rl"
	{ shared_lexer->env.te = p;p--;{ lexer_emit_u8_token(BLANK_LITERAL); }}
	goto st188;
st188:
#line 1 "NONE"
	{ shared_lexer->env.ts = 0;}
	if ( ++p == pe )
		goto _test_eof188;
case 188:
#line 1 "NONE"
	{ shared_lexer->env.ts = p;}
#line 7000 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr256;
		case 13u: goto st189;
		case 34u: goto tr258;
		case 35u: goto tr259;
		case 39u: goto tr260;
		case 40u: goto tr261;
		case 41u: goto tr262;
		case 44u: goto tr264;
		case 46u: goto st200;
		case 58u: goto tr267;
		case 59u: goto tr268;
		case 60u: goto tr269;
		case 64u: goto st204;
		case 66u: goto tr272;
		case 80u: goto tr273;
		case 91u: goto tr274;
		case 93u: goto tr275;
		case 94u: goto st218;
		case 95u: goto tr277;
		case 97u: goto tr278;
		case 98u: goto tr272;
		case 102u: goto tr279;
		case 112u: goto tr273;
		case 116u: goto tr280;
		case 123u: goto tr281;
		case 125u: goto tr282;
		case 195u: goto tr283;
		case 203u: goto tr285;
		case 205u: goto tr286;
		case 223u: goto tr285;
		case 224u: goto tr287;
		case 225u: goto tr288;
		case 226u: goto tr289;
		case 227u: goto tr290;
		case 237u: goto tr292;
		case 239u: goto tr293;
		case 240u: goto tr294;
		case 243u: goto tr296;
	}
	if ( (*p) < 99u ) {
		if ( (*p) < 48u ) {
			if ( 43u <= (*p) && (*p) <= 45u )
				goto tr263;
		} else if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr271;
		} else
			goto tr266;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 206u ) {
			if ( 196u <= (*p) && (*p) <= 202u )
				goto tr284;
		} else if ( (*p) > 222u ) {
			if ( (*p) > 236u ) {
				if ( 241u <= (*p) && (*p) <= 242u )
					goto tr295;
			} else if ( (*p) >= 228u )
				goto tr291;
		} else
			goto tr284;
	} else
		goto tr271;
	goto tr255;
st189:
	if ( ++p == pe )
		goto _test_eof189;
case 189:
	if ( (*p) == 10u )
		goto tr256;
	goto tr297;
tr258:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st190;
st190:
	if ( ++p == pe )
		goto _test_eof190;
case 190:
#line 7080 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr298;
		case 13u: goto tr298;
		case 34u: goto tr300;
		case 92u: goto tr301;
	}
	goto tr299;
tr1:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr4:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st0;
tr5:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st0;
tr7:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st0;
tr8:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st0;
tr9:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st0;
tr10:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st0;
tr11:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st0;
tr12:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st0;
tr22:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
tr299:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st0;
st0:
	if ( ++p == pe )
		goto _test_eof0;
case 0:
#line 7186 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 34u: goto tr2;
		case 92u: goto st1;
	}
	goto tr1;
tr24:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st1;
tr301:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 7216 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 7253 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 7312 "turtle.c"
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
tr300:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st191;
st191:
	if ( ++p == pe )
		goto _test_eof191;
case 191:
#line 7392 "turtle.c"
	if ( (*p) == 34u )
		goto st12;
	goto tr302;
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr35:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st13;
tr36:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st13;
tr38:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st13;
tr39:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st13;
tr40:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st13;
tr41:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st13;
tr42:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st13;
tr43:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st13;
tr53:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
tr27:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 7503 "turtle.c"
	switch( (*p) ) {
		case 34u: goto tr31;
		case 92u: goto st16;
	}
	goto tr30;
tr28:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr31:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st14;
tr54:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
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
#line 7551 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st16;
tr29:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 7584 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
#line 7621 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 7680 "turtle.c"
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
tr259:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st192;
st192:
	if ( ++p == pe )
		goto _test_eof192;
case 192:
#line 7746 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr58;
		case 13u: goto st193;
	}
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	switch( (*p) ) {
		case 10u: goto tr58;
		case 13u: goto st193;
	}
	goto st27;
st193:
	if ( ++p == pe )
		goto _test_eof193;
case 193:
	if ( (*p) == 10u )
		goto tr58;
	goto tr304;
tr260:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st194;
st194:
	if ( ++p == pe )
		goto _test_eof194;
case 194:
#line 7776 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr298;
		case 13u: goto tr298;
		case 39u: goto tr306;
		case 92u: goto tr307;
	}
	goto tr305;
tr60:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr63:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st28;
tr64:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st28;
tr66:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st28;
tr67:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st28;
tr68:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st28;
tr69:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st28;
tr70:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st28;
tr71:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st28;
tr81:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
tr305:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 7882 "turtle.c"
	switch( (*p) ) {
		case 10u: goto tr0;
		case 13u: goto tr0;
		case 39u: goto tr61;
		case 92u: goto st29;
	}
	goto tr60;
tr83:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st29;
tr307:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 7912 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 7949 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 8008 "turtle.c"
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
tr306:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st195;
st195:
	if ( ++p == pe )
		goto _test_eof195;
case 195:
#line 8088 "turtle.c"
	if ( (*p) == 39u )
		goto st40;
	goto tr308;
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr94:
#line 157 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
	goto st41;
tr95:
#line 163 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
	goto st41;
tr97:
#line 169 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
	goto st41;
tr98:
#line 133 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
	goto st41;
tr99:
#line 151 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
	goto st41;
tr100:
#line 139 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
	goto st41;
tr101:
#line 145 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
	goto st41;
tr102:
#line 127 "turtle.rl"
	{
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
	goto st41;
tr112:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
tr86:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 8199 "turtle.c"
	switch( (*p) ) {
		case 39u: goto tr90;
		case 92u: goto st44;
	}
	goto tr89;
tr87:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr90:
#line 88 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }
	goto st42;
tr113:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 88 "turtle.rl"
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
#line 8247 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st44;
tr88:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 8280 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st46;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
#line 8317 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st50;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
#line 8376 "turtle.c"
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
tr263:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st196;
st196:
	if ( ++p == pe )
		goto _test_eof196;
case 196:
#line 8442 "turtle.c"
	if ( (*p) == 46u )
		goto st55;
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr266;
	goto tr298;
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
#line 230 "turtle.rl"
	{ shared_lexer->env.act = 9;}
	goto st197;
st197:
	if ( ++p == pe )
		goto _test_eof197;
case 197:
#line 8465 "turtle.c"
	switch( (*p) ) {
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr311;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 43u: goto st57;
		case 45u: goto st57;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr117;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr117;
st198:
	if ( ++p == pe )
		goto _test_eof198;
case 198:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto st198;
	goto tr312;
tr266:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 229 "turtle.rl"
	{ shared_lexer->env.act = 8;}
	goto st199;
st199:
	if ( ++p == pe )
		goto _test_eof199;
case 199:
#line 8508 "turtle.c"
	switch( (*p) ) {
		case 46u: goto st58;
		case 69u: goto st56;
		case 101u: goto st56;
	}
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr266;
	goto tr313;
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
st200:
	if ( ++p == pe )
		goto _test_eof200;
case 200:
	if ( 48u <= (*p) && (*p) <= 57u )
		goto tr116;
	goto tr315;
tr173:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st201;
tr267:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st201;
st201:
	if ( ++p == pe )
		goto _test_eof201;
case 201:
#line 8565 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 205u: goto tr317;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr318;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr129;
	goto tr316;
tr125:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st59;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
#line 8616 "turtle.c"
	if ( (*p) < 65u ) {
		if ( 48u <= (*p) && (*p) <= 57u )
			goto tr123;
	} else if ( (*p) > 70u ) {
		if ( 97u <= (*p) && (*p) <= 102u )
			goto tr123;
	} else
		goto tr123;
	goto tr122;
tr123:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st60;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
#line 8638 "turtle.c"
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
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st202;
st202:
	if ( ++p == pe )
		goto _test_eof202;
case 202:
#line 8662 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 45u: goto tr124;
		case 46u: goto tr126;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 204u: goto tr131;
		case 205u: goto tr132;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr135;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr124;
	goto tr316;
tr126:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st61;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
#line 8714 "turtle.c"
	switch( (*p) ) {
		case 37u: goto tr125;
		case 45u: goto tr124;
		case 46u: goto tr126;
		case 92u: goto tr127;
		case 95u: goto tr124;
		case 183u: goto tr124;
		case 195u: goto tr128;
		case 203u: goto tr130;
		case 204u: goto tr131;
		case 205u: goto tr132;
		case 223u: goto tr130;
		case 224u: goto tr133;
		case 225u: goto tr134;
		case 226u: goto tr135;
		case 227u: goto tr136;
		case 237u: goto tr138;
		case 239u: goto tr139;
		case 240u: goto tr140;
		case 243u: goto tr142;
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
				goto tr129;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr141;
		} else
			goto tr137;
	} else
		goto tr124;
	goto tr122;
tr127:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st62;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
#line 8766 "turtle.c"
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
tr128:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st63;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
#line 8792 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr124;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr124;
	} else
		goto tr124;
	goto tr122;
tr129:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st64;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
#line 8814 "turtle.c"
	goto tr124;
tr130:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 8828 "turtle.c"
	if ( 192u <= (*p) )
		goto tr122;
	goto tr124;
tr131:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 8844 "turtle.c"
	if ( (*p) <= 127u )
		goto tr122;
	goto tr124;
tr132:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st67;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
#line 8860 "turtle.c"
	if ( (*p) == 190u )
		goto tr122;
	goto tr124;
tr133:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st68;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
#line 8876 "turtle.c"
	if ( (*p) == 160u )
		goto tr131;
	if ( 161u <= (*p) )
		goto tr129;
	goto tr122;
tr134:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st69;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
#line 8894 "turtle.c"
	if ( (*p) == 191u )
		goto tr130;
	if ( 192u <= (*p) )
		goto tr122;
	goto tr129;
tr135:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st70;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
#line 8912 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr143;
		case 129u: goto tr144;
		case 134u: goto tr145;
		case 176u: goto tr131;
		case 191u: goto tr146;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr129;
	} else if ( (*p) >= 130u )
		goto tr129;
	goto tr122;
tr143:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st71;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
#line 8938 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 140u )
		goto tr124;
	goto tr122;
tr144:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st72;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
#line 8957 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
tr145:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st73;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
#line 8973 "turtle.c"
	if ( 144u <= (*p) )
		goto tr122;
	goto tr124;
tr146:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 8989 "turtle.c"
	if ( 176u <= (*p) )
		goto tr122;
	goto tr124;
tr136:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st75;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
#line 9005 "turtle.c"
	if ( (*p) == 128u )
		goto tr147;
	if ( 129u <= (*p) )
		goto tr129;
	goto tr122;
tr147:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st76;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
#line 9023 "turtle.c"
	if ( 129u <= (*p) )
		goto tr124;
	goto tr122;
tr137:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st77;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
#line 9039 "turtle.c"
	goto tr129;
tr138:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st78;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
#line 9053 "turtle.c"
	if ( (*p) == 159u )
		goto tr130;
	if ( 160u <= (*p) )
		goto tr122;
	goto tr129;
tr139:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st79;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
#line 9071 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr131;
		case 183u: goto tr148;
		case 191u: goto tr149;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr129;
	goto tr122;
tr148:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st80;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
#line 9092 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr122;
	goto tr124;
tr149:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st81;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
#line 9108 "turtle.c"
	if ( 190u <= (*p) )
		goto tr122;
	goto tr124;
tr140:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st82;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
#line 9124 "turtle.c"
	if ( (*p) == 144u )
		goto tr150;
	if ( 145u <= (*p) )
		goto tr137;
	goto tr122;
tr150:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st83;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
#line 9142 "turtle.c"
	if ( (*p) == 128u )
		goto tr131;
	if ( 129u <= (*p) )
		goto tr129;
	goto tr122;
tr141:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st84;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
#line 9160 "turtle.c"
	goto tr137;
tr142:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st85;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
#line 9174 "turtle.c"
	if ( (*p) == 175u )
		goto tr134;
	if ( 176u <= (*p) )
		goto tr122;
	goto tr137;
tr317:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st86;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 9192 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr124;
	} else if ( (*p) >= 176u )
		goto tr124;
	goto tr122;
tr318:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st87;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
#line 9211 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr151;
		case 129u: goto tr152;
		case 134u: goto tr145;
		case 176u: goto tr131;
		case 191u: goto tr146;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr129;
	} else if ( (*p) >= 130u )
		goto tr129;
	goto tr122;
tr151:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st88;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
#line 9237 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr124;
	goto tr122;
tr152:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st89;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
#line 9253 "turtle.c"
	if ( 176u <= (*p) )
		goto tr124;
	goto tr122;
tr269:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
	goto st203;
st203:
	if ( ++p == pe )
		goto _test_eof203;
case 203:
#line 9265 "turtle.c"
	switch( (*p) ) {
		case 34u: goto tr298;
		case 60u: goto tr298;
		case 62u: goto tr320;
		case 92u: goto tr321;
		case 94u: goto tr298;
		case 96u: goto tr298;
		case 123u: goto tr298;
		case 125u: goto tr298;
	}
	if ( (*p) <= 32u )
		goto tr298;
	goto tr319;
tr153:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr166:
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
tr319:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st90;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
#line 9321 "turtle.c"
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
#line 113 "turtle.rl"
	{
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }
	goto st91;
tr321:
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st91;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
#line 9357 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st93;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
#line 9386 "turtle.c"
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
#line 109 "turtle.rl"
	{
        mark = p;
    }
	goto st97;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
#line 9445 "turtle.c"
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
st204:
	if ( ++p == pe )
		goto _test_eof204;
case 204:
	switch( (*p) ) {
		case 98u: goto tr323;
		case 112u: goto tr324;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr298;
tr322:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st205;
tr329:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 221 "turtle.rl"
	{ shared_lexer->env.act = 3;}
	goto st205;
tr334:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 219 "turtle.rl"
	{ shared_lexer->env.act = 1;}
	goto st205;
st205:
	if ( ++p == pe )
		goto _test_eof205;
case 205:
#line 9547 "turtle.c"
	if ( (*p) == 45u )
		goto st102;
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
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
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st206;
st206:
	if ( ++p == pe )
		goto _test_eof206;
case 206:
#line 9579 "turtle.c"
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
	goto tr326;
tr323:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st207;
st207:
	if ( ++p == pe )
		goto _test_eof207;
case 207:
#line 9601 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 97u: goto tr327;
	}
	if ( (*p) > 90u ) {
		if ( 98u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr327:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st208;
st208:
	if ( ++p == pe )
		goto _test_eof208;
case 208:
#line 9622 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 115u: goto tr328;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr328:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st209;
st209:
	if ( ++p == pe )
		goto _test_eof209;
case 209:
#line 9643 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr329;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr324:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st210;
st210:
	if ( ++p == pe )
		goto _test_eof210;
case 210:
#line 9664 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 114u: goto tr330;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr330:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st211;
st211:
	if ( ++p == pe )
		goto _test_eof211;
case 211:
#line 9685 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 101u: goto tr331;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr331:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st212;
st212:
	if ( ++p == pe )
		goto _test_eof212;
case 212:
#line 9706 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 102u: goto tr332;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr332:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st213;
st213:
	if ( ++p == pe )
		goto _test_eof213;
case 213:
#line 9727 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 105u: goto tr333;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr333:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 243 "turtle.rl"
	{ shared_lexer->env.act = 17;}
	goto st214;
st214:
	if ( ++p == pe )
		goto _test_eof214;
case 214:
#line 9748 "turtle.c"
	switch( (*p) ) {
		case 45u: goto st102;
		case 120u: goto tr334;
	}
	if ( (*p) > 90u ) {
		if ( 97u <= (*p) && (*p) <= 122u )
			goto tr322;
	} else if ( (*p) >= 65u )
		goto tr322;
	goto tr326;
tr198:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 222 "turtle.rl"
	{ shared_lexer->env.act = 4;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st215;
tr202:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 220 "turtle.rl"
	{ shared_lexer->env.act = 2;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st215;
tr254:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 232 "turtle.rl"
	{ shared_lexer->env.act = 11;}
	goto st215;
tr271:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st215;
tr278:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 248 "turtle.rl"
	{ shared_lexer->env.act = 18;}
	goto st215;
st215:
	if ( ++p == pe )
		goto _test_eof215;
case 215:
#line 9835 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr171:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st103;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
#line 9886 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr172:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st104;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
#line 9937 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 95u: goto tr171;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr117;
tr174:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st105;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
#line 9987 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr171;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr171;
	} else
		goto tr171;
	goto tr117;
tr175:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st106;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
#line 10009 "turtle.c"
	goto tr171;
tr176:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st107;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
#line 10023 "turtle.c"
	if ( 192u <= (*p) )
		goto tr117;
	goto tr171;
tr177:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st108;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
#line 10039 "turtle.c"
	if ( (*p) <= 127u )
		goto tr117;
	goto tr171;
tr178:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st109;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
#line 10055 "turtle.c"
	if ( (*p) == 190u )
		goto tr117;
	goto tr171;
tr179:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st110;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
#line 10071 "turtle.c"
	if ( (*p) == 160u )
		goto tr177;
	if ( 161u <= (*p) )
		goto tr175;
	goto tr117;
tr180:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st111;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
#line 10089 "turtle.c"
	if ( (*p) == 191u )
		goto tr176;
	if ( 192u <= (*p) )
		goto tr117;
	goto tr175;
tr181:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st112;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
#line 10107 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr189;
		case 129u: goto tr190;
		case 134u: goto tr191;
		case 176u: goto tr177;
		case 191u: goto tr192;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr175;
	} else if ( (*p) >= 130u )
		goto tr175;
	goto tr117;
tr189:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st113;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
#line 10133 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr171;
	} else if ( (*p) >= 140u )
		goto tr171;
	goto tr117;
tr190:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st114;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
#line 10152 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr171;
tr191:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st115;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
#line 10168 "turtle.c"
	if ( 144u <= (*p) )
		goto tr117;
	goto tr171;
tr192:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st116;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
#line 10184 "turtle.c"
	if ( 176u <= (*p) )
		goto tr117;
	goto tr171;
tr182:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st117;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
#line 10200 "turtle.c"
	if ( (*p) == 128u )
		goto tr193;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr117;
tr193:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st118;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
#line 10218 "turtle.c"
	if ( 129u <= (*p) )
		goto tr171;
	goto tr117;
tr183:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st119;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
#line 10234 "turtle.c"
	goto tr175;
tr184:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st120;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
#line 10248 "turtle.c"
	if ( (*p) == 159u )
		goto tr176;
	if ( 160u <= (*p) )
		goto tr117;
	goto tr175;
tr185:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st121;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
#line 10266 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr177;
		case 183u: goto tr194;
		case 191u: goto tr195;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr175;
	goto tr117;
tr194:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st122;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
#line 10287 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr117;
	goto tr171;
tr195:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st123;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
#line 10303 "turtle.c"
	if ( 190u <= (*p) )
		goto tr117;
	goto tr171;
tr186:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st124;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
#line 10319 "turtle.c"
	if ( (*p) == 144u )
		goto tr196;
	if ( 145u <= (*p) )
		goto tr183;
	goto tr117;
tr196:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st125;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
#line 10337 "turtle.c"
	if ( (*p) == 128u )
		goto tr177;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr117;
tr187:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st126;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
#line 10355 "turtle.c"
	goto tr183;
tr188:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st127;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
#line 10369 "turtle.c"
	if ( (*p) == 175u )
		goto tr180;
	if ( 176u <= (*p) )
		goto tr117;
	goto tr183;
tr272:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st216;
st216:
	if ( ++p == pe )
		goto _test_eof216;
case 216:
#line 10397 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 65u: goto tr335;
		case 95u: goto tr171;
		case 97u: goto tr335;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 66u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr335:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st128;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
#line 10450 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 83u: goto tr197;
		case 95u: goto tr171;
		case 115u: goto tr197;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr197:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st129;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
#line 10503 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 69u: goto tr198;
		case 95u: goto tr171;
		case 101u: goto tr198;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr273:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st217;
st217:
	if ( ++p == pe )
		goto _test_eof217;
case 217:
#line 10566 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 82u: goto tr336;
		case 95u: goto tr171;
		case 114u: goto tr336;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr336:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st130;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
#line 10619 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 69u: goto tr199;
		case 95u: goto tr171;
		case 101u: goto tr199;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr199:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st131;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
#line 10672 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 70u: goto tr200;
		case 95u: goto tr171;
		case 102u: goto tr200;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr200:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st132;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
#line 10725 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 73u: goto tr201;
		case 95u: goto tr171;
		case 105u: goto tr201;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr201:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st133;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
#line 10778 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 88u: goto tr202;
		case 95u: goto tr171;
		case 120u: goto tr202;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
st218:
	if ( ++p == pe )
		goto _test_eof218;
case 218:
	if ( (*p) == 94u )
		goto tr337;
	goto tr298;
tr277:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
	goto st219;
st219:
	if ( ++p == pe )
		goto _test_eof219;
case 219:
#line 10840 "turtle.c"
	if ( (*p) == 58u )
		goto st134;
	goto tr298;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
	switch( (*p) ) {
		case 95u: goto tr203;
		case 195u: goto tr204;
		case 203u: goto tr206;
		case 205u: goto tr207;
		case 223u: goto tr206;
		case 224u: goto tr208;
		case 225u: goto tr209;
		case 226u: goto tr210;
		case 227u: goto tr211;
		case 237u: goto tr213;
		case 239u: goto tr214;
		case 240u: goto tr215;
		case 243u: goto tr217;
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
				goto tr205;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr216;
		} else
			goto tr212;
	} else
		goto tr205;
	goto tr0;
tr203:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st220;
st220:
	if ( ++p == pe )
		goto _test_eof220;
case 220:
#line 10898 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto tr219;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto tr220;
		case 203u: goto tr222;
		case 204u: goto tr223;
		case 205u: goto tr224;
		case 223u: goto tr222;
		case 224u: goto tr225;
		case 225u: goto tr226;
		case 226u: goto tr227;
		case 227u: goto tr228;
		case 237u: goto tr230;
		case 239u: goto tr231;
		case 240u: goto tr232;
		case 243u: goto tr234;
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
				goto tr221;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr233;
		} else
			goto tr229;
	} else
		goto tr203;
	goto tr339;
tr219:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st135;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
#line 10948 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr203;
		case 46u: goto tr219;
		case 95u: goto tr203;
		case 183u: goto tr203;
		case 195u: goto tr220;
		case 203u: goto tr222;
		case 204u: goto tr223;
		case 205u: goto tr224;
		case 223u: goto tr222;
		case 224u: goto tr225;
		case 225u: goto tr226;
		case 226u: goto tr227;
		case 227u: goto tr228;
		case 237u: goto tr230;
		case 239u: goto tr231;
		case 240u: goto tr232;
		case 243u: goto tr234;
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
				goto tr221;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr233;
		} else
			goto tr229;
	} else
		goto tr203;
	goto tr218;
tr220:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st136;
st136:
	if ( ++p == pe )
		goto _test_eof136;
case 136:
#line 10998 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr218;
tr221:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st137;
st137:
	if ( ++p == pe )
		goto _test_eof137;
case 137:
#line 11020 "turtle.c"
	goto tr203;
tr222:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st138;
st138:
	if ( ++p == pe )
		goto _test_eof138;
case 138:
#line 11034 "turtle.c"
	if ( 192u <= (*p) )
		goto tr218;
	goto tr203;
tr223:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st139;
st139:
	if ( ++p == pe )
		goto _test_eof139;
case 139:
#line 11050 "turtle.c"
	if ( 128u <= (*p) )
		goto tr203;
	goto tr218;
tr224:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st140;
st140:
	if ( ++p == pe )
		goto _test_eof140;
case 140:
#line 11066 "turtle.c"
	if ( (*p) == 190u )
		goto tr218;
	goto tr203;
tr225:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st141;
st141:
	if ( ++p == pe )
		goto _test_eof141;
case 141:
#line 11082 "turtle.c"
	if ( (*p) == 160u )
		goto tr223;
	if ( 161u <= (*p) )
		goto tr221;
	goto tr218;
tr226:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st142;
st142:
	if ( ++p == pe )
		goto _test_eof142;
case 142:
#line 11100 "turtle.c"
	if ( (*p) == 191u )
		goto tr222;
	if ( 192u <= (*p) )
		goto tr218;
	goto tr221;
tr227:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st143;
st143:
	if ( ++p == pe )
		goto _test_eof143;
case 143:
#line 11118 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr235;
		case 129u: goto tr236;
		case 134u: goto tr237;
		case 176u: goto tr223;
		case 191u: goto tr238;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr221;
	} else if ( (*p) >= 130u )
		goto tr221;
	goto tr218;
tr235:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st144;
st144:
	if ( ++p == pe )
		goto _test_eof144;
case 144:
#line 11144 "turtle.c"
	if ( (*p) > 141u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 140u )
		goto tr203;
	goto tr218;
tr236:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st145;
st145:
	if ( ++p == pe )
		goto _test_eof145;
case 145:
#line 11163 "turtle.c"
	if ( 129u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
tr237:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st146;
st146:
	if ( ++p == pe )
		goto _test_eof146;
case 146:
#line 11179 "turtle.c"
	if ( 144u <= (*p) )
		goto tr218;
	goto tr203;
tr238:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st147;
st147:
	if ( ++p == pe )
		goto _test_eof147;
case 147:
#line 11195 "turtle.c"
	if ( 176u <= (*p) )
		goto tr218;
	goto tr203;
tr228:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st148;
st148:
	if ( ++p == pe )
		goto _test_eof148;
case 148:
#line 11211 "turtle.c"
	if ( (*p) == 128u )
		goto tr239;
	if ( 129u <= (*p) )
		goto tr221;
	goto tr218;
tr239:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st149;
st149:
	if ( ++p == pe )
		goto _test_eof149;
case 149:
#line 11229 "turtle.c"
	if ( 129u <= (*p) )
		goto tr203;
	goto tr218;
tr229:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st150;
st150:
	if ( ++p == pe )
		goto _test_eof150;
case 150:
#line 11245 "turtle.c"
	goto tr221;
tr230:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st151;
st151:
	if ( ++p == pe )
		goto _test_eof151;
case 151:
#line 11259 "turtle.c"
	if ( (*p) == 159u )
		goto tr222;
	if ( 160u <= (*p) )
		goto tr218;
	goto tr221;
tr231:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st152;
st152:
	if ( ++p == pe )
		goto _test_eof152;
case 152:
#line 11277 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr223;
		case 183u: goto tr240;
		case 191u: goto tr241;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr221;
	goto tr218;
tr240:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st153;
st153:
	if ( ++p == pe )
		goto _test_eof153;
case 153:
#line 11298 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr218;
	goto tr203;
tr241:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st154;
st154:
	if ( ++p == pe )
		goto _test_eof154;
case 154:
#line 11314 "turtle.c"
	if ( 190u <= (*p) )
		goto tr218;
	goto tr203;
tr232:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st155;
st155:
	if ( ++p == pe )
		goto _test_eof155;
case 155:
#line 11330 "turtle.c"
	if ( (*p) == 144u )
		goto tr242;
	if ( 145u <= (*p) )
		goto tr229;
	goto tr218;
tr242:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st156;
st156:
	if ( ++p == pe )
		goto _test_eof156;
case 156:
#line 11348 "turtle.c"
	if ( (*p) == 128u )
		goto tr223;
	if ( 129u <= (*p) )
		goto tr221;
	goto tr218;
tr233:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st157;
st157:
	if ( ++p == pe )
		goto _test_eof157;
case 157:
#line 11366 "turtle.c"
	goto tr229;
tr234:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st158;
st158:
	if ( ++p == pe )
		goto _test_eof158;
case 158:
#line 11380 "turtle.c"
	if ( (*p) == 175u )
		goto tr226;
	if ( 176u <= (*p) )
		goto tr218;
	goto tr229;
tr204:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st159;
st159:
	if ( ++p == pe )
		goto _test_eof159;
case 159:
#line 11398 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr203;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr203;
	} else
		goto tr203;
	goto tr0;
tr205:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st160;
st160:
	if ( ++p == pe )
		goto _test_eof160;
case 160:
#line 11420 "turtle.c"
	goto tr203;
tr206:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st161;
st161:
	if ( ++p == pe )
		goto _test_eof161;
case 161:
#line 11434 "turtle.c"
	if ( 192u <= (*p) )
		goto tr0;
	goto tr203;
tr207:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st162;
st162:
	if ( ++p == pe )
		goto _test_eof162;
case 162:
#line 11450 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr203;
	} else if ( (*p) >= 176u )
		goto tr203;
	goto tr0;
tr208:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st163;
st163:
	if ( ++p == pe )
		goto _test_eof163;
case 163:
#line 11469 "turtle.c"
	if ( (*p) == 160u )
		goto tr243;
	if ( 161u <= (*p) )
		goto tr205;
	goto tr0;
tr243:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st164;
st164:
	if ( ++p == pe )
		goto _test_eof164;
case 164:
#line 11487 "turtle.c"
	if ( 128u <= (*p) )
		goto tr203;
	goto tr0;
tr209:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st165;
st165:
	if ( ++p == pe )
		goto _test_eof165;
case 165:
#line 11503 "turtle.c"
	if ( (*p) == 191u )
		goto tr206;
	if ( 192u <= (*p) )
		goto tr0;
	goto tr205;
tr210:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st166;
st166:
	if ( ++p == pe )
		goto _test_eof166;
case 166:
#line 11521 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr244;
		case 129u: goto tr245;
		case 134u: goto tr246;
		case 176u: goto tr243;
		case 191u: goto tr247;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr205;
	} else if ( (*p) >= 130u )
		goto tr205;
	goto tr0;
tr244:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st167;
st167:
	if ( ++p == pe )
		goto _test_eof167;
case 167:
#line 11547 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr203;
	goto tr0;
tr245:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st168;
st168:
	if ( ++p == pe )
		goto _test_eof168;
case 168:
#line 11563 "turtle.c"
	if ( 176u <= (*p) )
		goto tr203;
	goto tr0;
tr246:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st169;
st169:
	if ( ++p == pe )
		goto _test_eof169;
case 169:
#line 11579 "turtle.c"
	if ( 144u <= (*p) )
		goto tr0;
	goto tr203;
tr247:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st170;
st170:
	if ( ++p == pe )
		goto _test_eof170;
case 170:
#line 11595 "turtle.c"
	if ( 176u <= (*p) )
		goto tr0;
	goto tr203;
tr211:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st171;
st171:
	if ( ++p == pe )
		goto _test_eof171;
case 171:
#line 11611 "turtle.c"
	if ( (*p) == 128u )
		goto tr248;
	if ( 129u <= (*p) )
		goto tr205;
	goto tr0;
tr248:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st172;
st172:
	if ( ++p == pe )
		goto _test_eof172;
case 172:
#line 11629 "turtle.c"
	if ( 129u <= (*p) )
		goto tr203;
	goto tr0;
tr212:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st173;
st173:
	if ( ++p == pe )
		goto _test_eof173;
case 173:
#line 11645 "turtle.c"
	goto tr205;
tr213:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st174;
st174:
	if ( ++p == pe )
		goto _test_eof174;
case 174:
#line 11659 "turtle.c"
	if ( (*p) == 159u )
		goto tr206;
	if ( 160u <= (*p) )
		goto tr0;
	goto tr205;
tr214:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st175;
st175:
	if ( ++p == pe )
		goto _test_eof175;
case 175:
#line 11677 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr243;
		case 183u: goto tr249;
		case 191u: goto tr250;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr205;
	goto tr0;
tr249:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st176;
st176:
	if ( ++p == pe )
		goto _test_eof176;
case 176:
#line 11698 "turtle.c"
	if ( 144u <= (*p) && (*p) <= 175u )
		goto tr0;
	goto tr203;
tr250:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st177;
st177:
	if ( ++p == pe )
		goto _test_eof177;
case 177:
#line 11714 "turtle.c"
	if ( 190u <= (*p) )
		goto tr0;
	goto tr203;
tr215:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st178;
st178:
	if ( ++p == pe )
		goto _test_eof178;
case 178:
#line 11730 "turtle.c"
	if ( (*p) == 144u )
		goto tr251;
	if ( 145u <= (*p) )
		goto tr212;
	goto tr0;
tr251:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st179;
st179:
	if ( ++p == pe )
		goto _test_eof179;
case 179:
#line 11748 "turtle.c"
	if ( (*p) == 128u )
		goto tr243;
	if ( 129u <= (*p) )
		goto tr205;
	goto tr0;
tr216:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st180;
st180:
	if ( ++p == pe )
		goto _test_eof180;
case 180:
#line 11766 "turtle.c"
	goto tr212;
tr217:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st181;
st181:
	if ( ++p == pe )
		goto _test_eof181;
case 181:
#line 11780 "turtle.c"
	if ( (*p) == 175u )
		goto tr209;
	if ( 176u <= (*p) )
		goto tr0;
	goto tr212;
tr279:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st221;
st221:
	if ( ++p == pe )
		goto _test_eof221;
case 221:
#line 11808 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 97u: goto tr340;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 98u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr340:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st182;
st182:
	if ( ++p == pe )
		goto _test_eof182;
case 182:
#line 11860 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 108u: goto tr252;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr252:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st183;
st183:
	if ( ++p == pe )
		goto _test_eof183;
case 183:
#line 11912 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 115u: goto tr253;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr253:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st184;
st184:
	if ( ++p == pe )
		goto _test_eof184;
case 184:
#line 11964 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 101u: goto tr254;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr280:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st222;
st222:
	if ( ++p == pe )
		goto _test_eof222;
case 222:
#line 12026 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 114u: goto tr341;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr298;
tr341:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st185;
st185:
	if ( ++p == pe )
		goto _test_eof185;
case 185:
#line 12078 "turtle.c"
	switch( (*p) ) {
		case 45u: goto tr171;
		case 46u: goto tr172;
		case 58u: goto tr173;
		case 95u: goto tr171;
		case 117u: goto tr253;
		case 183u: goto tr171;
		case 195u: goto tr174;
		case 203u: goto tr176;
		case 204u: goto tr177;
		case 205u: goto tr178;
		case 223u: goto tr176;
		case 224u: goto tr179;
		case 225u: goto tr180;
		case 226u: goto tr181;
		case 227u: goto tr182;
		case 237u: goto tr184;
		case 239u: goto tr185;
		case 240u: goto tr186;
		case 243u: goto tr188;
	}
	if ( (*p) < 97u ) {
		if ( (*p) > 57u ) {
			if ( 65u <= (*p) && (*p) <= 90u )
				goto tr171;
		} else if ( (*p) >= 48u )
			goto tr171;
	} else if ( (*p) > 122u ) {
		if ( (*p) < 228u ) {
			if ( 196u <= (*p) && (*p) <= 222u )
				goto tr175;
		} else if ( (*p) > 236u ) {
			if ( 241u <= (*p) && (*p) <= 242u )
				goto tr187;
		} else
			goto tr183;
	} else
		goto tr171;
	goto tr0;
tr283:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st223;
st223:
	if ( ++p == pe )
		goto _test_eof223;
case 223:
#line 12140 "turtle.c"
	if ( (*p) < 152u ) {
		if ( 128u <= (*p) && (*p) <= 150u )
			goto tr171;
	} else if ( (*p) > 182u ) {
		if ( 184u <= (*p) )
			goto tr171;
	} else
		goto tr171;
	goto tr298;
tr284:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st224;
st224:
	if ( ++p == pe )
		goto _test_eof224;
case 224:
#line 12172 "turtle.c"
	goto tr171;
tr285:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st225;
st225:
	if ( ++p == pe )
		goto _test_eof225;
case 225:
#line 12196 "turtle.c"
	if ( 192u <= (*p) )
		goto tr298;
	goto tr171;
tr286:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st226;
st226:
	if ( ++p == pe )
		goto _test_eof226;
case 226:
#line 12222 "turtle.c"
	if ( (*p) > 189u ) {
		if ( 191u <= (*p) )
			goto tr171;
	} else if ( (*p) >= 176u )
		goto tr171;
	goto tr298;
tr287:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st227;
st227:
	if ( ++p == pe )
		goto _test_eof227;
case 227:
#line 12251 "turtle.c"
	if ( (*p) == 160u )
		goto tr177;
	if ( 161u <= (*p) )
		goto tr175;
	goto tr298;
tr288:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st228;
st228:
	if ( ++p == pe )
		goto _test_eof228;
case 228:
#line 12279 "turtle.c"
	if ( (*p) == 191u )
		goto tr176;
	if ( 192u <= (*p) )
		goto tr298;
	goto tr175;
tr289:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st229;
st229:
	if ( ++p == pe )
		goto _test_eof229;
case 229:
#line 12307 "turtle.c"
	switch( (*p) ) {
		case 128u: goto tr342;
		case 129u: goto tr343;
		case 134u: goto tr191;
		case 176u: goto tr177;
		case 191u: goto tr192;
	}
	if ( (*p) > 133u ) {
		if ( 177u <= (*p) && (*p) <= 190u )
			goto tr175;
	} else if ( (*p) >= 130u )
		goto tr175;
	goto tr298;
tr342:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st186;
st186:
	if ( ++p == pe )
		goto _test_eof186;
case 186:
#line 12333 "turtle.c"
	if ( 140u <= (*p) && (*p) <= 141u )
		goto tr171;
	goto tr0;
tr343:
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
	goto st187;
st187:
	if ( ++p == pe )
		goto _test_eof187;
case 187:
#line 12349 "turtle.c"
	if ( 176u <= (*p) )
		goto tr171;
	goto tr0;
tr290:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st230;
st230:
	if ( ++p == pe )
		goto _test_eof230;
case 230:
#line 12375 "turtle.c"
	if ( (*p) == 128u )
		goto tr193;
	if ( 129u <= (*p) )
		goto tr175;
	goto tr298;
tr291:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st231;
st231:
	if ( ++p == pe )
		goto _test_eof231;
case 231:
#line 12403 "turtle.c"
	goto tr175;
tr292:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st232;
st232:
	if ( ++p == pe )
		goto _test_eof232;
case 232:
#line 12427 "turtle.c"
	if ( (*p) == 159u )
		goto tr176;
	if ( 160u <= (*p) )
		goto tr298;
	goto tr175;
tr293:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st233;
st233:
	if ( ++p == pe )
		goto _test_eof233;
case 233:
#line 12455 "turtle.c"
	switch( (*p) ) {
		case 164u: goto tr177;
		case 183u: goto tr194;
		case 191u: goto tr195;
	}
	if ( 165u <= (*p) && (*p) <= 190u )
		goto tr175;
	goto tr298;
tr294:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st234;
st234:
	if ( ++p == pe )
		goto _test_eof234;
case 234:
#line 12486 "turtle.c"
	if ( (*p) == 144u )
		goto tr196;
	if ( 145u <= (*p) )
		goto tr183;
	goto tr298;
tr295:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st235;
st235:
	if ( ++p == pe )
		goto _test_eof235;
case 235:
#line 12514 "turtle.c"
	goto tr183;
tr296:
#line 1 "NONE"
	{ shared_lexer->env.te = p+1;}
#line 82 "turtle.rl"
	{
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }
#line 94 "turtle.rl"
	{
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, (*p));
            }
    }
#line 273 "turtle.rl"
	{ shared_lexer->env.act = 30;}
	goto st236;
st236:
	if ( ++p == pe )
		goto _test_eof236;
case 236:
#line 12538 "turtle.c"
	if ( (*p) == 175u )
		goto tr180;
	if ( 176u <= (*p) )
		goto tr298;
	goto tr183;
	}
	_test_eof188:  shared_lexer->env.cs = 188; goto _test_eof; 
	_test_eof189:  shared_lexer->env.cs = 189; goto _test_eof; 
	_test_eof190:  shared_lexer->env.cs = 190; goto _test_eof; 
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
	_test_eof191:  shared_lexer->env.cs = 191; goto _test_eof; 
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
	_test_eof192:  shared_lexer->env.cs = 192; goto _test_eof; 
	_test_eof27:  shared_lexer->env.cs = 27; goto _test_eof; 
	_test_eof193:  shared_lexer->env.cs = 193; goto _test_eof; 
	_test_eof194:  shared_lexer->env.cs = 194; goto _test_eof; 
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
	_test_eof195:  shared_lexer->env.cs = 195; goto _test_eof; 
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
	_test_eof196:  shared_lexer->env.cs = 196; goto _test_eof; 
	_test_eof55:  shared_lexer->env.cs = 55; goto _test_eof; 
	_test_eof197:  shared_lexer->env.cs = 197; goto _test_eof; 
	_test_eof56:  shared_lexer->env.cs = 56; goto _test_eof; 
	_test_eof57:  shared_lexer->env.cs = 57; goto _test_eof; 
	_test_eof198:  shared_lexer->env.cs = 198; goto _test_eof; 
	_test_eof199:  shared_lexer->env.cs = 199; goto _test_eof; 
	_test_eof58:  shared_lexer->env.cs = 58; goto _test_eof; 
	_test_eof200:  shared_lexer->env.cs = 200; goto _test_eof; 
	_test_eof201:  shared_lexer->env.cs = 201; goto _test_eof; 
	_test_eof59:  shared_lexer->env.cs = 59; goto _test_eof; 
	_test_eof60:  shared_lexer->env.cs = 60; goto _test_eof; 
	_test_eof202:  shared_lexer->env.cs = 202; goto _test_eof; 
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
	_test_eof203:  shared_lexer->env.cs = 203; goto _test_eof; 
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
	_test_eof204:  shared_lexer->env.cs = 204; goto _test_eof; 
	_test_eof205:  shared_lexer->env.cs = 205; goto _test_eof; 
	_test_eof102:  shared_lexer->env.cs = 102; goto _test_eof; 
	_test_eof206:  shared_lexer->env.cs = 206; goto _test_eof; 
	_test_eof207:  shared_lexer->env.cs = 207; goto _test_eof; 
	_test_eof208:  shared_lexer->env.cs = 208; goto _test_eof; 
	_test_eof209:  shared_lexer->env.cs = 209; goto _test_eof; 
	_test_eof210:  shared_lexer->env.cs = 210; goto _test_eof; 
	_test_eof211:  shared_lexer->env.cs = 211; goto _test_eof; 
	_test_eof212:  shared_lexer->env.cs = 212; goto _test_eof; 
	_test_eof213:  shared_lexer->env.cs = 213; goto _test_eof; 
	_test_eof214:  shared_lexer->env.cs = 214; goto _test_eof; 
	_test_eof215:  shared_lexer->env.cs = 215; goto _test_eof; 
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
	_test_eof216:  shared_lexer->env.cs = 216; goto _test_eof; 
	_test_eof128:  shared_lexer->env.cs = 128; goto _test_eof; 
	_test_eof129:  shared_lexer->env.cs = 129; goto _test_eof; 
	_test_eof217:  shared_lexer->env.cs = 217; goto _test_eof; 
	_test_eof130:  shared_lexer->env.cs = 130; goto _test_eof; 
	_test_eof131:  shared_lexer->env.cs = 131; goto _test_eof; 
	_test_eof132:  shared_lexer->env.cs = 132; goto _test_eof; 
	_test_eof133:  shared_lexer->env.cs = 133; goto _test_eof; 
	_test_eof218:  shared_lexer->env.cs = 218; goto _test_eof; 
	_test_eof219:  shared_lexer->env.cs = 219; goto _test_eof; 
	_test_eof134:  shared_lexer->env.cs = 134; goto _test_eof; 
	_test_eof220:  shared_lexer->env.cs = 220; goto _test_eof; 
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
	_test_eof163:  shared_lexer->env.cs = 163; goto _test_eof; 
	_test_eof164:  shared_lexer->env.cs = 164; goto _test_eof; 
	_test_eof165:  shared_lexer->env.cs = 165; goto _test_eof; 
	_test_eof166:  shared_lexer->env.cs = 166; goto _test_eof; 
	_test_eof167:  shared_lexer->env.cs = 167; goto _test_eof; 
	_test_eof168:  shared_lexer->env.cs = 168; goto _test_eof; 
	_test_eof169:  shared_lexer->env.cs = 169; goto _test_eof; 
	_test_eof170:  shared_lexer->env.cs = 170; goto _test_eof; 
	_test_eof171:  shared_lexer->env.cs = 171; goto _test_eof; 
	_test_eof172:  shared_lexer->env.cs = 172; goto _test_eof; 
	_test_eof173:  shared_lexer->env.cs = 173; goto _test_eof; 
	_test_eof174:  shared_lexer->env.cs = 174; goto _test_eof; 
	_test_eof175:  shared_lexer->env.cs = 175; goto _test_eof; 
	_test_eof176:  shared_lexer->env.cs = 176; goto _test_eof; 
	_test_eof177:  shared_lexer->env.cs = 177; goto _test_eof; 
	_test_eof178:  shared_lexer->env.cs = 178; goto _test_eof; 
	_test_eof179:  shared_lexer->env.cs = 179; goto _test_eof; 
	_test_eof180:  shared_lexer->env.cs = 180; goto _test_eof; 
	_test_eof181:  shared_lexer->env.cs = 181; goto _test_eof; 
	_test_eof221:  shared_lexer->env.cs = 221; goto _test_eof; 
	_test_eof182:  shared_lexer->env.cs = 182; goto _test_eof; 
	_test_eof183:  shared_lexer->env.cs = 183; goto _test_eof; 
	_test_eof184:  shared_lexer->env.cs = 184; goto _test_eof; 
	_test_eof222:  shared_lexer->env.cs = 222; goto _test_eof; 
	_test_eof185:  shared_lexer->env.cs = 185; goto _test_eof; 
	_test_eof223:  shared_lexer->env.cs = 223; goto _test_eof; 
	_test_eof224:  shared_lexer->env.cs = 224; goto _test_eof; 
	_test_eof225:  shared_lexer->env.cs = 225; goto _test_eof; 
	_test_eof226:  shared_lexer->env.cs = 226; goto _test_eof; 
	_test_eof227:  shared_lexer->env.cs = 227; goto _test_eof; 
	_test_eof228:  shared_lexer->env.cs = 228; goto _test_eof; 
	_test_eof229:  shared_lexer->env.cs = 229; goto _test_eof; 
	_test_eof186:  shared_lexer->env.cs = 186; goto _test_eof; 
	_test_eof187:  shared_lexer->env.cs = 187; goto _test_eof; 
	_test_eof230:  shared_lexer->env.cs = 230; goto _test_eof; 
	_test_eof231:  shared_lexer->env.cs = 231; goto _test_eof; 
	_test_eof232:  shared_lexer->env.cs = 232; goto _test_eof; 
	_test_eof233:  shared_lexer->env.cs = 233; goto _test_eof; 
	_test_eof234:  shared_lexer->env.cs = 234; goto _test_eof; 
	_test_eof235:  shared_lexer->env.cs = 235; goto _test_eof; 
	_test_eof236:  shared_lexer->env.cs = 236; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  shared_lexer->env.cs ) {
	case 189: goto tr297;
	case 190: goto tr298;
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
	case 191: goto tr302;
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
	case 192: goto tr298;
	case 27: goto tr0;
	case 193: goto tr304;
	case 194: goto tr298;
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
	case 195: goto tr308;
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
	case 196: goto tr298;
	case 55: goto tr0;
	case 197: goto tr311;
	case 56: goto tr117;
	case 57: goto tr117;
	case 198: goto tr312;
	case 199: goto tr313;
	case 58: goto tr120;
	case 200: goto tr315;
	case 201: goto tr316;
	case 59: goto tr122;
	case 60: goto tr122;
	case 202: goto tr316;
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
	case 203: goto tr298;
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
	case 204: goto tr298;
	case 205: goto tr117;
	case 102: goto tr117;
	case 206: goto tr326;
	case 207: goto tr326;
	case 208: goto tr326;
	case 209: goto tr326;
	case 210: goto tr326;
	case 211: goto tr326;
	case 212: goto tr326;
	case 213: goto tr326;
	case 214: goto tr326;
	case 215: goto tr117;
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
	case 216: goto tr298;
	case 128: goto tr0;
	case 129: goto tr0;
	case 217: goto tr298;
	case 130: goto tr0;
	case 131: goto tr0;
	case 132: goto tr0;
	case 133: goto tr0;
	case 218: goto tr298;
	case 219: goto tr298;
	case 134: goto tr0;
	case 220: goto tr339;
	case 135: goto tr218;
	case 136: goto tr218;
	case 137: goto tr218;
	case 138: goto tr218;
	case 139: goto tr218;
	case 140: goto tr218;
	case 141: goto tr218;
	case 142: goto tr218;
	case 143: goto tr218;
	case 144: goto tr218;
	case 145: goto tr218;
	case 146: goto tr218;
	case 147: goto tr218;
	case 148: goto tr218;
	case 149: goto tr218;
	case 150: goto tr218;
	case 151: goto tr218;
	case 152: goto tr218;
	case 153: goto tr218;
	case 154: goto tr218;
	case 155: goto tr218;
	case 156: goto tr218;
	case 157: goto tr218;
	case 158: goto tr218;
	case 159: goto tr0;
	case 160: goto tr0;
	case 161: goto tr0;
	case 162: goto tr0;
	case 163: goto tr0;
	case 164: goto tr0;
	case 165: goto tr0;
	case 166: goto tr0;
	case 167: goto tr0;
	case 168: goto tr0;
	case 169: goto tr0;
	case 170: goto tr0;
	case 171: goto tr0;
	case 172: goto tr0;
	case 173: goto tr0;
	case 174: goto tr0;
	case 175: goto tr0;
	case 176: goto tr0;
	case 177: goto tr0;
	case 178: goto tr0;
	case 179: goto tr0;
	case 180: goto tr0;
	case 181: goto tr0;
	case 221: goto tr298;
	case 182: goto tr0;
	case 183: goto tr0;
	case 184: goto tr0;
	case 222: goto tr298;
	case 185: goto tr0;
	case 223: goto tr298;
	case 224: goto tr298;
	case 225: goto tr298;
	case 226: goto tr298;
	case 227: goto tr298;
	case 228: goto tr298;
	case 229: goto tr298;
	case 186: goto tr0;
	case 187: goto tr0;
	case 230: goto tr298;
	case 231: goto tr298;
	case 232: goto tr298;
	case 233: goto tr298;
	case 234: goto tr298;
	case 235: goto tr298;
	case 236: goto tr298;
	}
	}

	}

#line 523 "turtle.rl"
        /* clang-format on */
        handler( ARDP_SUCCESS );
}
/*}}}*/
/* 'ardp_lexer_process_reader() {{{ */

int ardp_lexer_process_reader( lexer_reader reader, void *_Nullable reader_args)
{
    if (reader == NULL)
        return 1;

    int status = 0;

    if ( !ardp_lexer_is_ready() )
        return 2;

    shared_lexer->finished = 0;

    uint8_t *buf = malloc(sizeof(*buf)*BUFSIZE);

    size_t   have = 0;
    uint8_t *mark = NULL;
    bool     eof  = false;

    uint8_t  nbufsize = 1;

      while (!shared_lexer->finished) {
            uint8_t *p = buf + have;
            size_t space = BUFSIZE - have;

            if (space == 0) {
                    /* Finished space while scanning token without the data; Grow buffer*/
                    uint8_t* newbuf = malloc(sizeof(*buf)*(++nbufsize * BUFSIZE));
                    p = newbuf + have;
                    space = nbufsize - have;

                    /* patch up the pointers */
                    if (shared_lexer->env.ts != 0)
                        shared_lexer->env.ts = newbuf + (shared_lexer->env.ts - buf);

                    shared_lexer->env.te = newbuf + (shared_lexer->env.te - buf);
                    memcpy( newbuf, buf, have );
                    free(buf);
                    buf = newbuf;
            }

            int len = reader(p, space, reader_args);
            if ( len < space ) {
                    eof = true;
                    shared_lexer->finished = 1;
            }

            status = ardp_lexer_process_block( p, len, mark, eof );

            if (shared_lexer->env.cs == turtle_error ) {
                    /* Machine failed before finding a token.
                     *  I'm not yet sure if this
                           * is reachable. */
                     status = ARDP_LEXER_GENERIC_ERROR;
                     if (buf)
                        free(buf);
                     return status;
            }

            /* Decide if we need to preserve anything. */
            uint8_t *preserve = shared_lexer->env.ts;
            if ( preserve == 0 ) {
                have = 0;
            } else {
                /* There is data that needs to be shifted over. */
                have = (p+len) - preserve;
                memmove( buf, preserve, have );
                unsigned int shiftback = preserve - buf;
                if ( shared_lexer->env.ts != 0 )
                    shared_lexer->env.ts -= shiftback;
                shared_lexer->env.te -= shiftback;

                preserve = buf;
            }
      }

        if (buf)
          free(buf);

        //dispatch_queue_t *q = &shared_lexer->lexer_queue;
        //dispatch_queue_t *e = &shared_lexer->event_queue;

        // Optimalization to free CPU operations
        //while(!dispatch_queue_isempty(*q) || !dispatch_queue_isempty(*e))
        //        usleep(25);

        return status;
}


int ardp_lexer_process_reader_BUG( lexer_reader reader, void *_Nullable reader_args)
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
        dispatch_queue_t *e = &shared_lexer->event_queue;

        // Optimalization to free CPU operations
        while(!dispatch_queue_isempty(*q) || !dispatch_queue_isempty(*e))
                usleep(25);

        return status;
}
/*}}}*/
/* ardp_lexer_process_reader_old()  {{{ */
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
