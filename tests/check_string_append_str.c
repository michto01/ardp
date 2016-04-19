/*! @file check_string_append_str.c
 *  @brief Test to spot regression in string implemenation.
 *
 * This files tests the function which are supporsed to append string.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ardp/string.h>

int color_stdout_is_tty = -1;

int main( int argc, char **argv )
{
        int status;

        utf8 str = string_create( ( const uint8_t * )"AAA" );
        assert( str != NULL );
        utf8 rts = string_create( ( const uint8_t * )"BBB" );
        assert( rts != NULL );

        utf8 dummy = string_copy( str );
        assert( dummy != NULL );

        status = string_append_str( &dummy, rts );
        if ( status )
                return EXIT_FAILURE;

        if ( strcmp( ( const char * )dummy, ( const char * )"AAABBB" ) != 0 )
                return EXIT_FAILURE;

        string_dealloc( str );
        string_dealloc( rts );
        string_dealloc( dummy );
        return EXIT_SUCCESS;
}
