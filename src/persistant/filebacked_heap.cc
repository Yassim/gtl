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

struct chunk_type;
enum {
    kChunkFlag_Free = 1,
};
struct chunk_type {
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
};

void * image_base::alloc(size_type i_n)
{
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
    ::free(i_p);

}
size_type image_base::max_size(size_type i_n)         { return i_n; }

}
}

