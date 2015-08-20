#ifndef GTLPERSISTANT_OFFSET_PTR
#define GTLPERSISTANT_OFFSET_PTR

#ifndef GTL_TYPES
#include "../types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "../utilities.h"
#endif // !GTL_UTILS


namespace gtl {
namespace persistant {

// TODO: an actual pointer select
// TODO: test if a branchless solution, or an intrinsic is possible/better perf wise.
#define gtl_pselect( BE, T, F) ( (BE) ? (T) : (F) )

template< typename cfg_storage_type >
class base_offset_ptr
{
    typedef cfg_storage_type storage_type;

public:

    inline void * get()             { return gtl_pselect(0 != m_offset, reinterpret_cast<void*>(reinterpret_cast<intptr_t>(this) + static_cast<intptr_t>(m_offset)), nullptr); }
    inline const void * get() const { return gtl_pselect(0 != m_offset, reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(this) + static_cast<intptr_t>(m_offset)), nullptr); }

    inline void set(const void* i_p)      { m_offset = i_p == nullptr ? 0 : static_cast<storage_type>(reinterpret_cast<intptr_t>(i_p)-reinterpret_cast<intptr_t>(this)); }

    inline explicit operator bool() const    { return 0 != m_offset; }

    inline base_offset_ptr& operator=(base_offset_ptr& i_rhs)
    {
        set(i_rhs.get());
        return *this;
    }

    inline base_offset_ptr& operator=(void* i_rhs)
    {
        set(i_rhs);
        return *this;
    }
private:
    storage_type m_offset;
};

template<typename T, typename storage_type = int32_t>
class offset_ptr : public base_offset_ptr<storage_type>
{
    typedef base_offset_ptr<storage_type> base_type;
public:
    offset_ptr()                            { base_type::set(nullptr); }
    offset_ptr(T* i_p)                      { base_type::set(i_p); }
    offset_ptr(const offset_ptr& i_rhs)     { base_type::set(i_rhs.get()); }
    ~offset_ptr()                           { }

    inline       T* operator ->()           { return static_cast<T*>(base_type::get()); }
    inline const T* operator ->() const     { return static_cast<const T*>(base_type::get()); }
    inline       T& operator *()            { return *static_cast<T*>(base_type::get()); }
    inline const T& operator *() const      { return *static_cast<const T*>(base_type::get()); }
    inline operator T* ()                   { return static_cast<T*>(base_type::get()); }
    inline operator const T* () const       { return static_cast<const T*>(base_type::get()); }

    inline offset_ptr& operator=(offset_ptr& i_rhs)
    {
        base_type::set(i_rhs.get());
        return *this;
    }

    inline offset_ptr& operator=(T* i_rhs)
    {
        base_type::set(i_rhs);
        return *this;
    }
};


//GTL_LIFETIME_TYPE_SHOUOLD_USE(offset_ptr<T, storage_type>, conservitive);

} // end persistant

namespace meta {
    template<typename T, typename storage_type>
    struct lifetime_util_select< persistant::offset_ptr<T, storage_type> >
    {
        typedef lifetime_conservitive<persistant::offset_ptr<T, storage_type> > type;
    };
}

};

#endif // !GTLPERSISTANT_OFFSET_PTR
