/*! @file turtle_parser.y
 *
 * Template to generate the `Lemon` parser for the Turtle grammar.
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2016
 */

/* %name turtle_parser */
%extra_argument { struct parser* p }

//%token_type { utf8 }
//%destructor IRIREF { string_dealloc($$); }

%include {
        #include <assert.h>
        #include <stdio.h>
        #include <stddef.h>
        #include <stdlib.h>
        #include <stdio.h>
        #include <errno.h>

        #include <ardp/rdf.h>
        #include <ardp/string.h>
        #include <ardp/sequence.h>

        #include <ardp/lexer.h>
        #include <ardp/util.h>            // Various utilities
        #include <ardp/parser.h>          // Parser internals

#define YYERROR do {                                                               \
                        fprintf(stderr, "\n\nERROR\n\n");                          \
                        p->stats.n_errors++;                                       \
                }while(0)

const uint8_t *const xsd_uri    = (uint8_t *) "http://www.w3.org/2001/XMLSchema#";

const uint8_t *const rdfNil     = (uint8_t *) "http://www.nil.org/";
const uint8_t *const rdfFirst   = (uint8_t *) "http://www.w3.org/2001/XMLSchame#First";
const uint8_t *const rdfRest    = (uint8_t *) "http://www.w3.org/2001/XMLSchema#Rest";

#define XSD "xsd:"
const uint8_t *const rdfBoolean = (uint8_t*) XSD"Boolean";
const uint8_t *const rdfInt     = (uint8_t*) XSD"int";
const uint8_t *const rdfFloat   = (uint8_t*) XSD"float";
const uint8_t *const rdfDouble  = (uint8_t*) XSD"double";
const uint8_t *const rdfLong    = (uint8_t*) XSD"long";
const uint8_t *const rdfShort   = (uint8_t*) XSD"short";
const uint8_t *const rdfInteger = (uint8_t*) XSD"integer";
const uint8_t *const rdfDecimal = (uint8_t*) XSD"decimal";
const uint8_t *const rdfString  = (uint8_t*) XSD"string";

}

%parse_failure {
        fprintf(stderr, "Parser encountered error.\n"); /*parser->error.code = 1;*/
        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
}
%parse_accept  {
        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
}
%syntax_error  {
        p->stats.n_errors++;
        fprintf(stderr, "Syntax error: %s\n", NULL); /*parser->error.code = 2;*/
}

%start_symbol turtleDoc

/* [1] turleDoc ::= statement*  {{{ */
turtleDoc ::= statements.
// Root needs not to be recursive, so we use indirection -> statements
statements ::= statements statement.
statements ::= .
/*}}}*/
/* [2] statement ::= directive | triples '.' {{{ */
statement ::= directive.   { p->stats.n_directives++; }
statement ::= triples DOT.
//TODO:statement ::= error(B).    { fprintf(stderr, "Error %d", B); }
/*}}}*/
/* [3] directive ::= prefixID | base | sparqlPrefix | sparqlBase {{{ */
directive ::= prefixID.
directive ::= base.
directive ::= sparqlPrefix.
directive ::= sparqlBase.
/*}}}*/
/* [4] prefixID ::= '@prefix' PNAME_NS IRIREF '.' {{{ */
prefixID ::= PREFIX COLON IRIREF(B) DOT. {
        if(p->cb.add_namespace(":",B)) {
                string_dealloc(B);
                YYERROR;
        }
        string_dealloc(B);
}
prefixID ::= PREFIX QNAME(A) IRIREF(B) DOT. {
        if(p->cb.add_namespace(A,B)) {
                string_dealloc(A);
                string_dealloc(B);
                YYERROR;
        }
        string_dealloc(A);
        string_dealloc(B);
}
/*}}}*/
/* [5] base ::= '@base' IRIREF DOT. {{{ */
base ::= BASE IRIREF(A) DOT. {
        if (p->cb.rebase(A)) {
                string_dealloc(A);
                YYERROR;
        }
        string_dealloc(A);
}
/*}}}*/
/* [5s] sparqlBase ::= "BASE" IRIREF {{{ */
sparqlBase ::= SPARQL_BASE IRIREF(A). {
        if (p->cb.rebase(A)) {
                string_dealloc(A);
                YYERROR;
        }
        string_dealloc(A);
}
/*}}}*/
/* [6s] sparqlPrefix ::= "PREFIX" PNAME_NS IRIREF {{{ */
sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF(B). {
        utf8 colon = string_create(":");
        if (p->cb.add_namespace(colon, B)) {
                string_dealloc(colon);
                string_dealloc(B);
                YYERROR;
        }
        string_dealloc(colon);
        string_dealloc(B);
}
sparqlPrefix ::= SPARQL_PREFIX QNAME(A) IRIREF(B). {
        if (p->cb.add_namespace(A,B)) {
                string_dealloc(A);
                string_dealloc(B);
                YYERROR;
        }

        string_dealloc(A);
        string_dealloc(B);
}
/*}}}*/
/* [6] triples ::= subject predicateObjectList | blankNodePropertyList predicateObjectList?  {{{ */
triples ::= subject(A) predicateObjectList(B). {

        if (A && B) {
                /* have subject and property list with items */
                while ( sequence_size(B) ) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_unshift(B);
                        t->subject = rdf_term_copy(A);

                        p->cb.statement(t);
                        rdf_statement_free(t);
                }
        }

        if (A)
                rdf_term_free(A);
        if (B)
                sequence_free(B);
}
triples ::= blankNodePropertyList(A) predicateObjectList_astr(B). {

        if (A && B) {
                while ( sequence_size(B) ) {
                        struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(B);
                        s->subject = rdf_term_copy(A);

                        p->cb.statement(s);
                        rdf_statement_free(s);
                }
        }

        if (A)
                rdf_term_free(A);
        if (B)
                sequence_free(B);
}

