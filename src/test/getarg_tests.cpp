#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-KNC");
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", false));
    BOOST_CHECK(GetBoolArg("-KNC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-KNCo"));
    BOOST_CHECK(!GetBoolArg("-KNCo", false));
    BOOST_CHECK(GetBoolArg("-KNCo", true));

    ResetArgs("-KNC=0");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", false));
    BOOST_CHECK(!GetBoolArg("-KNC", true));

    ResetArgs("-KNC=1");
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", false));
    BOOST_CHECK(GetBoolArg("-KNC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noKNC");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", false));
    BOOST_CHECK(!GetBoolArg("-KNC", true));

    ResetArgs("-noKNC=1");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", false));
    BOOST_CHECK(!GetBoolArg("-KNC", true));

    ResetArgs("-KNC -noKNC");  // -KNC should win
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", false));
    BOOST_CHECK(GetBoolArg("-KNC", true));

    ResetArgs("-KNC=1 -noKNC=1");  // -KNC should win
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", false));
    BOOST_CHECK(GetBoolArg("-KNC", true));

    ResetArgs("-KNC=0 -noKNC=0");  // -KNC should win
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", false));
    BOOST_CHECK(!GetBoolArg("-KNC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--KNC=1");
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", false));
    BOOST_CHECK(GetBoolArg("-KNC", true));

    ResetArgs("--noKNC=1");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", false));
    BOOST_CHECK(!GetBoolArg("-KNC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KNC", "eleven"), "eleven");

    ResetArgs("-KNC -bar");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KNC", "eleven"), "");

    ResetArgs("-KNC=");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-KNC", "eleven"), "");

    ResetArgs("-KNC=11");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-KNC", "eleven"), "11");

    ResetArgs("-KNC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-KNC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-KNC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-KNC", 0), 0);

    ResetArgs("-KNC -bar");
    BOOST_CHECK_EQUAL(GetArg("-KNC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-KNC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-KNC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-KNC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-KNC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--KNC");
    BOOST_CHECK_EQUAL(GetBoolArg("-KNC"), true);

    ResetArgs("--KNC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-KNC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noKNC");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", true));
    BOOST_CHECK(!GetBoolArg("-KNC", false));

    ResetArgs("-noKNC=1");
    BOOST_CHECK(!GetBoolArg("-KNC"));
    BOOST_CHECK(!GetBoolArg("-KNC", true));
    BOOST_CHECK(!GetBoolArg("-KNC", false));

    ResetArgs("-noKNC=0");
    BOOST_CHECK(GetBoolArg("-KNC"));
    BOOST_CHECK(GetBoolArg("-KNC", true));
    BOOST_CHECK(GetBoolArg("-KNC", false));

    ResetArgs("-KNC --noKNC");
    BOOST_CHECK(GetBoolArg("-KNC"));

    ResetArgs("-noKNC -KNC"); // KNC always wins:
    BOOST_CHECK(GetBoolArg("-KNC"));
}

BOOST_AUTO_TEST_SUITE_END()
