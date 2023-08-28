#ifndef SOCKET
#define SOCKET

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;



struct JoystickState {
    float linear;
    float angular;
};

class Socket_manager{
    public:

    int clientSocket;
    double receivedYAxis, receivedZAxis;
    char buffer[4096];
    size_t bufferSize = sizeof(buffer); 
    
    int createServerSocket();
    
    void configureServerAddress(struct sockaddr_in& serverAddress);
    
    int bindServerSocket(int serverSocket, struct sockaddr_in& serverAddress);
    
    int listenForConnection(int serverSocket);
    
    int acceptClientConnection(int serverSocket, struct sockaddr_in& clientAddress, socklen_t& clientAddressLength);
    
    void receiveAndProcessJSONData(int clientSocket, char buffer[], size_t bufferSize , double& yAxis, double& zAxis);

    int socket_init();

};

#endif // SOCKET