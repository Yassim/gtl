#ifndef GTL_VECTOR
#define GTL_VECTOR

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#ifndef GTL_MEMORY
#include "memory.h"
#endif // !GTL_MEMORY

#ifndef GTL_GROW
#include "grow.h"
#endif

#ifndef GTL_LOCKING
#include "locking.h"
#endif // !GTL_LOCKING

#ifndef GTL_UTILS
#include "utilities.h"
#endif // !GTL_UTILS

#ifndef GTLMETA_LIFETIME
#include "meta\lifetime_util.h"
#endif // !GTLMETA_LIFETIME

namespace gtl {

template<
    typename i_type,
    class    i_heap,
    typename i_count = int,
    class    i_locking = locking_noop,
    typename i_growth = grow_pow2_cap<512>,
    typename i_ptr = i_type*,
    typename i_lifetime = typename meta::lifetime_util_select<i_type>::type
> struct vector_cfg
{
    typedef typename i_type     T;
    typedef          i_heap     heap_base;
    typedef typename i_count    count_type;
    typedef typename i_locking  lock_base;
    typedef typename i_growth   growth_policy;
    typedef typename i_ptr      ptr_type;
    typedef typename i_lifetime lifetime_util;
};

template<typename cfg_type>
class base_vector
    : private cfg_type::heap_base
    , private cfg_type::lock_base
{
    typedef typename cfg_type::count_type count_type;
    typedef typename cfg_type::ptr_type ptr_type;
    typedef typename cfg_type::heap_base base0_type;
    typedef typename cfg_type::lock_base base1_type;
    typedef typename cfg_type::growth_policy growth_policy;
    typedef typename cfg_type::lifetime_util lifetime_util;
public:
    typedef typename cfg_type::T T;
    typedef typename cfg_type::T value_type;
    typedef typename cfg_type::lock_base lock_type;

    class Allocator : public cfg_type::heap_base {};
    

    // Construction
    explicit base_vector(const Allocator& i_heap)
        : base0_type(i_heap)
        , m_p(nullptr)
        , m_count(0)
        , m_capacity(0)
    {}
    base_vector() : base_vector(Allocator()) {}
//    explicit base_vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator());
    base_vector(size_type count, const T& value, const Allocator& alloc = Allocator()) : base_vector(alloc) { resize(count, value); }
    explicit base_vector(size_type count) : base_vector(Allocator()) { resize(count); }
    explicit base_vector(size_type count, const Allocator& alloc = Allocator()) : base_vector(alloc) { resize(count); }
    template< class InputIt >
    base_vector(InputIt first, InputIt last, const Allocator& alloc = Allocator()) : base_vector(alloc) { assign(first, last); }
    base_vector(const base_vector& other) : base_vector(static_cast<const Allocator&>(other)) { assign(other.begin(), other.end()); }
    base_vector(const base_vector& other, const Allocator& alloc) : base_vector(alloc) { assign(other.begin(), other.end()); }
//        base_vector(base_vector&& other)
//        base_vector(base_vector&& other, const Allocator& alloc);
//    base_vector(std::initializer_list<T> init, const Allocator& alloc = Allocator());

    ~base_vector()
    {
        clear();
        shrink_to_fit();
    }

    base_vector& operator=(const base_vector& other)
    {
        if (this != &other) {
            base_vector tmp(other).swap(*this);
        }
        return *this;
    }
    //base_vector& operator=(base_vector&& other);
    //base_vector& operator=(std::initializer_list<T> ilist);

    void assign(size_type count, const T& value)
    {
        clear();
        insert(begin(), count, value);
    }

    template< class InputIt >
    void assign(InputIt first, InputIt last)
    {
        clear();
        insert(begin(), first, last);
    }
    //   void assign(std::initializer_list<T> ilist);


    // this we are actually object shearing 
    inline Allocator allocator() const             { return Allocator(*this); }
    inline lock_type& locking()                    { return static_cast<lock_type&>(*this); }

    // Element access
    inline T&       at(size_type i_ndx)                 { gtl_assert(i_ndx < m_count); return m_p[i_ndx]; }
    inline const T& at(size_type i_ndx) const           { gtl_assert(i_ndx < m_count); return m_p[i_ndx]; }
    inline T&       operator[](size_type i_ndx)         { gtl_assert(i_ndx < m_count); return m_p[i_ndx]; }
    inline const T& operator[](size_type i_ndx) const   { gtl_assert(i_ndx < m_count); return m_p[i_ndx]; }
    inline T&       front()                             { gtl_assert(m_count); return m_p[0]; }
    inline const T& front() const                       { gtl_assert(m_count); return m_p[0]; }
    inline T&       back()                              { gtl_assert(m_count); return m_p[m_count - 1]; }
    inline const T& back() const                        { gtl_assert(m_count); return m_p[m_count - 1]; }
    inline T*       data()                              { gtl_assert(m_count); return m_p; }
    inline const T* data() const                        { gtl_assert(m_count); return m_p; }

    // Iterators
    typedef T*          iterator;
    typedef const T*    const_iterator;

    inline iterator       begin()                       { return m_p; }
    inline const_iterator begin() const                 { return m_p; }
    inline const_iterator cbegin() const                { return m_p; }
    inline iterator       end()                         { return m_p + m_count; }
    inline const_iterator end() const                   { return m_p + m_count; }
    inline const_iterator cend() const                  { return m_p + m_count; }
    // inline iterator       rbegin()                       { return m_p; }
    // inline const_iterator rbegin() const                 { return m_p; }
    // inline const_iterator crbegin() const                { return m_p; }
    // inline iterator       rend()                         { return m_p + m_count; }
    // inline const_iterator rend() const                   { return m_p + m_count; }
    // inline const_iterator crend() const                  { return m_p + m_count; }

    // Capacity
    inline bool      empty() const                      { return 0 == m_count; }
    inline size_type size() const                       { return m_count; }
    inline size_type max_size() const                   { return base0_type::max_size(sizeof(T) * std::numeric_limits<count_type>::max()) / sizeof(T); }

    void             reserve(size_type i_n)
    {
        const size_type n = growth_policy::next_size(i_n);
        if (n > static_cast<size_type>(m_capacity)) {
            /*static_if(lifetime_util::kReallocSensitive) {
                T* p = reinterpret_cast<T*>(base0_type::alloc(n * sizeof(T)));
                lifetime_util::move_non_overlap(p, begin(), end());
                base0_type::free(m_p);
                m_p = p;
            } static_else {
                m_p = reinterpret_cast<T*>(base0_type::realloc(m_p, n * sizeof(T)));
            }*/
            m_p = meta::typesafe_realloc<lifetime_util>(m_p, m_count, n, static_cast<base0_type&>(*this));
            m_capacity = n;
        }
    }

    inline size_type capacity() const                   { return m_capacity;  }

    void             shrink_to_fit()
    {
        if (m_count) {
            const size_type n = growth_policy::next_size(m_count);
            /*static_if(lifetime_util::kReallocSensitive) {
                T* p = reinterpret_cast<T*>(base0_type::alloc(n * sizeof(T)));
                lifetime_util::move_non_overlap(p, begin(), end());
                base0_type::free(m_p);
                m_p = p;
            } static_else{
                m_p = reinterpret_cast<T*>(base0_type::realloc(m_p, n * sizeof(T)));
            }*/
            m_p = meta::typesafe_realloc<lifetime_util>(m_p, m_count, n, static_cast<base0_type&>(*this));
            m_capacity = n;
        } else {
            base0_type::free(m_p);
            m_p = nullptr;
            m_capacity = 0;
        }
    }

    // Modifiers
    void             clear()
    {
        lifetime_util::deconstruct_range(m_p, m_p + m_count);
        m_count = 0;
    }

    iterator insert(iterator pos, const T& value)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::value_construct_range(pos, pos + 1, value);
        m_count++;
    }

