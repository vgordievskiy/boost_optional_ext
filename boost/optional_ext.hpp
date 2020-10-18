#pragma once

#include <type_traits>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>


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

struct ArgFunctor
{
};
struct ArgValue
{
};

template <typename Arg>
using argument_type_t = std::conditional_t<type_traits::is_callable<Arg>::value == boost::true_type::value, ArgFunctor, ArgValue>;

} // namespace type_traits

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
struct optional_value_type<const boost::optional<T>>
{
    using type = const typename boost::optional<T>::value_type;
};

template <typename T>
struct optional_value_type<const boost::optional<T>&>
{
    using type = typename boost::optional<T>::reference_const_type;
};

template <typename T>
struct optional_value_type<boost::optional<T>&>
{
    using type = typename boost::optional<T>::reference_type;
};

template <typename T>
struct is_optional_type : public boost::false_type
{
};

template <typename T>
struct is_optional_type<boost::optional<T>> : public boost::true_type
{
};

template <typename T>
struct is_optional_type<const boost::optional<T>&> : public boost::true_type
{
};

template <typename T>
struct is_optional_type<boost::optional<T>&> : public boost::true_type
{
};

template <class TOptional>
using optional_value_type_t = typename optional_detail::optional_value_type<TOptional>::type;

template <typename FuncType, typename TOptional>
using get_functor_result_type_t = typename boost::result_of<FuncType(optional_value_type_t<TOptional>)>::type;

template <typename TOptional>
using is_operator_applicable = is_optional_type<TOptional>;


template <typename F>
struct THigherOrderFunction : F
{
    constexpr THigherOrderFunction(F&& f)
        : F{std::forward<F>(f)}
    {
    }
};

template <typename T>
struct is_higher_order_function : public boost::false_type
{
};

template <typename T>
struct is_higher_order_function<THigherOrderFunction<T>> : public boost::true_type
{
};

template <typename T>
struct is_higher_order_function<THigherOrderFunction<T>&> : public boost::true_type
{
};

template <typename T>
struct is_higher_order_function<THigherOrderFunction<T>&&> : public boost::true_type
{
};

template <typename T>
struct is_higher_order_function<const THigherOrderFunction<T>> : public boost::true_type
{
};

template <typename T>
struct is_higher_order_function<const THigherOrderFunction<T>&> : public boost::true_type
{
};


// clang-format off

template<typename TOptional, typename FuncType, bool isUsed, bool isHOF>
struct TOptinalTypes;

template<typename TOptional, typename FuncType, bool isHOF>
struct TOptinalTypes<TOptional, FuncType, false, isHOF>
{
    using op_value_type = void;
    using invoc_result = void;
    using is_invoc_result_optional = void;
    using deduced_result = void;
    static constexpr bool is_applicable = false;
    static constexpr bool has_higher_order_functon = false;
};

template<typename TOptional, typename FuncType>
struct TOptinalTypes<TOptional, FuncType, true, false>
{
    using op_value_type = optional_value_type_t<TOptional>;
    using invoc_result = get_functor_result_type_t<FuncType, TOptional>;
    using is_invoc_result_optional = is_optional_type<invoc_result>;
    using deduced_result = boost::conditional_t<
        is_invoc_result_optional::value,
        optional_value_type_t<invoc_result>,
            invoc_result>;

    static constexpr bool is_applicable = is_operator_applicable<TOptional>::value == boost::true_type::value;

    static constexpr bool is_map_like = is_invoc_result_optional::value == boost::false_type::value;
    static constexpr bool is_flat_map_like = is_invoc_result_optional::value == boost::true_type::value;
    static constexpr bool has_higher_order_functon = false;
};

template<typename TOptional, typename FuncType>
struct TOptinalTypes<TOptional, FuncType, true, true>
{
    using op_value_type = optional_value_type_t<TOptional>;
    using invoc_result = typename boost::result_of<FuncType(TOptional)>::type;
    using is_invoc_result_optional = is_optional_type<invoc_result>;
    using deduced_result = invoc_result;

    static constexpr bool is_applicable = is_operator_applicable<TOptional>::value == boost::true_type::value;
    static constexpr bool is_map_like = is_invoc_result_optional::value == boost::false_type::value;
    static constexpr bool is_flat_map_like = is_invoc_result_optional::value == boost::true_type::value;
    static constexpr bool has_higher_order_functon = true;
};


template<typename TOptional, typename FuncType, typename ArgType, bool IsUsed>
struct TOptinalNoneTypes
{
    static constexpr bool is_applicable = false;
};

template<typename TOptional, typename FuncType>
struct TOptinalNoneTypes<TOptional, FuncType, type_traits::ArgFunctor, true>
{
    using op_value_type = optional_value_type_t<TOptional>;
    using invoc_result = typename boost::result_of<FuncType()>::type;
    using is_invoc_result_optional = is_optional_type<invoc_result>;
    using deduced_result = boost::conditional_t<
        is_invoc_result_optional::value,
        optional_value_type_t<invoc_result>,
            invoc_result>;

    static constexpr bool is_applicable = is_operator_applicable<TOptional>::value == boost::true_type::value;
    static constexpr bool is_arg_callable = type_traits::is_callable<FuncType>::value == boost::true_type::value;
};

template<typename TOptional, typename ValueType>
struct TOptinalNoneTypes<TOptional, ValueType, type_traits::ArgValue, true>
{
    using op_value_type = optional_value_type_t<TOptional>;
    using invoc_result = ValueType;
    using is_invoc_result_optional = is_optional_type<invoc_result>;
    using deduced_result = boost::conditional_t<
        is_invoc_result_optional::value,
        optional_value_type_t<invoc_result>,
            invoc_result>;

