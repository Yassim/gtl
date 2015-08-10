#ifndef GTLPERSISTANT_DEVIRTUALISED_BASE
#define GTLPERSISTANT_DEVIRTUALISED_BASE


#ifndef GTL_TYPES
#include "..\types.h"
#endif // !GTL_TYPES

#ifndef GTL_UTILS
#include "..\utilities.h"
#endif // !GTL_UTILS


namespace gtl {
namespace persistant {

    template<typename class_storage_type, size_type kNoClasses, size_type kNoMethods>
    class devirtualised_base
    {
    protected:
        class A {}; //fully formed class
        typedef void (A::*method_type)(void); // normal pointer to method. NOT multiple virtual.

        inline devirtualised_base(class_storage_type i_class) :km_class(i_class) {}
        inline ~devirtualised_base() {}

        // Voids
        template<size_type kMethod>
        inline void void_method()
        {
            typedef void (A::*l_method)();
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            (reinterpret_cast<A*>(this)->*to_invoke)();
        }

        template<size_type kMethod, gtl_tmp_typename1>
        inline void void_method(gtl_dec_typename1)
        {
            typedef void (A::*l_method)(gtl_dec_typename1);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename1);
        }

        template<size_type kMethod, gtl_tmp_typename2>
        inline void void_method(gtl_dec_typename2)
        {
            typedef void (A::*l_method)(gtl_dec_typename2);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename2);
        }

        template<size_type kMethod, gtl_tmp_typename3>
        inline void void_method(gtl_dec_typename3)
        {
            typedef void (A::*l_method)(gtl_dec_typename3);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename3);
        }

        template<size_type kMethod, gtl_tmp_typename4>
        inline void void_method(gtl_dec_typename4)
        {
            typedef void (A::*l_method)(gtl_dec_typename4);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename4);
        }

        // With returns
        template<typename rt, size_type kMethod>
        inline rt method()
        {
            typedef rt (A::*l_method)();
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            return (reinterpret_cast<A*>(this)->*to_invoke)();
        }

        template<typename rt, size_type kMethod, gtl_tmp_typename1>
        inline rt method(gtl_dec_typename1)
        {
            typedef rt (A::*l_method)(gtl_dec_typename1);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            return (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename1);
        }

        template<typename rt, size_type kMethod, gtl_tmp_typename2>
        inline rt method(gtl_dec_typename2)
        {
            typedef rt (A::*l_method)(gtl_dec_typename2);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            return (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename2);
        }

        template<typename rt, size_type kMethod, gtl_tmp_typename3>
        inline rt method(gtl_dec_typename3)
        {
            typedef rt (A::*l_method)(gtl_dec_typename3);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            return (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename3);
        }

        template<typename rt, size_type kMethod, gtl_tmp_typename4>
        inline rt method(gtl_dec_typename4)
        {
            typedef rt (A::*l_method)(gtl_dec_typename4);
            const l_method to_invoke = reinterpret_cast<l_method>(sm_vtbl[kMethod][km_class]);
            return (reinterpret_cast<A*>(this)->*to_invoke)(gtl_use_typename4);
        }

        static void set_vtbl(class_storage_type i_class, method_type i_methods[kNoMethods])
        {
            for (size_type i = 0; i < kNoMethods; ++i)
                sm_vtbl[i][i_class] = i_methods[i];
        }

    private:
        devirtualised_base();

        // the only member
        const class_storage_type km_class;

        // The vtable for this "tree"
        static method_type sm_vtbl[kNoMethods][kNoClasses];
    };

    template<typename class_storage_type, size_type kNoClasses, size_type kNoMethods>
    typename devirtualised_base<class_storage_type, kNoClasses, kNoMethods>::method_type devirtualised_base<class_storage_type, kNoClasses, kNoMethods>::sm_vtbl[kNoMethods][kNoClasses] = { 0 };
}
}

#endif // !GTLPERSISTANT_DEVIRTUALISED_BASE
