#include "..\include/gtl/vector.h"
#include "..\include/gtl/array.h"
#include "..\include/gtl/map.h"
#include "..\include/gtl/persistant\devirtualised_base.h"
//#include <vector>
//#include <utility>

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

void test_array()
{
    using namespace gtl;

    int tfoo[5] = { 6, 4, 3, 2, 1 };

    for (auto i = begin<int, 5>(tfoo); i != end<int, 5>(tfoo); ++i) {
        printf("& %d\n", *i);
    }

    for (int i : tfoo) {
        printf("$ %d\n", i);
    }

    {int j = 5; }
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

void test_map()
{
    gtl::map<int, int> tfoo;
}

void test_devirtualised()
{
    enum {
        kClassInt,
        kClassFloat,
        kClassNoof
    };

    enum {
        kMethodPrint,
        kMethodPrintStr,
        kMethod_AsInt,
        kMethod_AsFloat,
        kMethodNoof
    };

    class test_base : public gtl::persistant::devirtualised_base < char, kClassNoof, kMethodNoof >
    {
        typedef  gtl::persistant::devirtualised_base < char, kClassNoof, kMethodNoof > base;
    protected:
        typedef  base::method_type method_type;
    public:
        test_base(char i_class) :base(i_class) {}

        void print()
        {
            void_method<kMethodPrint>();
        }

        void print(const char * i_str)
        {
            void_method<kMethodPrintStr>(i_str);
        }

        int asInt()
        {
            return method<int, kMethod_AsInt>();
        }

        float asFloat()
        {
            return method<float, kMethod_AsFloat>();
        }

        void test()
        {
            print();
            print("test str");
            printf("as int %d\n", asInt());
            printf("as float %f\n", asFloat());
        }
    };

    class dv_int : public test_base
    {
    public:
        dv_int(int i_value) :test_base(kClassInt), m_value(i_value) { init(); }
    private:
        int m_value;

        void print()
        {
            printf("int %d\n", m_value);
        }

        void print_str(const char *& i_str)
        {
            printf("int %s, %d\n", i_str, m_value);
        }

        int asInt()
        {
            return m_value;
        }

        float asFloat()
        {
            return static_cast<float>(m_value);
        }

        static void init()
        {
            static bool done = false;
            if (!done) {
                test_base::method_type fncs[kMethodNoof] = {
                    reinterpret_cast<test_base::method_type>(&dv_int::print),
                    reinterpret_cast<test_base::method_type>(&dv_int::print_str),
                    reinterpret_cast<test_base::method_type>(&dv_int::asInt),
                    reinterpret_cast<test_base::method_type>(&dv_int::asFloat) };

                set_vtbl(kClassInt, fncs);
                done = true;
            }
        }
    };

    class dv_float : public test_base
    {
    public:
        dv_float(float i_value) :test_base(kClassFloat), m_value(i_value) { init(); }
    private:
        float m_value;

        void print()
        {
            printf("float %f\n", m_value);
        }

        void print_str(const char *& i_str)
        {
            printf("float %s, %f\n", i_str, m_value);
        }

        int asInt()
        {
            return static_cast<int>(m_value);
        }

        float asFloat()
        {
            return m_value;
        }

        static void init()
        {
            static bool done = false;
            if (!done) {
                test_base::method_type fncs[kMethodNoof] = {
                    reinterpret_cast<test_base::method_type>(&dv_float::print),
                    reinterpret_cast<test_base::method_type>(&dv_float::print_str),
                    reinterpret_cast<test_base::method_type>(&dv_float::asInt),
                    reinterpret_cast<test_base::method_type>(&dv_float::asFloat) };

                set_vtbl(kClassFloat, fncs);
                done = true;
            }
        }
    };


    dv_int foo(42);
    dv_float bar(3.1415f);

    foo.test();
    bar.test();
}

int main(int i_argc, char ** i_argv)
{
    test_grow_pow2();
    test_lifetime_conservitive();
    test_vector();
    test_array();
    test_devirtualised();
    return 0;
}