    iterator insert(const_iterator pos, const T& value)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::value_construct_range(pos, pos + 1, value);
        m_count++;
    }

    //iterator insert(const_iterator pos, T&& value);

    void    insert(iterator pos, size_type count, const T& value)
    {
        reserve(m_count + count);
        lifetime_util::move(pos + count, pos, end());
        lifetime_util::value_construct_range(pos, pos + count, value);
        m_count += count;
    }

    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        reserve(m_count + count);
        lifetime_util::move(pos + count, pos, end());
        lifetime_util::value_construct_range(pos, pos + count, value);
        m_count += count;
    }

    template< class InputIt >
    void insert(iterator pos, InputIt first, InputIt last)
    {
        size_type count = count(first, last);
        reserve(m_count + count);
        lifetime_util::move(pos + count, pos, end());
        lifetime_util::value_construct_range(pos, pos + count, value);
        m_count += count;
    }

    template< class InputIt >
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        size_type count = count(first, last);
        reserve(m_count + count);
        lifetime_util::move(pos + count, pos, end());
        lifetime_util::value_construct_range(pos, pos + count, value);
        m_count += count;
    }

    //iterator insert(const_iterator pos, std::initializer_list<T> ilist);

    template< gtl_tmp_typename1 >
    iterator emplace(const_iterator pos, gtl_dec_typename1)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::emplace(pos, gtl_use_typename1);
        m_count++;
    }

    template< gtl_tmp_typename2 >
    iterator emplace(const_iterator pos, gtl_dec_typename2)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::emplace(pos, gtl_use_typename2);
        m_count++;
    }

    template< gtl_tmp_typename3 >
    iterator emplace(const_iterator pos, gtl_dec_typename3)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::emplace(pos, gtl_use_typename3);
        m_count++;
    }

    template< gtl_tmp_typename4 >
    iterator emplace(const_iterator pos, gtl_dec_typename4)
    {
        reserve(m_count + 1);
        lifetime_util::move(pos + 1, pos, end());
        lifetime_util::emplace(pos, gtl_use_typename4);
        m_count++;
    }

    iterator erase(iterator pos)
    {
        gtl_assert(m_count);
        lifetime_util::deconstruct_range(pos, pos+1);
        lifetime_util::move(pos, pos + 1, end());
        m_count--;
    }

    iterator erase(const_iterator pos)
    {
        gtl_assert(m_count);
        lifetime_util::deconstruct_range(pos, pos + 1);
        lifetime_util::move(pos, pos + 1, end());
        m_count--;
    }

    iterator erase(iterator first, iterator last)
    {
        lifetime_util::deconstruct_range(pos, last);
        lifetime_util::move(pos, last, end());
        m_count -= last - first;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        lifetime_util::deconstruct_range(pos, last);
        lifetime_util::move(pos, last, end());
        m_count -= last - first;
    }

    void push_back(const T& value)
    {
        reserve(m_count + 1);
        lifetime_util::value_construct_range(end(), end()+1, value);
        m_count++;
    }

    //void push_back(T&& value);

    template< gtl_tmp_typename1 >
    void emplace_back(gtl_dec_typename1)
    {
        reserve(m_count + 1);
        lifetime_util::emplace(end(), gtl_use_typename1);
        m_count++;
    }

    template< gtl_tmp_typename2 >
    void emplace_back(gtl_dec_typename2)
    {
        reserve(m_count + 1);
        lifetime_util::emplace(end(), gtl_use_typename2);
        m_count++;
    }

    template< gtl_tmp_typename3 >
    void emplace_back(gtl_dec_typename3)
    {
        reserve(m_count + 1);
        lifetime_util::emplace(end(), gtl_use_typename3);
        m_count++;
    }

    template< gtl_tmp_typename4 >
    void emplace_back(gtl_dec_typename4)
    {
        reserve(m_count + 1);
        lifetime_util::emplace(end(), gtl_use_typename4);
        m_count++;
    }

    void pop_back()
    {
        gtl_assert(m_count);
        lifetime_util::deconstruct_range(end()-1, end());
        m_count--;
    }

    void resize(size_type count)
    {
        if (count < m_count) {
            lifetime_util::deconstruct_range(m_p + count, m_p + m_count);
            m_count = count;
        } else
        if (count > m_count) {
            reserve(count);
            lifetime_util::default_construct_range(m_p + m_count, m_p + count);
            m_count = count;
        }
    }

    void resize(size_type count, const value_type& value)
    {
        if (count < m_count) {
            lifetime_util::deconstruct_range(m_p + count, m_p + m_count);
            m_count = count;
        }
        else
        if (count > m_count) {
            reserve(count);
            lifetime_util::value_construct_range(m_p + m_count, m_p + count, value);
            m_count = count;
        }
    }

    void swap(base_vector& i_rhs)
    {
        gtl_assert(1); // TODO: heaps are the same
        swap(m_p, i_rhs.m_p);
        swap(m_count, i_rhs.m_count);
        swap(m_capacity, i_rhs.m_capacity);
    }

