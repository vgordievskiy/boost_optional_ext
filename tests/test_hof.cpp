#include <boost/test/unit_test.hpp>

#include <boost/optional.hpp>
#include <boost/optional_ext.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/type_index.hpp>

#include <limits>
#include <functional>
#include <map>
#include <tuple>
#include <iostream>

BOOST_AUTO_TEST_SUITE( higher_order_functions )

BOOST_AUTO_TEST_CASE(case_toRefOp)
{
    const auto op = boost::make_optional(1);

    const auto resNoRef = op;
    const auto resRef = toRefOp(op);

    BOOST_CHECK(resNoRef.get_ptr() != op.get_ptr());
    BOOST_CHECK_EQUAL(resRef.get_ptr(), op.get_ptr());
}

BOOST_AUTO_TEST_CASE(case_filter_if_Accepted)
{
    const auto op = boost::make_optional(std::string("!"));

    const auto res = toRefOp(op) | hof::filter_if(([](auto&& el) { return !el.empty(); }));

    BOOST_REQUIRE_MESSAGE(res.has_value(), "boost::optional has no value!");
    BOOST_CHECK_EQUAL(res.get(), op.get());
    BOOST_CHECK_EQUAL(res.get_ptr(), op.get_ptr());
}

BOOST_AUTO_TEST_CASE(case_filter_if_NotAccepted)
{
    const auto op = boost::make_optional(std::string("!"));

    const auto res = toRefOp(op) | hof::filter_if(([](auto&& el) { return el.empty(); }));

    BOOST_REQUIRE_MESSAGE(!res.has_value(), "boost::optional has a value!");
}

BOOST_AUTO_TEST_CASE(case_filter_if_not_Accepted)
{
    const auto op = boost::make_optional(std::string("!"));

    const auto res = toRefOp(op) | hof::filter_if_not(([](auto&& el) { return el.empty(); }));

    BOOST_REQUIRE_MESSAGE(res.has_value(), "boost::optional has no value!");
    BOOST_CHECK_EQUAL(res.get(), op.get());
    BOOST_CHECK_EQUAL(res.get_ptr(), op.get_ptr());
}

BOOST_AUTO_TEST_CASE(case_filter_if_not_NotAccepted)
{
    const auto op = boost::make_optional(std::string("!"));

    const auto res = toRefOp(op) | hof::filter_if_not(([](auto&& el) { return !el.empty(); }));

    BOOST_REQUIRE_MESSAGE(!res.has_value(), "boost::optional has a value!");
}

BOOST_AUTO_TEST_CASE(case_filter_if_not_not_const_Accepted)
{
    auto op = boost::make_optional(std::string("!"));

    auto res = toRefOp(op) | hof::filter_if_not(([](auto&& el) { return el.empty(); }));

    BOOST_REQUIRE_MESSAGE(res.has_value(), "boost::optional has no value!");
    BOOST_CHECK_EQUAL(res.get(), op.get());
    BOOST_CHECK_EQUAL(res.get_ptr(), op.get_ptr());
}

BOOST_AUTO_TEST_CASE(case_match_some)
{
    auto op = boost::make_optional(std::make_tuple(10, std::string("ten")));

    size_t strLen = 0;

    auto isPositive = [](auto&& el) { return std::get<0>(el) > 0; };
    auto log = [&strLen](auto&& el) mutable { 
        BOOST_TEST_MESSAGE("value is " << std::get<1>(el));
        strLen = std::get<1>(el).size();
    };

    auto res = toRefOp(op)
     | hof::filter_if(isPositive)
     | hof::match_some(log)
    ;

    BOOST_REQUIRE_MESSAGE(res.has_value(), "boost::optional has no value!");
    BOOST_CHECK_EQUAL(strLen, std::get<1>(*res).size());
}

BOOST_AUTO_TEST_CASE(case_match_some_none)
{
    auto op = boost::make_optional(std::make_tuple(-10, std::string("ten")));

    auto isPositive = [](auto&& el) { return std::get<0>(el) > 0; };
    auto some = [](auto&& el) { 
        BOOST_TEST_MESSAGE("value is " << std::get<1>(el));
    };

    bool isNone = false;

    auto none = [&isNone]() mutable {
        isNone = true;
    };

    auto res = toRefOp(op)
     | hof::filter_if(isPositive)
     | hof::match(some, none)
    ;

    BOOST_REQUIRE_MESSAGE(!res.has_value(), "boost::optional has a value!");
    BOOST_CHECK_EQUAL(isNone, true);
}

BOOST_AUTO_TEST_CASE(case_match_some_2)
{
    auto op = boost::make_optional(std::make_tuple(10, std::string("ten")));

    bool isSome = false;

    auto isPositive = [](auto&& el) { return std::get<0>(el) > 0; };
    auto some = [&isSome](auto&& el) { 
        BOOST_TEST_MESSAGE("value is " << std::get<1>(el));
        isSome = true;
    };

    auto res = toRefOp(op)
     | hof::filter_if(isPositive)
     | hof::match_some(some);

    BOOST_REQUIRE_MESSAGE(res.has_value(), "boost::optional has no value!");
    BOOST_CHECK_EQUAL(isSome, true);
}

BOOST_AUTO_TEST_SUITE_END()
