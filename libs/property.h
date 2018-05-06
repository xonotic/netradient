#ifndef INCLUDED_IMPORTEXPORT_H
#define INCLUDED_IMPORTEXPORT_H

#include "generic/callback.h"
#include "string/string.h"

template<class T>
struct Property {
    // todo: just return T, don't use continuation passing style
    Callback<void(const Callback<void(T)> &returnz)> get;
    Callback<void(T value)> set;
};

// implementation

template<class Self, class T = Self>
struct PropertyImpl {
    static void Export(const Self &self, const Callback<void(T)> &returnz) {
        returnz(self);
    }

    static void Import(Self &self, T value) {
        self = value;
    }
};

namespace detail {

    template<class I>
    using propertyimpl_self = typename std::remove_reference<get_argument<decltype(&I::Import), 0>>::type;

    template<class I>
    using propertyimpl_other = get_argument<decltype(&I::Import), 1>;

    template<class I>
    using propertyimpl_other_free = get_argument<decltype(&I::Import), 0>;

}

// adaptor

template<
        class Self,
        class T = Self,
        class I = PropertyImpl<Self, T>
>
struct PropertyAdaptor {
    using Type = Self;
    using Other = T;

    using Get = ConstReferenceCaller<Self, void(const Callback<void(T)> &), I::Export>;
    using Set = ReferenceCaller<Self, void(T), I::Import>;
};

template<
        class T,
        class I
>
struct PropertyAdaptorFree {
    using Other = T;

    using Get = FreeCaller<void(const Callback<void(T)> &), I::Export>;
    using Set = FreeCaller<void(T), I::Import>;
};

// explicit full

template<class A>
Property<typename A::Other> make_property(typename A::Type &self) {
    return {typename A::Get(self), typename A::Set(self)};
}

template<class A>
Property<typename A::Other> make_property() {
    return {typename A::Get(), typename A::Set()};
}

// explicit impl

template<class I, class Self = detail::propertyimpl_self<I>, class T = detail::propertyimpl_other<I>>
using property_impl = PropertyAdaptor<Self, T, I>;

template<class I, class Self, class T = detail::propertyimpl_other<I>>
Property<T> make_property(Self &self) {
    return make_property<property_impl<I>>(self);
}

template<class I, class T = detail::propertyimpl_other_free<I>>
using property_impl_free = PropertyAdaptorFree<T, I>;

template<class I, class T = detail::propertyimpl_other_free<I>>
Property<T> make_property() {
    return make_property<property_impl_free<I>>();
}

// implicit

template<class Self, class T = Self>
Property<T> make_property(Self &self) {
    return make_property<PropertyAdaptor<Self, T>>(self);
}

// chain

template <class DST, class SRC, class X, class A>
struct PropertyChainImpl {
    static void ExportThunk(const Callback<void(DST)> &self, SRC value) {
        PropertyImpl<SRC, DST>::Export(value, self);
    }

    static void Export(const X &self, const Callback<void(DST)> &returnz) {
        A::Get::thunk_(self, ConstReferenceCaller<Callback<void(DST)>, void(SRC), ExportThunk>(returnz));
    }

    static void Import(X &self, DST value) {
        SRC out;
        PropertyImpl<SRC, DST>::Import(out, value);
        A::Set::thunk_(self, out);
    }
};

template<class I_Outer, class I_Inner>
Property<detail::propertyimpl_other<I_Outer>> make_property_chain(detail::propertyimpl_self<I_Inner> &it) {
    using DST = detail::propertyimpl_other<I_Outer>;
    using SRC = detail::propertyimpl_self<I_Outer>;
    using X = detail::propertyimpl_self<I_Inner>;
    using A = property_impl<I_Inner>;
    return make_property<PropertyAdaptor<X, DST, PropertyChainImpl<DST, SRC, X, A>>>(it);
}

template <class DST, class SRC, class A>
struct PropertyChainImpl_free {
    static void ExportThunk(const Callback<void(DST)> &self, SRC value) {
        PropertyImpl<SRC, DST>::Export(value, self);
    }

    static void Export(const Callback<void(DST)> &returnz) {
        A::Get::thunk_(nullptr, ConstReferenceCaller<Callback<void(DST)>, void(SRC), ExportThunk>(returnz));
    }

    static void Import(DST value) {
        SRC out;
        PropertyImpl<SRC, DST>::Import(out, value);
        A::Set::thunk_(nullptr, out);
    }
};

template<class I_Outer, class I_Inner>
Property<detail::propertyimpl_other<I_Outer>> make_property_chain() {
    using DST = detail::propertyimpl_other<I_Outer>;
    using SRC = detail::propertyimpl_self<I_Outer>;
    using A = property_impl_free<I_Inner>;
    return make_property<PropertyAdaptorFree<DST, PropertyChainImpl_free<DST, SRC, A>>>();
}

// specializations

template<>
struct PropertyImpl<CopiedString, const char *> {
    static void Export(const CopiedString &self, const Callback<void(const char *)> &returnz) {
        returnz(self.c_str());
    }

    static void Import(CopiedString &self, const char *value) {
        self = value;
    }
};

#endif
