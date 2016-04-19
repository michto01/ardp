/*! @file check_string_strlen.c
 *  @brief Test to spot regression in string implemenation.
 *
 * This files tests the function which are checks length of string
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ardp/string.h>


int color_stdout_is_tty = -1;

int main( int argc, char **argv )
{
        size_t len = 3;
        utf8 str = string_create_n((const uint8_t*)"abc", len, len);
        if (string_strlen(str) != len)
          return EXIT_FAILURE;

        string_dealloc(str);
        return EXIT_SUCCESS;
}
