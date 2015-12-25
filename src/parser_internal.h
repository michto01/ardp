#ifndef ENTEE_PARSER_INTERNAL_H_
#define ENTEE_PARSER_INTERNAL_H_

#include "string.h"
#include "util.h"


struct entee_parser_s {
    utf8            string;
    int             line;
    int             finished;
    entee_reader    reader;
    var             reader_arg;
    entee_handler   handler;
    var             handler_arg;
};

#endif /* ENTEE_PARSER_INTERNAL_H_ */
