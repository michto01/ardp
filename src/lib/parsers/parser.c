

ardp_status_t ardp_parser_create();
ardp_status_t ardp_parser_init( ardp_parser_config_t *cfg );
ardp_status_t ardp_parser_consume( ardp_token_t token );
ardp_status_t ardp_parser_set_parse_handler( void *fc );
ardp_status_t ardp_parser_set_error_handler( void *fc );
ardp_status_t ardp_parser_destroy();

ardp_parser_push( ( token ){RDF_BNODE, {1, 3}, "_:"} );
ardp_parser_pop();

struct _cursor {
        size_t line;
        size_t col;
} cursor;

struct _token {
        enum type;

        struct cursor pos;

        union {
                int i;
                double dbl;
                utf8 str;
                char chr;
        } value;
} token;

struct {
        const uint8_t *uri;
        const uint8_t *name;
} ardp_rdf_turtle_prefix;

struct _parser {
        bool ignore_types; // do not append informations about the type eg. ^^lang:@en
        bool expand_prefixes;

        int state;
        ardp_rdf_turtle_prefix *prefixes; // hold prefixes and base URI
        size_t n_prefixes;
        uint8_t *_Nullable base_uri;

        ardp_linked_list *tokens; // holds pseudo AST
        void ( *error )( struct cursor cur, token tok );
        void ( *event )( token tok );
} parser;

ardp_state_t ardp_parser_state() {
        return NULL;
}

ardp_status_t ardp_parser_set_base_uri( utf8 *_Nonnull uri ) {
        if ( uri is NULL ) {
                return 0; // TODO: should return the correct error value.
        }

        if ( ardp_parser_state() isnt ARDP_STATE_READY ) {
                return 0; // TODO: should return not ready err value
        }

        if ( parser->base_uri ) {
                free( parser->base_uri );
        }

        // FIXME: should have copy function inside the 'string.h'
        string_header_t *hdr = string_hdr( uri );
        memcpy( parser->base_uri, uri, hdr->len );
        return 1; // TODO: should have proper return code
}

ardp_rdf_turtle_prefix_t *ardp_parser_prefix_find( const uint8_t *name, size_t len ) {
        if ( ardp_parser_state() isnt ARDP_STATUS_READY )
                return -1;

        for ( size_t i = 0; i < prefix->n_prefixes; i++ ) {
                const uint8_t *const prefix_name = &parser->prefixes[i].name;
                if ( !memcmp( prefix_name, name, length ) )
                        return &parser->prefixes[i];
        }
}

ardp_status_t ardp_parser_prefix_add( const uint8_t *name, const uint8_t *uri ) {
        if ( ardp_parser_state() isnt ARDP_STATUS_READY )
                return -1; // TODO: meaningful state error

        ardp_rdf_turtle_prefix_t *const prefix = ardp_parser_prefix_find( name, strlen( name ) );
        if ( prefix ) {
                uint8_t *old_predix_uri = ( uint8_t * )prefix->uri;
                prefix->uri = memcpy( parser->uri, uri, strlen( uri ) );
                free( &old_prefix_uri );
        } else {
                parser->prefixes = ( SerdPrefix * )realloc(
                    parser->prefixes,
                    ( ++parser->n_prefixes ) * sizeof( struct ardp_rdf_turtle_prefix ) );
                parser->prefixes[parser->n_prefixes - 1].name = ardp_node_copy( name );
                parser->prefixes[parser->n_prefixes - 1].uri  = ardp_node_copy( uri );
        }
}
