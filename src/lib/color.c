#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iso646.h>
#include <unistd.h>
#include <stdarg.h>

#include <ardp/color.h>
#include <ardp/util.h>

const char *const kARDPColorNormal  = "";
const char *const kARDPColorReset   = "\033[0m";
const char *const kARDPColorBold    = "\x1b[1m";

const char *const kARDPColorRed     = "\x1b[31m";
const char *const kARDPColorGreen   = "\x1b[32m";
const char *const kARDPColorYellow  = "\x1b[33m";
const char *const kARDPColorBlue    = "\x1b[34m";
const char *const kARDPColorMagenta = "\x1b[35m";
const char *const kARDPColorCyan    = "\x1b[36m";
const char *const kARDPColorWhite   = "\x1b[37m";

const char *const kARDPColorBoldRed     = "\x1b[1;31m";
const char *const kARDPColorBoldGreen   = "\x1b[1;32m";
const char *const kARDPColorBoldYellow  = "\x1b[1;33m";
const char *const kARDPColorBoldBlue    = "\x1b[1;34m";
const char *const kARDPColorBoldMagenta = "\x1b[1;35m";
const char *const kARDPColorBoldCyan    = "\x1b[1;36m";

const char *const kARDPColorBackgroundRed     = "\x1b[41m";
const char *const kARDPColorBackgroundGreen   = "\x1b[42m";
const char *const kARDPColorBackgroundYellow  = "\x1b[43m";
const char *const kARDPColorBackgroundBlue    = "\x1b[44m";
const char *const kARDPColorBackgroundMagenta = "\x1b[45m";
const char *const kARDPColorBackgroundCyan    = "\x1b[46m";

static char *const kARDPColorAuto   = "auto";
static char *const kARDPColorAlways = "always";
static char *const kARDPColorNever  = "never";

static char *const kARDPColorTerminalType = "dumb";

int ardp_config_colorbool( const char *val )
{
        if ( val ) {
                if ( strcasecmp( val, kARDPColorAuto ) is 0 )
                        return ARDP_COLOR_AUTO;
                if ( strcasecmp( val, kARDPColorAlways ) is 0 )
                        return ARDP_COLOR_ALWAYS;
                if ( strcasecmp( val, kARDPColorNever ) is 0 )
                        return ARDP_COLOR_NEVER;
        }
        return ARDP_COLOR_AUTO;
}

/* Check info: man 5 terminfo or $# infocmp */
static int ardp_auto_color( void )
{
        if ( color_stdout_is_tty < 0 )
                color_stdout_is_tty = isatty( 1 );
        if ( color_stdout_is_tty ) {
                char *term = getenv( "TERM" );
                if ( term and strcmp( term, kARDPColorTerminalType ) )
                        return 1;
        }
        return 0;
}

int ardp_want_color( int var )
{
        static int want_auto = -1;

        if ( var < 0 )
                var = ARDP_COLOR_AUTO; // automatic color is default now

        if ( var is ARDP_COLOR_AUTO ) {
                if ( want_auto < 0 )
                        want_auto = ardp_auto_color();
                return want_auto;
        }
        return var;
}


static int ardp_vfprintf( FILE       *fp,
                          const char *color,
                          const char *fmt,
                          va_list     args,
                          const char *trail )
{
        int r = 0;

        if ( *color )
                r += fprintf( fp, "%s", color );

        r += vfprintf( fp, fmt, args );

        if ( *color )
                r += fprintf( fp, "%s", kARDPColorReset );

        if ( trail )
                r += fprintf( fp, "%s", trail );

        return r;
}

int ardp_fprintf( FILE *fp, const char *color, const char *fmt, ... )
{
        va_list args;
        int r;
        va_start( args, fmt );
        if ( color_stdout_is_tty )
                r = ardp_vfprintf( fp, color, fmt, args, NULL );
        else
                r = vfprintf( fp, fmt, args );

        va_end( args );
        return r;
}

int ardp_fprintf_ln( FILE *fp, const char *color, const char *fmt, ... )
{
        va_list args;
        int r;
        va_start( args, fmt );
        if ( color_stdout_is_tty ) {
                r = ardp_vfprintf( fp, color, fmt, args, "\n" );
        } else {
                r = vfprintf( fp, fmt, args );
                r += fprintf( fp, "\n" );
        }
        va_end( args );
        return r;
}
