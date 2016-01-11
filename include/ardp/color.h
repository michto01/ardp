#ifndef __ARDP_COLOR_H__
#define __ARDP_COLOR_H__

#include <stdio.h>

#define ARDP_COLOR_NORMAL ""
#define ARDP_COLOR_RESET "\033[0m" // \033[m
#define ARDP_COLOR_BOLD "\x1b[1m"

#define ARDP_COLOR_RED "\x1b[31m"
#define ARDP_COLOR_GREEN "\x1b[32m"
#define ARDP_COLOR_YELLOW "\x1b[33m"
#define ARDP_COLOR_BLUE "\x1b[34m"
#define ARDP_COLOR_MAGENTA "\x1b[35m"
#define ARDP_COLOR_CYAN "\x1b[36m"
#define ARDP_COLOR_WHITE "\x1b[37m"

#define ARDP_COLOR_BOLD_RED "\x1b[1;31m"
#define ARDP_COLOR_BOLD_GREEN "\x1b[1;32m"
#define ARDP_COLOR_BOLD_YELLOW "\x1b[1;33m"
#define ARDP_COLOR_BOLD_BLUE "\x1b[1;34m"
#define ARDP_COLOR_BOLD_MAGENTA "\x1b[1;35m"
#define ARDP_COLOR_BOLD_CYAN "\x1b[1;36m"

#define ARDP_COLOR_BG_RED "\x1b[41m"
#define ARDP_COLOR_BG_GREEN "\x1b[42m"
#define ARDP_COLOR_BG_YELLOW "\x1b[43m"
#define ARDP_COLOR_BG_BLUE "\x1b[44m"
#define ARDP_COLOR_BG_MAGENTA "\x1b[45m"
#define ARDP_COLOR_BG_CYAN "\x1b[46m"


/* Coloring of the output */
#define ARDP_COLOR_UNKNOWN -1
#define ARDP_COLOR_NEVER 0
#define ARDP_COLOR_ALWAYS 1
#define ARDP_COLOR_AUTO 2

/*
 * Generally the color code will lazily figure this out itself, but
 * this provides a mechanism for callers to override autodetection.
 */
extern int color_stdout_is_tty;

/**
  * Print to stream with color Supported
  *
  * @param[in,out] fp File pointer ( usually STDERR, STDOUT)
  * @param[in] color  Color to be used for printing the message with;
  * @param[in] fmt    Formatting string
  * @param     ...    Aditional variadic parameters
  *
  * @returns 0 is success, value if otherwise.
  */
int ardp_fprintf( FILE *fp, const char *color, const char *fmt, ... )
    __attribute__( ( __format__( __printf__, 3, 4 ) ) );
int ardp_fprintf_ln( FILE *fp, const char *color, const char *fmt, ... )
    __attribute__( ( __format__( __printf__, 3, 4 ) ) );


/*
 * Set the color buffer to the raw color bytes; this is useful for initializing
 * default color variables.
 */
void ardp_color_set( char *dst, const char *color_bytes );

int ardp_config_colorbool( const char *val );
int ardp_want_color( int var );


// void ardp_print_strbuf(FILE *fp, const char *color, const struct strbuf *sb);

#endif /* __ARDP_COLOR_H__ */
