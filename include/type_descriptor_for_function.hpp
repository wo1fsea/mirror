#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_FUNCTION_

#include <any>
#include <tuple>
#include <vector>
#include <string>
#include <functional>
#include <type_traits>

#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <function_traits.hpp>

namespace mirror
{
template <typename T>
class type_descriptor_t;

template <typename raw_signature>
class type_descriptor_for_function_impl;

template <typename raw_signature>
class type_descriptor_for_method_impl;

template <typename signature, typename return_type, typename args_list_type, int args>
struct function_invoke_helper;

template <typename signature, typename instance_type, typename return_type, typename args_list_type, int args>
struct method_invoke_helper;

template <typename raw_signature> 
class type_descriptor_for_function : public type_descriptor_for_function_impl<raw_signature>, public type_descriptor_t<raw_signature>
{
public:
	using type_descriptor_t<raw_signature>::type_descriptor_t;
	virtual std::tuple<bool, std::any> invoke(std::any function_ptr, std::vector<std::any> args)
	{
		return type_descriptor_for_function_impl<raw_signature>::invoke(function_ptr, args);
	}
};

template <typename raw_signature> 
struct type_descriptor_for_method : public type_descriptor_for_method_impl<raw_signature>, public type_descriptor_t<raw_signature>
{
public:
	using type_descriptor_t<raw_signature>::type_descriptor_t;
	virtual std::tuple<bool, std::any> invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		return type_descriptor_for_method_impl<raw_signature>::invoke(instance_ptr, function_ptr, args);
	}
};

template <typename raw_signature>
class type_descriptor_for_function_impl
{
private:
	template <bool has_no_return>
	std::tuple<bool, std::any> _invoke(std::any function_ptr, std::vector<std::any> args);

