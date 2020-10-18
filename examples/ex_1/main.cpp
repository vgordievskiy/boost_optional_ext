#include "data_service/CDefDataProvider.h"
#include <stdio.h>
#include <iostream>

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

boost::optional<double> toDouble(const std::string& value)
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
void print(const T& value)
{
    std::cout << "new value from provider: " << value << std::endl;
}

int main()
{

    services::CDefDataProvider provider;

    double acc = 0.0;
    uint32_t errors = 0;

    provider.onNewData([&acc, &errors, &provider](const services::IDataProvider::Data& data) mutable {

        auto errorHandler = [&errors] () mutable {
            std::cout << "a wrong data recieved" << std::endl;
            errors += 1;
        };

        auto filter = [] (auto&& el) { return el >= 0 && el <= 50; };

        auto log = [] (auto&& el) {
            std::cout << "New Value accepted: " << el << std::endl;
        };

        /* In an old manner it would be look like this:
            try {
                auto value = boost::lexical_cast<double>(data);
                print(value);
                if (value >= 0 && value <= 50)
                {
                    std::cout << "New Value accepted: " << value << std::endl;
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
                 | hof::match_some(log)
                 <<= 0;

        std::cout << "----step-----ACC-[" << acc << "] " << std::endl;

        if (acc >= 100)
        {
            provider.stop();
        }
    });

    provider.start();

    provider.wait();

    std::cout << "Final Acc: [" << acc << "], Errors: " << errors << std::endl;

    return 0;
}