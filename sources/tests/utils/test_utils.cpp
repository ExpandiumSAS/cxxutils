#define BOOST_TEST_MODULE test_utils

#include <cxxutils/unit_test.hpp>
#include <cxxutils/system_config.hpp>

#include <cxxu/utils.hpp>

BOOST_AUTO_TEST_CASE(trim_left)
{
	BOOST_CHECK_EQUAL(cxxu::trim_left(""), "");
	BOOST_CHECK_EQUAL(cxxu::trim_left(" "), "");
	BOOST_CHECK_EQUAL(cxxu::trim_left("\t"), "");
	BOOST_CHECK_EQUAL(cxxu::trim_left("    "), "");
	BOOST_CHECK_EQUAL(cxxu::trim_left("\t\t\t"), "");
	BOOST_CHECK_EQUAL(cxxu::trim_left(" \t\t \t  \t "), "");

	BOOST_CHECK_EQUAL(cxxu::trim_left(" A"), "A");
	BOOST_CHECK_EQUAL(cxxu::trim_left("   A  "), "A  ");
	BOOST_CHECK_EQUAL(cxxu::trim_left(" \t\tA"), "A");
	BOOST_CHECK_EQUAL(cxxu::trim_left("   \t\tABCDEF"), "ABCDEF");
	BOOST_CHECK_EQUAL(cxxu::trim_left("A"), "A");
	BOOST_CHECK_EQUAL(cxxu::trim_left("ABCDEF"), "ABCDEF");
}
