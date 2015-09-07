#include "lol_unit.h"

#include "../include/gtl/memory.h"
#include "../include/gtl/specialized/phaseable.h"

template<typename T>
T min(T i_a, T i_b)
{
    return i_a < i_b ? i_a : i_b;
}

struct health
{
    float m_heath, m_max, m_rate;

    void set(float i_h, float i_m, float i_r)
    {
        m_heath = i_h;
        m_max = i_m;
        m_rate = i_r;
    }

    void heal() {
        m_heath = min(m_heath + m_rate, m_max);
    }

    static void batch(health * i, health * e)
    {
        do {
            i->heal();
        } while (++i != e);
    }
};

void test_phaseable()
{
    {
        gtl::specialized::phasable_container< gtl::specialized::phaseable_cfg<health, gtl::system_heap> > to_test;

        health* a = to_test.alloc(); a->set(100.0f, 100.0f, 1.0f);
        health* b = to_test.alloc(); b->set(10.0f, 100.0f, 1.0f);
        health* c = to_test.alloc(); c->set(10.0f, 100.0f, 0.0f);

        to_test.run_phase<&health::heal>();
        to_test.batch(&health::batch);
    }

    {
        gtl::specialized::phasable_container< gtl::specialized::phaseable_cfg<health, gtl::system_heap> > to_test;

        for (int i = 0; i < 10000; i++) {
            to_test.alloc()->set((float)i, 100.0f, sinf((float)i / 12.0f));
        }

        to_test.run_phase<&health::heal>();
        to_test.batch(&health::batch);
    }
}


int main(int i_argc, char ** i_argv)
{
    (void)i_argc;
    (void)i_argv;

    test_phaseable();

    lol::TextTestRunner tester;
    return tester.Run() ? 0 : 1;
}
