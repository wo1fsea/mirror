#include <boost/test/unit_test.hpp>
#include <mirror.h>

BOOST_AUTO_TEST_SUITE(test)

// BOOST_FIXTURE_TEST_CASE( test_case1, F ) {
//    BOOST_CHECK( i == 0 );
//    ++i;
// }

BOOST_AUTO_TEST_CASE(test_primitive) {
    int test_int = 0;
    auto td = mirror::type_descriptor_resolver<decltype(test_int)>::get();
    
    BOOST_REQUIRE(td != nullptr );
}


BOOST_AUTO_TEST_SUITE_END()