#include <iostream>
#include <string>
#include <functional>

#include "type_descriptor.hpp"

class test_sub_class
{
	public:
		bool bool_member = false;
		char char_member = 'a';
		int int_member = 1;
		float float_member = 1.1;
		double double_member = 1.1;
		std::string string_member = "string";

};

class test_class
{
	public:
		bool bool_member = false;

		char char_member = 'a';

		int int_member = 1;

		float float_member = 1.1;
		double double_member = 1.1;
		std::string string_member = "string";

		test_sub_class stc;

		// test_class(){}
		int a(int a, int b, int c, int d){return a+b+c+d;}
};

REFLECTABLE(
	test_sub_class,
	(
		bool_member,
		char_member,
		int_member,
		float_member,
		double_member,
		string_member
	)
)

REFLECTABLE(
	test_class, 
	(
		bool_member, 
		char_member, 
		int_member, 
		float_member, 
		double_member,
		string_member,
		stc
	)
)

int main() {
	test_class tc;
	auto td = type_descriptor_resolver<decltype(tc)>::get();
	std::cout << td->set_property(&tc, "float_member", 1) << std::endl;
	std::cout << std::any_cast<float>(td->get_property(&tc, "float_member")) << std::endl;
	std::cout << "a " << tc.float_member << std::endl;

	std::cout << td->set_property_from_string(&tc, "float_member", "1.01") << std::endl;
	std::cout << "a " << tc.float_member << std::endl;
	
	std::cout << td->set_property_from_string(&tc, "stc", "{\"int_member\":\"1000\"}") << std::endl;
	std::cout << "i " << tc.stc.int_member << std::endl;

	std::cout << td->get_value_to_string(&tc) << std::endl;
	
	for(auto it : td->members)
	{
		std::cout << it.type_descriptor_ptr->name << ":" << td->get_property_to_string(&tc, it.name) << std::endl;
	}
	std::cout << td->name << ", " << td->size << std::endl;
	
	return 0;
}


