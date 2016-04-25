/*! @file parser.c
 *
 * Lemon based interface for the generated code.
 *
 * vim: set syntax=c.doxygen
 *
 * @author Tomas Michalek <tomas.michalek.st@vbs.cz>
 * @date   2016
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

/*!
 * @fn    u8_concatenate
 * @brief Cut the string up to start marker.
 *
 * @param[in,out] uri Manipulated string.
 * @param[in] start Marker for the beginining of cut.
 *
 * @note The buffer (capacity) remains allocated.
 *
 * @return ARDP_SUCCESS, ARDP_FAILURE
 */
/* u8_concatenate() {{{ */
int u8_concatenate(utf8 uri, size_t start)
{
        if (!uri)
                return ARDP_FAILURE;

        struct string_header *hdr = string_hdr(uri);

        int64_t len = hdr->length - start;
        if (len < 0)
                return ARDP_FAILURE;

        var s = memmove(uri, uri+start, len);
        if (s) {
                hdr->length -= start;
                memset(uri+hdr->length, 0, hdr->capacity - hdr->length);
                return ARDP_SUCCESS;
        }
        return ARDP_FAILURE;
}
/*}}}*/

typedef enum {
        FRONT,
        BACK
} dir;


/*!
 * @fn    term_uri_append
 * @brief Append the string to uri with parameter of direction.
 *
 * @param[in] t   Term to process.
 * @param[in] uri String to append.
 * @param[in] direction Side of the term to append the uri to.
 *
 * @note function is destructive for the uri parameter.
 */
/* term_uri_append() {{{*/
static int term_uri_append(struct rdf_term *t, utf8 uri, dir direction)
{
        if (!t->value.uri)
                return ARDP_FAILURE;

        if (!uri)
                return ARDP_FAILURE;

        int status;

        switch(direction) {
                case FRONT:
                        status = string_append_str(&uri, t->value.uri);
                        if (!status) {
                                string_dealloc(t->value.uri);
                                t->value.uri = uri;
                        }
                        return status;
                case BACK:
                        status = string_append_str(&t->value.uri, uri);
                        string_dealloc(uri);
                        return status;
                default:
                        return ARDP_FAILURE; /* wrong direction */
        }
}
/*}}}*/

/* uri_has_scheme() {{{*/
static inline int uri_has_scheme(struct rdf_term *t)
{
        if (!t)
                return ARDP_FAILURE;

        /* RFC3986: scheme ::= ALPHA *( ALPHA | DIGIT | '+' | '-' | '.' ) */
        if (!term_is_uri(t))
                return ARDP_FAILURE;

        utf8 uri = t->value.uri;

        if (!isalpha(uri[0]))
                return ARDP_FAILURE; /* Invalid scheme initial character => URI is relative. */

        #pragma loop vectorize
        for(uint8_t c; (c = *++uri) != '\0';) {
                switch(c) {
                        case ':':
                                return ARDP_SUCCESS; /* End of scheme */
                        case '+':
                        case '-':
                        case '.':
                                break; /* Valid sheme separators*/
                        default:
                                if (!isalnum(c))
                                        return ARDP_FAILURE; /* Invalid scheme character */
                }
        }

        return ARDP_FAILURE;
}

static inline int is_relative_uri(struct rdf_term* t)
{
        return uri_has_scheme(t) == ARDP_FAILURE;
}
/*}}}*/

/* expand_relative_uri() {{{*/
static int expand_relative_uri(struct rdf_term *t)
{
        if (!term_is_uri(t))
                return ARDP_FAILURE;

        if (!is_relative_uri(t))
                return ARDP_FAILURE;

        if (!shared_parser_internal->base)
                return ARDP_FAILURE;

        return term_uri_append(t, string_copy(shared_parser_internal->base), FRONT);
}
/*}}}*/
/* extract_prefix() {{{*/
static inline int extract_prefix(struct rdf_term *t, char **prefix, size_t *loc)
{
        const char delim = ':';
        uint8_t *search = (uint8_t *) ssechr((char *)(t)->value.uri, delim);
        if (search == NULL)
                return ARDP_FAILURE; // <Doesn't have the delimenete

        ptrdiff_t l = search - t->value.uri;

        *prefix = calloc(1, sizeof(char) * (l + 2)); // ':\0' at the end
        memcpy(*prefix, t->value.uri, l + 1);

        if (*prefix)
                *loc = l+1;

        return ARDP_SUCCESS;
}
/*}}}*/
/* expand_curie() {{{*/
static int expand_curie(struct rdf_term *t)
{
        if (unlikely(!term_is_curie(t)))
                return ARDP_FAILURE;

        const char delim = ':';
        char *dlm  = ":";

        var value;
        int status;

        struct parser *p = shared_parser_internal;

        if (t->value.uri[0] == delim && string_strlen(t->value.uri) > 1) {
                status = map_get(p->namespaces, dlm, &value);
                if (status == ARDP_MAP_OK) {
                        utf8 s = string_copy((utf8) value);
                        u8_concatenate(t->value.uri, 1);
                        return term_uri_append(t, s, FRONT);
                } else {
                        return ARDP_FAILURE; // No prefix found.
                }
        }

        char *prefix;
        size_t l;
        status = extract_prefix(t, &prefix, &l);

        if (status == ARDP_SUCCESS && prefix) {
                status = map_get(p->namespaces, prefix, &value);
                if (status == ARDP_MAP_OK) {
                        utf8 s = string_copy((utf8) value);
                        free(prefix);

                        u8_concatenate(t->value.uri, l);
                        return term_uri_append(t, s, FRONT);
                } else if (status == ARDP_MAP_MISSING) {
                        //Prefix missing
                        free(prefix);
                        return ARDP_FAILURE;
                }
                else {
                        free(prefix);
                        return ARDP_FAILURE; // No prefix found
                }
        }

        if (prefix)
                free(prefix);

        return ARDP_FAILURE; // Should not reach here
}
/*}}}*/

