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

#include <ardp/lexer.turtle.h>
#include <ardp/util.h>
#include <ardp/color.h>

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

static int lexer_error(int level, const char *_Nullable str)
{
        return fprintf(stderr, "[LEXER-%d]: %s",level, str);
}

int main( int argc, char** argv )
{
        int status = ARDP_UNKNOWN;

        status = ardp_lexer_create();

        if ( status isnt ARDP_SUCCESS )
                die("[Panic] Couldn't create shared lexer instance");

        status = ardp_lexer_defaults();

        if (status isnt ARDP_SUCCESS)
                die("Couldn't load defaults settings.");

        //Configure the lexer for turtle
        {
                struct ardp_lexer_config cfg;

                cfg.logging.level = DEBUG;
                cfg.logging.eprintf = &lexer_error;

                __block int (^token)(int,const char *_Nullable) = ^int( int type, const char *_Nullable value){
                        printf("[%d] == %s\n", type, value);
                        return ARDP_SUCCESS;
                };

               cfg.cb.token = token;

                status = ardp_lexer_init(&cfg);

                if (status isnt ARDP_SUCCESS)
                        die("Initialization error!");
        }

        uint8_t *mark = NULL;

        status = ardp_lexer_process_block(kLexerTestText, strlen((const char*)kLexerTestText), mark, true);

        fprintf(stdout, "Hello: %s", status == ARDP_SUCCESS ? "YES":"NO");
/*
        ardp_lexer_turtle_process_block( kLexerTestText, strlen((const char*)kLexerTestText), mark, true, ^(int success){
                                if(success isnt ARDP_SUCCESS)
                                        printf("Error while lexing block!");
                                else
                                        ardp_lexer_tutle_finish();
                        });
*/
        for (;;); // wait forever

        return EXIT_SUCCESS;
}
