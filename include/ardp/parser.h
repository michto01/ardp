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
#include <ardp/rdf.h>
#include <ardp/hashmap.h>

/*! @struct parser
 *
 *  Parser inner structure which describes its behaviour.
 *  It's specific for the TURTLE RDF specification.
 *
 *  For another specifications will require to add other properties.
 */
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
                int expand_iri;
                const char *_Nullable bprefix;
        } extra;
};

/*!
 * @fn ardp_parser_create
 * @brief Create and initialize all prerequisites for shared_parser.
 */
int ardp_parser_create(void);

/*!
 * @fn    ardp_parser_exec
 * @brief Call internal parer to process the token.
 *
 * @param[in] type  Numeric categorization of the token.
 * @param[in] value Token actual value.
 *
 * @note Pre `Lemon` specification, the parser accepts values in void* format,
 *       to allow opaque value handling inside the parser.
 */
void ardp_parser_exec(int type, void *_Nullable value);

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
 * @fn    ardp_parser_add_namespace
 * @brief Add the prefix to the parser stack.
 *
 * If the qname already exists the iri is overwritten.
 */
int ardp_parser_add_namespace(utf8 _Nullable qname, utf8 _Nullable iri);

/*!
 * @fn    ardp_parser_rebase
 * @brief
 */

/*
struct _ardp_turtle_parser {

                uint64_t    n_bnode;

                uint8_t*    baseURI;
                map_t       namespaces;
                struct pair bnodeLabels;

                int         error;
                size_t      n_triples;
                size_t      n_directives;
                size_t      n_errors;

        } ardp_turtle_parser;



        static bool changeBase( const char* base )
        {
                assert( shared_parser_internals );

                struct ardp_turtle_parser* this = shared_parser_internals;

                if(this->baseURI);
                        free(this->baseURI);

                this->baseURI = base;
                return true; // Error checking should be employed
        }

        static bool addNamespace( const char* qname, const char* iri)
        {
                assert( shared_parser_internals );
                struct ardp_turtle_parser* this = shared_parser_internals;
                if( is_prefix_qname(qname) )
                        if ( iri )
                                map_push( qname, iri );
        }
*/

/*

Grammar for the triples parser:

TRIPLE = SUBJECT PREDICATE OBJECT

SUBJECT = ARDP_IRI
        | ARDP_BLANK_NODE

PREDICATE = ARDP_IRI

OBJECT = ARDP_IRI
       | ARDP_BLANK_NODE
       | ARDP_SIMPLE_LITERAL_VALUE
       | ARDP_LANGUAGE_TAGGED_LITERAL_VALUE ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE
       | ARDP_DATATYPE_LITERAL_VALUE ARDP_DATATYPE_LITERAL_IRI

*/

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
