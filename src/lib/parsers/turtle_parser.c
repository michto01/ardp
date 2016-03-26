/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 14 "./turtle_parser.y"

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
//        #include <ardp/lexer.turtle.h>    // Terminals specification
        #include <ardp/util.h>            // Various utilities
        #include <ardp/parser.h>          // Parser internals
#line 38 "./turtle_parser.y"


#define YYERROR do { fprintf(stderr, "\n\nERROR\n\n");  }while(0)

static uint8_t* generate_bnodeid(void* parser, uint8_t* bnode) {
        return "hello";
}
//extern utf8     string_create(const uint8_t* s);
//extern utf8     string_append(utf8 a, utf8 b);
//extern utf8     string_append_s(utf8 a, const uint8_t* b);


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
#line 601 "./turtle_parser.y"


static void generate_statement(void* parser, struct rdf_statement* t) {

        return;
/*
        void *turtle_parser = (void*)parser->context;
        struct rdf_statement *statement = &parser->statement;

        if(!t->subject || !t->predicate || !t->object)
                return;

        if(!parser->cb.statement)
                return;

/* FIXME: emmit graph
  if(!parser->emitted_default_graph && !turtle_parser->graph_name) {
    /* for non-TRIG - start default graph at first triple * /
    raptor_parser_start_graph(parser, NULL, 0);
    parser->emitted_default_graph++;
  }
*/

        /* Two choices for subject for Turtle */
/*
        switch (t->subject->type) {
        case RDF_TERM_URI:
                statement->subject = rdf_term_from_uri(t->subject->value.uri);
                break;
        case RDF_TERM_BLANK:
                statement->subject = rdf_term_from_blank(t->subject->value.blank);
                break;

        default:
                fprintf(stderr, "subject type is not resource!\n");
                return;
        }

        /* Predicates are URIs but check for bad ordinals */
        /* @FIXME: repair ordinal checking, missing check function */
        if(!strncmp((const char*)(t->predicate->value.uri), "http://www.w3.org/1999/02/22-rdf-syntax-ns#_", 44)) {
        //        unsigned char* predicate_uri_string = t->predicate->value.uri;
        //        int predicate_ordinal = rdf_aux_check_ordinal(predicate_uri_string+44);
        //        if(predicate_ordinal <= 0)
        //                parser_error(parser, "Illegal ordinal value %d in property '%s'.", predicate_ordinal, predicate_uri_string);
        }
/*
        statement->predicate = rdf_term_from_uri(t->predicate->value.uri);


        /* Three choices for object for Turtle */
/*        switch(t->object->type) {
        case RDF_TERM_URI:
                statement->object = rdf_term_from_uri(t->object->value.uri);
                break;
        case RDF_TERM_BLANK:
                statement->object = rdf_term_from_blank(t->object->value.blank);
                break;

        case RDF_TERM_LITERAL:
                statement->object = rdf_term_from_literal(
                                                t->object->value.literal.string,
                                                t->object->value.literal.datatype,
                                                t->object->value.literal.language);
                break;

        case RDF_TERM_UNKNOWN:
        default:
                // ERROR: invalid term.
                break;
        }

        /* Generate the statement */
  /*      (*parser->cb.statement)(parser->user_data, statement);

        { // Subject
                rdf_term_free(statement->subject);
                statement->subject = NULL;
        }
        { // Predicate
                rdf_term_free(statement->predicate);
                statement->predicate = NULL;
        }
        { // Object
                rdf_term_free(statement->object);
                statement->object = NULL;
        }
        { // Graph
                if (statement->graph) {
                        rdf_term_free(statement->graph);
                        statement->graph = NULL;
                }
        }*/
}

