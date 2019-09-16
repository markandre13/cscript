#include <runtime.hh>
#include "fmemopen.h"
#include "gtest.h"

using namespace std;

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
        
        node_t *result = rt->call("main", 3, 7);
        node_pretty_print(stdout, result);
        printf("\n");

//        rt->println("le function", 1, 2, "hello", 3.1415);

    }

}

