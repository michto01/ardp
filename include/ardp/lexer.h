/*! @file lexer.h
 *
 * Define the formal interface for the ARDP lexer (tokenizer) as well as defines
 * the tokens for the turtle (and N-triples) grammar.
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2015
 *
 * @bug    Internal dispatch queue code needs to be cleaned up and locks need to be
 *         set properly (currently not working as designed).
 */
#pragma once

/* System/Global headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <dispatch/dispatch.h>

/* Project headers */
#include <ardp/util.h>



#ifdef __cplusplus
extern "C" {
#endif /* C++ */

/*!
 * @enum  lexer_status
 * @brief Internal state of the lexer used to guard against restricted memory access.
 */
enum lexer_status {
        ARDP_LEXER_TURTLE_STATUS_UNKNOWN,
        ARDP_LEXER_TURTLE_STATUS_CREATED,
        ARDP_LEXER_TURTLE_STATUS_PREINITIALIZED,
        ARDP_LEXER_TURTLE_STATUS_READY
};

/*!
 * @enum lexer_error
 * @brief Set of errors, which can be encountered at the runtime of the lexer
 */
enum lexer_error {
        ARDP_ERROR_LEXER_INIT_PREREQUISITE = 101,
        ARDP_ERROR_LEXER_MALLOC,

        ARDP_LEXER_READER_NOT_READY,
        ARDP_LEXER_GENERIC_ERROR,
        ARDP_LEXER_ERROR,
        ARDP_LEXER_OMEM
};

enum lexer_log_level {
        ALERT   = 1,    /*!< Should be corrected immediately  */
        CRITICAL,       /*!< Critical conditions */
        ERROR,          /*!< Error conditions */
        WARNING,        /*!< May indicate that an error will occur if action is not taken.  */
        NOTICE,         /*!< Events that are unusual, but not error conditions. */
        INFO,           /*!< Normal operational messages that require no action. */
        DEBUG           /*!< Information useful to developers for debugging the application. */
};


/*!
 * @deprecated No longer in use.
 */
struct token; /* forward declaration */
struct ardp_lexer_config {
        struct {
                int level;
                int ( *_Nullable eprintf )( int level,  const char *_Nullable str );
        } logging;

        struct {
                int ( ^_Nullable stoken )( struct token tok);
                int ( ^_Nullable token  )( int type, const char *_Nullable  value );
        } cb;
};


/*!
 * @fn    lexer_reader
 * @brief Callback shorthand for the data reader (file - sequential read).
 *
 * @param[in] buffer    Buffer of incoming data.
 * @param[in] len       Length of the buffer.
 * @param[in] arg       Additional arguments of the reader.
 *
 * @return State of the read.
 */
typedef int ( *_Nullable lexer_reader )( unsigned char *_Nonnull buffer,
                                         unsigned len,
                                         void *_Nullable arg );

/*!
 * @struct token
 * @brief  Holds the single lexical unit and its value.
 *
 * @FIXME: Geared to be Turtle token type - needs to be generalized.
 */
struct token {
        int type;              /*!< Enumerated token type */
        char *_Nullable lexem; /*!< Actual value of the token (optional) */
};


/*!
 * @struct lexer
 * @brief  Hold informations about the lexer as well as the its internal state.
 *
 * @note   The current implementation hides this structure from the user by declaring
 *         global variable shared_lexer. This is used for convenience as user don't
 *         need to type the lexer in the function call. Future version(s) should
 *         include the option to allow user to specify this on their own if they want
 *         to overload this for eg.
 */
struct lexer {
        /*!
         * Queue (~thread) to run lexer at.
         */
        dispatch_queue_t _Null_unspecified lexer_queue;
        /*!
         * Queue (~thread) for events dispatching.
         */
        dispatch_queue_t _Null_unspecified event_queue;

        /*!
         * Internal state of the lexer.
         */
        enum lexer_status state;

        /*!
         * Current line count in scanned file/block.
         */
        off_t line;

        /*!
         * Denotes if the lexer is still running (used only in file processing)
         *
         * @note Should be moved to callback pointer argument.
         */
        int finished;

        /*!
         * Lexer environment setup used in Ragel.
         *
         * These variables are required by Ragel to work properly.
         */
        struct {
                int cs;         /*!< Current state */
                int act;        /*!< Actual state */

                uint8_t *_Nullable ts; /*!< Token start */
                uint8_t *_Nullable te; /*!< Token end */
        } env;

        /*!
         * Debugger and logger helper and level.
         */
        struct {
                /*!
                 * How verbose is the output
                 */
                int level;

                /*!
                 * Log function to call for the level
                 */
                int ( *_Nullable eprintf )( int level,  const char *_Nullable str );
        } log;

        /*!
         * Callback used in the lexer
         *
         * @note This section was reduced due to rewrite of the lexer states.
         */
        struct {
                /*!
                 * Function which emits the token to the parent object(s).
                 *
                 * @param[in] type  Type of the token.
                 * @param[in] value Optional value of the token.
                 *
                 * @FIXME: Should take arbitrary token type not only the turtle enum.
                 */
                int ( ^_Nullable stoken )( struct token token);
                int ( ^_Nullable token  )( int type, const char *_Nullable  value );
        } cb;
};

//FIXME: remove ( depreciated to conform to C converions functions )
typedef void ( ^_Nullable completation_block )( int success );

/**
  * Create new shared instance of lexer.
  *
  * @return Status of operation. Non-null value if error, zero otherwise.
  */
int ardp_lexer_create( void );

/*!
 * @fn     ardp_lexer_defaults();
 * @brief  Preinitialize lexer with defaults values and prepare it for initialization.
 * @return Status of operation. Non-null if error, zero otherwise.
 */
int ardp_lexer_defaults();

/*!
 * @fn     ardp_lexer_init
 * @brief  Inject configuration into the lexer.
 *
 * @param[in]  cfg     Configuration for the lexer internals.*
 * @return    Status of operation. Non-null if error, zero otherwise.
 */
int ardp_lexer_init( struct ardp_lexer_config *_Nullable cfg );

/**
  * Destroy the shared instance of the lexer.
  *
  * Clean the lexer with all of its subsidiary structures.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
void ardp_lexer_destroy( void );

/**
  * Get state of the lexer.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
int ardp_lexer_state( void );

/**
  * Check ready state of the lexer.
  *
  * Shorthand for the `ardp_lexer_turtle_state()` with probing the state
  * to `ARDP_LEXER_STATUS_READY`.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
bool ardp_lexer_is_ready( void );

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
int ardp_lexer_process_block( uint8_t *_Nullable p, size_t len, uint8_t *_Nullable mark, bool is_eof);

/**
  * Process reader until the input is exhausted.
  *
  * Continually process the blocks of data until the reader data stops providing
  * new data to feed the underlying `ardp_lexer_turtle_process_block`.
  *
  * @param[in] reader       Function pointer to reader more data.
  * @param[in] reader_args  Additional arguments for the reader.
  *
  * @return Status of operation. Non-null if error, zero otherwise.
  */
int ardp_lexer_process_reader( lexer_reader reader, void *_Nullable reader_args);


#ifdef __cplusplus
}
#endif /* C++ */