#line 197 "./turtle_parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 57
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE void*
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  struct parser* p ;
#define ParseARG_PDECL , struct parser* p 
#define ParseARG_FETCH  struct parser* p  = yypParser->p 
#define ParseARG_STORE yypParser->p  = p 
#define YYNSTATE 79
#define YYNRULE 60
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    51,   52,   19,   54,   55,   56,   57,    8,    2,    7,
 /*    10 */    18,   27,   20,   21,   76,   78,    5,   48,   49,   50,
 /*    20 */    58,   71,   30,   66,   41,   11,   15,   26,   46,   68,
 /*    30 */    69,   70,   72,   73,   74,   75,   71,   10,   13,   53,
 /*    40 */    46,   15,   39,   40,   68,   69,   70,   72,   73,   74,
 /*    50 */    75,   25,   10,   63,   71,   46,   62,   31,   32,   67,
 /*    60 */   140,    1,   68,   69,   70,   72,   73,   74,   75,   42,
 /*    70 */    10,   31,   32,   46,    9,    4,   65,   60,   12,    6,
 /*    80 */    59,   14,   34,   36,   37,   38,   71,   24,   43,   44,
 /*    90 */    45,   35,   29,   61,   68,   69,   70,   72,   73,   74,
 /*   100 */    75,  141,   10,   31,   32,   46,  141,   31,   32,  141,
 /*   110 */   141,    6,  141,   14,   77,   36,   37,   38,  141,  141,
 /*   120 */    43,   44,   45,   79,  141,   17,  141,   31,   32,   22,
 /*   130 */    28,   16,   31,   32,  141,    6,   23,   14,    3,   77,
 /*   140 */    64,   33,    3,  141,   76,   78,   45,  141,   76,   78,
 /*   150 */   141,   47,  141,    3,  141,  141,  141,  141,  141,   76,
 /*   160 */    78,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    27,   28,   29,   30,   31,   32,   33,   34,   37,   36,
 /*    10 */     3,    3,    5,    5,   43,   44,   10,   44,   45,   46,
 /*    20 */     1,   36,   13,   38,   19,   20,   41,    4,   55,   44,
 /*    30 */    45,   46,   47,   48,   49,   50,   36,   52,   38,    1,
 /*    40 */    55,   41,   53,   54,   44,   45,   46,   47,   48,   49,
 /*    50 */    50,    4,   52,    4,   36,   55,    4,    4,    5,   41,
 /*    60 */    25,   26,   44,   45,   46,   47,   48,   49,   50,   44,
 /*    70 */    52,    4,    5,   55,    9,   51,   40,    1,   39,   12,
 /*    80 */     1,   14,   15,   16,   17,   18,   36,   42,   21,   22,
 /*    90 */    23,   41,    4,    4,   44,   45,   46,   47,   48,   49,
 /*   100 */    50,   56,   52,    4,    5,   55,   56,    4,    5,   56,
 /*   110 */    56,   12,   56,   14,   11,   16,   17,   18,   56,   56,
 /*   120 */    21,   22,   23,    0,   56,    2,   56,    4,    5,    6,
 /*   130 */     7,    8,    4,    5,   56,   12,   35,   14,   37,   11,
 /*   140 */    35,   13,   37,   56,   43,   44,   23,   56,   43,   44,
 /*   150 */    56,   35,   56,   37,   56,   56,   56,   56,   56,   43,
 /*   160 */    44,
};
#define YY_SHIFT_USE_DFLT (-1)
#define YY_SHIFT_MAX 29
static const short yy_shift_ofst[] = {
 /*     0 */    -1,  123,   99,   99,   67,   99,  128,  103,  103,  103,
 /*    10 */     5,   53,   65,   -1,   -1,   -1,    7,    8,   52,   38,
 /*    20 */    49,   47,   23,    9,    6,   79,   76,   88,   89,   19,
};
#define YY_REDUCE_USE_DFLT (-30)
#define YY_REDUCE_MAX 15
static const signed char yy_reduce_ofst[] = {
 /*     0 */    35,  -27,  -15,    0,   50,   18,  101,  116,  105,  -29,
 /*    10 */   -11,   25,   36,   39,   24,   45,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    81,  139,  139,  139,  139,  104,  139,   96,  139,  101,
 /*    10 */   129,  139,   97,   99,  123,  105,  139,  139,  139,  139,
 /*    20 */   139,  139,  139,  139,  102,  139,  139,  139,  139,  139,
 /*    30 */   120,  134,  135,  138,  121,  122,  124,  125,  126,  127,
 /*    40 */   128,  130,  131,  132,  133,  136,  137,   95,  108,  109,
 /*    50 */   110,   80,   82,   83,   84,   85,   86,   87,   88,   89,
 /*    60 */    90,   91,   92,   93,   94,   98,  100,  103,  112,  113,
 /*    70 */   114,  115,  116,  117,  118,  119,  106,  107,  111,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "DOT",           "PREFIX",        "COLON",       
  "IRIREF",        "QNAME",         "BASE",          "SPARQL_BASE", 
  "SPARQL_PREFIX",  "SEMICOLON",     "COMMA",         "A",           
  "L_SQUARE",      "R_SQUARE",      "L_CURLY",       "R_CURLY",     
  "INTEGER_LITERAL",  "DECIMAL_LITERAL",  "DOUBLE_LITERAL",  "LANGTAG",     
  "HAT",           "BOOLEAN_LITERAL",  "STRING_LITERAL",  "BLANK_LITERAL",
  "error",         "turtleDoc",     "statements",    "statement",   
  "directive",     "triples",       "prefixID",      "base",        
  "sparqlPrefix",  "sparqlBase",    "subject",       "predicateObjectList",
  "blankNodePropertyList",  "verb",          "objectList",    "predicateObjectList_ast",
  "predicateObjectList_qst",  "object",        "objectList_ast",  "predicate",   
  "iri",           "blankNode",     "collection",    "literal",     
  "rdfLiteral",    "numericalLiteral",  "booleanLiteral",  "collection_ast",
  "string",        "rdfLiteral_qst",  "rdfLiteral_opt",  "anon",        
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "turtleDoc ::= statements",
 /*   1 */ "statements ::= statements statement",
 /*   2 */ "statements ::=",
 /*   3 */ "statement ::= directive",
 /*   4 */ "statement ::= triples DOT",
 /*   5 */ "directive ::= prefixID",
 /*   6 */ "directive ::= base",
 /*   7 */ "directive ::= sparqlPrefix",
 /*   8 */ "directive ::= sparqlBase",
 /*   9 */ "prefixID ::= PREFIX COLON IRIREF DOT",
 /*  10 */ "prefixID ::= PREFIX QNAME IRIREF DOT",
 /*  11 */ "base ::= BASE IRIREF DOT",
 /*  12 */ "sparqlBase ::= SPARQL_BASE IRIREF",
 /*  13 */ "sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF",
 /*  14 */ "sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF",
 /*  15 */ "triples ::= subject predicateObjectList",
 /*  16 */ "triples ::= blankNodePropertyList predicateObjectList",
 /*  17 */ "triples ::= blankNodePropertyList",
 /*  18 */ "predicateObjectList ::= verb objectList predicateObjectList_ast",
 /*  19 */ "predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst",
 /*  20 */ "predicateObjectList_ast ::=",
 /*  21 */ "predicateObjectList_qst ::= SEMICOLON verb objectList",
 /*  22 */ "predicateObjectList_qst ::= SEMICOLON",
 /*  23 */ "objectList ::= object objectList_ast",
 /*  24 */ "objectList_ast ::= objectList_ast COMMA object",
 /*  25 */ "objectList_ast ::= objectList_ast COMMA",
 /*  26 */ "objectList_ast ::=",
 /*  27 */ "verb ::= predicate",
 /*  28 */ "verb ::= A",
 /*  29 */ "subject ::= iri",
 /*  30 */ "subject ::= blankNode",
 /*  31 */ "subject ::= collection",
 /*  32 */ "predicate ::= iri",
 /*  33 */ "object ::= iri",
 /*  34 */ "object ::= blankNode",
 /*  35 */ "object ::= collection",
 /*  36 */ "object ::= blankNodePropertyList",
 /*  37 */ "object ::= literal",
 /*  38 */ "literal ::= rdfLiteral",
 /*  39 */ "literal ::= numericalLiteral",
 /*  40 */ "literal ::= booleanLiteral",
 /*  41 */ "blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE",
 /*  42 */ "collection ::= L_CURLY collection_ast R_CURLY",
 /*  43 */ "collection_ast ::= collection_ast object",
 /*  44 */ "collection_ast ::=",
 /*  45 */ "numericalLiteral ::= INTEGER_LITERAL",
 /*  46 */ "numericalLiteral ::= DECIMAL_LITERAL",
 /*  47 */ "numericalLiteral ::= DOUBLE_LITERAL",
 /*  48 */ "rdfLiteral ::= string rdfLiteral_qst",
 /*  49 */ "rdfLiteral_qst ::= rdfLiteral_opt",
 /*  50 */ "rdfLiteral_qst ::=",
 /*  51 */ "rdfLiteral_opt ::= LANGTAG",
 /*  52 */ "rdfLiteral_opt ::= HAT iri",
 /*  53 */ "booleanLiteral ::= BOOLEAN_LITERAL",
 /*  54 */ "string ::= STRING_LITERAL",
 /*  55 */ "iri ::= IRIREF",
 /*  56 */ "iri ::= QNAME",
 /*  57 */ "blankNode ::= BLANK_LITERAL",
 /*  58 */ "blankNode ::= anon",
 /*  59 */ "anon ::= L_SQUARE R_SQUARE",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 25, 1 },
  { 26, 2 },
  { 26, 0 },
  { 27, 1 },
  { 27, 2 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 30, 4 },
  { 30, 4 },
  { 31, 3 },
  { 33, 2 },
  { 32, 3 },
  { 32, 3 },
  { 29, 2 },
  { 29, 2 },
  { 29, 1 },
  { 35, 3 },
  { 39, 2 },
  { 39, 0 },
  { 40, 3 },
  { 40, 1 },
  { 38, 2 },
  { 42, 3 },
  { 42, 2 },
  { 42, 0 },
  { 37, 1 },
  { 37, 1 },
  { 34, 1 },
  { 34, 1 },
  { 34, 1 },
  { 43, 1 },
  { 41, 1 },
  { 41, 1 },
  { 41, 1 },
  { 41, 1 },
  { 41, 1 },
  { 47, 1 },
  { 47, 1 },
  { 47, 1 },
  { 36, 3 },
  { 46, 3 },
  { 51, 2 },
  { 51, 0 },
  { 49, 1 },
  { 49, 1 },
  { 49, 1 },
  { 48, 2 },
  { 53, 1 },
  { 53, 0 },
  { 54, 1 },
  { 54, 2 },
  { 50, 1 },
  { 52, 1 },
  { 44, 1 },
  { 44, 1 },
  { 45, 1 },
  { 45, 1 },
  { 55, 2 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 9: /* prefixID ::= PREFIX COLON IRIREF DOT */
#line 132 "./turtle_parser.y"
{
                fprintf(stdout, "prefix for universal ':' for %s\n", yymsp[-1].minor.yy0);
           }
#line 989 "./turtle_parser.c"
        break;
      case 10: /* prefixID ::= PREFIX QNAME IRIREF DOT */
#line 135 "./turtle_parser.y"
{
                fprintf(stdout, "prefix for: %s, uri: %s\n", yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
           }
#line 996 "./turtle_parser.c"
        break;
      case 11: /* base ::= BASE IRIREF DOT */
#line 140 "./turtle_parser.y"
{
                fprintf(stdout, "base: %s", yymsp[-1].minor.yy0);
       }
#line 1003 "./turtle_parser.c"
        break;
      case 12: /* sparqlBase ::= SPARQL_BASE IRIREF */
#line 145 "./turtle_parser.y"
{
                fprintf(stdout, "BASE_SPARQL ::= %s\n", yymsp[0].minor.yy0);
             }
#line 1010 "./turtle_parser.c"
        break;
      case 13: /* sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF */
#line 150 "./turtle_parser.y"
{
                        fprintf(stdout, "SPARQL prefix s: ':', uri: %s\n", yymsp[0].minor.yy0);
                        // parser->addPrefix(":",yymsp[0].minor.yy0);
               }
#line 1018 "./turtle_parser.c"
        break;
      case 14: /* sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF */
#line 154 "./turtle_parser.y"
{
                        fprintf(stdout, "SPARQL prefix s: %s, uri: %s\n", yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
                        // parser->addPrefix(yymsp[-1].minor.yy0,yymsp[0].minor.yy0);
               }
#line 1026 "./turtle_parser.c"
        break;
      case 15: /* triples ::= subject predicateObjectList */
#line 160 "./turtle_parser.y"
{
        size_t i;

        if (yymsp[-1].minor.yy0 && yymsp[0].minor.yy0) {
                /* have subject and property list with items */
                while ( sequence_size(yymsp[0].minor.yy0) ) {
                        fprintf(stderr, "HEEEEEEELOOOOO\n\n");
                        struct rdf_statement* t = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy0);
                        t->subject = rdf_term_copy(yymsp[-1].minor.yy0);
                        generate_turtle_statement(/*parser*/NULL, t);
                        rdf_statement_free(t);
                }

                //for (i = 0; i < sequence_size(yymsp[0].minor.yy0); i++) {
                //        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(yymsp[0].minor.yy0, i);
                //        t->subject = rdf_term_copy(yymsp[-1].minor.yy0);
                //}
                //for (i = 0; i < sequence_size(yymsp[0].minor.yy0); i++) {
                //        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(yymsp[0].minor.yy0, i);
                //
                //        generate_turtle_statement(NULL/*parser*/, t);
                //}
        }

        if (yymsp[0].minor.yy0)
                sequence_free(yymsp[0].minor.yy0);

        if (yymsp[-1].minor.yy0)
                rdf_term_free(yymsp[-1].minor.yy0);
}
#line 1060 "./turtle_parser.c"
        break;
      case 16: /* triples ::= blankNodePropertyList predicateObjectList */
#line 190 "./turtle_parser.y"
{
        size_t i;

        if (yymsp[-1].minor.yy0 && yymsp[0].minor.yy0) {
                /* have subject and property list with items */
                for (i = 0; i < sequence_size(yymsp[0].minor.yy0); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(yymsp[0].minor.yy0, i);
                        t->subject = rdf_term_copy(yymsp[-1].minor.yy0);
                }
                for (i = 0; i < sequence_size(yymsp[0].minor.yy0); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(yymsp[0].minor.yy0, i);
                        generate_turtle_statement(NULL/*parser*/, t);
                }
        }

        if (yymsp[0].minor.yy0)
                sequence_free(yymsp[0].minor.yy0);

        if (yymsp[-1].minor.yy0)
                rdf_term_free(yymsp[-1].minor.yy0);
}
#line 1085 "./turtle_parser.c"
        break;
      case 18: /* predicateObjectList ::= verb objectList predicateObjectList_ast */
#line 214 "./turtle_parser.y"
{
        if (yymsp[-2].minor.yy0 && yymsp[-1].minor.yy0) {
                for (size_t i = 0; i < sequence_size(yymsp[-1].minor.yy0); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(yymsp[-1].minor.yy0, i);

                        t->predicate = rdf_term_copy(yymsp[-2].minor.yy0);
                }
                /* @TODO: double copy -> should be optimalized */
                if (yymsp[0].minor.yy0) {
                        assert(0);
                        while ( sequence_size(yymsp[0].minor.yy0) ) {
                                struct rdf_statement* t = (struct rdf_statement*)
                                                                sequence_unshift(yymsp[0].minor.yy0);
                                if (sequence_push(yymsp[-1].minor.yy0, t)) {
                                        sequence_free(yymsp[-1].minor.yy0);
                                        sequence_free(yymsp[0].minor.yy0);
                                        rdf_term_free(yymsp[-2].minor.yy0);
                                        YYERROR;
                                }
                        }

                        sequence_free(yymsp[-1].minor.yy0);
                }

                if (yymsp[-2].minor.yy0)
                        rdf_term_free(yymsp[-2].minor.yy0);

                yygotominor.yy0 = yymsp[-1].minor.yy0;
        }
}
#line 1120 "./turtle_parser.c"
        break;
      case 19: /* predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst */
#line 246 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy0) {
                if (yymsp[0].minor.yy0)
                        yygotominor.yy0 = yymsp[0].minor.yy0;
                else
                        yygotominor.yy0 = NULL;
        } else {
                while ( sequence_size(yymsp[0].minor.yy0) ) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_unshift(yymsp[0].minor.yy0);
                        if (sequence_push(yymsp[-1].minor.yy0,t)) {
                                sequence_free(yymsp[-1].minor.yy0);
                                sequence_free(yymsp[0].minor.yy0);
                                YYERROR;
                        }
                }
                sequence_free(yymsp[0].minor.yy0);
                yygotominor.yy0 = yymsp[-1].minor.yy0;
        }
}
#line 1144 "./turtle_parser.c"
        break;
      case 20: /* predicateObjectList_ast ::= */
      case 22: /* predicateObjectList_qst ::= SEMICOLON */ yytestcase(yyruleno==22);
