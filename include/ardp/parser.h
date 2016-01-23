#ifndef __ARDP_PARSER_H__
#define __ARDP_PARSER_H__


#if __has_feature( nullability )
#else
#define _Nullable
#define _Nonnull
#define _Null_unspecified
#endif

#include <ardp/string.h>

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

typedef enum __attribute__( ( flag_enum ) ) {
        ARDP_IRI,
        ARDP_BLANK_NODE,
        ARDP_SIMPLE_LITERAL_VALUE,
        ARDP_LANGUAGE_TAGGED_LITERAL_VALUE,
        ARDP_DATATYPE_LITERAL_VALUE,
        ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE,
        ARDP_DATATYPE_LITERAL_IRI,
} ardp_token_type;

typedef struct ardp_parser_s ardp_parser;

ardp_parser *_Nullable ardp_new_parser();

void ardp_free_parser( ardp_parser *_Nullable parser );

int ardp_parser_parse( ardp_parser *_Nonnull parser );

typedef int ( *_Nullable ardp_reader )( unsigned char *_Nonnull buffer,
                                        unsigned len,
                                        void *_Nullable arg );
typedef void ( *_Nullable ardp_handler )( ardp_token_type type,
                                          utf8 _Nullable s,
                                          void *_Nullable arg );
typedef void ( *_Nullable ardp_error )( const char *_Nonnull buffer, size_t len, size_t line );

void ardp_parser_set_reader( ardp_parser *_Nonnull parser,
                             ardp_reader reader,
                             void *_Nullable reader_arg );
void ardp_parser_set_handler( ardp_parser *_Nonnull parser,
                              ardp_handler handler,
                              void *_Nullable handler_arg );


#endif /* __ARDP_PARSER_H__ */
