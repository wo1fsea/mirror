#include <iostream>
#include <string>
#include <functional>

#include "type_descriptor.hpp"

class test_class{
	public:
	bool bool_member;

	char char_member;

	int int_member;

	float float_member;
	double double_member;

	test_class(){}
	int a(int a, int b, int c, int d){return a+b+c+d;}
};

REFLECTABLE(
	test_class, 
	(
		bool_member, 
		char_member, 
		int_member, 
		float_member, 
		double_member
	)
)

int main() {
	test_class tc;
	auto td = type_descriptor_resolver<decltype(tc)>::get();
	std::cout << td->set_property(&tc, "int_member", 1) << std::endl;
	std::cout << std::any_cast<double>(td->get_property(&tc, "double_member")) << std::endl;
	std::cout << "a " << tc.int_member << std::endl;

	for(auto it : td->members)
	{
		std::cout << it.type_descriptor_ptr->name << std::endl;
	}
	std::cout << td->name << ", " << td->size << std::endl;
	
	return 0;
}


