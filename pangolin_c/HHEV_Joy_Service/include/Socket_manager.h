
// #include <iostream>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
#include <queue>
// #include <nlohmann/json.hpp>
#include <iostream>
#include "tcpserver.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

JoystickState joystick_data;

class Socket_manager{
public:
    
    queue<float> q;
    // JoystickState joystick_data;
    int socket_init_()
    {
        queue<float> q;
        // Initialize server socket..
        TCPServer<> tcpServer;
        
        // printf("1");
        // When a new client connected:
        tcpServer.onNewConnection = [&](TCPSocket<> *newClient) {
            cout << "New client: [";
            cout << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << endl;

            newClient->onMessageReceived = [this, newClient, &q](string message) {
                cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << endl;
                try {
                    json jsonData = json::parse(message);
                    // joystick_data.linear = jsonData["yAxis"];
                    float zAxis = jsonData["zAxis"];

                    q.push(zAxis);
                    // float a = q.front();
                    // std::cout << zAxis <<q.size() << q.back()<< std::endl;
                    // std::cout << zAxis << std::endl;
                } catch (json::parse_error& e) {
                    // std::cerr << "Error parsing JSON: " << e.what() << std::endl;
                }
                newClient->Send("OK!");
            };
            
            // If you want to use raw bytes
            /*
            newClient->onRawMessageReceived = [newClient](const char* message, int length) {
                cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << "(" << length << ")" << endl;
                newClient->Send("OK!");
            };
            */
            
            newClient->onSocketClosed = [newClient](int errorCode) {
                cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << endl;
                cout << flush;
            };
        };
        // printf("2");
        // Bind the server to a port.
        tcpServer.Bind(8000, [](int errorCode, string errorMessage) {
            // BINDING FAILED:
            cout << errorCode << " : " << errorMessage << endl;
        });

        // Start Listening the server.
        tcpServer.Listen([](int errorCode, string errorMessage) {
            // LISTENING FAILED:
            cout << errorCode << " : " << errorMessage << endl;
        });

        // You should do an input loop, so the program won't terminate immediately
        string input;
        getline(cin, input);
        while (input != "exit")
        {
            getline(cin, input);
            // printf("\n1");
        }

        // Close the server before exiting the program.
        tcpServer.Close();

        return 0;
    }

    JoystickState get_data(){
    if (q.size() != 0) {
            joystick_data.linear = q.back();
            std::cout <<"11"<< std::endl;
            q.pop();
        }
        return joystick_data;
    }
};