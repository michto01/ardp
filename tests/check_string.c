#include <assert.h>
#include <string.h>

#include <ardp/util.h>
#include <ardp/color.h>
#include <ardp/string.h>

int color_stdout_is_tty; /* Need to be puted per specification (extern int) */

int main( int argc, char **argv ) {
        utf8 str = string_new();

        string_append_char( &str, 'h' );
        string_append_char( &str, 't' );
        string_append_char( &str, 't' );
        string_append_char( &str, 'p' );
        string_append_char( &str, ':' );
        string_append_char( &str, '/' );
        string_append_char( &str, '/' );
        string_append_char( &str, 'g' );
        string_append_char( &str, 'o' );
        string_append_char( &str, 'o' );
        string_append_char( &str, 'g' );
        string_append_char( &str, 'l' );
        string_append_char( &str, 'e' );
        string_append_char( &str, '.' );
        string_append_char( &str, 'c' );
        string_append_char( &str, 'o' );
        string_append_char( &str, 'm' );
        string_append_char( &str, '/' );
        string_append_char( &str, '?' );
        string_append_char( &str, 's' );
        string_append_char( &str, '=' );
        string_append_char( &str, 'f' );
        string_append_char( &str, 'i' );
        string_append_char( &str, 's' );
        string_append_char( &str, 'h' );

        int result = strcmp( ( const char * )str, "http://google.com/?s=fish" );

        if ( result isnt 0 ) {
                string_dealloc( str );
                return -1; // Error while parsing string
        }

        string_dealloc( str );

        str = string_alloc(3);
        string_append_utf8(&str, 3990);

        string_dealloc(str);

        return 0;
}
