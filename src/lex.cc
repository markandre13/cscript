#include "lex.hh"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct {
  token_e tkn;
  const char *keyword;
} keyword_t;

static keyword_t keywords[] = {
  { TKN_CLASS, "class" },
  { TKN_STRUCT, "struct"},
  { TKN_ENUM, "enum" },
  { TKN_FOR, "for" },
  { TKN_IF, "if" },
  { TKN_ELSE, "else" },
  { TKN_WHILE, "while" },
  { TKN_DO, "do" },
  { TKN_SWITCH, "switch" },
  { TKN_CASE, "case" },
  { TKN_DEFAULT, "default" },
  { TKN_BREAK, "break" },
  { TKN_CONTINUE, "continue" },
  { TKN_GOTO, "goto" },
  { TKN_RETURN, "return" },
  { TKN_PRIVATE, "private" },
  { TKN_PROTECTED, "protected" },
  { TKN_PUBLIC, "public" },
  { TKN_SIZEOF, "sizeof" },
  { TKN_THIS, "this" },
  { TKN_TRUE, "true" },
  { TKN_FALSE, "false" },
  { TKN_FRIEND, "friend" },
  { TKN_TYPEDEF, "typedef" },
  { TKN_AUTO, "auto" },
  { TKN_REGISTER, "register" },
  { TKN_STATIC, "static" },
  { TKN_EXTERN, "extern" },
  { TKN_MUTABLE, "mutable" },
  { TKN_INLINE, "inline" },
  { TKN_VIRTUAL, "virtual" },
  { TKN_EXPLICIT, "explicit" },
  { TKN_WCHAR_T, "wchar_t"},
  { TKN_BOOL, "bool" },
  { TKN_SHORT, "short" },
  { TKN_INT, "int" },
  { TKN_LONG, "long" },
  { TKN_SIGNED, "signed" },
  { TKN_UNSIGNED, "unsigned" },
  { TKN_FLOAT, "float" },
  { TKN_DOUBLE, "double" },
  { TKN_VOID, "void" },
  { TKN_NONE, NULL }
};

static token_e
keywordByName(const char *name)
{
  for (keyword_t *p = keywords; p->keyword; ++p) {
    if (strcmp(name, p->keyword)==0)
      return p->tkn;
  }
  return TKN_NONE;
}

static const char*
keywordByToken(token_e tkn)
{
  if (tkn<=255) {
    static char buffer[2];
    buffer[0] = tkn;
    buffer[1] = 0;
    return buffer;
  }
  switch(tkn) {
    case TKN_ELLIPSIS:return "...";
    case TKN_COL_COL: return "::";
    case TKN_OR:      return "||";
    case TKN_AND:     return "&&";
    case TKN_INC:     return "++";
    case TKN_DEC:     return "--";
    case TKN_EQ:      return "==";
    case TKN_NEQ:     return "!=";
    case TKN_GE:      return ">=";
    case TKN_LE:      return "<=";
    case TKN_SHR:     return ">>";
    case TKN_SHL:     return "<<";
    case TKN_PTR:     return "->";
    case TKN_APLUS:   return "+=";
    case TKN_AMINUS:  return "-=";
    case TKN_AMULT:   return "*=";
    case TKN_ADIV:    return "/=";
    case TKN_AMOD:    return "%=";
    case TKN_AXOR:    return "^=";
    case TKN_AAND:    return "&=";
    case TKN_AOR:     return "|=";
    case TKN_ASHL:    return "<<=";
    case TKN_ASHR:    return ">>=";
    default:
      ;
  }
  for (keyword_t *p = keywords; p->keyword; ++p) {
    if (p->tkn==tkn)
      return p->keyword;
  }

  return NULL;
}

static inline const char* keywordByToken(int tkn) { return keywordByToken(static_cast<token_e>(tkn)); }

FILE* lex_in = 0;
static node_t* lexstack[10];
static size_t lex_sp=0;
static char* yytext=0;
static size_t yytext_size, yytext_capacity=0;

static inline void yyput(int c) {
  if (yytext_size+1>=yytext_capacity) {
    if (yytext_capacity == 0)
      yytext_capacity = 128;
    else
      yytext_capacity <<= 1;
    yytext = (char*)realloc(yytext, yytext_capacity);
  }
  yytext[yytext_size] = c;
  yytext[++yytext_size] = 0;
}

