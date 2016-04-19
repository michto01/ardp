/*! @file check_rdf_term_copy.c
 *  @brief Test to spot regression in rdf_term implemenation.
 *
 * This files tests the function which copy of term.
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
  utf8 x = string_create( "http://subje.ct" );
  struct rdf_term *s = rdf_term_from_uri(x);
  string_dealloc(x);

  struct rdf_term *s2 = rdf_term_copy(s);
  if (!s2 || !s2->value.uri)
      return EXIT_FAILURE;

  struct rdf_term *c = rdf_term_from_curie( (x = string_create( "rdf:shortStuff" )) );
  string_dealloc(x);

  struct rdf_term *c2 = rdf_term_copy(c);
  if (!c2 || !s->value.uri)
      return EXIT_FAILURE;

  struct rdf_term *b = rdf_term_from_blank( (x=string_create( "blank1" )) );
  string_dealloc(x);
  struct rdf_term *b2 = rdf_term_copy(b);
  if (!b2 || !s->value.blank)
      return EXIT_FAILURE;

  struct rdf_term *l1 = rdf_term_from_literal( (x=string_create( "literal1" )), NULL, NULL );
  string_dealloc(x);
  struct rdf_term *l2 = rdf_term_copy(l1);
  if ((!l2) || (!l2->value.literal.string))
      return EXIT_FAILURE;

  rdf_term_free(s);
  rdf_term_free(s2);
  rdf_term_free(c);
  rdf_term_free(c2);
  rdf_term_free(b);
  rdf_term_free(b2);
  rdf_term_free(l1);
  rdf_term_free(l2);
  return EXIT_SUCCESS;
}
