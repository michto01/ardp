/*! @file parser.h
 *
 * Interface for the parser, with the interface for the turtle parser.
 */
#pragma once

#if __has_feature( nullability )
#else
#define _Nullable
#define _Nonnull
#define _Null_unspecified
#endif

#include <ardp/string.h>
#include <ardp/hashmap.h>
#include <ardp/rdf.h>

/*! @struct parser
 *
 *  Parser inner structure which describes its behaviour.
 *  It's specific for the TURTLE RDF specification.
 *
 *  For another specifications will require to add other properties.
 */
/* struct parser {{{ */
struct parser {
        /*!
         * Keeps track of the last blank-node id.
         */
        uint64_t n_bnode;

        /*!
         * Bese uri for the <#something> URIs.
         */
        utf8 _Nullable base;

        /*!
         * Namespaces map.
         */
        map_t _Nullable namespaces;

        /*!
         * Statistics for the parser.
         */
        struct {
                /*!
                 * Triples counter
                 */
                size_t n_triples;
                /*!
                 * Counts the directives found while parsing.
                 */
                size_t n_directives;
                /*!
                 * Counts error which occured while parsing (lexer's are separate).
                 */
                size_t n_errors;
                /*!
                 * Current line
                 * @note used to track errors
                 */
                size_t line;
                /*!
                 * Current column
                 * @note used to track errors
                 * @warning Not implemented yet in lexer.
                 */
                size_t column;
        } stats;

        /*!
         * @struct cb
         *
         * Groups together the callback functions.
         *
         * @note Those functions are part of the object and are linked after
         *       creation of the object.
         */
        struct {
                /*!
                 * @fn rebase
                 *
                 * @param[in] base New URI to use as the base.
                 *
                 * @return 0 if successful, non-zero otherwise.
                 *
                 * @note This function copies the `base` variable.
                 */
                int (*_Nullable rebase)(utf8 _Nullable base);

                /*!
                 * @fn     add_namespace
                 * @brief  Adds pair of (id, iri) to the internal namespaces.
                 *
                 * @param[in] qname Prefix to be added to namespaces.
                 * @param[in] iri   IRI that goes with the prefix
                 *
                 * @return 0 on success, 1 otherwise.
                 *
                 * @note prefixes should be ':' "terminated" => s[len -1] = ':'.
                 */
                int (*_Nullable add_namespace)(utf8 _Nullable qname, utf8 _Nullable iri);

                /*!
                 * @fn    statement
                 * @brief Callback function fired when parser completes the statement.
                 *
                 * @param[in] s Emitted statement.
                 *
                 * @return 0 of success, non-zero otherwise.
                 */
                int (*_Nullable statement)(struct rdf_statement* _Nullable s);

                /*!
                 * @fn    generate_bnode
                 * @brief Generates bnode id from interal state of the parser.
                 *
                 * return BNode string on success, NULL otherwise.
                 */
                utf8 _Nullable (*_Nullable generate_bnode)();
        } cb;

        struct {
                int show_datatype;
                int expand_iri;
                int expand_curie;
                const char *_Nullable bprefix;
        } extra;
};
/*}}}*/

/*!
 * @typedef datatype_sys_t
 * @brief   Auxiliary internal token type for parser to distinguish values.
 *
 * @warning The structue is suppset to be filled with only one value and the other,
 *          is supposed to be NULL. If both are send, `rdf_trem_from_literal` will
 *          return NULL.
 */
typedef struct _datatype_sys_t {
        /*!
         *  Denotes that the object has langtag.
         */
        utf8 _Nullable langtag;
        /*!
         * Denotes that the object has datatype.
         */
        utf8 _Nullable datatype;
} datatype_sys_t;

/*!
 * @fn ardp_parser_create
 * @brief Create and initialize all prerequisites for shared_parser.
 *
 * @return 0 if successful, non-zero otherwise.
 */
int ardp_parser_create(void);

