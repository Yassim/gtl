#ifndef GTL_ARRAY
#define GTL_ARRAY

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "utilities.h"
#endif // !GTL_UTILS

#ifndef GTLMETA_LIFETIME
#include "meta/lifetime_util.h"
#endif // !GTLMETA_LIFETIME

namespace gtl {

template<typename T, size_type N>
class array
{
private:
    T m_data[N];
};

// TEMPLATE FUNCTION begin
template<typename T, int N> 
inline T* begin(T i_array[N])
{	// get beginning of sequence
    return i_array;
}

// TEMPLATE FUNCTION begin
template<typename T, size_type N>
inline T* end(T i_array[N])
{	// get beginning of sequence
    return i_array + N;
}

// TEMPLATE FUNCTION begin
template<typename T, size_type N>
inline const T* cbegin(const T i_array[N])
{	// get beginning of sequence
    return i_array;
}

// TEMPLATE FUNCTION begin
template<typename T, int N>
inline const T* cend(const T i_array[N])
{	// get beginning of sequence
    return i_array + N;
}

};

#endif // !GTL_ARRAY
