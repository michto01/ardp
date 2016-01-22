#include <stdio.h>
#include <string.h>
//#include <check.h>

#include <ardp/util.h>
#include <ardp/hashmap.h>

int main( int argc, char **argv ) {
        map_t *map = map_create( 1 );

        if ( map is NULL ) {
                fprintf( stderr, "Map not allocated.\n" );
                return -1;
        }

        int status = map_push( map, "key", "value" );
        if ( status isnt 0 ) {
                fprintf( stderr, "Map key not pushed.\n" );
                return -1;
        }

        var data;

        status = map_get( map, "key", &data );
        if ( status isnt 0 ) {
                fprintf( stderr, "Could not get info from map.\n" );
                return status;
        }

        status = strcmp( ( char * )data, "value" );
        // ck_assert( strcmp( ( char * )data, "value" ) == 0 );

        if ( status == 0 ) {
                return 0; // Alles gutte
        } else {
                return status + ( -100 );
        }
}
