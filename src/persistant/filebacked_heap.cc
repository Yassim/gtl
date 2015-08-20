#include "../../include/gtl/persistant/filebacked_heap.h"

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

//filebacked_heap::filebacked_heap() {}
//filebacked_heap::filebacked_heap(const filebacked_heap&) {}
//filebacked_heap::~filebacked_heap() {}
//
//void * filebacked_heap::alloc(size_type i_n)               { return ::malloc(i_n); }
//void * filebacked_heap::realloc(void * i_p, size_type i_n) { return ::realloc(i_p, i_n); }
//void filebacked_heap::free(void * i_p)                     { ::free(i_p); }
//size_type filebacked_heap::max_size(size_type i_n)         { return i_n; }

}
}

