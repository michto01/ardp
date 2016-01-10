#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <errno.h>
#include <iso646.h>

#include <ardp/util.h>
#include <ardp/parser.h>
#include <ardp/parser_internal.h>
#include <ardp/color.h>

#define BUFSIZE 0x10000

static unsigned int parse_hex( const uint8_t *src, unsigned int len ) {
    unsigned int i;
    unsigned int j;

    for ( i = j = 0; j < len; j++ ) {
        i *= 16;
        char c = src[j];
        if ( isxdigit(c) ) {
            if ( isdigit(c) ) {
                i += c - '0';
            } else {
                i += toupper(c) - 'A' + 10;
            }
        }
    }
    return i;
}

static void emit( ardp_parser *parser, ardp_token_type type ) {
        parser->handler( type, parser->string, parser->handler_arg );
}

/**
  * Main definition for the N-triples parser.
  */
%%{
    machine ntriples;
    include rdf_grammar_common "grammars/rdf.grammar.common.rl";

    access parser->;

    alphtype unsigned char;

    action mark {
        mark = p;
    }

    action unescape {
        mark += 2; // we have two prior characters '\' + /'U'/i
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 )
            parser->finished = 1;
    }

    action unescape_char {
       const unsigned char c = *(mark + 1);
       mark = 0;
       switch ( c ) {
         case 't':  string_append_char( &parser->string, '\t'); break;
         case 'b':  string_append_char( &parser->string, '\b'); break;
         case 'n':  string_append_char( &parser->string, '\n'); break;
         case 'r':  string_append_char( &parser->string, '\r'); break;
         case 'f':  string_append_char( &parser->string, '\f'); break;
         case '\"': string_append_char( &parser->string, '\"'); break;
         case '\'': string_append_char( &parser->string, '\''); break;
         case '\\': string_append_char( &parser->string, '\\'); break;
         default: break;
       }
    }

    action putChar {
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }

    action endTriple {
        // TODO: call handler?
    }

    action endLine {
        parser->line++;
    }

    action startString {
        if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }

    action endSimpleLiteral   { emit(parser, ARDP_SIMPLE_LITERAL_VALUE); }
    action endDatatypeLiteral { emit(parser, ARDP_DATATYPE_LITERAL_IRI); }
    action datatypeLiteral    { emit(parser, ARDP_DATATYPE_LITERAL_VALUE); }
    action langtagLiteral     { emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_VALUE); }
    action endLangtagLiteral  { emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }
    action endIRI             { emit(parser, ARDP_IRI); }
    action endBlankNode       { emit(parser, ARDP_BLANK_NODE); }

    PN_CHARS_BASE    = UNICODE $putChar;
    PN_CHARS_U       = PN_CHARS_BASE | ('_' | ':') $putChar;
    PN_CHARS         = PN_CHARS_U    | ( _PN_CHARS ) $putChar;
    ECHAR            = _ECHAR >mark %unescape_char;
    UCHAR            = _UCHAR >mark %unescape;


    scheme           = (ALPHA (ALPHA | DIGIT | '+' | '-' | '.')*) $putChar;
    STRING_VALUE     = (^(0x22 | 0x5C | 0xA | 0xD) $putChar | ECHAR | UCHAR)* >startString;
    IRIREF_VALUE     = scheme >startString ':' $putChar (^(0x00 .. 0x20 | '<' | '>' | '"' | '{' | '}' | '|' | '^' | '`' | '\\') $putChar | UCHAR)*;
    IRIREF           = '<' IRIREF_VALUE '>';
    BLANK_NODE_LABEL = '_:' >startString (PN_CHARS_U | DIGIT $putChar) ((PN_CHARS | '.' $putChar)* PN_CHARS)?;
    LANGTAG_VALUE    = ALPHA+ $putChar >startString ('-' $putChar ((ALPHA | DIGIT) $putChar)+)*;

    datatypeLiteral  = '"' STRING_VALUE '"' '^^' >datatypeLiteral     IRIREF    %endDatatypeLiteral;
    langtagLiteral   = '"' STRING_VALUE '"' '@'  >langtagLiteral  LANGTAG_VALUE %endLangtagLiteral;
    simpleLiteral    = '"' STRING_VALUE '"' %endSimpleLiteral;

    literal          = datatypeLiteral | langtagLiteral | simpleLiteral;
    iri              = IRIREF %endIRI;
    blankNode        = BLANK_NODE_LABEL %endBlankNode;
    subject          = iri | blankNode;
    predicate        = iri;
    object           = literal | iri | blankNode;
    triple           = subject WS* predicate WS* object WS* :>> '.' WS* %endTriple;

    comment          = '#' (any -- EOL)*;
    line             = WS* triple? comment? (EOL @endLine)+;

    skip_line := (any -- EOL)* (EOL @endLine) @{ fgoto main; };

    main := line* $err{
        ardp_fprintf(stderr, ARDP_COLOR_RED, "[ERROR]:");
        ardp_fprintf_ln(stderr, ARDP_COLOR_BOLD, " on line: %d\n", parser->line);
        fhold; fgoto skip_line;
    };

    write data;
}%%

ardp_parser* ardp_new_parser( void ) {
    ardp_parser *parser              = malloc(sizeof(ardp_parser));
                  parser->line        = 1;
                  parser->finished    = 0;
                  parser->string      = NULL;
                  parser->reader      = NULL;
                  parser->reader_arg  = NULL;
                  parser->handler     = NULL;
                  parser->handler_arg = NULL;
    return parser;
}

void ardp_free_parser( ardp_parser *parser ) {
    if ( parser->string isnt NULL )
        string_dealloc( parser->string );
    free(parser);
}

void ardp_parser_set_reader( ardp_parser *parser, ardp_reader reader, void *reader_arg ) {
    parser->reader     = reader;
    parser->reader_arg = reader_arg;
}

void ardp_parser_set_handler( ardp_parser *parser, ardp_handler handler, void *handler_arg ) {
    parser->handler     = handler;
    parser->handler_arg = handler_arg;
}

void ardp_parser_init( ardp_parser * _Nonnull parser ) {
    %% write init;
}

void ardp_parser_parse_block( ardp_parser * _Nonnull  parser,
                              uint8_t     * _Nullable p,
                              size_t        len,
                              uint8_t     * _Nullable mark,
                              bool          is_eof ) {
    /* pe points to 1 byte beyond the end of this block of data */
    const uint8_t *pe  = p + len;
    /* Indicates the end of all data, 0 if not in this block */
    const uint8_t *eof = (is_eof) ? pe : ((uint8_t*) 0);
    %% write exec;
}

int ardp_parser_parse( ardp_parser * _Nonnull parser ) {

    if( parser->reader is NULL )
        return 0;

    ardp_parser_init( parser );

    int status = 1;

    uint8_t    buf[BUFSIZE];
    size_t     have = 0;
    uint8_t   *mark = NULL;
    bool       eof  = false;

    while ( not parser->finished ) {
        uint8_t *p = buf + have;

        size_t space = BUFSIZE - have;
        if ( space <= 0 ) {
            /* Out of buffer space */
            status = 0;
            break; /* goto exit */
        }

        int len = parser->reader( p, space, parser->reader_arg );

        if ( len < space ) {
            eof = true;
            parser->finished = 1;
        }

        ardp_parser_parse_block( parser, p, len, mark, eof );

        if ( parser->cs is ntriples_error ) {
              status = 0;
              break; /* goto exit */
        } else if ( mark ) {
              have = (p + len) - mark;
              memmove( buf, mark, have );
              mark = buf;
        } else {
              have = 0;
        }
    }

    return status;
}
