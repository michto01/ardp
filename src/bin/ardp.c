#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef HAVE_STDINT
 #include <stdint.h>
#endif

#include <getopt.h>
#include <zlib.h>
#include <bzlib.h>

//#include <libdispatch.h>

#include <ardp/parser.h>
#include <ardp/string.h>
#include <ardp/color.h>
#include <ardp/util.h>
#include "config.h"

typedef enum {
    STATE_SUBJECT,
    STATE_PREDICATE,
    STATE_OBJECT,
    STATE_EXTRA
} parser_state;

parser_state state = STATE_SUBJECT;
int triples = 0;

int color_stdout_is_tty = -1;

static const char ardp_usage_string[] = PACKAGE
	" [-v | --version] [-h | --help] [-u | --usage] [-f | --file <path>]"    "\n"
	"     [-b | --use-bzip] [-c | --color auto:none:always]"                 "\n"
	"     [-s | --syntax tutle:nt:nq:guess]"                                 "\n";

static const char ardp_help_string[] = PACKAGE_STRING "\n"
  "\t"  "-v --version" "\t\t" "Display current version";

static void help_option_ln( FILE *fs , const char* option, const char *desc ) {
  fprintf(fs, "\t");
  ardp_fprintf(fs, ARDP_COLOR_BOLD, "%s", option);
  fprintf(fs, "\t\t");
  ardp_fprintf_ln(fs, ARDP_COLOR_NORMAL, "%s", desc);
}

static void help( FILE *fs ) {
  ardp_fprintf_ln(fs, ARDP_COLOR_NORMAL, PACKAGE_STRING);
  help_option_ln(fs, "-v --version",  "Display version string");
  help_option_ln(fs, "-h --help",     "Print this help");
  help_option_ln(fs, "-u --usage",    "Print compact help");
  help_option_ln(fs, "-f --file",     "File or archive to be processed");
  help_option_ln(fs, "-s --syntax",   "Process file with selected syntax parser [guess, turtle, nt, nq]");
  help_option_ln(fs, "-b --use-bzip", "Use BZip library to read file/archive");
  fprintf(fs, "\n");
  help_option_ln(fs, "-c --color",    "Change the coloring of the output");
}


int read_gzip(unsigned char* p, unsigned int len, void *arg) {
    gzFile file = arg;
    return gzread(file, p, len);
}

int read_bzip(unsigned char* p, unsigned int len, void *arg) {
    BZFILE *file = arg;
    return BZ2_bzread(file, p, len);
}

void handler(ardp_token_type type, utf8 s, void *arg) {

    switch (state) {
        case STATE_SUBJECT: {
            ardp_fprintf(stdout, ARDP_COLOR_CYAN, "%s ", s);
            state = STATE_PREDICATE;
            triples += 1;
            goto cleanup;
        }
        case STATE_PREDICATE: {
            ardp_fprintf(stdout, ARDP_COLOR_RED, "%s ", s);
            state = STATE_OBJECT;
            goto cleanup;
        }
        case STATE_OBJECT: {
            ardp_fprintf(stdout, ARDP_COLOR_GREEN, "%s ", s);
            switch ( type ) {
                case ARDP_LANGUAGE_TAGGED_LITERAL_VALUE:
                case ARDP_DATATYPE_LITERAL_VALUE: {
                    state = STATE_EXTRA;
                    goto cleanup;
                }
                default: {
                    state = STATE_SUBJECT;
                    ardp_fprintf(stdout, ARDP_COLOR_NORMAL, "\n");
                    goto cleanup;
                }
            }
            goto cleanup;
        }
        case STATE_EXTRA: {
            ardp_fprintf(stdout, ARDP_COLOR_BLUE, "%s\n", s);
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

    char *filename = NULL;
    bool isBzip    = false;

    // auto detect color predispositions at start
    color_stdout_is_tty = ardp_want_color( -1 );

    // parse options
    int c;
    while ( (c = getopt(argc, argv, "bhuavrf:c:")) isnt -1 ) {
        switch ( c ) {
            case 'f':
                filename = optarg;
                break;
            case 'b':
                isBzip = true;
                break;
            case 'h':
                help(stdout);
                return EXIT_SUCCESS;
            case 'u':
                ardp_fprintf(stderr, ARDP_COLOR_NORMAL, ardp_usage_string);
                return EXIT_SUCCESS;
            case 'v':
                ardp_fprintf_ln(stdout, ARDP_COLOR_BOLD, PACKAGE_STRING);
                return EXIT_SUCCESS;

            case 'c': {
                int clr = ardp_config_colorbool(optarg);
                if( clr is ARDP_COLOR_AUTO) {
                  color_stdout_is_tty = ardp_want_color( -1 );
                } else {
                  color_stdout_is_tty = clr;
                }
            }
            break;

            default:
                ardp_fprintf(stderr, ARDP_COLOR_NORMAL, ardp_usage_string);
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
    ardp_parser *parser = ardp_new_parser();
    ardp_parser_set_reader  (parser, (ardp_reader) (isBzip ? read_bzip : read_gzip), file);
    ardp_parser_set_handler (parser, handler, NULL);

//    ardp_parser_set_handler(parser, ^void(ardp_token_type type, const char *s, void *arg){
//        switch ( type ) {
//            case STATE_SUBJECT:
//                printf("Hello");
//                break;
//
//            default:
//                break;
//        }
//    }, NULL);

    ardp_parser_parse (parser);
    ardp_free_parser  (parser);

    fprintf(stderr, "triples: %d\n", triples);

    // close
    if ( isBzip ) {
        BZ2_bzclose(file);
    } else {
        if ( gzeof(file) is 0 ) {
            int err;
            const char *error_string = gzerror(file, &err);
            if ( err ) {
                fprintf(stderr, "gzip error: %s.\n", error_string);
                return EXIT_FAILURE;
            }
        }

        gzclose(file);
    }

    return EXIT_SUCCESS;
}