    static constexpr bool is_applicable = is_operator_applicable<TOptional>::value == boost::true_type::value;
    static constexpr bool is_arg_callable = type_traits::is_callable<ValueType>::value == boost::true_type::value;
};
// clang-format on

} // namespace optional_detail



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
// clang-format off
template <typename TOptional,
          typename Functor,
          typename deducer = optional_detail::TOptinalTypes<
            TOptional,
            Functor,
            optional_detail::is_operator_applicable<TOptional>::value,
            optional_detail::is_higher_order_function<Functor>::value>,
          typename deduced_result = typename deducer::deduced_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0,
          typename boost::enable_if_c<deducer::is_map_like, int>::type = 0,
          typename boost::enable_if_c<!deducer::has_higher_order_functon, int>::type = 0>
// clang-format on
boost::optional<deduced_result> operator|(TOptional&& op, Functor&& f)
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
// clang-format off
template <typename TOptional,
          typename Functor,
          typename deducer = optional_detail::TOptinalTypes<
            TOptional,
            Functor,
            optional_detail::is_operator_applicable<TOptional>::value,
            optional_detail::is_higher_order_function<Functor>::value>,
          typename deduced_result = typename deducer::deduced_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0,
          typename boost::enable_if_c<deducer::is_flat_map_like, int>::type = 1,
          typename boost::enable_if_c<!deducer::has_higher_order_functon, int>::type = 0>
// clang-format on
boost::optional<deduced_result> operator|(TOptional&& op, Functor&& f)
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
 * It's special override for the pipe op. It's needed for implementing additional operators as HOF
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
 *        | op::filter_if([](const auto& el) { return el > 0; })
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
// clang-format off
template <typename TOptional,
          typename Functor,
          typename deducer = optional_detail::TOptinalTypes<
            TOptional,
            Functor,
            optional_detail::is_operator_applicable<TOptional>::value,
            optional_detail::is_higher_order_function<Functor>::value>,
          typename deduced_result = typename deducer::deduced_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0,
          typename boost::enable_if_c<deducer::is_flat_map_like, int>::type = 1,
          typename boost::enable_if_c<deducer::has_higher_order_functon, int>::type = 1>
// clang-format on
decltype(auto) operator|(TOptional&& op, Functor&& f)
{
    return f(std::forward<TOptional>(op));
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
template <typename TOptional,
          typename Functor,
          typename deducer = optional_detail::TOptinalNoneTypes<TOptional, Functor, type_traits::argument_type_t<Functor>, optional_detail::is_operator_applicable<TOptional>::value>,
          typename deduced_result = typename deducer::deduced_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0>
boost::optional<deduced_result> operator|=(TOptional&& op, Functor&& f)
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
template <typename TOptional,
          typename Functor,
          typename deducer = optional_detail::TOptinalNoneTypes<TOptional, Functor, type_traits::argument_type_t<Functor>, optional_detail::is_operator_applicable<TOptional>::value>,
          typename deduced_result = typename deducer::invoc_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0,
          typename boost::enable_if_c<deducer::is_arg_callable, int>::type = 0>
deduced_result operator<<=(TOptional&& op, Functor&& f)
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
template <typename TOptional,
          typename ValueType,
          typename deducer = optional_detail::TOptinalNoneTypes<TOptional, ValueType, type_traits::argument_type_t<ValueType>, optional_detail::is_operator_applicable<TOptional>::value>,
          typename deduced_result = typename deducer::invoc_result,
          typename boost::enable_if_c<deducer::is_applicable, int>::type = 0,
          typename boost::enable_if_c<!deducer::is_arg_callable, int>::type = 1>
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


template<typename T>
typename boost::optional<typename boost::optional<T>::reference_const_type> toRefOp(const boost::optional<T>& op)
{
    return op.get();
}

template<typename T>
typename boost::optional<typename boost::optional<T>::reference_type> toRefOp(boost::optional<T>& op)
{
    return op.get();
}


namespace hof {
namespace {
void emptyNone(){}

} // namespace

template <typename TPred>
decltype(auto) filter_if(TPred&& pred)
{
    return optional_detail::THigherOrderFunction(
        [pred = std::forward<TPred>(pred)](auto&& op) mutable
        {
            if (op)
            {
                if (pred(op.get()))
                {
                    return op;
                }
            }

            return decltype(op)(boost::none);
        });
}

template <typename TPred>
decltype(auto) filter_if_not(TPred&& pred)
{
    return optional_detail::THigherOrderFunction(
        [pred = std::forward<TPred>(pred)](auto&& op) mutable
        {
            if (op)
            {
                if (!pred(op.get()))
                {
                    return op;
                }
            }

            return decltype(op)(boost::none);
        });
}

template <typename TSome, typename TNone = void()>
decltype(auto) match(TSome&& some, TNone&& none = emptyNone)
{
    return optional_detail::THigherOrderFunction(
        [some = std::forward<TSome>(some), none = std::forward<TNone>(none)](auto&& op) mutable
        {
            if (op)
            {
                some(op.get());
            }
            else
            {
                none();
            }

            return op;
        });
}

template <typename TFunctor>
decltype(auto) match_some(TFunctor&& fn)
{
    return optional_detail::THigherOrderFunction(
        [some = std::forward<TFunctor>(fn)](auto&& op) mutable
        {
            if (op)
            {
                some(op.get());
            }

            return op;
        });
}

template <typename TFunctor>
decltype(auto) match_none(TFunctor&& fn)
{
    return optional_detail::THigherOrderFunction(
        [none = std::forward<TFunctor>(fn)](auto&& op) mutable
        {
            if (!op)
            {
                none();
            }

            return op;
        });
}


} // namespace op
