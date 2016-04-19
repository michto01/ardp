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
%%{
    machine turtle;
    include rdf_turtle_tokens "../grammars/rdf.turtle.tokens.rl";

    alphtype unsigned char;
    access shared_lexer->env.;

    action u8_create {
        if (shared_lexer->string != NULL)
                string_dealloc(shared_lexer->string);
        shared_lexer->string = string_new();
    }

    action u8_finish {
            if (shared_lexer->string) {
                string_finish(shared_lexer->string);
            }
    }

    action u8_push {
            if (shared_lexer->string) {
                string_append_char(&shared_lexer->string, fc);
            }
    }

    action u8_check {
                if (shared_lexer->string) {
                        if (shared_lexer->string[string_strlen(shared_lexer->string)] == '.') {
                                struct string_header *x = string_hdr(shared_lexer->string);
                                shared_lexer->string[x->length--] = '\0';
                        }
                }
    }

    action mark {
        mark = p;
    }

    action unescape {
        uint32_t codepoint = hex(mark, p - mark);
        mark = NULL;
        if (likely(shared_lexer->string))
                if (!string_append_utf8(&shared_lexer->string, codepoint))
                        shared_lexer->finished = 1;
    }

    # TERMINALS

    UCHAR     = '\\' . ( ('u' xdigit{4} >mark %unescape) | ('U' xdigit{8} >mark %unescape) )
              ;

    ECHAR     = '\\'. (
                        ('t' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\t');
                              }
                        )
                        |
                        ('b' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\b');
                              }
                        )
                        |
                        ('n' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\n');
                              }
                        )
                        |
                        ('r' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\r');
                              }
                        )
                        |
                        ('f' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\f');
                              }
                        )
                        |
                        ('"' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\"');
                              }
                        )
                        |
                        ('\'' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\'');
                              }
                        )
                        |
                        ('\\' ${
                                if (shared_lexer->string)
                                        string_append_char(&shared_lexer->string, '\\');
                              }
                        )
              )
              ;

    # FIXME: This is not as specification specifies the IRIREF, is it enought ?
    IRI_VALUE = (^([<>{}\"`^\\] | 0x00 .. 0x20) $u8_push | UCHAR )* >u8_create %u8_finish;

    #IRIREF    = '<' (PN_CHARS | '.' | ':' | '/' | '\\' | '#' | '@' | '%' | '&' | UCHAR)* '>';
    IRIREF = '<' IRI_VALUE :>> '>' %! { ardp_fprintf(stderr, kARDPColorCyan, "Lexer error on line: %lld, reason: IRI value contains forbidden characters.\n", shared_lexer->line); };

    STRING_VALUE_QUOTE             = ( ^(0x22 | 0x5C | 0xA | 0xD) $u8_push | ECHAR | UCHAR )*;
    STRING_VALUE_SINGLE_QUOTE      = ( ^(0x27 | 0x5C | 0xA | 0xD) $u8_push | ECHAR | UCHAR )*;

    STRING_VALUE_LONG_QUOTE        = ( ('"' | '""')? (^('"' | '\\' | "'''") $u8_push | ECHAR | UCHAR) )*;
    STRING_VALUE_LONG_SINGLE_QUOTE = ( ("'" | "''")? (^("'" | '\\' | '"""') $u8_push | ECHAR | UCHAR) )*;

    STR_Q    = ( STRING_VALUE_QUOTE        >u8_create %u8_finish );
    STR_DQ   = ( STRING_VALUE_SINGLE_QUOTE >u8_create %u8_finish );

    STR_LQ   = ( STRING_VALUE_LONG_QUOTE        >u8_create %u8_finish );
    STR_LDQ  = ( STRING_VALUE_LONG_SINGLE_QUOTE >u8_create %u8_finish );

    STRING_LITERAL_QUOTE        = '"' STR_Q :>> '"'  %! { ardp_fprintf(stderr, kARDPColorCyan, "Lexer error on line:%lld STRING value contains forbidden characters.\n", shared_lexer->line); };
    STRING_LITERAL_SINGLE_QUOTE = "'" STR_DQ :>> "'"  %! { ardp_fprintf(stderr, kARDPColorCyan, "Lexer error on line:%lld: String value contains forbidden characters.\n", shared_lexer->line); };

    STRING_LITERAL_LONG_QUOTE        = '"""' . STR_LQ :>  '"""';
    STRING_LITERAL_LONG_SINGLE_QUOTE = "'''" . STR_LDQ :> "'''";

    BLANK_NODE_LABEL = '_:' >u8_create
                        (PN_CHARS_U $u8_push | digit $u8_push )
                              (
                                (((PN_CHARS | '.')*  (PN_CHARS)) $u8_push)
                              )?

                        %u8_finish;


    QNAME     = (PN_PREFIX? ':' PN_LOCAL?) >u8_create %u8_finish $u8_push;


    #
    ## Scanner (Lexer with backtracking)
    #

    main := |*

        PREFIX        => { lexer_emit_token_const(PREFIX); };
        SPARQL_PREFIX => { lexer_emit_token_const(SPARQL_PREFIX); };
        BASE          => { lexer_emit_token_const(BASE); };
        SPARQL_BASE   => { lexer_emit_token_const(SPARQL_BASE); };

        BLANK_NODE_LABEL  { lexer_emit_u8_token(BLANK_LITERAL); };
        QNAME             { lexer_emit_u8_token(QNAME); };
        IRIREF            { lexer_emit_u8_token(IRIREF); };


        INTEGER { lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); };
        DECIMAL { lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); };
        DOUBLE  { lexer_emit_token( DOUBLE_LITERAL, var(ts), var(te) - var(ts)); };
        BOOLEAN { lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); };

        STRING_LITERAL_LONG_QUOTE        => { lexer_emit_u8_token(STRING_LITERAL); };
        STRING_LITERAL_LONG_SINGLE_QUOTE => { lexer_emit_u8_token(STRING_LITERAL); };

        STRING_LITERAL_QUOTE        { lexer_emit_u8_token(STRING_LITERAL); };
        STRING_LITERAL_SINGLE_QUOTE { lexer_emit_u8_token(STRING_LITERAL); };

        HAT => {
              lexer_emit_token_const(HAT);
        };
        LANGTAG => {
              // '@'. VALUE
              lexer_emit_token(LANGTAG, var(ts)+1, var(te) - (var(ts)+1));
        };

        A         => { lexer_emit_token_const(A); };
        DOT       => { lexer_emit_token_const(DOT); };
        COMMA     => { lexer_emit_token_const(COMMA); };
        SEMICOLON => { lexer_emit_token_const(SEMICOLON); };

        L_SQUARE  => { lexer_emit_token_const(L_SQUARE); };
        R_SQUARE  => { lexer_emit_token_const(R_SQUARE); };
        L_ROUND   => { lexer_emit_token_const(L_ROUND);  };
        R_ROUND   => { lexer_emit_token_const(R_ROUND);  };
        L_CURLY   => { lexer_emit_token_const(L_CURLY);  };
        R_CURLY   => { lexer_emit_token_const(R_CURLY);  };

        EOL => {
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        };

        COMMENT {
              dispatch_sync(shared_lexer->event_queue, ^{
                  shared_lexer->line++;
              });
        };

        #WS*;
        any; #invalids to be skiped
    *|;

    write data;
}%%
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
        %% write init;
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
        %% write exec;
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
          %% write exec;
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