#line 266 "./turtle_parser.y"
{ yygotominor.yy0 = NULL; }
#line 1150 "./turtle_parser.c"
        break;
      case 21: /* predicateObjectList_qst ::= SEMICOLON verb objectList */
#line 268 "./turtle_parser.y"
{
        if (yymsp[-1].minor.yy0 && yymsp[0].minor.yy0) {
                /* non-empty property list */
                for (size_t i = 0; i < sequence_size(yymsp[0].minor.yy0); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(yymsp[0].minor.yy0, i);
                        t->predicate = rdf_term_copy(yymsp[-1].minor.yy0);
                }
                yygotominor.yy0 = yymsp[0].minor.yy0;
        } else {
                yygotominor.yy0 = NULL;
        }
}
#line 1167 "./turtle_parser.c"
        break;
      case 23: /* objectList ::= object objectList_ast */
#line 284 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[-1].minor.yy0, NULL);
        if (!yymsp[0].minor.yy0) {
                yygotominor.yy0 = sequence_create();
                if (!yygotominor.yy0)
                        YYERROR; /* failed to  create sequence. */
                if ( sequence_push(yygotominor.yy0, triple) ) {
                        sequence_free(yygotominor.yy0);
                        YYERROR; /* couldn't push the object into sequence*/
                }
        } else {
                yygotominor.yy0 = yymsp[0].minor.yy0;
                sequence_shift(yygotominor.yy0, triple); /* prepend object to the sequence */
        }
}
#line 1186 "./turtle_parser.c"
        break;
      case 24: /* objectList_ast ::= objectList_ast COMMA object */
