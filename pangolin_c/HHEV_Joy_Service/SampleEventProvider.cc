#include <wiringPi.h>
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <softServo.h>
#include <unistd.h>
#include <time.h>
#include "acfw/cm/Cm.h"
#include "SampleEvent.h"

#include "acfw/execute/EventBasedRunnable.h"
#include "acfw/log/Logger.h"
#include "acfw/execute/Timer.h"

#include "Pangolin_controller.h"
#include "json.hpp"

#include <time.h>
// #include "Socket_manager.h"
#include "SampleEvent.h"
#include "acfw/log/Logger.h"
#include "acfw/execute/Timer.h"
#include "acfw/cm/Cm.h"
// #include "services/Twist.h"
// #include "services/Twist.h"
// #include "TinyJson.h"

// #include "Socket_manager.h"
#include <iostream>
// #include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <netinet/in.h> // Input struct for sockets
#include <arpa/inet.h>  // Helper functions for ip conversion
#include <thread>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <queue>

#include <unistd.h>

// #include "services/Twist.h"
// #include "services/Twist.h"
// #include "TinyJson.h"

// #include "Socket_manager.h"
#include <iostream>

using namespace acfw::cm;
using namespace sample;
using namespace std;
using namespace Pangolin;
using json = nlohmann::json;

struct JoystickState {
    float linear;
    float angular;
};

JoystickState joystick_data;
queue<JoystickState> q;

int server_app(sockaddr_in server_addr)
{
    
    int server;
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0)
    {
        std::cout << "Server: Failed to open socket." << std::endl;
        return 1;
    }
    int opted{1};
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opted, sizeof(opted)))
    { // Set option at socket level to let bind reuse local addresses.
        std::cout << "Server: Failed to set socket options - " << strerror(errno) << std::endl;
        return 1;
    }
    // Bind
    std::cout << "Server: Binding...\n";
    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cout << "Sever: Failed to bind to address - " << strerror(errno) << "\n";
        return 1;
    }
    // Listen
    std::cout << "Server: Listening...\n";
    if (listen(server, 3) < 0)
    {
        std::cout << "Server: Failed to start listening - " << strerror(errno) << std::endl;
        return 1;
    }

    // Accept
    int conn;
    socklen_t server_length = sizeof(server_addr);
    std::cout << "Server: Accept connection...\n";
    conn = accept(server, (struct sockaddr *)&server_addr, &server_length);
    if (conn < 0)
    {
        std::cout << "Server: Failed to accept connection." << std::endl;
        return 1;
    }



    int reader;
    char buffer[1024];
    size_t bufferSize = sizeof(buffer); 
    while(true){
        memset(buffer, 0, sizeof(buffer));
        reader = recv(conn, &buffer, bufferSize, MSG_WAITFORONE);
        // reader = read(conn, &buffer, 1024);
        
        char msg[]{"Server response."};
        std::string receivedData(buffer, bufferSize);
        // std::cout << "Server: Read " << reader << " bytes: \"" << receivedData << "\"\n";
        try {
            json jsonData = json::parse(receivedData);
            joystick_data.linear = jsonData["yAxis"];
            joystick_data.angular = jsonData["zAxis"];
            q.push(joystick_data);
            // std::cout << joystick_data.linear << joystick_data.angular << " bytes: \"" << "\"\n";
        } catch (json::parse_error& e) {
            // std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
        
        // send(conn, msg, strlen(msg), 0);
        usleep(100);
    }
    // std::cout << "Server: Sent response to client.\n";

    std::cout << "Server: Cleaning up." << std::endl;
    shutdown(conn, SHUT_RDWR);
    shutdown(server, SHUT_RDWR);

    return 0;
}

int socket_main()
{
    // (void)argc;
    // (void)argv; // Ignore unused input variables

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8000); // Set port to network order

    std::thread t1(server_app, server_addr);

    if (inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr) <= 0)
    {
        std::cout << "Failed to convert IP to binary format." << std::endl;
        return 1;
    }
    // server_addr.sin_addr.s_addr = INADDR_LOOPBACK;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // std::thread t2(client_app, server_addr);
    t1.detach();
    // t2.join();

    return 0;
}

JoystickState getjoyState() {
        if (q.size() != 0) {
            joystick_data = q.back();
            q.pop();
        }
        return  joystick_data;
    }

// PangolinController pangolin_ctrl;

static void onState(Service::State state, void* userData)
{
    (void)userData;
    cout << "Provider state changed to: " << (int)state << endl;
}

int main (void)
{
    acfw::cm::initialize();
    // pangolin_ctrl.initServoStatus();

    const char* spec = R"(
    {
        "class": "event",
        "role": "server",
        "type": "sample::SampleEvent",
        "name": "sample/event",
        "endpoints": [
            {
                "type": "dds",
                "config": {
                    "topic": "sample/event"
                }
            }
        ]
    }
    )";
    // Create service
    SampleEventProvider provider;
    auto r = provider.create(0, spec);
    if (r != Result::OK)
    {
        cout << "Create service provider failed: " << (int)r << endl;
        return 1;
    }
    provider.setStateHandler(onState, &provider);

    // Start service
    r = provider.start();
    if (r != Result::OK)
    {
        cout << "Create service provider failed: " << (int)r << endl;
        return 2;
    }


    // socket.socket_init_();
    std::thread t2(socket_main);
    t2.detach();

    // Keep sending current date time event
    std::uint64_t seqNum = 0;

    // float receivedYAxis, receivedZAxis;
    // char buffer[4096];
    // size_t bufferSize = sizeof(buffer); 
    while (true)
    {   
        if (provider.getState() == Service::State::STARTED)
        {
            // memset(buffer, 0, sizeof(buffer));
            // receiveAndProcessJSONData(clientSocket, buffer, bufferSize, receivedYAxis, receivedZAxis);
            SampleEvent evt;
            // joystick_data = socket.get_data();
            joystick_data = getjoyState();
            evt.linear = -joystick_data.linear;
            evt.angular = joystick_data.angular;
            r = provider.sendEvent(&evt);
            cout << "linear =" << evt.linear << " angular=" << evt.angular << " result=" << (int)r << endl;
        }
        usleep(100000);
    }


    acfw::cm::release();
    // close(clientSocket);
    // close(serverSocket);
    return 0;


}
