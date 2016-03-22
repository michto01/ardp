#include <stdio.h>
#include "turtle_parser.h"
#include "turtle_parser.c"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//extern void* ParserAlloc(void*);
//extern int ParserParse(void*, int, const char*);
//extern void ParserFree(void*);

#include <ardp/string.h>

void* shared_parser;

/*!
 * @fn ardp_parser_create
 * @brief Create and initialize all prerequisites for shared_parser.
 */
int ardp_parser_create(void)
{
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
        Parse(shared_parser, type, value);
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
        Parse(shared_parser, 0,0);
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

        ParseTrace(stdout, "@parser: ");

        ardp_parser_exec( PREFIX, 0);
        ardp_parser_exec( COLON,  0);
        ardp_parser_exec( IRIREF, "http://google.com");
        ardp_parser_exec( DOT,    0);

        //ardp_parser_exec( PREFIX, 0);

        ardp_parser_exec( SPARQL_PREFIX, 0);
        ardp_parser_exec( QNAME,         "foaf:manga");
        ardp_parser_exec( IRIREF,        "http://google.com");

        ardp_parser_exec( IRIREF,         string_create("http://grog.com#subject"));
        ardp_parser_exec( A,              0);
        ardp_parser_exec( STRING_LITERAL, string_create("2.0"));
        ardp_parser_exec( DOT,            0);

        ardp_parser_exec( BLANK_LITERAL,  string_create("chelsee"));
        ardp_parser_exec( QNAME,          string_create("foaf:Mamlas"));
        ardp_parser_exec( STRING_LITERAL, string_create("Random string to see"));
        ardp_parser_exec( DOT,            0);

        ardp_parser_finish();
        ardp_parser_destroy();
        return EXIT_SUCCESS;
}
