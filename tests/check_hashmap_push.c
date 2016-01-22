#include <stdio.h>
#include <ardp/hashmap.h>

int main( int argc, char **argv ) {
        map_t *map = map_create( 1 );

        if ( map is NULL ) {
                fprintf( stderr, "Map not allocated.\n" );
                return -1;
        }

        return map_push( map, "key", "value" );
}
