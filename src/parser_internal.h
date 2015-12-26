#ifndef ARDP_PARSER_INTERNAL_H_
#define ARDP_PARSER_INTERNAL_H_

#include "string.h"
#include "util.h"


struct ardp_parser_s {
    utf8            string;
    int             line;
    int             finished;
    ardp_reader    reader;
    var             reader_arg;
    ardp_handler   handler;
    var             handler_arg;
};

#endif /* ARDP_PARSER_INTERNAL_H_ */
