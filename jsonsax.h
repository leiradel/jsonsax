#ifndef JSONSAX_H
#define JSONSAX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

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

typedef int ( *jsonsax_handler_t )( void*, jsonsax_event_t, const char*, size_t );

#ifdef JSONSAX_ERRORS
extern const char* jsonsax_errors[];
#endif

jsonsax_result_t jsonsax_parse( const char* json, void* userdata, jsonsax_handler_t handler );

#ifdef __cplusplus
}
#endif

#endif /* JSONSAX_H */
