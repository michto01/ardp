/*! @file check_rdf_term_create.c
 *  @brief Test to spot regression in rdf_term implemenation.
 *
 * This files tests the function which create term itself.
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include <errno.h>

 #include <errno.h>
 #include <iso646.h>
 #include <string.h>
 #include <assert.h>
 #include <dispatch/dispatch.h>

 #include <ardp/lexer.h>
 #include <ardp/util.h>
 #include <ardp/color.h>
 #include <ardp/parser.h>

 #include <zlib.h>
 #include <bzlib.h>

 #include <unistd.h>

int color_stdout_is_tty = -1;

static int lexer_error(int level, const char *_Nullable str)
{
        return fprintf(stderr, "[LEXER-%d]: %s",level, str);
}

int read_gzip( uint8_t *p, unsigned int len, void *arg )
{
        gzFile file = arg;
        return gzread(file, p, len);
}

static uint8_t *_Nullable const kLexerTestText= ( uint8_t * )
      "  @base <http://example.org/> .\n"
      "  @prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n"
      "  @prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .\n"
      "  @prefix foaf: <http://xmlns.com/foaf/0.1/> .\n"
      "  @prefix rel: <http://www.perceive.net/schemas/relationship/> . \n"
      ""
      "  <#green-goblin>\n"
      "      rel:enemyOf <#spiderman> ;\n"
      "      a foaf:Person ;    # in the context of the Marvel universe\n"
      "      foaf:name \"Green Goblin\" .\n"
      ""
      "  <#spiderman>\n"
      "      rel:enemyOf <#green-goblin> ;\n"
      "      a foaf:Person ;\n"
      "     foaf:name \'Spiderman\' , \"Человек-паук\"@ru .\n"
      ""
      "  @prefix : <http://example.org/stuff/1.0/> .\n"
      "  (1 -2.0 -3E-1 true false) :p \"w\" ."
      ""
      "  :a :b\n"
      "  [ rdf:first \"apple\";\n"
      "     rdf:rest [ rdf:first \"banana\";\n"
      "     rdf:rest rdf:nil ]\n"
      "  ] ."
      ;

int main( int argc, char **argv )
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);

  size_t c = 0;

  int status = ARDP_UNKNOWN;

        status = ardp_lexer_create();

        if ( status isnt ARDP_SUCCESS )
                die("[Panic] Couldn't create shared lexer instance");

        status = ardp_lexer_defaults();

        if (status isnt ARDP_SUCCESS)
                die("Couldn't load defaults settings.");


        { //Configure the lexer for turtle
                struct ardp_lexer_config cfg;

                cfg.logging.level   = CRITICAL;
                cfg.logging.eprintf = &lexer_error;

                cfg.cb.stoken = ^int(int type, utf8 value, size_t line, size_t col){
                        fprintf(stdout, "(type: %d, line: %ld, col: %ld, value: %s)\n", type, line, col, value);
                        return ARDP_SUCCESS;
                };

                status = ardp_lexer_init(&cfg);

                if (status isnt ARDP_SUCCESS)
                        die("Initialization error!");
        }

        void* file = gzopen( "../../tests/ttl/connectivity-of-lod-datasets.ttl", "rb" );

        ardp_lexer_process_reader(read_gzip, file);
        ardp_lexer_destroy();

        // LEXER - PARSER //

        if ( gzeof( file ) is 0 ) {
                int err;
                const char *error_string = gzerror( file, &err );
                if ( err ) {
                        fprintf( stderr, "gzip error: %s.\n", error_string );
                        return EXIT_FAILURE;
                }
        }

        gzclose( file );


  return EXIT_SUCCESS;
}
