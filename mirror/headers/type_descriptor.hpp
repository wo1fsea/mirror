#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_
#define _MIRROR_TYPE_DESCRIPTOR_

#include <string>
#include <vector>
#include <map>
#include <any>

#include "error_handler.hpp"

namespace mirror
{

class type_descriptor
{
public:
    const char *name;
    size_t size;

    type_descriptor(const char *type_name, size_t type_size) : name(type_name), size(type_size) {}

    virtual bool set_value(void *ptr, std::any value) = 0;
    virtual std::any get_value(void *ptr) = 0;

    virtual bool set_value_from_string(void *ptr, std::string string_value) = 0;
    virtual std::string get_value_to_string(void *ptr) = 0;
};

template <typename T>
class type_descriptor_t : public type_descriptor
{
public:
    using type = T;
    using type_descriptor::type_descriptor;
    
    bool set_value(void *ptr, std::any value)
    {
        try
        {
            auto t_ptr = static_cast<T *>(ptr);
            auto value2 = type_converter<T>::convert(value);
            *t_ptr = std::any_cast<T>(value2);
            return true;
        }
        catch (const std::bad_any_cast &bace)
        {
            runtime_error_handler(bace);
            return false;
        }
    }

    std::any get_value(void *ptr)
    {
        std::any value = *static_cast<T *>(ptr);
        return value;
    }

    bool set_value_from_string(void *ptr, std::string string_value)
    {
        auto t_ptr = static_cast<T *>(ptr);
        return string_converter<T>::set_from_string(ptr, string_value);
    }

    std::string get_value_to_string(void *ptr)
    {
        auto t_ptr = static_cast<T *>(ptr);
        std::string string_value = string_converter<T>::to_string(*t_ptr);
        return string_value;
    }
};

} // namespace mirror

#endif