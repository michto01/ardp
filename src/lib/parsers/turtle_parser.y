/*! @file turtle_parser.y
 *
 * Template to generate the `Lemon` parser for the Turtle grammar.
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2016
 */

//%name turtle_parser

//%extra_argument { ardp_parser* parser }


%include {
        #include <assert.h>
        #include <stdio.h>
        #include <stddef.h>
        #include <stdlib.h>
        #include <stdio.h>
        #include <errno.h>

        #include <ardp/rdf.h>
        #include <ardp/sequence.h>

        #include <ardp/lexer.h>
//        #include <ardp/lexer.turtle.h>    // Terminals specification
        #include <ardp/util.h>            // Various utilities
        #include <ardp/parser.h>          // Parser internals
}

%parse_failure { fprintf(stderr, "Parser failure\n"); /*parser->error.code = 1;*/ }
%parse_accept  { fprintf(stderr, "Final parser statistics.\n"); }
%syntax_error  { fprintf(stderr, "Syntax error\n"); /*parser->error.code = 2;*/ }

//%token_type { struct { int type, const char* value } } //Token type

//%type QNAME            { const char* }
//%type BLANK_LITERAL    { const char* }
//%type STRING_LITERAL   { const char* }
//%type DOUBLE_LITERAL   { const char* }
//%type INTEGER_LITERAL  { const char* }
//%type DECIMAL_LITERAL  { const char* }
//%type IRIREF           { const char* }
//%type LANGTAG          { const char* }

//%type subject          { const char* }
//%destruction subject { free_subject($$); }
%start_symbol turtleDoc

%include {

#define YYERROR do { fprintf(stderr, "\n\nERROR\n\n");  }while(0)

static uint8_t* generate_bnodeid(void* parser, uint8_t* bnode) {
        return "hello";
}
extern utf8     string_create(const uint8_t* s);
extern utf8     string_append(utf8 a, utf8 b);
extern utf8     string_append_s(utf8 a, const uint8_t* b);


static struct rdf_statement* generate_turtle_statement(void* x, void* s)
{
        return rdf_statement_from_nodes(NULL,NULL,NULL,NULL);
}

#define XSD "xsd:"
uint8_t *const xsd_uri    = (uint8_t *) "http://www.w3.org/2001/XMLSchema";

uint8_t *const rdfNil     = (uint8_t *) "http://www.nil.org/";
uint8_t *const rdfFirst   = (uint8_t *) "http://www.w3.org/2001/XMLSchame#First";
uint8_t *const rdfRest    = (uint8_t *) "http://www.w3.org/2001/XMLSchema#Rest";

uint8_t *const rdfBoolean = (uint8_t*) XSD"Boolean";
uint8_t *const rdfInt     = (uint8_t*) XSD"int";
uint8_t *const rdfFloat   = (uint8_t*) XSD"float";
uint8_t *const rdfDouble  = (uint8_t*) XSD"double";
uint8_t *const rdfLong    = (uint8_t*) XSD"long";
uint8_t *const rdfShort   = (uint8_t*) XSD"short";
uint8_t *const rdfInteger = (uint8_t*) XSD"integer";
uint8_t *const rdfDecimal = (uint8_t*) XSD"decimal";
uint8_t *const rdfString  = (uint8_t*) XSD"string";

//{{{
/*        struct _ardp_turtle_parser {

                uint64_t    n_bnode;

                uint8_t*    baseURI;
                map_t       namespaces;
                struct pair bnodeLabels;
                utf8        curSubject;
                utf8        curPredicate;

                sequence?   seq; // Collections Objects

                int         error;
                size_t      n_triples;
                size_t      n_directives;
                size_t      n_errors;

        } ardp_turtle_parser;



        static bool changeBase( const char* base )
        {
                assert( shared_parser_internals );

                struct ardp_turtle_parser* this = shared_parser_internals;

                if(this->baseURI);
                        free(this->baseURI);

                this->baseURI = base;
                return true; // Error checking should be employed
        }

        static bool addNamespace( const char* qname, const char* iri)
        {
                assert( shared_parser_internals );
                struct ardp_turtle_parser* this = shared_parser_internals;
                if( is_prefix_qname(qname) )
                        if ( iri )
                                map_push( qname, iri );
        }
*/
//}}}
}

