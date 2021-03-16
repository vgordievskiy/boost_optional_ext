#include "data_service/CDefDataProvider.h"
#include <stdio.h>
#include <chrono>
#include <iterator>
#include <iostream>
#include <numeric>
#include <cstdlib> 

#include <boost/optional.hpp>
#include <boost/optional_ext.hpp>

#include <boost/lexical_cast.hpp>

template<typename T>
boost::optional<const T&> toOp(const T& value)
{
    return value;
}

template<typename T>
boost::optional<T&> toOp(T& value)
{
    return value;
}

boost::optional<double> toDouble(const std::string& value) noexcept
{
    try {
        return boost::lexical_cast<double>(value);
    }
    catch (const boost::bad_lexical_cast& exc)
    {
        std::cerr << exc.what() << std::endl;
        return boost::none;
    }
}

template<typename T>
void print(const T& value) noexcept
{
    std::cout << "new value from provider: " << value << std::endl;
}

int main()
{

    services::CDefDataProvider provider;

    double acc = 0.0;
    uint32_t errors = 0;

    uint64_t avarageTime = 0;
    uint16_t count = 0;
    std::list<double> items;
    auto bi = std::back_inserter(items);

    provider.onNewData([&avarageTime, &count, &acc, &errors, &bi, &provider](const services::IDataProvider::Data& data) mutable {

        auto errorHandler = [&errors] () mutable noexcept {
            std::cout << "a wrong data recieved" << std::endl;
            errors += 1;
        };

        auto filter = [] (auto&& el) noexcept
            { 
                return std::isgreaterequal(el, 0.0) && std::islessequal(el, 50.0);
            };

        auto accept = [&bi] (auto&& el) mutable noexcept {
            *bi = el;
            std::cout << "New Value accepted: " << el << std::endl;
        };

        auto start = std::chrono::high_resolution_clock::now(); 

        // In an old manner it would be look like this:
        /*
            try {
                auto value = boost::lexical_cast<double>(data);
                print(value);
                if (value >= 0 && value <= 50)
                {
                    std::cout << "New Value accepted: " << value << std::endl;
                    *bi = value;
                    acc += value;
                }
            }
            catch (const boost::bad_lexical_cast& exc)
            {
                std::cerr << exc.what() << std::endl;
                errors += 1;
            };
        */

        acc += toOp(data)
                 | toDouble
                 | hof::match(print<double>, errorHandler)
                 | hof::filter_if(filter)
                 | hof::match_some(accept)
                 <<= 0.0;

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        count += 1;
        avarageTime += duration.count();

        std::cout << "----step-----ACC-[" << acc << "] avarageTime: " << avarageTime / count << " microseconds " << std::endl;

        if (acc >= 100)
        {
            provider.stop();
        }
    });

    provider.start();

    provider.wait();

    std::cout << "Final Acc: [" << acc << "], Errors: " << errors << std::endl;
    std::cout << "Elements are: ";
    auto check = 0;
    for (const auto& el: items)
    {
        check += el;
        std::cout << el << ", ";
    }
    std::cout << std::endl << " Sum is " << check << std::endl;

    return 0;
}