node_t*
lex0()
{
  if (lex_sp > 0)
    return lexstack[--lex_sp];

  yytext_size = 0;
  if (yytext)
    yytext[0]=0;
//  printf("lex\n");
  unsigned state = 0;
  bool loop = true;
  do {
    int c = getc(lex_in);
//printf("lex: %c (%i) [%u]\n", c>=32?c:'.', c, state);
    if (c==EOF)
      loop = false;
    switch(state) {
      case 0: // skip space
        switch(c) {
          case ';':
          case '(':
          case ')':
          case '{':
          case '}':
          case ',':
            yyput(c);
            return node_new(c);
          case '|': state = 10; break;
          case '&': state = 11; break;
          case '"': state = 12; break;
          case '+': state = 16; break;
          case '-': state = 17; break;
          case '=': state = 18; break;
          case ':': state = 19; break;
          case '<': state = 20; break;
          case '>': state = 21; break;
          case '!': state = 22; break;
          case '*': state = 23; break;
          case '/': state = 24; break;
          case '%': state = 25; break;
          case '^': state = 26; break;
          case '.': state = 27; break;
          case EOF:
            break;
          default:
            if (isalpha(c)) {
              yyput(c);
              state = 14;
            } else
            if (isdigit(c)) {
              state = 15;
              yyput(c);
            } else
            if (!isspace(c)) {
              fprintf(stderr, "error: unexpected '%c' (%i)\n", c, c);
              exit(EXIT_FAILURE);
            }
        }
        break;
      case 16: // +
        switch(c) {
          case '+': return node_new(TKN_INC);
          case '=': return node_new(TKN_APLUS);
        }
        ungetc(c, lex_in);
        return node_new('+');
      case 17: // -
        switch(c) {
          case '-': return node_new(TKN_DEC);
          case '=': return node_new(TKN_AMINUS);
          case '>': return node_new(TKN_PTR);
        }
        ungetc(c, lex_in);
        return node_new('-');
      case 18: // =
        if (c=='=')
          return node_new(TKN_EQ);
        ungetc(c, lex_in);
        return node_new('=');
      case 19: // :
        if (c==':')
          return node_new(TKN_COL_COL);
        ungetc(c, lex_in);
        return node_new(':');
      case 20: // <
        switch(c) {
          case '<': state=30; break;
          case '=': return node_new(TKN_LE);
          default:
            ungetc(c, lex_in);
            return node_new('<');
        }
        break;
      case 30: // <<
        if (c=='=')
          return node_new(TKN_ASHL);
        ungetc(c, lex_in);
        return node_new(TKN_SHL);
      case 21: // >
        switch(c) {
          case '>': state=31; break;
          case '=': return node_new(TKN_GE);
          default:
            ungetc(c, lex_in);
            return node_new('>');
        }
        break;
      case 31:
        if (c=='=')
          return node_new(TKN_ASHR);
        ungetc(c, lex_in);
        return node_new(TKN_SHR);
      case 22: // !
        if (c=='=')
          return node_new(TKN_NEQ);
        ungetc(c, lex_in);
        return node_new('!');
      case 23: // *
        if (c=='=')
          return node_new(TKN_AMULT);
        ungetc(c, lex_in);
        return node_new('*');
      case 24: // /
        if (c=='=')
          return node_new(TKN_ADIV);
        if (c=='*') {
          state = 40;
          break;
        }
        if (c=='/') {
          state = 42;
          break;
        }
        ungetc(c, lex_in);
        return node_new('/');
        
      case 40: // /*...
        if (c=='*')
          state = 41;
        break;
      case 41: // /*...*
        if (c=='/')
          state = 0;
        else if (c!='*')
          state = 40;
        break;
      case 42: // //...
        if (c=='\n')
          state = 0;
        break;
        
      case 25: // %
        if (c=='=')
          return node_new(TKN_AMOD);
        ungetc(c, lex_in);
        return node_new('%');
      case 26: // ^
        if (c=='=')
          return node_new(TKN_AXOR);
        ungetc(c, lex_in);
        return node_new('^');
      case 27: // .
        if (c=='.')
          state = 28;
        ungetc(c, lex_in);
        return node_new('.');
      case 28: // ..
        if (c=='.')
          return node_new(TKN_ELLIPSIS);
        fprintf(stderr, "error: unexpected '..%c' (%i)\n", c, c);
        exit(EXIT_FAILURE);        
      case 10: // |
        switch(c) {
          case '|': return node_new(TKN_OR);
          case '=': return node_new(TKN_AOR);
        }
        ungetc(c, lex_in);
        return node_new('|');
      case 11: // &
        switch(c) {
          case '&': return node_new(TKN_AND);
          case '=': return node_new(TKN_AAND);
        }
        ungetc(c, lex_in);
        return node_new('&');
      case 12: // string
        switch(c) {
          case '"':
            return node_new_txt(TKN_STRING, yytext);
          case '\\':
            yyput(c);
            state = 13;
            break;
          default:
	    yyput(c);
        }
        break;
      case 13:
        yyput(c);
        state = 12;
        break;
      case 14: // identifier
        if (isalnum(c) || c=='_') {
          yyput(c);
        } else {
          ungetc(c, lex_in);
          token_e tkn = keywordByName(yytext);
          if (tkn!=TKN_NONE)
            return node_new(tkn);
/*
          if (world.type.find(yytext) != world.type.end()) {
            return node_new_txt(TKN_CLASS_NAME, yytext);
          }
*/
//fprintf(stderr, "new identifier '%s'\n", yytext);
          return node_new_txt(TKN_IDENTIFIER, yytext);
        }
        break;  
      case 15: // decimal
        if (isdigit(c)) {
          yyput(c);
        } else {
          ungetc(c, lex_in);
	  return node_new_int(yytext);
        }
        break;  
    }
  } while(loop);
  if (state==12 || state==13) {
    fprintf(stderr, "unexpected EOF in string\n");
    exit(EXIT_FAILURE);
  }
  if (state==14) {
    return node_new_txt(TKN_IDENTIFIER, yytext);
  }
//  printf("EOF\n");
  return 0;
}

