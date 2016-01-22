#include <string.h>
#include <stdbool.h>
#include <ardp/hash.h>

unsigned int hash_simple( char *s ) {
        register unsigned int hash;
        register unsigned char *p;

        for ( hash = 0, p = ( unsigned char * )s; *p; p++ )
                hash = 31 * hash + *p;

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_rs( char *s ) {
        unsigned int b    = 378551;
        unsigned int a    = 63689;
        unsigned int hash = 0;

        while ( *s ) {
                hash = hash * a + ( *s++ );
                a *= b;
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_js( char *s ) {
        unsigned int hash = 1315423911;

        while ( *s ) {
                hash ^= ( ( hash << 5 ) + ( *s++ ) + ( hash >> 2 ) );
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_pjw( char *s ) {
        unsigned int bits_in_unsigned_int = ( unsigned int )( sizeof( unsigned int ) * 8 );
        unsigned int three_quarters       = ( unsigned int )( ( bits_in_unsigned_int * 3 ) / 4 );
        unsigned int one_eighth           = ( unsigned int )( bits_in_unsigned_int / 8 );

        unsigned int high_bits = ( unsigned int )( 0xFFFFFFFF )
                                 << ( bits_in_unsigned_int - one_eighth );
        unsigned int hash = 0;
        unsigned int test = 0;

        while ( *s ) {
                hash = ( hash << one_eighth ) + ( *s++ );
                if ( ( test = hash & high_bits ) != 0 ) {
                        hash = ( ( hash ^ ( test >> three_quarters ) ) & ( ~high_bits ) );
                }
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_elf( char *s ) {
        unsigned int hash = 0;
        unsigned int x    = 0;

        while ( *s ) {
                hash = ( hash << 4 ) + ( *s++ );
                if ( ( x = hash & 0xF0000000L ) != 0 ) {
                        hash ^= ( x >> 24 );
                        hash &= ~x;
                }
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_bkdr( char *s ) {
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;

        while ( *s ) {
                hash = hash * seed + ( *s++ );
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_sdbm( char *s ) {
        unsigned int hash = 0;

        while ( *s ) {
                hash = ( *s++ ) + ( hash << 6 ) + ( hash << 16 ) - hash;
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_djb( char *s ) {
        unsigned int hash = 5381;

        while ( *s ) {
                hash += ( hash << 5 ) + ( *s++ );
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_ap( char *s ) {
        unsigned int hash = 0;
        for ( size_t i = 0; *s; i++ ) {
                if ( ( i & 1 ) == 0 ) {
                        hash ^= ( ( hash << 7 ) ^ ( *s++ ) ^ ( hash >> 3 ) );
                } else {
                        hash ^= ( ~( ( hash << 11 ) ^ ( *s++ ) ^ ( hash >> 5 ) ) );
                }
        }

        return ( hash & 0x7FFFFFFF );
}

unsigned int hash_crc( char *s ) {
        unsigned int nleft        = strlen( s );
        unsigned long long sum    = 0;
        unsigned short int *w     = ( unsigned short int * )s;
        unsigned short int answer = 0;

        /*
         * Our algorithm is simple, using a 32 bit accumulator (sum), we add
         * sequential 16 bit words to it, and at the end, fold back all the
         * carry bits from the top 16 bits into the lower 16 bits.
         */
        while ( nleft > 1 ) {
                sum += *w++;
                nleft -= 2;
        }

        /*
         * mop up an odd byte, if necessary
         */
        if ( 1 == nleft ) {
                *( unsigned char * )( &answer ) = *( unsigned char * )w;
                sum += answer;
        }

        /*
         * add back carry outs from top 16 bits to low 16 bits
         * add hi 16 to low 16
         */
        sum = ( sum >> 16 ) + ( sum & 0xFFFF );
        /* add carry */
        sum += ( sum >> 16 );
        /* truncate to 16 bits */
        answer = ~sum;

        return ( answer & 0xFFFFFFFF );
}
