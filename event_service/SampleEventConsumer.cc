
#include <unistd.h>
#include <iostream>

#include "acfw/cm/Cm.h"
#include "SampleEvent.h"

using namespace acfw::cm;
using namespace sample;
using namespace std;

static void onState(Service::State state, void* userData)
{
    (void)userData;
    cout << "Consumer state changed to: " << (int)state << endl;
}

static void onEvent(const SampleEvent* event, void* userData)
{
    (void)userData;
    cout<< "linear: "<< event->linear << "\n"<< "angular: " << event->angular << "\n" << endl;
    // cout << "Received event datetime=" << event->datetime
    //      << " seqNum=" << event->seqNum << endl;
    SampleEvent::free(event);
}

int main()
{
    acfw::cm::initialize();

    const char* spec = R"(
{
    "class": "event",
    "role": "client",
    "type": "sample::SampleEvent",
    "name": "sample/event"
}
)";

    // Create service
    SampleEventConsumer consumer;
    auto r = consumer.create(0, spec);
    if (r != Result::OK)
    {
        cout << "Create service consumer failed: " << (int)r << endl;
        return 1;
    }
    consumer.setStateHandler(onState, &consumer);
    consumer.setEventHandler(onEvent, &consumer);

    // Start service
    r = consumer.start();
    if (r != Result::OK)
    {
        cout << "Create service consumer failed: " << (int)r << endl;
        return 2;
    }

    // Keep receiving data for 60 seconds
    sleep(60);

    acfw::cm::release();
    return 0;
}
