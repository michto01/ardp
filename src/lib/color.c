#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iso646.h>
#include <unistd.h>
#include <stdarg.h>

#include <ardp/color.h>
#include <ardp/util.h>

int ardp_config_colorbool( const char *val ) {
        if ( val ) {
                if ( strcasecmp( val, "auto" ) is 0 )
                        return ARDP_COLOR_AUTO;
                if ( strcasecmp( val, "always" ) is 0 )
                        return ARDP_COLOR_ALWAYS;
                if ( strcasecmp( val, "never" ) is 0 )
                        return ARDP_COLOR_NEVER;
        }
        return ARDP_COLOR_AUTO;
}

/* Check info: man 5 terminfo or $# infocmp */
static int ardp_auto_color( void ) {
        if ( color_stdout_is_tty < 0 )
                color_stdout_is_tty = isatty( 1 );
        if ( color_stdout_is_tty /*or ( pager_in_use() and pager_use_color )*/ ) {
                char *term = getenv( "TERM" );
                if ( term and strcmp( term, "dumb" ) )
                        return 1;
        }
        return 0;
}

int ardp_want_color( int var ) {
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


static int
    ardp_vfprintf( FILE *fp, const char *color, const char *fmt, va_list args, const char *trail ) {
        int r = 0;

        if ( *color )
                r += fprintf( fp, "%s", color );

        r += vfprintf( fp, fmt, args );

        if ( *color )
                r += fprintf( fp, "%s", ARDP_COLOR_RESET );

        if ( trail )
                r += fprintf( fp, "%s", trail );

        return r;
}

int ardp_fprintf( FILE *fp, const char *color, const char *fmt, ... ) {
        va_list args;
        int r;
        va_start( args, fmt );
        if ( color_stdout_is_tty ) {
                r = ardp_vfprintf( fp, color, fmt, args, NULL );
        } else {
                r = vfprintf( fp, fmt, args );
        }
        va_end( args );
        return r;
}

int ardp_fprintf_ln( FILE *fp, const char *color, const char *fmt, ... ) {
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
