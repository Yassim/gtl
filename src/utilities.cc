#include "../include/gtl/utilities.h"

namespace gtl {

bool overlap(intptr_t i_dst, intptr_t i_src, size_type i_n, size_type i_st)
{
    const size_type s = i_n * i_st;
    const intptr_t minp = min(i_src, i_dst);
    const intptr_t maxp = max(i_src + s, i_dst + s);
    const size_type diff = maxp - minp;
    return diff < s * 2;
}

}

