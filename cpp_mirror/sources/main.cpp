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

template <> 
struct type_descriptor_resolver<test_class>{ 
   static type_descriptor<test_class>* get() 
   { 
       static type_descriptor<test_class> td("test_class", sizeof(test_class));
	   td.members = {
		   member_descriptor(
			   "bool_member", 
				offsetof(test_class, bool_member),
			   type_descriptor_resolver<decltype(test_class::bool_member)>::get()
		   ),

	   };
       return &td; 
   } 
};


int main() {
	int i = 0;
	auto td = type_descriptor_resolver<test_class>::get();
	for(auto it : td->members)
	{
		std::cout << it.type_descriptor_ptr->name << std::endl;
	}
	std::cout << td->name << ", " << td->size << std::endl;
	return 0;
}
