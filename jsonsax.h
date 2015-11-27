#ifndef JSONSAX_H
#define JSONSAX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

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

#ifdef JSONSAX_ERRORS
extern const char* jsonsax_errors[];
#endif

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

int jsonsax_parse( const char* json, const jsonsax_handlers_t* handlers, void* userdata );

#ifdef __cplusplus
}
#endif

#endif /* JSONSAX_H */
