/*! @file parser.c
 *
 * Lemon based interface for the generated code.
 *
 * vim: set syntax=c.doxygen
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include <ardp/rdf.h>
#include <ardp/string.h>
#include <ardp/parser.h>
#include <ardp/hashmap.h>
#include <ardp/color.h>
#include <ardp/util.h>

#include "turtle_parser.h"
#include "turtle_parser.c"

#define __STDC_WANT_LIB_EXT1__ 1

/*! Opaque internal structure of the parser */
struct parser* shared_parser_internal;

/*! Opaque parser used by lemon */
static void* shared_parser;


/* transform_uri() {{{ */
static inline int term_is_uri(struct rdf_term *t)
{
        return (t)->type == RDF_TERM_URI;
}

static inline int term_is_curie(struct rdf_term *t)
{
        return (t)->type == RDF_TERM_CURIE;
}

static inline int term_is_blank(struct rdf_term *t)
{
        return (t)->type == RDF_TERM_BLANK;
}

static inline int term_is_literal(struct rdf_term *t)
{
        return (t)->type == RDF_TERM_LITERAL;
}

static int uri_has_scheme(struct rdf_term *t)
{
        if (!t)
                return 0;

        /* RFC3986: scheme ::= ALPHA *( ALPHA | DIGIT | '+' | '-' | '.' ) */
        if (!term_is_uri(t))
                return 0;

        utf8 uri = t->value.uri;

        if (!isalpha(uri[0]))
                return 0; /* Invalid scheme initial character => URI is relative. */

        for(uint8_t c; (c = *++uri) != '\0';) {
                switch(c) {
                        case ':':
                                return 1; /* End of scheme */
                        case '+':
                        case '-':
                        case '.':
                                break; /* Valid sheme separators*/
                        default:
                                if (!isalnum(c))
                                        return 0; /* Invalid scheme character */
                }
        }

        return 0;
}

static int is_relative_uri(struct rdf_term* t)
{
        return !uri_has_scheme(t);
}
/*
uint8_t const *ssechr(uint8_t const *s, uint8_t ch)
{
        __m128i zero = _mm_setzero_si128();
        __m128i cx16 = _mm_set1_epi8(ch); // (ch) replicated 16 times.
        while (1) {
                __m128i  x = _mm_loadu_si128((__m128i const *)s);
                unsigned u = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, x));
                unsigned v = _mm_movemask_epi8(_mm_cmpeq_epi8(cx16, x))
                        & ~u & (u - 1);
                if (v) return s + ffs(v) - 1;
                if (u) return  NULL;
                s += 16;

        }
}
*/
static int expand_curie(struct rdf_term *t)
{
        if (!term_is_curie(t))
                return 0;

        const char delim /*alignas(char)*/ = ':';

        if ((t)->value.uri[0] == delim) {
                void *value;
                utf8 uri;
                int r = map_get(shared_parser_internal->namespaces,":",  &value);
                uri = (utf8)value;

                if ( r == ARDP_MAP_OK && uri ) {
                        int ret = string_append(&uri, (t)->value.uri + 1);
                        string_dealloc((t)->value.uri);
                        (t)->value.uri = uri;
                        return ret;
                } else {
                        return 14; // No prefix found.
                }
        }

        uint8_t *search = strchr((t)->value.uri, delim);

        if (search == NULL)
                return 15; // <Doesn't have the delimenete

        ptrdiff_t loc = search - t->value.uri;

        uint8_t *prefix = calloc(1, sizeof(*prefix) + loc + 2); // ':\0' at the end
        memcpy(prefix, (t)->value.uri, loc + 1);

        var value;
        utf8 str;
        int r = map_get(shared_parser_internal->namespaces, prefix, &value);

        if (r == 0) {
                str = (utf8)value;
                int ret = string_append(&str, (t)->value.uri + 1);
                string_dealloc((t)->value.uri);
                (t)->value.uri = str;
                return ret;
        } else {
                return 14; // No prefix found
        }

        return 20; // Should not readch here
}

