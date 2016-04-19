/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 15 "./turtle_parser.y"

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

#define YYMALLOCARGTYPE  u64

#define YYERROR do {                                                               \
                        ardp_fprintf(stderr,                                       \
                                     kARDPColorMagenta,                            \
                                     "\n[parser error] on line: %lu\n\n", p->stats.line);   \
                        p->stats.n_errors++;                                       \
                }while(0)

const uint8_t *const xsd_uri    = (uint8_t *) "http://www.w3.org/2001/XMLSchema#";

const uint8_t *const rdfNil     = (uint8_t *) "http://www.w3.org/1999/02/22-rdf-syntax-ns#nil";
const uint8_t *const rdfFirst   = (uint8_t *) "http://www.w3.org/2001/XMLSchema#First";
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

const uint8_t *const rdfType = (uint8_t *) "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
#line 52 "./turtle_parser.c"
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
#define YYNOCODE 58
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE void*
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  utf8 yy2;
  datatype_sys_t* yy8;
  struct sequence* yy33;
  struct rdf_term* yy79;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  struct parser* p ;
#define ParseARG_PDECL , struct parser* p 
#define ParseARG_FETCH  struct parser* p  = yypParser->p 
#define ParseARG_STORE yypParser->p  = p 
#define YYNSTATE 80
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
 /*     0 */    52,   53,   21,   55,   56,   57,   58,    8,   54,    6,
 /*    10 */    31,   32,   61,   48,   42,   47,    2,   78,   49,   50,
 /*    20 */    51,   72,   77,   79,   13,   31,   32,   15,   66,   46,
 /*    30 */    69,   70,   71,   73,   74,   75,   76,   72,   10,    9,
 /*    40 */    67,   46,   22,   15,   24,   12,   69,   70,   71,   73,
 /*    50 */    74,   75,   76,   63,   10,   72,    3,   46,   20,   59,
 /*    60 */    28,   68,   77,   79,   69,   70,   71,   73,   74,   75,
 /*    70 */    76,   18,   10,   31,   32,   46,   41,   11,  141,    1,
 /*    80 */     4,    7,   27,   14,   34,   36,   37,   38,   72,   23,
 /*    90 */    43,   44,   45,   30,   35,   39,   40,   69,   70,   71,
 /*   100 */    73,   74,   75,   76,    5,   10,   60,   19,   46,   80,
 /*   110 */    64,   17,   62,   31,   32,   25,   29,   16,  142,   26,
 /*   120 */   142,    7,    2,   14,   31,   32,   31,   32,   77,   79,
 /*   130 */   142,   78,   45,   33,    7,  142,   14,  142,   36,   37,
 /*   140 */    38,  142,  142,   43,   44,   45,  142,   65,  142,  142,
 /*   150 */     2,  142,  142,  142,  142,  142,   77,   79,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    27,   28,   29,   30,   31,   32,   33,   34,    1,   36,
 /*    10 */     4,    5,    1,   35,   45,   37,   38,   11,   45,   46,
 /*    20 */    47,   36,   44,   45,   39,    4,    5,   42,   41,   56,
 /*    30 */    45,   46,   47,   48,   49,   50,   51,   36,   53,    9,
 /*    40 */    39,   56,    3,   42,    5,   40,   45,   46,   47,   48,
 /*    50 */    49,   50,   51,    4,   53,   36,   38,   56,    3,    1,
 /*    60 */     5,   42,   44,   45,   45,   46,   47,   48,   49,   50,
 /*    70 */    51,    4,   53,    4,    5,   56,   19,   20,   25,   26,
 /*    80 */    52,   12,    4,   14,   15,   16,   17,   18,   36,   43,
 /*    90 */    21,   22,   23,   13,   42,   54,   55,   45,   46,   47,
 /*   100 */    48,   49,   50,   51,   10,   53,    1,    4,   56,    0,
 /*   110 */     4,    2,    4,    4,    5,    6,    7,    8,   57,   35,
 /*   120 */    57,   12,   38,   14,    4,    5,    4,    5,   44,   45,
 /*   130 */    57,   11,   23,   13,   12,   57,   14,   57,   16,   17,
 /*   140 */    18,   57,   57,   21,   22,   23,   57,   35,   57,   57,
 /*   150 */    38,   57,   57,   57,   57,   57,   44,   45,
};
#define YY_SHIFT_USE_DFLT (-1)
#define YY_SHIFT_MAX 29
static const signed char yy_shift_ofst[] = {
 /*     0 */    -1,  109,  122,  122,   69,  122,    6,  120,    6,    6,
 /*    10 */    57,   21,   30,   -1,   -1,   -1,   55,   39,   11,   58,
 /*    20 */    49,    7,  103,   94,   78,   67,   80,  105,  106,  108,
};
#define YY_REDUCE_USE_DFLT (-32)
#define YY_REDUCE_MAX 15
static const signed char yy_reduce_ofst[] = {
 /*     0 */    53,  -27,  -15,    1,   52,   19,  -22,   84,  112,   18,
 /*    10 */    41,  -31,  -13,    5,   28,   46,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    82,  140,  140,  140,  140,  140,   98,  140,  140,  103,
 /*    10 */   130,  140,   99,  101,  124,  106,  140,  140,  140,  140,
 /*    20 */   140,  140,  140,  104,  140,  140,  140,  140,  140,  140,
 /*    30 */   121,  135,  136,  139,  122,  123,  125,  126,  127,  128,
 /*    40 */   129,  131,  132,  133,  134,  137,  138,   96,   97,  109,
 /*    50 */   110,  111,   81,   83,   84,   85,   86,   87,   88,   89,
 /*    60 */    90,   91,   92,   93,   94,   95,  100,  102,  105,  113,
 /*    70 */   114,  115,  116,  117,  118,  119,  120,  107,  108,  112,
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
  "blankNodePropertyList",  "predicateObjectList_astr",  "verb",          "objectList",  
  "predicateObjectList_ast",  "predicateObjectList_qst",  "object",        "objectList_ast",
  "predicate",     "iri",           "blankNode",     "collection",  
  "literal",       "rdfLiteral",    "numericalLiteral",  "booleanLiteral",
  "collection_ast",  "string",        "rdfLiteral_qst",  "rdfLiteral_opt",
  "anon",        
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
 /*  16 */ "triples ::= blankNodePropertyList predicateObjectList_astr",
 /*  17 */ "predicateObjectList_astr ::= predicateObjectList",
 /*  18 */ "predicateObjectList_astr ::=",
 /*  19 */ "predicateObjectList ::= verb objectList predicateObjectList_ast",
 /*  20 */ "predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst",
 /*  21 */ "predicateObjectList_ast ::=",
 /*  22 */ "predicateObjectList_qst ::= SEMICOLON verb objectList",
 /*  23 */ "predicateObjectList_qst ::= SEMICOLON",
 /*  24 */ "objectList ::= object objectList_ast",
 /*  25 */ "objectList_ast ::= objectList_ast COMMA object",
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
      /* TERMINAL Destructor */
    case 1: /* DOT */
    case 2: /* PREFIX */
    case 3: /* COLON */
    case 4: /* IRIREF */
    case 5: /* QNAME */
    case 6: /* BASE */
    case 7: /* SPARQL_BASE */
    case 8: /* SPARQL_PREFIX */
    case 9: /* SEMICOLON */
    case 10: /* COMMA */
    case 11: /* A */
    case 12: /* L_SQUARE */
    case 13: /* R_SQUARE */
    case 14: /* L_CURLY */
    case 15: /* R_CURLY */
    case 16: /* INTEGER_LITERAL */
    case 17: /* DECIMAL_LITERAL */
    case 18: /* DOUBLE_LITERAL */
    case 19: /* LANGTAG */
    case 20: /* HAT */
    case 21: /* BOOLEAN_LITERAL */
    case 22: /* STRING_LITERAL */
    case 23: /* BLANK_LITERAL */
{
#line 13 "./turtle_parser.y"
 string_dealloc((yypminor->yy0)); 
#line 529 "./turtle_parser.c"
}
      break;
    case 34: /* subject */
    case 36: /* blankNodePropertyList */
    case 38: /* verb */
    case 42: /* object */
    case 44: /* predicate */
    case 45: /* iri */
    case 46: /* blankNode */
    case 47: /* collection */
    case 48: /* literal */
    case 49: /* rdfLiteral */
    case 50: /* numericalLiteral */
    case 51: /* booleanLiteral */
    case 56: /* anon */
{
#line 334 "./turtle_parser.y"
 rdf_term_free((yypminor->yy79)); 
#line 548 "./turtle_parser.c"
}
      break;
    case 35: /* predicateObjectList */
    case 39: /* objectList */
    case 40: /* predicateObjectList_ast */
    case 41: /* predicateObjectList_qst */
    case 43: /* objectList_ast */
    case 52: /* collection_ast */
{
#line 201 "./turtle_parser.y"
 sequence_free((yypminor->yy33)); 
#line 560 "./turtle_parser.c"
}
      break;
    case 53: /* string */
{
#line 647 "./turtle_parser.y"
 string_dealloc((yypminor->yy2)); 
#line 567 "./turtle_parser.c"
}
      break;
    case 54: /* rdfLiteral_qst */
    case 55: /* rdfLiteral_opt */
{
#line 592 "./turtle_parser.y"
 string_dealloc((yypminor->yy8)->datatype); string_dealloc((yypminor->yy8)->langtag); if((yypminor->yy8)) free((yypminor->yy8)); 
#line 575 "./turtle_parser.c"
}
      break;
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
#line 80 "./turtle_parser.y"

        ardp_fprintf(stderr, kARDPColorMagenta, "Parser stack overflow.\n");
#line 751 "./turtle_parser.c"
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
  { 37, 1 },
  { 37, 0 },
  { 35, 3 },
  { 40, 2 },
  { 40, 0 },
  { 41, 3 },
  { 41, 1 },
  { 39, 2 },
  { 43, 3 },
  { 43, 0 },
  { 38, 1 },
  { 38, 1 },
  { 34, 1 },
  { 34, 1 },
  { 34, 1 },
  { 44, 1 },
  { 42, 1 },
  { 42, 1 },
  { 42, 1 },
  { 42, 1 },
  { 42, 1 },
  { 48, 1 },
  { 48, 1 },
  { 48, 1 },
  { 36, 3 },
  { 47, 3 },
  { 52, 2 },
  { 52, 0 },
  { 50, 1 },
  { 50, 1 },
  { 50, 1 },
  { 49, 2 },
  { 54, 1 },
  { 54, 0 },
  { 55, 1 },
  { 55, 2 },
  { 51, 1 },
  { 53, 1 },
  { 45, 1 },
  { 45, 1 },
  { 46, 1 },
  { 46, 1 },
  { 56, 2 },
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
      case 3: /* statement ::= directive */
#line 93 "./turtle_parser.y"
{ p->stats.n_directives++; }
#line 925 "./turtle_parser.c"
        break;
      case 4: /* statement ::= triples DOT */
#line 94 "./turtle_parser.y"
{
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
#line 932 "./turtle_parser.c"
        break;
      case 9: /* prefixID ::= PREFIX COLON IRIREF DOT */
#line 103 "./turtle_parser.y"
{
        if(p->cb.add_namespace((uint8_t *)":",yymsp[-1].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                YYERROR;
        } else {
                string_dealloc(yymsp[-1].minor.yy0);
        }
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,3,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
#line 947 "./turtle_parser.c"
        break;
      case 10: /* prefixID ::= PREFIX QNAME IRIREF DOT */
#line 111 "./turtle_parser.y"
{
        if(p->cb.add_namespace(yymsp[-2].minor.yy0,yymsp[-1].minor.yy0))
                YYERROR;
        string_dealloc(yymsp[-2].minor.yy0);
        string_dealloc(yymsp[-1].minor.yy0);
  yy_destructor(yypParser,2,&yymsp[-3].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
#line 959 "./turtle_parser.c"
        break;
      case 11: /* base ::= BASE IRIREF DOT */
#line 119 "./turtle_parser.y"
{
        if (p->cb.rebase(yymsp[-1].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                YYERROR;
        } else {
                string_dealloc(yymsp[-1].minor.yy0);
        }
  yy_destructor(yypParser,6,&yymsp[-2].minor);
  yy_destructor(yypParser,1,&yymsp[0].minor);
}
#line 973 "./turtle_parser.c"
        break;
      case 12: /* sparqlBase ::= SPARQL_BASE IRIREF */
#line 129 "./turtle_parser.y"
{
        if (p->cb.rebase(yymsp[0].minor.yy0)) {
                string_dealloc(yymsp[0].minor.yy0);
                YYERROR;
        } else {
                string_dealloc(yymsp[0].minor.yy0);
        }
  yy_destructor(yypParser,7,&yymsp[-1].minor);
}
#line 986 "./turtle_parser.c"
        break;
      case 13: /* sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF */
#line 139 "./turtle_parser.y"
{
        utf8 colon = string_create((uint8_t *)":");
        if (p->cb.add_namespace(colon, yymsp[0].minor.yy0))
                YYERROR;

        string_dealloc(yymsp[0].minor.yy0);
        string_dealloc(colon);
  yy_destructor(yypParser,8,&yymsp[-2].minor);
  yy_destructor(yypParser,3,&yymsp[-1].minor);
}
#line 1000 "./turtle_parser.c"
        break;
      case 14: /* sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF */
#line 147 "./turtle_parser.y"
{
        if (p->cb.add_namespace(yymsp[-1].minor.yy0,yymsp[0].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);
                YYERROR;
        } else {
                string_dealloc(yymsp[-1].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);
        }
  yy_destructor(yypParser,8,&yymsp[-2].minor);
}
#line 1015 "./turtle_parser.c"
        break;
      case 15: /* triples ::= subject predicateObjectList */
#line 159 "./turtle_parser.y"
{
        if (yymsp[-1].minor.yy79 && yymsp[0].minor.yy33) {
                /* have subject and property list with items */
                #pragma vectorize enable
                while ( sequence_size(yymsp[0].minor.yy33) ) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy33);
                        t->subject = rdf_term_copy(yymsp[-1].minor.yy79);

                        p->cb.statement(t);
                        rdf_statement_free(t);
                }
        }

        if (yymsp[-1].minor.yy79)
                rdf_term_free(yymsp[-1].minor.yy79);
        if (yymsp[0].minor.yy33)
                sequence_free(yymsp[0].minor.yy33);
}
#line 1037 "./turtle_parser.c"
        break;
      case 16: /* triples ::= blankNodePropertyList predicateObjectList_astr */
#line 177 "./turtle_parser.y"
{

        if (yymsp[-1].minor.yy79 && yymsp[0].minor.yy0) {
                while ( sequence_size(yymsp[0].minor.yy0) ) {
                        struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy0);
                        s->subject = rdf_term_copy(yymsp[-1].minor.yy79);

                        p->cb.statement(s);
                        rdf_statement_free(s);
                }
        }

        if (yymsp[-1].minor.yy79)
                rdf_term_free(yymsp[-1].minor.yy79);
        if (yymsp[0].minor.yy0)
                sequence_free(yymsp[0].minor.yy0);
}
#line 1058 "./turtle_parser.c"
        break;
      case 17: /* predicateObjectList_astr ::= predicateObjectList */
#line 195 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[0].minor.yy33; }
#line 1063 "./turtle_parser.c"
        break;
      case 18: /* predicateObjectList_astr ::= */
#line 196 "./turtle_parser.y"
{ yygotominor.yy0 = NULL; }
#line 1068 "./turtle_parser.c"
        break;
      case 19: /* predicateObjectList ::= verb objectList predicateObjectList_ast */
#line 202 "./turtle_parser.y"
{
        if (yymsp[-2].minor.yy79 && yymsp[-1].minor.yy33) {
                for (size_t i = 0; i < sequence_size(yymsp[-1].minor.yy33); i++) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_get_at(yymsp[-1].minor.yy33, i);
                        t->predicate = rdf_term_copy(yymsp[-2].minor.yy79);
                }
                /* @TODO: double copy -> should be optimalized */
                if (yymsp[0].minor.yy33) {
                        while ( sequence_size(yymsp[0].minor.yy33) ) {
                                struct rdf_statement* t2 = (struct rdf_statement*)
                                                                sequence_unshift(yymsp[0].minor.yy33);
                                if (sequence_push(yymsp[-1].minor.yy33, t2)) {
                                        sequence_free(yymsp[-1].minor.yy33);
                                        sequence_free(yymsp[0].minor.yy33);
                                        rdf_term_free(yymsp[-2].minor.yy79);
                                        rdf_statement_free(t2);
                                        YYERROR;
                                }
                        }
                        sequence_free(yymsp[0].minor.yy33);
                }

                rdf_term_free(yymsp[-2].minor.yy79);

                yygotominor.yy33 = yymsp[-1].minor.yy33;
        } else {
                if (yymsp[-2].minor.yy79)
                        rdf_term_free(yymsp[-2].minor.yy79);
                if (yymsp[-1].minor.yy33)
                        sequence_free(yymsp[-1].minor.yy33);

                if (yymsp[0].minor.yy33)
                        yygotominor.yy33 = yymsp[0].minor.yy33;
                else
                        yygotominor.yy33 = NULL;
        }
}
#line 1109 "./turtle_parser.c"
        break;
      case 20: /* predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst */
#line 242 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy33) {
                if (yymsp[0].minor.yy33)
                        yygotominor.yy33 = yymsp[0].minor.yy33;
                else
                        yygotominor.yy33 = NULL;
        } else {
                while ( sequence_size(yymsp[0].minor.yy33) ) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy33);
                        if (sequence_push(yymsp[-1].minor.yy33,t)) {
                                sequence_free(yymsp[-1].minor.yy33);
                                sequence_free(yymsp[0].minor.yy33);
                                YYERROR;
                        }
                }
                sequence_free(yymsp[0].minor.yy33);
                yygotominor.yy33 = yymsp[-1].minor.yy33;
        }
}
#line 1132 "./turtle_parser.c"
        break;
      case 21: /* predicateObjectList_ast ::= */
      case 26: /* objectList_ast ::= */ yytestcase(yyruleno==26);
