#include <stdio.h>
#include <errno.h>

#include <ardp/lexer.h>
#include <ardp/grammar.turtle.h>

#include <ardp/color.h>
#include <ardp/util.h>
#include <ardp/error.h>

#include "config.h"


struct lexer_t* shared_lexer;

int lexer_init(void);
int lexer_set_grammar(enum grammar);
int lexer_set_handler((void *)fc);
int lexer_set_handler(error);

enum {
    READY,
    PAUSED,
    TOKENIZING,
    CREATED,
    INITALIZED,
}

typedef struct {
  struct {
    int cs;
    int eof;
    char buf[];
  } internal;

  enum state;

  enum tokens;

  struct {
    void (*on_error)(int line, int column, int code, const char *description);
    void (*on_get_more_data)(reader_t reader);
    void (*token)(enum tokens token, const char* content);
    void (*pause)();
    void (*restore)();
    void (*destroy)();
    void (*will_destroy)();
    void (*will_emit_token)();
    void (*did_emit_token)();
  } cb;

} lexer_obj;

typedef lexer_obj lexer_t = { 0 };

static lexer_t lexer;

int lexer_create( void );

ardp_status_t status;

ardp_config;
ardp_lexer_config;
ardp_parser_config;

ardp_lexer_config_default( ardp_lexer_config * cfg ); // fill structure with defaults;

ardp_status_t ardp_lexer_init( ardp_lexer_config* cfg );

ardp_lexer_process_block( char[] block, size_t len );
ardp_lexer_process_file( FILE *file );
ardp_lexer_process_url( const char * url);


ardp_status_t ardp_lexer_process( void ) {
    if ( ardp_lexer_ready() is false ) {
        ardp_error_msg(ERROR, "LEXER", " >>> Lexer has not been initalized at this point.");
        return ARDP_FAILURE;
    }

    while ( data in reader() ) {
       readintokens();
       token_emit();
    }

};
