#ifndef GTLSPECIALIZED_PHASABLE
#define GTLSPECIALIZED_PHASABLE


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
namespace specialized {

template<typename message_type>
class messageable_interface
{
public:
    typedef void(*on_msg_type)(messageable_interface * i_self, message_type i_msg);

    ~messageable_interface() {}
    messageable_interface() :opaque(nullptr), on_msg(no_op) {}
    messageable_interface(void * i_opaque, on_msg_type i_on_msg) :opaque(i_opaque), on_msg(i_on_msg) {}

    inline void msg(message_type i_msg) {
        on_msg(this, i_msg);
    }

private:
    void * opaque;
    on_msg_type on_msg;

    static void no_op(messageable_interface * i_self, message_type i_msg) {
        (void)i_self;
        (void)i_msg;
    }
};

template<size_type kBitCount>
class bitset
{
    typedef uint32_t block_type;
    enum {
        kBlockCount = kBitCount / (sizeof(block_type) * 8),
        kAllBits = (block_type)~0,

        // 1 2 4 8 16 32 64
        // 0 1 2 3 4  5  6
        // 0 0 0 1 2  4  8
        kSlotShift = 5,
        kSlotMask = (1 << kSlotShift) - 1,
    };
public:
    bitset() { zero_all(); }

    inline void zero_all() {
        memset(m_bits, 0, sizeof(m_bits));
    }

    inline bool all() const {
        const block_type * i = m_bits;
        const block_type * const e = m_bits + kBlockCount;
        block_type acc = *i++;
        for (; i != e; ++i) {
            acc &= *i;
        }
        return acc == kAllBits;
    }

    inline void set_one(size_type i_ndx)
    {
        gtl_assert(i_ndx < kBitCount);
        const size_type slot = i_ndx >> kSlotShift;
        const block_type mask = 1 << (i_ndx & kSlotMask);
        m_bits[slot] |= mask;
    }

    inline void set_zero(size_type i_ndx)
    {
        gtl_assert(i_ndx < kBitCount);
        const size_type slot = i_ndx >> kSlotShift;
        const block_type mask = 1 << (i_ndx & kSlotMask);
        m_bits[slot] &= ~mask;
    }

    inline block_type operator[] (size_type i_ndx)
    {
        gtl_assert(i_ndx < kBitCount);
        const size_type slot = i_ndx >> kSlotShift;
        const block_type mask = 1 << (i_ndx & kSlotMask);
        return m_bits[slot] & mask;
    }

