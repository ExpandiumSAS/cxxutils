#define BOOST_TEST_MODULE test_any_visitor

#include <cxxutils/unit_test.hpp>
#include <cxxutils/system_config.hpp>

#include <cxxu/any_visitor.hpp>

BOOST_AUTO_TEST_CASE(any_visitor_basic)
{
    int int_count = 0;
    int string_count = 0;

    cxxu::any_visitor visitor;

    auto f = [&int_count](const auto& i) { ++int_count; };
    visitor.insert_visitor<int>(f);
    visitor.insert_visitor<long long>(f);

    visitor.insert_visitor<std::string>([&string_count](const auto& i) {
        ++string_count;
    });

    boost::any any_int(5);
    visitor(any_int);

    boost::any any_longlong(123);
    visitor(any_longlong);

    boost::any any_string(std::string("hello"));
    visitor(any_string);

    BOOST_CHECK_EQUAL(int_count, 2);
    BOOST_CHECK_EQUAL(string_count, 1);
}