private:
    ptr_type        m_p;
    count_type      m_count;
    count_type      m_capacity;
};

//
// The vector
//
template<typename T, class THeap = system_heap>
class vector : public base_vector < vector_cfg< T, THeap > >
{
    typedef base_vector < vector_cfg< T, THeap > > base;
public:
    typedef typename base::Allocator Allocator;

    explicit vector(const Allocator& i_heap) : base(i_heap)  {}
    vector() : base(Allocator()) {}
    //    explicit base_vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator());
    vector(size_type count, const T& value, const Allocator& alloc = Allocator()) : base(count, value, alloc) { }
    explicit vector(size_type count) : base(count) {  }
    explicit vector(size_type count, const Allocator& alloc = Allocator()) : base(count, alloc) { }
    template< class InputIt >
    vector(InputIt first, InputIt last, const Allocator& alloc = Allocator()) : base(first, last, alloc) {  }
    vector(const vector& other) : base(other) {  }
    vector(const vector& other, const Allocator& alloc) : base(other, alloc) {  }
    //        vector(vector&& other)
    //        vector(vector&& other, const Allocator& alloc);
    //    vector(std::initializer_list<T> init, const Allocator& alloc = Allocator());
    inline ~vector() { }
};


template<typename T, class THeap> inline
const typename vector<T, THeap>::iterator begin(vector<T, THeap>& i_vector)
{
    return i_vector.begin();
}

template<typename T, class THeap> inline
const typename vector<T, THeap>::iterator end(vector<T, THeap>& i_vector)
{
    return i_vector.end();
}

};

#endif GTL_VECTOR
