
#include <unistd.h>
#include <time.h>
#include <iostream>
#include "acfw/cm/Cm.h"
#include "SampleEvent.h"

#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <nlohmann/json.hpp>

using namespace acfw::cm;
using namespace sample;
using namespace std;
using json = nlohmann::json;

int createServerSocket() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        exit(1);
    }
    return serverSocket;
}

void configureServerAddress(struct sockaddr_in& serverAddress) {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
}

int bindServerSocket(int serverSocket, struct sockaddr_in& serverAddress) {
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error: Could not bind to port." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return serverSocket;
}

int listenForConnection(int serverSocket) {
    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error: Could not listen to socket." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return serverSocket;
}

int acceptClientConnection(int serverSocket, struct sockaddr_in& clientAddress, socklen_t& clientAddressLength) {
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error: Could not accept client connection." << std::endl;
        close(serverSocket);
        exit(1);
    }
    return clientSocket;
}

void receiveAndProcessJSONData(int clientSocket, char buffer[], size_t bufferSize , float& yAxis, float& zAxis) {
    int bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data." << std::endl;
    } else if (bytesRead == 0) {
        std::cerr << "Connection closed by the remote host." << std::endl;
    } else {
        std::string receivedData(buffer, bytesRead);
        try {
            json jsonData = json::parse(receivedData);
            yAxis = jsonData["yAxis"];
            zAxis = jsonData["zAxis"];
        } catch (json::parse_error& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    }
    return;
}

static void onState(Service::State state, void* userData)
{
    (void)userData;
    cout << "Provider state changed to: " << (int)state << endl;
}

int main()
{
    //socket init
    int serverSocket = createServerSocket();

    struct sockaddr_in serverAddress;
    configureServerAddress(serverAddress);

    serverSocket = bindServerSocket(serverSocket, serverAddress);
    serverSocket = listenForConnection(serverSocket);

    std::cout << "Server listening on port 8000..." << std::endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = acceptClientConnection(serverSocket, clientAddress, clientAddressLength);

    std::cout << "Client connected." << std::endl;

    //hhevos 
    acfw::cm::initialize();

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

    // Keep sending current date time event
    std::uint64_t seqNum = 0;

    float receivedYAxis, receivedZAxis;
    char buffer[4096];
    size_t bufferSize = sizeof(buffer); 
    while (true)
    {   
        if (provider.getState() == Service::State::STARTED)
        {
            memset(buffer, 0, sizeof(buffer));
            receiveAndProcessJSONData(clientSocket, buffer, bufferSize, receivedYAxis, receivedZAxis);
            SampleEvent evt;
            evt.linear = -receivedYAxis;
            evt.angular = receivedZAxis;
            r = provider.sendEvent(&evt);
            cout << "linear =" << evt.linear << " angular=" << evt.angular << " result=" << (int)r << endl;
        }
    }


    acfw::cm::release();
    close(clientSocket);
    close(serverSocket);
    return 0;
} 