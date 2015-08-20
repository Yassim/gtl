#ifndef GTL_MEMORY
#define GTL_MEMORY

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#include <stdlib.h>

namespace gtl {

class system_heap
{
public:
    system_heap() {}
    system_heap(const system_heap& ) {}
    ~system_heap() {}

    inline void * alloc(size_type i_n)               { return ::malloc(i_n); }
    inline void * realloc(void * i_p, size_type i_n) { return ::realloc(i_p, i_n); }
    inline void free(void * i_p)                     { ::free(i_p); }
    inline size_type max_size(size_type i_n)         { return i_n; }
};

#if defined(MSVC)
template<size_type kAlign>
class aligned_system_heap
{
public:
    aligned_system_heap() {}
    aligned_system_heap(const aligned_system_heap& ) {}
    ~aligned_system_heap() {}

    inline void * alloc(size_type i_n)               { return ::_aligned_malloc(i_n, kAlign); }
    inline void * realloc(void * i_p, size_type i_n) { return ::_aligned_realloc(i_p, i_n, kAlign); }
    inline void free(void * i_p)                     { ::_aligned_free(i_p); }
    inline size_type max_size(size_type i_n)         { return i_n; }
};
#endif

};

#endif // !GTL_MEMORY