predicateObjectList_astr(A) ::= predicateObjectList(B).  { A = B; }
predicateObjectList_astr(A) ::= . { A = NULL; }
/*}}}*/
/* [7] predicateObjectList ::= verb objectList (';' (verb objectList)?)* {{{ */
predicateObjectList(A) ::= verb(B) objectList(C) predicateObjectList_ast(D). {
        if (B && C) {
                for (size_t i = 0; i < sequence_size(C); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(C, i);

                        t->predicate = rdf_term_copy(B);
                }
                /* @TODO: double copy -> should be optimalized */
                if (D) {
                        while ( sequence_size(D) ) {
                                struct rdf_statement* t2 = (struct rdf_statement*)
                                                                sequence_unshift(D);
                                if (sequence_push(C, t2)) {
                                        sequence_free(C);
                                        sequence_free(D);
                                        rdf_term_free(B);
                                        YYERROR;
                                }
                        }
                        sequence_free(D);
                }

                if (B)
                        rdf_term_free(B);

                A = C;
        }
        else {
                if (D)
                        A = D;
                else
                        A = NULL;
        }

}
// (...)*
predicateObjectList_ast(A) ::= predicateObjectList_ast(B) predicateObjectList_qst(C). {
        if (!B) {
                if (C)
                        A = C;
                else
                        A = NULL;
        } else {
                while ( sequence_size(C) ) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_unshift(C);
                        if (sequence_push(B,t)) {
                                sequence_free(B);
                                sequence_free(C);
                                YYERROR;
                        }
                }
                sequence_free(C);
                A = B;
        }
}
predicateObjectList_ast(A) ::= . { A = NULL; }
// ';' (verb objectList)?
predicateObjectList_qst(A) ::= SEMICOLON verb(B) objectList(C). {
        if (B && C) {
                /* non-empty property list */
                for (size_t i = 0; i < sequence_size(C); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(C, i);
                        t->predicate = rdf_term_copy(B);
                }
                A = C;
        } else {
                A = NULL;
        }
}
predicateObjectList_qst(A) ::= SEMICOLON. { A = NULL; }
/*}}}*/
/* [8] objectList ::= object (',' object)* {{{ */
objectList(A) ::= object(B) objectList_ast(C). {
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, B, NULL);
        if (!C) {
                A = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
                if (!A)
                        YYERROR; /* failed to  create sequence. */
                if ( sequence_push(A, triple) ) {
                        sequence_free(A);
                        YYERROR; /* couldn't push the object into sequence*/
                }
        } else {
                A = C;
                sequence_shift(A, triple); /* prepend object to the sequence */
        }
}
objectList_ast(A) ::= objectList_ast(B) COMMA object(C). {
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, C, NULL);
        if (!B) {
                A = sequence_create((sequence_free_handler) &rdf_statement_free, NULL, NULL);
                if (!A)
                        YYERROR;
        } else {
                A = B;
        }
        sequence_push(A, triple);
}
/* Parts of Turtle specification do not specify the custom error situations not
 * handled by the grammar. this is one of such cases.
 */