node_t *lex()
{
  node_t *n = lex0();

//  printf("lex:");
//  if (n)
//    node_print0(stdout, n, 1);

  return n;
}

void   
lexfree(node_t *n)
{
  if (!n)
    return;
  if (n->text)
    free(n->text);
  if (n->down) {
    fprintf(stderr, "ERROR: lexfree for node containing child\n");
    exit(EXIT_FAILURE);
  }
  if (n->next) {
    fprintf(stderr, "ERROR: lexfree for node containing sibling\n");
    exit(EXIT_FAILURE);
  }
  free(n);
}

void
unlex(node_t *n)   
{
  if (!n)
    return;

//  printf("unlex:");
//  node_print0(stdout, n, 1);

  if (n->down) {
    unlex(n->down);
    n->down = 0;
  }
  if (n->next) {
    unlex(n->next);
    n->next = 0;
  }
  if (lex_sp>=10) {
    fprintf(stderr, "lex stack overflow\n");
    exit(EXIT_FAILURE);
  }
  lexstack[lex_sp++] = n;
}

node_t*
node_new(token_e tkn)
{
  node_t *o = (node_t*)malloc(sizeof(node_t));
  o->tkn = tkn;
  o->text = NULL;
  o->next = o->down = NULL;
  return o;
}

node_t*
node_new_txt(token_e tkn, const char *txt)
{
  node_t *o = (node_t*)malloc(sizeof(node_t));
  o->tkn = tkn;
  o->text = strdup(txt);
  o->next = o->down = NULL;
  return o;
}

node_t*
node_new_int(const char *txt) {
  node_t *n = node_new_txt(TKN_VALUE_INT, txt);
  n->value.i = atoi(txt);
  return n;
}

node_t*
node_new_double(const char *txt) {
  node_t *n = node_new_txt(TKN_VALUE_DOUBLE, txt);
  n->value.d = atof(txt);
  return n;
}

node_t*
node_new_value(int value) {
  node_t *n = node_new(TKN_VALUE_INT);
  n->value.i = value;
  return n;
}

node_t*
node_new_value(double value) {
  node_t *n = node_new(TKN_VALUE_DOUBLE);
  n->value.i = value;
  return n;
}


