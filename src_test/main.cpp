#include "..\include/gtl/vector.h"
#include "..\include/gtl/grow.h"

#include <stdio.h>

void test_lifetime_conservitive()
{
    struct test_t
    {
        test_t() :value(0) { cc++;  }
        explicit test_t(int i_v) :value(i_v) { cc++;  }
        test_t(const test_t& i_rhs) :value(i_rhs.value) { cc++; }
        ~test_t() { value = -1; cc--;  }

        test_t& operator=(const test_t& i_rhs)
        {
            gtl_assert(this != &i_rhs); // no self assignment here
            value = i_rhs.value;
            return *this;
        }

        int value;
        int cc;
    };


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

void test_vector()
{
    gtl::vector<int> tfoo;
    size_t n = sizeof(tfoo);

    tfoo.push_back(1);
    tfoo.emplace_back(2);
    tfoo.push_back(3);
    tfoo.push_back(4);

    for (auto i = begin(tfoo); i != end(tfoo); ++i) {
        printf("& %d\n", *i);
    }

    for (int i : tfoo) {
        printf("$ %d\n", i);
    }

    {int j= 5; }
}

void test_grow_pow2()
{
    for (gtl::size_type i = 0; i <= 2048; i++)
    {
        auto j = gtl::grow_pow2_cap<512>::next_size(i);
        printf("@@ %d -> %d\n", i, j);
        i = j;
    }
}

int main(int i_argc, char ** i_argv)
{
    test_grow_pow2();
    test_lifetime_conservitive();
    test_vector();
    return 0;
}
