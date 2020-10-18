# boost_optional_ext
### It is a pipe operator for the boost::optional
### it gives you a way to write your code in FP manner.

# Sample

Consider a given interface:

```C++
namespace services
{
    class IDataProvider
    {
        public:

        using Data = std::string;

        using Connection = boost::signals2::connection;
        using FNewData = void(const Data&);
        using FNewDataHandler = std::function<FNewData>;

        virtual ~IDataProvider() = default;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void wait() = 0;

        virtual Connection onNewData(const FNewDataHandler& handler) = 0;

    };
} // end namespace service
```

Conditions:

* a new data comes as the std::string
* a new data is textual representation of floating point type with sign.
* the data may have an error (it isn't a correct representation of floating point type)
* it's required to sum all numbers are satisfying criterion: num > 0 and num < 50
* it's required to sum a count of errors.
* criterion of stop: the sum >= 100.

The following code snippet implements described above.

```C++

namespace {

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

} // end namespace

void DataConsumer::startDataHandler(service::IDataProvider& provider)
{
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

        // it's an example of usage boost_optional_ext
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

}

```

# How to configure and build example and tests

1. run ./configure.sh
2. run ./build.sh

   The build artifacts are in ./Build/bin 
