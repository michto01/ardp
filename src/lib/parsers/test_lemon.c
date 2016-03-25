/*! @file parser.c
 *
 * Lemon based interface for the generated code.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <ardp/string.h>
#include <ardp/parser.h>
#include <ardp/rdf.h>
#include <ardp/hashmap.h>

#include "turtle_parser.h"
#include "turtle_parser.c"

#define __STDC_WANT_LIB_EXT1__ 1

struct parser* shared_parser_internal;
void* shared_parser;


int clasify_uri(uint8_t* uri)
{
        if (uri[0] == '#')
                return 1;

        return 0;
}

int transform_uri(struct parser* p, struct rdf_term* t)
{
        if (t->type != RDF_TERM_URI)
                return 1;

        const char delim = ':';

        char *search = strrchr((char *)t->value.uri, delim);

        if (search == NULL)
                return 15; // Not prefixed, ignore

        ptrdiff_t loc = search - (char *)t->value.uri; //change signess of the string

        if (loc == 0) {
                /* This is the empty prefix: ":somthing" */
                void* value;
                int r = map_get(p->namespaces, ":", value);

                if (r == ARDP_MAP_OK && value)
                        return string_prepend(t->value.uri, (utf8) value);
                else
                        return 14; // Empty prefix not found.

        } else {
                char* check = strrchr(search - 1, delim); // Check if there is any other delims.
                if (check != NULL)
                        return 13; // Too many delimeters.

                char prefix[loc +1];
                prefix[loc]   = '\0';
                prefix[loc+1] = '\0';

                memcpy(prefix, t->value.uri, loc); // Check if it is correct
                void *value;
                int r = map_get(p->namespaces, delim, value);

                if (r == ARDP_MAP_OK && value) {
                        int s = string_prepend(t->value.uri, (utf8) value);
                        if (!s)
                                return string_prepend(t->value.uri, search + 1);
                        else
                                return s; // Error return
                } else {
                        return 14; // Prefix not found
                }
        }

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
                        /*
                         * | PREFIX | # | PREFIX |
                         *
                         * string_strlen(prefix) + 1 + string_strlen(prefix)
                         */
                }

        }

        while(token) {
                puts(token);
                token = strtok_s(NULL, &strmax, delim, &next_token);
        }
#endif
#endif
        return 0;
}

///  Parser ///

static struct parser* ardp_parser_create_internal(void)
{
        struct parser* p = calloc(1, sizeof(*p));
        if (!p)
                return NULL;

        p->namespaces = map_create(8);
}

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
        return (shared_parser) ? 0:1;
}

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

/*!
 * @fn    ardp_parser_destroy
 * @brief Unallocate parser after it is not necessary anymore.
 */
void ardp_parser_destroy(void)
{
        ParseFree(shared_parser, free);
}

int color_stdout_is_tty = -1;

int main ( int argc, char **argv )
{
        fprintf(stderr, "Starting parser...\n");
        ardp_parser_create();

        ParseTrace(stdout, "$parser: ");

        ardp_parser_exec( PREFIX, 0);
        ardp_parser_exec( COLON,  0);
        ardp_parser_exec( IRIREF, "http://google.com");
        ardp_parser_exec( DOT,    0);

        ardp_parser_exec( SPARQL_PREFIX, 0);
        ardp_parser_exec( QNAME,         "foaf:manga");
        ardp_parser_exec( IRIREF,        "http://google.com");

        ardp_parser_exec( IRIREF,         string_create("http://grog.com#subject"));
        ardp_parser_exec( A,              0);
        ardp_parser_exec( STRING_LITERAL, string_create("2.0"));
        ardp_parser_exec( DOT,            0);

        ardp_parser_exec( /*BLANK_LITERAL*/IRIREF,  string_create("chelsee"));
        ardp_parser_exec( QNAME,          string_create("foaf:Mamlas"));
        ardp_parser_exec( STRING_LITERAL, string_create("Random string to see"));
        ardp_parser_exec( DOT,            0);

        ardp_parser_finish();
        ardp_parser_destroy();
        return EXIT_SUCCESS;
}