/* [1] turleDoc ::= statement*  {{{ */
turtleDoc ::= statements.
// Root needs not to be recursive, so we use indirection -> statements
statements ::= statements statement.
statements ::= .
/*}}}*/
/* [2] statement ::= directive | triples '.' {{{ */
statement ::= directive.
statement ::= triples DOT.
/*}}}*/
/* [3] directive ::= prefixID | base | sparqlPrefix | sparqlBase {{{ */
directive ::= prefixID.
directive ::= base.
directive ::= sparqlPrefix.
directive ::= sparqlBase.
/*}}}*/
/* [4] prefixID ::= '@prefix' PNAME_NS IRIREF '.' {{{ */
prefixID ::= PREFIX COLON IRIREF(B) DOT. {
                fprintf(stdout, "prefix for universal ':' for %s\n", B);
           }
prefixID ::= PREFIX QNAME(A) IRIREF(B) DOT. {
                fprintf(stdout, "prefix for: %s, uri: %s\n", A, B);
           }
/*}}}*/
/* [5] base ::= '@base' IRIREF DOT. {{{ */
base ::= BASE IRIREF(A) DOT. {
                fprintf(stdout, "base: %s", A);
       }
/*}}}*/
/* [5s] sparqlBase ::= "BASE" IRIREF {{{ */
sparqlBase ::= SPARQL_BASE IRIREF(A). {
                fprintf(stdout, "BASE_SPARQL ::= %s\n", A);
             }
/*}}}*/
/* [6s] sparqlPrefix ::= "PREFIX" PNAME_NS IRIREF {{{ */
sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF(B). {
                        fprintf(stdout, "SPARQL prefix s: ':', uri: %s\n", B);
                        // parser->addPrefix(":",B);
               }
sparqlPrefix ::= SPARQL_PREFIX QNAME(A) IRIREF(B). {
                        fprintf(stdout, "SPARQL prefix s: %s, uri: %s\n", A, B);
                        // parser->addPrefix(A,B);
               }
/*}}}*/
/* [6] triples ::= subject predicateObjectList | blankNodePropertyList predicateObjectList?  {{{ */
triples ::= subject(A) predicateObjectList(B). {
        size_t i;

        if (A && B) {
                /* have subject and property list with items */
                for (i = 0; i < sequence_size(B); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(B, i);
                        t->subject = rdf_term_copy(A);
                }
                for (i = 0; i < sequence_size(B); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(B, i);

                        generate_turtle_statement(NULL/*parser*/, t);
                }
        }

        if (B)
                sequence_free(B);

        if (A)
                rdf_term_free(A);
}
triples ::= blankNodePropertyList(A) predicateObjectList(B). {
        size_t i;

        if (A && B) {
                /* have subject and property list with items */
                for (i = 0; i < sequence_size(B); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(B, i);
                        t->subject = rdf_term_copy(A);
                }
                for (i = 0; i < sequence_size(B); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(B, i);
                        generate_turtle_statement(NULL/*parser*/, t);
                }
        }

        if (B)
                sequence_free(B);

        if (A)
                rdf_term_free(A);
}
triples ::= blankNodePropertyList. // ???
/*}}}*/
/* [7] predicateObjectList ::= verb objectList (';' (verb objectList)?)* {{{ */
predicateObjectList(A) ::= verb(B) objectList(C) predicateObjectList_ast(D). {
        if (B && C) {
                for (size_t i = 0; i < sequence_size(C); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(C, i);

                        fprintf(stderr, "Bazinga: %s\n", (utf8) ((struct rdf_term*)B)->value.uri);
                        t->predicate = rdf_term_copy((struct rdf_term*)B);
                        fprintf(stderr, "After Bazinga :D");
                }
                /* @TODO: double copy -> should be optimalized */
                if (D) {
                        assert(0);
                        while ( sequence_size(D) ) {
                                struct rdf_statement* t = (struct rdf_statement*)
                                                                sequence_unshift(D);
                                if (sequence_push(C, t)) {
                                        sequence_free(C);
                                        sequence_free(D);
                                        rdf_term_free(B);
                                        YYERROR;
                                }
                        }

                        sequence_free(C);
                }

                if (B)
                        rdf_term_free(B);

                A = C;
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
                A = sequence_create();
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
                A = sequence_create();
                if (!A)
                        YYERROR;
        }
        sequence_push(A, triple);
}
/* Parts of Turtle specification do not specify the custom error situations not
 * handled by the grammar. this is one of such cases.
 */