#line 261 "./turtle_parser.y"
{ yygotominor.yy33 = NULL; }
#line 1138 "./turtle_parser.c"
        break;
      case 22: /* predicateObjectList_qst ::= SEMICOLON verb objectList */
#line 266 "./turtle_parser.y"
{
        if (yymsp[-1].minor.yy79 && yymsp[0].minor.yy33) {
                /* non-empty property list */
                for (size_t i = 0; i < sequence_size(yymsp[0].minor.yy33); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(yymsp[0].minor.yy33, i);
                        t->predicate = rdf_term_copy(yymsp[-1].minor.yy79);
                }
                yygotominor.yy33 = yymsp[0].minor.yy33;
        } else {
                yygotominor.yy33 = NULL;
        }
        rdf_term_free(yymsp[-1].minor.yy79);
  yy_destructor(yypParser,9,&yymsp[-2].minor);
}
#line 1157 "./turtle_parser.c"
        break;
      case 23: /* predicateObjectList_qst ::= SEMICOLON */
#line 280 "./turtle_parser.y"
{ yygotominor.yy33 = NULL;   yy_destructor(yypParser,9,&yymsp[0].minor);
}
#line 1163 "./turtle_parser.c"
        break;
      case 24: /* objectList ::= object objectList_ast */
#line 285 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[-1].minor.yy79, NULL);
        if (!yymsp[0].minor.yy33) {
                yygotominor.yy33 = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
                if (!yygotominor.yy33)
                        YYERROR; /* failed to  create sequence. */
                if ( sequence_push(yygotominor.yy33, triple) ) {
                        sequence_free(yygotominor.yy33);
                        rdf_statement_free(triple);
                        YYERROR; /* couldn't push the object into sequence*/
                }
        } else {
                yygotominor.yy33 = yymsp[0].minor.yy33;
                sequence_shift(yygotominor.yy33, triple); /* prepend object to the sequence */
        }
}
#line 1183 "./turtle_parser.c"
        break;
      case 25: /* objectList_ast ::= objectList_ast COMMA object */
