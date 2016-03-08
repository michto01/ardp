#ifndef __ARDP_LEXER_TURTLE_H__
#define __ARDP_LEXER_TURTLE_H__

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

#include <dispatch/dispatch.h>

enum turtle_token_type {
        A = 201,
        HAT,
        DOT,
        COMMA,
        SEMICOLON,

        L_SQUARE,
        R_SQUARE,
        L_ROUND,
        R_ROUND,
        L_CURLY,
        R_CURLY,

        PREFIX,
        BASE,
        SPARQL_PREFIX,
        SPARQL_BASE,

        STRING_LITERAL,
        BLANK_LITERAL,
        QNAME_LITERAL,
        URI_LITERAL,

        IDENTIFIER,
        LANGTAG,

        INTEGER_LITERAL,
        DECIMAL_LITERAL,
        DOUBLE_LITERAL,
        BOOLEAN_LITERAL,

        INVALID
};

enum { ARDP_SUCCESS = 0, ARDP_FAILURE, ARDP_UNKNOWN };

enum lexer_status {
        ARDP_LEXER_TURTLE_STATUS_UNKNOWN,
        ARDP_LEXER_TURTLE_STATUS_CREATED,
        ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED,
        ARDP_LEXER_TURTLE_STATUS_READY
};

enum lexer_error {
        ARDP_ERROR_LEXER_INIT_PREREQUISITE = 101,
        ARDP_ERROR_LEXER_MALLOC,

        ARDP_LEXER_READER_NOT_READY,
        ARDP_LEXER_GENERIC_ERROR,
        ARDP_LEXER_ERROR,
        ARDP_LEXER_OMEM
};

struct ardp_lexer_turtle_config {
};

typedef int ( *_Nullable lexer_reader )( unsigned char *_Nonnull buffer,
                                         unsigned len,
                                         void *_Nullable arg );

struct token {
        enum turtle_token_type type;
        char *_Nullable lexem;
};

struct lexer {

        dispatch_queue_t _Null_unspecified lexer_queue;
        dispatch_queue_t _Null_unspecified event_queue;

        enum lexer_status state;

        off_t line;
        int finished;

        struct {
                int cs;
                int act;

                uint8_t *_Nullable ts;
                uint8_t *_Nullable te;
        } env;

        struct {
                int level;
                int ( *_Nullable eprintf )( const char *_Nullable str );
        } log;

        struct {
                int ( ^_Nullable token )( enum turtle_token_type type, const char *_Nullable str );
        } cb;
};

typedef void ( ^_Nullable completation_block )( int success );

/**
  * Create new shared instance of lexer.
  *
  * @return Status of operation. Non-null value if error, zero otherwise.
  */
int ardp_lexer_turtle_create( void );

/**
  * Preinitialize lexer with defaults values and prepare it for initalisation.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_turtle_defaults( completation_block handler );

/**
  * Inject configuration into the lexer.
  *
  * @param[in]  cfg Configuration for the lexer internals.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_trutle_init( struct ardp_lexer_turtle_config *_Nullable cfg,
                             completation_block handler );

/**
  * Destroy the shared instance of the lexer.
  *
  * Clean the lexer with all of its subsidiary structures.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_turtle_destroy( void );

/**
  * Get state of the lexer.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
int ardp_lexer_turtle_state( void );

/**
  * Check ready state of the lexer.
  *
  * Shorthand for the `ardp_lexer_turtle_state()` with probing the state
  * to `ARDP_LEXER_STATUS_READY`.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
int ardp_lexer_turtle_is_ready( void );

/**
  * Lex the block of data.
  *
  * Process single block of data
  *
  * @param[in] p      Data to be processed.
  * @param[in] len    Length of the data.
  * @param[in] mark   Helper to process the string marks.
  * @param[in] is_eof End of the file indicator.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_turtle_process_block( uint8_t *_Nullable p,
                                      size_t len,
                                      uint8_t *_Nullable mark,
                                      bool is_eof,
                                      completation_block handler );

/**
  * Process reader until the input is exhousted.
  *
  * Continually process the blocks of data until the reader data stops providing
  * new data to feed the underlying `ardp_lexer_turtle_process_block`.
  *
  * @param[in] reader       Function pointer to reader more data.
  * @param[in] reader_args  Additional arguments for the reader.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_turtle_process_reader( lexer_reader reader,
                                       void *_Nullable reader_args,
                                       completation_block handler );

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* __ARDP_LEXER_TURTLE_H__ */
