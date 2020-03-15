#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_

#include <any>
#include <tuple>
#include <vector>
#include <string>
#include <functional>

#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

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
	virtual std::tuple<bool, std::any> invoke(std::any function_ptr, std::vector<std::any> args)
	{
		return type_descriptor_for_function_impl::invoke(function_ptr, args);
	}
};

template <typename return_type, typename... args_types>
struct type_descriptor_for_function<return_type (*)(args_types...)> : public type_descriptor_for_function_impl<return_type, args_types...>, public type_descriptor
{
public:
	using type_descriptor::type_descriptor;
	virtual std::tuple<bool, std::any> invoke(std::any function_ptr, std::vector<std::any> args)
	{
		return type_descriptor_for_function_impl::invoke(function_ptr, args);
	};
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_for_function<return_type (instance_type::*)(args_types...)> : public type_descriptor_for_method_impl<instance_type, return_type, args_types...>, public type_descriptor
{
public:
	using type_descriptor::type_descriptor;
	virtual std::tuple<bool, std::any> invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		return type_descriptor_for_method_impl::invoke(instance_ptr, function_ptr, args);
	}
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

	return_type call(std::function<signature> function, args_list args)
	{
		return function_invoke_helper<return_type, signature, args_list, args_size>::call(function, args);
	}
};

template <typename... args_types>
class type_descriptor_for_function_impl<void, args_types...>
{
public:
	using return_type = void;
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
			function_invoke_helper<return_type, signature, args_list, args_size>::invoke(function, args);
			return {true, std::any()};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	return_type call(std::function<signature> function, args_list args)
	{
		return function_invoke_helper<return_type, signature, args_list, args_size>::call(function, args);
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
			auto function_ptr2 = std::any_cast<signature>(function_ptr);
			auto instance_ptr2 = std::any_cast<instance_type *>(instance_ptr);
			return {true, std::any(method_invoke_helper<instance_type, return_type, signature, args_list, args_size>::invoke(instance_ptr2, function_ptr2, args))};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	return_type call(instance_type *instance_ptr, signature function_ptr, args_list args)
	{
		return method_invoke_helper<instance_type, return_type, signature, args_list, args_size>::call(instance_ptr, function_ptr, args);
	}
};

template <typename instance_type, typename... args_types>
class type_descriptor_for_method_impl<instance_type, void, args_types...>
{
public:
	using return_type = void;
	using signature = return_type (instance_type::*)(args_types...);
	using args_list = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_list>::type;

	static constexpr size_t args_size = std::tuple_size_v<args_list>;

	std::tuple<bool, std::any> invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto function_ptr2 = std::any_cast<signature>(function_ptr);
			auto instance_ptr2 = std::any_cast<instance_type *>(instance_ptr);
			method_invoke_helper<instance_type, return_type, signature, args_list, args_size>::invoke(instance_ptr2, function_ptr2, args);
			return {true, std::any()};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	return_type call(instance_type *instance_ptr, signature function_ptr, args_list args)
	{
		return method_invoke_helper<instance_type, return_type, signature, args_list, args_size>::call(instance_ptr, function_ptr, args);
	}
};

#define _ARGS0(idx)                                  \
	BOOST_PP_IF(idx, BOOST_PP_COMMA, BOOST_PP_EMPTY) \
	() std::any_cast<i_args_type<idx>>(args[idx])
#define _ARGS1(idx)                                  \
	BOOST_PP_IF(idx, BOOST_PP_COMMA, BOOST_PP_EMPTY) \
	() std::get<idx>(args)

#define _ARGS_LIST0(n, idx, text) _ARGS0(idx)
#define _ARGS_LIST1(n, idx, text) _ARGS1(idx)

template <typename return_type, typename signature, typename args_list_type, int args>
struct function_invoke_helper;
#define DEFINE_FUNCTION_INVOKE_HELPER(args_size)                                                 \
	template <typename return_type, typename signature, typename args_list_type>                 \
	struct function_invoke_helper<return_type, signature, args_list_type, args_size>             \
	{                                                                                            \
		template <int i>                                                                         \
		using i_args_type = typename std::tuple_element<i, args_list_type>::type;                \
                                                                                                 \
		static return_type invoke(std::function<signature> function, std::vector<std::any> args) \
		{                                                                                        \
			return function(BOOST_PP_REPEAT(args_size, _ARGS_LIST0, text));                      \
		}                                                                                        \
                                                                                                 \
		static return_type call(std::function<signature> function, args_list_type args)          \
		{                                                                                        \
			return function(BOOST_PP_REPEAT(args_size, _ARGS_LIST1, text));                      \
		}                                                                                        \
	};

template <typename instance_type, typename return_type, typename signature, typename args_list_type, int args>
struct method_invoke_helper;

#define DEFINE_METHOD_INVOKE_HELPER(args_size)                                                                                                    \
	template <typename instance_type, typename return_type, typename signature, typename args_list_type>                                          \
	struct method_invoke_helper<instance_type, return_type, signature, args_list_type, args_size>                                                 \
	{                                                                                                                                             \
		template <int i>                                                                                                                          \
		using i_args_type = typename std::tuple_element<i, args_list_type>::type;                                                                 \
                                                                                                                                                  \
		static return_type invoke(instance_type *instance_ptr, signature function_ptr, std::vector<std::any> args)                                \
		{                                                                                                                                         \
			auto function = std::mem_fn(function_ptr);                                                                                            \
			return function(instance_ptr BOOST_PP_IF(args_size, BOOST_PP_COMMA, BOOST_PP_EMPTY)() BOOST_PP_REPEAT(args_size, _ARGS_LIST0, text)); \
		}                                                                                                                                         \
                                                                                                                                                  \
		static return_type call(instance_type *instance_ptr, signature function_ptr, args_list_type args)                                         \
		{                                                                                                                                         \
			auto function = std::mem_fn(function_ptr);                                                                                            \
			return function(instance_ptr BOOST_PP_IF(args_size, BOOST_PP_COMMA, BOOST_PP_EMPTY)() BOOST_PP_REPEAT(args_size, _ARGS_LIST1, text)); \
		}                                                                                                                                         \
	};

#define DEFINE_FUNCTION_INVOK_HELPER_REPEAT(n, idx, text) DEFINE_FUNCTION_INVOKE_HELPER(idx)
BOOST_PP_REPEAT(128, DEFINE_FUNCTION_INVOK_HELPER_REPEAT, text)
#define DEFINE_METHOD_INVOK_HELPER_REPEAT(n, idx, text) DEFINE_METHOD_INVOKE_HELPER(idx)
BOOST_PP_REPEAT(128, DEFINE_METHOD_INVOK_HELPER_REPEAT, text)

#undef _ARGS0
#undef _ARGS1
#undef _ARGS_LIST0
#undef _ARGS_LIST1
#undef DEFINE_FUNCTION_INVOK_HELPER_REPEAT
#undef DEFINE_METHOD_INVOK_HELPER_REPEAT

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