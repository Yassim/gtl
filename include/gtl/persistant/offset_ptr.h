#ifndef GTLPERSISTANT_OFFSET_PTR
#define GTLPERSISTANT_OFFSET_PTR

#ifndef GTL_TYPES
#include "../types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "../utilities.h"
#endif // !GTL_UTILS

#ifndef GTLMETA_LIFETIME
#include "../meta/lifetime_util.h"
#endif // !GTLMETA_LIFETIME


namespace gtl {
namespace persistant {

// TODO: an actual pointer select
// TODO: test if a branchless solution, or an intrinsic is possible/better perf wise.
#define gtl_pselect( BE, T, F) ( (BE) ? (T) : (F) )

template<
    typename i_storage_type,
    int i_kMinAlignment = 1,
    int i_kFlagBits = i_kMinAlignment-1
> struct offset_ptr_cfg
{
    typedef i_storage_type  storage_type;
    static const i_storage_type   kMinAlignment = i_kMinAlignment;
    static const i_storage_type   kFlagBits = i_kFlagBits;
};


template< typename offset_ptr_cfg >
class base_offset_ptr
{
public:
    typedef typename offset_ptr_cfg::storage_type storage_type;

    inline operator bool() const    { return 0 != m_offset; }

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

protected:
    static const storage_type   kMinAlignment = offset_ptr_cfg::kMinAlignment;
    static const storage_type   kFlagBits = offset_ptr_cfg::kFlagBits;
    static const storage_type   kMaskBits = ~offset_ptr_cfg::kFlagBits;

    inline intptr_t value() const   { return static_cast<intptr_t>(m_offset & kMaskBits); }

    inline void * get()             { return gtl_pselect(0 != value(), reinterpret_cast<void*>(reinterpret_cast<intptr_t>(this) + value()), nullptr); }
    inline const void * get() const { return gtl_pselect(0 != value(), reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(this) + value()), nullptr); }

    void set(const void* i_p)      
    {
        const intptr_t p = reinterpret_cast<intptr_t>(i_p);
        const storage_type v = static_cast<storage_type>(p - reinterpret_cast<intptr_t>(this));
        gtl_assert(0 == (v & kMaskBits));

        m_offset = (i_p == nullptr ? 0 : v) | flags(); 
    }

    inline storage_type flags() const { return m_offset & kFlagBits; }
    void set_flags(storage_type i_flags)
    {
        gtl_assert((i_flags & kMaskBits) == i_flags);
        m_offset = (m_offset & kMaskBits) | i_flags; 
    }
private:
    storage_type m_offset;
};

template<typename T, typename offset_type = int32_t>
class offset_ptr : public base_offset_ptr< offset_ptr_cfg< offset_type > >
{
    typedef base_offset_ptr<offset_ptr_cfg< offset_type> > base_type;
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

template<typename T, typename offset_type = int32_t>
class offset_ptr_wflags : public base_offset_ptr< offset_ptr_cfg< offset_type, sizeof(offset_type) > >
{
    typedef base_offset_ptr< offset_ptr_cfg< offset_type, sizeof(offset_type) > > base_type;
    typedef typename base_type::storage_type storage_type;

public:
    offset_ptr_wflags()                            { base_type::set(nullptr); }
    offset_ptr_wflags(T* i_p)                      { base_type::set(i_p); }
    offset_ptr_wflags(const offset_ptr_wflags& i_rhs)     { base_type::set(i_rhs.get()); }
    ~offset_ptr_wflags()                           { }

    inline       T* operator ->()           { return static_cast<T*>(base_type::get()); }
    inline const T* operator ->() const     { return static_cast<const T*>(base_type::get()); }
    inline       T& operator *()            { return *static_cast<T*>(base_type::get()); }
    inline const T& operator *() const      { return *static_cast<const T*>(base_type::get()); }
    inline operator T* ()                   { return static_cast<T*>(base_type::get()); }
    inline operator const T* () const       { return static_cast<const T*>(base_type::get()); }

    inline offset_ptr_wflags& operator=(offset_ptr_wflags& i_rhs)
    {
        base_type::set(i_rhs.get());
        return *this;
    }

    inline offset_ptr_wflags& operator=(T* i_rhs)
    {
        base_type::set(i_rhs);
        return *this;
    }

    inline storage_type flags() const    { return base_type::flags();  }
    void set_flags(storage_type i_flags) { base_type::set_flags(i_flags); }
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