int transform_uri(struct parser* p, struct rdf_term** t)
{
        return -1;
// C11 non-working functions {{{
#ifdef __STDC_HAS_LIB_EXT1__
#ifdef __FORCE_DISABLE

        char *token;
        char *next_token; // iternals of the `strtok_s`
        const char delim = ':';

        char  *uri    = (char *)t->value.uri;
        size_t strmax = string_strlen(t->value.uri);
        token = strtok_s(uri, string_strlen(t->value.uri), delim, &next_token);

        if (token) {
                void* value;
                int result = map_get(p->namespaces, token, value);
                if (result == ARDP_MAP_OK && value) {
                }
        }
        while(token) {
                token = strtok_s(NULL, &strmax, delim, &next_token);
        }
#endif
#endif
// }}}
        return 0;
}
/*}}}*/
/* _rebase() {{{  */
static int _rebase(utf8 _Nullable base)
{
        if (unlikely(!shared_parser_internal))
                return 10; // No shared parser = panic!

        if (shared_parser_internal->base)
                string_dealloc(shared_parser_internal->base);

        shared_parser_internal->base = string_copy(base);

        return 0;
}
/*}}}*/
/* _add_namespace() {{{ */
static int _add_namespace(utf8 _Nullable qname, utf8 _Nullable iri)
{
        if (unlikely(!shared_parser_internal))
                return 10; // No shared parser exist

        if (unlikely(!shared_parser_internal->namespaces))
                shared_parser_internal->namespaces = map_create(8);

        return map_push(shared_parser_internal->namespaces, qname, iri);
}
/*}}}*/
/* _triple() {{{ */
int _triple(struct rdf_statement* s)
{
        uint8_t* subject   = NULL;
        uint8_t* predicate = NULL;
        uint8_t* object    = NULL;

        uint8_t* prefix;
        uint8_t* type;

        int c = 0; // subject is blank
        int p = 0; // predicate is blank
        int o = 0; // object is blank
        int t = 0; // object value

        if (s->subject) {
                switch (s->subject->type) {
                        case RDF_TERM_CURIE:
                                expand_curie(s->subject);
                        case RDF_TERM_URI:
                                //transform_uri(shared_parser_internal, &s->subject);
                                subject = s->subject->value.uri;
                                break;
                        case RDF_TERM_BLANK:
                                c = 1;
                                subject = s->subject->value.blank;
                                break;
                        case RDF_TERM_LITERAL:
                                subject = s->subject->value.literal.string;
                                break;
                        default:
                                subject = NULL;
                                break;
                }
        }

        if (s->predicate) {
                switch (s->predicate->type) {
                        case RDF_TERM_CURIE:
                                expand_curie(s->predicate);
                        case RDF_TERM_URI:
                                //transform_uri(shared_parser_internal, &s->predicate);
                                predicate = s->predicate->value.uri;
                                break;
                        case RDF_TERM_BLANK:
                                p = 1;
                                predicate = s->predicate->value.blank;
                                break;
                        case RDF_TERM_LITERAL:
                                predicate = s->predicate->value.literal.string;
                                break;
                        default:
                                predicate = NULL;
                                break;
                }
        }

        if (s->object) {
                switch (s->object->type) {
                        case RDF_TERM_CURIE:
                                expand_curie(s->object);
                        case RDF_TERM_URI:
                                //transform_uri(shared_parser_internal, &s->object);
                                object = s->object->value.uri;
                                break;
                        case RDF_TERM_BLANK:
                                o = 1;
                                object = s->object->value.blank;
                                break;
                        case RDF_TERM_LITERAL: {
                                object = s->object->value.literal.string;
                                if (s->object->value.literal.language)
                                        t = 1;
                                if (s->object->value.literal.datatype)
                                        t = 2;

                                switch(t) {
                                        default:
                                                break;
                                        case 1:
                                                prefix = (uint8_t *)"@";
                                                type   = s->object->value.literal.language;
                                                break;
                                        case 2:
                                                prefix = (uint8_t *)"^^";
                                                type   = s->object->value.literal.datatype;
                                                break;
                                }
                                }
                                break;
                        default:
                                object = NULL;
                                break;
                }
        }

        ardp_fprintf(stdout, kARDPColorRed,   "%s%s ", c ? "_:":"", subject);
        ardp_fprintf(stdout, kARDPColorGreen, "%s%s ", p ? "_:":"", predicate);
        ardp_fprintf(stdout, kARDPColorBold,  "%s%s",  o ? "_:":"", object);
        if (t && shared_parser_internal->extra.show_datatype)
                ardp_fprintf(stdout, kARDPColorYellow, "%s%s", prefix, type);

        ardp_fprintf(stdout, kARDPColorNormal, "\n");

        if (likely(shared_parser_internal))
                shared_parser_internal->stats.n_triples++;

        return 0;
}
/*}}}*/
/* _bnode() {{{ */
utf8 _bnode(void)
{
        if (unlikely(!shared_parser_internal))
                return NULL;

        char buf[20];
        const char* prefix = (shared_parser_internal->extra.bprefix) ? shared_parser_internal->extra.bprefix : "b";

        snprintf(buf, 20, "%s%llu", prefix, shared_parser_internal->n_bnode++);

        return string_create(buf);
}
/*}}}*/

