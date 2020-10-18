#pragma once

#include <functional>
#include <boost/signals2/connection.hpp>

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