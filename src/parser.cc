#include "lex.hh"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// C++ Grammar (From the Annex A of ISO/IEC 14882:1998 (The C++ Standard)

static bool trace = true;

// A.1 Keywords
// typedef-name
// namespace-name
static node_t* class_name();
// enum-name
// template-name

// A.2 Lexical conventions

// A.3 Basic concepts
static node_t* translation_unit();

// A.4 Expressions
static node_t* primary_expression();
static node_t* id_expression();
static node_t* unqualified_id();
static node_t* qualified_id();
static node_t* nested_name_specifier();
// class_or_namespace_name
static node_t* postfix_expression();
static node_t* expression_list();
// pseudo-destructor-name
static node_t* unary_expression();
static node_t* unary_operator();
// new-expression
// new-placement
// new-type-id
// new declarator
// direct-new-declarator
// delete-expression
static node_t* cast_expression();
static node_t* pm_expression();
static node_t* multiplicative_expression();
static node_t* additive_expression();
static node_t* shift_expression();
static node_t* relational_expression();
static node_t* equality_expression();
static node_t* and_expression();
static node_t* exclusive_or_expression();
static node_t* inclusive_or_expression();
static node_t* logical_and_expression();
static node_t* logical_or_expression();
static node_t* conditional_expression();
static node_t* assignment_expression();
static node_t* assignment_operator();
static node_t* expression();

// A.5 Statements
static node_t* statement();
static node_t* labeled_statement();
static node_t* expression_statement();
static node_t* compound_statement();
static node_t* statement_seq();
static node_t* selection_statement();
static node_t* condition();
static node_t* iteration_statement();
static node_t* for_init_statement();
static node_t* jump_statement();
static node_t* declaration_statement();

// A.6 Declarations
static node_t* declaration_seq();
static node_t* declaration();
static node_t* block_declaration();
static node_t* simple_declaration();
static node_t* decl_specifier();
static node_t* decl_specifier_seq();
static node_t* storage_class_specifier();
static node_t* function_specifier();
// typedef-name
static node_t* type_specifier();
static node_t* simple_type_specifier();
static node_t* type_name();
// elaborated-type-specifier
// enum-name
// enum-specifier
// enumerator-list
// enumerator-definition
// enumerator
// namespace-name
// original-namespace-name
// namespace-definition
// named-namespace-definition
// original-namespace-definition
// extension-namespace-definition
// unnamed-namespace-definition
// namespace-body
// namespace-alias
// namespace-alias-definition
// qualified-namespace-specifier
// using-declaration
// using-directive
// asm-definition
// linkage-specification

// A.7 Declarators
static node_t* init_declarator_list();
static node_t* init_declarator();
static node_t* declarator();
static node_t* direct_declarator();    
static node_t* ptr_operator();
// cv-qualifier-seq
// cv-qualifier
static node_t* declarator_id();
static node_t* type_id();
static node_t* type_specifier_seq();
static node_t* abstract_declarator();
static node_t* direct_abstract_declarator();
static node_t* parameter_declaration_clause();
static node_t* parameter_declaration_list();
static node_t* parameter_declaration();
static node_t* function_definition();
static node_t* function_body();
// initializer
// initializer-clause
// initializer-list

// A.7 Classes
static node_t* class_name();
// static node_t* class_specifier();
// static node_t* class_head();
// class-key
// static node_t* member_specification();
// static node_t* member_declaration();
// member-declarator-list
// member-declarator
// pure-specifier
// constant-initializer

// A.9 Derieved classes
// base-clause
// base-specifier-list
// base-specifier
// access-specifier

// A.10 Special member functions
// conversion-function-id
// conversion-type-id
// conversion-declarator
// ctor-initializer
// mem-initializer-list
// mem-initializer
// mem-initializer-id

// A.11 Overloading
// operator-function-id
// operator

// A.12 Templates
// template-declaration
// template-parameter-list
// template-parameter
// type-parameter
// template-id
// template-name
// template-argument-list
// template-argument
// explicit-instantiation
// explicit-specialization

// A.13 Exception handling
// try-block
// function-try-block
// handler-seq
// handler
// exception-declaration
// throw-expression
// exception-specification
// type-id-list

// A.14 Preprocessing directives

// ------------------------------

void
error(const char *message)
{
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}

node_t*
parse(FILE *in)
{
  if (lex_in)
    error("lex/parse are not re-entrant");
  lex_in = in;
  auto result = translation_unit();
  lex_in = nullptr;
  return result;
}

static node_t*
identifier()
{
  node_t *n = lex();
  if (!n)
    return n;
  if (n->tkn==TKN_IDENTIFIER)
    return n;
  unlex(n);
  n=0;
  return n;
}


/*
 *
 * A.1 Keywords
 *
 */

/*
 *
 * A.3 Basic concepts
 *
 */

node_t*
translation_unit()
{
  return declaration_seq();
}

/*
 *
 * A.4 Expressions
 *
 */

/*
primary-expression:
                  literal
                  'this'
                  ( expression ) 
                  id-expression
*/
node_t*
primary_expression()
{
  node_t *n0 = lex();
  if (!n0)
    return 0;
  if (n0->tkn == TKN_IDENTIFIER) {
    if (trace)
      printf("primary-expression -> identifier\n");
    return n0;
  }
  if ( n0->tkn == TKN_VALUE_INT ||
       n0->tkn == TKN_VALUE_DOUBLE )
  {
    if (trace)
      printf("primary-expression -> value\n");
    return n0;
  }
  if (n0->tkn == TKN_STRING) {
    if (trace)
      printf("primary-expression -> string\n");
    return n0;
  }
  if (n0->tkn == TKN_TRUE || n0->tkn == TKN_FALSE) {
    if (trace)
      printf("primary-expression -> boolean\n");
    return n0;
  }
  if (n0->tkn=='(') {
    node_t *n1 = expression();
    if (!n1) {
      fprintf(stderr, "unexpected EOF after '(' in primary_expression\n");
      exit(EXIT_FAILURE);
    }
    node_t *n2 = lex();
    if (!n2 || n2->tkn!=')') {
      fprintf(stderr, "expected ')' in primary_expression\n");
      exit(EXIT_FAILURE);
    }
    lexfree(n0);
    lexfree(n2);
    if (trace)
      printf("primary-expression -> '(' expression ')'\n");
    return n1;
  }
  unlex(n0);
  n0 = id_expression();
  if (n0) {
    if (trace)
      printf("primary-expression -> id-expression:\n");
    node_print(stdout, n0);
    return n0;
  }
  return 0;
}

/*
id-expression:
    unqualified-id
    qualified-id
*/
node_t*
id_expression()
{
  node_t *n0;
  n0 = unqualified_id();
  if (n0)
    return n0;
  n0 = qualified_id();
  return n0;
}

