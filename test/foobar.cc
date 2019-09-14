#include <lex.hh>
#include "fmemopen.h"
#include "gtest.h"

static node_t* test(const char *source) {
        auto in = fmemopen((void*)source, strlen(source), "r");
        auto root = compile(in);
        node_print(stdout, root);
        fclose(in);
        return root;
}

namespace {

    TEST(Basics, Empty) {
        test(R"(int main(unsigned int a, int b)
{
  return a + b;
}
)");

//        eval("main(7, 3)");
//        call("main", 7, 3).toInt()

//        ASSERT_EQ(call("main", 7, 3).toInt(), 10);
    }

}
