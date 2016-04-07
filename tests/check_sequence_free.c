#include <stdio.h>
#include <ardp/sequence.h>
#include <ardp/rdf.h>


int color_stdout_is_tty = -1;

int main (int argc, char **argv)
{
        struct sequence *ss = sequence_create((sequence_free_handler)&rdf_statement_free,NULL,NULL);

        struct rdf_term *s = rdf_term_from_uri(string_create("http://goog.le"));
        struct rdf_term *p = rdf_term_from_uri(string_create("http://predica.te"));
        struct rdf_term *o = rdf_term_from_uri(string_create("http://obje.ct"));

        struct rdf_statement *x = rdf_statement_from_nodes(s,p,o,NULL);
        struct rdf_statement *y = rdf_statement_copy(x);
        struct rdf_statement *z = rdf_statement_copy(y);

        sequence_push(ss, x);
        sequence_push(ss, y);
        sequence_push(ss, z);

        rdf_statement_free(x);
        fprintf(stderr, "Freed");
        //rdf_statement_free(y);
        //rdf_statement_free(z);

        sequence_free(ss);

        return 0;
}
