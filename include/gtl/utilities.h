#ifndef GTL_UTILS
#define GTL_UTILS

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#include <assert.h>

namespace gtl {

#pragma warning( disable : 4127 ) //  warning C4127: conditional expression is constant  
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

    bool overlap(intptr_t i_dst, intptr_t i_src, size_type i_n, size_type i_st);

    template<typename T>
    bool overlap(T* i_dst, T* i_src, T* i_end)
    {
        return overlap(reinterpret_cast<intptr_t>(i_dst), reinterpret_cast<intptr_t>(i_src), i_end - i_src, sizeof(T));
    }

    template< class InputIt >
    size_type distance(InputIt first, InputIt last)
    {
        size_type c = 0;
        while (first++ != last) c++;
        return c;
    }

    template<typename t1, typename t2>
    struct pair
    {
        typedef t1 first_type;
        typedef t2 second_type;

        inline pair() {}
        template<typename U, typename V>
        inline pair(const pair<U, V>& pr) : first(pr.first), second(pr.second) {}
        inline pair(const first_type& a, const second_type& b) : first(a), second(b) {}
        inline ~pair() {}

        pair& operator=(const pair& i_rhs)
        {
            pair(i_rhs).swap(*this);
            return *this;
        }

        void swap(pair& i_rhs)
        {
            swap(first, i_rhs.first);
            swap(second, i_rhs.second);
        }

        first_type first;
        second_type second;
    };

    template<typename t1, typename t2>
    void swap(pair<t1, t2>& i_a, pair<t1, t2>& i_b)
    {
        i_a.swap(i_b);
    }

    template<typename t1, typename t2, typename i1, typename i2>
    class iterator_pair
    {
    public:
        template<typename t1_inner, typename t2_inner>
        struct pair_cheat : public pair<t1_inner, t2_inner>
        {
            typedef t1_inner first_type;
            typedef t2_inner second_type;

            inline pair_cheat(const first_type& a, const second_type& b) :pair<t1_inner, t2_inner>(a, b) {}

            inline pair<t1_inner, t2_inner>& operator*()
            {
                return *this;
            }

            inline const pair<t1_inner, t2_inner>& operator*() const
            {
                return *this;
            }

            // probably only need these for the cheat to work
            inline const pair<t1_inner, t2_inner>* operator->() const
            {
                return this;
            }

            inline  pair<t1_inner, t2_inner>* operator->()
            {
                return this;
            }
        };

        typedef t1 first_type;
        typedef t2 second_type;
        typedef pair<first_type, second_type> value_type;
        typedef pair_cheat<first_type, second_type> cheat_value_type;
        typedef i1 first_iterator;
        typedef i2 second_iterator;

        inline iterator_pair(const iterator_pair& i_rhs) : m_i1(i_rhs.m_i1), m_i2(i_rhs.m_i2) {}
        inline iterator_pair(const first_iterator& i_i1, const second_iterator& i_i2) : m_i1(i_i1), m_i2(i_i2) {}
        inline ~iterator_pair() {}

        inline iterator_pair& operator++()
        {
            ++m_i1;
            ++m_i2;
            return *this;
        }

        inline iterator_pair operator++(int)
        {
            iterator_pair t(*this);
            ++*this;
            return t;
        }

        inline bool operator==(const iterator_pair& i_rhs)
        {
            return m_i1 == i_rhs.m_i1; // should just need to compare only the first.
        }

        inline bool operator!=(const iterator_pair& i_rhs)
        {
            return m_i1 != i_rhs.m_i1; // should just need to compare only the first.
        }

        inline value_type operator*() const
        {
            return value_type(*m_i1, *m_i2);
        }

        inline cheat_value_type operator->() const
        {
            return cheat_value_type(*m_i1, *m_i2);
        }

    private:
        inline iterator_pair() {}

        first_iterator  m_i1;
        second_iterator m_i2;
    };
};


#endif // !GTL_UTILS
