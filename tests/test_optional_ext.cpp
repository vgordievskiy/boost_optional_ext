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

BOOST_AUTO_TEST_SUITE( base_cases )

/**
 * used short names:
 * lv - lvalue
 * rv - rvalue
 * op - an optional
 * tr - transform function - used in the | oprator
 * ft - filter function    - used in the | operator
 * ex - extractor function - used in the <<= operator
 */

BOOST_AUTO_TEST_CASE(Test_rv_tr_IsNotApplied)
{
    const int originValue = 1;
    boost::optional<int> op = originValue;

    auto res = op |
        [](const auto& value)
    {
        if (value != 1)
        {
            return 1000;
        }
        else
        {
            return value;
        }
    };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), originValue);
    BOOST_CHECK(res.get() != 1000);
}

BOOST_AUTO_TEST_CASE(Test_lv_tr_IsNotApplied)
{
    const int originValue = 1;
    boost::optional<int> op = originValue;

    auto lv_tr = [](const auto& value)
    {
        if (value != 1)
        {
            return 1000;
        }
        else
        {
            return value;
        }
    };

    auto res = op | lv_tr;

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), originValue);
    BOOST_CHECK(res.get() != 1000);
}

BOOST_AUTO_TEST_CASE(Test_rv_tr_IsApplied)
{
    const int originValue = 1;
    const int newValue = 1000;
    boost::optional<int> op = originValue;

    auto res = op |
        [newValue](const auto& value)
    {
        if (value == 1)
        {
            return newValue;
        }
        else
        {
            return value;
        }
    };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK(res.get() != originValue);
    BOOST_CHECK_EQUAL(res.get(), newValue);
}

BOOST_AUTO_TEST_CASE(Test_lv_tr_IsApplied)
{
    const int originValue = 1;
    const int newValue = 1000;
    boost::optional<int> op = originValue;

    auto lv_tr = [newValue](const auto& value)
    {
        if (value == 1)
        {
            return newValue;
        }
        else
        {
            return value;
        }
    };

    auto res = op | lv_tr;

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK(res.get() != originValue);
    BOOST_CHECK_EQUAL(res.get(), newValue);
}

BOOST_AUTO_TEST_CASE(Test_rvOp_rvTr_lvTr)
{
    auto lv_Tr = [](const auto& value) { return value * 2; };

    //         rv optional             | rv tr                                       | lv tr
    auto res = boost::optional<int>(1) | [](const auto& value) { return value + 1; } | lv_Tr;

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 4);
}

BOOST_AUTO_TEST_CASE(Test_rvOp_lvTr_rvTr)
{
    auto lv_Tr = [](const auto& value) { return value * 2; };

    //         rv optional             | lv tr | rv tr
    auto res = boost::optional<int>(1) | lv_Tr | [](const auto& value) { return value + 1; };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 3);
}

BOOST_AUTO_TEST_CASE(Test_lvOp_rvTr_lvTr)
{
    auto lv_Tr = [](const auto& value) { return value * 2; };

    auto lvOp = boost::optional<int>(1);

    //          lv optional | rv tr                                       | lv tr
    auto res = /*   */ lvOp | [](const auto& value) { return value + 1; } | lv_Tr;

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 4);
}

BOOST_AUTO_TEST_CASE(Test_lvOp_lvTr_rvTr)
{
    auto lv_Tr = [](const auto& value) { return value * 2; };

    auto lvOp = boost::optional<int>(1);

    //         lv optional | lv tr | rv tr
    auto res = /*  */ lvOp | lv_Tr | [](const auto& value) { return value + 1; };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 3);
}

BOOST_AUTO_TEST_CASE(Test_lvOp_lvTr_rvTr_equal_references)
{
    auto lv_Tr = [](auto& value) { return boost::optional<int&>(value); };

    auto lvOp = boost::optional<int>(1);

    //         lv optional | lv tr | rv tr
    auto res = /*  */ lvOp | lv_Tr | [](auto& value) { return boost::optional<int&>(value); };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 1);
    BOOST_CHECK_EQUAL(&lvOp.get(), &res.get());
}

BOOST_AUTO_TEST_CASE(Test_lvOp_lvTr_rvTr_equal_const_references)
{
    auto lv_Tr = [](const auto& value) { return boost::optional<const int&>(value); };

    auto lvOp = boost::optional<int>(1);

    //         lv optional | lv tr | rv tr
    auto res = /*  */ lvOp | lv_Tr | [](const auto& value) { return boost::optional<const int&>(value); };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 1);
    BOOST_CHECK_EQUAL(&lvOp.get(), &res.get());
}

BOOST_AUTO_TEST_CASE(Test_lvOp_lvTr_rvTr_not_equal_references_when_ret_as_value)
{
    auto lv_Tr = [](const auto& value) { return value; };

    auto lvOp = boost::optional<int>(1);

    //         lv optional | lv tr | rv tr
    auto res = /*  */ lvOp | lv_Tr | [](const auto& value) { return value; };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 1);
    BOOST_CHECK(&lvOp.get() != &res.get());
}

BOOST_AUTO_TEST_CASE(Test_lvOp_lvTr_rvTr_not_equal_references_when_ret_as_optional)
{
    auto lv_Tr = [](const auto& value) { return boost::optional<int>(value); };

    auto lvOp = boost::optional<int>(1);

    //         lv optional | lv tr | rv tr
    auto res = /*  */ lvOp | lv_Tr | [](const auto& value) { return boost::optional<int>(value); };

    BOOST_CHECK_EQUAL(res.has_value(), true);
    BOOST_CHECK_EQUAL(res.get(), 1);
    BOOST_CHECK(&lvOp.get() != &res.get());
}