/*
unqualified-id:
    identifier
    operator-function-id
    conversion-function-id
    ~ class-name
    template-id
*/
node_t*
unqualified_id()
{
  node_t *n0;
  n0 = lex();
  if (!n0 || n0->tkn!=TKN_IDENTIFIER) {
    unlex(n0);
    return 0; 
  }
  return n0;
}

/*
qualified-id:
    ::opt nested-name-specifier templateopt unqualified-id
    :: identifier
    :: operator-function-id
    :: template-id
*/
node_t*
qualified_id()
{
//printf("qualified_id\n");
  node_t *n0 = nested_name_specifier();
  if (!n0)
    return 0;
//printf("  n0:");
//n0->print(stdout);
  // template
  node_t *n2 = unqualified_id();
//printf("  n2:");
//if (n2) n2->print(stdout); else printf("\n");
  if (!n2) {
    unlex(n2);
    unlex(n0);
    return 0; 
  }
  node_append(n0, n2);
  return n0;
}

/*
nested-name-specifier:
    class-or-namespace-name :: nested-name-specifieropt
    class-or-namespace-name :: template nested-name-specifier
*/
// <class> '::' [<class> '::' [...]]
node_t*
nested_name_specifier()
{
  node_t *n0 = lex();
  if (!n0 || n0->tkn!=TKN_CLASS_NAME) {
    unlex(n0);
    return 0; 
  }
  node_t *n1 = lex();
  if (!n1 || n1->tkn!=TKN_COL_COL) {
    unlex(n1);
    unlex(n0);
    return 0; 
  }
  node_t *n2 = nested_name_specifier();
  if (n2) {
    node_append(n0, n2);
  }
  lexfree(n1);
  return n0;  
}

/*
class-or-namespace-name:
    class-name
    namespace-name
*/

/*
postfix-expression:
    primary-expression
    postfix-expression [ expression ]
    postfix-expression ( expression-listopt )
    simple-type-specifier ( expression-listopt )
    "typename" ::opt nested-name-specifier identifier ( expression-listopt )
    "typename" ::opt nested-name-specifier templateopt template-id ( expression-listopt )
    postfix-expression . templateopt id-expression
    postfix-expression -> templateopt id-expression
    postfix-expression . pseudo-destructor-name
    postfix-expression -> pseudo-destructor-name
    postfix-expression ++ 
    postfix-expression -- 
    "dynamic_cast" < type-id > ( expression )
    "static_cast" < type-id > ( expression )
    "reinterpret_cast" < type-id > ( expression )
    "const_cast" < type-id > ( expression )
    type-id ( expression )
    type-id ( type-id )
*/
node_t*
postfix_expression()
{
  node_t *n0, *n1;
  n0 = primary_expression();
  if (!n0)
    return n0;

  if (trace) {
    printf("postfix-expression:\n");
    node_print(stdout, n0);
  }

  n1 = lex();
  if (!n1) { 
    return n0;
  }
   
  if (n1->tkn=='.') {
    node_t *n2 = lex();
    if (n2->tkn != TKN_IDENTIFIER) {
      fprintf(stderr, "THE DOT FAILED\n");
      exit(1);
    }
    node_append(n0, n2);
    lexfree(n1);   
    n1 = lex();    
  }
   
  if (n1->tkn=='(') {
    if (n0->tkn != TKN_IDENTIFIER /* && n0->tkn != TKN_CLASS_NAME */) 
    {
      fprintf(stderr, "postfix_expression: function call not implemented for:\n");
      node_print(stderr, n0);
      exit(1);
    }
     
    node_t *n2 = expression_list();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_t *n3 = lex();
    if (!n3 || n3->tkn!=')' ) {
      fprintf(stderr, "missing ')' after expression_list in postfix_expression\n");
      node_print0(stderr, n0, 1);
      node_print0(stderr, n1 ,1);
      node_print0(stderr, n2, 1);
      fprintf(stderr, "but got:");
      node_print0(stderr, n3, 1);
    }
    
    // non-standard here-document extension
    node_t *n4 = lex();
    if (n4 && n4->tkn==TKN_SHL) {
      lexfree(n4);
      node_t *n4 = lex();
      if (!n4 || n4->tkn!=TKN_IDENTIFIER)
        error("illegal here-document limiter");
      while(true) {
        int c = fgetc(lex_in);
        if (c==EOF)
          error("unexpected end of here-document\n");
        if (c=='\n')
          break;
      }
      size_t p = 0;
      char *str = (char*)malloc(16);
      size_t capacity=16, size=0;
      while(true) {
        int c = fgetc(lex_in);
        if (c==EOF)
          error("unexpected end of here-document\n");
        if (c==n4->text[p]) {
          ++p;
          if (n4->text[p]==0)
            break;
        } else {
          p=0;
        }
        if (size+2 > capacity) {
          capacity<<=1;
          str = (char*)realloc(str, capacity);
        }
        str[size++] = c;
      }
      str[size-p]=0;
      node_t *arg = node_new(TKN_STRING);
      arg->text = str;
      node_append(n2, arg);
      lexfree(n4);
    } else {
      unlex(n4);
    }
    
/*   
printf("expression list for function call:\n");
node_print(stdout, n2);
*/
    node_t *nx = node_new(TKN_FUNCTION_CALL);
    node_append(nx, n0); // id
    node_append(nx, n2); // expression list
    n0 = nx;
    lexfree(n1);
    lexfree(n3);
    return n0;
  }
   
  unlex(n1);
  return n0;
}

/*
expression-list:
    assignment-expression
    expression-list , assignment-expression
*/
node_t*
expression_list()
{
  node_t *n0 = 0;
  node_t *n1 = assignment_expression();
  if (!n1)
    return n0;
  n0 = node_new(TKN_EXPRESSION_LIST);
  node_append(n0, n1);

  while(true) {
    node_t *n2 = lex();
    if (!n2 || n2->tkn != ',') {
      unlex(n2);
      return n0;
    }
    lexfree(n2);

    n1 = assignment_expression();
    if (!n1) {
      fprintf(stderr, "expected assignment_expression after ',' in expression_list\n");
      exit(EXIT_FAILURE);
    }
    node_append(n0, n1);
  }
  return 0;
}

/*
pseudo-destructor-name:
    ::opt nested-name-specifieropt type-name :: ~ type-name
    ::opt nested-name-specifier "template" template-id :: ~ type-name
    ::opt nested-name-specifieropt ~ type-name
*/

