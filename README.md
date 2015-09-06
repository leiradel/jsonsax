# JSON SAX

**jsonsax** is a very small streamming API to parse JSON code. It calls into your code during the parsing, without allocating any memory.

Streamming APIs are great when the entire document tree is too big to maintain in memory, when you only need a small part of the document. It's also very fast.

To use JSON SAX, just implement your event handlers:

    typedef struct
    {
      int ( *start_document )( void* userdata );
      int ( *end_document )( void* userdata );
      int ( *start_object )( void* userdata );
      int ( *end_object )( void* userdata );
      int ( *start_array )( void* userdata );
      int ( *end_array )( void* userdata );
      int ( *key )( void* userdata, const char* name, size_t length );
      int ( *index )( void* userdata, unsigned int index );
      int ( *string )( void* userdata, const char* string, size_t length );
      int ( *number )( void* userdata, const char* number, size_t length );
      int ( *boolean )( void* userdata, int istrue );
      int ( *null )( void* userdata );
    }
    jsonsax_handlers_t;

and pass then to `jsonsax_parse` along with your in-memory JSON document:

    int jsonsax_parse( const char* json, const jsonsax_handlers_t* handlers, void* userdata );

`jsonsax_parse` returns one of the following values:

    enum
    {
      JSONSAX_OK = 0,
      JSONSAX_INTERRUPTED,
      JSONSAX_MISSING_KEY,
      JSONSAX_UNTERMINATED_KEY,
      JSONSAX_MISSING_VALUE,
      JSONSAX_UNTERMINATED_OBJECT,
      JSONSAX_UNTERMINATED_ARRAY,
      JSONSAX_UNTERMINATED_STRING,
      JSONSAX_INVALID_VALUE
    };

If an event handler returns a value different from zero, the parsing stops and `json_parse` returns `JSONSAX_INTERRUPTED`. The meaning of each event handler is:

* `start_document` is called before anything is parsed
* `end_document` is called after everything was parsed, if the parsing was not interrupted by the user or by an error
* `start_object` is called when a `{` signaling the start of an object is found
* `end_object` is called when a `}` signaling the end of an object is found
* `start_array` is called when a `[` signaling the start of an array is found
* `end_array` is called when a `]` signaling the end of an array is found
* `key` is called when a key of a `<key, value>` pair of an object is found
* `index` is called before every value inside an array, with increasing values for index, starting at 0
* `string` is called when a value of type `string` is found
* `number` is called when a value of type `number` is found
* `boolean` is called when the value `true` or `false` is found, with `istrue` holding 1 if the value is `true`, and 0 otherwise
* `null` is called when a `null` value is found
