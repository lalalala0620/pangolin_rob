
#include <unistd.h>
#include <iostream>

#include <thread>
#include <ctime>
#include "acfw/cm/Cm.h"
#include "SampleEvent.h"
#include "Pangolin_controller.h"

using namespace acfw::cm;
using namespace sample;
using namespace std;
using namespace Pangolin;

bool isLoop = false;
clock_t a,b; 
PangolinController pangolin_ctrl;
static void onState(Service::State state, void* userData)
{
    (void)userData;
    // cout << "Consumer state changed to: " << (int)state << endl;
}

static void onEvent(const SampleEvent* event, void* userData)
{
    (void)userData;
    cout<< "linear: "<< event->linear << "\n"<< "angular: " << event->angular << "\n" << endl;
    // cout << "Received event datetime=" << event->datetime
    //      << " seqNum=" << event->seqNum << endl;

    // if (isLoop == 1){
    //     std::future<bool> f1 = std::async(std::launch::async, [&] {
    //         return static_cast<bool>(pangolin_ctrl.cmdProcessLoop(event->linear, event->angular));
    //     });
    //     std::future<bool> f1 = std::async(std::launch::async, &PangolinController::cmdP)

    //     isLoop = f1.get();
    //     cout << isLoop << endl;
    // }
    // else{
    //     isLoop = 0;
    //     cout << isLoop << endl;
    // }

    pangolin_ctrl.controlThread(event->linear, event->angular);

    // pangolin_ctrl.cmdProcessLoop(1.0, 0.0);
    // if (isLoop == false){
    //     isLoop = true;

    //     pangolin_ctrl.cmdProcessLoop(event->linear, event->angular);
    // }
    // else{
        
    //     isLoop = false;
    // }
    // pangolin_ctrl.cmdProcessLoop(event->linear, event->angular);
    SampleEvent::free(event);

        // while(true){
    //     std::vector<float> linear = {0.0, 0.0, 0.0};

    //     std::vector<float> angular = {0.0, 0.0, 1.0};

    //     pangolin_ctrl.cmdProcessLoop(linear, angular);
    // }
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
    // sleep(60);
    while(true){
    }

    acfw::cm::release();
    return 0;
    
}
