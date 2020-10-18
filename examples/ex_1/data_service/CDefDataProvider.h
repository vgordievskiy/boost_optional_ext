#pragma once

#include <atomic>
#include <thread>
#include <boost/noncopyable.hpp>
#include <boost/signals2/signal.hpp>
#include "IDataProvider.h"

namespace services
{
class CDefDataProvider: public IDataProvider, boost::noncopyable
{
    public:

    CDefDataProvider();
    ~CDefDataProvider() override;

    void start() override;
    void stop() override;

    void wait() override;

    Connection onNewData(const FNewDataHandler& handler) override;

    private:
    void setNewData(const Data& price);

    private:
    boost::signals2::signal<IDataProvider::FNewData> m_newPriceReady;

    std::atomic<bool> m_isStopped{true};
    std::thread m_worker;

};

} // end namespace services