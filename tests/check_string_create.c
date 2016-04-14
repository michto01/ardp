/*! @file check_string_create.c
 *
 *
 *  Check if the various string functions are implemented properly.
 */

#include <ardp/string.h>
#include <stdio.h>
#include <assert.h>

int color_stdout_is_tty = -1;

int main(int argc, char** argv)
{
        utf8 string;

        // Test creation of counted string
        assert( (string_create_n((const uint8_t*)"test", 4, 4) != NULL) );
        // Test creation of string literal of unspecified lenght
        assert( (string_create((const uint8_t *)"string1") != NULL) );

        return 0;
}
