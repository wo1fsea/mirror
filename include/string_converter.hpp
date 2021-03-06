#pragma once
#ifndef _MIRROR_STRING_CONVERTOR_
#define _MIRROR_STRING_CONVERTOR_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "error_handler.hpp"

namespace mirror
{
template <typename T>
struct type_descriptor_resolver;
template <typename T>
class type_descriptor_for_class;

// primitive type 2 string convert func
// template <typename T>
// struct string_converter
// {
//     static bool set_from_string(void *ptr, std::string &string_value)
//     {
//         return false;
//     }

//     static T from_string(std::string &string_value)
//     {
//         auto td = type_descriptor_resolver<T>::get();
//         auto value = T();
//         return value;
//     }

//     static std::string to_string(T value)
//     {
//         return "";
//     }
// };

template <typename T>
struct string_converter
{
    static bool set_from_string(void *ptr, std::string &string_value)
    {
        auto td = dynamic_cast<type_descriptor_for_class<T> *>(type_descriptor_resolver<T>::get());
        bool set = false;

        if (td)
        {
            try
            {
                std::stringstream sstream(string_value);
                boost::property_tree::ptree pt;
                read_json(sstream, pt);

                for (auto it : td->members)
                {
                    boost::optional<std::string> member_string_value = pt.get_optional<std::string>(it->name);
                    if (member_string_value.has_value())
                    {
                        td->set_property_from_string(ptr, it->name, member_string_value.get());
                        set = true;
                    }
                }
            }
            catch (boost::property_tree::ptree_error pte)
            {
                runtime_error_handler(pte);
            }
        }
        return set;
    }

    static T from_string(std::string &string_value)
    {
        auto td = dynamic_cast<type_descriptor_for_class<T> *>(type_descriptor_resolver<T>::get());
        auto value = T();
        if (td)
        {
            try
            {
                std::stringstream sstream(string_value);
                boost::property_tree::ptree pt;
                read_json(sstream, pt);

                for (auto it : td->members)
                {
                    boost::optional<std::string> member_string_value = pt.get_optional<std::string>(it.name);
                    if (member_string_value.has_value())
                    {
                        td->set_property_from_string(&value, it.name, member_string_value.get());
                    }
                }
            }
            catch (boost::property_tree::ptree_error pte)
            {
                runtime_error_handler(pte);
            }
        }
        return value;
    }