#line 303 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[0].minor.yy79, NULL);
        if (!yymsp[-2].minor.yy33) {
                yygotominor.yy33 = sequence_create((sequence_free_handler) &rdf_statement_free, NULL, NULL);
                if (!yygotominor.yy33)
                        YYERROR;
        } else {
                yygotominor.yy33 = yymsp[-2].minor.yy33;
        }
        sequence_push(yygotominor.yy33, triple);
  yy_destructor(yypParser,10,&yymsp[-1].minor);
}
#line 1199 "./turtle_parser.c"
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
#line 323 "./turtle_parser.y"
{ yygotominor.yy79 = yymsp[0].minor.yy79; }
#line 1216 "./turtle_parser.c"
        break;
      case 28: /* verb ::= A */
#line 324 "./turtle_parser.y"
{
        utf8 s = string_create(rdfType);
        yygotominor.yy79 = rdf_term_from_uri(s);
        string_dealloc(s);
        if (!yygotominor.yy79)
                YYERROR;
  yy_destructor(yypParser,11,&yymsp[0].minor);
}
#line 1228 "./turtle_parser.c"
        break;
      case 41: /* blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE */
#line 369 "./turtle_parser.y"
{

        utf8 s = p->cb.generate_bnode();
        yygotominor.yy79 = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!yygotominor.yy79) {
                sequence_free(yymsp[-1].minor.yy33);
                yygotominor.yy79 = NULL;
                YYERROR;
        } else {
                if (yymsp[-1].minor.yy33) {
                        /* Handle non empty list */
                        while(sequence_size(yymsp[-1].minor.yy33)) {
                                struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(yymsp[-1].minor.yy33);
                                s->subject = rdf_term_copy(yygotominor.yy79);

                                p->cb.statement(s);
                                rdf_statement_free(s);
                        }
                        sequence_free(yymsp[-1].minor.yy33);
                }
        }
  yy_destructor(yypParser,12,&yymsp[-2].minor);
  yy_destructor(yypParser,13,&yymsp[0].minor);
}
#line 1258 "./turtle_parser.c"
        break;
      case 42: /* collection ::= L_CURLY collection_ast R_CURLY */
