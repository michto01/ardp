#ifndef __ARDP_PARSER_H__
#define __ARDP_PARSER_H__

#include <ardp/string.h>

typedef struct ardp_parser_s ardp_parser;

ardp_parser *ardp_new_parser();

void ardp_free_parser(ardp_parser *parser);

int ardp_parser_parse(ardp_parser *parser);

typedef int (*ardp_reader)(unsigned char *buffer, unsigned len, void *arg);

void ardp_parser_set_reader(ardp_parser *parser,
                             ardp_reader reader,
                             void *reader_arg);

/*

Grammar for the triples parser:

TRIPLE = SUBJECT PREDICATE OBJECT

SUBJECT = ARDP_IRI
        | ARDP_BLANK_NODE

PREDICATE = ARDP_IRI

OBJECT = ARDP_IRI
       | ARDP_BLANK_NODE
       | ARDP_SIMPLE_LITERAL_VALUE
       | ARDP_LANGUAGE_TAGGED_LITERAL_VALUE ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE
       | ARDP_DATATYPE_LITERAL_VALUE ARDP_DATATYPE_LITERAL_IRI

*/

typedef enum {
    ARDP_IRI,
    ARDP_BLANK_NODE,
    ARDP_SIMPLE_LITERAL_VALUE,
    ARDP_LANGUAGE_TAGGED_LITERAL_VALUE,
    ARDP_DATATYPE_LITERAL_VALUE,
    ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE,
    ARDP_DATATYPE_LITERAL_IRI,
} ardp_token_type;

typedef void (*ardp_handler)(ardp_token_type type, utf8 s, void *arg);

void ardp_parser_set_handler(ardp_parser *parser,
                              ardp_handler handler,
                              void *handler_arg);


#endif /* __ARDP_PARSER_H__ */
