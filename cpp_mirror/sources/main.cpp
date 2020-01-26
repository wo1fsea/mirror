#include <iostream>
#include <string>

#include "type_descriptor.hpp"

class test_class{
	public:
	bool bool_member;

	char char_member;

	int int_member;

	float float_member;
	double double_member;
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
	int i = 0;
	auto td = type_descriptor_resolver<test_class>::get();
	for(auto it : td->members)
	{
		std::cout << it.type_descriptor_ptr->name << std::endl;
	}
	std::cout << td->name << ", " << td->size << std::endl;
	std::cout << sizeof(std::vector<member_descriptor>) << std::endl;
	std::cout << sizeof(std::vector<member_descriptor>*) << std::endl;
	return 0;
}
