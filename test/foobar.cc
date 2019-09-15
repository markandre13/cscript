#include <lex.hh>
#include "fmemopen.h"
#include "gtest.h"
#include <map>
#include <string>
#include <functional>

using namespace std;

class Scope {
    map<string, node_t*> functions;
    map<string, std::function<node_t*(node_t*)>> native_functions;
  public:
    void insert(node_t*);
    void native(const string name, std::function<node_t*(node_t*)> cb);
    node_t* call(const string name);
  protected:
    node_t* eval(node_t*);
};

static Scope* test(const char *source) {
        printf("--------- parse --------\n");
        auto in = fmemopen((void*)source, strlen(source), "r");
        auto root = parse(in);
        fclose(in);

        printf("--------- print --------\n");
        node_print(stdout, root);

        printf("----- pretty print -----\n");
        node_pretty_print(stdout, root);

        auto scope = new Scope();
        scope->insert(root);

        return scope;
}

namespace {

    TEST(Basics, Empty) {
        auto scope = test(R"(int main(unsigned int a, int b)
{
  // return a + b;
  println("hello", 7);
  return 3 + 7;
}
)");
        printf("------- evaluate -------\n");
        scope->native("println", [](node_t *args) {
          for(node_t *p = args; p; p=p->next) {
            printf("%s", p->text);
            if (p->next)
              printf(" ");
          }
          printf("\n");
          return nullptr;
        });
        
        node_t *result = scope->call("main");
        node_pretty_print(stdout, result);
        printf("\n");
    }

}

void
Scope::insert(node_t *node) {
  assert(node->tkn == TKN_DECLARATION_SEQ);
  for (node_t *p = node->down; p; p=p->next) {
    assert(p->tkn == TKN_FUNCTION);
    functions[p->text] = p;
  }
}

void
Scope::native(const string name, std::function<node_t*(node_t*)> cb) {
  native_functions[name] = cb;
}

node_t*
Scope::call(const string name) {
  node_t *n0 = node_new(TKN_FUNCTION_CALL);
  node_t *n1 = node_new(TKN_IDENTIFIER);
  n1->text = (char*)name.c_str();
  node_append(n0, n1);
  return eval(n0);
}

node_t*
Scope::eval(node_t *node) {
  switch(node->tkn) {
    case TKN_FUNCTION_CALL: {
      string identifier = node->down->text;
      
      auto native_fun = native_functions.find(identifier);
      if (native_fun != native_functions.end()) {
        return native_fun->second(node->down->next->down);
      }
    
      auto fun = functions.find(identifier);
      if (fun == functions.end()) {
        fprintf(stderr, "unknown function '%s'\n", node->down->text);
        exit(1);
      }
      auto body = fun->second->down->next->next;
      return eval(body);
      // return type
      // parameter list
      // body
    } break;
    case TKN_STATEMENT_SEQ:
      for(node_t *p = node->down; p; p=p->next) {
        auto result = eval(p);
        if (p->tkn == TKN_RETURN)
          return result;
      }
      break;
    case TKN_EXPRESSION:
      return eval(node->down);
    case TKN_RETURN:
      return eval(node->down);
    case TKN_VALUE_INT:
    case TKN_VALUE_DOUBLE:
      return node;
    case '+': {
      auto n0 = eval(node->down);
      auto n1 = eval(node->down->next);
      if (n0->tkn == TKN_VALUE_INT && n1->tkn == TKN_VALUE_INT) {
        node_t *n = node_new(TKN_VALUE_INT);
        n->value.i = n0->value.i + n1->value.i;
        return n; // FIXME: memory leak
      }
      fprintf(stderr, "no code to evaluate + node\n");
      node_print(stderr, node);
      exit(1);
    } break;
    default:
      fprintf(stderr, "no code to evaluate node\n");
      node_print(stderr, node);
      exit(1);
  }
  return nullptr;
}
