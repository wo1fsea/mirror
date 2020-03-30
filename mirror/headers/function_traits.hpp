#pragma once
#ifndef _MIRROR_FUNCTION_TRAITS_
#define _MIRROR_FUNCTION_TRAITS_

#include <type_traits>
#include <tuple>

namespace mirror
{
template <typename _raw_signature>
struct function_traits
{
	using instance_type = void*;
	using return_type = void;
	using args_tuple = std::tuple<>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using raw_signature = _raw_signature;
	using void_signature = void*;
	using signature = void*;

	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = false;

	static constexpr size_t args_size = 0;
	static constexpr bool has_no_return = false;
};

// function
template <typename _return_type, typename... args_types>
struct function_traits<_return_type(args_types...)>
{
	using instance_type = void*;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void*;
	using raw_signature = return_type(args_types...);
	using signature = return_type(*)(args_types...);

	static constexpr bool is_func = true;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = false;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _return_type, typename... args_types>
struct function_traits<_return_type(args_types...) noexcept>
{
	using instance_type = void*;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void*;
	using raw_signature = return_type(args_types...) noexcept;
	using signature = return_type(*)(args_types...)noexcept;

	static constexpr bool is_func = true;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = false;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _return_type, typename... args_types>
struct function_traits<_return_type (*)(args_types...)>
{
	using instance_type = void*;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void *;
	using raw_signature = return_type (*)(args_types...);
	using signature = return_type (*)(args_types...);

	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = true;
	static constexpr bool is_mem_func_ptr = false;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _return_type, typename... args_types>
struct function_traits<_return_type (*)(args_types...) noexcept>
{
	using instance_type = void *;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void *;
	using raw_signature = return_type (*)(args_types...) noexcept;
	using signature = return_type (*)(args_types...) noexcept;


	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = true;
	static constexpr bool is_mem_func_ptr = false;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _instance_type, typename _return_type, typename... args_types>
struct function_traits<_return_type (_instance_type::*)(args_types...)>
{
	using instance_type = _instance_type;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void (instance_type::*)(void);
	using raw_signature = return_type(instance_type::*)(args_types...);
	using signature = return_type(instance_type::*)(args_types...);

	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = true;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _instance_type, typename _return_type, typename... args_types>
struct function_traits<_return_type (_instance_type::*)(args_types...) noexcept>
{
	using instance_type = _instance_type;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void (instance_type::*)(void);
	using raw_signature = return_type (instance_type::*)(args_types...) noexcept;
	using signature = return_type (instance_type::*)(args_types...)noexcept;


	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = true;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _instance_type, typename _return_type, typename... args_types>
struct function_traits<_return_type (_instance_type::*)(args_types...) const>
{
	using instance_type = _instance_type;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void (instance_type::*)(void);
	using raw_signature = return_type (instance_type::*)(args_types...) const;
	using signature = return_type (instance_type::*)(args_types...)const;


	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = true;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

template <typename _instance_type, typename _return_type, typename... args_types>
struct function_traits<_return_type (_instance_type::*)(args_types...) const noexcept>
{
	using instance_type = _instance_type;
	using return_type = _return_type;
	using args_tuple = std::tuple<args_types...>;
	template <int i>
	using i_args_type = typename std::tuple_element<i, args_tuple>::type;

	using void_signature = void (instance_type::*)(void);
	using raw_signature = return_type (instance_type::*)(args_types...) const noexcept;
	using signature = return_type (instance_type::*)(args_types...)const noexcept;


	static constexpr bool is_func = false;
	static constexpr bool is_func_ptr = false;
	static constexpr bool is_mem_func_ptr = true;

	static constexpr size_t args_size = std::tuple_size_v<args_tuple>;
	static constexpr bool has_no_return = std::is_void<return_type>::value;
};

} // namespace mirror

#endif