/*!
 * @fn    ardp_parser_exec
 * @brief Call internal parer to process the token.
 *
 * @param[in] type  Numeric categorization of the token.
 * @param[in] value Token actual value.
 * @param[in] line  Line position of the token.
 * @param[in] col   Column position of the token.
 *
 * @note column recognision is not yet implemented in lexers
 *
 * @note Pre `Lemon` specification, the parser accepts values in void* format,
 *       to allow opaque value handling inside the parser.
 */
void ardp_parser_exec(int type, void *_Nullable value, size_t line, size_t col);

/*!
 * @fn    ardp_parser_finish
 * @brief Flush parser to end one parsing unit.
 *
 * Signals to parser that we reached end of input (EOF, ...) and that it is
 * supposed to flush all data in buffer.
 *
 * @note Equivalent of calling ardp_parser_exec(0,0);
 */
void ardp_parser_finish(void);

/*!
 * @fn ardp_parser_destroy
 *
 * Destroy shared lexer. And all its underlying structures.
 */
void ardp_parser_destroy(void);

/*!
 * @fn    ardp_parser_add_namespace
 * @brief Add the prefix to the parser stack.
 *
 * @param[in] qname Prefix to be inserted into stack.
 * @param[in] iri   IRI to be associated with the prefix.
 *
 * @warning If the qname already exists the iri is overwritten.
 */
int ardp_parser_add_namespace(utf8 _Nullable qname, utf8 _Nullable iri);

/*!
 * @fn    ardp_parser_set_default_base
 * @brief Set the ARDP default base as the document should default to its URI.
 *
 * @param[in] base Base string buffer to use as the default.
 *
 * @return ARDP_SUCCESS on success, ARDP_FAILURE otherwise.
 */
int ardp_parser_set_default_base(const char *_Nullable base);

/*!
 * @fn    ardp_parser_set_option
 * @brief Set option to fine-tune the options of the parser.
 *
 * @param[in] key Key of setting to setup.
 * @param[in] value Value to set the option with.
 *
 * Avaliable options: show:datatypes, expand:uri, expand:curie
 *
 * @return ARDP_SUCCESS on success, ARDP_FAILURE otherwise.
 */
int ardp_parser_set_option(const uint8_t *_Nullable key, var _Nullable *_Nullable value);

/*!
 * @fn    ardp_parser_trace
 * @brief Print out parser process.
 *
 * Helps with the debugging of the parser as it writes its current stack as well as
 * reductions and other useful informations.
 */
void ardp_parser_trace(void);

/*
typedef enum {
        ARDP_IRI,
        ARDP_BLANK_NODE,
        ARDP_SIMPLE_LITERAL_VALUE,
        ARDP_LANGUAGE_TAGGED_LITERAL_VALUE,
        ARDP_DATATYPE_LITERAL_VALUE,
        ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE,
        ARDP_DATATYPE_LITERAL_IRI,
} ardp_token_type;

typedef struct ardp_parser_s ardp_parser;

ardp_parser *_Nullable ardp_new_parser();

void ardp_free_parser( ardp_parser *_Nullable parser );

int ardp_parser_parse( ardp_parser *_Nonnull parser );

typedef int ( *_Nullable ardp_reader )( unsigned char *_Nonnull buffer,
                                        unsigned len,
                                        void *_Nullable arg );

typedef void ( *_Nullable ardp_handler )( ardp_token_type type,
                                          utf8 _Nullable s,
                                          void *_Nullable arg );

typedef void ( *_Nullable ardp_error )( const char *_Nonnull buffer, size_t len, size_t line );

void ardp_parser_set_reader( ardp_parser *_Nonnull parser,
                             ardp_reader reader,
                             void *_Nullable reader_arg );

void ardp_parser_set_handler( ardp_parser *_Nonnull parser,
                              ardp_handler handler,
                              void *_Nullable handler_arg );
*/
