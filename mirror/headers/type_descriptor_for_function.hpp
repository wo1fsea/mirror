#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_

#include <any>
#include <tuple>
#include <vector>
#include <string>
#include <functional>

#include <boost/preprocessor/tuple/rem.hpp>

namespace mirror
{

template <typename signature>
class type_descriptor_for_function;

template <typename return_type, typename... args_types>
class type_descriptor_for_function<return_type(args_types...)> : public type_descriptor
{
public:
    using type_descriptor::type_descriptor;

    using signature = return_type(args_types...);
    using args_list = std::tuple<args_types...>;
    template <int i>
    using i_args_type = typename std::tuple_element<i, args_list>::type;

    static constexpr size_t args_size = std::tuple_size_v<args_list>;

    template <int i>
    return_type invoke() {}

    std::tuple<bool, std::any> invoke(std::function<signature> function, std::vector<std::any> args)
    {
        try
        {
            return {true, std::any(invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args))};
        }
        catch (const std::bad_any_cast &bace)
        {
            runtime_error_handler(bace);
            return {false, std::any()};
        }
    }

    return_type invoke(std::function<signature> function, args_list args)
    {
        return invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
    }
    virtual bool set_value(void *ptr, std::any value) { return false; }
    virtual std::any get_value(void *ptr) { return std::any(); }

    virtual bool set_value_from_string(void *ptr, std::string string_value) { return false; }
    virtual std::string get_value_to_string(void *ptr) { return ""; }
};

template <typename return_type, typename signature, typename args_list_type, int args>
struct invoke_helper;

#define DEFINE_INVOKE_HELPER(args_size, args_list_0, args_list1)                                 \
    template <typename return_type, typename signature, typename args_list_type>                 \
    struct invoke_helper<return_type, signature, args_list_type, args_size>                      \
    {                                                                                            \
        template <int i>                                                                         \
        using i_args_type = typename std::tuple_element<i, args_list_type>::type;                \
        static return_type invoke(std::function<signature> function, std::vector<std::any> args) \
        {                                                                                        \
            return function(BOOST_PP_TUPLE_REM(args_size) args_list_0);                          \
        }                                                                                        \
                                                                                                 \
        static return_type invoke(std::function<signature> function, args_list_type args)        \
        {                                                                                        \
            return function(BOOST_PP_TUPLE_REM(args_size) args_list1);                           \
        }                                                                                        \
    };

#define _ARGS0(idx) std::any_cast<i_args_type<idx>>(args[idx])
#define _ARGS1(idx) std::get<idx>(args)

DEFINE_INVOKE_HELPER(1, (_ARGS0(0)), (_ARGS1(0)))
DEFINE_INVOKE_HELPER(2, (_ARGS0(0), _ARGS0(1)), (_ARGS1(0), _ARGS1(1)))
DEFINE_INVOKE_HELPER(3, (_ARGS0(0), _ARGS0(1), _ARGS0(2)), (_ARGS1(0), _ARGS1(1), _ARGS1(2)))
DEFINE_INVOKE_HELPER(4, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3)))
DEFINE_INVOKE_HELPER(5, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4)))
DEFINE_INVOKE_HELPER(6, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5)))
DEFINE_INVOKE_HELPER(7, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5), _ARGS0(6)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5), _ARGS1(6)))
DEFINE_INVOKE_HELPER(8, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5), _ARGS0(6), _ARGS0(7)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5), _ARGS1(6), _ARGS1(7)))

/*
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/array/push_back.hpp>
#include <boost/preprocessor/array/size.hpp>

#define MAEK_INVOKER_HELPER_ARGS0(idx) std::any_cast<i_args_type<idx>>(args[idx])
#define MAEK_INVOKER_HELPER_ARGS1(idx) std::get<idx>(args)

#define INVOKER_HELPER_ARGS_ARRAY0 (0, ())
#define INVOKER_HELPER_ARGS_ARRAY1 (0, ())

DEFINE_INVOKE_HELPER(
    3,
    BOOST_PP_ARRAY_DATA(
        BOOST_PP_ARRAY_PUSH_BACK(
            BOOST_PP_ARRAY_PUSH_BACK(
                BOOST_PP_ARRAY_PUSH_BACK(
                    INVOKER_HELPER_ARGS_ARRAY0,
                    MAEK_INVOKER_HELPER_ARGS0(0)),
                MAEK_INVOKER_HELPER_ARGS0(1)),
            MAEK_INVOKER_HELPER_ARGS0(2))),
    (std::get<0>(args), std::get<1>(args), std::get<2>(args)))

*/
template <typename signature>
struct type_descriptor_resolver;

template <typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type(args_types...)> : public type_descriptor
{
    using T = return_type(args_types...);
    static type_descriptor_for_function<T> *get()
    {
        static type_descriptor_for_function<T> td("function", sizeof(&a));
        return &td;
    }
};

} // namespace mirror

#endif