#include <iostream>
#include <string>
#include <functional>

#include "mirror.h"

enum enum_t
{
	enum_0,
	enum_1,
};

class test_sub_class
{
public:
	enum_t enum_member = enum_t::enum_0;
	bool bool_member = false;
	char char_member = 'a';
	int int_member = 1;
	float float_member = 1.1f;
	double double_member = 1.1;
	std::string string_member = "string";
};

class test_class
{
public:
	bool bool_member = false;

	char char_member = 'a';

	int int_member = 1;

	float float_member = 1.1f;
	double double_member = 1.1;
	std::string string_member = "string";

	test_sub_class stc;

	// test_class(){}
	int a(int a, int b, int c) { 
		return a + b + c;
	}
	static int b(int a, int b, int c) { return a + b + c; }
};

REFLECTABLE_ENUM(
	enum_t,
	(
		enum_t::enum_0,
		enum_t::enum_1))

	REFLECTABLE_CLASS(
		test_sub_class,
		(
			enum_member,
			bool_member,
			char_member,
			int_member,
			float_member,
			double_member,
			string_member
			)
	)

	REFLECTABLE_CLASS_WITH_METHOD(
		test_class,
		(
			bool_member,
			char_member,
			int_member,
			float_member,
			double_member,
			string_member,
			stc),
			(a, b)
	)

	// std::unordered_map
	// std::unordered_multimap
	// std::unordered_set
	// std::vector
	// std::array

	int a(int a, int b, int c) { return a + b + c; }



int main()
{
	test_class tc;

	auto f = mirror::type_descriptor_resolver<decltype(&test_class::a)>::get();
	std::cout << std::any_cast<int>(std::get<1>(f->invoke(&tc, &test_class::a, { std::any(1), std::any(2), std::any(3) })));
	//auto q =  mirror::method_descriptor("a");
	//q.type_descriptor_ptr = f; 
	/*
	auto tttt = std::mem_fn(&test_class::a);
	std::cout << tttt(tc, 1, 2, 3, 4) << std::endl;
	
	auto atd = mirror::type_descriptor_resolver<decltype(&test_class::b)>::get();
	std::cout << atd->invoke(a, std::make_tuple(1, 2, 3)) << std::endl;
	std::cout << std::get<0>(atd->invoke(a, { std::any(1), std::any(2), std::any(3) })) << std::endl;

	auto td = mirror::type_descriptor_resolver<decltype(tc)>::get();
	std::function<int(int, int, int, int)> ff;
	//ff(1, 2, 3, 4);

	std::cout << td->set_property(&tc, "float_member", 1) << std::endl;
	std::cout << std::any_cast<float>(td->get_property(&tc, "float_member")) << std::endl;
	std::cout << "a " << tc.float_member << std::endl;

	std::cout << td->set_property_from_string(&tc, "float_member", "1.01") << std::endl;
	std::cout << "a " << tc.float_member << std::endl;

	std::cout << td->set_property_from_string(&tc, "stc", "{\"int_member\":\"1000\"}") << std::endl;
	std::cout << "i " << tc.stc.int_member << std::endl;

	std::cout << td->get_value_to_string(&tc) << std::endl;

	for (auto it : td->members)
	{
		std::cout << it.type_descriptor_ptr->name << ":" << td->get_property_to_string(&tc, it.name) << std::endl;
	}
	std::cout << td->name << ", " << td->size << std::endl;
	std::cout << "go " << std::endl;
	*/
	return 0;
}

