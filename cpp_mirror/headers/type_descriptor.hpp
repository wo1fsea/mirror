#include <string>
#include <vector>
#include <map>

#include <boost/preprocessor.hpp>

#include "primitive_type.hpp"

#define type_name(x) #x

class type_descriptor_base;

struct member_descriptor
{
    const char *name;
    size_t offset;
    type_descriptor_base *type_descriptor_ptr;
};

class type_descriptor_base
{
    public:
    using members_descriptor = std::vector<member_descriptor>;
        const char *name;
        size_t size;
        members_descriptor members;
        type_descriptor_base(const char *type_name, size_t type_size, members_descriptor type_members={}):
        name(type_name), size(type_size), members(type_members){}
};

template<typename T>
class type_descriptor: public type_descriptor_base
{
    using type = T;
    public:
        type_descriptor(const char *type_name, size_t type_size, members_descriptor type_members={}): 
        type_descriptor_base(type_name, type_size, type_members){}
};

template<typename T>
struct type_descriptor_resolver{
   static type_descriptor<T>* get()
   {
       return nullptr;
   }
};

#define PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER(x) \
template <> \
struct type_descriptor_resolver<x>{ \
   static type_descriptor<x>* get() \
   { \
       static type_descriptor<x> td(#x, sizeof(x)); \
       return &td; \
   } \
};


#define PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH(r, data, i, x) \
PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER(x)

BOOST_PP_SEQ_FOR_EACH_I(PRIMITIVE_TYPE_DESCRIPTOR_RESOLVER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(PRIMITIVE_TYPE), PRIMITIVE_TYPE))

#define REFLECTABLE_MEMBER(member_name) \
member_descriptor{ \
    #member_name, \
    offsetof(T, member_name), \
    type_descriptor_resolver<decltype(T::member_name)>::get() \
},

#define REFLECTABLE_MEMBER_FOR_EACH(r, data, i, x) \
REFLECTABLE_MEMBER(x)


#define REFLECTABLE(class_name, members) \
template <> \
struct type_descriptor_resolver<class_name>{ \
    using T = class_name; \
    static type_descriptor<T>* get() \
    { \
        static type_descriptor<T> td( \
		    #class_name, \
		    sizeof(T), \
		    { \
                BOOST_PP_SEQ_FOR_EACH_I(REFLECTABLE_MEMBER_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOOST_PP_TUPLE_SIZE(members), members)) \
		    } \
	   ); \
       return &td; \
    } \
};