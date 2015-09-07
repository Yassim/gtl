#ifndef GTLSPECIALIZED_INTERFACE
#define GTLSPECIALIZED_INTERFACE


#ifndef GTL_TYPES
#include "../types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "../utilities.h"
#endif // !GTL_UTILS


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

}
}
#endif // GTLSPECIALIZED_INTERFACE
