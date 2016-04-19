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
  if (s->type != RDF_TERM_URI || !s->value.uri)
      return EXIT_FAILURE;

  struct rdf_term *c = rdf_term_from_curie( string_create( "rdf:shortStuff" ) );
  if (c->type != RDF_TERM_CURIE || !c->value.uri)
      return EXIT_FAILURE;

  struct rdf_term *b = rdf_term_from_blank( string_create( "blank1" ) );
  if (b->type != RDF_TERM_BLANK || !b->value.blank)
      return EXIT_FAILURE;

  struct rdf_term *l1 = rdf_term_from_literal( string_create( "literal1" ), NULL, NULL );
  if (l1->type != RDF_TERM_LITERAL || !l1->value.literal.string)
      return EXIT_FAILURE;

  rdf_term_free(s);
  rdf_term_free(c);
  rdf_term_free(b);
  rdf_term_free(l1);
  return EXIT_SUCCESS;
}
