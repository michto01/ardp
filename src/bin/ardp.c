#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <zlib.h>
#include <bzlib.h>
#include <stdint.h>

//#include <curl/curl.h> // to be integrated as direct url read

#include <ardp/parser.h>
#include <ardp/string.h>
#include <ardp/color.h>
#include <ardp/util.h>


typedef enum { STATE_SUBJECT, STATE_PREDICATE, STATE_OBJECT, STATE_EXTRA } parser_state;

parser_state state = STATE_SUBJECT;
int triples        = 0;

int color_stdout_is_tty = -1;

static const char ardp_usage_string[] =
    "usage: " PACKAGE " [-v | --version] [-h | --help] [-u | --usage] [-f | --file <path>]"
    "\n"
    "            [-b | --use-bzip] [-c | --color auto:none:always]"
    "\n"
    "            [-s | --syntax tutle:nt:nq:guess]";

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
        help_option_ln( fs, "-f --file", "File or archive to be processed" );
        help_option_ln( fs, "-s --syntax",
                        "Process file with selected syntax parser [guess, turtle, nt, nq]" );
        help_option_ln( fs, "-b --use-bzip", "Use BZip library to read file/archive" );
        help_option_ln( fs, "", "" );
        help_option_ln( fs, "-c --color", "Change the coloring of the output" );
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
int read_gzip( unsigned char *p, unsigned int len, void *arg ) {
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
int read_bzip( unsigned char *p, unsigned int len, void *arg ) {
        BZFILE *file = arg;
        return BZ2_bzread( file, p, len );
}

void parser( ardp_token_type token, utf8 value, void *ard) 
{
	switch(state) {
		case STATE_SUBJECT:
			ardp_fprintf( stdout, kARDPColorCyan, "%s", value);
	}
}

void handler( ardp_token_type type, utf8 s, void *arg ) {
        switch ( state ) {
                case STATE_SUBJECT: {
                        ardp_fprintf( stdout, ARDP_COLOR_CYAN, "%s ", s );
                        state = STATE_PREDICATE;
                        triples += 1;
                        goto cleanup;
                }
                case STATE_PREDICATE: {
                        ardp_fprintf( stdout, ARDP_COLOR_RED, "%s ", s );
                        state = STATE_OBJECT;
                        goto cleanup;
                }
                case STATE_OBJECT: {
                        ardp_fprintf( stdout, ARDP_COLOR_GREEN, "%s ", s );
                        switch ( type ) {
                                case ARDP_LANGUAGE_TAGGED_LITERAL_VALUE:
                                case ARDP_DATATYPE_LITERAL_VALUE: {
                                        state = STATE_EXTRA;
                                        goto cleanup;
                                }
                                default: {
                                        state = STATE_SUBJECT;
                                        ardp_fprintf( stdout, ARDP_COLOR_NORMAL, "\n" );
                                        goto cleanup;
                                }
                        }
                        goto cleanup;
                }
                case STATE_EXTRA: {
                        ardp_fprintf( stdout, ARDP_COLOR_BLUE, "%s\n", s );
                        state = STATE_SUBJECT;
                        goto cleanup;
                }
        }

cleanup:
        // string_dealloc(s);
        return;
}

extern void test_string( void );

int main( int argc, char **argv ) {
        char *filename = NULL;
        bool is_bzip   = false;

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
            {"color",     required_argument, 0, 'c' },
            {"syntax",    required_argument, 0, 's' },
            {"file",      required_argument, 0, 'f' },
            {0,           0,                 0,  0  }
        };
        /* clang-format on */


        // not enought arguments provided
        if ( argc < 2 ) {
                ardp_fprintf( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                return EXIT_FAILURE;
        }

        if ( setvbuf( stdout, NULL, _IONBF, 0 ) ) {
                perror( "[ardp:internal] Failed to change buffering mode for stdout" );
                return EXIT_FAILURE;
        }

        while ( ( opt = getopt_long( argc, argv, "bhuvc:f:s:", long_options, &long_index ) )isnt -
                1 ) {
                switch ( opt ) {
                        case 'f': filename = optarg; break;
                        case 'b': is_bzip = true; break;
                        case 'h': help( stdout ); return EXIT_SUCCESS;
                        case 'u':
                                ardp_fprintf_ln( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                                return EXIT_SUCCESS;
                        case 'v':
                                ardp_fprintf_ln( stdout, ARDP_COLOR_BOLD, PACKAGE_STRING );
                                return EXIT_SUCCESS;
                        case 's':
                                ardp_fprintf_ln(
                                    stderr, ARDP_COLOR_RED,
                                    "Currently only placeholder as we recognize only N-triples." );
                                return EXIT_SUCCESS;
                        case 'c': {
                                int clr = ardp_config_colorbool( optarg );
                                if ( clr is ARDP_COLOR_AUTO ) {
                                        color_stdout_is_tty = ardp_want_color( -1 );
                                } else {
                                        color_stdout_is_tty = clr;
                                }
                        } break;

                        default:
                                ardp_fprintf( stderr, ARDP_COLOR_NORMAL, ardp_usage_string );
                                return EXIT_FAILURE;
                }
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
                fprintf( stderr, "Unable to open file \"%s\"", filename );
                return EXIT_FAILURE;
        }

        // parse
        ardp_parser *parser = ardp_new_parser();
        ardp_parser_set_reader( parser, ( ardp_reader )( is_bzip ? read_bzip : read_gzip ), file );
        ardp_parser_set_handler( parser, handler, NULL );

        ardp_parser_parse( parser );
        ardp_free_parser( parser );

        fprintf( stderr, "triples: %d\n", triples );

        // close
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
