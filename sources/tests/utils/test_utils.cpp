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

BOOST_AUTO_TEST_CASE(trim_right)
{
    BOOST_CHECK_EQUAL(cxxu::trim_right(""), "");
    BOOST_CHECK_EQUAL(cxxu::trim_right(" "), "");
    BOOST_CHECK_EQUAL(cxxu::trim_right("\t"), "");
    BOOST_CHECK_EQUAL(cxxu::trim_right("    "), "");
    BOOST_CHECK_EQUAL(cxxu::trim_right("\t\t\t"), "");
    BOOST_CHECK_EQUAL(cxxu::trim_right(" \t\t \t  \t "), "");

    BOOST_CHECK_EQUAL(cxxu::trim_right("A "), "A");
    BOOST_CHECK_EQUAL(cxxu::trim_right("   A  "), "   A");
    BOOST_CHECK_EQUAL(cxxu::trim_right("A \t\t"), "A");
    BOOST_CHECK_EQUAL(cxxu::trim_right("ABCDEF   \t\t"), "ABCDEF");
    BOOST_CHECK_EQUAL(cxxu::trim_right("A"), "A");
    BOOST_CHECK_EQUAL(cxxu::trim_right("ABCDEF"), "ABCDEF");
}

BOOST_AUTO_TEST_CASE(trim)
{
    BOOST_CHECK_EQUAL(cxxu::trim(""), "");
    BOOST_CHECK_EQUAL(cxxu::trim(" "), "");
    BOOST_CHECK_EQUAL(cxxu::trim("\t"), "");
    BOOST_CHECK_EQUAL(cxxu::trim("    "), "");
    BOOST_CHECK_EQUAL(cxxu::trim("\t\t\t"), "");
    BOOST_CHECK_EQUAL(cxxu::trim(" \t\t \t  \t "), "");

    BOOST_CHECK_EQUAL(cxxu::trim("A "), "A");
    BOOST_CHECK_EQUAL(cxxu::trim("   A  "), "A");
    BOOST_CHECK_EQUAL(cxxu::trim(" \t\tA \t\t"), "A");
    BOOST_CHECK_EQUAL(cxxu::trim("   \t\tABCDEF   \t\t"), "ABCDEF");
    BOOST_CHECK_EQUAL(cxxu::trim("A"), "A");
    BOOST_CHECK_EQUAL(cxxu::trim("ABCDEF"), "ABCDEF");
}