	template <>
	std::tuple<bool, std::any> _invoke<false>(std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto function = std::any_cast<signature>(function_ptr);
			return {true, std::any(function_invoke_helper<signature, return_type, args_tuple, args_size>::invoke(function, args))};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	template <>
	std::tuple<bool, std::any> _invoke<true>(std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto function = std::any_cast<signature>(function_ptr);
			function_invoke_helper<signature, return_type, args_tuple, args_size>::invoke(function, args);
			return {true, std::any()};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

public:
	using void_signature = typename function_traits<raw_signature>::void_signature;
	using signature = typename function_traits<raw_signature>::signature;

	using return_type = typename function_traits<raw_signature>::return_type;
	using args_tuple = typename function_traits<raw_signature>::args_tuple;
	template <int i>
	using i_args_type = typename function_traits<raw_signature>::template i_args_type<i>;

	static constexpr size_t args_size = function_traits<raw_signature>::args_size;
	static constexpr bool has_no_return = function_traits<raw_signature>::has_no_return;

	std::tuple<bool, std::any> invoke(std::any instance_ptr, void_signature function_ptr, std::vector<std::any> args)
	{
		return _invoke<has_no_return>(instance_ptr, static_cast<signature>(function_ptr), args);
	}
	
	std::tuple<bool, std::any> invoke(std::any function_ptr, std::vector<std::any> args)
	{
		return _invoke<has_no_return>(function_ptr, args);
	}

	return_type call(std::function<signature> function, args_tuple args)
	{
		return function_invoke_helper<signature, return_type, args_tuple, args_size>::call(function, args);
	}
};

template <typename raw_signature>
class type_descriptor_for_method_impl
{
private:
	template <bool has_no_return>
	std::tuple<bool, std::any> _invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args);

	template <>
	std::tuple<bool, std::any> _invoke<false>(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto function_ptr2 = std::any_cast<signature>(function_ptr);
			auto instance_ptr2 = std::any_cast<instance_type *>(instance_ptr);
			return {true, std::any(method_invoke_helper<signature, instance_type, return_type, args_tuple, args_size>::invoke(instance_ptr2, function_ptr2, args))};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

	template <>
	std::tuple<bool, std::any> _invoke<true>(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		try
		{
			auto function_ptr2 = std::any_cast<signature>(function_ptr);
			auto instance_ptr2 = std::any_cast<instance_type *>(instance_ptr);
			method_invoke_helper<signature, instance_type, return_type, args_tuple, args_size>::invoke(instance_ptr2, function_ptr2, args);
			return {true, std::any()};
		}
		catch (const std::bad_any_cast &bace)
		{
			runtime_error_handler(bace);
			return {false, std::any()};
		}
	}

public:
	using void_signature = typename function_traits<raw_signature>::void_signature;
	using signature = typename function_traits<raw_signature>::signature;

	using instance_type = typename function_traits<raw_signature>::instance_type;
	using return_type = typename function_traits<raw_signature>::return_type;
	using args_tuple = typename function_traits<raw_signature>::args_tuple;
	template <int i>
	using i_args_type = typename function_traits<raw_signature>::template i_args_type<i>;

	static constexpr size_t args_size = function_traits<raw_signature>::args_size;
	static constexpr bool has_no_return = function_traits<raw_signature>::has_no_return;


	std::tuple<bool, std::any> invoke(std::any instance_ptr, void_signature function_ptr, std::vector<std::any> args)
	{
		return _invoke<has_no_return>(instance_ptr, static_cast<signature>(function_ptr), args);
	}
	
	std::tuple<bool, std::any> invoke(std::any instance_ptr, std::any function_ptr, std::vector<std::any> args)
	{
		return _invoke<has_no_return>(instance_ptr, function_ptr, args);
	}

	return_type call(instance_type *instance_ptr, signature function_ptr, args_tuple args)
	{
		return method_invoke_helper<signature, instance_type, return_type, args_tuple, args_size>::call(instance_ptr, function_ptr, args);
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

#define DEFINE_FUNCTION_INVOKE_HELPER(args_size)                                     \
	template <typename signature, typename return_type, typename args_list_type>     \
	struct function_invoke_helper<signature, return_type, args_list_type, args_size> \
	{                                                                                \
		template <int i>                                                             \
		using i_args_type = typename std::tuple_element<i, args_list_type>::type;    \
                                                                                     \
		static return_type invoke(signature function, std::vector<std::any> args)    \
		{                                                                            \
			return function(BOOST_PP_REPEAT(args_size, _ARGS_LIST0, text));          \
		}                                                                            \
                                                                                     \
		static return_type call(signature function, args_list_type args)             \
		{                                                                            \
			return function(BOOST_PP_REPEAT(args_size, _ARGS_LIST1, text));          \
		}                                                                            \
	};

#define DEFINE_METHOD_INVOKE_HELPER(args_size)                                                                                                    \
	template <typename signature, typename instance_type, typename return_type, typename args_list_type>                                          \
	struct method_invoke_helper<signature, instance_type, return_type, args_list_type, args_size>                                                 \
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
	using signature = return_type (*)(args_types...);
	static type_descriptor_for_function<return_type(args_types...)> *get()
	{
		static type_descriptor_for_function<return_type(args_types...)> td("function");
		return &td;
	}
};

template <typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type(args_types...) noexcept>
{
	using signature = return_type (*)(args_types...) noexcept;
	static type_descriptor_for_function<return_type(args_types...) noexcept> *get()
	{
		static type_descriptor_for_function<return_type(args_types...) noexcept> td("function");
		return &td;
	}
};

template <typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (*)(args_types...)>
{
	using signature = return_type (*)(args_types...);
	static type_descriptor_for_function<return_type(*)(args_types...)>*get()
	{
		static type_descriptor_for_function<return_type(*)(args_types...)> td("function_ptr");
		return &td;
	}
};

template <typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (*)(args_types...) noexcept>
{
	using signature = return_type (*)(args_types...) noexcept;
	static type_descriptor_for_function<return_type(*)(args_types...) noexcept>*get()
	{
		static type_descriptor_for_function<return_type(*)(args_types...) noexcept> td("function_ptr");
		return &td;
	}
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (instance_type::*)(args_types...)>
{
	using signature = return_type (instance_type::*)(args_types...);
	static type_descriptor_for_method<return_type(instance_type::*)(args_types...)>*get()
	{
		static type_descriptor_for_method<return_type(instance_type::*)(args_types...)> td("class_method_ptr");
		return &td;
	}
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (instance_type::*)(args_types...) noexcept>
{
	using signature = return_type (instance_type::*)(args_types...) noexcept;
	static type_descriptor_for_method<return_type(instance_type::*)(args_types...) noexcept>*get()
	{
		static type_descriptor_for_method<return_type(instance_type::*)(args_types...) noexcept> td("class_method_ptr");
		return &td;
	}
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (instance_type::*)(args_types...) const>
{
	using signature = return_type (instance_type::*)(args_types...) const;
	static type_descriptor_for_method<return_type(instance_type::*)(args_types...) const>*get()
	{
		static type_descriptor_for_method<return_type(instance_type::*)(args_types...) const> td("class_method_ptr");
		return &td;
	}
};

template <typename instance_type, typename return_type, typename... args_types>
struct type_descriptor_resolver<return_type (instance_type::*)(args_types...) const noexcept>
{
	using signature = return_type (instance_type::*)(args_types...) const noexcept;
	static type_descriptor_for_method<return_type(instance_type::*)(args_types...) const noexcept>*get()
	{
		static type_descriptor_for_method<return_type(instance_type::*)(args_types...) const noexcept> td("class_method_ptr");
		return &td;
	}
};

// const noexcept

} // namespace mirror

#endif