/*! @file ardp.c
 *
 * The main of the ARDP RDF parser.
 *
 * @todo Move auxiliary functions to separate file (help_option_ln, ...)
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2015
 */
#include "config.h"

#include <dispatch/dispatch.h>
#include <iso646.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#if HAVE_UNISTD
#include <unistd.h>
#endif

#include <ctype.h>
#include <getopt.h>
#include <stdint.h>

#include <zlib.h>
#include <bzlib.h>
//#include <curl/curl.h> // to be integrated as direct url read

#include <ardp/lexer.h>
#include <ardp/parser.h>
#include <ardp/color.h>
#include <ardp/util.h>

int color_stdout_is_tty = -1;

static const char ardp_usage_string[] =
    "usage: " PACKAGE " [-v | --version] [-h | --help] [-u | --usage] <path-to-file>"
    "\n"
    "            [-b | --use-bzip] [-c | --color auto:none:always]"
    "\n"
    "            [-s | --syntax tutle:nt:nq:guess]"
    "\n"
    "            [-w | --verbose <0-7>]"
    "\n"
    "            [-x | --disable-uri-expansion]"
    "\n"
    "            [-z | --disable-curie-expansion]"
    "\n"
    "            [-d | --disable-show-datatype]"
    "\n"
    "            [-y | --enable-lexer-debug]"
    "\n"
    "            [-q | --enable-parser-debug]"
    ;

/**
  * Helper function to write the application options with support for `inteligent`
  * color detection and printing.
  *
  * @param[out] fs      Pointer to output file/stream
  * @param[in]  option  Option which we are describing.
  * @param[in]  desc    Description of the option
  */
static void help_option_ln( FILE *fs, const char *option, const char *desc ) {
        fprintf( fs, "\t" );
        ardp_fprintf( fs, ARDP_COLOR_BOLD, "%s", option );
        fprintf( fs, "\t\t" );
        ardp_fprintf_ln( fs, ARDP_COLOR_NORMAL, "%s", desc );
}

/**
  * Help info dump
  *
  * Prints the usage help with explanation of each option.
  *
  * @param[in,out] fs File pointer/stream to dump the info to.
  */
static void help( FILE *fs ) {
        ardp_fprintf_ln( fs, ARDP_COLOR_NORMAL, PACKAGE_STRING );
        help_option_ln( fs, "-v --version", "Display version string" );
        help_option_ln( fs, "-h --help", "Print this help" );
        help_option_ln( fs, "-u --usage", "Print compact help" );
        //help_option_ln( fs, "-f --file", "File or archive to be processed" );
        help_option_ln( fs, "-s --syntax",
                        "Process file with selected syntax parser [guess, turtle, nt, nq]" );
        help_option_ln( fs, "-b --use-bzip", "Use BZip library to read file/archive" );
        help_option_ln( fs, "", "" );
        help_option_ln( fs, "-c --color", "Change the coloring of the output" );
        help_option_ln( fs, "", "" );
        help_option_ln( fs, "-w --verbose", "Set verbosity of the output for debuging");
        help_option_ln( fs, "-y --enable-lexer-debug", "Set the lexer into debug mode explicitly");
        help_option_ln( fs, "-q --enable-parser-debug", "Set the parser into debug mode explicitly");
        help_option_ln( fs, "-x --disable-uri-expansion", "Disable the expansion of the URIs" );
        help_option_ln( fs, "-z --disable-curie-expansion", "Disable the expansion of the CURIEs" );
        help_option_ln( fs, "-d --disable-show-datatype", "Disable the datatype printout" );
}

/**
  * Read using GZIP method
  *
  * @note Used for reading of @c*.gz archives and normal @c*.nt files.
  *
  * @param[out] p     Pointer to block of memory to be read into.
  * @param[in]  len   Length of the block to be read.
  * @param[in]  arg   File desccriptors and info.
  */
static int read_gzip( unsigned char *p, unsigned int len, void *arg )
{
        gzFile file = arg;
        return gzread( file, p, len );
}

/**
  * Read using BZIP method
  *
  * @param[out] p     Pointer to block of memory to be read into.
  * @param[in]  len   Length of the block to be read.
  * @param[in]  arg   File desccriptors and info.
  */
static int read_bzip( unsigned char *p, unsigned int len, void *arg )
{
        BZFILE *file = arg;
        return BZ2_bzread( file, p, len );
}

static int lexer_error(int level, const char *_Nullable str)
{
        return ardp_fprintf_ln(stderr, kARDPColorMagenta , "[lexer]: %s", /*level,*/ str);
}

