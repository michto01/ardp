#include <stdio.h>
#include <errno.h>
#include <iso646.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include <dispatch/dispatch.h>

#include <ardp/lexer.turtle.h>
//#include <ardp/grammar.turtle.h>

#include <ardp/util.h>
#include <ardp/color.h>
//#include <ardp/error.h>


//#include "config.h"

#define BUFSIZE ( 0x10000 )

/**
  * Global variable holding the shared lexer;
  */
static struct lexer *_Nullable shared_lexer;

#define var( s ) ( shared_lexer->env.s )

static void die( const char *msg );

/* clang-format off */
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

 # Apollon message: telnet $IP $AP_PORT > {"msg":{"type":"Camera","data":{"CurrentState":""}}}

}%% /* clang-format on */

        void format( void )
{ /* formating bug off clang-format 3.7 (will be fixed in next release)*/
}

/* Local function declaration for the internals of lexer object */

static void lexer_emit_token( enum turtle_token_type type, uint8_t *_Nullable str, size_t len )
{
        assert( shared_lexer ); /* sanity check */

        dispatch_async( shared_lexer->event_queue, ^{
          char *p_str;

          p_str    = malloc( ( len + 1 ) * sizeof( *p_str ) );
          p_str[0] = '\0';
          strncat( p_str, ( const char * )str, len );

          shared_lexer->cb.token( type, p_str );
        } );
}

static void lexer_emit_token_const( enum turtle_token_type type )
{
        assert( shared_lexer ); /* sanity check */

        dispatch_async( shared_lexer->event_queue, ^{
          shared_lexer->cb.token( type, NULL );
        } );
}

/* @FIXME: callback resend */
static void lexer_error( const char *str )
{
        // shared_lexer->cb.error(str);
}

void ardp_lexer_turtle_debug()
{
        printf( "Shared lexer %s created.\n", shared_lexer ? "is" : "isn't" );

        const char states[][15] = {"created", "initialized", "ready", "unknown"};

        printf( "It's state is: %s\n", states[shared_lexer->state] );
}

int ardp_lexer_turtle_create()
{
        if ( shared_lexer isnt NULL )
                ardp_lexer_turtle_destroy();

        shared_lexer = malloc( sizeof( struct lexer ) );

        if ( shared_lexer is NULL ) {
                return ARDP_ERROR_LEXER_MALLOC;
        }

        shared_lexer->lexer_queue =
                dispatch_queue_create( "eu.cre8iv.ardp.Lexer", DISPATCH_QUEUE_SERIAL );
        shared_lexer->event_queue = dispatch_queue_create( "eu.cre8iv.ardp.LexerEvent", 0 );

        shared_lexer->state = ARDP_LEXER_TURTLE_STATUS_CREATED;
        return ARDP_SUCCESS;
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

void ardp_lexer_turtle_destroy()
{
        if ( shared_lexer isnt NULL )
                free( shared_lexer );
}

int ardp_lexer_turtle_state()
{
        if ( shared_lexer is NULL )
                return ARDP_LEXER_TURTLE_STATUS_UNKNOWN;
        else {
                __block int state;
                state = shared_lexer->state;
                return state;
        }
}

int ardp_lexer_turtle_is_ready()
{
        return ( ardp_lexer_turtle_state() is ARDP_LEXER_TURTLE_STATUS_READY );
}


void ardp_lexer_turtle_process_block( uint8_t *_Nullable v,
                                      size_t len,
                                      uint8_t *_Nullable mark,
                                      bool is_eof,
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
          %%write exec;
        /* clang-format on */
        handler( ARDP_SUCCESS );
}

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
        uint8_t *mark = NULL;
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

static void die( const char *message )
{
        if ( message )
                fprintf( stderr, "%s\n", message );

        exit( EXIT_FAILURE );
}

int main( int argc, char **argv )
{
        dispatch_queue_t lexer_queue =
                dispatch_queue_create( "eu.cre8iv.ardp.lexer.exec", DISPATCH_QUEUE_SERIAL );

        dispatch_sync( lexer_queue, ^{

          if ( ardp_lexer_turtle_create() isnt ARDP_SUCCESS ) {
                  die( "[PANIC] Couldn't create lexer instance!" );
          }

          ardp_lexer_turtle_defaults( ^( int success ) {
            if ( success isnt ARDP_SUCCESS )
                    die( "Defaults error!\n" );
          } );

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
          __block uint8_t *str = ( uint8_t * )

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
        } );

        if ( shared_lexer->env.cs is turtle_error ) {
                return 100;
        }

        for ( ;; )
                ;

        return 0;
}
