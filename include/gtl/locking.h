#ifndef GTL_LOCKING
#define GTL_LOCKING

#ifndef GTL_TYPES
#include "types.h"
#endif // !GTL_TYPES

#include <stdlib.h>

namespace gtl {

    struct locking_noop
    {
        void lock() {};
        void unlock() {};
    };
}
#endif // !GTL_LOCKING