#line 397 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy33) {
                utf8 s = string_create(rdfNil);
                yygotominor.yy79 = rdf_term_from_uri(s);
                string_dealloc(s);
                if (!yygotominor.yy79)
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

                for (int i = sequence_size(yymsp[-1].minor.yy33) - 1; i >= 0; i--) {
                        struct rdf_statement* t = (struct rdf_statement *)
                                                        sequence_get_at(yymsp[-1].minor.yy33, i);

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

                sequence_free(yymsp[-1].minor.yy33);
                rdf_term_free(first);
                rdf_term_free(rest);

                yygotominor.yy79 = object;

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

                        sequence_free(yymsp[-1].minor.yy33);
                        YYERROR;
                }
        }
  yy_destructor(yypParser,14,&yymsp[-2].minor);
  yy_destructor(yypParser,15,&yymsp[0].minor);
}
#line 1383 "./turtle_parser.c"
        break;
      case 43: /* collection_ast ::= collection_ast object */
#line 519 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy33) {
                yymsp[-1].minor.yy33 = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
                if (!yymsp[-1].minor.yy33)
                        YYERROR;
        }

        if (yymsp[0].minor.yy79) {
                struct rdf_statement* t = rdf_statement_from_nodes(NULL, NULL, yymsp[0].minor.yy79, NULL);
                if (!t)
                        YYERROR;

                if (sequence_push(yymsp[-1].minor.yy33, t)) {
                        sequence_free(yymsp[-1].minor.yy33);
                        yymsp[-1].minor.yy33 = NULL;
                        YYERROR;
                }
        }

        yygotominor.yy33 = yymsp[-1].minor.yy33;
}
#line 1408 "./turtle_parser.c"
        break;
      case 44: /* collection_ast ::= */
