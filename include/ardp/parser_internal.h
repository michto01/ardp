#ifndef __ARDP_PARSER_INTERNAL_H__
#define __ARDP_PARSER_INTERNAL_H__

#include "string.h"
#include "util.h"


struct ardp_parser_s {
    utf8            string;
    int             line;
    int             finished;
    ardp_reader     reader;
    var             reader_arg;
    ardp_handler    handler;
    var             handler_arg;
    // ragel internals
    int             cs;
};

#endif /* __ARDP_PARSER_INTERNAL_H__ */