///  Parser ///
/* ardp_parser_create_internal() {{{ */
static struct parser* ardp_parser_create_internal(void)
{
        struct parser* p = calloc(1, sizeof(*p));
        if (!p)
                return NULL;

        p->namespaces        = map_create(8);
        p->cb.rebase         = &_rebase;
        p->cb.add_namespace  = &_add_namespace;
        p->cb.statement      = &_triple;
        p->cb.generate_bnode = &_bnode;
        return p;
} /*}}}*/
/* ardp_parser_destroy_internal() {{{*/
static void ardp_parser_destroy_internal(void)
{
        if (unlikely(!shared_parser_internal))
                return;

        struct parser* this = shared_parser_internal;

        if (this->extra.bprefix) {
                /*FIXME: Should free the bprefix*/
                //char* t = this->extra.bprefix;
                //free(t);
        }
        if (this->base)
                string_dealloc(this->base);

        if (this->namespaces)
                map_free(this->namespaces);

        free(this);
        shared_parser_internal = NULL;
}
/*}}}*/

/* ardp_parser_create() {{{*/
/*!
 * @fn ardp_parser_create
 * @brief Create and initialize all prerequisites for shared_parser.
 */
int ardp_parser_create(void)
{
        shared_parser_internal = ardp_parser_create_internal();
        if (!shared_parser_internal)
                return 1;


        shared_parser = ParseAlloc(malloc);
        return (shared_parser == NULL);
} /*}}}*/
/* ardp_parser_exec() {{{ */
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
void ardp_parser_exec(int type, void* value)
{
        Parse(shared_parser, type, value, shared_parser_internal);
}
/*}}}*/
/* ardp_parser_finish() {{{ */
/*!
 * @fn    ardp_parser_finish
 * @brief Flush parser to end one parsing unit.
 *
 * Signals to parser that we reached end of input (EOF, ...) and that it is
 * supposed to flush all data in buffer.
 *
 * @note Equivalent of calling ardp_parser_exec(0,0);
 */
void ardp_parser_finish(void)
{
        Parse(shared_parser, 0,0, shared_parser_internal);
}
/*}}}*/
/* ardp_parser_destroy() {{{ */
/*!
 * @fn    ardp_parser_destroy
 * @brief Unallocate parser after it is not necessary anymore.
 */
void ardp_parser_destroy(void)
{
        ardp_parser_destroy_internal();
        ParseFree(shared_parser, free);
}
/*}}}*/

void ardp_parser_trace(void)
{
        ParseTrace(stdout, "[@parser]: ");
}

/* TEST-BUILD  {{{ */
#ifdef TEST_BUILD
int color_stdout_is_tty = -1;

