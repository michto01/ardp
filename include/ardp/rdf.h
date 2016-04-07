/*! @file rdf.h
 *
 * This file describes the data types specified in #RDF_term.
 *
 */
#pragma once


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <stdio.h>
#include <ctype.h>
/* for ptrdiff_t */
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#include <stdarg.h>

#include "util.h"
#include "string.h"

/*
struct _utf8 {
        size_t   len;
        uint8_t* value;
};
typedef struct _utf8* utf8; // forward declaration of string struct.
*/

/*!
 * @enum  rdf_term_type
 * @brief Denotes type of the RDF term.
 *
 * @note  Specifies subset of operations on 'union'
 */
enum rdf_term_type {
        RDF_TERM_UNKNOWN,
        RDF_TERM_URI,
        RDF_TERM_CURIE,
        RDF_TERM_BLANK,
        RDF_TERM_LITERAL
};

/*!
 * @struct rdf_term_literal_value
 *
 * @note   Literal can have either datatype of language. But one or another
 *         needs to be set
 */
struct rdf_term_literal_value {
        /**
          * String value of the literal.
          */
        utf8 string;
        /**
          * Datatype of the string.
          */
        utf8 datatype; //Should be URI type ?
        /**
          * Language of the string.
          */
        utf8 language;
};

/*!
 * @union rdf_term_value
 *
 * Opaque type for the literals.
 */
union rdf_term_value {
        utf8 uri;
        utf8 blank;
        struct rdf_term_literal_value literal;
};

struct rdf_term {
        enum   rdf_term_type  type;
        union  rdf_term_value value;
};

struct rdf_statement {
        struct rdf_term* subject;
        struct rdf_term* predicate;
        struct rdf_term* object;
        struct rdf_term* graph; // To be implemented with TRiG parser.
};

/* RDF_TERM {{{ */
/*!
 * @fn    rdf_term_from_uri
 * @brief Create new term from URI.
 *
 * @param[in] uri URI to process.
 *
 * @return Term, NULL if error.
 *
 * @note URI Should be prepared beforehand.
 */
struct rdf_term* rdf_term_from_uri(utf8 uri);

/*!
 * @fn    rdf_term_from_curie
 * @brief Create new term from CURIE.
 *
 * @param[in] uri The string to transform into.
 *
 * @return Term, NULL if error.
 *
 */
struct rdf_term* rdf_term_from_curie(utf8 uri);

/*!
 * @fn    rdf_term_from_blank
 * @brief Create new term from BLANK node.
 *
 * @param[in] blank BlankNode to expand or to generate new id.
 *
 * @return Term, NULL if error.
 */
struct rdf_term* rdf_term_from_blank(utf8 blank);

/*!
 * @fn    rdf_term_from_literal
 * @brief Create new term from Literal.
 *
 * @param[in] literal  String representing the literal.
 * @param[in] lang     Optional language specifier.
 * @param[in] datatype Optional data type.
 *
 * @return Term representing the literal, NULL if error.
 */
struct rdf_term* rdf_term_from_literal(utf8 literal, utf8 lang, utf8 datatype);

/*!
 * @fn    rdf_term_copy
 * @brief Create new copy of the term.
 *
 * @param[in] t Root term to copy.
 *
 * @return Pointer to new copy, NULL on error.
 */
struct rdf_term* rdf_term_copy(struct rdf_term* t);

/*!
 * @fn    rdf_term_free
 * @brief Free allocated memory.
 *
 * @param[in] t Term to free.
 */
void rdf_term_free(struct rdf_term* t);

/*!
 * @fn    rdf_term_equals
 * @brief Comparison of two terms.
 *
 * @param[in] a First term for comparison.
 * @param[in] b Second term for comparison.
 *
 * @return 1 iff true, 0 otherwise.
 */
int rdf_term_equals(const struct rdf_term* a, const struct rdf_term* b);

/*!
 * @fn    rdf_term_compare
 * @brief Compare two terms.
 *
 * @param[in] a First term for comparison.
 * @param[in] b Second term for comparison.
 *
 * @return a </> b
 */
int rdf_term_compare(const struct rdf_term* a, const struct rdf_term* b);
/*}}}*/
/* RDF_STATEMENT {{{ */

/*! @fn    rdf_statement_create
 *  @brief Create new empty RDF statement.
 *
 *  @return Statement, NULL on error.
 */
struct rdf_statement* rdf_statement_create(void);

/*! @fn    rdf_statement_with_nodes
 *  @brief Creates the RDF statement with specified quadruple.
 *
 *  @param[in] subject    Subject part of the triple.
 *  @param[in] predicate  Predicate part of the triple.
 *  @param[in] object     Object part of the triple.
 *  @param[in] graph      TriG graph component (not in TURTLE),
 *
 *  @return Statement, NULL on error.
 *
 *  @note  All fields of statement are optional.
 */
struct rdf_statement* rdf_statement_from_nodes( struct rdf_term* subject,
                                                struct rdf_term* predicate,
                                                struct rdf_term* object,
                                                struct rdf_term* graph );

/*! @fn    rdf_statement_zero
 *  @brief Ensures that all fields of statement are NULL.
 *
 *  @note  To be used on static properties as.
 *
 *  @param[in] s RDF structure to be zeroed.
 */
void rdf_statement_zero(struct rdf_statement* s);

/*! @fn    rdf_statement_copy
 *  @brief Make copy of `rdf_statement`.
 *
 *  @param[in] s RDF statement to copy.
 *
 *  @return Pointer to copy or NULL.
 */
struct rdf_statement* rdf_statement_copy(struct rdf_statement* s);

/*! @fn    rdf_statement_free
 *  @brief Clear all fields of the statement.
 *
 *  @param[in] s RDF statement to clear.
 */
void rdf_statement_clear(struct rdf_statement* s);

/*! @fn    rdf_statement_free
 *  @brief Free the RDF statement from memory.
 *
 *  @param[in] s RDF statement to free.
 */
void rdf_statement_free(struct rdf_statement* s);

/*!
 * @fn rdf_statement_compare
 *
 * @param[in] a Base structure.
 * @param[in] b Structure to be compared against base structure.
 *
 * @return <0 if a is before b, 0 if equal, >0 if a is after b<Paste>
 */
int rdf_statement_compare(struct rdf_statement* a, struct rdf_statement* b);

/*!
 * @fn  rdf_statement_equals
 *
 * Compares the two staments for equality.
 *
 * @param[in] a First argument
 * @param[in] b Second argument
 *
 * @return '1' iff 'a' == 'b'
 */
int rdf_statement_equals(struct rdf_statement* a, struct rdf_statement* b);

/* }}} */
