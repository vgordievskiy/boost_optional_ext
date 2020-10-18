#include "CDefDataProvider.h"

#include <chrono>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/lexical_cast.hpp>

namespace services
{
    CDefDataProvider::CDefDataProvider()
    {}

    CDefDataProvider::~CDefDataProvider()
    {
        stop();
        wait();
    }

    IDataProvider::Connection CDefDataProvider::onNewData(const FNewDataHandler& handler)
    {
        return m_newPriceReady.connect(handler);
    }

    void CDefDataProvider::setNewData(const Data& price)
    {
        if (!m_newPriceReady.empty())
        {
            m_newPriceReady(price);
        }
    }

    void CDefDataProvider::start()
    {
        m_isStopped.store(false, std::memory_order_relaxed);
        m_worker = std::thread([this] {
            boost::random::random_device rng;

            boost::random::uniform_real_distribution<> dist(-100.0, 100.0);
            boost::random::uniform_int_distribution<> errDist(1, 100);

            while (!m_isStopped.load(std::memory_order_relaxed))
            { 
                using namespace std::chrono_literals;

                auto isError = errDist(rng) % 5 == 0;

                if (isError)
                {
                    setNewData("an error");   
                }
                else {
                    auto newValue = dist(rng);
                    setNewData(boost::lexical_cast<std::string>(newValue));
                }

                std::this_thread::sleep_for(1s);
            }
        });
    }

    void CDefDataProvider::stop()
    {
        m_isStopped.store(true, std::memory_order_relaxed);
    }

    void CDefDataProvider::wait()
    {
        if (m_worker.joinable())
        {
            m_worker.join();
        }
    }

} // end namepsace services