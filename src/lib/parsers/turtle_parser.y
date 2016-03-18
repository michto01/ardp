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

        #include <ardp/lexer.h>
//        #include <ardp/lexer.turtle.h>    // Terminals specification
        #include <ardp/util.h>            // Various utilities
        #include <ardp/parser.h>          // Parser internals
}

%parse_failure { fprintf(stderr, "Parser failure\n"); /*parser->error.code = 1;*/ }
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
triples ::= subject(A) predicateObjectList. {
                fprintf(stdout, "[triple] with subject:%s\n", A);
          }
triples ::= blankNodePropertyList predicateObjectList.
triples ::= blankNodePropertyList.
/*}}}*/
/* [7] predicateObjectList ::= verb objectList (';' (verb objectList)?)* {{{ */
predicateObjectList     ::= verb objectList predicateObjectList_ast.
// (...)*
predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst.
predicateObjectList_ast ::= .
// ';' (verb objectList)?
predicateObjectList_qst ::= predicateObjectList_qst verb objectList.
predicateObjectList_qst ::= SEMICOLON.
/*}}}*/
/* [8] objectList ::= object (',' object)* {{{ */
objectList     ::= object objectList_ast.
objectList_ast ::= objectList_ast COMMA object.
objectList_ast ::= .
/*}}}*/
/* [9] verb ::= predicate | 'a' {{{ */
verb(A) ::= predicate(B). {
            A = B;
            fprintf(stdout, "[verb:predicate] == %s\n", B);
       }
verb(A) ::= A. {
            A = "http://www.w3c.org/RDF/1999/#type";
            fprintf(stdout, "[verb:a] == ...#type\n");
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
collection     ::= L_CURLY collection_ast R_CURLY.
collection_ast ::= collection_ast object.
collection_ast ::= .
/*}}}*/

/* [16] NumericalLiteral ::= INTEGER | DECIMAL | DOUBLE {{{ */
numericalLiteral(A) ::= INTEGER_LITERAL(B). { A = B; }
numericalLiteral(A) ::= DECIMAL_LITERAL(B). { A = B; }
numericalLiteral(A) ::=  DOUBLE_LITERAL(B). { A = B; }
/*}}}*/
/* [128s] RDFLiteral ::= String (LANGTAG | '^^' iri)? {{{ */
rdfLiteral(A)  ::= string(B) rdfLiteral_qst. {A = B; } //FOR NOW IGNORE LANGTAG
rdfLiteral_qst ::= rdfLiteral_opt.
rdfLiteral_qst ::= .
rdfLiteral_opt ::= LANGTAG.
rdfLiteral_opt ::= HAT iri.
/*}}}*/
/* [133s] BooleanLiteral ::= 'true' | 'false' {{{ */
booleanLiteral(A) ::= BOOLEAN_LITERAL(B). { A = B; }
/*}}}*/
/* [17] String ::= STRING_LITERAL_QUOTE | STRING_LITERAL_SINGLE_QUOTE | STRING_LITERAL_LONG_SINGLE_QUOTE | STRING_LITERAL_LONG_QUOTE {{{ */
string(A) ::= STRING_LITERAL(B). {A = B;}
/*}}}*/
/* [135s] iri ::= IRIREF | PrefixedName (=> PNAME_LN | PNAME_NS) {{{ */
iri(A) ::= IRIREF(B). { A = B; }
iri(A) ::= QNAME(B).  { A = B; }
/*}}}*/
/* [137s] BlankNode ::= BLANK_NODE_LABEL | ANON {{{ */
blankNode(A) ::= BLANK_LITERAL(B). { A = B; }
blankNode(A) ::= anon(B). { A = B; }
/*}}}*/
/* ![162s] ANON ::= '[' WS* ']' {{{ */
anon(A) ::= L_SQUARE R_SQUARE. { A = "ANON"; }
/*}}}*/

/*
ardp_status_t parser->push_prefix(utf8 string, utf8 iri);
utf8 parser->expand_qname(utf8 qname);
utf8 parser->base()
utf8 parser->set_base(utf8 uri);
*/
