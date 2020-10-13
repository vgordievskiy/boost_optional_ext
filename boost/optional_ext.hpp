#pragma once

#include <type_traits>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>

namespace optional_detail {

template <typename T>
struct optional_value_type
{
    using type = T;
};

template <typename T>
struct optional_value_type<boost::optional<T>>
{
    using type = typename boost::optional<T>::value_type;
};

template <typename T>
struct is_optional_type : public boost::false_type
{
};

template <typename T>
struct is_optional_type<boost::optional<T>> : public boost::true_type
{
};

} // namespace optional_detail

namespace type_traits {

template <typename T>
class has_operator_fn
{
    typedef char true_value;
    struct false_value
    {
        char x[2];
    };

    template <typename C>
    static true_value has_operator(decltype(&C::operator()));
    template <typename C>
    static false_value has_operator(...);

public:
    enum
    {
        value = sizeof(has_operator<T>(0)) == sizeof(char) ? boost::true_type::value : boost::false_type::value
    };
};

template <class T>
struct is_callable : std::integral_constant<bool, has_operator_fn<typename std::remove_reference<T>::type>::value || std::is_function<typename std::remove_reference<T>::type>::value>
{
};

} // namespace type_traits

/**
* It's a pipe operator for applying transformations for boost::optional
* The next function will be applied if boost::optional isn't empty
* it's alias the "map" operator
* @param op is a boost::optional<T>
* @param f is a function that takes boost::optional<T>::value_type and returns a new value
* @return a new boost::optional
*
* an example of usage:
*
*    int value;
*    std::cin >> value;
*
*    auto op = boost::optional<int>(value)
*        // can use as filter
*        | [] (int val) -> boost::optional<int>
*          {
*            return val >= 0 ? boost::optional<int>(val) : boost::none;
*          }
*        // can be used as map function
*        | [](int val)
*          {
*            return "Value is: " + std::to_string(val);
*          };
*
*     if (op)
*     {
*       std::cout << op.get() << std::endl;
*     }
*     else
*     {
*         std::cout << "op is empty" << std::endl;
*     }
*/
template <class TOptional,
          typename FuncType,
          typename TOptionalParam = typename std::remove_reference_t<TOptional>::value_type,
          typename FuncResult = typename boost::result_of<FuncType(TOptionalParam&)>::type,
          typename IsFuncResOptional = optional_detail::is_optional_type<FuncResult>,
          typename TOptionalResult = boost::conditional_t<IsFuncResOptional::value, typename optional_detail::optional_value_type<FuncResult>::type, FuncResult>,
          typename boost::enable_if_c<boost::is_same<std::remove_reference_t<TOptional>, boost::optional<TOptionalParam>>::value == boost::true_type::value, int>::type = 0,
          typename boost::enable_if_c<IsFuncResOptional::value == boost::false_type::value, int>::type = 0>
boost::optional<TOptionalResult> operator|(TOptional&& op, FuncType&& f)
{
    if (op)
    {
        return f(op.get());
    }
    else
    {
        return boost::none;
    }
}

/**
 * It's a pipe operator for applying extractor for boost::optional
 * The next function will be applied if boost::optional is empty
 * it's alias the "flat_map" operator
 * @param op is a boost::optional<T>
 * @param f is a function that returns a new boost::optional
 * @return boost::optional
 *
 * an example of usage:
 *
 *    int value;
 *    std::cin >> value;
 *
 *    auto op = boost::optional<int>(value)
 *        // can use as filter
 *        | [] (int val) -> boost::optional<int>
 *          {
 *            return val >= 0 ? boost::optional<int>(val) : boost::none;
 *          };
 *
 *     if (op)
 *     {
 *       std::cout << op.get() << std::endl;
 *     }
 *     else
 *     {
 *         std::cout << "op is empty" << std::endl;
 *     }
 */
template <class TOptional,
          typename FuncType,
          typename TOptionalParam = typename std::remove_reference_t<TOptional>::value_type,
          typename FuncResult = typename boost::result_of<FuncType(TOptionalParam&)>::type,
          typename IsFuncResOptional = optional_detail::is_optional_type<FuncResult>,
          typename TOptionalResult = boost::conditional_t<IsFuncResOptional::value, typename optional_detail::optional_value_type<FuncResult>::type, FuncResult>,
          typename boost::enable_if_c<boost::is_same<std::remove_reference_t<TOptional>, boost::optional<TOptionalParam>>::value == boost::true_type::value, int>::type = 0,
          typename boost::enable_if_c<IsFuncResOptional::value == boost::true_type::value, int>::type = 1>
