#ifndef GTLMETA_LIFETIME
#define GTLMETA_LIFETIME


#ifndef GTL_TYPES
#include "..\types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "..\utilities.h"
#endif // !GTL_UTILS


#include <new>
#include <string.h>

namespace gtl {
namespace meta {

template<typename T>
struct lifetime_pod
{
    enum {
        kReallocSensitive = 0
    };

    static void default_construct_range(T* i_begin, T* const i_end) 
    {
        /* no op  */
    }

    static void value_construct_range(T* i_begin, T* const i_end, const T& i_value)
    {
        while (i_begin != i_end) 
            *i_begin++ = i_value;
    }

    static void deconstruct_range(T* i_begin, T* const i_end) 
    {
        /* no op  */
    }

    static void move(T* i_dst, T* i_src, T* const i_end)
    {
        memmove(i_dst, i_src, sizeof(T) * (i_end - i_dst));
    }

    static void copy(T* i_dst, T* i_src, T* const i_end)
    {
        memcpy(i_dst, i_src, sizeof(T) * (i_end - i_dst));
    }

    template< gtl_tmp_typename1 >
    static void emplace(T* i_p, gtl_dec_typename1)
    {
        *i_p = gtl_use_typename1;
    }

    // delibritly left un implimented
    template< gtl_tmp_typename2 >
    static void emplace(T* i_p, gtl_dec_typename2);
    template< gtl_tmp_typename3 >
    static void emplace(T* i_p, gtl_dec_typename3);
    template< gtl_tmp_typename4 >
    static void emplace(T* i_p, gtl_dec_typename4);
};

template<typename T>
struct lifetime_simple
{
    enum {
        kReallocSensitive = 0
    };

    static void default_construct_range(T* i_begin, T* const i_end)
    {
        while (i_begin != i_end)
            new (i_being++) T();
    }

    static void value_construct_range(T* i_begin, T* const i_end, const T& i_value)
    {
        while (i_begin != i_end)
            new (i_being++) T(i_value);
    }

    static void deconstruct_range(T* i_begin, T* const i_end)
    {
        while (i_begin != i_end) {
            //::operator delete (i_being, i_begin); // should be what we call, as apposed to ~T()
            i_begin->~T();
            ++i_begin;
        }
    }

    static void move(T* i_dst, T* i_src, T* const i_end)
    {
        memmove(i_dst, i_src, sizeof(T) * (i_end - i_dst))
    }

    static void copy(T* i_dst, T* i_src, T* const i_end)
    {
        memcpy(i_dst, i_src, sizeof(T) * (i_end - i_dst))
    }


    template< gtl_tmp_typename1 >
    static void emplace(T* i_p, gtl_dec_typename1)
    {
        new (i_p) T(gtl_use_typename1);
    }

    template< gtl_tmp_typename2 >
    static void emplace(T* i_p, gtl_dec_typename2)
    {
        new (i_p) T(gtl_use_typename2);
    }

    template< gtl_tmp_typename3 >
    static void emplace(T* i_p, gtl_dec_typename3)
    {
        new (i_p) T(gtl_use_typename3);
    }

    template< gtl_tmp_typename4 >
    static void emplace(T* i_p, gtl_dec_typename4)
    {
        new (i_p) T(gtl_use_typename4);
    }
};

template<typename T>
struct lifetime_conservitive
{
    enum {
        kReallocSensitive = 1
    };

    static void default_construct_range(T* i_begin, T* const i_end)
    {
        while (i_begin != i_end)
            new (i_begin++) T();
    }

    static void value_construct_range(T* i_begin, T* const i_end, const T& i_value)
    {
        while (i_begin != i_end)
            new (i_begin++) T(i_value);
    }

    static void deconstruct_range(T* i_begin, T* const i_end)
    {
        while (i_begin != i_end) {
            //::operator delete (i_being, i_begin); // should be what we call, as apposed to ~T()
            i_begin->~T();
            ++i_begin;
        }
    }



    static void move(T* i_dst, T* i_src, T* i_end)
    {
        gtl_assert(i_dst != i_src);
        gtl_assert(overlap(i_dst, i_src, i_end));
        if (i_dst > i_src) {
            //dst is higher than src, do this in reverse
            const size_type n = i_end - i_src;
            const size_type diff = i_dst - i_src;

            //  s d e  
            // |x|x|.|?|
            // |.|x|x|?|
            // n = 2, diff = 1

            i_dst += n;
            i_src += n;
            i_end -= n;

            T* const create_end = i_dst - diff;
            T*       assign_end = i_end + diff;


            while (i_dst != create_end)
                new (--i_dst) T(*--i_src);

            while (i_dst != i_end)
                *--i_dst = *--i_src;

            while (assign_end != i_end) {
                //::operator delete (assign_end, assign_end);
                assign_end--;
                assign_end->~T();
            }

        } else {
            const size_type n = i_end - i_src;
            const size_type diff = i_src - i_dst;
            T* const create_end = i_dst + diff;
            T*       assign_end = i_dst + n;

            //  d s   e
            // |.|x|x|?|
            // |x|x|.|?|
            // n = 2, diff = 1

            while (i_dst != create_end)
                new (i_dst++) T(*i_src++);

            while (i_dst != i_end)
                *i_dst++ = *i_src++;

            while (assign_end != i_end) {
                //::operator delete (assign_end, assign_end);
                assign_end->~T();
                assign_end++;
            }
        }
    }

    static void copy(T* i_dst, T* i_src, T* const i_end)
    {
        gtl_assert(!overlap(i_dst, i_src, i_end));
        while (i_src != i_end)
            new (i_dst++) T(*i_src++);
    }

    template< gtl_tmp_typename1 >
    static void emplace(T* i_p, gtl_dec_typename1)
    {
        new (i_p)T(gtl_use_typename1);
    }

    template< gtl_tmp_typename2 >
    static void emplace(T* i_p, gtl_dec_typename2)
    {
        new (i_p)T(gtl_use_typename2);
    }

    template< gtl_tmp_typename3 >
    static void emplace(T* i_p, gtl_dec_typename3)
    {
        new (i_p)T(gtl_use_typename3);
    }

    template< gtl_tmp_typename4 >
    static void emplace(T* i_p, gtl_dec_typename4)
    {
        new (i_p)T(gtl_use_typename4);
    }
};

// default is simple
template<typename T>
struct lifetime_util_select
{
    typedef typename lifetime_simple<T> type;
};


#define GTL_LIFETIME_TYPE_SHOUOLD_USE(TYPE, POLICY) \
template<>                                          \
struct lifetime_util_select<TYPE>                   \
{                                                   \
    typedef lifetime_ ## POLICY <TYPE> type;  \
};


GTL_LIFETIME_TYPE_SHOUOLD_USE(int, pod);

};
};

#endif // !GTLMETA_LIFETIME
