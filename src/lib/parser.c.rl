#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <ardp/util.h>
#include <ardp/parser_internal.h>
#include <ardp/parser.h>



#define BUFSIZE 0x10000

// TODO:
// - error handling, also when string methods fail

static unsigned int parse_hex( const uint8_t *src, unsigned int len ) {
    unsigned int i = 0;

    for ( unsigned int j = 0; j < len; j++ ) {
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

%%{
    machine ntriples;

    alphtype unsigned char;

    action mark {
        mark = p;
    }

    action unescape {
        unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }

    action endTriple {
        // TODO: call handler?
    }

    action endLine {
        parser->line++;
    }

    action putChar {
        if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
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
    action datatypeLiteral    { emit(parser, ARDP_DATATYPE_LITERAL_VALUE); }
    action langtagLiteral     { emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_VALUE); }
    action endLangtagLiteral  { emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }
    action endDatatypeLiteral { emit(parser, ARDP_DATATYPE_LITERAL_IRI); }
    action endIRI             { emit(parser, ARDP_IRI); }
    action endBlankNode       { emit(parser, ARDP_BLANK_NODE); }

    WS    = ' '  | '\t';
    EOL   = '\r' | '\n';
    ALPHA = 'A' .. 'Z' | 'a' .. 'z';
    DIGIT = '0' .. '9';
    HEX   = DIGIT | 'A' .. 'F' | 'a' .. 'f';

    PN_CHARS_BASE = (
                  ALPHA
                     # The RDF and Turtle spec specifies Unicode codepoints,
                     # but files are encoded (and parsed) as UTF-8:
    # 0x00C0 .. 0x00D6
                  | 0xC3 0x80..0x96
    # 0x00D8 .. 0x00F6
                  | 0xC3 0x98..0xB6
    # 0x00F8 .. 0x02FF
                  | 0xC3 0xB8..0xFF
                  | 0xC4..0xCA 0x00..0xFF
                  | 0xCB 0x00..0xBF
    # 0x0370 .. 0x037D
                  | 0xCD 0xB0..0xBD
    # 0x037F .. 0x1FFF = 0x037F .. 0x07FF | 0x0800 .. 0x1FFF
                  | 0xCD 0xBF..0xFF
                  | 0xCE..0xDE 0x00..0xFF
                  | 0xDF 0x00..0xBF
                  | 0xE0 0xA0 0x80..0xFF
                  | 0xE0 0xA1..0xFF 0x00..0xFF
                  | 0xE1 0x00 0x00..0xFF
                  | 0xE1 0x01..0xBE 0x00..0xFF
                  | 0xE1 0xBF 0x00..0xBF
    # 0x200C .. 0x200D
                  | 0xE2 0x80 0x8C..0x8D
    # 0x2070 .. 0x218F
                  | 0xE2 0x81 0xB0..0xFF
                  | 0xE2 0x82..0x85 0x00..0xFF
                  | 0xE2 0x86 0x00..0x8F
    # 0x2C00 .. 0x2FEF
                  | 0xE2 0xB0 0x80..0xFF
                  | 0xE2 0xB1..0xBE 0x00..0xFF
                  | 0xE2 0xBF 0x00..0xAF
    # 0x3001 .. 0xD7FF
                  | 0xE3 0x80 0x81..0xFF
                  | 0xE3 0x81..0xFF 0x00..0xFF
                  | 0xE4..0xEC (0x00..0xFF){2} #0x00..0xFF
                  | 0xED 0x00 0x00..0xFF
                  | 0xED 0x01..0x9E 0x00..0xFF
                  | 0xED 0x9F 0x00..0xBF
    # 0xF900 .. 0xFDCF
                  | 0xEF 0xA4 0x80..0xFF
                  | 0xEF 0xA5..0xB6 0x00..0xFF
                  | 0xEF 0xB7 0x00..0x8F
    # 0xFDF0 .. 0xFFFD
                  | 0xEF 0xB7 0xB0..0xFF
                  | 0xEF 0xB8..0xBE 0x00..0xFF
                  | 0xEF 0xBF 0x00..0xBD
    # 0x10000 .. 0xEFFFF
                  | 0xF0 0x90 0x80 0x80..0xFF
                  | 0xF0 0x90 0x81..0xFF 0x00..0xFF
                  | 0xF0 0x91..0xFF (0x00..0xFF){2}  #0x00..0xFF
                  | 0xF1..0xF2 (0x00..0xFF){3}       #0x00..0xFF 0x00..0xFF
                  | 0xF3 0x00 0x00 0x00..0xFF
                  | 0xF3 0x00 0x01..0xFF 0x00..0xFF
                  | 0xF3 0x01..0xAE (0x00..0xFF){2}  #0x00..0xFF
                  | 0xF3 0xAF 0x00 0x00..0xFF
                  | 0xF3 0xAF 0x01..0xBE 0x00..0xFF
                  | 0xF3 0xAF 0xBF 0x00..0xBF
                  ) $putChar;

    PN_CHARS_U = PN_CHARS_BASE | ('_' | ':') $putChar;
    PN_CHARS   = PN_CHARS_U
               | ( '-' | DIGIT | 0x00B7
                  | 0xCC 0x80..0xFF      | 0xCD 0x00..0xAF        # 0x0300 .. 0x036F
                  | 0xE2 0x80 0xBF..0xFF | 0xE2 0x81 0x00..0x80   # 0x203F .. 0x2040
               ) $putChar;

    ECHAR  = '\\' .
            (  ('t'  ${ string_append_char( &parser->string, '\t'); })
             | ('b'  ${ string_append_char( &parser->string, '\b'); })
             | ('n'  ${ string_append_char( &parser->string, '\n'); })
             | ('r'  ${ string_append_char( &parser->string, '\r'); })
             | ('f'  ${ string_append_char( &parser->string, '\f'); })
             | ('"'  ${ string_append_char( &parser->string, '\"'); })
             | ('\'' ${ string_append_char( &parser->string, '\''); })
             | ('\\' ${ string_append_char( &parser->string, '\\'); })
             );

    UCHAR = '\\' . ( ('u' HEX{4} >mark %unescape) | ('U' HEX{8} >mark %unescape) );

    STRING_VALUE     = (^(0x22 | 0x5C | 0xA | 0xD) $putChar | ECHAR | UCHAR)* >startString;
    scheme           = (ALPHA (ALPHA | DIGIT | '+' | '-' | '.')*) $putChar;
    IRIREF_VALUE     = scheme >startString ':' $putChar (^(0x00 .. 0x20 | '<' | '>' | '"' | '{' | '}' | '|' | '^' | '`' | '\\') $putChar | UCHAR)*;
    IRIREF           = '<' IRIREF_VALUE '>';
    BLANK_NODE_LABEL = '_:' >startString (PN_CHARS_U | DIGIT $putChar) ((PN_CHARS | '.' $putChar)* PN_CHARS)?;

    LANGTAG_VALUE    = ALPHA+ $putChar >startString ('-' $putChar ((ALPHA | DIGIT) $putChar)+)*;
    datatypeLiteral  = '"' STRING_VALUE '"' '^^' >datatypeLiteral IRIREF %endDatatypeLiteral;
    langtagLiteral   = '"' STRING_VALUE '"' '@' >langtagLiteral LANGTAG_VALUE %endLangtagLiteral;
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

    skip_line := (any -- EOL)* (EOL @endLine) @{
        fgoto main;
    };

    main := line* $err{
        // TODO: failed on line parser->line
        printf(_CLR_RED "[ERROR]" _CLR_RESET " on line: %d\n", parser->line );
//        parser_error_callback(parser->line);
        fhold; fgoto skip_line;
    };

    write data;
}%%

ardp_parser *ardp_new_parser( void ) {
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

//typedef unsigned char uchar;

int ardp_parser_parse(ardp_parser *parser) {
    if ( parser->reader is NULL or parser->handler is NULL )
        return 0;

    int cs;

    %% write init;

    uint8_t buf[BUFSIZE];
    uint8_t *mark = 0;

    int have = 0;
    int res  = 1;

    ardp_reader reader = parser->reader;

    while ( parser->finished is 0 ) {
        uint8_t *pe;
        uint8_t *p   = buf + have;
        uint8_t *eof = 0;

        int space = BUFSIZE - have;
        if ( space is 0 ) {
            // out of buffer space
            res = 0;
            break;
        }

        int len = reader( p, space, parser->reader_arg );
        pe = p + len;

        if ( len < space ) {
            eof = pe;
            parser->finished = 1;
        }

        %% write exec;

        if ( cs is ntriples_error ) {
            res = 0;
            break;
        } else if (mark) {
            have = pe - mark;
            memmove( buf, mark, have );
            mark = buf;
        } else {
            have = 0;
        }
    }

    return res;
}
