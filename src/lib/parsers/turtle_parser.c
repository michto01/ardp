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

#line 47 "./turtle_parser.c"
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
  datatype_sys_t* yy8;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  struct parser* p ;
#define ParseARG_PDECL , struct parser* p 
#define ParseARG_FETCH  struct parser* p  = yypParser->p 
#define ParseARG_STORE yypParser->p  = p 
#define YYNSTATE 80
#define YYNRULE 61
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
 /*    70 */    76,   18,   10,   31,   32,   46,   41,   11,  142,    1,
 /*    80 */     4,    7,   27,   14,   34,   36,   37,   38,   72,   23,
 /*    90 */    43,   44,   45,   30,   35,   39,   40,   69,   70,   71,
 /*   100 */    73,   74,   75,   76,    5,   10,   60,   19,   46,   80,
 /*   110 */    64,   17,   62,   31,   32,   25,   29,   16,  143,   26,
 /*   120 */   143,    7,    2,   14,   31,   32,   31,   32,   77,   79,
 /*   130 */   143,   78,   45,   33,    7,  143,   14,  143,   36,   37,
 /*   140 */    38,  143,  143,   43,   44,   45,  143,   65,  143,  143,
 /*   150 */     2,  143,  143,  143,  143,  143,   77,   79,
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
 /*     0 */    82,  141,  141,  141,  141,  106,   98,  141,  141,  103,
 /*    10 */   131,  141,   99,  101,  125,  107,  141,  141,  141,  141,
 /*    20 */   141,  141,  141,  104,  141,  141,  141,  141,  141,  141,
 /*    30 */   122,  136,  137,  140,  123,  124,  126,  127,  128,  129,
 /*    40 */   130,  132,  133,  134,  135,  138,  139,   96,   97,  110,
 /*    50 */   111,  112,   81,   83,   84,   85,   86,   87,   88,   89,
 /*    60 */    90,   91,   92,   93,   94,   95,  100,  102,  105,  114,
 /*    70 */   115,  116,  117,  118,  119,  120,  121,  108,  109,  113,
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
 /*  26 */ "objectList_ast ::= objectList_ast COMMA",
 /*  27 */ "objectList_ast ::=",
 /*  28 */ "verb ::= predicate",
 /*  29 */ "verb ::= A",
 /*  30 */ "subject ::= iri",
 /*  31 */ "subject ::= blankNode",
 /*  32 */ "subject ::= collection",
 /*  33 */ "predicate ::= iri",
 /*  34 */ "object ::= iri",
 /*  35 */ "object ::= blankNode",
 /*  36 */ "object ::= collection",
 /*  37 */ "object ::= blankNodePropertyList",
 /*  38 */ "object ::= literal",
 /*  39 */ "literal ::= rdfLiteral",
 /*  40 */ "literal ::= numericalLiteral",
 /*  41 */ "literal ::= booleanLiteral",
 /*  42 */ "blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE",
 /*  43 */ "collection ::= L_CURLY collection_ast R_CURLY",
 /*  44 */ "collection_ast ::= collection_ast object",
 /*  45 */ "collection_ast ::=",
 /*  46 */ "numericalLiteral ::= INTEGER_LITERAL",
 /*  47 */ "numericalLiteral ::= DECIMAL_LITERAL",
 /*  48 */ "numericalLiteral ::= DOUBLE_LITERAL",
 /*  49 */ "rdfLiteral ::= string rdfLiteral_qst",
 /*  50 */ "rdfLiteral_qst ::= rdfLiteral_opt",
 /*  51 */ "rdfLiteral_qst ::=",
 /*  52 */ "rdfLiteral_opt ::= LANGTAG",
 /*  53 */ "rdfLiteral_opt ::= HAT iri",
 /*  54 */ "booleanLiteral ::= BOOLEAN_LITERAL",
 /*  55 */ "string ::= STRING_LITERAL",
 /*  56 */ "iri ::= IRIREF",
 /*  57 */ "iri ::= QNAME",
 /*  58 */ "blankNode ::= BLANK_LITERAL",
 /*  59 */ "blankNode ::= anon",
 /*  60 */ "anon ::= L_SQUARE R_SQUARE",
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
  { 37, 1 },
  { 37, 0 },
  { 35, 3 },
  { 40, 2 },
  { 40, 0 },
  { 41, 3 },
  { 41, 1 },
  { 39, 2 },
  { 43, 3 },
  { 43, 2 },
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
#line 76 "./turtle_parser.y"
{ p->stats.n_directives++; }
#line 839 "./turtle_parser.c"
        break;
      case 9: /* prefixID ::= PREFIX COLON IRIREF DOT */
