#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_ENUM_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_ENUM_

#include <type_traits>
#include <limits>

#include "type_descriptor_for_enum.hpp"
#include "string_converter.hpp"

namespace mirror
{

class type_descriptor;
template <typename T>
class type_descriptor_t;

template <typename T>
class type_descriptor_for_enum : public type_descriptor_t<T>
{
public:
    using underlying_type = typename std::underlying_type<T>::type;

    static const underlying_type enumerator_unfound_value = std::numeric_limits<underlying_type>::max();
    static const char enumerator_unfound_string[];
    std::map<std::string, underlying_type> enumerators_map;
    std::map<underlying_type, std::string> enumerators_map_r;

    type_descriptor_for_enum(const char *type_name, size_t type_size, std::map<std::string, int> enum_enumerators_map = {}) : type_descriptor_t(type_name, type_size),
                                                                                                                              enumerators_map(enum_enumerators_map)
    {
        for (auto const &element : enumerators_map)
        {
            enumerators_map_r.emplace(element.second, element.first);
        }
    }

    std::vector<std::string> get_enumerator_names()
    {
        std::vector<std::string> keys;
        for (auto const &element : enumerators_map)
        {
            keys.push_back(element.first);
        }
        return keys;
    }

    underlying_type get_enumerator_value(const std::string &enumerator_name)
    {
        auto p = enumerators_map.find(enumerator_name);
        if (p == enumerators_map.end())
        {
            return enumerator_unfound_value;
        }
        return  static_cast<underlying_type>(p->second);
    }

    std::string get_enumerator_name(underlying_type enumerator_value)
    {
        auto p = enumerators_map_r.find(enumerator_value);
        if (p == enumerators_map_r.end())
        {
            return enumerator_unfound_string;
        }
        return p->second;
    }
};

#define REFLECTABLE_ENUM_ENUMERATORS(enumerator_name) \
    {#enumerator_name, enumerator_name},

#define REFLECTABLE_ENUM_ENUMERATORS_FOR_EACH(r, data, i, x) \
    REFLECTABLE_ENUM_ENUMERATORS(x)

#define REFLECTABLE_ENUM(enum_name, enumerators)                                                                                                               \
    static_assert(std::is_enum<enum_name>::value, #enum_name "is not an enum type.");                                                                          \
    template <>                                                                                                                                                \
    struct type_descriptor_resolver<enum_name>                                                                                                                 \
    {                                                                                                                                                          \
        using T = enum_name;                                                                                                                                   \
        static type_descriptor_for_enum<T> *get()                                                                                                              \
        {                                                                                                                                                      \
            static type_descriptor_for_enum<T> td(                                                                                                             \
                #enum_name,                                                                                                                                    \
                sizeof(T),                                                                                                                                     \
                {BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_ENUM_ENUMERATORS_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(enumerators), enumerators))}); \
            return &td;                                                                                                                                        \
        }                                                                                                                                                      \
    };                                                                                                                                                         \
    ENUM_CONVERTER(enum_name)                                                                                                                                  \
    const char type_descriptor_for_enum<enum_name>::enumerator_unfound_string[] = {"enumerator_unfound"};

} // namespace mirror

#endif