#line 299 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[0].minor.yy0, NULL);
        if (!yymsp[-2].minor.yy0) {
                yygotominor.yy0 = sequence_create();
                if (!yygotominor.yy0)
                        YYERROR;
        }
        sequence_push(yygotominor.yy0, triple);
}
#line 1199 "./turtle_parser.c"
        break;
      case 25: /* objectList_ast ::= objectList_ast COMMA */
#line 311 "./turtle_parser.y"
{ /* Custom handling of ES */ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 1204 "./turtle_parser.c"
        break;
      case 27: /* verb ::= predicate */
      case 29: /* subject ::= iri */ yytestcase(yyruleno==29);
      case 30: /* subject ::= blankNode */ yytestcase(yyruleno==30);
      case 31: /* subject ::= collection */ yytestcase(yyruleno==31);
      case 32: /* predicate ::= iri */ yytestcase(yyruleno==32);
      case 33: /* object ::= iri */ yytestcase(yyruleno==33);
      case 34: /* object ::= blankNode */ yytestcase(yyruleno==34);
      case 35: /* object ::= collection */ yytestcase(yyruleno==35);
      case 36: /* object ::= blankNodePropertyList */ yytestcase(yyruleno==36);
      case 37: /* object ::= literal */ yytestcase(yyruleno==37);
      case 38: /* literal ::= rdfLiteral */ yytestcase(yyruleno==38);
      case 39: /* literal ::= numericalLiteral */ yytestcase(yyruleno==39);
      case 40: /* literal ::= booleanLiteral */ yytestcase(yyruleno==40);
      case 54: /* string ::= STRING_LITERAL */ yytestcase(yyruleno==54);
#line 315 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 1222 "./turtle_parser.c"
        break;
      case 28: /* verb ::= A */
#line 316 "./turtle_parser.y"
{
        utf8 s = string_create("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
        yygotominor.yy0 = rdf_term_from_uri(s);
        string_dealloc(s);
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1233 "./turtle_parser.c"
        break;
      case 41: /* blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE */
#line 346 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 1238 "./turtle_parser.c"
        break;
      case 42: /* collection ::= L_CURLY collection_ast R_CURLY */
#line 349 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy0) {
                utf8 s = string_create(rdfNil);
                yygotominor.yy0 = rdf_term_from_uri(s);
                string_dealloc(s);
                if (!yygotominor.yy0)
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

                for (int i = sequence_size(yymsp[-1].minor.yy0) - 1; i >= 0; i--) {
                        struct rdf_statement* t = (struct rdf_statement *)
                                                        sequence_get_at(yymsp[-1].minor.yy0, i);

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

                sequence_free(yymsp[-1].minor.yy0);
                rdf_term_free(first);
                rdf_term_free(rest);

                yygotominor.yy0 = object;

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

                        sequence_free(yymsp[-1].minor.yy0);
                        YYERROR;
                }
        }
}
#line 1362 "./turtle_parser.c"
        break;
      case 43: /* collection_ast ::= collection_ast object */
#line 469 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy0) {
                yymsp[-1].minor.yy0 = sequence_create();
                if (!yymsp[-1].minor.yy0)
                        YYERROR;
        }

        if (yymsp[0].minor.yy0) {
                struct rdf_statement* t = rdf_statement_from_nodes(NULL, NULL, yymsp[0].minor.yy0, NULL);
                if (!t)
                        YYERROR;

                if (sequence_push(yymsp[-1].minor.yy0, t)) {
                        sequence_free(yymsp[-1].minor.yy0);
                        yymsp[-1].minor.yy0 = NULL;
                        YYERROR;
                }
        }

        yygotominor.yy0 = yymsp[-1].minor.yy0;
}
#line 1387 "./turtle_parser.c"
        break;
      case 44: /* collection_ast ::= */
