#ifndef GTL_MAP
#define GTL_MAP

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
#include "meta/lifetime_util.h"
#endif // !GTLMETA_LIFETIME

namespace gtl {

template<
    typename i_key_type,
    typename i_data_type,
    class    i_heap,
    typename i_count = int,
    typename i_growth = grow_pow2_cap<512>,
    typename i_key_ptr = i_key_type*,
    typename i_data_ptr = i_data_type*,
    typename i_key_lifetime = typename meta::lifetime_util_select<i_key_type>::type,
    typename i_data_lifetime = typename meta::lifetime_util_select<i_data_type>::type
> struct map_cfg
{
    typedef i_key_type     key_type;
    typedef i_data_type   data_type;
    typedef i_heap     heap_base;
    typedef i_count    count_type;
    typedef i_growth   growth_policy;
    typedef i_key_ptr  key_ptr_type;
    typedef i_data_ptr  data_ptr_type;
    typedef i_key_lifetime key_lifetime_util;
    typedef i_data_lifetime data_lifetime_util;
};

template<typename cfg_type>
class base_map
    : private cfg_type::heap_base
{
    typedef typename cfg_type::key_type             key_type;
    typedef typename cfg_type::data_type            data_type;
    typedef typename cfg_type::count_type           count_type;
    typedef typename cfg_type::key_ptr_type         key_ptr_type;
    typedef typename cfg_type::data_ptr_type        data_ptr_type;
    typedef typename cfg_type::heap_base            base0_type;
    typedef typename cfg_type::growth_policy        growth_policy;
    typedef typename cfg_type::key_lifetime_util    key_lifetime_util;
    typedef typename cfg_type::data_lifetime_util   data_lifetime_util;
public:
    typedef  pair<const key_type&, data_type&>      value_type;

    class Allocator : public cfg_type::heap_base {};

    // Construction
    explicit base_map(const Allocator& i_heap)
        : base0_type(i_heap)
        , m_keys(nullptr)
        , m_datas(nullptr)
        , m_count(0)
        , m_capacity(0)
    {}
    base_map()
        : base0_type(Allocator())
        , m_keys(nullptr)
        , m_datas(nullptr)
        , m_count(0)
        , m_capacity(0)
    {} //: base_map(Allocator()) {}

    ~base_map()
    {
        clear();
        shrink_to_fit();
    }

    base_map& operator=(const base_map& other)
    {
        if (this != &other) {
            base_map(other).swap(*this);
        }
        return *this;
    }

    // this we are actually object shearing 
    inline Allocator allocator() const             { return Allocator(*this); }

    // Element access
    inline data_type&       at(const key_type& i_ndx)                 { return m_datas[index_of(i_ndx)]; }
    inline const data_type& at(const key_type& i_ndx) const           { return m_datas[index_of(i_ndx)]; }
    inline data_type&       operator[](const key_type& i_ndx)         { return m_datas[index_of(i_ndx)]; }
    inline const data_type& operator[](const key_type& i_ndx) const   { return m_datas[index_of(i_ndx)]; }

    // Iterators
    typedef iterator_pair<const key_type&, data_type&, key_type*, data_type*>                           iterator;
    typedef iterator_pair<const key_type&, const data_type&, const key_type*, const data_type*>         const_iterator;
    //typedef const T*    const_iterator;

    inline iterator       begin()                       { return iterator(m_keys, m_datas); }
    inline const_iterator begin() const                 { return const_iterator(m_keys, m_datas); }
    inline const_iterator cbegin() const                { return const_iterator(m_keys, m_datas); }
    inline iterator       end()                         { return iterator(m_keys + m_count, m_datas + m_count); }
    inline const_iterator end() const                   { return const_iterator(m_keys + m_count, m_datas + m_count); }
    inline const_iterator cend() const                  { return const_iterator(m_keys + m_count, m_datas + m_count);; }
    //inline iterator       rbegin()                      { return m_p; }
    //inline const_iterator rbegin() const                { return m_p; }
    //inline const_iterator crbegin() const               { return m_p; }
    //inline iterator       rend()                        { return m_p + m_count; }
    //inline const_iterator rend() const                  { return m_p + m_count; }
    //inline const_iterator crend() const                 { return m_p + m_count; }

    // Capacity
    inline bool      empty() const                      { return 0 == m_count; }
    inline size_type size() const                       { return m_count; }
    inline size_type max_size() const                   { return base0_type::max_size((sizeof(key_type) + sizeof(data_type)) * std::numeric_limits<count_type>::max()) / (sizeof(key_type) + sizeof(data_type)); }

    void             reserve(size_type i_n)
    {
        const size_type n = growth_policy::next_size(i_n);
        if (n > static_cast<size_type>(m_capacity)) {
            m_keys = meta::typesafe_realloc<key_lifetime_util>(m_keys, m_count, n, static_cast<base0_type&>(*this));
            m_datas = meta::typesafe_realloc<data_lifetime_util>(m_datas, m_count, n, static_cast<base0_type&>(*this));
            m_capacity = n;
        }
    }

    inline size_type capacity() const                   { return m_capacity; }

    void             shrink_to_fit()
    {
        if (m_count) {
            const size_type n = growth_policy::next_size(m_count);
            m_keys = meta::typesafe_realloc<key_lifetime_util>(m_keys, m_count, n, static_cast<base0_type&>(*this));
            m_datas = meta::typesafe_realloc<data_lifetime_util>(m_datas, m_count, n, static_cast<base0_type&>(*this));
            m_capacity = n;
        } else {
            base0_type::free(m_keys);
            base0_type::free(m_datas);
            m_keys = nullptr;
            m_datas = nullptr;
            m_capacity = 0;
        }
    }

    // Modifiers
    void             clear()
    {
        key_lifetime_util::deconstruct_range(m_keys, m_keys + m_count);
        data_lifetime_util::deconstruct_range(m_datas, m_datas + m_count);
        m_count = 0;
    }





    void swap(base_map& i_rhs)
    {
        gtl_assert(1); // TODO: heaps are the same
        swap(m_keys, i_rhs.m_keys);
        swap(m_datas, i_rhs.m_datas);
        swap(m_count, i_rhs.m_count);
        swap(m_capacity, i_rhs.m_capacity);
    }

private:
    size_type index_of(const key_type& i_key)
    {
        // permitted to grow
        key_type * i = m_keys;
        key_type * e = m_keys + m_count;
        size_type ndx = m_count;

        //TODO: better than liniear
        for (; i != e; ++i) {
            if (*i == i_key) {
                return i - m_keys;
            }
            if (*i > i_key) {
                // where it should be, but isnt.
                //lets expand and slot this in.
                ndx = i - m_keys;
                break;
            }
        }
        // else not here, and should be at the end
        // ndx should be untouched, thus at m_count
        reserve(m_count + 1);
        // refresh itorators
        i = m_keys + ndx;
        e = m_keys + m_count;
        key_lifetime_util::move(i + 1, i, e);
        key_lifetime_util::value_construct_range(i, i + 1, i_key);

        data_type * di = m_datas + ndx;
        data_type * de = m_datas + m_count;

        data_lifetime_util::move(di + 1, di, de);
        data_lifetime_util::default_construct_range(di, di + 1);

        m_count++;

        return ndx;
    }

    size_type index_of(const key_type& i_key) const
    {
        // assert if not found
        key_type * i = m_keys;
        key_type * e = m_keys + m_count;
        size_type ndx = 0;

        //TODO: better than liniear
        for (; i != e; ++i) {
            if (*i == i_key) {
                return i - m_keys;
            }
            if (*i > i_key) {
                break;
            }
        }
        gtl_assert_msg(false, "Key not found");
        return ~0;
    }

    key_ptr_type    m_keys;
    data_ptr_type   m_datas;
    count_type      m_count;
    count_type      m_capacity;
};

template<typename key_type, typename data_type, class THeap = system_heap>
class map : public base_map < map_cfg< key_type, data_type, THeap > >
{
};

template<typename key_type, typename data_type, class THeap>
inline const typename map<key_type, data_type, THeap>::iterator begin(map<key_type, data_type, THeap>& i_map)
{
    return i_map.begin();
}

template<typename key_type, typename data_type, class THeap>
inline const typename map<key_type, data_type, THeap>::iterator end(map<key_type, data_type, THeap>& i_map)
{
    return i_map.end();
}


};

#endif // !GTL_MAP
