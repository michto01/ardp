#include <assert.h>
#include <string.h>

#include "string.h"

bool test_string_addition_with_realloc(void);

void test_string(void) {
   bool result = false;

   result = test_string_addition_with_realloc();
}

bool test_string_addition_with_realloc(void) {
  utf8 str = string_new();
      string_append_char( &str, 'h');
      string_append_char( &str, 't');
      string_append_char( &str, 't');
      string_append_char( &str, 'p');
      string_append_char( &str, ':');
      string_append_char( &str, '/');
      string_append_char( &str, '/');
      string_append_char( &str, 'g');
      string_append_char( &str, 'o');
      string_append_char( &str, 'o');
      string_append_char( &str, 'g');
      string_append_char( &str, 'l');
      string_append_char( &str, 'e');
      string_append_char( &str, '.');
      string_append_char( &str, 'c');
      string_append_char( &str, 'o');
      string_append_char( &str, 'm');
      string_append_char( &str, '/');
      string_append_char( &str, '?');
      string_append_char( &str, 's');
      string_append_char( &str, '=');
      string_append_char( &str, 'f');
      string_append_char( &str, 'i');
      string_append_char( &str, 's');
      string_append_char( &str, 'h');

  int result;
  assert( (result = strcmp((const char* )str, "http://google.com/?s=fish")) == 0 );

  string_dealloc(str);

  return result ? false : true;
}
