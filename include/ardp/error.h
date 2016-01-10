#ifndef __ARDP_ERROR_H__
#define __ARDP_ERROR_H__

#include <ardp/color.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/*
typedef struct {
    size_t line;
    size_t column;
} ardp_error_processing;

typedef struct {
    size_t line;
    size_t column;
} ardp_error_another;

typedef struct {
    int level;
    int code;
    const char *domain;
    const char *description;
    int type;
    union {
      ardp_error_processing error;
      ardp_error_another err;
    } aditional info;
} ardp_error_t;

ardp_error_t* err;

ardp_error_message(int level, const char* domain, const char *desc);

//#__output__
// ardp_error_msg(WARNING, "LEXER", "Not enought space");
*/

static inline void ardp_error_msg( int lvl, const char * _Nullable domain, const char * _Nonnull desc) {
    int color = ARDP_COLOR_NORMAL;
    switch( lvl ) {
        case INFO:
          color = ARDP_COLOR_GREEN;
          break;
        case WARNING:
          color = ARDP_COLOR_YELLOW;
          break;
        case MESSAGE:
          color = ARDP_COLOR_BLUE;
          break
        case ERROR:
          color = ARDP_COLOR_RED;
          break
        default:
          break;
    }

    if ( domain )
        ardp_fprintf(stderr, color, "[%s]: ", domain);

    ardp_fprintf(stderr, ARDP_COLOR_NORMAL, "%s\n", desc);
}


#endif /* __ARDP_ERROR_H__ */