/*
unary-expression:
    postfix-expression
    ++ cast-expression
    -- cast-expression
    unary-operator cast-expression
    sizeof unary-expression
    sizeof ( type-id ) 
    new-expression
    delete-expression
*/
static node_t*
unary_expression()
{
  node_t *n0 = postfix_expression();
  if (n0)
    return n0;
  
  n0 = unary_operator();
  if (n0) {
    node_t *n1 = cast_expression();
    if (n1) {
      node_append(n0, n1);
      return n0;
    }
    unlex(n0);
  }
  
  n0 = lex();
  if (!n0)
    return n0;
  
  if (n0->tkn==TKN_INC || n0->tkn==TKN_DEC) {
    node_t *n1 = cast_expression();
    if (n1) {
      node_append(n0, n1);
      return n0;
    }
  }
  
  if (n0->tkn==TKN_SIZEOF) {
     node_t *n1 = unary_expression();
     if (n1) {
       node_append(n0, n1);
       return n0;
     }
  }

  node_t *n1 = lex();
  if (!n1 || n1->tkn!='(') {
    unlex(n1);
    unlex(n0);
    return 0;
  }
  
  node_t *n2 = type_id();
  if (!n2) {
    unlex(n1);
    unlex(n0);
    return 0;
  }
  
  node_t *n3 = lex();
  if (!n3 || n3->tkn != ')') {
    error("expected closing ')' after sizeof (...");
  }
  
  lexfree(n1);
  node_append(n0, n2);
  lexfree(n3);
    
  return n0;
}

static node_t*
unary_operator()
{
  node_t *n = lex();
  if (!n)
    return 0;
  switch(n->tkn) {
    case '*':
    case '&':
    case '+':
    case '-':
    case '!':
    case '~':
      return n;
  }
  unlex(n);
  return 0;
}

/*
new-expression:
    ::opt new new-placementopt new-type-id new-initializeropt
    ::opt new new-placementopt ( type-id ) new-initializeropt
*/

/*
new-placement:
    ( expression-list )
*/

/*
new-type-id:
    type-specifier-seq new-declaratoropt
*/

/*
new-declarator:
    ptr-operator new-declaratoropt
    direct-new-declarator
*/

/*
direct-new-declarator:
    [ expression ]
    direct-new-declarator [ constant-expression ]
*/

/*
new-initializer:
    ( expression-listopt )
*/

/*
delete-expression:
    ::opt delete cast-expression
    ::opt delete [ ] cast-expression
*/

/*
cast-expression:
    unary-expression
    ( type-id ) cast-expression
*/
node_t* cast_expression()
{
  node_t *n0 = unary_expression();
  if (n0)
    return n0;

  n0 = lex();
  if (!n0 || n0->tkn!='(') {
    unlex(n0);
    return 0;
  }
  node_t *n1 = type_id();
  if (!n1) {
    unlex(n0);
    return 0;
  }
  node_t *n2 = lex();
  if (!n2 || n2->tkn!=')') {
    unlex(n1);
    unlex(n0);
    return 0;
  }
  node_t *n3 = cast_expression();
  if (!n3) {
    unlex(n2);
    unlex(n1);
    unlex(n0);
  }
  
  n0->tkn = TKN_CAST_EXPRESSION;
  node_append(n0, n1);
  lexfree(n2);
  node_append(n0, n3);
  return n0;
}

/*
pm-expression:
    cast-expression
    pm-expression . * cast-expression
    pm-expression -> * cast-expression
*/
node_t* pm_expression()
{
  node_t *n0 = cast_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1)
    return n0;
  if (n1->tkn!='.' && n1->tkn!=TKN_PTR) {
    unlex(n1);
    return n0;
  }
  node_t *n2 = lex();
  if (!n2) {
    unlex(n1);
    return n0;
  }
  if (n2->tkn!='*') {
    unlex(n2);
    unlex(n1);
    return n0;
  }
  node_t *n3 = pm_expression();
  if (!n3) {
    unlex(n3);
    unlex(n2);
    unlex(n1);
    return n0;
  }
  node_append(n1, n0);
  node_append(n1, n2);
  node_append(n2, n3);
  return n1;
}

