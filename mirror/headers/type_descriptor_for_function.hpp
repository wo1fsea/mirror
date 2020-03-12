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
class type_descriptor;

template <typename signature>
class type_descriptor_for_function : public type_descriptor
{
};

template <typename return_type, typename... args_types>
class type_descriptor_for_function_impl;

template <typename instance_type, typename return_type, typename... args_types>
class type_descriptor_for_method_impl;

template <typename return_type, typename... args_types>
class type_descriptor_for_function<return_type(args_types...)> : public type_descriptor_for_function_impl<return_type, args_types...>, public type_descriptor
{
public:
	using type_descriptor::type_descriptor;
};

template <typename return_type, typename... args_types>
struct type_descriptor_for_function<return_type (*)(args_types...)> : public type_descriptor_for_function_impl<return_type, args_types...>, public type_descriptor
{
public:
	using type_descriptor::type_descriptor;
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_for_function<return_type (instance_type::*)(args_types...)> : public type_descriptor_for_method_impl<instance_type, return_type, args_types...>, public type_descriptor
{
public:
	using type_descriptor::type_descriptor;
};

template <typename return_type, typename... args_types>
class type_descriptor_for_function_impl
{
public:
	using signature = return_type(args_types...);
	using args_list = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_list>::type;

	static constexpr size_t args_size = std::tuple_size_v<args_list>;

	std::tuple<bool, std::any> invoke(std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			std::function<signature> function = std::any_cast<signature *>(function_ptr);
			return {true, std::any(function_invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args))};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	return_type invoke(std::function<signature> function, args_list args)
	{
		return function_invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
	}
};

template <typename instance_type, typename return_type, typename... args_types>
class type_descriptor_for_method_impl
{
public:
	using signature = return_type (instance_type::*)(args_types...);
	using args_list = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_list>::type;

	static constexpr size_t args_size = std::tuple_size_v<args_list>;

	std::tuple<bool, std::any> invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto fptr = std::any_cast<signature>(function_ptr);
			auto function = std::mem_fn(fptr);
			return {true, std::any(function(std::any_cast<instance_type *>(instance_ptr), std::any_cast<i_args_type<0>>(args[0]),std::any_cast<i_args_type<1>>(args[1]),std::any_cast<i_args_type<2>>(args[2])))};
			//return { false, std::any() };
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	return_type invoke(std::function<signature> function, args_list args)
	{
		throw true;
		//return function_invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
	}
};

template <typename return_type, typename signature, typename args_list_type, int args>
struct function_invoke_helper;

#define DEFINE_FUNCTION_INVOKE_HELPER(args_size, args_list_0, args_list1)                        \
	template <typename return_type, typename signature, typename args_list_type>                 \
	struct function_invoke_helper<return_type, signature, args_list_type, args_size>             \
	{                                                                                            \
		template <int i>                                                                         \
		using i_args_type = typename std::tuple_element<i, args_list_type>::type;                \
                                                                                                 \
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

template <typename instance_type, typename return_type, typename signature, typename args_list_type, int args>
struct invoke_helper;

#define DEFINE_METHOD_INVOKE_HELPER(args_size, args_list_0, args_list1)                                                \
	template <typename instance_type, typename return_type, typename signature, typename args_list_type>               \
	struct method_invoke_helper<instance_type, return_type, signature, args_list_type, args_size>                            \
	{                                                                                                                  \
		template <int i>                                                                                               \
		using i_args_type = typename std::tuple_element<i, args_list_type>::type;                                      \
                                                                                                                       \
		static return_type invoke(std::function<signature> function, std::vector<std::any> args)                       \
		{                                                                                                              \
			return function(BOOST_PP_TUPLE_REM(args_size) args_list_0);                                                \
		}                                                                                                              \
                                                                                                                       \
		static return_type invoke(std::function<signature> function, args_list_type args)                              \
		{                                                                                                              \
			return function(BOOST_PP_TUPLE_REM(args_size) args_list1);                                                 \
		}                                                                                                              \
                                                                                                                       \
		static return_type invoke(instance_type instance, std::mem_fn<signature> function, std::vector<std::any> args) \
		{                                                                                                              \
			return function(instance, BOOST_PP_TUPLE_REM(args_size) args_list_0);                                      \
		}                                                                                                              \
                                                                                                                       \
		static return_type invoke(instance_type instance, std::mem_fn<signature> function, args_list_type args)        \
		{                                                                                                              \
			return function(instance, BOOST_PP_TUPLE_REM(args_size) args_list1);                                       \
		}                                                                                                              \
	};

#define _ARGS0(idx) std::any_cast<i_args_type<idx>>(args[idx])
#define _ARGS1(idx) std::get<idx>(args)

DEFINE_FUNCTION_INVOKE_HELPER(1, (_ARGS0(0)), (_ARGS1(0)))
DEFINE_FUNCTION_INVOKE_HELPER(2, (_ARGS0(0), _ARGS0(1)), (_ARGS1(0), _ARGS1(1)))
DEFINE_FUNCTION_INVOKE_HELPER(3, (_ARGS0(0), _ARGS0(1), _ARGS0(2)), (_ARGS1(0), _ARGS1(1), _ARGS1(2)))
DEFINE_FUNCTION_INVOKE_HELPER(4, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3)))
DEFINE_FUNCTION_INVOKE_HELPER(5, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4)))
DEFINE_FUNCTION_INVOKE_HELPER(6, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5)))
DEFINE_FUNCTION_INVOKE_HELPER(7, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5), _ARGS0(6)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5), _ARGS1(6)))
DEFINE_FUNCTION_INVOKE_HELPER(8, (_ARGS0(0), _ARGS0(1), _ARGS0(2), _ARGS0(3), _ARGS0(4), _ARGS0(5), _ARGS0(6), _ARGS0(7)), (_ARGS1(0), _ARGS1(1), _ARGS1(2), _ARGS1(3), _ARGS1(4), _ARGS1(5), _ARGS1(6), _ARGS1(7)))

#undef _ARGS0
#undef _ARGS1

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
struct type_descriptor_resolver<return_type(args_types...)>
{
	using T = return_type(args_types...);
	static type_descriptor_for_function<T> *get()
	{
		static type_descriptor_for_function<T> td("function", 0);
		return &td;
	}
};

template <typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (*)(args_types...)>
{
	using T = return_type (*)(args_types...);
	static type_descriptor_for_function<T> *get()
	{
		static type_descriptor_for_function<T> td("function_ptr", sizeof(&a));
		return &td;
	}
};

template <typename class_name, typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (class_name::*)(args_types...)>
{
	using T = return_type (class_name::*)(args_types...);
	static type_descriptor_for_function<T> *get()
	{
		static type_descriptor_for_function<T> td("class_method_ptr", sizeof(&a));
		return &td;
	}
};

// const noexcept

} // namespace mirror

#endif