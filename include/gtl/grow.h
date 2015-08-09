#ifndef GTL_GROW
#define GTL_GROW

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "utilities.h"
#endif // !GTL_UTILS

namespace gtl {

    template<size_type kCap>
    struct grow_pow2_cap
    {
        static size_type next_size(size_type i_in)
        {
            const size_type capped = ((i_in / kCap) + 1) * kCap;
            size_type pow2 = i_in-1;
            pow2 |= pow2 >> 1;
            pow2 |= pow2 >> 2;
            pow2 |= pow2 >> 4;
            pow2 |= pow2 >> 8;
            pow2 |= pow2 >> 16;
//#if sizeof(size_type) >= 8
//            pow2 |= pow2 >> 32;
//#endif
            pow2 += 1;
            return min(capped, pow2);
        }
    };

    struct grow_by1
    {
        static size_type next_size(size_type i_in)
        {
            return i_in;
        }
    };
}
#endif // !GTL_GROW
