/*! @file check_rdf_term_create.c
 *  @brief Test to spot regression in rdf_term implemenation.
 *
 * This files tests the function which create term itself.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ardp/rdf.h>
#include <ardp/string.h>


int color_stdout_is_tty = -1;

int main( int argc, char **argv )
{
  struct rdf_term *s = rdf_term_from_uri( string_create( "http://subje.ct" ) );
  struct rdf_term *c = rdf_term_from_curie( string_create( "rdf:shortStuff" ) );
  struct rdf_term *l = rdf_term_from_literal( string_create( "literal1" ), NULL, NULL );
  struct rdf_term *b = rdf_term_from_blank( string_create( "blank1" ) );

  struct rdf_statement *st = rdf_statement_from_nodes(s,c,l,b);
  if (!st->subject || !st->predicate || !st->object || !st->graph)
    return EXIT_FAILURE;

  rdf_statement_free(st); // Consequently free-ies all the terms

  return EXIT_SUCCESS;
}
