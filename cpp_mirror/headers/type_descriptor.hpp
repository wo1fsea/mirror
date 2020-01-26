#include <string>
#include <vector>
#include <map>

#include <boost/preprocessor.hpp>

#include "primitive_type.hpp"

#define type_name(x) #x

class type_descriptor_base;

class member_descriptor
{
    public:
        std::string name;
        size_t offset;
        type_descriptor_base *type_descriptor_ptr;
        member_descriptor(const std::string member_name, size_t member_offset, type_descriptor_base *member_type_descriptor_ptr):
        name(member_name), offset(member_offset), type_descriptor_ptr(member_type_descriptor_ptr){}
};

class type_descriptor_base
{
    public:
        std::string name;
        size_t size;
        std::vector<member_descriptor> members;
        type_descriptor_base(const std::string type_name, size_t type_size):
        name(type_name), size(type_size){}

        type_descriptor_base& add_member(member_descriptor m)
        {
            return *this;
        }
};

template<typename T>
class type_descriptor: public type_descriptor_base
{
    using type = T;
    public:
        type_descriptor(const std::string type_name, size_t type_size): 
        type_descriptor_base(type_name, type_size)
        {
            
        }
};

template<typename T>
struct type_descriptor_resolver{
   static type_descriptor<T>* get()
   {
       return nullptr;
   }
};

#define ENABLE_TYPE_DESCRIPTOR_RESOLVER(x) \
template <> \
struct type_descriptor_resolver<x>{ \
   static type_descriptor<x>* get() \
   { \
       static type_descriptor<x> td(#x, sizeof(x)); \
       return &td; \
   } \
};


#define ENABLE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH(r, data, i, x) \
ENABLE_TYPE_DESCRIPTOR_RESOLVER(x)

BOOST_PP_SEQ_FOR_EACH_I(ENABLE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(PRIMITIVE_TYPE), PRIMITIVE_TYPE))

class type_descriptor_manager
{

};