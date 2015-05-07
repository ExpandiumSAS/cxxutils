#define BOOST_TEST_MODULE test_utils

#include <cxxutils/unit_test.hpp>
#include <cxxutils/system_config.hpp>
#include <utils/logging.hpp>

#include <cxxu/utils.hpp>

struct utils_fixture
{
	utils_fixture()
    {
        logging::disable_logging();
    }

};

BOOST_FIXTURE_TEST_SUITE(utils_test, utils_fixture);

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

BOOST_AUTO_TEST_SUITE_END();
