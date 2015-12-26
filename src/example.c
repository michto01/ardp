#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <zlib.h>
#include <bzlib.h>

//#include <libdispatch.h>

#include "parser.h"
#include "string.h"
#include "util.h"

typedef enum {
    STATE_SUBJECT,
    STATE_PREDICATE,
    STATE_OBJECT,
    STATE_EXTRA
} parser_state;

parser_state state = STATE_SUBJECT;
int triples = 0;

int read_gzip(unsigned char* p, unsigned int len, void *arg) {
    gzFile file = arg;
    return gzread(file, p, len);
}

int read_bzip(unsigned char* p, unsigned int len, void *arg) {
    BZFILE *file = arg;
    return BZ2_bzread(file, p, len);
}

void handler(entee_token_type type, utf8 s, void *arg) {

    switch (state) {
        case STATE_SUBJECT: {
            printf(CLR_RED "{\n" CLR_GREEN  "\tsubject:  " CLR_RESET  " %s\n", s);
            state = STATE_PREDICATE;
            triples += 1;
            goto cleanup;
        }
        case STATE_PREDICATE: {
            printf(CLR_GREEN "\tpredicate:" CLR_YELLOW  " %s\n" CLR_RESET, s);
            state = STATE_OBJECT;
            goto cleanup;
        }
        case STATE_OBJECT: {
            printf(CLR_GREEN "\tobject:   " CLR_CYAN " %s\n" CLR_RED "},\n" CLR_RESET, s);
            switch (type) {
                case ENTEE_LANGUAGE_TAGGED_LITERAL_VALUE:
                case ENTEE_DATATYPE_LITERAL_VALUE: {
                    state = STATE_EXTRA;
                    goto cleanup;
                }
                default: {
                    state = STATE_SUBJECT;
                    goto cleanup;
                }
            }
            goto cleanup;
        }
        case STATE_EXTRA: {
            printf(CLR_CYAN "+" CLR_MAGENTA "...: %s\n" CLR_RESET, s);
            state = STATE_SUBJECT;
            goto cleanup;
        }
    }

cleanup:
    //string_dealloc(s);
    return;
}

extern void test_string(void);

int main( int argc, char **argv ) {

    //test_string();

    test_string();

    char* filename = NULL;

    bool isBzip = false;

    // parse options
    int c;
    while ( (c = getopt(argc, argv, "bf:")) isnt -1 ) {
        switch ( c ) {
            case 'f':
                filename = optarg;
                break;
            case 'b':
                isBzip = true;
                break;

            default:
                printf("Error in parameters.");
                return EXIT_FAILURE;
        }
    }

    if ( not filename )
        return EXIT_FAILURE;


    // open
    void *file = ( isBzip ? BZ2_bzopen(filename, "rb") : gzopen(filename, "rb") );

    if ( !file ) {
        fprintf(stderr, "Unable to open file \"%s\"", filename);
        return EXIT_FAILURE;
    }

    // parse
    entee_parser *parser = entee_new_parser();

    entee_reader reader  = isBzip ? read_bzip : read_gzip;

    entee_parser_set_reader  (parser, reader, file);
    entee_parser_set_handler (parser, handler, NULL);

//    entee_parser_set_handler(parser, ^void(entee_token_type type, const char *s, void *arg){
//        switch ( type ) {
//            case STATE_SUBJECT:
//                printf("Hello");
//                break;
//
//            default:
//                break;
//        }
//    }, NULL);

    entee_parser_parse (parser);
    entee_free_parser  (parser);

    fprintf(stderr, "triples: %d\n", triples);

    // close
    if ( isBzip ) {
        BZ2_bzclose(file);
    } else {
        if ( gzeof(file) is 0 ) {
            int err;
            const char *error_string = gzerror(file, &err);
            if (err) {
                fprintf(stderr, "gzip error: %s.\n", error_string);
                return EXIT_FAILURE;
            }
        }

        gzclose(file);
    }

    return EXIT_SUCCESS;
}
