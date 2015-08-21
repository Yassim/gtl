#include "lol_unit.h"
#include "../include/gtl/map.h"

using namespace lol;
using namespace gtl;

LOLUNIT_FIXTURE(Map)
{
    LOLUNIT_TEST(Basic)
    {
        map<int, int> tfoo;

        tfoo[0] = 9;
        tfoo[2] = 8;
        tfoo[-1] = -1;
        tfoo[1] = 42;

        for (auto i : tfoo) {
            printf("kv %d %d/n", i.first, i.second);
            if (i.first == 0) i.second = 10;
        }

        tfoo.begin()->second = 11;

        for (auto i : tfoo) {
            printf("kv %d %d/n", i.first, i.second);
        }
    }
};

