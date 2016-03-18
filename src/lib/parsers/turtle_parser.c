/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 13 "./turtle_parser.y"

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
#line 45 "./turtle_parser.y"

/*        struct _ardp_turtle_parser {
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
#line 61 "./turtle_parser.c"
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
#define ParseARG_SDECL
#define ParseARG_PDECL
#define ParseARG_FETCH
#define ParseARG_STORE
#define YYNSTATE 79
#define YYNRULE 59
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
 /*     0 */    51,   52,   27,   54,   55,   56,   57,    7,    3,    8,
 /*    10 */    26,   23,   28,   29,   75,   77,   61,   48,   49,   50,
 /*    20 */    25,   70,   60,   14,   40,   11,   15,   59,   45,   67,
 /*    30 */    68,   69,   71,   72,   73,   74,   70,   10,   65,    5,
 /*    40 */    45,   15,   38,   39,   67,   68,   69,   71,   72,   73,
 /*    50 */    74,   21,   10,   78,   70,   45,   12,   30,   31,   66,
 /*    60 */   139,    1,   67,   68,   69,   71,   72,   73,   74,   22,
 /*    70 */    10,   30,   31,   45,   41,   46,    9,   62,    4,    6,
 /*    80 */    20,   13,   33,   35,   36,   37,   70,   58,   42,   43,
 /*    90 */    44,   34,   53,   63,   67,   68,   69,   71,   72,   73,
 /*   100 */    74,  140,   10,   30,   31,   45,  140,   30,   31,  140,
 /*   110 */   140,    6,  140,   13,   76,   35,   36,   37,  140,  140,
 /*   120 */    42,   43,   44,   79,  140,   17,  140,   30,   31,   18,
 /*   130 */    24,   16,   30,   31,  140,    6,   19,   13,    2,   76,
 /*   140 */    47,   32,    2,  140,   75,   77,   44,  140,   75,   77,
 /*   150 */   140,   64,  140,    2,  140,  140,  140,  140,  140,   75,
 /*   160 */    77,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    27,   28,   29,   30,   31,   32,   33,   34,   37,   36,
 /*    10 */     3,    3,    5,    5,   43,   44,    4,   44,   45,   46,
 /*    20 */     4,   36,    1,   38,   19,   20,   41,    1,   55,   44,
 /*    30 */    45,   46,   47,   48,   49,   50,   36,   52,   38,   10,
 /*    40 */    55,   41,   53,   54,   44,   45,   46,   47,   48,   49,
 /*    50 */    50,    4,   52,   13,   36,   55,   39,    4,    5,   41,
 /*    60 */    25,   26,   44,   45,   46,   47,   48,   49,   50,    4,
 /*    70 */    52,    4,    5,   55,   44,    9,   40,    4,   51,   12,
 /*    80 */    42,   14,   15,   16,   17,   18,   36,    1,   21,   22,
 /*    90 */    23,   41,    1,    4,   44,   45,   46,   47,   48,   49,
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
 /*    10 */     5,   53,   66,   -1,   -1,   -1,    7,    8,   65,   40,
 /*    20 */    29,   26,   21,   16,   12,   86,   73,   91,   89,   47,
};
#define YY_REDUCE_USE_DFLT (-30)
#define YY_REDUCE_MAX 15
static const signed char yy_reduce_ofst[] = {
 /*     0 */    35,  -27,  -15,    0,   50,   18,  101,  116,  105,  -29,
 /*    10 */   -11,   30,   36,   27,   17,   38,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    81,  138,  138,  138,  138,  138,  138,  138,   96,   98,
 /*    10 */   128,  138,   97,  122,   99,  104,  138,  138,  138,  138,
 /*    20 */   102,  138,  138,  138,  138,  138,  138,  138,  138,  138,
 /*    30 */   133,  134,  137,  120,  121,  123,  124,  125,  126,  127,
 /*    40 */   129,  130,  131,  132,  135,  136,  101,   95,  107,  108,
 /*    50 */   109,   80,   82,   83,   84,   85,   86,   87,   88,   89,
 /*    60 */    90,   91,   92,   93,   94,  100,  103,  111,  112,  113,
 /*    70 */   114,  115,  116,  117,  118,  105,  106,  110,  119,
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
 /*  21 */ "predicateObjectList_qst ::= predicateObjectList_qst verb objectList",
 /*  22 */ "predicateObjectList_qst ::= SEMICOLON",
 /*  23 */ "objectList ::= object objectList_ast",
 /*  24 */ "objectList_ast ::= objectList_ast COMMA object",
 /*  25 */ "objectList_ast ::=",
 /*  26 */ "verb ::= predicate",
 /*  27 */ "verb ::= A",
 /*  28 */ "subject ::= iri",
 /*  29 */ "subject ::= blankNode",
 /*  30 */ "subject ::= collection",
 /*  31 */ "predicate ::= iri",
 /*  32 */ "object ::= iri",
 /*  33 */ "object ::= blankNode",
 /*  34 */ "object ::= collection",
 /*  35 */ "object ::= blankNodePropertyList",
 /*  36 */ "object ::= literal",
 /*  37 */ "literal ::= rdfLiteral",
 /*  38 */ "literal ::= numericalLiteral",
 /*  39 */ "literal ::= booleanLiteral",
 /*  40 */ "blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE",
 /*  41 */ "collection ::= L_CURLY collection_ast R_CURLY",
 /*  42 */ "collection_ast ::= collection_ast object",
 /*  43 */ "collection_ast ::=",
 /*  44 */ "numericalLiteral ::= INTEGER_LITERAL",
 /*  45 */ "numericalLiteral ::= DECIMAL_LITERAL",
 /*  46 */ "numericalLiteral ::= DOUBLE_LITERAL",
 /*  47 */ "rdfLiteral ::= string rdfLiteral_qst",
 /*  48 */ "rdfLiteral_qst ::= rdfLiteral_opt",
 /*  49 */ "rdfLiteral_qst ::=",
 /*  50 */ "rdfLiteral_opt ::= LANGTAG",
 /*  51 */ "rdfLiteral_opt ::= HAT iri",
 /*  52 */ "booleanLiteral ::= BOOLEAN_LITERAL",
 /*  53 */ "string ::= STRING_LITERAL",
 /*  54 */ "iri ::= IRIREF",
 /*  55 */ "iri ::= QNAME",
 /*  56 */ "blankNode ::= BLANK_LITERAL",
 /*  57 */ "blankNode ::= anon",
 /*  58 */ "anon ::= L_SQUARE R_SQUARE",
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
#line 103 "./turtle_parser.y"
{
                fprintf(stdout, "prefix for universal ':' for %s\n", yymsp[-1].minor.yy0);
           }
#line 851 "./turtle_parser.c"
        break;
      case 10: /* prefixID ::= PREFIX QNAME IRIREF DOT */
#line 106 "./turtle_parser.y"
{
                fprintf(stdout, "prefix for: %s, uri: %s\n", yymsp[-2].minor.yy0, yymsp[-1].minor.yy0);
           }
#line 858 "./turtle_parser.c"
        break;
      case 11: /* base ::= BASE IRIREF DOT */
#line 111 "./turtle_parser.y"
{
                fprintf(stdout, "base: %s", yymsp[-1].minor.yy0);
       }
#line 865 "./turtle_parser.c"
        break;
      case 12: /* sparqlBase ::= SPARQL_BASE IRIREF */
#line 116 "./turtle_parser.y"
{
                fprintf(stdout, "BASE_SPARQL ::= %s\n", yymsp[0].minor.yy0);
             }
#line 872 "./turtle_parser.c"
        break;
      case 13: /* sparqlPrefix ::= SPARQL_PREFIX COLON IRIREF */
#line 121 "./turtle_parser.y"
{
                        fprintf(stdout, "SPARQL prefix s: ':', uri: %s\n", yymsp[0].minor.yy0);
                        // parser->addPrefix(":",yymsp[0].minor.yy0);
               }
#line 880 "./turtle_parser.c"
        break;
      case 14: /* sparqlPrefix ::= SPARQL_PREFIX QNAME IRIREF */
#line 125 "./turtle_parser.y"
{
                        fprintf(stdout, "SPARQL prefix s: %s, uri: %s\n", yymsp[-1].minor.yy0, yymsp[0].minor.yy0);
                        // parser->addPrefix(yymsp[-1].minor.yy0,yymsp[0].minor.yy0);
               }
#line 888 "./turtle_parser.c"
        break;
      case 15: /* triples ::= subject predicateObjectList */
#line 131 "./turtle_parser.y"
{
                fprintf(stdout, "[triple] with subject:%s\n", yymsp[-1].minor.yy0);
          }
#line 895 "./turtle_parser.c"
        break;
      case 26: /* verb ::= predicate */
#line 152 "./turtle_parser.y"
{
            yygotominor.yy0 = yymsp[0].minor.yy0;
            fprintf(stdout, "[verb:predicate] == %s\n", yymsp[0].minor.yy0);
       }
#line 903 "./turtle_parser.c"
        break;
      case 27: /* verb ::= A */
#line 156 "./turtle_parser.y"
{
            yygotominor.yy0 = "http://www.w3c.org/RDF/1999/#type";
            fprintf(stdout, "[verb:a] == ...#type\n");
       }
#line 911 "./turtle_parser.c"
        break;
      case 28: /* subject ::= iri */
#line 162 "./turtle_parser.y"
{
                yygotominor.yy0 = yymsp[0].minor.yy0;
                fprintf(stdout, "[subject:iri] == %s\n", yymsp[0].minor.yy0);
          }
#line 919 "./turtle_parser.c"
        break;
      case 29: /* subject ::= blankNode */
#line 166 "./turtle_parser.y"
{
                yygotominor.yy0 = yymsp[0].minor.yy0;
                fprintf(stdout, "[subject:bnode] == %s\n", yymsp[0].minor.yy0);
          }
#line 927 "./turtle_parser.c"
        break;
      case 30: /* subject ::= collection */
#line 170 "./turtle_parser.y"
{
                yygotominor.yy0 = yymsp[0].minor.yy0;
                fprintf(stdout, "[subject:collection] == %s\n", yymsp[0].minor.yy0);
          }
#line 935 "./turtle_parser.c"
        break;
      case 31: /* predicate ::= iri */
#line 176 "./turtle_parser.y"
{
                yygotominor.yy0 = yymsp[0].minor.yy0;
                fprintf(stdout, "[predicate:iri] == %s\n", yygotominor.yy0);
            }
#line 943 "./turtle_parser.c"
        break;
      case 32: /* object ::= iri */
      case 33: /* object ::= blankNode */ yytestcase(yyruleno==33);
      case 34: /* object ::= collection */ yytestcase(yyruleno==34);
      case 35: /* object ::= blankNodePropertyList */ yytestcase(yyruleno==35);
      case 36: /* object ::= literal */ yytestcase(yyruleno==36);
      case 37: /* literal ::= rdfLiteral */ yytestcase(yyruleno==37);
      case 39: /* literal ::= booleanLiteral */ yytestcase(yyruleno==39);
      case 44: /* numericalLiteral ::= INTEGER_LITERAL */ yytestcase(yyruleno==44);
      case 45: /* numericalLiteral ::= DECIMAL_LITERAL */ yytestcase(yyruleno==45);
      case 46: /* numericalLiteral ::= DOUBLE_LITERAL */ yytestcase(yyruleno==46);
      case 52: /* booleanLiteral ::= BOOLEAN_LITERAL */ yytestcase(yyruleno==52);
      case 54: /* iri ::= IRIREF */ yytestcase(yyruleno==54);
      case 55: /* iri ::= QNAME */ yytestcase(yyruleno==55);
      case 56: /* blankNode ::= BLANK_LITERAL */ yytestcase(yyruleno==56);
      case 57: /* blankNode ::= anon */ yytestcase(yyruleno==57);
#line 182 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 962 "./turtle_parser.c"
        break;
      case 38: /* literal ::= numericalLiteral */
#line 191 "./turtle_parser.y"
{yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 967 "./turtle_parser.c"
        break;
      case 40: /* blankNodePropertyList ::= L_SQUARE predicateObjectList R_SQUARE */
#line 195 "./turtle_parser.y"
{ yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 972 "./turtle_parser.c"
        break;
      case 47: /* rdfLiteral ::= string rdfLiteral_qst */
#line 209 "./turtle_parser.y"
{yygotominor.yy0 = yymsp[-1].minor.yy0; }
#line 977 "./turtle_parser.c"
        break;
      case 53: /* string ::= STRING_LITERAL */
#line 219 "./turtle_parser.y"
{yygotominor.yy0 = yymsp[0].minor.yy0;}
#line 982 "./turtle_parser.c"
        break;
      case 58: /* anon ::= L_SQUARE R_SQUARE */
#line 230 "./turtle_parser.y"
{ yygotominor.yy0 = "ANON"; }
#line 987 "./turtle_parser.c"
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
      /* (16) triples ::= blankNodePropertyList predicateObjectList */ yytestcase(yyruleno==16);
      /* (17) triples ::= blankNodePropertyList */ yytestcase(yyruleno==17);
      /* (18) predicateObjectList ::= verb objectList predicateObjectList_ast */ yytestcase(yyruleno==18);
      /* (19) predicateObjectList_ast ::= predicateObjectList_ast predicateObjectList_qst */ yytestcase(yyruleno==19);
      /* (20) predicateObjectList_ast ::= */ yytestcase(yyruleno==20);
      /* (21) predicateObjectList_qst ::= predicateObjectList_qst verb objectList */ yytestcase(yyruleno==21);
      /* (22) predicateObjectList_qst ::= SEMICOLON */ yytestcase(yyruleno==22);
      /* (23) objectList ::= object objectList_ast */ yytestcase(yyruleno==23);
      /* (24) objectList_ast ::= objectList_ast COMMA object */ yytestcase(yyruleno==24);
      /* (25) objectList_ast ::= */ yytestcase(yyruleno==25);
      /* (41) collection ::= L_CURLY collection_ast R_CURLY */ yytestcase(yyruleno==41);
      /* (42) collection_ast ::= collection_ast object */ yytestcase(yyruleno==42);
      /* (43) collection_ast ::= */ yytestcase(yyruleno==43);
      /* (48) rdfLiteral_qst ::= rdfLiteral_opt */ yytestcase(yyruleno==48);
      /* (49) rdfLiteral_qst ::= */ yytestcase(yyruleno==49);
      /* (50) rdfLiteral_opt ::= LANGTAG */ yytestcase(yyruleno==50);
      /* (51) rdfLiteral_opt ::= HAT iri */ yytestcase(yyruleno==51);
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
#line 27 "./turtle_parser.y"
 fprintf(stderr, "Parser failure\n"); /*parser->error.code = 1;*/ 
#line 1063 "./turtle_parser.c"
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
#line 28 "./turtle_parser.y"
 fprintf(stderr, "Syntax error\n"); /*parser->error.code = 2;*/ 
#line 1080 "./turtle_parser.c"
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