objectList_ast(A) ::= objectList_ast(B) COMMA . { /* Custom handling of ES */ A = B; }
objectList_ast(A) ::= . { A = NULL; }
/*}}}*/
/* [9] verb ::= predicate | 'a' {{{ */
verb(A) ::= predicate(B). { A = B; }
verb(A) ::= A. {
        utf8 s = string_create("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
        A = rdf_term_from_uri(s);
        string_dealloc(s);
        if (!A)
                YYERROR;
}
/*}}}*/
/* [10] subject ::= iri | BlankNode | collection {{{ */
subject(A) ::= iri(B).        { A = B; }
subject(A) ::= blankNode(B).  { A = B; }
subject(A) ::= collection(B). { A = B; }
/*}}}*/
/* [11] predicate ::= iri {{{ */
predicate(A) ::= iri(B). { A = B; }
/*}}}*/
/* [12] object ::= iri | BlankNode | collection | blankNodePropertyList | literal {{{ */
object(A) ::= iri(B).                   { A = B; }
object(A) ::= blankNode(B).             { A = B; }
object(A) ::= collection(B).            { A = B; }
object(A) ::= blankNodePropertyList(B). { A = B; }
object(A) ::= literal(B).               { A = B; }
/*}}}*/

/* [13] literal ::= RDFLiteral | NumericalLiteral | BooleanLiteral {{{ */
literal(A) ::= rdfLiteral(B).       { A = B; }
literal(A) ::= numericalLiteral(B). { A = B; }
literal(A) ::= booleanLiteral(B).   { A = B; }
/*}}}*/
/* [14] blankNodePropertyList ::= '[' predicateObjectList  ']' {{{ */
blankNodePropertyList(A) ::= L_SQUARE predicateObjectList(B) R_SQUARE. {

        utf8 s = p->cb.generate_bnode();
        A = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!A) {
                sequence_free(B);
                A = NULL;
                YYERROR;
        } else {
                if (B) {
                        /* Handle non empty list */
                        while(sequence_size(B)) {
                                struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(B);
                                s->subject = rdf_term_copy(A);

                                p->cb.statement(s);
                                rdf_statement_free(s);
                        }
                }
        }
}
/*}}}*/
/* [15] collection ::= '(' object* ')' {{{ */
collection(A) ::= L_CURLY collection_ast(B) R_CURLY. {
        if (!B) {
                utf8 s = string_create(rdfNil);
                A = rdf_term_from_uri(s);
                string_dealloc(s);
                if (!A)
                        YYERROR;
        } else {
                int error = 0;

                struct rdf_term* first  = NULL;
                struct rdf_term* rest   = NULL;
                struct rdf_term* object = NULL;
                struct rdf_term* blank  = NULL;

                uint8_t const *errmsg = NULL;

                { /* RDF:term:First */
                        utf8 tmp = string_create(rdfFirst);
                        first = rdf_term_from_uri(tmp);
                        string_dealloc(tmp);
                        if (!first) {
                                error = 1;
                                goto err;
                        }
                }

                { /* RDF:term:Rest */
                        utf8 tmp = string_create(rdfRest);
                        rest = rdf_term_from_uri(tmp);
                        string_dealloc(tmp);
                        if (!rest) {
                                error = 1;
                                goto err;
                        }
                }

                /* Prepared uri(s), non-empty list  */
                { /* Object initialization */
                        utf8 tmp = string_create(rdfNil);
                        object = rdf_term_from_uri(tmp);
                        string_dealloc(tmp);
                        if (!object) {
                                error = 1;
                                goto err;
                        }
                }

                for (int i = sequence_size(B) - 1; i >= 0; i--) {
                        struct rdf_statement* t = (struct rdf_statement *)
                                                        sequence_get_at(B, i);

                        utf8 s = p->cb.generate_bnode();
                        if (!s) {
                                string_dealloc(s);
                                error = 1;
                                goto err;
                        }

                        blank = rdf_term_from_blank(s);
                        string_dealloc(s);

                        if (!blank) {
                                error = 1;
                                goto err;
                        }

                        { /* rdf:First */
                                t->subject   = blank;
                                t->predicate = first;
                                /* t->object = already set */

                                p->cb.statement(t);
                        }
                        { /* rdf:Rest */
                                struct rdf_term* temp = t->object;

                                t->subject   = blank;
                                t->predicate = rest;
                                t->object    = object;

                                p->cb.statement(t);

                          /* end */
                                t->subject = NULL;
                                t->predicate = NULL;
                                t->object = temp;
                        }

                        rdf_term_free(object);
                        object = blank;
                        blank  = NULL;
                }

                sequence_free(B);
                rdf_term_free(first);
                rdf_term_free(rest);

                A = object;

                err: /* Error handling */
                if (error) {
                        if (blank)
                                rdf_term_free(blank);

                        if (object)
                                rdf_term_free(object);

                        if (first)
                                rdf_term_free(first);

                        if (rest)
                                rdf_term_free(rest);

                        sequence_free(B);
                        YYERROR;
                }
        }
}
collection_ast(A) ::= collection_ast(B) object(C). {
        if (!B) {
                B = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
                if (!B)
                        YYERROR;
        }

        if (C) {
                struct rdf_statement* t = rdf_statement_from_nodes(NULL, NULL, C, NULL);
                if (!t)
                        YYERROR;

                if (sequence_push(B, t)) {
                        sequence_free(B);
                        B = NULL;
                        YYERROR;
                }
        }

        A = B;
}
collection_ast(A) ::= . { A = NULL;  }
/*}}}*/

