#include <lex.hh>
#include "fmemopen.h"
#include "gtest.h"
#include <map>
#include <string>
#include <functional>

using namespace std;

class Runtime {
    map<string, node_t*> functions;
    map<string, std::function<node_t*(node_t*)>> native_functions;
    map<string, node_t*> variables;
  public:
    void insert(node_t*);
    void native(const string name, std::function<node_t*(node_t*)> cb);

    template <typename... T>
    node_t* call(const char *name, T... t) {
      node_t *statement = node_new(TKN_FUNCTION_CALL);
      
      node_t *identifier = node_new(TKN_IDENTIFIER);
      identifier->text = (char*)name;
      node_append(statement, identifier);

      node_t *exprlist = node_new(TKN_EXPRESSION_LIST);
      node_append(statement, exprlist);
      
      return call0(statement, t...);
    }
    
  protected:
    node_t* call0(node_t *statement) {
        return eval(statement);
    }
 
    template <typename H, typename... T>
    node_t* call0(node_t *statement, H p, T... t) {
      node_append(statement->down->next, node_new_int(p));
      return call0(statement, t...);
    }

    node_t* eval(node_t*);
};

static Runtime* test(const char *source) {
        printf("--------- parse --------\n");
        auto in = fmemopen((void*)source, strlen(source), "r");
        auto root = parse(in);
        fclose(in);

        printf("--------- print --------\n");
        node_print(stdout, root);

        printf("----- pretty print -----\n");
        node_pretty_print(stdout, root);

        auto rt = new Runtime();
        rt->insert(root);

        return rt;
}

namespace {

    TEST(Basics, Empty) {
        auto rt = test(R"(int main(int a, int b)
{
  println("hello", 7);
  return a + b;
}
)");
        printf("------- evaluate -------\n");
        rt->native("println", [](node_t *args) {
          for(node_t *p = args; p; p=p->next) {
            printf("%s", p->text);
            if (p->next)
              printf(" ");
          }
          printf("\n");
          return nullptr;
        });
        
        node_t *result = rt->call("main", "3", "7");
        node_pretty_print(stdout, result);
        printf("\n");

//        rt->println("le function", 1, 2, "hello", 3.1415);

    }

}

void
Runtime::insert(node_t *node) {
  assert(node->tkn == TKN_DECLARATION_SEQ);
  for (node_t *p = node->down; p; p=p->next) {
    assert(p->tkn == TKN_FUNCTION);
    functions[p->text] = p;
  }
}

void
Runtime::native(const string name, std::function<node_t*(node_t*)> cb) {
  native_functions[name] = cb;
}

node_t*
Runtime::eval(node_t *node) {
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
      auto returnType = fun->second->down;
      auto parameterList = fun->second->down->next->down;
      auto body = fun->second->down->next->next;
      
      auto expressionList = node->down->next->down;
      printf("call with\n");
      node_print(stdout, expressionList);
      
      printf("call to\n");
      node_print(stdout, parameterList);
      for(auto p=parameterList, e=expressionList; p; p=p->next, e=e->next) {
        auto id = p->down->next;
        auto value = e;
        variables[id->text] = value;
        printf("%s = %i\n", id->text, value->value.i);
      }
      
      // take parameter list
      
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
    case TKN_IDENTIFIER:
      return variables[node->text];
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