    inline size_type first_zero_index() const
    {
        gtl_assert(!all());
        const block_type * i = m_bits;
        for (; *i == kAllBits; ++i) {}
        const block_type tj = *i;
        int_fast32_t j = 0;
        for (; (tj & (1 << j)); ++j) {}
        return j;
    }

private:
    block_type m_bits[kBlockCount];
};

template<
    typename i_type,
    class    i_heap,
    size_type i_pagecount = 256,
   // typename i_ptr = i_type*,
    typename i_lifetime = typename meta::lifetime_util_select<i_type>::type
> struct phaseable_cfg
{
    typedef i_type     T;
    typedef i_heap     heap_base;
    enum {
        kPageCount = i_pagecount
    };
  //  typedef i_ptr      ptr_type;
    typedef i_lifetime lifetime_util;
};

template<typename cfg_type>
class phasable_container
    : private cfg_type::heap_base
{
    typedef typename cfg_type::heap_base base_type;
    typedef typename cfg_type::T T;
    typedef typename cfg_type::lifetime_util lifetime_util;

    enum {
        kPageCount = cfg_type::kPageCount
    };

    typedef uint8_t byte;
    struct page_t {
        byte                m_items[sizeof(T) * kPageCount];
        bitset<kPageCount>  m_inuse;
        page_t *            m_next;
    };

    page_t * m_first;
    page_t * m_last;
    size_type m_count;

    void * first_free()
    {
        // for each page
        for (page_t * pi = m_first; pi; pi = pi->m_next) {
            if (pi->m_inuse.all()) {
                continue;
            }
            size_type firstfree = pi->m_inuse.first_zero_index();
            void * out = pi->m_items + sizeof(T) * firstfree;
            pi->m_inuse.set_one(firstfree);
            return out;
        }
        // new page.
        page_t * p = (page_t*)base_type::alloc(sizeof(page_t));

        // could just zero the entire page.
        p->m_inuse.zero_all();
        p->m_next = nullptr;

        if (m_last) {
            m_last->m_next = p;
            m_last = p;
        } else {
            m_first = m_last = p;
        }

        p->m_inuse.set_one(0);
        return p->m_items;
    }

    void release(void * i_ptr)
    {
        // for all pages
        for (page_t * ppi = nullptr, * pi = m_first; pi; ppi = pi, pi = pi->next) {
            // ptr in this page?
            if (pi <= i_ptr && i_ptr <= (pi + 1)) {
                T * ii = reinterpret_cast<T*>(pi->m_items);
                T * i = reinterpret_cast<T*>(i_ptr);
                pi->m_inuse.set_zero(i - ii);
                lifetime_util::deconstruct_range(i, i+1);

                // tidy page?
                if (pi->m_inuse.all_zero()) {
                    if (ppi) {
                        ppi->m_next = pi->m_next;
                    } else {
                        m_first = pi->m_next;
                    }
                    if (!pi->m_next) {
                        m_last = ppi;
                    }
                    base_type::free(pi);
                }
            }
        }
    }
public:
    phasable_container()
        : m_first(0)
        , m_last(0)
        , m_count(0)
    {}

    ~phasable_container()
    {
        for (page_t * pi = m_first; pi; ) {
            page_t * np = pi->m_next;
            base_type::free(pi);
            pi = np;
        }
    }

    T * alloc()
    {
        T * p = reinterpret_cast<T*>(first_free());
        lifetime_util::emplace(p);
        return p;
    }

    void free(T * i_ptr)
    {
    }

    template<typename TBatchOp>
    void batch(TBatchOp i_batch_op)
    {
        // for each page
        for (page_t * pi = m_first; pi; pi = pi->m_next) {
            T * ii = reinterpret_cast<T*>(pi->m_items);
            T * ie = ii + kPageCount;

            if (pi->m_inuse.all()) {
                i_batch_op(ii, ie);
            } else {
                for (T* s = ii; s != ie; ) {

                    // find start.
                    for (; (s != ie) && (!pi->m_inuse[s - ii]); ++s) {}

                    // find end
                    T* e = s;
                    for (; (e != ie) && (pi->m_inuse[e - ii]); ++e) {}

                    // if we have a valid range, do work
                    if (s != e) {
                        i_batch_op(s, e);
                    }
                    // start is now the end
                    s = e;
                }
            }
        }
    }

    // phasing of methods
    template<void (T::*k_method)() >
    void run_phase()
    {
        // for each page
        for (page_t * pi = m_first; pi; pi = pi->m_next) {

            // make item itterators.
            T * ii = reinterpret_cast<T*>(pi->m_items);
            T * ie = ii + kPageCount;

            // if all items on the page are inuse
            if (pi->m_inuse.all()) {
                // doa slighlty faster loop with no insue checking. (hopfully, the common case).
                for (; ii != ie; ++ii) {
                    (ii->*k_method)();
                }
            } else {
                // make a working iterator
                for (T * i = ii; i != ie; ++i) {
                    // test for inuse.
                    // calc index by pointer
                    if (pi->m_inuse[i - ii]) { 
                        (i->*k_method)();
                    }
                }
            }
        }
    }

   /* template<void (T::*i_op)(gtl_dec_typename1), gtl_tmp_typename1 >
    void run_phase(gtl_dec_typename1)
    {
        for (page_t * pi = m_first; pi; pi = pi->next) {
            i_type * ii = pi->m_items;
            for (int i = 0; i < kPageCount; i++, ii++) {
                if (pi->m_inuse[i]) {
                    (ii->*i_op)(gtl_use_typename1);
                }
            }
        }
    }

    template< gtl_tmp_typename2 >
    void run_phase(void (T::*i_op)(gtl_dec_typename2), gtl_dec_typename2)
    {
        for (page_t * pi = m_first; pi; pi = pi->next) {
            i_type * ii = pi->m_items;
            for (int i = 0; i < kPageCount; i++, ii++) {
                if (pi->m_inuse[i]) {
                    (ii->*i_op)(gtl_use_typename2);
                }
            }
        }
    }

    template< gtl_tmp_typename3 >
    void run_phase(void (T::*i_op)(gtl_dec_typename3), gtl_dec_typename3)
    {
        for (page_t * pi = m_first; pi; pi = pi->next) {
            i_type * ii = pi->m_items;
            for (int i = 0; i < kPageCount; i++, ii++) {
                if (pi->m_inuse[i]) {
                    (ii->*i_op)(gtl_use_typename3);
                }
            }
        }
    }

    template< gtl_tmp_typename4 >
    void run_phase(void (T::*i_op)(gtl_dec_typename4), gtl_dec_typename4)
    {
        for (page_t * pi = m_first; pi; pi = pi->next) {
            i_type * ii = pi->m_items;
            for (int i = 0; i < kPageCount; i++, ii++) {
                if (pi->m_inuse[i]) {
                    (ii->*i_op)(gtl_use_typename4);
                }
            }
        }
    }
    */
};

}
}

#endif//GTLSPECIALIZED_PHASABLE
