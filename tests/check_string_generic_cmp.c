/*! @file check_string_generic_cmp.c
 *  @brief Test to spot regression in string implemenation.
 *
 * This files tests the function which compares two string in bounded length.
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

        status = string_generic_cmp((const uint8_t*)"aha",(const uint8_t*)"aha", 3);
        if (!status)
            EXIT_FAILURE;

        status = string_generic_cmp((const uint8_t*)"bub",(const uint8_t*)"aha", 3);
        if (status)
            EXIT_FAILURE;

        status = string_generic_cmp((const uint8_t*)"ahaah",(const uint8_t*)"aha", 3);
        if (!status)
            EXIT_FAILURE;


        return EXIT_SUCCESS;
}
