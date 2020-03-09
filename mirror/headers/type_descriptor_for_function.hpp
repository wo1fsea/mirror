#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_

#include <any>
#include <tuple>
#include <vector>
#include <string>
#include <functional>

#include <boost/preprocessor/repetition/repeat.hpp>



namespace mirror
{
template<typename signature>
class type_descriptor_for_function;

template<typename return_type, typename...args_types>
class type_descriptor_for_function<return_type(args_types...)>: public type_descriptor
{
public:
    using type_descriptor::type_descriptor;

    using signature = return_type(args_types...);
    using args_list = std::tuple<args_types...>;
    template <int i>
    using i_args_type = typename std::tuple_element<i, args_list>::type;

	static constexpr size_t args_size = std::tuple_size_v<args_list>;
    
    template<int i>
    return_type invoke(){}

    return_type invoke (std::function<signature> function, std::vector<std::any> args)
    {
        return invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
    }

    return_type invoke(std::function<signature> function, args_list args)
    {
        return invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
    }
    virtual bool set_value(void *ptr, std::any value){return false;}
    virtual std::any get_value(void *ptr){return std::any();}

    virtual bool set_value_from_string(void *ptr, std::string string_value){return false;}
    virtual std::string get_value_to_string(void *ptr){return "";}
};

template<typename return_type, typename signature, typename args_list_type, int args>
struct invoke_helper;

template<typename return_type, typename signature, typename args_list_type>
struct invoke_helper<return_type, signature, args_list_type, 3>
{
    template <int i>
    using i_args_type = typename std::tuple_element<i, args_list_type>::type;
    static return_type invoke (std::function<signature> function, std::vector<std::any> args)
    {
        return function(std::any_cast<i_args_type<0>>(args[0]), std::any_cast<i_args_type<1>>(args[1]), std::any_cast<i_args_type<2>>(args[2]));
    }
    
    static return_type invoke(std::function<signature> function, args_list_type args)
    {
        return function(std::get<0>(args), std::get<1>(args), std::get<2>(args));
    }
};

} // namespace mirror

#endif