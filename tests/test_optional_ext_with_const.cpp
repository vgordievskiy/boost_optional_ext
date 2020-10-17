
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

BOOST_AUTO_TEST_SUITE( const_cases )

bool testFn(const boost::optional<std::string>& op)
{
    auto ret = op | [](auto&& el)
    {
        return !el.empty();
    };
    return ret.get_value_or(false);
}

BOOST_AUTO_TEST_CASE(case_const_ref_positive)
{
    auto op = boost::make_optional<std::string>("hello");

    auto res = testFn(op);
    BOOST_CHECK(res == true);
}

BOOST_AUTO_TEST_CASE(case_const_ref_negative)
{   
    auto op = boost::make_optional<std::string>("");

    auto res = testFn(op);
    BOOST_CHECK(res == false);
}

BOOST_AUTO_TEST_CASE(std_function)
{
    std::function<int(const int)> stdFun = [](const int el) { return el * 0; };

    const auto cOp = boost::make_optional(1); 

    auto op = cOp | stdFun;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

int freeFn(const int el)
{
    return el * 0;
}

BOOST_AUTO_TEST_CASE(free_fn)
{
    const auto cOp = boost::make_optional(1); 
    auto op = cOp | freeFn;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

BOOST_AUTO_TEST_CASE(std_bind)
{
    class TestClass
    {
        int m_multiplier;

    public:
        TestClass(int val)
            : m_multiplier(val)
        {
        }

        int action(int val)
        {
            return val * m_multiplier;
        }
    };

    TestClass task = 10;

    const auto cOp = boost::make_optional(1); 

    auto op = cOp | std::bind(&TestClass::action, &task, std::placeholders::_1);

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 10);
}

BOOST_AUTO_TEST_CASE(static_method)
{
    class TestClass
    {
    public:
        static int action(int val)
        {
            return val * 0;
        }
    };

    const auto cOp = boost::make_optional(1); 
    auto op = cOp | TestClass::action;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

template <typename T>
std::string freeTmplFn(const T& el)
{
    return std::to_string(el);
}

BOOST_AUTO_TEST_CASE(template_fn_with_instantiation)
{
    const auto cOp = boost::make_optional(1); 
    auto op =  cOp | freeTmplFn<int>;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), "1");
}

BOOST_AUTO_TEST_SUITE_END()