/*! @file rdf.term.c
 *
 * Implements the `rdf term`
 *
 */
#include "rdf.h"
#include <stdlib.h>
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

struct rdf_term* rdf_term_copy(struct rdf_term* t)
{
        if (!t)
                return NULL;

        struct rdf_term* c = calloc(1, sizeof(*c));

        if (!c)
                return NULL;


        c->type = t->type;

        switch(c->type) {
                case RDF_TERM_BLANK:
                        c->value.blank = string_copy(t->value.blank);
                        break;

                case RDF_TERM_LITERAL:
                        c->value.literal.string = string_copy(t->value.literal.string);
                        if (t->value.literal.datatype)
                                c->value.literal.datatype
                                        = string_copy(t->value.literal.datatype);
                        if (t->value.literal.language)
                                c->value.literal.language
                                        = string_copy(t->value.literal.language);
                        break;

                case RDF_TERM_URI:
                        c->value.uri = string_copy(c->value.uri);
                        break;

                case RDF_TERM_UNKNOWN:
                default:
                        break;
        }

        return c;
}

void rdf_term_free(struct rdf_term* t)
{
        if(!t)
                return;

        switch(t->type) {
                case RDF_TERM_URI:
                        string_dealloc(t->value.uri);
                        t->value.uri = NULL;
                        break;


                case RDF_TERM_BLANK:
                        string_dealloc(t->value.blank);
                        t->value.blank = NULL;
                        break;

                case RDF_TERM_LITERAL:
                        string_dealloc(t->value.literal.string);
                        string_dealloc(t->value.literal.datatype);
                        string_dealloc(t->value.literal.language);
                        t->value.literal.string   = NULL;
                        t->value.literal.language = NULL;
                        t->value.literal.datatype = NULL;
                        break;

                case RDF_TERM_UNKNOWN:
                default:
                        break;
        }

        free(t);
}

int rdf_term_equals(struct rdf_term* a, struct rdf_term* b)
{
        int ret = 0;

        if (!a || !b)
                return 0;

        if (a->type != b->type)
                return 0;

        if (a == b)
                return 1;

        switch (a->type) {
        case RDF_TERM_URI:
                if (string_strlen(a->value.uri) != string_strlen(b->value.uri))
                        break;
                ret = !string_generic_cmp(a->value.uri, b->value.uri, (int)string_strlen(b->value.uri) );
                break;

        case RDF_TERM_BLANK:
                if (string_strlen(a->value.blank) != string_strlen(b->value.blank))
                        break;
                ret = !string_generic_cmp(a->value.blank, a->value.blank, string_strlen(a->value.blank));
                break;

        case RDF_TERM_LITERAL:

        case RDF_TERM_UNKNOWN:
        default:
                break;
        }

        return 0;
}