BOOST_AUTO_TEST_CASE(Test_rvOp_rvFt_check_str_is_true_negative_case)
{
    const std::string str = "false";
    auto lv_op = boost::make_optional(str) | [](const auto& el) { return boost::make_optional(el == "true", true); };

    BOOST_CHECK_EQUAL(lv_op.has_value(), false);
}

BOOST_AUTO_TEST_CASE(Test_rvOp_rvFt_check_str_is_true_positive_case)
{
    const std::string str = "true";
    auto lv_op = boost::make_optional(str) | [](const auto& el) { return boost::make_optional(el == "true", true); };

    BOOST_CHECK_EQUAL(lv_op.has_value(), true);
    BOOST_CHECK_EQUAL(lv_op.get(), true);
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str)
{
    auto lv_ft_tr = [](const auto& el) { return boost::make_optional(el > 0, "+" + std::to_string(el)); };
    auto lv_op_positive_case = boost::make_optional(1) | lv_ft_tr;
    auto lv_op_negatie_case = boost::make_optional(-1) | lv_ft_tr;

    BOOST_CHECK_EQUAL(lv_op_positive_case.has_value(), true);
    BOOST_CHECK_EQUAL(lv_op_positive_case.get(), "+1");
    BOOST_CHECK_EQUAL(lv_op_negatie_case.has_value(), false);
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str_with_return_def_value)
{
    auto lv_ft_tr = [](const auto& el) { return boost::make_optional(el > 0, "+" + std::to_string(el)); };
    auto lv_op = boost::make_optional(-1) | lv_ft_tr |= []() -> std::string { return "not a positive number"; };

    BOOST_CHECK_EQUAL(lv_op.has_value(), true);
    BOOST_CHECK_EQUAL(lv_op.get(), "not a positive number");
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str_with_no_value_from_external_service)
{
    auto askValueFromMoon = []() { return false; };

    auto lv_ft_tr = [](const auto& el) { return boost::make_optional(el > 0, "+" + std::to_string(el)); };
    auto lv_op = boost::make_optional(-1) | lv_ft_tr |= [externalService = askValueFromMoon]() { return boost::make_optional<std::string>(externalService(), "default value"); };

    BOOST_CHECK_EQUAL(lv_op.has_value(), false);
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str_with_value_from_external_service)
{
    auto askValueFromMoon = []() { return true; };

    auto lv_ft_tr = [](const auto& el) { return boost::make_optional(el > 0, "+" + std::to_string(el)); };
    auto lv_op = boost::make_optional(-1) | lv_ft_tr |= [externalService = askValueFromMoon]() { return boost::make_optional<std::string>(externalService(), "default value"); };

    BOOST_CHECK_EQUAL(lv_op.has_value(), true);
    BOOST_CHECK_EQUAL(lv_op.get(), "default value");
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str_with_default_value_from_function)
{
    auto lv_ft_tr = [](const auto& el)
    {
        try
        {
            return boost::make_optional<int>(std::stoi(el));
        }
        catch (const std::exception& exc)
        {
            return boost::optional<int>(boost::none);
        }
    };

    auto lv_value = boost::make_optional<std::string>("not a number") | lv_ft_tr <<= []() { return std::numeric_limits<int>::max(); };

    BOOST_CHECK_EQUAL(lv_value, std::numeric_limits<int>::max());
}

BOOST_AUTO_TEST_CASE(Test_convert_int_to_str_with_default_value_from_value)
{
    auto lv_ft_tr = [](const auto& el)
    {
        try
        {
            return boost::make_optional<int>(std::stoi(el));
        }
        catch (const std::exception& exc)
        {
            return boost::optional<int>(boost::none);
        }
    };

    auto lv_value = boost::make_optional<std::string>("not a number") | lv_ft_tr <<= std::numeric_limits<int>::max();

    BOOST_CHECK_EQUAL(lv_value, std::numeric_limits<int>::max());
}

BOOST_AUTO_TEST_CASE(Test_std_function)
{
    std::function<int(const int)> stdFun = [](const int el) { return el * 0; };

    auto op = boost::make_optional(1000) | stdFun;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

int freeFn(const int el)
{
    return el * 0;
}

BOOST_AUTO_TEST_CASE(Test_free_fn)
{
    auto op = boost::make_optional(1000) | freeFn;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

BOOST_AUTO_TEST_CASE(Test_std_bind)
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

    auto op = boost::make_optional(1) | std::bind(&TestClass::action, &task, std::placeholders::_1);

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 10);
}

BOOST_AUTO_TEST_CASE(Test_static_method)
{
    class TestClass
    {
    public:
        static int action(int val)
        {
            return val * 0;
        }
    };

    auto op = boost::make_optional(1) | TestClass::action;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), 0);
}

template <typename T>
std::string freeTmplFn(const T& el)
{
    return std::to_string(el);
}

BOOST_AUTO_TEST_CASE(Test_template_fn_with_instantiation)
{
    auto op = boost::make_optional(1) | freeTmplFn<int>;

    BOOST_CHECK_EQUAL(op.has_value(), true);
    BOOST_CHECK_EQUAL(op.get(), "1");
}

BOOST_AUTO_TEST_SUITE_END()