objectList_ast(A) ::= objectList_ast(B) COMMA . { /* Custom handling of ES */ A = B; }
objectList_ast ::= .
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
blankNodePropertyList(A) ::= L_SQUARE predicateObjectList(B) R_SQUARE. { A = B; }
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

                        uint8_t* bid = generate_bnodeid(NULL, NULL);
                        utf8 s = string_create(bid);
                        if (!bid || !s) {
                                string_dealloc(s);
                                error = 1;
                                goto err;
                        }

                        blank = rdf_term_from_blank(bid);
                        string_dealloc(s);
                        free(bid);

                        if (!blank) {
                                error = 1;
                                goto err;
                        }

                        { /* rdf:First */
                                t->subject   = blank;
                                t->predicate = first;
                                /* t->object = already set */

                                generate_turtle_statement(/*parser*/NULL, t);
                        }
                        { /* rdf:Rest */
                                struct rdf_term* temp = t->object;

                                t->subject   = blank;
                                t->predicate = rest;
                                t->object    = object;
                                generate_turtle_statement(/*parser*/NULL, t);

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
                B = sequence_create();
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
        A = rdf_term_from_literal(string_create(B), NULL, string_create(rdfInteger));
        if (!A)
                YYERROR;
}
numericalLiteral(A) ::= DECIMAL_LITERAL(B). {
        A = rdf_term_from_literal(string_create(B), NULL, string_create(rdfDecimal));
        if (!A)
                YYERROR;
}
numericalLiteral(A) ::=  DOUBLE_LITERAL(B). {
        A = rdf_term_from_literal(string_create(B), NULL, string_create(rdfDouble));
        if (!A)
                YYERROR;
}
/*}}}*/
/* [128s] RDFLiteral ::= String (LANGTAG | '^^' iri)? {{{ */
rdfLiteral(A)  ::= string(B) rdfLiteral_qst(C). {
        if (!C)
                A = rdf_term_from_literal(B,NULL,NULL);
        else
                A = rdf_term_from_literal(B,/*((C.type == 1))? C.value.lang:*/NULL,
                                               /* ((C.type == 2))? C.value.iri :*/ NULL );

        if (!A)
                YYERROR;
} //FOR NOW IGNORE LANGTAG

rdfLiteral_qst(A) ::= rdfLiteral_opt(X). {A = X;}
rdfLiteral_qst(A) ::= . {A = NULL;}

//%type rdfLiteral_opt { struct { int type; union { struct rdf_term* iri, utf8 lang } value } }
rdfLiteral_opt(A) ::= LANGTAG(X). { A/*.type = 1; A.value.lang*/ = X; }
rdfLiteral_opt(A) ::= HAT iri(X). { A/*.type = 2; A.value.iri*/  = X; }
/*}}}*/
/* [133s] BooleanLiteral ::= 'true' | 'false' {{{ */
booleanLiteral(A) ::= BOOLEAN_LITERAL(B). {
        A = rdf_term_from_literal(string_create(B), NULL, string_create(rdfBoolean));
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
                A =  rdf_term_from_uri(B);
                string_dealloc(B);

                if (!A)
                        YYERROR;

        } else {
                A = NULL;
        }
}
iri(A) ::= QNAME(B).  {
        if (B) {
                //TODO: qname qualification
                A = rdf_term_from_uri(B);
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
        uint8_t* z = generate_bnodeid(NULL, B);
        utf8 id = string_create(B);

        if (!id)
                YYERROR;

        A = rdf_term_from_blank(id);
        string_dealloc(id);

        if (!A)
                YYERROR;
}
blankNode(A) ::= anon. {
        uint8_t* id = generate_bnodeid(NULL, NULL);
        if (!id)
                YYERROR;

        A = rdf_term_from_blank(id);

        if (!A)
                YYERROR;
}
/*}}}*/
/* ![162s] ANON ::= '[' WS* ']' {{{ */
anon ::= L_SQUARE R_SQUARE.
/*}}}*/

/*
ardp_status_t parser->push_prefix(utf8 string, utf8 iri);
utf8 parser->expand_qname(utf8 qname);
utf8 parser->base()
utf8 parser->set_base(utf8 uri);
*/
