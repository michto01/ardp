/*! @file rdf.statement.c
 *
 * Implements the RDF 'statement' interfaces declared in `rdf.h`.
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2016
 */

#include <ardp/rdf.h>

/* rdf_statement_create() {{{ */
struct rdf_statement* rdf_statement_create(void)
{
        struct rdf_statement* statement = calloc (1, sizeof(*statement) );
        return (statement) ? statement : NULL; //pretty shure that now its pointless constuct, but it will not hurt.
} /* }}} */
/* rdf_statement_from_nodes(s,p,o,g) {{{ */
struct rdf_statement* rdf_statement_from_nodes( struct rdf_term* subject,
                                                struct rdf_term* predicate,
                                                struct rdf_term* object,
                                                struct rdf_term* graph )
{
        struct rdf_statement* statement = rdf_statement_create();

        if ( statement == NULL ) {

                //rdf_term_free does NULL check
                rdf_term_free(subject);
                rdf_term_free(predicate);
                rdf_term_free(object);
                rdf_term_free(graph);

                return NULL;
        }

        statement->subject   = subject;
        statement->predicate = predicate;
        statement->object    = object;
        statement->graph     = graph;

        return statement;
} /*}}}*/
/* rdf_statement_copy(statement) {{{ */
struct rdf_statement* rdf_statement_copy(struct rdf_statement* s)
{
        if(!s) // Preflight sanity check
                return NULL;

        struct rdf_statement* s2 = rdf_statement_create();
        if(!s2)
                return NULL;

        // (void)0 should work as ASM NO-OP ... let's test it.
        (!s->subject)   ? (void)0 : rdf_term_copy(s->subject);
        (!s->predicate) ? (void)0 : rdf_term_copy(s->predicate);
        (!s->object)    ? (void)0 : rdf_term_copy(s->object);
        (!s->graph)     ? (void)0 : rdf_term_copy(s->graph);

        return s2;
} /*}}}*/

/* rdf_statement_zero(statement) {{{ */
void rdf_statement_zero(struct rdf_statement* s)
{
        assert(s); // sanity check if we even have something.
        memset(s, 0, sizeof(*s));
} /* }}} */
/* rdf_statement_clear(statement) {{{ */
void rdf_statement_clear(struct rdf_statement* s)
{
        if (!s)
                return;

        // rdf_term_free does NULL check
        rdf_term_free(s->subject);   s->subject   = NULL;
        rdf_term_free(s->predicate); s->predicate = NULL;
        rdf_term_free(s->object);    s->object    = NULL;
        rdf_term_free(s->graph);     s->graph     = NULL;
} /*}}}*/
/* rdf_statement_free(statement) {{{ */
void rdf_statement_free(struct rdf_statement* s)
{
        if(!s)
                return;

        rdf_statement_clear(s);
        free(s);
} /*}}}*/

/* rdf_statement_compare(a,b) {{{ */
int rdf_statement_compare(struct rdf_statement* a, struct rdf_statement* b)
{
        int ret = 0;

        if ( !a || !b ) {
                ptrdiff_t pd = (b - a);     // if either are NULL, return dff order.
                return (pd > 0) - (pd < 0); // return 'sign'
        }

        ret = rdf_term_compare(a->subject,b->subject);

        if(ret)
                goto exit;

        ret = rdf_term_compare(a->predicate,b->predicate);

        if (ret)
                goto exit;

        ret = rdf_term_compare(a->object, b->object);

        if (ret)
                goto exit;

        ret = rdf_term_compare(a->graph, b->graph);

exit:
        return ret;
} /*}}}*/
/* rdf_statement_equals(a,b) {{{ */
int rdf_statement_equals(struct rdf_statement* a, struct rdf_statement* b)
{
        if ( !a || !b )
                return 0;

        if(!rdf_term_equals(a->subject, b->subject))
                return 0;

        if (!rdf_term_equals(a->predicate, b->predicate))
                return 0;

        if (!rdf_term_equals(a->object, b->object))
                return 0;

        if (!rdf_term_equals(a->graph, b->graph))
                return 0;

        return 1;
} /*}}}*/
