#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_PRIMITIVE_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_PRIMITIVE_

#include <string>
#include <cstdint>

namespace mirror
{

template <typename T>
class type_descriptor_t;

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

// void*

template <typename T>
class type_descriptor_for_primitive : public type_descriptor_t<T>
{
public:
    using type_descriptor_t<T>::type_descriptor_t;
};

} // namespace mirror

#endif