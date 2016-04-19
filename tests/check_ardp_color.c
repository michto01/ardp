#include <stdio.h>
#include <stdlib.h>
#include <ardp/color.h>

int color_stdout_is_tty = -1;

int main( int argc, char** argv )
{
	ardp_fprintf_ln( stdout, ARDP_COLOR_NORMAL, "Hello" );
	ardp_fprintf_ln( stdout, kARDPColorRed, "Achtung die panzer!");

	ardp_fprintf( stdout, ARDP_COLOR_NORMAL, "Hello" );
	ardp_fprintf( stdout, kARDPColorRed, "Achtung die panzer!");
	return EXIT_SUCCESS;
}
