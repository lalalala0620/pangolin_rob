#include <unistd.h>
#include <time.h>

#include "acfw/execute/EventBasedRunnable.h"
#include "acfw/log/Logger.h"
#include "acfw/execute/Timer.h"

#include "services/Twist.h"
#include "services/Twist.h"
#include "TinyJson.h"

#include "Socket_manager.h"

using namespace acfw::core;
using namespace acfw::execute;
using namespace acfw::cm;

// using namespace sample;


class SampleEventRunnable : public EventBasedRunnable
{
    services::TwistConsumer m_twistConsumer;
    services::TwistProvider m_twistProvider;

    Timer m_timer;
    Socket_manager socket;

public:
    SampleEventRunnable()
    {
        // Register message handlers (on() methods below)
        registerMsg<Service::State, services::TwistConsumer&>(*this);
        registerMsg<services::Twist, services::TwistConsumer&>(*this);
        registerMsg<Service::State, services::TwistProvider&>(*this);
    }

    void on(const Service::State& state, services::TwistConsumer& instance)
    {
        // TODO: Handle state change here
        (void)instance;
        InfoLog("services::TwistConsumer state changed to %d", (int)state);
    }

    void on(const services::Twist& msg, services::TwistConsumer& instance)
    {
        // TODO: Handle event
        (void)msg;
        (void)instance;
        InfoLog("Event consumer received event datatime=%.2f seqNum=%.2f",
            msg.linear,
            msg.angular);
    }
    void on(const Service::State& state, services::TwistProvider& instance)
    {
        // TODO: Handle state change here
        (void)instance;
        InfoLog("services::TwistProvider state changed to %d", (int)state);
    }


    bool init(const std::string& config) noexcept override
    {
        EventBasedRunnable::init(config);
        // Create services
        if (m_twistConsumer.create(id(), R"(
            {
                "class": "event",
                "role": "client",
                "type": "services::Twist",
                "name": "twist"
            }
            )") != Result::OK)
        {
            ErrorLog("Create m_twistConsumer failed");
            return false;
        }
        if (m_twistProvider.create(id(), R"(
            {
                "class": "event",
                "role": "server",
                "type": "services::Twist",
                "endpoints": [
                    {
                        "type": "dds",
                        "config": {
                            "topic": "twist"
                        }
                    }
                ],
                "name": "twist"
            }
            )") != Result::OK)
        {
            ErrorLog("Create m_twistProvider failed");
            return false;
        }

        // std::string error;
        // Json configObj = Json::parse(config, error);
        // if (!error.empty())
        // {
        //     ErrorLog("Couldn't parse runnable config: %s", error.c_str());
        //     return false;
        // }

        // const auto spec = configObj["services"]["0"];
        // auto r          = m_twistProvider.create(id(), spec.dump().c_str());
        // if (r != Result::OK)
        // {
        //     ErrorLog("Create service provider failed: %d", (int)r);
        //     return false;
        // }

        // sleep(1);
        socket.clientSocket = socket.socket_init();

        // Start services
        if (m_twistConsumer.start() != Result::OK)
        {
            ErrorLog("Start m_twistConsumer failed");
            return false;
        }
        if (m_twistProvider.start() != Result::OK)
        {
            ErrorLog("Start m_twistProvider failed");
            return false;
        }

        m_timer.reset(Millisec(100), [this]() {
            this->onTimer();
        });
        m_timer.startAfter(Millisec(100));

        return true;
    }

    void onTimer()
    {
        if (m_twistProvider.getState() == Service::State::STARTED)
        {
            memset(socket.buffer, 0, sizeof(socket.buffer));
            socket.receiveAndProcessJSONData(socket.clientSocket, socket.buffer, socket.bufferSize, socket.receivedYAxis, socket.receivedZAxis);
            services::Twist evt;
            evt.linear = -socket.receivedYAxis;
            evt.angular = socket.receivedZAxis;
            auto r     = m_twistProvider.sendEvent(&evt);
            InfoLog("linear =%.2f angular=%.2f result=%d",evt.linear,evt.angular,(int)r);
        }
    }
};

DEFINE_RUNNABLE_PLUGIN(SampleEventRunnable)