    static std::string to_string(T value)
    {
        auto td = dynamic_cast<type_descriptor_for_class<T> *>(type_descriptor_resolver<T>::get());
        if (td)
        {
            try
            {
                boost::property_tree::ptree pt;
                for (auto it : td->members)
                {
                    auto member_string_value = td->get_property_to_string(&value, it->name);
                    pt.put(it->name, member_string_value);
                }
                std::stringstream sstream;
                write_json(sstream, pt);
                return sstream.str();
            }
            catch (boost::property_tree::ptree_error pte)
            {
                runtime_error_handler(pte);
            }
        }

        return "";
    }
};

#define NUMBER_STRING_CONVERTER(_typename, convert_func1, convert_func2)      \
    template <>                                                               \
    struct string_converter<_typename>                                        \
    {                                                                         \
        static bool set_from_string(void *ptr, std::string &string_value)     \
        {                                                                     \
            _typename *ptr_t = static_cast<_typename *>(ptr);                 \
            _typename value = 0;                                              \
            try                                                               \
            {                                                                 \
                *ptr_t = static_cast<_typename>(convert_func1(string_value)); \
                return true;                                                  \
            }                                                                 \
            catch (std::invalid_argument iae)                                 \
            {                                                                 \
                runtime_error_handler(iae);                                   \
            }                                                                 \
            catch (std::out_of_range oore)                                    \
            {                                                                 \
                runtime_error_handler(oore);                                  \
            }                                                                 \
            return false;                                                     \
        }                                                                     \
                                                                              \
        static _typename from_string(std::string &string_value)               \
        {                                                                     \
            try                                                               \
            {                                                                 \
                return static_cast<_typename>(convert_func1(string_value));   \
            }                                                                 \
            catch (std::invalid_argument iae)                                 \
            {                                                                 \
                runtime_error_handler(iae);                                   \
            }                                                                 \
            catch (std::out_of_range oore)                                    \
            {                                                                 \
                runtime_error_handler(oore);                                  \
            }                                                                 \
            return 0;                                                         \
        }                                                                     \
                                                                              \
        static std::string to_string(_typename &value)                        \
        {                                                                     \
            return convert_func2(value);                                      \
        }                                                                     \
    };

NUMBER_STRING_CONVERTER(bool, std::stoi, std::to_string)

NUMBER_STRING_CONVERTER(char, std::stoi, std::to_string)
NUMBER_STRING_CONVERTER(unsigned char, std::stoul, std::to_string)

NUMBER_STRING_CONVERTER(short, std::stoi, std::to_string)
NUMBER_STRING_CONVERTER(unsigned short, std::stoul, std::to_string)

NUMBER_STRING_CONVERTER(int, std::stoi, std::to_string)
NUMBER_STRING_CONVERTER(unsigned int, std::stoul, std::to_string)

NUMBER_STRING_CONVERTER(long, std::stol, std::to_string)
NUMBER_STRING_CONVERTER(unsigned long, std::stoul, std::to_string)

NUMBER_STRING_CONVERTER(long long, std::stol, std::to_string)
NUMBER_STRING_CONVERTER(unsigned long long, std::stoull, std::to_string)

NUMBER_STRING_CONVERTER(wchar_t, std::stoi, std::to_string)

NUMBER_STRING_CONVERTER(float, std::stof, std::to_string)

NUMBER_STRING_CONVERTER(double, std::stod, std::to_string)

NUMBER_STRING_CONVERTER(long double, std::stold, std::to_string)

template <>
struct string_converter<std::string>
{
    static bool set_from_string(void *ptr, std::string &string_value)
    {
        std::string *ptr_t = static_cast<std::string *>(ptr);
        *ptr_t = string_value;
        return true;
    }

    static std::string from_string(std::string &string_value)
    {
        return string_value;
    }

    static std::string to_string(std::string &value)
    {
        return value;
    }
};

} // namespace mirror

#define ENUM_CONVERTER(_typename)                                                                                                      \
    template <>                                                                                                                        \
    struct mirror::string_converter<_typename>                                                                                         \
    {                                                                                                                                  \
        static bool set_from_string(void *ptr, std::string &string_value)                                                              \
        {                                                                                                                              \
            _typename *ptr_t = static_cast<_typename *>(ptr);                                                                          \
            auto td = dynamic_cast<mirror::type_descriptor_for_enum<_typename> *>(mirror::type_descriptor_resolver<_typename>::get()); \
            if (td)                                                                                                                    \
            {                                                                                                                          \
                *ptr_t = static_cast<_typename>(td->get_enumerator_value(string_value));                                               \
                return true;                                                                                                           \
            }                                                                                                                          \
            return false;                                                                                                              \
        }                                                                                                                              \
                                                                                                                                       \
        static _typename from_string(std::string &string_value)                                                                        \
        {                                                                                                                              \
            auto td = dynamic_cast<mirror::type_descriptor_for_enum<_typename> *>(mirror::type_descriptor_resolver<_typename>::get()); \
            if (td)                                                                                                                    \
            {                                                                                                                          \
                return static_cast<_typename>(td->get_enumerator_value(string_value));                                                 \
            }                                                                                                                          \
            return static_cast<_typename>(td->enumerator_unfound_value);                                                               \
        }                                                                                                                              \
                                                                                                                                       \
        static std::string to_string(_typename &value)                                                                                 \
        {                                                                                                                              \
            auto td = dynamic_cast<type_descriptor_for_enum<_typename> *>(mirror::type_descriptor_resolver<_typename>::get());         \
            if (td)                                                                                                                    \
            {                                                                                                                          \
                return td->get_enumerator_name((int)value);                                                                            \
            }                                                                                                                          \
            return td->enumerator_unfound_string;                                                                                      \
        }                                                                                                                              \
    };

#endif