#line 490 "./turtle_parser.y"
{ yygotominor.yy0 = NULL;  }
#line 1392 "./turtle_parser.c"
        break;
      case 45: /* numericalLiteral ::= INTEGER_LITERAL */
#line 494 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(string_create(yymsp[0].minor.yy0), NULL, string_create(rdfInteger));
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1401 "./turtle_parser.c"
        break;
      case 46: /* numericalLiteral ::= DECIMAL_LITERAL */
#line 499 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(string_create(yymsp[0].minor.yy0), NULL, string_create(rdfDecimal));
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1410 "./turtle_parser.c"
        break;
      case 47: /* numericalLiteral ::= DOUBLE_LITERAL */
#line 504 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(string_create(yymsp[0].minor.yy0), NULL, string_create(rdfDouble));
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1419 "./turtle_parser.c"
        break;
      case 48: /* rdfLiteral ::= string rdfLiteral_qst */
#line 511 "./turtle_parser.y"
{
        if (!yymsp[0].minor.yy0)
                yygotominor.yy0 = rdf_term_from_literal(yymsp[-1].minor.yy0,NULL,NULL);
        else
                yygotominor.yy0 = rdf_term_from_literal(yymsp[-1].minor.yy0,/*((yymsp[0].minor.yy0.type == 1))? yymsp[0].minor.yy0.value.lang:*/NULL,
                                               /* ((yymsp[0].minor.yy0.type == 2))? yymsp[0].minor.yy0.value.iri :*/ NULL );

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1433 "./turtle_parser.c"
        break;
      case 49: /* rdfLiteral_qst ::= rdfLiteral_opt */
#line 522 "./turtle_parser.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 1438 "./turtle_parser.c"
        break;
      case 50: /* rdfLiteral_qst ::= */
#line 523 "./turtle_parser.y"
{yygotominor.yy0 = NULL;}
#line 1443 "./turtle_parser.c"
        break;
      case 51: /* rdfLiteral_opt ::= LANGTAG */
#line 526 "./turtle_parser.y"
{ yygotominor.yy0/*.type = 1; yygotominor.yy0.value.lang*/ = yymsp[0].minor.yy0; }
#line 1448 "./turtle_parser.c"
        break;
      case 52: /* rdfLiteral_opt ::= HAT iri */
#line 527 "./turtle_parser.y"
{ yygotominor.yy0/*.type = 2; yygotominor.yy0.value.iri*/  = yymsp[0].minor.yy0; }
#line 1453 "./turtle_parser.c"
        break;
      case 53: /* booleanLiteral ::= BOOLEAN_LITERAL */
#line 530 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(string_create(yymsp[0].minor.yy0), NULL, string_create(rdfBoolean));
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1462 "./turtle_parser.c"
        break;
      case 55: /* iri ::= IRIREF */
#line 540 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                yygotominor.yy0 =  rdf_term_from_uri(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);

                if (!yygotominor.yy0)
                        YYERROR;

        } else {
                yygotominor.yy0 = NULL;
        }
}
#line 1478 "./turtle_parser.c"
        break;
      case 56: /* iri ::= QNAME */
