/*! @file rdf.term.c
 *
 * Implements the `rdf term`
 *
 */
#include <stdlib.h>

#include <ardp/rdf.h>
#include <ardp/util.h>
#include <ardp/string.h>

//extern utf8 string_copy(utf8 s);

struct rdf_term* rdf_term_from_uri(utf8 uri)
{
        if (!uri)
                return NULL; // PANIC quickly.

        struct rdf_term* t = calloc(1, sizeof(*t));
        if (!t)
                return NULL; //Abort

        if (  string_strlen(uri) == 0 ) {
                t->type      = RDF_TERM_URI;
                t->value.uri = string_create_n("null", 4, 4);
        } else {
                t->type      = RDF_TERM_URI;
                t->value.uri = string_copy(uri);
        }
        return t;
}

struct rdf_term* rdf_term_from_curie(utf8 uri)
{
        if (!uri)
                return NULL; // Panic, fast.

        struct rdf_term *t = calloc(1, sizeof(*t));
        if (!t)
                return NULL; // Abort after unsucessfull allocation.

        t->type      = RDF_TERM_CURIE;
        t->value.uri = string_copy(uri);

        return t;
}

struct rdf_term* rdf_term_from_literal(utf8 literal, utf8 lang, utf8 datatype)
{
        if (lang && !*lang)
                return NULL;

        if (lang && datatype)
                return NULL;

        struct rdf_term* t = calloc(1, sizeof(*t));

        if (!t)
                return NULL;

        t->type = RDF_TERM_LITERAL;
        t->value.literal.string   = string_copy(literal); //FIXME: add checking for empty string?
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

        if (blank)
                t->value.blank = string_copy(blank);
        else
                return NULL;

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
                case RDF_TERM_CURIE:
                        c->value.uri = string_copy(t->value.uri);
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
                case RDF_TERM_CURIE:
                case RDF_TERM_URI: {
                        if (t->value.uri)
                                string_dealloc(t->value.uri);
                        t->value.uri = NULL;
                        break;
                }

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

int rdf_term_equals(const struct rdf_term* a, const struct rdf_term* b)
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
        case RDF_TERM_CURIE:
                if (string_strlen(a->value.uri) != string_strlen(b->value.uri))
                        break;
                ret = !string_generic_cmp(a->value.uri, b->value.uri,
                                (int)string_strlen(b->value.uri) ); break;

        case RDF_TERM_BLANK:
                if (string_strlen(a->value.blank) !=
                                string_strlen(b->value.blank)) break;
                ret = !string_generic_cmp(a->value.blank, b->value.blank,
                                string_strlen(a->value.blank)); break;

        case RDF_TERM_LITERAL:
                /* VALUE */
                if (string_strlen(a->value.literal.string) !=
                                string_strlen(b->value.literal.string))
                        break;
                ret = !string_generic_cmp(a->value.literal.string,
                                b->value.literal.string,
                                string_strlen(a->value.literal.string));
                if(!ret)
                        break;
                /* LANGTAG */
                if (a->value.literal.language && b->value.literal.language) {
                         ret = !string_generic_cmp(a->value.literal.language,
                                b->value.literal.language,
                                string_strlen(a->value.literal.language));
                        if(!ret)
                                break;
                } else if (a->value.literal.language || b->value.literal.language) {
                        ret = 0; /* Only one has langtag */
                        break;
                }


                if (a->value.literal.datatype && b->value.literal.datatype) {
                        ret = !string_generic_cmp(a->value.literal.datatype,
                                b->value.literal.datatype,
                                string_strlen(a->value.literal.datatype));
                        if(!ret)
                                break;
                } else if (a->value.literal.datatype || b->value.literal.datatype) {
                        ret = 0;
                }
                break;

        case RDF_TERM_UNKNOWN:
        default:
                break;
        }

        return ret;
}

// FIXME: implement comparisons.
int rdf_term_compare(const struct rdf_term* a, const struct rdf_term* b)
{
        int r = 0;

        if (!a || !b) {
                if(!a && !b)
                       return 0; /* Both are equal */

                return a ? 1 : (-1);
        }

        if (a->type != b->type)
               return (a->type - b->type);


        switch(a->type) {
        case RDF_TERM_URI:
        case RDF_TERM_CURIE:
                break;

        case RDF_TERM_BLANK:
                break;

        case RDF_TERM_LITERAL:
                break;

        case RDF_TERM_UNKNOWN:
        default:
                break;
        }

        return r;
}

