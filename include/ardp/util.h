#ifndef __ARDP_UTIL_H__
#define __ARDP_UTIL_H__

#define is   ==
#define isnt !=

#define or  ||
#define and &&
#define not !

#define ARDP_SUCCESS true
#define ARDP_FAILURE false

typedef void* var;

/* Supported colors */

#define _CLR_NORMAL   "\x1B[0m"
#define _CLR_RED      "\x1B[31m"
#define _CLR_GREEN    "\x1B[32m"
#define _CLR_YELLOW   "\x1B[33m"
#define _CLR_BLUE     "\x1B[34m"
#define _CLR_MAGENTA  "\x1B[35m"
#define _CLR_CYAN     "\x1B[36m"
#define _CLR_WHITE    "\x1B[37m"
#define _CLR_RESET    "\033[0m"

/* Supported colors 2 */

#define CLR_NORMAL   "\x1B[0m"
#define CLR_RED      "\x1B[31m"
#define CLR_GREEN    "\x1B[32m"
#define CLR_YELLOW   "\x1B[33m"
#define CLR_BLUE     "\x1B[34m"
#define CLR_MAGENTA  "\x1B[35m"
#define CLR_CYAN     "\x1B[36m"
#define CLR_WHITE    "\x1B[37m"
#define CLR_RESET    "\033[0m"


#endif /* __ARDP_UTIL_H__ */
