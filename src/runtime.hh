#include "lex.hh"

#include <string>
#include <map>
#include <functional>

class Runtime {
    std::map<std::string, node_t*> functions;
    std::map<std::string, std::function<node_t*(node_t*)>> native_functions;
    std::map<std::string, node_t*> variables;
  public:
    void insert(node_t*);
    void native(const std::string name, std::function<node_t*(node_t*)> cb);

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
      node_append(statement->down->next, node_new_value(p));
      return call0(statement, t...);
    }

    node_t* eval(node_t*);
};
