/*! @file rdf.term.c
 *
 * Implements the `rdf term`
 *
 */
#include "rdf.h"
#include <ardp/util.h>
#include <ardp/string.h>
#include "../../../include/ardp/string.h"

extern utf8 string_copy(utf8 s);
extern utf8 generate_bnode_id();


struct rdf_term* rdf_term_from_uri(utf8 uri)
{
        if ((!uri) || string_strlen(uri) == 0)
                return NULL; // PANIC quickly.

        struct rdf_term* t = calloc(1, sizeof(*t));

        if (!t)
                return NULL; //Abort

        t->type      = RDF_TERM_URI;
        t->value.uri = string_copy(uri);
        return t;
}

struct rdf_term* rdf_term_from_literal(utf8 literal, utf8 lang, utf8 datatype)
{
        //FIXME: empty language error
        //if (lang /*&& !*lang*/)
        //        return NULL;

        if (lang && datatype)
                return NULL;


        struct rdf_term* t = calloc(1, sizeof(*t));

        if (!t)
                return NULL;

        t->type = RDF_TERM_LITERAL;
        t->value.literal.string = string_copy(literal); //FIXME: add checking for empty string?
        t->value.literal.datatype = (!datatype) ? NULL : string_copy(datatype);
        t->value.literal.language = (!lang)     ? NULL : string_copy(lang);

        return t;
}

struct rdf_term* rdf_term_from_blank( utf8 blank )
{
        struct rdf_term* t = calloc(1, sizeof(*t));
        if (!t)
                return NULL;

        t->type = RDF_TERM_BLANK;

        if (blank) {
                t->value.blank = string_copy(blank);
        } else {
                t->value.blank = generate_bnode_id();
        }

        return t;
}