/* [16] NumericalLiteral ::= INTEGER | DECIMAL | DOUBLE {{{ */
numericalLiteral(A) ::= INTEGER_LITERAL(B). {
        A = rdf_term_from_literal(B, NULL, string_create(rdfInteger));
        string_dealloc(B);
        if (!A)
                YYERROR;
}
numericalLiteral(A) ::= DECIMAL_LITERAL(B). {
        A = rdf_term_from_literal(B, NULL, string_create(rdfDecimal));
        string_dealloc(B);
        if (!A)
                YYERROR;
}
numericalLiteral(A) ::=  DOUBLE_LITERAL(B). {
        A = rdf_term_from_literal(B, NULL, string_create(rdfDouble));
        string_dealloc(B);
        if (!A)
                YYERROR;
}
/*}}}*/
/* [128s] RDFLiteral ::= String (LANGTAG | '^^' iri)? {{{ */
rdfLiteral(A)  ::= string(B) rdfLiteral_qst(C). {
        if (!C) {
                A = rdf_term_from_literal(B,NULL,NULL);
        } else {
                A = rdf_term_from_literal(B, C->langtag, C->datatype);
                string_dealloc(C->langtag);
                string_dealloc(C->datatype);
                free(C);
        }
        string_dealloc(B);

        if (!A)
                YYERROR;
} //FOR NOW IGNORE LANGTAG

%type rdfLiteral_qst { datatype_sys_t* }
%type rdfLiteral_opt { datatype_sys_t* }

rdfLiteral_qst(A) ::= rdfLiteral_opt(X). {A = X;}
rdfLiteral_qst(A) ::= . {A = NULL;}

rdfLiteral_opt(A) ::= LANGTAG(X). {
        if (X) {
                datatype_sys_t *s = calloc(1, sizeof(struct _datatype_sys_t));
                if (s) {
                        s->datatype = NULL;
                        s->langtag  = X;
                        A = s;
                } else {
                        string_dealloc(X);
                        A = NULL;
                }
        } else {
                A = NULL;
        }
}
rdfLiteral_opt(A) ::= HAT iri(X). {
        if (X) {
                datatype_sys_t *s = calloc(1, sizeof(struct _datatype_sys_t));
                if (s) {
                        struct rdf_term *t = X;
                        if (t->type == RDF_TERM_URI)
                                s->datatype = string_copy(t->value.uri);
                        else
                                s->datatype = NULL;
                        s->langtag = NULL;
                        A = s;
                        rdf_term_free(X);
                } else {
                        rdf_term_free(X);
                        A = NULL;
                }
        } else {
                A = NULL;
        }
}
/*}}}*/
/* [133s] BooleanLiteral ::= 'true' | 'false' {{{ */
booleanLiteral(A) ::= BOOLEAN_LITERAL(B). {
        A = rdf_term_from_literal(B, NULL, string_create(rdfBoolean));
        string_dealloc(B);

        if (!A)
                YYERROR;
}
/*}}}*/
/* [17] String ::= STRING_LITERAL_QUOTE | STRING_LITERAL_SINGLE_QUOTE | STRING_LITERAL_LONG_SINGLE_QUOTE | STRING_LITERAL_LONG_QUOTE {{{ */
string(A) ::= STRING_LITERAL(B). { A = B; }
/*}}}*/
/* [135s] iri ::= IRIREF | PrefixedName (=> PNAME_LN | PNAME_NS) {{{ */
iri(A) ::= IRIREF(B). {
        if (B) {
                A = rdf_term_from_uri(B);
                string_dealloc(B);

                if (!A)
                        YYERROR;

        } else {
                A = NULL;
        }
}
iri(A) ::= QNAME(B).  {
        if (B) {
                A = rdf_term_from_curie(B);
                string_dealloc(B);

                if (!A)
                        YYERROR;
        } else {
                A = NULL;
        }
}
/*}}}*/
/* [137s] BlankNode ::= BLANK_NODE_LABEL | ANON {{{ */
blankNode(A) ::= BLANK_LITERAL(B). {
        if (!B) {
                utf8 s = p->cb.generate_bnode();
                A = rdf_term_from_blank(s);
                string_dealloc(s);
        } else {
                A = rdf_term_from_blank(B);
                string_dealloc(B);
        }

        if (!A)
                YYERROR;
}
blankNode(A) ::= anon. {
        utf8 s = p->cb.generate_bnode();
        A = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!A)
                YYERROR;
}
/*}}}*/
/* ![162s] ANON ::= '[' WS* ']' {{{ */
anon ::= L_SQUARE R_SQUARE.
/*}}}*/