#line 540 "./turtle_parser.y"
{ yygotominor.yy33 = NULL;  }
#line 1413 "./turtle_parser.c"
        break;
      case 45: /* numericalLiteral ::= INTEGER_LITERAL */
#line 547 "./turtle_parser.y"
{
        utf8 dt = string_create(rdfInteger);
        yygotominor.yy79 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, dt);
        string_dealloc(dt);
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy79)
                YYERROR;
}
#line 1425 "./turtle_parser.c"
        break;
      case 46: /* numericalLiteral ::= DECIMAL_LITERAL */
#line 555 "./turtle_parser.y"
{
        utf8 dt = string_create(rdfDecimal);
        yygotominor.yy79 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, dt);
        string_dealloc(dt);
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy79)
                YYERROR;
}
#line 1437 "./turtle_parser.c"
        break;
      case 47: /* numericalLiteral ::= DOUBLE_LITERAL */
#line 563 "./turtle_parser.y"
{
        utf8 dt = string_create(rdfDouble);
        yygotominor.yy79 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, dt);
        string_dealloc(dt);
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy79)
                YYERROR;
}
#line 1449 "./turtle_parser.c"
        break;
      case 48: /* rdfLiteral ::= string rdfLiteral_qst */
#line 575 "./turtle_parser.y"
{
        if (!yymsp[0].minor.yy8) {
                yygotominor.yy79 = rdf_term_from_literal(yymsp[-1].minor.yy2,NULL,NULL);
        } else {
                yygotominor.yy79 = rdf_term_from_literal(yymsp[-1].minor.yy2, yymsp[0].minor.yy8->langtag, yymsp[0].minor.yy8->datatype);
                string_dealloc(yymsp[0].minor.yy8->langtag);
                string_dealloc(yymsp[0].minor.yy8->datatype);
                free(yymsp[0].minor.yy8);
        }
        string_dealloc(yymsp[-1].minor.yy2);

        if (!yygotominor.yy79)
                YYERROR;
}
#line 1467 "./turtle_parser.c"
        break;
      case 49: /* rdfLiteral_qst ::= rdfLiteral_opt */
