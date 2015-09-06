#ifndef GTLPERSISTANT_FILE_BACKED_HEAP
#define GTLPERSISTANT_FILE_BACKED_HEAP


#ifndef GTL_TYPES
#include "../types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "../utilities.h"
#endif // !GTL_UTILS


namespace gtl {
namespace persistant {

class image_base
{
public:
    image_base(const char* i_path, size_type i_kHeaderSize);
    ~image_base();

    void * alloc(size_type i_n);
    void * realloc(void * i_p, size_type i_n);
    void free(void * i_p);
    size_type max_size(size_type i_n);

    inline bool contains(const void* i_p) const { return m_start <= i_p && i_p < m_end;  }

    void* header_ptr();
    const void* header_ptr() const;

    static image_base* find_owner(const void* i_p);
private:
    image_base();
    image_base& operator=(const image_base&);

    image_base*     m_nextImage;
    const size_type kHeaderSize;

    void*           m_start;
    void*           m_end;


    struct chunk_type;
    struct chunk_type_free;
    chunk_type_free * m_free;

    static image_base* sm_first_image;
};

template< typename header>
class image : public image_base
{
public:
    typedef typename header header_type;

    image(const char* i_path) : image_base(i_path, sizeof(header_type)) {}
    ~image();

    header_type& header()               { *reinterpret_cast<header_type*>(header_ptr()); }
    const header_type& header() const   { *reinterpret_cast<header_type*>(header_ptr()); }

private:
    image();
    image& operator=(const image&);
};

class filebacked_heap
{
public:
    filebacked_heap()                       { gtl_assert(image_base::find_owner(this)); }
    filebacked_heap(const filebacked_heap&) { gtl_assert(image_base::find_owner(this)); }
    ~filebacked_heap() {}

    void * alloc(size_type i_n)             
    { 
        image_base* image = image_base::find_owner(this);
        gtl_assert(image);
        return image->alloc(i_n);
    }

    void * realloc(void * i_p, size_type i_n) 
    {
        image_base* image = image_base::find_owner(this);
        gtl_assert(image);
        gtl_assert(image == image_base::find_owner(i_p));
        return image->realloc(i_p, i_n);
    }

    void free(void * i_p)
    { 
        image_base* image = image_base::find_owner(this);
        gtl_assert(image);
        gtl_assert(image == image_base::find_owner(i_p));
        return image->free(i_p); 
    }

    size_type max_size(size_type i_n)       { return image_base::find_owner(this)->max_size(i_n); }
};

}
}


#endif // !GTLPERSISTANT_FILE_BACKED_HEAP
