#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_CLASS_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_CLASS_

#include <any>
#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <boost/preprocessor.hpp>

#include "string_converter.hpp"

namespace mirror
{
class type_descriptor;
template <typename T>
class type_descriptor_t;

template <typename T>
struct type_converter
{
    static std::any convert(std::any value)
    {
        return value;
    }
};

template <>
struct type_converter<float>
{
    static std::any convert(std::any value)
    {
        if (value.type() == typeid(double))
        {
            auto double_value = std::any_cast<double>(value);
            return std::any((float)double_value);
        }
        if (value.type() == typeid(int))
        {
            auto int_value = std::any_cast<int>(value);
            return std::any((float)int_value);
        }
        return value;
    }
};

struct member_descriptor
{
    const char *name;
    size_t offset;
    type_descriptor *type_descriptor_ptr;
};

template <typename T>
class type_descriptor_for_class : public type_descriptor_t<T>
{
public:
    using member_descriptors = std::vector<member_descriptor>;
    using member_descriptors_map = std::map<std::string, member_descriptor>;

    member_descriptors members;
    member_descriptors_map members_map;

    type_descriptor_for_class(const char *type_name, size_t type_size, member_descriptors type_members = {}) : type_descriptor_t(type_name, type_size),
                                                                                                               members(type_members)
    {
        for (auto member : members)
        {
            members_map.emplace(member.name, member);
        }
    }

    member_descriptor *get_member_descriptor(std::string member_name)
    {
        auto it = members_map.find(member_name);
        return it == members_map.end() ? nullptr : &it->second;
    }

    bool set_property(void *ptr, std::string member_name, std::any value)
    {
        if (!ptr)
        {
            return false;
        }

        auto md = this->get_member_descriptor(member_name);

        if (!md)
        {
            return false;
        }

        if (!md->type_descriptor_ptr)
        {
            return false;
        }

        return md->type_descriptor_ptr->set_value((char *)ptr + md->offset, value);
    }

    std::any get_property(void *ptr, std::string member_name)
    {
        std::any value;
        if (!ptr)
        {
            return value;
        }

        auto md = this->get_member_descriptor(member_name);
        if (!md || !md->type_descriptor_ptr)
        {
            return value;
        }

        return md->type_descriptor_ptr->get_value((char *)ptr + md->offset);
    }

    bool set_property_from_string(void *ptr, std::string member_name, std::string string_value)
    {
        if (!ptr)
        {
            return false;
        }

        auto md = this->get_member_descriptor(member_name);

        if (!md)
        {
            return false;
        }

        if (!md->type_descriptor_ptr)
        {
            return false;
        }

        return md->type_descriptor_ptr->set_value_from_string((char *)ptr + md->offset, string_value);
    }

    std::string get_property_to_string(void *ptr, std::string member_name)
    {
        std::string string_value;
        if (!ptr)
        {
            return string_value;
        }

        auto md = this->get_member_descriptor(member_name);
        if (!md || !md->type_descriptor_ptr)
        {
            return string_value;
        }

        return md->type_descriptor_ptr->get_value_to_string((char *)ptr + md->offset);
    }
};

#define REFLECTABLE_MEMBER(member_name) \
    member_descriptor{                  \
        #member_name,                   \
        offsetof(T, member_name),       \
        type_descriptor_resolver<decltype(T::member_name)>::get()},

#define REFLECTABLE_MEMBER_FOR_EACH(r, data, i, x) \
    REFLECTABLE_MEMBER(x)

#define REFLECTABLE_CLASS(class_name, members)                                                                                               \
    static_assert(std::is_default_constructible<class_name>::value, #class_name "is not default constructible");                             \
    template <>                                                                                                                              \
    struct type_descriptor_resolver<class_name>                                                                                              \
    {                                                                                                                                        \
        using T = class_name;                                                                                                                \
        static type_descriptor_for_class<T> *get()                                                                                           \
        {                                                                                                                                    \
            static type_descriptor_for_class<T> td(                                                                                          \
                #class_name,                                                                                                                 \
                sizeof(T),                                                                                                                   \
                {BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_MEMBER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(members), members))}); \
            return &td;                                                                                                                      \
        }                                                                                                                                    \
    };
} // namespace mirror

#endif