#line 595 "./turtle_parser.y"
{yygotominor.yy8 = yymsp[0].minor.yy8;}
#line 1472 "./turtle_parser.c"
        break;
      case 50: /* rdfLiteral_qst ::= */
#line 596 "./turtle_parser.y"
{yygotominor.yy8 = NULL;}
#line 1477 "./turtle_parser.c"
        break;
      case 51: /* rdfLiteral_opt ::= LANGTAG */
#line 598 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                datatype_sys_t *s = calloc(1, sizeof(*s));
                if (s) {
                        s->datatype = NULL;
                        s->langtag  = yymsp[0].minor.yy0;
                        yygotominor.yy8 = s;
                } else {
                        string_dealloc(yymsp[0].minor.yy0);
                        yygotominor.yy8 = NULL;
                }
        } else {
                yygotominor.yy8 = NULL;
        }
}
#line 1496 "./turtle_parser.c"
        break;
      case 52: /* rdfLiteral_opt ::= HAT iri */
#line 613 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy79) {
                datatype_sys_t *s = calloc(1, sizeof(*s));
                if (s) {
                        struct rdf_term *t = yymsp[0].minor.yy79;
                        if (t->type == RDF_TERM_URI)
                                s->datatype = string_copy(t->value.uri);
                        else
                                s->datatype = NULL;
                        s->langtag = NULL;
                        yygotominor.yy8 = s;
                        rdf_term_free(yymsp[0].minor.yy79);
                } else {
                        rdf_term_free(yymsp[0].minor.yy79);
                        yygotominor.yy8 = NULL;
                }
        } else {
                yygotominor.yy8 = NULL;
        }
  yy_destructor(yypParser,20,&yymsp[-1].minor);
}
#line 1521 "./turtle_parser.c"
        break;
      case 53: /* booleanLiteral ::= BOOLEAN_LITERAL */
