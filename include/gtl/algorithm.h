#ifndef GTL_ALGORITHM
#define GTL_ALGORITHM

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES


namespace gtl {

    template< typename const_iterator>
    struct search_result
    {
        const_iterator  pos;
        bool            found;
    };

    namespace ordered {

        // O(n)=n
        template<typename T>
        search_result<const T*> linear_search(const T& i_needle, const T * i_haystack, size_type i_n)
        {
            search_result<const T*> out = { nullptr, false };

            if (i_n) {
                const T * end = i_haystack + i_n;
                do {
                    const T&  n = *i_haystack;

                    if (n > i_needle) {
                        break;
                    }
                    else
                        if (!(n < i_needle)) {
                            out.found = true;
                            break;
                        }
                } while (++i_haystack != end);

                out.pos = i_haystack;
            }

            return out;
        }

        // O(n)=log(n)
        template<typename T>
        search_result<const T*> binary_search(const T& i_needle, const T * i_haystack, size_type i_n)
        {
            search_result<const T*> out = { nullptr, false };

            if (i_n) {
                size_type low = 0, high = i_n, mid = 0;

                while (low < high) {
                    mid = low + ((high - low) / 2); // avoids over flows for very large numbers.
                    const T&  n = i_haystack[mid];
                    if (i_needle < n) {
                        high = mid;
                    }
                    else
                        if (i_needle > n) {
                            low = ++mid;
                        }
                        else {
                            out.found = true;
                            break;
                        }
                }

                out.pos = i_haystack + mid;
            }

            return out;
        }

        template<typename T>
        float ratio(const T& i_low, const T& i_mid, const T& i_high)
        {
            return abs(i_mid - i_low) / (float)abs(i_high - i_low);
        }

        // O(n)=log(log(n))
        //     =1 for even distrobuted sets
        template<typename T>
        search_result<const T*> interpolate_search(const T& i_needle, const T * i_haystack, size_type i_n)
        {
            search_result<const T*> out = { nullptr, false };

            if (i_n) {
                size_type low = 0, high = i_n - 1;

                if (i_needle <= i_haystack[low]) {
                    out.pos = i_haystack + low;
                    out.found = i_haystack[low] == i_needle;
                    return out;
                }

                if (i_needle > i_haystack[high]) {
                    out.pos = i_haystack + i_n;
                    return out;
                }

                while (low <= high) {
                    if (low == high) {
                        out.pos = i_haystack + low;
                        out.found = i_haystack[low] == i_needle;
                        return out;
                    }
                    //size_type k = (i_needle - i_haystack[low]) / (i_haystack[high] - i_haystack[low]);
                    size_type mid = low + (i_needle - i_haystack[low]) * ((high - low) / (i_haystack[high] - i_haystack[low]));
                    out.pos = i_haystack + mid;
                    const T&  n = i_haystack[mid];
                    if (i_needle < n) {
                        high = (mid > 1) ? mid - 1 : 0;
                    }
                    else
                        if (i_needle > n) {
                            low = mid + 1;
                        }
                        else {
                            out.found = true;
                            break;
                        }
                }
            }

            return out;
        }
    }
}

#endif // GTL_ALGORITHM
