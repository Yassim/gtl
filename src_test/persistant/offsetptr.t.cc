#include "../lol_unit.h"
#include "../../include/gtl/persistant/offset_ptr.h"

using namespace lol;
using namespace gtl;

LOLUNIT_FIXTURE(OffsetPtr)
{
    LOLUNIT_TEST(Ptr)
    {
        gtl::persistant::offset_ptr<int, int16_t> tfoo;
        int bar = 0;
        tfoo = &bar;
    
        *tfoo = 42;
    
        //{int j = 5; (void)j; }
    
        //typedef gtl::persistant::offset_ptr<int, int16_t> tptr;
        //typedef gtl::vector<tptr> tvector;
        //typedef gtl::base_vector< gtl::vector_cfg<int, gtl::system_heap, int16_t, gtl::grow_by1, tptr > > tipvector;
    
        //tvector tafoo;
        //int ee = gtl::meta::lifetime_util_select<tptr>::type::kReallocSensitive;
    
        ////auto tt = gtl::meta::lifetime_util_select<tvector>::ptr_type();
        //int e = gtl::meta::lifetime_util_select<tvector>::type::kReallocSensitive;
        //int eee = gtl::meta::lifetime_util_select<tipvector>::type::kReallocSensitive;
        //(void)e;
        //(void)ee;
        //(void)eee;
        ////(void)tt;
    
        //tafoo.push_back(tfoo);
        //tafoo.push_back(tfoo);
        //tafoo.push_back(tfoo);
        //tafoo.push_back(tfoo);
    
        //for (auto i : tafoo) {
        //    (*i)++;
        //}
    }
};