void
node_append(node_t *n0, node_t *n1)
{
  if (!n0->down) {
    n0->down = n1;
    return;
  }
  node_t *p = n0->down;
  while(p->next)
    p = p->next;
  p->next = n1;
}

void
node_append_next(node_t *n0, node_t *n1)
{
  if (!n0->next) {
    n0->next = n1;
    return;
  }
  node_t *p = n0->next;
  while(p->next)
    p = p->next;
  p->next = n1;
}

void
print_indent(FILE *out, unsigned indent) {
  while(indent) {
    fprintf(out, "  ");
    --indent;
  }
}

void
node_print0(FILE *out, node_t *n, unsigned depth)
{
  for(unsigned i=0; i<depth; ++i) fprintf(out, "  ");
  fprintf(out, "[%p] ", n);
  if (!n)
    return;
  switch(n->tkn) {
    case TKN_NONE:
      fprintf(out, "none\n");
      break;
    case TKN_STATEMENT_SEQ:
      fprintf(out, "statement-seq\n");
      break;
    case TKN_EXPRESSION:
      fprintf(out, "expression\n");
      break;
    case TKN_EXPRESSION_LIST:
      fprintf(out, "expression-list\n");
      break;
    case TKN_DECL_SPECIFIER_SEQ:
      fprintf(out, "decl-specifier-seq\n");
      break;
    case TKN_INIT_DECLARATOR_LIST:
      fprintf(out, "init-declarator-list\n");
      break;
    case TKN_PARAMETER_DECLARATION_LIST:
      fprintf(out, "parameter-declaration-list\n");
      break;
    case TKN_DECLARATION_SEQ:
      fprintf(out, "declaration-seq\n");
      break;
    case TKN_DECLARATOR:
      fprintf(out, "declarator\n");
      break;
    case TKN_PARAMETER_DECLARATION:
      fprintf(out, "parameter-declaration\n");
      break;

    case TKN_IDENTIFIER:
      fprintf(out, "id \"%s\"\n", n->text);
      break;
    case TKN_FUNCTION:
      fprintf(out, "function %s(...)\n", n->text);
      break;
    case TKN_FUNCTION_CALL:
      fprintf(out, "call function '%s'\n", n&&n->down&&n->down->text ? n->down->text : ("null"));
      break;
    case TKN_CLASS_NAME:
      fprintf(out, "class-name %s\n", n->text);
      break;
    case TKN_VALUE_INT:
      fprintf(out, "value-int %i\n", n->value.i);
      break;
    case TKN_VALUE_DOUBLE:
      fprintf(out, "value-double %f\n", n->value.d);
      break;
    case TKN_STRING:
      fprintf(out, "string \"%s\"\n", n->text);
      break;
    default: {
      const char *name = keywordByToken(n->tkn);
      if (name) {
        fprintf(out, "%s\n", name);
      } else {
        if (n->tkn<=255) {
          fprintf(out, "token '%c'\n", n->tkn);
        } else {
          fprintf(out, "token %i\n", n->tkn);
        }
      }
    } break;
  }
  if (n->down) node_print0(out, n->down, depth+1);
  if (n->next) node_print0(out, n->next, depth);
}

