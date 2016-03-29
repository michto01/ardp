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

#include <ardp/rdf.h>
#include <ardp/string.h>
#include <ardp/parser.h>
#include <ardp/hashmap.h>
#include <ardp/color.h>
#include <ardp/util.h>

#include "turtle_parser.h"
#include "turtle_parser.c"

#define __STDC_WANT_LIB_EXT1__ 1

struct parser* shared_parser_internal;
void* shared_parser;


/* transform_uri() {{{ */
int transform_uri(struct parser* p, struct rdf_term** t)
{
        return -1;


        if ((*t)->type != RDF_TERM_URI)
                return 1;

        if (unlikely(p->namespaces))
                p->namespaces = map_create(8);

        //map_push(p->namespaces, string_create(":"), string_create("http://ex.org/"));

        const char delim = ':';
        const char pref  = '#';

        if ((*t)->value.uri[0] == pref) {
                if (p->base) {
                        // Check for the prefix[0] for double append
                        return string_prepend(&(*t)->value.uri, p->base);
                }
                return 16;
        }

        char *search = strrchr((char *)(*t)->value.uri, delim);

        if (search == NULL)
                return 15; // Not prefixed, ignore

        ptrdiff_t loc = search - (char *)(*t)->value.uri; //change signess of the string

        if (loc == 0) {
                /* This is the empty prefix: ":something" */
                void* value;
                int r = map_get(p->namespaces, string_create(":"), &value);


                if (r == ARDP_MAP_OK && value)
                        return string_prepend(&(*t)->value.uri, (utf8) value);
                else
                        return 14; // Empty prefix not found.
        } else {
                char* check = strrchr(search - 1, delim); // Check if there is any other delims.
                if (check != NULL)
                        return 13; // Too many delimeters.


                char prefix[loc + 1];
                prefix[loc]= '\0';
                memcpy(prefix, (*t)->value.uri, loc); // Check if it is correct


                void *value;
                int r = map_get(p->namespaces, prefix, &value);

                if (r == ARDP_MAP_OK && value) {
                        int s = string_prepend(&(*t)->value.uri, (utf8) value);
                        if (!s)
                                return string_prepend(&(*t)->value.uri, search + 1);
                        else
                                return s; // Error return
                } else {
                        return 14; // Prefix not found
                }
        }
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

        return map_push(shared_parser_internal->namespaces, (char *) qname, iri);
}
/*}}}*/
/* _triple() {{{ */
int _triple(struct rdf_statement* s)
{
        uint8_t* subject;
        uint8_t* predicate;
        uint8_t* object;

        switch (s->subject->type) {
        case RDF_TERM_URI:
                transform_uri(shared_parser_internal, &s->subject);
                subject = s->subject->value.uri;
                break;
        case RDF_TERM_BLANK:
                subject = s->subject->value.blank;
                break;
        case RDF_TERM_LITERAL:
                subject = s->subject->value.literal.string;
                break;
        default:
                subject = NULL;
                break;
        }

        switch (s->predicate->type) {
        case RDF_TERM_URI:
                transform_uri(shared_parser_internal, &s->predicate);
                predicate = s->predicate->value.uri;
                break;
        case RDF_TERM_BLANK:
                predicate = s->predicate->value.blank;
                break;
        case RDF_TERM_LITERAL:
                predicate = s->predicate->value.literal.string;
                break;
        default:
                predicate = NULL;
                break;
        }

        switch (s->object->type) {
        case RDF_TERM_URI:
                transform_uri(shared_parser_internal, &s->object);
                object = s->object->value.uri;
                break;
        case RDF_TERM_BLANK:
                object = s->object->value.blank;
                break;
        case RDF_TERM_LITERAL:
                object = s->object->value.literal.string;
                break;
        default:
                subject = NULL;
                break;
        }

        ardp_fprintf(stdout, kARDPColorRed,   "%s%s ", s->subject->type == RDF_TERM_BLANK ? "_:":"", subject);
        ardp_fprintf(stdout, kARDPColorGreen, "%s%s ", s->predicate->type == RDF_TERM_BLANK ? "_:":"", predicate);
        ardp_fprintf(stdout, kARDPColorBold,  "%s%s\n", s->object->type == RDF_TERM_BLANK ? "_:":"", object);

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

        p->namespaces = map_create(8);


        p->cb.rebase         = &_rebase;
        p->cb.add_namespace  = &_add_namespace;
        p->cb.statement      = &_triple;
        p->cb.generate_bnode = &_bnode;
        return p;
} /*}}}*/
/* ardp_parser_create() {{{*/
/*!
 * @fn ardp_parser_create
 * @brief Create and initialize all prerequisites for shared_parser.
 */
int ardp_parser_create(void)
{
        shared_parser_internal = ardp_parser_create_internal();
        if (!shared_parser_internal)
                return 0;


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
        ParseFree(shared_parser, free);
}
/*}}}*/

int color_stdout_is_tty = -1;


int main ( int argc, char **argv )
{
        ardp_parser_create();
        //ParseTrace(stdout, "$parser: ");
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