#line 637 "./turtle_parser.y"
{
        yygotominor.yy79 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, string_create(rdfBoolean));
        string_dealloc(yymsp[0].minor.yy0);

        if (!yygotominor.yy79)
                YYERROR;
}
#line 1532 "./turtle_parser.c"
        break;
      case 54: /* string ::= STRING_LITERAL */
#line 648 "./turtle_parser.y"
{ yygotominor.yy2 = yymsp[0].minor.yy0; }
#line 1537 "./turtle_parser.c"
        break;
      case 55: /* iri ::= IRIREF */
#line 653 "./turtle_parser.y"
{
        yygotominor.yy79 = rdf_term_from_uri(yymsp[0].minor.yy0);
        string_dealloc(yymsp[0].minor.yy0);

        if (!yygotominor.yy79) {
                YYERROR;
                yygotominor.yy79 = NULL;
        }
}
#line 1550 "./turtle_parser.c"
        break;
      case 56: /* iri ::= QNAME */
#line 662 "./turtle_parser.y"
{
        yygotominor.yy79 = rdf_term_from_curie(yymsp[0].minor.yy0);
        string_dealloc(yymsp[0].minor.yy0);

        if (!yygotominor.yy79) {
                YYERROR;
                yygotominor.yy79 = NULL;
        }

}
#line 1564 "./turtle_parser.c"
        break;
      case 57: /* blankNode ::= BLANK_LITERAL */
