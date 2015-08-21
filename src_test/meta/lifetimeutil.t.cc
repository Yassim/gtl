#include "../lol_unit.h"
#include "../../include/gtl/meta/lifetime_util.h"

using namespace lol;
using namespace gtl;

LOLUNIT_FIXTURE(Lifetime)
{
    struct test_t
    {
        test_t() :value(0) { cc++; }
        explicit test_t(int i_v) :value(i_v) { cc++; }
        test_t(const test_t& i_rhs) :value(i_rhs.value) { cc++; }
        ~test_t() { value = -1; cc--; }

        test_t& operator=(const test_t& i_rhs)
        {
            gtl_assert(this != &i_rhs); // no self assignment here
            value = i_rhs.value;
            return *this;
        }

        int value;
        int cc;
    };

    LOLUNIT_TEST(TestConservitive)
    {
        test_t a[10];
        test_t b[10];

        for (int i = 0; i < 10; i++) {
            a[i].value = i + 1;
            a[i].cc = 1;
            b[i].cc = 1;
        }

        gtl::meta::lifetime_conservitive<test_t>::deconstruct_range(b, b + 10);
        gtl::meta::lifetime_conservitive<test_t>::default_construct_range(b, b + 10);
        gtl::meta::lifetime_conservitive<test_t>::deconstruct_range(b, b + 10);
        gtl::meta::lifetime_conservitive<test_t>::value_construct_range(b, b + 10, test_t(42));
        gtl::meta::lifetime_conservitive<test_t>::deconstruct_range(b, b + 10);
        gtl::meta::lifetime_conservitive<test_t>::copy(b, a, a + 10);

        gtl::meta::lifetime_conservitive<test_t>::deconstruct_range(a + 9, a + 10);
        gtl::meta::lifetime_conservitive<test_t>::move(a + 1, a, a + 9); // should stomp 10
        gtl::meta::lifetime_conservitive<test_t>::move(a, a + 2, a + 10);
    }
};
