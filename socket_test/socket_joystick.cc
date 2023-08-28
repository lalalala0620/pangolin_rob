#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <nlohmann/json.hpp>

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
    serverAddress.sin_port = htons(8000);  // 使用端口8000
    serverAddress.sin_addr.s_addr = INADDR_ANY; // 监听所有网络接口
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

void receiveAndProcessJSONData(int clientSocket, char buffer[], size_t bufferSize , double& yAxis, double& zAxis) {
    int bytesRead = recv(clientSocket, buffer, bufferSize, 0);
    if (bytesRead == -1) {
        std::cerr << "Error receiving data." << std::endl;
        // 可以在此处执行错误处理，或者跳过并继续等待数据
    } else if (bytesRead == 0) {
        std::cerr << "Connection closed by the remote host." << std::endl;
         // 断开连接，退出循环
    } else {
        // std::string receivedData(buffer, bytesRead);
        // std::cout << receivedData << zAxis << std::endl;
        try {
            json jsonData = json::parse(buffer);
            yAxis = jsonData["yAxis"];
            zAxis = jsonData["zAxis"];
            std::cout << "Received JSON Data:" << jsonData << std::endl;
            // std::cout << "yAxis: " << yAxis << std::endl;
            // std::cout << "zAxis: " << zAxis << std::endl;
        } catch (json::parse_error& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            // 可以在此处执行错误处理，或者跳过并继续等待数据
        }
    }
    return;
}

int main() {
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
    double receivedYAxis, receivedZAxis;
    char buffer[4096];
    size_t bufferSize = sizeof(buffer); 
    while(true){
        memset(buffer, 0, sizeof(buffer));
        receiveAndProcessJSONData(clientSocket, buffer, bufferSize, receivedYAxis, receivedZAxis);
        std::cout << receivedYAxis << std::endl;
    }


    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}