void
node_pretty_print(FILE *out, node_t *n, unsigned indent)
{
  assert(n);
  switch(n->tkn) {
    case TKN_NONE:
      print_indent(out, indent);
      fprintf(out, "none\n");
      break;
    case TKN_DECLARATION_SEQ:
      for (node_t *p = n->down; p; p=p->next) {
        node_pretty_print(out, p, indent);
      }
      break;
    case TKN_STATEMENT_SEQ:
      for (node_t *p = n->down; p; p=p->next) {
        node_pretty_print(out, p, indent);
      }
      break;
    case TKN_EXPRESSION:
//      node_print(out, n);
      assert(n->down);
      node_pretty_print(out, n->down, indent);
      break;
    case TKN_EXPRESSION_LIST:
      for (node_t *p = n->down; p; p=p->next) {
        node_pretty_print(out, p, indent);
        if (p->next)
          fprintf(out, ", ");
      }
      break;
    case TKN_DECLARATOR:
      assert(n->down->tkn == TKN_DECL_SPECIFIER_SEQ);
      node_pretty_print(out, n->down->down, indent);
      assert(n->down->next->tkn == TKN_INIT_DECLARATOR_LIST);
      fprintf(out, " ");
      for (node_t *p = n->down->next->down; p; p=p->next) {
        node_pretty_print(out, p, indent);
        if (p->next)
          fprintf(out, ", ");
      }
      // node_print(out, n);
      break;
    case TKN_PARAMETER_DECLARATION_LIST:
      for(node_t *p = n->down; p; p=p->next) {
        assert(p->tkn == TKN_DECL_SPECIFIER_SEQ);
        node_pretty_print(out, p, indent);
        if (p->next)
          fprintf(out, ", ");
      }
      break;
    case TKN_DECL_SPECIFIER_SEQ:
      for(node_t *p = n->down; p; p=p->next) {
        node_pretty_print(out, p, indent);
        if (p->next) {
          switch(p->next->tkn) {
            case TKN_VALUE_INT:
            case TKN_VALUE_DOUBLE:
              fprintf(out, " = ");
              break;
            default:
              fprintf(out, " ");
          }
        }
      }
      break;
    case TKN_PARAMETER_DECLARATION:
      fprintf(out, "parameter-declaration\n");
      break;
    case TKN_IDENTIFIER:
      fprintf(out, "%s", n->text);
      break;
    case TKN_FUNCTION:
      assert(n->down->tkn == TKN_DECL_SPECIFIER_SEQ);
      assert(n->down->next->tkn == TKN_PARAMETER_DECLARATION_LIST);
      assert(n->down->next->next->tkn == TKN_STATEMENT_SEQ);
      node_pretty_print(out, n->down, indent+1);
      fprintf(out, " %s(", n->text);
      node_pretty_print(out, n->down->next, indent+1);
      fprintf(out, ") {\n");
      node_pretty_print(out, n->down->next->next, indent+1);
      fprintf(out, "}\n");
      break;
    case TKN_FUNCTION_CALL:
      print_indent(out, indent);
      fprintf(out, "%s(", n->down->text);
      node_pretty_print(out, n->down->next, indent);
      fprintf(out, ");\n");
      break;
    case TKN_IF:
      print_indent(out, indent);
      fprintf(out, "if (");
      node_pretty_print(out, n->down, indent);
      fprintf(out, ") {\n");
      node_pretty_print(out, n->down->next, indent+1);
      if (n->down->next->next) {
        print_indent(out, indent);
        fprintf(out, "} else {\n");
        node_pretty_print(out, n->down->next->next, indent+1);
      }
      print_indent(out, indent);
      fprintf(out, "}\n");
      break;
    case TKN_RETURN:
      print_indent(out, indent);
      fprintf(out, "return (");
      node_pretty_print(out, n->down, indent);
      fprintf(out, ");\n");
      break;
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case TKN_SHL:
    case TKN_SHR:
    case TKN_EQ:
    case TKN_NEQ:
    case TKN_LE:
    case TKN_GE:
    case '<':
    case '>':
       node_pretty_print(out, n->down);
       fprintf(out, "%s", keywordByToken(n->tkn));
       node_pretty_print(out, n->down->next, indent);
       break;
    case TKN_CLASS_NAME:
      fprintf(out, "%s\n", n->text);
      break;
    case TKN_VALUE_INT:
      fprintf(out, "%i", n->value.i);
      break;
    case TKN_VALUE_DOUBLE:
      fprintf(out, "%f", n->value.d);
      break;
    case TKN_STRING:
      fprintf(out, "\"%s\"", n->text);
      break;
    case TKN_INT:
    case TKN_UNSIGNED:
    case TKN_FLOAT:
    case TKN_DOUBLE:
    case TKN_TRUE:
    case TKN_FALSE:
      fprintf(out, "%s", keywordByToken(n->tkn));
      break;
    default: {
      const char *name = keywordByToken(n->tkn);
      if (name) {
        fprintf(out, "token: %s", name);
      } else {
        if (n->tkn<=255) {
          fprintf(out, "token '%c'\n", n->tkn);
        } else {
          fprintf(out, "token %i\n", n->tkn);
        }
      }
      exit(1);
    } break;
  }
}