/*
multiplicative-expression:
    pm-expression
    multiplicative-expression * pm-expression
    multiplicative-expression / pm-expression
    multiplicative-expression % pm-expression
*/
node_t*
multiplicative_expression()
{
  node_t *n0 = unary_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='*' || n1->tkn=='/' || n1->tkn=='%') {
    node_t *n2 = multiplicative_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
additive-expression:
    multiplicative-expression
    additive-expression + multiplicative-expression
    additive-expression - multiplicative-expression
*/
node_t*
additive_expression()
{
  node_t *n0 = multiplicative_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='+' || n1->tkn=='-') {
    node_t *n2 = additive_expression();
    if (!n2) {
      unlex(n1);
      if (trace)
        printf("additive_expression -> multiplicative_expression\n");
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    if (trace)
      printf("additive_expression -> multiplicative_expression '+' additive_expression\n");
    return n1;
  }
  if (trace)
    printf("additive_expression -> multiplicative_expression\n");
  unlex(n1);
  return n0;
}

/*
shift-expression:
    additive-expression
    shift-expression << additive-expression
    shift-expression >> additive-expression
*/
node_t*
shift_expression()
{
  node_t *n0 = additive_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if( n1->tkn==TKN_SHL ||
      n1->tkn==TKN_SHR ) 
  {
    node_t *n2 = shift_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
relational-expression:
    shift-expression
    relational-expression < shift-expression
    relational-expression > shift-expression
    relational-expression <= shift-expression
    relational-expression >= shift-expression
*/
node_t*
relational_expression()
{
  node_t *n0 = shift_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='<' ||
      n1->tkn=='>' ||
      n1->tkn==TKN_LE ||
      n1->tkn==TKN_GE ) 
  {
    node_t *n2 = relational_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
equality-expression:
    relational-expression
    equality-expression == relational-expression
    equality-expression != relational-expression
*/
node_t*
equality_expression()
{
  node_t *n0 = relational_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn==TKN_EQ ||
      n1->tkn==TKN_NEQ ) 
  {
    node_t *n2 = equality_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
and-expression:
    equality-expression
    and-expression & equality-expression
*/
node_t*
and_expression()
{
  node_t *n0 = equality_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='&') {
    node_t *n2 = and_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
exclusive-or-expression:
    and-expression
    exclusive-or-expression ^ and-expression
*/
node_t*
exclusive_or_expression()
{
  node_t *n0 = and_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='^') {
    node_t *n2 = exclusive_or_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
inclusive-or-expression:
    exclusive-or-expression
    inclusive-or-expression | exclusive-or-expression
*/
node_t*
inclusive_or_expression()
{
  node_t *n0 = exclusive_or_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn=='|') {
    node_t *n2 = inclusive_or_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
logical-and-expression:
    inclusive-or-expression
    logical-and-expression && inclusive-or-expression
*/
node_t*
logical_and_expression()
{
  node_t *n0 = inclusive_or_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn==TKN_AND) {
    node_t *n2 = logical_and_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
logical-or-expression:
    logical-and-expression
    logical-and-expression || logical-or-expression
*/
node_t*
logical_or_expression()
{
  node_t *n0 = logical_and_expression();
  if (!n0)
    return 0;
  node_t *n1 = lex();
  if (!n1) {
    return n0;
  }
  if (n1->tkn==TKN_OR) {
    node_t *n2 = logical_or_expression();
    if (!n2) {
      unlex(n1);
      return n0;
    }
    node_append(n1, n0);
    node_append(n1, n2);
    return n1;
  }
  unlex(n1);
  return n0;
}

/*
conditional-expression:
    logical-or-expression
    logical-or-expression ? expression : assignment-expression
*/
node_t*
conditional_expression()
{
  node_t *n0, *n1, *n2, *n3, *n4;
  n0 = logical_or_expression();
  if (!n0)
    return n0;
  n1 = lex();
  if (!n1)
    goto l0;
  if (n1->tkn!='?')
    goto l1;
  n2 = expression();
  if (!n2)
    goto l1;
  n3 = lex();
  if (!n3)
    goto l2;
  if (n3->tkn != ':')
    goto l3;
  n4 = assignment_expression();
  if (!n4)
    goto l3;
  
  n1->tkn = TKN_CONDITIONAL_EXPRESSION;
  node_append(n1, n0);
  node_append(n1, n2);
  lexfree(n3);
  node_append(n1, n4);
  if (trace)
    printf("conditional-expression -> logical-or-expression ? expression : assignment-expression\n");
  return n1;
  
l3:
  unlex(n3);
l2:
  unlex(n2);
l1:
  unlex(n1);
l0:
  if (trace)
    printf("conditional-expression -> logical-or-expression\n");
  return n0;
}

/*
assignment-expression:
    conditional-expression
    logical-or-expression assignment-operator assignment-expression
    throw-expression
*/
node_t*
assignment_expression()
{
  node_t *n0;
  n0 = conditional_expression();
  if (n0) {
    if (trace)
      printf("assignment_expression -> conditional_expression\n");
    return n0;
  }

  n0 = logical_or_expression();
  if (!n0)
    return 0;
  node_t *n1 = assignment_operator();
  if (!n1) {
    if (trace)
      printf("assignment_expression -> logical_or_expression\n");
    return n0;
  }

  node_t *n2 = assignment_expression();
  if (!n2) {
    unlex(n1);
    if (trace)
      printf("assignment_expression -> logical_or_expression\n");
    return n0;
  }
  node_append(n1, n0);
  node_append(n1, n2);
  if (trace)
    printf("assignment_expression -> logical_or_expression assignment_operator assignment_expression\n");
  return n1;
}

/*
assignment-operator: one of
    = *= /= %= += -= <<= >>= &= ^= |= 
*/
node_t *assignment_operator()
{
  node_t *n = lex();
  if (!n)
    return 0;
  switch(n->tkn) {
    case '=':
    case TKN_AMULT:
    case TKN_ADIV:
    case TKN_AMOD:
    case TKN_APLUS:
    case TKN_AMINUS:
    case TKN_ASHL:
    case TKN_ASHR:
    case TKN_AAND:
    case TKN_AXOR:
    case TKN_AOR:
      return n;
  }
  unlex(n);
  return 0;
}

/*
expression:
    assignment-expression
    expression , assignment-expression
*/
node_t*
expression()
{
  node_t *n0, *n1, *n2;
  n0 = n1 = n2 = 0;
  while(true) {
    n1 = assignment_expression();
    if (!n1) {
      unlex(n2);
      break;
    }
    if (!n0)
      n0 = node_new(TKN_EXPRESSION);
    node_append(n0, n1);
    n2 = lex();
    if (!n2 || n2->tkn!=',') {
      unlex(n2);
      break;
    }
  }
  return n0;
}  

/*
constant-expression:
    conditional-expression
*/
node_t*
constant_expression()
{
  return conditional_expression();
}

/*
 *
 * A.5 Statements
 *
 */

/*
statement:
    labeled-statement
    expression-statement
    compound-statement
    selection-statement
    iteration-statement
    jump-statement
    declaration-statement
    try-block
*/
node_t*
statement()
{
  node_t *n0;

  n0 = labeled_statement();
  if (n0) {
    if (trace)
      printf("statement -> labeled-statement\n");
    return n0;
  }

  n0 = expression_statement();
  if (n0) {
    if (trace)
      printf("statement -> expression-statement\n");
    return n0;
  }

  n0 = compound_statement();
  if (n0) {
    if (trace)
      printf("statement -> compound-statement\n");
    return n0;
  }

  n0 = selection_statement();
  if (n0) {
    if (trace)
      printf("statement -> selection-statement\n");
    return n0;
  }

  n0 = iteration_statement();
  if (n0) {
    if (trace)
      printf("statement -> iteration-statement\n");
    return n0;
  }

  n0 = jump_statement();
  if (n0) {
    if (trace)
      printf("statement -> jump-statement\n");
    return n0;
  }
  
  n0 = declaration_statement();
  if (n0) {
    if (trace)
      printf("statement -> declaration-statement\n");
    return n0;
  }
  
/*
  n0 = lex();
  if (n0) {
    printf("NOT A STATEMENT:\n");
    node_print(stdout, n0);
    unlex(n0);
  }
*/
  return 0;
}

/*
labeled-statement:
    identifier : statement
    case constant-expression : statement
    default : statement
*/
node_t*
labeled_statement()
{
  node_t *n0, *n1, *n2, *n3;
  n1 = n2 = n3 = 0;
  n0 = lex();
  if (!n0)
    return n0;
  switch(n0->tkn) {
    case TKN_IDENTIFIER:
      break;
    case TKN_CASE:
      n1 = constant_expression();
      if (!n1) {
        unlex(n0);
        return 0;
      }
      break;
    case TKN_DEFAULT:
      break;
    default:
      unlex(n0);
      return 0;
  }
  n2 = lex();
  if (n2 && n2->tkn==':') {
    n3 = statement();
    if (n3) {
      if (n1)
        node_append(n0, n1);
      node_append(n0, n3);
      return n0;
    }
  }
  unlex(n2);
  unlex(n1);
  unlex(n0);
  return 0;
}

/*
  expression-statement: [expression] ';'
 */
node_t*
expression_statement()
{
  node_t *n0 = expression();
  node_t *n1 = lex();
  if (!n1 || n1->tkn!=';') {
    if (n1) unlex(n1);
    if (n0) unlex(n0);
    return 0;
  }
  lexfree(n1);
  if (!n0) {
//    n0 = node_new(TKN_EXPRESSION);
    fprintf(stderr, "empty expression statement\n");
  }
  return n0;
}

/*
compound-statement:
    { statement-seqopt } 
*/
node_t*
compound_statement()
{
  node_t *n0 = lex();
  if (!n0)
    return 0;
  if (n0->tkn != '{') {
    unlex(n0);
    return 0; 
  }
  node_t *n1 = statement_seq();
  if (!n1) {
    n1 = node_new(TKN_STATEMENT_SEQ);
  }
   
  node_t *n2 = lex();
  if (!n2 || n2->tkn != '}') {
    fprintf(stderr, "compound_statement: expected statement or a closing '}'\n");
    if (n1) {
      fprintf(stderr, "after:\n");
      node_print0(stderr, n1, 1);
    }
    if (n2) {
      fprintf(stderr, "but got:\n");
      node_print0(stderr, n2, 1);
    }
    exit(1);
  }
  lexfree(n0);
  lexfree(n2);
  return n1;  
}

/*
statement-seq:
    statement
    statement-seq statement
*/
node_t*
statement_seq()
{
//printf("statement-seq\n");
  node_t *n = 0;
  while(true) {
    node_t *e = statement();
    if (!e) {
//printf("not a statement\n");
      break;
    }
//printf("  got statement:\n");
//node_print0(stdout, e, 2);
    if (!n)
      n = node_new(TKN_STATEMENT_SEQ);
    node_append(n, e);
  }
//printf("----------------------\n");
  return n;
}

/*
selection-statement:
    if ( condition ) statement
    if ( condition ) statement else statement
    switch ( condition ) statement
*/
node_t*
selection_statement()
{
  node_t *n0 = lex();
  if (n0->tkn == TKN_IF) {
    printf("selection_statement: if ...\n");
    node_t *n1 = lex();
    if (!n1 || n1->tkn!='(')
      error("expected '(' after if");
    node_t *n2 = condition();
    node_t *n3 = lex();
    if (!n3 || n3->tkn!=')') {
      printf("after\n");
      node_print0(stdout, n2, 1);
      printf("i got\n");
      node_print0(stdout, n3, 1);
      error("expected ')' after if(...");
    }
    node_t *n4 = statement();
    if (!n4)
      error("expected statement after if(...)");
    node_t *n5 = lex(), *n6;

    printf("check for else\n");
    printf("%c\n", n5->tkn);

    if (n5->tkn == TKN_ELSE) {
      n6 = statement();
      if (!n6)
        error("expected statement after if(...) ... else");
    } else {
      unlex(n5);
      n5 = 0;
    }
    if (!n2) {
      n2 = node_new(TKN_EXPRESSION);
    }

    lexfree(n1);
    node_append(n0, n2);
    lexfree(n3);
    node_append(n0, n4);
    if (n5) {
      lexfree(n5);
      node_append(n0, n6);
    }
    return n0;
  }
  unlex(n0);
  return 0;
}

/*
condition:
    expression
    type-specifier-seq declarator = assignment-expression
*/
node_t*
condition()
{
  node_t *n0 = expression();
  if (n0) {
    if (trace)
      printf("condition -> expression\n");
    return n0;
  }
  // type-specifier-seq declarator = assignment-expression
  return 0;
}

/*
iteration-statement:
    while ( condition ) statement
    do statement while ( expression ) ; 
    for ( for-init-statement conditionopt ; expressionopt ) statement
*/
node_t*
iteration_statement()
{
  node_t *n0, *n1, *n2, *n3, *n4, *n5, *n6, *n7;
  n0 = lex();
  if (!n0)
    return n0;
  n1 = n2 = n3 = n4 = n5 = n6 = n7 = 0;
  switch(n0->tkn) {
    case TKN_WHILE:
      n1 = lex();
      if (!n1 || n1->tkn!='(')
        break;
      n2 = condition();
      if (!n2)
        break;
      n3 = lex();
      if (!n3 || n3->tkn!=')')
        break;
      n4 = statement();
      if (!n4)
        break;
      lexfree(n1);
      node_append(n0, n2);
      lexfree(n3);
      node_append(n0, n4);
      return n0;
    case TKN_DO:
      n1 = statement();
      if (!n1)
        break;
      n2 = lex();
      if (!n2 || n2->tkn != TKN_WHILE)
        break;
      n3 = lex();
      if (!n3 || n3->tkn != '(')
        break;
      n4 = expression();
      if (!n4)
        break;
      n5 = lex();
      if (!n5 || n5->tkn != ')')
        break;
      node_append(n0, n1);
      lexfree(n2);
      lexfree(n3);
      node_append(n0, n4);
      lexfree(n5);
      return n0;
    case TKN_FOR:
      n1 = lex();
      if (!n1 || n1->tkn!='(')
        break;
      n2 = for_init_statement();
      if (!n2)
        break;
      n3 = condition();
      n4 = lex();
      if (!n4 || n4->tkn!=';')
        break;
      n5 = expression();
      n6 = lex();
      if (!n6 || n6->tkn!=')')
        break;
      n7 = statement();
      if (!n7)
        break;
      lexfree(n1);
      node_append(n0, n2);
      node_append(n0, n3 ? n3 : node_new(TKN_NONE));
      lexfree(n4);
      node_append(n0, n5 ? n5 : node_new(TKN_NONE));
      lexfree(n6);
      node_append(n0, n7);
      return n0;
  }
  unlex(n7);
  unlex(n6);
  unlex(n5);
  unlex(n4);
  unlex(n3);
  unlex(n2);
  unlex(n1);
  unlex(n0);
  return 0;
}

/*
for-init-statement:
    expression-statement
    simple-declaration
*/
node_t*
for_init_statement()
{
  node_t *n0;
  n0 = expression_statement();
  if (n0)
    return n0;
  return simple_declaration();
}

/*
jump-statement:
    break ;
    continue ;
    return expressionopt ; 
    goto identifier ;
*/
node_t*
jump_statement()
{
  node_t *n0, *n1, *n2;
  n1 = n2 = 0;
  n0 = lex();
  if (!n0)
    return 0;
  switch(n0->tkn) {
    case TKN_BREAK:
    case TKN_CONTINUE:
      break;
    case TKN_RETURN:
      n1 = expression();
      break;
    case TKN_GOTO:
      n1 = identifier();
      break;
    default:
      unlex(n0);
      return 0;
  }
  n2 = lex();
  if (!n2 || n2->tkn!=';') {
    unlex(n2);
    unlex(n1);
    unlex(n0);
    return 0;
  }
  if (n1)
    node_append(n0, n1);
  free(n2);
  return n0;
}

/*
declaration-statement:
    block-declaration
*/
node_t*
declaration_statement()
{
  node_t *n0 = block_declaration();
  if (n0) {
    if (trace)
      printf("declaration-statement -> block-declaration\n");
    return n0;
  }
  return 0;
}

/*
 *
 * A.6 Declarations
 *
 */

/*
declaration-seq:
    declaration
    declaration-seq declaration
*/
node_t*
declaration_seq()
{
  node_t *seq = 0;
  while(true) {
    node_t *decl = declaration();
    if (!decl)
      return seq;
    if (!seq)
      seq = node_new(TKN_DECLARATION_SEQ);
    node_append(seq, decl);
  }
}

/*
declaration:
    block-declaration
    function-definition
    template-declaration
    explicit-instantiation
    explicit-specialization
    linkage-specification
    namespace-definition
*/
node_t*
declaration()
{
  node_t *n0;
  n0 = block_declaration();
  if (n0)
    return n0;
  n0 = function_definition();
  return n0;
}

/*
block-declaration:
    simple-declaration
    asm-definition
    namespace-alias-definition
    using-declaration
    using-directive
*/
node_t*
block_declaration() 
{
  node_t *n0 = simple_declaration();
  if (n0) {
    if (trace)
      printf("block_declaration -> simple_declaration\n");
  }
  return n0;
}

/*
simple-declaration:
                  [decl-specifier-seq] [init-declarator-list] ;
*/   
node_t*
simple_declaration()
{
  node_t *n0 = decl_specifier();
   
  node_t *n1 = init_declarator_list();

  node_t *n2 = lex();
  if (!n2 || n2->tkn != ';') {
// fprintf(stderr, "%i '%c'\n", n2->tkn, n2->tkn);
    if (n2)
      unlex(n2);
    if (n1)
      unlex(n1);
    if (n0)
      unlex(n0);
    return 0;
  }
/*
printf("------------------- simple declaration -----------------\n");
if (n0) {
  printf("got decl_specifier()\n");
  node_print0(stdout, n0, 1);
}
if (n1) {
  printf("got init_declarator_list()\n");
  node_print0(stdout, n1, 1);
}
node_print(stdout, n2);
*/
  if (n0 && n1) {
    node_t *n = node_new(TKN_DECLARATOR);
/*
    // special for late binding of class name...
    if (n0->tkn==TKN_IDENTIFIER) {
      n0->tkn=TKN_CLASS_NAME;
    }
*/
    if (trace)
      printf("simple-declaration -> decl-specifier-seq init-declarator-list\n");
    node_t *nx;
    nx = node_new(TKN_DECL_SPECIFIER_SEQ);
    node_append(nx, n0);
    node_append(n, nx);
    nx = node_new(TKN_INIT_DECLARATOR_LIST);
    node_append(nx, n1);
    node_append(n, nx);
    lexfree(n2);
    return n;
  }
  lexfree(n2);
  if (n0) {
     if (trace)
       printf("simple-declaration -> decl-specifier-seq\n");
    return n0;
  }
  if (trace)
    printf("simple-declaration -> init-declarator-list\n");
  return n1;
}

/*
decl-specifier:
                  storage-class-specifier
                  type-specifier
                  function-specifier
                  'friend'
                  'typedef'
*/
node_t*
decl_specifier()
{
  node_t *n0;
  n0 = storage_class_specifier();
  if (n0)
    return n0;
  n0 = type_specifier();
  if (n0)
    return n0;
  n0 = function_specifier();
    return n0;
  n0 = lex();
  if (!n0)
    return n0;
  if (n0->tkn == TKN_FRIEND || n0->tkn == TKN_TYPEDEF)
    return n0;
  unlex(n0);
  return 0;
}

/*
decl-specifier-seq:
    decl-specifier-seqopt decl-specifier
*/
node_t* decl_specifier_seq() {
  node_t *seq = 0;
  while(true) {
    node_t *n0 = decl_specifier();
    if (!n0)
      return seq;
    if (!seq)
      seq = node_new(TKN_DECL_SPECIFIER_SEQ);
    node_append(seq, n0);
  }
}

/*
storage-class-specifier:
    auto
    register
    static
    extern
    mutable
*/
node_t*
storage_class_specifier() {
  node_t *n0 = lex();
  if (!n0)
    return n0;
  switch(n0->tkn) {
    case TKN_AUTO:
    case TKN_REGISTER:
    case TKN_STATIC:
    case TKN_EXTERN:
    case TKN_MUTABLE:
      return n0;
  }
  unlex(n0);
  return 0;
}

/*
function-specifier:
    inline
    virtual
    explicit
*/
node_t*
function_specifier() {
  node_t *n0 = lex();
  if (!n0)
    return n0;
  switch(n0->tkn) {
    case TKN_INLINE:
    case TKN_VIRTUAL:
    case TKN_EXPLICIT:
      return n0;
  }
  unlex(n0);
  return 0;
}

/*
typedef-name:
    identifier
*/

/*
type-specifier:
    simple-type-specifier
    class-specifier
    enum-specifier
    elaborated-type-specifier
    cv-qualifier
*/
node_t*
type_specifier()
{
  return simple_type_specifier();
/*
  node_t *n0 = simple_type_specifier();
  if (n0)
    return n0;

  n0 = class_specifier();
  if (n0)
    return n0;

  // special for late binding of class name...
  return identifier();
*/
}

/*
simple-type-specifier:
    ::opt nested-name-specifieropt type-name
    ::opt nested-name-specifier template template-id
    char
    wchar_t
    bool
    short
    int
    long
    signed
    unsigned
    float
    double
    void
*/
node_t*
simple_type_specifier()
{
  node_t *n0 = lex();
  if (n0) {
    if (   
      n0->tkn==TKN_CHAR ||
      n0->tkn==TKN_WCHAR_T ||
      n0->tkn==TKN_BOOL ||   
      n0->tkn==TKN_SHORT ||  
      n0->tkn==TKN_INT ||    
      n0->tkn==TKN_LONG ||   
      n0->tkn==TKN_SIGNED || 
      n0->tkn==TKN_UNSIGNED ||
      n0->tkn==TKN_FLOAT ||   
      n0->tkn==TKN_DOUBLE ||  
      n0->tkn==TKN_VOID )     
    {
      return n0;
    }
    unlex(n0);
  }
  return type_name();
}

/*
type-name:
    class-name
    enum-name
    typedef-name
*/
node_t*
type_name()
{
  return class_name();
}

/*
elaborated-type-specifier:
    class-key ::opt nested-name-specifieropt identifier
    enum ::opt nested-name-specifieropt identifier
    typename ::opt nested-name-specifier identifier
    typename ::opt nested-name-specifier templateopt template-id
*/

/*
enum-name:
    identifier
*/

/*
enum-specifier:
    enum identifieropt { enumerator-listopt }
*/

/*
enumerator-list:
    enumerator-definition
    enumerator-list , enumerator-definition
*/

/*
enumerator-definition:
    enumerator
    enumerator = constant-expression
*/

/*
enumerator:
    identifier
*/

/*
namespace-name:
    original-namespace-name
    namespace-alias

original-namespace-name
    identifier

namespace-definition:
    named-namespace-definition
    unnamed-namespace-definition

named-namespace-definition:
    original-namespace-definition
    extension-namespace-definition

original-namespace-definition:
    namespace identifier { namespace-body } 

extension-namespace-definition:
    namespace original-namespace-name { namespace-body } 

unnamed-namespace-definition:
    namespace { namespace-body } 

namespace-body:
    declaration-seqopt

namespace-alias:
    identifier

namespace-alias-definition:
    namespace identifier = qualified-namespace-specifier ; 

qualified-namespace-specifier:
    ::opt nested-name-specifieropt namespace-name

using-declaration:
    using typenameopt ::opt nested-name-specifier unqualified-id ; 
    using :: unqualified-id ; 

using-directive:
    using namespace ::opt nested-name-specifieropt namespace-name ; 

asm-definition:
    asm { string-literal } 

linkage-specification:
    extern string-literal { declaration-seqopt } 
    extern string-literal declaration 
*/

/*
 *
 * A.7 Declarators
 *
 */

/*
init-declarator-list:
    init-declarator
    init-declarator-list init-declarator
*/
node_t*
init_declarator_list()
{
  node_t *n0 = 0;
  while(true) {
    node_t *n1 = init_declarator();
    if (!n1)
      break;
    if (!n0)
      n0 = n1;
    else
      node_append_next(n0, n1);
    node_t *n2 = lex();
    if (!n2 || n2->tkn!=',') {
      unlex(n2);
      break;
    }
    lexfree(n2);
  }
//printf("return init-declarator-list\n");
//node_print(stdout, n0);
  return n0;
}

/*
init-declarator:
    declarator initializeropt
*/
node_t*
init_declarator()
{
  return declarator();
}

/*
declarator:
    direct-declarator
    ptr-operator declarator
*/
node_t*
declarator()
{
  return direct_declarator();
}

/*
direct-declarator:
    declarator-id
    direct-declarator ( parameter-declaration-clause ) cv-qualifier-seqopt exception-specificationopt
    direct-declarator [ constant-expressionopt ] 
    ( declarator )
*/
node_t*
direct_declarator()
{
  return declarator_id();
}

/*
ptr-operator:
    * cv-qualifier-seqopt
    & 
    ::opt nested-name-specifier * cv-qualifier-seqopt
*/
node_t*
ptr_operator()
{
  node_t *n0 = lex();
  if (n0 && (n0->tkn == '*' || n0->tkn == '&'))
    return n0;
  if (n0)
    unlex(n0);
  return 0;
}

/*
cv-qualifier-seq:
    cv-qualifier cv-qualifier-seqopt
*/

/*
cv-qualifier:
    const
    volatile
*/

/*
declarator-id:
    ::opt nested-name-specifieropt type-name
*/
node_t*
declarator_id()
{
  return id_expression();
}

/*
type-id:
    type-specifier-seq [abstract-declarator]
*/
node_t*
type_id()
{
  node_t *n0 = type_specifier_seq();
  if (!n0)
    return 0;
  node_t *n1 = abstract_declarator();
  if (!n1) {
    if (trace)
      printf("typeid -> type-specifier-seq");
    return n0;
  }
  if (trace)
    printf("typeid -> type-specifier-seq abstract-declarator");
  node_append(n0, n1);
  return n0;
}

/*
type-specifier-seq:
    type-specifier [type-specifier-seq]
*/
node_t*
type_specifier_seq()
{
  node_t *n0 = 0;
  while(true) {
    node_t *n1 = type_specifier();
    if (!n1)
      break;
    if (!n0)
      n0 = n1;
    else
      node_append_next(n0, n1);
  }
  return n0;
}

/*
abstract-declarator:
    ptr-operator [abstract-declarator]
    direct-abstract-declarator
*/
node_t*
abstract_declarator()
{
  node_t *n0;
  
  n0 = ptr_operator();
  if (n0) {
    node_t *n1 = abstract_declarator();
    if (n1)
      node_append(n0, n1);
    return n0;
  }
  
  return direct_abstract_declarator();
}

/*
direct-abstract-declarator:
    [direct-abstract-declarator] '(' parameter-declaration-clause ')' [cv-qualifier-seq] [exception-specification]
    [direct-abstract-declarator] '[' [constant-expression] ']' 
    ( abstract-declarator ) 
*/
node_t*
direct_abstract_declarator()
{
  return 0;
}

/*
parameter-declaration-clause:
    parameter-declaration-listopt ...opt
    parameter-declaration-list , ...
aka    
    none
    ...
    parameter-declaration-list
    parameter-declaration-list ...
    parameter-declaration-list , ...
*/
node_t*
parameter_declaration_clause() {
  node_t *list = parameter_declaration_list();
  node_t *n1 = lex();
  if (!list) {
    // none
    if (!n1)
      return node_new(TKN_PARAMETER_DECLARATION_LIST);
    // ...
    if (n1->tkn == TKN_ELLIPSIS) {
      list = node_new(TKN_PARAMETER_DECLARATION_LIST);
      node_append(list, n1);
      return list;
    }
    unlex(n1);
    return 0;
  }
  // parameter-declaration-list ...
  if (n1 && n1->tkn == TKN_ELLIPSIS) {
    node_append(list, n1);
    return list;
  }
  // parameter-declaration-list , ...
  if (n1 && n1->tkn == ',') {
    node_t *n2 = lex();
    if (n2 && n2->tkn == TKN_ELLIPSIS) {
      free(n1);
      node_append(list, n2);
      return list;
    }
    unlex(n2);
  }
  // parameter-declaration-list
  unlex(n1);
  return list;
}

/*
parameter-declaration-list:
    parameter-declaration
    parameter-declaration-list , parameter-declaration
*/
node_t*
parameter_declaration_list() {
  node_t *list = 0;
  while(true) {
    node_t *n0 = parameter_declaration();
    if (!n0)
      return list;
    if (!list)
      list = node_new(TKN_PARAMETER_DECLARATION_LIST);
    node_append(list, n0);
    node_t *n1 = lex();
    if (!n1)
      return list;
    if (n1->tkn != ',') {
      unlex(n1);
      return list;
    }
    free(n1);
  }
}

/*
parameter-declaration:
    decl-specifier-seq declarator
    decl-specifier-seq declarator = assignment-expression
    decl-specifier-seq abstract-declaratoropt
    decl-specifier-seq abstract-declaratoropt = assignment-expression

    aka.    
    decl-specifier-seq [declarator|abstract-declarator] [= assignment-expression]
*/
node_t*
parameter_declaration() {
  node_t *n0 = decl_specifier_seq();
  if (!n0)
    return n0;
  
  node_t *n1 = declarator();
  if (!n1) {
    n1 = abstract_declarator();
  }
  if (!n1)
    n1 = node_new(TKN_NONE);
  node_append(n0, n1);

  node_t *n2 = lex();
  if (n2 || n2->tkn == '=') {
    node_t *n3 = assignment_expression();
    if (n3) {
      free(n2);
      node_append(n0, n3);
    } else {
      unlex(n2);
    }
  } else {
    unlex(n2);
  }
  return n0;
}

/*
function-definition:
    decl-specifier-seqopt declarator ctor-initializeropt function-body
    decl-specifier-seqopt declarator function-try-block
*/

/*
function-definition:
   [decl-specifier-seq] declarator [ctor-initializer] function-body
   [decl-specifier-seq] declarator function-try-block
 */
node_t*
function_definition()
{
  node_t *n0 = decl_specifier_seq();

  node_t *n1 = lex();
  if (!n1 || n1->tkn != TKN_IDENTIFIER) {
    unlex(n1);
    unlex(n0);
    return 0;
  }
  
  node_t *n2 = lex();
  if (!n2 || n2->tkn != '(') {
    unlex(n2);
    unlex(n1);
    unlex(n0);
    return 0;
  }
  
  node_t *n3 = parameter_declaration_clause();

  node_t *n4 = lex();
  if (!n4 || n4->tkn != ')') {
    unlex(n4);
    unlex(n2);
    unlex(n1);
    unlex(n0);
    return 0;
  }
  node_t *n5 = function_body();
  if (!n5)
    error("expected function body after function definition\n");
  lexfree(n2);
  lexfree(n4);
  n1->tkn = TKN_FUNCTION;
  if (!n0) {
    n0 = node_new(TKN_DECL_SPECIFIER_SEQ);
    node_append(n0, node_new(TKN_INT));
  }
  if (!n3)
    n3 = node_new(TKN_PARAMETER_DECLARATION_LIST);
  node_append(n1, n0);  // return type
  node_append(n1, n3);  // parameter_declaration
  node_append(n1, n5);	// body
  return n1;
}

/*
function-body:
    compound-statement
*/
node_t*
function_body()
{
  return compound_statement();
}

/*
initializer:
    = initializer-clause ( expression-list ) 

initializer-clause:
    assignment-expression
    { initializer-list ,opt } 
    { } 

initializer-list:
    initializer-clause
    initializer-list , initializer-clause 
*/

/*
 *
 * A.8 Classes
 *
 */
node_t*
class_name()
{
/*
  node_t *n0 = lex();
  if (n0 && n0->tkn == TKN_CLASS_NAME)
    return n0;
  unlex(n0);  
*/
  return 0;   
}

/*
class-name:
    identifier
    template-id
*/
/*
class-specifier:
    class-head { member-specificationopt } 

class-head:
    class-key identifieropt base-clauseopt
    class-key nested-name-specifieropt identifier base-clauseopt
    class-key nested-name-specifieropt template-id base-clauseopt

class-key:
    class 
    struct 
    union 

member-specification:
    member-declaration member-specificationopt
    access-specifier : member-specificationopt

member-declaration:
    decl-specifier-seqopt member-declarator-listopt ; 
    function-definition ;opt
    ::opt nested-name-specifier templateopt unqualified-id ; 
    using-declaration
    template-declaration

member-declarator-list:
    member-declarator
    member-declarator-list , member-declarator

member-declarator:
    declarator pure-specifieropt
    declarator constant-initializeropt
    identifieropt : constant-expression

pure-specifier:
    = 0 

constant-initializer:
    = constant-expression

A.9 Derieved classes

base-clause:
    : base-specifier-list

base-specifier-list:
    base-specifier
    base-specifier-list , base-specifier

base-specifier:
    ::opt nested-name-specifieropt class-name
    virtual access-specifieropt ::opt nested-name-specifieropt class-name
    access-specifieropt virtualopt ::opt nested-name-specifieropt class-name

access-specifier:
    private
    protected
    public

A.10 Special member functions

conversion-function-id:
    operator conversion-type-id

conversion-type-id:
    type-specifier-seq conversion-declaratoropt

conversion-declarator:
    ptr-operator conversion-declaratoropt

ctor-initializer:
    : mem-initializer-list

mem-initializer-list:
    mem-initializer
    mem-initializer-list , mem-initializer

mem-initializer:
    mem-initializer-id { expression-listopt } 

mem-initializer-id:
    ::opt nested-name-specifieropt class-name
    identifier

A.11 Overloading

operator-function-id:
    operator operator

operator: one of
    new delete new[] delete[]
    + - * / % ^ & | ~
    ! = < > += -= *= /= %=
    ^= &= |= << >> <<= >>= == !=
    <= >= && || ++ -- , ->* -> 
    () []

A.12 Templates

template-declaration:
    exportopt template < template-parameter-list > declaration

template-parameter-list:
    template-parameter
    template-parameter-list , template-parameter

template-parameter:
    type-parameter
    parameter-declaration

type-parameter:
    class identifieropt
    class identifieropt = type-id
    typename identifieropt
    typename identifieropt = type-id
    template < template-parameter-list > class identifieropt
    template < template-parameter-list > class identifieropt = id-expression

template-id:
    template-name < template-argument-listopt > class 

template-name:
    identifier

template-argument-list:
    template-argument
    template-argument-list , template-argument

template-argument:
    assignment-expression
    type-id
    id-expression

explicit-instantiation:
    template declaration

explicit-specialization:
    template < > declaration

A.13 Exception handling

try-block:
    try compound-statement handler-seq

function-try-block:
    try ctor-initializeropt function-body-handler-seq

handler-seq:
    handler handler-seqopt

handler:
    catch { exception-declaration } compound-statement

exception-declaration:
    type-specifier-seq declarator
    type-specifier-seq abstract-declarator
    type-specifier-seq
    ... 

throw-expression:
    throw assignment-expressionopt

exception-specification:
    throw ( type-id-listopt ) 

type-id-list:
    type-id
    type-id-list , type-id 
*/
