//typedef unsigned char uchar;

// int ardp_parser_parse( ardp_parser *parser ) {
//     if ( parser->reader is NULL or parser->handler is NULL )
//         return 0;
//
//     int cs;
//
//     %% write init;
//
//     uint8_t buf[BUFSIZE];
//     uint8_t *mark = 0;
//
//     int have = 0;
//     int res  = 1;
//
//     ardp_reader reader = parser->reader;
//
//     while ( not parser->finished ) {
//         uint8_t *pe;
//         uint8_t *p   = buf + have;
//         uint8_t *eof = 0;
//
//         int space = BUFSIZE - have;
//         if ( space is 0 ) {
//             // out of buffer space
//             res = 0;
//             break;
//         }
//
//         int len = reader( p, space, parser->reader_arg );
//         pe = p + len;
//
//         if ( len < space ) {
//             eof = pe;
//             parser->finished = 1;
//         }
//
//         %% write exec;
//
//         if ( cs is ntriples_error ) {
//             res = 0;
//             break;
//         } else if (mark) {
//             have = pe - mark;
//             memmove( buf, mark, have );
//             mark = buf;
//         } else {
//             have = 0;
//         }
//     }
//
//     return res;
// }