#line 552 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                //TODO: qname qualification
                yygotominor.yy0 = rdf_term_from_uri(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);

                if (!yygotominor.yy0)
                        YYERROR;
        } else {
                yygotominor.yy0 = NULL;
        }
}
#line 1494 "./turtle_parser.c"
        break;
      case 57: /* blankNode ::= BLANK_LITERAL */
#line 566 "./turtle_parser.y"
{
        uint8_t* z = generate_bnodeid(NULL, yymsp[0].minor.yy0);
        utf8 id = string_create(yymsp[0].minor.yy0);

        if (!id)
                YYERROR;

        yygotominor.yy0 = rdf_term_from_blank(id);
        string_dealloc(id);

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1511 "./turtle_parser.c"
        break;
      case 58: /* blankNode ::= anon */
#line 579 "./turtle_parser.y"
{
        uint8_t* id = generate_bnodeid(NULL, NULL);
        if (!id)
                YYERROR;

        yygotominor.yy0 = rdf_term_from_blank(id);

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1525 "./turtle_parser.c"
        break;
      default:
      /* (0) turtleDoc ::= statements */ yytestcase(yyruleno==0);
      /* (1) statements ::= statements statement */ yytestcase(yyruleno==1);
      /* (2) statements ::= */ yytestcase(yyruleno==2);
      /* (3) statement ::= directive */ yytestcase(yyruleno==3);
      /* (4) statement ::= triples DOT */ yytestcase(yyruleno==4);
      /* (5) directive ::= prefixID */ yytestcase(yyruleno==5);
      /* (6) directive ::= base */ yytestcase(yyruleno==6);
      /* (7) directive ::= sparqlPrefix */ yytestcase(yyruleno==7);
      /* (8) directive ::= sparqlBase */ yytestcase(yyruleno==8);
      /* (17) triples ::= blankNodePropertyList */ yytestcase(yyruleno==17);
      /* (26) objectList_ast ::= */ yytestcase(yyruleno==26);
      /* (59) anon ::= L_SQUARE R_SQUARE */ yytestcase(yyruleno==59);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
#line 32 "./turtle_parser.y"
 fprintf(stderr, "Parser failure\n"); /*parser->error.code = 1;*/ 
#line 1587 "./turtle_parser.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 34 "./turtle_parser.y"
 fprintf(stderr, "Syntax error\n"); /*parser->error.code = 2;*/ 
#line 1604 "./turtle_parser.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
#line 33 "./turtle_parser.y"
 fprintf(stderr, "Final parser statistics.\n"); 
#line 1625 "./turtle_parser.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
