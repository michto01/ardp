#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <ardp/util.h>
#include <ardp/parser.h>
#include <ardp/parser_internal.h>

#define BUFSIZE 0x10000

typedef struct ardp_pareser ardp_parser_obj_t;

ardp_parser_obj_t* ardp_parser_obj_constructor() {
    return NULL;
}

void adrp_parser_obj_destroy( ardp_parser_obj_t *self ) ??<
??>

typedef struct {

}

typedef int  (*ardp_parser_obj_reader)(unsigned char *buffer, unsigned len, void *arg);
typedef void (*ardp_parser_obj_handler)(ardp_token_type type, utf8 s, void *arg);

typedef struct {
  int cs;

  ardp_parser_obj_reader  reader;
  ardp_parser_obj_handler handler;



} Parser;


typedef struct {

} Plugin;



/*
parser_t* p = Parser();

p->init->begin();
p->init->syntax(ARDP_SYNTAX_NQUADS);
p->init->handler(void *fn, void *args);
p->init->finnish();

p->read->block(const char* block);
p->read->file(void *reader_fn, void *args);
p->read->finish();

void * parser = new(Parser);

*/
