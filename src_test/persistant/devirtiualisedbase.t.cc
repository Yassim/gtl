#include "../lol_unit.h"
using namespace lol;

#include "../../include/gtl/persistant/devirtualised_base.h"

LOLUNIT_FIXTURE(Devirtualsed)
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
            printf("as int %d/n", asInt());
            printf("as float %f/n", asFloat());
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
            printf("int %d/n", m_value);
        }

        void print_str(const char *& i_str)
        {
            printf("int %s, %d/n", i_str, m_value);
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
            printf("float %f/n", m_value);
        }

        void print_str(const char *& i_str)
        {
            printf("float %s, %f/n", i_str, m_value);
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

    LOLUNIT_TEST(Construct_Int)
    {
        dv_int foo(42);
    }

    LOLUNIT_TEST(Construct_Float)
    {
        dv_float foo(3.1415f);
    }
};



//
//void test_offsetptr()
//{
//    gtl::persistant::offset_ptr<int, int16_t> tfoo;
//    int bar = 0;
//    tfoo = &bar;
//
//    *tfoo = 42;
//
//    {int j = 5; (void)j; }
//
//    typedef gtl::persistant::offset_ptr<int, int16_t> tptr;
//    typedef gtl::vector<tptr> tvector;
//    typedef gtl::base_vector< gtl::vector_cfg<int, gtl::system_heap, int16_t, gtl::grow_by1, tptr > > tipvector;
//
//    tvector tafoo;
//    int ee = gtl::meta::lifetime_util_select<tptr>::type::kReallocSensitive;
//
//    //auto tt = gtl::meta::lifetime_util_select<tvector>::ptr_type();
//    int e = gtl::meta::lifetime_util_select<tvector>::type::kReallocSensitive;
//    int eee = gtl::meta::lifetime_util_select<tipvector>::type::kReallocSensitive;
//    (void)e;
//    (void)ee;
//    (void)eee;
//    //(void)tt;
//
//    tafoo.push_back(tfoo);
//    tafoo.push_back(tfoo);
//    tafoo.push_back(tfoo);
//    tafoo.push_back(tfoo);
//
//    for (auto i : tafoo) {
//        (*i)++;
//    }
//}
