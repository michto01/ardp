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

parser_t* p = parser_create();

p->syntax = ARDP_SYNTAX_NQUADS;
p->READER(void *fn, void *args);
p->reader()
