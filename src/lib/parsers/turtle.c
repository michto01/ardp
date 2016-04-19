/*! @file parsers/turtle.c
 *
 * Paser lexer/parser shared internals
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2016
 */

#include <ardp/util.h>
#include <ardp/parser.h>
#include <ardp/hashmap.h>
#include <ardp/hash.h>

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

ardp_status_t ardp_resolve_qname( map_t* map, const char* qname  );


#ifdef __cplusplus
}
#endif /* C++ */

//---------------------------------------------------------------------------------
// LEMON parser implementation
//---------------------------------------------------------------------------------

/*
[1]     turtleDoc       ::=     statement*
[2]     statement       ::=     directive | triples '.'
[3]     directive       ::=     prefixID | base | sparqlPrefix | sparqlBase
[4]     prefixID        ::=     '@prefix' PNAME_NS IRIREF '.'
[5]     base            ::=     '@base' IRIREF '.'
[5s]    sparqlBase      ::=     "BASE" IRIREF
[6s]    sparqlPrefix    ::=     "PREFIX" PNAME_NS IRIREF
[6]     triples         ::=     subject predicateObjectList | blankNodePropertyList predicateObjectList?
[7]     predicateObjectList     ::=     verb objectList (';' (verb objectList)?)*
[8]     objectList      ::=     object (',' object)*
[9]     verb            ::=     predicate | 'a'
[10]    subject         ::=     iri | BlankNode | collection
[11]    predicate       ::=     iri
[12]    object          ::=     iri | BlankNode | collection | blankNodePropertyList | literal
*/

// [1] turtleDoc
turtleDoc ::= turtleDoc statement.
turtleDoc ::= .

// [2] statement
statement ::= directive.
statement ::= triple DOT.

// [3] directive
directive ::= prefixID.
directive ::= base.
directive ::= sparqlPrefix.
directive ::= sparqlBase.

// [4],[5]
prefixID ::= PREFIX QNAME IRIREF DOT.
base     ::= BASE IRIREF DOT.

// [6s],[5s]
sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF.
sparqlBase   ::= SPARQL_BASE IRIREF.

// [6]
triples       ::= subject predicateObjectList.
triples       ::= blankNodePropertyList predicateObjectList.
triples       ::= blankNodePropertyList.

/*
triples       ::= Triples_blank.
Triples_blank ::= Triples_blank predicateObjectList.
Triples_blank ::= blankNodePropertyList.
*/

/* [7] <predicateObjectList> {{{ */
// [7]  predicateObjectList ::= verb objectList (';' (verb objectList)?)*
predicateObjectList ::= predicateObjectList POL_arg.
predicateObjectList ::= verb objectList.

// [7b] (...)*
POL_arg ::= POL_arg POL_var.
POL_arg ::= .

// [7a] ';' (verb objectList)?
POL_var ::= SEMICOLON  verb objectList.
POL_var ::= SEMICOLON.
/* [7] </predicateObjectList> }}}*/


// [8] object (',' object)*
objectList ::= objectList objectList_ast.
objectList ::= object.
// [8a] (',' object)*
objectList_ast ::= objectList_ast COLON object.
objectList_ast ::= .

// [9] verb
verb ::= A.
verb ::= predicate.

// [10] subject
subject ::= iri.
subject ::= BlankNode.
subject ::= collection.

// [11] predicate
predicate ::= iri

// [12] object
object ::= iri.
object ::= BlankNode.
object ::= collection.
object ::= blankPropertyList.
object ::= literal.


// [14] blankNodePropertyList	::= '[' predicateObjectList ']'
blankNodeProperyList ::= L_SQUARE predicateObjectList R_SQUARE.


/* @rule: [15]
collection  ::= '(' object* ')' */
collection  ::= L_CURLY collection_object R_CURLY.

collection_object ::= collection_object object.
collection_object ::= .

/* @rule: [13]
literal     ::= RDFLiteral | NumericLiteral | BooleanLiteral */
literal     ::= RDFLiteral.
literal     ::= NumericLiteral.
literal     ::= BOOLEAN_LITERAL.

/* @rule: [128s]
RDFLiteral  ::= String (LANGTAG | '^^' iri)? */
RDFLiteral  ::= String RDFLiteral_q.

RDFLiteral_q ::= HAT iri.
RDFLiteral_q ::= LANGTAG.
RDFLiteral_q ::= .

/* @rule[16]
NumericLiteral ::= INTEGER | DECIMAL | DOUBLE */
NumericLiteral ::= INTEGER_LITERAL.
NumericLiteral ::= DECIMAL_LITERAL.
NumericLiteral ::= DOUBLE_LITERAL.

String    ::= STRING_LITERAL .

iri       ::= QNAME .
iri       ::= IRIREF .

BlankNode ::= BLANK_LITERAL .
BlankNode ::= ANON .

ANON      ::= L_SQUARE R_SQUARE.
