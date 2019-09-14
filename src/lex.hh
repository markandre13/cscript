#include <stdio.h>


typedef enum {
  TKN_NONE = 0,
  
  // A.4 Expressions
  TKN_EXPRESSION_LIST = 256,
  TKN_CAST_EXPRESSION,
  TKN_CONDITIONAL_EXPRESSION,
  TKN_EXPRESSION,
  
  // A.5 Statements
  TKN_STATEMENT_SEQ,
  
  // A.6 Declarations
  TKN_DECLARATOR,
  TKN_DECL_SPECIFIER_SEQ,

  // A.7 Declarators
  TKN_INIT_DECLARATOR_LIST,
  TKN_PARAMETER_DECLARATION_LIST,
  TKN_PARAMETER_DECLARATION,

  // Keywords
  TKN_FOR,
  TKN_IF,
  TKN_ELSE,
  TKN_WHILE,
  TKN_DO,   
  TKN_SWITCH,
  TKN_CASE,
  TKN_DEFAULT,
  TKN_BREAK,
  TKN_CONTINUE,
  TKN_GOTO,
  TKN_RETURN,
  TKN_CLASS,
  TKN_STRUCT,
  TKN_PRIVATE,
  TKN_PROTECTED,
  TKN_PUBLIC,
  TKN_ENUM,
  TKN_SIZEOF,
  TKN_THIS,
  TKN_TRUE,
  TKN_FALSE,

  TKN_FRIEND,
  TKN_TYPEDEF,
  TKN_AUTO,
  TKN_REGISTER,
  TKN_STATIC,
  TKN_EXTERN,
  TKN_MUTABLE,
  TKN_INLINE,
  TKN_VIRTUAL,
  TKN_EXPLICIT,

  TKN_IDENTIFIER,
  TKN_FUNCTION,
  TKN_FUNCTION_CALL,
  TKN_CLASS_NAME,

  TKN_CHAR,
  TKN_WCHAR_T,
  TKN_BOOL,
  TKN_SHORT,  
  TKN_INT,
  TKN_LONG,
  TKN_SIGNED,
  TKN_UNSIGNED,
  TKN_FLOAT,
  TKN_DOUBLE,
  TKN_VOID,

  TKN_VALUE,
  TKN_STRING,
  
  TKN_ELLIPSIS,   // '...'
  TKN_COL_COL,    // '::'
  TKN_OR,   	  // '||'
  TKN_AND,        // '&&'
  TKN_INC,        // '++'
  TKN_DEC,        // '--'
  TKN_EQ,         // '=='
  TKN_NEQ,        // '!='
  TKN_LE,         // '<='
  TKN_GE,         // '>='
  TKN_SHL,        // '<<'
  TKN_SHR,        // '>>'
  TKN_PTR,        // '->'
  TKN_APLUS,	  // '+='
  TKN_AMINUS,     // '-='
  TKN_AMULT,      // '*='
  TKN_ADIV,       // '/='
  TKN_AMOD,       // '%='
  TKN_AXOR,       // '^='
  TKN_AAND,       // '&='
  TKN_AOR,        // '|='
  TKN_ASHL,       // '<<='
  TKN_ASHR,       // '>>='

  TKN_EOF
} token_e;

typedef struct _node_t {
  int tkn;
  char *text;
  struct _node_t *next, *down;
} node_t;

extern FILE* lex_in;
node_t* compile(FILE *in);
node_t* lex();
void unlex(node_t*);
void lexfree(node_t*);
void node_append(node_t*, node_t*);
void node_append_next(node_t*, node_t*);
void node_print0(FILE *out, node_t *n, unsigned depth);
inline void node_print(FILE *out, node_t *n) {
  node_print0(out, n, 0);
}
node_t *node_new(token_e tkn);
inline node_t *node_new(int tkn) {
  return node_new(static_cast<token_e>(tkn));
}
node_t *node_new_txt(token_e tkn, const char *txt);
void node_compile(FILE *out, node_t *n, unsigned indent=0);