/* transform_uri() {{{ */
int transform_uri(struct parser* p, struct rdf_term** t)
{
        return -1;
//za C11 non-working functions {{{
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
                return ARDP_FAILURE; // No shared parser = panic!

        if (shared_parser_internal->base)
                string_dealloc(shared_parser_internal->base);

        shared_parser_internal->base = string_copy(base);

        return ARDP_SUCCESS;
}
/*}}}*/
/* _add_namespace() {{{ */
static int _add_namespace(utf8 _Nullable qname, utf8 _Nullable iri)
{
        if (unlikely(!shared_parser_internal))
                return ARDP_FAILURE; // No shared parser exist

        if (unlikely(!shared_parser_internal->namespaces))
                shared_parser_internal->namespaces = map_create(8);

        return map_push(shared_parser_internal->namespaces, (char *)string_copy(qname), string_copy(iri));
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
                                if (shared_parser_internal->extra.expand_curie)
                                        expand_curie(s->subject);
                        case RDF_TERM_URI:
                                if (shared_parser_internal->extra.expand_iri)
                                        expand_relative_uri(s->subject);
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
                                if (shared_parser_internal->extra.expand_curie)
                                        expand_curie(s->predicate);
                        case RDF_TERM_URI:
                                if (shared_parser_internal->extra.expand_iri)
                                        expand_relative_uri(s->predicate);
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
                                if (shared_parser_internal->extra.expand_curie)
                                        expand_curie(s->object);
                        case RDF_TERM_URI:
                                if (shared_parser_internal->extra.expand_iri)
                                        expand_relative_uri(s->object);
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

        return string_create((uint8_t *)buf);
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
 * @param[in] line  Line position of the token.
 * @param[in] col   Column position of the token.
 *
 * @note column recognision is not yet implemented in lexers
 *
 * @note Pre `Lemon` specification, the parser accepts values in void* format,
 *       to allow opaque value handling inside the parser.
 */
void ardp_parser_exec(int type, void* value, size_t line, size_t col)
{
        if (likely(shared_parser_internal)) {
                shared_parser_internal->stats.line = line;
                shared_parser_internal->stats.column = col;
        }
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

/* ardp_parser_set_default_base() {{{ */
int ardp_parser_set_default_base(const char* base)
{
        if (unlikely(!shared_parser_internal))
                return ARDP_FAILURE;

        char realbase[130];
        realpath(base,realbase);

        char schemabase[255];
        snprintf(schemabase, 255, "file://%s", realbase);

        shared_parser_internal->base = string_create(schemabase);
        return ARDP_SUCCESS;
}
/*}}}*/
/* ardp_parser_set_option() {{{*/
int ardp_parser_set_option(const uint8_t *key, var *value)
{
        if (!string_generic_cmp(key, (const uint8_t *) "expand:uri", strlen(key))) {
                shared_parser_internal->extra.expand_iri = (int) *value;
                return ARDP_SUCCESS;
        }

        if (!string_generic_cmp(key, (const uint8_t *) "expand:curie", strlen(key))) {
                shared_parser_internal->extra.expand_curie = (int) *value;
                return ARDP_SUCCESS;
        }

        if (!string_generic_cmp(key, (const uint8_t *) "show:datatype", strlen(key))) {
                shared_parser_internal->extra.show_datatype = (int) *value;
                return ARDP_SUCCESS;
        }

        return ARDP_FAILURE;
}
/*}}}*/

/* ardp_parser_trace() {{{*/
void ardp_parser_trace(void)
{
        ParseTrace(stdout, "[@parser]: ");
}
/*}}}*/

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