int main( int argc, char **argv )
{
        ardp_parser_create();
       // ParseTrace(stdout, "$parser: ");
        {
                /*
                 *
                 * :a :b ( "apple" "banana"  ) .
                 *
                 */
                ardp_parser_exec(QNAME, string_create(":a"));
                ardp_parser_exec(QNAME, string_create(":b"));
                ardp_parser_exec(L_CURLY, 0);
                ardp_parser_exec(STRING_LITERAL, string_create("apple"));
                ardp_parser_exec(STRING_LITERAL, string_create("orange"));
                ardp_parser_exec(STRING_LITERAL, string_create("banana"));
                ardp_parser_exec(R_CURLY, 0);
                ardp_parser_exec(DOT, 0);
        }

        ardp_parser_exec(SPARQL_BASE, 0);
        ardp_parser_exec(IRIREF, string_create("http://mamlas.org/"));

        ardp_parser_exec(SPARQL_PREFIX, 0);
        ardp_parser_exec(QNAME, string_create("ex:"));
        ardp_parser_exec(IRIREF, string_create("http://example.org/#"));

        ardp_parser_exec(SPARQL_PREFIX, 0);
        ardp_parser_exec(COLON, 0);
        ardp_parser_exec(IRIREF, string_create("https://ex.org/"));

        {
                /*
                 *
                 * <#green-goblin>
                 *     rel:enemyOf <#spiderman> ;
                 *         a foaf:Person ;    # in the context of the Marvel universe
                 *             foaf:name "Green Goblin" .
                 *
                 */

                ardp_parser_exec(IRIREF, string_create("#green-goblin"));
                ardp_parser_exec(QNAME,  string_create("rel:enemyOf"));
                ardp_parser_exec(IRIREF, string_create("#spiderman"));
                ardp_parser_exec(SEMICOLON, 0);

                ardp_parser_exec(A, 0);
                ardp_parser_exec(QNAME,  string_create("foaf:Person"));
                ardp_parser_exec(SEMICOLON, 0);

                ardp_parser_exec(QNAME,  string_create("foaf:name"));
                ardp_parser_exec(STRING_LITERAL, string_create("Green Goblin"));
                ardp_parser_exec(DOT, 0);

        }
        {
                /*
                 *
                 * <http://www.w3.org/TR/rdf-syntax-grammar>
                 *   dc:title "RDF/XML Syntax Specification (Revised)" ;
                 *   ex:editor [
                 *         ex:fullname "Dave Beckett";
                 *         ex:homePage <http://purl.org/net/dajobe/>
                 *   ] .
                 *
                 * expected:
                 * IRI dc:title STRING
                 * IRI ex:editor _:b0
                 * _:b0 ex:fullname "Dave Becekt"
                 * _:b0 ex:homePage IRI
                 *
                 */
                ardp_parser_exec(IRIREF, string_create("http://w3.org/TR/rdf-syntax-grammar"));
                ardp_parser_exec(QNAME, string_create("dc:title"));
                ardp_parser_exec(STRING_LITERAL, string_create("RDF/XML Syntax Specs ..."));
                ardp_parser_exec(SEMICOLON, 0);
                ardp_parser_exec(QNAME, string_create("ex:editor"));
                ardp_parser_exec(L_SQUARE, 0);
                ardp_parser_exec(QNAME, string_create("ex:fullname"));
                ardp_parser_exec(STRING_LITERAL, string_create("Dave Beckett"));
                ardp_parser_exec(SEMICOLON, 0);
                ardp_parser_exec(QNAME, string_create("ex:homePage"));
                ardp_parser_exec(IRIREF, string_create("http://purl.org/net/dajobe/"));
                ardp_parser_exec(R_SQUARE, 0);
                ardp_parser_exec(DOT, 0);

        }

        ardp_parser_finish();
        ardp_parser_destroy();

        return EXIT_SUCCESS;
}
#endif /* TEST_BUILD */
/*}}}*/
