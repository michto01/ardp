/*! @file check_string_create.c
 *
 *
 *  Check if the various string functions are implemented properly.
 */

#include <ardp/string.h>
#include <stdio.h>

int main(int argc, char** argv)
{
        utf8 string;

        // Test creation of counted string
        assert(string_create_n("test", 4));
        // Test creation of string literal of unspecified lenght
        assert(string_create("string1"));

        return 0;
}
