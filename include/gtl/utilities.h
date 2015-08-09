#ifndef GTL_UTILS
#define GTL_UTILS

#include <assert.h>

namespace gtl {

#define static_if   if
#define static_else else

#define gtl_assert(X)   assert(X)
#define gtl_assert_msg(X, MSG)   assert((X) && !(MSG))


// gtl_(tmp|dec|use)_typename(N)
// used instead of varadic macros.
// A lot of game compilers are stuck in time from before C++11
// so Ill be declaring 4 overloads.
// if your in a case where you need more than 4. please consider
// wrapping your args into a struct and passing that.
#define gtl_tmp_typename1   typename type1
#define gtl_dec_typename1   const type1& t1
#define gtl_use_typename1   t1

#define gtl_tmp_typename2   gtl_tmp_typename1, typename type2
#define gtl_dec_typename2   gtl_dec_typename1, const type2& t2
#define gtl_use_typename2   gtl_use_typename1, t2

#define gtl_tmp_typename3   gtl_tmp_typename2, typename type3
#define gtl_dec_typename3   gtl_dec_typename2, const type3& t3
#define gtl_use_typename3   gtl_use_typename2, t3

#define gtl_tmp_typename4   gtl_tmp_typename3, typename type4
#define gtl_dec_typename4   gtl_dec_typename3, const type4& t4
#define gtl_use_typename4   gtl_use_typename3, t4

    template<typename T>
    inline void swap(T& i_a, T& i_b)
    {
        T t = i_a;
        i_a = i_b;
        i_b = t;
    }

    template<typename T>
    inline T min(const T i_a, const T i_b)
    {
        return i_a < i_b ? i_a : i_b;
    }

    template<typename T>
    inline T max(const T i_a, const T i_b)
    {
        return i_a > i_b ? i_a : i_b;
    }

    static bool overlap(intptr_t i_dst, intptr_t i_src, size_type i_n, size_type i_st)
    {
        const size_type s = i_n * i_st;
        const intptr_t minp = min(i_src, i_dst);
        const intptr_t maxp = max(i_src + s, i_dst + s);
        const size_type diff = maxp - minp;
        return diff < s * 2;
    }

    template<typename T>
    static bool overlap(T* i_dst, T* i_src, T* i_end)
    {
        return overlap(reinterpret_cast<intptr_t>(i_dst), reinterpret_cast<intptr_t>(i_src), i_end - i_src, sizeof(T));
    }

    template< class InputIt >
    size_type count(InputIt first, InputIt last)
    {
        size_type c = 0;
        while (first++ != last) c++;
        return c;
    }
};


#endif // !GTL_UTILS
