#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_RESOLVER_
#define _MIRROR_TYPE_DESCRIPTOR_RESOLVER_

#include <boost/preprocessor.hpp>

namespace mirror
{

#define PRIMITIVE_TYPE (           \
    bool,                          \
    char, unsigned char,           \
    short, unsigned short,         \
    int, unsigned int,             \
    long, unsigned long,           \
    long long, unsigned long long, \
    wchar_t,                       \
    float,                         \
    double,                        \
    long double,                   \
    std::string)

class type_descriptor;
template <typename T>
class type_descriptor_for_primitive;

template <typename T>
struct type_descriptor_resolver
{
    static type_descriptor *get()
    {
        // static_assert(false, "unsupported type.");
        return nullptr;
    }
};

#define PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER(x)                          \
    template <>                                                        \
    struct type_descriptor_resolver<x>                                 \
    {                                                                  \
        static type_descriptor *get()                                  \
        {                                                              \
            static type_descriptor_for_primitive<x> td(#x);			   \
            return &td;                                                \
        }                                                              \
    };

#define PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH(r, data, i, x) \
    PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER(x)

#define PRIMITIVE_TYPE_POINTER_DESCRIPTOR_RESOLVER_FOR_EACH(r, data, i, x) \
    PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER(x *)

BOOST_PP_SEQ_FOR_EACH_I(PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(PRIMITIVE_TYPE), PRIMITIVE_TYPE))
BOOST_PP_SEQ_FOR_EACH_I(PRIMITIVE_TYPE_POINTER_DESCRIPTOR_RESOLVER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(PRIMITIVE_TYPE), PRIMITIVE_TYPE))

} // namespace mirror

#endif