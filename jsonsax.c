#include "jsonsax.h"

#include <setjmp.h>
#include <string.h>
#include <ctype.h>

#ifdef JSONSAX_ERRORS
const char* jsonsax_errors[] =
{
  "Ok",
  "Interrupted",
  "Missing key",
  "Unterminated key",
  "Missing value",
  "Unterminated object",
  "Unterminated array",
  "Unterminated string",
  "Invalid value"
};
#endif

typedef struct
{
  jsonsax_handler_t handler;
  
  const char* json;
  void*       ud;
  jmp_buf     env;
}
state_t;

static inline void skip_spaces( state_t* state )
{
  while ( isspace( *state->json ) )
  {
    state->json++;
  }
}

static inline void skip_digits( state_t* state )
{
  while ( isdigit( *state->json ) )
  {
    state->json++;
  }
}

#define HANDLE( event, str, num ) \
  do { \
    if ( state->handler( state->ud, event, str, num ) ) \
      longjmp( state->env, JSONSAX_INTERRUPTED ); \
  } while ( 0 )
  
static void parse_value( state_t* state );

static void parse_object( state_t* state )
{
  /* we're sure the current character is a '{' */
  HANDLE( JSONSAX_OBJECT, state->json++, 1 );
  skip_spaces( state );
  
  while ( *state->json != '}' )
  {
    if ( *state->json != '"' )
    {
      longjmp( state->env, JSONSAX_MISSING_KEY );
    }
    
    const char* name = ++state->json;
    
    for ( ;; )
    {
      const char* quote = strchr( state->json, '"' );
      
      if ( !quote )
      {
        longjmp( state->env, JSONSAX_UNTERMINATED_KEY );
      }
      
      state->json = quote + 1;
      
      if ( quote[ -1 ] != '\\' )
      {
        break;
      }
    }
    
    HANDLE( JSONSAX_KEY, name, state->json - name - 1 );
    skip_spaces( state );
    
    if ( *state->json != ':' )
    {
      longjmp( state->env, JSONSAX_MISSING_VALUE );
    }
    
    state->json++;
    skip_spaces( state );
    parse_value( state );
    skip_spaces( state );
    
    if ( *state->json != ',' )
    {
      break;
    }
    
    state->json++;
    skip_spaces( state );
  }
  
  if ( *state->json != '}' )
  {
    longjmp( state->env, JSONSAX_UNTERMINATED_OBJECT );
  }
  
  HANDLE( JSONSAX_OBJECT, state->json++, 0 );
}

static void parse_array( state_t* state )
{
  unsigned int ndx = 0;
  
  /* we're sure the current character is a '[' */
  HANDLE( JSONSAX_ARRAY, state->json++, 1 );
  skip_spaces( state );
  
  while ( *state->json != ']' )
  {
    HANDLE( JSONSAX_INDEX, state->json, ndx++ );
    parse_value( state );
    skip_spaces( state );
    
    if ( *state->json != ',' )
    {
      break;
    }
    
    state->json++;
    skip_spaces( state );
  }
  
  if ( *state->json != ']' )
  {
    longjmp( state->env, JSONSAX_UNTERMINATED_ARRAY );
  }
  
  HANDLE( JSONSAX_ARRAY, state->json++, 0 );
}

static void parse_string( state_t* state )
{
  const char* string = ++state->json;
  
  for ( ;; )
  {
    const char* quote = strchr( state->json, '"' );
    
    if ( !quote )
    {
      longjmp( state->env, JSONSAX_UNTERMINATED_STRING );
    }
    
    state->json = quote + 1;
    
    if ( quote[ -1 ] != '\\' )
    {
      break;
    }
  }
  
  HANDLE( JSONSAX_STRING, string, state->json - string - 1 );
}

static void parse_boolean( state_t* state )
{
  if ( !strncmp( state->json, "true", 4 ) )
  {
    state->json += 4;
    HANDLE( JSONSAX_BOOLEAN, NULL, 1 );
  }
  else if ( !strncmp( state->json, "false", 5 ) )
  {
    state->json += 5;
    HANDLE( JSONSAX_BOOLEAN, NULL, 0 );
  }
  else
  {
    longjmp( state->env, JSONSAX_INVALID_VALUE );
  }
}

static void parse_null( state_t* state )
{
  if ( !strncmp( state->json + 1, "ull", 3 ) ) /* we're sure the current character is a 'n' */
  {
    state->json += 4;
    HANDLE( JSONSAX_NULL, NULL, 0 );
  }
  else
  {
    longjmp( state->env, JSONSAX_INVALID_VALUE );
  }
}

static void parse_number( state_t* state )
{
  const char* number = state->json;
  
  if ( *state->json == '-' )
  {
    state->json++;
  }
  
  if ( !isdigit( *state->json ) )
  {
    longjmp( state->env, JSONSAX_INVALID_VALUE );
  }
  
  skip_digits( state );
  
  if ( *state->json == '.' )
  {
    state->json++;
    
    if ( !isdigit( *state->json ) )
    {
      longjmp( state->env, JSONSAX_INVALID_VALUE );
    }
    
    skip_digits( state );
  }
  
  if ( *state->json == 'e' || *state->json == 'E' )
  {
    state->json++;
    
    if ( *state->json == '-' || *state->json == '+' )
    {
      state->json++;
    }
    
    if ( !isdigit( *state->json ) )
    {
      longjmp( state->env, JSONSAX_INVALID_VALUE );
    }
    
    skip_digits( state );
  }
  
  HANDLE( JSONSAX_NUMBER, number, state->json - number );
}

static void parse_value( state_t* state )
{
  skip_spaces( state );
  
  switch ( *state->json )
  {
  case '{':
    parse_object( state );
    break;
  
  case '[':
    parse_array( state );
    break;
    
  case '"':
    parse_string( state );
    break;
    
  case 't':
  case 'f':
    parse_boolean( state );
    break;
    
  case 'n':
    parse_null( state );
    break;
    
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-':
    parse_number( state );
    break;
    
  default:
    longjmp( state->env, JSONSAX_INVALID_VALUE );
  }
}

jsonsax_result_t jsonsax_parse( const char* json, void* userdata, jsonsax_handler_t handler )
{
  state_t state;
  int res;
  
  state.json = json;
  state.handler = handler;
  state.ud = userdata;
  
  if ( ( res = setjmp( state.env ) ) == 0 )
  {
    if ( handler( userdata, JSONSAX_DOCUMENT, NULL, 1 ) )
    {
      return JSONSAX_INTERRUPTED;
    }

    parse_value( &state );

    if ( handler( userdata, JSONSAX_DOCUMENT, NULL, 0 ) )
    {
      return JSONSAX_INTERRUPTED;
    }
    
    res = JSONSAX_OK;
  }
  
  return res;
}
