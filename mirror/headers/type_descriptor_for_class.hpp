#pragma once
#ifndef _MIRROR_TYPE_DESCRIPTOR_FOR_CLASS_
#define _MIRROR_TYPE_DESCRIPTOR_FOR_CLASS_

#include <any>
#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <memory>
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
		const char* name;
		size_t offset;
		type_descriptor* type_descriptor_ptr;

		member_descriptor(const char* _name, size_t _offset, type_descriptor* _type_descriptor_ptr) : name(_name), offset(_offset), type_descriptor_ptr(_type_descriptor_ptr) {}
	};

	template <typename _parent_type>
	struct member_descriptor_of : public member_descriptor
	{
	public:
		using member_descriptor::member_descriptor;

		static type_descriptor* get_parent_type_descriptor_ptr()
		{
			return type_descriptor_resolver<_parent_type>::get();
		}
	};

	class method_descriptor
	{
	public:
		const char* name;
		type_descriptor * type_descriptor_ptr;
		method_descriptor(const char* _name, type_descriptor* _type_descriptor_ptr) : name(_name), type_descriptor_ptr(_type_descriptor_ptr) {}

		virtual std::tuple<bool, std::any> invoke(std::any instance_ptr, std::vector<std::any> args)
		{
			return {false, std::any()};
		}

	};

	template <typename _parent_type>
	class method_descriptor_of : public method_descriptor
	{
	public:
		using void_signature = typename void(_parent_type::*)(void);

		void_signature method_ptr;

		method_descriptor_of(const char* _name, void_signature _method_ptr, type_descriptor* _type_descriptor_ptr) : method_ptr(_method_ptr), method_descriptor(_name, _type_descriptor_ptr) {}

		static type_descriptor* get_parent_type_descriptor_ptr()
		{
			return type_descriptor_resolver<_parent_type>::get();
		}
	};

	template <typename _parent_type, typename signature>
	class method_descriptor_with_signature_of : public method_descriptor_of<_parent_type>
	{
	public:
		using method_descriptor_of::method_descriptor_of;
		virtual std::tuple<bool, std::any> invoke(std::any instance_ptr, std::vector<std::any> args)
		{
			auto method_ptr_t = (signature)(method_ptr);
			if (type_descriptor_ptr)
			{
				return type_descriptor_ptr->invoke(instance_ptr, method_ptr_t, args);
			}
			else 
			{
				return {false, std::any()};
			}
		}
	};
	
	template <typename T>
	class type_descriptor_for_class : public type_descriptor_t<T>
	{
	public:
		using member_descriptors = std::vector<std::shared_ptr<member_descriptor_of<T>>>;
		using method_descriptors = std::vector<std::shared_ptr<method_descriptor_of<T>>>;

		using member_descriptors_map = std::map<std::string, std::shared_ptr<member_descriptor_of<T>>>;
		using method_descriptors_map = std::map<std::string, std::shared_ptr<method_descriptor_of<T>>>;

		member_descriptors members;
		member_descriptors_map members_map;

		method_descriptors methods;
		method_descriptors_map methods_map;
		
		type_descriptor_for_class(const char* type_name, member_descriptors type_members = {}, method_descriptors type_methods = {}) : type_descriptor_t(type_name),
			members(type_members), methods(type_methods)
		{
			for (auto member : members)
			{
				members_map.emplace(member->name, member);
			}
			for (auto method : methods)
			{
				methods_map.emplace(method->name, method);
			}
		}
				
		member_descriptor_of<T>* get_member_descriptor(std::string member_name)
		{
			auto it = members_map.find(member_name);
			return it == members_map.end() ? nullptr : it->second.get();
		}

		method_descriptor_of<T>* get_method_descriptor(std::string method_name)
		{
			auto it = methods_map.find(method_name);
			return it == methods_map.end() ? nullptr : it->second.get();
		}

		bool set_property(void* ptr, std::string member_name, std::any value)
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

			return md->type_descriptor_ptr->set_value((char*)ptr + md->offset, value);
		}

		std::any get_property(void* ptr, std::string member_name)
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

			return md->type_descriptor_ptr->get_value((char*)ptr + md->offset);
		}

		bool set_property_from_string(void* ptr, std::string member_name, std::string string_value)
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

			return md->type_descriptor_ptr->set_value_from_string((char*)ptr + md->offset, string_value);
		}

		std::string get_property_to_string(void* ptr, std::string member_name)
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

			return md->type_descriptor_ptr->get_value_to_string((char*)ptr + md->offset);
		}

		std::tuple<bool,std::any> invoke_method(void *instance_ptr, std::string method_name, std::vector<std::any> args)
		{
			 if (!instance_ptr)
			 {
			 	return {false, std::any()};
			 }

			 auto md = this->get_method_descriptor(method_name);
			 if (!md || !md->type_descriptor_ptr)
			 {
			 	return {false, std::any()};
			 }
			 auto ptr_t = static_cast<T*>(instance_ptr);
			 return md->invoke(ptr_t, args);
		}
	};

} // namespace mirror


template <typename T>
struct mirror::type_descriptor_resolver;

#define REFLECTABLE_MEMBER(member_name) \
    std::make_shared<mirror::member_descriptor_of<T>>(     \
        #member_name,                   \
        offsetof(T, member_name),       \
        mirror::type_descriptor_resolver<decltype(T::member_name)>::get()),

#define REFLECTABLE_MEMBER_FOR_EACH(r, data, i, x) \
    REFLECTABLE_MEMBER(x)

#define REFLECTABLE_METHOD(method_name) \
     std::make_shared<mirror::method_descriptor_with_signature_of<T, decltype(&T::method_name)>>(     \
        #method_name,                   \
		(mirror::method_descriptor_of<T>::void_signature)&T::method_name,				\
        mirror::type_descriptor_resolver<decltype(&T::method_name)>::get()),

#define REFLECTABLE_METHOD_FOR_EACH(r, data, i, x) \
    REFLECTABLE_METHOD(x)

#define REFLECTABLE_CLASS(class_name, members)                                                                                               \
    static_assert(std::is_default_constructible<class_name>::value, #class_name "is not default constructible");                             \
    template <>                                                                                                                              \
    struct mirror::type_descriptor_resolver<class_name>                                                                                      \
    {                                                                                                                                        \
        using T = class_name;                                                                                                                \
        static mirror::type_descriptor_for_class<T> *get()                                                                                   \
        {                                                                                                                                    \
            static type_descriptor_for_class<T> td(                                                                                          \
                #class_name,                                                                                                                 \
                {BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_MEMBER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(members), members))}); \
            return &td;                                                                                                                      \
        }                                                                                                                                    \
    };

#define REFLECTABLE_CLASS_WITH_METHOD(class_name, members, methods)																		     \
    static_assert(std::is_default_constructible<class_name>::value, #class_name "is not default constructible");                             \
    template <>                                                                                                                              \
    struct mirror::type_descriptor_resolver<class_name>                                                                                      \
    {                                                                                                                                        \
        using T = class_name;                                                                                                                \
        static mirror::type_descriptor_for_class<T> *get()                                                                                   \
        {                                                                                                                                    \
            static type_descriptor_for_class<T> td(                                                                                          \
                #class_name,                                                                                                                 \
                {BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_MEMBER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(members), members))},  \
                {BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_METHOD_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(methods), methods))}   \
            );																																 \
            return &td;                                                                                                                      \
        }                                                                                                                                    \
    };



#endif