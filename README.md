# JSON SAX

**jsonsax** is a very small streamming API to parse JSON code. It calls into your code during the parsing, without allocating any memory.

Streamming APIs are great when the entire document tree is too big to maintain in memory, when you only need a small part of the document. It's also very fast.

To use JSON SAX, just implement your event handler:

```C
int handler( void* userdata, int event, const char* arg1, size_t arg2 );
```
and pass it to `jsonsax_parse` along with your in-memory JSON document:

```C
jsonsax_result_t jsonsax_parse( const char* json, void* userdata, int ( *handler )( void*, jsonsax_event_t, const char*, size_t ) );
```

The event handler receives the userdata passed to `jsonsax_parse`, the event type, and two arguments depending on the event.

The possible events are:

```C
typedef enum
{
  JSONSAX_DOCUMENT,
  JSONSAX_OBJECT,
  JSONSAX_ARRAY,
  JSONSAX_KEY,
  JSONSAX_INDEX,
  JSONSAX_STRING,
  JSONSAX_NUMBER,
  JSONSAX_BOOLEAN,
  JSONSAX_NULL
}
jsonsax_event_t;
```

And the meaning of the arguments are:

* `JSONSAX_DOCUMENT`: `NULL`, and a number which is `1` at the beginning of the document, and `0` at its ending.
* `JSONSAX_OBJECT`: A pointer to the `{` character which is the beginning of the document, or to the `}` which is its ending, and a number which is `1` at the beginning of the document, and `0` at its ending.
* `JSONSAX_ARRAY`: A pointer to the `[` character which is the beginning of the array, or to the `]` which is its ending, and a number which is `1` at the beginning of the array, and `0` at its ending.
* `JSONSAX_KEY`: A pointer to the string which contains the name of the key, and a number with its length.
* `JSONSAX_INDEX`: A pointer to the first character of the array element, and a number which is the `0`-based index of the element.
* `JSONSAX_STRING`: A pointer to the string, and a number with its length.
* `JSONSAX_NUMBER`: A pointer to the string containing the number, and a number with its length.
* `JSONSAX_BOOLEAN`: `NULL`, and a number which is `0` if the boolean is `false`, or `1` if its `true`.
* `JSONSAX_NULL`: `NULL` and `0`.

If an event handler returns a value different from zero, the parsing stops and `json_parse` returns `JSONSAX_INTERRUPTED`. `jsonsax_parse` returns one of the following values:

```C
typedef enum
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
}
jsonsax_result_t;
```

## Building

Just add `jsonsax.h` and `jsonsax.c` to your project. `jsonsax.c` requires a C99 compiler.

## Changelog

### 0.3

* Fixed parsing of escape sequences
* Added a **building** section to the README

### 0.2

* Removed all the different handlers, now there's only one handler which receives the event type