boost::optional<TOptionalResult> operator|(TOptional&& op, FuncType&& f)
{
    if (op)
    {
        auto ret = f(op.get());
        if (ret)
        {
            return boost::optional<TOptionalResult>(ret.get());
        }
        else
        {
            return boost::none;
        }
    }
    else
    {
        return boost::none;
    }
}

/**
 * It's a pipe operator for applying extractor for boost::optional
 * The next function will be applied if boost::optional is empty
 * @param op is a boost::optional<T>
 * @param f is a function that returns a new boost::optional
 * @return boost::optional
 *
 * an example of usage:
 *
 *    int value;
 *    std::cin >> value;
 *
 *    auto op = boost::optional<int>(-1)
 *        // check value
 *        | [] (int val) -> boost::optional<int>
 *          {
 *            return val >= 0 ? boost::optional<int>(val) : boost::none;
 *          }
 *        // if it's boost::none
 *        |= []()
 *          {
 *            return 0;
 *          };
 *
 *     if (op)
 *     {
 *       std::cout << op.get() << std::endl;
 *     }
 *     else
 *     {
 *         std::cout << "op is empty" << std::endl;
 *     }
 */
template <class TOptional,
          typename FuncType,
          typename TOptionalParam = typename std::remove_reference_t<TOptional>::value_type,
          typename FuncResult = typename boost::result_of<FuncType()>::type,
          typename IsFuncResOptional = optional_detail::is_optional_type<FuncResult>,
          typename TOptionalResult = boost::conditional_t<IsFuncResOptional::value, typename optional_detail::optional_value_type<FuncResult>::type, FuncResult>,
          typename boost::enable_if_c<boost::is_same<std::remove_reference_t<TOptional>, boost::optional<TOptionalParam>>::value == boost::true_type::value, int>::type = 0>
boost::optional<TOptionalResult> operator|=(TOptional&& op, FuncType&& f)
{
    if (op)
    {
        return op;
    }
    else
    {
        return f();
    }
}

/**
 * It's a pipe operator for applying extractor for boost::optional
 * The next function will be applied if boost::optional is empty
 * @param op is a boost::optional<T>
 * @param f is a function that returns a default value
 * @return boost::optional<T>::value_type
 *
 * an example of usage:
 *
 *    int value;
 *    std::cin >> value;
 *
 *    auto myValue = boost::optional<int>(-1)
 *        // check value
 *        | [] (int val) -> boost::optional<int>
 *          {
 *            return val >= 0 ? boost::optional<int>(val) : boost::none;
 *          }
 *        // if it's boost::none
 *        <<= []()
 *          {
 *            return 0;
 *          };
 *
 *       std::cout << myValue << std::endl;
 */
template <class TOptional,
          typename FuncType,
          typename TOptionalParam = typename std::remove_reference_t<TOptional>::value_type,
          typename IsNestedOptional = optional_detail::is_optional_type<typename TOptional::value_type>,
          typename FuncResult = typename boost::result_of<FuncType()>::type,
          typename boost::enable_if_c<boost::is_same<std::remove_reference_t<TOptional>, boost::optional<TOptionalParam>>::value == boost::true_type::value, int>::type = 0>
FuncResult operator<<=(TOptional&& op, FuncType&& f)
{
    if (op)
    {
        return op.get();
    }
    else
    {
        return f();
    }
}

/**
 * It's a pipe operator for applying extractor for boost::optional
 * The next function will be applied if boost::optional is empty
 * @param op is a boost::optional<T>
 * @param value is a default value
 * @return boost::optional<T>::value_type
 *
 * an example of usage:
 *
 *    int value;
 *    std::cin >> value;
 *
 *    auto myValue = boost::optional<int>(-1)
 *        // check value
 *        | [] (int val) -> boost::optional<int>
 *          {
 *            return val >= 0 ? boost::optional<int>(val) : boost::none;
 *          }
 *        // if it's boost::none
 *        <<= 0
 *
 *       std::cout << myValue << std::endl;
 */
template <class TOptional,
          typename ValueType,
          typename TOptionalParam = typename std::remove_reference_t<TOptional>::value_type,
          typename boost::enable_if_c<boost::is_same<std::remove_reference_t<TOptional>, boost::optional<TOptionalParam>>::value == boost::true_type::value, int>::type = 1,
          typename boost::enable_if_c<type_traits::is_callable<ValueType>::value == boost::false_type::value, int>::type = 1>
ValueType operator<<=(TOptional&& op, ValueType&& value)
{
    if (op)
    {
        return op.get();
    }
    else
    {
        return value;
    }
}
