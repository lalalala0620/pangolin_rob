#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    // 创建一个Socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //IPv4 TCP 0:auto
    if (serverSocket == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    // 准备服务器地址结构体
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);  // 设置端口号  htons 函数用于将主机字节序（通常是小端字节序）转换为网络字节序（大端字节序），因为网络字节序在网络通信中通常是标准的。
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // 监听所有可用的网络接口

    // 绑定Socket到地址
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {   //这里通过类型转换将 serverAddress 结构体指针转换为通用的 struct sockaddr* 类型，因为 bind 函数需要接受一个通用的地址结构体指针作为参数。这个地址结构体描述了服务器的地址和端口。
        std::cerr << "Error: Could not bind to port." << std::endl;
        return 1;
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error: Could not listen on socket." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8000..." << std::endl;

    // 接受客户端连接
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error: Could not accept client connection." << std::endl;
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // 从客户端接收数据并回传
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        // if (bytesRead <= 0) {
        //     std::cerr << "Client disconnected." << std::endl;
        //     break;
        // }
        std::cout << "Received: " << buffer << std::endl;

        // 回传数据给客户端
        // send(clientSocket, buffer, bytesRead, 0);
    }

    // 关闭Socket连接
    close(clientSocket);
    close(serverSocket);

    return 0;
}