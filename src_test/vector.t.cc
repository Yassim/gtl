#include "lol_unit.h"
#include "../include/gtl/vector.h"

using namespace lol;
using namespace gtl;

LOLUNIT_FIXTURE(Vector)
{
    LOLUNIT_TEST(Basic)
    {
        vector<int> tfoo;
        size_t n = sizeof(tfoo);
        (void)n;

        tfoo.push_back(1);
        tfoo.emplace_back(2);
        tfoo.push_back(3);
        tfoo.push_back(4);

        for (auto i = begin(tfoo); i != end(tfoo); ++i) {
            printf("& %d/n", *i);
        }

        for (int i : tfoo) {
            printf("$ %d/n", i);
        }

    {int j = 5; (void)j; }
    }
};
