/*! @file check_rdf_term_is.c
 *  @brief Test to spot regression in string implemenation.
 *
 * This files tests the function which are checks length of string
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
  if (!term_is_uri(s))
      return EXIT_FAILURE;

  struct rdf_term *c = rdf_term_from_curie( string_create( "rdf:shortStuff" ) );
  if (!term_is_curie(c))
      return EXIT_FAILURE;

  struct rdf_term *b = rdf_term_from_blank( string_create( "blank1" ) );
  if (!term_is_blank(b))
      return EXIT_FAILURE;

  struct rdf_term *l1 = rdf_term_from_literal( string_create( "literal1" ), NULL, NULL );
  if (!term_is_literal(l1))
      return EXIT_FAILURE;


  return EXIT_SUCCESS;
}