#line 87 "./turtle_parser.y"
{
        if(p->cb.add_namespace(":",yymsp[-1].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                YYERROR;
        }
        string_dealloc(yymsp[-1].minor.yy0);
}
#line 850 "./turtle_parser.c"
        break;
      case 10: /* prefixID ::= PREFIX QNAME IRIREF DOT */
#line 94 "./turtle_parser.y"
{
        if(p->cb.add_namespace(yymsp[-2].minor.yy0,yymsp[-1].minor.yy0)) {
                string_dealloc(yymsp[-2].minor.yy0);
                string_dealloc(yymsp[-1].minor.yy0);
                YYERROR;
        }
        string_dealloc(yymsp[-2].minor.yy0);
        string_dealloc(yymsp[-1].minor.yy0);
}
#line 863 "./turtle_parser.c"
        break;
      case 11: /* base ::= BASE IRIREF DOT */
#line 105 "./turtle_parser.y"
{
        if (p->cb.rebase(yymsp[-1].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                YYERROR;
        }
        string_dealloc(yymsp[-1].minor.yy0);
}
#line 874 "./turtle_parser.c"
        break;
      case 12: /* sparqlBase ::= SPARQL_BASE IRIREF */
#line 114 "./turtle_parser.y"
{
        if (p->cb.rebase(yymsp[0].minor.yy0)) {
                string_dealloc(yymsp[0].minor.yy0);
                YYERROR;
        }
        string_dealloc(yymsp[0].minor.yy0);
}
#line 885 "./turtle_parser.c"
        break;
      case 13: /* sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF */
#line 123 "./turtle_parser.y"
{
        utf8 colon = string_create(":");
        if (p->cb.add_namespace(colon, yymsp[0].minor.yy0)) {
                string_dealloc(colon);
                string_dealloc(yymsp[0].minor.yy0);
                YYERROR;
        }
        string_dealloc(colon);
        string_dealloc(yymsp[0].minor.yy0);
}
#line 899 "./turtle_parser.c"
        break;
      case 14: /* sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF */
#line 133 "./turtle_parser.y"
{
        if (p->cb.add_namespace(yymsp[-1].minor.yy0,yymsp[0].minor.yy0)) {
                string_dealloc(yymsp[-1].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);
                YYERROR;
        }

        string_dealloc(yymsp[-1].minor.yy0);
        string_dealloc(yymsp[0].minor.yy0);
}
#line 913 "./turtle_parser.c"
        break;
      case 15: /* triples ::= subject predicateObjectList */
#line 145 "./turtle_parser.y"
{

        if (yymsp[-1].minor.yy0 && yymsp[0].minor.yy0) {
                /* have subject and property list with items */
                while ( sequence_size(yymsp[0].minor.yy0) ) {
                        struct rdf_statement* t = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy0);
                        t->subject = rdf_term_copy(yymsp[-1].minor.yy0);

                        p->cb.statement(t);
                        rdf_statement_free(t);
                }
        }

        if (yymsp[-1].minor.yy0)
                rdf_term_free(yymsp[-1].minor.yy0);
        if (yymsp[0].minor.yy0)
                sequence_free(yymsp[0].minor.yy0);
}
#line 935 "./turtle_parser.c"
        break;
      case 16: /* triples ::= blankNodePropertyList predicateObjectList_astr */
#line 163 "./turtle_parser.y"
{

        if (yymsp[-1].minor.yy0 && yymsp[0].minor.yy0) {
                while ( sequence_size(yymsp[0].minor.yy0) ) {
                        struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(yymsp[0].minor.yy0);
                        s->subject = rdf_term_copy(yymsp[-1].minor.yy0);

                        p->cb.statement(s);
                        rdf_statement_free(s);
                }
        }

        if (yymsp[-1].minor.yy0)
                rdf_term_free(yymsp[-1].minor.yy0);
        if (yymsp[0].minor.yy0)
                sequence_free(yymsp[0].minor.yy0);
}
#line 956 "./turtle_parser.c"
        break;
      case 17: /* predicateObjectList_astr ::= predicateObjectList */
      case 28: /* verb ::= predicate */ yytestcase(yyruleno==28);
      case 30: /* subject ::= iri */ yytestcase(yyruleno==30);
      case 31: /* subject ::= blankNode */ yytestcase(yyruleno==31);
      case 32: /* subject ::= collection */ yytestcase(yyruleno==32);
      case 33: /* predicate ::= iri */ yytestcase(yyruleno==33);
      case 34: /* object ::= iri */ yytestcase(yyruleno==34);
      case 35: /* object ::= blankNode */ yytestcase(yyruleno==35);
      case 36: /* object ::= collection */ yytestcase(yyruleno==36);
      case 37: /* object ::= blankNodePropertyList */ yytestcase(yyruleno==37);
      case 38: /* object ::= literal */ yytestcase(yyruleno==38);
      case 39: /* literal ::= rdfLiteral */ yytestcase(yyruleno==39);
      case 40: /* literal ::= numericalLiteral */ yytestcase(yyruleno==40);
      case 41: /* literal ::= booleanLiteral */ yytestcase(yyruleno==41);
      case 55: /* string ::= STRING_LITERAL */ yytestcase(yyruleno==55);
#line 181 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 975 "./turtle_parser.c"
        break;
      case 18: /* predicateObjectList_astr ::= */
      case 21: /* predicateObjectList_ast ::= */ yytestcase(yyruleno==21);
      case 23: /* predicateObjectList_qst ::= SEMICOLON */ yytestcase(yyruleno==23);
      case 27: /* objectList_ast ::= */ yytestcase(yyruleno==27);
#line 182 "./turtle_parser.y"
{ yygotominor.yy0 = NULL; }
#line 983 "./turtle_parser.c"
        break;
      case 19: /* predicateObjectList ::= verb objectList predicateObjectList_ast */
#line 185 "./turtle_parser.y"
{
        if (yymsp[-2].minor.yy0 && yymsp[-1].minor.yy0) {
                for (size_t i = 0; i < sequence_size(yymsp[-1].minor.yy0); i++) {
                        struct rdf_statement* t = (struct rdf_statement*)
                                                        sequence_get_at(yymsp[-1].minor.yy0, i);

                        t->predicate = rdf_term_copy(yymsp[-2].minor.yy0);
                }
                /* @TODO: double copy -> should be optimalized */
                if (yymsp[0].minor.yy0) {
                        while ( sequence_size(yymsp[0].minor.yy0) ) {
                                struct rdf_statement* t2 = (struct rdf_statement*)
                                                                sequence_unshift(yymsp[0].minor.yy0);
                                if (sequence_push(yymsp[-1].minor.yy0, t2)) {
                                        sequence_free(yymsp[-1].minor.yy0);
                                        sequence_free(yymsp[0].minor.yy0);
                                        rdf_term_free(yymsp[-2].minor.yy0);
                                        YYERROR;
                                }
                        }
                        sequence_free(yymsp[0].minor.yy0);
                }

                if (yymsp[-2].minor.yy0)
                        rdf_term_free(yymsp[-2].minor.yy0);

                yygotominor.yy0 = yymsp[-1].minor.yy0;
        }
        else {
                if (yymsp[0].minor.yy0)
                        yygotominor.yy0 = yymsp[0].minor.yy0;
                else
                        yygotominor.yy0 = NULL;
        }

}
#line 1023 "./turtle_parser.c"
        break;
      case 20: /* predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst */
#line 222 "./turtle_parser.y"
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
#line 1047 "./turtle_parser.c"
        break;
      case 22: /* predicateObjectList_qst ::= SEMICOLON verb objectList */
#line 244 "./turtle_parser.y"
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
#line 1064 "./turtle_parser.c"
        break;
      case 24: /* objectList ::= object objectList_ast */
#line 260 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[-1].minor.yy0, NULL);
        if (!yymsp[0].minor.yy0) {
                yygotominor.yy0 = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
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
#line 1083 "./turtle_parser.c"
        break;
      case 25: /* objectList_ast ::= objectList_ast COMMA object */
#line 275 "./turtle_parser.y"
{
        struct rdf_statement* triple = rdf_statement_from_nodes(NULL, NULL, yymsp[0].minor.yy0, NULL);
        if (!yymsp[-2].minor.yy0) {
                yygotominor.yy0 = sequence_create((sequence_free_handler) &rdf_statement_free, NULL, NULL);
                if (!yygotominor.yy0)
                        YYERROR;
        } else {
                yygotominor.yy0 = yymsp[-2].minor.yy0;
        }
        sequence_push(yygotominor.yy0, triple);
}
#line 1098 "./turtle_parser.c"
        break;
      case 26: /* objectList_ast ::= objectList_ast COMMA */
#line 289 "./turtle_parser.y"
{ /* Custom handling of ES */ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 1103 "./turtle_parser.c"
        break;
      case 29: /* verb ::= A */
#line 294 "./turtle_parser.y"
{
        utf8 s = string_create("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
        yygotominor.yy0 = rdf_term_from_uri(s);
        string_dealloc(s);
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1114 "./turtle_parser.c"
        break;
      case 42: /* blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE */
#line 324 "./turtle_parser.y"
{

        utf8 s = p->cb.generate_bnode();
        yygotominor.yy0 = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!yygotominor.yy0) {
                sequence_free(yymsp[-1].minor.yy0);
                yygotominor.yy0 = NULL;
                YYERROR;
        } else {
                if (yymsp[-1].minor.yy0) {
                        /* Handle non empty list */
                        while(sequence_size(yymsp[-1].minor.yy0)) {
                                struct rdf_statement* s = (struct rdf_statement*) sequence_unshift(yymsp[-1].minor.yy0);
                                s->subject = rdf_term_copy(yygotominor.yy0);

                                p->cb.statement(s);
                                rdf_statement_free(s);
                        }
                }
        }
}
#line 1141 "./turtle_parser.c"
        break;
      case 43: /* collection ::= L_CURLY collection_ast R_CURLY */
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
#line 1264 "./turtle_parser.c"
        break;
      case 44: /* collection_ast ::= collection_ast object */
#line 468 "./turtle_parser.y"
{
        if (!yymsp[-1].minor.yy0) {
                yymsp[-1].minor.yy0 = sequence_create((sequence_free_handler)&rdf_statement_free, NULL, NULL);
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
#line 1289 "./turtle_parser.c"
        break;
      case 45: /* collection_ast ::= */
#line 489 "./turtle_parser.y"
{ yygotominor.yy0 = NULL;  }
#line 1294 "./turtle_parser.c"
        break;
      case 46: /* numericalLiteral ::= INTEGER_LITERAL */
#line 493 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, string_create(rdfInteger));
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1304 "./turtle_parser.c"
        break;
      case 47: /* numericalLiteral ::= DECIMAL_LITERAL */
#line 499 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, string_create(rdfDecimal));
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1314 "./turtle_parser.c"
        break;
      case 48: /* numericalLiteral ::= DOUBLE_LITERAL */
#line 505 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, string_create(rdfDouble));
        string_dealloc(yymsp[0].minor.yy0);
        if (!yygotominor.yy0)
                YYERROR;
}
#line 1324 "./turtle_parser.c"
        break;
      case 49: /* rdfLiteral ::= string rdfLiteral_qst */
#line 513 "./turtle_parser.y"
{
        if (!yymsp[0].minor.yy8) {
                yygotominor.yy0 = rdf_term_from_literal(yymsp[-1].minor.yy0,NULL,NULL);
        } else {
                yygotominor.yy0 = rdf_term_from_literal(yymsp[-1].minor.yy0, yymsp[0].minor.yy8->langtag, yymsp[0].minor.yy8->datatype);
                string_dealloc(yymsp[0].minor.yy8->langtag);
                string_dealloc(yymsp[0].minor.yy8->datatype);
                free(yymsp[0].minor.yy8);
        }
        string_dealloc(yymsp[-1].minor.yy0);

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1342 "./turtle_parser.c"
        break;
      case 50: /* rdfLiteral_qst ::= rdfLiteral_opt */
#line 531 "./turtle_parser.y"
{yygotominor.yy8 = yymsp[0].minor.yy8;}
#line 1347 "./turtle_parser.c"
        break;
      case 51: /* rdfLiteral_qst ::= */
#line 532 "./turtle_parser.y"
{yygotominor.yy8 = NULL;}
#line 1352 "./turtle_parser.c"
        break;
      case 52: /* rdfLiteral_opt ::= LANGTAG */
#line 534 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                datatype_sys_t *s = calloc(1, sizeof(struct _datatype_sys_t));
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
#line 1371 "./turtle_parser.c"
        break;
      case 53: /* rdfLiteral_opt ::= HAT iri */
#line 549 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                datatype_sys_t *s = calloc(1, sizeof(struct _datatype_sys_t));
                if (s) {
                        struct rdf_term *t = yymsp[0].minor.yy0;
                        if (t->type == RDF_TERM_URI)
                                s->datatype = string_copy(t->value.uri);
                        else
                                s->datatype = NULL;
                        s->langtag = NULL;
                        yygotominor.yy8 = s;
                        rdf_term_free(yymsp[0].minor.yy0);
                } else {
                        rdf_term_free(yymsp[0].minor.yy0);
                        yygotominor.yy8 = NULL;
                }
        } else {
                yygotominor.yy8 = NULL;
        }
}
#line 1395 "./turtle_parser.c"
        break;
      case 54: /* booleanLiteral ::= BOOLEAN_LITERAL */
#line 571 "./turtle_parser.y"
{
        yygotominor.yy0 = rdf_term_from_literal(yymsp[0].minor.yy0, NULL, string_create(rdfBoolean));
        string_dealloc(yymsp[0].minor.yy0);

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1406 "./turtle_parser.c"
        break;
      case 56: /* iri ::= IRIREF */
#line 583 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                yygotominor.yy0 = rdf_term_from_uri(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);

                if (!yygotominor.yy0)
                        YYERROR;

        } else {
                yygotominor.yy0 = NULL;
        }
}
#line 1422 "./turtle_parser.c"
        break;
      case 57: /* iri ::= QNAME */
#line 595 "./turtle_parser.y"
{
        if (yymsp[0].minor.yy0) {
                yygotominor.yy0 = rdf_term_from_curie(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);

                if (!yygotominor.yy0)
                        YYERROR;
        } else {
                yygotominor.yy0 = NULL;
        }
}
#line 1437 "./turtle_parser.c"
        break;
      case 58: /* blankNode ::= BLANK_LITERAL */
#line 608 "./turtle_parser.y"
{
        if (!yymsp[0].minor.yy0) {
                utf8 s = p->cb.generate_bnode();
                yygotominor.yy0 = rdf_term_from_blank(s);
                string_dealloc(s);
        } else {
                yygotominor.yy0 = rdf_term_from_blank(yymsp[0].minor.yy0);
                string_dealloc(yymsp[0].minor.yy0);
        }

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1454 "./turtle_parser.c"
        break;
      case 59: /* blankNode ::= anon */
#line 621 "./turtle_parser.y"
{
        utf8 s = p->cb.generate_bnode();
        yygotominor.yy0 = rdf_term_from_blank(s);
        string_dealloc(s);

        if (!yygotominor.yy0)
                YYERROR;
}
#line 1466 "./turtle_parser.c"
        break;
      default:
      /* (0) turtleDoc ::= statements */ yytestcase(yyruleno==0);
      /* (1) statements ::= statements statement */ yytestcase(yyruleno==1);
      /* (2) statements ::= */ yytestcase(yyruleno==2);
      /* (4) statement ::= triples DOT */ yytestcase(yyruleno==4);
      /* (5) directive ::= prefixID */ yytestcase(yyruleno==5);
      /* (6) directive ::= base */ yytestcase(yyruleno==6);
      /* (7) directive ::= sparqlPrefix */ yytestcase(yyruleno==7);
      /* (8) directive ::= sparqlBase */ yytestcase(yyruleno==8);
      /* (60) anon ::= L_SQUARE R_SQUARE */ yytestcase(yyruleno==60);
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
#line 55 "./turtle_parser.y"

        fprintf(stderr, "Parser encountered error.\n"); /*parser->error.code = 1;*/
        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
#line 1527 "./turtle_parser.c"
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
#line 62 "./turtle_parser.y"

        p->stats.n_errors++;
        fprintf(stderr, "Syntax error: %s\n", NULL); /*parser->error.code = 2;*/
#line 1546 "./turtle_parser.c"
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
#line 59 "./turtle_parser.y"

        fprintf(stderr, "\n\nFinal status: (%lu) t, (%lu) d, (%lu) e\n", p->stats.n_triples, p->stats.n_directives, p->stats.n_errors);
#line 1568 "./turtle_parser.c"
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
