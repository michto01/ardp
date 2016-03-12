/* vim: set ts=8 sw=4 tw=0 noet : set foldmethod=marker */

// HEADERS {{{ --------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>              /* Included for 'stderr' */
#include <iso646.h>             /* verbose || and && */
#include <string.h>             /* C buffer manipulation functions */
#include <assert.h>             /* Asserts in the code */
#include <dispatch/dispatch.h>  /* Clang GCD dispatch_* functions */

#include <ardp/lexer.turtle.h>
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
%%{
    machine turtle;
    include rdf_turtle_tokens "../grammars/rdf.turtle.tokens.rl";

    alphtype unsigned char;
    access shared_lexer->env.;

    main := |*

        PREFIX        => { lexer_emit_token_const(PREFIX); };
        SPARQL_PREFIX => { lexer_emit_token_const(SPARQL_PREFIX); };
        BASE          => { lexer_emit_token_const(BASE); };
        SPARQL_BASE   => { lexer_emit_token_const(SPARQL_BASE); };


        QNAME => {
              lexer_emit_token(QNAME_LITERAL, var(ts), var(te) - var(ts));
        };

        IRIREF => {
              // '<' URI_LITERAL '>'
              lexer_emit_token(URI_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        };

        INTEGER   => { lexer_emit_token(INTEGER_LITERAL, var(ts), var(te) - var(ts)); };
        DECIMAL   => { lexer_emit_token(DECIMAL_LITERAL, var(ts), var(te) - var(ts)); };
        DOUBLE    => { lexer_emit_token(DOUBLE_LITERAL,  var(ts), var(te) - var(ts)); };
        BOOLEAN   => { lexer_emit_token(BOOLEAN_LITERAL, var(ts), var(te) - var(ts)); };


        STRING_LITERAL_QUOTE => {
              lexer_emit_token(STRING_LITERAL, var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        };
        STRING_LITERAL_SINGLE_QUOTE => {
              lexer_emit_token(STRING_LITERAL,  var(ts) + 1, (var(te) - 1) - (var(ts) + 1));
        };
        STRING_LITERAL_LONG_QUOTE => {
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        };
        STRING_LITERAL_LONG_SINGLE_QUOTE => {
              lexer_emit_token(STRING_LITERAL, var(ts) + 3, (var(te) - 3) - (var(ts) + 3));
        };

        HAT => {
              lexer_emit_token_const(HAT);
        };
        LANGTAG => {
              lexer_emit_token(LANGTAG,var(ts), var(te) - var(ts));
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
              dispatch_async(shared_lexer->event_queue, ^{
                shared_lexer->line++;
              });
        };

        COMMENT;
        WS*;

        any; #invalids to be skiped
    *|;

    write data;
}%%
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
/*}}}*/
/* lexer_emit_token_const() {{{ */
static void lexer_emit_token_const( enum turtle_token_type type )
{
        assert( shared_lexer ); /* sanity check */

        dispatch_async( shared_lexer->event_queue, ^{
            shared_lexer->cb.token( type, NULL );

            if (shared_lexer->log.level < NOTICE)
                log( DEBUG, "Token emmitted");
        });
}
/*}}}*/
/* ardp_lexer_turtle_debug() {{{ */
void ardp_lexer_turtle_debug( void )
{
        printf( "Shared lexer %s created.\n", shared_lexer ? "is" : "isn't" );

        const char states[][15] = {"created", "initialized", "ready", "unknown"};

        printf( "It's state is: %s\n", states[shared_lexer->state] );
}
/*}}}*/
/* ardp_lexer_turtle_create() {{{ */
int ardp_lexer_turtle_create(void)
{
        if ( shared_lexer isnt NULL ) {
            log( WARNING, "The lexer did already exist. Destroying it." );
            ardp_lexer_turtle_destroy();
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
int ardp_lexer_turtle_defaults_s(void)
{
    switch(ardp_lexer_turtle_state()) {
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
void ardp_lexer_turtle_defaults( completation_block handler )
{
        if ( ardp_lexer_turtle_state() is ARDP_LEXER_TURTLE_STATUS_CREATED ) {
                struct lexer *this = shared_lexer;
                this->state        = ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED;
                this->line         = 0;
                /* ... */

                handler( ARDP_SUCCESS );
        } else {
                printf( "Wrong state: %d state\n", shared_lexer->state );
                handler( ARDP_FAILURE );
        }
}
/*}}}*/
/* ardp_lexer_turtle_init() {{{*/
int ardp_lexer_turtle_init_s( struct ardp_lexer_turtle_config *_Nullable cfg)
{
    switch( ardp_lexer_turtle_state() ) {
    case ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED:
    case ARDP_LEXER_TURTLE_STATUS_READY:
        /* clang-format off */
        %% write init;
        /* clang-format on */
        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_READY;
        log( INFO, "Shared lexer initialized." );
        return ARDP_SUCCESS;

    default:
        log( ERROR, "Wrong state. Initialization aborted." );
        return ARDP_FAILURE;
    }
}
void ardp_lexer_trutle_init( struct ardp_lexer_turtle_config *_Nullable cfg,
                             completation_block handler )
{
        if ( ardp_lexer_turtle_state() < ARDP_LEXER_TURTLE_STATUS_CREATED ) {
                handler( ARDP_ERROR_LEXER_INIT_PREREQUISITE );
                return;
        }

        /* clang-format off */
          %% write init;
        /* clang-format on */

        //  shared_lexer->cb     = cfg.cb;
        //  shared_lexer->log    = cfg.log;
        //  shared_lexer->env    = cfg.env;
        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_READY;

        handler( ARDP_SUCCESS );
}
/*}}}*/
/* ardp_lexer_turtle_destroy() {{{ */
void ardp_lexer_turtle_destroy()
{
        if ( shared_lexer isnt NULL ) {
                free( shared_lexer );
                log(INFO, "Freed the shared lexer.");
        }
}

int ardp_lexer_turtle_state()
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
bool ardp_lexer_turtle_is_ready()
{
        return ( ardp_lexer_turtle_state() is ARDP_LEXER_TURTLE_STATUS_READY );
}
/*}}}*/
/* ardp_lexer_turtle_process_block() {{{ */
int ardp_lexer_turtle_process_block_s( uint8_t *_Nullable v,
                                     size_t             len,
                                     uint8_t *_Nullable mark,
                                     bool               is_eof)
{
    if ( not ardp_lexer_turtle_is_ready() ) {
        log (ERROR, "Shared lexer is note yet ready to process the data.");
        return ARDP_FAILURE;
    }

    __block uint8_t *_Nullable p  = v; /* Indirection to allow the GCD */
    const   uint8_t *_Nullable pe = p + len; /* Points to fist byte beyond the data */
    const   uint8_t *_Nullable eof= (is_eof) ? pe : nullptr; /* Indicated end of all data*/

    /* clang-format off */
    %% write exec;
    /* clang-format on */

    return ARDP_SUCCESS;
}

void ardp_lexer_turtle_process_block( uint8_t *_Nullable v,
                                      size_t             len,
                                      uint8_t *_Nullable mark,
                                      bool               is_eof,
                                      completation_block handler )
{
        if ( !ardp_lexer_turtle_is_ready() ) {
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
/* ardp_lexer_turtle_reader() {{{ */
void ardp_lexer_turtle_process_reader( lexer_reader reader,
                                       void *_Nullable reader_args,
                                       completation_block handler )
{
        if ( ardp_lexer_turtle_is_ready() ) {
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
        uint8_t *mark = nullptr;
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

int main( int argc, char **argv )
{
          if ( ardp_lexer_turtle_create() isnt ARDP_SUCCESS ) {
                  die( "[PANIC] Couldn't create lexer instance!" );
          }

          int status = ARDP_UNKNOWN;

          status = ardp_lexer_turtle_defaults_s();

          if ( status isnt ARDP_SUCCESS ) {
                die( "Defaults error!\n" );
          }

          ardp_lexer_turtle_debug();

          ardp_lexer_trutle_init( NULL, ^( int success ) {
            if ( success isnt ARDP_SUCCESS )
                    die( "Initialization error!\n" );
          } );

          shared_lexer->cb.token = ^int( enum turtle_token_type type, const char *_Nullable str ) {
            printf( "%d - %s\n", type, str );
            return 0;
          };


          uint8_t *mark;
          /*__block*/
          uint8_t *str = ( uint8_t * )

                  "  @base <http://example.org/> .\n"
                  "  @prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n"
                  "  @prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .\n"
                  "  @prefix foaf: <http://xmlns.com/foaf/0.1/> .\n"
                  "  @prefix rel: <http://www.perceive.net/schemas/relationship/> . \n"
                  ""
                  "  <#green-goblin>\n"
                  "      rel:enemyOf <#spiderman> ;\n"
                  "      a foaf:Person ;    # in the context of the Marvel universe\n"
                  "      foaf:name \"Green Goblin\" .\n"
                  ""
                  "  <#spiderman>\n"
                  "      rel:enemyOf <#green-goblin> ;\n"
                  "      a foaf:Person ;\n"
                  "     foaf:name \'Spiderman\' , \"Человек-паук\"@ru .\n"
                  ""
                  "@prefix : <http://example.org/stuff/1.0/> .\n"
                  "(1 -2.0 -3E-1 true false) :p \"w\" ."
                  ""
                  ":a :b\n"
                  "[ rdf:first \"apple\";\n"
                  "rdf:rest [ rdf:first \"banana\";\n"
                  "rdf:rest rdf:nil ]\n"
                  "] .";


          //"@base foaf: <http://example.org> .\n"
          //                                    "@prefix foaf: <https://example.org>.\n\n\r"
          //                                    "<subject> <predicate> <object> .\n"
          //                                    "[ ] ( _b: ; a \"a\" <local>, <look>) .";


          ardp_lexer_turtle_process_block( str, strlen( ( const char * )str ), mark, true,
                                           ^( int success ) {
                                             if ( success isnt ARDP_SUCCESS ) {
                                                     printf( "Error while processing the block" );
                                             }
                                           } );

        if ( shared_lexer->env.cs is turtle_error ) {
                return 100;
        }

        for ( ;; )/* wait forever */;

        return 0;
}
