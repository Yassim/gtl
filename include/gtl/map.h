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
#include "meta\lifetime_util.h"
#endif // !GTLMETA_LIFETIME

namespace gtl {

template<
    typename i_key_type,
    typename i_data_type,
    class    i_heap,
    typename i_count = int,
    class    i_locking = locking_noop,
    typename i_growth = grow_pow2_cap<512>,
    typename i_key_ptr = i_key_type*,
    typename i_data_ptr = i_data_type*,
    typename i_key_lifetime = typename meta::lifetime_util_select<i_key_type>::type,
    typename i_data_lifetime = typename meta::lifetime_util_select<i_data_type>::type
> struct map_cfg
{
    typedef typename i_key_type     key_type;
    typedef typename i_data_type   data_type;
    typedef          i_heap     heap_base;
    typedef typename i_count    count_type;
    typedef typename i_locking  lock_base;
    typedef typename i_growth   growth_policy;
    typedef typename i_key_ptr  key_ptr_type;
    typedef typename i_data_ptr  data_ptr_type;
    typedef typename i_key_lifetime key_lifetime_util;
    typedef typename i_data_lifetime data_lifetime_util;
};

template<typename cfg_type>
class base_map
    : private cfg_type::heap_base
    , private cfg_type::lock_base
{
    typedef typename cfg_type::key_type             key_type;
    typedef typename cfg_type::data_type            data_type;
    typedef typename cfg_type::count_type           count_type;
    typedef typename cfg_type::key_ptr_type         key_ptr_type;
    typedef typename cfg_type::data_ptr_type        data_ptr_type;
    typedef typename cfg_type::heap_base            base0_type;
    typedef typename cfg_type::lock_base            base1_type;
    typedef typename cfg_type::growth_policy        growth_policy;
    typedef typename cfg_type::key_lifetime_util    key_lifetime_util;
    typedef typename cfg_type::data_lifetime_util   data_lifetime_util;
public:
    typedef typename cfg_type::lock_base            lock_type;

    typedef  pair<const key_type&, data_type&>      value_type;

    class Allocator : public cfg_type::heap_base {};

private:
    key_ptr_type    m_keys;
    data_ptr_type   m_datas;
    count_type      m_count;
    count_type      m_capacity;
};

template<typename key_type, typename data_type, class THeap = SystemHeap>
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
