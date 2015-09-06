#include "../../include/gtl/persistant/filebacked_heap.h"
#include "../../include/gtl/persistant/offset_ptr.h"

namespace gtl {
namespace persistant {

image_base* image_base::sm_first_image = nullptr;

/*static*/
image_base* image_base::find_owner(const void* i_p)
{
    for (image_base* i = sm_first_image; i; i = i->m_nextImage) {
        if (i->contains(i_p)) {
            return i;
        }
    }

    return nullptr;
}

image_base::image_base(const char* i_path, size_type i_kHeaderSize)
    : kHeaderSize(i_kHeaderSize)
{
    (void)i_path;
    m_nextImage = sm_first_image;
    sm_first_image = this;
}

image_base::~image_base()
{
    if (sm_first_image == this) {
        sm_first_image = this->m_nextImage;
    } else {
        for (image_base* i = sm_first_image; i; i = i->m_nextImage) {
            if (i->m_nextImage == this) {
                i->m_nextImage = this->m_nextImage;
                break;
            }
        }
    }
}


struct image_base::chunk_type 
{
    enum {
        kChunkFlag_Free = 1,
    };

    offset_ptr_wflags<chunk_type>   next; // could also be used for size
    offset_ptr<chunk_type>          prev; 

    inline bool IsFree() const  { return next.flags() & kChunkFlag_Free; }
    inline void SetFree()       { next.set_flags(kChunkFlag_Free);  }
    inline size_type Size() const
    {
        const chunk_type * n = next;
        const size_type c = (n - this)-1;
        return c * sizeof(chunk_type);
    }
    static inline chunk_type* from_void_ptr(void* i_p) { return reinterpret_cast<chunk_type*>(i_p)-1; }
    inline void * to_void_ptr() { return this + 1; }
    chunk_type_free * make_free()
    {
        chunk_type * out = this;
        if (next->IsFree()) {
            // consolidate next;
            next = next->next;
        }

        if (prev->IsFree()) {
            out = prev;
            out->next = next.get();
        }

        out->SetFree();

        return reinterpret_cast<chunk_type_free*>(out);
    }
};

struct image_base::chunk_type_free : public image_base::chunk_type
{
    offset_ptr<chunk_type_free> next_free; // perhaps a double linked list?
};

void * image_base::alloc(size_type i_n)
{
    for (chunk_type_free *p = nullptr, * i = m_free; i; p = i, i = i->next_free) {
        if (i->Size() <= i_n) { // first fit

            return i->to_void_ptr();
        }
    }

    // extend space
    return ::malloc(i_n);
}

void * image_base::realloc(void * i_p, size_type i_n) 
{
    void * p = alloc(i_n);
    memcpy(p, i_p, i_n);
    free(i_p);
    return p; 
}

void image_base::free(void * i_p)
{
    chunk_type* c = chunk_type::from_void_ptr(i_p);
    c->make_free(); // not right yet
}

size_type image_base::max_size(size_type i_n)         { return i_n; }

}
}