#line 676 "./turtle_parser.y"
{
        if (!yymsp[0].minor.yy0) {
                utf8 s = p->cb.generate_bnode();
                yygotominor.yy79 = rdf_term_from_blank(s);
                string_dealloc(s);
        } else {
                yygotominor.yy79 = rdf_term_from_blank(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);
        }

        if (!yygotominor.yy79)
                YYERROR;
}
#line 1581 "./turtle_parser.c"
        break;
      case 58: /* blankNode ::= anon */
#line 689 "./turtle_parser.y"
{
        yygotominor.yy79 = yymsp[0].minor.yy79;
}
#line 1588 "./turtle_parser.c"
        break;
      case 59: /* anon ::= L_SQUARE R_SQUARE */
#line 696 "./turtle_parser.y"
{
        utf8 s = p->cb.generate_bnode();
        yygotominor.yy79 = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!yygotominor.yy79) {
                YYERROR;
                yygotominor.yy79 = NULL;
        }
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yy_destructor(yypParser,13,&yymsp[0].minor);
}
#line 1604 "./turtle_parser.c"
        break;
      default:
      /* (0) turtleDoc ::= statements */ yytestcase(yyruleno==0);
      /* (1) statements ::= statements statement */ yytestcase(yyruleno==1);
      /* (2) statements ::= */ yytestcase(yyruleno==2);
      /* (5) directive ::= prefixID */ yytestcase(yyruleno==5);
      /* (6) directive ::= base */ yytestcase(yyruleno==6);
      /* (7) directive ::= sparqlPrefix */ yytestcase(yyruleno==7);
      /* (8) directive ::= sparqlBase */ yytestcase(yyruleno==8);
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
#line 60 "./turtle_parser.y"

        ardp_fprintf(stderr, kARDPColorMagenta, "Parser encountered error.\n"); /*parser->error.code = 1;*/
        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
#line 1663 "./turtle_parser.c"
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
#line 67 "./turtle_parser.y"

        p->stats.n_errors++;
        //int tok = yypParser->yystack[yypParser->yyidx-1].major;
        ardp_fprintf(stderr, kARDPColorMagenta, "Syntax error on line: %ld near %s" /*" (%s)"*/ ", expected: ", p->stats.line, TOKEN/*, yyTokenName[tok]*/); /*parser->error.code = 2;*/
        int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
        for (int i = 0; i < n; ++i) {
                int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
                if (a < YYNSTATE + YYNRULE)
                        ardp_fprintf(stderr, kARDPColorBoldMagenta, "%s ", yyTokenName[i]);

        }
        fprintf(stderr, "\n");
#line 1691 "./turtle_parser.c"
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
#line 64 "./turtle_parser.y"

        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
#line 1713 "./turtle_parser.c"
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