int main( int argc, char **argv ) {
        char *filename = NULL;
        bool is_bzip   = false;

        int uri_expansion   = 1;
        int curie_expansion = 1;
        int show_datatype   = 1;

        int verbose = 0;


        /* auto detect color predispositions at start */
        color_stdout_is_tty = ardp_want_color( -1 );

        /* parse options */
        int opt;
        int long_index = 0;
        /* Specifying the expected options */ /* clang-format off */
        static struct option long_options[] = {
            {"version",   no_argument,       0, 'v' },
            {"help",      no_argument,       0, 'h' },
            {"usage",     no_argument,       0, 'u' },
            {"use-bzip",  no_argument,       0, 'b' },

            {"enable-lexer-debug",      no_argument, 0, 'y'},
            {"enable-parser-debug",     no_argument, 0, 'q'},
            {"disable-uri-expansion",   no_argument, 0, 'x'},
            {"disable-curie-expansion", no_argument, 0, 'z'},
            {"disable-show-datatype",   no_argument, 0, 'd'},


            {"color",     required_argument, 0, 'c' },
            {"syntax",    required_argument, 0, 's' },
            {"file",      required_argument, 0, 'f' },
            {"verbose",   required_argument, 0, 'w' },
            {0,           0,                 0,  0  }
        };
        /* clang-format on */

        /* Not enought arguments provided */
        if ( argc < 2 ) {
                ardp_fprintf( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                return EXIT_FAILURE;
        }

        if ( setvbuf( stdout, NULL, _IONBF, 0 ) ) {
                perror( "[ardp:internal] Failed to change buffering mode for stdout." );
                return EXIT_FAILURE;
        }

        if (setvbuf(stderr, NULL, _IONBF, 0)) {
                perror("[ardp:internal] Failed to change buffering mode for stderr.");
                return EXIT_FAILURE;
        }

        while ( ( opt = getopt_long( argc, argv, "yqbhuvxzdc:s:w:", long_options, &long_index ) ) isnt - 1 ) {
                switch ( opt ) {
                        /*case 'f':
                                filename = optarg;
                                break;*/
                        case 'b':
                                is_bzip = true;
                                break;
                        case 'h':
                                help( stdout );
                                return EXIT_SUCCESS;
                        case 'u':
                                ardp_fprintf_ln( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                                return EXIT_SUCCESS;
                        case 'v':
                                ardp_fprintf_ln( stdout, ARDP_COLOR_BOLD, PACKAGE_STRING );
                                return EXIT_SUCCESS;
                        case 's':
                                ardp_fprintf_ln(
                                    stderr,
                                    ARDP_COLOR_RED,
                                    "Currently only placeholder as we recognize only N-triples."
                                );
                                return EXIT_SUCCESS;

                        case 'x':
                                uri_expansion = 0;
                                break;

                        case 'z':
                                curie_expansion = 0;
                                break;

                        case 'd':
                                show_datatype = 0;
                                break;

                        case 'w':
                                if (isdigit((((char *)optarg)[0])))
                                        verbose = (((char *)optarg)[0]) - '0';
                                else
                                        verbose = 1;
                                break;

                        case 'y':
                                verbose += 10;
                                break;

                        case 'q':
                                verbose += 20;
                                break;

                        case 'c': {
                                int clr = ardp_config_colorbool(optarg);
                                if ( clr is ARDP_COLOR_AUTO ) {
                                        color_stdout_is_tty = ardp_want_color(-1);
                                } else {
                                        color_stdout_is_tty = clr;
                                }
                        } break;

                        default:
                                ardp_fprintf( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                                return EXIT_FAILURE;
                }
        }

        for (size_t i = optind; i < argc; i++) {
                //printf("Process: %s\n", argv[i]);
                filename = argv[i];
        }

        if ( not filename ) {
                ardp_fprintf_ln( stderr, ARDP_COLOR_NORMAL,
                                 "File to process not provided, exiting...\n" );
                ardp_fprintf_ln( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                return EXIT_FAILURE;
        }

        // open
        void *file = ( is_bzip ? BZ2_bzopen( filename, "rb" ) : gzopen( filename, "rb" ) );

        if ( !file ) {
                ardp_fprintf( stderr, kARDPColorRed, "Unable to open file \"%s\"\n", filename );
                return EXIT_FAILURE;
        }

        //-- LEXER - PARSER  --//
        if (ardp_lexer_create() isnt ARDP_SUCCESS)
                die("[ardp panic]: Couldn't create shared lexer instance\n");
        if (ardp_lexer_defaults() isnt ARDP_SUCCESS)
                die("[ardp panic]: Couldn't load defaults for the lexer.\n");

        { //Lexer preconfiguration
                struct ardp_lexer_config cfg;

                cfg.logging.level   = (verbose >= 10) ? DEBUG : ERROR;//verbose; // ERROR;
                cfg.logging.eprintf = &lexer_error;

                cfg.cb.stoken = ^int(int type, utf8 value, size_t line, size_t col) {
                        if (verbose == 2 || verbose >= 10)
                                ardp_fprintf_ln(stderr, kARDPColorMagenta, "[lexer]: type: %d, value: %s", type, value);
                        ardp_parser_exec(type, value, line, col);
                        return ARDP_SUCCESS;
                };

                if (ardp_lexer_init(&cfg) isnt ARDP_SUCCESS)
                        die("[ardp panic]: Initialization error!\n");
        }

        if (ardp_parser_create() isnt ARDP_SUCCESS)
                die("[ardp panic]: Failed to create parser instance.\n");

        if (verbose == 3 || verbose > 20)
                ardp_parser_trace();

        /* Set the defualt URI to the document uri */
        ardp_parser_set_default_base(filename);

        ardp_parser_set_option((uint8_t *)"show:datatype", (void **) &show_datatype);
        ardp_parser_set_option((uint8_t *)"expand:uri",    (void **) &uri_expansion);
        ardp_parser_set_option((uint8_t *)"expand:curie",  (void **) &curie_expansion);

        ardp_lexer_process_reader( is_bzip ? read_bzip : read_gzip, file );

        ardp_parser_finish();
        ardp_parser_destroy();
        ardp_lexer_destroy();

        // LEXER - PARSER //

        if ( is_bzip ) {
                BZ2_bzclose( file );
        } else {
                if ( gzeof( file ) is 0 ) {
                        int err;
                        const char *error_string = gzerror( file, &err );
                        if ( err ) {
                                fprintf( stderr, "gzip error: %s.\n", error_string );
                                return EXIT_FAILURE;
                        }
                }

                gzclose( file );
        }

        return EXIT_SUCCESS;
}
